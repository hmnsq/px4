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

#شامل <px4_platform_common/defines.h>
#شامل <px4_platform_common/log.h>
#شامل <px4_platform_common/posix.h>
#شامل <px4_platform_common/shutdown.h>
#شامل <parameters/px4_parameters.hpp>

#شامل <string.h>
#شامل <stdbool.h>
#شامل <stdint.h>
#شامل <errno.h>

#شامل <parameters/param.h>

#شامل <lib/tinybson/tinybson.h>
#شامل "flashparams.h"
#شامل "flashfs.h"
#شامل "../param_translation.h"

#اگر 0
# define debug(fmt, args...)            انجامبده { warnx(fmt, ##args); } وقتی(0)
#وگرنه
# define debug(fmt, args...)            انجامبده { } وقتی(0)
#endif


/**
 * Storage for modified parameters.
 */
ساختار param_wbuf_s {
	اجتماع param_مقدار_u     val;
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
ایستا منطقی log_stale;

/* Replay failed inside a CRC-valid record. A delta appended behind it would
 * never be reached, so the next save rewrites the store. */
ایستا منطقی log_unreadable;

ایستا منطقی
any_unsaved(param_filter_func filter)
{
	برای (param_t param = 0; param < user_config.PARAM_شمار; param++) {
		اگر ((filter == nullptr || filter(param)) && param_مقدار_unsaved(param)) {
			بازگشت درست;
		}
	}

	بازگشت نادرست;
}

/*
 * A snapshot holds every override in RAM, equal to the current default or
 * not: the boot compaction runs before the airframe applies its set-default
 * values, so at that point "equals the default" says nothing. A delta holds
 * the unsaved params; one that no longer overrides, or now matches the
 * default in force, is written as a BSON null so replay drops it, which is
 * what the file backend's "don't export defaults" gives after a reload.
 */
ایستا صحیح
encode_params(bson_encoder_s *encoder, param_filter_func filter, منطقی snapshot)
{
	برای (param_t param = 0; param < user_config.PARAM_شمار; param++) {
		اگر (filter && !filter(param)) {
			ادامه;
		}

		اگر (snapshot) {
			اگر (!user_config.contains(param)) {
				ادامه;
			}

		} وگرنه {
			اگر (!param_مقدار_unsaved(param)) {
				ادامه;
			}

			اگر (!user_config.contains(param) || param_مقدار_is_default(param)) {
				اگر (bson_encoder_append_null(encoder, param_name(param))) {
					debug("BSON append failed for '%s'", param_name(param));
					بازگشت -1;
				}

				ادامه;
			}
		}

		انتخاب (param_type(param)) {
		حالت PARAM_TYPE_INT32:
			اگر (bson_encoder_append_int32(encoder, param_name(param), user_config.get(param).i)) {
				debug("BSON append failed for '%s'", param_name(param));
				بازگشت -1;
			}

			بشکن;

		حالت PARAM_TYPE_FLOAT:
			اگر (bson_encoder_append_double(encoder, param_name(param), (دوگانه)user_config.get(param).f)) {
				debug("BSON append failed for '%s'", param_name(param));
				بازگشت -1;
			}

			بشکن;

		پیشفرض:
			debug("unrecognized parameter type");
			بازگشت -1;
		}
	}

	بازگشت 0;
}

/* The caller frees bson_encoder_buf_data() whatever the result. */
ایستا صحیح
encode_document(bson_encoder_s *encoder, param_filter_func filter, منطقی snapshot)
{
	bson_encoder_init_buf(encoder, nullptr, 0);

	صحیح result = encode_params(encoder, filter, snapshot);

	اگر (result == 0) {
		result = bson_encoder_fini(encoder);
	}

	بازگشت result;
}

ایستا صحیح
stage_write_buffer(bson_encoder_s *encoder, uint8_t **buffer, size_t *enc_size)
{
	تهی *enc_buff = bson_encoder_buf_data(encoder);
	*enc_size = bson_encoder_buf_size(encoder);

	اگر (enc_buff == nullptr) {
		بازگشت -ENOMEM;
	}

	size_t alloc_size = *enc_size;
	صحیح result = parameter_flashfs_alloc(parameters_token, buffer, &alloc_size);

	اگر (result != OK) {
		بازگشت result;
	}

	اگر (alloc_size < *enc_size) {
		parameter_flashfs_free();
		بازگشت -ENOMEM;
	}

	memcpy(*buffer, enc_buff, *enc_size);
	بازگشت OK;
}

ایستا صحیح
commit_append(uint8_t *buffer, size_t enc_size)
{
	صحیح result = parameter_flashfs_write(parameters_token, buffer, enc_size);
	parameter_flashfs_free();

	اگر (result == (صحیح)enc_size) {
		بازگشت OK;
	}

	بازگشت result < 0 ? result : -EFBIG;
}

ایستا صحیح
export_delta(param_filter_func filter)
{
	bson_encoder_s encoder{};
	صحیح result = encode_document(&encoder, filter, نادرست);

	اگر (result == 0) {
		uint8_t *buffer;
		size_t enc_size;
		result = stage_write_buffer(&encoder, &buffer, &enc_size);

		اگر (result == OK) {
			result = commit_append(buffer, enc_size);
		}
	}

	آزاد(bson_encoder_buf_data(&encoder));
	بازگشت result;
}

/* Everything that can fail before the erase, so a failure here leaves the
 * log untouched and still loadable. */
ایستا صحیح
stage_snapshot(bson_encoder_s *encoder, uint8_t **buffer, size_t *enc_size)
{
	صحیح result = stage_write_buffer(encoder, buffer, enc_size);

	اگر (result != OK) {
		بازگشت result;
	}

	اگر (*enc_size > parameter_flashfs_max_payload()) {
		parameter_flashfs_free();
		بازگشت -ENOSPC;
	}

	بازگشت OK;
}

ایستا صحیح
commit_snapshot(uint8_t *buffer, size_t enc_size)
{
	اگر (parameter_flashfs_blank() != 1) {
		صحیح rv = parameter_flashfs_erase();

		اگر (rv < 0) {
			parameter_flashfs_free();
			بازگشت rv;
		}
	}

	صحیح result = parameter_flashfs_write(parameters_token, buffer, enc_size);

	اگر (result != (صحیح)enc_size && parameter_flashfs_blank() == 1) {
		result = parameter_flashfs_write(parameters_token, buffer, enc_size);
	}

	parameter_flashfs_free();

	اگر (result == (صحیح)enc_size) {
		بازگشت OK;
	}

	بازگشت result < 0 ? result : -EFBIG;
}

ایستا صحیح
compact_to_snapshot()
{
	bson_encoder_s encoder{};
	صحیح result = encode_document(&encoder, nullptr, درست);

	اگر (result == 0) {
		uint8_t *buffer;
		size_t enc_size;
		result = stage_snapshot(&encoder, &buffer, &enc_size);

		اگر (result == OK) {
			result = commit_snapshot(buffer, enc_size);
		}
	}

	اگر (result == OK) {
		log_unreadable = نادرست;
	}

	آزاد(bson_encoder_buf_data(&encoder));
	بازگشت result;
}

ایستا منطقی
has_record(flash_file_token_t token)
{
	uint8_t *buffer = nullptr;
	size_t buf_size = 0;
	بازگشت parameter_flashfs_read(token, &buffer, &buf_size) >= 0;
}

ایستا صحیح
param_export_internal(param_filter_func filter)
{
	/* A delta only means something behind a snapshot that replays. A legacy
	 * record still present means the boot migration did not run; replay
	 * would take it over any delta appended behind it. */
	اگر (log_unreadable || has_record(parameters_legacy_token) || !has_record(parameters_token)) {
		بازگشت compact_to_snapshot();
	}

	اگر (!any_unsaved(filter)) {
		بازگشت 0;
	}

	صحیح result = export_delta(filter);

	اگر (result != -ENOSPC) {
		بازگشت result;
	}

	/* Same-bank erase stalls the CPU for ~1 s; last resort when a single
	 * save does not fit the tail. Compact writes the full RAM snapshot so a
	 * filtered save cannot drop params not in the delta. */
	PX4_WARN("flashparams: param sector full, compacting");
	بازگشت compact_to_snapshot();
}


ایستا منطقی
record_is_stale(bson_گره_t گره_2)
{
	ثابت param_t param = param_find_no_notification(گره_2->name);

	اگر (param == PARAM_INVALID) {
		بازگشت درست;
	}

	انتخاب (گره_2->type) {
	حالت BSON_INT32:
		بازگشت param_type(param) != PARAM_TYPE_INT32;

	حالت BSON_DOUBLE:
		بازگشت param_type(param) != PARAM_TYPE_FLOAT;

	پیشفرض:
		بازگشت نادرست;
	}
}

ایستا صحیح
param_import_callback(bson_decoder_t decoder, bson_گره_t گره_2)
{
	اعشاری f;
	int32_t i;
	تهی *v = nullptr;
	صحیح result = -1;

	/*
	 * EOO means the end of the parameter object. (Currently not supporting
	 * nested BSON objects).
	 */
	اگر (گره_2->type == BSON_EOO) {
		debug("end of parameters");
		بازگشت 0;
	}

	/* A tombstone has no value. The decoder leaves the previous node's in
	 * the union and the translations read it without a type check. */
	ثابت منطقی tombstone = گره_2->type == BSON_nullptr || گره_2->type == BSON_UNDEFINED;

	اگر (!tombstone) {
		اگر (record_is_stale(گره_2)) {
			log_stale = درست;
		}

		اگر (param_modify_on_import(گره_2) == param_modify_on_import_ret::PARAM_SKIP_IMPORT) {
			بازگشت 1;
		}
	}

	/*
	 * Find the parameter this node represents.  If we don't know it,
	 * ignore the node.
	 */
	param_t param = param_find_no_notification(گره_2->name);

	اگر (param == PARAM_INVALID) {
		debug("ignoring unrecognised parameter '%s'", گره_2->name);
		بازگشت 1;
	}

	/*
	 * Handle setting the parameter from the node
	 */

	انتخاب (گره_2->type) {
	حالت BSON_nullptr:
	حالت BSON_UNDEFINED:
		user_config.reset(param);
		result = 1;
		برو out;

	حالت BSON_INT32:
		اگر (param_type(param) != PARAM_TYPE_INT32) {
			PX4_WARN("unexpected type for %s", گره_2->name);
			result = 1; // just skip this entry
			برو out;
		}

		i = گره_2->i32;
		v = &i;
		بشکن;

	حالت BSON_DOUBLE:
		اگر (param_type(param) != PARAM_TYPE_FLOAT) {
			PX4_WARN("unexpected type for %s", گره_2->name);
			result = 1; // just skip this entry
			برو out;
		}

		f = گره_2->d;
		v = &f;
		بشکن;

	پیشفرض:
		PX4_ERR("%s unrecognised node type %d", گره_2->name, گره_2->type);
		result = 1; // just skip this entry
		برو out;
	}

	اگر (param_set_external(param, v, درست, درست)) {
		debug("error setting value for '%s'", گره_2->name);
		برو out;
	}

	/* don't return zero, that means EOF */
	result = 1;

out:
	بازگشت result;
}

ایستا صحیح
import_one_entry(uint8_t *buffer, size_t buf_size, تهی *arg)
{
	صحیح *result = static_cast<صحیح *>(arg);
	bson_decoder_s decoder{};

	اگر (bson_decoder_init_buf(&decoder, buffer, buf_size, param_import_callback)) {
		debug("decoder init failed");
		*result = -1;
		بازگشت -1;
	}

	انجامبده {
		*result = bson_decoder_next(&decoder);

	} وقتی (*result > 0);

	بازگشت *result < 0 ? *result : 0;
}

ایستا صحیح
param_import_internal(منطقی *legacy)
{
	صحیح result = 0;
	log_stale = نادرست;
	صحیح n = parameter_flashfs_walk(parameters_legacy_token, import_one_entry, &result);
	*legacy = n > 0;

	اگر (n == 0) {
		n = parameter_flashfs_walk(parameters_token, import_one_entry, &result);
	}

	اگر (n < 0) {
		debug("flash walk failed (%d)", n);
		log_unreadable = درست;
		بازگشت n;
	}

	اگر (n == 0) {
		/* No valid entry found. A fully erased store is blank (first boot, or
		 * after switching firmware): report "not yet stored" (1), matching the
		 * convention used by param_load_default(). A store that holds data but
		 * no valid entry (torn write, bit-rot, or foreign data) is corrupt and
		 * must not be silently reseeded - report it so the boot recovery runs. */
		اگر (parameter_flashfs_blank() == 1) {
			بازگشت 1;
		}

		debug("flash holds data but no valid entry");
		بازگشت -EILSEQ;
	}

	اگر (result < 0) {
		debug("BSON error decoding parameters");
		بازگشت result;
	}

	بازگشت 0;
}

/*
 * Same-bank erase stalls instruction fetch for ~1 s on H7. Do it here at
 * boot, before sensors or DShot start, and only when a burst of deltas would
 * no longer fit, the store is still in the legacy layout, or the log holds
 * records this firmware translated or could not use. Only a failure after
 * the erase is an error: until then the log is intact and RAM holds what it
 * said, so the boot must not be reported as a corrupt store.
 */
ایستا صحیح
compact_after_import(منطقی legacy)
{
	bson_encoder_s encoder{};
	صحیح result = encode_document(&encoder, nullptr, درست);

	اگر (result != 0) {
		PX4_ERR("flashparams: snapshot encode failed (%d), log kept", result);
		آزاد(bson_encoder_buf_data(&encoder));
		بازگشت 0;
	}

	ثابت size_t enc_size = bson_encoder_buf_size(&encoder);
	صحیح need = (legacy || log_stale) ? 1 : parameter_flashfs_needs_compact(parameters_token, enc_size);
	result = 0;

	اگر (need > 0) {
		uint8_t *buffer;
		size_t staged_size;
		result = stage_snapshot(&encoder, &buffer, &staged_size);

		اگر (result != OK) {
			PX4_ERR("flashparams: cannot stage snapshot (%d), log kept", result);
			result = 0;

		} وگرنه {
			PX4_INFO("flashparams: compacting param sector%s", legacy ? " (legacy layout)" : "");
			result = commit_snapshot(buffer, staged_size);

			اگر (result != OK) {
				PX4_ERR("flashparams: compact failed (%d)", result);
			}
		}
	}

	آزاد(bson_encoder_buf_data(&encoder));
	بازگشت result;
}

صحیح flash_param_save(param_filter_func filter)
{
	بازگشت param_export_internal(filter);
}

صحیح flash_param_import()
{
	منطقی legacy = نادرست;
	صحیح result = param_import_internal(&legacy);

	اگر (result != 0) {
		بازگشت result;
	}

	بازگشت compact_after_import(legacy);
}
