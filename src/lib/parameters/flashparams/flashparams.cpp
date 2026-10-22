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

#ਅਨ੍ਤਰ੍ਭਾਵ <px4_platform_common/defines.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <px4_platform_common/log.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <px4_platform_common/posix.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <px4_platform_common/shutdown.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <parameters/px4_parameters.hpp>

#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdbool.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdint.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <errno.h>

#ਅਨ੍ਤਰ੍ਭਾਵ <parameters/param.h>

#ਅਨ੍ਤਰ੍ਭਾਵ <lib/tinybson/tinybson.h>
#ਅਨ੍ਤਰ੍ਭਾਵ "flashparams.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "flashfs.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "../param_translation.h"

#ਜੇ 0
# define debug(fmt, ਦਲੀਲਾਂ...)            ਕਰੋ { warnx(fmt, ##args); } ਜਦੋਂ(0)
#ਨਹੀਂਤਾਂ
# define debug(fmt, ਦਲੀਲਾਂ...)            ਕਰੋ { } ਜਦੋਂ(0)
#ਜੇ_ਅੰਤ


/**
 * Storage for modified parameters.
 */
ਬਣਤਰ param_wbuf_s {
	ਸੰਘ param_ਮੁੱਲ_u     val;
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
ਸਥਿਰ ਤਾਰਕਿਕ log_stale;

/* Replay failed inside a CRC-valid record. A delta appended behind it would
 * never be reached, so the next save rewrites the store. */
ਸਥਿਰ ਤਾਰਕਿਕ log_unreadable;

ਸਥਿਰ ਤਾਰਕਿਕ
any_unsaved(param_filter_func filter)
{
	ਲਈ (param_t param = 0; param < user_config.PARAM_ਗਿਣਤੀ; param++) {
		ਜੇ ((filter == ਖਾਲੀ_ਸੂਚਕ || filter(param)) && param_ਮੁੱਲ_unsaved(param)) {
			ਵਾਪਸ ਸੱਚ;
		}
	}

	ਵਾਪਸ ਝੂਠ;
}

/*
 * A snapshot holds every override in RAM, equal to the current default or
 * not: the boot compaction runs before the airframe applies its set-default
 * values, so at that point "equals the default" says nothing. A delta holds
 * the unsaved params; one that no longer overrides, or now matches the
 * default in force, is written as a BSON null so replay drops it, which is
 * what the file backend's "don't export defaults" gives after a reload.
 */
ਸਥਿਰ ਪੂਰਨ
encode_params(bson_encoder_s *encoder, param_filter_func filter, ਤਾਰਕਿਕ snapshot)
{
	ਲਈ (param_t param = 0; param < user_config.PARAM_ਗਿਣਤੀ; param++) {
		ਜੇ (filter && !filter(param)) {
			ਜਾਰੀ;
		}

		ਜੇ (snapshot) {
			ਜੇ (!user_config.ਸ਼ਾਮਲ_ਹੈ(param)) {
				ਜਾਰੀ;
			}

		} ਨਹੀਂਤਾਂ {
			ਜੇ (!param_ਮੁੱਲ_unsaved(param)) {
				ਜਾਰੀ;
			}

			ਜੇ (!user_config.ਸ਼ਾਮਲ_ਹੈ(param) || param_ਮੁੱਲ_is_default(param)) {
				ਜੇ (bson_encoder_append_null(encoder, param_name(param))) {
					debug("BSON append failed for '%s'", param_name(param));
					ਵਾਪਸ -1;
				}

				ਜਾਰੀ;
			}
		}

		ਚੋਣ (param_type(param)) {
		ਹਾਲਤ PARAM_TYPE_INT32:
			ਜੇ (bson_encoder_append_int32(encoder, param_name(param), user_config.get(param).i)) {
				debug("BSON append failed for '%s'", param_name(param));
				ਵਾਪਸ -1;
			}

			ਰੋਕੋ;

		ਹਾਲਤ PARAM_TYPE_FLOAT:
			ਜੇ (bson_encoder_append_double(encoder, param_name(param), (ਦੁੱਗਣਾ)user_config.get(param).f)) {
				debug("BSON append failed for '%s'", param_name(param));
				ਵਾਪਸ -1;
			}

			ਰੋਕੋ;

		ਮੂਲ:
			debug("unrecognized parameter type");
			ਵਾਪਸ -1;
		}
	}

	ਵਾਪਸ 0;
}

/* The caller frees bson_encoder_buf_data() whatever the result. */
ਸਥਿਰ ਪੂਰਨ
encode_document(bson_encoder_s *encoder, param_filter_func filter, ਤਾਰਕਿਕ snapshot)
{
	bson_encoder_init_buf(encoder, ਖਾਲੀ_ਸੂਚਕ, 0);

	ਪੂਰਨ ਨਤੀਜਾ = encode_params(encoder, filter, snapshot);

	ਜੇ (ਨਤੀਜਾ == 0) {
		ਨਤੀਜਾ = bson_encoder_fini(encoder);
	}

	ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ ਪੂਰਨ
stage_write_ਬਫਰ(bson_encoder_s *encoder, uint8_t **ਬਫਰ_2, size_t *enc_ਮਾਪ)
{
	ਖਾਲੀ *enc_buff = bson_encoder_buf_ਅੰਕੜੇ(encoder);
	*enc_ਮਾਪ = bson_encoder_buf_ਮਾਪ(encoder);

	ਜੇ (enc_buff == ਖਾਲੀ_ਸੂਚਕ) {
		ਵਾਪਸ -ENOMEM;
	}

	size_t alloc_ਮਾਪ = *enc_ਮਾਪ;
	ਪੂਰਨ ਨਤੀਜਾ = parameter_flashfs_alloc(parameters_token, ਬਫਰ_2, &alloc_ਮਾਪ);

	ਜੇ (ਨਤੀਜਾ != OK) {
		ਵਾਪਸ ਨਤੀਜਾ;
	}

	ਜੇ (alloc_ਮਾਪ < *enc_ਮਾਪ) {
		parameter_flashfs_free();
		ਵਾਪਸ -ENOMEM;
	}

	ਸਮ੍ਰਿਤੀ_ਨਕਲ(*ਬਫਰ_2, enc_buff, *enc_ਮਾਪ);
	ਵਾਪਸ OK;
}

ਸਥਿਰ ਪੂਰਨ
commit_append(uint8_t *ਬਫਰ_2, size_t enc_ਮਾਪ)
{
	ਪੂਰਨ ਨਤੀਜਾ = parameter_flashfs_write(parameters_token, ਬਫਰ_2, enc_ਮਾਪ);
	parameter_flashfs_free();

	ਜੇ (ਨਤੀਜਾ == (ਪੂਰਨ)enc_ਮਾਪ) {
		ਵਾਪਸ OK;
	}

	ਵਾਪਸ ਨਤੀਜਾ < 0 ? ਨਤੀਜਾ : -EFBIG;
}

ਸਥਿਰ ਪੂਰਨ
export_delta(param_filter_func filter)
{
	bson_encoder_s encoder{};
	ਪੂਰਨ ਨਤੀਜਾ = encode_document(&encoder, filter, ਝੂਠ);

	ਜੇ (ਨਤੀਜਾ == 0) {
		uint8_t *ਬਫਰ_2;
		size_t enc_ਮਾਪ;
		ਨਤੀਜਾ = stage_write_ਬਫਰ(&encoder, &ਬਫਰ_2, &enc_ਮਾਪ);

		ਜੇ (ਨਤੀਜਾ == OK) {
			ਨਤੀਜਾ = commit_append(ਬਫਰ_2, enc_ਮਾਪ);
		}
	}

	ਮੁਕਤ(bson_encoder_buf_ਅੰਕੜੇ(&encoder));
	ਵਾਪਸ ਨਤੀਜਾ;
}

/* Everything that can fail before the erase, so a failure here leaves the
 * log untouched and still loadable. */
ਸਥਿਰ ਪੂਰਨ
stage_snapshot(bson_encoder_s *encoder, uint8_t **ਬਫਰ_2, size_t *enc_ਮਾਪ)
{
	ਪੂਰਨ ਨਤੀਜਾ = stage_write_ਬਫਰ(encoder, ਬਫਰ_2, enc_ਮਾਪ);

	ਜੇ (ਨਤੀਜਾ != OK) {
		ਵਾਪਸ ਨਤੀਜਾ;
	}

	ਜੇ (*enc_ਮਾਪ > parameter_flashfs_max_payload()) {
		parameter_flashfs_free();
		ਵਾਪਸ -ENOSPC;
	}

	ਵਾਪਸ OK;
}

ਸਥਿਰ ਪੂਰਨ
commit_snapshot(uint8_t *ਬਫਰ_2, size_t enc_ਮਾਪ)
{
	ਜੇ (parameter_flashfs_blank() != 1) {
		ਪੂਰਨ rv = parameter_flashfs_erase();

		ਜੇ (rv < 0) {
			parameter_flashfs_free();
			ਵਾਪਸ rv;
		}
	}

	ਪੂਰਨ ਨਤੀਜਾ = parameter_flashfs_write(parameters_token, ਬਫਰ_2, enc_ਮਾਪ);

	ਜੇ (ਨਤੀਜਾ != (ਪੂਰਨ)enc_ਮਾਪ && parameter_flashfs_blank() == 1) {
		ਨਤੀਜਾ = parameter_flashfs_write(parameters_token, ਬਫਰ_2, enc_ਮਾਪ);
	}

	parameter_flashfs_free();

	ਜੇ (ਨਤੀਜਾ == (ਪੂਰਨ)enc_ਮਾਪ) {
		ਵਾਪਸ OK;
	}

	ਵਾਪਸ ਨਤੀਜਾ < 0 ? ਨਤੀਜਾ : -EFBIG;
}

ਸਥਿਰ ਪੂਰਨ
compact_ਮੰਜ਼ਿਲ_snapshot()
{
	bson_encoder_s encoder{};
	ਪੂਰਨ ਨਤੀਜਾ = encode_document(&encoder, ਖਾਲੀ_ਸੂਚਕ, ਸੱਚ);

	ਜੇ (ਨਤੀਜਾ == 0) {
		uint8_t *ਬਫਰ_2;
		size_t enc_ਮਾਪ;
		ਨਤੀਜਾ = stage_snapshot(&encoder, &ਬਫਰ_2, &enc_ਮਾਪ);

		ਜੇ (ਨਤੀਜਾ == OK) {
			ਨਤੀਜਾ = commit_snapshot(ਬਫਰ_2, enc_ਮਾਪ);
		}
	}

	ਜੇ (ਨਤੀਜਾ == OK) {
		log_unreadable = ਝੂਠ;
	}

	ਮੁਕਤ(bson_encoder_buf_ਅੰਕੜੇ(&encoder));
	ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ ਤਾਰਕਿਕ
has_ਰਿਕਾਰਡ(flash_file_token_t token)
{
	uint8_t *ਬਫਰ_2 = ਖਾਲੀ_ਸੂਚਕ;
	size_t buf_ਮਾਪ = 0;
	ਵਾਪਸ parameter_flashfs_read(token, &ਬਫਰ_2, &buf_ਮਾਪ) >= 0;
}

ਸਥਿਰ ਪੂਰਨ
param_export_ਅੰਦਰੂਨੀ(param_filter_func filter)
{
	/* A delta only means something behind a snapshot that replays. A legacy
	 * record still present means the boot migration did not run; replay
	 * would take it over any delta appended behind it. */
	ਜੇ (log_unreadable || has_ਰਿਕਾਰਡ(parameters_legacy_token) || !has_ਰਿਕਾਰਡ(parameters_token)) {
		ਵਾਪਸ compact_ਮੰਜ਼ਿਲ_snapshot();
	}

	ਜੇ (!any_unsaved(filter)) {
		ਵਾਪਸ 0;
	}

	ਪੂਰਨ ਨਤੀਜਾ = export_delta(filter);

	ਜੇ (ਨਤੀਜਾ != -ENOSPC) {
		ਵਾਪਸ ਨਤੀਜਾ;
	}

	/* Same-bank erase stalls the CPU for ~1 s; last resort when a single
	 * save does not fit the tail. Compact writes the full RAM snapshot so a
	 * filtered save cannot drop params not in the delta. */
	PX4_WARN("flashparams: param sector full, compacting");
	ਵਾਪਸ compact_ਮੰਜ਼ਿਲ_snapshot();
}


ਸਥਿਰ ਤਾਰਕਿਕ
ਰਿਕਾਰਡ_is_stale(bson_ਗੰਢ_t ਗੰਢ_2)
{
	ਅਚਲ param_t param = param_ਲੱਭੋ_no_notification(ਗੰਢ_2->name);

	ਜੇ (param == PARAM_INVALID) {
		ਵਾਪਸ ਸੱਚ;
	}

	ਚੋਣ (ਗੰਢ_2->type) {
	ਹਾਲਤ BSON_INT32:
		ਵਾਪਸ param_type(param) != PARAM_TYPE_INT32;

	ਹਾਲਤ BSON_DOUBLE:
		ਵਾਪਸ param_type(param) != PARAM_TYPE_FLOAT;

	ਮੂਲ:
		ਵਾਪਸ ਝੂਠ;
	}
}

ਸਥਿਰ ਪੂਰਨ
param_import_callback(bson_decoder_t decoder, bson_ਗੰਢ_t ਗੰਢ_2)
{
	ਦਸ਼ਮਲਵ f;
	int32_t i;
	ਖਾਲੀ *v = ਖਾਲੀ_ਸੂਚਕ;
	ਪੂਰਨ ਨਤੀਜਾ = -1;

	/*
	 * EOO means the end of the parameter object. (Currently not supporting
	 * nested BSON objects).
	 */
	ਜੇ (ਗੰਢ_2->type == BSON_EOO) {
		debug("end of parameters");
		ਵਾਪਸ 0;
	}

	/* A tombstone has no value. The decoder leaves the previous node's in
	 * the union and the translations read it without a type check. */
	ਅਚਲ ਤਾਰਕਿਕ tombstone = ਗੰਢ_2->type == BSON_nullptr || ਗੰਢ_2->type == BSON_UNDEFINED;

	ਜੇ (!tombstone) {
		ਜੇ (ਰਿਕਾਰਡ_is_stale(ਗੰਢ_2)) {
			log_stale = ਸੱਚ;
		}

		ਜੇ (param_modify_on_import(ਗੰਢ_2) == param_modify_on_import_ret::PARAM_SKIP_IMPORT) {
			ਵਾਪਸ 1;
		}
	}

	/*
	 * Find the parameter this node represents.  If we don't know it,
	 * ignore the node.
	 */
	param_t param = param_ਲੱਭੋ_no_notification(ਗੰਢ_2->name);

	ਜੇ (param == PARAM_INVALID) {
		debug("ignoring unrecognised parameter '%s'", ਗੰਢ_2->name);
		ਵਾਪਸ 1;
	}

	/*
	 * Handle setting the parameter from the node
	 */

	ਚੋਣ (ਗੰਢ_2->type) {
	ਹਾਲਤ BSON_nullptr:
	ਹਾਲਤ BSON_UNDEFINED:
		user_config.reset(param);
		ਨਤੀਜਾ = 1;
		ਜਾਓ ਬਾਹਰਲਾ;

	ਹਾਲਤ BSON_INT32:
		ਜੇ (param_type(param) != PARAM_TYPE_INT32) {
			PX4_WARN("unexpected type for %s", ਗੰਢ_2->name);
			ਨਤੀਜਾ = 1; // just skip this entry
			ਜਾਓ ਬਾਹਰਲਾ;
		}

		i = ਗੰਢ_2->i32;
		v = &i;
		ਰੋਕੋ;

	ਹਾਲਤ BSON_DOUBLE:
		ਜੇ (param_type(param) != PARAM_TYPE_FLOAT) {
			PX4_WARN("unexpected type for %s", ਗੰਢ_2->name);
			ਨਤੀਜਾ = 1; // just skip this entry
			ਜਾਓ ਬਾਹਰਲਾ;
		}

		f = ਗੰਢ_2->d;
		v = &f;
		ਰੋਕੋ;

	ਮੂਲ:
		PX4_ERR("%s unrecognised node type %d", ਗੰਢ_2->name, ਗੰਢ_2->type);
		ਨਤੀਜਾ = 1; // just skip this entry
		ਜਾਓ ਬਾਹਰਲਾ;
	}

	ਜੇ (param_ਸੈੱਟ_external(param, v, ਸੱਚ, ਸੱਚ)) {
		debug("error setting value for '%s'", ਗੰਢ_2->name);
		ਜਾਓ ਬਾਹਰਲਾ;
	}

	/* don't return zero, that means EOF */
	ਨਤੀਜਾ = 1;

ਬਾਹਰਲਾ:
	ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ ਪੂਰਨ
import_one_ਇੰਦਰਾਜ(uint8_t *ਬਫਰ_2, size_t buf_ਮਾਪ, ਖਾਲੀ *arg)
{
	ਪੂਰਨ *ਨਤੀਜਾ = static_cast<ਪੂਰਨ *>(arg);
	bson_decoder_s decoder{};

	ਜੇ (bson_decoder_init_buf(&decoder, ਬਫਰ_2, buf_ਮਾਪ, param_import_callback)) {
		debug("decoder init failed");
		*ਨਤੀਜਾ = -1;
		ਵਾਪਸ -1;
	}

	ਕਰੋ {
		*ਨਤੀਜਾ = bson_decoder_ਅਗਲਾ(&decoder);

	} ਜਦੋਂ (*ਨਤੀਜਾ > 0);

	ਵਾਪਸ *ਨਤੀਜਾ < 0 ? *ਨਤੀਜਾ : 0;
}

ਸਥਿਰ ਪੂਰਨ
param_import_ਅੰਦਰੂਨੀ(ਤਾਰਕਿਕ *legacy)
{
	ਪੂਰਨ ਨਤੀਜਾ = 0;
	log_stale = ਝੂਠ;
	ਪੂਰਨ n = parameter_flashfs_walk(parameters_legacy_token, import_one_ਇੰਦਰਾਜ, &ਨਤੀਜਾ);
	*legacy = n > 0;

	ਜੇ (n == 0) {
		n = parameter_flashfs_walk(parameters_token, import_one_ਇੰਦਰਾਜ, &ਨਤੀਜਾ);
	}

	ਜੇ (n < 0) {
		debug("flash walk failed (%d)", n);
		log_unreadable = ਸੱਚ;
		ਵਾਪਸ n;
	}

	ਜੇ (n == 0) {
		/* No valid entry found. A fully erased store is blank (first boot, or
		 * after switching firmware): report "not yet stored" (1), matching the
		 * convention used by param_load_default(). A store that holds data but
		 * no valid entry (torn write, bit-rot, or foreign data) is corrupt and
		 * must not be silently reseeded - report it so the boot recovery runs. */
		ਜੇ (parameter_flashfs_blank() == 1) {
			ਵਾਪਸ 1;
		}

		debug("flash holds data but no valid entry");
		ਵਾਪਸ -EILSEQ;
	}

	ਜੇ (ਨਤੀਜਾ < 0) {
		debug("BSON error decoding parameters");
		ਵਾਪਸ ਨਤੀਜਾ;
	}

	ਵਾਪਸ 0;
}

/*
 * Same-bank erase stalls instruction fetch for ~1 s on H7. Do it here at
 * boot, before sensors or DShot start, and only when a burst of deltas would
 * no longer fit, the store is still in the legacy layout, or the log holds
 * records this firmware translated or could not use. Only a failure after
 * the erase is an error: until then the log is intact and RAM holds what it
 * said, so the boot must not be reported as a corrupt store.
 */
ਸਥਿਰ ਪੂਰਨ
compact_after_import(ਤਾਰਕਿਕ legacy)
{
	bson_encoder_s encoder{};
	ਪੂਰਨ ਨਤੀਜਾ = encode_document(&encoder, ਖਾਲੀ_ਸੂਚਕ, ਸੱਚ);

	ਜੇ (ਨਤੀਜਾ != 0) {
		PX4_ERR("flashparams: snapshot encode failed (%d), log kept", ਨਤੀਜਾ);
		ਮੁਕਤ(bson_encoder_buf_ਅੰਕੜੇ(&encoder));
		ਵਾਪਸ 0;
	}

	ਅਚਲ size_t enc_ਮਾਪ = bson_encoder_buf_ਮਾਪ(&encoder);
	ਪੂਰਨ need = (legacy || log_stale) ? 1 : parameter_flashfs_needs_compact(parameters_token, enc_ਮਾਪ);
	ਨਤੀਜਾ = 0;

	ਜੇ (need > 0) {
		uint8_t *ਬਫਰ_2;
		size_t staged_ਮਾਪ;
		ਨਤੀਜਾ = stage_snapshot(&encoder, &ਬਫਰ_2, &staged_ਮਾਪ);

		ਜੇ (ਨਤੀਜਾ != OK) {
			PX4_ERR("flashparams: cannot stage snapshot (%d), log kept", ਨਤੀਜਾ);
			ਨਤੀਜਾ = 0;

		} ਨਹੀਂਤਾਂ {
			PX4_INFO("flashparams: compacting param sector%s", legacy ? " (legacy layout)" : "");
			ਨਤੀਜਾ = commit_snapshot(ਬਫਰ_2, staged_ਮਾਪ);

			ਜੇ (ਨਤੀਜਾ != OK) {
				PX4_ERR("flashparams: compact failed (%d)", ਨਤੀਜਾ);
			}
		}
	}

	ਮੁਕਤ(bson_encoder_buf_ਅੰਕੜੇ(&encoder));
	ਵਾਪਸ ਨਤੀਜਾ;
}

ਪੂਰਨ flash_param_save(param_filter_func filter)
{
	ਵਾਪਸ param_export_ਅੰਦਰੂਨੀ(filter);
}

ਪੂਰਨ flash_param_import()
{
	ਤਾਰਕਿਕ legacy = ਝੂਠ;
	ਪੂਰਨ ਨਤੀਜਾ = param_import_ਅੰਦਰੂਨੀ(&legacy);

	ਜੇ (ਨਤੀਜਾ != 0) {
		ਵਾਪਸ ਨਤੀਜਾ;
	}

	ਵਾਪਸ compact_after_import(legacy);
}
