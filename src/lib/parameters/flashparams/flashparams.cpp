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

#إذا 0
# define debug(fmt, args...)            افعل { warnx(fmt, ##args); } طالما(0)
#وإلا
# define debug(fmt, args...)            افعل { } طالما(0)
#endif


/**
 * Storage for modified parameters.
 */
بنية param_wbuf_s {
	اتحاد param_بەها_u     val;
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
	لكل (param_t param = 0; param < user_config.PARAM_ژماردن; param++) {
		إذا ((filter == nullptr || filter(param)) && param_بەها_unsaved(param)) {
			أرجع true;
		}
	}

	أرجع خطأ;
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
	لكل (param_t param = 0; param < user_config.PARAM_ژماردن; param++) {
		إذا (filter && !filter(param)) {
			استمر;
		}

		إذا (snapshot) {
			إذا (!user_config.contains(param)) {
				استمر;
			}

		} وإلا {
			إذا (!param_بەها_unsaved(param)) {
				استمر;
			}

			إذا (!user_config.contains(param) || param_بەها_is_default(param)) {
				إذا (bson_encoder_append_null(encoder, param_name(param))) {
					debug("BSON append failed for '%s'", param_name(param));
					أرجع -1;
				}

				استمر;
			}
		}

		بدل (param_type(param)) {
		حالة PARAM_TYPE_INT32:
			إذا (bson_encoder_append_int32(encoder, param_name(param), user_config.get(param).i)) {
				debug("BSON append failed for '%s'", param_name(param));
				أرجع -1;
			}

			اكسر;

		حالة PARAM_TYPE_FLOAT:
			إذا (bson_encoder_append_double(encoder, param_name(param), (مزدوج)user_config.get(param).f)) {
				debug("BSON append failed for '%s'", param_name(param));
				أرجع -1;
			}

			اكسر;

		افتراضي:
			debug("unrecognized parameter type");
			أرجع -1;
		}
	}

	أرجع 0;
}

/* The caller frees bson_encoder_buf_data() whatever the result. */
ساكن صحيح
encode_document(bson_encoder_s *encoder, param_filter_func filter, منطقي snapshot)
{
	bson_encoder_init_buf(encoder, nullptr, 0);

	صحيح result = encode_params(encoder, filter, snapshot);

	إذا (result == 0) {
		result = bson_encoder_fini(encoder);
	}

	أرجع result;
}

ساكن صحيح
stage_write_buffer(bson_encoder_s *encoder, uint8_t **buffer, size_t *enc_size)
{
	فراغ *enc_buff = bson_encoder_buf_data(encoder);
	*enc_size = bson_encoder_buf_size(encoder);

	إذا (enc_buff == nullptr) {
		أرجع -ENOMEM;
	}

	size_t alloc_size = *enc_size;
	صحيح result = parameter_flashfs_alloc(parameters_token, buffer, &alloc_size);

	إذا (result != OK) {
		أرجع result;
	}

	إذا (alloc_size < *enc_size) {
		parameter_flashfs_free();
		أرجع -ENOMEM;
	}

	memcpy(*buffer, enc_buff, *enc_size);
	أرجع OK;
}

ساكن صحيح
commit_append(uint8_t *buffer, size_t enc_size)
{
	صحيح result = parameter_flashfs_write(parameters_token, buffer, enc_size);
	parameter_flashfs_free();

	إذا (result == (صحيح)enc_size) {
		أرجع OK;
	}

	أرجع result < 0 ? result : -EFBIG;
}

ساكن صحيح
export_delta(param_filter_func filter)
{
	bson_encoder_s encoder{};
	صحيح result = encode_document(&encoder, filter, خطأ);

	إذا (result == 0) {
		uint8_t *buffer;
		size_t enc_size;
		result = stage_write_buffer(&encoder, &buffer, &enc_size);

		إذا (result == OK) {
			result = commit_append(buffer, enc_size);
		}
	}

	حرر(bson_encoder_buf_data(&encoder));
	أرجع result;
}

/* Everything that can fail before the erase, so a failure here leaves the
 * log untouched and still loadable. */
ساكن صحيح
stage_snapshot(bson_encoder_s *encoder, uint8_t **buffer, size_t *enc_size)
{
	صحيح result = stage_write_buffer(encoder, buffer, enc_size);

	إذا (result != OK) {
		أرجع result;
	}

	إذا (*enc_size > parameter_flashfs_max_payload()) {
		parameter_flashfs_free();
		أرجع -ENOSPC;
	}

	أرجع OK;
}

ساكن صحيح
commit_snapshot(uint8_t *buffer, size_t enc_size)
{
	إذا (parameter_flashfs_blank() != 1) {
		صحيح rv = parameter_flashfs_erase();

		إذا (rv < 0) {
			parameter_flashfs_free();
			أرجع rv;
		}
	}

	صحيح result = parameter_flashfs_write(parameters_token, buffer, enc_size);

	إذا (result != (صحيح)enc_size && parameter_flashfs_blank() == 1) {
		result = parameter_flashfs_write(parameters_token, buffer, enc_size);
	}

	parameter_flashfs_free();

	إذا (result == (صحيح)enc_size) {
		أرجع OK;
	}

	أرجع result < 0 ? result : -EFBIG;
}

ساكن صحيح
compact_to_snapshot()
{
	bson_encoder_s encoder{};
	صحيح result = encode_document(&encoder, nullptr, true);

	إذا (result == 0) {
		uint8_t *buffer;
		size_t enc_size;
		result = stage_snapshot(&encoder, &buffer, &enc_size);

		إذا (result == OK) {
			result = commit_snapshot(buffer, enc_size);
		}
	}

	إذا (result == OK) {
		log_unreadable = خطأ;
	}

	حرر(bson_encoder_buf_data(&encoder));
	أرجع result;
}

ساكن منطقي
has_record(flash_file_token_t token)
{
	uint8_t *buffer = nullptr;
	size_t buf_size = 0;
	أرجع parameter_flashfs_read(token, &buffer, &buf_size) >= 0;
}

ساكن صحيح
param_export_internal(param_filter_func filter)
{
	/* A delta only means something behind a snapshot that replays. A legacy
	 * record still present means the boot migration did not run; replay
	 * would take it over any delta appended behind it. */
	إذا (log_unreadable || has_record(parameters_legacy_token) || !has_record(parameters_token)) {
		أرجع compact_to_snapshot();
	}

	إذا (!any_unsaved(filter)) {
		أرجع 0;
	}

	صحيح result = export_delta(filter);

	إذا (result != -ENOSPC) {
		أرجع result;
	}

	/* Same-bank erase stalls the CPU for ~1 s; last resort when a single
	 * save does not fit the tail. Compact writes the full RAM snapshot so a
	 * filtered save cannot drop params not in the delta. */
	PX4_WARN("flashparams: param sector full, compacting");
	أرجع compact_to_snapshot();
}


ساكن منطقي
record_is_stale(bson_گرێ_t گرێ_2)
{
	ثابت param_t param = param_find_no_notification(گرێ_2->name);

	إذا (param == PARAM_INVALID) {
		أرجع true;
	}

	بدل (گرێ_2->type) {
	حالة BSON_INT32:
		أرجع param_type(param) != PARAM_TYPE_INT32;

	حالة BSON_DOUBLE:
		أرجع param_type(param) != PARAM_TYPE_FLOAT;

	افتراضي:
		أرجع خطأ;
	}
}

ساكن صحيح
param_import_callback(bson_decoder_t decoder, bson_گرێ_t گرێ_2)
{
	عائم f;
	int32_t i;
	فراغ *v = nullptr;
	صحيح result = -1;

	/*
	 * EOO means the end of the parameter object. (Currently not supporting
	 * nested BSON objects).
	 */
	إذا (گرێ_2->type == BSON_EOO) {
		debug("end of parameters");
		أرجع 0;
	}

	/* A tombstone has no value. The decoder leaves the previous node's in
	 * the union and the translations read it without a type check. */
	ثابت منطقي tombstone = گرێ_2->type == BSON_nullptr || گرێ_2->type == BSON_UNDEFINED;

	إذا (!tombstone) {
		إذا (record_is_stale(گرێ_2)) {
			log_stale = true;
		}

		إذا (param_modify_on_import(گرێ_2) == param_modify_on_import_ret::PARAM_SKIP_IMPORT) {
			أرجع 1;
		}
	}

	/*
	 * Find the parameter this node represents.  If we don't know it,
	 * ignore the node.
	 */
	param_t param = param_find_no_notification(گرێ_2->name);

	إذا (param == PARAM_INVALID) {
		debug("ignoring unrecognised parameter '%s'", گرێ_2->name);
		أرجع 1;
	}

	/*
	 * Handle setting the parameter from the node
	 */

	بدل (گرێ_2->type) {
	حالة BSON_nullptr:
	حالة BSON_UNDEFINED:
		user_config.reset(param);
		result = 1;
		اذهب out;

	حالة BSON_INT32:
		إذا (param_type(param) != PARAM_TYPE_INT32) {
			PX4_WARN("unexpected type for %s", گرێ_2->name);
			result = 1; // just skip this entry
			اذهب out;
		}

		i = گرێ_2->i32;
		v = &i;
		اكسر;

	حالة BSON_DOUBLE:
		إذا (param_type(param) != PARAM_TYPE_FLOAT) {
			PX4_WARN("unexpected type for %s", گرێ_2->name);
			result = 1; // just skip this entry
			اذهب out;
		}

		f = گرێ_2->d;
		v = &f;
		اكسر;

	افتراضي:
		PX4_ERR("%s unrecognised node type %d", گرێ_2->name, گرێ_2->type);
		result = 1; // just skip this entry
		اذهب out;
	}

	إذا (param_set_external(param, v, true, true)) {
		debug("error setting value for '%s'", گرێ_2->name);
		اذهب out;
	}

	/* don't return zero, that means EOF */
	result = 1;

out:
	أرجع result;
}

ساكن صحيح
import_one_entry(uint8_t *buffer, size_t buf_size, فراغ *arg)
{
	صحيح *result = static_cast<صحيح *>(arg);
	bson_decoder_s decoder{};

	إذا (bson_decoder_init_buf(&decoder, buffer, buf_size, param_import_callback)) {
		debug("decoder init failed");
		*result = -1;
		أرجع -1;
	}

	افعل {
		*result = bson_decoder_next(&decoder);

	} طالما (*result > 0);

	أرجع *result < 0 ? *result : 0;
}

ساكن صحيح
param_import_internal(منطقي *legacy)
{
	صحيح result = 0;
	log_stale = خطأ;
	صحيح n = parameter_flashfs_walk(parameters_legacy_token, import_one_entry, &result);
	*legacy = n > 0;

	إذا (n == 0) {
		n = parameter_flashfs_walk(parameters_token, import_one_entry, &result);
	}

	إذا (n < 0) {
		debug("flash walk failed (%d)", n);
		log_unreadable = true;
		أرجع n;
	}

	إذا (n == 0) {
		/* No valid entry found. A fully erased store is blank (first boot, or
		 * after switching firmware): report "not yet stored" (1), matching the
		 * convention used by param_load_default(). A store that holds data but
		 * no valid entry (torn write, bit-rot, or foreign data) is corrupt and
		 * must not be silently reseeded - report it so the boot recovery runs. */
		إذا (parameter_flashfs_blank() == 1) {
			أرجع 1;
		}

		debug("flash holds data but no valid entry");
		أرجع -EILSEQ;
	}

	إذا (result < 0) {
		debug("BSON error decoding parameters");
		أرجع result;
	}

	أرجع 0;
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
	صحيح result = encode_document(&encoder, nullptr, true);

	إذا (result != 0) {
		PX4_ERR("flashparams: snapshot encode failed (%d), log kept", result);
		حرر(bson_encoder_buf_data(&encoder));
		أرجع 0;
	}

	ثابت size_t enc_size = bson_encoder_buf_size(&encoder);
	صحيح need = (legacy || log_stale) ? 1 : parameter_flashfs_needs_compact(parameters_token, enc_size);
	result = 0;

	إذا (need > 0) {
		uint8_t *buffer;
		size_t staged_size;
		result = stage_snapshot(&encoder, &buffer, &staged_size);

		إذا (result != OK) {
			PX4_ERR("flashparams: cannot stage snapshot (%d), log kept", result);
			result = 0;

		} وإلا {
			PX4_INFO("flashparams: compacting param sector%s", legacy ? " (legacy layout)" : "");
			result = commit_snapshot(buffer, staged_size);

			إذا (result != OK) {
				PX4_ERR("flashparams: compact failed (%d)", result);
			}
		}
	}

	حرر(bson_encoder_buf_data(&encoder));
	أرجع result;
}

صحيح flash_param_save(param_filter_func filter)
{
	أرجع param_export_internal(filter);
}

صحيح flash_param_import()
{
	منطقي legacy = خطأ;
	صحيح result = param_import_internal(&legacy);

	إذا (result != 0) {
		أرجع result;
	}

	أرجع compact_after_import(legacy);
}
