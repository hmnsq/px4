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

#অন্তর্ভাৱ <px4_platform_common/defines.h>
#অন্তর্ভাৱ <px4_platform_common/log.h>
#অন্তর্ভাৱ <px4_platform_common/posix.h>
#অন্তর্ভাৱ <px4_platform_common/shutdown.h>
#অন্তর্ভাৱ <parameters/px4_parameters.hpp>

#অন্তর্ভাৱ <string.h>
#অন্তর্ভাৱ <stdbool.h>
#অন্তর্ভাৱ <stdint.h>
#অন্তর্ভাৱ <errno.h>

#অন্তর্ভাৱ <parameters/param.h>

#অন্তর্ভাৱ <lib/tinybson/tinybson.h>
#অন্তর্ভাৱ "flashparams.h"
#অন্তর্ভাৱ "flashfs.h"
#অন্তর্ভাৱ "../param_translation.h"

#যদি 0
# define debug(fmt, args...)            কৰা { warnx(fmt, ##args); } যেতিয়ালৈ(0)
#অন্যথা
# define debug(fmt, args...)            কৰা { } যেতিয়ালৈ(0)
#endif


/**
 * Storage for modified parameters.
 */
সংরচনা param_wbuf_s {
	সঙ্ঘ param_মান_u     val;
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
স্থির তার্কিক log_stale;

/* Replay failed inside a CRC-valid record. A delta appended behind it would
 * never be reached, so the next save rewrites the store. */
স্থির তার্কিক log_unreadable;

স্থির তার্কিক
any_unsaved(param_filter_func filter)
{
	বাবে (param_t param = 0; param < user_config.PARAM_গুনতি; param++) {
		যদি ((filter == nullptr || filter(param)) && param_মান_unsaved(param)) {
			ঘূৰাই সঁচা;
		}
	}

	ঘূৰাই মিছা;
}

/*
 * A snapshot holds every override in RAM, equal to the current default or
 * not: the boot compaction runs before the airframe applies its set-default
 * values, so at that point "equals the default" says nothing. A delta holds
 * the unsaved params; one that no longer overrides, or now matches the
 * default in force, is written as a BSON null so replay drops it, which is
 * what the file backend's "don't export defaults" gives after a reload.
 */
স্থির পূর্ণাঙ্ক
encode_params(bson_encoder_s *encoder, param_filter_func filter, তার্কিক snapshot)
{
	বাবে (param_t param = 0; param < user_config.PARAM_গুনতি; param++) {
		যদি (filter && !filter(param)) {
			অনুৱর্ত;
		}

		যদি (snapshot) {
			যদি (!user_config.contains(param)) {
				অনুৱর্ত;
			}

		} অন্যথা {
			যদি (!param_মান_unsaved(param)) {
				অনুৱর্ত;
			}

			যদি (!user_config.contains(param) || param_মান_is_default(param)) {
				যদি (bson_encoder_append_null(encoder, param_name(param))) {
					debug("BSON append failed for '%s'", param_name(param));
					ঘূৰাই -1;
				}

				অনুৱর্ত;
			}
		}

		ৱিকল্প (param_type(param)) {
		পক্ষ PARAM_TYPE_INT32:
			যদি (bson_encoder_append_int32(encoder, param_name(param), user_config.get(param).i)) {
				debug("BSON append failed for '%s'", param_name(param));
				ঘূৰাই -1;
			}

			ভঙ্গ;

		পক্ষ PARAM_TYPE_FLOAT:
			যদি (bson_encoder_append_double(encoder, param_name(param), (দ্ৱিগুণ)user_config.get(param).f)) {
				debug("BSON append failed for '%s'", param_name(param));
				ঘূৰাই -1;
			}

			ভঙ্গ;

		স্ৱভাৱ:
			debug("unrecognized parameter type");
			ঘূৰাই -1;
		}
	}

	ঘূৰাই 0;
}

/* The caller frees bson_encoder_buf_data() whatever the result. */
স্থির পূর্ণাঙ্ক
encode_document(bson_encoder_s *encoder, param_filter_func filter, তার্কিক snapshot)
{
	bson_encoder_init_buf(encoder, nullptr, 0);

	পূর্ণাঙ্ক result = encode_params(encoder, filter, snapshot);

	যদি (result == 0) {
		result = bson_encoder_fini(encoder);
	}

	ঘূৰাই result;
}

স্থির পূর্ণাঙ্ক
stage_write_buffer(bson_encoder_s *encoder, uint8_t **buffer, size_t *enc_size)
{
	শূন্য *enc_buff = bson_encoder_buf_data(encoder);
	*enc_size = bson_encoder_buf_size(encoder);

	যদি (enc_buff == nullptr) {
		ঘূৰাই -ENOMEM;
	}

	size_t alloc_size = *enc_size;
	পূর্ণাঙ্ক result = parameter_flashfs_alloc(parameters_token, buffer, &alloc_size);

	যদি (result != OK) {
		ঘূৰাই result;
	}

	যদি (alloc_size < *enc_size) {
		parameter_flashfs_free();
		ঘূৰাই -ENOMEM;
	}

	memcpy(*buffer, enc_buff, *enc_size);
	ঘূৰাই OK;
}

স্থির পূর্ণাঙ্ক
commit_append(uint8_t *buffer, size_t enc_size)
{
	পূর্ণাঙ্ক result = parameter_flashfs_write(parameters_token, buffer, enc_size);
	parameter_flashfs_free();

	যদি (result == (পূর্ণাঙ্ক)enc_size) {
		ঘূৰাই OK;
	}

	ঘূৰাই result < 0 ? result : -EFBIG;
}

স্থির পূর্ণাঙ্ক
export_delta(param_filter_func filter)
{
	bson_encoder_s encoder{};
	পূর্ণাঙ্ক result = encode_document(&encoder, filter, মিছা);

	যদি (result == 0) {
		uint8_t *buffer;
		size_t enc_size;
		result = stage_write_buffer(&encoder, &buffer, &enc_size);

		যদি (result == OK) {
			result = commit_append(buffer, enc_size);
		}
	}

	মুক্ত(bson_encoder_buf_data(&encoder));
	ঘূৰাই result;
}

/* Everything that can fail before the erase, so a failure here leaves the
 * log untouched and still loadable. */
স্থির পূর্ণাঙ্ক
stage_snapshot(bson_encoder_s *encoder, uint8_t **buffer, size_t *enc_size)
{
	পূর্ণাঙ্ক result = stage_write_buffer(encoder, buffer, enc_size);

	যদি (result != OK) {
		ঘূৰাই result;
	}

	যদি (*enc_size > parameter_flashfs_max_payload()) {
		parameter_flashfs_free();
		ঘূৰাই -ENOSPC;
	}

	ঘূৰাই OK;
}

স্থির পূর্ণাঙ্ক
commit_snapshot(uint8_t *buffer, size_t enc_size)
{
	যদি (parameter_flashfs_blank() != 1) {
		পূর্ণাঙ্ক rv = parameter_flashfs_erase();

		যদি (rv < 0) {
			parameter_flashfs_free();
			ঘূৰাই rv;
		}
	}

	পূর্ণাঙ্ক result = parameter_flashfs_write(parameters_token, buffer, enc_size);

	যদি (result != (পূর্ণাঙ্ক)enc_size && parameter_flashfs_blank() == 1) {
		result = parameter_flashfs_write(parameters_token, buffer, enc_size);
	}

	parameter_flashfs_free();

	যদি (result == (পূর্ণাঙ্ক)enc_size) {
		ঘূৰাই OK;
	}

	ঘূৰাই result < 0 ? result : -EFBIG;
}

স্থির পূর্ণাঙ্ক
compact_to_snapshot()
{
	bson_encoder_s encoder{};
	পূর্ণাঙ্ক result = encode_document(&encoder, nullptr, সঁচা);

	যদি (result == 0) {
		uint8_t *buffer;
		size_t enc_size;
		result = stage_snapshot(&encoder, &buffer, &enc_size);

		যদি (result == OK) {
			result = commit_snapshot(buffer, enc_size);
		}
	}

	যদি (result == OK) {
		log_unreadable = মিছা;
	}

	মুক্ত(bson_encoder_buf_data(&encoder));
	ঘূৰাই result;
}

স্থির তার্কিক
has_record(flash_file_token_t token)
{
	uint8_t *buffer = nullptr;
	size_t buf_size = 0;
	ঘূৰাই parameter_flashfs_read(token, &buffer, &buf_size) >= 0;
}

স্থির পূর্ণাঙ্ক
param_export_internal(param_filter_func filter)
{
	/* A delta only means something behind a snapshot that replays. A legacy
	 * record still present means the boot migration did not run; replay
	 * would take it over any delta appended behind it. */
	যদি (log_unreadable || has_record(parameters_legacy_token) || !has_record(parameters_token)) {
		ঘূৰাই compact_to_snapshot();
	}

	যদি (!any_unsaved(filter)) {
		ঘূৰাই 0;
	}

	পূর্ণাঙ্ক result = export_delta(filter);

	যদি (result != -ENOSPC) {
		ঘূৰাই result;
	}

	/* Same-bank erase stalls the CPU for ~1 s; last resort when a single
	 * save does not fit the tail. Compact writes the full RAM snapshot so a
	 * filtered save cannot drop params not in the delta. */
	PX4_WARN("flashparams: param sector full, compacting");
	ঘূৰাই compact_to_snapshot();
}


স্থির তার্কিক
record_is_stale(bson_গ্ৰন্থি_t গ্ৰন্থি_2)
{
	ধ্রুৱ param_t param = param_find_no_notification(গ্ৰন্থি_2->name);

	যদি (param == PARAM_INVALID) {
		ঘূৰাই সঁচা;
	}

	ৱিকল্প (গ্ৰন্থি_2->type) {
	পক্ষ BSON_INT32:
		ঘূৰাই param_type(param) != PARAM_TYPE_INT32;

	পক্ষ BSON_DOUBLE:
		ঘূৰাই param_type(param) != PARAM_TYPE_FLOAT;

	স্ৱভাৱ:
		ঘূৰাই মিছা;
	}
}

স্থির পূর্ণাঙ্ক
param_import_callback(bson_decoder_t decoder, bson_গ্ৰন্থি_t গ্ৰন্থি_2)
{
	ভিন্ন f;
	int32_t i;
	শূন্য *v = nullptr;
	পূর্ণাঙ্ক result = -1;

	/*
	 * EOO means the end of the parameter object. (Currently not supporting
	 * nested BSON objects).
	 */
	যদি (গ্ৰন্থি_2->type == BSON_EOO) {
		debug("end of parameters");
		ঘূৰাই 0;
	}

	/* A tombstone has no value. The decoder leaves the previous node's in
	 * the union and the translations read it without a type check. */
	ধ্রুৱ তার্কিক tombstone = গ্ৰন্থি_2->type == BSON_nullptr || গ্ৰন্থি_2->type == BSON_UNDEFINED;

	যদি (!tombstone) {
		যদি (record_is_stale(গ্ৰন্থি_2)) {
			log_stale = সঁচা;
		}

		যদি (param_modify_on_import(গ্ৰন্থি_2) == param_modify_on_import_ret::PARAM_SKIP_IMPORT) {
			ঘূৰাই 1;
		}
	}

	/*
	 * Find the parameter this node represents.  If we don't know it,
	 * ignore the node.
	 */
	param_t param = param_find_no_notification(গ্ৰন্থি_2->name);

	যদি (param == PARAM_INVALID) {
		debug("ignoring unrecognised parameter '%s'", গ্ৰন্থি_2->name);
		ঘূৰাই 1;
	}

	/*
	 * Handle setting the parameter from the node
	 */

	ৱিকল্প (গ্ৰন্থি_2->type) {
	পক্ষ BSON_nullptr:
	পক্ষ BSON_UNDEFINED:
		user_config.reset(param);
		result = 1;
		গচ্ছ out;

	পক্ষ BSON_INT32:
		যদি (param_type(param) != PARAM_TYPE_INT32) {
			PX4_WARN("unexpected type for %s", গ্ৰন্থি_2->name);
			result = 1; // just skip this entry
			গচ্ছ out;
		}

		i = গ্ৰন্থি_2->i32;
		v = &i;
		ভঙ্গ;

	পক্ষ BSON_DOUBLE:
		যদি (param_type(param) != PARAM_TYPE_FLOAT) {
			PX4_WARN("unexpected type for %s", গ্ৰন্থি_2->name);
			result = 1; // just skip this entry
			গচ্ছ out;
		}

		f = গ্ৰন্থি_2->d;
		v = &f;
		ভঙ্গ;

	স্ৱভাৱ:
		PX4_ERR("%s unrecognised node type %d", গ্ৰন্থি_2->name, গ্ৰন্থি_2->type);
		result = 1; // just skip this entry
		গচ্ছ out;
	}

	যদি (param_set_external(param, v, সঁচা, সঁচা)) {
		debug("error setting value for '%s'", গ্ৰন্থি_2->name);
		গচ্ছ out;
	}

	/* don't return zero, that means EOF */
	result = 1;

out:
	ঘূৰাই result;
}

স্থির পূর্ণাঙ্ক
import_one_entry(uint8_t *buffer, size_t buf_size, শূন্য *arg)
{
	পূর্ণাঙ্ক *result = static_cast<পূর্ণাঙ্ক *>(arg);
	bson_decoder_s decoder{};

	যদি (bson_decoder_init_buf(&decoder, buffer, buf_size, param_import_callback)) {
		debug("decoder init failed");
		*result = -1;
		ঘূৰাই -1;
	}

	কৰা {
		*result = bson_decoder_next(&decoder);

	} যেতিয়ালৈ (*result > 0);

	ঘূৰাই *result < 0 ? *result : 0;
}

স্থির পূর্ণাঙ্ক
param_import_internal(তার্কিক *legacy)
{
	পূর্ণাঙ্ক result = 0;
	log_stale = মিছা;
	পূর্ণাঙ্ক n = parameter_flashfs_walk(parameters_legacy_token, import_one_entry, &result);
	*legacy = n > 0;

	যদি (n == 0) {
		n = parameter_flashfs_walk(parameters_token, import_one_entry, &result);
	}

	যদি (n < 0) {
		debug("flash walk failed (%d)", n);
		log_unreadable = সঁচা;
		ঘূৰাই n;
	}

	যদি (n == 0) {
		/* No valid entry found. A fully erased store is blank (first boot, or
		 * after switching firmware): report "not yet stored" (1), matching the
		 * convention used by param_load_default(). A store that holds data but
		 * no valid entry (torn write, bit-rot, or foreign data) is corrupt and
		 * must not be silently reseeded - report it so the boot recovery runs. */
		যদি (parameter_flashfs_blank() == 1) {
			ঘূৰাই 1;
		}

		debug("flash holds data but no valid entry");
		ঘূৰাই -EILSEQ;
	}

	যদি (result < 0) {
		debug("BSON error decoding parameters");
		ঘূৰাই result;
	}

	ঘূৰাই 0;
}

/*
 * Same-bank erase stalls instruction fetch for ~1 s on H7. Do it here at
 * boot, before sensors or DShot start, and only when a burst of deltas would
 * no longer fit, the store is still in the legacy layout, or the log holds
 * records this firmware translated or could not use. Only a failure after
 * the erase is an error: until then the log is intact and RAM holds what it
 * said, so the boot must not be reported as a corrupt store.
 */
স্থির পূর্ণাঙ্ক
compact_after_import(তার্কিক legacy)
{
	bson_encoder_s encoder{};
	পূর্ণাঙ্ক result = encode_document(&encoder, nullptr, সঁচা);

	যদি (result != 0) {
		PX4_ERR("flashparams: snapshot encode failed (%d), log kept", result);
		মুক্ত(bson_encoder_buf_data(&encoder));
		ঘূৰাই 0;
	}

	ধ্রুৱ size_t enc_size = bson_encoder_buf_size(&encoder);
	পূর্ণাঙ্ক need = (legacy || log_stale) ? 1 : parameter_flashfs_needs_compact(parameters_token, enc_size);
	result = 0;

	যদি (need > 0) {
		uint8_t *buffer;
		size_t staged_size;
		result = stage_snapshot(&encoder, &buffer, &staged_size);

		যদি (result != OK) {
			PX4_ERR("flashparams: cannot stage snapshot (%d), log kept", result);
			result = 0;

		} অন্যথা {
			PX4_INFO("flashparams: compacting param sector%s", legacy ? " (legacy layout)" : "");
			result = commit_snapshot(buffer, staged_size);

			যদি (result != OK) {
				PX4_ERR("flashparams: compact failed (%d)", result);
			}
		}
	}

	মুক্ত(bson_encoder_buf_data(&encoder));
	ঘূৰাই result;
}

পূর্ণাঙ্ক flash_param_save(param_filter_func filter)
{
	ঘূৰাই param_export_internal(filter);
}

পূর্ণাঙ্ক flash_param_import()
{
	তার্কিক legacy = মিছা;
	পূর্ণাঙ্ক result = param_import_internal(&legacy);

	যদি (result != 0) {
		ঘূৰাই result;
	}

	ঘূৰাই compact_after_import(legacy);
}
