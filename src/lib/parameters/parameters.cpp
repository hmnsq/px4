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

#إذا defined(__PX4_NUTTX)
#ضمن <nuttx/crc32.h>
#وإلا
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

#إذا defined(__PX4_NUTTX) && !defined(CONFIG_BUILD_FLAT)
#ضمن <px4_platform/board_ctrl.h>
#ضمن "parameters_ioctl.h"
#endif

#إذا defined(FLASH_BASED_PARAMS)
#ضمن "flashparams/flashparams.h"
#وإلا
مضمن ساكن صحيح flash_param_save(param_filter_func filter) { إرجاع -1; }
مضمن ساكن صحيح flash_param_import() { إرجاع -1; }
#endif

ساكن حرف *param_default_file = nullptr;
ساكن حرف *param_backup_file = nullptr;

#ضمن "autosave.h"
ساكن ParamAutosave *autosave_instance {nullptr};

ساكن px4::AtomicBitset<param_info_تعداد> params_active;  // params found
ساكن px4::AtomicBitset<param_info_تعداد> params_unsaved;

// Params the save in progress is writing. They are moved out of params_unsaved
// before their values are read, so a param_set that lands while the save runs
// marks its param again and reaches the next save. Flash-param saves write only
// unsaved params, so nothing else would ever write that change.
ساكن px4::AtomicBitset<param_info_تعداد> params_saving;

ساكن ConstLayer firmware_defaults;
ساكن DynamicSparseLayer runtime_defaults{&firmware_defaults};
DynamicSparseLayer user_config{&runtime_defaults};

/** parameter update topic handle */
#إذا not defined(CONFIG_PARAM_REMOTE)
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
#إذا defined(CONFIG_PARAM_PRIMARY)
# include "parameters_primary.h"
#endif // CONFIG_PARAM_PRIMARY
#إذا defined(CONFIG_PARAM_REMOTE)
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
	param_find_perf = perf_alloc(PC_تعداد, "param: find");
	param_get_perf = perf_alloc(PC_تعداد, "param: get");
	param_set_perf = perf_alloc(PC_ELAPSED, "param: set");

#إذا defined(__PX4_NUTTX) && !defined(CONFIG_BUILD_FLAT)
	px4_register_boardct_ioctl(_PARAMIOCBASE, param_ioctl);
#endif

#إذا defined(CONFIG_PARAM_PRIMARY)
	param_primary_init();
#endif // CONFIG_PARAM_PRIMARY

#إذا defined(CONFIG_PARAM_REMOTE)
	param_remote_init();
#endif // CONFIG_PARAM_REMOTE

#إذا not defined(CONFIG_PARAM_REMOTE)
	autosave_instance = جديد ParamAutosave();
#endif
}


فراغ
param_notify_changes()
{
// Don't send if this is a remote node. Only the primary
// sends out update notices
#إذا not defined(CONFIG_PARAM_REMOTE)
	parameter_update_s pup {};
	pup.instance = param_instance++;
	pup.get_تعداد_2 = perf_event_تعداد(param_get_perf);
	pup.set_تعداد = perf_event_تعداد(param_set_perf);
	pup.find_تعداد = perf_event_تعداد(param_find_perf);
	pup.export_تعداد = perf_event_تعداد(param_export_perf);
	pup.active = params_active.تعداد_2();
	pup.changed = user_config.size();
	pup.custom_default = runtime_defaults.size();
	pup.timestamp = hrt_absolute_time();

	إذا (param_topic == nullptr) {
		param_topic = orb_advertise(ORB_ID(parameter_update), &pup);

	} وإلا {
		orb_publish(ORB_ID(parameter_update), param_topic, &pup);
	}

#endif
}

ساكن param_t param_find_internal(ثابت حرف *name, منطقي notification)
{
	perf_تعداد(param_find_perf);

	param_t وسط;
	param_t front = 0;
	param_t last = param_info_تعداد;

	/* perform a binary search of the known parameters */

	بينما (front <= last) {
		وسط = front + (last - front) / 2;
		صحيح ret = strcmp(name, param_name(وسط));

		إذا (ret == 0) {
			إذا (notification) {
				param_set_used(وسط);
			}

			إرجاع وسط;

		} وإلا إذا (وسط == front) {
			/* An end point has been hit, but there has been no match */
			اكسر;

		} وإلا إذا (ret < 0) {
			last = وسط;

		} وإلا {
			front = وسط;
		}
	}

	/* not found */
	إرجاع PARAM_INVALID;
}

param_t param_find(ثابت حرف *name)
{
	إرجاع param_find_internal(name, true);
}

param_t param_find_no_notification(ثابت حرف *name)
{
	إرجاع param_find_internal(name, خطأ);
}

unsigned param_تعداد_used()
{
	إرجاع params_active.تعداد_2();
}

param_t param_for_used_index(unsigned index)
{
	// walk all params and count used params
	إذا (index < param_info_تعداد) {
		unsigned used_تعداد = 0;

		لكل (صحيح i = 0; i < params_active.size(); i++) {
			إذا (params_active[i]) {
				// we found the right used count,
				//  return the param value
				إذا (index == used_تعداد) {
					إرجاع static_cast<param_t>(i);
				}

				used_تعداد++;
			}
		}
	}

	إرجاع PARAM_INVALID;
}

صحيح param_get_used_index(param_t param)
{
	/* this tests for out of bounds and does a constant time lookup */
	إذا (!param_used(param)) {
		إرجاع -1;
	}

	/* walk all params and count, now knowing that it has a valid index */
	صحيح used_تعداد = 0;

	لكل (صحيح i = 0; i < params_active.size(); i++) {
		إذا (params_active[i]) {

			إذا (param == i) {
				إرجاع used_تعداد;
			}

			used_تعداد++;
		}
	}

	إرجاع -1;
}

منطقي
param_قيمة_unsaved(param_t param)
{
	إرجاع handle_in_range(param) ? (params_unsaved[param] || params_saving[param]) : خطأ;
}

صحيح
param_get(param_t param, فراغ *val)
{
	perf_تعداد(param_get_perf);

	إذا (!handle_in_range(param)) {
		PX4_ERR("get: param %" PRId16 " invalid", param);
		إرجاع PX4_ERROR;
	}

	إذا (!params_active[param]) {
		PX4_DEBUG("get: param %" PRId16 " (%s) not active", param, param_name(param));
	}

	صحيح result = PX4_ERROR;

	إذا (val) {

		auto retrieve_قيمة = user_config.get(param);

		بدل (param_type(param)) {
		حالة PARAM_TYPE_INT32:
			memcpy(val, &retrieve_قيمة.i, حجم(retrieve_قيمة.i));
			إرجاع PX4_OK;

		حالة PARAM_TYPE_FLOAT:
			memcpy(val, &retrieve_قيمة.f, حجم(retrieve_قيمة.f));
			إرجاع PX4_OK;
		}
	}

	إرجاع result;
}

صحيح
param_get_default_قيمة_internal(param_t param, فراغ *default_val)
{
	إذا (!handle_in_range(param)) {
		PX4_ERR("get default value: param %d invalid", param);
		إرجاع PX4_ERROR;
	}

	إذا (default_val) {
		بدل (param_type(param)) {
		حالة PARAM_TYPE_INT32: {
				int32_t val = runtime_defaults.get(param).i;
				memcpy(default_val, &val, حجم(val));
				إرجاع PX4_OK;
			}

		حالة PARAM_TYPE_FLOAT: {
				عائم val = runtime_defaults.get(param).f;
				memcpy(default_val, &val, حجم(val));
				إرجاع PX4_OK;
			}
		}
	}

	إرجاع PX4_ERROR;
}

صحيح
param_get_default_قيمة(param_t param, فراغ *default_val)
{
	إذا (!handle_in_range(param)) {
		إرجاع PX4_ERROR;
	}

	صحيح ret = param_get_default_قيمة_internal(param, default_val);
	إرجاع ret;
}

منطقي param_قيمة_is_default(param_t param)
{
	إذا (!handle_in_range(param)) {
		إرجاع true;
	}

	إذا (!user_config.contains(param)) {
		// if user config does not contain it, consider it default.
		إرجاع true;

	} وإلا {
		// compare with default value
		ثابت param_قيمة_u user_config_قيمة = user_config.get(param);
		ثابت param_قيمة_u runtime_default_قيمة = runtime_defaults.get(param);

		بدل (param_type(param)) {
		حالة PARAM_TYPE_INT32: {
				إرجاع user_config_قيمة.i == runtime_default_قيمة.i;
			}

		حالة PARAM_TYPE_FLOAT: {
				إرجاع fabsf(user_config_قيمة.f - runtime_default_قيمة.f) < FLT_EPSILON;
			}
		}
	}

	إرجاع true;
}

ساكن فراغ
param_autosave()
{
	إذا (autosave_instance) {
		autosave_instance->request();
	}
}

فراغ
param_control_autosave(منطقي enable)
{
	إذا (autosave_instance) {
		autosave_instance->enable(enable);
	}
}

ساكن صحيح
param_set_internal(param_t param, ثابت فراغ *val, منطقي mark_saved, منطقي notify_changes, منطقي update_remote = true,
		   منطقي autosave = true)
{
	إذا (!handle_in_range(param)) {
		PX4_ERR("set invalid param %d", param);
		إرجاع PX4_ERROR;
	}

	إذا (val == nullptr) {
		PX4_ERR("set invalid value");
		إرجاع PX4_ERROR;
	}

	إذا (param_is_readonly(param)) {
		PX4_WARN("param %s is read-only", param_name(param));
		إرجاع PX4_ERROR;
	}

	صحيح result = -1;
	منطقي param_changed = خطأ;
	perf_begin(param_set_perf);

	ثابت param_قيمة_u user_config_قيمة = user_config.get(param);
	param_قيمة_u new_قيمة{};

	بدل (param_type(param)) {
	حالة PARAM_TYPE_INT32:
		memcpy(&new_قيمة.i, val, حجم(new_قيمة.i));
		param_changed = user_config_قيمة.i != new_قيمة.i;
		اكسر;

	حالة PARAM_TYPE_FLOAT:
		memcpy(&new_قيمة.f, val, حجم(new_قيمة.f));
		param_changed = fabsf(user_config_قيمة.f - new_قيمة.f) > FLT_EPSILON;
		اكسر;

	افتراضي: {
			PX4_ERR("param_set invalid param type for %s", param_name(param));
			اكسر;
		}
	}

	إذا (user_config.store(param, new_قيمة)) {
		إذا (param_changed) {
			params_unsaved.set(param, !mark_saved);
		}

		result = PX4_OK;

	} وإلا {
		PX4_ERR("param_set failed to store param %s", param_name(param));
		result = PX4_ERROR;
	}

	إذا ((result == PX4_OK) && param_changed && !mark_saved && autosave) { // this is false when importing parameters
		param_autosave();
	}

	// If this is the parameter server, make sure that the remote is updated
#إذا defined(CONFIG_PARAM_PRIMARY)

	إذا (param_changed && update_remote) {
		param_primary_set_قيمة(param, val);
	}

#endif

	// If this is the parameter remote, make sure that the primary is updated
#إذا defined(CONFIG_PARAM_REMOTE)

	إذا (param_changed && update_remote) {
		param_remote_set_قيمة(param, val);
	}

#endif

	perf_end(param_set_perf);

	/*
	 * If we set something, now that we have unlocked, go ahead and advertise that
	 * a thing has been set.
	 */
	إذا ((result == PX4_OK) && param_changed && notify_changes) {
		param_notify_changes();
	}

	إرجاع result;
}

#إذا defined(FLASH_BASED_PARAMS)
صحيح param_set_external(param_t param, ثابت فراغ *val, منطقي mark_saved, منطقي notify_changes)
{
	إرجاع param_set_internal(param, val, mark_saved, notify_changes);
}

فراغ param_get_external(param_t param, فراغ *val)
{
	param_get(param, val);
}
#endif

صحيح param_set(param_t param, ثابت فراغ *val)
{
	إرجاع param_set_internal(param, val, خطأ, true);
}

صحيح param_set_no_notification(param_t param, ثابت فراغ *val)
{
	إرجاع param_set_internal(param, val, خطأ, خطأ);
}

صحيح param_set_no_remote_update(param_t param, ثابت فراغ *val, منطقي notify)
{
	إرجاع param_set_internal(param, val, خطأ, notify, خطأ);
}

منطقي param_used(param_t param)
{
	إذا (handle_in_range(param)) {
		إرجاع params_active[param];
	}

	إرجاع خطأ;
}

فراغ param_set_used(param_t param)
{
	إذا (handle_in_range(param)) {
#إذا defined(CONFIG_PARAM_REMOTE)

		إذا (!param_used(param)) {
			param_remote_set_used(param);
		}

#endif

		params_active.set(param, true);
	}
}

صحيح param_set_default_قيمة(param_t param, ثابت فراغ *val)
{
	إذا (!handle_in_range(param)) {
		PX4_ERR("set default value invalid param %d", param);
		إرجاع PX4_ERROR;
	}

	إذا (val == nullptr) {
		PX4_ERR("set default value invalid value");
		إرجاع PX4_ERROR;
	}

	إذا (param_is_readonly(param)) {
		PX4_WARN("param %s is read-only", param_name(param));
		إرجاع PX4_ERROR;
	}

	صحيح result = PX4_ERROR;


	// check if param being set to default value
	منطقي setting_to_static_default = خطأ;
	ثابت param_قيمة_u firmware_default_قيمة = firmware_defaults.get(param);

	بدل (param_type(param)) {
	حالة PARAM_TYPE_INT32: {
			int32_t new_قيمة;
			memcpy(&new_قيمة, val, حجم(new_قيمة));
			setting_to_static_default = firmware_default_قيمة.i == new_قيمة;
			اكسر;
		}

	حالة PARAM_TYPE_FLOAT: {
			عائم new_قيمة;
			memcpy(&new_قيمة, val, حجم(new_قيمة));
			setting_to_static_default = fabsf(firmware_default_قيمة.f - new_قيمة) <= FLT_EPSILON;
			اكسر;
		}
	}

	إذا (setting_to_static_default) {
		runtime_defaults.reset(param);

		result = PX4_OK;

	} وإلا {
		param_قيمة_u new_قيمة{};

		بدل (param_type(param)) {
		حالة PARAM_TYPE_INT32: {
				memcpy(&new_قيمة.i, val, حجم(new_قيمة.i));
				اكسر;
			}

		حالة PARAM_TYPE_FLOAT: {
				memcpy(&new_قيمة.f, val, حجم(new_قيمة.f));
				اكسر;
			}

		افتراضي:
			اكسر;
		}

		إذا (runtime_defaults.store(param, new_قيمة)) {
			user_config.refresh(param);
			result = PX4_OK;

		} وإلا {
			result = PX4_ERROR;
		}
	}


	إذا ((result == PX4_OK) && param_used(param)) {
		// send notification if param is already in use
		param_notify_changes();
	}

	إرجاع result;
}

ساكن صحيح param_reset_internal(param_t param, منطقي notify = true, منطقي autosave = true)
{
#إذا defined(CONFIG_PARAM_REMOTE)
	// Remote doesn't support reset
	إرجاع خطأ;
#endif

	إذا (param_is_readonly(param)) {
		إرجاع 0;  // silently skip — param_reset_all loops over all params
	}

	منطقي param_found = user_config.contains(param);

	إذا (handle_in_range(param)) {
		user_config.reset(param);
	}

	إذا (param_found) {
		/* Persist the reset. Flash-param saves append only unsaved params,
		 * so a reset has to show up as a tombstone or replay keeps the
		 * previous override. */
		params_unsaved.set(param, true);
	}

	إذا (autosave) {
		param_autosave();
	}

	إذا (param_found && notify) {
		param_notify_changes();
	}

#إذا defined(CONFIG_PARAM_PRIMARY)
	param_primary_reset(param);
#endif

	إرجاع param_found;
}

صحيح param_reset(param_t param) { إرجاع param_reset_internal(param, true); }
صحيح param_reset_no_notification(param_t param) { إرجاع param_reset_internal(param, خطأ); }

ساكن فراغ
param_reset_all_internal(منطقي auto_save)
{
#إذا defined(CONFIG_PARAM_REMOTE)
	// Remote doesn't support reset
	إرجاع;
#endif

	لكل (param_t param = 0; handle_in_range(param); param++) {
		param_reset_internal(param, خطأ, خطأ);
	}

	إذا (auto_save) {
		param_autosave();
	}

#إذا defined(CONFIG_PARAM_PRIMARY)
	param_primary_reset_all();
#endif

	param_notify_changes();
}

فراغ
param_reset_all()
{
	param_reset_all_internal(true);
}

فراغ
param_reset_excludes(ثابت حرف *excludes[], صحيح رقم_excludes)
{
	لكل (param_t param = 0; handle_in_range(param); param++) {
		ثابت حرف *name = param_name(param);
		منطقي exclude = خطأ;

		لكل (صحيح index = 0; index < رقم_excludes; index ++) {
			صحيح len = strlen(excludes[index]);

			إذا ((excludes[index][len - 1] == '*'
			     && strncmp(name, excludes[index], len - 1) == 0)
			    || strcmp(name, excludes[index]) == 0) {

				exclude = true;
				اكسر;
			}
		}

		إذا (!exclude) {
			param_reset(param);
		}
	}
}

فراغ
param_reset_specific(ثابت حرف *resets[], صحيح رقم_resets)
{
	لكل (param_t param = 0; handle_in_range(param); param++) {
		ثابت حرف *name = param_name(param);
		منطقي reset = خطأ;

		لكل (صحيح index = 0; index < رقم_resets; index++) {
			صحيح len = strlen(resets[index]);

			إذا ((resets[index][len - 1] == '*'
			     && strncmp(name, resets[index], len - 1) == 0)
			    || strcmp(name, resets[index]) == 0) {

				reset = true;
				اكسر;
			}
		}

		إذا (reset) {
			param_reset(param);
		}
	}
}

صحيح
param_set_default_file(ثابت حرف *filename)
{
	إذا (filename && param_backup_file && strcmp(filename, param_backup_file) == 0) {
		PX4_ERR("default file can't be the same as the backup file %s", filename);
		إرجاع PX4_ERROR;
	}

#ifdef FLASH_BASED_PARAMS
	// the default for flash-based params is always the FLASH
	(فراغ)filename;
#وإلا

	إذا (param_default_file != nullptr) {
		// we assume this is not in use by some other thread
		حرر(param_default_file);
		param_default_file = nullptr;
	}

	إذا (filename) {
		param_default_file = strdup(filename);
	}

#endif /* FLASH_BASED_PARAMS */

	إرجاع 0;
}

ثابت حرف *param_get_default_file()
{
	إرجاع param_default_file;
}

صحيح param_set_backup_file(ثابت حرف *filename)
{
	إذا (filename && param_default_file && strcmp(filename, param_default_file) == 0) {
		PX4_ERR("backup file can't be the same as the default file %s", filename);
		إرجاع PX4_ERROR;
	}

	إذا (param_backup_file != nullptr) {
		// we assume this is not in use by some other thread
		حرر(param_backup_file);
		param_backup_file = nullptr;
	}

	إذا (filename) {
		param_backup_file = strdup(filename);

	} وإلا {
		param_backup_file = nullptr; // backup disabled
	}

	إرجاع 0;
}

ثابت حرف *param_get_backup_file()
{
	إرجاع param_backup_file;
}

ساكن صحيح param_export_internal(صحيح fd, param_filter_func filter);
ساكن صحيح param_verify(صحيح fd);

صحيح param_save_default(منطقي blocking)
{
	PX4_DEBUG("param_save_default");

	// take the file lock
	إذا (blocking) {
		pthread_mutex_lock(&file_mutex);

	} وإلا {
		إذا (pthread_mutex_trylock(&file_mutex) != 0) {
			PX4_DEBUG("param_save_default: file lock failed (already locked)");
			إرجاع -EWOULDBLOCK;
		}
	}

	صحيح shutdown_lock_ret = px4_shutdown_lock();

	إذا (shutdown_lock_ret != 0) {
		PX4_ERR("px4_shutdown_lock() failed (%i)", shutdown_lock_ret);
	}

	صحيح res = PX4_ERROR;
	ثابت حرف *filename = param_get_default_file();

	لكل (param_t param = 0; handle_in_range(param); param++) {
		إذا (params_unsaved[param]) {
			params_saving.set(param, true);
			params_unsaved.set(param, خطأ);
		}
	}

	إذا (filename) {
		ساكن constexpr صحيح MAX_ATTEMPTS = 3;

		لكل (صحيح attempt = 1; attempt <= MAX_ATTEMPTS; attempt++) {
			// write parameters to file
			صحيح fd = ::open(filename, O_WRONLY | O_CREAT | O_TRUNC, PX4_O_MODE_666);

			إذا (fd > -1) {
				perf_begin(param_export_perf);
				res = param_export_internal(fd, nullptr);
				perf_end(param_export_perf);
				::close(fd);

				إذا (res == PX4_OK) {
					// reopen file to verify
					صحيح fd_verify = ::open(filename, O_RDONLY, PX4_O_MODE_666);
					res = param_verify(fd_verify) || lseek(fd_verify, 0, SEEK_SET) || param_verify(fd_verify);
					::close(fd_verify);
				}
			}

			إذا (res == PX4_OK) {
				اكسر;

			} وإلا {
				PX4_ERR("parameter export to %s failed (%d) attempt %d", filename, res, attempt);
				px4_usleep(10000); // wait at least 10 milliseconds before trying again
			}
		}

	} وإلا {
		perf_begin(param_export_perf);
		res = flash_param_save(nullptr);
		perf_end(param_export_perf);
	}

	إذا (res != PX4_OK) {
		PX4_ERR("param export failed (%d)", res);

		لكل (param_t param = 0; handle_in_range(param); param++) {
			إذا (params_saving[param]) {
				params_unsaved.set(param, true);
			}
		}
	}

	params_saving.reset();

	إذا (res == PX4_OK) {
		// backup file
		إذا (param_backup_file) {
			صحيح fd_backup_file = ::open(param_backup_file, O_WRONLY | O_CREAT | O_TRUNC, PX4_O_MODE_666);

			إذا (fd_backup_file > -1) {
				صحيح backup_export_ret = param_export_internal(fd_backup_file, nullptr);
				::close(fd_backup_file);

				إذا (backup_export_ret != 0) {
					PX4_ERR("backup parameter export to %s failed (%d)", param_backup_file, backup_export_ret);

				} وإلا {
					// verify export
					صحيح fd_verify = ::open(param_backup_file, O_RDONLY, PX4_O_MODE_666);
					param_verify(fd_verify);
					::close(fd_verify);
				}
			}
		}
	}

	pthread_mutex_unlock(&file_mutex);

	إذا (shutdown_lock_ret == 0) {
		px4_shutdown_unlock();
	}

	إرجاع res;
}

/**
 * @return 0 on success, 1 if all params have not yet been stored, -1 if device open failed, -2 if writing parameters failed
 */
صحيح
param_load_default()
{
	صحيح res = 0;
	ثابت حرف *filename = param_get_default_file();

	إذا (!filename) {
		إرجاع param_load(-1);
	}

	صحيح fd_load = ::open(filename, O_RDONLY);

	إذا (fd_load < 0) {
		/* no parameter file is OK, otherwise this is an error */
		إذا (errno != ENOENT) {
			PX4_ERR("open '%s' for reading failed", filename);
			إرجاع -1;
		}

		إرجاع 1;
	}

	صحيح result = param_load(fd_load);
	::close(fd_load);

	إذا (result == 1) {
		/* blank file: nothing stored yet */
		إرجاع 1;
	}

	إذا (result != 0) {
		PX4_ERR("error reading parameters from '%s'", filename);
		إرجاع -2;
	}

	إرجاع res;
}

ساكن صحيح param_verify_callback(bson_decoder_t decoder, bson_عقدة_t عقدة_2)
{
	إذا (عقدة_2->type == BSON_EOO) {
		إرجاع 0;
	}

	// find the parameter this node represents
	param_t param = param_find_no_notification(عقدة_2->name);

	إذا (param == PARAM_INVALID) {
		PX4_ERR("verify: invalid parameter '%s'", عقدة_2->name);
		إرجاع -1;
	}

	// handle verifying the parameter from the node
	بدل (عقدة_2->type) {
	حالة BSON_INT32: {
			إذا (param_type(param) != PARAM_TYPE_INT32) {
				PX4_ERR("verify: invalid param type %d for '%s' (BSON_INT32)", param_type(param), عقدة_2->name);
				إرجاع -1;
			}

			int32_t value;

			إذا (param_get(param, &value) == 0) {
				إذا (value == عقدة_2->i32) {
					إرجاع 1; // valid

				} وإلا {
					PX4_ERR("verify: '%s' invalid BSON value %" PRIi32 "(expected %" PRIi32 ")", عقدة_2->name, عقدة_2->i32, value);
				}
			}
		}
		اكسر;

	حالة BSON_DOUBLE: {
			إذا (param_type(param) != PARAM_TYPE_FLOAT) {
				PX4_ERR("verify: invalid param type %d for '%s' (BSON_DOUBLE)", param_type(param), عقدة_2->name);
				إرجاع -1;
			}

			عائم value;

			إذا (param_get(param, &value) == 0) {
				إذا (fabsf(value - (عائم)عقدة_2->d) <= FLT_EPSILON) {
					إرجاع 1; // valid

				} وإلا {
					PX4_ERR("verify: '%s' invalid BSON value %.3f (expected %.3f)", عقدة_2->name, عقدة_2->d, (مزدوج)value);
				}
			}
		}
		اكسر;

	افتراضي:
		PX4_ERR("verify: '%s' invalid node type %d", عقدة_2->name, عقدة_2->type);
	}

	إرجاع -1;
}

ساكن صحيح param_verify(صحيح fd)
{
	PX4_DEBUG("param_verify");

	إذا (fd < 0) {
		إرجاع -1;
	}

	إذا (lseek(fd, 0, SEEK_SET) != 0) {
		PX4_ERR("verify: seek failed");
		إرجاع -1;
	}

	bson_decoder_s decoder{};

	إذا (bson_decoder_init_file(&decoder, fd, param_verify_callback) == 0) {
		صحيح result = -1;

		افعل {
			result = bson_decoder_next(&decoder);

		} بينما (result > 0);

		إذا (result == 0) {
			إذا (decoder.total_document_size != decoder.total_decoded_size) {
				PX4_ERR("BSON document size (%" PRId32 ") doesn't match bytes decoded (%" PRId32 ")", decoder.total_document_size,
					decoder.total_decoded_size);

			} وإلا {
				إرجاع 0;
			}

		} وإلا إذا (result == -ENODATA) {
			PX4_ERR("verify: no BSON data");

		} وإلا {
			PX4_ERR("verify: failed (%d)", result);
		}
	}

	إرجاع -1;
}

صحيح
param_export(ثابت حرف *filename, param_filter_func filter)
{
	PX4_DEBUG("param_export");

	صحيح shutdown_lock_ret = px4_shutdown_lock();

	إذا (shutdown_lock_ret != 0) {
		PX4_ERR("px4_shutdown_lock() failed (%i)", shutdown_lock_ret);
	}

	// take the file lock
	إذا (pthread_mutex_trylock(&file_mutex) != 0) {
		PX4_ERR("param_export: file lock failed (already locked)");
		إرجاع PX4_ERROR;
	}

	صحيح fd = ::open(filename, O_RDWR | O_CREAT, PX4_O_MODE_666);
	صحيح result = PX4_ERROR;

	perf_begin(param_export_perf);

	إذا (fd > -1) {
		result = param_export_internal(fd, filter);

	} وإلا {
		result = flash_param_save(filter);
	}

	perf_end(param_export_perf);

	pthread_mutex_unlock(&file_mutex);

	إذا (shutdown_lock_ret == 0) {
		px4_shutdown_unlock();
	}

	إرجاع result;
}

// internal parameter export, caller is responsible for locking
ساكن صحيح param_export_internal(صحيح fd, param_filter_func filter)
{
	PX4_DEBUG("param_export_internal");
	ثابت auto changed_params = user_config.containedAsBitset();

	صحيح result = -1;
	bson_encoder_s encoder{};
	uint8_t bson_buffer[256];

	إذا (lseek(fd, 0, SEEK_SET) != 0) {
		PX4_ERR("export seek failed %d", errno);
		إرجاع -1;
	}

	إذا (bson_encoder_init_buf_file(&encoder, fd, &bson_buffer, حجم(bson_buffer)) != 0) {
		اذهب out;
	}

	لكل (param_t param = 0; handle_in_range(param); param++) {
		إذا (!changed_params[param] || (filter && !filter(param))) {
			استمر;
		}

		ثابت param_قيمة_u runtime_default_قيمة = runtime_defaults.get(param);
		ثابت param_قيمة_u user_config_قيمة = user_config.get(param);

		// don't export default values
		بدل (param_type(param)) {
		حالة PARAM_TYPE_INT32:
			إذا (user_config_قيمة.i == runtime_default_قيمة.i) {
				PX4_DEBUG("skipping %s %" PRIi32 " export", param_name(param), runtime_default_قيمة.i);
				استمر;
			}

			اكسر;

		حالة PARAM_TYPE_FLOAT:
			إذا (fabsf(user_config_قيمة.f - runtime_default_قيمة.f) <= FLT_EPSILON) {
				PX4_DEBUG("skipping %s %.3f export", param_name(param), (مزدوج)runtime_default_قيمة.f);
				استمر;
			}

			اكسر;
		}

		ثابت حرف *name = param_name(param);
		ثابت size_t size = param_size(param);

		/* append the appropriate BSON type object */
		بدل (param_type(param)) {
		حالة PARAM_TYPE_INT32: {
				ثابت int32_t i = user_config_قيمة.i;
				PX4_DEBUG("exporting: %s (%d) size: %lu val: %" PRIi32, name, param, (long unsigned صحيح)size, i);

				إذا (bson_encoder_append_int32(&encoder, name, i) != 0) {
					PX4_ERR("BSON append failed for '%s'", name);
					اذهب out;
				}
			}
			اكسر;

		حالة PARAM_TYPE_FLOAT: {
				ثابت مزدوج f = (مزدوج)user_config_قيمة.f;
				PX4_DEBUG("exporting: %s (%d) size: %lu val: %.3f", name, param, (long unsigned صحيح)size, (مزدوج)f);

				إذا (bson_encoder_append_double(&encoder, name, f) != 0) {
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

	إذا (result == 0) {
		إذا (bson_encoder_fini(&encoder) != PX4_OK) {
			PX4_ERR("BSON encoder finalize failed");
			result = -1;
		}
	}

	إرجاع result;
}

#إذا defined(FLASH_BASED_PARAMS)
// A file is never the flash backend's store, so what an import changes is unsaved until a delta carries it.
// As on the file backend, the import itself does not save.
ساكن constexpr منطقي file_import_is_saved = خطأ;
#وإلا
ساكن constexpr منطقي file_import_is_saved = true;
#endif

ساكن صحيح
param_import_callback(bson_decoder_t decoder, bson_عقدة_t عقدة_2)
{
	/*
	 * EOO means the end of the parameter object. (Currently not supporting
	 * nested BSON objects).
	 */
	إذا (عقدة_2->type == BSON_EOO) {
		PX4_DEBUG("end of parameters");
		إرجاع 0;
	}

	// A tombstone has no value. The decoder leaves the previous node's in the union and the translations
	// read it without a type check.
	ثابت منطقي tombstone = عقدة_2->type == BSON_nullptr || عقدة_2->type == BSON_UNDEFINED;

	// if we do param_set() directly in the translation, set PARAM_SKIP_IMPORT as return value and return here
	إذا (!tombstone && param_modify_on_import(عقدة_2) == param_modify_on_import_ret::PARAM_SKIP_IMPORT) {
		إرجاع 1;
	}

	// Find the parameter this node represents.  If we don't know it, ignore the node.
	param_t param = param_find_no_notification(عقدة_2->name);

	إذا (param == PARAM_INVALID) {
		PX4_WARN("ignoring unrecognised parameter '%s'", عقدة_2->name);
		إرجاع 1;
	}

	// Handle setting the parameter from the node
	بدل (عقدة_2->type) {
	حالة BSON_nullptr:
	حالة BSON_UNDEFINED:
		param_reset_internal(param, true, خطأ);
		إرجاع 1;

	حالة BSON_INT32: {
			إذا (param_type(param) == PARAM_TYPE_INT32) {
				int32_t i = عقدة_2->i32;
				param_set_internal(param, &i, file_import_is_saved, true, true, خطأ);
				PX4_DEBUG("Imported %s with value %" PRIi32, param_name(param), i);

			} وإلا {
				PX4_WARN("unexpected type for %s", عقدة_2->name);
			}
		}
		اكسر;

	حالة BSON_DOUBLE: {
			إذا (param_type(param) == PARAM_TYPE_FLOAT) {
				عائم f = عقدة_2->d;
				param_set_internal(param, &f, file_import_is_saved, true, true, خطأ);
				PX4_DEBUG("Imported %s with value %f", param_name(param), (مزدوج)f);

			} وإلا {
				PX4_WARN("unexpected type for %s", عقدة_2->name);
			}
		}
		اكسر;

	افتراضي:
		PX4_ERR("import: unrecognised node type for '%s'", عقدة_2->name);
	}

	// don't return zero, that means EOF
	إرجاع 1;
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

	إذا (fstat(fd, &st) == 0 && S_ISREG(st.st_mode) && st.st_size == 0) {
		إرجاع 1;
	}

	int32_t doc_size = 0;
	ssize_t len = اقرأ(fd, &doc_size, حجم(doc_size));

	إذا (len == 0 || (len == (ssize_t)حجم(doc_size) && (doc_size == 0 || doc_size == -1))) {
		إرجاع 1;
	}

	إذا (lseek(fd, 0, SEEK_SET) != 0) {
		PX4_ERR("import lseek failed (%d)", errno);
		إرجاع -1;
	}

	ساكن constexpr صحيح MAX_ATTEMPTS = 3;

	لكل (صحيح attempt = 1; attempt <= MAX_ATTEMPTS; attempt++) {
		bson_decoder_s decoder{};

		إذا (bson_decoder_init_file(&decoder, fd, param_import_callback) == 0) {
			صحيح result = -1;

			افعل {
				result = bson_decoder_next(&decoder);

			} بينما (result > 0);

			إذا (result == 0) {
				إذا (decoder.total_document_size == decoder.total_decoded_size) {
					PX4_INFO("BSON document size %" PRId32 " bytes, decoded %" PRId32 " bytes (INT32:%" PRIu16 ", FLOAT:%" PRIu16 ")",
						 decoder.total_document_size, decoder.total_decoded_size,
						 decoder.تعداد_عقدة_int32, decoder.تعداد_عقدة_double);

					إرجاع 0;

				} وإلا {
					PX4_ERR("BSON document size (%" PRId32 ") doesn't match bytes decoded (%" PRId32 ")",
						decoder.total_document_size, decoder.total_decoded_size);
				}

			} وإلا إذا (result == -ENODATA) {
				// silently retry as a precaution unless this is our last attempt
				إذا (attempt == MAX_ATTEMPTS) {
					PX4_DEBUG("BSON: no data");
					إرجاع 0;
				}

			} وإلا {
				PX4_ERR("param import failed (%d) attempt %d", result, attempt);
			}

		} وإلا {
			PX4_ERR("param import bson decoder init failed (attempt %d)", attempt);
		}

		إذا (attempt != MAX_ATTEMPTS) {
			إذا (lseek(fd, 0, SEEK_SET) != 0) {
				PX4_ERR("import lseek failed (%d)", errno);
			}

			px4_usleep(10000); // wait at least 10 milliseconds before trying again
		}
	}

	إرجاع -1;
}

#إذا defined(FLASH_BASED_PARAMS)
ساكن صحيح flash_backend_locked(صحيح (*op)())
{
	/* Same order as param_save_default: mutex then shutdown lock. Autosave
	 * uses a trylock, so a compact holds it off the flash programming. */
	pthread_mutex_lock(&file_mutex);

	صحيح shutdown_lock_ret = px4_shutdown_lock();

	إذا (shutdown_lock_ret != 0) {
		PX4_ERR("px4_shutdown_lock() failed (%i)", shutdown_lock_ret);
	}

	صحيح result = op();

	pthread_mutex_unlock(&file_mutex);

	إذا (shutdown_lock_ret == 0) {
		px4_shutdown_unlock();
	}

	إرجاع result;
}

ساكن صحيح flash_load_locked()
{
	/* Reset under the lock so a pending autosave cannot append tombstones
	 * of the just-cleared RAM before import/compact run. */
	param_reset_all_internal(خطأ);
	صحيح result = flash_param_import();

	إذا (result == 0 || result == 1) {
		params_unsaved.reset();
	}

	إرجاع result;
}
#endif

صحيح
param_import(صحيح fd)
{
	إذا (fd < 0) {
#إذا defined(FLASH_BASED_PARAMS)
		إرجاع flash_backend_locked(flash_param_import);
#وإلا
		إرجاع flash_param_import();
#endif
	}

	إرجاع param_import_internal(fd);
}

صحيح
param_load(صحيح fd)
{
	إذا (fd < 0) {
#إذا defined(FLASH_BASED_PARAMS)
		إرجاع flash_backend_locked(flash_load_locked);
#وإلا
		إرجاع PX4_ERROR;
#endif
	}

	param_reset_all_internal(خطأ);
	صحيح result = param_import_internal(fd);

#إذا !defined(FLASH_BASED_PARAMS)

	إذا (result >= 0) {
		params_unsaved.reset();
	}

#endif

	إرجاع result;
}

فراغ
param_foreach(فراغ (*func)(فراغ *arg, param_t param), فراغ *arg, منطقي only_changed, منطقي only_used)
{
	param_t	param;

	لكل (param = 0; handle_in_range(param); param++) {

		/* if requested, skip unchanged values */
		إذا (only_changed && (!user_config.contains(param))) {
			استمر;
		}

		إذا (only_used && !param_used(param)) {
			استمر;
		}

		func(arg, param);
	}
}

uint32_t param_hash_check()
{
	uint32_t param_hash = 0;

	/* compute the CRC32 over all string param names and 4 byte values */
	لكل (param_t param = 0; handle_in_range(param); param++) {
		إذا (!param_used(param) || param_is_volatile(param)) {
			استمر;
		}

		ثابت حرف *name = param_name(param);
		auto value = user_config.get(param).i;
		ثابت فراغ *val = (فراغ *)&value;
		param_hash = crc32part((ثابت uint8_t *)name, strlen(name), param_hash);
		param_hash = crc32part((ثابت uint8_t *)val, param_size(param), param_hash);
	}

	إرجاع param_hash;
}

فراغ param_print_status()
{
	PX4_INFO("summary: %d/%d (used/total)", param_تعداد_used(), param_تعداد());

#ifndef FLASH_BASED_PARAMS
	ثابت حرف *filename = param_get_default_file();

	إذا (filename != nullptr) {
		PX4_INFO("file: %s", param_get_default_file());
	}

	إذا (param_backup_file) {
		PX4_INFO("backup file: %s", param_backup_file);
	}

#endif /* FLASH_BASED_PARAMS */

	PX4_INFO("storage array: %d/%d elements (%zu bytes total)",
		 user_config.size(), firmware_defaults.size(), (size_t)user_config.byteSize());


	PX4_INFO("storage array (custom defaults): %d/%d elements (%zu bytes total)",
		 runtime_defaults.size(), firmware_defaults.size(), (size_t)runtime_defaults.byteSize());

	إذا (autosave_instance) {
		PX4_INFO("auto save: %s", autosave_instance->enabled() ? "on" : "off");

		hrt_abstime last_autosave = autosave_instance->lastAutosave();

		إذا (last_autosave > 0) {
			PX4_INFO("last auto save: %.3f seconds ago", hrt_elapsed_time(&last_autosave) * 1e-6);
		}
	}

	perf_print_counter(param_export_perf);
	perf_print_counter(param_find_perf);
	perf_print_counter(param_get_perf);
	perf_print_counter(param_set_perf);

#إذا defined(CONFIG_PARAM_PRIMARY)
	بنية param_primary_counters counts;
	param_primary_get_counters(&counts);
	PX4_INFO("set value requests received: %" PRIu32 ", set value responses sent: %" PRIu32,
		 counts.set_قيمة_request_received, counts.set_قيمة_response_sent);
	PX4_INFO("set value requests sent: %" PRIu32 ", set value responses received: %" PRIu32,
		 counts.set_قيمة_request_sent, counts.set_قيمة_response_received);
	PX4_INFO("resets sent: %" PRIu32 ", set used requests received: %" PRIu32,
		 counts.reset_sent, counts.set_used_received);
#endif

#إذا defined(CONFIG_PARAM_REMOTE)
	بنية param_remote_counters counts;
	param_remote_get_counters(&counts);
	PX4_INFO("set value requests received: %" PRIu32 ", set value responses sent: %" PRIu32,
		 counts.set_قيمة_request_received, counts.set_قيمة_response_sent);
	PX4_INFO("set value requests sent: %" PRIu32 ", set value responses received: %" PRIu32,
		 counts.set_قيمة_request_sent, counts.set_قيمة_response_received);
	PX4_INFO("resets received: %" PRIu32 ", set used requests sent: %" PRIu32,
		 counts.reset_received, counts.set_used_sent);
#endif

}
