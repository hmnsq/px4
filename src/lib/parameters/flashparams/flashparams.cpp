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

#यदि 0
# define debug(fmt, तर्काः...)            कुरु { warnx(fmt, ##args); } यावत्(0)
#अन्यथा
# define debug(fmt, तर्काः...)            कुरु { } यावत्(0)
#यदि_अन्तः


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
		यदि ((filter == शून्यसूचकः || filter(param)) && param_मानम्_unsaved(param)) {
			प्रत्यागम सत्य;
		}
	}

	प्रत्यागम असत्य;
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
		यदि (filter && !filter(param)) {
			अनुवर्त;
		}

		यदि (snapshot) {
			यदि (!user_config.अन्तर्भवति(param)) {
				अनुवर्त;
			}

		} अन्यथा {
			यदि (!param_मानम्_unsaved(param)) {
				अनुवर्त;
			}

			यदि (!user_config.अन्तर्भवति(param) || param_मानम्_is_default(param)) {
				यदि (bson_encoder_append_null(encoder, param_name(param))) {
					debug("BSON append failed for '%s'", param_name(param));
					प्रत्यागम -1;
				}

				अनुवर्त;
			}
		}

		विकल्प (param_type(param)) {
		पक्ष PARAM_TYPE_INT32:
			यदि (bson_encoder_append_int32(encoder, param_name(param), user_config.get(param).i)) {
				debug("BSON append failed for '%s'", param_name(param));
				प्रत्यागम -1;
			}

			भङ्ग;

		पक्ष PARAM_TYPE_FLOAT:
			यदि (bson_encoder_append_double(encoder, param_name(param), (द्विगुण)user_config.get(param).f)) {
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
	bson_encoder_init_buf(encoder, शून्यसूचकः, 0);

	पूर्णाङ्क परिणाम = encode_params(encoder, filter, snapshot);

	यदि (परिणाम == 0) {
		परिणाम = bson_encoder_fini(encoder);
	}

	प्रत्यागम परिणाम;
}

स्थिर पूर्णाङ्क
stage_write_बफर(bson_encoder_s *encoder, uint8_t **बफर_2, size_t *enc_आकार)
{
	शून्य *enc_buff = bson_encoder_buf_दत्त(encoder);
	*enc_आकार = bson_encoder_buf_आकार(encoder);

	यदि (enc_buff == शून्यसूचकः) {
		प्रत्यागम -ENOMEM;
	}

	size_t alloc_आकार = *enc_आकार;
	पूर्णाङ्क परिणाम = parameter_flashfs_alloc(parameters_token, बफर_2, &alloc_आकार);

	यदि (परिणाम != OK) {
		प्रत्यागम परिणाम;
	}

	यदि (alloc_आकार < *enc_आकार) {
		parameter_flashfs_free();
		प्रत्यागम -ENOMEM;
	}

	स्मृतिं_प्रतिलिख(*बफर_2, enc_buff, *enc_आकार);
	प्रत्यागम OK;
}

स्थिर पूर्णाङ्क
commit_append(uint8_t *बफर_2, size_t enc_आकार)
{
	पूर्णाङ्क परिणाम = parameter_flashfs_write(parameters_token, बफर_2, enc_आकार);
	parameter_flashfs_free();

	यदि (परिणाम == (पूर्णाङ्क)enc_आकार) {
		प्रत्यागम OK;
	}

	प्रत्यागम परिणाम < 0 ? परिणाम : -EFBIG;
}

स्थिर पूर्णाङ्क
export_delta(param_filter_func filter)
{
	bson_encoder_s encoder{};
	पूर्णाङ्क परिणाम = encode_document(&encoder, filter, असत्य);

	यदि (परिणाम == 0) {
		uint8_t *बफर_2;
		size_t enc_आकार;
		परिणाम = stage_write_बफर(&encoder, &बफर_2, &enc_आकार);

		यदि (परिणाम == OK) {
			परिणाम = commit_append(बफर_2, enc_आकार);
		}
	}

	मुक्त(bson_encoder_buf_दत्त(&encoder));
	प्रत्यागम परिणाम;
}

/* Everything that can fail before the erase, so a failure here leaves the
 * log untouched and still loadable. */
स्थिर पूर्णाङ्क
stage_snapshot(bson_encoder_s *encoder, uint8_t **बफर_2, size_t *enc_आकार)
{
	पूर्णाङ्क परिणाम = stage_write_बफर(encoder, बफर_2, enc_आकार);

	यदि (परिणाम != OK) {
		प्रत्यागम परिणाम;
	}

	यदि (*enc_आकार > parameter_flashfs_max_payload()) {
		parameter_flashfs_free();
		प्रत्यागम -ENOSPC;
	}

	प्रत्यागम OK;
}

स्थिर पूर्णाङ्क
commit_snapshot(uint8_t *बफर_2, size_t enc_आकार)
{
	यदि (parameter_flashfs_blank() != 1) {
		पूर्णाङ्क rv = parameter_flashfs_erase();

		यदि (rv < 0) {
			parameter_flashfs_free();
			प्रत्यागम rv;
		}
	}

	पूर्णाङ्क परिणाम = parameter_flashfs_write(parameters_token, बफर_2, enc_आकार);

	यदि (परिणाम != (पूर्णाङ्क)enc_आकार && parameter_flashfs_blank() == 1) {
		परिणाम = parameter_flashfs_write(parameters_token, बफर_2, enc_आकार);
	}

	parameter_flashfs_free();

	यदि (परिणाम == (पूर्णाङ्क)enc_आकार) {
		प्रत्यागम OK;
	}

	प्रत्यागम परिणाम < 0 ? परिणाम : -EFBIG;
}

स्थिर पूर्णाङ्क
compact_गन्तव्यम्_snapshot()
{
	bson_encoder_s encoder{};
	पूर्णाङ्क परिणाम = encode_document(&encoder, शून्यसूचकः, सत्य);

	यदि (परिणाम == 0) {
		uint8_t *बफर_2;
		size_t enc_आकार;
		परिणाम = stage_snapshot(&encoder, &बफर_2, &enc_आकार);

		यदि (परिणाम == OK) {
			परिणाम = commit_snapshot(बफर_2, enc_आकार);
		}
	}

	यदि (परिणाम == OK) {
		log_unreadable = असत्य;
	}

	मुक्त(bson_encoder_buf_दत्त(&encoder));
	प्रत्यागम परिणाम;
}

स्थिर तार्किक
has_लेखः(flash_file_token_t token)
{
	uint8_t *बफर_2 = शून्यसूचकः;
	size_t buf_आकार = 0;
	प्रत्यागम parameter_flashfs_read(token, &बफर_2, &buf_आकार) >= 0;
}

स्थिर पूर्णाङ्क
param_export_आन्तरिकम्(param_filter_func filter)
{
	/* A delta only means something behind a snapshot that replays. A legacy
	 * record still present means the boot migration did not run; replay
	 * would take it over any delta appended behind it. */
	यदि (log_unreadable || has_लेखः(parameters_legacy_token) || !has_लेखः(parameters_token)) {
		प्रत्यागम compact_गन्तव्यम्_snapshot();
	}

	यदि (!any_unsaved(filter)) {
		प्रत्यागम 0;
	}

	पूर्णाङ्क परिणाम = export_delta(filter);

	यदि (परिणाम != -ENOSPC) {
		प्रत्यागम परिणाम;
	}

	/* Same-bank erase stalls the CPU for ~1 s; last resort when a single
	 * save does not fit the tail. Compact writes the full RAM snapshot so a
	 * filtered save cannot drop params not in the delta. */
	PX4_WARN("flashparams: param sector full, compacting");
	प्रत्यागम compact_गन्तव्यम्_snapshot();
}


स्थिर तार्किक
लेखः_is_stale(bson_ग्रन्थिः_t ग्रन्थिः_2)
{
	ध्रुव param_t param = param_find_no_notification(ग्रन्थिः_2->name);

	यदि (param == PARAM_INVALID) {
		प्रत्यागम सत्य;
	}

	विकल्प (ग्रन्थिः_2->type) {
	पक्ष BSON_INT32:
		प्रत्यागम param_type(param) != PARAM_TYPE_INT32;

	पक्ष BSON_DOUBLE:
		प्रत्यागम param_type(param) != PARAM_TYPE_FLOAT;

	स्वभाव:
		प्रत्यागम असत्य;
	}
}

स्थिर पूर्णाङ्क
param_import_callback(bson_decoder_t decoder, bson_ग्रन्थिः_t ग्रन्थिः_2)
{
	भिन्न f;
	int32_t i;
	शून्य *v = शून्यसूचकः;
	पूर्णाङ्क परिणाम = -1;

	/*
	 * EOO means the end of the parameter object. (Currently not supporting
	 * nested BSON objects).
	 */
	यदि (ग्रन्थिः_2->type == BSON_EOO) {
		debug("end of parameters");
		प्रत्यागम 0;
	}

	/* A tombstone has no value. The decoder leaves the previous node's in
	 * the union and the translations read it without a type check. */
	ध्रुव तार्किक tombstone = ग्रन्थिः_2->type == BSON_nullptr || ग्रन्थिः_2->type == BSON_UNDEFINED;

	यदि (!tombstone) {
		यदि (लेखः_is_stale(ग्रन्थिः_2)) {
			log_stale = सत्य;
		}

		यदि (param_modify_on_import(ग्रन्थिः_2) == param_modify_on_import_ret::PARAM_SKIP_IMPORT) {
			प्रत्यागम 1;
		}
	}

	/*
	 * Find the parameter this node represents.  If we don't know it,
	 * ignore the node.
	 */
	param_t param = param_find_no_notification(ग्रन्थिः_2->name);

	यदि (param == PARAM_INVALID) {
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
		परिणाम = 1;
		गच्छ बहिर्गमनम्;

	पक्ष BSON_INT32:
		यदि (param_type(param) != PARAM_TYPE_INT32) {
			PX4_WARN("unexpected type for %s", ग्रन्थिः_2->name);
			परिणाम = 1; // just skip this entry
			गच्छ बहिर्गमनम्;
		}

		i = ग्रन्थिः_2->i32;
		v = &i;
		भङ्ग;

	पक्ष BSON_DOUBLE:
		यदि (param_type(param) != PARAM_TYPE_FLOAT) {
			PX4_WARN("unexpected type for %s", ग्रन्थिः_2->name);
			परिणाम = 1; // just skip this entry
			गच्छ बहिर्गमनम्;
		}

		f = ग्रन्थिः_2->d;
		v = &f;
		भङ्ग;

	स्वभाव:
		PX4_ERR("%s unrecognised node type %d", ग्रन्थिः_2->name, ग्रन्थिः_2->type);
		परिणाम = 1; // just skip this entry
		गच्छ बहिर्गमनम्;
	}

	यदि (param_समुच्चयः_external(param, v, सत्य, सत्य)) {
		debug("error setting value for '%s'", ग्रन्थिः_2->name);
		गच्छ बहिर्गमनम्;
	}

	/* don't return zero, that means EOF */
	परिणाम = 1;

बहिर्गमनम्:
	प्रत्यागम परिणाम;
}

स्थिर पूर्णाङ्क
import_one_प्रविष्टिः(uint8_t *बफर_2, size_t buf_आकार, शून्य *arg)
{
	पूर्णाङ्क *परिणाम = static_cast<पूर्णाङ्क *>(arg);
	bson_decoder_s decoder{};

	यदि (bson_decoder_init_buf(&decoder, बफर_2, buf_आकार, param_import_callback)) {
		debug("decoder init failed");
		*परिणाम = -1;
		प्रत्यागम -1;
	}

	कुरु {
		*परिणाम = bson_decoder_अग्र(&decoder);

	} यावत् (*परिणाम > 0);

	प्रत्यागम *परिणाम < 0 ? *परिणाम : 0;
}

स्थिर पूर्णाङ्क
param_import_आन्तरिकम्(तार्किक *legacy)
{
	पूर्णाङ्क परिणाम = 0;
	log_stale = असत्य;
	पूर्णाङ्क n = parameter_flashfs_walk(parameters_legacy_token, import_one_प्रविष्टिः, &परिणाम);
	*legacy = n > 0;

	यदि (n == 0) {
		n = parameter_flashfs_walk(parameters_token, import_one_प्रविष्टिः, &परिणाम);
	}

	यदि (n < 0) {
		debug("flash walk failed (%d)", n);
		log_unreadable = सत्य;
		प्रत्यागम n;
	}

	यदि (n == 0) {
		/* No valid entry found. A fully erased store is blank (first boot, or
		 * after switching firmware): report "not yet stored" (1), matching the
		 * convention used by param_load_default(). A store that holds data but
		 * no valid entry (torn write, bit-rot, or foreign data) is corrupt and
		 * must not be silently reseeded - report it so the boot recovery runs. */
		यदि (parameter_flashfs_blank() == 1) {
			प्रत्यागम 1;
		}

		debug("flash holds data but no valid entry");
		प्रत्यागम -EILSEQ;
	}

	यदि (परिणाम < 0) {
		debug("BSON error decoding parameters");
		प्रत्यागम परिणाम;
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
	पूर्णाङ्क परिणाम = encode_document(&encoder, शून्यसूचकः, सत्य);

	यदि (परिणाम != 0) {
		PX4_ERR("flashparams: snapshot encode failed (%d), log kept", परिणाम);
		मुक्त(bson_encoder_buf_दत्त(&encoder));
		प्रत्यागम 0;
	}

	ध्रुव size_t enc_आकार = bson_encoder_buf_आकार(&encoder);
	पूर्णाङ्क need = (legacy || log_stale) ? 1 : parameter_flashfs_needs_compact(parameters_token, enc_आकार);
	परिणाम = 0;

	यदि (need > 0) {
		uint8_t *बफर_2;
		size_t staged_आकार;
		परिणाम = stage_snapshot(&encoder, &बफर_2, &staged_आकार);

		यदि (परिणाम != OK) {
			PX4_ERR("flashparams: cannot stage snapshot (%d), log kept", परिणाम);
			परिणाम = 0;

		} अन्यथा {
			PX4_INFO("flashparams: compacting param sector%s", legacy ? " (legacy layout)" : "");
			परिणाम = commit_snapshot(बफर_2, staged_आकार);

			यदि (परिणाम != OK) {
				PX4_ERR("flashparams: compact failed (%d)", परिणाम);
			}
		}
	}

	मुक्त(bson_encoder_buf_दत्त(&encoder));
	प्रत्यागम परिणाम;
}

पूर्णाङ्क flash_param_save(param_filter_func filter)
{
	प्रत्यागम param_export_आन्तरिकम्(filter);
}

पूर्णाङ्क flash_param_import()
{
	तार्किक legacy = असत्य;
	पूर्णाङ्क परिणाम = param_import_आन्तरिकम्(&legacy);

	यदि (परिणाम != 0) {
		प्रत्यागम परिणाम;
	}

	प्रत्यागम compact_after_import(legacy);
}
