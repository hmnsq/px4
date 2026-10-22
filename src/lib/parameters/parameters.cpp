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
#شامل "param.h"
#شامل "param_translation.h"
#شامل <parameters/px4_parameters.hpp>
#شامل <lib/tinybson/tinybson.h>

#اگر defined(__PX4_NUTTX)
#شامل <nuttx/crc32.h>
#وگرنه
#شامل <crc32.h>
#endif
#شامل <float.h>
#شامل <math.h>
#شامل <sys/stat.h>

#شامل <containers/Bitset.hpp>
#شامل <drivers/drv_hrt.h>
#شامل <lib/perf/perf_counter.h>
#شامل <px4_platform_common/px4_config.h>
#شامل <px4_platform_common/atomic_bitset.h>
#شامل <px4_platform_common/defines.h>
#شامل <px4_platform_common/posix.h>
#شامل <px4_platform_common/sem.h>
#شامل <px4_platform_common/shutdown.h>
#شامل <px4_platform_common/micro_hal.h>

using فضاینام time_literals;

#شامل "uORB/uORB.h"
#شامل "uORB/topics/parameter_update.h"
#شامل <uORB/Subscription.hpp>

#شامل "ExhaustiveLayer.h"
#شامل "ConstLayer.h"
#شامل "DynamicSparseLayer.h"
#شامل "StaticSparseLayer.h"

#شامل "atomic_transaction.h"

/* Include functions common to user and kernel sides */
#شامل "parameters_common.cpp"

#اگر defined(__PX4_NUTTX) && !defined(CONFIG_BUILD_FLAT)
#شامل <px4_platform/board_ctrl.h>
#شامل "parameters_ioctl.h"
#endif

#اگر defined(FLASH_BASED_PARAMS)
#شامل "flashparams/flashparams.h"
#وگرنه
درونخطی ایستا صحیح flash_param_save(param_filter_func filter) { بازگشت -1; }
درونخطی ایستا صحیح flash_param_import() { بازگشت -1; }
#endif

ایستا نویسه *param_default_file = nullptr;
ایستا نویسه *param_backup_file = nullptr;

#شامل "autosave.h"
ایستا ParamAutosave *autosave_instance {nullptr};

ایستا px4::AtomicBitset<param_info_شمار> params_active;  // params found
ایستا px4::AtomicBitset<param_info_شمار> params_unsaved;

// Params the save in progress is writing. They are moved out of params_unsaved
// before their values are read, so a param_set that lands while the save runs
// marks its param again and reaches the next save. Flash-param saves write only
// unsaved params, so nothing else would ever write that change.
ایستا px4::AtomicBitset<param_info_شمار> params_saving;

ایستا ConstLayer firmware_defaults;
ایستا DynamicSparseLayer runtime_defaults{&firmware_defaults};
DynamicSparseLayer user_config{&runtime_defaults};

/** parameter update topic handle */
#اگر not defined(CONFIG_PARAM_REMOTE)
ایستا orb_advert_t param_topic = nullptr;
ایستا unsigned صحیح param_instance = 0;
#endif

ایستا perf_counter_t param_export_perf;
ایستا perf_counter_t param_find_perf;
ایستا perf_counter_t param_get_perf;
ایستا perf_counter_t param_set_perf;

ایستا pthread_mutex_t file_mutex  =
	PTHREAD_MUTEX_INITIALIZER; ///< this protects against concurrent param saves (file or flash access).

// Support for remote parameter node
#اگر defined(CONFIG_PARAM_PRIMARY)
# include "parameters_primary.h"
#endif // CONFIG_PARAM_PRIMARY
#اگر defined(CONFIG_PARAM_REMOTE)
# include "parameters_remote.h"
#endif // CONFIG_PARAM_REMOTE

تهی
param_init()
{
#ifdef __PX4_POSIX
	// Set up the recursive param mutex before any thread can access parameters.
	AtomicTransaction::initialize();
#endif

	param_export_perf = perf_alloc(PC_ELAPSED, "param: export");
	param_find_perf = perf_alloc(PC_شمار, "param: find");
	param_get_perf = perf_alloc(PC_شمار, "param: get");
	param_set_perf = perf_alloc(PC_ELAPSED, "param: set");

#اگر defined(__PX4_NUTTX) && !defined(CONFIG_BUILD_FLAT)
	px4_register_boardct_ioctl(_PARAMIOCBASE, param_ioctl);
#endif

#اگر defined(CONFIG_PARAM_PRIMARY)
	param_primary_init();
#endif // CONFIG_PARAM_PRIMARY

#اگر defined(CONFIG_PARAM_REMOTE)
	param_remote_init();
#endif // CONFIG_PARAM_REMOTE

#اگر not defined(CONFIG_PARAM_REMOTE)
	autosave_instance = جدید ParamAutosave();
#endif
}


تهی
param_notify_changes()
{
// Don't send if this is a remote node. Only the primary
// sends out update notices
#اگر not defined(CONFIG_PARAM_REMOTE)
	parameter_update_s pup {};
	pup.instance = param_instance++;
	pup.get_شمار_2 = perf_event_شمار(param_get_perf);
	pup.set_شمار = perf_event_شمار(param_set_perf);
	pup.find_شمار = perf_event_شمار(param_find_perf);
	pup.export_شمار = perf_event_شمار(param_export_perf);
	pup.active = params_active.شمار();
	pup.changed = user_config.size();
	pup.custom_default = runtime_defaults.size();
	pup.timestamp = hrt_absolute_time();

	اگر (param_topic == nullptr) {
		param_topic = orb_advertise(ORB_ID(parameter_update), &pup);

	} وگرنه {
		orb_publish(ORB_ID(parameter_update), param_topic, &pup);
	}

#endif
}

ایستا param_t param_find_internal(ثابت نویسه *name, منطقی notification)
{
	perf_شمار(param_find_perf);

	param_t میانه;
	param_t front = 0;
	param_t last = param_info_شمار;

	/* perform a binary search of the known parameters */

	وقتی (front <= last) {
		میانه = front + (last - front) / 2;
		صحیح ret = strcmp(name, param_name(میانه));

		اگر (ret == 0) {
			اگر (notification) {
				param_set_used(میانه);
			}

			بازگشت میانه;

		} وگرنه اگر (میانه == front) {
			/* An end point has been hit, but there has been no match */
			بشکن;

		} وگرنه اگر (ret < 0) {
			last = میانه;

		} وگرنه {
			front = میانه;
		}
	}

	/* not found */
	بازگشت PARAM_INVALID;
}

param_t param_find(ثابت نویسه *name)
{
	بازگشت param_find_internal(name, درست);
}

param_t param_find_no_notification(ثابت نویسه *name)
{
	بازگشت param_find_internal(name, نادرست);
}

unsigned param_شمار_used()
{
	بازگشت params_active.شمار();
}

param_t param_for_used_index(unsigned index)
{
	// walk all params and count used params
	اگر (index < param_info_شمار) {
		unsigned used_شمار = 0;

		برای (صحیح i = 0; i < params_active.size(); i++) {
			اگر (params_active[i]) {
				// we found the right used count,
				//  return the param value
				اگر (index == used_شمار) {
					بازگشت static_cast<param_t>(i);
				}

				used_شمار++;
			}
		}
	}

	بازگشت PARAM_INVALID;
}

صحیح param_get_used_index(param_t param)
{
	/* this tests for out of bounds and does a constant time lookup */
	اگر (!param_used(param)) {
		بازگشت -1;
	}

	/* walk all params and count, now knowing that it has a valid index */
	صحیح used_شمار = 0;

	برای (صحیح i = 0; i < params_active.size(); i++) {
		اگر (params_active[i]) {

			اگر (param == i) {
				بازگشت used_شمار;
			}

			used_شمار++;
		}
	}

	بازگشت -1;
}

منطقی
param_مقدار_unsaved(param_t param)
{
	بازگشت handle_in_range(param) ? (params_unsaved[param] || params_saving[param]) : نادرست;
}

صحیح
param_get(param_t param, تهی *val)
{
	perf_شمار(param_get_perf);

	اگر (!handle_in_range(param)) {
		PX4_ERR("get: param %" PRId16 " invalid", param);
		بازگشت PX4_ERROR;
	}

	اگر (!params_active[param]) {
		PX4_DEBUG("get: param %" PRId16 " (%s) not active", param, param_name(param));
	}

	صحیح result = PX4_ERROR;

	اگر (val) {

		auto retrieve_مقدار = user_config.get(param);

		انتخاب (param_type(param)) {
		حالت PARAM_TYPE_INT32:
			memcpy(val, &retrieve_مقدار.i, اندازه(retrieve_مقدار.i));
			بازگشت PX4_OK;

		حالت PARAM_TYPE_FLOAT:
			memcpy(val, &retrieve_مقدار.f, اندازه(retrieve_مقدار.f));
			بازگشت PX4_OK;
		}
	}

	بازگشت result;
}

صحیح
param_get_default_مقدار_internal(param_t param, تهی *default_val)
{
	اگر (!handle_in_range(param)) {
		PX4_ERR("get default value: param %d invalid", param);
		بازگشت PX4_ERROR;
	}

	اگر (default_val) {
		انتخاب (param_type(param)) {
		حالت PARAM_TYPE_INT32: {
				int32_t val = runtime_defaults.get(param).i;
				memcpy(default_val, &val, اندازه(val));
				بازگشت PX4_OK;
			}

		حالت PARAM_TYPE_FLOAT: {
				اعشاری val = runtime_defaults.get(param).f;
				memcpy(default_val, &val, اندازه(val));
				بازگشت PX4_OK;
			}
		}
	}

	بازگشت PX4_ERROR;
}

صحیح
param_get_default_مقدار(param_t param, تهی *default_val)
{
	اگر (!handle_in_range(param)) {
		بازگشت PX4_ERROR;
	}

	صحیح ret = param_get_default_مقدار_internal(param, default_val);
	بازگشت ret;
}

منطقی param_مقدار_is_default(param_t param)
{
	اگر (!handle_in_range(param)) {
		بازگشت درست;
	}

	اگر (!user_config.contains(param)) {
		// if user config does not contain it, consider it default.
		بازگشت درست;

	} وگرنه {
		// compare with default value
		ثابت param_مقدار_u user_config_مقدار = user_config.get(param);
		ثابت param_مقدار_u runtime_default_مقدار = runtime_defaults.get(param);

		انتخاب (param_type(param)) {
		حالت PARAM_TYPE_INT32: {
				بازگشت user_config_مقدار.i == runtime_default_مقدار.i;
			}

		حالت PARAM_TYPE_FLOAT: {
				بازگشت fabsf(user_config_مقدار.f - runtime_default_مقدار.f) < FLT_EPSILON;
			}
		}
	}

	بازگشت درست;
}

ایستا تهی
param_autosave()
{
	اگر (autosave_instance) {
		autosave_instance->request();
	}
}

تهی
param_control_autosave(منطقی enable)
{
	اگر (autosave_instance) {
		autosave_instance->enable(enable);
	}
}

ایستا صحیح
param_set_internal(param_t param, ثابت تهی *val, منطقی mark_saved, منطقی notify_changes, منطقی update_remote = درست,
		   منطقی autosave = درست)
{
	اگر (!handle_in_range(param)) {
		PX4_ERR("set invalid param %d", param);
		بازگشت PX4_ERROR;
	}

	اگر (val == nullptr) {
		PX4_ERR("set invalid value");
		بازگشت PX4_ERROR;
	}

	اگر (param_is_readonly(param)) {
		PX4_WARN("param %s is read-only", param_name(param));
		بازگشت PX4_ERROR;
	}

	صحیح result = -1;
	منطقی param_changed = نادرست;
	perf_begin(param_set_perf);

	ثابت param_مقدار_u user_config_مقدار = user_config.get(param);
	param_مقدار_u new_مقدار{};

	انتخاب (param_type(param)) {
	حالت PARAM_TYPE_INT32:
		memcpy(&new_مقدار.i, val, اندازه(new_مقدار.i));
		param_changed = user_config_مقدار.i != new_مقدار.i;
		بشکن;

	حالت PARAM_TYPE_FLOAT:
		memcpy(&new_مقدار.f, val, اندازه(new_مقدار.f));
		param_changed = fabsf(user_config_مقدار.f - new_مقدار.f) > FLT_EPSILON;
		بشکن;

	پیشفرض: {
			PX4_ERR("param_set invalid param type for %s", param_name(param));
			بشکن;
		}
	}

	اگر (user_config.store(param, new_مقدار)) {
		اگر (param_changed) {
			params_unsaved.set(param, !mark_saved);
		}

		result = PX4_OK;

	} وگرنه {
		PX4_ERR("param_set failed to store param %s", param_name(param));
		result = PX4_ERROR;
	}

	اگر ((result == PX4_OK) && param_changed && !mark_saved && autosave) { // this is false when importing parameters
		param_autosave();
	}

	// If this is the parameter server, make sure that the remote is updated
#اگر defined(CONFIG_PARAM_PRIMARY)

	اگر (param_changed && update_remote) {
		param_primary_set_مقدار(param, val);
	}

#endif

	// If this is the parameter remote, make sure that the primary is updated
#اگر defined(CONFIG_PARAM_REMOTE)

	اگر (param_changed && update_remote) {
		param_remote_set_مقدار(param, val);
	}

#endif

	perf_end(param_set_perf);

	/*
	 * If we set something, now that we have unlocked, go ahead and advertise that
	 * a thing has been set.
	 */
	اگر ((result == PX4_OK) && param_changed && notify_changes) {
		param_notify_changes();
	}

	بازگشت result;
}

#اگر defined(FLASH_BASED_PARAMS)
صحیح param_set_external(param_t param, ثابت تهی *val, منطقی mark_saved, منطقی notify_changes)
{
	بازگشت param_set_internal(param, val, mark_saved, notify_changes);
}

تهی param_get_external(param_t param, تهی *val)
{
	param_get(param, val);
}
#endif

صحیح param_set(param_t param, ثابت تهی *val)
{
	بازگشت param_set_internal(param, val, نادرست, درست);
}

صحیح param_set_no_notification(param_t param, ثابت تهی *val)
{
	بازگشت param_set_internal(param, val, نادرست, نادرست);
}

صحیح param_set_no_remote_update(param_t param, ثابت تهی *val, منطقی notify)
{
	بازگشت param_set_internal(param, val, نادرست, notify, نادرست);
}

منطقی param_used(param_t param)
{
	اگر (handle_in_range(param)) {
		بازگشت params_active[param];
	}

	بازگشت نادرست;
}

تهی param_set_used(param_t param)
{
	اگر (handle_in_range(param)) {
#اگر defined(CONFIG_PARAM_REMOTE)

		اگر (!param_used(param)) {
			param_remote_set_used(param);
		}

#endif

		params_active.set(param, درست);
	}
}

صحیح param_set_default_مقدار(param_t param, ثابت تهی *val)
{
	اگر (!handle_in_range(param)) {
		PX4_ERR("set default value invalid param %d", param);
		بازگشت PX4_ERROR;
	}

	اگر (val == nullptr) {
		PX4_ERR("set default value invalid value");
		بازگشت PX4_ERROR;
	}

	اگر (param_is_readonly(param)) {
		PX4_WARN("param %s is read-only", param_name(param));
		بازگشت PX4_ERROR;
	}

	صحیح result = PX4_ERROR;


	// check if param being set to default value
	منطقی setting_to_static_default = نادرست;
	ثابت param_مقدار_u firmware_default_مقدار = firmware_defaults.get(param);

	انتخاب (param_type(param)) {
	حالت PARAM_TYPE_INT32: {
			int32_t new_مقدار;
			memcpy(&new_مقدار, val, اندازه(new_مقدار));
			setting_to_static_default = firmware_default_مقدار.i == new_مقدار;
			بشکن;
		}

	حالت PARAM_TYPE_FLOAT: {
			اعشاری new_مقدار;
			memcpy(&new_مقدار, val, اندازه(new_مقدار));
			setting_to_static_default = fabsf(firmware_default_مقدار.f - new_مقدار) <= FLT_EPSILON;
			بشکن;
		}
	}

	اگر (setting_to_static_default) {
		runtime_defaults.reset(param);

		result = PX4_OK;

	} وگرنه {
		param_مقدار_u new_مقدار{};

		انتخاب (param_type(param)) {
		حالت PARAM_TYPE_INT32: {
				memcpy(&new_مقدار.i, val, اندازه(new_مقدار.i));
				بشکن;
			}

		حالت PARAM_TYPE_FLOAT: {
				memcpy(&new_مقدار.f, val, اندازه(new_مقدار.f));
				بشکن;
			}

		پیشفرض:
			بشکن;
		}

		اگر (runtime_defaults.store(param, new_مقدار)) {
			user_config.refresh(param);
			result = PX4_OK;

		} وگرنه {
			result = PX4_ERROR;
		}
	}


	اگر ((result == PX4_OK) && param_used(param)) {
		// send notification if param is already in use
		param_notify_changes();
	}

	بازگشت result;
}

ایستا صحیح param_reset_internal(param_t param, منطقی notify = درست, منطقی autosave = درست)
{
#اگر defined(CONFIG_PARAM_REMOTE)
	// Remote doesn't support reset
	بازگشت نادرست;
#endif

	اگر (param_is_readonly(param)) {
		بازگشت 0;  // silently skip — param_reset_all loops over all params
	}

	منطقی param_found = user_config.contains(param);

	اگر (handle_in_range(param)) {
		user_config.reset(param);
	}

	اگر (param_found) {
		/* Persist the reset. Flash-param saves append only unsaved params,
		 * so a reset has to show up as a tombstone or replay keeps the
		 * previous override. */
		params_unsaved.set(param, درست);
	}

	اگر (autosave) {
		param_autosave();
	}

	اگر (param_found && notify) {
		param_notify_changes();
	}

#اگر defined(CONFIG_PARAM_PRIMARY)
	param_primary_reset(param);
#endif

	بازگشت param_found;
}

صحیح param_reset(param_t param) { بازگشت param_reset_internal(param, درست); }
صحیح param_reset_no_notification(param_t param) { بازگشت param_reset_internal(param, نادرست); }

ایستا تهی
param_reset_all_internal(منطقی auto_save)
{
#اگر defined(CONFIG_PARAM_REMOTE)
	// Remote doesn't support reset
	بازگشت;
#endif

	برای (param_t param = 0; handle_in_range(param); param++) {
		param_reset_internal(param, نادرست, نادرست);
	}

	اگر (auto_save) {
		param_autosave();
	}

#اگر defined(CONFIG_PARAM_PRIMARY)
	param_primary_reset_all();
#endif

	param_notify_changes();
}

تهی
param_reset_all()
{
	param_reset_all_internal(درست);
}

تهی
param_reset_excludes(ثابت نویسه *excludes[], صحیح رقم_excludes)
{
	برای (param_t param = 0; handle_in_range(param); param++) {
		ثابت نویسه *name = param_name(param);
		منطقی exclude = نادرست;

		برای (صحیح index = 0; index < رقم_excludes; index ++) {
			صحیح len = strlen(excludes[index]);

			اگر ((excludes[index][len - 1] == '*'
			     && strncmp(name, excludes[index], len - 1) == 0)
			    || strcmp(name, excludes[index]) == 0) {

				exclude = درست;
				بشکن;
			}
		}

		اگر (!exclude) {
			param_reset(param);
		}
	}
}

تهی
param_reset_specific(ثابت نویسه *resets[], صحیح رقم_resets)
{
	برای (param_t param = 0; handle_in_range(param); param++) {
		ثابت نویسه *name = param_name(param);
		منطقی reset = نادرست;

		برای (صحیح index = 0; index < رقم_resets; index++) {
			صحیح len = strlen(resets[index]);

			اگر ((resets[index][len - 1] == '*'
			     && strncmp(name, resets[index], len - 1) == 0)
			    || strcmp(name, resets[index]) == 0) {

				reset = درست;
				بشکن;
			}
		}

		اگر (reset) {
			param_reset(param);
		}
	}
}

صحیح
param_set_default_file(ثابت نویسه *filename)
{
	اگر (filename && param_backup_file && strcmp(filename, param_backup_file) == 0) {
		PX4_ERR("default file can't be the same as the backup file %s", filename);
		بازگشت PX4_ERROR;
	}

#ifdef FLASH_BASED_PARAMS
	// the default for flash-based params is always the FLASH
	(تهی)filename;
#وگرنه

	اگر (param_default_file != nullptr) {
		// we assume this is not in use by some other thread
		آزاد(param_default_file);
		param_default_file = nullptr;
	}

	اگر (filename) {
		param_default_file = strdup(filename);
	}

#endif /* FLASH_BASED_PARAMS */

	بازگشت 0;
}

ثابت نویسه *param_get_default_file()
{
	بازگشت param_default_file;
}

صحیح param_set_backup_file(ثابت نویسه *filename)
{
	اگر (filename && param_default_file && strcmp(filename, param_default_file) == 0) {
		PX4_ERR("backup file can't be the same as the default file %s", filename);
		بازگشت PX4_ERROR;
	}

	اگر (param_backup_file != nullptr) {
		// we assume this is not in use by some other thread
		آزاد(param_backup_file);
		param_backup_file = nullptr;
	}

	اگر (filename) {
		param_backup_file = strdup(filename);

	} وگرنه {
		param_backup_file = nullptr; // backup disabled
	}

	بازگشت 0;
}

ثابت نویسه *param_get_backup_file()
{
	بازگشت param_backup_file;
}

ایستا صحیح param_export_internal(صحیح fd, param_filter_func filter);
ایستا صحیح param_verify(صحیح fd);

صحیح param_save_default(منطقی blocking)
{
	PX4_DEBUG("param_save_default");

	// take the file lock
	اگر (blocking) {
		pthread_mutex_lock(&file_mutex);

	} وگرنه {
		اگر (pthread_mutex_trylock(&file_mutex) != 0) {
			PX4_DEBUG("param_save_default: file lock failed (already locked)");
			بازگشت -EWOULDBLOCK;
		}
	}

	صحیح shutdown_lock_ret = px4_shutdown_lock();

	اگر (shutdown_lock_ret != 0) {
		PX4_ERR("px4_shutdown_lock() failed (%i)", shutdown_lock_ret);
	}

	صحیح res = PX4_ERROR;
	ثابت نویسه *filename = param_get_default_file();

	برای (param_t param = 0; handle_in_range(param); param++) {
		اگر (params_unsaved[param]) {
			params_saving.set(param, درست);
			params_unsaved.set(param, نادرست);
		}
	}

	اگر (filename) {
		ایستا constexpr صحیح MAX_ATTEMPTS = 3;

		برای (صحیح attempt = 1; attempt <= MAX_ATTEMPTS; attempt++) {
			// write parameters to file
			صحیح fd = ::open(filename, O_WRONLY | O_CREAT | O_TRUNC, PX4_O_MODE_666);

			اگر (fd > -1) {
				perf_begin(param_export_perf);
				res = param_export_internal(fd, nullptr);
				perf_end(param_export_perf);
				::close(fd);

				اگر (res == PX4_OK) {
					// reopen file to verify
					صحیح fd_verify = ::open(filename, O_RDONLY, PX4_O_MODE_666);
					res = param_verify(fd_verify) || lseek(fd_verify, 0, SEEK_SET) || param_verify(fd_verify);
					::close(fd_verify);
				}
			}

			اگر (res == PX4_OK) {
				بشکن;

			} وگرنه {
				PX4_ERR("parameter export to %s failed (%d) attempt %d", filename, res, attempt);
				px4_usleep(10000); // wait at least 10 milliseconds before trying again
			}
		}

	} وگرنه {
		perf_begin(param_export_perf);
		res = flash_param_save(nullptr);
		perf_end(param_export_perf);
	}

	اگر (res != PX4_OK) {
		PX4_ERR("param export failed (%d)", res);

		برای (param_t param = 0; handle_in_range(param); param++) {
			اگر (params_saving[param]) {
				params_unsaved.set(param, درست);
			}
		}
	}

	params_saving.reset();

	اگر (res == PX4_OK) {
		// backup file
		اگر (param_backup_file) {
			صحیح fd_backup_file = ::open(param_backup_file, O_WRONLY | O_CREAT | O_TRUNC, PX4_O_MODE_666);

			اگر (fd_backup_file > -1) {
				صحیح backup_export_ret = param_export_internal(fd_backup_file, nullptr);
				::close(fd_backup_file);

				اگر (backup_export_ret != 0) {
					PX4_ERR("backup parameter export to %s failed (%d)", param_backup_file, backup_export_ret);

				} وگرنه {
					// verify export
					صحیح fd_verify = ::open(param_backup_file, O_RDONLY, PX4_O_MODE_666);
					param_verify(fd_verify);
					::close(fd_verify);
				}
			}
		}
	}

	pthread_mutex_unlock(&file_mutex);

	اگر (shutdown_lock_ret == 0) {
		px4_shutdown_unlock();
	}

	بازگشت res;
}

/**
 * @return 0 on success, 1 if all params have not yet been stored, -1 if device open failed, -2 if writing parameters failed
 */
صحیح
param_load_default()
{
	صحیح res = 0;
	ثابت نویسه *filename = param_get_default_file();

	اگر (!filename) {
		بازگشت param_load(-1);
	}

	صحیح fd_load = ::open(filename, O_RDONLY);

	اگر (fd_load < 0) {
		/* no parameter file is OK, otherwise this is an error */
		اگر (errno != ENOENT) {
			PX4_ERR("open '%s' for reading failed", filename);
			بازگشت -1;
		}

		بازگشت 1;
	}

	صحیح result = param_load(fd_load);
	::close(fd_load);

	اگر (result == 1) {
		/* blank file: nothing stored yet */
		بازگشت 1;
	}

	اگر (result != 0) {
		PX4_ERR("error reading parameters from '%s'", filename);
		بازگشت -2;
	}

	بازگشت res;
}

ایستا صحیح param_verify_callback(bson_decoder_t decoder, bson_گره_t گره_2)
{
	اگر (گره_2->type == BSON_EOO) {
		بازگشت 0;
	}

	// find the parameter this node represents
	param_t param = param_find_no_notification(گره_2->name);

	اگر (param == PARAM_INVALID) {
		PX4_ERR("verify: invalid parameter '%s'", گره_2->name);
		بازگشت -1;
	}

	// handle verifying the parameter from the node
	انتخاب (گره_2->type) {
	حالت BSON_INT32: {
			اگر (param_type(param) != PARAM_TYPE_INT32) {
				PX4_ERR("verify: invalid param type %d for '%s' (BSON_INT32)", param_type(param), گره_2->name);
				بازگشت -1;
			}

			int32_t value;

			اگر (param_get(param, &value) == 0) {
				اگر (value == گره_2->i32) {
					بازگشت 1; // valid

				} وگرنه {
					PX4_ERR("verify: '%s' invalid BSON value %" PRIi32 "(expected %" PRIi32 ")", گره_2->name, گره_2->i32, value);
				}
			}
		}
		بشکن;

	حالت BSON_DOUBLE: {
			اگر (param_type(param) != PARAM_TYPE_FLOAT) {
				PX4_ERR("verify: invalid param type %d for '%s' (BSON_DOUBLE)", param_type(param), گره_2->name);
				بازگشت -1;
			}

			اعشاری value;

			اگر (param_get(param, &value) == 0) {
				اگر (fabsf(value - (اعشاری)گره_2->d) <= FLT_EPSILON) {
					بازگشت 1; // valid

				} وگرنه {
					PX4_ERR("verify: '%s' invalid BSON value %.3f (expected %.3f)", گره_2->name, گره_2->d, (دوگانه)value);
				}
			}
		}
		بشکن;

	پیشفرض:
		PX4_ERR("verify: '%s' invalid node type %d", گره_2->name, گره_2->type);
	}

	بازگشت -1;
}

ایستا صحیح param_verify(صحیح fd)
{
	PX4_DEBUG("param_verify");

	اگر (fd < 0) {
		بازگشت -1;
	}

	اگر (lseek(fd, 0, SEEK_SET) != 0) {
		PX4_ERR("verify: seek failed");
		بازگشت -1;
	}

	bson_decoder_s decoder{};

	اگر (bson_decoder_init_file(&decoder, fd, param_verify_callback) == 0) {
		صحیح result = -1;

		انجامبده {
			result = bson_decoder_next(&decoder);

		} وقتی (result > 0);

		اگر (result == 0) {
			اگر (decoder.total_document_size != decoder.total_decoded_size) {
				PX4_ERR("BSON document size (%" PRId32 ") doesn't match bytes decoded (%" PRId32 ")", decoder.total_document_size,
					decoder.total_decoded_size);

			} وگرنه {
				بازگشت 0;
			}

		} وگرنه اگر (result == -ENODATA) {
			PX4_ERR("verify: no BSON data");

		} وگرنه {
			PX4_ERR("verify: failed (%d)", result);
		}
	}

	بازگشت -1;
}

صحیح
param_export(ثابت نویسه *filename, param_filter_func filter)
{
	PX4_DEBUG("param_export");

	صحیح shutdown_lock_ret = px4_shutdown_lock();

	اگر (shutdown_lock_ret != 0) {
		PX4_ERR("px4_shutdown_lock() failed (%i)", shutdown_lock_ret);
	}

	// take the file lock
	اگر (pthread_mutex_trylock(&file_mutex) != 0) {
		PX4_ERR("param_export: file lock failed (already locked)");
		بازگشت PX4_ERROR;
	}

	صحیح fd = ::open(filename, O_RDWR | O_CREAT, PX4_O_MODE_666);
	صحیح result = PX4_ERROR;

	perf_begin(param_export_perf);

	اگر (fd > -1) {
		result = param_export_internal(fd, filter);

	} وگرنه {
		result = flash_param_save(filter);
	}

	perf_end(param_export_perf);

	pthread_mutex_unlock(&file_mutex);

	اگر (shutdown_lock_ret == 0) {
		px4_shutdown_unlock();
	}

	بازگشت result;
}

// internal parameter export, caller is responsible for locking
ایستا صحیح param_export_internal(صحیح fd, param_filter_func filter)
{
	PX4_DEBUG("param_export_internal");
	ثابت auto changed_params = user_config.containedAsBitset();

	صحیح result = -1;
	bson_encoder_s encoder{};
	uint8_t bson_buffer[256];

	اگر (lseek(fd, 0, SEEK_SET) != 0) {
		PX4_ERR("export seek failed %d", errno);
		بازگشت -1;
	}

	اگر (bson_encoder_init_buf_file(&encoder, fd, &bson_buffer, اندازه(bson_buffer)) != 0) {
		برو out;
	}

	برای (param_t param = 0; handle_in_range(param); param++) {
		اگر (!changed_params[param] || (filter && !filter(param))) {
			ادامه;
		}

		ثابت param_مقدار_u runtime_default_مقدار = runtime_defaults.get(param);
		ثابت param_مقدار_u user_config_مقدار = user_config.get(param);

		// don't export default values
		انتخاب (param_type(param)) {
		حالت PARAM_TYPE_INT32:
			اگر (user_config_مقدار.i == runtime_default_مقدار.i) {
				PX4_DEBUG("skipping %s %" PRIi32 " export", param_name(param), runtime_default_مقدار.i);
				ادامه;
			}

			بشکن;

		حالت PARAM_TYPE_FLOAT:
			اگر (fabsf(user_config_مقدار.f - runtime_default_مقدار.f) <= FLT_EPSILON) {
				PX4_DEBUG("skipping %s %.3f export", param_name(param), (دوگانه)runtime_default_مقدار.f);
				ادامه;
			}

			بشکن;
		}

		ثابت نویسه *name = param_name(param);
		ثابت size_t size = param_size(param);

		/* append the appropriate BSON type object */
		انتخاب (param_type(param)) {
		حالت PARAM_TYPE_INT32: {
				ثابت int32_t i = user_config_مقدار.i;
				PX4_DEBUG("exporting: %s (%d) size: %lu val: %" PRIi32, name, param, (long unsigned صحیح)size, i);

				اگر (bson_encoder_append_int32(&encoder, name, i) != 0) {
					PX4_ERR("BSON append failed for '%s'", name);
					برو out;
				}
			}
			بشکن;

		حالت PARAM_TYPE_FLOAT: {
				ثابت دوگانه f = (دوگانه)user_config_مقدار.f;
				PX4_DEBUG("exporting: %s (%d) size: %lu val: %.3f", name, param, (long unsigned صحیح)size, (دوگانه)f);

				اگر (bson_encoder_append_double(&encoder, name, f) != 0) {
					PX4_ERR("BSON append failed for '%s'", name);
					برو out;
				}
			}
			بشکن;

		پیشفرض:
			PX4_ERR("%s unrecognized parameter type %d, skipping export", name, param_type(param));
		}
	}

	result = 0;

out:

	اگر (result == 0) {
		اگر (bson_encoder_fini(&encoder) != PX4_OK) {
			PX4_ERR("BSON encoder finalize failed");
			result = -1;
		}
	}

	بازگشت result;
}

#اگر defined(FLASH_BASED_PARAMS)
// A file is never the flash backend's store, so what an import changes is unsaved until a delta carries it.
// As on the file backend, the import itself does not save.
ایستا constexpr منطقی file_import_is_saved = نادرست;
#وگرنه
ایستا constexpr منطقی file_import_is_saved = درست;
#endif

ایستا صحیح
param_import_callback(bson_decoder_t decoder, bson_گره_t گره_2)
{
	/*
	 * EOO means the end of the parameter object. (Currently not supporting
	 * nested BSON objects).
	 */
	اگر (گره_2->type == BSON_EOO) {
		PX4_DEBUG("end of parameters");
		بازگشت 0;
	}

	// A tombstone has no value. The decoder leaves the previous node's in the union and the translations
	// read it without a type check.
	ثابت منطقی tombstone = گره_2->type == BSON_nullptr || گره_2->type == BSON_UNDEFINED;

	// if we do param_set() directly in the translation, set PARAM_SKIP_IMPORT as return value and return here
	اگر (!tombstone && param_modify_on_import(گره_2) == param_modify_on_import_ret::PARAM_SKIP_IMPORT) {
		بازگشت 1;
	}

	// Find the parameter this node represents.  If we don't know it, ignore the node.
	param_t param = param_find_no_notification(گره_2->name);

	اگر (param == PARAM_INVALID) {
		PX4_WARN("ignoring unrecognised parameter '%s'", گره_2->name);
		بازگشت 1;
	}

	// Handle setting the parameter from the node
	انتخاب (گره_2->type) {
	حالت BSON_nullptr:
	حالت BSON_UNDEFINED:
		param_reset_internal(param, درست, نادرست);
		بازگشت 1;

	حالت BSON_INT32: {
			اگر (param_type(param) == PARAM_TYPE_INT32) {
				int32_t i = گره_2->i32;
				param_set_internal(param, &i, file_import_is_saved, درست, درست, نادرست);
				PX4_DEBUG("Imported %s with value %" PRIi32, param_name(param), i);

			} وگرنه {
				PX4_WARN("unexpected type for %s", گره_2->name);
			}
		}
		بشکن;

	حالت BSON_DOUBLE: {
			اگر (param_type(param) == PARAM_TYPE_FLOAT) {
				اعشاری f = گره_2->d;
				param_set_internal(param, &f, file_import_is_saved, درست, درست, نادرست);
				PX4_DEBUG("Imported %s with value %f", param_name(param), (دوگانه)f);

			} وگرنه {
				PX4_WARN("unexpected type for %s", گره_2->name);
			}
		}
		بشکن;

	پیشفرض:
		PX4_ERR("import: unrecognised node type for '%s'", گره_2->name);
	}

	// don't return zero, that means EOF
	بازگشت 1;
}

ایستا صحیح
param_import_internal(صحیح fd)
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
	ساختار stat st;

	اگر (fstat(fd, &st) == 0 && S_ISREG(st.st_mode) && st.st_size == 0) {
		بازگشت 1;
	}

	int32_t doc_size = 0;
	ssize_t len = اقرأ(fd, &doc_size, اندازه(doc_size));

	اگر (len == 0 || (len == (ssize_t)اندازه(doc_size) && (doc_size == 0 || doc_size == -1))) {
		بازگشت 1;
	}

	اگر (lseek(fd, 0, SEEK_SET) != 0) {
		PX4_ERR("import lseek failed (%d)", errno);
		بازگشت -1;
	}

	ایستا constexpr صحیح MAX_ATTEMPTS = 3;

	برای (صحیح attempt = 1; attempt <= MAX_ATTEMPTS; attempt++) {
		bson_decoder_s decoder{};

		اگر (bson_decoder_init_file(&decoder, fd, param_import_callback) == 0) {
			صحیح result = -1;

			انجامبده {
				result = bson_decoder_next(&decoder);

			} وقتی (result > 0);

			اگر (result == 0) {
				اگر (decoder.total_document_size == decoder.total_decoded_size) {
					PX4_INFO("BSON document size %" PRId32 " bytes, decoded %" PRId32 " bytes (INT32:%" PRIu16 ", FLOAT:%" PRIu16 ")",
						 decoder.total_document_size, decoder.total_decoded_size,
						 decoder.شمار_گره_int32, decoder.شمار_گره_double);

					بازگشت 0;

				} وگرنه {
					PX4_ERR("BSON document size (%" PRId32 ") doesn't match bytes decoded (%" PRId32 ")",
						decoder.total_document_size, decoder.total_decoded_size);
				}

			} وگرنه اگر (result == -ENODATA) {
				// silently retry as a precaution unless this is our last attempt
				اگر (attempt == MAX_ATTEMPTS) {
					PX4_DEBUG("BSON: no data");
					بازگشت 0;
				}

			} وگرنه {
				PX4_ERR("param import failed (%d) attempt %d", result, attempt);
			}

		} وگرنه {
			PX4_ERR("param import bson decoder init failed (attempt %d)", attempt);
		}

		اگر (attempt != MAX_ATTEMPTS) {
			اگر (lseek(fd, 0, SEEK_SET) != 0) {
				PX4_ERR("import lseek failed (%d)", errno);
			}

			px4_usleep(10000); // wait at least 10 milliseconds before trying again
		}
	}

	بازگشت -1;
}

#اگر defined(FLASH_BASED_PARAMS)
ایستا صحیح flash_backend_locked(صحیح (*op)())
{
	/* Same order as param_save_default: mutex then shutdown lock. Autosave
	 * uses a trylock, so a compact holds it off the flash programming. */
	pthread_mutex_lock(&file_mutex);

	صحیح shutdown_lock_ret = px4_shutdown_lock();

	اگر (shutdown_lock_ret != 0) {
		PX4_ERR("px4_shutdown_lock() failed (%i)", shutdown_lock_ret);
	}

	صحیح result = op();

	pthread_mutex_unlock(&file_mutex);

	اگر (shutdown_lock_ret == 0) {
		px4_shutdown_unlock();
	}

	بازگشت result;
}

ایستا صحیح flash_load_locked()
{
	/* Reset under the lock so a pending autosave cannot append tombstones
	 * of the just-cleared RAM before import/compact run. */
	param_reset_all_internal(نادرست);
	صحیح result = flash_param_import();

	اگر (result == 0 || result == 1) {
		params_unsaved.reset();
	}

	بازگشت result;
}
#endif

صحیح
param_import(صحیح fd)
{
	اگر (fd < 0) {
#اگر defined(FLASH_BASED_PARAMS)
		بازگشت flash_backend_locked(flash_param_import);
#وگرنه
		بازگشت flash_param_import();
#endif
	}

	بازگشت param_import_internal(fd);
}

صحیح
param_load(صحیح fd)
{
	اگر (fd < 0) {
#اگر defined(FLASH_BASED_PARAMS)
		بازگشت flash_backend_locked(flash_load_locked);
#وگرنه
		بازگشت PX4_ERROR;
#endif
	}

	param_reset_all_internal(نادرست);
	صحیح result = param_import_internal(fd);

#اگر !defined(FLASH_BASED_PARAMS)

	اگر (result >= 0) {
		params_unsaved.reset();
	}

#endif

	بازگشت result;
}

تهی
param_foreach(تهی (*func)(تهی *arg, param_t param), تهی *arg, منطقی only_changed, منطقی only_used)
{
	param_t	param;

	برای (param = 0; handle_in_range(param); param++) {

		/* if requested, skip unchanged values */
		اگر (only_changed && (!user_config.contains(param))) {
			ادامه;
		}

		اگر (only_used && !param_used(param)) {
			ادامه;
		}

		func(arg, param);
	}
}

uint32_t param_hash_check()
{
	uint32_t param_hash = 0;

	/* compute the CRC32 over all string param names and 4 byte values */
	برای (param_t param = 0; handle_in_range(param); param++) {
		اگر (!param_used(param) || param_is_volatile(param)) {
			ادامه;
		}

		ثابت نویسه *name = param_name(param);
		auto value = user_config.get(param).i;
		ثابت تهی *val = (تهی *)&value;
		param_hash = crc32part((ثابت uint8_t *)name, strlen(name), param_hash);
		param_hash = crc32part((ثابت uint8_t *)val, param_size(param), param_hash);
	}

	بازگشت param_hash;
}

تهی param_print_status()
{
	PX4_INFO("summary: %d/%d (used/total)", param_شمار_used(), param_شمار());

#ifndef FLASH_BASED_PARAMS
	ثابت نویسه *filename = param_get_default_file();

	اگر (filename != nullptr) {
		PX4_INFO("file: %s", param_get_default_file());
	}

	اگر (param_backup_file) {
		PX4_INFO("backup file: %s", param_backup_file);
	}

#endif /* FLASH_BASED_PARAMS */

	PX4_INFO("storage array: %d/%d elements (%zu bytes total)",
		 user_config.size(), firmware_defaults.size(), (size_t)user_config.byteSize());


	PX4_INFO("storage array (custom defaults): %d/%d elements (%zu bytes total)",
		 runtime_defaults.size(), firmware_defaults.size(), (size_t)runtime_defaults.byteSize());

	اگر (autosave_instance) {
		PX4_INFO("auto save: %s", autosave_instance->enabled() ? "on" : "off");

		hrt_abstime last_autosave = autosave_instance->lastAutosave();

		اگر (last_autosave > 0) {
			PX4_INFO("last auto save: %.3f seconds ago", hrt_elapsed_time(&last_autosave) * 1e-6);
		}
	}

	perf_print_counter(param_export_perf);
	perf_print_counter(param_find_perf);
	perf_print_counter(param_get_perf);
	perf_print_counter(param_set_perf);

#اگر defined(CONFIG_PARAM_PRIMARY)
	ساختار param_primary_counters counts;
	param_primary_get_counters(&counts);
	PX4_INFO("set value requests received: %" PRIu32 ", set value responses sent: %" PRIu32,
		 counts.set_مقدار_request_received, counts.set_مقدار_response_sent);
	PX4_INFO("set value requests sent: %" PRIu32 ", set value responses received: %" PRIu32,
		 counts.set_مقدار_request_sent, counts.set_مقدار_response_received);
	PX4_INFO("resets sent: %" PRIu32 ", set used requests received: %" PRIu32,
		 counts.reset_sent, counts.set_used_received);
#endif

#اگر defined(CONFIG_PARAM_REMOTE)
	ساختار param_remote_counters counts;
	param_remote_get_counters(&counts);
	PX4_INFO("set value requests received: %" PRIu32 ", set value responses sent: %" PRIu32,
		 counts.set_مقدار_request_received, counts.set_مقدار_response_sent);
	PX4_INFO("set value requests sent: %" PRIu32 ", set value responses received: %" PRIu32,
		 counts.set_مقدار_request_sent, counts.set_مقدار_response_received);
	PX4_INFO("resets received: %" PRIu32 ", set used requests sent: %" PRIu32,
		 counts.reset_received, counts.set_used_sent);
#endif

}
