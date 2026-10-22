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

#ប្រសិនបើ 0
# define debug(fmt, args...)            do { warnx(fmt, ##args); } ខណៈពេល(0)
#បើមិនដូច្នេះ
# define debug(fmt, args...)            do { } ខណៈពេល(0)
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
	សម្រាប់ (param_t param = 0; param < user_config.PARAM_COUNT; param++) {
		ប្រសិនបើ ((filter == nullptr || filter(param)) && param_value_unsaved(param)) {
			ត្រឡប់ true;
		}
	}

	ត្រឡប់ false;
}

/*
 * A snapshot holds every override in RAM, equal to the current default or
 * not: the boot compaction runs before the airframe applies its set-default
 * values, so at that point "equals the default" says nothing. A delta holds
 * the unsaved params; one that no longer overrides, or now matches the
 * default in force, is written as a BSON null so replay drops it, which is
 * what the file backend's "don't export defaults" gives after a reload.
 */
static ចំនួនគត់
encode_params(bson_encoder_s *encoder, param_filter_func filter, bool snapshot)
{
	សម្រាប់ (param_t param = 0; param < user_config.PARAM_COUNT; param++) {
		ប្រសិនបើ (filter && !filter(param)) {
			continue;
		}

		ប្រសិនបើ (snapshot) {
			ប្រសិនបើ (!user_config.contains(param)) {
				continue;
			}

		} បើមិនដូច្នេះ {
			ប្រសិនបើ (!param_value_unsaved(param)) {
				continue;
			}

			ប្រសិនបើ (!user_config.contains(param) || param_value_is_default(param)) {
				ប្រសិនបើ (bson_encoder_append_null(encoder, param_name(param))) {
					debug("BSON append failed for '%s'", param_name(param));
					ត្រឡប់ -1;
				}

				continue;
			}
		}

		switch (param_type(param)) {
		pak_sa PARAM_TYPE_INT32:
			ប្រសិនបើ (bson_encoder_append_int32(encoder, param_name(param), user_config.get(param).i)) {
				debug("BSON append failed for '%s'", param_name(param));
				ត្រឡប់ -1;
			}

			bha_nga;

		pak_sa PARAM_TYPE_FLOAT:
			ប្រសិនបើ (bson_encoder_append_double(encoder, param_name(param), (dviguu_na)user_config.get(param).f)) {
				debug("BSON append failed for '%s'", param_name(param));
				ត្រឡប់ -1;
			}

			bha_nga;

		default:
			debug("unrecognized parameter type");
			ត្រឡប់ -1;
		}
	}

	ត្រឡប់ 0;
}

/* The caller frees bson_encoder_buf_data() whatever the result. */
static ចំនួនគត់
encode_document(bson_encoder_s *encoder, param_filter_func filter, bool snapshot)
{
	bson_encoder_init_buf(encoder, nullptr, 0);

	ចំនួនគត់ result = encode_params(encoder, filter, snapshot);

	ប្រសិនបើ (result == 0) {
		result = bson_encoder_fini(encoder);
	}

	ត្រឡប់ result;
}

static ចំនួនគត់
stage_write_buffer(bson_encoder_s *encoder, uint8_t **buffer, size_t *enc_size)
{
	void *enc_buff = bson_encoder_buf_data(encoder);
	*enc_size = bson_encoder_buf_size(encoder);

	ប្រសិនបើ (enc_buff == nullptr) {
		ត្រឡប់ -ENOMEM;
	}

	size_t alloc_size = *enc_size;
	ចំនួនគត់ result = parameter_flashfs_alloc(parameters_token, buffer, &alloc_size);

	ប្រសិនបើ (result != OK) {
		ត្រឡប់ result;
	}

	ប្រសិនបើ (alloc_size < *enc_size) {
		parameter_flashfs_free();
		ត្រឡប់ -ENOMEM;
	}

	memcpy(*buffer, enc_buff, *enc_size);
	ត្រឡប់ OK;
}

static ចំនួនគត់
commit_append(uint8_t *buffer, size_t enc_size)
{
	ចំនួនគត់ result = parameter_flashfs_write(parameters_token, buffer, enc_size);
	parameter_flashfs_free();

	ប្រសិនបើ (result == (ចំនួនគត់)enc_size) {
		ត្រឡប់ OK;
	}

	ត្រឡប់ result < 0 ? result : -EFBIG;
}

static ចំនួនគត់
export_delta(param_filter_func filter)
{
	bson_encoder_s encoder{};
	ចំនួនគត់ result = encode_document(&encoder, filter, false);

	ប្រសិនបើ (result == 0) {
		uint8_t *buffer;
		size_t enc_size;
		result = stage_write_buffer(&encoder, &buffer, &enc_size);

		ប្រសិនបើ (result == OK) {
			result = commit_append(buffer, enc_size);
		}
	}

	free(bson_encoder_buf_data(&encoder));
	ត្រឡប់ result;
}

/* Everything that can fail before the erase, so a failure here leaves the
 * log untouched and still loadable. */
static ចំនួនគត់
stage_snapshot(bson_encoder_s *encoder, uint8_t **buffer, size_t *enc_size)
{
	ចំនួនគត់ result = stage_write_buffer(encoder, buffer, enc_size);

	ប្រសិនបើ (result != OK) {
		ត្រឡប់ result;
	}

	ប្រសិនបើ (*enc_size > parameter_flashfs_max_payload()) {
		parameter_flashfs_free();
		ត្រឡប់ -ENOSPC;
	}

	ត្រឡប់ OK;
}

static ចំនួនគត់
commit_snapshot(uint8_t *buffer, size_t enc_size)
{
	ប្រសិនបើ (parameter_flashfs_blank() != 1) {
		ចំនួនគត់ rv = parameter_flashfs_erase();

		ប្រសិនបើ (rv < 0) {
			parameter_flashfs_free();
			ត្រឡប់ rv;
		}
	}

	ចំនួនគត់ result = parameter_flashfs_write(parameters_token, buffer, enc_size);

	ប្រសិនបើ (result != (ចំនួនគត់)enc_size && parameter_flashfs_blank() == 1) {
		result = parameter_flashfs_write(parameters_token, buffer, enc_size);
	}

	parameter_flashfs_free();

	ប្រសិនបើ (result == (ចំនួនគត់)enc_size) {
		ត្រឡប់ OK;
	}

	ត្រឡប់ result < 0 ? result : -EFBIG;
}

static ចំនួនគត់
compact_to_snapshot()
{
	bson_encoder_s encoder{};
	ចំនួនគត់ result = encode_document(&encoder, nullptr, true);

	ប្រសិនបើ (result == 0) {
		uint8_t *buffer;
		size_t enc_size;
		result = stage_snapshot(&encoder, &buffer, &enc_size);

		ប្រសិនបើ (result == OK) {
			result = commit_snapshot(buffer, enc_size);
		}
	}

	ប្រសិនបើ (result == OK) {
		log_unreadable = false;
	}

	free(bson_encoder_buf_data(&encoder));
	ត្រឡប់ result;
}

static bool
has_record(flash_file_token_t token)
{
	uint8_t *buffer = nullptr;
	size_t buf_size = 0;
	ត្រឡប់ parameter_flashfs_read(token, &buffer, &buf_size) >= 0;
}

static ចំនួនគត់
param_export_internal(param_filter_func filter)
{
	/* A delta only means something behind a snapshot that replays. A legacy
	 * record still present means the boot migration did not run; replay
	 * would take it over any delta appended behind it. */
	ប្រសិនបើ (log_unreadable || has_record(parameters_legacy_token) || !has_record(parameters_token)) {
		ត្រឡប់ compact_to_snapshot();
	}

	ប្រសិនបើ (!any_unsaved(filter)) {
		ត្រឡប់ 0;
	}

	ចំនួនគត់ result = export_delta(filter);

	ប្រសិនបើ (result != -ENOSPC) {
		ត្រឡប់ result;
	}

	/* Same-bank erase stalls the CPU for ~1 s; last resort when a single
	 * save does not fit the tail. Compact writes the full RAM snapshot so a
	 * filtered save cannot drop params not in the delta. */
	PX4_WARN("flashparams: param sector full, compacting");
	ត្រឡប់ compact_to_snapshot();
}


static bool
record_is_stale(bson_node_t node)
{
	const param_t param = param_find_no_notification(node->name);

	ប្រសិនបើ (param == PARAM_INVALID) {
		ត្រឡប់ true;
	}

	switch (node->type) {
	pak_sa BSON_INT32:
		ត្រឡប់ param_type(param) != PARAM_TYPE_INT32;

	pak_sa BSON_DOUBLE:
		ត្រឡប់ param_type(param) != PARAM_TYPE_FLOAT;

	default:
		ត្រឡប់ false;
	}
}

static ចំនួនគត់
param_import_callback(bson_decoder_t decoder, bson_node_t node)
{
	float f;
	int32_t i;
	void *v = nullptr;
	ចំនួនគត់ result = -1;

	/*
	 * EOO means the end of the parameter object. (Currently not supporting
	 * nested BSON objects).
	 */
	ប្រសិនបើ (node->type == BSON_EOO) {
		debug("end of parameters");
		ត្រឡប់ 0;
	}

	/* A tombstone has no value. The decoder leaves the previous node's in
	 * the union and the translations read it without a type check. */
	const bool tombstone = node->type == BSON_nullptr || node->type == BSON_UNDEFINED;

	ប្រសិនបើ (!tombstone) {
		ប្រសិនបើ (record_is_stale(node)) {
			log_stale = true;
		}

		ប្រសិនបើ (param_modify_on_import(node) == param_modify_on_import_ret::PARAM_SKIP_IMPORT) {
			ត្រឡប់ 1;
		}
	}

	/*
	 * Find the parameter this node represents.  If we don't know it,
	 * ignore the node.
	 */
	param_t param = param_find_no_notification(node->name);

	ប្រសិនបើ (param == PARAM_INVALID) {
		debug("ignoring unrecognised parameter '%s'", node->name);
		ត្រឡប់ 1;
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
		ប្រសិនបើ (param_type(param) != PARAM_TYPE_INT32) {
			PX4_WARN("unexpected type for %s", node->name);
			result = 1; // just skip this entry
			goto out;
		}

		i = node->i32;
		v = &i;
		bha_nga;

	pak_sa BSON_DOUBLE:
		ប្រសិនបើ (param_type(param) != PARAM_TYPE_FLOAT) {
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

	ប្រសិនបើ (param_set_external(param, v, true, true)) {
		debug("error setting value for '%s'", node->name);
		goto out;
	}

	/* don't return zero, that means EOF */
	result = 1;

out:
	ត្រឡប់ result;
}

static ចំនួនគត់
import_one_entry(uint8_t *buffer, size_t buf_size, void *arg)
{
	ចំនួនគត់ *result = static_cast<ចំនួនគត់ *>(arg);
	bson_decoder_s decoder{};

	ប្រសិនបើ (bson_decoder_init_buf(&decoder, buffer, buf_size, param_import_callback)) {
		debug("decoder init failed");
		*result = -1;
		ត្រឡប់ -1;
	}

	do {
		*result = bson_decoder_next(&decoder);

	} ខណៈពេល (*result > 0);

	ត្រឡប់ *result < 0 ? *result : 0;
}

static ចំនួនគត់
param_import_internal(bool *legacy)
{
	ចំនួនគត់ result = 0;
	log_stale = false;
	ចំនួនគត់ n = parameter_flashfs_walk(parameters_legacy_token, import_one_entry, &result);
	*legacy = n > 0;

	ប្រសិនបើ (n == 0) {
		n = parameter_flashfs_walk(parameters_token, import_one_entry, &result);
	}

	ប្រសិនបើ (n < 0) {
		debug("flash walk failed (%d)", n);
		log_unreadable = true;
		ត្រឡប់ n;
	}

	ប្រសិនបើ (n == 0) {
		/* No valid entry found. A fully erased store is blank (first boot, or
		 * after switching firmware): report "not yet stored" (1), matching the
		 * convention used by param_load_default(). A store that holds data but
		 * no valid entry (torn write, bit-rot, or foreign data) is corrupt and
		 * must not be silently reseeded - report it so the boot recovery runs. */
		ប្រសិនបើ (parameter_flashfs_blank() == 1) {
			ត្រឡប់ 1;
		}

		debug("flash holds data but no valid entry");
		ត្រឡប់ -EILSEQ;
	}

	ប្រសិនបើ (result < 0) {
		debug("BSON error decoding parameters");
		ត្រឡប់ result;
	}

	ត្រឡប់ 0;
}

/*
 * Same-bank erase stalls instruction fetch for ~1 s on H7. Do it here at
 * boot, before sensors or DShot start, and only when a burst of deltas would
 * no longer fit, the store is still in the legacy layout, or the log holds
 * records this firmware translated or could not use. Only a failure after
 * the erase is an error: until then the log is intact and RAM holds what it
 * said, so the boot must not be reported as a corrupt store.
 */
static ចំនួនគត់
compact_after_import(bool legacy)
{
	bson_encoder_s encoder{};
	ចំនួនគត់ result = encode_document(&encoder, nullptr, true);

	ប្រសិនបើ (result != 0) {
		PX4_ERR("flashparams: snapshot encode failed (%d), log kept", result);
		free(bson_encoder_buf_data(&encoder));
		ត្រឡប់ 0;
	}

	const size_t enc_size = bson_encoder_buf_size(&encoder);
	ចំនួនគត់ need = (legacy || log_stale) ? 1 : parameter_flashfs_needs_compact(parameters_token, enc_size);
	result = 0;

	ប្រសិនបើ (need > 0) {
		uint8_t *buffer;
		size_t staged_size;
		result = stage_snapshot(&encoder, &buffer, &staged_size);

		ប្រសិនបើ (result != OK) {
			PX4_ERR("flashparams: cannot stage snapshot (%d), log kept", result);
			result = 0;

		} បើមិនដូច្នេះ {
			PX4_INFO("flashparams: compacting param sector%s", legacy ? " (legacy layout)" : "");
			result = commit_snapshot(buffer, staged_size);

			ប្រសិនបើ (result != OK) {
				PX4_ERR("flashparams: compact failed (%d)", result);
			}
		}
	}

	free(bson_encoder_buf_data(&encoder));
	ត្រឡប់ result;
}

ចំនួនគត់ flash_param_save(param_filter_func filter)
{
	ត្រឡប់ param_export_internal(filter);
}

ចំនួនគត់ flash_param_import()
{
	bool legacy = false;
	ចំនួនគត់ result = param_import_internal(&legacy);

	ប្រសិនបើ (result != 0) {
		ត្រឡប់ result;
	}

	ត្រឡប់ compact_after_import(legacy);
}
