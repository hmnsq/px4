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

#అన్తర్భావ <px4_platform_common/defines.h>
#అన్తర్భావ <px4_platform_common/log.h>
#అన్తర్భావ <px4_platform_common/posix.h>
#అన్తర్భావ <px4_platform_common/shutdown.h>
#అన్తర్భావ <parameters/px4_parameters.hpp>

#అన్తర్భావ <string.h>
#అన్తర్భావ <stdbool.h>
#అన్తర్భావ <stdint.h>
#అన్తర్భావ <errno.h>

#అన్తర్భావ <parameters/param.h>

#అన్తర్భావ <lib/tinybson/tinybson.h>
#అన్తర్భావ "flashparams.h"
#అన్తర్భావ "flashfs.h"
#అన్తర్భావ "../param_translation.h"

#ఒకవేళ 0
# define debug(fmt, వాదనలు...)            చేయి { warnx(fmt, ##args); } అయితే(0)
#లేకపోతే
# define debug(fmt, వాదనలు...)            చేయి { } అయితే(0)
#అయితే_ముగింపు


/**
 * Storage for modified parameters.
 */
నిర్మాణం param_wbuf_s {
	సంఘం param_విలువ_u     val;
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
స్థిర తార్కిక log_stale;

/* Replay failed inside a CRC-valid record. A delta appended behind it would
 * never be reached, so the next save rewrites the store. */
స్థిర తార్కిక log_unreadable;

స్థిర తార్కిక
any_unsaved(param_filter_func filter)
{
	కొరకు (param_t param = 0; param < user_config.PARAM_లెక్క; param++) {
		ఒకవేళ ((filter == శూన్యసూచిక || filter(param)) && param_విలువ_unsaved(param)) {
			తిరిగి నిజం;
		}
	}

	తిరిగి అబద్ధం;
}

/*
 * A snapshot holds every override in RAM, equal to the current default or
 * not: the boot compaction runs before the airframe applies its set-default
 * values, so at that point "equals the default" says nothing. A delta holds
 * the unsaved params; one that no longer overrides, or now matches the
 * default in force, is written as a BSON null so replay drops it, which is
 * what the file backend's "don't export defaults" gives after a reload.
 */
స్థిర పూర్ణాంకం
encode_params(bson_encoder_s *encoder, param_filter_func filter, తార్కిక snapshot)
{
	కొరకు (param_t param = 0; param < user_config.PARAM_లెక్క; param++) {
		ఒకవేళ (filter && !filter(param)) {
			కొనసాగు;
		}

		ఒకవేళ (snapshot) {
			ఒకవేళ (!user_config.కలిగియుంది(param)) {
				కొనసాగు;
			}

		} లేకపోతే {
			ఒకవేళ (!param_విలువ_unsaved(param)) {
				కొనసాగు;
			}

			ఒకవేళ (!user_config.కలిగియుంది(param) || param_విలువ_is_default(param)) {
				ఒకవేళ (bson_encoder_append_null(encoder, param_name(param))) {
					debug("BSON append failed for '%s'", param_name(param));
					తిరిగి -1;
				}

				కొనసాగు;
			}
		}

		ఎంపిక (param_type(param)) {
		స్థితి PARAM_TYPE_INT32:
			ఒకవేళ (bson_encoder_append_int32(encoder, param_name(param), user_config.get(param).i)) {
				debug("BSON append failed for '%s'", param_name(param));
				తిరిగి -1;
			}

			ఆపు;

		స్థితి PARAM_TYPE_FLOAT:
			ఒకవేళ (bson_encoder_append_double(encoder, param_name(param), (ద్విగుణ)user_config.get(param).f)) {
				debug("BSON append failed for '%s'", param_name(param));
				తిరిగి -1;
			}

			ఆపు;

		అప్రమేయ:
			debug("unrecognized parameter type");
			తిరిగి -1;
		}
	}

	తిరిగి 0;
}

/* The caller frees bson_encoder_buf_data() whatever the result. */
స్థిర పూర్ణాంకం
encode_document(bson_encoder_s *encoder, param_filter_func filter, తార్కిక snapshot)
{
	bson_encoder_init_buf(encoder, శూన్యసూచిక, 0);

	పూర్ణాంకం ఫలితం = encode_params(encoder, filter, snapshot);

	ఒకవేళ (ఫలితం == 0) {
		ఫలితం = bson_encoder_fini(encoder);
	}

	తిరిగి ఫలితం;
}

స్థిర పూర్ణాంకం
stage_write_బఫరు(bson_encoder_s *encoder, uint8_t **బఫరు_2, size_t *enc_కొలత)
{
	శూన్య *enc_buff = bson_encoder_buf_దత్తాంశం(encoder);
	*enc_కొలత = bson_encoder_buf_కొలత(encoder);

	ఒకవేళ (enc_buff == శూన్యసూచిక) {
		తిరిగి -ENOMEM;
	}

	size_t alloc_కొలత = *enc_కొలత;
	పూర్ణాంకం ఫలితం = parameter_flashfs_alloc(parameters_token, బఫరు_2, &alloc_కొలత);

	ఒకవేళ (ఫలితం != OK) {
		తిరిగి ఫలితం;
	}

	ఒకవేళ (alloc_కొలత < *enc_కొలత) {
		parameter_flashfs_free();
		తిరిగి -ENOMEM;
	}

	జ్ఞాపకం_నకలు(*బఫరు_2, enc_buff, *enc_కొలత);
	తిరిగి OK;
}

స్థిర పూర్ణాంకం
commit_append(uint8_t *బఫరు_2, size_t enc_కొలత)
{
	పూర్ణాంకం ఫలితం = parameter_flashfs_write(parameters_token, బఫరు_2, enc_కొలత);
	parameter_flashfs_free();

	ఒకవేళ (ఫలితం == (పూర్ణాంకం)enc_కొలత) {
		తిరిగి OK;
	}

	తిరిగి ఫలితం < 0 ? ఫలితం : -EFBIG;
}

స్థిర పూర్ణాంకం
export_delta(param_filter_func filter)
{
	bson_encoder_s encoder{};
	పూర్ణాంకం ఫలితం = encode_document(&encoder, filter, అబద్ధం);

	ఒకవేళ (ఫలితం == 0) {
		uint8_t *బఫరు_2;
		size_t enc_కొలత;
		ఫలితం = stage_write_బఫరు(&encoder, &బఫరు_2, &enc_కొలత);

		ఒకవేళ (ఫలితం == OK) {
			ఫలితం = commit_append(బఫరు_2, enc_కొలత);
		}
	}

	విడుదల(bson_encoder_buf_దత్తాంశం(&encoder));
	తిరిగి ఫలితం;
}

/* Everything that can fail before the erase, so a failure here leaves the
 * log untouched and still loadable. */
స్థిర పూర్ణాంకం
stage_snapshot(bson_encoder_s *encoder, uint8_t **బఫరు_2, size_t *enc_కొలత)
{
	పూర్ణాంకం ఫలితం = stage_write_బఫరు(encoder, బఫరు_2, enc_కొలత);

	ఒకవేళ (ఫలితం != OK) {
		తిరిగి ఫలితం;
	}

	ఒకవేళ (*enc_కొలత > parameter_flashfs_max_payload()) {
		parameter_flashfs_free();
		తిరిగి -ENOSPC;
	}

	తిరిగి OK;
}

స్థిర పూర్ణాంకం
commit_snapshot(uint8_t *బఫరు_2, size_t enc_కొలత)
{
	ఒకవేళ (parameter_flashfs_blank() != 1) {
		పూర్ణాంకం rv = parameter_flashfs_erase();

		ఒకవేళ (rv < 0) {
			parameter_flashfs_free();
			తిరిగి rv;
		}
	}

	పూర్ణాంకం ఫలితం = parameter_flashfs_write(parameters_token, బఫరు_2, enc_కొలత);

	ఒకవేళ (ఫలితం != (పూర్ణాంకం)enc_కొలత && parameter_flashfs_blank() == 1) {
		ఫలితం = parameter_flashfs_write(parameters_token, బఫరు_2, enc_కొలత);
	}

	parameter_flashfs_free();

	ఒకవేళ (ఫలితం == (పూర్ణాంకం)enc_కొలత) {
		తిరిగి OK;
	}

	తిరిగి ఫలితం < 0 ? ఫలితం : -EFBIG;
}

స్థిర పూర్ణాంకం
compact_గమ్యం_snapshot()
{
	bson_encoder_s encoder{};
	పూర్ణాంకం ఫలితం = encode_document(&encoder, శూన్యసూచిక, నిజం);

	ఒకవేళ (ఫలితం == 0) {
		uint8_t *బఫరు_2;
		size_t enc_కొలత;
		ఫలితం = stage_snapshot(&encoder, &బఫరు_2, &enc_కొలత);

		ఒకవేళ (ఫలితం == OK) {
			ఫలితం = commit_snapshot(బఫరు_2, enc_కొలత);
		}
	}

	ఒకవేళ (ఫలితం == OK) {
		log_unreadable = అబద్ధం;
	}

	విడుదల(bson_encoder_buf_దత్తాంశం(&encoder));
	తిరిగి ఫలితం;
}

స్థిర తార్కిక
has_రికార్డు(flash_file_token_t token)
{
	uint8_t *బఫరు_2 = శూన్యసూచిక;
	size_t buf_కొలత = 0;
	తిరిగి parameter_flashfs_read(token, &బఫరు_2, &buf_కొలత) >= 0;
}

స్థిర పూర్ణాంకం
param_export_అంతర్గత(param_filter_func filter)
{
	/* A delta only means something behind a snapshot that replays. A legacy
	 * record still present means the boot migration did not run; replay
	 * would take it over any delta appended behind it. */
	ఒకవేళ (log_unreadable || has_రికార్డు(parameters_legacy_token) || !has_రికార్డు(parameters_token)) {
		తిరిగి compact_గమ్యం_snapshot();
	}

	ఒకవేళ (!any_unsaved(filter)) {
		తిరిగి 0;
	}

	పూర్ణాంకం ఫలితం = export_delta(filter);

	ఒకవేళ (ఫలితం != -ENOSPC) {
		తిరిగి ఫలితం;
	}

	/* Same-bank erase stalls the CPU for ~1 s; last resort when a single
	 * save does not fit the tail. Compact writes the full RAM snapshot so a
	 * filtered save cannot drop params not in the delta. */
	PX4_WARN("flashparams: param sector full, compacting");
	తిరిగి compact_గమ్యం_snapshot();
}


స్థిర తార్కిక
రికార్డు_is_stale(bson_కణుపు_t కణుపు_2)
{
	స్థిరరాశి param_t param = param_కనుగొను_no_notification(కణుపు_2->name);

	ఒకవేళ (param == PARAM_INVALID) {
		తిరిగి నిజం;
	}

	ఎంపిక (కణుపు_2->type) {
	స్థితి BSON_INT32:
		తిరిగి param_type(param) != PARAM_TYPE_INT32;

	స్థితి BSON_DOUBLE:
		తిరిగి param_type(param) != PARAM_TYPE_FLOAT;

	అప్రమేయ:
		తిరిగి అబద్ధం;
	}
}

స్థిర పూర్ణాంకం
param_import_callback(bson_decoder_t decoder, bson_కణుపు_t కణుపు_2)
{
	దశాంశం f;
	int32_t i;
	శూన్య *v = శూన్యసూచిక;
	పూర్ణాంకం ఫలితం = -1;

	/*
	 * EOO means the end of the parameter object. (Currently not supporting
	 * nested BSON objects).
	 */
	ఒకవేళ (కణుపు_2->type == BSON_EOO) {
		debug("end of parameters");
		తిరిగి 0;
	}

	/* A tombstone has no value. The decoder leaves the previous node's in
	 * the union and the translations read it without a type check. */
	స్థిరరాశి తార్కిక tombstone = కణుపు_2->type == BSON_nullptr || కణుపు_2->type == BSON_UNDEFINED;

	ఒకవేళ (!tombstone) {
		ఒకవేళ (రికార్డు_is_stale(కణుపు_2)) {
			log_stale = నిజం;
		}

		ఒకవేళ (param_modify_on_import(కణుపు_2) == param_modify_on_import_ret::PARAM_SKIP_IMPORT) {
			తిరిగి 1;
		}
	}

	/*
	 * Find the parameter this node represents.  If we don't know it,
	 * ignore the node.
	 */
	param_t param = param_కనుగొను_no_notification(కణుపు_2->name);

	ఒకవేళ (param == PARAM_INVALID) {
		debug("ignoring unrecognised parameter '%s'", కణుపు_2->name);
		తిరిగి 1;
	}

	/*
	 * Handle setting the parameter from the node
	 */

	ఎంపిక (కణుపు_2->type) {
	స్థితి BSON_nullptr:
	స్థితి BSON_UNDEFINED:
		user_config.reset(param);
		ఫలితం = 1;
		వెళ్ళు బహిర్గతం;

	స్థితి BSON_INT32:
		ఒకవేళ (param_type(param) != PARAM_TYPE_INT32) {
			PX4_WARN("unexpected type for %s", కణుపు_2->name);
			ఫలితం = 1; // just skip this entry
			వెళ్ళు బహిర్గతం;
		}

		i = కణుపు_2->i32;
		v = &i;
		ఆపు;

	స్థితి BSON_DOUBLE:
		ఒకవేళ (param_type(param) != PARAM_TYPE_FLOAT) {
			PX4_WARN("unexpected type for %s", కణుపు_2->name);
			ఫలితం = 1; // just skip this entry
			వెళ్ళు బహిర్గతం;
		}

		f = కణుపు_2->d;
		v = &f;
		ఆపు;

	అప్రమేయ:
		PX4_ERR("%s unrecognised node type %d", కణుపు_2->name, కణుపు_2->type);
		ఫలితం = 1; // just skip this entry
		వెళ్ళు బహిర్గతం;
	}

	ఒకవేళ (param_సమితి_external(param, v, నిజం, నిజం)) {
		debug("error setting value for '%s'", కణుపు_2->name);
		వెళ్ళు బహిర్గతం;
	}

	/* don't return zero, that means EOF */
	ఫలితం = 1;

బహిర్గతం:
	తిరిగి ఫలితం;
}

స్థిర పూర్ణాంకం
import_one_నమోదు(uint8_t *బఫరు_2, size_t buf_కొలత, శూన్య *arg)
{
	పూర్ణాంకం *ఫలితం = static_cast<పూర్ణాంకం *>(arg);
	bson_decoder_s decoder{};

	ఒకవేళ (bson_decoder_init_buf(&decoder, బఫరు_2, buf_కొలత, param_import_callback)) {
		debug("decoder init failed");
		*ఫలితం = -1;
		తిరిగి -1;
	}

	చేయి {
		*ఫలితం = bson_decoder_తదుపరి(&decoder);

	} అయితే (*ఫలితం > 0);

	తిరిగి *ఫలితం < 0 ? *ఫలితం : 0;
}

స్థిర పూర్ణాంకం
param_import_అంతర్గత(తార్కిక *legacy)
{
	పూర్ణాంకం ఫలితం = 0;
	log_stale = అబద్ధం;
	పూర్ణాంకం n = parameter_flashfs_walk(parameters_legacy_token, import_one_నమోదు, &ఫలితం);
	*legacy = n > 0;

	ఒకవేళ (n == 0) {
		n = parameter_flashfs_walk(parameters_token, import_one_నమోదు, &ఫలితం);
	}

	ఒకవేళ (n < 0) {
		debug("flash walk failed (%d)", n);
		log_unreadable = నిజం;
		తిరిగి n;
	}

	ఒకవేళ (n == 0) {
		/* No valid entry found. A fully erased store is blank (first boot, or
		 * after switching firmware): report "not yet stored" (1), matching the
		 * convention used by param_load_default(). A store that holds data but
		 * no valid entry (torn write, bit-rot, or foreign data) is corrupt and
		 * must not be silently reseeded - report it so the boot recovery runs. */
		ఒకవేళ (parameter_flashfs_blank() == 1) {
			తిరిగి 1;
		}

		debug("flash holds data but no valid entry");
		తిరిగి -EILSEQ;
	}

	ఒకవేళ (ఫలితం < 0) {
		debug("BSON error decoding parameters");
		తిరిగి ఫలితం;
	}

	తిరిగి 0;
}

/*
 * Same-bank erase stalls instruction fetch for ~1 s on H7. Do it here at
 * boot, before sensors or DShot start, and only when a burst of deltas would
 * no longer fit, the store is still in the legacy layout, or the log holds
 * records this firmware translated or could not use. Only a failure after
 * the erase is an error: until then the log is intact and RAM holds what it
 * said, so the boot must not be reported as a corrupt store.
 */
స్థిర పూర్ణాంకం
compact_after_import(తార్కిక legacy)
{
	bson_encoder_s encoder{};
	పూర్ణాంకం ఫలితం = encode_document(&encoder, శూన్యసూచిక, నిజం);

	ఒకవేళ (ఫలితం != 0) {
		PX4_ERR("flashparams: snapshot encode failed (%d), log kept", ఫలితం);
		విడుదల(bson_encoder_buf_దత్తాంశం(&encoder));
		తిరిగి 0;
	}

	స్థిరరాశి size_t enc_కొలత = bson_encoder_buf_కొలత(&encoder);
	పూర్ణాంకం need = (legacy || log_stale) ? 1 : parameter_flashfs_needs_compact(parameters_token, enc_కొలత);
	ఫలితం = 0;

	ఒకవేళ (need > 0) {
		uint8_t *బఫరు_2;
		size_t staged_కొలత;
		ఫలితం = stage_snapshot(&encoder, &బఫరు_2, &staged_కొలత);

		ఒకవేళ (ఫలితం != OK) {
			PX4_ERR("flashparams: cannot stage snapshot (%d), log kept", ఫలితం);
			ఫలితం = 0;

		} లేకపోతే {
			PX4_INFO("flashparams: compacting param sector%s", legacy ? " (legacy layout)" : "");
			ఫలితం = commit_snapshot(బఫరు_2, staged_కొలత);

			ఒకవేళ (ఫలితం != OK) {
				PX4_ERR("flashparams: compact failed (%d)", ఫలితం);
			}
		}
	}

	విడుదల(bson_encoder_buf_దత్తాంశం(&encoder));
	తిరిగి ఫలితం;
}

పూర్ణాంకం flash_param_save(param_filter_func filter)
{
	తిరిగి param_export_అంతర్గత(filter);
}

పూర్ణాంకం flash_param_import()
{
	తార్కిక legacy = అబద్ధం;
	పూర్ణాంకం ఫలితం = param_import_అంతర్గత(&legacy);

	ఒకవేళ (ఫలితం != 0) {
		తిరిగి ఫలితం;
	}

	తిరిగి compact_after_import(legacy);
}
