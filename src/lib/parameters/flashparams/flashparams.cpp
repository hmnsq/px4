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

#ଅନ୍ତର୍ଭାଵ <px4_platform_common/defines.h>
#ଅନ୍ତର୍ଭାଵ <px4_platform_common/log.h>
#ଅନ୍ତର୍ଭାଵ <px4_platform_common/posix.h>
#ଅନ୍ତର୍ଭାଵ <px4_platform_common/shutdown.h>
#ଅନ୍ତର୍ଭାଵ <parameters/px4_parameters.hpp>

#ଅନ୍ତର୍ଭାଵ <string.h>
#ଅନ୍ତର୍ଭାଵ <stdbool.h>
#ଅନ୍ତର୍ଭାଵ <stdint.h>
#ଅନ୍ତର୍ଭାଵ <errno.h>

#ଅନ୍ତର୍ଭାଵ <parameters/param.h>

#ଅନ୍ତର୍ଭାଵ <lib/tinybson/tinybson.h>
#ଅନ୍ତର୍ଭାଵ "flashparams.h"
#ଅନ୍ତର୍ଭାଵ "flashfs.h"
#ଅନ୍ତର୍ଭାଵ "../param_translation.h"

#ଯଦି 0
# define debug(fmt, args...)            କର { warnx(fmt, ##args); } ଯେତେବେଳେ(0)
#ଅନ୍ୟଥା
# define debug(fmt, args...)            କର { } ଯେତେବେଳେ(0)
#endif


/**
 * Storage for modified parameters.
 */
ସଂରଚନା param_wbuf_s {
	ସଂଘ param_ମୂଲ୍ୟ_u     val;
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
ସ୍ଥିର ତାର୍କିକ log_stale;

/* Replay failed inside a CRC-valid record. A delta appended behind it would
 * never be reached, so the next save rewrites the store. */
ସ୍ଥିର ତାର୍କିକ log_unreadable;

ସ୍ଥିର ତାର୍କିକ
any_unsaved(param_filter_func filter)
{
	ପାଇଁ (param_t param = 0; param < user_config.PARAM_ଗଣନା; param++) {
		ଯଦି ((filter == nullptr || filter(param)) && param_ମୂଲ୍ୟ_unsaved(param)) {
			ଫେରନ୍ତୁ ସତ୍ୟ;
		}
	}

	ଫେରନ୍ତୁ ମିଥ୍ୟା;
}

/*
 * A snapshot holds every override in RAM, equal to the current default or
 * not: the boot compaction runs before the airframe applies its set-default
 * values, so at that point "equals the default" says nothing. A delta holds
 * the unsaved params; one that no longer overrides, or now matches the
 * default in force, is written as a BSON null so replay drops it, which is
 * what the file backend's "don't export defaults" gives after a reload.
 */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
encode_params(bson_encoder_s *encoder, param_filter_func filter, ତାର୍କିକ snapshot)
{
	ପାଇଁ (param_t param = 0; param < user_config.PARAM_ଗଣନା; param++) {
		ଯଦି (filter && !filter(param)) {
			ଜାରି;
		}

		ଯଦି (snapshot) {
			ଯଦି (!user_config.contains(param)) {
				ଜାରି;
			}

		} ଅନ୍ୟଥା {
			ଯଦି (!param_ମୂଲ୍ୟ_unsaved(param)) {
				ଜାରି;
			}

			ଯଦି (!user_config.contains(param) || param_ମୂଲ୍ୟ_is_default(param)) {
				ଯଦି (bson_encoder_append_null(encoder, param_name(param))) {
					debug("BSON append failed for '%s'", param_name(param));
					ଫେରନ୍ତୁ -1;
				}

				ଜାରି;
			}
		}

		ବାଛ (param_type(param)) {
		ସ୍ଥିତି PARAM_TYPE_INT32:
			ଯଦି (bson_encoder_append_int32(encoder, param_name(param), user_config.get(param).i)) {
				debug("BSON append failed for '%s'", param_name(param));
				ଫେରନ୍ତୁ -1;
			}

			ଭାଙ୍ଗ;

		ସ୍ଥିତି PARAM_TYPE_FLOAT:
			ଯଦି (bson_encoder_append_double(encoder, param_name(param), (ଦ୍ୱିଗୁଣ)user_config.get(param).f)) {
				debug("BSON append failed for '%s'", param_name(param));
				ଫେରନ୍ତୁ -1;
			}

			ଭାଙ୍ଗ;

		ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
			debug("unrecognized parameter type");
			ଫେରନ୍ତୁ -1;
		}
	}

	ଫେରନ୍ତୁ 0;
}

/* The caller frees bson_encoder_buf_data() whatever the result. */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
encode_document(bson_encoder_s *encoder, param_filter_func filter, ତାର୍କିକ snapshot)
{
	bson_encoder_init_buf(encoder, nullptr, 0);

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା result = encode_params(encoder, filter, snapshot);

	ଯଦି (result == 0) {
		result = bson_encoder_fini(encoder);
	}

	ଫେରନ୍ତୁ result;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
stage_write_buffer(bson_encoder_s *encoder, uint8_t **buffer, size_t *enc_size)
{
	ଶୂନ୍ୟ *enc_buff = bson_encoder_buf_data(encoder);
	*enc_size = bson_encoder_buf_size(encoder);

	ଯଦି (enc_buff == nullptr) {
		ଫେରନ୍ତୁ -ENOMEM;
	}

	size_t alloc_size = *enc_size;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା result = parameter_flashfs_alloc(parameters_token, buffer, &alloc_size);

	ଯଦି (result != OK) {
		ଫେରନ୍ତୁ result;
	}

	ଯଦି (alloc_size < *enc_size) {
		parameter_flashfs_free();
		ଫେରନ୍ତୁ -ENOMEM;
	}

	memcpy(*buffer, enc_buff, *enc_size);
	ଫେରନ୍ତୁ OK;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
commit_append(uint8_t *buffer, size_t enc_size)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା result = parameter_flashfs_write(parameters_token, buffer, enc_size);
	parameter_flashfs_free();

	ଯଦି (result == (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)enc_size) {
		ଫେରନ୍ତୁ OK;
	}

	ଫେରନ୍ତୁ result < 0 ? result : -EFBIG;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
export_delta(param_filter_func filter)
{
	bson_encoder_s encoder{};
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା result = encode_document(&encoder, filter, ମିଥ୍ୟା);

	ଯଦି (result == 0) {
		uint8_t *buffer;
		size_t enc_size;
		result = stage_write_buffer(&encoder, &buffer, &enc_size);

		ଯଦି (result == OK) {
			result = commit_append(buffer, enc_size);
		}
	}

	ମୁକ୍ତ(bson_encoder_buf_data(&encoder));
	ଫେରନ୍ତୁ result;
}

/* Everything that can fail before the erase, so a failure here leaves the
 * log untouched and still loadable. */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
stage_snapshot(bson_encoder_s *encoder, uint8_t **buffer, size_t *enc_size)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା result = stage_write_buffer(encoder, buffer, enc_size);

	ଯଦି (result != OK) {
		ଫେରନ୍ତୁ result;
	}

	ଯଦି (*enc_size > parameter_flashfs_max_payload()) {
		parameter_flashfs_free();
		ଫେରନ୍ତୁ -ENOSPC;
	}

	ଫେରନ୍ତୁ OK;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
commit_snapshot(uint8_t *buffer, size_t enc_size)
{
	ଯଦି (parameter_flashfs_blank() != 1) {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା rv = parameter_flashfs_erase();

		ଯଦି (rv < 0) {
			parameter_flashfs_free();
			ଫେରନ୍ତୁ rv;
		}
	}

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା result = parameter_flashfs_write(parameters_token, buffer, enc_size);

	ଯଦି (result != (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)enc_size && parameter_flashfs_blank() == 1) {
		result = parameter_flashfs_write(parameters_token, buffer, enc_size);
	}

	parameter_flashfs_free();

	ଯଦି (result == (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)enc_size) {
		ଫେରନ୍ତୁ OK;
	}

	ଫେରନ୍ତୁ result < 0 ? result : -EFBIG;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
compact_to_snapshot()
{
	bson_encoder_s encoder{};
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା result = encode_document(&encoder, nullptr, ସତ୍ୟ);

	ଯଦି (result == 0) {
		uint8_t *buffer;
		size_t enc_size;
		result = stage_snapshot(&encoder, &buffer, &enc_size);

		ଯଦି (result == OK) {
			result = commit_snapshot(buffer, enc_size);
		}
	}

	ଯଦି (result == OK) {
		log_unreadable = ମିଥ୍ୟା;
	}

	ମୁକ୍ତ(bson_encoder_buf_data(&encoder));
	ଫେରନ୍ତୁ result;
}

ସ୍ଥିର ତାର୍କିକ
has_record(flash_file_token_t token)
{
	uint8_t *buffer = nullptr;
	size_t buf_size = 0;
	ଫେରନ୍ତୁ parameter_flashfs_read(token, &buffer, &buf_size) >= 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
param_export_internal(param_filter_func filter)
{
	/* A delta only means something behind a snapshot that replays. A legacy
	 * record still present means the boot migration did not run; replay
	 * would take it over any delta appended behind it. */
	ଯଦି (log_unreadable || has_record(parameters_legacy_token) || !has_record(parameters_token)) {
		ଫେରନ୍ତୁ compact_to_snapshot();
	}

	ଯଦି (!any_unsaved(filter)) {
		ଫେରନ୍ତୁ 0;
	}

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା result = export_delta(filter);

	ଯଦି (result != -ENOSPC) {
		ଫେରନ୍ତୁ result;
	}

	/* Same-bank erase stalls the CPU for ~1 s; last resort when a single
	 * save does not fit the tail. Compact writes the full RAM snapshot so a
	 * filtered save cannot drop params not in the delta. */
	PX4_WARN("flashparams: param sector full, compacting");
	ଫେରନ୍ତୁ compact_to_snapshot();
}


ସ୍ଥିର ତାର୍କିକ
record_is_stale(bson_ଗଣ୍ଠି_t ଗଣ୍ଠି_2)
{
	ଅଚଳ param_t param = param_find_no_notification(ଗଣ୍ଠି_2->name);

	ଯଦି (param == PARAM_INVALID) {
		ଫେରନ୍ତୁ ସତ୍ୟ;
	}

	ବାଛ (ଗଣ୍ଠି_2->type) {
	ସ୍ଥିତି BSON_INT32:
		ଫେରନ୍ତୁ param_type(param) != PARAM_TYPE_INT32;

	ସ୍ଥିତି BSON_DOUBLE:
		ଫେରନ୍ତୁ param_type(param) != PARAM_TYPE_FLOAT;

	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		ଫେରନ୍ତୁ ମିଥ୍ୟା;
	}
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
param_import_callback(bson_decoder_t decoder, bson_ଗଣ୍ଠି_t ଗଣ୍ଠି_2)
{
	ଦଶମିକ f;
	int32_t i;
	ଶୂନ୍ୟ *v = nullptr;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା result = -1;

	/*
	 * EOO means the end of the parameter object. (Currently not supporting
	 * nested BSON objects).
	 */
	ଯଦି (ଗଣ୍ଠି_2->type == BSON_EOO) {
		debug("end of parameters");
		ଫେରନ୍ତୁ 0;
	}

	/* A tombstone has no value. The decoder leaves the previous node's in
	 * the union and the translations read it without a type check. */
	ଅଚଳ ତାର୍କିକ tombstone = ଗଣ୍ଠି_2->type == BSON_nullptr || ଗଣ୍ଠି_2->type == BSON_UNDEFINED;

	ଯଦି (!tombstone) {
		ଯଦି (record_is_stale(ଗଣ୍ଠି_2)) {
			log_stale = ସତ୍ୟ;
		}

		ଯଦି (param_modify_on_import(ଗଣ୍ଠି_2) == param_modify_on_import_ret::PARAM_SKIP_IMPORT) {
			ଫେରନ୍ତୁ 1;
		}
	}

	/*
	 * Find the parameter this node represents.  If we don't know it,
	 * ignore the node.
	 */
	param_t param = param_find_no_notification(ଗଣ୍ଠି_2->name);

	ଯଦି (param == PARAM_INVALID) {
		debug("ignoring unrecognised parameter '%s'", ଗଣ୍ଠି_2->name);
		ଫେରନ୍ତୁ 1;
	}

	/*
	 * Handle setting the parameter from the node
	 */

	ବାଛ (ଗଣ୍ଠି_2->type) {
	ସ୍ଥିତି BSON_nullptr:
	ସ୍ଥିତି BSON_UNDEFINED:
		user_config.reset(param);
		result = 1;
		ଯାଅ out;

	ସ୍ଥିତି BSON_INT32:
		ଯଦି (param_type(param) != PARAM_TYPE_INT32) {
			PX4_WARN("unexpected type for %s", ଗଣ୍ଠି_2->name);
			result = 1; // just skip this entry
			ଯାଅ out;
		}

		i = ଗଣ୍ଠି_2->i32;
		v = &i;
		ଭାଙ୍ଗ;

	ସ୍ଥିତି BSON_DOUBLE:
		ଯଦି (param_type(param) != PARAM_TYPE_FLOAT) {
			PX4_WARN("unexpected type for %s", ଗଣ୍ଠି_2->name);
			result = 1; // just skip this entry
			ଯାଅ out;
		}

		f = ଗଣ୍ଠି_2->d;
		v = &f;
		ଭାଙ୍ଗ;

	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		PX4_ERR("%s unrecognised node type %d", ଗଣ୍ଠି_2->name, ଗଣ୍ଠି_2->type);
		result = 1; // just skip this entry
		ଯାଅ out;
	}

	ଯଦି (param_set_external(param, v, ସତ୍ୟ, ସତ୍ୟ)) {
		debug("error setting value for '%s'", ଗଣ୍ଠି_2->name);
		ଯାଅ out;
	}

	/* don't return zero, that means EOF */
	result = 1;

out:
	ଫେରନ୍ତୁ result;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
import_one_entry(uint8_t *buffer, size_t buf_size, ଶୂନ୍ୟ *arg)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା *result = static_cast<ପୂର୍ଣ୍ଣସଂଖ୍ୟା *>(arg);
	bson_decoder_s decoder{};

	ଯଦି (bson_decoder_init_buf(&decoder, buffer, buf_size, param_import_callback)) {
		debug("decoder init failed");
		*result = -1;
		ଫେରନ୍ତୁ -1;
	}

	କର {
		*result = bson_decoder_next(&decoder);

	} ଯେତେବେଳେ (*result > 0);

	ଫେରନ୍ତୁ *result < 0 ? *result : 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
param_import_internal(ତାର୍କିକ *legacy)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା result = 0;
	log_stale = ମିଥ୍ୟା;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା n = parameter_flashfs_walk(parameters_legacy_token, import_one_entry, &result);
	*legacy = n > 0;

	ଯଦି (n == 0) {
		n = parameter_flashfs_walk(parameters_token, import_one_entry, &result);
	}

	ଯଦି (n < 0) {
		debug("flash walk failed (%d)", n);
		log_unreadable = ସତ୍ୟ;
		ଫେରନ୍ତୁ n;
	}

	ଯଦି (n == 0) {
		/* No valid entry found. A fully erased store is blank (first boot, or
		 * after switching firmware): report "not yet stored" (1), matching the
		 * convention used by param_load_default(). A store that holds data but
		 * no valid entry (torn write, bit-rot, or foreign data) is corrupt and
		 * must not be silently reseeded - report it so the boot recovery runs. */
		ଯଦି (parameter_flashfs_blank() == 1) {
			ଫେରନ୍ତୁ 1;
		}

		debug("flash holds data but no valid entry");
		ଫେରନ୍ତୁ -EILSEQ;
	}

	ଯଦି (result < 0) {
		debug("BSON error decoding parameters");
		ଫେରନ୍ତୁ result;
	}

	ଫେରନ୍ତୁ 0;
}

/*
 * Same-bank erase stalls instruction fetch for ~1 s on H7. Do it here at
 * boot, before sensors or DShot start, and only when a burst of deltas would
 * no longer fit, the store is still in the legacy layout, or the log holds
 * records this firmware translated or could not use. Only a failure after
 * the erase is an error: until then the log is intact and RAM holds what it
 * said, so the boot must not be reported as a corrupt store.
 */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
compact_after_import(ତାର୍କିକ legacy)
{
	bson_encoder_s encoder{};
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା result = encode_document(&encoder, nullptr, ସତ୍ୟ);

	ଯଦି (result != 0) {
		PX4_ERR("flashparams: snapshot encode failed (%d), log kept", result);
		ମୁକ୍ତ(bson_encoder_buf_data(&encoder));
		ଫେରନ୍ତୁ 0;
	}

	ଅଚଳ size_t enc_size = bson_encoder_buf_size(&encoder);
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା need = (legacy || log_stale) ? 1 : parameter_flashfs_needs_compact(parameters_token, enc_size);
	result = 0;

	ଯଦି (need > 0) {
		uint8_t *buffer;
		size_t staged_size;
		result = stage_snapshot(&encoder, &buffer, &staged_size);

		ଯଦି (result != OK) {
			PX4_ERR("flashparams: cannot stage snapshot (%d), log kept", result);
			result = 0;

		} ଅନ୍ୟଥା {
			PX4_INFO("flashparams: compacting param sector%s", legacy ? " (legacy layout)" : "");
			result = commit_snapshot(buffer, staged_size);

			ଯଦି (result != OK) {
				PX4_ERR("flashparams: compact failed (%d)", result);
			}
		}
	}

	ମୁକ୍ତ(bson_encoder_buf_data(&encoder));
	ଫେରନ୍ତୁ result;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା flash_param_save(param_filter_func filter)
{
	ଫେରନ୍ତୁ param_export_internal(filter);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା flash_param_import()
{
	ତାର୍କିକ legacy = ମିଥ୍ୟା;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା result = param_import_internal(&legacy);

	ଯଦି (result != 0) {
		ଫେରନ୍ତୁ result;
	}

	ଫେରନ୍ତୁ compact_after_import(legacy);
}
