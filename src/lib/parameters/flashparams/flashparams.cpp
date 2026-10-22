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

#अन्तर्भाव <px4_platform_common/defines.h>
#अन्तर्भाव <px4_platform_common/log.h>
#अन्तर्भाव <px4_platform_common/posix.h>
#अन्तर्भाव <px4_platform_common/shutdown.h>
#अन्तर्भाव <parameters/px4_parameters.hpp>

#अन्तर्भाव <string.h>
#अन्तर्भाव <stdbool.h>
#अन्तर्भाव <stdint.h>
#अन्तर्भाव <errno.h>

#अन्तर्भाव <parameters/param.h>

#अन्तर्भाव <lib/tinybson/tinybson.h>
#अन्तर्भाव "flashparams.h"
#अन्तर्भाव "flashfs.h"
#अन्तर्भाव "../param_translation.h"

#जइ 0
# define debug(fmt, args...)            करेहि { warnx(fmt, ##args); } जाव(0)
#अण्णहा
# define debug(fmt, args...)            करेहि { } जाव(0)
#endif


/**
 * Storage for modified parameters.
 */
संरचना param_wbuf_s {
	सङ्घ param_मानम्_u     val;
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
स्थिर तार्किक log_stale;

/* Replay failed inside a CRC-valid record. A delta appended behind it would
 * never be reached, so the next save rewrites the store. */
स्थिर तार्किक log_unreadable;

स्थिर तार्किक
any_unsaved(param_filter_func filter)
{
	प्रति (param_t param = 0; param < user_config.PARAM_गणनम्; param++) {
		जइ ((filter == nullptr || filter(param)) && param_मानम्_unsaved(param)) {
			प्रत्यागम सच्च;
		}
	}

	प्रत्यागम मिच्छ;
}

/*
 * A snapshot holds every override in RAM, equal to the current default or
 * not: the boot compaction runs before the airframe applies its set-default
 * values, so at that point "equals the default" says nothing. A delta holds
 * the unsaved params; one that no longer overrides, or now matches the
 * default in force, is written as a BSON null so replay drops it, which is
 * what the file backend's "don't export defaults" gives after a reload.
 */
स्थिर पूर्णाङ्क
encode_params(bson_encoder_s *encoder, param_filter_func filter, तार्किक snapshot)
{
	प्रति (param_t param = 0; param < user_config.PARAM_गणनम्; param++) {
		जइ (filter && !filter(param)) {
			अनुवर्त;
		}

		जइ (snapshot) {
			जइ (!user_config.contains(param)) {
				अनुवर्त;
			}

		} अण्णहा {
			जइ (!param_मानम्_unsaved(param)) {
				अनुवर्त;
			}

			जइ (!user_config.contains(param) || param_मानम्_is_default(param)) {
				जइ (bson_encoder_append_null(encoder, param_name(param))) {
					debug("BSON append failed for '%s'", param_name(param));
					प्रत्यागम -1;
				}

				अनुवर्त;
			}
		}

		विकल्प (param_type(param)) {
		पक्ष PARAM_TYPE_INT32:
			जइ (bson_encoder_append_int32(encoder, param_name(param), user_config.get(param).i)) {
				debug("BSON append failed for '%s'", param_name(param));
				प्रत्यागम -1;
			}

			भङ्ग;

		पक्ष PARAM_TYPE_FLOAT:
			जइ (bson_encoder_append_double(encoder, param_name(param), (द्विगुण)user_config.get(param).f)) {
				debug("BSON append failed for '%s'", param_name(param));
				प्रत्यागम -1;
			}

			भङ्ग;

		स्वभाव:
			debug("unrecognized parameter type");
			प्रत्यागम -1;
		}
	}

	प्रत्यागम 0;
}

/* The caller frees bson_encoder_buf_data() whatever the result. */
स्थिर पूर्णाङ्क
encode_document(bson_encoder_s *encoder, param_filter_func filter, तार्किक snapshot)
{
	bson_encoder_init_buf(encoder, nullptr, 0);

	पूर्णाङ्क result = encode_params(encoder, filter, snapshot);

	जइ (result == 0) {
		result = bson_encoder_fini(encoder);
	}

	प्रत्यागम result;
}

स्थिर पूर्णाङ्क
stage_write_buffer(bson_encoder_s *encoder, uint8_t **buffer, size_t *enc_size)
{
	शून्य *enc_buff = bson_encoder_buf_data(encoder);
	*enc_size = bson_encoder_buf_size(encoder);

	जइ (enc_buff == nullptr) {
		प्रत्यागम -ENOMEM;
	}

	size_t alloc_size = *enc_size;
	पूर्णाङ्क result = parameter_flashfs_alloc(parameters_token, buffer, &alloc_size);

	जइ (result != OK) {
		प्रत्यागम result;
	}

	जइ (alloc_size < *enc_size) {
		parameter_flashfs_free();
		प्रत्यागम -ENOMEM;
	}

	memcpy(*buffer, enc_buff, *enc_size);
	प्रत्यागम OK;
}

स्थिर पूर्णाङ्क
commit_append(uint8_t *buffer, size_t enc_size)
{
	पूर्णाङ्क result = parameter_flashfs_write(parameters_token, buffer, enc_size);
	parameter_flashfs_free();

	जइ (result == (पूर्णाङ्क)enc_size) {
		प्रत्यागम OK;
	}

	प्रत्यागम result < 0 ? result : -EFBIG;
}

स्थिर पूर्णाङ्क
export_delta(param_filter_func filter)
{
	bson_encoder_s encoder{};
	पूर्णाङ्क result = encode_document(&encoder, filter, मिच्छ);

	जइ (result == 0) {
		uint8_t *buffer;
		size_t enc_size;
		result = stage_write_buffer(&encoder, &buffer, &enc_size);

		जइ (result == OK) {
			result = commit_append(buffer, enc_size);
		}
	}

	मुक्त(bson_encoder_buf_data(&encoder));
	प्रत्यागम result;
}

/* Everything that can fail before the erase, so a failure here leaves the
 * log untouched and still loadable. */
स्थिर पूर्णाङ्क
stage_snapshot(bson_encoder_s *encoder, uint8_t **buffer, size_t *enc_size)
{
	पूर्णाङ्क result = stage_write_buffer(encoder, buffer, enc_size);

	जइ (result != OK) {
		प्रत्यागम result;
	}

	जइ (*enc_size > parameter_flashfs_max_payload()) {
		parameter_flashfs_free();
		प्रत्यागम -ENOSPC;
	}

	प्रत्यागम OK;
}

स्थिर पूर्णाङ्क
commit_snapshot(uint8_t *buffer, size_t enc_size)
{
	जइ (parameter_flashfs_blank() != 1) {
		पूर्णाङ्क rv = parameter_flashfs_erase();

		जइ (rv < 0) {
			parameter_flashfs_free();
			प्रत्यागम rv;
		}
	}

	पूर्णाङ्क result = parameter_flashfs_write(parameters_token, buffer, enc_size);

	जइ (result != (पूर्णाङ्क)enc_size && parameter_flashfs_blank() == 1) {
		result = parameter_flashfs_write(parameters_token, buffer, enc_size);
	}

	parameter_flashfs_free();

	जइ (result == (पूर्णाङ्क)enc_size) {
		प्रत्यागम OK;
	}

	प्रत्यागम result < 0 ? result : -EFBIG;
}

स्थिर पूर्णाङ्क
compact_to_snapshot()
{
	bson_encoder_s encoder{};
	पूर्णाङ्क result = encode_document(&encoder, nullptr, सच्च);

	जइ (result == 0) {
		uint8_t *buffer;
		size_t enc_size;
		result = stage_snapshot(&encoder, &buffer, &enc_size);

		जइ (result == OK) {
			result = commit_snapshot(buffer, enc_size);
		}
	}

	जइ (result == OK) {
		log_unreadable = मिच्छ;
	}

	मुक्त(bson_encoder_buf_data(&encoder));
	प्रत्यागम result;
}

स्थिर तार्किक
has_record(flash_file_token_t token)
{
	uint8_t *buffer = nullptr;
	size_t buf_size = 0;
	प्रत्यागम parameter_flashfs_read(token, &buffer, &buf_size) >= 0;
}

स्थिर पूर्णाङ्क
param_export_internal(param_filter_func filter)
{
	/* A delta only means something behind a snapshot that replays. A legacy
	 * record still present means the boot migration did not run; replay
	 * would take it over any delta appended behind it. */
	जइ (log_unreadable || has_record(parameters_legacy_token) || !has_record(parameters_token)) {
		प्रत्यागम compact_to_snapshot();
	}

	जइ (!any_unsaved(filter)) {
		प्रत्यागम 0;
	}

	पूर्णाङ्क result = export_delta(filter);

	जइ (result != -ENOSPC) {
		प्रत्यागम result;
	}

	/* Same-bank erase stalls the CPU for ~1 s; last resort when a single
	 * save does not fit the tail. Compact writes the full RAM snapshot so a
	 * filtered save cannot drop params not in the delta. */
	PX4_WARN("flashparams: param sector full, compacting");
	प्रत्यागम compact_to_snapshot();
}


स्थिर तार्किक
record_is_stale(bson_ग्रन्थिः_t ग्रन्थिः_2)
{
	ध्रुव param_t param = param_find_no_notification(ग्रन्थिः_2->name);

	जइ (param == PARAM_INVALID) {
		प्रत्यागम सच्च;
	}

	विकल्प (ग्रन्थिः_2->type) {
	पक्ष BSON_INT32:
		प्रत्यागम param_type(param) != PARAM_TYPE_INT32;

	पक्ष BSON_DOUBLE:
		प्रत्यागम param_type(param) != PARAM_TYPE_FLOAT;

	स्वभाव:
		प्रत्यागम मिच्छ;
	}
}

स्थिर पूर्णाङ्क
param_import_callback(bson_decoder_t decoder, bson_ग्रन्थिः_t ग्रन्थिः_2)
{
	भिन्न f;
	int32_t i;
	शून्य *v = nullptr;
	पूर्णाङ्क result = -1;

	/*
	 * EOO means the end of the parameter object. (Currently not supporting
	 * nested BSON objects).
	 */
	जइ (ग्रन्थिः_2->type == BSON_EOO) {
		debug("end of parameters");
		प्रत्यागम 0;
	}

	/* A tombstone has no value. The decoder leaves the previous node's in
	 * the union and the translations read it without a type check. */
	ध्रुव तार्किक tombstone = ग्रन्थिः_2->type == BSON_nullptr || ग्रन्थिः_2->type == BSON_UNDEFINED;

	जइ (!tombstone) {
		जइ (record_is_stale(ग्रन्थिः_2)) {
			log_stale = सच्च;
		}

		जइ (param_modify_on_import(ग्रन्थिः_2) == param_modify_on_import_ret::PARAM_SKIP_IMPORT) {
			प्रत्यागम 1;
		}
	}

	/*
	 * Find the parameter this node represents.  If we don't know it,
	 * ignore the node.
	 */
	param_t param = param_find_no_notification(ग्रन्थिः_2->name);

	जइ (param == PARAM_INVALID) {
		debug("ignoring unrecognised parameter '%s'", ग्रन्थिः_2->name);
		प्रत्यागम 1;
	}

	/*
	 * Handle setting the parameter from the node
	 */

	विकल्प (ग्रन्थिः_2->type) {
	पक्ष BSON_nullptr:
	पक्ष BSON_UNDEFINED:
		user_config.reset(param);
		result = 1;
		गच्छ out;

	पक्ष BSON_INT32:
		जइ (param_type(param) != PARAM_TYPE_INT32) {
			PX4_WARN("unexpected type for %s", ग्रन्थिः_2->name);
			result = 1; // just skip this entry
			गच्छ out;
		}

		i = ग्रन्थिः_2->i32;
		v = &i;
		भङ्ग;

	पक्ष BSON_DOUBLE:
		जइ (param_type(param) != PARAM_TYPE_FLOAT) {
			PX4_WARN("unexpected type for %s", ग्रन्थिः_2->name);
			result = 1; // just skip this entry
			गच्छ out;
		}

		f = ग्रन्थिः_2->d;
		v = &f;
		भङ्ग;

	स्वभाव:
		PX4_ERR("%s unrecognised node type %d", ग्रन्थिः_2->name, ग्रन्थिः_2->type);
		result = 1; // just skip this entry
		गच्छ out;
	}

	जइ (param_set_external(param, v, सच्च, सच्च)) {
		debug("error setting value for '%s'", ग्रन्थिः_2->name);
		गच्छ out;
	}

	/* don't return zero, that means EOF */
	result = 1;

out:
	प्रत्यागम result;
}

स्थिर पूर्णाङ्क
import_one_entry(uint8_t *buffer, size_t buf_size, शून्य *arg)
{
	पूर्णाङ्क *result = static_cast<पूर्णाङ्क *>(arg);
	bson_decoder_s decoder{};

	जइ (bson_decoder_init_buf(&decoder, buffer, buf_size, param_import_callback)) {
		debug("decoder init failed");
		*result = -1;
		प्रत्यागम -1;
	}

	करेहि {
		*result = bson_decoder_next(&decoder);

	} जाव (*result > 0);

	प्रत्यागम *result < 0 ? *result : 0;
}

स्थिर पूर्णाङ्क
param_import_internal(तार्किक *legacy)
{
	पूर्णाङ्क result = 0;
	log_stale = मिच्छ;
	पूर्णाङ्क n = parameter_flashfs_walk(parameters_legacy_token, import_one_entry, &result);
	*legacy = n > 0;

	जइ (n == 0) {
		n = parameter_flashfs_walk(parameters_token, import_one_entry, &result);
	}

	जइ (n < 0) {
		debug("flash walk failed (%d)", n);
		log_unreadable = सच्च;
		प्रत्यागम n;
	}

	जइ (n == 0) {
		/* No valid entry found. A fully erased store is blank (first boot, or
		 * after switching firmware): report "not yet stored" (1), matching the
		 * convention used by param_load_default(). A store that holds data but
		 * no valid entry (torn write, bit-rot, or foreign data) is corrupt and
		 * must not be silently reseeded - report it so the boot recovery runs. */
		जइ (parameter_flashfs_blank() == 1) {
			प्रत्यागम 1;
		}

		debug("flash holds data but no valid entry");
		प्रत्यागम -EILSEQ;
	}

	जइ (result < 0) {
		debug("BSON error decoding parameters");
		प्रत्यागम result;
	}

	प्रत्यागम 0;
}

/*
 * Same-bank erase stalls instruction fetch for ~1 s on H7. Do it here at
 * boot, before sensors or DShot start, and only when a burst of deltas would
 * no longer fit, the store is still in the legacy layout, or the log holds
 * records this firmware translated or could not use. Only a failure after
 * the erase is an error: until then the log is intact and RAM holds what it
 * said, so the boot must not be reported as a corrupt store.
 */
स्थिर पूर्णाङ्क
compact_after_import(तार्किक legacy)
{
	bson_encoder_s encoder{};
	पूर्णाङ्क result = encode_document(&encoder, nullptr, सच्च);

	जइ (result != 0) {
		PX4_ERR("flashparams: snapshot encode failed (%d), log kept", result);
		मुक्त(bson_encoder_buf_data(&encoder));
		प्रत्यागम 0;
	}

	ध्रुव size_t enc_size = bson_encoder_buf_size(&encoder);
	पूर्णाङ्क need = (legacy || log_stale) ? 1 : parameter_flashfs_needs_compact(parameters_token, enc_size);
	result = 0;

	जइ (need > 0) {
		uint8_t *buffer;
		size_t staged_size;
		result = stage_snapshot(&encoder, &buffer, &staged_size);

		जइ (result != OK) {
			PX4_ERR("flashparams: cannot stage snapshot (%d), log kept", result);
			result = 0;

		} अण्णहा {
			PX4_INFO("flashparams: compacting param sector%s", legacy ? " (legacy layout)" : "");
			result = commit_snapshot(buffer, staged_size);

			जइ (result != OK) {
				PX4_ERR("flashparams: compact failed (%d)", result);
			}
		}
	}

	मुक्त(bson_encoder_buf_data(&encoder));
	प्रत्यागम result;
}

पूर्णाङ्क flash_param_save(param_filter_func filter)
{
	प्रत्यागम param_export_internal(filter);
}

पूर्णाङ्क flash_param_import()
{
	तार्किक legacy = मिच्छ;
	पूर्णाङ्क result = param_import_internal(&legacy);

	जइ (result != 0) {
		प्रत्यागम result;
	}

	प्रत्यागम compact_after_import(legacy);
}
