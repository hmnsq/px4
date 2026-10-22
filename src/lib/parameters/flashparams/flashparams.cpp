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

#അന്തര്ഭാവ <px4_platform_common/defines.h>
#അന്തര്ഭാവ <px4_platform_common/log.h>
#അന്തര്ഭാവ <px4_platform_common/posix.h>
#അന്തര്ഭാവ <px4_platform_common/shutdown.h>
#അന്തര്ഭാവ <parameters/px4_parameters.hpp>

#അന്തര്ഭാവ <string.h>
#അന്തര്ഭാവ <stdbool.h>
#അന്തര്ഭാവ <stdint.h>
#അന്തര്ഭാവ <errno.h>

#അന്തര്ഭാവ <parameters/param.h>

#അന്തര്ഭാവ <lib/tinybson/tinybson.h>
#അന്തര്ഭാവ "flashparams.h"
#അന്തര്ഭാവ "flashfs.h"
#അന്തര്ഭാവ "../param_translation.h"

#എങ്കിൽ 0
# define debug(fmt, args...)            ചെയ്യുക { warnx(fmt, ##args); } ആകുമ്പോൾ(0)
#അല്ലെങ്കിൽ
# define debug(fmt, args...)            ചെയ്യുക { } ആകുമ്പോൾ(0)
#endif


/**
 * Storage for modified parameters.
 */
ഘടന param_wbuf_s {
	സംഘം param_മൂല്യം_u     val;
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
സ്ഥിതം തർക്കം log_stale;

/* Replay failed inside a CRC-valid record. A delta appended behind it would
 * never be reached, so the next save rewrites the store. */
സ്ഥിതം തർക്കം log_unreadable;

സ്ഥിതം തർക്കം
any_unsaved(param_filter_func filter)
{
	വേണ്ടി (param_t param = 0; param < user_config.PARAM_എണ്ണം; param++) {
		എങ്കിൽ ((filter == nullptr || filter(param)) && param_മൂല്യം_unsaved(param)) {
			തിരികെ സത്യം;
		}
	}

	തിരികെ അസത്യം;
}

/*
 * A snapshot holds every override in RAM, equal to the current default or
 * not: the boot compaction runs before the airframe applies its set-default
 * values, so at that point "equals the default" says nothing. A delta holds
 * the unsaved params; one that no longer overrides, or now matches the
 * default in force, is written as a BSON null so replay drops it, which is
 * what the file backend's "don't export defaults" gives after a reload.
 */
സ്ഥിതം പൂർണ്ണസംഖ്യ
encode_params(bson_encoder_s *encoder, param_filter_func filter, തർക്കം snapshot)
{
	വേണ്ടി (param_t param = 0; param < user_config.PARAM_എണ്ണം; param++) {
		എങ്കിൽ (filter && !filter(param)) {
			തുടരുക;
		}

		എങ്കിൽ (snapshot) {
			എങ്കിൽ (!user_config.contains(param)) {
				തുടരുക;
			}

		} അല്ലെങ്കിൽ {
			എങ്കിൽ (!param_മൂല്യം_unsaved(param)) {
				തുടരുക;
			}

			എങ്കിൽ (!user_config.contains(param) || param_മൂല്യം_is_default(param)) {
				എങ്കിൽ (bson_encoder_append_null(encoder, param_name(param))) {
					debug("BSON append failed for '%s'", param_name(param));
					തിരികെ -1;
				}

				തുടരുക;
			}
		}

		തിരഞ്ഞെടുപ്പ് (param_type(param)) {
		അവസ്ഥ PARAM_TYPE_INT32:
			എങ്കിൽ (bson_encoder_append_int32(encoder, param_name(param), user_config.get(param).i)) {
				debug("BSON append failed for '%s'", param_name(param));
				തിരികെ -1;
			}

			നിർത്തുക;

		അവസ്ഥ PARAM_TYPE_FLOAT:
			എങ്കിൽ (bson_encoder_append_double(encoder, param_name(param), (ഇരട്ട)user_config.get(param).f)) {
				debug("BSON append failed for '%s'", param_name(param));
				തിരികെ -1;
			}

			നിർത്തുക;

		സ്ഥിരസ്ഥിതി:
			debug("unrecognized parameter type");
			തിരികെ -1;
		}
	}

	തിരികെ 0;
}

/* The caller frees bson_encoder_buf_data() whatever the result. */
സ്ഥിതം പൂർണ്ണസംഖ്യ
encode_document(bson_encoder_s *encoder, param_filter_func filter, തർക്കം snapshot)
{
	bson_encoder_init_buf(encoder, nullptr, 0);

	പൂർണ്ണസംഖ്യ result = encode_params(encoder, filter, snapshot);

	എങ്കിൽ (result == 0) {
		result = bson_encoder_fini(encoder);
	}

	തിരികെ result;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ
stage_write_buffer(bson_encoder_s *encoder, uint8_t **buffer, size_t *enc_size)
{
	ശൂന്യം *enc_buff = bson_encoder_buf_data(encoder);
	*enc_size = bson_encoder_buf_size(encoder);

	എങ്കിൽ (enc_buff == nullptr) {
		തിരികെ -ENOMEM;
	}

	size_t alloc_size = *enc_size;
	പൂർണ്ണസംഖ്യ result = parameter_flashfs_alloc(parameters_token, buffer, &alloc_size);

	എങ്കിൽ (result != OK) {
		തിരികെ result;
	}

	എങ്കിൽ (alloc_size < *enc_size) {
		parameter_flashfs_free();
		തിരികെ -ENOMEM;
	}

	memcpy(*buffer, enc_buff, *enc_size);
	തിരികെ OK;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ
commit_append(uint8_t *buffer, size_t enc_size)
{
	പൂർണ്ണസംഖ്യ result = parameter_flashfs_write(parameters_token, buffer, enc_size);
	parameter_flashfs_free();

	എങ്കിൽ (result == (പൂർണ്ണസംഖ്യ)enc_size) {
		തിരികെ OK;
	}

	തിരികെ result < 0 ? result : -EFBIG;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ
export_delta(param_filter_func filter)
{
	bson_encoder_s encoder{};
	പൂർണ്ണസംഖ്യ result = encode_document(&encoder, filter, അസത്യം);

	എങ്കിൽ (result == 0) {
		uint8_t *buffer;
		size_t enc_size;
		result = stage_write_buffer(&encoder, &buffer, &enc_size);

		എങ്കിൽ (result == OK) {
			result = commit_append(buffer, enc_size);
		}
	}

	സ്വതന്ത്രം(bson_encoder_buf_data(&encoder));
	തിരികെ result;
}

/* Everything that can fail before the erase, so a failure here leaves the
 * log untouched and still loadable. */
സ്ഥിതം പൂർണ്ണസംഖ്യ
stage_snapshot(bson_encoder_s *encoder, uint8_t **buffer, size_t *enc_size)
{
	പൂർണ്ണസംഖ്യ result = stage_write_buffer(encoder, buffer, enc_size);

	എങ്കിൽ (result != OK) {
		തിരികെ result;
	}

	എങ്കിൽ (*enc_size > parameter_flashfs_max_payload()) {
		parameter_flashfs_free();
		തിരികെ -ENOSPC;
	}

	തിരികെ OK;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ
commit_snapshot(uint8_t *buffer, size_t enc_size)
{
	എങ്കിൽ (parameter_flashfs_blank() != 1) {
		പൂർണ്ണസംഖ്യ rv = parameter_flashfs_erase();

		എങ്കിൽ (rv < 0) {
			parameter_flashfs_free();
			തിരികെ rv;
		}
	}

	പൂർണ്ണസംഖ്യ result = parameter_flashfs_write(parameters_token, buffer, enc_size);

	എങ്കിൽ (result != (പൂർണ്ണസംഖ്യ)enc_size && parameter_flashfs_blank() == 1) {
		result = parameter_flashfs_write(parameters_token, buffer, enc_size);
	}

	parameter_flashfs_free();

	എങ്കിൽ (result == (പൂർണ്ണസംഖ്യ)enc_size) {
		തിരികെ OK;
	}

	തിരികെ result < 0 ? result : -EFBIG;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ
compact_to_snapshot()
{
	bson_encoder_s encoder{};
	പൂർണ്ണസംഖ്യ result = encode_document(&encoder, nullptr, സത്യം);

	എങ്കിൽ (result == 0) {
		uint8_t *buffer;
		size_t enc_size;
		result = stage_snapshot(&encoder, &buffer, &enc_size);

		എങ്കിൽ (result == OK) {
			result = commit_snapshot(buffer, enc_size);
		}
	}

	എങ്കിൽ (result == OK) {
		log_unreadable = അസത്യം;
	}

	സ്വതന്ത്രം(bson_encoder_buf_data(&encoder));
	തിരികെ result;
}

സ്ഥിതം തർക്കം
has_record(flash_file_token_t token)
{
	uint8_t *buffer = nullptr;
	size_t buf_size = 0;
	തിരികെ parameter_flashfs_read(token, &buffer, &buf_size) >= 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ
param_export_internal(param_filter_func filter)
{
	/* A delta only means something behind a snapshot that replays. A legacy
	 * record still present means the boot migration did not run; replay
	 * would take it over any delta appended behind it. */
	എങ്കിൽ (log_unreadable || has_record(parameters_legacy_token) || !has_record(parameters_token)) {
		തിരികെ compact_to_snapshot();
	}

	എങ്കിൽ (!any_unsaved(filter)) {
		തിരികെ 0;
	}

	പൂർണ്ണസംഖ്യ result = export_delta(filter);

	എങ്കിൽ (result != -ENOSPC) {
		തിരികെ result;
	}

	/* Same-bank erase stalls the CPU for ~1 s; last resort when a single
	 * save does not fit the tail. Compact writes the full RAM snapshot so a
	 * filtered save cannot drop params not in the delta. */
	PX4_WARN("flashparams: param sector full, compacting");
	തിരികെ compact_to_snapshot();
}


സ്ഥിതം തർക്കം
record_is_stale(bson_കണ്ണി_t കണ്ണി_2)
{
	സ്ഥിരം param_t param = param_find_no_notification(കണ്ണി_2->name);

	എങ്കിൽ (param == PARAM_INVALID) {
		തിരികെ സത്യം;
	}

	തിരഞ്ഞെടുപ്പ് (കണ്ണി_2->type) {
	അവസ്ഥ BSON_INT32:
		തിരികെ param_type(param) != PARAM_TYPE_INT32;

	അവസ്ഥ BSON_DOUBLE:
		തിരികെ param_type(param) != PARAM_TYPE_FLOAT;

	സ്ഥിരസ്ഥിതി:
		തിരികെ അസത്യം;
	}
}

സ്ഥിതം പൂർണ്ണസംഖ്യ
param_import_callback(bson_decoder_t decoder, bson_കണ്ണി_t കണ്ണി_2)
{
	ദശാംശം f;
	int32_t i;
	ശൂന്യം *v = nullptr;
	പൂർണ്ണസംഖ്യ result = -1;

	/*
	 * EOO means the end of the parameter object. (Currently not supporting
	 * nested BSON objects).
	 */
	എങ്കിൽ (കണ്ണി_2->type == BSON_EOO) {
		debug("end of parameters");
		തിരികെ 0;
	}

	/* A tombstone has no value. The decoder leaves the previous node's in
	 * the union and the translations read it without a type check. */
	സ്ഥിരം തർക്കം tombstone = കണ്ണി_2->type == BSON_nullptr || കണ്ണി_2->type == BSON_UNDEFINED;

	എങ്കിൽ (!tombstone) {
		എങ്കിൽ (record_is_stale(കണ്ണി_2)) {
			log_stale = സത്യം;
		}

		എങ്കിൽ (param_modify_on_import(കണ്ണി_2) == param_modify_on_import_ret::PARAM_SKIP_IMPORT) {
			തിരികെ 1;
		}
	}

	/*
	 * Find the parameter this node represents.  If we don't know it,
	 * ignore the node.
	 */
	param_t param = param_find_no_notification(കണ്ണി_2->name);

	എങ്കിൽ (param == PARAM_INVALID) {
		debug("ignoring unrecognised parameter '%s'", കണ്ണി_2->name);
		തിരികെ 1;
	}

	/*
	 * Handle setting the parameter from the node
	 */

	തിരഞ്ഞെടുപ്പ് (കണ്ണി_2->type) {
	അവസ്ഥ BSON_nullptr:
	അവസ്ഥ BSON_UNDEFINED:
		user_config.reset(param);
		result = 1;
		പോകുക out;

	അവസ്ഥ BSON_INT32:
		എങ്കിൽ (param_type(param) != PARAM_TYPE_INT32) {
			PX4_WARN("unexpected type for %s", കണ്ണി_2->name);
			result = 1; // just skip this entry
			പോകുക out;
		}

		i = കണ്ണി_2->i32;
		v = &i;
		നിർത്തുക;

	അവസ്ഥ BSON_DOUBLE:
		എങ്കിൽ (param_type(param) != PARAM_TYPE_FLOAT) {
			PX4_WARN("unexpected type for %s", കണ്ണി_2->name);
			result = 1; // just skip this entry
			പോകുക out;
		}

		f = കണ്ണി_2->d;
		v = &f;
		നിർത്തുക;

	സ്ഥിരസ്ഥിതി:
		PX4_ERR("%s unrecognised node type %d", കണ്ണി_2->name, കണ്ണി_2->type);
		result = 1; // just skip this entry
		പോകുക out;
	}

	എങ്കിൽ (param_set_external(param, v, സത്യം, സത്യം)) {
		debug("error setting value for '%s'", കണ്ണി_2->name);
		പോകുക out;
	}

	/* don't return zero, that means EOF */
	result = 1;

out:
	തിരികെ result;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ
import_one_entry(uint8_t *buffer, size_t buf_size, ശൂന്യം *arg)
{
	പൂർണ്ണസംഖ്യ *result = static_cast<പൂർണ്ണസംഖ്യ *>(arg);
	bson_decoder_s decoder{};

	എങ്കിൽ (bson_decoder_init_buf(&decoder, buffer, buf_size, param_import_callback)) {
		debug("decoder init failed");
		*result = -1;
		തിരികെ -1;
	}

	ചെയ്യുക {
		*result = bson_decoder_next(&decoder);

	} ആകുമ്പോൾ (*result > 0);

	തിരികെ *result < 0 ? *result : 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ
param_import_internal(തർക്കം *legacy)
{
	പൂർണ്ണസംഖ്യ result = 0;
	log_stale = അസത്യം;
	പൂർണ്ണസംഖ്യ n = parameter_flashfs_walk(parameters_legacy_token, import_one_entry, &result);
	*legacy = n > 0;

	എങ്കിൽ (n == 0) {
		n = parameter_flashfs_walk(parameters_token, import_one_entry, &result);
	}

	എങ്കിൽ (n < 0) {
		debug("flash walk failed (%d)", n);
		log_unreadable = സത്യം;
		തിരികെ n;
	}

	എങ്കിൽ (n == 0) {
		/* No valid entry found. A fully erased store is blank (first boot, or
		 * after switching firmware): report "not yet stored" (1), matching the
		 * convention used by param_load_default(). A store that holds data but
		 * no valid entry (torn write, bit-rot, or foreign data) is corrupt and
		 * must not be silently reseeded - report it so the boot recovery runs. */
		എങ്കിൽ (parameter_flashfs_blank() == 1) {
			തിരികെ 1;
		}

		debug("flash holds data but no valid entry");
		തിരികെ -EILSEQ;
	}

	എങ്കിൽ (result < 0) {
		debug("BSON error decoding parameters");
		തിരികെ result;
	}

	തിരികെ 0;
}

/*
 * Same-bank erase stalls instruction fetch for ~1 s on H7. Do it here at
 * boot, before sensors or DShot start, and only when a burst of deltas would
 * no longer fit, the store is still in the legacy layout, or the log holds
 * records this firmware translated or could not use. Only a failure after
 * the erase is an error: until then the log is intact and RAM holds what it
 * said, so the boot must not be reported as a corrupt store.
 */
സ്ഥിതം പൂർണ്ണസംഖ്യ
compact_after_import(തർക്കം legacy)
{
	bson_encoder_s encoder{};
	പൂർണ്ണസംഖ്യ result = encode_document(&encoder, nullptr, സത്യം);

	എങ്കിൽ (result != 0) {
		PX4_ERR("flashparams: snapshot encode failed (%d), log kept", result);
		സ്വതന്ത്രം(bson_encoder_buf_data(&encoder));
		തിരികെ 0;
	}

	സ്ഥിരം size_t enc_size = bson_encoder_buf_size(&encoder);
	പൂർണ്ണസംഖ്യ need = (legacy || log_stale) ? 1 : parameter_flashfs_needs_compact(parameters_token, enc_size);
	result = 0;

	എങ്കിൽ (need > 0) {
		uint8_t *buffer;
		size_t staged_size;
		result = stage_snapshot(&encoder, &buffer, &staged_size);

		എങ്കിൽ (result != OK) {
			PX4_ERR("flashparams: cannot stage snapshot (%d), log kept", result);
			result = 0;

		} അല്ലെങ്കിൽ {
			PX4_INFO("flashparams: compacting param sector%s", legacy ? " (legacy layout)" : "");
			result = commit_snapshot(buffer, staged_size);

			എങ്കിൽ (result != OK) {
				PX4_ERR("flashparams: compact failed (%d)", result);
			}
		}
	}

	സ്വതന്ത്രം(bson_encoder_buf_data(&encoder));
	തിരികെ result;
}

പൂർണ്ണസംഖ്യ flash_param_save(param_filter_func filter)
{
	തിരികെ param_export_internal(filter);
}

പൂർണ്ണസംഖ്യ flash_param_import()
{
	തർക്കം legacy = അസത്യം;
	പൂർണ്ണസംഖ്യ result = param_import_internal(&legacy);

	എങ്കിൽ (result != 0) {
		തിരികെ result;
	}

	തിരികെ compact_after_import(legacy);
}
