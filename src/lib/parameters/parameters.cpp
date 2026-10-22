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

#وصف_ڪريو PARAM_IMPLEMENTATION
#ضمن "param.h"
#ضمن "param_translation.h"
#ضمن <parameters/px4_parameters.hpp>
#ضمن <lib/tinybson/tinybson.h>

#إذا defined(__PX4_NUTTX)
#ضمن <nuttx/crc32.h>
#وإلا
#ضمن <crc32.h>
#جيڪڏهن_پورو
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

استعمال نطاق وقت_literals;

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
#جيڪڏهن_پورو

#إذا defined(FLASH_BASED_PARAMS)
#ضمن "flashparams/flashparams.h"
#وإلا
مضمن ساكن صحيح flash_param_save(param_filter_func filter) { أرجع -1; }
مضمن ساكن صحيح flash_param_import() { أرجع -1; }
#جيڪڏهن_پورو

ساكن حرف *param_default_file = خالي_اشارو;
ساكن حرف *param_backup_file = خالي_اشارو;

#ضمن "autosave.h"
ساكن ParamAutosave *autosave_instance {خالي_اشارو};

ساكن px4::AtomicBitset<param_info_ڳڻپ> params_سرگرم;  // params found
ساكن px4::AtomicBitset<param_info_ڳڻپ> params_unsaved;

// Params the save in progress is writing. They are moved out of params_unsaved
// before their values are read, so a param_set that lands while the save runs
// marks its param again and reaches the next save. Flash-param saves write only
// unsaved params, so nothing else would ever write that change.
ساكن px4::AtomicBitset<param_info_ڳڻپ> params_saving;

ساكن ConstLayer firmware_defaults;
ساكن DynamicSparseLayer runtime_defaults{&firmware_defaults};
DynamicSparseLayer user_config{&runtime_defaults};

/** parameter update topic handle */
#إذا not defined(CONFIG_PARAM_REMOTE)
ساكن orb_advert_t param_topic = خالي_اشارو;
ساكن بنانشان صحيح param_instance = 0;
#جيڪڏهن_پورو

ساكن perf_counter_t param_export_perf;
ساكن perf_counter_t param_ڳوليو_perf;
ساكن perf_counter_t param_get_perf;
ساكن perf_counter_t param_سيٽ_perf;

ساكن pthread_mutex_t file_mutex  =
	PTHREAD_MUTEX_INITIALIZER; ///< this protects against concurrent param saves (file or flash access).

// Support for remote parameter node
#إذا defined(CONFIG_PARAM_PRIMARY)
# include "parameters_primary.h"
#جيڪڏهن_پورو // CONFIG_PARAM_PRIMARY
#إذا defined(CONFIG_PARAM_REMOTE)
# include "parameters_remote.h"
#جيڪڏهن_پورو // CONFIG_PARAM_REMOTE

فراغ
param_init()
{
#جيڪڏهن_وصف_ٿيل __PX4_POSIX
	// Set up the recursive param mutex before any thread can access parameters.
	AtomicTransaction::initialize();
#جيڪڏهن_پورو

	param_export_perf = perf_alloc(PC_ELAPSED, "param: export");
	param_ڳوليو_perf = perf_alloc(PC_ڳڻپ, "param: find");
	param_get_perf = perf_alloc(PC_ڳڻپ, "param: get");
	param_سيٽ_perf = perf_alloc(PC_ELAPSED, "param: set");

#إذا defined(__PX4_NUTTX) && !defined(CONFIG_BUILD_FLAT)
	px4_رجسٽر_boardct_ioctl(_PARAMIOCBASE, param_ioctl);
#جيڪڏهن_پورو

#إذا defined(CONFIG_PARAM_PRIMARY)
	param_primary_init();
#جيڪڏهن_پورو // CONFIG_PARAM_PRIMARY

#إذا defined(CONFIG_PARAM_REMOTE)
	param_remote_init();
#جيڪڏهن_پورو // CONFIG_PARAM_REMOTE

#إذا not defined(CONFIG_PARAM_REMOTE)
	autosave_instance = جديد ParamAutosave();
#جيڪڏهن_پورو
}


فراغ
param_notify_changes()
{
// Don't send if this is a remote node. Only the primary
// sends out update notices
#إذا not defined(CONFIG_PARAM_REMOTE)
	parameter_update_s pup {};
	pup.instance = param_instance++;
	pup.get_ڳڻپ_2 = perf_event_ڳڻپ(param_get_perf);
	pup.سيٽ_ڳڻپ = perf_event_ڳڻپ(param_سيٽ_perf);
	pup.ڳوليو_ڳڻپ = perf_event_ڳڻپ(param_ڳوليو_perf);
	pup.export_ڳڻپ = perf_event_ڳڻپ(param_export_perf);
	pup.سرگرم_2 = params_سرگرم.ڳڻپ();
	pup.بدليو = user_config.سائيز();
	pup.custom_default = runtime_defaults.سائيز();
	pup.timestamp = hrt_absolute_وقت();

	إذا (param_topic == خالي_اشارو) {
		param_topic = orb_advertise(ORB_ID(parameter_update), &pup);

	} وإلا {
		orb_publish(ORB_ID(parameter_update), param_topic, &pup);
	}

#جيڪڏهن_پورو
}

ساكن param_t param_ڳوليو_اندروني(ثابت حرف *name, منطقي notification)
{
	perf_ڳڻپ(param_ڳوليو_perf);

	param_t وچ;
	param_t front = 0;
	param_t آخري = param_info_ڳڻپ;

	/* perform a binary search of the known parameters */

	طالما (front <= آخري) {
		وچ = front + (آخري - front) / 2;
		صحيح ret = strcmp(name, param_name(وچ));

		إذا (ret == 0) {
			إذا (notification) {
				param_سيٽ_استعمال_ٿيل(وچ);
			}

			أرجع وچ;

		} وإلا إذا (وچ == front) {
			/* An end point has been hit, but there has been no match */
			اكسر;

		} وإلا إذا (ret < 0) {
			آخري = وچ;

		} وإلا {
			front = وچ;
		}
	}

	/* not found */
	أرجع PARAM_INVALID;
}

param_t param_ڳوليو(ثابت حرف *name)
{
	أرجع param_ڳوليو_اندروني(name, سچ);
}

param_t param_ڳوليو_no_notification(ثابت حرف *name)
{
	أرجع param_ڳوليو_اندروني(name, خطأ);
}

بنانشان param_ڳڻپ_استعمال_ٿيل()
{
	أرجع params_سرگرم.ڳڻپ();
}

param_t param_for_استعمال_ٿيل_اشاريو(بنانشان اشاريو)
{
	// walk all params and count used params
	إذا (اشاريو < param_info_ڳڻپ) {
		بنانشان استعمال_ٿيل_ڳڻپ = 0;

		لكل (صحيح i = 0; i < params_سرگرم.سائيز(); i++) {
			إذا (params_سرگرم[i]) {
				// we found the right used count,
				//  return the param value
				إذا (اشاريو == استعمال_ٿيل_ڳڻپ) {
					أرجع static_cast<param_t>(i);
				}

				استعمال_ٿيل_ڳڻپ++;
			}
		}
	}

	أرجع PARAM_INVALID;
}

صحيح param_get_استعمال_ٿيل_اشاريو(param_t param)
{
	/* this tests for out of bounds and does a constant time lookup */
	إذا (!param_استعمال_ٿيل(param)) {
		أرجع -1;
	}

	/* walk all params and count, now knowing that it has a valid index */
	صحيح استعمال_ٿيل_ڳڻپ = 0;

	لكل (صحيح i = 0; i < params_سرگرم.سائيز(); i++) {
		إذا (params_سرگرم[i]) {

			إذا (param == i) {
				أرجع استعمال_ٿيل_ڳڻپ;
			}

			استعمال_ٿيل_ڳڻپ++;
		}
	}

	أرجع -1;
}

منطقي
param_قدر_unsaved(param_t param)
{
	أرجع handle_اندريون_range(param) ? (params_unsaved[param] || params_saving[param]) : خطأ;
}

صحيح
param_get(param_t param, فراغ *val)
{
	perf_ڳڻپ(param_get_perf);

	إذا (!handle_اندريون_range(param)) {
		PX4_ERR("get: param %" PRId16 " invalid", param);
		أرجع PX4_ERROR;
	}

	إذا (!params_سرگرم[param]) {
		PX4_DEBUG("get: param %" PRId16 " (%s) not active", param, param_name(param));
	}

	صحيح نتيجو = PX4_ERROR;

	إذا (val) {

		خودڪار retrieve_قدر = user_config.get(param);

		بدل (param_type(param)) {
		حالة PARAM_TYPE_INT32:
			يادگيري_نقل(val, &retrieve_قدر.i, حجم(retrieve_قدر.i));
			أرجع PX4_OK;

		حالة PARAM_TYPE_FLOAT:
			يادگيري_نقل(val, &retrieve_قدر.f, حجم(retrieve_قدر.f));
			أرجع PX4_OK;
		}
	}

	أرجع نتيجو;
}

صحيح
param_get_default_قدر_اندروني(param_t param, فراغ *default_val)
{
	إذا (!handle_اندريون_range(param)) {
		PX4_ERR("get default value: param %d invalid", param);
		أرجع PX4_ERROR;
	}

	إذا (default_val) {
		بدل (param_type(param)) {
		حالة PARAM_TYPE_INT32: {
				int32_t val = runtime_defaults.get(param).i;
				يادگيري_نقل(default_val, &val, حجم(val));
				أرجع PX4_OK;
			}

		حالة PARAM_TYPE_FLOAT: {
				عائم val = runtime_defaults.get(param).f;
				يادگيري_نقل(default_val, &val, حجم(val));
				أرجع PX4_OK;
			}
		}
	}

	أرجع PX4_ERROR;
}

صحيح
param_get_default_قدر(param_t param, فراغ *default_val)
{
	إذا (!handle_اندريون_range(param)) {
		أرجع PX4_ERROR;
	}

	صحيح ret = param_get_default_قدر_اندروني(param, default_val);
	أرجع ret;
}

منطقي param_قدر_is_default(param_t param)
{
	إذا (!handle_اندريون_range(param)) {
		أرجع سچ;
	}

	إذا (!user_config.شامل_آهي(param)) {
		// if user config does not contain it, consider it default.
		أرجع سچ;

	} وإلا {
		// compare with default value
		ثابت param_قدر_u user_config_قدر = user_config.get(param);
		ثابت param_قدر_u runtime_default_قدر = runtime_defaults.get(param);

		بدل (param_type(param)) {
		حالة PARAM_TYPE_INT32: {
				أرجع user_config_قدر.i == runtime_default_قدر.i;
			}

		حالة PARAM_TYPE_FLOAT: {
				أرجع fabsf(user_config_قدر.f - runtime_default_قدر.f) < FLT_EPSILON;
			}
		}
	}

	أرجع سچ;
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
param_سيٽ_اندروني(param_t param, ثابت فراغ *val, منطقي mark_saved, منطقي notify_changes, منطقي update_remote = سچ,
		   منطقي autosave = سچ)
{
	إذا (!handle_اندريون_range(param)) {
		PX4_ERR("set invalid param %d", param);
		أرجع PX4_ERROR;
	}

	إذا (val == خالي_اشارو) {
		PX4_ERR("set invalid value");
		أرجع PX4_ERROR;
	}

	إذا (param_is_readonly(param)) {
		PX4_WARN("param %s is read-only", param_name(param));
		أرجع PX4_ERROR;
	}

	صحيح نتيجو = -1;
	منطقي param_بدليو = خطأ;
	perf_begin(param_سيٽ_perf);

	ثابت param_قدر_u user_config_قدر = user_config.get(param);
	param_قدر_u new_قدر{};

	بدل (param_type(param)) {
	حالة PARAM_TYPE_INT32:
		يادگيري_نقل(&new_قدر.i, val, حجم(new_قدر.i));
		param_بدليو = user_config_قدر.i != new_قدر.i;
		اكسر;

	حالة PARAM_TYPE_FLOAT:
		يادگيري_نقل(&new_قدر.f, val, حجم(new_قدر.f));
		param_بدليو = fabsf(user_config_قدر.f - new_قدر.f) > FLT_EPSILON;
		اكسر;

	افتراضي: {
			PX4_ERR("param_set invalid param type for %s", param_name(param));
			اكسر;
		}
	}

	إذا (user_config.store(param, new_قدر)) {
		إذا (param_بدليو) {
			params_unsaved.سيٽ(param, !mark_saved);
		}

		نتيجو = PX4_OK;

	} وإلا {
		PX4_ERR("param_set failed to store param %s", param_name(param));
		نتيجو = PX4_ERROR;
	}

	إذا ((نتيجو == PX4_OK) && param_بدليو && !mark_saved && autosave) { // this is false when importing parameters
		param_autosave();
	}

	// If this is the parameter server, make sure that the remote is updated
#إذا defined(CONFIG_PARAM_PRIMARY)

	إذا (param_بدليو && update_remote) {
		param_primary_سيٽ_قدر(param, val);
	}

#جيڪڏهن_پورو

	// If this is the parameter remote, make sure that the primary is updated
#إذا defined(CONFIG_PARAM_REMOTE)

	إذا (param_بدليو && update_remote) {
		param_remote_سيٽ_قدر(param, val);
	}

#جيڪڏهن_پورو

	perf_پڇاڙي(param_سيٽ_perf);

	/*
	 * If we set something, now that we have unlocked, go ahead and advertise that
	 * a thing has been set.
	 */
	إذا ((نتيجو == PX4_OK) && param_بدليو && notify_changes) {
		param_notify_changes();
	}

	أرجع نتيجو;
}

#إذا defined(FLASH_BASED_PARAMS)
صحيح param_سيٽ_external(param_t param, ثابت فراغ *val, منطقي mark_saved, منطقي notify_changes)
{
	أرجع param_سيٽ_اندروني(param, val, mark_saved, notify_changes);
}

فراغ param_get_external(param_t param, فراغ *val)
{
	param_get(param, val);
}
#جيڪڏهن_پورو

صحيح param_سيٽ(param_t param, ثابت فراغ *val)
{
	أرجع param_سيٽ_اندروني(param, val, خطأ, سچ);
}

صحيح param_سيٽ_no_notification(param_t param, ثابت فراغ *val)
{
	أرجع param_سيٽ_اندروني(param, val, خطأ, خطأ);
}

صحيح param_سيٽ_no_remote_update(param_t param, ثابت فراغ *val, منطقي notify)
{
	أرجع param_سيٽ_اندروني(param, val, خطأ, notify, خطأ);
}

منطقي param_استعمال_ٿيل(param_t param)
{
	إذا (handle_اندريون_range(param)) {
		أرجع params_سرگرم[param];
	}

	أرجع خطأ;
}

فراغ param_سيٽ_استعمال_ٿيل(param_t param)
{
	إذا (handle_اندريون_range(param)) {
#إذا defined(CONFIG_PARAM_REMOTE)

		إذا (!param_استعمال_ٿيل(param)) {
			param_remote_سيٽ_استعمال_ٿيل(param);
		}

#جيڪڏهن_پورو

		params_سرگرم.سيٽ(param, سچ);
	}
}

صحيح param_سيٽ_default_قدر(param_t param, ثابت فراغ *val)
{
	إذا (!handle_اندريون_range(param)) {
		PX4_ERR("set default value invalid param %d", param);
		أرجع PX4_ERROR;
	}

	إذا (val == خالي_اشارو) {
		PX4_ERR("set default value invalid value");
		أرجع PX4_ERROR;
	}

	إذا (param_is_readonly(param)) {
		PX4_WARN("param %s is read-only", param_name(param));
		أرجع PX4_ERROR;
	}

	صحيح نتيجو = PX4_ERROR;


	// check if param being set to default value
	منطقي setting_منزل_static_default = خطأ;
	ثابت param_قدر_u firmware_default_قدر = firmware_defaults.get(param);

	بدل (param_type(param)) {
	حالة PARAM_TYPE_INT32: {
			int32_t new_قدر;
			يادگيري_نقل(&new_قدر, val, حجم(new_قدر));
			setting_منزل_static_default = firmware_default_قدر.i == new_قدر;
			اكسر;
		}

	حالة PARAM_TYPE_FLOAT: {
			عائم new_قدر;
			يادگيري_نقل(&new_قدر, val, حجم(new_قدر));
			setting_منزل_static_default = fabsf(firmware_default_قدر.f - new_قدر) <= FLT_EPSILON;
			اكسر;
		}
	}

	إذا (setting_منزل_static_default) {
		runtime_defaults.reset(param);

		نتيجو = PX4_OK;

	} وإلا {
		param_قدر_u new_قدر{};

		بدل (param_type(param)) {
		حالة PARAM_TYPE_INT32: {
				يادگيري_نقل(&new_قدر.i, val, حجم(new_قدر.i));
				اكسر;
			}

		حالة PARAM_TYPE_FLOAT: {
				يادگيري_نقل(&new_قدر.f, val, حجم(new_قدر.f));
				اكسر;
			}

		افتراضي:
			اكسر;
		}

		إذا (runtime_defaults.store(param, new_قدر)) {
			user_config.refresh(param);
			نتيجو = PX4_OK;

		} وإلا {
			نتيجو = PX4_ERROR;
		}
	}


	إذا ((نتيجو == PX4_OK) && param_استعمال_ٿيل(param)) {
		// send notification if param is already in use
		param_notify_changes();
	}

	أرجع نتيجو;
}

ساكن صحيح param_reset_اندروني(param_t param, منطقي notify = سچ, منطقي autosave = سچ)
{
#إذا defined(CONFIG_PARAM_REMOTE)
	// Remote doesn't support reset
	أرجع خطأ;
#جيڪڏهن_پورو

	إذا (param_is_readonly(param)) {
		أرجع 0;  // silently skip — param_reset_all loops over all params
	}

	منطقي param_مليو = user_config.شامل_آهي(param);

	إذا (handle_اندريون_range(param)) {
		user_config.reset(param);
	}

	إذا (param_مليو) {
		/* Persist the reset. Flash-param saves append only unsaved params,
		 * so a reset has to show up as a tombstone or replay keeps the
		 * previous override. */
		params_unsaved.سيٽ(param, سچ);
	}

	إذا (autosave) {
		param_autosave();
	}

	إذا (param_مليو && notify) {
		param_notify_changes();
	}

#إذا defined(CONFIG_PARAM_PRIMARY)
	param_primary_reset(param);
#جيڪڏهن_پورو

	أرجع param_مليو;
}

صحيح param_reset(param_t param) { أرجع param_reset_اندروني(param, سچ); }
صحيح param_reset_no_notification(param_t param) { أرجع param_reset_اندروني(param, خطأ); }

ساكن فراغ
param_reset_all_اندروني(منطقي auto_save)
{
#إذا defined(CONFIG_PARAM_REMOTE)
	// Remote doesn't support reset
	أرجع;
#جيڪڏهن_پورو

	لكل (param_t param = 0; handle_اندريون_range(param); param++) {
		param_reset_اندروني(param, خطأ, خطأ);
	}

	إذا (auto_save) {
		param_autosave();
	}

#إذا defined(CONFIG_PARAM_PRIMARY)
	param_primary_reset_all();
#جيڪڏهن_پورو

	param_notify_changes();
}

فراغ
param_reset_all()
{
	param_reset_all_اندروني(سچ);
}

فراغ
param_reset_excludes(ثابت حرف *excludes[], صحيح انگ_excludes)
{
	لكل (param_t param = 0; handle_اندريون_range(param); param++) {
		ثابت حرف *name = param_name(param);
		منطقي exclude = خطأ;

		لكل (صحيح اشاريو = 0; اشاريو < انگ_excludes; اشاريو ++) {
			صحيح len = لڙي_ڊيگهه(excludes[اشاريو]);

			إذا ((excludes[اشاريو][len - 1] == '*'
			     && لڙي_ڀيٽ(name, excludes[اشاريو], len - 1) == 0)
			    || strcmp(name, excludes[اشاريو]) == 0) {

				exclude = سچ;
				اكسر;
			}
		}

		إذا (!exclude) {
			param_reset(param);
		}
	}
}

فراغ
param_reset_specific(ثابت حرف *resets[], صحيح انگ_resets)
{
	لكل (param_t param = 0; handle_اندريون_range(param); param++) {
		ثابت حرف *name = param_name(param);
		منطقي reset = خطأ;

		لكل (صحيح اشاريو = 0; اشاريو < انگ_resets; اشاريو++) {
			صحيح len = لڙي_ڊيگهه(resets[اشاريو]);

			إذا ((resets[اشاريو][len - 1] == '*'
			     && لڙي_ڀيٽ(name, resets[اشاريو], len - 1) == 0)
			    || strcmp(name, resets[اشاريو]) == 0) {

				reset = سچ;
				اكسر;
			}
		}

		إذا (reset) {
			param_reset(param);
		}
	}
}

صحيح
param_سيٽ_default_file(ثابت حرف *filename)
{
	إذا (filename && param_backup_file && strcmp(filename, param_backup_file) == 0) {
		PX4_ERR("default file can't be the same as the backup file %s", filename);
		أرجع PX4_ERROR;
	}

#جيڪڏهن_وصف_ٿيل FLASH_BASED_PARAMS
	// the default for flash-based params is always the FLASH
	(فراغ)filename;
#وإلا

	إذا (param_default_file != خالي_اشارو) {
		// we assume this is not in use by some other thread
		حرر(param_default_file);
		param_default_file = خالي_اشارو;
	}

	إذا (filename) {
		param_default_file = strdup(filename);
	}

#جيڪڏهن_پورو /* FLASH_BASED_PARAMS */

	أرجع 0;
}

ثابت حرف *param_get_default_file()
{
	أرجع param_default_file;
}

صحيح param_سيٽ_backup_file(ثابت حرف *filename)
{
	إذا (filename && param_default_file && strcmp(filename, param_default_file) == 0) {
		PX4_ERR("backup file can't be the same as the default file %s", filename);
		أرجع PX4_ERROR;
	}

	إذا (param_backup_file != خالي_اشارو) {
		// we assume this is not in use by some other thread
		حرر(param_backup_file);
		param_backup_file = خالي_اشارو;
	}

	إذا (filename) {
		param_backup_file = strdup(filename);

	} وإلا {
		param_backup_file = خالي_اشارو; // backup disabled
	}

	أرجع 0;
}

ثابت حرف *param_get_backup_file()
{
	أرجع param_backup_file;
}

ساكن صحيح param_export_اندروني(صحيح fd, param_filter_func filter);
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
			أرجع -EWOULDBLOCK;
		}
	}

	صحيح shutdown_تالو_ret = px4_shutdown_تالو();

	إذا (shutdown_تالو_ret != 0) {
		PX4_ERR("px4_shutdown_lock() failed (%i)", shutdown_تالو_ret);
	}

	صحيح res = PX4_ERROR;
	ثابت حرف *filename = param_get_default_file();

	لكل (param_t param = 0; handle_اندريون_range(param); param++) {
		إذا (params_unsaved[param]) {
			params_saving.سيٽ(param, سچ);
			params_unsaved.سيٽ(param, خطأ);
		}
	}

	إذا (filename) {
		ساكن constexpr صحيح MAX_ATTEMPTS = 3;

		لكل (صحيح attempt = 1; attempt <= MAX_ATTEMPTS; attempt++) {
			// write parameters to file
			صحيح fd = ::open(filename, O_WRONLY | O_CREAT | O_TRUNC, PX4_O_گهڻائي_666);

			إذا (fd > -1) {
				perf_begin(param_export_perf);
				res = param_export_اندروني(fd, خالي_اشارو);
				perf_پڇاڙي(param_export_perf);
				::close(fd);

				إذا (res == PX4_OK) {
					// reopen file to verify
					صحيح fd_verify = ::open(filename, O_RDONLY, PX4_O_گهڻائي_666);
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
		res = flash_param_save(خالي_اشارو);
		perf_پڇاڙي(param_export_perf);
	}

	إذا (res != PX4_OK) {
		PX4_ERR("param export failed (%d)", res);

		لكل (param_t param = 0; handle_اندريون_range(param); param++) {
			إذا (params_saving[param]) {
				params_unsaved.سيٽ(param, سچ);
			}
		}
	}

	params_saving.reset();

	إذا (res == PX4_OK) {
		// backup file
		إذا (param_backup_file) {
			صحيح fd_backup_file = ::open(param_backup_file, O_WRONLY | O_CREAT | O_TRUNC, PX4_O_گهڻائي_666);

			إذا (fd_backup_file > -1) {
				صحيح backup_export_ret = param_export_اندروني(fd_backup_file, خالي_اشارو);
				::close(fd_backup_file);

				إذا (backup_export_ret != 0) {
					PX4_ERR("backup parameter export to %s failed (%d)", param_backup_file, backup_export_ret);

				} وإلا {
					// verify export
					صحيح fd_verify = ::open(param_backup_file, O_RDONLY, PX4_O_گهڻائي_666);
					param_verify(fd_verify);
					::close(fd_verify);
				}
			}
		}
	}

	pthread_mutex_unlock(&file_mutex);

	إذا (shutdown_تالو_ret == 0) {
		px4_shutdown_unlock();
	}

	أرجع res;
}

/**
 * @return 0 on success, 1 if all params have not yet been stored, -1 if device open failed, -2 if writing parameters failed
 */
صحيح
param_ڀريو_default()
{
	صحيح res = 0;
	ثابت حرف *filename = param_get_default_file();

	إذا (!filename) {
		أرجع param_ڀريو(-1);
	}

	صحيح fd_ڀريو = ::open(filename, O_RDONLY);

	إذا (fd_ڀريو < 0) {
		/* no parameter file is OK, otherwise this is an error */
		إذا (errno != ENOENT) {
			PX4_ERR("open '%s' for reading failed", filename);
			أرجع -1;
		}

		أرجع 1;
	}

	صحيح نتيجو = param_ڀريو(fd_ڀريو);
	::close(fd_ڀريو);

	إذا (نتيجو == 1) {
		/* blank file: nothing stored yet */
		أرجع 1;
	}

	إذا (نتيجو != 0) {
		PX4_ERR("error reading parameters from '%s'", filename);
		أرجع -2;
	}

	أرجع res;
}

ساكن صحيح param_verify_callback(bson_decoder_t decoder, bson_ڳنڍ_t ڳنڍ_2)
{
	إذا (ڳنڍ_2->type == BSON_EOO) {
		أرجع 0;
	}

	// find the parameter this node represents
	param_t param = param_ڳوليو_no_notification(ڳنڍ_2->name);

	إذا (param == PARAM_INVALID) {
		PX4_ERR("verify: invalid parameter '%s'", ڳنڍ_2->name);
		أرجع -1;
	}

	// handle verifying the parameter from the node
	بدل (ڳنڍ_2->type) {
	حالة BSON_INT32: {
			إذا (param_type(param) != PARAM_TYPE_INT32) {
				PX4_ERR("verify: invalid param type %d for '%s' (BSON_INT32)", param_type(param), ڳنڍ_2->name);
				أرجع -1;
			}

			int32_t value;

			إذا (param_get(param, &value) == 0) {
				إذا (value == ڳنڍ_2->i32) {
					أرجع 1; // valid

				} وإلا {
					PX4_ERR("verify: '%s' invalid BSON value %" PRIi32 "(expected %" PRIi32 ")", ڳنڍ_2->name, ڳنڍ_2->i32, value);
				}
			}
		}
		اكسر;

	حالة BSON_DOUBLE: {
			إذا (param_type(param) != PARAM_TYPE_FLOAT) {
				PX4_ERR("verify: invalid param type %d for '%s' (BSON_DOUBLE)", param_type(param), ڳنڍ_2->name);
				أرجع -1;
			}

			عائم value;

			إذا (param_get(param, &value) == 0) {
				إذا (fabsf(value - (عائم)ڳنڍ_2->d) <= FLT_EPSILON) {
					أرجع 1; // valid

				} وإلا {
					PX4_ERR("verify: '%s' invalid BSON value %.3f (expected %.3f)", ڳنڍ_2->name, ڳنڍ_2->d, (مزدوج)value);
				}
			}
		}
		اكسر;

	افتراضي:
		PX4_ERR("verify: '%s' invalid node type %d", ڳنڍ_2->name, ڳنڍ_2->type);
	}

	أرجع -1;
}

ساكن صحيح param_verify(صحيح fd)
{
	PX4_DEBUG("param_verify");

	إذا (fd < 0) {
		أرجع -1;
	}

	إذا (lseek(fd, 0, SEEK_SET) != 0) {
		PX4_ERR("verify: seek failed");
		أرجع -1;
	}

	bson_decoder_s decoder{};

	إذا (bson_decoder_init_file(&decoder, fd, param_verify_callback) == 0) {
		صحيح نتيجو = -1;

		افعل {
			نتيجو = bson_decoder_ايندڙ(&decoder);

		} طالما (نتيجو > 0);

		إذا (نتيجو == 0) {
			إذا (decoder.ڪل_document_سائيز != decoder.ڪل_decoded_سائيز) {
				PX4_ERR("BSON document size (%" PRId32 ") doesn't match bytes decoded (%" PRId32 ")", decoder.ڪل_document_سائيز,
					decoder.ڪل_decoded_سائيز);

			} وإلا {
				أرجع 0;
			}

		} وإلا إذا (نتيجو == -ENODATA) {
			PX4_ERR("verify: no BSON data");

		} وإلا {
			PX4_ERR("verify: failed (%d)", نتيجو);
		}
	}

	أرجع -1;
}

صحيح
param_export(ثابت حرف *filename, param_filter_func filter)
{
	PX4_DEBUG("param_export");

	صحيح shutdown_تالو_ret = px4_shutdown_تالو();

	إذا (shutdown_تالو_ret != 0) {
		PX4_ERR("px4_shutdown_lock() failed (%i)", shutdown_تالو_ret);
	}

	// take the file lock
	إذا (pthread_mutex_trylock(&file_mutex) != 0) {
		PX4_ERR("param_export: file lock failed (already locked)");
		أرجع PX4_ERROR;
	}

	صحيح fd = ::open(filename, O_RDWR | O_CREAT, PX4_O_گهڻائي_666);
	صحيح نتيجو = PX4_ERROR;

	perf_begin(param_export_perf);

	إذا (fd > -1) {
		نتيجو = param_export_اندروني(fd, filter);

	} وإلا {
		نتيجو = flash_param_save(filter);
	}

	perf_پڇاڙي(param_export_perf);

	pthread_mutex_unlock(&file_mutex);

	إذا (shutdown_تالو_ret == 0) {
		px4_shutdown_unlock();
	}

	أرجع نتيجو;
}

// internal parameter export, caller is responsible for locking
ساكن صحيح param_export_اندروني(صحيح fd, param_filter_func filter)
{
	PX4_DEBUG("param_export_internal");
	ثابت خودڪار بدليو_params = user_config.containedAsBitset();

	صحيح نتيجو = -1;
	bson_encoder_s encoder{};
	uint8_t bson_بفر[256];

	إذا (lseek(fd, 0, SEEK_SET) != 0) {
		PX4_ERR("export seek failed %d", errno);
		أرجع -1;
	}

	إذا (bson_encoder_init_buf_file(&encoder, fd, &bson_بفر, حجم(bson_بفر)) != 0) {
		اذهب ٻاهريون;
	}

	لكل (param_t param = 0; handle_اندريون_range(param); param++) {
		إذا (!بدليو_params[param] || (filter && !filter(param))) {
			استمر;
		}

		ثابت param_قدر_u runtime_default_قدر = runtime_defaults.get(param);
		ثابت param_قدر_u user_config_قدر = user_config.get(param);

		// don't export default values
		بدل (param_type(param)) {
		حالة PARAM_TYPE_INT32:
			إذا (user_config_قدر.i == runtime_default_قدر.i) {
				PX4_DEBUG("skipping %s %" PRIi32 " export", param_name(param), runtime_default_قدر.i);
				استمر;
			}

			اكسر;

		حالة PARAM_TYPE_FLOAT:
			إذا (fabsf(user_config_قدر.f - runtime_default_قدر.f) <= FLT_EPSILON) {
				PX4_DEBUG("skipping %s %.3f export", param_name(param), (مزدوج)runtime_default_قدر.f);
				استمر;
			}

			اكسر;
		}

		ثابت حرف *name = param_name(param);
		ثابت size_t سائيز = param_سائيز(param);

		/* append the appropriate BSON type object */
		بدل (param_type(param)) {
		حالة PARAM_TYPE_INT32: {
				ثابت int32_t i = user_config_قدر.i;
				PX4_DEBUG("exporting: %s (%d) size: %lu val: %" PRIi32, name, param, (ڊگهو بنانشان صحيح)سائيز, i);

				إذا (bson_encoder_append_int32(&encoder, name, i) != 0) {
					PX4_ERR("BSON append failed for '%s'", name);
					اذهب ٻاهريون;
				}
			}
			اكسر;

		حالة PARAM_TYPE_FLOAT: {
				ثابت مزدوج f = (مزدوج)user_config_قدر.f;
				PX4_DEBUG("exporting: %s (%d) size: %lu val: %.3f", name, param, (ڊگهو بنانشان صحيح)سائيز, (مزدوج)f);

				إذا (bson_encoder_append_double(&encoder, name, f) != 0) {
					PX4_ERR("BSON append failed for '%s'", name);
					اذهب ٻاهريون;
				}
			}
			اكسر;

		افتراضي:
			PX4_ERR("%s unrecognized parameter type %d, skipping export", name, param_type(param));
		}
	}

	نتيجو = 0;

ٻاهريون:

	إذا (نتيجو == 0) {
		إذا (bson_encoder_fini(&encoder) != PX4_OK) {
			PX4_ERR("BSON encoder finalize failed");
			نتيجو = -1;
		}
	}

	أرجع نتيجو;
}

#إذا defined(FLASH_BASED_PARAMS)
// A file is never the flash backend's store, so what an import changes is unsaved until a delta carries it.
// As on the file backend, the import itself does not save.
ساكن constexpr منطقي file_import_is_saved = خطأ;
#وإلا
ساكن constexpr منطقي file_import_is_saved = سچ;
#جيڪڏهن_پورو

ساكن صحيح
param_import_callback(bson_decoder_t decoder, bson_ڳنڍ_t ڳنڍ_2)
{
	/*
	 * EOO means the end of the parameter object. (Currently not supporting
	 * nested BSON objects).
	 */
	إذا (ڳنڍ_2->type == BSON_EOO) {
		PX4_DEBUG("end of parameters");
		أرجع 0;
	}

	// A tombstone has no value. The decoder leaves the previous node's in the union and the translations
	// read it without a type check.
	ثابت منطقي tombstone = ڳنڍ_2->type == BSON_nullptr || ڳنڍ_2->type == BSON_UNDEFINED;

	// if we do param_set() directly in the translation, set PARAM_SKIP_IMPORT as return value and return here
	إذا (!tombstone && param_modify_on_import(ڳنڍ_2) == param_modify_on_import_ret::PARAM_SKIP_IMPORT) {
		أرجع 1;
	}

	// Find the parameter this node represents.  If we don't know it, ignore the node.
	param_t param = param_ڳوليو_no_notification(ڳنڍ_2->name);

	إذا (param == PARAM_INVALID) {
		PX4_WARN("ignoring unrecognised parameter '%s'", ڳنڍ_2->name);
		أرجع 1;
	}

	// Handle setting the parameter from the node
	بدل (ڳنڍ_2->type) {
	حالة BSON_nullptr:
	حالة BSON_UNDEFINED:
		param_reset_اندروني(param, سچ, خطأ);
		أرجع 1;

	حالة BSON_INT32: {
			إذا (param_type(param) == PARAM_TYPE_INT32) {
				int32_t i = ڳنڍ_2->i32;
				param_سيٽ_اندروني(param, &i, file_import_is_saved, سچ, سچ, خطأ);
				PX4_DEBUG("Imported %s with value %" PRIi32, param_name(param), i);

			} وإلا {
				PX4_WARN("unexpected type for %s", ڳنڍ_2->name);
			}
		}
		اكسر;

	حالة BSON_DOUBLE: {
			إذا (param_type(param) == PARAM_TYPE_FLOAT) {
				عائم f = ڳنڍ_2->d;
				param_سيٽ_اندروني(param, &f, file_import_is_saved, سچ, سچ, خطأ);
				PX4_DEBUG("Imported %s with value %f", param_name(param), (مزدوج)f);

			} وإلا {
				PX4_WARN("unexpected type for %s", ڳنڍ_2->name);
			}
		}
		اكسر;

	افتراضي:
		PX4_ERR("import: unrecognised node type for '%s'", ڳنڍ_2->name);
	}

	// don't return zero, that means EOF
	أرجع 1;
}

ساكن صحيح
param_import_اندروني(صحيح fd)
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
		أرجع 1;
	}

	int32_t doc_سائيز = 0;
	ssize_t len = اقرأ(fd, &doc_سائيز, حجم(doc_سائيز));

	إذا (len == 0 || (len == (ssize_t)حجم(doc_سائيز) && (doc_سائيز == 0 || doc_سائيز == -1))) {
		أرجع 1;
	}

	إذا (lseek(fd, 0, SEEK_SET) != 0) {
		PX4_ERR("import lseek failed (%d)", errno);
		أرجع -1;
	}

	ساكن constexpr صحيح MAX_ATTEMPTS = 3;

	لكل (صحيح attempt = 1; attempt <= MAX_ATTEMPTS; attempt++) {
		bson_decoder_s decoder{};

		إذا (bson_decoder_init_file(&decoder, fd, param_import_callback) == 0) {
			صحيح نتيجو = -1;

			افعل {
				نتيجو = bson_decoder_ايندڙ(&decoder);

			} طالما (نتيجو > 0);

			إذا (نتيجو == 0) {
				إذا (decoder.ڪل_document_سائيز == decoder.ڪل_decoded_سائيز) {
					PX4_INFO("BSON document size %" PRId32 " bytes, decoded %" PRId32 " bytes (INT32:%" PRIu16 ", FLOAT:%" PRIu16 ")",
						 decoder.ڪل_document_سائيز, decoder.ڪل_decoded_سائيز,
						 decoder.ڳڻپ_ڳنڍ_int32, decoder.ڳڻپ_ڳنڍ_double);

					أرجع 0;

				} وإلا {
					PX4_ERR("BSON document size (%" PRId32 ") doesn't match bytes decoded (%" PRId32 ")",
						decoder.ڪل_document_سائيز, decoder.ڪل_decoded_سائيز);
				}

			} وإلا إذا (نتيجو == -ENODATA) {
				// silently retry as a precaution unless this is our last attempt
				إذا (attempt == MAX_ATTEMPTS) {
					PX4_DEBUG("BSON: no data");
					أرجع 0;
				}

			} وإلا {
				PX4_ERR("param import failed (%d) attempt %d", نتيجو, attempt);
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

	أرجع -1;
}

#إذا defined(FLASH_BASED_PARAMS)
ساكن صحيح flash_backend_locked(صحيح (*op)())
{
	/* Same order as param_save_default: mutex then shutdown lock. Autosave
	 * uses a trylock, so a compact holds it off the flash programming. */
	pthread_mutex_lock(&file_mutex);

	صحيح shutdown_تالو_ret = px4_shutdown_تالو();

	إذا (shutdown_تالو_ret != 0) {
		PX4_ERR("px4_shutdown_lock() failed (%i)", shutdown_تالو_ret);
	}

	صحيح نتيجو = op();

	pthread_mutex_unlock(&file_mutex);

	إذا (shutdown_تالو_ret == 0) {
		px4_shutdown_unlock();
	}

	أرجع نتيجو;
}

ساكن صحيح flash_ڀريو_locked()
{
	/* Reset under the lock so a pending autosave cannot append tombstones
	 * of the just-cleared RAM before import/compact run. */
	param_reset_all_اندروني(خطأ);
	صحيح نتيجو = flash_param_import();

	إذا (نتيجو == 0 || نتيجو == 1) {
		params_unsaved.reset();
	}

	أرجع نتيجو;
}
#جيڪڏهن_پورو

صحيح
param_import(صحيح fd)
{
	إذا (fd < 0) {
#إذا defined(FLASH_BASED_PARAMS)
		أرجع flash_backend_locked(flash_param_import);
#وإلا
		أرجع flash_param_import();
#جيڪڏهن_پورو
	}

	أرجع param_import_اندروني(fd);
}

صحيح
param_ڀريو(صحيح fd)
{
	إذا (fd < 0) {
#إذا defined(FLASH_BASED_PARAMS)
		أرجع flash_backend_locked(flash_ڀريو_locked);
#وإلا
		أرجع PX4_ERROR;
#جيڪڏهن_پورو
	}

	param_reset_all_اندروني(خطأ);
	صحيح نتيجو = param_import_اندروني(fd);

#إذا !defined(FLASH_BASED_PARAMS)

	إذا (نتيجو >= 0) {
		params_unsaved.reset();
	}

#جيڪڏهن_پورو

	أرجع نتيجو;
}

فراغ
param_foreach(فراغ (*func)(فراغ *arg, param_t param), فراغ *arg, منطقي only_بدليو, منطقي only_استعمال_ٿيل)
{
	param_t	param;

	لكل (param = 0; handle_اندريون_range(param); param++) {

		/* if requested, skip unchanged values */
		إذا (only_بدليو && (!user_config.شامل_آهي(param))) {
			استمر;
		}

		إذا (only_استعمال_ٿيل && !param_استعمال_ٿيل(param)) {
			استمر;
		}

		func(arg, param);
	}
}

uint32_t param_hash_check()
{
	uint32_t param_hash = 0;

	/* compute the CRC32 over all string param names and 4 byte values */
	لكل (param_t param = 0; handle_اندريون_range(param); param++) {
		إذا (!param_استعمال_ٿيل(param) || param_is_volatile(param)) {
			استمر;
		}

		ثابت حرف *name = param_name(param);
		خودڪار value = user_config.get(param).i;
		ثابت فراغ *val = (فراغ *)&value;
		param_hash = crc32part((ثابت uint8_t *)name, لڙي_ڊيگهه(name), param_hash);
		param_hash = crc32part((ثابت uint8_t *)val, param_سائيز(param), param_hash);
	}

	أرجع param_hash;
}

فراغ param_print_حالت()
{
	PX4_INFO("summary: %d/%d (used/total)", param_ڳڻپ_استعمال_ٿيل(), param_ڳڻپ());

#جيڪڏهن_وصف_نه_ٿيل FLASH_BASED_PARAMS
	ثابت حرف *filename = param_get_default_file();

	إذا (filename != خالي_اشارو) {
		PX4_INFO("file: %s", param_get_default_file());
	}

	إذا (param_backup_file) {
		PX4_INFO("backup file: %s", param_backup_file);
	}

#جيڪڏهن_پورو /* FLASH_BASED_PARAMS */

	PX4_INFO("storage array: %d/%d elements (%zu bytes total)",
		 user_config.سائيز(), firmware_defaults.سائيز(), (size_t)user_config.byte_سائيز());


	PX4_INFO("storage array (custom defaults): %d/%d elements (%zu bytes total)",
		 runtime_defaults.سائيز(), firmware_defaults.سائيز(), (size_t)runtime_defaults.byte_سائيز());

	إذا (autosave_instance) {
		PX4_INFO("auto save: %s", autosave_instance->enabled() ? "on" : "off");

		hrt_abstime آخري_autosave = autosave_instance->آخري_Autosave();

		إذا (آخري_autosave > 0) {
			PX4_INFO("last auto save: %.3f seconds ago", hrt_elapsed_وقت(&آخري_autosave) * 1e-6);
		}
	}

	perf_print_counter(param_export_perf);
	perf_print_counter(param_ڳوليو_perf);
	perf_print_counter(param_get_perf);
	perf_print_counter(param_سيٽ_perf);

#إذا defined(CONFIG_PARAM_PRIMARY)
	بنية param_primary_counters counts;
	param_primary_get_counters(&counts);
	PX4_INFO("set value requests received: %" PRIu32 ", set value responses sent: %" PRIu32,
		 counts.سيٽ_قدر_request_received, counts.سيٽ_قدر_response_موڪليل);
	PX4_INFO("set value requests sent: %" PRIu32 ", set value responses received: %" PRIu32,
		 counts.سيٽ_قدر_request_موڪليل, counts.سيٽ_قدر_response_received);
	PX4_INFO("resets sent: %" PRIu32 ", set used requests received: %" PRIu32,
		 counts.reset_موڪليل, counts.سيٽ_استعمال_ٿيل_received);
#جيڪڏهن_پورو

#إذا defined(CONFIG_PARAM_REMOTE)
	بنية param_remote_counters counts;
	param_remote_get_counters(&counts);
	PX4_INFO("set value requests received: %" PRIu32 ", set value responses sent: %" PRIu32,
		 counts.سيٽ_قدر_request_received, counts.سيٽ_قدر_response_موڪليل);
	PX4_INFO("set value requests sent: %" PRIu32 ", set value responses received: %" PRIu32,
		 counts.سيٽ_قدر_request_موڪليل, counts.سيٽ_قدر_response_received);
	PX4_INFO("resets received: %" PRIu32 ", set used requests sent: %" PRIu32,
		 counts.reset_received, counts.سيٽ_استعمال_ٿيل_موڪليل);
#جيڪڏهن_پورو

}
