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

#ضمن <px4_platform_common/defines.h>
#ضمن <px4_platform_common/log.h>
#ضمن <px4_platform_common/posix.h>
#ضمن <px4_platform_common/shutdown.h>
#ضمن <parameters/px4_parameters.hpp>

#ضمن <string.h>
#ضمن <stdbool.h>
#ضمن <stdint.h>
#ضمن <errno.h>

#ضمن <parameters/param.h>

#ضمن <lib/tinybson/tinybson.h>
#ضمن "flashparams.h"
#ضمن "flashfs.h"
#ضمن "../param_translation.h"

#ܐܢ 0
# define debug(fmt, args...)            ܥܒܕ { warnx(fmt, ##args); } ܟܡܐ(0)
#ܐܚܪܢܐ
# define debug(fmt, args...)            ܥܒܕ { } ܟܡܐ(0)
#endif


/**
 * Storage for modified parameters.
 */
بنية param_wbuf_s {
	اتحاد param_value_u     val;
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
ساكن منطقي log_stale;

/* Replay failed inside a CRC-valid record. A delta appended behind it would
 * never be reached, so the next save rewrites the store. */
ساكن منطقي log_unreadable;

ساكن منطقي
any_unsaved(param_filter_func filter)
{
	لكل (param_t param = 0; param < user_config.PARAM_COUNT; param++) {
		ܐܢ ((filter == nullptr || filter(param)) && param_value_unsaved(param)) {
			ܦܢܐ ܫܪܪܐ;
		}
	}

	ܦܢܐ ܕܓܠܐ;
}

/*
 * A snapshot holds every override in RAM, equal to the current default or
 * not: the boot compaction runs before the airframe applies its set-default
 * values, so at that point "equals the default" says nothing. A delta holds
 * the unsaved params; one that no longer overrides, or now matches the
 * default in force, is written as a BSON null so replay drops it, which is
 * what the file backend's "don't export defaults" gives after a reload.
 */
ساكن صحيح
encode_params(bson_encoder_s *encoder, param_filter_func filter, منطقي snapshot)
{
	لكل (param_t param = 0; param < user_config.PARAM_COUNT; param++) {
		ܐܢ (filter && !filter(param)) {
			استمر;
		}

		ܐܢ (snapshot) {
			ܐܢ (!user_config.contains(param)) {
				استمر;
			}

		} ܐܚܪܢܐ {
			ܐܢ (!param_value_unsaved(param)) {
				استمر;
			}

			ܐܢ (!user_config.contains(param) || param_value_is_default(param)) {
				ܐܢ (bson_encoder_append_null(encoder, param_name(param))) {
					debug("BSON append failed for '%s'", param_name(param));
					ܦܢܐ -1;
				}

				استمر;
			}
		}

		بدل (param_type(param)) {
		حالة PARAM_TYPE_INT32:
			ܐܢ (bson_encoder_append_int32(encoder, param_name(param), user_config.get(param).i)) {
				debug("BSON append failed for '%s'", param_name(param));
				ܦܢܐ -1;
			}

			اكسر;

		حالة PARAM_TYPE_FLOAT:
			ܐܢ (bson_encoder_append_double(encoder, param_name(param), (مزدوج)user_config.get(param).f)) {
				debug("BSON append failed for '%s'", param_name(param));
				ܦܢܐ -1;
			}

			اكسر;

		افتراضي:
			debug("unrecognized parameter type");
			ܦܢܐ -1;
		}
	}

	ܦܢܐ 0;
}

/* The caller frees bson_encoder_buf_data() whatever the result. */
ساكن صحيح
encode_document(bson_encoder_s *encoder, param_filter_func filter, منطقي snapshot)
{
	bson_encoder_init_buf(encoder, nullptr, 0);

	صحيح result = encode_params(encoder, filter, snapshot);

	ܐܢ (result == 0) {
		result = bson_encoder_fini(encoder);
	}

	ܦܢܐ result;
}

ساكن صحيح
stage_write_buffer(bson_encoder_s *encoder, uint8_t **buffer, size_t *enc_size)
{
	فراغ *enc_buff = bson_encoder_buf_data(encoder);
	*enc_size = bson_encoder_buf_size(encoder);

	ܐܢ (enc_buff == nullptr) {
		ܦܢܐ -ENOMEM;
	}

	size_t alloc_size = *enc_size;
	صحيح result = parameter_flashfs_alloc(parameters_token, buffer, &alloc_size);

	ܐܢ (result != OK) {
		ܦܢܐ result;
	}

	ܐܢ (alloc_size < *enc_size) {
		parameter_flashfs_free();
		ܦܢܐ -ENOMEM;
	}

	memcpy(*buffer, enc_buff, *enc_size);
	ܦܢܐ OK;
}

ساكن صحيح
commit_append(uint8_t *buffer, size_t enc_size)
{
	صحيح result = parameter_flashfs_write(parameters_token, buffer, enc_size);
	parameter_flashfs_free();

	ܐܢ (result == (صحيح)enc_size) {
		ܦܢܐ OK;
	}

	ܦܢܐ result < 0 ? result : -EFBIG;
}

ساكن صحيح
export_delta(param_filter_func filter)
{
	bson_encoder_s encoder{};
	صحيح result = encode_document(&encoder, filter, ܕܓܠܐ);

	ܐܢ (result == 0) {
		uint8_t *buffer;
		size_t enc_size;
		result = stage_write_buffer(&encoder, &buffer, &enc_size);

		ܐܢ (result == OK) {
			result = commit_append(buffer, enc_size);
		}
	}

	حرر(bson_encoder_buf_data(&encoder));
	ܦܢܐ result;
}

/* Everything that can fail before the erase, so a failure here leaves the
 * log untouched and still loadable. */
ساكن صحيح
stage_snapshot(bson_encoder_s *encoder, uint8_t **buffer, size_t *enc_size)
{
	صحيح result = stage_write_buffer(encoder, buffer, enc_size);

	ܐܢ (result != OK) {
		ܦܢܐ result;
	}

	ܐܢ (*enc_size > parameter_flashfs_max_payload()) {
		parameter_flashfs_free();
		ܦܢܐ -ENOSPC;
	}

	ܦܢܐ OK;
}

ساكن صحيح
commit_snapshot(uint8_t *buffer, size_t enc_size)
{
	ܐܢ (parameter_flashfs_blank() != 1) {
		صحيح rv = parameter_flashfs_erase();

		ܐܢ (rv < 0) {
			parameter_flashfs_free();
			ܦܢܐ rv;
		}
	}

	صحيح result = parameter_flashfs_write(parameters_token, buffer, enc_size);

	ܐܢ (result != (صحيح)enc_size && parameter_flashfs_blank() == 1) {
		result = parameter_flashfs_write(parameters_token, buffer, enc_size);
	}

	parameter_flashfs_free();

	ܐܢ (result == (صحيح)enc_size) {
		ܦܢܐ OK;
	}

	ܦܢܐ result < 0 ? result : -EFBIG;
}

ساكن صحيح
compact_to_snapshot()
{
	bson_encoder_s encoder{};
	صحيح result = encode_document(&encoder, nullptr, ܫܪܪܐ);

	ܐܢ (result == 0) {
		uint8_t *buffer;
		size_t enc_size;
		result = stage_snapshot(&encoder, &buffer, &enc_size);

		ܐܢ (result == OK) {
			result = commit_snapshot(buffer, enc_size);
		}
	}

	ܐܢ (result == OK) {
		log_unreadable = ܕܓܠܐ;
	}

	حرر(bson_encoder_buf_data(&encoder));
	ܦܢܐ result;
}

ساكن منطقي
has_record(flash_file_token_t token)
{
	uint8_t *buffer = nullptr;
	size_t buf_size = 0;
	ܦܢܐ parameter_flashfs_read(token, &buffer, &buf_size) >= 0;
}

ساكن صحيح
param_export_internal(param_filter_func filter)
{
	/* A delta only means something behind a snapshot that replays. A legacy
	 * record still present means the boot migration did not run; replay
	 * would take it over any delta appended behind it. */
	ܐܢ (log_unreadable || has_record(parameters_legacy_token) || !has_record(parameters_token)) {
		ܦܢܐ compact_to_snapshot();
	}

	ܐܢ (!any_unsaved(filter)) {
		ܦܢܐ 0;
	}

	صحيح result = export_delta(filter);

	ܐܢ (result != -ENOSPC) {
		ܦܢܐ result;
	}

	/* Same-bank erase stalls the CPU for ~1 s; last resort when a single
	 * save does not fit the tail. Compact writes the full RAM snapshot so a
	 * filtered save cannot drop params not in the delta. */
	PX4_WARN("flashparams: param sector full, compacting");
	ܦܢܐ compact_to_snapshot();
}


ساكن منطقي
record_is_stale(bson_node_t node)
{
	ثابت param_t param = param_find_no_notification(node->name);

	ܐܢ (param == PARAM_INVALID) {
		ܦܢܐ ܫܪܪܐ;
	}

	بدل (node->type) {
	حالة BSON_INT32:
		ܦܢܐ param_type(param) != PARAM_TYPE_INT32;

	حالة BSON_DOUBLE:
		ܦܢܐ param_type(param) != PARAM_TYPE_FLOAT;

	افتراضي:
		ܦܢܐ ܕܓܠܐ;
	}
}

ساكن صحيح
param_import_callback(bson_decoder_t decoder, bson_node_t node)
{
	عائم f;
	int32_t i;
	فراغ *v = nullptr;
	صحيح result = -1;

	/*
	 * EOO means the end of the parameter object. (Currently not supporting
	 * nested BSON objects).
	 */
	ܐܢ (node->type == BSON_EOO) {
		debug("end of parameters");
		ܦܢܐ 0;
	}

	/* A tombstone has no value. The decoder leaves the previous node's in
	 * the union and the translations read it without a type check. */
	ثابت منطقي tombstone = node->type == BSON_nullptr || node->type == BSON_UNDEFINED;

	ܐܢ (!tombstone) {
		ܐܢ (record_is_stale(node)) {
			log_stale = ܫܪܪܐ;
		}

		ܐܢ (param_modify_on_import(node) == param_modify_on_import_ret::PARAM_SKIP_IMPORT) {
			ܦܢܐ 1;
		}
	}

	/*
	 * Find the parameter this node represents.  If we don't know it,
	 * ignore the node.
	 */
	param_t param = param_find_no_notification(node->name);

	ܐܢ (param == PARAM_INVALID) {
		debug("ignoring unrecognised parameter '%s'", node->name);
		ܦܢܐ 1;
	}

	/*
	 * Handle setting the parameter from the node
	 */

	بدل (node->type) {
	حالة BSON_nullptr:
	حالة BSON_UNDEFINED:
		user_config.reset(param);
		result = 1;
		اذهب out;

	حالة BSON_INT32:
		ܐܢ (param_type(param) != PARAM_TYPE_INT32) {
			PX4_WARN("unexpected type for %s", node->name);
			result = 1; // just skip this entry
			اذهب out;
		}

		i = node->i32;
		v = &i;
		اكسر;

	حالة BSON_DOUBLE:
		ܐܢ (param_type(param) != PARAM_TYPE_FLOAT) {
			PX4_WARN("unexpected type for %s", node->name);
			result = 1; // just skip this entry
			اذهب out;
		}

		f = node->d;
		v = &f;
		اكسر;

	افتراضي:
		PX4_ERR("%s unrecognised node type %d", node->name, node->type);
		result = 1; // just skip this entry
		اذهب out;
	}

	ܐܢ (param_set_external(param, v, ܫܪܪܐ, ܫܪܪܐ)) {
		debug("error setting value for '%s'", node->name);
		اذهب out;
	}

	/* don't return zero, that means EOF */
	result = 1;

out:
	ܦܢܐ result;
}

ساكن صحيح
import_one_entry(uint8_t *buffer, size_t buf_size, فراغ *arg)
{
	صحيح *result = static_cast<صحيح *>(arg);
	bson_decoder_s decoder{};

	ܐܢ (bson_decoder_init_buf(&decoder, buffer, buf_size, param_import_callback)) {
		debug("decoder init failed");
		*result = -1;
		ܦܢܐ -1;
	}

	ܥܒܕ {
		*result = bson_decoder_next(&decoder);

	} ܟܡܐ (*result > 0);

	ܦܢܐ *result < 0 ? *result : 0;
}

ساكن صحيح
param_import_internal(منطقي *legacy)
{
	صحيح result = 0;
	log_stale = ܕܓܠܐ;
	صحيح n = parameter_flashfs_walk(parameters_legacy_token, import_one_entry, &result);
	*legacy = n > 0;

	ܐܢ (n == 0) {
		n = parameter_flashfs_walk(parameters_token, import_one_entry, &result);
	}

	ܐܢ (n < 0) {
		debug("flash walk failed (%d)", n);
		log_unreadable = ܫܪܪܐ;
		ܦܢܐ n;
	}

	ܐܢ (n == 0) {
		/* No valid entry found. A fully erased store is blank (first boot, or
		 * after switching firmware): report "not yet stored" (1), matching the
		 * convention used by param_load_default(). A store that holds data but
		 * no valid entry (torn write, bit-rot, or foreign data) is corrupt and
		 * must not be silently reseeded - report it so the boot recovery runs. */
		ܐܢ (parameter_flashfs_blank() == 1) {
			ܦܢܐ 1;
		}

		debug("flash holds data but no valid entry");
		ܦܢܐ -EILSEQ;
	}

	ܐܢ (result < 0) {
		debug("BSON error decoding parameters");
		ܦܢܐ result;
	}

	ܦܢܐ 0;
}

/*
 * Same-bank erase stalls instruction fetch for ~1 s on H7. Do it here at
 * boot, before sensors or DShot start, and only when a burst of deltas would
 * no longer fit, the store is still in the legacy layout, or the log holds
 * records this firmware translated or could not use. Only a failure after
 * the erase is an error: until then the log is intact and RAM holds what it
 * said, so the boot must not be reported as a corrupt store.
 */
ساكن صحيح
compact_after_import(منطقي legacy)
{
	bson_encoder_s encoder{};
	صحيح result = encode_document(&encoder, nullptr, ܫܪܪܐ);

	ܐܢ (result != 0) {
		PX4_ERR("flashparams: snapshot encode failed (%d), log kept", result);
		حرر(bson_encoder_buf_data(&encoder));
		ܦܢܐ 0;
	}

	ثابت size_t enc_size = bson_encoder_buf_size(&encoder);
	صحيح need = (legacy || log_stale) ? 1 : parameter_flashfs_needs_compact(parameters_token, enc_size);
	result = 0;

	ܐܢ (need > 0) {
		uint8_t *buffer;
		size_t staged_size;
		result = stage_snapshot(&encoder, &buffer, &staged_size);

		ܐܢ (result != OK) {
			PX4_ERR("flashparams: cannot stage snapshot (%d), log kept", result);
			result = 0;

		} ܐܚܪܢܐ {
			PX4_INFO("flashparams: compacting param sector%s", legacy ? " (legacy layout)" : "");
			result = commit_snapshot(buffer, staged_size);

			ܐܢ (result != OK) {
				PX4_ERR("flashparams: compact failed (%d)", result);
			}
		}
	}

	حرر(bson_encoder_buf_data(&encoder));
	ܦܢܐ result;
}

صحيح flash_param_save(param_filter_func filter)
{
	ܦܢܐ param_export_internal(filter);
}

صحيح flash_param_import()
{
	منطقي legacy = ܕܓܠܐ;
	صحيح result = param_import_internal(&legacy);

	ܐܢ (result != 0) {
		ܦܢܐ result;
	}

	ܦܢܐ compact_after_import(legacy);
}
