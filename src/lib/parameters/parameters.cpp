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

#تعریف_کن PARAM_IMPLEMENTATION
#ضمن "param.h"
#ضمن "param_translation.h"
#ضمن <parameters/px4_parameters.hpp>
#ضمن <lib/tinybson/tinybson.h>

#إذا defined(__PX4_NUTTX)
#ضمن <nuttx/crc32.h>
#وإلا
#ضمن <crc32.h>
#پایان_اگر
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

استفاده نطاق زمان_literals;

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
#پایان_اگر

#إذا defined(FLASH_BASED_PARAMS)
#ضمن "flashparams/flashparams.h"
#وإلا
مضمن ساكن صحيح flash_param_save(param_filter_func filter) { أرجع -1; }
مضمن ساكن صحيح flash_param_import() { أرجع -1; }
#پایان_اگر

ساكن حرف *param_default_file = اشاره_تهی;
ساكن حرف *param_backup_file = اشاره_تهی;

#ضمن "autosave.h"
ساكن ParamAutosave *autosave_instance {اشاره_تهی};

ساكن px4::AtomicBitset<param_info_شمار> params_فعال;  // params found
ساكن px4::AtomicBitset<param_info_شمار> params_unsaved;

// Params the save in progress is writing. They are moved out of params_unsaved
// before their values are read, so a param_set that lands while the save runs
// marks its param again and reaches the next save. Flash-param saves write only
// unsaved params, so nothing else would ever write that change.
ساكن px4::AtomicBitset<param_info_شمار> params_saving;

ساكن ConstLayer firmware_defaults;
ساكن DynamicSparseLayer runtime_defaults{&firmware_defaults};
DynamicSparseLayer user_config{&runtime_defaults};

/** parameter update topic handle */
#إذا not defined(CONFIG_PARAM_REMOTE)
ساكن orb_advert_t param_topic = اشاره_تهی;
ساكن بیعلامت صحيح param_instance = 0;
#پایان_اگر

ساكن perf_counter_t param_export_perf;
ساكن perf_counter_t param_پیدا_کن_perf;
ساكن perf_counter_t param_get_perf;
ساكن perf_counter_t param_مجموعه_perf;

ساكن pthread_mutex_t file_mutex  =
	PTHREAD_MUTEX_INITIALIZER; ///< this protects against concurrent param saves (file or flash access).

// Support for remote parameter node
#إذا defined(CONFIG_PARAM_PRIMARY)
# include "parameters_primary.h"
#پایان_اگر // CONFIG_PARAM_PRIMARY
#إذا defined(CONFIG_PARAM_REMOTE)
# include "parameters_remote.h"
#پایان_اگر // CONFIG_PARAM_REMOTE

فراغ
param_init()
{
#اگر_تعریف_شده __PX4_POSIX
	// Set up the recursive param mutex before any thread can access parameters.
	AtomicTransaction::initialize();
#پایان_اگر

	param_export_perf = perf_alloc(PC_ELAPSED, "param: export");
	param_پیدا_کن_perf = perf_alloc(PC_شمار, "param: find");
	param_get_perf = perf_alloc(PC_شمار, "param: get");
	param_مجموعه_perf = perf_alloc(PC_ELAPSED, "param: set");

#إذا defined(__PX4_NUTTX) && !defined(CONFIG_BUILD_FLAT)
	px4_ثبات_boardct_ioctl(_PARAMIOCBASE, param_ioctl);
#پایان_اگر

#إذا defined(CONFIG_PARAM_PRIMARY)
	param_primary_init();
#پایان_اگر // CONFIG_PARAM_PRIMARY

#إذا defined(CONFIG_PARAM_REMOTE)
	param_remote_init();
#پایان_اگر // CONFIG_PARAM_REMOTE

#إذا not defined(CONFIG_PARAM_REMOTE)
	autosave_instance = جديد ParamAutosave();
#پایان_اگر
}


فراغ
param_notify_changes()
{
// Don't send if this is a remote node. Only the primary
// sends out update notices
#إذا not defined(CONFIG_PARAM_REMOTE)
	parameter_update_s pup {};
	pup.instance = param_instance++;
	pup.get_شمار_2 = perf_event_شمار(param_get_perf);
	pup.مجموعه_شمار = perf_event_شمار(param_مجموعه_perf);
	pup.پیدا_کن_شمار = perf_event_شمار(param_پیدا_کن_perf);
	pup.export_شمار = perf_event_شمار(param_export_perf);
	pup.فعال_2 = params_فعال.شمار();
	pup.تغییریافته = user_config.اندازه();
	pup.custom_default = runtime_defaults.اندازه();
	pup.timestamp = hrt_absolute_زمان();

	إذا (param_topic == اشاره_تهی) {
		param_topic = orb_advertise(ORB_ID(parameter_update), &pup);

	} وإلا {
		orb_publish(ORB_ID(parameter_update), param_topic, &pup);
	}

#پایان_اگر
}

ساكن param_t param_پیدا_کن_داخلی(ثابت حرف *name, منطقي notification)
{
	perf_شمار(param_پیدا_کن_perf);

	param_t میانه;
	param_t front = 0;
	param_t آخرین = param_info_شمار;

	/* perform a binary search of the known parameters */

	طالما (front <= آخرین) {
		میانه = front + (آخرین - front) / 2;
		صحيح ret = strcmp(name, param_name(میانه));

		إذا (ret == 0) {
			إذا (notification) {
				param_مجموعه_استفاده_شده(میانه);
			}

			أرجع میانه;

		} وإلا إذا (میانه == front) {
			/* An end point has been hit, but there has been no match */
			اكسر;

		} وإلا إذا (ret < 0) {
			آخرین = میانه;

		} وإلا {
			front = میانه;
		}
	}

	/* not found */
	أرجع PARAM_INVALID;
}

param_t param_پیدا_کن(ثابت حرف *name)
{
	أرجع param_پیدا_کن_داخلی(name, درست);
}

param_t param_پیدا_کن_no_notification(ثابت حرف *name)
{
	أرجع param_پیدا_کن_داخلی(name, خطأ);
}

بیعلامت param_شمار_استفاده_شده()
{
	أرجع params_فعال.شمار();
}

param_t param_for_استفاده_شده_شاخص(بیعلامت شاخص)
{
	// walk all params and count used params
	إذا (شاخص < param_info_شمار) {
		بیعلامت استفاده_شده_شمار = 0;

		لكل (صحيح i = 0; i < params_فعال.اندازه(); i++) {
			إذا (params_فعال[i]) {
				// we found the right used count,
				//  return the param value
				إذا (شاخص == استفاده_شده_شمار) {
					أرجع static_cast<param_t>(i);
				}

				استفاده_شده_شمار++;
			}
		}
	}

	أرجع PARAM_INVALID;
}

صحيح param_get_استفاده_شده_شاخص(param_t param)
{
	/* this tests for out of bounds and does a constant time lookup */
	إذا (!param_استفاده_شده(param)) {
		أرجع -1;
	}

	/* walk all params and count, now knowing that it has a valid index */
	صحيح استفاده_شده_شمار = 0;

	لكل (صحيح i = 0; i < params_فعال.اندازه(); i++) {
		إذا (params_فعال[i]) {

			إذا (param == i) {
				أرجع استفاده_شده_شمار;
			}

			استفاده_شده_شمار++;
		}
	}

	أرجع -1;
}

منطقي
param_مقدار_unsaved(param_t param)
{
	أرجع handle_درون_range(param) ? (params_unsaved[param] || params_saving[param]) : خطأ;
}

صحيح
param_get(param_t param, فراغ *val)
{
	perf_شمار(param_get_perf);

	إذا (!handle_درون_range(param)) {
		PX4_ERR("get: param %" PRId16 " invalid", param);
		أرجع PX4_ERROR;
	}

	إذا (!params_فعال[param]) {
		PX4_DEBUG("get: param %" PRId16 " (%s) not active", param, param_name(param));
	}

	صحيح نتیجه = PX4_ERROR;

	إذا (val) {

		خودکار retrieve_مقدار = user_config.get(param);

		بدل (param_type(param)) {
		حالة PARAM_TYPE_INT32:
			کپی_حافظه(val, &retrieve_مقدار.i, حجم(retrieve_مقدار.i));
			أرجع PX4_OK;

		حالة PARAM_TYPE_FLOAT:
			کپی_حافظه(val, &retrieve_مقدار.f, حجم(retrieve_مقدار.f));
			أرجع PX4_OK;
		}
	}

	أرجع نتیجه;
}

صحيح
param_get_default_مقدار_داخلی(param_t param, فراغ *default_val)
{
	إذا (!handle_درون_range(param)) {
		PX4_ERR("get default value: param %d invalid", param);
		أرجع PX4_ERROR;
	}

	إذا (default_val) {
		بدل (param_type(param)) {
		حالة PARAM_TYPE_INT32: {
				int32_t val = runtime_defaults.get(param).i;
				کپی_حافظه(default_val, &val, حجم(val));
				أرجع PX4_OK;
			}

		حالة PARAM_TYPE_FLOAT: {
				عائم val = runtime_defaults.get(param).f;
				کپی_حافظه(default_val, &val, حجم(val));
				أرجع PX4_OK;
			}
		}
	}

	أرجع PX4_ERROR;
}

صحيح
param_get_default_مقدار(param_t param, فراغ *default_val)
{
	إذا (!handle_درون_range(param)) {
		أرجع PX4_ERROR;
	}

	صحيح ret = param_get_default_مقدار_داخلی(param, default_val);
	أرجع ret;
}

منطقي param_مقدار_is_default(param_t param)
{
	إذا (!handle_درون_range(param)) {
		أرجع درست;
	}

	إذا (!user_config.شامل_است(param)) {
		// if user config does not contain it, consider it default.
		أرجع درست;

	} وإلا {
		// compare with default value
		ثابت param_مقدار_u user_config_مقدار = user_config.get(param);
		ثابت param_مقدار_u runtime_default_مقدار = runtime_defaults.get(param);

		بدل (param_type(param)) {
		حالة PARAM_TYPE_INT32: {
				أرجع user_config_مقدار.i == runtime_default_مقدار.i;
			}

		حالة PARAM_TYPE_FLOAT: {
				أرجع fabsf(user_config_مقدار.f - runtime_default_مقدار.f) < FLT_EPSILON;
			}
		}
	}

	أرجع درست;
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
param_مجموعه_داخلی(param_t param, ثابت فراغ *val, منطقي mark_saved, منطقي notify_changes, منطقي update_remote = درست,
		   منطقي autosave = درست)
{
	إذا (!handle_درون_range(param)) {
		PX4_ERR("set invalid param %d", param);
		أرجع PX4_ERROR;
	}

	إذا (val == اشاره_تهی) {
		PX4_ERR("set invalid value");
		أرجع PX4_ERROR;
	}

	إذا (param_is_readonly(param)) {
		PX4_WARN("param %s is read-only", param_name(param));
		أرجع PX4_ERROR;
	}

	صحيح نتیجه = -1;
	منطقي param_تغییریافته = خطأ;
	perf_begin(param_مجموعه_perf);

	ثابت param_مقدار_u user_config_مقدار = user_config.get(param);
	param_مقدار_u new_مقدار{};

	بدل (param_type(param)) {
	حالة PARAM_TYPE_INT32:
		کپی_حافظه(&new_مقدار.i, val, حجم(new_مقدار.i));
		param_تغییریافته = user_config_مقدار.i != new_مقدار.i;
		اكسر;

	حالة PARAM_TYPE_FLOAT:
		کپی_حافظه(&new_مقدار.f, val, حجم(new_مقدار.f));
		param_تغییریافته = fabsf(user_config_مقدار.f - new_مقدار.f) > FLT_EPSILON;
		اكسر;

	افتراضي: {
			PX4_ERR("param_set invalid param type for %s", param_name(param));
			اكسر;
		}
	}

	إذا (user_config.store(param, new_مقدار)) {
		إذا (param_تغییریافته) {
			params_unsaved.مجموعه(param, !mark_saved);
		}

		نتیجه = PX4_OK;

	} وإلا {
		PX4_ERR("param_set failed to store param %s", param_name(param));
		نتیجه = PX4_ERROR;
	}

	إذا ((نتیجه == PX4_OK) && param_تغییریافته && !mark_saved && autosave) { // this is false when importing parameters
		param_autosave();
	}

	// If this is the parameter server, make sure that the remote is updated
#إذا defined(CONFIG_PARAM_PRIMARY)

	إذا (param_تغییریافته && update_remote) {
		param_primary_مجموعه_مقدار(param, val);
	}

#پایان_اگر

	// If this is the parameter remote, make sure that the primary is updated
#إذا defined(CONFIG_PARAM_REMOTE)

	إذا (param_تغییریافته && update_remote) {
		param_remote_مجموعه_مقدار(param, val);
	}

#پایان_اگر

	perf_انتها(param_مجموعه_perf);

	/*
	 * If we set something, now that we have unlocked, go ahead and advertise that
	 * a thing has been set.
	 */
	إذا ((نتیجه == PX4_OK) && param_تغییریافته && notify_changes) {
		param_notify_changes();
	}

	أرجع نتیجه;
}

#إذا defined(FLASH_BASED_PARAMS)
صحيح param_مجموعه_external(param_t param, ثابت فراغ *val, منطقي mark_saved, منطقي notify_changes)
{
	أرجع param_مجموعه_داخلی(param, val, mark_saved, notify_changes);
}

فراغ param_get_external(param_t param, فراغ *val)
{
	param_get(param, val);
}
#پایان_اگر

صحيح param_مجموعه(param_t param, ثابت فراغ *val)
{
	أرجع param_مجموعه_داخلی(param, val, خطأ, درست);
}

صحيح param_مجموعه_no_notification(param_t param, ثابت فراغ *val)
{
	أرجع param_مجموعه_داخلی(param, val, خطأ, خطأ);
}

صحيح param_مجموعه_no_remote_update(param_t param, ثابت فراغ *val, منطقي notify)
{
	أرجع param_مجموعه_داخلی(param, val, خطأ, notify, خطأ);
}

منطقي param_استفاده_شده(param_t param)
{
	إذا (handle_درون_range(param)) {
		أرجع params_فعال[param];
	}

	أرجع خطأ;
}

فراغ param_مجموعه_استفاده_شده(param_t param)
{
	إذا (handle_درون_range(param)) {
#إذا defined(CONFIG_PARAM_REMOTE)

		إذا (!param_استفاده_شده(param)) {
			param_remote_مجموعه_استفاده_شده(param);
		}

#پایان_اگر

		params_فعال.مجموعه(param, درست);
	}
}

صحيح param_مجموعه_default_مقدار(param_t param, ثابت فراغ *val)
{
	إذا (!handle_درون_range(param)) {
		PX4_ERR("set default value invalid param %d", param);
		أرجع PX4_ERROR;
	}

	إذا (val == اشاره_تهی) {
		PX4_ERR("set default value invalid value");
		أرجع PX4_ERROR;
	}

	إذا (param_is_readonly(param)) {
		PX4_WARN("param %s is read-only", param_name(param));
		أرجع PX4_ERROR;
	}

	صحيح نتیجه = PX4_ERROR;


	// check if param being set to default value
	منطقي setting_مقصد_static_default = خطأ;
	ثابت param_مقدار_u firmware_default_مقدار = firmware_defaults.get(param);

	بدل (param_type(param)) {
	حالة PARAM_TYPE_INT32: {
			int32_t new_مقدار;
			کپی_حافظه(&new_مقدار, val, حجم(new_مقدار));
			setting_مقصد_static_default = firmware_default_مقدار.i == new_مقدار;
			اكسر;
		}

	حالة PARAM_TYPE_FLOAT: {
			عائم new_مقدار;
			کپی_حافظه(&new_مقدار, val, حجم(new_مقدار));
			setting_مقصد_static_default = fabsf(firmware_default_مقدار.f - new_مقدار) <= FLT_EPSILON;
			اكسر;
		}
	}

	إذا (setting_مقصد_static_default) {
		runtime_defaults.reset(param);

		نتیجه = PX4_OK;

	} وإلا {
		param_مقدار_u new_مقدار{};

		بدل (param_type(param)) {
		حالة PARAM_TYPE_INT32: {
				کپی_حافظه(&new_مقدار.i, val, حجم(new_مقدار.i));
				اكسر;
			}

		حالة PARAM_TYPE_FLOAT: {
				کپی_حافظه(&new_مقدار.f, val, حجم(new_مقدار.f));
				اكسر;
			}

		افتراضي:
			اكسر;
		}

		إذا (runtime_defaults.store(param, new_مقدار)) {
			user_config.refresh(param);
			نتیجه = PX4_OK;

		} وإلا {
			نتیجه = PX4_ERROR;
		}
	}


	إذا ((نتیجه == PX4_OK) && param_استفاده_شده(param)) {
		// send notification if param is already in use
		param_notify_changes();
	}

	أرجع نتیجه;
}

ساكن صحيح param_reset_داخلی(param_t param, منطقي notify = درست, منطقي autosave = درست)
{
#إذا defined(CONFIG_PARAM_REMOTE)
	// Remote doesn't support reset
	أرجع خطأ;
#پایان_اگر

	إذا (param_is_readonly(param)) {
		أرجع 0;  // silently skip — param_reset_all loops over all params
	}

	منطقي param_یافت = user_config.شامل_است(param);

	إذا (handle_درون_range(param)) {
		user_config.reset(param);
	}

	إذا (param_یافت) {
		/* Persist the reset. Flash-param saves append only unsaved params,
		 * so a reset has to show up as a tombstone or replay keeps the
		 * previous override. */
		params_unsaved.مجموعه(param, درست);
	}

	إذا (autosave) {
		param_autosave();
	}

	إذا (param_یافت && notify) {
		param_notify_changes();
	}

#إذا defined(CONFIG_PARAM_PRIMARY)
	param_primary_reset(param);
#پایان_اگر

	أرجع param_یافت;
}

صحيح param_reset(param_t param) { أرجع param_reset_داخلی(param, درست); }
صحيح param_reset_no_notification(param_t param) { أرجع param_reset_داخلی(param, خطأ); }

ساكن فراغ
param_reset_all_داخلی(منطقي auto_save)
{
#إذا defined(CONFIG_PARAM_REMOTE)
	// Remote doesn't support reset
	أرجع;
#پایان_اگر

	لكل (param_t param = 0; handle_درون_range(param); param++) {
		param_reset_داخلی(param, خطأ, خطأ);
	}

	إذا (auto_save) {
		param_autosave();
	}

#إذا defined(CONFIG_PARAM_PRIMARY)
	param_primary_reset_all();
#پایان_اگر

	param_notify_changes();
}

فراغ
param_reset_all()
{
	param_reset_all_داخلی(درست);
}

فراغ
param_reset_excludes(ثابت حرف *excludes[], صحيح رقم_excludes)
{
	لكل (param_t param = 0; handle_درون_range(param); param++) {
		ثابت حرف *name = param_name(param);
		منطقي exclude = خطأ;

		لكل (صحيح شاخص = 0; شاخص < رقم_excludes; شاخص ++) {
			صحيح len = طول_رشته(excludes[شاخص]);

			إذا ((excludes[شاخص][len - 1] == '*'
			     && مقایسه_رشته(name, excludes[شاخص], len - 1) == 0)
			    || strcmp(name, excludes[شاخص]) == 0) {

				exclude = درست;
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
	لكل (param_t param = 0; handle_درون_range(param); param++) {
		ثابت حرف *name = param_name(param);
		منطقي reset = خطأ;

		لكل (صحيح شاخص = 0; شاخص < رقم_resets; شاخص++) {
			صحيح len = طول_رشته(resets[شاخص]);

			إذا ((resets[شاخص][len - 1] == '*'
			     && مقایسه_رشته(name, resets[شاخص], len - 1) == 0)
			    || strcmp(name, resets[شاخص]) == 0) {

				reset = درست;
				اكسر;
			}
		}

		إذا (reset) {
			param_reset(param);
		}
	}
}

صحيح
param_مجموعه_default_file(ثابت حرف *filename)
{
	إذا (filename && param_backup_file && strcmp(filename, param_backup_file) == 0) {
		PX4_ERR("default file can't be the same as the backup file %s", filename);
		أرجع PX4_ERROR;
	}

#اگر_تعریف_شده FLASH_BASED_PARAMS
	// the default for flash-based params is always the FLASH
	(فراغ)filename;
#وإلا

	إذا (param_default_file != اشاره_تهی) {
		// we assume this is not in use by some other thread
		حرر(param_default_file);
		param_default_file = اشاره_تهی;
	}

	إذا (filename) {
		param_default_file = strdup(filename);
	}

#پایان_اگر /* FLASH_BASED_PARAMS */

	أرجع 0;
}

ثابت حرف *param_get_default_file()
{
	أرجع param_default_file;
}

صحيح param_مجموعه_backup_file(ثابت حرف *filename)
{
	إذا (filename && param_default_file && strcmp(filename, param_default_file) == 0) {
		PX4_ERR("backup file can't be the same as the default file %s", filename);
		أرجع PX4_ERROR;
	}

	إذا (param_backup_file != اشاره_تهی) {
		// we assume this is not in use by some other thread
		حرر(param_backup_file);
		param_backup_file = اشاره_تهی;
	}

	إذا (filename) {
		param_backup_file = strdup(filename);

	} وإلا {
		param_backup_file = اشاره_تهی; // backup disabled
	}

	أرجع 0;
}

ثابت حرف *param_get_backup_file()
{
	أرجع param_backup_file;
}

ساكن صحيح param_export_داخلی(صحيح fd, param_filter_func filter);
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

	صحيح shutdown_قفل_ret = px4_shutdown_قفل();

	إذا (shutdown_قفل_ret != 0) {
		PX4_ERR("px4_shutdown_lock() failed (%i)", shutdown_قفل_ret);
	}

	صحيح res = PX4_ERROR;
	ثابت حرف *filename = param_get_default_file();

	لكل (param_t param = 0; handle_درون_range(param); param++) {
		إذا (params_unsaved[param]) {
			params_saving.مجموعه(param, درست);
			params_unsaved.مجموعه(param, خطأ);
		}
	}

	إذا (filename) {
		ساكن constexpr صحيح MAX_ATTEMPTS = 3;

		لكل (صحيح attempt = 1; attempt <= MAX_ATTEMPTS; attempt++) {
			// write parameters to file
			صحيح fd = ::open(filename, O_WRONLY | O_CREAT | O_TRUNC, PX4_O_نما_666);

			إذا (fd > -1) {
				perf_begin(param_export_perf);
				res = param_export_داخلی(fd, اشاره_تهی);
				perf_انتها(param_export_perf);
				::close(fd);

				إذا (res == PX4_OK) {
					// reopen file to verify
					صحيح fd_verify = ::open(filename, O_RDONLY, PX4_O_نما_666);
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
		res = flash_param_save(اشاره_تهی);
		perf_انتها(param_export_perf);
	}

	إذا (res != PX4_OK) {
		PX4_ERR("param export failed (%d)", res);

		لكل (param_t param = 0; handle_درون_range(param); param++) {
			إذا (params_saving[param]) {
				params_unsaved.مجموعه(param, درست);
			}
		}
	}

	params_saving.reset();

	إذا (res == PX4_OK) {
		// backup file
		إذا (param_backup_file) {
			صحيح fd_backup_file = ::open(param_backup_file, O_WRONLY | O_CREAT | O_TRUNC, PX4_O_نما_666);

			إذا (fd_backup_file > -1) {
				صحيح backup_export_ret = param_export_داخلی(fd_backup_file, اشاره_تهی);
				::close(fd_backup_file);

				إذا (backup_export_ret != 0) {
					PX4_ERR("backup parameter export to %s failed (%d)", param_backup_file, backup_export_ret);

				} وإلا {
					// verify export
					صحيح fd_verify = ::open(param_backup_file, O_RDONLY, PX4_O_نما_666);
					param_verify(fd_verify);
					::close(fd_verify);
				}
			}
		}
	}

	pthread_mutex_unlock(&file_mutex);

	إذا (shutdown_قفل_ret == 0) {
		px4_shutdown_unlock();
	}

	أرجع res;
}

/**
 * @return 0 on success, 1 if all params have not yet been stored, -1 if device open failed, -2 if writing parameters failed
 */
صحيح
param_بارگیری_default()
{
	صحيح res = 0;
	ثابت حرف *filename = param_get_default_file();

	إذا (!filename) {
		أرجع param_بارگیری(-1);
	}

	صحيح fd_بارگیری = ::open(filename, O_RDONLY);

	إذا (fd_بارگیری < 0) {
		/* no parameter file is OK, otherwise this is an error */
		إذا (errno != ENOENT) {
			PX4_ERR("open '%s' for reading failed", filename);
			أرجع -1;
		}

		أرجع 1;
	}

	صحيح نتیجه = param_بارگیری(fd_بارگیری);
	::close(fd_بارگیری);

	إذا (نتیجه == 1) {
		/* blank file: nothing stored yet */
		أرجع 1;
	}

	إذا (نتیجه != 0) {
		PX4_ERR("error reading parameters from '%s'", filename);
		أرجع -2;
	}

	أرجع res;
}

ساكن صحيح param_verify_callback(bson_decoder_t decoder, bson_گره_t گره_2)
{
	إذا (گره_2->type == BSON_EOO) {
		أرجع 0;
	}

	// find the parameter this node represents
	param_t param = param_پیدا_کن_no_notification(گره_2->name);

	إذا (param == PARAM_INVALID) {
		PX4_ERR("verify: invalid parameter '%s'", گره_2->name);
		أرجع -1;
	}

	// handle verifying the parameter from the node
	بدل (گره_2->type) {
	حالة BSON_INT32: {
			إذا (param_type(param) != PARAM_TYPE_INT32) {
				PX4_ERR("verify: invalid param type %d for '%s' (BSON_INT32)", param_type(param), گره_2->name);
				أرجع -1;
			}

			int32_t value;

			إذا (param_get(param, &value) == 0) {
				إذا (value == گره_2->i32) {
					أرجع 1; // valid

				} وإلا {
					PX4_ERR("verify: '%s' invalid BSON value %" PRIi32 "(expected %" PRIi32 ")", گره_2->name, گره_2->i32, value);
				}
			}
		}
		اكسر;

	حالة BSON_DOUBLE: {
			إذا (param_type(param) != PARAM_TYPE_FLOAT) {
				PX4_ERR("verify: invalid param type %d for '%s' (BSON_DOUBLE)", param_type(param), گره_2->name);
				أرجع -1;
			}

			عائم value;

			إذا (param_get(param, &value) == 0) {
				إذا (fabsf(value - (عائم)گره_2->d) <= FLT_EPSILON) {
					أرجع 1; // valid

				} وإلا {
					PX4_ERR("verify: '%s' invalid BSON value %.3f (expected %.3f)", گره_2->name, گره_2->d, (مزدوج)value);
				}
			}
		}
		اكسر;

	افتراضي:
		PX4_ERR("verify: '%s' invalid node type %d", گره_2->name, گره_2->type);
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
		صحيح نتیجه = -1;

		افعل {
			نتیجه = bson_decoder_بعدی(&decoder);

		} طالما (نتیجه > 0);

		إذا (نتیجه == 0) {
			إذا (decoder.مجموع_document_اندازه != decoder.مجموع_decoded_اندازه) {
				PX4_ERR("BSON document size (%" PRId32 ") doesn't match bytes decoded (%" PRId32 ")", decoder.مجموع_document_اندازه,
					decoder.مجموع_decoded_اندازه);

			} وإلا {
				أرجع 0;
			}

		} وإلا إذا (نتیجه == -ENODATA) {
			PX4_ERR("verify: no BSON data");

		} وإلا {
			PX4_ERR("verify: failed (%d)", نتیجه);
		}
	}

	أرجع -1;
}

صحيح
param_export(ثابت حرف *filename, param_filter_func filter)
{
	PX4_DEBUG("param_export");

	صحيح shutdown_قفل_ret = px4_shutdown_قفل();

	إذا (shutdown_قفل_ret != 0) {
		PX4_ERR("px4_shutdown_lock() failed (%i)", shutdown_قفل_ret);
	}

	// take the file lock
	إذا (pthread_mutex_trylock(&file_mutex) != 0) {
		PX4_ERR("param_export: file lock failed (already locked)");
		أرجع PX4_ERROR;
	}

	صحيح fd = ::open(filename, O_RDWR | O_CREAT, PX4_O_نما_666);
	صحيح نتیجه = PX4_ERROR;

	perf_begin(param_export_perf);

	إذا (fd > -1) {
		نتیجه = param_export_داخلی(fd, filter);

	} وإلا {
		نتیجه = flash_param_save(filter);
	}

	perf_انتها(param_export_perf);

	pthread_mutex_unlock(&file_mutex);

	إذا (shutdown_قفل_ret == 0) {
		px4_shutdown_unlock();
	}

	أرجع نتیجه;
}

// internal parameter export, caller is responsible for locking
ساكن صحيح param_export_داخلی(صحيح fd, param_filter_func filter)
{
	PX4_DEBUG("param_export_internal");
	ثابت خودکار تغییریافته_params = user_config.containedAsBitset();

	صحيح نتیجه = -1;
	bson_encoder_s encoder{};
	uint8_t bson_میانگیر[256];

	إذا (lseek(fd, 0, SEEK_SET) != 0) {
		PX4_ERR("export seek failed %d", errno);
		أرجع -1;
	}

	إذا (bson_encoder_init_buf_file(&encoder, fd, &bson_میانگیر, حجم(bson_میانگیر)) != 0) {
		اذهب بیرون;
	}

	لكل (param_t param = 0; handle_درون_range(param); param++) {
		إذا (!تغییریافته_params[param] || (filter && !filter(param))) {
			استمر;
		}

		ثابت param_مقدار_u runtime_default_مقدار = runtime_defaults.get(param);
		ثابت param_مقدار_u user_config_مقدار = user_config.get(param);

		// don't export default values
		بدل (param_type(param)) {
		حالة PARAM_TYPE_INT32:
			إذا (user_config_مقدار.i == runtime_default_مقدار.i) {
				PX4_DEBUG("skipping %s %" PRIi32 " export", param_name(param), runtime_default_مقدار.i);
				استمر;
			}

			اكسر;

		حالة PARAM_TYPE_FLOAT:
			إذا (fabsf(user_config_مقدار.f - runtime_default_مقدار.f) <= FLT_EPSILON) {
				PX4_DEBUG("skipping %s %.3f export", param_name(param), (مزدوج)runtime_default_مقدار.f);
				استمر;
			}

			اكسر;
		}

		ثابت حرف *name = param_name(param);
		ثابت size_t اندازه = param_اندازه(param);

		/* append the appropriate BSON type object */
		بدل (param_type(param)) {
		حالة PARAM_TYPE_INT32: {
				ثابت int32_t i = user_config_مقدار.i;
				PX4_DEBUG("exporting: %s (%d) size: %lu val: %" PRIi32, name, param, (دراز بیعلامت صحيح)اندازه, i);

				إذا (bson_encoder_append_int32(&encoder, name, i) != 0) {
					PX4_ERR("BSON append failed for '%s'", name);
					اذهب بیرون;
				}
			}
			اكسر;

		حالة PARAM_TYPE_FLOAT: {
				ثابت مزدوج f = (مزدوج)user_config_مقدار.f;
				PX4_DEBUG("exporting: %s (%d) size: %lu val: %.3f", name, param, (دراز بیعلامت صحيح)اندازه, (مزدوج)f);

				إذا (bson_encoder_append_double(&encoder, name, f) != 0) {
					PX4_ERR("BSON append failed for '%s'", name);
					اذهب بیرون;
				}
			}
			اكسر;

		افتراضي:
			PX4_ERR("%s unrecognized parameter type %d, skipping export", name, param_type(param));
		}
	}

	نتیجه = 0;

بیرون:

	إذا (نتیجه == 0) {
		إذا (bson_encoder_fini(&encoder) != PX4_OK) {
			PX4_ERR("BSON encoder finalize failed");
			نتیجه = -1;
		}
	}

	أرجع نتیجه;
}

#إذا defined(FLASH_BASED_PARAMS)
// A file is never the flash backend's store, so what an import changes is unsaved until a delta carries it.
// As on the file backend, the import itself does not save.
ساكن constexpr منطقي file_import_is_saved = خطأ;
#وإلا
ساكن constexpr منطقي file_import_is_saved = درست;
#پایان_اگر

ساكن صحيح
param_import_callback(bson_decoder_t decoder, bson_گره_t گره_2)
{
	/*
	 * EOO means the end of the parameter object. (Currently not supporting
	 * nested BSON objects).
	 */
	إذا (گره_2->type == BSON_EOO) {
		PX4_DEBUG("end of parameters");
		أرجع 0;
	}

	// A tombstone has no value. The decoder leaves the previous node's in the union and the translations
	// read it without a type check.
	ثابت منطقي tombstone = گره_2->type == BSON_nullptr || گره_2->type == BSON_UNDEFINED;

	// if we do param_set() directly in the translation, set PARAM_SKIP_IMPORT as return value and return here
	إذا (!tombstone && param_modify_on_import(گره_2) == param_modify_on_import_ret::PARAM_SKIP_IMPORT) {
		أرجع 1;
	}

	// Find the parameter this node represents.  If we don't know it, ignore the node.
	param_t param = param_پیدا_کن_no_notification(گره_2->name);

	إذا (param == PARAM_INVALID) {
		PX4_WARN("ignoring unrecognised parameter '%s'", گره_2->name);
		أرجع 1;
	}

	// Handle setting the parameter from the node
	بدل (گره_2->type) {
	حالة BSON_nullptr:
	حالة BSON_UNDEFINED:
		param_reset_داخلی(param, درست, خطأ);
		أرجع 1;

	حالة BSON_INT32: {
			إذا (param_type(param) == PARAM_TYPE_INT32) {
				int32_t i = گره_2->i32;
				param_مجموعه_داخلی(param, &i, file_import_is_saved, درست, درست, خطأ);
				PX4_DEBUG("Imported %s with value %" PRIi32, param_name(param), i);

			} وإلا {
				PX4_WARN("unexpected type for %s", گره_2->name);
			}
		}
		اكسر;

	حالة BSON_DOUBLE: {
			إذا (param_type(param) == PARAM_TYPE_FLOAT) {
				عائم f = گره_2->d;
				param_مجموعه_داخلی(param, &f, file_import_is_saved, درست, درست, خطأ);
				PX4_DEBUG("Imported %s with value %f", param_name(param), (مزدوج)f);

			} وإلا {
				PX4_WARN("unexpected type for %s", گره_2->name);
			}
		}
		اكسر;

	افتراضي:
		PX4_ERR("import: unrecognised node type for '%s'", گره_2->name);
	}

	// don't return zero, that means EOF
	أرجع 1;
}

ساكن صحيح
param_import_داخلی(صحيح fd)
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
			صحيح نتیجه = -1;

			افعل {
				نتیجه = bson_decoder_بعدی(&decoder);

			} طالما (نتیجه > 0);

			إذا (نتیجه == 0) {
				إذا (decoder.مجموع_document_اندازه == decoder.مجموع_decoded_اندازه) {
					PX4_INFO("BSON document size %" PRId32 " bytes, decoded %" PRId32 " bytes (INT32:%" PRIu16 ", FLOAT:%" PRIu16 ")",
						 decoder.مجموع_document_اندازه, decoder.مجموع_decoded_اندازه,
						 decoder.شمار_گره_int32, decoder.شمار_گره_double);

					أرجع 0;

				} وإلا {
					PX4_ERR("BSON document size (%" PRId32 ") doesn't match bytes decoded (%" PRId32 ")",
						decoder.مجموع_document_اندازه, decoder.مجموع_decoded_اندازه);
				}

			} وإلا إذا (نتیجه == -ENODATA) {
				// silently retry as a precaution unless this is our last attempt
				إذا (attempt == MAX_ATTEMPTS) {
					PX4_DEBUG("BSON: no data");
					أرجع 0;
				}

			} وإلا {
				PX4_ERR("param import failed (%d) attempt %d", نتیجه, attempt);
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

	صحيح shutdown_قفل_ret = px4_shutdown_قفل();

	إذا (shutdown_قفل_ret != 0) {
		PX4_ERR("px4_shutdown_lock() failed (%i)", shutdown_قفل_ret);
	}

	صحيح نتیجه = op();

	pthread_mutex_unlock(&file_mutex);

	إذا (shutdown_قفل_ret == 0) {
		px4_shutdown_unlock();
	}

	أرجع نتیجه;
}

ساكن صحيح flash_بارگیری_locked()
{
	/* Reset under the lock so a pending autosave cannot append tombstones
	 * of the just-cleared RAM before import/compact run. */
	param_reset_all_داخلی(خطأ);
	صحيح نتیجه = flash_param_import();

	إذا (نتیجه == 0 || نتیجه == 1) {
		params_unsaved.reset();
	}

	أرجع نتیجه;
}
#پایان_اگر

صحيح
param_import(صحيح fd)
{
	إذا (fd < 0) {
#إذا defined(FLASH_BASED_PARAMS)
		أرجع flash_backend_locked(flash_param_import);
#وإلا
		أرجع flash_param_import();
#پایان_اگر
	}

	أرجع param_import_داخلی(fd);
}

صحيح
param_بارگیری(صحيح fd)
{
	إذا (fd < 0) {
#إذا defined(FLASH_BASED_PARAMS)
		أرجع flash_backend_locked(flash_بارگیری_locked);
#وإلا
		أرجع PX4_ERROR;
#پایان_اگر
	}

	param_reset_all_داخلی(خطأ);
	صحيح نتیجه = param_import_داخلی(fd);

#إذا !defined(FLASH_BASED_PARAMS)

	إذا (نتیجه >= 0) {
		params_unsaved.reset();
	}

#پایان_اگر

	أرجع نتیجه;
}

فراغ
param_foreach(فراغ (*func)(فراغ *arg, param_t param), فراغ *arg, منطقي only_تغییریافته, منطقي only_استفاده_شده)
{
	param_t	param;

	لكل (param = 0; handle_درون_range(param); param++) {

		/* if requested, skip unchanged values */
		إذا (only_تغییریافته && (!user_config.شامل_است(param))) {
			استمر;
		}

		إذا (only_استفاده_شده && !param_استفاده_شده(param)) {
			استمر;
		}

		func(arg, param);
	}
}

uint32_t param_hash_check()
{
	uint32_t param_hash = 0;

	/* compute the CRC32 over all string param names and 4 byte values */
	لكل (param_t param = 0; handle_درون_range(param); param++) {
		إذا (!param_استفاده_شده(param) || param_is_volatile(param)) {
			استمر;
		}

		ثابت حرف *name = param_name(param);
		خودکار value = user_config.get(param).i;
		ثابت فراغ *val = (فراغ *)&value;
		param_hash = crc32part((ثابت uint8_t *)name, طول_رشته(name), param_hash);
		param_hash = crc32part((ثابت uint8_t *)val, param_اندازه(param), param_hash);
	}

	أرجع param_hash;
}

فراغ param_print_وضعیت()
{
	PX4_INFO("summary: %d/%d (used/total)", param_شمار_استفاده_شده(), param_شمار());

#اگر_تعریف_نشده FLASH_BASED_PARAMS
	ثابت حرف *filename = param_get_default_file();

	إذا (filename != اشاره_تهی) {
		PX4_INFO("file: %s", param_get_default_file());
	}

	إذا (param_backup_file) {
		PX4_INFO("backup file: %s", param_backup_file);
	}

#پایان_اگر /* FLASH_BASED_PARAMS */

	PX4_INFO("storage array: %d/%d elements (%zu bytes total)",
		 user_config.اندازه(), firmware_defaults.اندازه(), (size_t)user_config.byte_اندازه());


	PX4_INFO("storage array (custom defaults): %d/%d elements (%zu bytes total)",
		 runtime_defaults.اندازه(), firmware_defaults.اندازه(), (size_t)runtime_defaults.byte_اندازه());

	إذا (autosave_instance) {
		PX4_INFO("auto save: %s", autosave_instance->enabled() ? "on" : "off");

		hrt_abstime آخرین_autosave = autosave_instance->آخرین_Autosave();

		إذا (آخرین_autosave > 0) {
			PX4_INFO("last auto save: %.3f seconds ago", hrt_elapsed_زمان(&آخرین_autosave) * 1e-6);
		}
	}

	perf_print_counter(param_export_perf);
	perf_print_counter(param_پیدا_کن_perf);
	perf_print_counter(param_get_perf);
	perf_print_counter(param_مجموعه_perf);

#إذا defined(CONFIG_PARAM_PRIMARY)
	بنية param_primary_counters counts;
	param_primary_get_counters(&counts);
	PX4_INFO("set value requests received: %" PRIu32 ", set value responses sent: %" PRIu32,
		 counts.مجموعه_مقدار_request_received, counts.مجموعه_مقدار_response_فرستاده);
	PX4_INFO("set value requests sent: %" PRIu32 ", set value responses received: %" PRIu32,
		 counts.مجموعه_مقدار_request_فرستاده, counts.مجموعه_مقدار_response_received);
	PX4_INFO("resets sent: %" PRIu32 ", set used requests received: %" PRIu32,
		 counts.reset_فرستاده, counts.مجموعه_استفاده_شده_received);
#پایان_اگر

#إذا defined(CONFIG_PARAM_REMOTE)
	بنية param_remote_counters counts;
	param_remote_get_counters(&counts);
	PX4_INFO("set value requests received: %" PRIu32 ", set value responses sent: %" PRIu32,
		 counts.مجموعه_مقدار_request_received, counts.مجموعه_مقدار_response_فرستاده);
	PX4_INFO("set value requests sent: %" PRIu32 ", set value responses received: %" PRIu32,
		 counts.مجموعه_مقدار_request_فرستاده, counts.مجموعه_مقدار_response_received);
	PX4_INFO("resets received: %" PRIu32 ", set used requests sent: %" PRIu32,
		 counts.reset_received, counts.مجموعه_استفاده_شده_فرستاده);
#پایان_اگر

}
