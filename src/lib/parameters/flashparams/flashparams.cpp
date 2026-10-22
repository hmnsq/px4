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

#অন্তর্ভাব <px4_platform_common/defines.h>
#অন্তর্ভাব <px4_platform_common/log.h>
#অন্তর্ভাব <px4_platform_common/posix.h>
#অন্তর্ভাব <px4_platform_common/shutdown.h>
#অন্তর্ভাব <parameters/px4_parameters.hpp>

#অন্তর্ভাব <string.h>
#অন্তর্ভাব <stdbool.h>
#অন্তর্ভাব <stdint.h>
#অন্তর্ভাব <errno.h>

#অন্তর্ভাব <parameters/param.h>

#অন্তর্ভাব <lib/tinybson/tinybson.h>
#অন্তর্ভাব "flashparams.h"
#অন্তর্ভাব "flashfs.h"
#অন্তর্ভাব "../param_translation.h"

#যদি 0
# define debug(fmt, args...)            করো { warnx(fmt, ##args); } যখন(0)
#অন্যথা
# define debug(fmt, args...)            করো { } যখন(0)
#endif


/**
 * Storage for modified parameters.
 */
কাঠামো param_wbuf_s {
	সংঘ param_মান_u     val;
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
স্থির যৌক্তিক log_stale;

/* Replay failed inside a CRC-valid record. A delta appended behind it would
 * never be reached, so the next save rewrites the store. */
স্থির যৌক্তিক log_unreadable;

স্থির যৌক্তিক
any_unsaved(param_filter_func filter)
{
	জন্য (param_t param = 0; param < user_config.PARAM_গুনতি; param++) {
		যদি ((filter == nullptr || filter(param)) && param_মান_unsaved(param)) {
			ফেরত সত্য;
		}
	}

	ফেরত মিথ্যা;
}

/*
 * A snapshot holds every override in RAM, equal to the current default or
 * not: the boot compaction runs before the airframe applies its set-default
 * values, so at that point "equals the default" says nothing. A delta holds
 * the unsaved params; one that no longer overrides, or now matches the
 * default in force, is written as a BSON null so replay drops it, which is
 * what the file backend's "don't export defaults" gives after a reload.
 */
স্থির পূর্ণসংখ্যা
encode_params(bson_encoder_s *encoder, param_filter_func filter, যৌক্তিক snapshot)
{
	জন্য (param_t param = 0; param < user_config.PARAM_গুনতি; param++) {
		যদি (filter && !filter(param)) {
			চালু;
		}

		যদি (snapshot) {
			যদি (!user_config.contains(param)) {
				চালু;
			}

		} অন্যথা {
			যদি (!param_মান_unsaved(param)) {
				চালু;
			}

			যদি (!user_config.contains(param) || param_মান_is_default(param)) {
				যদি (bson_encoder_append_null(encoder, param_name(param))) {
					debug("BSON append failed for '%s'", param_name(param));
					ফেরত -1;
				}

				চালু;
			}
		}

		নির্বাচন (param_type(param)) {
		ক্ষেত্র PARAM_TYPE_INT32:
			যদি (bson_encoder_append_int32(encoder, param_name(param), user_config.get(param).i)) {
				debug("BSON append failed for '%s'", param_name(param));
				ফেরত -1;
			}

			বিরতি;

		ক্ষেত্র PARAM_TYPE_FLOAT:
			যদি (bson_encoder_append_double(encoder, param_name(param), (দ্বিগুণ)user_config.get(param).f)) {
				debug("BSON append failed for '%s'", param_name(param));
				ফেরত -1;
			}

			বিরতি;

		পূর্বনির্ধারিত:
			debug("unrecognized parameter type");
			ফেরত -1;
		}
	}

	ফেরত 0;
}

/* The caller frees bson_encoder_buf_data() whatever the result. */
স্থির পূর্ণসংখ্যা
encode_document(bson_encoder_s *encoder, param_filter_func filter, যৌক্তিক snapshot)
{
	bson_encoder_init_buf(encoder, nullptr, 0);

	পূর্ণসংখ্যা ফলাফল = encode_params(encoder, filter, snapshot);

	যদি (ফলাফল == 0) {
		ফলাফল = bson_encoder_fini(encoder);
	}

	ফেরত ফলাফল;
}

স্থির পূর্ণসংখ্যা
stage_write_বাফার(bson_encoder_s *encoder, uint8_t **বাফার_2, size_t *enc_আকার)
{
	শূন্য *enc_buff = bson_encoder_buf_তথ্য(encoder);
	*enc_আকার = bson_encoder_buf_আকার(encoder);

	যদি (enc_buff == nullptr) {
		ফেরত -ENOMEM;
	}

	size_t alloc_আকার = *enc_আকার;
	পূর্ণসংখ্যা ফলাফল = parameter_flashfs_alloc(parameters_token, বাফার_2, &alloc_আকার);

	যদি (ফলাফল != OK) {
		ফেরত ফলাফল;
	}

	যদি (alloc_আকার < *enc_আকার) {
		parameter_flashfs_free();
		ফেরত -ENOMEM;
	}

	memcpy(*বাফার_2, enc_buff, *enc_আকার);
	ফেরত OK;
}

স্থির পূর্ণসংখ্যা
commit_append(uint8_t *বাফার_2, size_t enc_আকার)
{
	পূর্ণসংখ্যা ফলাফল = parameter_flashfs_write(parameters_token, বাফার_2, enc_আকার);
	parameter_flashfs_free();

	যদি (ফলাফল == (পূর্ণসংখ্যা)enc_আকার) {
		ফেরত OK;
	}

	ফেরত ফলাফল < 0 ? ফলাফল : -EFBIG;
}

স্থির পূর্ণসংখ্যা
export_delta(param_filter_func filter)
{
	bson_encoder_s encoder{};
	পূর্ণসংখ্যা ফলাফল = encode_document(&encoder, filter, মিথ্যা);

	যদি (ফলাফল == 0) {
		uint8_t *বাফার_2;
		size_t enc_আকার;
		ফলাফল = stage_write_বাফার(&encoder, &বাফার_2, &enc_আকার);

		যদি (ফলাফল == OK) {
			ফলাফল = commit_append(বাফার_2, enc_আকার);
		}
	}

	মুক্ত(bson_encoder_buf_তথ্য(&encoder));
	ফেরত ফলাফল;
}

/* Everything that can fail before the erase, so a failure here leaves the
 * log untouched and still loadable. */
স্থির পূর্ণসংখ্যা
stage_snapshot(bson_encoder_s *encoder, uint8_t **বাফার_2, size_t *enc_আকার)
{
	পূর্ণসংখ্যা ফলাফল = stage_write_বাফার(encoder, বাফার_2, enc_আকার);

	যদি (ফলাফল != OK) {
		ফেরত ফলাফল;
	}

	যদি (*enc_আকার > parameter_flashfs_max_payload()) {
		parameter_flashfs_free();
		ফেরত -ENOSPC;
	}

	ফেরত OK;
}

স্থির পূর্ণসংখ্যা
commit_snapshot(uint8_t *বাফার_2, size_t enc_আকার)
{
	যদি (parameter_flashfs_blank() != 1) {
		পূর্ণসংখ্যা rv = parameter_flashfs_erase();

		যদি (rv < 0) {
			parameter_flashfs_free();
			ফেরত rv;
		}
	}

	পূর্ণসংখ্যা ফলাফল = parameter_flashfs_write(parameters_token, বাফার_2, enc_আকার);

	যদি (ফলাফল != (পূর্ণসংখ্যা)enc_আকার && parameter_flashfs_blank() == 1) {
		ফলাফল = parameter_flashfs_write(parameters_token, বাফার_2, enc_আকার);
	}

	parameter_flashfs_free();

	যদি (ফলাফল == (পূর্ণসংখ্যা)enc_আকার) {
		ফেরত OK;
	}

	ফেরত ফলাফল < 0 ? ফলাফল : -EFBIG;
}

স্থির পূর্ণসংখ্যা
compact_to_snapshot()
{
	bson_encoder_s encoder{};
	পূর্ণসংখ্যা ফলাফল = encode_document(&encoder, nullptr, সত্য);

	যদি (ফলাফল == 0) {
		uint8_t *বাফার_2;
		size_t enc_আকার;
		ফলাফল = stage_snapshot(&encoder, &বাফার_2, &enc_আকার);

		যদি (ফলাফল == OK) {
			ফলাফল = commit_snapshot(বাফার_2, enc_আকার);
		}
	}

	যদি (ফলাফল == OK) {
		log_unreadable = মিথ্যা;
	}

	মুক্ত(bson_encoder_buf_তথ্য(&encoder));
	ফেরত ফলাফল;
}

স্থির যৌক্তিক
has_record(flash_file_token_t token)
{
	uint8_t *বাফার_2 = nullptr;
	size_t buf_আকার = 0;
	ফেরত parameter_flashfs_read(token, &বাফার_2, &buf_আকার) >= 0;
}

স্থির পূর্ণসংখ্যা
param_export_internal(param_filter_func filter)
{
	/* A delta only means something behind a snapshot that replays. A legacy
	 * record still present means the boot migration did not run; replay
	 * would take it over any delta appended behind it. */
	যদি (log_unreadable || has_record(parameters_legacy_token) || !has_record(parameters_token)) {
		ফেরত compact_to_snapshot();
	}

	যদি (!any_unsaved(filter)) {
		ফেরত 0;
	}

	পূর্ণসংখ্যা ফলাফল = export_delta(filter);

	যদি (ফলাফল != -ENOSPC) {
		ফেরত ফলাফল;
	}

	/* Same-bank erase stalls the CPU for ~1 s; last resort when a single
	 * save does not fit the tail. Compact writes the full RAM snapshot so a
	 * filtered save cannot drop params not in the delta. */
	PX4_WARN("flashparams: param sector full, compacting");
	ফেরত compact_to_snapshot();
}


স্থির যৌক্তিক
record_is_stale(bson_গ্রন্থি_t গ্রন্থি_2)
{
	ধ্রুবক param_t param = param_find_no_notification(গ্রন্থি_2->name);

	যদি (param == PARAM_INVALID) {
		ফেরত সত্য;
	}

	নির্বাচন (গ্রন্থি_2->type) {
	ক্ষেত্র BSON_INT32:
		ফেরত param_type(param) != PARAM_TYPE_INT32;

	ক্ষেত্র BSON_DOUBLE:
		ফেরত param_type(param) != PARAM_TYPE_FLOAT;

	পূর্বনির্ধারিত:
		ফেরত মিথ্যা;
	}
}

স্থির পূর্ণসংখ্যা
param_import_callback(bson_decoder_t decoder, bson_গ্রন্থি_t গ্রন্থি_2)
{
	দশমিক f;
	int32_t i;
	শূন্য *v = nullptr;
	পূর্ণসংখ্যা ফলাফল = -1;

	/*
	 * EOO means the end of the parameter object. (Currently not supporting
	 * nested BSON objects).
	 */
	যদি (গ্রন্থি_2->type == BSON_EOO) {
		debug("end of parameters");
		ফেরত 0;
	}

	/* A tombstone has no value. The decoder leaves the previous node's in
	 * the union and the translations read it without a type check. */
	ধ্রুবক যৌক্তিক tombstone = গ্রন্থি_2->type == BSON_nullptr || গ্রন্থি_2->type == BSON_UNDEFINED;

	যদি (!tombstone) {
		যদি (record_is_stale(গ্রন্থি_2)) {
			log_stale = সত্য;
		}

		যদি (param_modify_on_import(গ্রন্থি_2) == param_modify_on_import_ret::PARAM_SKIP_IMPORT) {
			ফেরত 1;
		}
	}

	/*
	 * Find the parameter this node represents.  If we don't know it,
	 * ignore the node.
	 */
	param_t param = param_find_no_notification(গ্রন্থি_2->name);

	যদি (param == PARAM_INVALID) {
		debug("ignoring unrecognised parameter '%s'", গ্রন্থি_2->name);
		ফেরত 1;
	}

	/*
	 * Handle setting the parameter from the node
	 */

	নির্বাচন (গ্রন্থি_2->type) {
	ক্ষেত্র BSON_nullptr:
	ক্ষেত্র BSON_UNDEFINED:
		user_config.reset(param);
		ফলাফল = 1;
		যাও out;

	ক্ষেত্র BSON_INT32:
		যদি (param_type(param) != PARAM_TYPE_INT32) {
			PX4_WARN("unexpected type for %s", গ্রন্থি_2->name);
			ফলাফল = 1; // just skip this entry
			যাও out;
		}

		i = গ্রন্থি_2->i32;
		v = &i;
		বিরতি;

	ক্ষেত্র BSON_DOUBLE:
		যদি (param_type(param) != PARAM_TYPE_FLOAT) {
			PX4_WARN("unexpected type for %s", গ্রন্থি_2->name);
			ফলাফল = 1; // just skip this entry
			যাও out;
		}

		f = গ্রন্থি_2->d;
		v = &f;
		বিরতি;

	পূর্বনির্ধারিত:
		PX4_ERR("%s unrecognised node type %d", গ্রন্থি_2->name, গ্রন্থি_2->type);
		ফলাফল = 1; // just skip this entry
		যাও out;
	}

	যদি (param_set_external(param, v, সত্য, সত্য)) {
		debug("error setting value for '%s'", গ্রন্থি_2->name);
		যাও out;
	}

	/* don't return zero, that means EOF */
	ফলাফল = 1;

out:
	ফেরত ফলাফল;
}

স্থির পূর্ণসংখ্যা
import_one_entry(uint8_t *বাফার_2, size_t buf_আকার, শূন্য *arg)
{
	পূর্ণসংখ্যা *ফলাফল = static_cast<পূর্ণসংখ্যা *>(arg);
	bson_decoder_s decoder{};

	যদি (bson_decoder_init_buf(&decoder, বাফার_2, buf_আকার, param_import_callback)) {
		debug("decoder init failed");
		*ফলাফল = -1;
		ফেরত -1;
	}

	করো {
		*ফলাফল = bson_decoder_পরবর্তী(&decoder);

	} যখন (*ফলাফল > 0);

	ফেরত *ফলাফল < 0 ? *ফলাফল : 0;
}

স্থির পূর্ণসংখ্যা
param_import_internal(যৌক্তিক *legacy)
{
	পূর্ণসংখ্যা ফলাফল = 0;
	log_stale = মিথ্যা;
	পূর্ণসংখ্যা n = parameter_flashfs_walk(parameters_legacy_token, import_one_entry, &ফলাফল);
	*legacy = n > 0;

	যদি (n == 0) {
		n = parameter_flashfs_walk(parameters_token, import_one_entry, &ফলাফল);
	}

	যদি (n < 0) {
		debug("flash walk failed (%d)", n);
		log_unreadable = সত্য;
		ফেরত n;
	}

	যদি (n == 0) {
		/* No valid entry found. A fully erased store is blank (first boot, or
		 * after switching firmware): report "not yet stored" (1), matching the
		 * convention used by param_load_default(). A store that holds data but
		 * no valid entry (torn write, bit-rot, or foreign data) is corrupt and
		 * must not be silently reseeded - report it so the boot recovery runs. */
		যদি (parameter_flashfs_blank() == 1) {
			ফেরত 1;
		}

		debug("flash holds data but no valid entry");
		ফেরত -EILSEQ;
	}

	যদি (ফলাফল < 0) {
		debug("BSON error decoding parameters");
		ফেরত ফলাফল;
	}

	ফেরত 0;
}

/*
 * Same-bank erase stalls instruction fetch for ~1 s on H7. Do it here at
 * boot, before sensors or DShot start, and only when a burst of deltas would
 * no longer fit, the store is still in the legacy layout, or the log holds
 * records this firmware translated or could not use. Only a failure after
 * the erase is an error: until then the log is intact and RAM holds what it
 * said, so the boot must not be reported as a corrupt store.
 */
স্থির পূর্ণসংখ্যা
compact_after_import(যৌক্তিক legacy)
{
	bson_encoder_s encoder{};
	পূর্ণসংখ্যা ফলাফল = encode_document(&encoder, nullptr, সত্য);

	যদি (ফলাফল != 0) {
		PX4_ERR("flashparams: snapshot encode failed (%d), log kept", ফলাফল);
		মুক্ত(bson_encoder_buf_তথ্য(&encoder));
		ফেরত 0;
	}

	ধ্রুবক size_t enc_আকার = bson_encoder_buf_আকার(&encoder);
	পূর্ণসংখ্যা need = (legacy || log_stale) ? 1 : parameter_flashfs_needs_compact(parameters_token, enc_আকার);
	ফলাফল = 0;

	যদি (need > 0) {
		uint8_t *বাফার_2;
		size_t staged_আকার;
		ফলাফল = stage_snapshot(&encoder, &বাফার_2, &staged_আকার);

		যদি (ফলাফল != OK) {
			PX4_ERR("flashparams: cannot stage snapshot (%d), log kept", ফলাফল);
			ফলাফল = 0;

		} অন্যথা {
			PX4_INFO("flashparams: compacting param sector%s", legacy ? " (legacy layout)" : "");
			ফলাফল = commit_snapshot(বাফার_2, staged_আকার);

			যদি (ফলাফল != OK) {
				PX4_ERR("flashparams: compact failed (%d)", ফলাফল);
			}
		}
	}

	মুক্ত(bson_encoder_buf_তথ্য(&encoder));
	ফেরত ফলাফল;
}

পূর্ণসংখ্যা flash_param_save(param_filter_func filter)
{
	ফেরত param_export_internal(filter);
}

পূর্ণসংখ্যা flash_param_import()
{
	যৌক্তিক legacy = মিথ্যা;
	পূর্ণসংখ্যা ফলাফল = param_import_internal(&legacy);

	যদি (ফলাফল != 0) {
		ফেরত ফলাফল;
	}

	ফেরত compact_after_import(legacy);
}
