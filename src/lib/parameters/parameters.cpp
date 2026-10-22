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

#تعریف_کړه PARAM_IMPLEMENTATION
#ضمن "param.h"
#ضمن "param_translation.h"
#ضمن <parameters/px4_parameters.hpp>
#ضمن <lib/tinybson/tinybson.h>

#إذا defined(__PX4_NUTTX)
#ضمن <nuttx/crc32.h>
#وإلا
#ضمن <crc32.h>
#د_که_پای
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

کارول نطاق وخت_literals;

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
#د_که_پای

#إذا defined(FLASH_BASED_PARAMS)
#ضمن "flashparams/flashparams.h"
#وإلا
مضمن ساكن صحيح flash_param_save(param_filter_func filter) { أرجع -1; }
مضمن ساكن صحيح flash_param_import() { أرجع -1; }
#د_که_پای

ساكن حرف *param_default_file = تش_اشاره;
ساكن حرف *param_backup_file = تش_اشاره;

#ضمن "autosave.h"
ساكن ParamAutosave *autosave_instance {تش_اشاره};

ساكن px4::AtomicBitset<param_info_شمېر> params_فعال;  // params found
ساكن px4::AtomicBitset<param_info_شمېر> params_unsaved;

// Params the save in progress is writing. They are moved out of params_unsaved
// before their values are read, so a param_set that lands while the save runs
// marks its param again and reaches the next save. Flash-param saves write only
// unsaved params, so nothing else would ever write that change.
ساكن px4::AtomicBitset<param_info_شمېر> params_saving;

ساكن ConstLayer firmware_defaults;
ساكن DynamicSparseLayer runtime_defaults{&firmware_defaults};
DynamicSparseLayer user_config{&runtime_defaults};

/** parameter update topic handle */
#إذا not defined(CONFIG_PARAM_REMOTE)
ساكن orb_advert_t param_topic = تش_اشاره;
ساكن بینښه صحيح param_instance = 0;
#د_که_پای

ساكن perf_counter_t param_export_perf;
ساكن perf_counter_t param_پیدا_کړه_perf;
ساكن perf_counter_t param_get_perf;
ساكن perf_counter_t param_سېټ_perf;

ساكن pthread_mutex_t file_mutex  =
	PTHREAD_MUTEX_INITIALIZER; ///< this protects against concurrent param saves (file or flash access).

// Support for remote parameter node
#إذا defined(CONFIG_PARAM_PRIMARY)
# include "parameters_primary.h"
#د_که_پای // CONFIG_PARAM_PRIMARY
#إذا defined(CONFIG_PARAM_REMOTE)
# include "parameters_remote.h"
#د_که_پای // CONFIG_PARAM_REMOTE

فراغ
param_init()
{
#که_تعریف_شوی __PX4_POSIX
	// Set up the recursive param mutex before any thread can access parameters.
	AtomicTransaction::initialize();
#د_که_پای

	param_export_perf = perf_alloc(PC_ELAPSED, "param: export");
	param_پیدا_کړه_perf = perf_alloc(PC_شمېر, "param: find");
	param_get_perf = perf_alloc(PC_شمېر, "param: get");
	param_سېټ_perf = perf_alloc(PC_ELAPSED, "param: set");

#إذا defined(__PX4_NUTTX) && !defined(CONFIG_BUILD_FLAT)
	px4_ثبتوونکی_boardct_ioctl(_PARAMIOCBASE, param_ioctl);
#د_که_پای

#إذا defined(CONFIG_PARAM_PRIMARY)
	param_primary_init();
#د_که_پای // CONFIG_PARAM_PRIMARY

#إذا defined(CONFIG_PARAM_REMOTE)
	param_remote_init();
#د_که_پای // CONFIG_PARAM_REMOTE

#إذا not defined(CONFIG_PARAM_REMOTE)
	autosave_instance = جديد ParamAutosave();
#د_که_پای
}


فراغ
param_notify_changes()
{
// Don't send if this is a remote node. Only the primary
// sends out update notices
#إذا not defined(CONFIG_PARAM_REMOTE)
	parameter_update_s pup {};
	pup.instance = param_instance++;
	pup.get_شمېر_2 = perf_event_شمېر(param_get_perf);
	pup.سېټ_شمېر = perf_event_شمېر(param_سېټ_perf);
	pup.پیدا_کړه_شمېر = perf_event_شمېر(param_پیدا_کړه_perf);
	pup.export_شمېر = perf_event_شمېر(param_export_perf);
	pup.فعال_2 = params_فعال.شمېر();
	pup.بدل_شوی = user_config.اندازه();
	pup.custom_default = runtime_defaults.اندازه();
	pup.timestamp = hrt_absolute_وخت();

	إذا (param_topic == تش_اشاره) {
		param_topic = orb_advertise(ORB_ID(parameter_update), &pup);

	} وإلا {
		orb_publish(ORB_ID(parameter_update), param_topic, &pup);
	}

#د_که_پای
}

ساكن param_t param_پیدا_کړه_دننی(ثابت حرف *name, منطقي notification)
{
	perf_شمېر(param_پیدا_کړه_perf);

	param_t منځ;
	param_t front = 0;
	param_t وروستی = param_info_شمېر;

	/* perform a binary search of the known parameters */

	طالما (front <= وروستی) {
		منځ = front + (وروستی - front) / 2;
		صحيح ret = strcmp(name, param_name(منځ));

		إذا (ret == 0) {
			إذا (notification) {
				param_سېټ_کارول_شوی(منځ);
			}

			أرجع منځ;

		} وإلا إذا (منځ == front) {
			/* An end point has been hit, but there has been no match */
			اكسر;

		} وإلا إذا (ret < 0) {
			وروستی = منځ;

		} وإلا {
			front = منځ;
		}
	}

	/* not found */
	أرجع PARAM_INVALID;
}

param_t param_پیدا_کړه(ثابت حرف *name)
{
	أرجع param_پیدا_کړه_دننی(name, ریښتیا);
}

param_t param_پیدا_کړه_no_notification(ثابت حرف *name)
{
	أرجع param_پیدا_کړه_دننی(name, خطأ);
}

بینښه param_شمېر_کارول_شوی()
{
	أرجع params_فعال.شمېر();
}

param_t param_for_کارول_شوی_شاخص(بینښه شاخص)
{
	// walk all params and count used params
	إذا (شاخص < param_info_شمېر) {
		بینښه کارول_شوی_شمېر = 0;

		لكل (صحيح i = 0; i < params_فعال.اندازه(); i++) {
			إذا (params_فعال[i]) {
				// we found the right used count,
				//  return the param value
				إذا (شاخص == کارول_شوی_شمېر) {
					أرجع static_cast<param_t>(i);
				}

				کارول_شوی_شمېر++;
			}
		}
	}

	أرجع PARAM_INVALID;
}

صحيح param_get_کارول_شوی_شاخص(param_t param)
{
	/* this tests for out of bounds and does a constant time lookup */
	إذا (!param_کارول_شوی(param)) {
		أرجع -1;
	}

	/* walk all params and count, now knowing that it has a valid index */
	صحيح کارول_شوی_شمېر = 0;

	لكل (صحيح i = 0; i < params_فعال.اندازه(); i++) {
		إذا (params_فعال[i]) {

			إذا (param == i) {
				أرجع کارول_شوی_شمېر;
			}

			کارول_شوی_شمېر++;
		}
	}

	أرجع -1;
}

منطقي
param_ارزښت_unsaved(param_t param)
{
	أرجع handle_دننه_range(param) ? (params_unsaved[param] || params_saving[param]) : خطأ;
}

صحيح
param_get(param_t param, فراغ *val)
{
	perf_شمېر(param_get_perf);

	إذا (!handle_دننه_range(param)) {
		PX4_ERR("get: param %" PRId16 " invalid", param);
		أرجع PX4_ERROR;
	}

	إذا (!params_فعال[param]) {
		PX4_DEBUG("get: param %" PRId16 " (%s) not active", param, param_name(param));
	}

	صحيح پایله = PX4_ERROR;

	إذا (val) {

		خپلکاری retrieve_ارزښت = user_config.get(param);

		بدل (param_type(param)) {
		حالة PARAM_TYPE_INT32:
			حافظه_نقل(val, &retrieve_ارزښت.i, حجم(retrieve_ارزښت.i));
			أرجع PX4_OK;

		حالة PARAM_TYPE_FLOAT:
			حافظه_نقل(val, &retrieve_ارزښت.f, حجم(retrieve_ارزښت.f));
			أرجع PX4_OK;
		}
	}

	أرجع پایله;
}

صحيح
param_get_default_ارزښت_دننی(param_t param, فراغ *default_val)
{
	إذا (!handle_دننه_range(param)) {
		PX4_ERR("get default value: param %d invalid", param);
		أرجع PX4_ERROR;
	}

	إذا (default_val) {
		بدل (param_type(param)) {
		حالة PARAM_TYPE_INT32: {
				int32_t val = runtime_defaults.get(param).i;
				حافظه_نقل(default_val, &val, حجم(val));
				أرجع PX4_OK;
			}

		حالة PARAM_TYPE_FLOAT: {
				عائم val = runtime_defaults.get(param).f;
				حافظه_نقل(default_val, &val, حجم(val));
				أرجع PX4_OK;
			}
		}
	}

	أرجع PX4_ERROR;
}

صحيح
param_get_default_ارزښت(param_t param, فراغ *default_val)
{
	إذا (!handle_دننه_range(param)) {
		أرجع PX4_ERROR;
	}

	صحيح ret = param_get_default_ارزښت_دننی(param, default_val);
	أرجع ret;
}

منطقي param_ارزښت_is_default(param_t param)
{
	إذا (!handle_دننه_range(param)) {
		أرجع ریښتیا;
	}

	إذا (!user_config.لري(param)) {
		// if user config does not contain it, consider it default.
		أرجع ریښتیا;

	} وإلا {
		// compare with default value
		ثابت param_ارزښت_u user_config_ارزښت = user_config.get(param);
		ثابت param_ارزښت_u runtime_default_ارزښت = runtime_defaults.get(param);

		بدل (param_type(param)) {
		حالة PARAM_TYPE_INT32: {
				أرجع user_config_ارزښت.i == runtime_default_ارزښت.i;
			}

		حالة PARAM_TYPE_FLOAT: {
				أرجع fabsf(user_config_ارزښت.f - runtime_default_ارزښت.f) < FLT_EPSILON;
			}
		}
	}

	أرجع ریښتیا;
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
param_سېټ_دننی(param_t param, ثابت فراغ *val, منطقي mark_saved, منطقي notify_changes, منطقي update_remote = ریښتیا,
		   منطقي autosave = ریښتیا)
{
	إذا (!handle_دننه_range(param)) {
		PX4_ERR("set invalid param %d", param);
		أرجع PX4_ERROR;
	}

	إذا (val == تش_اشاره) {
		PX4_ERR("set invalid value");
		أرجع PX4_ERROR;
	}

	إذا (param_is_readonly(param)) {
		PX4_WARN("param %s is read-only", param_name(param));
		أرجع PX4_ERROR;
	}

	صحيح پایله = -1;
	منطقي param_بدل_شوی = خطأ;
	perf_begin(param_سېټ_perf);

	ثابت param_ارزښت_u user_config_ارزښت = user_config.get(param);
	param_ارزښت_u new_ارزښت{};

	بدل (param_type(param)) {
	حالة PARAM_TYPE_INT32:
		حافظه_نقل(&new_ارزښت.i, val, حجم(new_ارزښت.i));
		param_بدل_شوی = user_config_ارزښت.i != new_ارزښت.i;
		اكسر;

	حالة PARAM_TYPE_FLOAT:
		حافظه_نقل(&new_ارزښت.f, val, حجم(new_ارزښت.f));
		param_بدل_شوی = fabsf(user_config_ارزښت.f - new_ارزښت.f) > FLT_EPSILON;
		اكسر;

	افتراضي: {
			PX4_ERR("param_set invalid param type for %s", param_name(param));
			اكسر;
		}
	}

	إذا (user_config.store(param, new_ارزښت)) {
		إذا (param_بدل_شوی) {
			params_unsaved.سېټ(param, !mark_saved);
		}

		پایله = PX4_OK;

	} وإلا {
		PX4_ERR("param_set failed to store param %s", param_name(param));
		پایله = PX4_ERROR;
	}

	إذا ((پایله == PX4_OK) && param_بدل_شوی && !mark_saved && autosave) { // this is false when importing parameters
		param_autosave();
	}

	// If this is the parameter server, make sure that the remote is updated
#إذا defined(CONFIG_PARAM_PRIMARY)

	إذا (param_بدل_شوی && update_remote) {
		param_primary_سېټ_ارزښت(param, val);
	}

#د_که_پای

	// If this is the parameter remote, make sure that the primary is updated
#إذا defined(CONFIG_PARAM_REMOTE)

	إذا (param_بدل_شوی && update_remote) {
		param_remote_سېټ_ارزښت(param, val);
	}

#د_که_پای

	perf_پای_ټکی(param_سېټ_perf);

	/*
	 * If we set something, now that we have unlocked, go ahead and advertise that
	 * a thing has been set.
	 */
	إذا ((پایله == PX4_OK) && param_بدل_شوی && notify_changes) {
		param_notify_changes();
	}

	أرجع پایله;
}

#إذا defined(FLASH_BASED_PARAMS)
صحيح param_سېټ_external(param_t param, ثابت فراغ *val, منطقي mark_saved, منطقي notify_changes)
{
	أرجع param_سېټ_دننی(param, val, mark_saved, notify_changes);
}

فراغ param_get_external(param_t param, فراغ *val)
{
	param_get(param, val);
}
#د_که_پای

صحيح param_سېټ(param_t param, ثابت فراغ *val)
{
	أرجع param_سېټ_دننی(param, val, خطأ, ریښتیا);
}

صحيح param_سېټ_no_notification(param_t param, ثابت فراغ *val)
{
	أرجع param_سېټ_دننی(param, val, خطأ, خطأ);
}

صحيح param_سېټ_no_remote_update(param_t param, ثابت فراغ *val, منطقي notify)
{
	أرجع param_سېټ_دننی(param, val, خطأ, notify, خطأ);
}

منطقي param_کارول_شوی(param_t param)
{
	إذا (handle_دننه_range(param)) {
		أرجع params_فعال[param];
	}

	أرجع خطأ;
}

فراغ param_سېټ_کارول_شوی(param_t param)
{
	إذا (handle_دننه_range(param)) {
#إذا defined(CONFIG_PARAM_REMOTE)

		إذا (!param_کارول_شوی(param)) {
			param_remote_سېټ_کارول_شوی(param);
		}

#د_که_پای

		params_فعال.سېټ(param, ریښتیا);
	}
}

صحيح param_سېټ_default_ارزښت(param_t param, ثابت فراغ *val)
{
	إذا (!handle_دننه_range(param)) {
		PX4_ERR("set default value invalid param %d", param);
		أرجع PX4_ERROR;
	}

	إذا (val == تش_اشاره) {
		PX4_ERR("set default value invalid value");
		أرجع PX4_ERROR;
	}

	إذا (param_is_readonly(param)) {
		PX4_WARN("param %s is read-only", param_name(param));
		أرجع PX4_ERROR;
	}

	صحيح پایله = PX4_ERROR;


	// check if param being set to default value
	منطقي setting_منزل_static_default = خطأ;
	ثابت param_ارزښت_u firmware_default_ارزښت = firmware_defaults.get(param);

	بدل (param_type(param)) {
	حالة PARAM_TYPE_INT32: {
			int32_t new_ارزښت;
			حافظه_نقل(&new_ارزښت, val, حجم(new_ارزښت));
			setting_منزل_static_default = firmware_default_ارزښت.i == new_ارزښت;
			اكسر;
		}

	حالة PARAM_TYPE_FLOAT: {
			عائم new_ارزښت;
			حافظه_نقل(&new_ارزښت, val, حجم(new_ارزښت));
			setting_منزل_static_default = fabsf(firmware_default_ارزښت.f - new_ارزښت) <= FLT_EPSILON;
			اكسر;
		}
	}

	إذا (setting_منزل_static_default) {
		runtime_defaults.reset(param);

		پایله = PX4_OK;

	} وإلا {
		param_ارزښت_u new_ارزښت{};

		بدل (param_type(param)) {
		حالة PARAM_TYPE_INT32: {
				حافظه_نقل(&new_ارزښت.i, val, حجم(new_ارزښت.i));
				اكسر;
			}

		حالة PARAM_TYPE_FLOAT: {
				حافظه_نقل(&new_ارزښت.f, val, حجم(new_ارزښت.f));
				اكسر;
			}

		افتراضي:
			اكسر;
		}

		إذا (runtime_defaults.store(param, new_ارزښت)) {
			user_config.refresh(param);
			پایله = PX4_OK;

		} وإلا {
			پایله = PX4_ERROR;
		}
	}


	إذا ((پایله == PX4_OK) && param_کارول_شوی(param)) {
		// send notification if param is already in use
		param_notify_changes();
	}

	أرجع پایله;
}

ساكن صحيح param_reset_دننی(param_t param, منطقي notify = ریښتیا, منطقي autosave = ریښتیا)
{
#إذا defined(CONFIG_PARAM_REMOTE)
	// Remote doesn't support reset
	أرجع خطأ;
#د_که_پای

	إذا (param_is_readonly(param)) {
		أرجع 0;  // silently skip — param_reset_all loops over all params
	}

	منطقي param_وموندل = user_config.لري(param);

	إذا (handle_دننه_range(param)) {
		user_config.reset(param);
	}

	إذا (param_وموندل) {
		/* Persist the reset. Flash-param saves append only unsaved params,
		 * so a reset has to show up as a tombstone or replay keeps the
		 * previous override. */
		params_unsaved.سېټ(param, ریښتیا);
	}

	إذا (autosave) {
		param_autosave();
	}

	إذا (param_وموندل && notify) {
		param_notify_changes();
	}

#إذا defined(CONFIG_PARAM_PRIMARY)
	param_primary_reset(param);
#د_که_پای

	أرجع param_وموندل;
}

صحيح param_reset(param_t param) { أرجع param_reset_دننی(param, ریښتیا); }
صحيح param_reset_no_notification(param_t param) { أرجع param_reset_دننی(param, خطأ); }

ساكن فراغ
param_reset_all_دننی(منطقي auto_save)
{
#إذا defined(CONFIG_PARAM_REMOTE)
	// Remote doesn't support reset
	أرجع;
#د_که_پای

	لكل (param_t param = 0; handle_دننه_range(param); param++) {
		param_reset_دننی(param, خطأ, خطأ);
	}

	إذا (auto_save) {
		param_autosave();
	}

#إذا defined(CONFIG_PARAM_PRIMARY)
	param_primary_reset_all();
#د_که_پای

	param_notify_changes();
}

فراغ
param_reset_all()
{
	param_reset_all_دننی(ریښتیا);
}

فراغ
param_reset_excludes(ثابت حرف *excludes[], صحيح عدد_excludes)
{
	لكل (param_t param = 0; handle_دننه_range(param); param++) {
		ثابت حرف *name = param_name(param);
		منطقي exclude = خطأ;

		لكل (صحيح شاخص = 0; شاخص < عدد_excludes; شاخص ++) {
			صحيح len = مزي_اوږدوالی(excludes[شاخص]);

			إذا ((excludes[شاخص][len - 1] == '*'
			     && مزي_پرتله(name, excludes[شاخص], len - 1) == 0)
			    || strcmp(name, excludes[شاخص]) == 0) {

				exclude = ریښتیا;
				اكسر;
			}
		}

		إذا (!exclude) {
			param_reset(param);
		}
	}
}

فراغ
param_reset_specific(ثابت حرف *resets[], صحيح عدد_resets)
{
	لكل (param_t param = 0; handle_دننه_range(param); param++) {
		ثابت حرف *name = param_name(param);
		منطقي reset = خطأ;

		لكل (صحيح شاخص = 0; شاخص < عدد_resets; شاخص++) {
			صحيح len = مزي_اوږدوالی(resets[شاخص]);

			إذا ((resets[شاخص][len - 1] == '*'
			     && مزي_پرتله(name, resets[شاخص], len - 1) == 0)
			    || strcmp(name, resets[شاخص]) == 0) {

				reset = ریښتیا;
				اكسر;
			}
		}

		إذا (reset) {
			param_reset(param);
		}
	}
}

صحيح
param_سېټ_default_file(ثابت حرف *filename)
{
	إذا (filename && param_backup_file && strcmp(filename, param_backup_file) == 0) {
		PX4_ERR("default file can't be the same as the backup file %s", filename);
		أرجع PX4_ERROR;
	}

#که_تعریف_شوی FLASH_BASED_PARAMS
	// the default for flash-based params is always the FLASH
	(فراغ)filename;
#وإلا

	إذا (param_default_file != تش_اشاره) {
		// we assume this is not in use by some other thread
		حرر(param_default_file);
		param_default_file = تش_اشاره;
	}

	إذا (filename) {
		param_default_file = strdup(filename);
	}

#د_که_پای /* FLASH_BASED_PARAMS */

	أرجع 0;
}

ثابت حرف *param_get_default_file()
{
	أرجع param_default_file;
}

صحيح param_سېټ_backup_file(ثابت حرف *filename)
{
	إذا (filename && param_default_file && strcmp(filename, param_default_file) == 0) {
		PX4_ERR("backup file can't be the same as the default file %s", filename);
		أرجع PX4_ERROR;
	}

	إذا (param_backup_file != تش_اشاره) {
		// we assume this is not in use by some other thread
		حرر(param_backup_file);
		param_backup_file = تش_اشاره;
	}

	إذا (filename) {
		param_backup_file = strdup(filename);

	} وإلا {
		param_backup_file = تش_اشاره; // backup disabled
	}

	أرجع 0;
}

ثابت حرف *param_get_backup_file()
{
	أرجع param_backup_file;
}

ساكن صحيح param_export_دننی(صحيح fd, param_filter_func filter);
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

	صحيح shutdown_قلف_ret = px4_shutdown_قلف();

	إذا (shutdown_قلف_ret != 0) {
		PX4_ERR("px4_shutdown_lock() failed (%i)", shutdown_قلف_ret);
	}

	صحيح res = PX4_ERROR;
	ثابت حرف *filename = param_get_default_file();

	لكل (param_t param = 0; handle_دننه_range(param); param++) {
		إذا (params_unsaved[param]) {
			params_saving.سېټ(param, ریښتیا);
			params_unsaved.سېټ(param, خطأ);
		}
	}

	إذا (filename) {
		ساكن constexpr صحيح MAX_ATTEMPTS = 3;

		لكل (صحيح attempt = 1; attempt <= MAX_ATTEMPTS; attempt++) {
			// write parameters to file
			صحيح fd = ::open(filename, O_WRONLY | O_CREAT | O_TRUNC, PX4_O_ډېرپېښ_666);

			إذا (fd > -1) {
				perf_begin(param_export_perf);
				res = param_export_دننی(fd, تش_اشاره);
				perf_پای_ټکی(param_export_perf);
				::close(fd);

				إذا (res == PX4_OK) {
					// reopen file to verify
					صحيح fd_verify = ::open(filename, O_RDONLY, PX4_O_ډېرپېښ_666);
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
		res = flash_param_save(تش_اشاره);
		perf_پای_ټکی(param_export_perf);
	}

	إذا (res != PX4_OK) {
		PX4_ERR("param export failed (%d)", res);

		لكل (param_t param = 0; handle_دننه_range(param); param++) {
			إذا (params_saving[param]) {
				params_unsaved.سېټ(param, ریښتیا);
			}
		}
	}

	params_saving.reset();

	إذا (res == PX4_OK) {
		// backup file
		إذا (param_backup_file) {
			صحيح fd_backup_file = ::open(param_backup_file, O_WRONLY | O_CREAT | O_TRUNC, PX4_O_ډېرپېښ_666);

			إذا (fd_backup_file > -1) {
				صحيح backup_export_ret = param_export_دننی(fd_backup_file, تش_اشاره);
				::close(fd_backup_file);

				إذا (backup_export_ret != 0) {
					PX4_ERR("backup parameter export to %s failed (%d)", param_backup_file, backup_export_ret);

				} وإلا {
					// verify export
					صحيح fd_verify = ::open(param_backup_file, O_RDONLY, PX4_O_ډېرپېښ_666);
					param_verify(fd_verify);
					::close(fd_verify);
				}
			}
		}
	}

	pthread_mutex_unlock(&file_mutex);

	إذا (shutdown_قلف_ret == 0) {
		px4_shutdown_unlock();
	}

	أرجع res;
}

/**
 * @return 0 on success, 1 if all params have not yet been stored, -1 if device open failed, -2 if writing parameters failed
 */
صحيح
param_بارول_default()
{
	صحيح res = 0;
	ثابت حرف *filename = param_get_default_file();

	إذا (!filename) {
		أرجع param_بارول(-1);
	}

	صحيح fd_بارول = ::open(filename, O_RDONLY);

	إذا (fd_بارول < 0) {
		/* no parameter file is OK, otherwise this is an error */
		إذا (errno != ENOENT) {
			PX4_ERR("open '%s' for reading failed", filename);
			أرجع -1;
		}

		أرجع 1;
	}

	صحيح پایله = param_بارول(fd_بارول);
	::close(fd_بارول);

	إذا (پایله == 1) {
		/* blank file: nothing stored yet */
		أرجع 1;
	}

	إذا (پایله != 0) {
		PX4_ERR("error reading parameters from '%s'", filename);
		أرجع -2;
	}

	أرجع res;
}

ساكن صحيح param_verify_callback(bson_decoder_t decoder, bson_غوټه_t غوټه_2)
{
	إذا (غوټه_2->type == BSON_EOO) {
		أرجع 0;
	}

	// find the parameter this node represents
	param_t param = param_پیدا_کړه_no_notification(غوټه_2->name);

	إذا (param == PARAM_INVALID) {
		PX4_ERR("verify: invalid parameter '%s'", غوټه_2->name);
		أرجع -1;
	}

	// handle verifying the parameter from the node
	بدل (غوټه_2->type) {
	حالة BSON_INT32: {
			إذا (param_type(param) != PARAM_TYPE_INT32) {
				PX4_ERR("verify: invalid param type %d for '%s' (BSON_INT32)", param_type(param), غوټه_2->name);
				أرجع -1;
			}

			int32_t value;

			إذا (param_get(param, &value) == 0) {
				إذا (value == غوټه_2->i32) {
					أرجع 1; // valid

				} وإلا {
					PX4_ERR("verify: '%s' invalid BSON value %" PRIi32 "(expected %" PRIi32 ")", غوټه_2->name, غوټه_2->i32, value);
				}
			}
		}
		اكسر;

	حالة BSON_DOUBLE: {
			إذا (param_type(param) != PARAM_TYPE_FLOAT) {
				PX4_ERR("verify: invalid param type %d for '%s' (BSON_DOUBLE)", param_type(param), غوټه_2->name);
				أرجع -1;
			}

			عائم value;

			إذا (param_get(param, &value) == 0) {
				إذا (fabsf(value - (عائم)غوټه_2->d) <= FLT_EPSILON) {
					أرجع 1; // valid

				} وإلا {
					PX4_ERR("verify: '%s' invalid BSON value %.3f (expected %.3f)", غوټه_2->name, غوټه_2->d, (مزدوج)value);
				}
			}
		}
		اكسر;

	افتراضي:
		PX4_ERR("verify: '%s' invalid node type %d", غوټه_2->name, غوټه_2->type);
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
		صحيح پایله = -1;

		افعل {
			پایله = bson_decoder_بل(&decoder);

		} طالما (پایله > 0);

		إذا (پایله == 0) {
			إذا (decoder.ټول_document_اندازه != decoder.ټول_decoded_اندازه) {
				PX4_ERR("BSON document size (%" PRId32 ") doesn't match bytes decoded (%" PRId32 ")", decoder.ټول_document_اندازه,
					decoder.ټول_decoded_اندازه);

			} وإلا {
				أرجع 0;
			}

		} وإلا إذا (پایله == -ENODATA) {
			PX4_ERR("verify: no BSON data");

		} وإلا {
			PX4_ERR("verify: failed (%d)", پایله);
		}
	}

	أرجع -1;
}

صحيح
param_export(ثابت حرف *filename, param_filter_func filter)
{
	PX4_DEBUG("param_export");

	صحيح shutdown_قلف_ret = px4_shutdown_قلف();

	إذا (shutdown_قلف_ret != 0) {
		PX4_ERR("px4_shutdown_lock() failed (%i)", shutdown_قلف_ret);
	}

	// take the file lock
	إذا (pthread_mutex_trylock(&file_mutex) != 0) {
		PX4_ERR("param_export: file lock failed (already locked)");
		أرجع PX4_ERROR;
	}

	صحيح fd = ::open(filename, O_RDWR | O_CREAT, PX4_O_ډېرپېښ_666);
	صحيح پایله = PX4_ERROR;

	perf_begin(param_export_perf);

	إذا (fd > -1) {
		پایله = param_export_دننی(fd, filter);

	} وإلا {
		پایله = flash_param_save(filter);
	}

	perf_پای_ټکی(param_export_perf);

	pthread_mutex_unlock(&file_mutex);

	إذا (shutdown_قلف_ret == 0) {
		px4_shutdown_unlock();
	}

	أرجع پایله;
}

// internal parameter export, caller is responsible for locking
ساكن صحيح param_export_دننی(صحيح fd, param_filter_func filter)
{
	PX4_DEBUG("param_export_internal");
	ثابت خپلکاری بدل_شوی_params = user_config.containedAsBitset();

	صحيح پایله = -1;
	bson_encoder_s encoder{};
	uint8_t bson_بفر[256];

	إذا (lseek(fd, 0, SEEK_SET) != 0) {
		PX4_ERR("export seek failed %d", errno);
		أرجع -1;
	}

	إذا (bson_encoder_init_buf_file(&encoder, fd, &bson_بفر, حجم(bson_بفر)) != 0) {
		اذهب بهر;
	}

	لكل (param_t param = 0; handle_دننه_range(param); param++) {
		إذا (!بدل_شوی_params[param] || (filter && !filter(param))) {
			استمر;
		}

		ثابت param_ارزښت_u runtime_default_ارزښت = runtime_defaults.get(param);
		ثابت param_ارزښت_u user_config_ارزښت = user_config.get(param);

		// don't export default values
		بدل (param_type(param)) {
		حالة PARAM_TYPE_INT32:
			إذا (user_config_ارزښت.i == runtime_default_ارزښت.i) {
				PX4_DEBUG("skipping %s %" PRIi32 " export", param_name(param), runtime_default_ارزښت.i);
				استمر;
			}

			اكسر;

		حالة PARAM_TYPE_FLOAT:
			إذا (fabsf(user_config_ارزښت.f - runtime_default_ارزښت.f) <= FLT_EPSILON) {
				PX4_DEBUG("skipping %s %.3f export", param_name(param), (مزدوج)runtime_default_ارزښت.f);
				استمر;
			}

			اكسر;
		}

		ثابت حرف *name = param_name(param);
		ثابت size_t اندازه = param_اندازه(param);

		/* append the appropriate BSON type object */
		بدل (param_type(param)) {
		حالة PARAM_TYPE_INT32: {
				ثابت int32_t i = user_config_ارزښت.i;
				PX4_DEBUG("exporting: %s (%d) size: %lu val: %" PRIi32, name, param, (اوږد بینښه صحيح)اندازه, i);

				إذا (bson_encoder_append_int32(&encoder, name, i) != 0) {
					PX4_ERR("BSON append failed for '%s'", name);
					اذهب بهر;
				}
			}
			اكسر;

		حالة PARAM_TYPE_FLOAT: {
				ثابت مزدوج f = (مزدوج)user_config_ارزښت.f;
				PX4_DEBUG("exporting: %s (%d) size: %lu val: %.3f", name, param, (اوږد بینښه صحيح)اندازه, (مزدوج)f);

				إذا (bson_encoder_append_double(&encoder, name, f) != 0) {
					PX4_ERR("BSON append failed for '%s'", name);
					اذهب بهر;
				}
			}
			اكسر;

		افتراضي:
			PX4_ERR("%s unrecognized parameter type %d, skipping export", name, param_type(param));
		}
	}

	پایله = 0;

بهر:

	إذا (پایله == 0) {
		إذا (bson_encoder_fini(&encoder) != PX4_OK) {
			PX4_ERR("BSON encoder finalize failed");
			پایله = -1;
		}
	}

	أرجع پایله;
}

#إذا defined(FLASH_BASED_PARAMS)
// A file is never the flash backend's store, so what an import changes is unsaved until a delta carries it.
// As on the file backend, the import itself does not save.
ساكن constexpr منطقي file_import_is_saved = خطأ;
#وإلا
ساكن constexpr منطقي file_import_is_saved = ریښتیا;
#د_که_پای

ساكن صحيح
param_import_callback(bson_decoder_t decoder, bson_غوټه_t غوټه_2)
{
	/*
	 * EOO means the end of the parameter object. (Currently not supporting
	 * nested BSON objects).
	 */
	إذا (غوټه_2->type == BSON_EOO) {
		PX4_DEBUG("end of parameters");
		أرجع 0;
	}

	// A tombstone has no value. The decoder leaves the previous node's in the union and the translations
	// read it without a type check.
	ثابت منطقي tombstone = غوټه_2->type == BSON_nullptr || غوټه_2->type == BSON_UNDEFINED;

	// if we do param_set() directly in the translation, set PARAM_SKIP_IMPORT as return value and return here
	إذا (!tombstone && param_modify_on_import(غوټه_2) == param_modify_on_import_ret::PARAM_SKIP_IMPORT) {
		أرجع 1;
	}

	// Find the parameter this node represents.  If we don't know it, ignore the node.
	param_t param = param_پیدا_کړه_no_notification(غوټه_2->name);

	إذا (param == PARAM_INVALID) {
		PX4_WARN("ignoring unrecognised parameter '%s'", غوټه_2->name);
		أرجع 1;
	}

	// Handle setting the parameter from the node
	بدل (غوټه_2->type) {
	حالة BSON_nullptr:
	حالة BSON_UNDEFINED:
		param_reset_دننی(param, ریښتیا, خطأ);
		أرجع 1;

	حالة BSON_INT32: {
			إذا (param_type(param) == PARAM_TYPE_INT32) {
				int32_t i = غوټه_2->i32;
				param_سېټ_دننی(param, &i, file_import_is_saved, ریښتیا, ریښتیا, خطأ);
				PX4_DEBUG("Imported %s with value %" PRIi32, param_name(param), i);

			} وإلا {
				PX4_WARN("unexpected type for %s", غوټه_2->name);
			}
		}
		اكسر;

	حالة BSON_DOUBLE: {
			إذا (param_type(param) == PARAM_TYPE_FLOAT) {
				عائم f = غوټه_2->d;
				param_سېټ_دننی(param, &f, file_import_is_saved, ریښتیا, ریښتیا, خطأ);
				PX4_DEBUG("Imported %s with value %f", param_name(param), (مزدوج)f);

			} وإلا {
				PX4_WARN("unexpected type for %s", غوټه_2->name);
			}
		}
		اكسر;

	افتراضي:
		PX4_ERR("import: unrecognised node type for '%s'", غوټه_2->name);
	}

	// don't return zero, that means EOF
	أرجع 1;
}

ساكن صحيح
param_import_دننی(صحيح fd)
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

	int32_t doc_اندازه = 0;
	ssize_t len = اقرأ(fd, &doc_اندازه, حجم(doc_اندازه));

	إذا (len == 0 || (len == (ssize_t)حجم(doc_اندازه) && (doc_اندازه == 0 || doc_اندازه == -1))) {
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
			صحيح پایله = -1;

			افعل {
				پایله = bson_decoder_بل(&decoder);

			} طالما (پایله > 0);

			إذا (پایله == 0) {
				إذا (decoder.ټول_document_اندازه == decoder.ټول_decoded_اندازه) {
					PX4_INFO("BSON document size %" PRId32 " bytes, decoded %" PRId32 " bytes (INT32:%" PRIu16 ", FLOAT:%" PRIu16 ")",
						 decoder.ټول_document_اندازه, decoder.ټول_decoded_اندازه,
						 decoder.شمېر_غوټه_int32, decoder.شمېر_غوټه_double);

					أرجع 0;

				} وإلا {
					PX4_ERR("BSON document size (%" PRId32 ") doesn't match bytes decoded (%" PRId32 ")",
						decoder.ټول_document_اندازه, decoder.ټول_decoded_اندازه);
				}

			} وإلا إذا (پایله == -ENODATA) {
				// silently retry as a precaution unless this is our last attempt
				إذا (attempt == MAX_ATTEMPTS) {
					PX4_DEBUG("BSON: no data");
					أرجع 0;
				}

			} وإلا {
				PX4_ERR("param import failed (%d) attempt %d", پایله, attempt);
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

	صحيح shutdown_قلف_ret = px4_shutdown_قلف();

	إذا (shutdown_قلف_ret != 0) {
		PX4_ERR("px4_shutdown_lock() failed (%i)", shutdown_قلف_ret);
	}

	صحيح پایله = op();

	pthread_mutex_unlock(&file_mutex);

	إذا (shutdown_قلف_ret == 0) {
		px4_shutdown_unlock();
	}

	أرجع پایله;
}

ساكن صحيح flash_بارول_locked()
{
	/* Reset under the lock so a pending autosave cannot append tombstones
	 * of the just-cleared RAM before import/compact run. */
	param_reset_all_دننی(خطأ);
	صحيح پایله = flash_param_import();

	إذا (پایله == 0 || پایله == 1) {
		params_unsaved.reset();
	}

	أرجع پایله;
}
#د_که_پای

صحيح
param_import(صحيح fd)
{
	إذا (fd < 0) {
#إذا defined(FLASH_BASED_PARAMS)
		أرجع flash_backend_locked(flash_param_import);
#وإلا
		أرجع flash_param_import();
#د_که_پای
	}

	أرجع param_import_دننی(fd);
}

صحيح
param_بارول(صحيح fd)
{
	إذا (fd < 0) {
#إذا defined(FLASH_BASED_PARAMS)
		أرجع flash_backend_locked(flash_بارول_locked);
#وإلا
		أرجع PX4_ERROR;
#د_که_پای
	}

	param_reset_all_دننی(خطأ);
	صحيح پایله = param_import_دننی(fd);

#إذا !defined(FLASH_BASED_PARAMS)

	إذا (پایله >= 0) {
		params_unsaved.reset();
	}

#د_که_پای

	أرجع پایله;
}

فراغ
param_foreach(فراغ (*func)(فراغ *arg, param_t param), فراغ *arg, منطقي only_بدل_شوی, منطقي only_کارول_شوی)
{
	param_t	param;

	لكل (param = 0; handle_دننه_range(param); param++) {

		/* if requested, skip unchanged values */
		إذا (only_بدل_شوی && (!user_config.لري(param))) {
			استمر;
		}

		إذا (only_کارول_شوی && !param_کارول_شوی(param)) {
			استمر;
		}

		func(arg, param);
	}
}

uint32_t param_hash_check()
{
	uint32_t param_hash = 0;

	/* compute the CRC32 over all string param names and 4 byte values */
	لكل (param_t param = 0; handle_دننه_range(param); param++) {
		إذا (!param_کارول_شوی(param) || param_is_volatile(param)) {
			استمر;
		}

		ثابت حرف *name = param_name(param);
		خپلکاری value = user_config.get(param).i;
		ثابت فراغ *val = (فراغ *)&value;
		param_hash = crc32part((ثابت uint8_t *)name, مزي_اوږدوالی(name), param_hash);
		param_hash = crc32part((ثابت uint8_t *)val, param_اندازه(param), param_hash);
	}

	أرجع param_hash;
}

فراغ param_print_حالت()
{
	PX4_INFO("summary: %d/%d (used/total)", param_شمېر_کارول_شوی(), param_شمېر());

#که_نه_تعریف_شوی FLASH_BASED_PARAMS
	ثابت حرف *filename = param_get_default_file();

	إذا (filename != تش_اشاره) {
		PX4_INFO("file: %s", param_get_default_file());
	}

	إذا (param_backup_file) {
		PX4_INFO("backup file: %s", param_backup_file);
	}

#د_که_پای /* FLASH_BASED_PARAMS */

	PX4_INFO("storage array: %d/%d elements (%zu bytes total)",
		 user_config.اندازه(), firmware_defaults.اندازه(), (size_t)user_config.byte_اندازه());


	PX4_INFO("storage array (custom defaults): %d/%d elements (%zu bytes total)",
		 runtime_defaults.اندازه(), firmware_defaults.اندازه(), (size_t)runtime_defaults.byte_اندازه());

	إذا (autosave_instance) {
		PX4_INFO("auto save: %s", autosave_instance->enabled() ? "on" : "off");

		hrt_abstime وروستی_autosave = autosave_instance->وروستی_Autosave();

		إذا (وروستی_autosave > 0) {
			PX4_INFO("last auto save: %.3f seconds ago", hrt_elapsed_وخت(&وروستی_autosave) * 1e-6);
		}
	}

	perf_print_counter(param_export_perf);
	perf_print_counter(param_پیدا_کړه_perf);
	perf_print_counter(param_get_perf);
	perf_print_counter(param_سېټ_perf);

#إذا defined(CONFIG_PARAM_PRIMARY)
	بنية param_primary_counters counts;
	param_primary_get_counters(&counts);
	PX4_INFO("set value requests received: %" PRIu32 ", set value responses sent: %" PRIu32,
		 counts.سېټ_ارزښت_request_received, counts.سېټ_ارزښت_response_لېږل_شوی);
	PX4_INFO("set value requests sent: %" PRIu32 ", set value responses received: %" PRIu32,
		 counts.سېټ_ارزښت_request_لېږل_شوی, counts.سېټ_ارزښت_response_received);
	PX4_INFO("resets sent: %" PRIu32 ", set used requests received: %" PRIu32,
		 counts.reset_لېږل_شوی, counts.سېټ_کارول_شوی_received);
#د_که_پای

#إذا defined(CONFIG_PARAM_REMOTE)
	بنية param_remote_counters counts;
	param_remote_get_counters(&counts);
	PX4_INFO("set value requests received: %" PRIu32 ", set value responses sent: %" PRIu32,
		 counts.سېټ_ارزښت_request_received, counts.سېټ_ارزښت_response_لېږل_شوی);
	PX4_INFO("set value requests sent: %" PRIu32 ", set value responses received: %" PRIu32,
		 counts.سېټ_ارزښت_request_لېږل_شوی, counts.سېټ_ارزښت_response_received);
	PX4_INFO("resets received: %" PRIu32 ", set used requests sent: %" PRIu32,
		 counts.reset_received, counts.سېټ_کارول_شوی_لېږل_شوی);
#د_که_پای

}
