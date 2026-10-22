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
#ضمن "param.h"
#ضمن "param_translation.h"
#ضمن <parameters/px4_parameters.hpp>
#ضمن <lib/tinybson/tinybson.h>

#אם defined(__PX4_NUTTX)
#ضمن <nuttx/crc32.h>
#אחרת
#ضمن <crc32.h>
#endif
#ضمن <float.h>
#ضمن <math.h>
#ضمن <sys/stat.h>

#ضمن <containers/Bitset.hpp>
#ضمن <drivers/drv_hrt.h>
#ضمن <lib/perf/perf_counter.h>
#ضمن <px4_platform_common/px4_config.h>
#ضمن <px4_platform_common/atomic_bitset.h>
#ضمن <px4_platform_common/defines.h>
#ضمن <px4_platform_common/posix.h>
#ضمن <px4_platform_common/sem.h>
#ضمن <px4_platform_common/shutdown.h>
#ضمن <px4_platform_common/micro_hal.h>

using نطاق time_literals;

#ضمن "uORB/uORB.h"
#ضمن "uORB/topics/parameter_update.h"
#ضمن <uORB/Subscription.hpp>

#ضمن "ExhaustiveLayer.h"
#ضمن "ConstLayer.h"
#ضمن "DynamicSparseLayer.h"
#ضمن "StaticSparseLayer.h"

#ضمن "atomic_transaction.h"

/* Include functions common to user and kernel sides */
#ضمن "parameters_common.cpp"

#אם defined(__PX4_NUTTX) && !defined(CONFIG_BUILD_FLAT)
#ضمن <px4_platform/board_ctrl.h>
#ضمن "parameters_ioctl.h"
#endif

#אם defined(FLASH_BASED_PARAMS)
#ضمن "flashparams/flashparams.h"
#אחרת
مضمن ساكن صحيح flash_param_save(param_filter_func filter) { החזר -1; }
مضمن ساكن صحيح flash_param_import() { החזר -1; }
#endif

ساكن حرف *param_default_file = nullptr;
ساكن حرف *param_backup_file = nullptr;

#ضمن "autosave.h"
ساكن ParamAutosave *autosave_instance {nullptr};

ساكن px4::AtomicBitset<param_info_count> params_active;  // params found
ساكن px4::AtomicBitset<param_info_count> params_unsaved;

// Params the save in progress is writing. They are moved out of params_unsaved
// before their values are read, so a param_set that lands while the save runs
// marks its param again and reaches the next save. Flash-param saves write only
// unsaved params, so nothing else would ever write that change.
ساكن px4::AtomicBitset<param_info_count> params_saving;

ساكن ConstLayer firmware_defaults;
ساكن DynamicSparseLayer runtime_defaults{&firmware_defaults};
DynamicSparseLayer user_config{&runtime_defaults};

/** parameter update topic handle */
#אם not defined(CONFIG_PARAM_REMOTE)
ساكن orb_advert_t param_topic = nullptr;
ساكن unsigned صحيح param_instance = 0;
#endif

ساكن perf_counter_t param_export_perf;
ساكن perf_counter_t param_find_perf;
ساكن perf_counter_t param_get_perf;
ساكن perf_counter_t param_set_perf;

ساكن pthread_mutex_t file_mutex  =
	PTHREAD_MUTEX_INITIALIZER; ///< this protects against concurrent param saves (file or flash access).

// Support for remote parameter node
#אם defined(CONFIG_PARAM_PRIMARY)
# include "parameters_primary.h"
#endif // CONFIG_PARAM_PRIMARY
#אם defined(CONFIG_PARAM_REMOTE)
# include "parameters_remote.h"
#endif // CONFIG_PARAM_REMOTE

فراغ
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

#אם defined(__PX4_NUTTX) && !defined(CONFIG_BUILD_FLAT)
	px4_register_boardct_ioctl(_PARAMIOCBASE, param_ioctl);
#endif

#אם defined(CONFIG_PARAM_PRIMARY)
	param_primary_init();
#endif // CONFIG_PARAM_PRIMARY

#אם defined(CONFIG_PARAM_REMOTE)
	param_remote_init();
#endif // CONFIG_PARAM_REMOTE

#אם not defined(CONFIG_PARAM_REMOTE)
	autosave_instance = جديد ParamAutosave();
#endif
}


فراغ
param_notify_changes()
{
// Don't send if this is a remote node. Only the primary
// sends out update notices
#אם not defined(CONFIG_PARAM_REMOTE)
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

	אם (param_topic == nullptr) {
		param_topic = orb_advertise(ORB_ID(parameter_update), &pup);

	} אחרת {
		orb_publish(ORB_ID(parameter_update), param_topic, &pup);
	}

#endif
}

ساكن param_t param_find_internal(ثابت حرف *name, منطقي notification)
{
	perf_count(param_find_perf);

	param_t middle;
	param_t front = 0;
	param_t last = param_info_count;

	/* perform a binary search of the known parameters */

	כלעוד (front <= last) {
		middle = front + (last - front) / 2;
		صحيح ret = strcmp(name, param_name(middle));

		אם (ret == 0) {
			אם (notification) {
				param_set_used(middle);
			}

			החזר middle;

		} אחרת אם (middle == front) {
			/* An end point has been hit, but there has been no match */
			اكسر;

		} אחרת אם (ret < 0) {
			last = middle;

		} אחרת {
			front = middle;
		}
	}

	/* not found */
	החזר PARAM_INVALID;
}

param_t param_find(ثابت حرف *name)
{
	החזר param_find_internal(name, אמת);
}

param_t param_find_no_notification(ثابت حرف *name)
{
	החזר param_find_internal(name, שקר);
}

unsigned param_count_used()
{
	החזר params_active.count();
}

param_t param_for_used_index(unsigned index)
{
	// walk all params and count used params
	אם (index < param_info_count) {
		unsigned used_count = 0;

		עבור (صحيح i = 0; i < params_active.size(); i++) {
			אם (params_active[i]) {
				// we found the right used count,
				//  return the param value
				אם (index == used_count) {
					החזר static_cast<param_t>(i);
				}

				used_count++;
			}
		}
	}

	החזר PARAM_INVALID;
}

صحيح param_get_used_index(param_t param)
{
	/* this tests for out of bounds and does a constant time lookup */
	אם (!param_used(param)) {
		החזר -1;
	}

	/* walk all params and count, now knowing that it has a valid index */
	صحيح used_count = 0;

	עבור (صحيح i = 0; i < params_active.size(); i++) {
		אם (params_active[i]) {

			אם (param == i) {
				החזר used_count;
			}

			used_count++;
		}
	}

	החזר -1;
}

منطقي
param_value_unsaved(param_t param)
{
	החזר handle_in_range(param) ? (params_unsaved[param] || params_saving[param]) : שקר;
}

صحيح
param_get(param_t param, فراغ *val)
{
	perf_count(param_get_perf);

	אם (!handle_in_range(param)) {
		PX4_ERR("get: param %" PRId16 " invalid", param);
		החזר PX4_ERROR;
	}

	אם (!params_active[param]) {
		PX4_DEBUG("get: param %" PRId16 " (%s) not active", param, param_name(param));
	}

	صحيح result = PX4_ERROR;

	אם (val) {

		auto retrieve_value = user_config.get(param);

		بدل (param_type(param)) {
		حالة PARAM_TYPE_INT32:
			memcpy(val, &retrieve_value.i, حجم(retrieve_value.i));
			החזר PX4_OK;

		حالة PARAM_TYPE_FLOAT:
			memcpy(val, &retrieve_value.f, حجم(retrieve_value.f));
			החזר PX4_OK;
		}
	}

	החזר result;
}

صحيح
param_get_default_value_internal(param_t param, فراغ *default_val)
{
	אם (!handle_in_range(param)) {
		PX4_ERR("get default value: param %d invalid", param);
		החזר PX4_ERROR;
	}

	אם (default_val) {
		بدل (param_type(param)) {
		حالة PARAM_TYPE_INT32: {
				int32_t val = runtime_defaults.get(param).i;
				memcpy(default_val, &val, حجم(val));
				החזר PX4_OK;
			}

		حالة PARAM_TYPE_FLOAT: {
				عائم val = runtime_defaults.get(param).f;
				memcpy(default_val, &val, حجم(val));
				החזר PX4_OK;
			}
		}
	}

	החזר PX4_ERROR;
}

صحيح
param_get_default_value(param_t param, فراغ *default_val)
{
	אם (!handle_in_range(param)) {
		החזר PX4_ERROR;
	}

	صحيح ret = param_get_default_value_internal(param, default_val);
	החזר ret;
}

منطقي param_value_is_default(param_t param)
{
	אם (!handle_in_range(param)) {
		החזר אמת;
	}

	אם (!user_config.contains(param)) {
		// if user config does not contain it, consider it default.
		החזר אמת;

	} אחרת {
		// compare with default value
		ثابت param_value_u user_config_value = user_config.get(param);
		ثابت param_value_u runtime_default_value = runtime_defaults.get(param);

		بدل (param_type(param)) {
		حالة PARAM_TYPE_INT32: {
				החזר user_config_value.i == runtime_default_value.i;
			}

		حالة PARAM_TYPE_FLOAT: {
				החזר fabsf(user_config_value.f - runtime_default_value.f) < FLT_EPSILON;
			}
		}
	}

	החזר אמת;
}

ساكن فراغ
param_autosave()
{
	אם (autosave_instance) {
		autosave_instance->request();
	}
}

فراغ
param_control_autosave(منطقي enable)
{
	אם (autosave_instance) {
		autosave_instance->enable(enable);
	}
}

ساكن صحيح
param_set_internal(param_t param, ثابت فراغ *val, منطقي mark_saved, منطقي notify_changes, منطقي update_remote = אמת,
		   منطقي autosave = אמת)
{
	אם (!handle_in_range(param)) {
		PX4_ERR("set invalid param %d", param);
		החזר PX4_ERROR;
	}

	אם (val == nullptr) {
		PX4_ERR("set invalid value");
		החזר PX4_ERROR;
	}

	אם (param_is_readonly(param)) {
		PX4_WARN("param %s is read-only", param_name(param));
		החזר PX4_ERROR;
	}

	صحيح result = -1;
	منطقي param_changed = שקר;
	perf_begin(param_set_perf);

	ثابت param_value_u user_config_value = user_config.get(param);
	param_value_u new_value{};

	بدل (param_type(param)) {
	حالة PARAM_TYPE_INT32:
		memcpy(&new_value.i, val, حجم(new_value.i));
		param_changed = user_config_value.i != new_value.i;
		اكسر;

	حالة PARAM_TYPE_FLOAT:
		memcpy(&new_value.f, val, حجم(new_value.f));
		param_changed = fabsf(user_config_value.f - new_value.f) > FLT_EPSILON;
		اكسر;

	افتراضي: {
			PX4_ERR("param_set invalid param type for %s", param_name(param));
			اكسر;
		}
	}

	אם (user_config.store(param, new_value)) {
		אם (param_changed) {
			params_unsaved.set(param, !mark_saved);
		}

		result = PX4_OK;

	} אחרת {
		PX4_ERR("param_set failed to store param %s", param_name(param));
		result = PX4_ERROR;
	}

	אם ((result == PX4_OK) && param_changed && !mark_saved && autosave) { // this is false when importing parameters
		param_autosave();
	}

	// If this is the parameter server, make sure that the remote is updated
#אם defined(CONFIG_PARAM_PRIMARY)

	אם (param_changed && update_remote) {
		param_primary_set_value(param, val);
	}

#endif

	// If this is the parameter remote, make sure that the primary is updated
#אם defined(CONFIG_PARAM_REMOTE)

	אם (param_changed && update_remote) {
		param_remote_set_value(param, val);
	}

#endif

	perf_end(param_set_perf);

	/*
	 * If we set something, now that we have unlocked, go ahead and advertise that
	 * a thing has been set.
	 */
	אם ((result == PX4_OK) && param_changed && notify_changes) {
		param_notify_changes();
	}

	החזר result;
}

#אם defined(FLASH_BASED_PARAMS)
صحيح param_set_external(param_t param, ثابت فراغ *val, منطقي mark_saved, منطقي notify_changes)
{
	החזר param_set_internal(param, val, mark_saved, notify_changes);
}

فراغ param_get_external(param_t param, فراغ *val)
{
	param_get(param, val);
}
#endif

صحيح param_set(param_t param, ثابت فراغ *val)
{
	החזר param_set_internal(param, val, שקר, אמת);
}

صحيح param_set_no_notification(param_t param, ثابت فراغ *val)
{
	החזר param_set_internal(param, val, שקר, שקר);
}

صحيح param_set_no_remote_update(param_t param, ثابت فراغ *val, منطقي notify)
{
	החזר param_set_internal(param, val, שקר, notify, שקר);
}

منطقي param_used(param_t param)
{
	אם (handle_in_range(param)) {
		החזר params_active[param];
	}

	החזר שקר;
}

فراغ param_set_used(param_t param)
{
	אם (handle_in_range(param)) {
#אם defined(CONFIG_PARAM_REMOTE)

		אם (!param_used(param)) {
			param_remote_set_used(param);
		}

#endif

		params_active.set(param, אמת);
	}
}

صحيح param_set_default_value(param_t param, ثابت فراغ *val)
{
	אם (!handle_in_range(param)) {
		PX4_ERR("set default value invalid param %d", param);
		החזר PX4_ERROR;
	}

	אם (val == nullptr) {
		PX4_ERR("set default value invalid value");
		החזר PX4_ERROR;
	}

	אם (param_is_readonly(param)) {
		PX4_WARN("param %s is read-only", param_name(param));
		החזר PX4_ERROR;
	}

	صحيح result = PX4_ERROR;


	// check if param being set to default value
	منطقي setting_to_static_default = שקר;
	ثابت param_value_u firmware_default_value = firmware_defaults.get(param);

	بدل (param_type(param)) {
	حالة PARAM_TYPE_INT32: {
			int32_t new_value;
			memcpy(&new_value, val, حجم(new_value));
			setting_to_static_default = firmware_default_value.i == new_value;
			اكسر;
		}

	حالة PARAM_TYPE_FLOAT: {
			عائم new_value;
			memcpy(&new_value, val, حجم(new_value));
			setting_to_static_default = fabsf(firmware_default_value.f - new_value) <= FLT_EPSILON;
			اكسر;
		}
	}

	אם (setting_to_static_default) {
		runtime_defaults.reset(param);

		result = PX4_OK;

	} אחרת {
		param_value_u new_value{};

		بدل (param_type(param)) {
		حالة PARAM_TYPE_INT32: {
				memcpy(&new_value.i, val, حجم(new_value.i));
				اكسر;
			}

		حالة PARAM_TYPE_FLOAT: {
				memcpy(&new_value.f, val, حجم(new_value.f));
				اكسر;
			}

		افتراضي:
			اكسر;
		}

		אם (runtime_defaults.store(param, new_value)) {
			user_config.refresh(param);
			result = PX4_OK;

		} אחרת {
			result = PX4_ERROR;
		}
	}


	אם ((result == PX4_OK) && param_used(param)) {
		// send notification if param is already in use
		param_notify_changes();
	}

	החזר result;
}

ساكن صحيح param_reset_internal(param_t param, منطقي notify = אמת, منطقي autosave = אמת)
{
#אם defined(CONFIG_PARAM_REMOTE)
	// Remote doesn't support reset
	החזר שקר;
#endif

	אם (param_is_readonly(param)) {
		החזר 0;  // silently skip — param_reset_all loops over all params
	}

	منطقي param_found = user_config.contains(param);

	אם (handle_in_range(param)) {
		user_config.reset(param);
	}

	אם (param_found) {
		/* Persist the reset. Flash-param saves append only unsaved params,
		 * so a reset has to show up as a tombstone or replay keeps the
		 * previous override. */
		params_unsaved.set(param, אמת);
	}

	אם (autosave) {
		param_autosave();
	}

	אם (param_found && notify) {
		param_notify_changes();
	}

#אם defined(CONFIG_PARAM_PRIMARY)
	param_primary_reset(param);
#endif

	החזר param_found;
}

صحيح param_reset(param_t param) { החזר param_reset_internal(param, אמת); }
صحيح param_reset_no_notification(param_t param) { החזר param_reset_internal(param, שקר); }

ساكن فراغ
param_reset_all_internal(منطقي auto_save)
{
#אם defined(CONFIG_PARAM_REMOTE)
	// Remote doesn't support reset
	החזר;
#endif

	עבור (param_t param = 0; handle_in_range(param); param++) {
		param_reset_internal(param, שקר, שקר);
	}

	אם (auto_save) {
		param_autosave();
	}

#אם defined(CONFIG_PARAM_PRIMARY)
	param_primary_reset_all();
#endif

	param_notify_changes();
}

فراغ
param_reset_all()
{
	param_reset_all_internal(אמת);
}

فراغ
param_reset_excludes(ثابت حرف *excludes[], صحيح num_excludes)
{
	עבור (param_t param = 0; handle_in_range(param); param++) {
		ثابت حرف *name = param_name(param);
		منطقي exclude = שקר;

		עבור (صحيح index = 0; index < num_excludes; index ++) {
			صحيح len = strlen(excludes[index]);

			אם ((excludes[index][len - 1] == '*'
			     && strncmp(name, excludes[index], len - 1) == 0)
			    || strcmp(name, excludes[index]) == 0) {

				exclude = אמת;
				اكسر;
			}
		}

		אם (!exclude) {
			param_reset(param);
		}
	}
}

فراغ
param_reset_specific(ثابت حرف *resets[], صحيح num_resets)
{
	עבור (param_t param = 0; handle_in_range(param); param++) {
		ثابت حرف *name = param_name(param);
		منطقي reset = שקר;

		עבור (صحيح index = 0; index < num_resets; index++) {
			صحيح len = strlen(resets[index]);

			אם ((resets[index][len - 1] == '*'
			     && strncmp(name, resets[index], len - 1) == 0)
			    || strcmp(name, resets[index]) == 0) {

				reset = אמת;
				اكسر;
			}
		}

		אם (reset) {
			param_reset(param);
		}
	}
}

صحيح
param_set_default_file(ثابت حرف *filename)
{
	אם (filename && param_backup_file && strcmp(filename, param_backup_file) == 0) {
		PX4_ERR("default file can't be the same as the backup file %s", filename);
		החזר PX4_ERROR;
	}

#ifdef FLASH_BASED_PARAMS
	// the default for flash-based params is always the FLASH
	(فراغ)filename;
#אחרת

	אם (param_default_file != nullptr) {
		// we assume this is not in use by some other thread
		حرر(param_default_file);
		param_default_file = nullptr;
	}

	אם (filename) {
		param_default_file = strdup(filename);
	}

#endif /* FLASH_BASED_PARAMS */

	החזר 0;
}

ثابت حرف *param_get_default_file()
{
	החזר param_default_file;
}

صحيح param_set_backup_file(ثابت حرف *filename)
{
	אם (filename && param_default_file && strcmp(filename, param_default_file) == 0) {
		PX4_ERR("backup file can't be the same as the default file %s", filename);
		החזר PX4_ERROR;
	}

	אם (param_backup_file != nullptr) {
		// we assume this is not in use by some other thread
		حرر(param_backup_file);
		param_backup_file = nullptr;
	}

	אם (filename) {
		param_backup_file = strdup(filename);

	} אחרת {
		param_backup_file = nullptr; // backup disabled
	}

	החזר 0;
}

ثابت حرف *param_get_backup_file()
{
	החזר param_backup_file;
}

ساكن صحيح param_export_internal(صحيح fd, param_filter_func filter);
ساكن صحيح param_verify(صحيح fd);

صحيح param_save_default(منطقي blocking)
{
	PX4_DEBUG("param_save_default");

	// take the file lock
	אם (blocking) {
		pthread_mutex_lock(&file_mutex);

	} אחרת {
		אם (pthread_mutex_trylock(&file_mutex) != 0) {
			PX4_DEBUG("param_save_default: file lock failed (already locked)");
			החזר -EWOULDBLOCK;
		}
	}

	صحيح shutdown_lock_ret = px4_shutdown_lock();

	אם (shutdown_lock_ret != 0) {
		PX4_ERR("px4_shutdown_lock() failed (%i)", shutdown_lock_ret);
	}

	صحيح res = PX4_ERROR;
	ثابت حرف *filename = param_get_default_file();

	עבור (param_t param = 0; handle_in_range(param); param++) {
		אם (params_unsaved[param]) {
			params_saving.set(param, אמת);
			params_unsaved.set(param, שקר);
		}
	}

	אם (filename) {
		ساكن constexpr صحيح MAX_ATTEMPTS = 3;

		עבור (صحيح attempt = 1; attempt <= MAX_ATTEMPTS; attempt++) {
			// write parameters to file
			صحيح fd = ::open(filename, O_WRONLY | O_CREAT | O_TRUNC, PX4_O_MODE_666);

			אם (fd > -1) {
				perf_begin(param_export_perf);
				res = param_export_internal(fd, nullptr);
				perf_end(param_export_perf);
				::close(fd);

				אם (res == PX4_OK) {
					// reopen file to verify
					صحيح fd_verify = ::open(filename, O_RDONLY, PX4_O_MODE_666);
					res = param_verify(fd_verify) || lseek(fd_verify, 0, SEEK_SET) || param_verify(fd_verify);
					::close(fd_verify);
				}
			}

			אם (res == PX4_OK) {
				اكسر;

			} אחרת {
				PX4_ERR("parameter export to %s failed (%d) attempt %d", filename, res, attempt);
				px4_usleep(10000); // wait at least 10 milliseconds before trying again
			}
		}

	} אחרת {
		perf_begin(param_export_perf);
		res = flash_param_save(nullptr);
		perf_end(param_export_perf);
	}

	אם (res != PX4_OK) {
		PX4_ERR("param export failed (%d)", res);

		עבור (param_t param = 0; handle_in_range(param); param++) {
			אם (params_saving[param]) {
				params_unsaved.set(param, אמת);
			}
		}
	}

	params_saving.reset();

	אם (res == PX4_OK) {
		// backup file
		אם (param_backup_file) {
			صحيح fd_backup_file = ::open(param_backup_file, O_WRONLY | O_CREAT | O_TRUNC, PX4_O_MODE_666);

			אם (fd_backup_file > -1) {
				صحيح backup_export_ret = param_export_internal(fd_backup_file, nullptr);
				::close(fd_backup_file);

				אם (backup_export_ret != 0) {
					PX4_ERR("backup parameter export to %s failed (%d)", param_backup_file, backup_export_ret);

				} אחרת {
					// verify export
					صحيح fd_verify = ::open(param_backup_file, O_RDONLY, PX4_O_MODE_666);
					param_verify(fd_verify);
					::close(fd_verify);
				}
			}
		}
	}

	pthread_mutex_unlock(&file_mutex);

	אם (shutdown_lock_ret == 0) {
		px4_shutdown_unlock();
	}

	החזר res;
}

/**
 * @return 0 on success, 1 if all params have not yet been stored, -1 if device open failed, -2 if writing parameters failed
 */
صحيح
param_load_default()
{
	صحيح res = 0;
	ثابت حرف *filename = param_get_default_file();

	אם (!filename) {
		החזר param_load(-1);
	}

	صحيح fd_load = ::open(filename, O_RDONLY);

	אם (fd_load < 0) {
		/* no parameter file is OK, otherwise this is an error */
		אם (errno != ENOENT) {
			PX4_ERR("open '%s' for reading failed", filename);
			החזר -1;
		}

		החזר 1;
	}

	صحيح result = param_load(fd_load);
	::close(fd_load);

	אם (result == 1) {
		/* blank file: nothing stored yet */
		החזר 1;
	}

	אם (result != 0) {
		PX4_ERR("error reading parameters from '%s'", filename);
		החזר -2;
	}

	החזר res;
}

ساكن صحيح param_verify_callback(bson_decoder_t decoder, bson_node_t node)
{
	אם (node->type == BSON_EOO) {
		החזר 0;
	}

	// find the parameter this node represents
	param_t param = param_find_no_notification(node->name);

	אם (param == PARAM_INVALID) {
		PX4_ERR("verify: invalid parameter '%s'", node->name);
		החזר -1;
	}

	// handle verifying the parameter from the node
	بدل (node->type) {
	حالة BSON_INT32: {
			אם (param_type(param) != PARAM_TYPE_INT32) {
				PX4_ERR("verify: invalid param type %d for '%s' (BSON_INT32)", param_type(param), node->name);
				החזר -1;
			}

			int32_t value;

			אם (param_get(param, &value) == 0) {
				אם (value == node->i32) {
					החזר 1; // valid

				} אחרת {
					PX4_ERR("verify: '%s' invalid BSON value %" PRIi32 "(expected %" PRIi32 ")", node->name, node->i32, value);
				}
			}
		}
		اكسر;

	حالة BSON_DOUBLE: {
			אם (param_type(param) != PARAM_TYPE_FLOAT) {
				PX4_ERR("verify: invalid param type %d for '%s' (BSON_DOUBLE)", param_type(param), node->name);
				החזר -1;
			}

			عائم value;

			אם (param_get(param, &value) == 0) {
				אם (fabsf(value - (عائم)node->d) <= FLT_EPSILON) {
					החזר 1; // valid

				} אחרת {
					PX4_ERR("verify: '%s' invalid BSON value %.3f (expected %.3f)", node->name, node->d, (مزدوج)value);
				}
			}
		}
		اكسر;

	افتراضي:
		PX4_ERR("verify: '%s' invalid node type %d", node->name, node->type);
	}

	החזר -1;
}

ساكن صحيح param_verify(صحيح fd)
{
	PX4_DEBUG("param_verify");

	אם (fd < 0) {
		החזר -1;
	}

	אם (lseek(fd, 0, SEEK_SET) != 0) {
		PX4_ERR("verify: seek failed");
		החזר -1;
	}

	bson_decoder_s decoder{};

	אם (bson_decoder_init_file(&decoder, fd, param_verify_callback) == 0) {
		صحيح result = -1;

		עשה {
			result = bson_decoder_next(&decoder);

		} כלעוד (result > 0);

		אם (result == 0) {
			אם (decoder.total_document_size != decoder.total_decoded_size) {
				PX4_ERR("BSON document size (%" PRId32 ") doesn't match bytes decoded (%" PRId32 ")", decoder.total_document_size,
					decoder.total_decoded_size);

			} אחרת {
				החזר 0;
			}

		} אחרת אם (result == -ENODATA) {
			PX4_ERR("verify: no BSON data");

		} אחרת {
			PX4_ERR("verify: failed (%d)", result);
		}
	}

	החזר -1;
}

صحيح
param_export(ثابت حرف *filename, param_filter_func filter)
{
	PX4_DEBUG("param_export");

	صحيح shutdown_lock_ret = px4_shutdown_lock();

	אם (shutdown_lock_ret != 0) {
		PX4_ERR("px4_shutdown_lock() failed (%i)", shutdown_lock_ret);
	}

	// take the file lock
	אם (pthread_mutex_trylock(&file_mutex) != 0) {
		PX4_ERR("param_export: file lock failed (already locked)");
		החזר PX4_ERROR;
	}

	صحيح fd = ::open(filename, O_RDWR | O_CREAT, PX4_O_MODE_666);
	صحيح result = PX4_ERROR;

	perf_begin(param_export_perf);

	אם (fd > -1) {
		result = param_export_internal(fd, filter);

	} אחרת {
		result = flash_param_save(filter);
	}

	perf_end(param_export_perf);

	pthread_mutex_unlock(&file_mutex);

	אם (shutdown_lock_ret == 0) {
		px4_shutdown_unlock();
	}

	החזר result;
}

// internal parameter export, caller is responsible for locking
ساكن صحيح param_export_internal(صحيح fd, param_filter_func filter)
{
	PX4_DEBUG("param_export_internal");
	ثابت auto changed_params = user_config.containedAsBitset();

	صحيح result = -1;
	bson_encoder_s encoder{};
	uint8_t bson_buffer[256];

	אם (lseek(fd, 0, SEEK_SET) != 0) {
		PX4_ERR("export seek failed %d", errno);
		החזר -1;
	}

	אם (bson_encoder_init_buf_file(&encoder, fd, &bson_buffer, حجم(bson_buffer)) != 0) {
		اذهب out;
	}

	עבור (param_t param = 0; handle_in_range(param); param++) {
		אם (!changed_params[param] || (filter && !filter(param))) {
			استمر;
		}

		ثابت param_value_u runtime_default_value = runtime_defaults.get(param);
		ثابت param_value_u user_config_value = user_config.get(param);

		// don't export default values
		بدل (param_type(param)) {
		حالة PARAM_TYPE_INT32:
			אם (user_config_value.i == runtime_default_value.i) {
				PX4_DEBUG("skipping %s %" PRIi32 " export", param_name(param), runtime_default_value.i);
				استمر;
			}

			اكسر;

		حالة PARAM_TYPE_FLOAT:
			אם (fabsf(user_config_value.f - runtime_default_value.f) <= FLT_EPSILON) {
				PX4_DEBUG("skipping %s %.3f export", param_name(param), (مزدوج)runtime_default_value.f);
				استمر;
			}

			اكسر;
		}

		ثابت حرف *name = param_name(param);
		ثابت size_t size = param_size(param);

		/* append the appropriate BSON type object */
		بدل (param_type(param)) {
		حالة PARAM_TYPE_INT32: {
				ثابت int32_t i = user_config_value.i;
				PX4_DEBUG("exporting: %s (%d) size: %lu val: %" PRIi32, name, param, (long unsigned صحيح)size, i);

				אם (bson_encoder_append_int32(&encoder, name, i) != 0) {
					PX4_ERR("BSON append failed for '%s'", name);
					اذهب out;
				}
			}
			اكسر;

		حالة PARAM_TYPE_FLOAT: {
				ثابت مزدوج f = (مزدوج)user_config_value.f;
				PX4_DEBUG("exporting: %s (%d) size: %lu val: %.3f", name, param, (long unsigned صحيح)size, (مزدوج)f);

				אם (bson_encoder_append_double(&encoder, name, f) != 0) {
					PX4_ERR("BSON append failed for '%s'", name);
					اذهب out;
				}
			}
			اكسر;

		افتراضي:
			PX4_ERR("%s unrecognized parameter type %d, skipping export", name, param_type(param));
		}
	}

	result = 0;

out:

	אם (result == 0) {
		אם (bson_encoder_fini(&encoder) != PX4_OK) {
			PX4_ERR("BSON encoder finalize failed");
			result = -1;
		}
	}

	החזר result;
}

#אם defined(FLASH_BASED_PARAMS)
// A file is never the flash backend's store, so what an import changes is unsaved until a delta carries it.
// As on the file backend, the import itself does not save.
ساكن constexpr منطقي file_import_is_saved = שקר;
#אחרת
ساكن constexpr منطقي file_import_is_saved = אמת;
#endif

ساكن صحيح
param_import_callback(bson_decoder_t decoder, bson_node_t node)
{
	/*
	 * EOO means the end of the parameter object. (Currently not supporting
	 * nested BSON objects).
	 */
	אם (node->type == BSON_EOO) {
		PX4_DEBUG("end of parameters");
		החזר 0;
	}

	// A tombstone has no value. The decoder leaves the previous node's in the union and the translations
	// read it without a type check.
	ثابت منطقي tombstone = node->type == BSON_nullptr || node->type == BSON_UNDEFINED;

	// if we do param_set() directly in the translation, set PARAM_SKIP_IMPORT as return value and return here
	אם (!tombstone && param_modify_on_import(node) == param_modify_on_import_ret::PARAM_SKIP_IMPORT) {
		החזר 1;
	}

	// Find the parameter this node represents.  If we don't know it, ignore the node.
	param_t param = param_find_no_notification(node->name);

	אם (param == PARAM_INVALID) {
		PX4_WARN("ignoring unrecognised parameter '%s'", node->name);
		החזר 1;
	}

	// Handle setting the parameter from the node
	بدل (node->type) {
	حالة BSON_nullptr:
	حالة BSON_UNDEFINED:
		param_reset_internal(param, אמת, שקר);
		החזר 1;

	حالة BSON_INT32: {
			אם (param_type(param) == PARAM_TYPE_INT32) {
				int32_t i = node->i32;
				param_set_internal(param, &i, file_import_is_saved, אמת, אמת, שקר);
				PX4_DEBUG("Imported %s with value %" PRIi32, param_name(param), i);

			} אחרת {
				PX4_WARN("unexpected type for %s", node->name);
			}
		}
		اكسر;

	حالة BSON_DOUBLE: {
			אם (param_type(param) == PARAM_TYPE_FLOAT) {
				عائم f = node->d;
				param_set_internal(param, &f, file_import_is_saved, אמת, אמת, שקר);
				PX4_DEBUG("Imported %s with value %f", param_name(param), (مزدوج)f);

			} אחרת {
				PX4_WARN("unexpected type for %s", node->name);
			}
		}
		اكسر;

	افتراضي:
		PX4_ERR("import: unrecognised node type for '%s'", node->name);
	}

	// don't return zero, that means EOF
	החזר 1;
}

ساكن صحيح
param_import_internal(صحيح fd)
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
	بنية stat st;

	אם (fstat(fd, &st) == 0 && S_ISREG(st.st_mode) && st.st_size == 0) {
		החזר 1;
	}

	int32_t doc_size = 0;
	ssize_t len = اقرأ(fd, &doc_size, حجم(doc_size));

	אם (len == 0 || (len == (ssize_t)حجم(doc_size) && (doc_size == 0 || doc_size == -1))) {
		החזר 1;
	}

	אם (lseek(fd, 0, SEEK_SET) != 0) {
		PX4_ERR("import lseek failed (%d)", errno);
		החזר -1;
	}

	ساكن constexpr صحيح MAX_ATTEMPTS = 3;

	עבור (صحيح attempt = 1; attempt <= MAX_ATTEMPTS; attempt++) {
		bson_decoder_s decoder{};

		אם (bson_decoder_init_file(&decoder, fd, param_import_callback) == 0) {
			صحيح result = -1;

			עשה {
				result = bson_decoder_next(&decoder);

			} כלעוד (result > 0);

			אם (result == 0) {
				אם (decoder.total_document_size == decoder.total_decoded_size) {
					PX4_INFO("BSON document size %" PRId32 " bytes, decoded %" PRId32 " bytes (INT32:%" PRIu16 ", FLOAT:%" PRIu16 ")",
						 decoder.total_document_size, decoder.total_decoded_size,
						 decoder.count_node_int32, decoder.count_node_double);

					החזר 0;

				} אחרת {
					PX4_ERR("BSON document size (%" PRId32 ") doesn't match bytes decoded (%" PRId32 ")",
						decoder.total_document_size, decoder.total_decoded_size);
				}

			} אחרת אם (result == -ENODATA) {
				// silently retry as a precaution unless this is our last attempt
				אם (attempt == MAX_ATTEMPTS) {
					PX4_DEBUG("BSON: no data");
					החזר 0;
				}

			} אחרת {
				PX4_ERR("param import failed (%d) attempt %d", result, attempt);
			}

		} אחרת {
			PX4_ERR("param import bson decoder init failed (attempt %d)", attempt);
		}

		אם (attempt != MAX_ATTEMPTS) {
			אם (lseek(fd, 0, SEEK_SET) != 0) {
				PX4_ERR("import lseek failed (%d)", errno);
			}

			px4_usleep(10000); // wait at least 10 milliseconds before trying again
		}
	}

	החזר -1;
}

#אם defined(FLASH_BASED_PARAMS)
ساكن صحيح flash_backend_locked(صحيح (*op)())
{
	/* Same order as param_save_default: mutex then shutdown lock. Autosave
	 * uses a trylock, so a compact holds it off the flash programming. */
	pthread_mutex_lock(&file_mutex);

	صحيح shutdown_lock_ret = px4_shutdown_lock();

	אם (shutdown_lock_ret != 0) {
		PX4_ERR("px4_shutdown_lock() failed (%i)", shutdown_lock_ret);
	}

	صحيح result = op();

	pthread_mutex_unlock(&file_mutex);

	אם (shutdown_lock_ret == 0) {
		px4_shutdown_unlock();
	}

	החזר result;
}

ساكن صحيح flash_load_locked()
{
	/* Reset under the lock so a pending autosave cannot append tombstones
	 * of the just-cleared RAM before import/compact run. */
	param_reset_all_internal(שקר);
	صحيح result = flash_param_import();

	אם (result == 0 || result == 1) {
		params_unsaved.reset();
	}

	החזר result;
}
#endif

صحيح
param_import(صحيح fd)
{
	אם (fd < 0) {
#אם defined(FLASH_BASED_PARAMS)
		החזר flash_backend_locked(flash_param_import);
#אחרת
		החזר flash_param_import();
#endif
	}

	החזר param_import_internal(fd);
}

صحيح
param_load(صحيح fd)
{
	אם (fd < 0) {
#אם defined(FLASH_BASED_PARAMS)
		החזר flash_backend_locked(flash_load_locked);
#אחרת
		החזר PX4_ERROR;
#endif
	}

	param_reset_all_internal(שקר);
	صحيح result = param_import_internal(fd);

#אם !defined(FLASH_BASED_PARAMS)

	אם (result >= 0) {
		params_unsaved.reset();
	}

#endif

	החזר result;
}

فراغ
param_foreach(فراغ (*func)(فراغ *arg, param_t param), فراغ *arg, منطقي only_changed, منطقي only_used)
{
	param_t	param;

	עבור (param = 0; handle_in_range(param); param++) {

		/* if requested, skip unchanged values */
		אם (only_changed && (!user_config.contains(param))) {
			استمر;
		}

		אם (only_used && !param_used(param)) {
			استمر;
		}

		func(arg, param);
	}
}

uint32_t param_hash_check()
{
	uint32_t param_hash = 0;

	/* compute the CRC32 over all string param names and 4 byte values */
	עבור (param_t param = 0; handle_in_range(param); param++) {
		אם (!param_used(param) || param_is_volatile(param)) {
			استمر;
		}

		ثابت حرف *name = param_name(param);
		auto value = user_config.get(param).i;
		ثابت فراغ *val = (فراغ *)&value;
		param_hash = crc32part((ثابت uint8_t *)name, strlen(name), param_hash);
		param_hash = crc32part((ثابت uint8_t *)val, param_size(param), param_hash);
	}

	החזר param_hash;
}

فراغ param_print_status()
{
	PX4_INFO("summary: %d/%d (used/total)", param_count_used(), param_count());

#ifndef FLASH_BASED_PARAMS
	ثابت حرف *filename = param_get_default_file();

	אם (filename != nullptr) {
		PX4_INFO("file: %s", param_get_default_file());
	}

	אם (param_backup_file) {
		PX4_INFO("backup file: %s", param_backup_file);
	}

#endif /* FLASH_BASED_PARAMS */

	PX4_INFO("storage array: %d/%d elements (%zu bytes total)",
		 user_config.size(), firmware_defaults.size(), (size_t)user_config.byteSize());


	PX4_INFO("storage array (custom defaults): %d/%d elements (%zu bytes total)",
		 runtime_defaults.size(), firmware_defaults.size(), (size_t)runtime_defaults.byteSize());

	אם (autosave_instance) {
		PX4_INFO("auto save: %s", autosave_instance->enabled() ? "on" : "off");

		hrt_abstime last_autosave = autosave_instance->lastAutosave();

		אם (last_autosave > 0) {
			PX4_INFO("last auto save: %.3f seconds ago", hrt_elapsed_time(&last_autosave) * 1e-6);
		}
	}

	perf_print_counter(param_export_perf);
	perf_print_counter(param_find_perf);
	perf_print_counter(param_get_perf);
	perf_print_counter(param_set_perf);

#אם defined(CONFIG_PARAM_PRIMARY)
	بنية param_primary_counters counts;
	param_primary_get_counters(&counts);
	PX4_INFO("set value requests received: %" PRIu32 ", set value responses sent: %" PRIu32,
		 counts.set_value_request_received, counts.set_value_response_sent);
	PX4_INFO("set value requests sent: %" PRIu32 ", set value responses received: %" PRIu32,
		 counts.set_value_request_sent, counts.set_value_response_received);
	PX4_INFO("resets sent: %" PRIu32 ", set used requests received: %" PRIu32,
		 counts.reset_sent, counts.set_used_received);
#endif

#אם defined(CONFIG_PARAM_REMOTE)
	بنية param_remote_counters counts;
	param_remote_get_counters(&counts);
	PX4_INFO("set value requests received: %" PRIu32 ", set value responses sent: %" PRIu32,
		 counts.set_value_request_received, counts.set_value_response_sent);
	PX4_INFO("set value requests sent: %" PRIu32 ", set value responses received: %" PRIu32,
		 counts.set_value_request_sent, counts.set_value_response_received);
	PX4_INFO("resets received: %" PRIu32 ", set used requests sent: %" PRIu32,
		 counts.reset_received, counts.set_used_sent);
#endif

}
