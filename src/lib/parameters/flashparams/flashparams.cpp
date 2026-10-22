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

#ಆದರೆ 0
# define debug(fmt, ವಾದಗಳು...)            ಮಾಡು { warnx(fmt, ##args); } ಆಗ(0)
#ಇಲ್ಲದಿದ್ದರೆ
# define debug(fmt, ವಾದಗಳು...)            ಮಾಡು { } ಆಗ(0)
#ಆದರೆ_ಅಂತ್ಯ


/**
 * Storage for modified parameters.
 */
ರಚನೆ param_wbuf_s {
	ಒಕ್ಕೂಟ param_ಮೌಲ್ಯ_u     val;
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
	ಗಾಗಿ (param_t param = 0; param < user_config.PARAM_ಎಣಿಕೆ; param++) {
		ಆದರೆ ((filter == ಶೂನ್ಯಸೂಚಕ || filter(param)) && param_ಮೌಲ್ಯ_unsaved(param)) {
			ಹಿಂತಿರುಗಿ ನಿಜ;
		}
	}

	ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}

/*
 * A snapshot holds every override in RAM, equal to the current default or
 * not: the boot compaction runs before the airframe applies its set-default
 * values, so at that point "equals the default" says nothing. A delta holds
 * the unsaved params; one that no longer overrides, or now matches the
 * default in force, is written as a BSON null so replay drops it, which is
 * what the file backend's "don't export defaults" gives after a reload.
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
encode_params(bson_encoder_s *encoder, param_filter_func filter, ತಾರ್ಕಿಕ snapshot)
{
	ಗಾಗಿ (param_t param = 0; param < user_config.PARAM_ಎಣಿಕೆ; param++) {
		ಆದರೆ (filter && !filter(param)) {
			ಮುಂದುವರಿ;
		}

		ಆದರೆ (snapshot) {
			ಆದರೆ (!user_config.ಒಳಗೊಂಡಿದೆ(param)) {
				ಮುಂದುವರಿ;
			}

		} ಇಲ್ಲದಿದ್ದರೆ {
			ಆದರೆ (!param_ಮೌಲ್ಯ_unsaved(param)) {
				ಮುಂದುವರಿ;
			}

			ಆದರೆ (!user_config.ಒಳಗೊಂಡಿದೆ(param) || param_ಮೌಲ್ಯ_is_default(param)) {
				ಆದರೆ (bson_encoder_append_null(encoder, param_name(param))) {
					debug("BSON append failed for '%s'", param_name(param));
					ಹಿಂತಿರುಗಿ -1;
				}

				ಮುಂದುವರಿ;
			}
		}

		ಆಯ್ಕೆ (param_type(param)) {
		ಸ್ಥಿತಿ PARAM_TYPE_INT32:
			ಆದರೆ (bson_encoder_append_int32(encoder, param_name(param), user_config.get(param).i)) {
				debug("BSON append failed for '%s'", param_name(param));
				ಹಿಂತಿರುಗಿ -1;
			}

			ನಿಲ್ಲಿಸು;

		ಸ್ಥಿತಿ PARAM_TYPE_FLOAT:
			ಆದರೆ (bson_encoder_append_double(encoder, param_name(param), (ದ್ವಿಗುಣ)user_config.get(param).f)) {
				debug("BSON append failed for '%s'", param_name(param));
				ಹಿಂತಿರುಗಿ -1;
			}

			ನಿಲ್ಲಿಸು;

		ಪೂರ್ವನಿಯೋಜಿತ:
			debug("unrecognized parameter type");
			ಹಿಂತಿರುಗಿ -1;
		}
	}

	ಹಿಂತಿರುಗಿ 0;
}

/* The caller frees bson_encoder_buf_data() whatever the result. */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
encode_document(bson_encoder_s *encoder, param_filter_func filter, ತಾರ್ಕಿಕ snapshot)
{
	bson_encoder_init_buf(encoder, ಶೂನ್ಯಸೂಚಕ, 0);

	ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ = encode_params(encoder, filter, snapshot);

	ಆದರೆ (ಫಲಿತಾಂಶ == 0) {
		ಫಲಿತಾಂಶ = bson_encoder_fini(encoder);
	}

	ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
stage_write_ಬಫರ್(bson_encoder_s *encoder, uint8_t **ಬಫರ್_2, size_t *enc_ಗಾತ್ರ)
{
	ಶೂನ್ಯ *enc_buff = bson_encoder_buf_ದತ್ತಾಂಶ(encoder);
	*enc_ಗಾತ್ರ = bson_encoder_buf_ಗಾತ್ರ(encoder);

	ಆದರೆ (enc_buff == ಶೂನ್ಯಸೂಚಕ) {
		ಹಿಂತಿರುಗಿ -ENOMEM;
	}

	size_t alloc_ಗಾತ್ರ = *enc_ಗಾತ್ರ;
	ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ = parameter_flashfs_alloc(parameters_token, ಬಫರ್_2, &alloc_ಗಾತ್ರ);

	ಆದರೆ (ಫಲಿತಾಂಶ != OK) {
		ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
	}

	ಆದರೆ (alloc_ಗಾತ್ರ < *enc_ಗಾತ್ರ) {
		parameter_flashfs_free();
		ಹಿಂತಿರುಗಿ -ENOMEM;
	}

	ಸ್ಮರಣೆ_ನಕಲು(*ಬಫರ್_2, enc_buff, *enc_ಗಾತ್ರ);
	ಹಿಂತಿರುಗಿ OK;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
commit_append(uint8_t *ಬಫರ್_2, size_t enc_ಗಾತ್ರ)
{
	ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ = parameter_flashfs_write(parameters_token, ಬಫರ್_2, enc_ಗಾತ್ರ);
	parameter_flashfs_free();

	ಆದರೆ (ಫಲಿತಾಂಶ == (ಪೂರ್ಣಾಂಕ)enc_ಗಾತ್ರ) {
		ಹಿಂತಿರುಗಿ OK;
	}

	ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ < 0 ? ಫಲಿತಾಂಶ : -EFBIG;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
export_delta(param_filter_func filter)
{
	bson_encoder_s encoder{};
	ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ = encode_document(&encoder, filter, ಸುಳ್ಳು);

	ಆದರೆ (ಫಲಿತಾಂಶ == 0) {
		uint8_t *ಬಫರ್_2;
		size_t enc_ಗಾತ್ರ;
		ಫಲಿತಾಂಶ = stage_write_ಬಫರ್(&encoder, &ಬಫರ್_2, &enc_ಗಾತ್ರ);

		ಆದರೆ (ಫಲಿತಾಂಶ == OK) {
			ಫಲಿತಾಂಶ = commit_append(ಬಫರ್_2, enc_ಗಾತ್ರ);
		}
	}

	ಬಿಡುಗಡೆ(bson_encoder_buf_ದತ್ತಾಂಶ(&encoder));
	ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/* Everything that can fail before the erase, so a failure here leaves the
 * log untouched and still loadable. */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
stage_snapshot(bson_encoder_s *encoder, uint8_t **ಬಫರ್_2, size_t *enc_ಗಾತ್ರ)
{
	ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ = stage_write_ಬಫರ್(encoder, ಬಫರ್_2, enc_ಗಾತ್ರ);

	ಆದರೆ (ಫಲಿತಾಂಶ != OK) {
		ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
	}

	ಆದರೆ (*enc_ಗಾತ್ರ > parameter_flashfs_max_payload()) {
		parameter_flashfs_free();
		ಹಿಂತಿರುಗಿ -ENOSPC;
	}

	ಹಿಂತಿರುಗಿ OK;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
commit_snapshot(uint8_t *ಬಫರ್_2, size_t enc_ಗಾತ್ರ)
{
	ಆದರೆ (parameter_flashfs_blank() != 1) {
		ಪೂರ್ಣಾಂಕ rv = parameter_flashfs_erase();

		ಆದರೆ (rv < 0) {
			parameter_flashfs_free();
			ಹಿಂತಿರುಗಿ rv;
		}
	}

	ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ = parameter_flashfs_write(parameters_token, ಬಫರ್_2, enc_ಗಾತ್ರ);

	ಆದರೆ (ಫಲಿತಾಂಶ != (ಪೂರ್ಣಾಂಕ)enc_ಗಾತ್ರ && parameter_flashfs_blank() == 1) {
		ಫಲಿತಾಂಶ = parameter_flashfs_write(parameters_token, ಬಫರ್_2, enc_ಗಾತ್ರ);
	}

	parameter_flashfs_free();

	ಆದರೆ (ಫಲಿತಾಂಶ == (ಪೂರ್ಣಾಂಕ)enc_ಗಾತ್ರ) {
		ಹಿಂತಿರುಗಿ OK;
	}

	ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ < 0 ? ಫಲಿತಾಂಶ : -EFBIG;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
compact_ಗಮ್ಯ_snapshot()
{
	bson_encoder_s encoder{};
	ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ = encode_document(&encoder, ಶೂನ್ಯಸೂಚಕ, ನಿಜ);

	ಆದರೆ (ಫಲಿತಾಂಶ == 0) {
		uint8_t *ಬಫರ್_2;
		size_t enc_ಗಾತ್ರ;
		ಫಲಿತಾಂಶ = stage_snapshot(&encoder, &ಬಫರ್_2, &enc_ಗಾತ್ರ);

		ಆದರೆ (ಫಲಿತಾಂಶ == OK) {
			ಫಲಿತಾಂಶ = commit_snapshot(ಬಫರ್_2, enc_ಗಾತ್ರ);
		}
	}

	ಆದರೆ (ಫಲಿತಾಂಶ == OK) {
		log_unreadable = ಸುಳ್ಳು;
	}

	ಬಿಡುಗಡೆ(bson_encoder_buf_ದತ್ತಾಂಶ(&encoder));
	ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ ತಾರ್ಕಿಕ
has_ದಾಖಲೆ(flash_file_token_t token)
{
	uint8_t *ಬಫರ್_2 = ಶೂನ್ಯಸೂಚಕ;
	size_t buf_ಗಾತ್ರ = 0;
	ಹಿಂತಿರುಗಿ parameter_flashfs_read(token, &ಬಫರ್_2, &buf_ಗಾತ್ರ) >= 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
param_export_ಆಂತರಿಕ(param_filter_func filter)
{
	/* A delta only means something behind a snapshot that replays. A legacy
	 * record still present means the boot migration did not run; replay
	 * would take it over any delta appended behind it. */
	ಆದರೆ (log_unreadable || has_ದಾಖಲೆ(parameters_legacy_token) || !has_ದಾಖಲೆ(parameters_token)) {
		ಹಿಂತಿರುಗಿ compact_ಗಮ್ಯ_snapshot();
	}

	ಆದರೆ (!any_unsaved(filter)) {
		ಹಿಂತಿರುಗಿ 0;
	}

	ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ = export_delta(filter);

	ಆದರೆ (ಫಲಿತಾಂಶ != -ENOSPC) {
		ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
	}

	/* Same-bank erase stalls the CPU for ~1 s; last resort when a single
	 * save does not fit the tail. Compact writes the full RAM snapshot so a
	 * filtered save cannot drop params not in the delta. */
	PX4_WARN("flashparams: param sector full, compacting");
	ಹಿಂತಿರುಗಿ compact_ಗಮ್ಯ_snapshot();
}


ಸ್ಥಿರ ತಾರ್ಕಿಕ
ದಾಖಲೆ_is_stale(bson_ಗಂಟು_t ಗಂಟು_2)
{
	ಸ್ಥಿರಾಂಕ param_t param = param_ಕಂಡುಹಿಡಿ_no_notification(ಗಂಟು_2->name);

	ಆದರೆ (param == PARAM_INVALID) {
		ಹಿಂತಿರುಗಿ ನಿಜ;
	}

	ಆಯ್ಕೆ (ಗಂಟು_2->type) {
	ಸ್ಥಿತಿ BSON_INT32:
		ಹಿಂತಿರುಗಿ param_type(param) != PARAM_TYPE_INT32;

	ಸ್ಥಿತಿ BSON_DOUBLE:
		ಹಿಂತಿರುಗಿ param_type(param) != PARAM_TYPE_FLOAT;

	ಪೂರ್ವನಿಯೋಜಿತ:
		ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
	}
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
param_import_callback(bson_decoder_t decoder, bson_ಗಂಟು_t ಗಂಟು_2)
{
	ದಶಮಾಂಶ f;
	int32_t i;
	ಶೂನ್ಯ *v = ಶೂನ್ಯಸೂಚಕ;
	ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ = -1;

	/*
	 * EOO means the end of the parameter object. (Currently not supporting
	 * nested BSON objects).
	 */
	ಆದರೆ (ಗಂಟು_2->type == BSON_EOO) {
		debug("end of parameters");
		ಹಿಂತಿರುಗಿ 0;
	}

	/* A tombstone has no value. The decoder leaves the previous node's in
	 * the union and the translations read it without a type check. */
	ಸ್ಥಿರಾಂಕ ತಾರ್ಕಿಕ tombstone = ಗಂಟು_2->type == BSON_nullptr || ಗಂಟು_2->type == BSON_UNDEFINED;

	ಆದರೆ (!tombstone) {
		ಆದರೆ (ದಾಖಲೆ_is_stale(ಗಂಟು_2)) {
			log_stale = ನಿಜ;
		}

		ಆದರೆ (param_modify_on_import(ಗಂಟು_2) == param_modify_on_import_ret::PARAM_SKIP_IMPORT) {
			ಹಿಂತಿರುಗಿ 1;
		}
	}

	/*
	 * Find the parameter this node represents.  If we don't know it,
	 * ignore the node.
	 */
	param_t param = param_ಕಂಡುಹಿಡಿ_no_notification(ಗಂಟು_2->name);

	ಆದರೆ (param == PARAM_INVALID) {
		debug("ignoring unrecognised parameter '%s'", ಗಂಟು_2->name);
		ಹಿಂತಿರುಗಿ 1;
	}

	/*
	 * Handle setting the parameter from the node
	 */

	ಆಯ್ಕೆ (ಗಂಟು_2->type) {
	ಸ್ಥಿತಿ BSON_nullptr:
	ಸ್ಥಿತಿ BSON_UNDEFINED:
		user_config.reset(param);
		ಫಲಿತಾಂಶ = 1;
		ಹೋಗು ಹೊರಹೋಗು;

	ಸ್ಥಿತಿ BSON_INT32:
		ಆದರೆ (param_type(param) != PARAM_TYPE_INT32) {
			PX4_WARN("unexpected type for %s", ಗಂಟು_2->name);
			ಫಲಿತಾಂಶ = 1; // just skip this entry
			ಹೋಗು ಹೊರಹೋಗು;
		}

		i = ಗಂಟು_2->i32;
		v = &i;
		ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ BSON_DOUBLE:
		ಆದರೆ (param_type(param) != PARAM_TYPE_FLOAT) {
			PX4_WARN("unexpected type for %s", ಗಂಟು_2->name);
			ಫಲಿತಾಂಶ = 1; // just skip this entry
			ಹೋಗು ಹೊರಹೋಗು;
		}

		f = ಗಂಟು_2->d;
		v = &f;
		ನಿಲ್ಲಿಸು;

	ಪೂರ್ವನಿಯೋಜಿತ:
		PX4_ERR("%s unrecognised node type %d", ಗಂಟು_2->name, ಗಂಟು_2->type);
		ಫಲಿತಾಂಶ = 1; // just skip this entry
		ಹೋಗು ಹೊರಹೋಗು;
	}

	ಆದರೆ (param_ಗಣ_external(param, v, ನಿಜ, ನಿಜ)) {
		debug("error setting value for '%s'", ಗಂಟು_2->name);
		ಹೋಗು ಹೊರಹೋಗು;
	}

	/* don't return zero, that means EOF */
	ಫಲಿತಾಂಶ = 1;

ಹೊರಹೋಗು:
	ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
import_one_ನಮೂದು(uint8_t *ಬಫರ್_2, size_t buf_ಗಾತ್ರ, ಶೂನ್ಯ *arg)
{
	ಪೂರ್ಣಾಂಕ *ಫಲಿತಾಂಶ = static_cast<ಪೂರ್ಣಾಂಕ *>(arg);
	bson_decoder_s decoder{};

	ಆದರೆ (bson_decoder_init_buf(&decoder, ಬಫರ್_2, buf_ಗಾತ್ರ, param_import_callback)) {
		debug("decoder init failed");
		*ಫಲಿತಾಂಶ = -1;
		ಹಿಂತಿರುಗಿ -1;
	}

	ಮಾಡು {
		*ಫಲಿತಾಂಶ = bson_decoder_ಮುಂದಿನ(&decoder);

	} ಆಗ (*ಫಲಿತಾಂಶ > 0);

	ಹಿಂತಿರುಗಿ *ಫಲಿತಾಂಶ < 0 ? *ಫಲಿತಾಂಶ : 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
param_import_ಆಂತರಿಕ(ತಾರ್ಕಿಕ *legacy)
{
	ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ = 0;
	log_stale = ಸುಳ್ಳು;
	ಪೂರ್ಣಾಂಕ n = parameter_flashfs_walk(parameters_legacy_token, import_one_ನಮೂದು, &ಫಲಿತಾಂಶ);
	*legacy = n > 0;

	ಆದರೆ (n == 0) {
		n = parameter_flashfs_walk(parameters_token, import_one_ನಮೂದು, &ಫಲಿತಾಂಶ);
	}

	ಆದರೆ (n < 0) {
		debug("flash walk failed (%d)", n);
		log_unreadable = ನಿಜ;
		ಹಿಂತಿರುಗಿ n;
	}

	ಆದರೆ (n == 0) {
		/* No valid entry found. A fully erased store is blank (first boot, or
		 * after switching firmware): report "not yet stored" (1), matching the
		 * convention used by param_load_default(). A store that holds data but
		 * no valid entry (torn write, bit-rot, or foreign data) is corrupt and
		 * must not be silently reseeded - report it so the boot recovery runs. */
		ಆದರೆ (parameter_flashfs_blank() == 1) {
			ಹಿಂತಿರುಗಿ 1;
		}

		debug("flash holds data but no valid entry");
		ಹಿಂತಿರುಗಿ -EILSEQ;
	}

	ಆದರೆ (ಫಲಿತಾಂಶ < 0) {
		debug("BSON error decoding parameters");
		ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
	}

	ಹಿಂತಿರುಗಿ 0;
}

/*
 * Same-bank erase stalls instruction fetch for ~1 s on H7. Do it here at
 * boot, before sensors or DShot start, and only when a burst of deltas would
 * no longer fit, the store is still in the legacy layout, or the log holds
 * records this firmware translated or could not use. Only a failure after
 * the erase is an error: until then the log is intact and RAM holds what it
 * said, so the boot must not be reported as a corrupt store.
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
compact_after_import(ತಾರ್ಕಿಕ legacy)
{
	bson_encoder_s encoder{};
	ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ = encode_document(&encoder, ಶೂನ್ಯಸೂಚಕ, ನಿಜ);

	ಆದರೆ (ಫಲಿತಾಂಶ != 0) {
		PX4_ERR("flashparams: snapshot encode failed (%d), log kept", ಫಲಿತಾಂಶ);
		ಬಿಡುಗಡೆ(bson_encoder_buf_ದತ್ತಾಂಶ(&encoder));
		ಹಿಂತಿರುಗಿ 0;
	}

	ಸ್ಥಿರಾಂಕ size_t enc_ಗಾತ್ರ = bson_encoder_buf_ಗಾತ್ರ(&encoder);
	ಪೂರ್ಣಾಂಕ need = (legacy || log_stale) ? 1 : parameter_flashfs_needs_compact(parameters_token, enc_ಗಾತ್ರ);
	ಫಲಿತಾಂಶ = 0;

	ಆದರೆ (need > 0) {
		uint8_t *ಬಫರ್_2;
		size_t staged_ಗಾತ್ರ;
		ಫಲಿತಾಂಶ = stage_snapshot(&encoder, &ಬಫರ್_2, &staged_ಗಾತ್ರ);

		ಆದರೆ (ಫಲಿತಾಂಶ != OK) {
			PX4_ERR("flashparams: cannot stage snapshot (%d), log kept", ಫಲಿತಾಂಶ);
			ಫಲಿತಾಂಶ = 0;

		} ಇಲ್ಲದಿದ್ದರೆ {
			PX4_INFO("flashparams: compacting param sector%s", legacy ? " (legacy layout)" : "");
			ಫಲಿತಾಂಶ = commit_snapshot(ಬಫರ್_2, staged_ಗಾತ್ರ);

			ಆದರೆ (ಫಲಿತಾಂಶ != OK) {
				PX4_ERR("flashparams: compact failed (%d)", ಫಲಿತಾಂಶ);
			}
		}
	}

	ಬಿಡುಗಡೆ(bson_encoder_buf_ದತ್ತಾಂಶ(&encoder));
	ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಪೂರ್ಣಾಂಕ flash_param_save(param_filter_func filter)
{
	ಹಿಂತಿರುಗಿ param_export_ಆಂತರಿಕ(filter);
}

ಪೂರ್ಣಾಂಕ flash_param_import()
{
	ತಾರ್ಕಿಕ legacy = ಸುಳ್ಳು;
	ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ = param_import_ಆಂತರಿಕ(&legacy);

	ಆದರೆ (ಫಲಿತಾಂಶ != 0) {
		ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
	}

	ಹಿಂತಿರುಗಿ compact_after_import(legacy);
}
