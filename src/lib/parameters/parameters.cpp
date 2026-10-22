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

#ако defined(__PX4_NUTTX)
#include <nuttx/crc32.h>
#инако
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

#ако defined(__PX4_NUTTX) && !defined(CONFIG_BUILD_FLAT)
#include <px4_platform/board_ctrl.h>
#include "parameters_ioctl.h"
#endif

#ако defined(FLASH_BASED_PARAMS)
#include "flashparams/flashparams.h"
#инако
inline static цео flash_param_save(param_filter_func filter) { врати -1; }
inline static цео flash_param_import() { врати -1; }
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
#ако not defined(CONFIG_PARAM_REMOTE)
static orb_advert_t param_topic = nullptr;
static unsigned цео param_instance = 0;
#endif

static perf_counter_t param_export_perf;
static perf_counter_t param_find_perf;
static perf_counter_t param_get_perf;
static perf_counter_t param_set_perf;

static pthread_mutex_t file_mutex  =
	PTHREAD_MUTEX_INITIALIZER; ///< this protects against concurrent param saves (file or flash access).

// Support for remote parameter node
#ако defined(CONFIG_PARAM_PRIMARY)
# include "parameters_primary.h"
#endif // CONFIG_PARAM_PRIMARY
#ако defined(CONFIG_PARAM_REMOTE)
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

#ако defined(__PX4_NUTTX) && !defined(CONFIG_BUILD_FLAT)
	px4_register_boardct_ioctl(_PARAMIOCBASE, param_ioctl);
#endif

#ако defined(CONFIG_PARAM_PRIMARY)
	param_primary_init();
#endif // CONFIG_PARAM_PRIMARY

#ако defined(CONFIG_PARAM_REMOTE)
	param_remote_init();
#endif // CONFIG_PARAM_REMOTE

#ако not defined(CONFIG_PARAM_REMOTE)
	autosave_instance = new ParamAutosave();
#endif
}


void
param_notify_changes()
{
// Don't send if this is a remote node. Only the primary
// sends out update notices
#ако not defined(CONFIG_PARAM_REMOTE)
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

	ако (param_topic == nullptr) {
		param_topic = orb_advertise(ORB_ID(parameter_update), &pup);

	} инако {
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

	док (front <= last) {
		middle = front + (last - front) / 2;
		цео ret = strcmp(name, param_name(middle));

		ако (ret == 0) {
			ако (notification) {
				param_set_used(middle);
			}

			врати middle;

		} инако ако (middle == front) {
			/* An end point has been hit, but there has been no match */
			break;

		} инако ако (ret < 0) {
			last = middle;

		} инако {
			front = middle;
		}
	}

	/* not found */
	врати PARAM_INVALID;
}

param_t param_find(const char *name)
{
	врати param_find_internal(name, true);
}

param_t param_find_no_notification(const char *name)
{
	врати param_find_internal(name, false);
}

unsigned param_count_used()
{
	врати params_active.count();
}

param_t param_for_used_index(unsigned index)
{
	// walk all params and count used params
	ако (index < param_info_count) {
		unsigned used_count = 0;

		за (цео i = 0; i < params_active.size(); i++) {
			ако (params_active[i]) {
				// we found the right used count,
				//  return the param value
				ако (index == used_count) {
					врати static_cast<param_t>(i);
				}

				used_count++;
			}
		}
	}

	врати PARAM_INVALID;
}

цео param_get_used_index(param_t param)
{
	/* this tests for out of bounds and does a constant time lookup */
	ако (!param_used(param)) {
		врати -1;
	}

	/* walk all params and count, now knowing that it has a valid index */
	цео used_count = 0;

	за (цео i = 0; i < params_active.size(); i++) {
		ако (params_active[i]) {

			ако (param == i) {
				врати used_count;
			}

			used_count++;
		}
	}

	врати -1;
}

bool
param_value_unsaved(param_t param)
{
	врати handle_in_range(param) ? (params_unsaved[param] || params_saving[param]) : false;
}

цео
param_get(param_t param, void *val)
{
	perf_count(param_get_perf);

	ако (!handle_in_range(param)) {
		PX4_ERR("get: param %" PRId16 " invalid", param);
		врати PX4_ERROR;
	}

	ако (!params_active[param]) {
		PX4_DEBUG("get: param %" PRId16 " (%s) not active", param, param_name(param));
	}

	цео result = PX4_ERROR;

	ако (val) {

		auto retrieve_value = user_config.get(param);

		switch (param_type(param)) {
		case PARAM_TYPE_INT32:
			memcpy(val, &retrieve_value.i, sizeof(retrieve_value.i));
			врати PX4_OK;

		case PARAM_TYPE_FLOAT:
			memcpy(val, &retrieve_value.f, sizeof(retrieve_value.f));
			врати PX4_OK;
		}
	}

	врати result;
}

цео
param_get_default_value_internal(param_t param, void *default_val)
{
	ако (!handle_in_range(param)) {
		PX4_ERR("get default value: param %d invalid", param);
		врати PX4_ERROR;
	}

	ако (default_val) {
		switch (param_type(param)) {
		case PARAM_TYPE_INT32: {
				int32_t val = runtime_defaults.get(param).i;
				memcpy(default_val, &val, sizeof(val));
				врати PX4_OK;
			}

		case PARAM_TYPE_FLOAT: {
				float val = runtime_defaults.get(param).f;
				memcpy(default_val, &val, sizeof(val));
				врати PX4_OK;
			}
		}
	}

	врати PX4_ERROR;
}

цео
param_get_default_value(param_t param, void *default_val)
{
	ако (!handle_in_range(param)) {
		врати PX4_ERROR;
	}

	цео ret = param_get_default_value_internal(param, default_val);
	врати ret;
}

bool param_value_is_default(param_t param)
{
	ако (!handle_in_range(param)) {
		врати true;
	}

	ако (!user_config.contains(param)) {
		// if user config does not contain it, consider it default.
		врати true;

	} инако {
		// compare with default value
		const param_value_u user_config_value = user_config.get(param);
		const param_value_u runtime_default_value = runtime_defaults.get(param);

		switch (param_type(param)) {
		case PARAM_TYPE_INT32: {
				врати user_config_value.i == runtime_default_value.i;
			}

		case PARAM_TYPE_FLOAT: {
				врати fabsf(user_config_value.f - runtime_default_value.f) < FLT_EPSILON;
			}
		}
	}

	врати true;
}

static void
param_autosave()
{
	ако (autosave_instance) {
		autosave_instance->request();
	}
}

void
param_control_autosave(bool enable)
{
	ако (autosave_instance) {
		autosave_instance->enable(enable);
	}
}

static цео
param_set_internal(param_t param, const void *val, bool mark_saved, bool notify_changes, bool update_remote = true,
		   bool autosave = true)
{
	ако (!handle_in_range(param)) {
		PX4_ERR("set invalid param %d", param);
		врати PX4_ERROR;
	}

	ако (val == nullptr) {
		PX4_ERR("set invalid value");
		врати PX4_ERROR;
	}

	ако (param_is_readonly(param)) {
		PX4_WARN("param %s is read-only", param_name(param));
		врати PX4_ERROR;
	}

	цео result = -1;
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

	ако (user_config.store(param, new_value)) {
		ако (param_changed) {
			params_unsaved.set(param, !mark_saved);
		}

		result = PX4_OK;

	} инако {
		PX4_ERR("param_set failed to store param %s", param_name(param));
		result = PX4_ERROR;
	}

	ако ((result == PX4_OK) && param_changed && !mark_saved && autosave) { // this is false when importing parameters
		param_autosave();
	}

	// If this is the parameter server, make sure that the remote is updated
#ако defined(CONFIG_PARAM_PRIMARY)

	ако (param_changed && update_remote) {
		param_primary_set_value(param, val);
	}

#endif

	// If this is the parameter remote, make sure that the primary is updated
#ако defined(CONFIG_PARAM_REMOTE)

	ако (param_changed && update_remote) {
		param_remote_set_value(param, val);
	}

#endif

	perf_end(param_set_perf);

	/*
	 * If we set something, now that we have unlocked, go ahead and advertise that
	 * a thing has been set.
	 */
	ако ((result == PX4_OK) && param_changed && notify_changes) {
		param_notify_changes();
	}

	врати result;
}

#ако defined(FLASH_BASED_PARAMS)
цео param_set_external(param_t param, const void *val, bool mark_saved, bool notify_changes)
{
	врати param_set_internal(param, val, mark_saved, notify_changes);
}

void param_get_external(param_t param, void *val)
{
	param_get(param, val);
}
#endif

цео param_set(param_t param, const void *val)
{
	врати param_set_internal(param, val, false, true);
}

цео param_set_no_notification(param_t param, const void *val)
{
	врати param_set_internal(param, val, false, false);
}

цео param_set_no_remote_update(param_t param, const void *val, bool notify)
{
	врати param_set_internal(param, val, false, notify, false);
}

bool param_used(param_t param)
{
	ако (handle_in_range(param)) {
		врати params_active[param];
	}

	врати false;
}

void param_set_used(param_t param)
{
	ако (handle_in_range(param)) {
#ако defined(CONFIG_PARAM_REMOTE)

		ако (!param_used(param)) {
			param_remote_set_used(param);
		}

#endif

		params_active.set(param, true);
	}
}

цео param_set_default_value(param_t param, const void *val)
{
	ако (!handle_in_range(param)) {
		PX4_ERR("set default value invalid param %d", param);
		врати PX4_ERROR;
	}

	ако (val == nullptr) {
		PX4_ERR("set default value invalid value");
		врати PX4_ERROR;
	}

	ако (param_is_readonly(param)) {
		PX4_WARN("param %s is read-only", param_name(param));
		врати PX4_ERROR;
	}

	цео result = PX4_ERROR;


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

	ако (setting_to_static_default) {
		runtime_defaults.reset(param);

		result = PX4_OK;

	} инако {
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

		ако (runtime_defaults.store(param, new_value)) {
			user_config.refresh(param);
			result = PX4_OK;

		} инако {
			result = PX4_ERROR;
		}
	}


	ако ((result == PX4_OK) && param_used(param)) {
		// send notification if param is already in use
		param_notify_changes();
	}

	врати result;
}

static цео param_reset_internal(param_t param, bool notify = true, bool autosave = true)
{
#ако defined(CONFIG_PARAM_REMOTE)
	// Remote doesn't support reset
	врати false;
#endif

	ако (param_is_readonly(param)) {
		врати 0;  // silently skip — param_reset_all loops over all params
	}

	bool param_found = user_config.contains(param);

	ако (handle_in_range(param)) {
		user_config.reset(param);
	}

	ако (param_found) {
		/* Persist the reset. Flash-param saves append only unsaved params,
		 * so a reset has to show up as a tombstone or replay keeps the
		 * previous override. */
		params_unsaved.set(param, true);
	}

	ако (autosave) {
		param_autosave();
	}

	ако (param_found && notify) {
		param_notify_changes();
	}

#ако defined(CONFIG_PARAM_PRIMARY)
	param_primary_reset(param);
#endif

	врати param_found;
}

цео param_reset(param_t param) { врати param_reset_internal(param, true); }
цео param_reset_no_notification(param_t param) { врати param_reset_internal(param, false); }

static void
param_reset_all_internal(bool auto_save)
{
#ако defined(CONFIG_PARAM_REMOTE)
	// Remote doesn't support reset
	врати;
#endif

	за (param_t param = 0; handle_in_range(param); param++) {
		param_reset_internal(param, false, false);
	}

	ако (auto_save) {
		param_autosave();
	}

#ако defined(CONFIG_PARAM_PRIMARY)
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
param_reset_excludes(const char *excludes[], цео num_excludes)
{
	за (param_t param = 0; handle_in_range(param); param++) {
		const char *name = param_name(param);
		bool exclude = false;

		за (цео index = 0; index < num_excludes; index ++) {
			цео len = strlen(excludes[index]);

			ако ((excludes[index][len - 1] == '*'
			     && strncmp(name, excludes[index], len - 1) == 0)
			    || strcmp(name, excludes[index]) == 0) {

				exclude = true;
				break;
			}
		}

		ако (!exclude) {
			param_reset(param);
		}
	}
}

void
param_reset_specific(const char *resets[], цео num_resets)
{
	за (param_t param = 0; handle_in_range(param); param++) {
		const char *name = param_name(param);
		bool reset = false;

		за (цео index = 0; index < num_resets; index++) {
			цео len = strlen(resets[index]);

			ако ((resets[index][len - 1] == '*'
			     && strncmp(name, resets[index], len - 1) == 0)
			    || strcmp(name, resets[index]) == 0) {

				reset = true;
				break;
			}
		}

		ако (reset) {
			param_reset(param);
		}
	}
}

цео
param_set_default_file(const char *filename)
{
	ако (filename && param_backup_file && strcmp(filename, param_backup_file) == 0) {
		PX4_ERR("default file can't be the same as the backup file %s", filename);
		врати PX4_ERROR;
	}

#ifdef FLASH_BASED_PARAMS
	// the default for flash-based params is always the FLASH
	(void)filename;
#инако

	ако (param_default_file != nullptr) {
		// we assume this is not in use by some other thread
		free(param_default_file);
		param_default_file = nullptr;
	}

	ако (filename) {
		param_default_file = strdup(filename);
	}

#endif /* FLASH_BASED_PARAMS */

	врати 0;
}

const char *param_get_default_file()
{
	врати param_default_file;
}

цео param_set_backup_file(const char *filename)
{
	ако (filename && param_default_file && strcmp(filename, param_default_file) == 0) {
		PX4_ERR("backup file can't be the same as the default file %s", filename);
		врати PX4_ERROR;
	}

	ако (param_backup_file != nullptr) {
		// we assume this is not in use by some other thread
		free(param_backup_file);
		param_backup_file = nullptr;
	}

	ако (filename) {
		param_backup_file = strdup(filename);

	} инако {
		param_backup_file = nullptr; // backup disabled
	}

	врати 0;
}

const char *param_get_backup_file()
{
	врати param_backup_file;
}

static цео param_export_internal(цео fd, param_filter_func filter);
static цео param_verify(цео fd);

цео param_save_default(bool blocking)
{
	PX4_DEBUG("param_save_default");

	// take the file lock
	ако (blocking) {
		pthread_mutex_lock(&file_mutex);

	} инако {
		ако (pthread_mutex_trylock(&file_mutex) != 0) {
			PX4_DEBUG("param_save_default: file lock failed (already locked)");
			врати -EWOULDBLOCK;
		}
	}

	цео shutdown_lock_ret = px4_shutdown_lock();

	ако (shutdown_lock_ret != 0) {
		PX4_ERR("px4_shutdown_lock() failed (%i)", shutdown_lock_ret);
	}

	цео res = PX4_ERROR;
	const char *filename = param_get_default_file();

	за (param_t param = 0; handle_in_range(param); param++) {
		ако (params_unsaved[param]) {
			params_saving.set(param, true);
			params_unsaved.set(param, false);
		}
	}

	ако (filename) {
		static constexpr цео MAX_ATTEMPTS = 3;

		за (цео attempt = 1; attempt <= MAX_ATTEMPTS; attempt++) {
			// write parameters to file
			цео fd = ::open(filename, O_WRONLY | O_CREAT | O_TRUNC, PX4_O_MODE_666);

			ако (fd > -1) {
				perf_begin(param_export_perf);
				res = param_export_internal(fd, nullptr);
				perf_end(param_export_perf);
				::close(fd);

				ако (res == PX4_OK) {
					// reopen file to verify
					цео fd_verify = ::open(filename, O_RDONLY, PX4_O_MODE_666);
					res = param_verify(fd_verify) || lseek(fd_verify, 0, SEEK_SET) || param_verify(fd_verify);
					::close(fd_verify);
				}
			}

			ако (res == PX4_OK) {
				break;

			} инако {
				PX4_ERR("parameter export to %s failed (%d) attempt %d", filename, res, attempt);
				px4_usleep(10000); // wait at least 10 milliseconds before trying again
			}
		}

	} инако {
		perf_begin(param_export_perf);
		res = flash_param_save(nullptr);
		perf_end(param_export_perf);
	}

	ако (res != PX4_OK) {
		PX4_ERR("param export failed (%d)", res);

		за (param_t param = 0; handle_in_range(param); param++) {
			ако (params_saving[param]) {
				params_unsaved.set(param, true);
			}
		}
	}

	params_saving.reset();

	ако (res == PX4_OK) {
		// backup file
		ако (param_backup_file) {
			цео fd_backup_file = ::open(param_backup_file, O_WRONLY | O_CREAT | O_TRUNC, PX4_O_MODE_666);

			ако (fd_backup_file > -1) {
				цео backup_export_ret = param_export_internal(fd_backup_file, nullptr);
				::close(fd_backup_file);

				ако (backup_export_ret != 0) {
					PX4_ERR("backup parameter export to %s failed (%d)", param_backup_file, backup_export_ret);

				} инако {
					// verify export
					цео fd_verify = ::open(param_backup_file, O_RDONLY, PX4_O_MODE_666);
					param_verify(fd_verify);
					::close(fd_verify);
				}
			}
		}
	}

	pthread_mutex_unlock(&file_mutex);

	ако (shutdown_lock_ret == 0) {
		px4_shutdown_unlock();
	}

	врати res;
}

/**
 * @return 0 on success, 1 if all params have not yet been stored, -1 if device open failed, -2 if writing parameters failed
 */
цео
param_load_default()
{
	цео res = 0;
	const char *filename = param_get_default_file();

	ако (!filename) {
		врати param_load(-1);
	}

	цео fd_load = ::open(filename, O_RDONLY);

	ако (fd_load < 0) {
		/* no parameter file is OK, otherwise this is an error */
		ако (errno != ENOENT) {
			PX4_ERR("open '%s' for reading failed", filename);
			врати -1;
		}

		врати 1;
	}

	цео result = param_load(fd_load);
	::close(fd_load);

	ако (result == 1) {
		/* blank file: nothing stored yet */
		врати 1;
	}

	ако (result != 0) {
		PX4_ERR("error reading parameters from '%s'", filename);
		врати -2;
	}

	врати res;
}

static цео param_verify_callback(bson_decoder_t decoder, bson_node_t node)
{
	ако (node->type == BSON_EOO) {
		врати 0;
	}

	// find the parameter this node represents
	param_t param = param_find_no_notification(node->name);

	ако (param == PARAM_INVALID) {
		PX4_ERR("verify: invalid parameter '%s'", node->name);
		врати -1;
	}

	// handle verifying the parameter from the node
	switch (node->type) {
	case BSON_INT32: {
			ако (param_type(param) != PARAM_TYPE_INT32) {
				PX4_ERR("verify: invalid param type %d for '%s' (BSON_INT32)", param_type(param), node->name);
				врати -1;
			}

			int32_t value;

			ако (param_get(param, &value) == 0) {
				ако (value == node->i32) {
					врати 1; // valid

				} инако {
					PX4_ERR("verify: '%s' invalid BSON value %" PRIi32 "(expected %" PRIi32 ")", node->name, node->i32, value);
				}
			}
		}
		break;

	case BSON_DOUBLE: {
			ако (param_type(param) != PARAM_TYPE_FLOAT) {
				PX4_ERR("verify: invalid param type %d for '%s' (BSON_DOUBLE)", param_type(param), node->name);
				врати -1;
			}

			float value;

			ако (param_get(param, &value) == 0) {
				ако (fabsf(value - (float)node->d) <= FLT_EPSILON) {
					врати 1; // valid

				} инако {
					PX4_ERR("verify: '%s' invalid BSON value %.3f (expected %.3f)", node->name, node->d, (double)value);
				}
			}
		}
		break;

	default:
		PX4_ERR("verify: '%s' invalid node type %d", node->name, node->type);
	}

	врати -1;
}

static цео param_verify(цео fd)
{
	PX4_DEBUG("param_verify");

	ако (fd < 0) {
		врати -1;
	}

	ако (lseek(fd, 0, SEEK_SET) != 0) {
		PX4_ERR("verify: seek failed");
		врати -1;
	}

	bson_decoder_s decoder{};

	ако (bson_decoder_init_file(&decoder, fd, param_verify_callback) == 0) {
		цео result = -1;

		do {
			result = bson_decoder_next(&decoder);

		} док (result > 0);

		ако (result == 0) {
			ако (decoder.total_document_size != decoder.total_decoded_size) {
				PX4_ERR("BSON document size (%" PRId32 ") doesn't match bytes decoded (%" PRId32 ")", decoder.total_document_size,
					decoder.total_decoded_size);

			} инако {
				врати 0;
			}

		} инако ако (result == -ENODATA) {
			PX4_ERR("verify: no BSON data");

		} инако {
			PX4_ERR("verify: failed (%d)", result);
		}
	}

	врати -1;
}

цео
param_export(const char *filename, param_filter_func filter)
{
	PX4_DEBUG("param_export");

	цео shutdown_lock_ret = px4_shutdown_lock();

	ако (shutdown_lock_ret != 0) {
		PX4_ERR("px4_shutdown_lock() failed (%i)", shutdown_lock_ret);
	}

	// take the file lock
	ако (pthread_mutex_trylock(&file_mutex) != 0) {
		PX4_ERR("param_export: file lock failed (already locked)");
		врати PX4_ERROR;
	}

	цео fd = ::open(filename, O_RDWR | O_CREAT, PX4_O_MODE_666);
	цео result = PX4_ERROR;

	perf_begin(param_export_perf);

	ако (fd > -1) {
		result = param_export_internal(fd, filter);

	} инако {
		result = flash_param_save(filter);
	}

	perf_end(param_export_perf);

	pthread_mutex_unlock(&file_mutex);

	ако (shutdown_lock_ret == 0) {
		px4_shutdown_unlock();
	}

	врати result;
}

// internal parameter export, caller is responsible for locking
static цео param_export_internal(цео fd, param_filter_func filter)
{
	PX4_DEBUG("param_export_internal");
	const auto changed_params = user_config.containedAsBitset();

	цео result = -1;
	bson_encoder_s encoder{};
	uint8_t bson_buffer[256];

	ако (lseek(fd, 0, SEEK_SET) != 0) {
		PX4_ERR("export seek failed %d", errno);
		врати -1;
	}

	ако (bson_encoder_init_buf_file(&encoder, fd, &bson_buffer, sizeof(bson_buffer)) != 0) {
		goto out;
	}

	за (param_t param = 0; handle_in_range(param); param++) {
		ако (!changed_params[param] || (filter && !filter(param))) {
			continue;
		}

		const param_value_u runtime_default_value = runtime_defaults.get(param);
		const param_value_u user_config_value = user_config.get(param);

		// don't export default values
		switch (param_type(param)) {
		case PARAM_TYPE_INT32:
			ако (user_config_value.i == runtime_default_value.i) {
				PX4_DEBUG("skipping %s %" PRIi32 " export", param_name(param), runtime_default_value.i);
				continue;
			}

			break;

		case PARAM_TYPE_FLOAT:
			ако (fabsf(user_config_value.f - runtime_default_value.f) <= FLT_EPSILON) {
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
				PX4_DEBUG("exporting: %s (%d) size: %lu val: %" PRIi32, name, param, (long unsigned цео)size, i);

				ако (bson_encoder_append_int32(&encoder, name, i) != 0) {
					PX4_ERR("BSON append failed for '%s'", name);
					goto out;
				}
			}
			break;

		case PARAM_TYPE_FLOAT: {
				const double f = (double)user_config_value.f;
				PX4_DEBUG("exporting: %s (%d) size: %lu val: %.3f", name, param, (long unsigned цео)size, (double)f);

				ако (bson_encoder_append_double(&encoder, name, f) != 0) {
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

	ако (result == 0) {
		ако (bson_encoder_fini(&encoder) != PX4_OK) {
			PX4_ERR("BSON encoder finalize failed");
			result = -1;
		}
	}

	врати result;
}

#ако defined(FLASH_BASED_PARAMS)
// A file is never the flash backend's store, so what an import changes is unsaved until a delta carries it.
// As on the file backend, the import itself does not save.
static constexpr bool file_import_is_saved = false;
#инако
static constexpr bool file_import_is_saved = true;
#endif

static цео
param_import_callback(bson_decoder_t decoder, bson_node_t node)
{
	/*
	 * EOO means the end of the parameter object. (Currently not supporting
	 * nested BSON objects).
	 */
	ако (node->type == BSON_EOO) {
		PX4_DEBUG("end of parameters");
		врати 0;
	}

	// A tombstone has no value. The decoder leaves the previous node's in the union and the translations
	// read it without a type check.
	const bool tombstone = node->type == BSON_nullptr || node->type == BSON_UNDEFINED;

	// if we do param_set() directly in the translation, set PARAM_SKIP_IMPORT as return value and return here
	ако (!tombstone && param_modify_on_import(node) == param_modify_on_import_ret::PARAM_SKIP_IMPORT) {
		врати 1;
	}

	// Find the parameter this node represents.  If we don't know it, ignore the node.
	param_t param = param_find_no_notification(node->name);

	ако (param == PARAM_INVALID) {
		PX4_WARN("ignoring unrecognised parameter '%s'", node->name);
		врати 1;
	}

	// Handle setting the parameter from the node
	switch (node->type) {
	case BSON_nullptr:
	case BSON_UNDEFINED:
		param_reset_internal(param, true, false);
		врати 1;

	case BSON_INT32: {
			ако (param_type(param) == PARAM_TYPE_INT32) {
				int32_t i = node->i32;
				param_set_internal(param, &i, file_import_is_saved, true, true, false);
				PX4_DEBUG("Imported %s with value %" PRIi32, param_name(param), i);

			} инако {
				PX4_WARN("unexpected type for %s", node->name);
			}
		}
		break;

	case BSON_DOUBLE: {
			ако (param_type(param) == PARAM_TYPE_FLOAT) {
				float f = node->d;
				param_set_internal(param, &f, file_import_is_saved, true, true, false);
				PX4_DEBUG("Imported %s with value %f", param_name(param), (double)f);

			} инако {
				PX4_WARN("unexpected type for %s", node->name);
			}
		}
		break;

	default:
		PX4_ERR("import: unrecognised node type for '%s'", node->name);
	}

	// don't return zero, that means EOF
	врати 1;
}

static цео
param_import_internal(цео fd)
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

	ако (fstat(fd, &st) == 0 && S_ISREG(st.st_mode) && st.st_size == 0) {
		врати 1;
	}

	int32_t doc_size = 0;
	ssize_t len = read(fd, &doc_size, sizeof(doc_size));

	ако (len == 0 || (len == (ssize_t)sizeof(doc_size) && (doc_size == 0 || doc_size == -1))) {
		врати 1;
	}

	ако (lseek(fd, 0, SEEK_SET) != 0) {
		PX4_ERR("import lseek failed (%d)", errno);
		врати -1;
	}

	static constexpr цео MAX_ATTEMPTS = 3;

	за (цео attempt = 1; attempt <= MAX_ATTEMPTS; attempt++) {
		bson_decoder_s decoder{};

		ако (bson_decoder_init_file(&decoder, fd, param_import_callback) == 0) {
			цео result = -1;

			do {
				result = bson_decoder_next(&decoder);

			} док (result > 0);

			ако (result == 0) {
				ако (decoder.total_document_size == decoder.total_decoded_size) {
					PX4_INFO("BSON document size %" PRId32 " bytes, decoded %" PRId32 " bytes (INT32:%" PRIu16 ", FLOAT:%" PRIu16 ")",
						 decoder.total_document_size, decoder.total_decoded_size,
						 decoder.count_node_int32, decoder.count_node_double);

					врати 0;

				} инако {
					PX4_ERR("BSON document size (%" PRId32 ") doesn't match bytes decoded (%" PRId32 ")",
						decoder.total_document_size, decoder.total_decoded_size);
				}

			} инако ако (result == -ENODATA) {
				// silently retry as a precaution unless this is our last attempt
				ако (attempt == MAX_ATTEMPTS) {
					PX4_DEBUG("BSON: no data");
					врати 0;
				}

			} инако {
				PX4_ERR("param import failed (%d) attempt %d", result, attempt);
			}

		} инако {
			PX4_ERR("param import bson decoder init failed (attempt %d)", attempt);
		}

		ако (attempt != MAX_ATTEMPTS) {
			ако (lseek(fd, 0, SEEK_SET) != 0) {
				PX4_ERR("import lseek failed (%d)", errno);
			}

			px4_usleep(10000); // wait at least 10 milliseconds before trying again
		}
	}

	врати -1;
}

#ако defined(FLASH_BASED_PARAMS)
static цео flash_backend_locked(цео (*op)())
{
	/* Same order as param_save_default: mutex then shutdown lock. Autosave
	 * uses a trylock, so a compact holds it off the flash programming. */
	pthread_mutex_lock(&file_mutex);

	цео shutdown_lock_ret = px4_shutdown_lock();

	ако (shutdown_lock_ret != 0) {
		PX4_ERR("px4_shutdown_lock() failed (%i)", shutdown_lock_ret);
	}

	цео result = op();

	pthread_mutex_unlock(&file_mutex);

	ако (shutdown_lock_ret == 0) {
		px4_shutdown_unlock();
	}

	врати result;
}

static цео flash_load_locked()
{
	/* Reset under the lock so a pending autosave cannot append tombstones
	 * of the just-cleared RAM before import/compact run. */
	param_reset_all_internal(false);
	цео result = flash_param_import();

	ако (result == 0 || result == 1) {
		params_unsaved.reset();
	}

	врати result;
}
#endif

цео
param_import(цео fd)
{
	ако (fd < 0) {
#ако defined(FLASH_BASED_PARAMS)
		врати flash_backend_locked(flash_param_import);
#инако
		врати flash_param_import();
#endif
	}

	врати param_import_internal(fd);
}

цео
param_load(цео fd)
{
	ако (fd < 0) {
#ако defined(FLASH_BASED_PARAMS)
		врати flash_backend_locked(flash_load_locked);
#инако
		врати PX4_ERROR;
#endif
	}

	param_reset_all_internal(false);
	цео result = param_import_internal(fd);

#ако !defined(FLASH_BASED_PARAMS)

	ако (result >= 0) {
		params_unsaved.reset();
	}

#endif

	врати result;
}

void
param_foreach(void (*func)(void *arg, param_t param), void *arg, bool only_changed, bool only_used)
{
	param_t	param;

	за (param = 0; handle_in_range(param); param++) {

		/* if requested, skip unchanged values */
		ако (only_changed && (!user_config.contains(param))) {
			continue;
		}

		ако (only_used && !param_used(param)) {
			continue;
		}

		func(arg, param);
	}
}

uint32_t param_hash_check()
{
	uint32_t param_hash = 0;

	/* compute the CRC32 over all string param names and 4 byte values */
	за (param_t param = 0; handle_in_range(param); param++) {
		ако (!param_used(param) || param_is_volatile(param)) {
			continue;
		}

		const char *name = param_name(param);
		auto value = user_config.get(param).i;
		const void *val = (void *)&value;
		param_hash = crc32part((const uint8_t *)name, strlen(name), param_hash);
		param_hash = crc32part((const uint8_t *)val, param_size(param), param_hash);
	}

	врати param_hash;
}

void param_print_status()
{
	PX4_INFO("summary: %d/%d (used/total)", param_count_used(), param_count());

#ifndef FLASH_BASED_PARAMS
	const char *filename = param_get_default_file();

	ако (filename != nullptr) {
		PX4_INFO("file: %s", param_get_default_file());
	}

	ако (param_backup_file) {
		PX4_INFO("backup file: %s", param_backup_file);
	}

#endif /* FLASH_BASED_PARAMS */

	PX4_INFO("storage array: %d/%d elements (%zu bytes total)",
		 user_config.size(), firmware_defaults.size(), (size_t)user_config.byteSize());


	PX4_INFO("storage array (custom defaults): %d/%d elements (%zu bytes total)",
		 runtime_defaults.size(), firmware_defaults.size(), (size_t)runtime_defaults.byteSize());

	ако (autosave_instance) {
		PX4_INFO("auto save: %s", autosave_instance->enabled() ? "on" : "off");

		hrt_abstime last_autosave = autosave_instance->lastAutosave();

		ако (last_autosave > 0) {
			PX4_INFO("last auto save: %.3f seconds ago", hrt_elapsed_time(&last_autosave) * 1e-6);
		}
	}

	perf_print_counter(param_export_perf);
	perf_print_counter(param_find_perf);
	perf_print_counter(param_get_perf);
	perf_print_counter(param_set_perf);

#ако defined(CONFIG_PARAM_PRIMARY)
	struct param_primary_counters counts;
	param_primary_get_counters(&counts);
	PX4_INFO("set value requests received: %" PRIu32 ", set value responses sent: %" PRIu32,
		 counts.set_value_request_received, counts.set_value_response_sent);
	PX4_INFO("set value requests sent: %" PRIu32 ", set value responses received: %" PRIu32,
		 counts.set_value_request_sent, counts.set_value_response_received);
	PX4_INFO("resets sent: %" PRIu32 ", set used requests received: %" PRIu32,
		 counts.reset_sent, counts.set_used_received);
#endif

#ако defined(CONFIG_PARAM_REMOTE)
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
