/****************************************************************************
 *
 *   Copyright (c) 2012-2022 PX4 Development Team. All rights reserved.
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
 * @file parameters.cpp
 *
 * Global parameter store.
 *
 * Note that it might make sense to convert this into a driver.  That would
 * offer some interesting options regarding state for e.g. ORB advertisements
 * and background parameter saving.
 */

#define PARAM_IMPLEMENTATION
#include "param.h"
#include "param_translation.h"
#include <parameters/px4_parameters.hpp>
#include <lib/tinybson/tinybson.h>

#nếu defined(__PX4_NUTTX)
#include <nuttx/crc32.h>
#không_thì
#include <crc32.h>
#endif
#include <float.h>
#include <math.h>
#include <sys/stat.h>

#include <containers/Bitset.hpp>
#include <drivers/drv_hrt.h>
#include <lib/perf/perf_counter.h>
#include <px4_platform_common/px4_config.h>
#include <px4_platform_common/atomic_bitset.h>
#include <px4_platform_common/defines.h>
#include <px4_platform_common/posix.h>
#include <px4_platform_common/sem.h>
#include <px4_platform_common/shutdown.h>
#include <px4_platform_common/micro_hal.h>

using namespace time_literals;

#include "uORB/uORB.h"
#include "uORB/topics/parameter_update.h"
#include <uORB/Subscription.hpp>

#include "ExhaustiveLayer.h"
#include "ConstLayer.h"
#include "DynamicSparseLayer.h"
#include "StaticSparseLayer.h"

#include "atomic_transaction.h"

/* Include functions common to user and kernel sides */
#include "parameters_common.cpp"

#nếu defined(__PX4_NUTTX) && !defined(CONFIG_BUILD_FLAT)
#include <px4_platform/board_ctrl.h>
#include "parameters_ioctl.h"
#endif

#nếu defined(FLASH_BASED_PARAMS)
#include "flashparams/flashparams.h"
#không_thì
inline static số_nguyên flash_param_save(param_filter_func filter) { trả_về -1; }
inline static số_nguyên flash_param_import() { trả_về -1; }
#endif

static char *param_default_file = nullptr;
static char *param_backup_file = nullptr;

#include "autosave.h"
static ParamAutosave *autosave_instance {nullptr};

static px4::AtomicBitset<param_info_count> params_active;  // params found
static px4::AtomicBitset<param_info_count> params_unsaved;

// Params the save in progress is writing. They are moved out of params_unsaved
// before their values are read, so a param_set that lands while the save runs
// marks its param again and reaches the next save. Flash-param saves write only
// unsaved params, so nothing else would ever write that change.
static px4::AtomicBitset<param_info_count> params_saving;

static ConstLayer firmware_defaults;
static DynamicSparseLayer runtime_defaults{&firmware_defaults};
DynamicSparseLayer user_config{&runtime_defaults};

/** parameter update topic handle */
#nếu not defined(CONFIG_PARAM_REMOTE)
static orb_advert_t param_topic = nullptr;
static unsigned số_nguyên param_instance = 0;
#endif

static perf_counter_t param_export_perf;
static perf_counter_t param_find_perf;
static perf_counter_t param_get_perf;
static perf_counter_t param_set_perf;

static pthread_mutex_t file_mutex  =
	PTHREAD_MUTEX_INITIALIZER; ///< this protects against concurrent param saves (file or flash access).

// Support for remote parameter node
#nếu defined(CONFIG_PARAM_PRIMARY)
# include "parameters_primary.h"
#endif // CONFIG_PARAM_PRIMARY
#nếu defined(CONFIG_PARAM_REMOTE)
# include "parameters_remote.h"
#endif // CONFIG_PARAM_REMOTE

void
param_init()
{
#ifdef __PX4_POSIX
	// Set up the recursive param mutex before any thread can access parameters.
	AtomicTransaction::initialize();
#endif

	param_export_perf = perf_alloc(PC_ELAPSED, "param: export");
	param_find_perf = perf_alloc(PC_COUNT, "param: find");
	param_get_perf = perf_alloc(PC_COUNT, "param: get");
	param_set_perf = perf_alloc(PC_ELAPSED, "param: set");

#nếu defined(__PX4_NUTTX) && !defined(CONFIG_BUILD_FLAT)
	px4_register_boardct_ioctl(_PARAMIOCBASE, param_ioctl);
#endif

#nếu defined(CONFIG_PARAM_PRIMARY)
	param_primary_init();
#endif // CONFIG_PARAM_PRIMARY

#nếu defined(CONFIG_PARAM_REMOTE)
	param_remote_init();
#endif // CONFIG_PARAM_REMOTE

#nếu not defined(CONFIG_PARAM_REMOTE)
	autosave_instance = new ParamAutosave();
#endif
}


void
param_notify_changes()
{
// Don't send if this is a remote node. Only the primary
// sends out update notices
#nếu not defined(CONFIG_PARAM_REMOTE)
	parameter_update_s pup {};
	pup.instance = param_instance++;
	pup.get_count = perf_event_count(param_get_perf);
	pup.set_count = perf_event_count(param_set_perf);
	pup.find_count = perf_event_count(param_find_perf);
	pup.export_count = perf_event_count(param_export_perf);
	pup.active = params_active.count();
	pup.changed = user_config.size();
	pup.custom_default = runtime_defaults.size();
	pup.timestamp = hrt_absolute_time();

	nếu (param_topic == nullptr) {
		param_topic = orb_advertise(ORB_ID(parameter_update), &pup);

	} không_thì {
		orb_publish(ORB_ID(parameter_update), param_topic, &pup);
	}

#endif
}

static param_t param_find_internal(const char *name, bool notification)
{
	perf_count(param_find_perf);

	param_t middle;
	param_t front = 0;
	param_t last = param_info_count;

	/* perform a binary search of the known parameters */

	trong_khi (front <= last) {
		middle = front + (last - front) / 2;
		số_nguyên ret = strcmp(name, param_name(middle));

		nếu (ret == 0) {
			nếu (notification) {
				param_set_used(middle);
			}

			trả_về middle;

		} không_thì nếu (middle == front) {
			/* An end point has been hit, but there has been no match */
			break;

		} không_thì nếu (ret < 0) {
			last = middle;

		} không_thì {
			front = middle;
		}
	}

	/* not found */
	trả_về PARAM_INVALID;
}

param_t param_find(const char *name)
{
	trả_về param_find_internal(name, true);
}

param_t param_find_no_notification(const char *name)
{
	trả_về param_find_internal(name, false);
}

unsigned param_count_used()
{
	trả_về params_active.count();
}

param_t param_for_used_index(unsigned index)
{
	// walk all params and count used params
	nếu (index < param_info_count) {
		unsigned used_count = 0;

		cho (số_nguyên i = 0; i < params_active.size(); i++) {
			nếu (params_active[i]) {
				// we found the right used count,
				//  return the param value
				nếu (index == used_count) {
					trả_về static_cast<param_t>(i);
				}

				used_count++;
			}
		}
	}

	trả_về PARAM_INVALID;
}

số_nguyên param_get_used_index(param_t param)
{
	/* this tests for out of bounds and does a constant time lookup */
	nếu (!param_used(param)) {
		trả_về -1;
	}

	/* walk all params and count, now knowing that it has a valid index */
	số_nguyên used_count = 0;

	cho (số_nguyên i = 0; i < params_active.size(); i++) {
		nếu (params_active[i]) {

			nếu (param == i) {
				trả_về used_count;
			}

			used_count++;
		}
	}

	trả_về -1;
}

bool
param_value_unsaved(param_t param)
{
	trả_về handle_in_range(param) ? (params_unsaved[param] || params_saving[param]) : false;
}

số_nguyên
param_get(param_t param, void *val)
{
	perf_count(param_get_perf);

	nếu (!handle_in_range(param)) {
		PX4_ERR("get: param %" PRId16 " invalid", param);
		trả_về PX4_ERROR;
	}

	nếu (!params_active[param]) {
		PX4_DEBUG("get: param %" PRId16 " (%s) not active", param, param_name(param));
	}

	số_nguyên result = PX4_ERROR;

	nếu (val) {

		auto retrieve_value = user_config.get(param);

		switch (param_type(param)) {
		case PARAM_TYPE_INT32:
			memcpy(val, &retrieve_value.i, sizeof(retrieve_value.i));
			trả_về PX4_OK;

		case PARAM_TYPE_FLOAT:
			memcpy(val, &retrieve_value.f, sizeof(retrieve_value.f));
			trả_về PX4_OK;
		}
	}

	trả_về result;
}

số_nguyên
param_get_default_value_internal(param_t param, void *default_val)
{
	nếu (!handle_in_range(param)) {
		PX4_ERR("get default value: param %d invalid", param);
		trả_về PX4_ERROR;
	}

	nếu (default_val) {
		switch (param_type(param)) {
		case PARAM_TYPE_INT32: {
				int32_t val = runtime_defaults.get(param).i;
				memcpy(default_val, &val, sizeof(val));
				trả_về PX4_OK;
			}

		case PARAM_TYPE_FLOAT: {
				float val = runtime_defaults.get(param).f;
				memcpy(default_val, &val, sizeof(val));
				trả_về PX4_OK;
			}
		}
	}

	trả_về PX4_ERROR;
}

số_nguyên
param_get_default_value(param_t param, void *default_val)
{
	nếu (!handle_in_range(param)) {
		trả_về PX4_ERROR;
	}

	số_nguyên ret = param_get_default_value_internal(param, default_val);
	trả_về ret;
}

bool param_value_is_default(param_t param)
{
	nếu (!handle_in_range(param)) {
		trả_về true;
	}

	nếu (!user_config.contains(param)) {
		// if user config does not contain it, consider it default.
		trả_về true;

	} không_thì {
		// compare with default value
		const param_value_u user_config_value = user_config.get(param);
		const param_value_u runtime_default_value = runtime_defaults.get(param);

		switch (param_type(param)) {
		case PARAM_TYPE_INT32: {
				trả_về user_config_value.i == runtime_default_value.i;
			}

		case PARAM_TYPE_FLOAT: {
				trả_về fabsf(user_config_value.f - runtime_default_value.f) < FLT_EPSILON;
			}
		}
	}

	trả_về true;
}

static void
param_autosave()
{
	nếu (autosave_instance) {
		autosave_instance->request();
	}
}

void
param_control_autosave(bool enable)
{
	nếu (autosave_instance) {
		autosave_instance->enable(enable);
	}
}

static số_nguyên
param_set_internal(param_t param, const void *val, bool mark_saved, bool notify_changes, bool update_remote = true,
		   bool autosave = true)
{
	nếu (!handle_in_range(param)) {
		PX4_ERR("set invalid param %d", param);
		trả_về PX4_ERROR;
	}

	nếu (val == nullptr) {
		PX4_ERR("set invalid value");
		trả_về PX4_ERROR;
	}

	nếu (param_is_readonly(param)) {
		PX4_WARN("param %s is read-only", param_name(param));
		trả_về PX4_ERROR;
	}

	số_nguyên result = -1;
	bool param_changed = false;
	perf_begin(param_set_perf);

	const param_value_u user_config_value = user_config.get(param);
	param_value_u new_value{};

	switch (param_type(param)) {
	case PARAM_TYPE_INT32:
		memcpy(&new_value.i, val, sizeof(new_value.i));
		param_changed = user_config_value.i != new_value.i;
		break;

	case PARAM_TYPE_FLOAT:
		memcpy(&new_value.f, val, sizeof(new_value.f));
		param_changed = fabsf(user_config_value.f - new_value.f) > FLT_EPSILON;
		break;

	default: {
			PX4_ERR("param_set invalid param type for %s", param_name(param));
			break;
		}
	}

	nếu (user_config.store(param, new_value)) {
		nếu (param_changed) {
			params_unsaved.set(param, !mark_saved);
		}

		result = PX4_OK;

	} không_thì {
		PX4_ERR("param_set failed to store param %s", param_name(param));
		result = PX4_ERROR;
	}

	nếu ((result == PX4_OK) && param_changed && !mark_saved && autosave) { // this is false when importing parameters
		param_autosave();
	}

	// If this is the parameter server, make sure that the remote is updated
#nếu defined(CONFIG_PARAM_PRIMARY)

	nếu (param_changed && update_remote) {
		param_primary_set_value(param, val);
	}

#endif

	// If this is the parameter remote, make sure that the primary is updated
#nếu defined(CONFIG_PARAM_REMOTE)

	nếu (param_changed && update_remote) {
		param_remote_set_value(param, val);
	}

#endif

	perf_end(param_set_perf);

	/*
	 * If we set something, now that we have unlocked, go ahead and advertise that
	 * a thing has been set.
	 */
	nếu ((result == PX4_OK) && param_changed && notify_changes) {
		param_notify_changes();
	}

	trả_về result;
}

#nếu defined(FLASH_BASED_PARAMS)
số_nguyên param_set_external(param_t param, const void *val, bool mark_saved, bool notify_changes)
{
	trả_về param_set_internal(param, val, mark_saved, notify_changes);
}

void param_get_external(param_t param, void *val)
{
	param_get(param, val);
}
#endif

số_nguyên param_set(param_t param, const void *val)
{
	trả_về param_set_internal(param, val, false, true);
}

số_nguyên param_set_no_notification(param_t param, const void *val)
{
	trả_về param_set_internal(param, val, false, false);
}

số_nguyên param_set_no_remote_update(param_t param, const void *val, bool notify)
{
	trả_về param_set_internal(param, val, false, notify, false);
}

bool param_used(param_t param)
{
	nếu (handle_in_range(param)) {
		trả_về params_active[param];
	}

	trả_về false;
}

void param_set_used(param_t param)
{
	nếu (handle_in_range(param)) {
#nếu defined(CONFIG_PARAM_REMOTE)

		nếu (!param_used(param)) {
			param_remote_set_used(param);
		}

#endif

		params_active.set(param, true);
	}
}

số_nguyên param_set_default_value(param_t param, const void *val)
{
	nếu (!handle_in_range(param)) {
		PX4_ERR("set default value invalid param %d", param);
		trả_về PX4_ERROR;
	}

	nếu (val == nullptr) {
		PX4_ERR("set default value invalid value");
		trả_về PX4_ERROR;
	}

	nếu (param_is_readonly(param)) {
		PX4_WARN("param %s is read-only", param_name(param));
		trả_về PX4_ERROR;
	}

	số_nguyên result = PX4_ERROR;


	// check if param being set to default value
	bool setting_to_static_default = false;
	const param_value_u firmware_default_value = firmware_defaults.get(param);

	switch (param_type(param)) {
	case PARAM_TYPE_INT32: {
			int32_t new_value;
			memcpy(&new_value, val, sizeof(new_value));
			setting_to_static_default = firmware_default_value.i == new_value;
			break;
		}

	case PARAM_TYPE_FLOAT: {
			float new_value;
			memcpy(&new_value, val, sizeof(new_value));
			setting_to_static_default = fabsf(firmware_default_value.f - new_value) <= FLT_EPSILON;
			break;
		}
	}

	nếu (setting_to_static_default) {
		runtime_defaults.reset(param);

		result = PX4_OK;

	} không_thì {
		param_value_u new_value{};

		switch (param_type(param)) {
		case PARAM_TYPE_INT32: {
				memcpy(&new_value.i, val, sizeof(new_value.i));
				break;
			}

		case PARAM_TYPE_FLOAT: {
				memcpy(&new_value.f, val, sizeof(new_value.f));
				break;
			}

		default:
			break;
		}

		nếu (runtime_defaults.store(param, new_value)) {
			user_config.refresh(param);
			result = PX4_OK;

		} không_thì {
			result = PX4_ERROR;
		}
	}


	nếu ((result == PX4_OK) && param_used(param)) {
		// send notification if param is already in use
		param_notify_changes();
	}

	trả_về result;
}

static số_nguyên param_reset_internal(param_t param, bool notify = true, bool autosave = true)
{
#nếu defined(CONFIG_PARAM_REMOTE)
	// Remote doesn't support reset
	trả_về false;
#endif

	nếu (param_is_readonly(param)) {
		trả_về 0;  // silently skip — param_reset_all loops over all params
	}

	bool param_found = user_config.contains(param);

	nếu (handle_in_range(param)) {
		user_config.reset(param);
	}

	nếu (param_found) {
		/* Persist the reset. Flash-param saves append only unsaved params,
		 * so a reset has to show up as a tombstone or replay keeps the
		 * previous override. */
		params_unsaved.set(param, true);
	}

	nếu (autosave) {
		param_autosave();
	}

	nếu (param_found && notify) {
		param_notify_changes();
	}

#nếu defined(CONFIG_PARAM_PRIMARY)
	param_primary_reset(param);
#endif

	trả_về param_found;
}

số_nguyên param_reset(param_t param) { trả_về param_reset_internal(param, true); }
số_nguyên param_reset_no_notification(param_t param) { trả_về param_reset_internal(param, false); }

static void
param_reset_all_internal(bool auto_save)
{
#nếu defined(CONFIG_PARAM_REMOTE)
	// Remote doesn't support reset
	trả_về;
#endif

	cho (param_t param = 0; handle_in_range(param); param++) {
		param_reset_internal(param, false, false);
	}

	nếu (auto_save) {
		param_autosave();
	}

#nếu defined(CONFIG_PARAM_PRIMARY)
	param_primary_reset_all();
#endif

	param_notify_changes();
}

void
param_reset_all()
{
	param_reset_all_internal(true);
}

void
param_reset_excludes(const char *excludes[], số_nguyên num_excludes)
{
	cho (param_t param = 0; handle_in_range(param); param++) {
		const char *name = param_name(param);
		bool exclude = false;

		cho (số_nguyên index = 0; index < num_excludes; index ++) {
			số_nguyên len = strlen(excludes[index]);

			nếu ((excludes[index][len - 1] == '*'
			     && strncmp(name, excludes[index], len - 1) == 0)
			    || strcmp(name, excludes[index]) == 0) {

				exclude = true;
				break;
			}
		}

		nếu (!exclude) {
			param_reset(param);
		}
	}
}

void
param_reset_specific(const char *resets[], số_nguyên num_resets)
{
	cho (param_t param = 0; handle_in_range(param); param++) {
		const char *name = param_name(param);
		bool reset = false;

		cho (số_nguyên index = 0; index < num_resets; index++) {
			số_nguyên len = strlen(resets[index]);

			nếu ((resets[index][len - 1] == '*'
			     && strncmp(name, resets[index], len - 1) == 0)
			    || strcmp(name, resets[index]) == 0) {

				reset = true;
				break;
			}
		}

		nếu (reset) {
			param_reset(param);
		}
	}
}

số_nguyên
param_set_default_file(const char *filename)
{
	nếu (filename && param_backup_file && strcmp(filename, param_backup_file) == 0) {
		PX4_ERR("default file can't be the same as the backup file %s", filename);
		trả_về PX4_ERROR;
	}

#ifdef FLASH_BASED_PARAMS
	// the default for flash-based params is always the FLASH
	(void)filename;
#không_thì

	nếu (param_default_file != nullptr) {
		// we assume this is not in use by some other thread
		free(param_default_file);
		param_default_file = nullptr;
	}

	nếu (filename) {
		param_default_file = strdup(filename);
	}

#endif /* FLASH_BASED_PARAMS */

	trả_về 0;
}

const char *param_get_default_file()
{
	trả_về param_default_file;
}

số_nguyên param_set_backup_file(const char *filename)
{
	nếu (filename && param_default_file && strcmp(filename, param_default_file) == 0) {
		PX4_ERR("backup file can't be the same as the default file %s", filename);
		trả_về PX4_ERROR;
	}

	nếu (param_backup_file != nullptr) {
		// we assume this is not in use by some other thread
		free(param_backup_file);
		param_backup_file = nullptr;
	}

	nếu (filename) {
		param_backup_file = strdup(filename);

	} không_thì {
		param_backup_file = nullptr; // backup disabled
	}

	trả_về 0;
}

const char *param_get_backup_file()
{
	trả_về param_backup_file;
}

static số_nguyên param_export_internal(số_nguyên fd, param_filter_func filter);
static số_nguyên param_verify(số_nguyên fd);

số_nguyên param_save_default(bool blocking)
{
	PX4_DEBUG("param_save_default");

	// take the file lock
	nếu (blocking) {
		pthread_mutex_lock(&file_mutex);

	} không_thì {
		nếu (pthread_mutex_trylock(&file_mutex) != 0) {
			PX4_DEBUG("param_save_default: file lock failed (already locked)");
			trả_về -EWOULDBLOCK;
		}
	}

	số_nguyên shutdown_lock_ret = px4_shutdown_lock();

	nếu (shutdown_lock_ret != 0) {
		PX4_ERR("px4_shutdown_lock() failed (%i)", shutdown_lock_ret);
	}

	số_nguyên res = PX4_ERROR;
	const char *filename = param_get_default_file();

	cho (param_t param = 0; handle_in_range(param); param++) {
		nếu (params_unsaved[param]) {
			params_saving.set(param, true);
			params_unsaved.set(param, false);
		}
	}

	nếu (filename) {
		static constexpr số_nguyên MAX_ATTEMPTS = 3;

		cho (số_nguyên attempt = 1; attempt <= MAX_ATTEMPTS; attempt++) {
			// write parameters to file
			số_nguyên fd = ::open(filename, O_WRONLY | O_CREAT | O_TRUNC, PX4_O_MODE_666);

			nếu (fd > -1) {
				perf_begin(param_export_perf);
				res = param_export_internal(fd, nullptr);
				perf_end(param_export_perf);
				::close(fd);

				nếu (res == PX4_OK) {
					// reopen file to verify
					số_nguyên fd_verify = ::open(filename, O_RDONLY, PX4_O_MODE_666);
					res = param_verify(fd_verify) || lseek(fd_verify, 0, SEEK_SET) || param_verify(fd_verify);
					::close(fd_verify);
				}
			}

			nếu (res == PX4_OK) {
				break;

			} không_thì {
				PX4_ERR("parameter export to %s failed (%d) attempt %d", filename, res, attempt);
				px4_usleep(10000); // wait at least 10 milliseconds before trying again
			}
		}

	} không_thì {
		perf_begin(param_export_perf);
		res = flash_param_save(nullptr);
		perf_end(param_export_perf);
	}

	nếu (res != PX4_OK) {
		PX4_ERR("param export failed (%d)", res);

		cho (param_t param = 0; handle_in_range(param); param++) {
			nếu (params_saving[param]) {
				params_unsaved.set(param, true);
			}
		}
	}

	params_saving.reset();

	nếu (res == PX4_OK) {
		// backup file
		nếu (param_backup_file) {
			số_nguyên fd_backup_file = ::open(param_backup_file, O_WRONLY | O_CREAT | O_TRUNC, PX4_O_MODE_666);

			nếu (fd_backup_file > -1) {
				số_nguyên backup_export_ret = param_export_internal(fd_backup_file, nullptr);
				::close(fd_backup_file);

				nếu (backup_export_ret != 0) {
					PX4_ERR("backup parameter export to %s failed (%d)", param_backup_file, backup_export_ret);

				} không_thì {
					// verify export
					số_nguyên fd_verify = ::open(param_backup_file, O_RDONLY, PX4_O_MODE_666);
					param_verify(fd_verify);
					::close(fd_verify);
				}
			}
		}
	}

	pthread_mutex_unlock(&file_mutex);

	nếu (shutdown_lock_ret == 0) {
		px4_shutdown_unlock();
	}

	trả_về res;
}

/**
 * @return 0 on success, 1 if all params have not yet been stored, -1 if device open failed, -2 if writing parameters failed
 */
số_nguyên
param_load_default()
{
	số_nguyên res = 0;
	const char *filename = param_get_default_file();

	nếu (!filename) {
		trả_về param_load(-1);
	}

	số_nguyên fd_load = ::open(filename, O_RDONLY);

	nếu (fd_load < 0) {
		/* no parameter file is OK, otherwise this is an error */
		nếu (errno != ENOENT) {
			PX4_ERR("open '%s' for reading failed", filename);
			trả_về -1;
		}

		trả_về 1;
	}

	số_nguyên result = param_load(fd_load);
	::close(fd_load);

	nếu (result == 1) {
		/* blank file: nothing stored yet */
		trả_về 1;
	}

	nếu (result != 0) {
		PX4_ERR("error reading parameters from '%s'", filename);
		trả_về -2;
	}

	trả_về res;
}

static số_nguyên param_verify_callback(bson_decoder_t decoder, bson_node_t node)
{
	nếu (node->type == BSON_EOO) {
		trả_về 0;
	}

	// find the parameter this node represents
	param_t param = param_find_no_notification(node->name);

	nếu (param == PARAM_INVALID) {
		PX4_ERR("verify: invalid parameter '%s'", node->name);
		trả_về -1;
	}

	// handle verifying the parameter from the node
	switch (node->type) {
	case BSON_INT32: {
			nếu (param_type(param) != PARAM_TYPE_INT32) {
				PX4_ERR("verify: invalid param type %d for '%s' (BSON_INT32)", param_type(param), node->name);
				trả_về -1;
			}

			int32_t value;

			nếu (param_get(param, &value) == 0) {
				nếu (value == node->i32) {
					trả_về 1; // valid

				} không_thì {
					PX4_ERR("verify: '%s' invalid BSON value %" PRIi32 "(expected %" PRIi32 ")", node->name, node->i32, value);
				}
			}
		}
		break;

	case BSON_DOUBLE: {
			nếu (param_type(param) != PARAM_TYPE_FLOAT) {
				PX4_ERR("verify: invalid param type %d for '%s' (BSON_DOUBLE)", param_type(param), node->name);
				trả_về -1;
			}

			float value;

			nếu (param_get(param, &value) == 0) {
				nếu (fabsf(value - (float)node->d) <= FLT_EPSILON) {
					trả_về 1; // valid

				} không_thì {
					PX4_ERR("verify: '%s' invalid BSON value %.3f (expected %.3f)", node->name, node->d, (double)value);
				}
			}
		}
		break;

	default:
		PX4_ERR("verify: '%s' invalid node type %d", node->name, node->type);
	}

	trả_về -1;
}

static số_nguyên param_verify(số_nguyên fd)
{
	PX4_DEBUG("param_verify");

	nếu (fd < 0) {
		trả_về -1;
	}

	nếu (lseek(fd, 0, SEEK_SET) != 0) {
		PX4_ERR("verify: seek failed");
		trả_về -1;
	}

	bson_decoder_s decoder{};

	nếu (bson_decoder_init_file(&decoder, fd, param_verify_callback) == 0) {
		số_nguyên result = -1;

		do {
			result = bson_decoder_next(&decoder);

		} trong_khi (result > 0);

		nếu (result == 0) {
			nếu (decoder.total_document_size != decoder.total_decoded_size) {
				PX4_ERR("BSON document size (%" PRId32 ") doesn't match bytes decoded (%" PRId32 ")", decoder.total_document_size,
					decoder.total_decoded_size);

			} không_thì {
				trả_về 0;
			}

		} không_thì nếu (result == -ENODATA) {
			PX4_ERR("verify: no BSON data");

		} không_thì {
			PX4_ERR("verify: failed (%d)", result);
		}
	}

	trả_về -1;
}

số_nguyên
param_export(const char *filename, param_filter_func filter)
{
	PX4_DEBUG("param_export");

	số_nguyên shutdown_lock_ret = px4_shutdown_lock();

	nếu (shutdown_lock_ret != 0) {
		PX4_ERR("px4_shutdown_lock() failed (%i)", shutdown_lock_ret);
	}

	// take the file lock
	nếu (pthread_mutex_trylock(&file_mutex) != 0) {
		PX4_ERR("param_export: file lock failed (already locked)");
		trả_về PX4_ERROR;
	}

	số_nguyên fd = ::open(filename, O_RDWR | O_CREAT, PX4_O_MODE_666);
	số_nguyên result = PX4_ERROR;

	perf_begin(param_export_perf);

	nếu (fd > -1) {
		result = param_export_internal(fd, filter);

	} không_thì {
		result = flash_param_save(filter);
	}

	perf_end(param_export_perf);

	pthread_mutex_unlock(&file_mutex);

	nếu (shutdown_lock_ret == 0) {
		px4_shutdown_unlock();
	}

	trả_về result;
}

// internal parameter export, caller is responsible for locking
static số_nguyên param_export_internal(số_nguyên fd, param_filter_func filter)
{
	PX4_DEBUG("param_export_internal");
	const auto changed_params = user_config.containedAsBitset();

	số_nguyên result = -1;
	bson_encoder_s encoder{};
	uint8_t bson_buffer[256];

	nếu (lseek(fd, 0, SEEK_SET) != 0) {
		PX4_ERR("export seek failed %d", errno);
		trả_về -1;
	}

	nếu (bson_encoder_init_buf_file(&encoder, fd, &bson_buffer, sizeof(bson_buffer)) != 0) {
		goto out;
	}

	cho (param_t param = 0; handle_in_range(param); param++) {
		nếu (!changed_params[param] || (filter && !filter(param))) {
			continue;
		}

		const param_value_u runtime_default_value = runtime_defaults.get(param);
		const param_value_u user_config_value = user_config.get(param);

		// don't export default values
		switch (param_type(param)) {
		case PARAM_TYPE_INT32:
			nếu (user_config_value.i == runtime_default_value.i) {
				PX4_DEBUG("skipping %s %" PRIi32 " export", param_name(param), runtime_default_value.i);
				continue;
			}

			break;

		case PARAM_TYPE_FLOAT:
			nếu (fabsf(user_config_value.f - runtime_default_value.f) <= FLT_EPSILON) {
				PX4_DEBUG("skipping %s %.3f export", param_name(param), (double)runtime_default_value.f);
				continue;
			}

			break;
		}

		const char *name = param_name(param);
		const size_t size = param_size(param);

		/* append the appropriate BSON type object */
		switch (param_type(param)) {
		case PARAM_TYPE_INT32: {
				const int32_t i = user_config_value.i;
				PX4_DEBUG("exporting: %s (%d) size: %lu val: %" PRIi32, name, param, (long unsigned số_nguyên)size, i);

				nếu (bson_encoder_append_int32(&encoder, name, i) != 0) {
					PX4_ERR("BSON append failed for '%s'", name);
					goto out;
				}
			}
			break;

		case PARAM_TYPE_FLOAT: {
				const double f = (double)user_config_value.f;
				PX4_DEBUG("exporting: %s (%d) size: %lu val: %.3f", name, param, (long unsigned số_nguyên)size, (double)f);

				nếu (bson_encoder_append_double(&encoder, name, f) != 0) {
					PX4_ERR("BSON append failed for '%s'", name);
					goto out;
				}
			}
			break;

		default:
			PX4_ERR("%s unrecognized parameter type %d, skipping export", name, param_type(param));
		}
	}

	result = 0;

out:

	nếu (result == 0) {
		nếu (bson_encoder_fini(&encoder) != PX4_OK) {
			PX4_ERR("BSON encoder finalize failed");
			result = -1;
		}
	}

	trả_về result;
}

#nếu defined(FLASH_BASED_PARAMS)
// A file is never the flash backend's store, so what an import changes is unsaved until a delta carries it.
// As on the file backend, the import itself does not save.
static constexpr bool file_import_is_saved = false;
#không_thì
static constexpr bool file_import_is_saved = true;
#endif

static số_nguyên
param_import_callback(bson_decoder_t decoder, bson_node_t node)
{
	/*
	 * EOO means the end of the parameter object. (Currently not supporting
	 * nested BSON objects).
	 */
	nếu (node->type == BSON_EOO) {
		PX4_DEBUG("end of parameters");
		trả_về 0;
	}

	// A tombstone has no value. The decoder leaves the previous node's in the union and the translations
	// read it without a type check.
	const bool tombstone = node->type == BSON_nullptr || node->type == BSON_UNDEFINED;

	// if we do param_set() directly in the translation, set PARAM_SKIP_IMPORT as return value and return here
	nếu (!tombstone && param_modify_on_import(node) == param_modify_on_import_ret::PARAM_SKIP_IMPORT) {
		trả_về 1;
	}

	// Find the parameter this node represents.  If we don't know it, ignore the node.
	param_t param = param_find_no_notification(node->name);

	nếu (param == PARAM_INVALID) {
		PX4_WARN("ignoring unrecognised parameter '%s'", node->name);
		trả_về 1;
	}

	// Handle setting the parameter from the node
	switch (node->type) {
	case BSON_nullptr:
	case BSON_UNDEFINED:
		param_reset_internal(param, true, false);
		trả_về 1;

	case BSON_INT32: {
			nếu (param_type(param) == PARAM_TYPE_INT32) {
				int32_t i = node->i32;
				param_set_internal(param, &i, file_import_is_saved, true, true, false);
				PX4_DEBUG("Imported %s with value %" PRIi32, param_name(param), i);

			} không_thì {
				PX4_WARN("unexpected type for %s", node->name);
			}
		}
		break;

	case BSON_DOUBLE: {
			nếu (param_type(param) == PARAM_TYPE_FLOAT) {
				float f = node->d;
				param_set_internal(param, &f, file_import_is_saved, true, true, false);
				PX4_DEBUG("Imported %s with value %f", param_name(param), (double)f);

			} không_thì {
				PX4_WARN("unexpected type for %s", node->name);
			}
		}
		break;

	default:
		PX4_ERR("import: unrecognised node type for '%s'", node->name);
	}

	// don't return zero, that means EOF
	trả_về 1;
}

static số_nguyên
param_import_internal(số_nguyên fd)
{
	/* blank source (nothing stored yet): an empty file, or zeroed/erased
	 * storage where the BSON document length would be. Unlike the flash
	 * backend, which scans the whole store, this only inspects the leading
	 * document length - a length of 0 or -1 can never start a valid BSON
	 * document, so nothing parseable is diverted here regardless of what
	 * follows. */

	/* A zero-length regular file is blank. NuttX FAT can't read() a 0-byte
	 * file (no cluster chain -> error, not EOF), so the content check below
	 * would miss it; detect it by size. Char-device backends (e.g. FRAM)
	 * report no meaningful size and fall through to the content check. */
	struct stat st;

	nếu (fstat(fd, &st) == 0 && S_ISREG(st.st_mode) && st.st_size == 0) {
		trả_về 1;
	}

	int32_t doc_size = 0;
	ssize_t len = read(fd, &doc_size, sizeof(doc_size));

	nếu (len == 0 || (len == (ssize_t)sizeof(doc_size) && (doc_size == 0 || doc_size == -1))) {
		trả_về 1;
	}

	nếu (lseek(fd, 0, SEEK_SET) != 0) {
		PX4_ERR("import lseek failed (%d)", errno);
		trả_về -1;
	}

	static constexpr số_nguyên MAX_ATTEMPTS = 3;

	cho (số_nguyên attempt = 1; attempt <= MAX_ATTEMPTS; attempt++) {
		bson_decoder_s decoder{};

		nếu (bson_decoder_init_file(&decoder, fd, param_import_callback) == 0) {
			số_nguyên result = -1;

			do {
				result = bson_decoder_next(&decoder);

			} trong_khi (result > 0);

			nếu (result == 0) {
				nếu (decoder.total_document_size == decoder.total_decoded_size) {
					PX4_INFO("BSON document size %" PRId32 " bytes, decoded %" PRId32 " bytes (INT32:%" PRIu16 ", FLOAT:%" PRIu16 ")",
						 decoder.total_document_size, decoder.total_decoded_size,
						 decoder.count_node_int32, decoder.count_node_double);

					trả_về 0;

				} không_thì {
					PX4_ERR("BSON document size (%" PRId32 ") doesn't match bytes decoded (%" PRId32 ")",
						decoder.total_document_size, decoder.total_decoded_size);
				}

			} không_thì nếu (result == -ENODATA) {
				// silently retry as a precaution unless this is our last attempt
				nếu (attempt == MAX_ATTEMPTS) {
					PX4_DEBUG("BSON: no data");
					trả_về 0;
				}

			} không_thì {
				PX4_ERR("param import failed (%d) attempt %d", result, attempt);
			}

		} không_thì {
			PX4_ERR("param import bson decoder init failed (attempt %d)", attempt);
		}

		nếu (attempt != MAX_ATTEMPTS) {
			nếu (lseek(fd, 0, SEEK_SET) != 0) {
				PX4_ERR("import lseek failed (%d)", errno);
			}

			px4_usleep(10000); // wait at least 10 milliseconds before trying again
		}
	}

	trả_về -1;
}

#nếu defined(FLASH_BASED_PARAMS)
static số_nguyên flash_backend_locked(số_nguyên (*op)())
{
	/* Same order as param_save_default: mutex then shutdown lock. Autosave
	 * uses a trylock, so a compact holds it off the flash programming. */
	pthread_mutex_lock(&file_mutex);

	số_nguyên shutdown_lock_ret = px4_shutdown_lock();

	nếu (shutdown_lock_ret != 0) {
		PX4_ERR("px4_shutdown_lock() failed (%i)", shutdown_lock_ret);
	}

	số_nguyên result = op();

	pthread_mutex_unlock(&file_mutex);

	nếu (shutdown_lock_ret == 0) {
		px4_shutdown_unlock();
	}

	trả_về result;
}

static số_nguyên flash_load_locked()
{
	/* Reset under the lock so a pending autosave cannot append tombstones
	 * of the just-cleared RAM before import/compact run. */
	param_reset_all_internal(false);
	số_nguyên result = flash_param_import();

	nếu (result == 0 || result == 1) {
		params_unsaved.reset();
	}

	trả_về result;
}
#endif

số_nguyên
param_import(số_nguyên fd)
{
	nếu (fd < 0) {
#nếu defined(FLASH_BASED_PARAMS)
		trả_về flash_backend_locked(flash_param_import);
#không_thì
		trả_về flash_param_import();
#endif
	}

	trả_về param_import_internal(fd);
}

số_nguyên
param_load(số_nguyên fd)
{
	nếu (fd < 0) {
#nếu defined(FLASH_BASED_PARAMS)
		trả_về flash_backend_locked(flash_load_locked);
#không_thì
		trả_về PX4_ERROR;
#endif
	}

	param_reset_all_internal(false);
	số_nguyên result = param_import_internal(fd);

#nếu !defined(FLASH_BASED_PARAMS)

	nếu (result >= 0) {
		params_unsaved.reset();
	}

#endif

	trả_về result;
}

void
param_foreach(void (*func)(void *arg, param_t param), void *arg, bool only_changed, bool only_used)
{
	param_t	param;

	cho (param = 0; handle_in_range(param); param++) {

		/* if requested, skip unchanged values */
		nếu (only_changed && (!user_config.contains(param))) {
			continue;
		}

		nếu (only_used && !param_used(param)) {
			continue;
		}

		func(arg, param);
	}
}

uint32_t param_hash_check()
{
	uint32_t param_hash = 0;

	/* compute the CRC32 over all string param names and 4 byte values */
	cho (param_t param = 0; handle_in_range(param); param++) {
		nếu (!param_used(param) || param_is_volatile(param)) {
			continue;
		}

		const char *name = param_name(param);
		auto value = user_config.get(param).i;
		const void *val = (void *)&value;
		param_hash = crc32part((const uint8_t *)name, strlen(name), param_hash);
		param_hash = crc32part((const uint8_t *)val, param_size(param), param_hash);
	}

	trả_về param_hash;
}

void param_print_status()
{
	PX4_INFO("summary: %d/%d (used/total)", param_count_used(), param_count());

#ifndef FLASH_BASED_PARAMS
	const char *filename = param_get_default_file();

	nếu (filename != nullptr) {
		PX4_INFO("file: %s", param_get_default_file());
	}

	nếu (param_backup_file) {
		PX4_INFO("backup file: %s", param_backup_file);
	}

#endif /* FLASH_BASED_PARAMS */

	PX4_INFO("storage array: %d/%d elements (%zu bytes total)",
		 user_config.size(), firmware_defaults.size(), (size_t)user_config.byteSize());


	PX4_INFO("storage array (custom defaults): %d/%d elements (%zu bytes total)",
		 runtime_defaults.size(), firmware_defaults.size(), (size_t)runtime_defaults.byteSize());

	nếu (autosave_instance) {
		PX4_INFO("auto save: %s", autosave_instance->enabled() ? "on" : "off");

		hrt_abstime last_autosave = autosave_instance->lastAutosave();

		nếu (last_autosave > 0) {
			PX4_INFO("last auto save: %.3f seconds ago", hrt_elapsed_time(&last_autosave) * 1e-6);
		}
	}

	perf_print_counter(param_export_perf);
	perf_print_counter(param_find_perf);
	perf_print_counter(param_get_perf);
	perf_print_counter(param_set_perf);

#nếu defined(CONFIG_PARAM_PRIMARY)
	struct param_primary_counters counts;
	param_primary_get_counters(&counts);
	PX4_INFO("set value requests received: %" PRIu32 ", set value responses sent: %" PRIu32,
		 counts.set_value_request_received, counts.set_value_response_sent);
	PX4_INFO("set value requests sent: %" PRIu32 ", set value responses received: %" PRIu32,
		 counts.set_value_request_sent, counts.set_value_response_received);
	PX4_INFO("resets sent: %" PRIu32 ", set used requests received: %" PRIu32,
		 counts.reset_sent, counts.set_used_received);
#endif

#nếu defined(CONFIG_PARAM_REMOTE)
	struct param_remote_counters counts;
	param_remote_get_counters(&counts);
	PX4_INFO("set value requests received: %" PRIu32 ", set value responses sent: %" PRIu32,
		 counts.set_value_request_received, counts.set_value_response_sent);
	PX4_INFO("set value requests sent: %" PRIu32 ", set value responses received: %" PRIu32,
		 counts.set_value_request_sent, counts.set_value_response_received);
	PX4_INFO("resets received: %" PRIu32 ", set used requests sent: %" PRIu32,
		 counts.reset_received, counts.set_used_sent);
#endif

}
