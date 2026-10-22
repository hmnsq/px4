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

#जर 0
# define debug(fmt, args...)            करा { warnx(fmt, ##args); } पर्यंत(0)
#अन्यथा
# define debug(fmt, args...)            करा { } पर्यंत(0)
#endif


/**
 * Storage for modified parameters.
 */
संरचना param_wbuf_s {
	संघ param_मूल्य_u     val;
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
	साठी (param_t param = 0; param < user_config.PARAM_मोजणी; param++) {
		जर ((filter == nullptr || filter(param)) && param_मूल्य_unsaved(param)) {
			परत सत्य;
		}
	}

	परत असत्य;
}

/*
 * A snapshot holds every override in RAM, equal to the current default or
 * not: the boot compaction runs before the airframe applies its set-default
 * values, so at that point "equals the default" says nothing. A delta holds
 * the unsaved params; one that no longer overrides, or now matches the
 * default in force, is written as a BSON null so replay drops it, which is
 * what the file backend's "don't export defaults" gives after a reload.
 */
स्थिर पूर्णांक
encode_params(bson_encoder_s *encoder, param_filter_func filter, तार्किक snapshot)
{
	साठी (param_t param = 0; param < user_config.PARAM_मोजणी; param++) {
		जर (filter && !filter(param)) {
			सुरूठेवा;
		}

		जर (snapshot) {
			जर (!user_config.contains(param)) {
				सुरूठेवा;
			}

		} अन्यथा {
			जर (!param_मूल्य_unsaved(param)) {
				सुरूठेवा;
			}

			जर (!user_config.contains(param) || param_मूल्य_is_default(param)) {
				जर (bson_encoder_append_null(encoder, param_name(param))) {
					debug("BSON append failed for '%s'", param_name(param));
					परत -1;
				}

				सुरूठेवा;
			}
		}

		निवड (param_type(param)) {
		स्थिती PARAM_TYPE_INT32:
			जर (bson_encoder_append_int32(encoder, param_name(param), user_config.get(param).i)) {
				debug("BSON append failed for '%s'", param_name(param));
				परत -1;
			}

			थांबा;

		स्थिती PARAM_TYPE_FLOAT:
			जर (bson_encoder_append_double(encoder, param_name(param), (द्विगुण)user_config.get(param).f)) {
				debug("BSON append failed for '%s'", param_name(param));
				परत -1;
			}

			थांबा;

		पूर्वनिर्धारित:
			debug("unrecognized parameter type");
			परत -1;
		}
	}

	परत 0;
}

/* The caller frees bson_encoder_buf_data() whatever the result. */
स्थिर पूर्णांक
encode_document(bson_encoder_s *encoder, param_filter_func filter, तार्किक snapshot)
{
	bson_encoder_init_buf(encoder, nullptr, 0);

	पूर्णांक result = encode_params(encoder, filter, snapshot);

	जर (result == 0) {
		result = bson_encoder_fini(encoder);
	}

	परत result;
}

स्थिर पूर्णांक
stage_write_buffer(bson_encoder_s *encoder, uint8_t **buffer, माप_प्रकार *enc_size)
{
	शून्य *enc_buff = bson_encoder_buf_data(encoder);
	*enc_size = bson_encoder_buf_size(encoder);

	जर (enc_buff == nullptr) {
		परत -ENOMEM;
	}

	माप_प्रकार alloc_size = *enc_size;
	पूर्णांक result = parameter_flashfs_alloc(parameters_token, buffer, &alloc_size);

	जर (result != OK) {
		परत result;
	}

	जर (alloc_size < *enc_size) {
		parameter_flashfs_free();
		परत -ENOMEM;
	}

	स_नकल(*buffer, enc_buff, *enc_size);
	परत OK;
}

स्थिर पूर्णांक
commit_append(uint8_t *buffer, माप_प्रकार enc_size)
{
	पूर्णांक result = parameter_flashfs_write(parameters_token, buffer, enc_size);
	parameter_flashfs_free();

	जर (result == (पूर्णांक)enc_size) {
		परत OK;
	}

	परत result < 0 ? result : -EFBIG;
}

स्थिर पूर्णांक
export_delta(param_filter_func filter)
{
	bson_encoder_s encoder{};
	पूर्णांक result = encode_document(&encoder, filter, असत्य);

	जर (result == 0) {
		uint8_t *buffer;
		माप_प्रकार enc_size;
		result = stage_write_buffer(&encoder, &buffer, &enc_size);

		जर (result == OK) {
			result = commit_append(buffer, enc_size);
		}
	}

	मुक्त(bson_encoder_buf_data(&encoder));
	परत result;
}

/* Everything that can fail before the erase, so a failure here leaves the
 * log untouched and still loadable. */
स्थिर पूर्णांक
stage_snapshot(bson_encoder_s *encoder, uint8_t **buffer, माप_प्रकार *enc_size)
{
	पूर्णांक result = stage_write_buffer(encoder, buffer, enc_size);

	जर (result != OK) {
		परत result;
	}

	जर (*enc_size > parameter_flashfs_max_payload()) {
		parameter_flashfs_free();
		परत -ENOSPC;
	}

	परत OK;
}

स्थिर पूर्णांक
commit_snapshot(uint8_t *buffer, माप_प्रकार enc_size)
{
	जर (parameter_flashfs_blank() != 1) {
		पूर्णांक rv = parameter_flashfs_erase();

		जर (rv < 0) {
			parameter_flashfs_free();
			परत rv;
		}
	}

	पूर्णांक result = parameter_flashfs_write(parameters_token, buffer, enc_size);

	जर (result != (पूर्णांक)enc_size && parameter_flashfs_blank() == 1) {
		result = parameter_flashfs_write(parameters_token, buffer, enc_size);
	}

	parameter_flashfs_free();

	जर (result == (पूर्णांक)enc_size) {
		परत OK;
	}

	परत result < 0 ? result : -EFBIG;
}

स्थिर पूर्णांक
compact_to_snapshot()
{
	bson_encoder_s encoder{};
	पूर्णांक result = encode_document(&encoder, nullptr, सत्य);

	जर (result == 0) {
		uint8_t *buffer;
		माप_प्रकार enc_size;
		result = stage_snapshot(&encoder, &buffer, &enc_size);

		जर (result == OK) {
			result = commit_snapshot(buffer, enc_size);
		}
	}

	जर (result == OK) {
		log_unreadable = असत्य;
	}

	मुक्त(bson_encoder_buf_data(&encoder));
	परत result;
}

स्थिर तार्किक
has_record(flash_file_token_t token)
{
	uint8_t *buffer = nullptr;
	माप_प्रकार buf_size = 0;
	परत parameter_flashfs_read(token, &buffer, &buf_size) >= 0;
}

स्थिर पूर्णांक
param_export_internal(param_filter_func filter)
{
	/* A delta only means something behind a snapshot that replays. A legacy
	 * record still present means the boot migration did not run; replay
	 * would take it over any delta appended behind it. */
	जर (log_unreadable || has_record(parameters_legacy_token) || !has_record(parameters_token)) {
		परत compact_to_snapshot();
	}

	जर (!any_unsaved(filter)) {
		परत 0;
	}

	पूर्णांक result = export_delta(filter);

	जर (result != -ENOSPC) {
		परत result;
	}

	/* Same-bank erase stalls the CPU for ~1 s; last resort when a single
	 * save does not fit the tail. Compact writes the full RAM snapshot so a
	 * filtered save cannot drop params not in the delta. */
	PX4_WARN("flashparams: param sector full, compacting");
	परत compact_to_snapshot();
}


स्थिर तार्किक
record_is_stale(bson_गाठ_t गाठ_2)
{
	स्थिरांक param_t param = param_find_no_notification(गाठ_2->name);

	जर (param == PARAM_INVALID) {
		परत सत्य;
	}

	निवड (गाठ_2->type) {
	स्थिती BSON_INT32:
		परत param_type(param) != PARAM_TYPE_INT32;

	स्थिती BSON_DOUBLE:
		परत param_type(param) != PARAM_TYPE_FLOAT;

	पूर्वनिर्धारित:
		परत असत्य;
	}
}

स्थिर पूर्णांक
param_import_callback(bson_decoder_t decoder, bson_गाठ_t गाठ_2)
{
	दशांश f;
	int32_t i;
	शून्य *v = nullptr;
	पूर्णांक result = -1;

	/*
	 * EOO means the end of the parameter object. (Currently not supporting
	 * nested BSON objects).
	 */
	जर (गाठ_2->type == BSON_EOO) {
		debug("end of parameters");
		परत 0;
	}

	/* A tombstone has no value. The decoder leaves the previous node's in
	 * the union and the translations read it without a type check. */
	स्थिरांक तार्किक tombstone = गाठ_2->type == BSON_nullptr || गाठ_2->type == BSON_UNDEFINED;

	जर (!tombstone) {
		जर (record_is_stale(गाठ_2)) {
			log_stale = सत्य;
		}

		जर (param_modify_on_import(गाठ_2) == param_modify_on_import_ret::PARAM_SKIP_IMPORT) {
			परत 1;
		}
	}

	/*
	 * Find the parameter this node represents.  If we don't know it,
	 * ignore the node.
	 */
	param_t param = param_find_no_notification(गाठ_2->name);

	जर (param == PARAM_INVALID) {
		debug("ignoring unrecognised parameter '%s'", गाठ_2->name);
		परत 1;
	}

	/*
	 * Handle setting the parameter from the node
	 */

	निवड (गाठ_2->type) {
	स्थिती BSON_nullptr:
	स्थिती BSON_UNDEFINED:
		user_config.reset(param);
		result = 1;
		जा out;

	स्थिती BSON_INT32:
		जर (param_type(param) != PARAM_TYPE_INT32) {
			PX4_WARN("unexpected type for %s", गाठ_2->name);
			result = 1; // just skip this entry
			जा out;
		}

		i = गाठ_2->i32;
		v = &i;
		थांबा;

	स्थिती BSON_DOUBLE:
		जर (param_type(param) != PARAM_TYPE_FLOAT) {
			PX4_WARN("unexpected type for %s", गाठ_2->name);
			result = 1; // just skip this entry
			जा out;
		}

		f = गाठ_2->d;
		v = &f;
		थांबा;

	पूर्वनिर्धारित:
		PX4_ERR("%s unrecognised node type %d", गाठ_2->name, गाठ_2->type);
		result = 1; // just skip this entry
		जा out;
	}

	जर (param_set_external(param, v, सत्य, सत्य)) {
		debug("error setting value for '%s'", गाठ_2->name);
		जा out;
	}

	/* don't return zero, that means EOF */
	result = 1;

out:
	परत result;
}

स्थिर पूर्णांक
import_one_entry(uint8_t *buffer, माप_प्रकार buf_size, शून्य *arg)
{
	पूर्णांक *result = static_cast<पूर्णांक *>(arg);
	bson_decoder_s decoder{};

	जर (bson_decoder_init_buf(&decoder, buffer, buf_size, param_import_callback)) {
		debug("decoder init failed");
		*result = -1;
		परत -1;
	}

	करा {
		*result = bson_decoder_next(&decoder);

	} पर्यंत (*result > 0);

	परत *result < 0 ? *result : 0;
}

स्थिर पूर्णांक
param_import_internal(तार्किक *legacy)
{
	पूर्णांक result = 0;
	log_stale = असत्य;
	पूर्णांक n = parameter_flashfs_walk(parameters_legacy_token, import_one_entry, &result);
	*legacy = n > 0;

	जर (n == 0) {
		n = parameter_flashfs_walk(parameters_token, import_one_entry, &result);
	}

	जर (n < 0) {
		debug("flash walk failed (%d)", n);
		log_unreadable = सत्य;
		परत n;
	}

	जर (n == 0) {
		/* No valid entry found. A fully erased store is blank (first boot, or
		 * after switching firmware): report "not yet stored" (1), matching the
		 * convention used by param_load_default(). A store that holds data but
		 * no valid entry (torn write, bit-rot, or foreign data) is corrupt and
		 * must not be silently reseeded - report it so the boot recovery runs. */
		जर (parameter_flashfs_blank() == 1) {
			परत 1;
		}

		debug("flash holds data but no valid entry");
		परत -EILSEQ;
	}

	जर (result < 0) {
		debug("BSON error decoding parameters");
		परत result;
	}

	परत 0;
}

/*
 * Same-bank erase stalls instruction fetch for ~1 s on H7. Do it here at
 * boot, before sensors or DShot start, and only when a burst of deltas would
 * no longer fit, the store is still in the legacy layout, or the log holds
 * records this firmware translated or could not use. Only a failure after
 * the erase is an error: until then the log is intact and RAM holds what it
 * said, so the boot must not be reported as a corrupt store.
 */
स्थिर पूर्णांक
compact_after_import(तार्किक legacy)
{
	bson_encoder_s encoder{};
	पूर्णांक result = encode_document(&encoder, nullptr, सत्य);

	जर (result != 0) {
		PX4_ERR("flashparams: snapshot encode failed (%d), log kept", result);
		मुक्त(bson_encoder_buf_data(&encoder));
		परत 0;
	}

	स्थिरांक माप_प्रकार enc_size = bson_encoder_buf_size(&encoder);
	पूर्णांक need = (legacy || log_stale) ? 1 : parameter_flashfs_needs_compact(parameters_token, enc_size);
	result = 0;

	जर (need > 0) {
		uint8_t *buffer;
		माप_प्रकार staged_size;
		result = stage_snapshot(&encoder, &buffer, &staged_size);

		जर (result != OK) {
			PX4_ERR("flashparams: cannot stage snapshot (%d), log kept", result);
			result = 0;

		} अन्यथा {
			PX4_INFO("flashparams: compacting param sector%s", legacy ? " (legacy layout)" : "");
			result = commit_snapshot(buffer, staged_size);

			जर (result != OK) {
				PX4_ERR("flashparams: compact failed (%d)", result);
			}
		}
	}

	मुक्त(bson_encoder_buf_data(&encoder));
	परत result;
}

पूर्णांक flash_param_save(param_filter_func filter)
{
	परत param_export_internal(filter);
}

पूर्णांक flash_param_import()
{
	तार्किक legacy = असत्य;
	पूर्णांक result = param_import_internal(&legacy);

	जर (result != 0) {
		परत result;
	}

	परत compact_after_import(legacy);
}
