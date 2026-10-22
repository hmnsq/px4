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

#ಅನ್ತರ್ಭಾವ <px4_platform_common/defines.h>
#ಅನ್ತರ್ಭಾವ <px4_platform_common/log.h>
#ಅನ್ತರ್ಭಾವ <px4_platform_common/posix.h>
#ಅನ್ತರ್ಭಾವ <px4_platform_common/shutdown.h>
#ಅನ್ತರ್ಭಾವ <parameters/px4_parameters.hpp>

#ಅನ್ತರ್ಭಾವ <string.h>
#ಅನ್ತರ್ಭಾವ <stdbool.h>
#ಅನ್ತರ್ಭಾವ <stdint.h>
#ಅನ್ತರ್ಭಾವ <errno.h>

#ಅನ್ತರ್ಭಾವ <parameters/param.h>

#ಅನ್ತರ್ಭಾವ <lib/tinybson/tinybson.h>
#ಅನ್ತರ್ಭಾವ "flashparams.h"
#ಅನ್ತರ್ಭಾವ "flashfs.h"
#ಅನ್ತರ್ಭಾವ "../param_translation.h"

#ಯದಿ 0
# define debug(fmt, args...)            ಕುರು { warnx(fmt, ##args); } ಯಾವತ್(0)
#ಅನ್ಯಥಾ
# define debug(fmt, args...)            ಕುರು { } ಯಾವತ್(0)
#endif


/**
 * Storage for modified parameters.
 */
ಸಂರಚನಾ param_wbuf_s {
	ಸಙ್ಘ param_ಮೌಲ್ಯ_u     val;
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
ಸ್ಥಿರ ತಾರ್ಕಿಕ log_stale;

/* Replay failed inside a CRC-valid record. A delta appended behind it would
 * never be reached, so the next save rewrites the store. */
ಸ್ಥಿರ ತಾರ್ಕಿಕ log_unreadable;

ಸ್ಥಿರ ತಾರ್ಕಿಕ
any_unsaved(param_filter_func filter)
{
	ಪ್ರತಿ (param_t param = 0; param < user_config.PARAM_ಎಣಿಕೆ; param++) {
		ಯದಿ ((filter == nullptr || filter(param)) && param_ಮೌಲ್ಯ_unsaved(param)) {
			ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
		}
	}

	ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
}

/*
 * A snapshot holds every override in RAM, equal to the current default or
 * not: the boot compaction runs before the airframe applies its set-default
 * values, so at that point "equals the default" says nothing. A delta holds
 * the unsaved params; one that no longer overrides, or now matches the
 * default in force, is written as a BSON null so replay drops it, which is
 * what the file backend's "don't export defaults" gives after a reload.
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
encode_params(bson_encoder_s *encoder, param_filter_func filter, ತಾರ್ಕಿಕ snapshot)
{
	ಪ್ರತಿ (param_t param = 0; param < user_config.PARAM_ಎಣಿಕೆ; param++) {
		ಯದಿ (filter && !filter(param)) {
			ಅನುವರ್ತ;
		}

		ಯದಿ (snapshot) {
			ಯದಿ (!user_config.contains(param)) {
				ಅನುವರ್ತ;
			}

		} ಅನ್ಯಥಾ {
			ಯದಿ (!param_ಮೌಲ್ಯ_unsaved(param)) {
				ಅನುವರ್ತ;
			}

			ಯದಿ (!user_config.contains(param) || param_ಮೌಲ್ಯ_is_default(param)) {
				ಯದಿ (bson_encoder_append_null(encoder, param_name(param))) {
					debug("BSON append failed for '%s'", param_name(param));
					ಪ್ರತ್ಯಾಗಮ -1;
				}

				ಅನುವರ್ತ;
			}
		}

		ವಿಕಲ್ಪ (param_type(param)) {
		ಪಕ್ಷ PARAM_TYPE_INT32:
			ಯದಿ (bson_encoder_append_int32(encoder, param_name(param), user_config.get(param).i)) {
				debug("BSON append failed for '%s'", param_name(param));
				ಪ್ರತ್ಯಾಗಮ -1;
			}

			ಭಙ್ಗ;

		ಪಕ್ಷ PARAM_TYPE_FLOAT:
			ಯದಿ (bson_encoder_append_double(encoder, param_name(param), (ದ್ವಿಗುಣ)user_config.get(param).f)) {
				debug("BSON append failed for '%s'", param_name(param));
				ಪ್ರತ್ಯಾಗಮ -1;
			}

			ಭಙ್ಗ;

		ಸ್ವಭಾವ:
			debug("unrecognized parameter type");
			ಪ್ರತ್ಯಾಗಮ -1;
		}
	}

	ಪ್ರತ್ಯಾಗಮ 0;
}

/* The caller frees bson_encoder_buf_data() whatever the result. */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
encode_document(bson_encoder_s *encoder, param_filter_func filter, ತಾರ್ಕಿಕ snapshot)
{
	bson_encoder_init_buf(encoder, nullptr, 0);

	ಪೂರ್ಣಾಙ್ಕ result = encode_params(encoder, filter, snapshot);

	ಯದಿ (result == 0) {
		result = bson_encoder_fini(encoder);
	}

	ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
stage_write_buffer(bson_encoder_s *encoder, uint8_t **buffer, size_t *enc_size)
{
	ಶೂನ್ಯ *enc_buff = bson_encoder_buf_data(encoder);
	*enc_size = bson_encoder_buf_size(encoder);

	ಯದಿ (enc_buff == nullptr) {
		ಪ್ರತ್ಯಾಗಮ -ENOMEM;
	}

	size_t alloc_size = *enc_size;
	ಪೂರ್ಣಾಙ್ಕ result = parameter_flashfs_alloc(parameters_token, buffer, &alloc_size);

	ಯದಿ (result != OK) {
		ಪ್ರತ್ಯಾಗಮ result;
	}

	ಯದಿ (alloc_size < *enc_size) {
		parameter_flashfs_free();
		ಪ್ರತ್ಯಾಗಮ -ENOMEM;
	}

	memcpy(*buffer, enc_buff, *enc_size);
	ಪ್ರತ್ಯಾಗಮ OK;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
commit_append(uint8_t *buffer, size_t enc_size)
{
	ಪೂರ್ಣಾಙ್ಕ result = parameter_flashfs_write(parameters_token, buffer, enc_size);
	parameter_flashfs_free();

	ಯದಿ (result == (ಪೂರ್ಣಾಙ್ಕ)enc_size) {
		ಪ್ರತ್ಯಾಗಮ OK;
	}

	ಪ್ರತ್ಯಾಗಮ result < 0 ? result : -EFBIG;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
export_delta(param_filter_func filter)
{
	bson_encoder_s encoder{};
	ಪೂರ್ಣಾಙ್ಕ result = encode_document(&encoder, filter, ಅಸತ್ಯ);

	ಯದಿ (result == 0) {
		uint8_t *buffer;
		size_t enc_size;
		result = stage_write_buffer(&encoder, &buffer, &enc_size);

		ಯದಿ (result == OK) {
			result = commit_append(buffer, enc_size);
		}
	}

	ಮುಕ್ತ(bson_encoder_buf_data(&encoder));
	ಪ್ರತ್ಯಾಗಮ result;
}

/* Everything that can fail before the erase, so a failure here leaves the
 * log untouched and still loadable. */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
stage_snapshot(bson_encoder_s *encoder, uint8_t **buffer, size_t *enc_size)
{
	ಪೂರ್ಣಾಙ್ಕ result = stage_write_buffer(encoder, buffer, enc_size);

	ಯದಿ (result != OK) {
		ಪ್ರತ್ಯಾಗಮ result;
	}

	ಯದಿ (*enc_size > parameter_flashfs_max_payload()) {
		parameter_flashfs_free();
		ಪ್ರತ್ಯಾಗಮ -ENOSPC;
	}

	ಪ್ರತ್ಯಾಗಮ OK;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
commit_snapshot(uint8_t *buffer, size_t enc_size)
{
	ಯದಿ (parameter_flashfs_blank() != 1) {
		ಪೂರ್ಣಾಙ್ಕ rv = parameter_flashfs_erase();

		ಯದಿ (rv < 0) {
			parameter_flashfs_free();
			ಪ್ರತ್ಯಾಗಮ rv;
		}
	}

	ಪೂರ್ಣಾಙ್ಕ result = parameter_flashfs_write(parameters_token, buffer, enc_size);

	ಯದಿ (result != (ಪೂರ್ಣಾಙ್ಕ)enc_size && parameter_flashfs_blank() == 1) {
		result = parameter_flashfs_write(parameters_token, buffer, enc_size);
	}

	parameter_flashfs_free();

	ಯದಿ (result == (ಪೂರ್ಣಾಙ್ಕ)enc_size) {
		ಪ್ರತ್ಯಾಗಮ OK;
	}

	ಪ್ರತ್ಯಾಗಮ result < 0 ? result : -EFBIG;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
compact_to_snapshot()
{
	bson_encoder_s encoder{};
	ಪೂರ್ಣಾಙ್ಕ result = encode_document(&encoder, nullptr, ಸತ್ಯ);

	ಯದಿ (result == 0) {
		uint8_t *buffer;
		size_t enc_size;
		result = stage_snapshot(&encoder, &buffer, &enc_size);

		ಯದಿ (result == OK) {
			result = commit_snapshot(buffer, enc_size);
		}
	}

	ಯದಿ (result == OK) {
		log_unreadable = ಅಸತ್ಯ;
	}

	ಮುಕ್ತ(bson_encoder_buf_data(&encoder));
	ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ ತಾರ್ಕಿಕ
has_record(flash_file_token_t token)
{
	uint8_t *buffer = nullptr;
	size_t buf_size = 0;
	ಪ್ರತ್ಯಾಗಮ parameter_flashfs_read(token, &buffer, &buf_size) >= 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
param_export_internal(param_filter_func filter)
{
	/* A delta only means something behind a snapshot that replays. A legacy
	 * record still present means the boot migration did not run; replay
	 * would take it over any delta appended behind it. */
	ಯದಿ (log_unreadable || has_record(parameters_legacy_token) || !has_record(parameters_token)) {
		ಪ್ರತ್ಯಾಗಮ compact_to_snapshot();
	}

	ಯದಿ (!any_unsaved(filter)) {
		ಪ್ರತ್ಯಾಗಮ 0;
	}

	ಪೂರ್ಣಾಙ್ಕ result = export_delta(filter);

	ಯದಿ (result != -ENOSPC) {
		ಪ್ರತ್ಯಾಗಮ result;
	}

	/* Same-bank erase stalls the CPU for ~1 s; last resort when a single
	 * save does not fit the tail. Compact writes the full RAM snapshot so a
	 * filtered save cannot drop params not in the delta. */
	PX4_WARN("flashparams: param sector full, compacting");
	ಪ್ರತ್ಯಾಗಮ compact_to_snapshot();
}


ಸ್ಥಿರ ತಾರ್ಕಿಕ
record_is_stale(bson_ಗಂಟು_t ಗಂಟು_2)
{
	ಧ್ರುವ param_t param = param_find_no_notification(ಗಂಟು_2->name);

	ಯದಿ (param == PARAM_INVALID) {
		ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
	}

	ವಿಕಲ್ಪ (ಗಂಟು_2->type) {
	ಪಕ್ಷ BSON_INT32:
		ಪ್ರತ್ಯಾಗಮ param_type(param) != PARAM_TYPE_INT32;

	ಪಕ್ಷ BSON_DOUBLE:
		ಪ್ರತ್ಯಾಗಮ param_type(param) != PARAM_TYPE_FLOAT;

	ಸ್ವಭಾವ:
		ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
	}
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
param_import_callback(bson_decoder_t decoder, bson_ಗಂಟು_t ಗಂಟು_2)
{
	ಭಿನ್ನ f;
	int32_t i;
	ಶೂನ್ಯ *v = nullptr;
	ಪೂರ್ಣಾಙ್ಕ result = -1;

	/*
	 * EOO means the end of the parameter object. (Currently not supporting
	 * nested BSON objects).
	 */
	ಯದಿ (ಗಂಟು_2->type == BSON_EOO) {
		debug("end of parameters");
		ಪ್ರತ್ಯಾಗಮ 0;
	}

	/* A tombstone has no value. The decoder leaves the previous node's in
	 * the union and the translations read it without a type check. */
	ಧ್ರುವ ತಾರ್ಕಿಕ tombstone = ಗಂಟು_2->type == BSON_nullptr || ಗಂಟು_2->type == BSON_UNDEFINED;

	ಯದಿ (!tombstone) {
		ಯದಿ (record_is_stale(ಗಂಟು_2)) {
			log_stale = ಸತ್ಯ;
		}

		ಯದಿ (param_modify_on_import(ಗಂಟು_2) == param_modify_on_import_ret::PARAM_SKIP_IMPORT) {
			ಪ್ರತ್ಯಾಗಮ 1;
		}
	}

	/*
	 * Find the parameter this node represents.  If we don't know it,
	 * ignore the node.
	 */
	param_t param = param_find_no_notification(ಗಂಟು_2->name);

	ಯದಿ (param == PARAM_INVALID) {
		debug("ignoring unrecognised parameter '%s'", ಗಂಟು_2->name);
		ಪ್ರತ್ಯಾಗಮ 1;
	}

	/*
	 * Handle setting the parameter from the node
	 */

	ವಿಕಲ್ಪ (ಗಂಟು_2->type) {
	ಪಕ್ಷ BSON_nullptr:
	ಪಕ್ಷ BSON_UNDEFINED:
		user_config.reset(param);
		result = 1;
		ಗಚ್ಛ out;

	ಪಕ್ಷ BSON_INT32:
		ಯದಿ (param_type(param) != PARAM_TYPE_INT32) {
			PX4_WARN("unexpected type for %s", ಗಂಟು_2->name);
			result = 1; // just skip this entry
			ಗಚ್ಛ out;
		}

		i = ಗಂಟು_2->i32;
		v = &i;
		ಭಙ್ಗ;

	ಪಕ್ಷ BSON_DOUBLE:
		ಯದಿ (param_type(param) != PARAM_TYPE_FLOAT) {
			PX4_WARN("unexpected type for %s", ಗಂಟು_2->name);
			result = 1; // just skip this entry
			ಗಚ್ಛ out;
		}

		f = ಗಂಟು_2->d;
		v = &f;
		ಭಙ್ಗ;

	ಸ್ವಭಾವ:
		PX4_ERR("%s unrecognised node type %d", ಗಂಟು_2->name, ಗಂಟು_2->type);
		result = 1; // just skip this entry
		ಗಚ್ಛ out;
	}

	ಯದಿ (param_set_external(param, v, ಸತ್ಯ, ಸತ್ಯ)) {
		debug("error setting value for '%s'", ಗಂಟು_2->name);
		ಗಚ್ಛ out;
	}

	/* don't return zero, that means EOF */
	result = 1;

out:
	ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
import_one_entry(uint8_t *buffer, size_t buf_size, ಶೂನ್ಯ *arg)
{
	ಪೂರ್ಣಾಙ್ಕ *result = static_cast<ಪೂರ್ಣಾಙ್ಕ *>(arg);
	bson_decoder_s decoder{};

	ಯದಿ (bson_decoder_init_buf(&decoder, buffer, buf_size, param_import_callback)) {
		debug("decoder init failed");
		*result = -1;
		ಪ್ರತ್ಯಾಗಮ -1;
	}

	ಕುರು {
		*result = bson_decoder_next(&decoder);

	} ಯಾವತ್ (*result > 0);

	ಪ್ರತ್ಯಾಗಮ *result < 0 ? *result : 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
param_import_internal(ತಾರ್ಕಿಕ *legacy)
{
	ಪೂರ್ಣಾಙ್ಕ result = 0;
	log_stale = ಅಸತ್ಯ;
	ಪೂರ್ಣಾಙ್ಕ n = parameter_flashfs_walk(parameters_legacy_token, import_one_entry, &result);
	*legacy = n > 0;

	ಯದಿ (n == 0) {
		n = parameter_flashfs_walk(parameters_token, import_one_entry, &result);
	}

	ಯದಿ (n < 0) {
		debug("flash walk failed (%d)", n);
		log_unreadable = ಸತ್ಯ;
		ಪ್ರತ್ಯಾಗಮ n;
	}

	ಯದಿ (n == 0) {
		/* No valid entry found. A fully erased store is blank (first boot, or
		 * after switching firmware): report "not yet stored" (1), matching the
		 * convention used by param_load_default(). A store that holds data but
		 * no valid entry (torn write, bit-rot, or foreign data) is corrupt and
		 * must not be silently reseeded - report it so the boot recovery runs. */
		ಯದಿ (parameter_flashfs_blank() == 1) {
			ಪ್ರತ್ಯಾಗಮ 1;
		}

		debug("flash holds data but no valid entry");
		ಪ್ರತ್ಯಾಗಮ -EILSEQ;
	}

	ಯದಿ (result < 0) {
		debug("BSON error decoding parameters");
		ಪ್ರತ್ಯಾಗಮ result;
	}

	ಪ್ರತ್ಯಾಗಮ 0;
}

/*
 * Same-bank erase stalls instruction fetch for ~1 s on H7. Do it here at
 * boot, before sensors or DShot start, and only when a burst of deltas would
 * no longer fit, the store is still in the legacy layout, or the log holds
 * records this firmware translated or could not use. Only a failure after
 * the erase is an error: until then the log is intact and RAM holds what it
 * said, so the boot must not be reported as a corrupt store.
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
compact_after_import(ತಾರ್ಕಿಕ legacy)
{
	bson_encoder_s encoder{};
	ಪೂರ್ಣಾಙ್ಕ result = encode_document(&encoder, nullptr, ಸತ್ಯ);

	ಯದಿ (result != 0) {
		PX4_ERR("flashparams: snapshot encode failed (%d), log kept", result);
		ಮುಕ್ತ(bson_encoder_buf_data(&encoder));
		ಪ್ರತ್ಯಾಗಮ 0;
	}

	ಧ್ರುವ size_t enc_size = bson_encoder_buf_size(&encoder);
	ಪೂರ್ಣಾಙ್ಕ need = (legacy || log_stale) ? 1 : parameter_flashfs_needs_compact(parameters_token, enc_size);
	result = 0;

	ಯದಿ (need > 0) {
		uint8_t *buffer;
		size_t staged_size;
		result = stage_snapshot(&encoder, &buffer, &staged_size);

		ಯದಿ (result != OK) {
			PX4_ERR("flashparams: cannot stage snapshot (%d), log kept", result);
			result = 0;

		} ಅನ್ಯಥಾ {
			PX4_INFO("flashparams: compacting param sector%s", legacy ? " (legacy layout)" : "");
			result = commit_snapshot(buffer, staged_size);

			ಯದಿ (result != OK) {
				PX4_ERR("flashparams: compact failed (%d)", result);
			}
		}
	}

	ಮುಕ್ತ(bson_encoder_buf_data(&encoder));
	ಪ್ರತ್ಯಾಗಮ result;
}

ಪೂರ್ಣಾಙ್ಕ flash_param_save(param_filter_func filter)
{
	ಪ್ರತ್ಯಾಗಮ param_export_internal(filter);
}

ಪೂರ್ಣಾಙ್ಕ flash_param_import()
{
	ತಾರ್ಕಿಕ legacy = ಅಸತ್ಯ;
	ಪೂರ್ಣಾಙ್ಕ result = param_import_internal(&legacy);

	ಯದಿ (result != 0) {
		ಪ್ರತ್ಯಾಗಮ result;
	}

	ಪ್ರತ್ಯಾಗಮ compact_after_import(legacy);
}
