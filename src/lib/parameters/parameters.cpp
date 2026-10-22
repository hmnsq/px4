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

#تعریف_کرو PARAM_IMPLEMENTATION
#شامل "param.h"
#شامل "param_translation.h"
#شامل <parameters/px4_parameters.hpp>
#شامل <lib/tinybson/tinybson.h>

#اگر defined(__PX4_NUTTX)
#شامل <nuttx/crc32.h>
#ورنہ
#شامل <crc32.h>
#اگر_ختم
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

استعمال نامخلا وقت_literals;

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
#اگر_ختم

#اگر defined(FLASH_BASED_PARAMS)
#شامل "flashparams/flashparams.h"
#ورنہ
اندرونی ساکن صحیح flash_param_save(param_filter_func filter) { واپس -1; }
اندرونی ساکن صحیح flash_param_import() { واپس -1; }
#اگر_ختم

ساکن حرف *param_default_file = خالی_اشاریہ;
ساکن حرف *param_backup_file = خالی_اشاریہ;

#شامل "autosave.h"
ساکن ParamAutosave *autosave_instance {خالی_اشاریہ};

ساکن px4::AtomicBitset<param_info_گنتی> params_فعال;  // params found
ساکن px4::AtomicBitset<param_info_گنتی> params_unsaved;

// Params the save in progress is writing. They are moved out of params_unsaved
// before their values are read, so a param_set that lands while the save runs
// marks its param again and reaches the next save. Flash-param saves write only
// unsaved params, so nothing else would ever write that change.
ساکن px4::AtomicBitset<param_info_گنتی> params_saving;

ساکن ConstLayer firmware_defaults;
ساکن DynamicSparseLayer runtime_defaults{&firmware_defaults};
DynamicSparseLayer user_config{&runtime_defaults};

/** parameter update topic handle */
#اگر not defined(CONFIG_PARAM_REMOTE)
ساکن orb_advert_t param_topic = خالی_اشاریہ;
ساکن غیرعلامتی صحیح param_instance = 0;
#اگر_ختم

ساکن perf_شمار_t param_export_perf;
ساکن perf_شمار_t param_ڈھونڈو_perf;
ساکن perf_شمار_t param_get_perf;
ساکن perf_شمار_t param_سیٹ_perf;

ساکن pthread_mutex_t file_mutex  =
	PTHREAD_MUTEX_INITIALIZER; ///< this protects against concurrent param saves (file or flash access).

// Support for remote parameter node
#اگر defined(CONFIG_PARAM_PRIMARY)
# include "parameters_primary.h"
#اگر_ختم // CONFIG_PARAM_PRIMARY
#اگر defined(CONFIG_PARAM_REMOTE)
# include "parameters_remote.h"
#اگر_ختم // CONFIG_PARAM_REMOTE

خالی
param_init()
{
#اگر_تعریف_شدہ __PX4_POSIX
	// Set up the recursive param mutex before any thread can access parameters.
	AtomicTransaction::initialize();
#اگر_ختم

	param_export_perf = perf_alloc(PC_ELAPSED, "param: export");
	param_ڈھونڈو_perf = perf_alloc(PC_گنتی, "param: find");
	param_get_perf = perf_alloc(PC_گنتی, "param: get");
	param_سیٹ_perf = perf_alloc(PC_ELAPSED, "param: set");

#اگر defined(__PX4_NUTTX) && !defined(CONFIG_BUILD_FLAT)
	px4_رجسٹر_boardct_ioctl(_PARAMIOCBASE, param_ioctl);
#اگر_ختم

#اگر defined(CONFIG_PARAM_PRIMARY)
	param_primary_init();
#اگر_ختم // CONFIG_PARAM_PRIMARY

#اگر defined(CONFIG_PARAM_REMOTE)
	param_remote_init();
#اگر_ختم // CONFIG_PARAM_REMOTE

#اگر not defined(CONFIG_PARAM_REMOTE)
	autosave_instance = نیا ParamAutosave();
#اگر_ختم
}


خالی
param_notify_changes()
{
// Don't send if this is a remote node. Only the primary
// sends out update notices
#اگر not defined(CONFIG_PARAM_REMOTE)
	parameter_update_s pup {};
	pup.instance = param_instance++;
	pup.get_گنتی_2 = perf_event_گنتی(param_get_perf);
	pup.سیٹ_گنتی = perf_event_گنتی(param_سیٹ_perf);
	pup.ڈھونڈو_گنتی = perf_event_گنتی(param_ڈھونڈو_perf);
	pup.export_گنتی = perf_event_گنتی(param_export_perf);
	pup.فعال_2 = params_فعال.گنتی();
	pup.بدلا = user_config.سائز();
	pup.custom_default = runtime_defaults.سائز();
	pup.timestamp = hrt_absolute_وقت();

	اگر (param_topic == خالی_اشاریہ) {
		param_topic = orb_advertise(ORB_ID(parameter_update), &pup);

	} ورنہ {
		orb_publish(ORB_ID(parameter_update), param_topic, &pup);
	}

#اگر_ختم
}

ساکن param_t param_ڈھونڈو_اندرونی(ثابت حرف *name, منطقی notification)
{
	perf_گنتی(param_ڈھونڈو_perf);

	param_t درمیان;
	param_t front = 0;
	param_t آخری = param_info_گنتی;

	/* perform a binary search of the known parameters */

	جب (front <= آخری) {
		درمیان = front + (آخری - front) / 2;
		صحیح ret = strcmp(name, param_name(درمیان));

		اگر (ret == 0) {
			اگر (notification) {
				param_سیٹ_مستعمل(درمیان);
			}

			واپس درمیان;

		} ورنہ اگر (درمیان == front) {
			/* An end point has been hit, but there has been no match */
			توڑو;

		} ورنہ اگر (ret < 0) {
			آخری = درمیان;

		} ورنہ {
			front = درمیان;
		}
	}

	/* not found */
	واپس PARAM_INVALID;
}

param_t param_ڈھونڈو(ثابت حرف *name)
{
	واپس param_ڈھونڈو_اندرونی(name, سچ);
}

param_t param_ڈھونڈو_no_notification(ثابت حرف *name)
{
	واپس param_ڈھونڈو_اندرونی(name, جھوٹ);
}

غیرعلامتی param_گنتی_مستعمل()
{
	واپس params_فعال.گنتی();
}

param_t param_for_مستعمل_اشاریہ(غیرعلامتی اشاریہ)
{
	// walk all params and count used params
	اگر (اشاریہ < param_info_گنتی) {
		غیرعلامتی مستعمل_گنتی = 0;

		برائے (صحیح i = 0; i < params_فعال.سائز(); i++) {
			اگر (params_فعال[i]) {
				// we found the right used count,
				//  return the param value
				اگر (اشاریہ == مستعمل_گنتی) {
					واپس static_cast<param_t>(i);
				}

				مستعمل_گنتی++;
			}
		}
	}

	واپس PARAM_INVALID;
}

صحیح param_get_مستعمل_اشاریہ(param_t param)
{
	/* this tests for out of bounds and does a constant time lookup */
	اگر (!param_مستعمل(param)) {
		واپس -1;
	}

	/* walk all params and count, now knowing that it has a valid index */
	صحیح مستعمل_گنتی = 0;

	برائے (صحیح i = 0; i < params_فعال.سائز(); i++) {
		اگر (params_فعال[i]) {

			اگر (param == i) {
				واپس مستعمل_گنتی;
			}

			مستعمل_گنتی++;
		}
	}

	واپس -1;
}

منطقی
param_قدر_unsaved(param_t param)
{
	واپس handle_اندر_range(param) ? (params_unsaved[param] || params_saving[param]) : جھوٹ;
}

صحیح
param_get(param_t param, خالی *val)
{
	perf_گنتی(param_get_perf);

	اگر (!handle_اندر_range(param)) {
		PX4_ERR("get: param %" PRId16 " invalid", param);
		واپس PX4_ERROR;
	}

	اگر (!params_فعال[param]) {
		PX4_DEBUG("get: param %" PRId16 " (%s) not active", param, param_name(param));
	}

	صحیح نتیجہ = PX4_ERROR;

	اگر (val) {

		خودکار retrieve_قدر = user_config.get(param);

		انتخاب (param_type(param)) {
		حالت PARAM_TYPE_INT32:
			حافظہ_نقل(val, &retrieve_قدر.i, حجم(retrieve_قدر.i));
			واپس PX4_OK;

		حالت PARAM_TYPE_FLOAT:
			حافظہ_نقل(val, &retrieve_قدر.f, حجم(retrieve_قدر.f));
			واپس PX4_OK;
		}
	}

	واپس نتیجہ;
}

صحیح
param_get_default_قدر_اندرونی(param_t param, خالی *default_val)
{
	اگر (!handle_اندر_range(param)) {
		PX4_ERR("get default value: param %d invalid", param);
		واپس PX4_ERROR;
	}

	اگر (default_val) {
		انتخاب (param_type(param)) {
		حالت PARAM_TYPE_INT32: {
				int32_t val = runtime_defaults.get(param).i;
				حافظہ_نقل(default_val, &val, حجم(val));
				واپس PX4_OK;
			}

		حالت PARAM_TYPE_FLOAT: {
				اعشاری val = runtime_defaults.get(param).f;
				حافظہ_نقل(default_val, &val, حجم(val));
				واپس PX4_OK;
			}
		}
	}

	واپس PX4_ERROR;
}

صحیح
param_get_default_قدر(param_t param, خالی *default_val)
{
	اگر (!handle_اندر_range(param)) {
		واپس PX4_ERROR;
	}

	صحیح ret = param_get_default_قدر_اندرونی(param, default_val);
	واپس ret;
}

منطقی param_قدر_is_default(param_t param)
{
	اگر (!handle_اندر_range(param)) {
		واپس سچ;
	}

	اگر (!user_config.شامل_ہے(param)) {
		// if user config does not contain it, consider it default.
		واپس سچ;

	} ورنہ {
		// compare with default value
		ثابت param_قدر_u user_config_قدر = user_config.get(param);
		ثابت param_قدر_u runtime_default_قدر = runtime_defaults.get(param);

		انتخاب (param_type(param)) {
		حالت PARAM_TYPE_INT32: {
				واپس user_config_قدر.i == runtime_default_قدر.i;
			}

		حالت PARAM_TYPE_FLOAT: {
				واپس fabsf(user_config_قدر.f - runtime_default_قدر.f) < FLT_EPSILON;
			}
		}
	}

	واپس سچ;
}

ساکن خالی
param_autosave()
{
	اگر (autosave_instance) {
		autosave_instance->request();
	}
}

خالی
param_control_autosave(منطقی enable)
{
	اگر (autosave_instance) {
		autosave_instance->enable(enable);
	}
}

ساکن صحیح
param_سیٹ_اندرونی(param_t param, ثابت خالی *val, منطقی mark_saved, منطقی notify_changes, منطقی update_remote = سچ,
		   منطقی autosave = سچ)
{
	اگر (!handle_اندر_range(param)) {
		PX4_ERR("set invalid param %d", param);
		واپس PX4_ERROR;
	}

	اگر (val == خالی_اشاریہ) {
		PX4_ERR("set invalid value");
		واپس PX4_ERROR;
	}

	اگر (param_is_readonly(param)) {
		PX4_WARN("param %s is read-only", param_name(param));
		واپس PX4_ERROR;
	}

	صحیح نتیجہ = -1;
	منطقی param_بدلا = جھوٹ;
	perf_begin(param_سیٹ_perf);

	ثابت param_قدر_u user_config_قدر = user_config.get(param);
	param_قدر_u new_قدر{};

	انتخاب (param_type(param)) {
	حالت PARAM_TYPE_INT32:
		حافظہ_نقل(&new_قدر.i, val, حجم(new_قدر.i));
		param_بدلا = user_config_قدر.i != new_قدر.i;
		توڑو;

	حالت PARAM_TYPE_FLOAT:
		حافظہ_نقل(&new_قدر.f, val, حجم(new_قدر.f));
		param_بدلا = fabsf(user_config_قدر.f - new_قدر.f) > FLT_EPSILON;
		توڑو;

	طےشدہ: {
			PX4_ERR("param_set invalid param type for %s", param_name(param));
			توڑو;
		}
	}

	اگر (user_config.store(param, new_قدر)) {
		اگر (param_بدلا) {
			params_unsaved.سیٹ(param, !mark_saved);
		}

		نتیجہ = PX4_OK;

	} ورنہ {
		PX4_ERR("param_set failed to store param %s", param_name(param));
		نتیجہ = PX4_ERROR;
	}

	اگر ((نتیجہ == PX4_OK) && param_بدلا && !mark_saved && autosave) { // this is false when importing parameters
		param_autosave();
	}

	// If this is the parameter server, make sure that the remote is updated
#اگر defined(CONFIG_PARAM_PRIMARY)

	اگر (param_بدلا && update_remote) {
		param_primary_سیٹ_قدر(param, val);
	}

#اگر_ختم

	// If this is the parameter remote, make sure that the primary is updated
#اگر defined(CONFIG_PARAM_REMOTE)

	اگر (param_بدلا && update_remote) {
		param_remote_سیٹ_قدر(param, val);
	}

#اگر_ختم

	perf_انتہا(param_سیٹ_perf);

	/*
	 * If we set something, now that we have unlocked, go ahead and advertise that
	 * a thing has been set.
	 */
	اگر ((نتیجہ == PX4_OK) && param_بدلا && notify_changes) {
		param_notify_changes();
	}

	واپس نتیجہ;
}

#اگر defined(FLASH_BASED_PARAMS)
صحیح param_سیٹ_external(param_t param, ثابت خالی *val, منطقی mark_saved, منطقی notify_changes)
{
	واپس param_سیٹ_اندرونی(param, val, mark_saved, notify_changes);
}

خالی param_get_external(param_t param, خالی *val)
{
	param_get(param, val);
}
#اگر_ختم

صحیح param_سیٹ(param_t param, ثابت خالی *val)
{
	واپس param_سیٹ_اندرونی(param, val, جھوٹ, سچ);
}

صحیح param_سیٹ_no_notification(param_t param, ثابت خالی *val)
{
	واپس param_سیٹ_اندرونی(param, val, جھوٹ, جھوٹ);
}

صحیح param_سیٹ_no_remote_update(param_t param, ثابت خالی *val, منطقی notify)
{
	واپس param_سیٹ_اندرونی(param, val, جھوٹ, notify, جھوٹ);
}

منطقی param_مستعمل(param_t param)
{
	اگر (handle_اندر_range(param)) {
		واپس params_فعال[param];
	}

	واپس جھوٹ;
}

خالی param_سیٹ_مستعمل(param_t param)
{
	اگر (handle_اندر_range(param)) {
#اگر defined(CONFIG_PARAM_REMOTE)

		اگر (!param_مستعمل(param)) {
			param_remote_سیٹ_مستعمل(param);
		}

#اگر_ختم

		params_فعال.سیٹ(param, سچ);
	}
}

صحیح param_سیٹ_default_قدر(param_t param, ثابت خالی *val)
{
	اگر (!handle_اندر_range(param)) {
		PX4_ERR("set default value invalid param %d", param);
		واپس PX4_ERROR;
	}

	اگر (val == خالی_اشاریہ) {
		PX4_ERR("set default value invalid value");
		واپس PX4_ERROR;
	}

	اگر (param_is_readonly(param)) {
		PX4_WARN("param %s is read-only", param_name(param));
		واپس PX4_ERROR;
	}

	صحیح نتیجہ = PX4_ERROR;


	// check if param being set to default value
	منطقی setting_مقصد_static_default = جھوٹ;
	ثابت param_قدر_u firmware_default_قدر = firmware_defaults.get(param);

	انتخاب (param_type(param)) {
	حالت PARAM_TYPE_INT32: {
			int32_t new_قدر;
			حافظہ_نقل(&new_قدر, val, حجم(new_قدر));
			setting_مقصد_static_default = firmware_default_قدر.i == new_قدر;
			توڑو;
		}

	حالت PARAM_TYPE_FLOAT: {
			اعشاری new_قدر;
			حافظہ_نقل(&new_قدر, val, حجم(new_قدر));
			setting_مقصد_static_default = fabsf(firmware_default_قدر.f - new_قدر) <= FLT_EPSILON;
			توڑو;
		}
	}

	اگر (setting_مقصد_static_default) {
		runtime_defaults.reset(param);

		نتیجہ = PX4_OK;

	} ورنہ {
		param_قدر_u new_قدر{};

		انتخاب (param_type(param)) {
		حالت PARAM_TYPE_INT32: {
				حافظہ_نقل(&new_قدر.i, val, حجم(new_قدر.i));
				توڑو;
			}

		حالت PARAM_TYPE_FLOAT: {
				حافظہ_نقل(&new_قدر.f, val, حجم(new_قدر.f));
				توڑو;
			}

		طےشدہ:
			توڑو;
		}

		اگر (runtime_defaults.store(param, new_قدر)) {
			user_config.refresh(param);
			نتیجہ = PX4_OK;

		} ورنہ {
			نتیجہ = PX4_ERROR;
		}
	}


	اگر ((نتیجہ == PX4_OK) && param_مستعمل(param)) {
		// send notification if param is already in use
		param_notify_changes();
	}

	واپس نتیجہ;
}

ساکن صحیح param_reset_اندرونی(param_t param, منطقی notify = سچ, منطقی autosave = سچ)
{
#اگر defined(CONFIG_PARAM_REMOTE)
	// Remote doesn't support reset
	واپس جھوٹ;
#اگر_ختم

	اگر (param_is_readonly(param)) {
		واپس 0;  // silently skip — param_reset_all loops over all params
	}

	منطقی param_ملا = user_config.شامل_ہے(param);

	اگر (handle_اندر_range(param)) {
		user_config.reset(param);
	}

	اگر (param_ملا) {
		/* Persist the reset. Flash-param saves append only unsaved params,
		 * so a reset has to show up as a tombstone or replay keeps the
		 * previous override. */
		params_unsaved.سیٹ(param, سچ);
	}

	اگر (autosave) {
		param_autosave();
	}

	اگر (param_ملا && notify) {
		param_notify_changes();
	}

#اگر defined(CONFIG_PARAM_PRIMARY)
	param_primary_reset(param);
#اگر_ختم

	واپس param_ملا;
}

صحیح param_reset(param_t param) { واپس param_reset_اندرونی(param, سچ); }
صحیح param_reset_no_notification(param_t param) { واپس param_reset_اندرونی(param, جھوٹ); }

ساکن خالی
param_reset_all_اندرونی(منطقی auto_save)
{
#اگر defined(CONFIG_PARAM_REMOTE)
	// Remote doesn't support reset
	واپس;
#اگر_ختم

	برائے (param_t param = 0; handle_اندر_range(param); param++) {
		param_reset_اندرونی(param, جھوٹ, جھوٹ);
	}

	اگر (auto_save) {
		param_autosave();
	}

#اگر defined(CONFIG_PARAM_PRIMARY)
	param_primary_reset_all();
#اگر_ختم

	param_notify_changes();
}

خالی
param_reset_all()
{
	param_reset_all_اندرونی(سچ);
}

خالی
param_reset_excludes(ثابت حرف *excludes[], صحیح عدد_excludes)
{
	برائے (param_t param = 0; handle_اندر_range(param); param++) {
		ثابت حرف *name = param_name(param);
		منطقی exclude = جھوٹ;

		برائے (صحیح اشاریہ = 0; اشاریہ < عدد_excludes; اشاریہ ++) {
			صحیح len = لڑی_لمبائی(excludes[اشاریہ]);

			اگر ((excludes[اشاریہ][len - 1] == '*'
			     && لڑی_موازنہ(name, excludes[اشاریہ], len - 1) == 0)
			    || strcmp(name, excludes[اشاریہ]) == 0) {

				exclude = سچ;
				توڑو;
			}
		}

		اگر (!exclude) {
			param_reset(param);
		}
	}
}

خالی
param_reset_specific(ثابت حرف *resets[], صحیح عدد_resets)
{
	برائے (param_t param = 0; handle_اندر_range(param); param++) {
		ثابت حرف *name = param_name(param);
		منطقی reset = جھوٹ;

		برائے (صحیح اشاریہ = 0; اشاریہ < عدد_resets; اشاریہ++) {
			صحیح len = لڑی_لمبائی(resets[اشاریہ]);

			اگر ((resets[اشاریہ][len - 1] == '*'
			     && لڑی_موازنہ(name, resets[اشاریہ], len - 1) == 0)
			    || strcmp(name, resets[اشاریہ]) == 0) {

				reset = سچ;
				توڑو;
			}
		}

		اگر (reset) {
			param_reset(param);
		}
	}
}

صحیح
param_سیٹ_default_file(ثابت حرف *filename)
{
	اگر (filename && param_backup_file && strcmp(filename, param_backup_file) == 0) {
		PX4_ERR("default file can't be the same as the backup file %s", filename);
		واپس PX4_ERROR;
	}

#اگر_تعریف_شدہ FLASH_BASED_PARAMS
	// the default for flash-based params is always the FLASH
	(خالی)filename;
#ورنہ

	اگر (param_default_file != خالی_اشاریہ) {
		// we assume this is not in use by some other thread
		آزاد(param_default_file);
		param_default_file = خالی_اشاریہ;
	}

	اگر (filename) {
		param_default_file = strdup(filename);
	}

#اگر_ختم /* FLASH_BASED_PARAMS */

	واپس 0;
}

ثابت حرف *param_get_default_file()
{
	واپس param_default_file;
}

صحیح param_سیٹ_backup_file(ثابت حرف *filename)
{
	اگر (filename && param_default_file && strcmp(filename, param_default_file) == 0) {
		PX4_ERR("backup file can't be the same as the default file %s", filename);
		واپس PX4_ERROR;
	}

	اگر (param_backup_file != خالی_اشاریہ) {
		// we assume this is not in use by some other thread
		آزاد(param_backup_file);
		param_backup_file = خالی_اشاریہ;
	}

	اگر (filename) {
		param_backup_file = strdup(filename);

	} ورنہ {
		param_backup_file = خالی_اشاریہ; // backup disabled
	}

	واپس 0;
}

ثابت حرف *param_get_backup_file()
{
	واپس param_backup_file;
}

ساکن صحیح param_export_اندرونی(صحیح fd, param_filter_func filter);
ساکن صحیح param_verify(صحیح fd);

صحیح param_save_default(منطقی blocking)
{
	PX4_DEBUG("param_save_default");

	// take the file lock
	اگر (blocking) {
		pthread_mutex_lock(&file_mutex);

	} ورنہ {
		اگر (pthread_mutex_trylock(&file_mutex) != 0) {
			PX4_DEBUG("param_save_default: file lock failed (already locked)");
			واپس -EWOULDBLOCK;
		}
	}

	صحیح shutdown_تالا_ret = px4_shutdown_تالا();

	اگر (shutdown_تالا_ret != 0) {
		PX4_ERR("px4_shutdown_lock() failed (%i)", shutdown_تالا_ret);
	}

	صحیح res = PX4_ERROR;
	ثابت حرف *filename = param_get_default_file();

	برائے (param_t param = 0; handle_اندر_range(param); param++) {
		اگر (params_unsaved[param]) {
			params_saving.سیٹ(param, سچ);
			params_unsaved.سیٹ(param, جھوٹ);
		}
	}

	اگر (filename) {
		ساکن constexpr صحیح MAX_ATTEMPTS = 3;

		برائے (صحیح attempt = 1; attempt <= MAX_ATTEMPTS; attempt++) {
			// write parameters to file
			صحیح fd = ::open(filename, O_WRONLY | O_CREAT | O_TRUNC, PX4_O_منوال_666);

			اگر (fd > -1) {
				perf_begin(param_export_perf);
				res = param_export_اندرونی(fd, خالی_اشاریہ);
				perf_انتہا(param_export_perf);
				::close(fd);

				اگر (res == PX4_OK) {
					// reopen file to verify
					صحیح fd_verify = ::open(filename, O_RDONLY, PX4_O_منوال_666);
					res = param_verify(fd_verify) || lseek(fd_verify, 0, SEEK_SET) || param_verify(fd_verify);
					::close(fd_verify);
				}
			}

			اگر (res == PX4_OK) {
				توڑو;

			} ورنہ {
				PX4_ERR("parameter export to %s failed (%d) attempt %d", filename, res, attempt);
				px4_usleep(10000); // wait at least 10 milliseconds before trying again
			}
		}

	} ورنہ {
		perf_begin(param_export_perf);
		res = flash_param_save(خالی_اشاریہ);
		perf_انتہا(param_export_perf);
	}

	اگر (res != PX4_OK) {
		PX4_ERR("param export failed (%d)", res);

		برائے (param_t param = 0; handle_اندر_range(param); param++) {
			اگر (params_saving[param]) {
				params_unsaved.سیٹ(param, سچ);
			}
		}
	}

	params_saving.reset();

	اگر (res == PX4_OK) {
		// backup file
		اگر (param_backup_file) {
			صحیح fd_backup_file = ::open(param_backup_file, O_WRONLY | O_CREAT | O_TRUNC, PX4_O_منوال_666);

			اگر (fd_backup_file > -1) {
				صحیح backup_export_ret = param_export_اندرونی(fd_backup_file, خالی_اشاریہ);
				::close(fd_backup_file);

				اگر (backup_export_ret != 0) {
					PX4_ERR("backup parameter export to %s failed (%d)", param_backup_file, backup_export_ret);

				} ورنہ {
					// verify export
					صحیح fd_verify = ::open(param_backup_file, O_RDONLY, PX4_O_منوال_666);
					param_verify(fd_verify);
					::close(fd_verify);
				}
			}
		}
	}

	pthread_mutex_unlock(&file_mutex);

	اگر (shutdown_تالا_ret == 0) {
		px4_shutdown_unlock();
	}

	واپس res;
}

/**
 * @return 0 on success, 1 if all params have not yet been stored, -1 if device open failed, -2 if writing parameters failed
 */
صحیح
param_بھرو_default()
{
	صحیح res = 0;
	ثابت حرف *filename = param_get_default_file();

	اگر (!filename) {
		واپس param_بھرو(-1);
	}

	صحیح fd_بھرو = ::open(filename, O_RDONLY);

	اگر (fd_بھرو < 0) {
		/* no parameter file is OK, otherwise this is an error */
		اگر (errno != ENOENT) {
			PX4_ERR("open '%s' for reading failed", filename);
			واپس -1;
		}

		واپس 1;
	}

	صحیح نتیجہ = param_بھرو(fd_بھرو);
	::close(fd_بھرو);

	اگر (نتیجہ == 1) {
		/* blank file: nothing stored yet */
		واپس 1;
	}

	اگر (نتیجہ != 0) {
		PX4_ERR("error reading parameters from '%s'", filename);
		واپس -2;
	}

	واپس res;
}

ساکن صحیح param_verify_callback(bson_decoder_t decoder, bson_گرہ_t گرہ_2)
{
	اگر (گرہ_2->type == BSON_EOO) {
		واپس 0;
	}

	// find the parameter this node represents
	param_t param = param_ڈھونڈو_no_notification(گرہ_2->name);

	اگر (param == PARAM_INVALID) {
		PX4_ERR("verify: invalid parameter '%s'", گرہ_2->name);
		واپس -1;
	}

	// handle verifying the parameter from the node
	انتخاب (گرہ_2->type) {
	حالت BSON_INT32: {
			اگر (param_type(param) != PARAM_TYPE_INT32) {
				PX4_ERR("verify: invalid param type %d for '%s' (BSON_INT32)", param_type(param), گرہ_2->name);
				واپس -1;
			}

			int32_t value;

			اگر (param_get(param, &value) == 0) {
				اگر (value == گرہ_2->i32) {
					واپس 1; // valid

				} ورنہ {
					PX4_ERR("verify: '%s' invalid BSON value %" PRIi32 "(expected %" PRIi32 ")", گرہ_2->name, گرہ_2->i32, value);
				}
			}
		}
		توڑو;

	حالت BSON_DOUBLE: {
			اگر (param_type(param) != PARAM_TYPE_FLOAT) {
				PX4_ERR("verify: invalid param type %d for '%s' (BSON_DOUBLE)", param_type(param), گرہ_2->name);
				واپس -1;
			}

			اعشاری value;

			اگر (param_get(param, &value) == 0) {
				اگر (fabsf(value - (اعشاری)گرہ_2->d) <= FLT_EPSILON) {
					واپس 1; // valid

				} ورنہ {
					PX4_ERR("verify: '%s' invalid BSON value %.3f (expected %.3f)", گرہ_2->name, گرہ_2->d, (دوہرا)value);
				}
			}
		}
		توڑو;

	طےشدہ:
		PX4_ERR("verify: '%s' invalid node type %d", گرہ_2->name, گرہ_2->type);
	}

	واپس -1;
}

ساکن صحیح param_verify(صحیح fd)
{
	PX4_DEBUG("param_verify");

	اگر (fd < 0) {
		واپس -1;
	}

	اگر (lseek(fd, 0, SEEK_SET) != 0) {
		PX4_ERR("verify: seek failed");
		واپس -1;
	}

	bson_decoder_s decoder{};

	اگر (bson_decoder_init_file(&decoder, fd, param_verify_callback) == 0) {
		صحیح نتیجہ = -1;

		کرو {
			نتیجہ = bson_decoder_اگلا(&decoder);

		} جب (نتیجہ > 0);

		اگر (نتیجہ == 0) {
			اگر (decoder.کل_document_سائز != decoder.کل_decoded_سائز) {
				PX4_ERR("BSON document size (%" PRId32 ") doesn't match bytes decoded (%" PRId32 ")", decoder.کل_document_سائز,
					decoder.کل_decoded_سائز);

			} ورنہ {
				واپس 0;
			}

		} ورنہ اگر (نتیجہ == -ENODATA) {
			PX4_ERR("verify: no BSON data");

		} ورنہ {
			PX4_ERR("verify: failed (%d)", نتیجہ);
		}
	}

	واپس -1;
}

صحیح
param_export(ثابت حرف *filename, param_filter_func filter)
{
	PX4_DEBUG("param_export");

	صحیح shutdown_تالا_ret = px4_shutdown_تالا();

	اگر (shutdown_تالا_ret != 0) {
		PX4_ERR("px4_shutdown_lock() failed (%i)", shutdown_تالا_ret);
	}

	// take the file lock
	اگر (pthread_mutex_trylock(&file_mutex) != 0) {
		PX4_ERR("param_export: file lock failed (already locked)");
		واپس PX4_ERROR;
	}

	صحیح fd = ::open(filename, O_RDWR | O_CREAT, PX4_O_منوال_666);
	صحیح نتیجہ = PX4_ERROR;

	perf_begin(param_export_perf);

	اگر (fd > -1) {
		نتیجہ = param_export_اندرونی(fd, filter);

	} ورنہ {
		نتیجہ = flash_param_save(filter);
	}

	perf_انتہا(param_export_perf);

	pthread_mutex_unlock(&file_mutex);

	اگر (shutdown_تالا_ret == 0) {
		px4_shutdown_unlock();
	}

	واپس نتیجہ;
}

// internal parameter export, caller is responsible for locking
ساکن صحیح param_export_اندرونی(صحیح fd, param_filter_func filter)
{
	PX4_DEBUG("param_export_internal");
	ثابت خودکار بدلا_params = user_config.containedAsBitset();

	صحیح نتیجہ = -1;
	bson_encoder_s encoder{};
	uint8_t bson_بفر[256];

	اگر (lseek(fd, 0, SEEK_SET) != 0) {
		PX4_ERR("export seek failed %d", errno);
		واپس -1;
	}

	اگر (bson_encoder_init_buf_file(&encoder, fd, &bson_بفر, حجم(bson_بفر)) != 0) {
		جاؤ برآمد;
	}

	برائے (param_t param = 0; handle_اندر_range(param); param++) {
		اگر (!بدلا_params[param] || (filter && !filter(param))) {
			جاری;
		}

		ثابت param_قدر_u runtime_default_قدر = runtime_defaults.get(param);
		ثابت param_قدر_u user_config_قدر = user_config.get(param);

		// don't export default values
		انتخاب (param_type(param)) {
		حالت PARAM_TYPE_INT32:
			اگر (user_config_قدر.i == runtime_default_قدر.i) {
				PX4_DEBUG("skipping %s %" PRIi32 " export", param_name(param), runtime_default_قدر.i);
				جاری;
			}

			توڑو;

		حالت PARAM_TYPE_FLOAT:
			اگر (fabsf(user_config_قدر.f - runtime_default_قدر.f) <= FLT_EPSILON) {
				PX4_DEBUG("skipping %s %.3f export", param_name(param), (دوہرا)runtime_default_قدر.f);
				جاری;
			}

			توڑو;
		}

		ثابت حرف *name = param_name(param);
		ثابت size_t سائز = param_سائز(param);

		/* append the appropriate BSON type object */
		انتخاب (param_type(param)) {
		حالت PARAM_TYPE_INT32: {
				ثابت int32_t i = user_config_قدر.i;
				PX4_DEBUG("exporting: %s (%d) size: %lu val: %" PRIi32, name, param, (طویل غیرعلامتی صحیح)سائز, i);

				اگر (bson_encoder_append_int32(&encoder, name, i) != 0) {
					PX4_ERR("BSON append failed for '%s'", name);
					جاؤ برآمد;
				}
			}
			توڑو;

		حالت PARAM_TYPE_FLOAT: {
				ثابت دوہرا f = (دوہرا)user_config_قدر.f;
				PX4_DEBUG("exporting: %s (%d) size: %lu val: %.3f", name, param, (طویل غیرعلامتی صحیح)سائز, (دوہرا)f);

				اگر (bson_encoder_append_double(&encoder, name, f) != 0) {
					PX4_ERR("BSON append failed for '%s'", name);
					جاؤ برآمد;
				}
			}
			توڑو;

		طےشدہ:
			PX4_ERR("%s unrecognized parameter type %d, skipping export", name, param_type(param));
		}
	}

	نتیجہ = 0;

برآمد:

	اگر (نتیجہ == 0) {
		اگر (bson_encoder_fini(&encoder) != PX4_OK) {
			PX4_ERR("BSON encoder finalize failed");
			نتیجہ = -1;
		}
	}

	واپس نتیجہ;
}

#اگر defined(FLASH_BASED_PARAMS)
// A file is never the flash backend's store, so what an import changes is unsaved until a delta carries it.
// As on the file backend, the import itself does not save.
ساکن constexpr منطقی file_import_is_saved = جھوٹ;
#ورنہ
ساکن constexpr منطقی file_import_is_saved = سچ;
#اگر_ختم

ساکن صحیح
param_import_callback(bson_decoder_t decoder, bson_گرہ_t گرہ_2)
{
	/*
	 * EOO means the end of the parameter object. (Currently not supporting
	 * nested BSON objects).
	 */
	اگر (گرہ_2->type == BSON_EOO) {
		PX4_DEBUG("end of parameters");
		واپس 0;
	}

	// A tombstone has no value. The decoder leaves the previous node's in the union and the translations
	// read it without a type check.
	ثابت منطقی tombstone = گرہ_2->type == BSON_nullptr || گرہ_2->type == BSON_UNDEFINED;

	// if we do param_set() directly in the translation, set PARAM_SKIP_IMPORT as return value and return here
	اگر (!tombstone && param_modify_on_import(گرہ_2) == param_modify_on_import_ret::PARAM_SKIP_IMPORT) {
		واپس 1;
	}

	// Find the parameter this node represents.  If we don't know it, ignore the node.
	param_t param = param_ڈھونڈو_no_notification(گرہ_2->name);

	اگر (param == PARAM_INVALID) {
		PX4_WARN("ignoring unrecognised parameter '%s'", گرہ_2->name);
		واپس 1;
	}

	// Handle setting the parameter from the node
	انتخاب (گرہ_2->type) {
	حالت BSON_nullptr:
	حالت BSON_UNDEFINED:
		param_reset_اندرونی(param, سچ, جھوٹ);
		واپس 1;

	حالت BSON_INT32: {
			اگر (param_type(param) == PARAM_TYPE_INT32) {
				int32_t i = گرہ_2->i32;
				param_سیٹ_اندرونی(param, &i, file_import_is_saved, سچ, سچ, جھوٹ);
				PX4_DEBUG("Imported %s with value %" PRIi32, param_name(param), i);

			} ورنہ {
				PX4_WARN("unexpected type for %s", گرہ_2->name);
			}
		}
		توڑو;

	حالت BSON_DOUBLE: {
			اگر (param_type(param) == PARAM_TYPE_FLOAT) {
				اعشاری f = گرہ_2->d;
				param_سیٹ_اندرونی(param, &f, file_import_is_saved, سچ, سچ, جھوٹ);
				PX4_DEBUG("Imported %s with value %f", param_name(param), (دوہرا)f);

			} ورنہ {
				PX4_WARN("unexpected type for %s", گرہ_2->name);
			}
		}
		توڑو;

	طےشدہ:
		PX4_ERR("import: unrecognised node type for '%s'", گرہ_2->name);
	}

	// don't return zero, that means EOF
	واپس 1;
}

ساکن صحیح
param_import_اندرونی(صحیح fd)
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
	ساخت stat st;

	اگر (fstat(fd, &st) == 0 && S_ISREG(st.st_mode) && st.st_size == 0) {
		واپس 1;
	}

	int32_t doc_سائز = 0;
	ssize_t len = اقرأ(fd, &doc_سائز, حجم(doc_سائز));

	اگر (len == 0 || (len == (ssize_t)حجم(doc_سائز) && (doc_سائز == 0 || doc_سائز == -1))) {
		واپس 1;
	}

	اگر (lseek(fd, 0, SEEK_SET) != 0) {
		PX4_ERR("import lseek failed (%d)", errno);
		واپس -1;
	}

	ساکن constexpr صحیح MAX_ATTEMPTS = 3;

	برائے (صحیح attempt = 1; attempt <= MAX_ATTEMPTS; attempt++) {
		bson_decoder_s decoder{};

		اگر (bson_decoder_init_file(&decoder, fd, param_import_callback) == 0) {
			صحیح نتیجہ = -1;

			کرو {
				نتیجہ = bson_decoder_اگلا(&decoder);

			} جب (نتیجہ > 0);

			اگر (نتیجہ == 0) {
				اگر (decoder.کل_document_سائز == decoder.کل_decoded_سائز) {
					PX4_INFO("BSON document size %" PRId32 " bytes, decoded %" PRId32 " bytes (INT32:%" PRIu16 ", FLOAT:%" PRIu16 ")",
						 decoder.کل_document_سائز, decoder.کل_decoded_سائز,
						 decoder.گنتی_گرہ_int32, decoder.گنتی_گرہ_double);

					واپس 0;

				} ورنہ {
					PX4_ERR("BSON document size (%" PRId32 ") doesn't match bytes decoded (%" PRId32 ")",
						decoder.کل_document_سائز, decoder.کل_decoded_سائز);
				}

			} ورنہ اگر (نتیجہ == -ENODATA) {
				// silently retry as a precaution unless this is our last attempt
				اگر (attempt == MAX_ATTEMPTS) {
					PX4_DEBUG("BSON: no data");
					واپس 0;
				}

			} ورنہ {
				PX4_ERR("param import failed (%d) attempt %d", نتیجہ, attempt);
			}

		} ورنہ {
			PX4_ERR("param import bson decoder init failed (attempt %d)", attempt);
		}

		اگر (attempt != MAX_ATTEMPTS) {
			اگر (lseek(fd, 0, SEEK_SET) != 0) {
				PX4_ERR("import lseek failed (%d)", errno);
			}

			px4_usleep(10000); // wait at least 10 milliseconds before trying again
		}
	}

	واپس -1;
}

#اگر defined(FLASH_BASED_PARAMS)
ساکن صحیح flash_backend_locked(صحیح (*op)())
{
	/* Same order as param_save_default: mutex then shutdown lock. Autosave
	 * uses a trylock, so a compact holds it off the flash programming. */
	pthread_mutex_lock(&file_mutex);

	صحیح shutdown_تالا_ret = px4_shutdown_تالا();

	اگر (shutdown_تالا_ret != 0) {
		PX4_ERR("px4_shutdown_lock() failed (%i)", shutdown_تالا_ret);
	}

	صحیح نتیجہ = op();

	pthread_mutex_unlock(&file_mutex);

	اگر (shutdown_تالا_ret == 0) {
		px4_shutdown_unlock();
	}

	واپس نتیجہ;
}

ساکن صحیح flash_بھرو_locked()
{
	/* Reset under the lock so a pending autosave cannot append tombstones
	 * of the just-cleared RAM before import/compact run. */
	param_reset_all_اندرونی(جھوٹ);
	صحیح نتیجہ = flash_param_import();

	اگر (نتیجہ == 0 || نتیجہ == 1) {
		params_unsaved.reset();
	}

	واپس نتیجہ;
}
#اگر_ختم

صحیح
param_import(صحیح fd)
{
	اگر (fd < 0) {
#اگر defined(FLASH_BASED_PARAMS)
		واپس flash_backend_locked(flash_param_import);
#ورنہ
		واپس flash_param_import();
#اگر_ختم
	}

	واپس param_import_اندرونی(fd);
}

صحیح
param_بھرو(صحیح fd)
{
	اگر (fd < 0) {
#اگر defined(FLASH_BASED_PARAMS)
		واپس flash_backend_locked(flash_بھرو_locked);
#ورنہ
		واپس PX4_ERROR;
#اگر_ختم
	}

	param_reset_all_اندرونی(جھوٹ);
	صحیح نتیجہ = param_import_اندرونی(fd);

#اگر !defined(FLASH_BASED_PARAMS)

	اگر (نتیجہ >= 0) {
		params_unsaved.reset();
	}

#اگر_ختم

	واپس نتیجہ;
}

خالی
param_foreach(خالی (*func)(خالی *arg, param_t param), خالی *arg, منطقی only_بدلا, منطقی only_مستعمل)
{
	param_t	param;

	برائے (param = 0; handle_اندر_range(param); param++) {

		/* if requested, skip unchanged values */
		اگر (only_بدلا && (!user_config.شامل_ہے(param))) {
			جاری;
		}

		اگر (only_مستعمل && !param_مستعمل(param)) {
			جاری;
		}

		func(arg, param);
	}
}

uint32_t param_hash_check()
{
	uint32_t param_hash = 0;

	/* compute the CRC32 over all string param names and 4 byte values */
	برائے (param_t param = 0; handle_اندر_range(param); param++) {
		اگر (!param_مستعمل(param) || param_is_volatile(param)) {
			جاری;
		}

		ثابت حرف *name = param_name(param);
		خودکار value = user_config.get(param).i;
		ثابت خالی *val = (خالی *)&value;
		param_hash = crc32part((ثابت uint8_t *)name, لڑی_لمبائی(name), param_hash);
		param_hash = crc32part((ثابت uint8_t *)val, param_سائز(param), param_hash);
	}

	واپس param_hash;
}

خالی param_print_حالت()
{
	PX4_INFO("summary: %d/%d (used/total)", param_گنتی_مستعمل(), param_گنتی());

#اگر_غیر_تعریف_شدہ FLASH_BASED_PARAMS
	ثابت حرف *filename = param_get_default_file();

	اگر (filename != خالی_اشاریہ) {
		PX4_INFO("file: %s", param_get_default_file());
	}

	اگر (param_backup_file) {
		PX4_INFO("backup file: %s", param_backup_file);
	}

#اگر_ختم /* FLASH_BASED_PARAMS */

	PX4_INFO("storage array: %d/%d elements (%zu bytes total)",
		 user_config.سائز(), firmware_defaults.سائز(), (size_t)user_config.byte_سائز());


	PX4_INFO("storage array (custom defaults): %d/%d elements (%zu bytes total)",
		 runtime_defaults.سائز(), firmware_defaults.سائز(), (size_t)runtime_defaults.byte_سائز());

	اگر (autosave_instance) {
		PX4_INFO("auto save: %s", autosave_instance->enabled() ? "on" : "off");

		hrt_abstime آخری_autosave = autosave_instance->آخری_Autosave();

		اگر (آخری_autosave > 0) {
			PX4_INFO("last auto save: %.3f seconds ago", hrt_elapsed_وقت(&آخری_autosave) * 1e-6);
		}
	}

	perf_print_شمار(param_export_perf);
	perf_print_شمار(param_ڈھونڈو_perf);
	perf_print_شمار(param_get_perf);
	perf_print_شمار(param_سیٹ_perf);

#اگر defined(CONFIG_PARAM_PRIMARY)
	ساخت param_primary_counters counts;
	param_primary_get_counters(&counts);
	PX4_INFO("set value requests received: %" PRIu32 ", set value responses sent: %" PRIu32,
		 counts.سیٹ_قدر_request_received, counts.سیٹ_قدر_response_بھیجا);
	PX4_INFO("set value requests sent: %" PRIu32 ", set value responses received: %" PRIu32,
		 counts.سیٹ_قدر_request_بھیجا, counts.سیٹ_قدر_response_received);
	PX4_INFO("resets sent: %" PRIu32 ", set used requests received: %" PRIu32,
		 counts.reset_بھیجا, counts.سیٹ_مستعمل_received);
#اگر_ختم

#اگر defined(CONFIG_PARAM_REMOTE)
	ساخت param_remote_counters counts;
	param_remote_get_counters(&counts);
	PX4_INFO("set value requests received: %" PRIu32 ", set value responses sent: %" PRIu32,
		 counts.سیٹ_قدر_request_received, counts.سیٹ_قدر_response_بھیجا);
	PX4_INFO("set value requests sent: %" PRIu32 ", set value responses received: %" PRIu32,
		 counts.سیٹ_قدر_request_بھیجا, counts.سیٹ_قدر_response_received);
	PX4_INFO("resets received: %" PRIu32 ", set used requests sent: %" PRIu32,
		 counts.reset_received, counts.سیٹ_مستعمل_بھیجا);
#اگر_ختم

}
