/****************************************************************************
 *
 *   Copyright (c) 2015 PX4 Development Team. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name PX4 nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

/**
 * @file flashparams.cpp
 *
 * Global flash based parameter store.
 *
 * This provides the mechanisms to interface to the PX4
 * parameter system but replace the IO with non file based flash
 * i/o routines. So that the code my be implemented on a SMALL memory
 * foot print device.
 */

#અન્તર્ભાવ <px4_platform_common/defines.h>
#અન્તર્ભાવ <px4_platform_common/log.h>
#અન્તર્ભાવ <px4_platform_common/posix.h>
#અન્તર્ભાવ <px4_platform_common/shutdown.h>
#અન્તર્ભાવ <parameters/px4_parameters.hpp>

#અન્તર્ભાવ <string.h>
#અન્તર્ભાવ <stdbool.h>
#અન્તર્ભાવ <stdint.h>
#અન્તર્ભાવ <errno.h>

#અન્તર્ભાવ <parameters/param.h>

#અન્તર્ભાવ <lib/tinybson/tinybson.h>
#અન્તર્ભાવ "flashparams.h"
#અન્તર્ભાવ "flashfs.h"
#અન્તર્ભાવ "../param_translation.h"

#જો 0
# define debug(fmt, args...)            કરો { warnx(fmt, ##args); } જ્યારે(0)
#નહીંતર
# define debug(fmt, args...)            કરો { } જ્યારે(0)
#endif


/**
 * Storage for modified parameters.
 */
સંરચના param_wbuf_s {
	સંઘ param_કિંમત_u     val;
	param_t                 param;
};

/*
 * Same-bank program/erase stalls instruction fetch on STM32H7. A full
 * snapshot rewrite is tens of ms; a 128 KB sector erase is ~1 s. In-service
 * saves therefore append a delta of unsaved params (a few 32-byte rows).
 * At boot, before sensors or DShot start, compact to one snapshot only when
 * the sector cannot fit a burst of deltas, so a disarm-edge UUID save does
 * not pay the erase every flight.
 *
 * Firmware before the log format wrote one parameters_legacy_token snapshot
 * and cannot replay deltas. It ignores parameters_token records and treats
 * a store without its own as empty (STM32H7 erases it at init), so a
 * downgrade boots on defaults instead of a partial set. A legacy record is
 * always the newest state, since only that firmware writes one: import it
 * alone and rewrite the store as a log-format snapshot.
 */

/* Replay met a record whose name or type is not in this firmware's parameter
 * table, so it is usable through a translation at best. The file backend
 * drops those on its next save; here they stay until a compaction, and a
 * translation is only kept for a few releases. A snapshot cannot hold such a
 * record, so compacting for it does not repeat. */
સ્થિર તાર્કિક log_stale;

/* Replay failed inside a CRC-valid record. A delta appended behind it would
 * never be reached, so the next save rewrites the store. */
સ્થિર તાર્કિક log_unreadable;

સ્થિર તાર્કિક
any_unsaved(param_filter_func filter)
{
	માટે (param_t param = 0; param < user_config.PARAM_ગણતરી; param++) {
		જો ((filter == nullptr || filter(param)) && param_કિંમત_unsaved(param)) {
			પાછા સાચું;
		}
	}

	પાછા ખોટું;
}

/*
 * A snapshot holds every override in RAM, equal to the current default or
 * not: the boot compaction runs before the airframe applies its set-default
 * values, so at that point "equals the default" says nothing. A delta holds
 * the unsaved params; one that no longer overrides, or now matches the
 * default in force, is written as a BSON null so replay drops it, which is
 * what the file backend's "don't export defaults" gives after a reload.
 */
સ્થિર પૂર્ણાંક
encode_params(bson_encoder_s *encoder, param_filter_func filter, તાર્કિક snapshot)
{
	માટે (param_t param = 0; param < user_config.PARAM_ગણતરી; param++) {
		જો (filter && !filter(param)) {
			ચાલુ;
		}

		જો (snapshot) {
			જો (!user_config.contains(param)) {
				ચાલુ;
			}

		} નહીંતર {
			જો (!param_કિંમત_unsaved(param)) {
				ચાલુ;
			}

			જો (!user_config.contains(param) || param_કિંમત_is_default(param)) {
				જો (bson_encoder_append_null(encoder, param_name(param))) {
					debug("BSON append failed for '%s'", param_name(param));
					પાછા -1;
				}

				ચાલુ;
			}
		}

		પસંદગી (param_type(param)) {
		સ્થિતિ PARAM_TYPE_INT32:
			જો (bson_encoder_append_int32(encoder, param_name(param), user_config.get(param).i)) {
				debug("BSON append failed for '%s'", param_name(param));
				પાછા -1;
			}

			તોડો;

		સ્થિતિ PARAM_TYPE_FLOAT:
			જો (bson_encoder_append_double(encoder, param_name(param), (દ્વિગુણ)user_config.get(param).f)) {
				debug("BSON append failed for '%s'", param_name(param));
				પાછા -1;
			}

			તોડો;

		મૂળભૂત:
			debug("unrecognized parameter type");
			પાછા -1;
		}
	}

	પાછા 0;
}

/* The caller frees bson_encoder_buf_data() whatever the result. */
સ્થિર પૂર્ણાંક
encode_document(bson_encoder_s *encoder, param_filter_func filter, તાર્કિક snapshot)
{
	bson_encoder_init_buf(encoder, nullptr, 0);

	પૂર્ણાંક result = encode_params(encoder, filter, snapshot);

	જો (result == 0) {
		result = bson_encoder_fini(encoder);
	}

	પાછા result;
}

સ્થિર પૂર્ણાંક
stage_write_buffer(bson_encoder_s *encoder, uint8_t **buffer, size_t *enc_size)
{
	શૂન્ય *enc_buff = bson_encoder_buf_data(encoder);
	*enc_size = bson_encoder_buf_size(encoder);

	જો (enc_buff == nullptr) {
		પાછા -ENOMEM;
	}

	size_t alloc_size = *enc_size;
	પૂર્ણાંક result = parameter_flashfs_alloc(parameters_token, buffer, &alloc_size);

	જો (result != OK) {
		પાછા result;
	}

	જો (alloc_size < *enc_size) {
		parameter_flashfs_free();
		પાછા -ENOMEM;
	}

	memcpy(*buffer, enc_buff, *enc_size);
	પાછા OK;
}

સ્થિર પૂર્ણાંક
commit_append(uint8_t *buffer, size_t enc_size)
{
	પૂર્ણાંક result = parameter_flashfs_write(parameters_token, buffer, enc_size);
	parameter_flashfs_free();

	જો (result == (પૂર્ણાંક)enc_size) {
		પાછા OK;
	}

	પાછા result < 0 ? result : -EFBIG;
}

સ્થિર પૂર્ણાંક
export_delta(param_filter_func filter)
{
	bson_encoder_s encoder{};
	પૂર્ણાંક result = encode_document(&encoder, filter, ખોટું);

	જો (result == 0) {
		uint8_t *buffer;
		size_t enc_size;
		result = stage_write_buffer(&encoder, &buffer, &enc_size);

		જો (result == OK) {
			result = commit_append(buffer, enc_size);
		}
	}

	મુક્ત(bson_encoder_buf_data(&encoder));
	પાછા result;
}

/* Everything that can fail before the erase, so a failure here leaves the
 * log untouched and still loadable. */
સ્થિર પૂર્ણાંક
stage_snapshot(bson_encoder_s *encoder, uint8_t **buffer, size_t *enc_size)
{
	પૂર્ણાંક result = stage_write_buffer(encoder, buffer, enc_size);

	જો (result != OK) {
		પાછા result;
	}

	જો (*enc_size > parameter_flashfs_max_payload()) {
		parameter_flashfs_free();
		પાછા -ENOSPC;
	}

	પાછા OK;
}

સ્થિર પૂર્ણાંક
commit_snapshot(uint8_t *buffer, size_t enc_size)
{
	જો (parameter_flashfs_blank() != 1) {
		પૂર્ણાંક rv = parameter_flashfs_erase();

		જો (rv < 0) {
			parameter_flashfs_free();
			પાછા rv;
		}
	}

	પૂર્ણાંક result = parameter_flashfs_write(parameters_token, buffer, enc_size);

	જો (result != (પૂર્ણાંક)enc_size && parameter_flashfs_blank() == 1) {
		result = parameter_flashfs_write(parameters_token, buffer, enc_size);
	}

	parameter_flashfs_free();

	જો (result == (પૂર્ણાંક)enc_size) {
		પાછા OK;
	}

	પાછા result < 0 ? result : -EFBIG;
}

સ્થિર પૂર્ણાંક
compact_to_snapshot()
{
	bson_encoder_s encoder{};
	પૂર્ણાંક result = encode_document(&encoder, nullptr, સાચું);

	જો (result == 0) {
		uint8_t *buffer;
		size_t enc_size;
		result = stage_snapshot(&encoder, &buffer, &enc_size);

		જો (result == OK) {
			result = commit_snapshot(buffer, enc_size);
		}
	}

	જો (result == OK) {
		log_unreadable = ખોટું;
	}

	મુક્ત(bson_encoder_buf_data(&encoder));
	પાછા result;
}

સ્થિર તાર્કિક
has_record(flash_file_token_t token)
{
	uint8_t *buffer = nullptr;
	size_t buf_size = 0;
	પાછા parameter_flashfs_read(token, &buffer, &buf_size) >= 0;
}

સ્થિર પૂર્ણાંક
param_export_internal(param_filter_func filter)
{
	/* A delta only means something behind a snapshot that replays. A legacy
	 * record still present means the boot migration did not run; replay
	 * would take it over any delta appended behind it. */
	જો (log_unreadable || has_record(parameters_legacy_token) || !has_record(parameters_token)) {
		પાછા compact_to_snapshot();
	}

	જો (!any_unsaved(filter)) {
		પાછા 0;
	}

	પૂર્ણાંક result = export_delta(filter);

	જો (result != -ENOSPC) {
		પાછા result;
	}

	/* Same-bank erase stalls the CPU for ~1 s; last resort when a single
	 * save does not fit the tail. Compact writes the full RAM snapshot so a
	 * filtered save cannot drop params not in the delta. */
	PX4_WARN("flashparams: param sector full, compacting");
	પાછા compact_to_snapshot();
}


સ્થિર તાર્કિક
record_is_stale(bson_ગાંઠ_t ગાંઠ_2)
{
	અચલ param_t param = param_find_no_notification(ગાંઠ_2->name);

	જો (param == PARAM_INVALID) {
		પાછા સાચું;
	}

	પસંદગી (ગાંઠ_2->type) {
	સ્થિતિ BSON_INT32:
		પાછા param_type(param) != PARAM_TYPE_INT32;

	સ્થિતિ BSON_DOUBLE:
		પાછા param_type(param) != PARAM_TYPE_FLOAT;

	મૂળભૂત:
		પાછા ખોટું;
	}
}

સ્થિર પૂર્ણાંક
param_import_callback(bson_decoder_t decoder, bson_ગાંઠ_t ગાંઠ_2)
{
	દશાંશ f;
	int32_t i;
	શૂન્ય *v = nullptr;
	પૂર્ણાંક result = -1;

	/*
	 * EOO means the end of the parameter object. (Currently not supporting
	 * nested BSON objects).
	 */
	જો (ગાંઠ_2->type == BSON_EOO) {
		debug("end of parameters");
		પાછા 0;
	}

	/* A tombstone has no value. The decoder leaves the previous node's in
	 * the union and the translations read it without a type check. */
	અચલ તાર્કિક tombstone = ગાંઠ_2->type == BSON_nullptr || ગાંઠ_2->type == BSON_UNDEFINED;

	જો (!tombstone) {
		જો (record_is_stale(ગાંઠ_2)) {
			log_stale = સાચું;
		}

		જો (param_modify_on_import(ગાંઠ_2) == param_modify_on_import_ret::PARAM_SKIP_IMPORT) {
			પાછા 1;
		}
	}

	/*
	 * Find the parameter this node represents.  If we don't know it,
	 * ignore the node.
	 */
	param_t param = param_find_no_notification(ગાંઠ_2->name);

	જો (param == PARAM_INVALID) {
		debug("ignoring unrecognised parameter '%s'", ગાંઠ_2->name);
		પાછા 1;
	}

	/*
	 * Handle setting the parameter from the node
	 */

	પસંદગી (ગાંઠ_2->type) {
	સ્થિતિ BSON_nullptr:
	સ્થિતિ BSON_UNDEFINED:
		user_config.reset(param);
		result = 1;
		જાઓ out;

	સ્થિતિ BSON_INT32:
		જો (param_type(param) != PARAM_TYPE_INT32) {
			PX4_WARN("unexpected type for %s", ગાંઠ_2->name);
			result = 1; // just skip this entry
			જાઓ out;
		}

		i = ગાંઠ_2->i32;
		v = &i;
		તોડો;

	સ્થિતિ BSON_DOUBLE:
		જો (param_type(param) != PARAM_TYPE_FLOAT) {
			PX4_WARN("unexpected type for %s", ગાંઠ_2->name);
			result = 1; // just skip this entry
			જાઓ out;
		}

		f = ગાંઠ_2->d;
		v = &f;
		તોડો;

	મૂળભૂત:
		PX4_ERR("%s unrecognised node type %d", ગાંઠ_2->name, ગાંઠ_2->type);
		result = 1; // just skip this entry
		જાઓ out;
	}

	જો (param_set_external(param, v, સાચું, સાચું)) {
		debug("error setting value for '%s'", ગાંઠ_2->name);
		જાઓ out;
	}

	/* don't return zero, that means EOF */
	result = 1;

out:
	પાછા result;
}

સ્થિર પૂર્ણાંક
import_one_entry(uint8_t *buffer, size_t buf_size, શૂન્ય *arg)
{
	પૂર્ણાંક *result = static_cast<પૂર્ણાંક *>(arg);
	bson_decoder_s decoder{};

	જો (bson_decoder_init_buf(&decoder, buffer, buf_size, param_import_callback)) {
		debug("decoder init failed");
		*result = -1;
		પાછા -1;
	}

	કરો {
		*result = bson_decoder_next(&decoder);

	} જ્યારે (*result > 0);

	પાછા *result < 0 ? *result : 0;
}

સ્થિર પૂર્ણાંક
param_import_internal(તાર્કિક *legacy)
{
	પૂર્ણાંક result = 0;
	log_stale = ખોટું;
	પૂર્ણાંક n = parameter_flashfs_walk(parameters_legacy_token, import_one_entry, &result);
	*legacy = n > 0;

	જો (n == 0) {
		n = parameter_flashfs_walk(parameters_token, import_one_entry, &result);
	}

	જો (n < 0) {
		debug("flash walk failed (%d)", n);
		log_unreadable = સાચું;
		પાછા n;
	}

	જો (n == 0) {
		/* No valid entry found. A fully erased store is blank (first boot, or
		 * after switching firmware): report "not yet stored" (1), matching the
		 * convention used by param_load_default(). A store that holds data but
		 * no valid entry (torn write, bit-rot, or foreign data) is corrupt and
		 * must not be silently reseeded - report it so the boot recovery runs. */
		જો (parameter_flashfs_blank() == 1) {
			પાછા 1;
		}

		debug("flash holds data but no valid entry");
		પાછા -EILSEQ;
	}

	જો (result < 0) {
		debug("BSON error decoding parameters");
		પાછા result;
	}

	પાછા 0;
}

/*
 * Same-bank erase stalls instruction fetch for ~1 s on H7. Do it here at
 * boot, before sensors or DShot start, and only when a burst of deltas would
 * no longer fit, the store is still in the legacy layout, or the log holds
 * records this firmware translated or could not use. Only a failure after
 * the erase is an error: until then the log is intact and RAM holds what it
 * said, so the boot must not be reported as a corrupt store.
 */
સ્થિર પૂર્ણાંક
compact_after_import(તાર્કિક legacy)
{
	bson_encoder_s encoder{};
	પૂર્ણાંક result = encode_document(&encoder, nullptr, સાચું);

	જો (result != 0) {
		PX4_ERR("flashparams: snapshot encode failed (%d), log kept", result);
		મુક્ત(bson_encoder_buf_data(&encoder));
		પાછા 0;
	}

	અચલ size_t enc_size = bson_encoder_buf_size(&encoder);
	પૂર્ણાંક need = (legacy || log_stale) ? 1 : parameter_flashfs_needs_compact(parameters_token, enc_size);
	result = 0;

	જો (need > 0) {
		uint8_t *buffer;
		size_t staged_size;
		result = stage_snapshot(&encoder, &buffer, &staged_size);

		જો (result != OK) {
			PX4_ERR("flashparams: cannot stage snapshot (%d), log kept", result);
			result = 0;

		} નહીંતર {
			PX4_INFO("flashparams: compacting param sector%s", legacy ? " (legacy layout)" : "");
			result = commit_snapshot(buffer, staged_size);

			જો (result != OK) {
				PX4_ERR("flashparams: compact failed (%d)", result);
			}
		}
	}

	મુક્ત(bson_encoder_buf_data(&encoder));
	પાછા result;
}

પૂર્ણાંક flash_param_save(param_filter_func filter)
{
	પાછા param_export_internal(filter);
}

પૂર્ણાંક flash_param_import()
{
	તાર્કિક legacy = ખોટું;
	પૂર્ણાંક result = param_import_internal(&legacy);

	જો (result != 0) {
		પાછા result;
	}

	પાછા compact_after_import(legacy);
}
