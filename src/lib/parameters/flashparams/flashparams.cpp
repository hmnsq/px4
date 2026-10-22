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
# define debug(fmt, دلیلاں...)            افعل { warnx(fmt, ##args); } طالما(0)
#وإلا
# define debug(fmt, دلیلاں...)            افعل { } طالما(0)
#جے_انت


/**
 * Storage for modified parameters.
 */
بنية param_wbuf_s {
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
ساكن منطقي log_stale;

/* Replay failed inside a CRC-valid record. A delta appended behind it would
 * never be reached, so the next save rewrites the store. */
ساكن منطقي log_unreadable;

ساكن منطقي
any_unsaved(param_filter_func filter)
{
	لكل (param_t param = 0; param < user_config.PARAM_گنتی; param++) {
		إذا ((filter == خالی_سوچک || filter(param)) && param_قدر_unsaved(param)) {
			أرجع سچ;
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
	لكل (param_t param = 0; param < user_config.PARAM_گنتی; param++) {
		إذا (filter && !filter(param)) {
			استمر;
		}

		إذا (snapshot) {
			إذا (!user_config.شامل_اے(param)) {
				استمر;
			}

		} وإلا {
			إذا (!param_قدر_unsaved(param)) {
				استمر;
			}

			إذا (!user_config.شامل_اے(param) || param_قدر_is_default(param)) {
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
	bson_encoder_init_buf(encoder, خالی_سوچک, 0);

	صحيح نتیجہ = encode_params(encoder, filter, snapshot);

	إذا (نتیجہ == 0) {
		نتیجہ = bson_encoder_fini(encoder);
	}

	أرجع نتیجہ;
}

ساكن صحيح
stage_write_بفر(bson_encoder_s *encoder, uint8_t **بفر_2, size_t *enc_ناپ)
{
	فراغ *enc_buff = bson_encoder_buf_انکڑے(encoder);
	*enc_ناپ = bson_encoder_buf_ناپ(encoder);

	إذا (enc_buff == خالی_سوچک) {
		أرجع -ENOMEM;
	}

	size_t alloc_ناپ = *enc_ناپ;
	صحيح نتیجہ = parameter_flashfs_alloc(parameters_token, بفر_2, &alloc_ناپ);

	إذا (نتیجہ != OK) {
		أرجع نتیجہ;
	}

	إذا (alloc_ناپ < *enc_ناپ) {
		parameter_flashfs_free();
		أرجع -ENOMEM;
	}

	حافظہ_نقل(*بفر_2, enc_buff, *enc_ناپ);
	أرجع OK;
}

ساكن صحيح
commit_append(uint8_t *بفر_2, size_t enc_ناپ)
{
	صحيح نتیجہ = parameter_flashfs_write(parameters_token, بفر_2, enc_ناپ);
	parameter_flashfs_free();

	إذا (نتیجہ == (صحيح)enc_ناپ) {
		أرجع OK;
	}

	أرجع نتیجہ < 0 ? نتیجہ : -EFBIG;
}

ساكن صحيح
export_delta(param_filter_func filter)
{
	bson_encoder_s encoder{};
	صحيح نتیجہ = encode_document(&encoder, filter, خطأ);

	إذا (نتیجہ == 0) {
		uint8_t *بفر_2;
		size_t enc_ناپ;
		نتیجہ = stage_write_بفر(&encoder, &بفر_2, &enc_ناپ);

		إذا (نتیجہ == OK) {
			نتیجہ = commit_append(بفر_2, enc_ناپ);
		}
	}

	حرر(bson_encoder_buf_انکڑے(&encoder));
	أرجع نتیجہ;
}

/* Everything that can fail before the erase, so a failure here leaves the
 * log untouched and still loadable. */
ساكن صحيح
stage_snapshot(bson_encoder_s *encoder, uint8_t **بفر_2, size_t *enc_ناپ)
{
	صحيح نتیجہ = stage_write_بفر(encoder, بفر_2, enc_ناپ);

	إذا (نتیجہ != OK) {
		أرجع نتیجہ;
	}

	إذا (*enc_ناپ > parameter_flashfs_max_payload()) {
		parameter_flashfs_free();
		أرجع -ENOSPC;
	}

	أرجع OK;
}

ساكن صحيح
commit_snapshot(uint8_t *بفر_2, size_t enc_ناپ)
{
	إذا (parameter_flashfs_blank() != 1) {
		صحيح rv = parameter_flashfs_erase();

		إذا (rv < 0) {
			parameter_flashfs_free();
			أرجع rv;
		}
	}

	صحيح نتیجہ = parameter_flashfs_write(parameters_token, بفر_2, enc_ناپ);

	إذا (نتیجہ != (صحيح)enc_ناپ && parameter_flashfs_blank() == 1) {
		نتیجہ = parameter_flashfs_write(parameters_token, بفر_2, enc_ناپ);
	}

	parameter_flashfs_free();

	إذا (نتیجہ == (صحيح)enc_ناپ) {
		أرجع OK;
	}

	أرجع نتیجہ < 0 ? نتیجہ : -EFBIG;
}

ساكن صحيح
compact_منزل_snapshot()
{
	bson_encoder_s encoder{};
	صحيح نتیجہ = encode_document(&encoder, خالی_سوچک, سچ);

	إذا (نتیجہ == 0) {
		uint8_t *بفر_2;
		size_t enc_ناپ;
		نتیجہ = stage_snapshot(&encoder, &بفر_2, &enc_ناپ);

		إذا (نتیجہ == OK) {
			نتیجہ = commit_snapshot(بفر_2, enc_ناپ);
		}
	}

	إذا (نتیجہ == OK) {
		log_unreadable = خطأ;
	}

	حرر(bson_encoder_buf_انکڑے(&encoder));
	أرجع نتیجہ;
}

ساكن منطقي
has_ریکارڈ(flash_file_token_t token)
{
	uint8_t *بفر_2 = خالی_سوچک;
	size_t buf_ناپ = 0;
	أرجع parameter_flashfs_read(token, &بفر_2, &buf_ناپ) >= 0;
}

ساكن صحيح
param_export_اندرونی(param_filter_func filter)
{
	/* A delta only means something behind a snapshot that replays. A legacy
	 * record still present means the boot migration did not run; replay
	 * would take it over any delta appended behind it. */
	إذا (log_unreadable || has_ریکارڈ(parameters_legacy_token) || !has_ریکارڈ(parameters_token)) {
		أرجع compact_منزل_snapshot();
	}

	إذا (!any_unsaved(filter)) {
		أرجع 0;
	}

	صحيح نتیجہ = export_delta(filter);

	إذا (نتیجہ != -ENOSPC) {
		أرجع نتیجہ;
	}

	/* Same-bank erase stalls the CPU for ~1 s; last resort when a single
	 * save does not fit the tail. Compact writes the full RAM snapshot so a
	 * filtered save cannot drop params not in the delta. */
	PX4_WARN("flashparams: param sector full, compacting");
	أرجع compact_منزل_snapshot();
}


ساكن منطقي
ریکارڈ_is_stale(bson_گرہ_t گرہ_2)
{
	ثابت param_t param = param_لبھو_no_notification(گرہ_2->name);

	إذا (param == PARAM_INVALID) {
		أرجع سچ;
	}

	بدل (گرہ_2->type) {
	حالة BSON_INT32:
		أرجع param_type(param) != PARAM_TYPE_INT32;

	حالة BSON_DOUBLE:
		أرجع param_type(param) != PARAM_TYPE_FLOAT;

	افتراضي:
		أرجع خطأ;
	}
}

ساكن صحيح
param_import_callback(bson_decoder_t decoder, bson_گرہ_t گرہ_2)
{
	عائم f;
	int32_t i;
	فراغ *v = خالی_سوچک;
	صحيح نتیجہ = -1;

	/*
	 * EOO means the end of the parameter object. (Currently not supporting
	 * nested BSON objects).
	 */
	إذا (گرہ_2->type == BSON_EOO) {
		debug("end of parameters");
		أرجع 0;
	}

	/* A tombstone has no value. The decoder leaves the previous node's in
	 * the union and the translations read it without a type check. */
	ثابت منطقي tombstone = گرہ_2->type == BSON_nullptr || گرہ_2->type == BSON_UNDEFINED;

	إذا (!tombstone) {
		إذا (ریکارڈ_is_stale(گرہ_2)) {
			log_stale = سچ;
		}

		إذا (param_modify_on_import(گرہ_2) == param_modify_on_import_ret::PARAM_SKIP_IMPORT) {
			أرجع 1;
		}
	}

	/*
	 * Find the parameter this node represents.  If we don't know it,
	 * ignore the node.
	 */
	param_t param = param_لبھو_no_notification(گرہ_2->name);

	إذا (param == PARAM_INVALID) {
		debug("ignoring unrecognised parameter '%s'", گرہ_2->name);
		أرجع 1;
	}

	/*
	 * Handle setting the parameter from the node
	 */

	بدل (گرہ_2->type) {
	حالة BSON_nullptr:
	حالة BSON_UNDEFINED:
		user_config.reset(param);
		نتیجہ = 1;
		اذهب باہرلا;

	حالة BSON_INT32:
		إذا (param_type(param) != PARAM_TYPE_INT32) {
			PX4_WARN("unexpected type for %s", گرہ_2->name);
			نتیجہ = 1; // just skip this entry
			اذهب باہرلا;
		}

		i = گرہ_2->i32;
		v = &i;
		اكسر;

	حالة BSON_DOUBLE:
		إذا (param_type(param) != PARAM_TYPE_FLOAT) {
			PX4_WARN("unexpected type for %s", گرہ_2->name);
			نتیجہ = 1; // just skip this entry
			اذهب باہرلا;
		}

		f = گرہ_2->d;
		v = &f;
		اكسر;

	افتراضي:
		PX4_ERR("%s unrecognised node type %d", گرہ_2->name, گرہ_2->type);
		نتیجہ = 1; // just skip this entry
		اذهب باہرلا;
	}

	إذا (param_سیٹ_external(param, v, سچ, سچ)) {
		debug("error setting value for '%s'", گرہ_2->name);
		اذهب باہرلا;
	}

	/* don't return zero, that means EOF */
	نتیجہ = 1;

باہرلا:
	أرجع نتیجہ;
}

ساكن صحيح
import_one_اندراج(uint8_t *بفر_2, size_t buf_ناپ, فراغ *arg)
{
	صحيح *نتیجہ = static_cast<صحيح *>(arg);
	bson_decoder_s decoder{};

	إذا (bson_decoder_init_buf(&decoder, بفر_2, buf_ناپ, param_import_callback)) {
		debug("decoder init failed");
		*نتیجہ = -1;
		أرجع -1;
	}

	افعل {
		*نتیجہ = bson_decoder_اگلا(&decoder);

	} طالما (*نتیجہ > 0);

	أرجع *نتیجہ < 0 ? *نتیجہ : 0;
}

ساكن صحيح
param_import_اندرونی(منطقي *legacy)
{
	صحيح نتیجہ = 0;
	log_stale = خطأ;
	صحيح n = parameter_flashfs_walk(parameters_legacy_token, import_one_اندراج, &نتیجہ);
	*legacy = n > 0;

	إذا (n == 0) {
		n = parameter_flashfs_walk(parameters_token, import_one_اندراج, &نتیجہ);
	}

	إذا (n < 0) {
		debug("flash walk failed (%d)", n);
		log_unreadable = سچ;
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

	إذا (نتیجہ < 0) {
		debug("BSON error decoding parameters");
		أرجع نتیجہ;
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
	صحيح نتیجہ = encode_document(&encoder, خالی_سوچک, سچ);

	إذا (نتیجہ != 0) {
		PX4_ERR("flashparams: snapshot encode failed (%d), log kept", نتیجہ);
		حرر(bson_encoder_buf_انکڑے(&encoder));
		أرجع 0;
	}

	ثابت size_t enc_ناپ = bson_encoder_buf_ناپ(&encoder);
	صحيح need = (legacy || log_stale) ? 1 : parameter_flashfs_needs_compact(parameters_token, enc_ناپ);
	نتیجہ = 0;

	إذا (need > 0) {
		uint8_t *بفر_2;
		size_t staged_ناپ;
		نتیجہ = stage_snapshot(&encoder, &بفر_2, &staged_ناپ);

		إذا (نتیجہ != OK) {
			PX4_ERR("flashparams: cannot stage snapshot (%d), log kept", نتیجہ);
			نتیجہ = 0;

		} وإلا {
			PX4_INFO("flashparams: compacting param sector%s", legacy ? " (legacy layout)" : "");
			نتیجہ = commit_snapshot(بفر_2, staged_ناپ);

			إذا (نتیجہ != OK) {
				PX4_ERR("flashparams: compact failed (%d)", نتیجہ);
			}
		}
	}

	حرر(bson_encoder_buf_انکڑے(&encoder));
	أرجع نتیجہ;
}

صحيح flash_param_save(param_filter_func filter)
{
	أرجع param_export_اندرونی(filter);
}

صحيح flash_param_import()
{
	منطقي legacy = خطأ;
	صحيح نتیجہ = param_import_اندرونی(&legacy);

	إذا (نتیجہ != 0) {
		أرجع نتیجہ;
	}

	أرجع compact_after_import(legacy);
}
