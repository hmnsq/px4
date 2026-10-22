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

#அந்தர்பாவ <px4_platform_common/defines.h>
#அந்தர்பாவ <px4_platform_common/log.h>
#அந்தர்பாவ <px4_platform_common/posix.h>
#அந்தர்பாவ <px4_platform_common/shutdown.h>
#அந்தர்பாவ <parameters/px4_parameters.hpp>

#அந்தர்பாவ <string.h>
#அந்தர்பாவ <stdbool.h>
#அந்தர்பாவ <stdint.h>
#அந்தர்பாவ <errno.h>

#அந்தர்பாவ <parameters/param.h>

#அந்தர்பாவ <lib/tinybson/tinybson.h>
#அந்தர்பாவ "flashparams.h"
#அந்தர்பாவ "flashfs.h"
#அந்தர்பாவ "../param_translation.h"

#எனில் 0
# define debug(fmt, args...)            செய் { warnx(fmt, ##args); } போது(0)
#இல்லையெனில்
# define debug(fmt, args...)            செய் { } போது(0)
#endif


/**
 * Storage for modified parameters.
 */
கட்டமைப்பு param_wbuf_s {
	ஒன்றிப்பு param_மதிப்பு_u     val;
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
நிலையான தருக்க log_stale;

/* Replay failed inside a CRC-valid record. A delta appended behind it would
 * never be reached, so the next save rewrites the store. */
நிலையான தருக்க log_unreadable;

நிலையான தருக்க
any_unsaved(param_filter_func filter)
{
	க்கு (param_t param = 0; param < user_config.PARAM_மொத்தம்; param++) {
		எனில் ((filter == nullptr || filter(param)) && param_மதிப்பு_unsaved(param)) {
			திரும்பு உண்மை;
		}
	}

	திரும்பு பொய்;
}

/*
 * A snapshot holds every override in RAM, equal to the current default or
 * not: the boot compaction runs before the airframe applies its set-default
 * values, so at that point "equals the default" says nothing. A delta holds
 * the unsaved params; one that no longer overrides, or now matches the
 * default in force, is written as a BSON null so replay drops it, which is
 * what the file backend's "don't export defaults" gives after a reload.
 */
நிலையான முழு
encode_params(bson_encoder_s *encoder, param_filter_func filter, தருக்க snapshot)
{
	க்கு (param_t param = 0; param < user_config.PARAM_மொத்தம்; param++) {
		எனில் (filter && !filter(param)) {
			தொடர்;
		}

		எனில் (snapshot) {
			எனில் (!user_config.contains(param)) {
				தொடர்;
			}

		} இல்லையெனில் {
			எனில் (!param_மதிப்பு_unsaved(param)) {
				தொடர்;
			}

			எனில் (!user_config.contains(param) || param_மதிப்பு_is_default(param)) {
				எனில் (bson_encoder_append_null(encoder, param_name(param))) {
					debug("BSON append failed for '%s'", param_name(param));
					திரும்பு -1;
				}

				தொடர்;
			}
		}

		தேர்வு (param_type(param)) {
		நிலை PARAM_TYPE_INT32:
			எனில் (bson_encoder_append_int32(encoder, param_name(param), user_config.get(param).i)) {
				debug("BSON append failed for '%s'", param_name(param));
				திரும்பு -1;
			}

			நிறுத்து;

		நிலை PARAM_TYPE_FLOAT:
			எனில் (bson_encoder_append_double(encoder, param_name(param), (இரட்டை)user_config.get(param).f)) {
				debug("BSON append failed for '%s'", param_name(param));
				திரும்பு -1;
			}

			நிறுத்து;

		இயல்பு:
			debug("unrecognized parameter type");
			திரும்பு -1;
		}
	}

	திரும்பு 0;
}

/* The caller frees bson_encoder_buf_data() whatever the result. */
நிலையான முழு
encode_document(bson_encoder_s *encoder, param_filter_func filter, தருக்க snapshot)
{
	bson_encoder_init_buf(encoder, nullptr, 0);

	முழு result = encode_params(encoder, filter, snapshot);

	எனில் (result == 0) {
		result = bson_encoder_fini(encoder);
	}

	திரும்பு result;
}

நிலையான முழு
stage_write_buffer(bson_encoder_s *encoder, uint8_t **buffer, size_t *enc_size)
{
	வெறுமை *enc_buff = bson_encoder_buf_data(encoder);
	*enc_size = bson_encoder_buf_size(encoder);

	எனில் (enc_buff == nullptr) {
		திரும்பு -ENOMEM;
	}

	size_t alloc_size = *enc_size;
	முழு result = parameter_flashfs_alloc(parameters_token, buffer, &alloc_size);

	எனில் (result != OK) {
		திரும்பு result;
	}

	எனில் (alloc_size < *enc_size) {
		parameter_flashfs_free();
		திரும்பு -ENOMEM;
	}

	memcpy(*buffer, enc_buff, *enc_size);
	திரும்பு OK;
}

நிலையான முழு
commit_append(uint8_t *buffer, size_t enc_size)
{
	முழு result = parameter_flashfs_write(parameters_token, buffer, enc_size);
	parameter_flashfs_free();

	எனில் (result == (முழு)enc_size) {
		திரும்பு OK;
	}

	திரும்பு result < 0 ? result : -EFBIG;
}

நிலையான முழு
export_delta(param_filter_func filter)
{
	bson_encoder_s encoder{};
	முழு result = encode_document(&encoder, filter, பொய்);

	எனில் (result == 0) {
		uint8_t *buffer;
		size_t enc_size;
		result = stage_write_buffer(&encoder, &buffer, &enc_size);

		எனில் (result == OK) {
			result = commit_append(buffer, enc_size);
		}
	}

	விடுவி(bson_encoder_buf_data(&encoder));
	திரும்பு result;
}

/* Everything that can fail before the erase, so a failure here leaves the
 * log untouched and still loadable. */
நிலையான முழு
stage_snapshot(bson_encoder_s *encoder, uint8_t **buffer, size_t *enc_size)
{
	முழு result = stage_write_buffer(encoder, buffer, enc_size);

	எனில் (result != OK) {
		திரும்பு result;
	}

	எனில் (*enc_size > parameter_flashfs_max_payload()) {
		parameter_flashfs_free();
		திரும்பு -ENOSPC;
	}

	திரும்பு OK;
}

நிலையான முழு
commit_snapshot(uint8_t *buffer, size_t enc_size)
{
	எனில் (parameter_flashfs_blank() != 1) {
		முழு rv = parameter_flashfs_erase();

		எனில் (rv < 0) {
			parameter_flashfs_free();
			திரும்பு rv;
		}
	}

	முழு result = parameter_flashfs_write(parameters_token, buffer, enc_size);

	எனில் (result != (முழு)enc_size && parameter_flashfs_blank() == 1) {
		result = parameter_flashfs_write(parameters_token, buffer, enc_size);
	}

	parameter_flashfs_free();

	எனில் (result == (முழு)enc_size) {
		திரும்பு OK;
	}

	திரும்பு result < 0 ? result : -EFBIG;
}

நிலையான முழு
compact_to_snapshot()
{
	bson_encoder_s encoder{};
	முழு result = encode_document(&encoder, nullptr, உண்மை);

	எனில் (result == 0) {
		uint8_t *buffer;
		size_t enc_size;
		result = stage_snapshot(&encoder, &buffer, &enc_size);

		எனில் (result == OK) {
			result = commit_snapshot(buffer, enc_size);
		}
	}

	எனில் (result == OK) {
		log_unreadable = பொய்;
	}

	விடுவி(bson_encoder_buf_data(&encoder));
	திரும்பு result;
}

நிலையான தருக்க
has_record(flash_file_token_t token)
{
	uint8_t *buffer = nullptr;
	size_t buf_size = 0;
	திரும்பு parameter_flashfs_read(token, &buffer, &buf_size) >= 0;
}

நிலையான முழு
param_export_internal(param_filter_func filter)
{
	/* A delta only means something behind a snapshot that replays. A legacy
	 * record still present means the boot migration did not run; replay
	 * would take it over any delta appended behind it. */
	எனில் (log_unreadable || has_record(parameters_legacy_token) || !has_record(parameters_token)) {
		திரும்பு compact_to_snapshot();
	}

	எனில் (!any_unsaved(filter)) {
		திரும்பு 0;
	}

	முழு result = export_delta(filter);

	எனில் (result != -ENOSPC) {
		திரும்பு result;
	}

	/* Same-bank erase stalls the CPU for ~1 s; last resort when a single
	 * save does not fit the tail. Compact writes the full RAM snapshot so a
	 * filtered save cannot drop params not in the delta. */
	PX4_WARN("flashparams: param sector full, compacting");
	திரும்பு compact_to_snapshot();
}


நிலையான தருக்க
record_is_stale(bson_முனை_t முனை_2)
{
	மாறிலி param_t param = param_find_no_notification(முனை_2->name);

	எனில் (param == PARAM_INVALID) {
		திரும்பு உண்மை;
	}

	தேர்வு (முனை_2->type) {
	நிலை BSON_INT32:
		திரும்பு param_type(param) != PARAM_TYPE_INT32;

	நிலை BSON_DOUBLE:
		திரும்பு param_type(param) != PARAM_TYPE_FLOAT;

	இயல்பு:
		திரும்பு பொய்;
	}
}

நிலையான முழு
param_import_callback(bson_decoder_t decoder, bson_முனை_t முனை_2)
{
	தசம f;
	int32_t i;
	வெறுமை *v = nullptr;
	முழு result = -1;

	/*
	 * EOO means the end of the parameter object. (Currently not supporting
	 * nested BSON objects).
	 */
	எனில் (முனை_2->type == BSON_EOO) {
		debug("end of parameters");
		திரும்பு 0;
	}

	/* A tombstone has no value. The decoder leaves the previous node's in
	 * the union and the translations read it without a type check. */
	மாறிலி தருக்க tombstone = முனை_2->type == BSON_nullptr || முனை_2->type == BSON_UNDEFINED;

	எனில் (!tombstone) {
		எனில் (record_is_stale(முனை_2)) {
			log_stale = உண்மை;
		}

		எனில் (param_modify_on_import(முனை_2) == param_modify_on_import_ret::PARAM_SKIP_IMPORT) {
			திரும்பு 1;
		}
	}

	/*
	 * Find the parameter this node represents.  If we don't know it,
	 * ignore the node.
	 */
	param_t param = param_find_no_notification(முனை_2->name);

	எனில் (param == PARAM_INVALID) {
		debug("ignoring unrecognised parameter '%s'", முனை_2->name);
		திரும்பு 1;
	}

	/*
	 * Handle setting the parameter from the node
	 */

	தேர்வு (முனை_2->type) {
	நிலை BSON_nullptr:
	நிலை BSON_UNDEFINED:
		user_config.reset(param);
		result = 1;
		செல் out;

	நிலை BSON_INT32:
		எனில் (param_type(param) != PARAM_TYPE_INT32) {
			PX4_WARN("unexpected type for %s", முனை_2->name);
			result = 1; // just skip this entry
			செல் out;
		}

		i = முனை_2->i32;
		v = &i;
		நிறுத்து;

	நிலை BSON_DOUBLE:
		எனில் (param_type(param) != PARAM_TYPE_FLOAT) {
			PX4_WARN("unexpected type for %s", முனை_2->name);
			result = 1; // just skip this entry
			செல் out;
		}

		f = முனை_2->d;
		v = &f;
		நிறுத்து;

	இயல்பு:
		PX4_ERR("%s unrecognised node type %d", முனை_2->name, முனை_2->type);
		result = 1; // just skip this entry
		செல் out;
	}

	எனில் (param_set_external(param, v, உண்மை, உண்மை)) {
		debug("error setting value for '%s'", முனை_2->name);
		செல் out;
	}

	/* don't return zero, that means EOF */
	result = 1;

out:
	திரும்பு result;
}

நிலையான முழு
import_one_entry(uint8_t *buffer, size_t buf_size, வெறுமை *arg)
{
	முழு *result = static_cast<முழு *>(arg);
	bson_decoder_s decoder{};

	எனில் (bson_decoder_init_buf(&decoder, buffer, buf_size, param_import_callback)) {
		debug("decoder init failed");
		*result = -1;
		திரும்பு -1;
	}

	செய் {
		*result = bson_decoder_next(&decoder);

	} போது (*result > 0);

	திரும்பு *result < 0 ? *result : 0;
}

நிலையான முழு
param_import_internal(தருக்க *legacy)
{
	முழு result = 0;
	log_stale = பொய்;
	முழு n = parameter_flashfs_walk(parameters_legacy_token, import_one_entry, &result);
	*legacy = n > 0;

	எனில் (n == 0) {
		n = parameter_flashfs_walk(parameters_token, import_one_entry, &result);
	}

	எனில் (n < 0) {
		debug("flash walk failed (%d)", n);
		log_unreadable = உண்மை;
		திரும்பு n;
	}

	எனில் (n == 0) {
		/* No valid entry found. A fully erased store is blank (first boot, or
		 * after switching firmware): report "not yet stored" (1), matching the
		 * convention used by param_load_default(). A store that holds data but
		 * no valid entry (torn write, bit-rot, or foreign data) is corrupt and
		 * must not be silently reseeded - report it so the boot recovery runs. */
		எனில் (parameter_flashfs_blank() == 1) {
			திரும்பு 1;
		}

		debug("flash holds data but no valid entry");
		திரும்பு -EILSEQ;
	}

	எனில் (result < 0) {
		debug("BSON error decoding parameters");
		திரும்பு result;
	}

	திரும்பு 0;
}

/*
 * Same-bank erase stalls instruction fetch for ~1 s on H7. Do it here at
 * boot, before sensors or DShot start, and only when a burst of deltas would
 * no longer fit, the store is still in the legacy layout, or the log holds
 * records this firmware translated or could not use. Only a failure after
 * the erase is an error: until then the log is intact and RAM holds what it
 * said, so the boot must not be reported as a corrupt store.
 */
நிலையான முழு
compact_after_import(தருக்க legacy)
{
	bson_encoder_s encoder{};
	முழு result = encode_document(&encoder, nullptr, உண்மை);

	எனில் (result != 0) {
		PX4_ERR("flashparams: snapshot encode failed (%d), log kept", result);
		விடுவி(bson_encoder_buf_data(&encoder));
		திரும்பு 0;
	}

	மாறிலி size_t enc_size = bson_encoder_buf_size(&encoder);
	முழு need = (legacy || log_stale) ? 1 : parameter_flashfs_needs_compact(parameters_token, enc_size);
	result = 0;

	எனில் (need > 0) {
		uint8_t *buffer;
		size_t staged_size;
		result = stage_snapshot(&encoder, &buffer, &staged_size);

		எனில் (result != OK) {
			PX4_ERR("flashparams: cannot stage snapshot (%d), log kept", result);
			result = 0;

		} இல்லையெனில் {
			PX4_INFO("flashparams: compacting param sector%s", legacy ? " (legacy layout)" : "");
			result = commit_snapshot(buffer, staged_size);

			எனில் (result != OK) {
				PX4_ERR("flashparams: compact failed (%d)", result);
			}
		}
	}

	விடுவி(bson_encoder_buf_data(&encoder));
	திரும்பு result;
}

முழு flash_param_save(param_filter_func filter)
{
	திரும்பு param_export_internal(filter);
}

முழு flash_param_import()
{
	தருக்க legacy = பொய்;
	முழு result = param_import_internal(&legacy);

	எனில் (result != 0) {
		திரும்பு result;
	}

	திரும்பு compact_after_import(legacy);
}
