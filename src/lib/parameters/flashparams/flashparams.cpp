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

#include <px4_platform_common/defines.h>
#include <px4_platform_common/log.h>
#include <px4_platform_common/posix.h>
#include <px4_platform_common/shutdown.h>
#include <parameters/px4_parameters.hpp>

#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <errno.h>

#include <parameters/param.h>

#include <lib/tinybson/tinybson.h>
#include "flashparams.h"
#include "flashfs.h"
#include "../param_translation.h"

#ຖ້າ 0
# define debug(fmt, args...)            do { warnx(fmt, ##args); } ໃນຂະນະທີ່(0)
#ບໍ່ດັ່ງນັ້ນ
# define debug(fmt, args...)            do { } ໃນຂະນະທີ່(0)
#endif


/**
 * Storage for modified parameters.
 */
sa_nracanaa param_wbuf_s {
	sa_ngha param_value_u     val;
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
static bool log_stale;

/* Replay failed inside a CRC-valid record. A delta appended behind it would
 * never be reached, so the next save rewrites the store. */
static bool log_unreadable;

static bool
any_unsaved(param_filter_func filter)
{
	ສຳລັບ (param_t param = 0; param < user_config.PARAM_COUNT; param++) {
		ຖ້າ ((filter == nullptr || filter(param)) && param_value_unsaved(param)) {
			ສົ່ງຄືນ true;
		}
	}

	ສົ່ງຄືນ false;
}

/*
 * A snapshot holds every override in RAM, equal to the current default or
 * not: the boot compaction runs before the airframe applies its set-default
 * values, so at that point "equals the default" says nothing. A delta holds
 * the unsaved params; one that no longer overrides, or now matches the
 * default in force, is written as a BSON null so replay drops it, which is
 * what the file backend's "don't export defaults" gives after a reload.
 */
static ຈຳນວນເຕັມ
encode_params(bson_encoder_s *encoder, param_filter_func filter, bool snapshot)
{
	ສຳລັບ (param_t param = 0; param < user_config.PARAM_COUNT; param++) {
		ຖ້າ (filter && !filter(param)) {
			continue;
		}

		ຖ້າ (snapshot) {
			ຖ້າ (!user_config.contains(param)) {
				continue;
			}

		} ບໍ່ດັ່ງນັ້ນ {
			ຖ້າ (!param_value_unsaved(param)) {
				continue;
			}

			ຖ້າ (!user_config.contains(param) || param_value_is_default(param)) {
				ຖ້າ (bson_encoder_append_null(encoder, param_name(param))) {
					debug("BSON append failed for '%s'", param_name(param));
					ສົ່ງຄືນ -1;
				}

				continue;
			}
		}

		switch (param_type(param)) {
		pak_sa PARAM_TYPE_INT32:
			ຖ້າ (bson_encoder_append_int32(encoder, param_name(param), user_config.get(param).i)) {
				debug("BSON append failed for '%s'", param_name(param));
				ສົ່ງຄືນ -1;
			}

			bha_nga;

		pak_sa PARAM_TYPE_FLOAT:
			ຖ້າ (bson_encoder_append_double(encoder, param_name(param), (dviguu_na)user_config.get(param).f)) {
				debug("BSON append failed for '%s'", param_name(param));
				ສົ່ງຄືນ -1;
			}

			bha_nga;

		default:
			debug("unrecognized parameter type");
			ສົ່ງຄືນ -1;
		}
	}

	ສົ່ງຄືນ 0;
}

/* The caller frees bson_encoder_buf_data() whatever the result. */
static ຈຳນວນເຕັມ
encode_document(bson_encoder_s *encoder, param_filter_func filter, bool snapshot)
{
	bson_encoder_init_buf(encoder, nullptr, 0);

	ຈຳນວນເຕັມ result = encode_params(encoder, filter, snapshot);

	ຖ້າ (result == 0) {
		result = bson_encoder_fini(encoder);
	}

	ສົ່ງຄືນ result;
}

static ຈຳນວນເຕັມ
stage_write_buffer(bson_encoder_s *encoder, uint8_t **buffer, size_t *enc_size)
{
	void *enc_buff = bson_encoder_buf_data(encoder);
	*enc_size = bson_encoder_buf_size(encoder);

	ຖ້າ (enc_buff == nullptr) {
		ສົ່ງຄືນ -ENOMEM;
	}

	size_t alloc_size = *enc_size;
	ຈຳນວນເຕັມ result = parameter_flashfs_alloc(parameters_token, buffer, &alloc_size);

	ຖ້າ (result != OK) {
		ສົ່ງຄືນ result;
	}

	ຖ້າ (alloc_size < *enc_size) {
		parameter_flashfs_free();
		ສົ່ງຄືນ -ENOMEM;
	}

	memcpy(*buffer, enc_buff, *enc_size);
	ສົ່ງຄືນ OK;
}

static ຈຳນວນເຕັມ
commit_append(uint8_t *buffer, size_t enc_size)
{
	ຈຳນວນເຕັມ result = parameter_flashfs_write(parameters_token, buffer, enc_size);
	parameter_flashfs_free();

	ຖ້າ (result == (ຈຳນວນເຕັມ)enc_size) {
		ສົ່ງຄືນ OK;
	}

	ສົ່ງຄືນ result < 0 ? result : -EFBIG;
}

static ຈຳນວນເຕັມ
export_delta(param_filter_func filter)
{
	bson_encoder_s encoder{};
	ຈຳນວນເຕັມ result = encode_document(&encoder, filter, false);

	ຖ້າ (result == 0) {
		uint8_t *buffer;
		size_t enc_size;
		result = stage_write_buffer(&encoder, &buffer, &enc_size);

		ຖ້າ (result == OK) {
			result = commit_append(buffer, enc_size);
		}
	}

	free(bson_encoder_buf_data(&encoder));
	ສົ່ງຄືນ result;
}

/* Everything that can fail before the erase, so a failure here leaves the
 * log untouched and still loadable. */
static ຈຳນວນເຕັມ
stage_snapshot(bson_encoder_s *encoder, uint8_t **buffer, size_t *enc_size)
{
	ຈຳນວນເຕັມ result = stage_write_buffer(encoder, buffer, enc_size);

	ຖ້າ (result != OK) {
		ສົ່ງຄືນ result;
	}

	ຖ້າ (*enc_size > parameter_flashfs_max_payload()) {
		parameter_flashfs_free();
		ສົ່ງຄືນ -ENOSPC;
	}

	ສົ່ງຄືນ OK;
}

static ຈຳນວນເຕັມ
commit_snapshot(uint8_t *buffer, size_t enc_size)
{
	ຖ້າ (parameter_flashfs_blank() != 1) {
		ຈຳນວນເຕັມ rv = parameter_flashfs_erase();

		ຖ້າ (rv < 0) {
			parameter_flashfs_free();
			ສົ່ງຄືນ rv;
		}
	}

	ຈຳນວນເຕັມ result = parameter_flashfs_write(parameters_token, buffer, enc_size);

	ຖ້າ (result != (ຈຳນວນເຕັມ)enc_size && parameter_flashfs_blank() == 1) {
		result = parameter_flashfs_write(parameters_token, buffer, enc_size);
	}

	parameter_flashfs_free();

	ຖ້າ (result == (ຈຳນວນເຕັມ)enc_size) {
		ສົ່ງຄືນ OK;
	}

	ສົ່ງຄືນ result < 0 ? result : -EFBIG;
}

static ຈຳນວນເຕັມ
compact_to_snapshot()
{
	bson_encoder_s encoder{};
	ຈຳນວນເຕັມ result = encode_document(&encoder, nullptr, true);

	ຖ້າ (result == 0) {
		uint8_t *buffer;
		size_t enc_size;
		result = stage_snapshot(&encoder, &buffer, &enc_size);

		ຖ້າ (result == OK) {
			result = commit_snapshot(buffer, enc_size);
		}
	}

	ຖ້າ (result == OK) {
		log_unreadable = false;
	}

	free(bson_encoder_buf_data(&encoder));
	ສົ່ງຄືນ result;
}

static bool
has_record(flash_file_token_t token)
{
	uint8_t *buffer = nullptr;
	size_t buf_size = 0;
	ສົ່ງຄືນ parameter_flashfs_read(token, &buffer, &buf_size) >= 0;
}

static ຈຳນວນເຕັມ
param_export_internal(param_filter_func filter)
{
	/* A delta only means something behind a snapshot that replays. A legacy
	 * record still present means the boot migration did not run; replay
	 * would take it over any delta appended behind it. */
	ຖ້າ (log_unreadable || has_record(parameters_legacy_token) || !has_record(parameters_token)) {
		ສົ່ງຄືນ compact_to_snapshot();
	}

	ຖ້າ (!any_unsaved(filter)) {
		ສົ່ງຄືນ 0;
	}

	ຈຳນວນເຕັມ result = export_delta(filter);

	ຖ້າ (result != -ENOSPC) {
		ສົ່ງຄືນ result;
	}

	/* Same-bank erase stalls the CPU for ~1 s; last resort when a single
	 * save does not fit the tail. Compact writes the full RAM snapshot so a
	 * filtered save cannot drop params not in the delta. */
	PX4_WARN("flashparams: param sector full, compacting");
	ສົ່ງຄືນ compact_to_snapshot();
}


static bool
record_is_stale(bson_node_t node)
{
	const param_t param = param_find_no_notification(node->name);

	ຖ້າ (param == PARAM_INVALID) {
		ສົ່ງຄືນ true;
	}

	switch (node->type) {
	pak_sa BSON_INT32:
		ສົ່ງຄືນ param_type(param) != PARAM_TYPE_INT32;

	pak_sa BSON_DOUBLE:
		ສົ່ງຄືນ param_type(param) != PARAM_TYPE_FLOAT;

	default:
		ສົ່ງຄືນ false;
	}
}

static ຈຳນວນເຕັມ
param_import_callback(bson_decoder_t decoder, bson_node_t node)
{
	float f;
	int32_t i;
	void *v = nullptr;
	ຈຳນວນເຕັມ result = -1;

	/*
	 * EOO means the end of the parameter object. (Currently not supporting
	 * nested BSON objects).
	 */
	ຖ້າ (node->type == BSON_EOO) {
		debug("end of parameters");
		ສົ່ງຄືນ 0;
	}

	/* A tombstone has no value. The decoder leaves the previous node's in
	 * the union and the translations read it without a type check. */
	const bool tombstone = node->type == BSON_nullptr || node->type == BSON_UNDEFINED;

	ຖ້າ (!tombstone) {
		ຖ້າ (record_is_stale(node)) {
			log_stale = true;
		}

		ຖ້າ (param_modify_on_import(node) == param_modify_on_import_ret::PARAM_SKIP_IMPORT) {
			ສົ່ງຄືນ 1;
		}
	}

	/*
	 * Find the parameter this node represents.  If we don't know it,
	 * ignore the node.
	 */
	param_t param = param_find_no_notification(node->name);

	ຖ້າ (param == PARAM_INVALID) {
		debug("ignoring unrecognised parameter '%s'", node->name);
		ສົ່ງຄືນ 1;
	}

	/*
	 * Handle setting the parameter from the node
	 */

	switch (node->type) {
	pak_sa BSON_nullptr:
	pak_sa BSON_UNDEFINED:
		user_config.reset(param);
		result = 1;
		goto out;

	pak_sa BSON_INT32:
		ຖ້າ (param_type(param) != PARAM_TYPE_INT32) {
			PX4_WARN("unexpected type for %s", node->name);
			result = 1; // just skip this entry
			goto out;
		}

		i = node->i32;
		v = &i;
		bha_nga;

	pak_sa BSON_DOUBLE:
		ຖ້າ (param_type(param) != PARAM_TYPE_FLOAT) {
			PX4_WARN("unexpected type for %s", node->name);
			result = 1; // just skip this entry
			goto out;
		}

		f = node->d;
		v = &f;
		bha_nga;

	default:
		PX4_ERR("%s unrecognised node type %d", node->name, node->type);
		result = 1; // just skip this entry
		goto out;
	}

	ຖ້າ (param_set_external(param, v, true, true)) {
		debug("error setting value for '%s'", node->name);
		goto out;
	}

	/* don't return zero, that means EOF */
	result = 1;

out:
	ສົ່ງຄືນ result;
}

static ຈຳນວນເຕັມ
import_one_entry(uint8_t *buffer, size_t buf_size, void *arg)
{
	ຈຳນວນເຕັມ *result = static_cast<ຈຳນວນເຕັມ *>(arg);
	bson_decoder_s decoder{};

	ຖ້າ (bson_decoder_init_buf(&decoder, buffer, buf_size, param_import_callback)) {
		debug("decoder init failed");
		*result = -1;
		ສົ່ງຄືນ -1;
	}

	do {
		*result = bson_decoder_next(&decoder);

	} ໃນຂະນະທີ່ (*result > 0);

	ສົ່ງຄືນ *result < 0 ? *result : 0;
}

static ຈຳນວນເຕັມ
param_import_internal(bool *legacy)
{
	ຈຳນວນເຕັມ result = 0;
	log_stale = false;
	ຈຳນວນເຕັມ n = parameter_flashfs_walk(parameters_legacy_token, import_one_entry, &result);
	*legacy = n > 0;

	ຖ້າ (n == 0) {
		n = parameter_flashfs_walk(parameters_token, import_one_entry, &result);
	}

	ຖ້າ (n < 0) {
		debug("flash walk failed (%d)", n);
		log_unreadable = true;
		ສົ່ງຄືນ n;
	}

	ຖ້າ (n == 0) {
		/* No valid entry found. A fully erased store is blank (first boot, or
		 * after switching firmware): report "not yet stored" (1), matching the
		 * convention used by param_load_default(). A store that holds data but
		 * no valid entry (torn write, bit-rot, or foreign data) is corrupt and
		 * must not be silently reseeded - report it so the boot recovery runs. */
		ຖ້າ (parameter_flashfs_blank() == 1) {
			ສົ່ງຄືນ 1;
		}

		debug("flash holds data but no valid entry");
		ສົ່ງຄືນ -EILSEQ;
	}

	ຖ້າ (result < 0) {
		debug("BSON error decoding parameters");
		ສົ່ງຄືນ result;
	}

	ສົ່ງຄືນ 0;
}

/*
 * Same-bank erase stalls instruction fetch for ~1 s on H7. Do it here at
 * boot, before sensors or DShot start, and only when a burst of deltas would
 * no longer fit, the store is still in the legacy layout, or the log holds
 * records this firmware translated or could not use. Only a failure after
 * the erase is an error: until then the log is intact and RAM holds what it
 * said, so the boot must not be reported as a corrupt store.
 */
static ຈຳນວນເຕັມ
compact_after_import(bool legacy)
{
	bson_encoder_s encoder{};
	ຈຳນວນເຕັມ result = encode_document(&encoder, nullptr, true);

	ຖ້າ (result != 0) {
		PX4_ERR("flashparams: snapshot encode failed (%d), log kept", result);
		free(bson_encoder_buf_data(&encoder));
		ສົ່ງຄືນ 0;
	}

	const size_t enc_size = bson_encoder_buf_size(&encoder);
	ຈຳນວນເຕັມ need = (legacy || log_stale) ? 1 : parameter_flashfs_needs_compact(parameters_token, enc_size);
	result = 0;

	ຖ້າ (need > 0) {
		uint8_t *buffer;
		size_t staged_size;
		result = stage_snapshot(&encoder, &buffer, &staged_size);

		ຖ້າ (result != OK) {
			PX4_ERR("flashparams: cannot stage snapshot (%d), log kept", result);
			result = 0;

		} ບໍ່ດັ່ງນັ້ນ {
			PX4_INFO("flashparams: compacting param sector%s", legacy ? " (legacy layout)" : "");
			result = commit_snapshot(buffer, staged_size);

			ຖ້າ (result != OK) {
				PX4_ERR("flashparams: compact failed (%d)", result);
			}
		}
	}

	free(bson_encoder_buf_data(&encoder));
	ສົ່ງຄືນ result;
}

ຈຳນວນເຕັມ flash_param_save(param_filter_func filter)
{
	ສົ່ງຄືນ param_export_internal(filter);
}

ຈຳນວນເຕັມ flash_param_import()
{
	bool legacy = false;
	ຈຳນວນເຕັມ result = param_import_internal(&legacy);

	ຖ້າ (result != 0) {
		ສົ່ງຄືນ result;
	}

	ສົ່ງຄືນ compact_after_import(legacy);
}
