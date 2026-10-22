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
# define debug(fmt, دلائل...)            کرو { warnx(fmt, ##args); } جب(0)
#ورنہ
# define debug(fmt, دلائل...)            کرو { } جب(0)
#اگر_ختم


/**
 * Storage for modified parameters.
 */
ساخت param_wbuf_s {
	اتحاد param_قدر_u     val;
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
ساکن منطقی log_stale;

/* Replay failed inside a CRC-valid record. A delta appended behind it would
 * never be reached, so the next save rewrites the store. */
ساکن منطقی log_unreadable;

ساکن منطقی
any_unsaved(param_filter_func filter)
{
	برائے (param_t param = 0; param < user_config.PARAM_گنتی; param++) {
		اگر ((filter == خالی_اشاریہ || filter(param)) && param_قدر_unsaved(param)) {
			واپس سچ;
		}
	}

	واپس جھوٹ;
}

/*
 * A snapshot holds every override in RAM, equal to the current default or
 * not: the boot compaction runs before the airframe applies its set-default
 * values, so at that point "equals the default" says nothing. A delta holds
 * the unsaved params; one that no longer overrides, or now matches the
 * default in force, is written as a BSON null so replay drops it, which is
 * what the file backend's "don't export defaults" gives after a reload.
 */
ساکن صحیح
encode_params(bson_encoder_s *encoder, param_filter_func filter, منطقی snapshot)
{
	برائے (param_t param = 0; param < user_config.PARAM_گنتی; param++) {
		اگر (filter && !filter(param)) {
			جاری;
		}

		اگر (snapshot) {
			اگر (!user_config.شامل_ہے(param)) {
				جاری;
			}

		} ورنہ {
			اگر (!param_قدر_unsaved(param)) {
				جاری;
			}

			اگر (!user_config.شامل_ہے(param) || param_قدر_is_default(param)) {
				اگر (bson_encoder_append_null(encoder, param_name(param))) {
					debug("BSON append failed for '%s'", param_name(param));
					واپس -1;
				}

				جاری;
			}
		}

		انتخاب (param_type(param)) {
		حالت PARAM_TYPE_INT32:
			اگر (bson_encoder_append_int32(encoder, param_name(param), user_config.get(param).i)) {
				debug("BSON append failed for '%s'", param_name(param));
				واپس -1;
			}

			توڑو;

		حالت PARAM_TYPE_FLOAT:
			اگر (bson_encoder_append_double(encoder, param_name(param), (دوہرا)user_config.get(param).f)) {
				debug("BSON append failed for '%s'", param_name(param));
				واپس -1;
			}

			توڑو;

		طےشدہ:
			debug("unrecognized parameter type");
			واپس -1;
		}
	}

	واپس 0;
}

/* The caller frees bson_encoder_buf_data() whatever the result. */
ساکن صحیح
encode_document(bson_encoder_s *encoder, param_filter_func filter, منطقی snapshot)
{
	bson_encoder_init_buf(encoder, خالی_اشاریہ, 0);

	صحیح نتیجہ = encode_params(encoder, filter, snapshot);

	اگر (نتیجہ == 0) {
		نتیجہ = bson_encoder_fini(encoder);
	}

	واپس نتیجہ;
}

ساکن صحیح
stage_write_بفر(bson_encoder_s *encoder, uint8_t **بفر_2, size_t *enc_سائز)
{
	خالی *enc_buff = bson_encoder_buf_ڈیٹا(encoder);
	*enc_سائز = bson_encoder_buf_سائز(encoder);

	اگر (enc_buff == خالی_اشاریہ) {
		واپس -ENOMEM;
	}

	size_t alloc_سائز = *enc_سائز;
	صحیح نتیجہ = parameter_flashfs_alloc(parameters_token, بفر_2, &alloc_سائز);

	اگر (نتیجہ != OK) {
		واپس نتیجہ;
	}

	اگر (alloc_سائز < *enc_سائز) {
		parameter_flashfs_free();
		واپس -ENOMEM;
	}

	حافظہ_نقل(*بفر_2, enc_buff, *enc_سائز);
	واپس OK;
}

ساکن صحیح
commit_append(uint8_t *بفر_2, size_t enc_سائز)
{
	صحیح نتیجہ = parameter_flashfs_write(parameters_token, بفر_2, enc_سائز);
	parameter_flashfs_free();

	اگر (نتیجہ == (صحیح)enc_سائز) {
		واپس OK;
	}

	واپس نتیجہ < 0 ? نتیجہ : -EFBIG;
}

ساکن صحیح
export_delta(param_filter_func filter)
{
	bson_encoder_s encoder{};
	صحیح نتیجہ = encode_document(&encoder, filter, جھوٹ);

	اگر (نتیجہ == 0) {
		uint8_t *بفر_2;
		size_t enc_سائز;
		نتیجہ = stage_write_بفر(&encoder, &بفر_2, &enc_سائز);

		اگر (نتیجہ == OK) {
			نتیجہ = commit_append(بفر_2, enc_سائز);
		}
	}

	آزاد(bson_encoder_buf_ڈیٹا(&encoder));
	واپس نتیجہ;
}

/* Everything that can fail before the erase, so a failure here leaves the
 * log untouched and still loadable. */
ساکن صحیح
stage_snapshot(bson_encoder_s *encoder, uint8_t **بفر_2, size_t *enc_سائز)
{
	صحیح نتیجہ = stage_write_بفر(encoder, بفر_2, enc_سائز);

	اگر (نتیجہ != OK) {
		واپس نتیجہ;
	}

	اگر (*enc_سائز > parameter_flashfs_max_payload()) {
		parameter_flashfs_free();
		واپس -ENOSPC;
	}

	واپس OK;
}

ساکن صحیح
commit_snapshot(uint8_t *بفر_2, size_t enc_سائز)
{
	اگر (parameter_flashfs_blank() != 1) {
		صحیح rv = parameter_flashfs_erase();

		اگر (rv < 0) {
			parameter_flashfs_free();
			واپس rv;
		}
	}

	صحیح نتیجہ = parameter_flashfs_write(parameters_token, بفر_2, enc_سائز);

	اگر (نتیجہ != (صحیح)enc_سائز && parameter_flashfs_blank() == 1) {
		نتیجہ = parameter_flashfs_write(parameters_token, بفر_2, enc_سائز);
	}

	parameter_flashfs_free();

	اگر (نتیجہ == (صحیح)enc_سائز) {
		واپس OK;
	}

	واپس نتیجہ < 0 ? نتیجہ : -EFBIG;
}

ساکن صحیح
compact_مقصد_snapshot()
{
	bson_encoder_s encoder{};
	صحیح نتیجہ = encode_document(&encoder, خالی_اشاریہ, سچ);

	اگر (نتیجہ == 0) {
		uint8_t *بفر_2;
		size_t enc_سائز;
		نتیجہ = stage_snapshot(&encoder, &بفر_2, &enc_سائز);

		اگر (نتیجہ == OK) {
			نتیجہ = commit_snapshot(بفر_2, enc_سائز);
		}
	}

	اگر (نتیجہ == OK) {
		log_unreadable = جھوٹ;
	}

	آزاد(bson_encoder_buf_ڈیٹا(&encoder));
	واپس نتیجہ;
}

ساکن منطقی
has_ریکارڈ(flash_file_token_t token)
{
	uint8_t *بفر_2 = خالی_اشاریہ;
	size_t buf_سائز = 0;
	واپس parameter_flashfs_read(token, &بفر_2, &buf_سائز) >= 0;
}

ساکن صحیح
param_export_اندرونی(param_filter_func filter)
{
	/* A delta only means something behind a snapshot that replays. A legacy
	 * record still present means the boot migration did not run; replay
	 * would take it over any delta appended behind it. */
	اگر (log_unreadable || has_ریکارڈ(parameters_legacy_token) || !has_ریکارڈ(parameters_token)) {
		واپس compact_مقصد_snapshot();
	}

	اگر (!any_unsaved(filter)) {
		واپس 0;
	}

	صحیح نتیجہ = export_delta(filter);

	اگر (نتیجہ != -ENOSPC) {
		واپس نتیجہ;
	}

	/* Same-bank erase stalls the CPU for ~1 s; last resort when a single
	 * save does not fit the tail. Compact writes the full RAM snapshot so a
	 * filtered save cannot drop params not in the delta. */
	PX4_WARN("flashparams: param sector full, compacting");
	واپس compact_مقصد_snapshot();
}


ساکن منطقی
ریکارڈ_is_stale(bson_گرہ_t گرہ_2)
{
	ثابت param_t param = param_ڈھونڈو_no_notification(گرہ_2->name);

	اگر (param == PARAM_INVALID) {
		واپس سچ;
	}

	انتخاب (گرہ_2->type) {
	حالت BSON_INT32:
		واپس param_type(param) != PARAM_TYPE_INT32;

	حالت BSON_DOUBLE:
		واپس param_type(param) != PARAM_TYPE_FLOAT;

	طےشدہ:
		واپس جھوٹ;
	}
}

ساکن صحیح
param_import_callback(bson_decoder_t decoder, bson_گرہ_t گرہ_2)
{
	اعشاری f;
	int32_t i;
	خالی *v = خالی_اشاریہ;
	صحیح نتیجہ = -1;

	/*
	 * EOO means the end of the parameter object. (Currently not supporting
	 * nested BSON objects).
	 */
	اگر (گرہ_2->type == BSON_EOO) {
		debug("end of parameters");
		واپس 0;
	}

	/* A tombstone has no value. The decoder leaves the previous node's in
	 * the union and the translations read it without a type check. */
	ثابت منطقی tombstone = گرہ_2->type == BSON_nullptr || گرہ_2->type == BSON_UNDEFINED;

	اگر (!tombstone) {
		اگر (ریکارڈ_is_stale(گرہ_2)) {
			log_stale = سچ;
		}

		اگر (param_modify_on_import(گرہ_2) == param_modify_on_import_ret::PARAM_SKIP_IMPORT) {
			واپس 1;
		}
	}

	/*
	 * Find the parameter this node represents.  If we don't know it,
	 * ignore the node.
	 */
	param_t param = param_ڈھونڈو_no_notification(گرہ_2->name);

	اگر (param == PARAM_INVALID) {
		debug("ignoring unrecognised parameter '%s'", گرہ_2->name);
		واپس 1;
	}

	/*
	 * Handle setting the parameter from the node
	 */

	انتخاب (گرہ_2->type) {
	حالت BSON_nullptr:
	حالت BSON_UNDEFINED:
		user_config.reset(param);
		نتیجہ = 1;
		جاؤ برآمد;

	حالت BSON_INT32:
		اگر (param_type(param) != PARAM_TYPE_INT32) {
			PX4_WARN("unexpected type for %s", گرہ_2->name);
			نتیجہ = 1; // just skip this entry
			جاؤ برآمد;
		}

		i = گرہ_2->i32;
		v = &i;
		توڑو;

	حالت BSON_DOUBLE:
		اگر (param_type(param) != PARAM_TYPE_FLOAT) {
			PX4_WARN("unexpected type for %s", گرہ_2->name);
			نتیجہ = 1; // just skip this entry
			جاؤ برآمد;
		}

		f = گرہ_2->d;
		v = &f;
		توڑو;

	طےشدہ:
		PX4_ERR("%s unrecognised node type %d", گرہ_2->name, گرہ_2->type);
		نتیجہ = 1; // just skip this entry
		جاؤ برآمد;
	}

	اگر (param_سیٹ_external(param, v, سچ, سچ)) {
		debug("error setting value for '%s'", گرہ_2->name);
		جاؤ برآمد;
	}

	/* don't return zero, that means EOF */
	نتیجہ = 1;

برآمد:
	واپس نتیجہ;
}

ساکن صحیح
import_one_اندراج(uint8_t *بفر_2, size_t buf_سائز, خالی *arg)
{
	صحیح *نتیجہ = static_cast<صحیح *>(arg);
	bson_decoder_s decoder{};

	اگر (bson_decoder_init_buf(&decoder, بفر_2, buf_سائز, param_import_callback)) {
		debug("decoder init failed");
		*نتیجہ = -1;
		واپس -1;
	}

	کرو {
		*نتیجہ = bson_decoder_اگلا(&decoder);

	} جب (*نتیجہ > 0);

	واپس *نتیجہ < 0 ? *نتیجہ : 0;
}

ساکن صحیح
param_import_اندرونی(منطقی *legacy)
{
	صحیح نتیجہ = 0;
	log_stale = جھوٹ;
	صحیح n = parameter_flashfs_walk(parameters_legacy_token, import_one_اندراج, &نتیجہ);
	*legacy = n > 0;

	اگر (n == 0) {
		n = parameter_flashfs_walk(parameters_token, import_one_اندراج, &نتیجہ);
	}

	اگر (n < 0) {
		debug("flash walk failed (%d)", n);
		log_unreadable = سچ;
		واپس n;
	}

	اگر (n == 0) {
		/* No valid entry found. A fully erased store is blank (first boot, or
		 * after switching firmware): report "not yet stored" (1), matching the
		 * convention used by param_load_default(). A store that holds data but
		 * no valid entry (torn write, bit-rot, or foreign data) is corrupt and
		 * must not be silently reseeded - report it so the boot recovery runs. */
		اگر (parameter_flashfs_blank() == 1) {
			واپس 1;
		}

		debug("flash holds data but no valid entry");
		واپس -EILSEQ;
	}

	اگر (نتیجہ < 0) {
		debug("BSON error decoding parameters");
		واپس نتیجہ;
	}

	واپس 0;
}

/*
 * Same-bank erase stalls instruction fetch for ~1 s on H7. Do it here at
 * boot, before sensors or DShot start, and only when a burst of deltas would
 * no longer fit, the store is still in the legacy layout, or the log holds
 * records this firmware translated or could not use. Only a failure after
 * the erase is an error: until then the log is intact and RAM holds what it
 * said, so the boot must not be reported as a corrupt store.
 */
ساکن صحیح
compact_after_import(منطقی legacy)
{
	bson_encoder_s encoder{};
	صحیح نتیجہ = encode_document(&encoder, خالی_اشاریہ, سچ);

	اگر (نتیجہ != 0) {
		PX4_ERR("flashparams: snapshot encode failed (%d), log kept", نتیجہ);
		آزاد(bson_encoder_buf_ڈیٹا(&encoder));
		واپس 0;
	}

	ثابت size_t enc_سائز = bson_encoder_buf_سائز(&encoder);
	صحیح need = (legacy || log_stale) ? 1 : parameter_flashfs_needs_compact(parameters_token, enc_سائز);
	نتیجہ = 0;

	اگر (need > 0) {
		uint8_t *بفر_2;
		size_t staged_سائز;
		نتیجہ = stage_snapshot(&encoder, &بفر_2, &staged_سائز);

		اگر (نتیجہ != OK) {
			PX4_ERR("flashparams: cannot stage snapshot (%d), log kept", نتیجہ);
			نتیجہ = 0;

		} ورنہ {
			PX4_INFO("flashparams: compacting param sector%s", legacy ? " (legacy layout)" : "");
			نتیجہ = commit_snapshot(بفر_2, staged_سائز);

			اگر (نتیجہ != OK) {
				PX4_ERR("flashparams: compact failed (%d)", نتیجہ);
			}
		}
	}

	آزاد(bson_encoder_buf_ڈیٹا(&encoder));
	واپس نتیجہ;
}

صحیح flash_param_save(param_filter_func filter)
{
	واپس param_export_اندرونی(filter);
}

صحیح flash_param_import()
{
	منطقی legacy = جھوٹ;
	صحیح نتیجہ = param_import_اندرونی(&legacy);

	اگر (نتیجہ != 0) {
		واپس نتیجہ;
	}

	واپس compact_after_import(legacy);
}
