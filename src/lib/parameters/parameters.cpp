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

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PARAM_IMPLEMENTATION
#ਅਨ੍ਤਰ੍ਭਾਵ "param.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "param_translation.h"
#ਅਨ੍ਤਰ੍ਭਾਵ <parameters/px4_parameters.hpp>
#ਅਨ੍ਤਰ੍ਭਾਵ <lib/tinybson/tinybson.h>

#ਜੇ defined(__PX4_NUTTX)
#ਅਨ੍ਤਰ੍ਭਾਵ <nuttx/crc32.h>
#ਨਹੀਂਤਾਂ
#ਅਨ੍ਤਰ੍ਭਾਵ <crc32.h>
#ਜੇ_ਅੰਤ
#ਅਨ੍ਤਰ੍ਭਾਵ <float.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <math.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/stat.h>

#ਅਨ੍ਤਰ੍ਭਾਵ <containers/Bitset.hpp>
#ਅਨ੍ਤਰ੍ਭਾਵ <drivers/drv_hrt.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <lib/perf/perf_counter.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <px4_platform_common/px4_config.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <px4_platform_common/atomic_bitset.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <px4_platform_common/defines.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <px4_platform_common/posix.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <px4_platform_common/sem.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <px4_platform_common/shutdown.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <px4_platform_common/micro_hal.h>

ਵਰਤੋਂ ਨਾਮਾਕਾਸ਼ ਸਮਾਂ_literals;

#ਅਨ੍ਤਰ੍ਭਾਵ "uORB/uORB.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uORB/topics/parameter_update.h"
#ਅਨ੍ਤਰ੍ਭਾਵ <uORB/Subscription.hpp>

#ਅਨ੍ਤਰ੍ਭਾਵ "ExhaustiveLayer.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ConstLayer.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "DynamicSparseLayer.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "StaticSparseLayer.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "atomic_transaction.h"

/* Include functions common to user and kernel sides */
#ਅਨ੍ਤਰ੍ਭਾਵ "parameters_common.cpp"

#ਜੇ defined(__PX4_NUTTX) && !defined(CONFIG_BUILD_FLAT)
#ਅਨ੍ਤਰ੍ਭਾਵ <px4_platform/board_ctrl.h>
#ਅਨ੍ਤਰ੍ਭਾਵ "parameters_ioctl.h"
#ਜੇ_ਅੰਤ

#ਜੇ defined(FLASH_BASED_PARAMS)
#ਅਨ੍ਤਰ੍ਭਾਵ "flashparams/flashparams.h"
#ਨਹੀਂਤਾਂ
ਸਤਰਅੰਦਰ ਸਥਿਰ ਪੂਰਨ flash_param_save(param_filter_func filter) { ਵਾਪਸ -1; }
ਸਤਰਅੰਦਰ ਸਥਿਰ ਪੂਰਨ flash_param_import() { ਵਾਪਸ -1; }
#ਜੇ_ਅੰਤ

ਸਥਿਰ ਅੱਖਰ *param_default_file = ਖਾਲੀ_ਸੂਚਕ;
ਸਥਿਰ ਅੱਖਰ *param_backup_file = ਖਾਲੀ_ਸੂਚਕ;

#ਅਨ੍ਤਰ੍ਭਾਵ "autosave.h"
ਸਥਿਰ ParamAutosave *autosave_instance {ਖਾਲੀ_ਸੂਚਕ};

ਸਥਿਰ px4::AtomicBitset<param_info_ਗਿਣਤੀ> params_ਸਰਗਰਮ;  // params found
ਸਥਿਰ px4::AtomicBitset<param_info_ਗਿਣਤੀ> params_unsaved;

// Params the save in progress is writing. They are moved out of params_unsaved
// before their values are read, so a param_set that lands while the save runs
// marks its param again and reaches the next save. Flash-param saves write only
// unsaved params, so nothing else would ever write that change.
ਸਥਿਰ px4::AtomicBitset<param_info_ਗਿਣਤੀ> params_saving;

ਸਥਿਰ ConstLayer firmware_defaults;
ਸਥਿਰ DynamicSparseLayer runtime_defaults{&firmware_defaults};
DynamicSparseLayer user_config{&runtime_defaults};

/** parameter update topic handle */
#ਜੇ not defined(CONFIG_PARAM_REMOTE)
ਸਥਿਰ orb_advert_t param_topic = ਖਾਲੀ_ਸੂਚਕ;
ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ param_instance = 0;
#ਜੇ_ਅੰਤ

ਸਥਿਰ perf_counter_t param_export_perf;
ਸਥਿਰ perf_counter_t param_ਲੱਭੋ_perf;
ਸਥਿਰ perf_counter_t param_get_perf;
ਸਥਿਰ perf_counter_t param_ਸੈੱਟ_perf;

ਸਥਿਰ pthread_mutex_t file_mutex  =
	PTHREAD_MUTEX_INITIALIZER; ///< this protects against concurrent param saves (file or flash access).

// Support for remote parameter node
#ਜੇ defined(CONFIG_PARAM_PRIMARY)
# include "parameters_primary.h"
#ਜੇ_ਅੰਤ // CONFIG_PARAM_PRIMARY
#ਜੇ defined(CONFIG_PARAM_REMOTE)
# include "parameters_remote.h"
#ਜੇ_ਅੰਤ // CONFIG_PARAM_REMOTE

ਖਾਲੀ
param_init()
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __PX4_POSIX
	// Set up the recursive param mutex before any thread can access parameters.
	AtomicTransaction::initialize();
#ਜੇ_ਅੰਤ

	param_export_perf = perf_alloc(PC_ELAPSED, "param: export");
	param_ਲੱਭੋ_perf = perf_alloc(PC_ਗਿਣਤੀ, "param: find");
	param_get_perf = perf_alloc(PC_ਗਿਣਤੀ, "param: get");
	param_ਸੈੱਟ_perf = perf_alloc(PC_ELAPSED, "param: set");

#ਜੇ defined(__PX4_NUTTX) && !defined(CONFIG_BUILD_FLAT)
	px4_ਰਜਿਸਟਰ_boardct_ioctl(_PARAMIOCBASE, param_ioctl);
#ਜੇ_ਅੰਤ

#ਜੇ defined(CONFIG_PARAM_PRIMARY)
	param_primary_init();
#ਜੇ_ਅੰਤ // CONFIG_PARAM_PRIMARY

#ਜੇ defined(CONFIG_PARAM_REMOTE)
	param_remote_init();
#ਜੇ_ਅੰਤ // CONFIG_PARAM_REMOTE

#ਜੇ not defined(CONFIG_PARAM_REMOTE)
	autosave_instance = ਨਵਾਂ ParamAutosave();
#ਜੇ_ਅੰਤ
}


ਖਾਲੀ
param_notify_changes()
{
// Don't send if this is a remote node. Only the primary
// sends out update notices
#ਜੇ not defined(CONFIG_PARAM_REMOTE)
	parameter_update_s pup {};
	pup.instance = param_instance++;
	pup.get_ਗਿਣਤੀ_2 = perf_event_ਗਿਣਤੀ(param_get_perf);
	pup.ਸੈੱਟ_ਗਿਣਤੀ = perf_event_ਗਿਣਤੀ(param_ਸੈੱਟ_perf);
	pup.ਲੱਭੋ_ਗਿਣਤੀ = perf_event_ਗਿਣਤੀ(param_ਲੱਭੋ_perf);
	pup.export_ਗਿਣਤੀ = perf_event_ਗਿਣਤੀ(param_export_perf);
	pup.ਸਰਗਰਮ_2 = params_ਸਰਗਰਮ.ਗਿਣਤੀ_2();
	pup.ਬਦਲਿਆ = user_config.ਮਾਪ();
	pup.custom_default = runtime_defaults.ਮਾਪ();
	pup.timestamp = hrt_absolute_ਸਮਾਂ();

	ਜੇ (param_topic == ਖਾਲੀ_ਸੂਚਕ) {
		param_topic = orb_advertise(ORB_ID(parameter_update), &pup);

	} ਨਹੀਂਤਾਂ {
		orb_publish(ORB_ID(parameter_update), param_topic, &pup);
	}

#ਜੇ_ਅੰਤ
}

ਸਥਿਰ param_t param_ਲੱਭੋ_ਅੰਦਰੂਨੀ(ਅਚਲ ਅੱਖਰ *name, ਤਾਰਕਿਕ notification)
{
	perf_ਗਿਣਤੀ(param_ਲੱਭੋ_perf);

	param_t ਵਿਚਕਾਰ;
	param_t front = 0;
	param_t ਆਖਰੀ = param_info_ਗਿਣਤੀ;

	/* perform a binary search of the known parameters */

	ਜਦੋਂ (front <= ਆਖਰੀ) {
		ਵਿਚਕਾਰ = front + (ਆਖਰੀ - front) / 2;
		ਪੂਰਨ ret = strcmp(name, param_name(ਵਿਚਕਾਰ));

		ਜੇ (ret == 0) {
			ਜੇ (notification) {
				param_ਸੈੱਟ_ਵਰਤਿਆ(ਵਿਚਕਾਰ);
			}

			ਵਾਪਸ ਵਿਚਕਾਰ;

		} ਨਹੀਂਤਾਂ ਜੇ (ਵਿਚਕਾਰ == front) {
			/* An end point has been hit, but there has been no match */
			ਰੋਕੋ;

		} ਨਹੀਂਤਾਂ ਜੇ (ret < 0) {
			ਆਖਰੀ = ਵਿਚਕਾਰ;

		} ਨਹੀਂਤਾਂ {
			front = ਵਿਚਕਾਰ;
		}
	}

	/* not found */
	ਵਾਪਸ PARAM_INVALID;
}

param_t param_ਲੱਭੋ(ਅਚਲ ਅੱਖਰ *name)
{
	ਵਾਪਸ param_ਲੱਭੋ_ਅੰਦਰੂਨੀ(name, ਸੱਚ);
}

param_t param_ਲੱਭੋ_no_notification(ਅਚਲ ਅੱਖਰ *name)
{
	ਵਾਪਸ param_ਲੱਭੋ_ਅੰਦਰੂਨੀ(name, ਝੂਠ);
}

ਬਿਨਾਨਿਸ਼ਾਨੀ param_ਗਿਣਤੀ_ਵਰਤਿਆ()
{
	ਵਾਪਸ params_ਸਰਗਰਮ.ਗਿਣਤੀ_2();
}

param_t param_for_ਵਰਤਿਆ_ਸੂਚਕਾਂਕ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਸੂਚਕਾਂਕ)
{
	// walk all params and count used params
	ਜੇ (ਸੂਚਕਾਂਕ < param_info_ਗਿਣਤੀ) {
		ਬਿਨਾਨਿਸ਼ਾਨੀ ਵਰਤਿਆ_ਗਿਣਤੀ = 0;

		ਲਈ (ਪੂਰਨ i = 0; i < params_ਸਰਗਰਮ.ਮਾਪ(); i++) {
			ਜੇ (params_ਸਰਗਰਮ[i]) {
				// we found the right used count,
				//  return the param value
				ਜੇ (ਸੂਚਕਾਂਕ == ਵਰਤਿਆ_ਗਿਣਤੀ) {
					ਵਾਪਸ static_cast<param_t>(i);
				}

				ਵਰਤਿਆ_ਗਿਣਤੀ++;
			}
		}
	}

	ਵਾਪਸ PARAM_INVALID;
}

ਪੂਰਨ param_get_ਵਰਤਿਆ_ਸੂਚਕਾਂਕ(param_t param)
{
	/* this tests for out of bounds and does a constant time lookup */
	ਜੇ (!param_ਵਰਤਿਆ(param)) {
		ਵਾਪਸ -1;
	}

	/* walk all params and count, now knowing that it has a valid index */
	ਪੂਰਨ ਵਰਤਿਆ_ਗਿਣਤੀ = 0;

	ਲਈ (ਪੂਰਨ i = 0; i < params_ਸਰਗਰਮ.ਮਾਪ(); i++) {
		ਜੇ (params_ਸਰਗਰਮ[i]) {

			ਜੇ (param == i) {
				ਵਾਪਸ ਵਰਤਿਆ_ਗਿਣਤੀ;
			}

			ਵਰਤਿਆ_ਗਿਣਤੀ++;
		}
	}

	ਵਾਪਸ -1;
}

ਤਾਰਕਿਕ
param_ਮੁੱਲ_unsaved(param_t param)
{
	ਵਾਪਸ handle_ਵਿੱਚ_range(param) ? (params_unsaved[param] || params_saving[param]) : ਝੂਠ;
}

ਪੂਰਨ
param_get(param_t param, ਖਾਲੀ *val)
{
	perf_ਗਿਣਤੀ(param_get_perf);

	ਜੇ (!handle_ਵਿੱਚ_range(param)) {
		PX4_ERR("get: param %" PRId16 " invalid", param);
		ਵਾਪਸ PX4_ERROR;
	}

	ਜੇ (!params_ਸਰਗਰਮ[param]) {
		PX4_DEBUG("get: param %" PRId16 " (%s) not active", param, param_name(param));
	}

	ਪੂਰਨ ਨਤੀਜਾ = PX4_ERROR;

	ਜੇ (val) {

		ਸਵੈਚਾਲਿਤ retrieve_ਮੁੱਲ = user_config.get(param);

		ਚੋਣ (param_type(param)) {
		ਹਾਲਤ PARAM_TYPE_INT32:
			ਸਮ੍ਰਿਤੀ_ਨਕਲ(val, &retrieve_ਮੁੱਲ.i, ਆਕਾਰ(retrieve_ਮੁੱਲ.i));
			ਵਾਪਸ PX4_OK;

		ਹਾਲਤ PARAM_TYPE_FLOAT:
			ਸਮ੍ਰਿਤੀ_ਨਕਲ(val, &retrieve_ਮੁੱਲ.f, ਆਕਾਰ(retrieve_ਮੁੱਲ.f));
			ਵਾਪਸ PX4_OK;
		}
	}

	ਵਾਪਸ ਨਤੀਜਾ;
}

ਪੂਰਨ
param_get_default_ਮੁੱਲ_ਅੰਦਰੂਨੀ(param_t param, ਖਾਲੀ *default_val)
{
	ਜੇ (!handle_ਵਿੱਚ_range(param)) {
		PX4_ERR("get default value: param %d invalid", param);
		ਵਾਪਸ PX4_ERROR;
	}

	ਜੇ (default_val) {
		ਚੋਣ (param_type(param)) {
		ਹਾਲਤ PARAM_TYPE_INT32: {
				int32_t val = runtime_defaults.get(param).i;
				ਸਮ੍ਰਿਤੀ_ਨਕਲ(default_val, &val, ਆਕਾਰ(val));
				ਵਾਪਸ PX4_OK;
			}

		ਹਾਲਤ PARAM_TYPE_FLOAT: {
				ਦਸ਼ਮਲਵ val = runtime_defaults.get(param).f;
				ਸਮ੍ਰਿਤੀ_ਨਕਲ(default_val, &val, ਆਕਾਰ(val));
				ਵਾਪਸ PX4_OK;
			}
		}
	}

	ਵਾਪਸ PX4_ERROR;
}

ਪੂਰਨ
param_get_default_ਮੁੱਲ(param_t param, ਖਾਲੀ *default_val)
{
	ਜੇ (!handle_ਵਿੱਚ_range(param)) {
		ਵਾਪਸ PX4_ERROR;
	}

	ਪੂਰਨ ret = param_get_default_ਮੁੱਲ_ਅੰਦਰੂਨੀ(param, default_val);
	ਵਾਪਸ ret;
}

ਤਾਰਕਿਕ param_ਮੁੱਲ_is_default(param_t param)
{
	ਜੇ (!handle_ਵਿੱਚ_range(param)) {
		ਵਾਪਸ ਸੱਚ;
	}

	ਜੇ (!user_config.ਸ਼ਾਮਲ_ਹੈ(param)) {
		// if user config does not contain it, consider it default.
		ਵਾਪਸ ਸੱਚ;

	} ਨਹੀਂਤਾਂ {
		// compare with default value
		ਅਚਲ param_ਮੁੱਲ_u user_config_ਮੁੱਲ = user_config.get(param);
		ਅਚਲ param_ਮੁੱਲ_u runtime_default_ਮੁੱਲ = runtime_defaults.get(param);

		ਚੋਣ (param_type(param)) {
		ਹਾਲਤ PARAM_TYPE_INT32: {
				ਵਾਪਸ user_config_ਮੁੱਲ.i == runtime_default_ਮੁੱਲ.i;
			}

		ਹਾਲਤ PARAM_TYPE_FLOAT: {
				ਵਾਪਸ fabsf(user_config_ਮੁੱਲ.f - runtime_default_ਮੁੱਲ.f) < FLT_EPSILON;
			}
		}
	}

	ਵਾਪਸ ਸੱਚ;
}

ਸਥਿਰ ਖਾਲੀ
param_autosave()
{
	ਜੇ (autosave_instance) {
		autosave_instance->request();
	}
}

ਖਾਲੀ
param_control_autosave(ਤਾਰਕਿਕ enable)
{
	ਜੇ (autosave_instance) {
		autosave_instance->enable(enable);
	}
}

ਸਥਿਰ ਪੂਰਨ
param_ਸੈੱਟ_ਅੰਦਰੂਨੀ(param_t param, ਅਚਲ ਖਾਲੀ *val, ਤਾਰਕਿਕ mark_saved, ਤਾਰਕਿਕ notify_changes, ਤਾਰਕਿਕ update_remote = ਸੱਚ,
		   ਤਾਰਕਿਕ autosave = ਸੱਚ)
{
	ਜੇ (!handle_ਵਿੱਚ_range(param)) {
		PX4_ERR("set invalid param %d", param);
		ਵਾਪਸ PX4_ERROR;
	}

	ਜੇ (val == ਖਾਲੀ_ਸੂਚਕ) {
		PX4_ERR("set invalid value");
		ਵਾਪਸ PX4_ERROR;
	}

	ਜੇ (param_is_readonly(param)) {
		PX4_WARN("param %s is read-only", param_name(param));
		ਵਾਪਸ PX4_ERROR;
	}

	ਪੂਰਨ ਨਤੀਜਾ = -1;
	ਤਾਰਕਿਕ param_ਬਦਲਿਆ = ਝੂਠ;
	perf_begin(param_ਸੈੱਟ_perf);

	ਅਚਲ param_ਮੁੱਲ_u user_config_ਮੁੱਲ = user_config.get(param);
	param_ਮੁੱਲ_u new_ਮੁੱਲ{};

	ਚੋਣ (param_type(param)) {
	ਹਾਲਤ PARAM_TYPE_INT32:
		ਸਮ੍ਰਿਤੀ_ਨਕਲ(&new_ਮੁੱਲ.i, val, ਆਕਾਰ(new_ਮੁੱਲ.i));
		param_ਬਦਲਿਆ = user_config_ਮੁੱਲ.i != new_ਮੁੱਲ.i;
		ਰੋਕੋ;

	ਹਾਲਤ PARAM_TYPE_FLOAT:
		ਸਮ੍ਰਿਤੀ_ਨਕਲ(&new_ਮੁੱਲ.f, val, ਆਕਾਰ(new_ਮੁੱਲ.f));
		param_ਬਦਲਿਆ = fabsf(user_config_ਮੁੱਲ.f - new_ਮੁੱਲ.f) > FLT_EPSILON;
		ਰੋਕੋ;

	ਮੂਲ: {
			PX4_ERR("param_set invalid param type for %s", param_name(param));
			ਰੋਕੋ;
		}
	}

	ਜੇ (user_config.store(param, new_ਮੁੱਲ)) {
		ਜੇ (param_ਬਦਲਿਆ) {
			params_unsaved.ਸੈੱਟ(param, !mark_saved);
		}

		ਨਤੀਜਾ = PX4_OK;

	} ਨਹੀਂਤਾਂ {
		PX4_ERR("param_set failed to store param %s", param_name(param));
		ਨਤੀਜਾ = PX4_ERROR;
	}

	ਜੇ ((ਨਤੀਜਾ == PX4_OK) && param_ਬਦਲਿਆ && !mark_saved && autosave) { // this is false when importing parameters
		param_autosave();
	}

	// If this is the parameter server, make sure that the remote is updated
#ਜੇ defined(CONFIG_PARAM_PRIMARY)

	ਜੇ (param_ਬਦਲਿਆ && update_remote) {
		param_primary_ਸੈੱਟ_ਮੁੱਲ(param, val);
	}

#ਜੇ_ਅੰਤ

	// If this is the parameter remote, make sure that the primary is updated
#ਜੇ defined(CONFIG_PARAM_REMOTE)

	ਜੇ (param_ਬਦਲਿਆ && update_remote) {
		param_remote_ਸੈੱਟ_ਮੁੱਲ(param, val);
	}

#ਜੇ_ਅੰਤ

	perf_ਸਿਰਾ(param_ਸੈੱਟ_perf);

	/*
	 * If we set something, now that we have unlocked, go ahead and advertise that
	 * a thing has been set.
	 */
	ਜੇ ((ਨਤੀਜਾ == PX4_OK) && param_ਬਦਲਿਆ && notify_changes) {
		param_notify_changes();
	}

	ਵਾਪਸ ਨਤੀਜਾ;
}

#ਜੇ defined(FLASH_BASED_PARAMS)
ਪੂਰਨ param_ਸੈੱਟ_external(param_t param, ਅਚਲ ਖਾਲੀ *val, ਤਾਰਕਿਕ mark_saved, ਤਾਰਕਿਕ notify_changes)
{
	ਵਾਪਸ param_ਸੈੱਟ_ਅੰਦਰੂਨੀ(param, val, mark_saved, notify_changes);
}

ਖਾਲੀ param_get_external(param_t param, ਖਾਲੀ *val)
{
	param_get(param, val);
}
#ਜੇ_ਅੰਤ

ਪੂਰਨ param_ਸੈੱਟ(param_t param, ਅਚਲ ਖਾਲੀ *val)
{
	ਵਾਪਸ param_ਸੈੱਟ_ਅੰਦਰੂਨੀ(param, val, ਝੂਠ, ਸੱਚ);
}

ਪੂਰਨ param_ਸੈੱਟ_no_notification(param_t param, ਅਚਲ ਖਾਲੀ *val)
{
	ਵਾਪਸ param_ਸੈੱਟ_ਅੰਦਰੂਨੀ(param, val, ਝੂਠ, ਝੂਠ);
}

ਪੂਰਨ param_ਸੈੱਟ_no_remote_update(param_t param, ਅਚਲ ਖਾਲੀ *val, ਤਾਰਕਿਕ notify)
{
	ਵਾਪਸ param_ਸੈੱਟ_ਅੰਦਰੂਨੀ(param, val, ਝੂਠ, notify, ਝੂਠ);
}

ਤਾਰਕਿਕ param_ਵਰਤਿਆ(param_t param)
{
	ਜੇ (handle_ਵਿੱਚ_range(param)) {
		ਵਾਪਸ params_ਸਰਗਰਮ[param];
	}

	ਵਾਪਸ ਝੂਠ;
}

ਖਾਲੀ param_ਸੈੱਟ_ਵਰਤਿਆ(param_t param)
{
	ਜੇ (handle_ਵਿੱਚ_range(param)) {
#ਜੇ defined(CONFIG_PARAM_REMOTE)

		ਜੇ (!param_ਵਰਤਿਆ(param)) {
			param_remote_ਸੈੱਟ_ਵਰਤਿਆ(param);
		}

#ਜੇ_ਅੰਤ

		params_ਸਰਗਰਮ.ਸੈੱਟ(param, ਸੱਚ);
	}
}

ਪੂਰਨ param_ਸੈੱਟ_default_ਮੁੱਲ(param_t param, ਅਚਲ ਖਾਲੀ *val)
{
	ਜੇ (!handle_ਵਿੱਚ_range(param)) {
		PX4_ERR("set default value invalid param %d", param);
		ਵਾਪਸ PX4_ERROR;
	}

	ਜੇ (val == ਖਾਲੀ_ਸੂਚਕ) {
		PX4_ERR("set default value invalid value");
		ਵਾਪਸ PX4_ERROR;
	}

	ਜੇ (param_is_readonly(param)) {
		PX4_WARN("param %s is read-only", param_name(param));
		ਵਾਪਸ PX4_ERROR;
	}

	ਪੂਰਨ ਨਤੀਜਾ = PX4_ERROR;


	// check if param being set to default value
	ਤਾਰਕਿਕ setting_ਮੰਜ਼ਿਲ_static_default = ਝੂਠ;
	ਅਚਲ param_ਮੁੱਲ_u firmware_default_ਮੁੱਲ = firmware_defaults.get(param);

	ਚੋਣ (param_type(param)) {
	ਹਾਲਤ PARAM_TYPE_INT32: {
			int32_t new_ਮੁੱਲ;
			ਸਮ੍ਰਿਤੀ_ਨਕਲ(&new_ਮੁੱਲ, val, ਆਕਾਰ(new_ਮੁੱਲ));
			setting_ਮੰਜ਼ਿਲ_static_default = firmware_default_ਮੁੱਲ.i == new_ਮੁੱਲ;
			ਰੋਕੋ;
		}

	ਹਾਲਤ PARAM_TYPE_FLOAT: {
			ਦਸ਼ਮਲਵ new_ਮੁੱਲ;
			ਸਮ੍ਰਿਤੀ_ਨਕਲ(&new_ਮੁੱਲ, val, ਆਕਾਰ(new_ਮੁੱਲ));
			setting_ਮੰਜ਼ਿਲ_static_default = fabsf(firmware_default_ਮੁੱਲ.f - new_ਮੁੱਲ) <= FLT_EPSILON;
			ਰੋਕੋ;
		}
	}

	ਜੇ (setting_ਮੰਜ਼ਿਲ_static_default) {
		runtime_defaults.reset(param);

		ਨਤੀਜਾ = PX4_OK;

	} ਨਹੀਂਤਾਂ {
		param_ਮੁੱਲ_u new_ਮੁੱਲ{};

		ਚੋਣ (param_type(param)) {
		ਹਾਲਤ PARAM_TYPE_INT32: {
				ਸਮ੍ਰਿਤੀ_ਨਕਲ(&new_ਮੁੱਲ.i, val, ਆਕਾਰ(new_ਮੁੱਲ.i));
				ਰੋਕੋ;
			}

		ਹਾਲਤ PARAM_TYPE_FLOAT: {
				ਸਮ੍ਰਿਤੀ_ਨਕਲ(&new_ਮੁੱਲ.f, val, ਆਕਾਰ(new_ਮੁੱਲ.f));
				ਰੋਕੋ;
			}

		ਮੂਲ:
			ਰੋਕੋ;
		}

		ਜੇ (runtime_defaults.store(param, new_ਮੁੱਲ)) {
			user_config.refresh(param);
			ਨਤੀਜਾ = PX4_OK;

		} ਨਹੀਂਤਾਂ {
			ਨਤੀਜਾ = PX4_ERROR;
		}
	}


	ਜੇ ((ਨਤੀਜਾ == PX4_OK) && param_ਵਰਤਿਆ(param)) {
		// send notification if param is already in use
		param_notify_changes();
	}

	ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ ਪੂਰਨ param_reset_ਅੰਦਰੂਨੀ(param_t param, ਤਾਰਕਿਕ notify = ਸੱਚ, ਤਾਰਕਿਕ autosave = ਸੱਚ)
{
#ਜੇ defined(CONFIG_PARAM_REMOTE)
	// Remote doesn't support reset
	ਵਾਪਸ ਝੂਠ;
#ਜੇ_ਅੰਤ

	ਜੇ (param_is_readonly(param)) {
		ਵਾਪਸ 0;  // silently skip — param_reset_all loops over all params
	}

	ਤਾਰਕਿਕ param_ਮਿਲਿਆ = user_config.ਸ਼ਾਮਲ_ਹੈ(param);

	ਜੇ (handle_ਵਿੱਚ_range(param)) {
		user_config.reset(param);
	}

	ਜੇ (param_ਮਿਲਿਆ) {
		/* Persist the reset. Flash-param saves append only unsaved params,
		 * so a reset has to show up as a tombstone or replay keeps the
		 * previous override. */
		params_unsaved.ਸੈੱਟ(param, ਸੱਚ);
	}

	ਜੇ (autosave) {
		param_autosave();
	}

	ਜੇ (param_ਮਿਲਿਆ && notify) {
		param_notify_changes();
	}

#ਜੇ defined(CONFIG_PARAM_PRIMARY)
	param_primary_reset(param);
#ਜੇ_ਅੰਤ

	ਵਾਪਸ param_ਮਿਲਿਆ;
}

ਪੂਰਨ param_reset(param_t param) { ਵਾਪਸ param_reset_ਅੰਦਰੂਨੀ(param, ਸੱਚ); }
ਪੂਰਨ param_reset_no_notification(param_t param) { ਵਾਪਸ param_reset_ਅੰਦਰੂਨੀ(param, ਝੂਠ); }

ਸਥਿਰ ਖਾਲੀ
param_reset_all_ਅੰਦਰੂਨੀ(ਤਾਰਕਿਕ auto_save)
{
#ਜੇ defined(CONFIG_PARAM_REMOTE)
	// Remote doesn't support reset
	ਵਾਪਸ;
#ਜੇ_ਅੰਤ

	ਲਈ (param_t param = 0; handle_ਵਿੱਚ_range(param); param++) {
		param_reset_ਅੰਦਰੂਨੀ(param, ਝੂਠ, ਝੂਠ);
	}

	ਜੇ (auto_save) {
		param_autosave();
	}

#ਜੇ defined(CONFIG_PARAM_PRIMARY)
	param_primary_reset_all();
#ਜੇ_ਅੰਤ

	param_notify_changes();
}

ਖਾਲੀ
param_reset_all()
{
	param_reset_all_ਅੰਦਰੂਨੀ(ਸੱਚ);
}

ਖਾਲੀ
param_reset_excludes(ਅਚਲ ਅੱਖਰ *excludes[], ਪੂਰਨ ਅੰਕ_excludes)
{
	ਲਈ (param_t param = 0; handle_ਵਿੱਚ_range(param); param++) {
		ਅਚਲ ਅੱਖਰ *name = param_name(param);
		ਤਾਰਕਿਕ exclude = ਝੂਠ;

		ਲਈ (ਪੂਰਨ ਸੂਚਕਾਂਕ = 0; ਸੂਚਕਾਂਕ < ਅੰਕ_excludes; ਸੂਚਕਾਂਕ ++) {
			ਪੂਰਨ len = ਲੜੀ_ਲੰਬਾਈ(excludes[ਸੂਚਕਾਂਕ]);

			ਜੇ ((excludes[ਸੂਚਕਾਂਕ][len - 1] == '*'
			     && ਲੜੀ_ਤੁਲਨਾ(name, excludes[ਸੂਚਕਾਂਕ], len - 1) == 0)
			    || strcmp(name, excludes[ਸੂਚਕਾਂਕ]) == 0) {

				exclude = ਸੱਚ;
				ਰੋਕੋ;
			}
		}

		ਜੇ (!exclude) {
			param_reset(param);
		}
	}
}

ਖਾਲੀ
param_reset_specific(ਅਚਲ ਅੱਖਰ *resets[], ਪੂਰਨ ਅੰਕ_resets)
{
	ਲਈ (param_t param = 0; handle_ਵਿੱਚ_range(param); param++) {
		ਅਚਲ ਅੱਖਰ *name = param_name(param);
		ਤਾਰਕਿਕ reset = ਝੂਠ;

		ਲਈ (ਪੂਰਨ ਸੂਚਕਾਂਕ = 0; ਸੂਚਕਾਂਕ < ਅੰਕ_resets; ਸੂਚਕਾਂਕ++) {
			ਪੂਰਨ len = ਲੜੀ_ਲੰਬਾਈ(resets[ਸੂਚਕਾਂਕ]);

			ਜੇ ((resets[ਸੂਚਕਾਂਕ][len - 1] == '*'
			     && ਲੜੀ_ਤੁਲਨਾ(name, resets[ਸੂਚਕਾਂਕ], len - 1) == 0)
			    || strcmp(name, resets[ਸੂਚਕਾਂਕ]) == 0) {

				reset = ਸੱਚ;
				ਰੋਕੋ;
			}
		}

		ਜੇ (reset) {
			param_reset(param);
		}
	}
}

ਪੂਰਨ
param_ਸੈੱਟ_default_file(ਅਚਲ ਅੱਖਰ *filename)
{
	ਜੇ (filename && param_backup_file && strcmp(filename, param_backup_file) == 0) {
		PX4_ERR("default file can't be the same as the backup file %s", filename);
		ਵਾਪਸ PX4_ERROR;
	}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FLASH_BASED_PARAMS
	// the default for flash-based params is always the FLASH
	(ਖਾਲੀ)filename;
#ਨਹੀਂਤਾਂ

	ਜੇ (param_default_file != ਖਾਲੀ_ਸੂਚਕ) {
		// we assume this is not in use by some other thread
		ਮੁਕਤ(param_default_file);
		param_default_file = ਖਾਲੀ_ਸੂਚਕ;
	}

	ਜੇ (filename) {
		param_default_file = strdup(filename);
	}

#ਜੇ_ਅੰਤ /* FLASH_BASED_PARAMS */

	ਵਾਪਸ 0;
}

ਅਚਲ ਅੱਖਰ *param_get_default_file()
{
	ਵਾਪਸ param_default_file;
}

ਪੂਰਨ param_ਸੈੱਟ_backup_file(ਅਚਲ ਅੱਖਰ *filename)
{
	ਜੇ (filename && param_default_file && strcmp(filename, param_default_file) == 0) {
		PX4_ERR("backup file can't be the same as the default file %s", filename);
		ਵਾਪਸ PX4_ERROR;
	}

	ਜੇ (param_backup_file != ਖਾਲੀ_ਸੂਚਕ) {
		// we assume this is not in use by some other thread
		ਮੁਕਤ(param_backup_file);
		param_backup_file = ਖਾਲੀ_ਸੂਚਕ;
	}

	ਜੇ (filename) {
		param_backup_file = strdup(filename);

	} ਨਹੀਂਤਾਂ {
		param_backup_file = ਖਾਲੀ_ਸੂਚਕ; // backup disabled
	}

	ਵਾਪਸ 0;
}

ਅਚਲ ਅੱਖਰ *param_get_backup_file()
{
	ਵਾਪਸ param_backup_file;
}

ਸਥਿਰ ਪੂਰਨ param_export_ਅੰਦਰੂਨੀ(ਪੂਰਨ fd, param_filter_func filter);
ਸਥਿਰ ਪੂਰਨ param_verify(ਪੂਰਨ fd);

ਪੂਰਨ param_save_default(ਤਾਰਕਿਕ blocking)
{
	PX4_DEBUG("param_save_default");

	// take the file lock
	ਜੇ (blocking) {
		pthread_mutex_lock(&file_mutex);

	} ਨਹੀਂਤਾਂ {
		ਜੇ (pthread_mutex_trylock(&file_mutex) != 0) {
			PX4_DEBUG("param_save_default: file lock failed (already locked)");
			ਵਾਪਸ -EWOULDBLOCK;
		}
	}

	ਪੂਰਨ shutdown_ਤਾਲਾ_ret = px4_shutdown_ਤਾਲਾ();

	ਜੇ (shutdown_ਤਾਲਾ_ret != 0) {
		PX4_ERR("px4_shutdown_lock() failed (%i)", shutdown_ਤਾਲਾ_ret);
	}

	ਪੂਰਨ res = PX4_ERROR;
	ਅਚਲ ਅੱਖਰ *filename = param_get_default_file();

	ਲਈ (param_t param = 0; handle_ਵਿੱਚ_range(param); param++) {
		ਜੇ (params_unsaved[param]) {
			params_saving.ਸੈੱਟ(param, ਸੱਚ);
			params_unsaved.ਸੈੱਟ(param, ਝੂਠ);
		}
	}

	ਜੇ (filename) {
		ਸਥਿਰ constexpr ਪੂਰਨ MAX_ATTEMPTS = 3;

		ਲਈ (ਪੂਰਨ attempt = 1; attempt <= MAX_ATTEMPTS; attempt++) {
			// write parameters to file
			ਪੂਰਨ fd = ::open(filename, O_WRONLY | O_CREAT | O_TRUNC, PX4_O_ਬਹੁਲਕ_666);

			ਜੇ (fd > -1) {
				perf_begin(param_export_perf);
				res = param_export_ਅੰਦਰੂਨੀ(fd, ਖਾਲੀ_ਸੂਚਕ);
				perf_ਸਿਰਾ(param_export_perf);
				::close(fd);

				ਜੇ (res == PX4_OK) {
					// reopen file to verify
					ਪੂਰਨ fd_verify = ::open(filename, O_RDONLY, PX4_O_ਬਹੁਲਕ_666);
					res = param_verify(fd_verify) || lseek(fd_verify, 0, SEEK_SET) || param_verify(fd_verify);
					::close(fd_verify);
				}
			}

			ਜੇ (res == PX4_OK) {
				ਰੋਕੋ;

			} ਨਹੀਂਤਾਂ {
				PX4_ERR("parameter export to %s failed (%d) attempt %d", filename, res, attempt);
				px4_usleep(10000); // wait at least 10 milliseconds before trying again
			}
		}

	} ਨਹੀਂਤਾਂ {
		perf_begin(param_export_perf);
		res = flash_param_save(ਖਾਲੀ_ਸੂਚਕ);
		perf_ਸਿਰਾ(param_export_perf);
	}

	ਜੇ (res != PX4_OK) {
		PX4_ERR("param export failed (%d)", res);

		ਲਈ (param_t param = 0; handle_ਵਿੱਚ_range(param); param++) {
			ਜੇ (params_saving[param]) {
				params_unsaved.ਸੈੱਟ(param, ਸੱਚ);
			}
		}
	}

	params_saving.reset();

	ਜੇ (res == PX4_OK) {
		// backup file
		ਜੇ (param_backup_file) {
			ਪੂਰਨ fd_backup_file = ::open(param_backup_file, O_WRONLY | O_CREAT | O_TRUNC, PX4_O_ਬਹੁਲਕ_666);

			ਜੇ (fd_backup_file > -1) {
				ਪੂਰਨ backup_export_ret = param_export_ਅੰਦਰੂਨੀ(fd_backup_file, ਖਾਲੀ_ਸੂਚਕ);
				::close(fd_backup_file);

				ਜੇ (backup_export_ret != 0) {
					PX4_ERR("backup parameter export to %s failed (%d)", param_backup_file, backup_export_ret);

				} ਨਹੀਂਤਾਂ {
					// verify export
					ਪੂਰਨ fd_verify = ::open(param_backup_file, O_RDONLY, PX4_O_ਬਹੁਲਕ_666);
					param_verify(fd_verify);
					::close(fd_verify);
				}
			}
		}
	}

	pthread_mutex_unlock(&file_mutex);

	ਜੇ (shutdown_ਤਾਲਾ_ret == 0) {
		px4_shutdown_unlock();
	}

	ਵਾਪਸ res;
}

/**
 * @return 0 on success, 1 if all params have not yet been stored, -1 if device open failed, -2 if writing parameters failed
 */
ਪੂਰਨ
param_ਲੱਦੋ_default()
{
	ਪੂਰਨ res = 0;
	ਅਚਲ ਅੱਖਰ *filename = param_get_default_file();

	ਜੇ (!filename) {
		ਵਾਪਸ param_ਲੱਦੋ(-1);
	}

	ਪੂਰਨ fd_ਲੱਦੋ = ::open(filename, O_RDONLY);

	ਜੇ (fd_ਲੱਦੋ < 0) {
		/* no parameter file is OK, otherwise this is an error */
		ਜੇ (errno != ENOENT) {
			PX4_ERR("open '%s' for reading failed", filename);
			ਵਾਪਸ -1;
		}

		ਵਾਪਸ 1;
	}

	ਪੂਰਨ ਨਤੀਜਾ = param_ਲੱਦੋ(fd_ਲੱਦੋ);
	::close(fd_ਲੱਦੋ);

	ਜੇ (ਨਤੀਜਾ == 1) {
		/* blank file: nothing stored yet */
		ਵਾਪਸ 1;
	}

	ਜੇ (ਨਤੀਜਾ != 0) {
		PX4_ERR("error reading parameters from '%s'", filename);
		ਵਾਪਸ -2;
	}

	ਵਾਪਸ res;
}

ਸਥਿਰ ਪੂਰਨ param_verify_callback(bson_decoder_t decoder, bson_ਗੰਢ_t ਗੰਢ_2)
{
	ਜੇ (ਗੰਢ_2->type == BSON_EOO) {
		ਵਾਪਸ 0;
	}

	// find the parameter this node represents
	param_t param = param_ਲੱਭੋ_no_notification(ਗੰਢ_2->name);

	ਜੇ (param == PARAM_INVALID) {
		PX4_ERR("verify: invalid parameter '%s'", ਗੰਢ_2->name);
		ਵਾਪਸ -1;
	}

	// handle verifying the parameter from the node
	ਚੋਣ (ਗੰਢ_2->type) {
	ਹਾਲਤ BSON_INT32: {
			ਜੇ (param_type(param) != PARAM_TYPE_INT32) {
				PX4_ERR("verify: invalid param type %d for '%s' (BSON_INT32)", param_type(param), ਗੰਢ_2->name);
				ਵਾਪਸ -1;
			}

			int32_t value;

			ਜੇ (param_get(param, &value) == 0) {
				ਜੇ (value == ਗੰਢ_2->i32) {
					ਵਾਪਸ 1; // valid

				} ਨਹੀਂਤਾਂ {
					PX4_ERR("verify: '%s' invalid BSON value %" PRIi32 "(expected %" PRIi32 ")", ਗੰਢ_2->name, ਗੰਢ_2->i32, value);
				}
			}
		}
		ਰੋਕੋ;

	ਹਾਲਤ BSON_DOUBLE: {
			ਜੇ (param_type(param) != PARAM_TYPE_FLOAT) {
				PX4_ERR("verify: invalid param type %d for '%s' (BSON_DOUBLE)", param_type(param), ਗੰਢ_2->name);
				ਵਾਪਸ -1;
			}

			ਦਸ਼ਮਲਵ value;

			ਜੇ (param_get(param, &value) == 0) {
				ਜੇ (fabsf(value - (ਦਸ਼ਮਲਵ)ਗੰਢ_2->d) <= FLT_EPSILON) {
					ਵਾਪਸ 1; // valid

				} ਨਹੀਂਤਾਂ {
					PX4_ERR("verify: '%s' invalid BSON value %.3f (expected %.3f)", ਗੰਢ_2->name, ਗੰਢ_2->d, (ਦੁੱਗਣਾ)value);
				}
			}
		}
		ਰੋਕੋ;

	ਮੂਲ:
		PX4_ERR("verify: '%s' invalid node type %d", ਗੰਢ_2->name, ਗੰਢ_2->type);
	}

	ਵਾਪਸ -1;
}

ਸਥਿਰ ਪੂਰਨ param_verify(ਪੂਰਨ fd)
{
	PX4_DEBUG("param_verify");

	ਜੇ (fd < 0) {
		ਵਾਪਸ -1;
	}

	ਜੇ (lseek(fd, 0, SEEK_SET) != 0) {
		PX4_ERR("verify: seek failed");
		ਵਾਪਸ -1;
	}

	bson_decoder_s decoder{};

	ਜੇ (bson_decoder_init_file(&decoder, fd, param_verify_callback) == 0) {
		ਪੂਰਨ ਨਤੀਜਾ = -1;

		ਕਰੋ {
			ਨਤੀਜਾ = bson_decoder_ਅਗਲਾ(&decoder);

		} ਜਦੋਂ (ਨਤੀਜਾ > 0);

		ਜੇ (ਨਤੀਜਾ == 0) {
			ਜੇ (decoder.ਕੁੱਲ_document_ਮਾਪ != decoder.ਕੁੱਲ_decoded_ਮਾਪ) {
				PX4_ERR("BSON document size (%" PRId32 ") doesn't match bytes decoded (%" PRId32 ")", decoder.ਕੁੱਲ_document_ਮਾਪ,
					decoder.ਕੁੱਲ_decoded_ਮਾਪ);

			} ਨਹੀਂਤਾਂ {
				ਵਾਪਸ 0;
			}

		} ਨਹੀਂਤਾਂ ਜੇ (ਨਤੀਜਾ == -ENODATA) {
			PX4_ERR("verify: no BSON data");

		} ਨਹੀਂਤਾਂ {
			PX4_ERR("verify: failed (%d)", ਨਤੀਜਾ);
		}
	}

	ਵਾਪਸ -1;
}

ਪੂਰਨ
param_export(ਅਚਲ ਅੱਖਰ *filename, param_filter_func filter)
{
	PX4_DEBUG("param_export");

	ਪੂਰਨ shutdown_ਤਾਲਾ_ret = px4_shutdown_ਤਾਲਾ();

	ਜੇ (shutdown_ਤਾਲਾ_ret != 0) {
		PX4_ERR("px4_shutdown_lock() failed (%i)", shutdown_ਤਾਲਾ_ret);
	}

	// take the file lock
	ਜੇ (pthread_mutex_trylock(&file_mutex) != 0) {
		PX4_ERR("param_export: file lock failed (already locked)");
		ਵਾਪਸ PX4_ERROR;
	}

	ਪੂਰਨ fd = ::open(filename, O_RDWR | O_CREAT, PX4_O_ਬਹੁਲਕ_666);
	ਪੂਰਨ ਨਤੀਜਾ = PX4_ERROR;

	perf_begin(param_export_perf);

	ਜੇ (fd > -1) {
		ਨਤੀਜਾ = param_export_ਅੰਦਰੂਨੀ(fd, filter);

	} ਨਹੀਂਤਾਂ {
		ਨਤੀਜਾ = flash_param_save(filter);
	}

	perf_ਸਿਰਾ(param_export_perf);

	pthread_mutex_unlock(&file_mutex);

	ਜੇ (shutdown_ਤਾਲਾ_ret == 0) {
		px4_shutdown_unlock();
	}

	ਵਾਪਸ ਨਤੀਜਾ;
}

// internal parameter export, caller is responsible for locking
ਸਥਿਰ ਪੂਰਨ param_export_ਅੰਦਰੂਨੀ(ਪੂਰਨ fd, param_filter_func filter)
{
	PX4_DEBUG("param_export_internal");
	ਅਚਲ ਸਵੈਚਾਲਿਤ ਬਦਲਿਆ_params = user_config.containedAsBitset();

	ਪੂਰਨ ਨਤੀਜਾ = -1;
	bson_encoder_s encoder{};
	uint8_t bson_ਬਫਰ[256];

	ਜੇ (lseek(fd, 0, SEEK_SET) != 0) {
		PX4_ERR("export seek failed %d", errno);
		ਵਾਪਸ -1;
	}

	ਜੇ (bson_encoder_init_buf_file(&encoder, fd, &bson_ਬਫਰ, ਆਕਾਰ(bson_ਬਫਰ)) != 0) {
		ਜਾਓ ਬਾਹਰਲਾ;
	}

	ਲਈ (param_t param = 0; handle_ਵਿੱਚ_range(param); param++) {
		ਜੇ (!ਬਦਲਿਆ_params[param] || (filter && !filter(param))) {
			ਜਾਰੀ;
		}

		ਅਚਲ param_ਮੁੱਲ_u runtime_default_ਮੁੱਲ = runtime_defaults.get(param);
		ਅਚਲ param_ਮੁੱਲ_u user_config_ਮੁੱਲ = user_config.get(param);

		// don't export default values
		ਚੋਣ (param_type(param)) {
		ਹਾਲਤ PARAM_TYPE_INT32:
			ਜੇ (user_config_ਮੁੱਲ.i == runtime_default_ਮੁੱਲ.i) {
				PX4_DEBUG("skipping %s %" PRIi32 " export", param_name(param), runtime_default_ਮੁੱਲ.i);
				ਜਾਰੀ;
			}

			ਰੋਕੋ;

		ਹਾਲਤ PARAM_TYPE_FLOAT:
			ਜੇ (fabsf(user_config_ਮੁੱਲ.f - runtime_default_ਮੁੱਲ.f) <= FLT_EPSILON) {
				PX4_DEBUG("skipping %s %.3f export", param_name(param), (ਦੁੱਗਣਾ)runtime_default_ਮੁੱਲ.f);
				ਜਾਰੀ;
			}

			ਰੋਕੋ;
		}

		ਅਚਲ ਅੱਖਰ *name = param_name(param);
		ਅਚਲ size_t ਮਾਪ = param_ਮਾਪ(param);

		/* append the appropriate BSON type object */
		ਚੋਣ (param_type(param)) {
		ਹਾਲਤ PARAM_TYPE_INT32: {
				ਅਚਲ int32_t i = user_config_ਮੁੱਲ.i;
				PX4_DEBUG("exporting: %s (%d) size: %lu val: %" PRIi32, name, param, (ਲੰਮਾ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)ਮਾਪ, i);

				ਜੇ (bson_encoder_append_int32(&encoder, name, i) != 0) {
					PX4_ERR("BSON append failed for '%s'", name);
					ਜਾਓ ਬਾਹਰਲਾ;
				}
			}
			ਰੋਕੋ;

		ਹਾਲਤ PARAM_TYPE_FLOAT: {
				ਅਚਲ ਦੁੱਗਣਾ f = (ਦੁੱਗਣਾ)user_config_ਮੁੱਲ.f;
				PX4_DEBUG("exporting: %s (%d) size: %lu val: %.3f", name, param, (ਲੰਮਾ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)ਮਾਪ, (ਦੁੱਗਣਾ)f);

				ਜੇ (bson_encoder_append_double(&encoder, name, f) != 0) {
					PX4_ERR("BSON append failed for '%s'", name);
					ਜਾਓ ਬਾਹਰਲਾ;
				}
			}
			ਰੋਕੋ;

		ਮੂਲ:
			PX4_ERR("%s unrecognized parameter type %d, skipping export", name, param_type(param));
		}
	}

	ਨਤੀਜਾ = 0;

ਬਾਹਰਲਾ:

	ਜੇ (ਨਤੀਜਾ == 0) {
		ਜੇ (bson_encoder_fini(&encoder) != PX4_OK) {
			PX4_ERR("BSON encoder finalize failed");
			ਨਤੀਜਾ = -1;
		}
	}

	ਵਾਪਸ ਨਤੀਜਾ;
}

#ਜੇ defined(FLASH_BASED_PARAMS)
// A file is never the flash backend's store, so what an import changes is unsaved until a delta carries it.
// As on the file backend, the import itself does not save.
ਸਥਿਰ constexpr ਤਾਰਕਿਕ file_import_is_saved = ਝੂਠ;
#ਨਹੀਂਤਾਂ
ਸਥਿਰ constexpr ਤਾਰਕਿਕ file_import_is_saved = ਸੱਚ;
#ਜੇ_ਅੰਤ

ਸਥਿਰ ਪੂਰਨ
param_import_callback(bson_decoder_t decoder, bson_ਗੰਢ_t ਗੰਢ_2)
{
	/*
	 * EOO means the end of the parameter object. (Currently not supporting
	 * nested BSON objects).
	 */
	ਜੇ (ਗੰਢ_2->type == BSON_EOO) {
		PX4_DEBUG("end of parameters");
		ਵਾਪਸ 0;
	}

	// A tombstone has no value. The decoder leaves the previous node's in the union and the translations
	// read it without a type check.
	ਅਚਲ ਤਾਰਕਿਕ tombstone = ਗੰਢ_2->type == BSON_nullptr || ਗੰਢ_2->type == BSON_UNDEFINED;

	// if we do param_set() directly in the translation, set PARAM_SKIP_IMPORT as return value and return here
	ਜੇ (!tombstone && param_modify_on_import(ਗੰਢ_2) == param_modify_on_import_ret::PARAM_SKIP_IMPORT) {
		ਵਾਪਸ 1;
	}

	// Find the parameter this node represents.  If we don't know it, ignore the node.
	param_t param = param_ਲੱਭੋ_no_notification(ਗੰਢ_2->name);

	ਜੇ (param == PARAM_INVALID) {
		PX4_WARN("ignoring unrecognised parameter '%s'", ਗੰਢ_2->name);
		ਵਾਪਸ 1;
	}

	// Handle setting the parameter from the node
	ਚੋਣ (ਗੰਢ_2->type) {
	ਹਾਲਤ BSON_nullptr:
	ਹਾਲਤ BSON_UNDEFINED:
		param_reset_ਅੰਦਰੂਨੀ(param, ਸੱਚ, ਝੂਠ);
		ਵਾਪਸ 1;

	ਹਾਲਤ BSON_INT32: {
			ਜੇ (param_type(param) == PARAM_TYPE_INT32) {
				int32_t i = ਗੰਢ_2->i32;
				param_ਸੈੱਟ_ਅੰਦਰੂਨੀ(param, &i, file_import_is_saved, ਸੱਚ, ਸੱਚ, ਝੂਠ);
				PX4_DEBUG("Imported %s with value %" PRIi32, param_name(param), i);

			} ਨਹੀਂਤਾਂ {
				PX4_WARN("unexpected type for %s", ਗੰਢ_2->name);
			}
		}
		ਰੋਕੋ;

	ਹਾਲਤ BSON_DOUBLE: {
			ਜੇ (param_type(param) == PARAM_TYPE_FLOAT) {
				ਦਸ਼ਮਲਵ f = ਗੰਢ_2->d;
				param_ਸੈੱਟ_ਅੰਦਰੂਨੀ(param, &f, file_import_is_saved, ਸੱਚ, ਸੱਚ, ਝੂਠ);
				PX4_DEBUG("Imported %s with value %f", param_name(param), (ਦੁੱਗਣਾ)f);

			} ਨਹੀਂਤਾਂ {
				PX4_WARN("unexpected type for %s", ਗੰਢ_2->name);
			}
		}
		ਰੋਕੋ;

	ਮੂਲ:
		PX4_ERR("import: unrecognised node type for '%s'", ਗੰਢ_2->name);
	}

	// don't return zero, that means EOF
	ਵਾਪਸ 1;
}

ਸਥਿਰ ਪੂਰਨ
param_import_ਅੰਦਰੂਨੀ(ਪੂਰਨ fd)
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
	ਬਣਤਰ stat st;

	ਜੇ (fstat(fd, &st) == 0 && S_ISREG(st.st_mode) && st.st_size == 0) {
		ਵਾਪਸ 1;
	}

	int32_t doc_ਮਾਪ = 0;
	ssize_t len = ਪਠ(fd, &doc_ਮਾਪ, ਆਕਾਰ(doc_ਮਾਪ));

	ਜੇ (len == 0 || (len == (ssize_t)ਆਕਾਰ(doc_ਮਾਪ) && (doc_ਮਾਪ == 0 || doc_ਮਾਪ == -1))) {
		ਵਾਪਸ 1;
	}

	ਜੇ (lseek(fd, 0, SEEK_SET) != 0) {
		PX4_ERR("import lseek failed (%d)", errno);
		ਵਾਪਸ -1;
	}

	ਸਥਿਰ constexpr ਪੂਰਨ MAX_ATTEMPTS = 3;

	ਲਈ (ਪੂਰਨ attempt = 1; attempt <= MAX_ATTEMPTS; attempt++) {
		bson_decoder_s decoder{};

		ਜੇ (bson_decoder_init_file(&decoder, fd, param_import_callback) == 0) {
			ਪੂਰਨ ਨਤੀਜਾ = -1;

			ਕਰੋ {
				ਨਤੀਜਾ = bson_decoder_ਅਗਲਾ(&decoder);

			} ਜਦੋਂ (ਨਤੀਜਾ > 0);

			ਜੇ (ਨਤੀਜਾ == 0) {
				ਜੇ (decoder.ਕੁੱਲ_document_ਮਾਪ == decoder.ਕੁੱਲ_decoded_ਮਾਪ) {
					PX4_INFO("BSON document size %" PRId32 " bytes, decoded %" PRId32 " bytes (INT32:%" PRIu16 ", FLOAT:%" PRIu16 ")",
						 decoder.ਕੁੱਲ_document_ਮਾਪ, decoder.ਕੁੱਲ_decoded_ਮਾਪ,
						 decoder.ਗਿਣਤੀ_ਗੰਢ_int32, decoder.ਗਿਣਤੀ_ਗੰਢ_double);

					ਵਾਪਸ 0;

				} ਨਹੀਂਤਾਂ {
					PX4_ERR("BSON document size (%" PRId32 ") doesn't match bytes decoded (%" PRId32 ")",
						decoder.ਕੁੱਲ_document_ਮਾਪ, decoder.ਕੁੱਲ_decoded_ਮਾਪ);
				}

			} ਨਹੀਂਤਾਂ ਜੇ (ਨਤੀਜਾ == -ENODATA) {
				// silently retry as a precaution unless this is our last attempt
				ਜੇ (attempt == MAX_ATTEMPTS) {
					PX4_DEBUG("BSON: no data");
					ਵਾਪਸ 0;
				}

			} ਨਹੀਂਤਾਂ {
				PX4_ERR("param import failed (%d) attempt %d", ਨਤੀਜਾ, attempt);
			}

		} ਨਹੀਂਤਾਂ {
			PX4_ERR("param import bson decoder init failed (attempt %d)", attempt);
		}

		ਜੇ (attempt != MAX_ATTEMPTS) {
			ਜੇ (lseek(fd, 0, SEEK_SET) != 0) {
				PX4_ERR("import lseek failed (%d)", errno);
			}

			px4_usleep(10000); // wait at least 10 milliseconds before trying again
		}
	}

	ਵਾਪਸ -1;
}

#ਜੇ defined(FLASH_BASED_PARAMS)
ਸਥਿਰ ਪੂਰਨ flash_backend_locked(ਪੂਰਨ (*op)())
{
	/* Same order as param_save_default: mutex then shutdown lock. Autosave
	 * uses a trylock, so a compact holds it off the flash programming. */
	pthread_mutex_lock(&file_mutex);

	ਪੂਰਨ shutdown_ਤਾਲਾ_ret = px4_shutdown_ਤਾਲਾ();

	ਜੇ (shutdown_ਤਾਲਾ_ret != 0) {
		PX4_ERR("px4_shutdown_lock() failed (%i)", shutdown_ਤਾਲਾ_ret);
	}

	ਪੂਰਨ ਨਤੀਜਾ = op();

	pthread_mutex_unlock(&file_mutex);

	ਜੇ (shutdown_ਤਾਲਾ_ret == 0) {
		px4_shutdown_unlock();
	}

	ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ ਪੂਰਨ flash_ਲੱਦੋ_locked()
{
	/* Reset under the lock so a pending autosave cannot append tombstones
	 * of the just-cleared RAM before import/compact run. */
	param_reset_all_ਅੰਦਰੂਨੀ(ਝੂਠ);
	ਪੂਰਨ ਨਤੀਜਾ = flash_param_import();

	ਜੇ (ਨਤੀਜਾ == 0 || ਨਤੀਜਾ == 1) {
		params_unsaved.reset();
	}

	ਵਾਪਸ ਨਤੀਜਾ;
}
#ਜੇ_ਅੰਤ

ਪੂਰਨ
param_import(ਪੂਰਨ fd)
{
	ਜੇ (fd < 0) {
#ਜੇ defined(FLASH_BASED_PARAMS)
		ਵਾਪਸ flash_backend_locked(flash_param_import);
#ਨਹੀਂਤਾਂ
		ਵਾਪਸ flash_param_import();
#ਜੇ_ਅੰਤ
	}

	ਵਾਪਸ param_import_ਅੰਦਰੂਨੀ(fd);
}

ਪੂਰਨ
param_ਲੱਦੋ(ਪੂਰਨ fd)
{
	ਜੇ (fd < 0) {
#ਜੇ defined(FLASH_BASED_PARAMS)
		ਵਾਪਸ flash_backend_locked(flash_ਲੱਦੋ_locked);
#ਨਹੀਂਤਾਂ
		ਵਾਪਸ PX4_ERROR;
#ਜੇ_ਅੰਤ
	}

	param_reset_all_ਅੰਦਰੂਨੀ(ਝੂਠ);
	ਪੂਰਨ ਨਤੀਜਾ = param_import_ਅੰਦਰੂਨੀ(fd);

#ਜੇ !defined(FLASH_BASED_PARAMS)

	ਜੇ (ਨਤੀਜਾ >= 0) {
		params_unsaved.reset();
	}

#ਜੇ_ਅੰਤ

	ਵਾਪਸ ਨਤੀਜਾ;
}

ਖਾਲੀ
param_foreach(ਖਾਲੀ (*func)(ਖਾਲੀ *arg, param_t param), ਖਾਲੀ *arg, ਤਾਰਕਿਕ only_ਬਦਲਿਆ, ਤਾਰਕਿਕ only_ਵਰਤਿਆ)
{
	param_t	param;

	ਲਈ (param = 0; handle_ਵਿੱਚ_range(param); param++) {

		/* if requested, skip unchanged values */
		ਜੇ (only_ਬਦਲਿਆ && (!user_config.ਸ਼ਾਮਲ_ਹੈ(param))) {
			ਜਾਰੀ;
		}

		ਜੇ (only_ਵਰਤਿਆ && !param_ਵਰਤਿਆ(param)) {
			ਜਾਰੀ;
		}

		func(arg, param);
	}
}

uint32_t param_hash_check()
{
	uint32_t param_hash = 0;

	/* compute the CRC32 over all string param names and 4 byte values */
	ਲਈ (param_t param = 0; handle_ਵਿੱਚ_range(param); param++) {
		ਜੇ (!param_ਵਰਤਿਆ(param) || param_is_volatile(param)) {
			ਜਾਰੀ;
		}

		ਅਚਲ ਅੱਖਰ *name = param_name(param);
		ਸਵੈਚਾਲਿਤ value = user_config.get(param).i;
		ਅਚਲ ਖਾਲੀ *val = (ਖਾਲੀ *)&value;
		param_hash = crc32part((ਅਚਲ uint8_t *)name, ਲੜੀ_ਲੰਬਾਈ(name), param_hash);
		param_hash = crc32part((ਅਚਲ uint8_t *)val, param_ਮਾਪ(param), param_hash);
	}

	ਵਾਪਸ param_hash;
}

ਖਾਲੀ param_print_ਸਥਿਤੀ()
{
	PX4_INFO("summary: %d/%d (used/total)", param_ਗਿਣਤੀ_ਵਰਤਿਆ(), param_ਗਿਣਤੀ());

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ FLASH_BASED_PARAMS
	ਅਚਲ ਅੱਖਰ *filename = param_get_default_file();

	ਜੇ (filename != ਖਾਲੀ_ਸੂਚਕ) {
		PX4_INFO("file: %s", param_get_default_file());
	}

	ਜੇ (param_backup_file) {
		PX4_INFO("backup file: %s", param_backup_file);
	}

#ਜੇ_ਅੰਤ /* FLASH_BASED_PARAMS */

	PX4_INFO("storage array: %d/%d elements (%zu bytes total)",
		 user_config.ਮਾਪ(), firmware_defaults.ਮਾਪ(), (size_t)user_config.byte_ਮਾਪ());


	PX4_INFO("storage array (custom defaults): %d/%d elements (%zu bytes total)",
		 runtime_defaults.ਮਾਪ(), firmware_defaults.ਮਾਪ(), (size_t)runtime_defaults.byte_ਮਾਪ());

	ਜੇ (autosave_instance) {
		PX4_INFO("auto save: %s", autosave_instance->enabled() ? "on" : "off");

		hrt_abstime ਆਖਰੀ_autosave = autosave_instance->ਆਖਰੀ_Autosave();

		ਜੇ (ਆਖਰੀ_autosave > 0) {
			PX4_INFO("last auto save: %.3f seconds ago", hrt_elapsed_ਸਮਾਂ(&ਆਖਰੀ_autosave) * 1e-6);
		}
	}

	perf_print_counter(param_export_perf);
	perf_print_counter(param_ਲੱਭੋ_perf);
	perf_print_counter(param_get_perf);
	perf_print_counter(param_ਸੈੱਟ_perf);

#ਜੇ defined(CONFIG_PARAM_PRIMARY)
	ਬਣਤਰ param_primary_counters counts;
	param_primary_get_counters(&counts);
	PX4_INFO("set value requests received: %" PRIu32 ", set value responses sent: %" PRIu32,
		 counts.ਸੈੱਟ_ਮੁੱਲ_request_received, counts.ਸੈੱਟ_ਮੁੱਲ_response_ਭੇਜਿਆ);
	PX4_INFO("set value requests sent: %" PRIu32 ", set value responses received: %" PRIu32,
		 counts.ਸੈੱਟ_ਮੁੱਲ_request_ਭੇਜਿਆ, counts.ਸੈੱਟ_ਮੁੱਲ_response_received);
	PX4_INFO("resets sent: %" PRIu32 ", set used requests received: %" PRIu32,
		 counts.reset_ਭੇਜਿਆ, counts.ਸੈੱਟ_ਵਰਤਿਆ_received);
#ਜੇ_ਅੰਤ

#ਜੇ defined(CONFIG_PARAM_REMOTE)
	ਬਣਤਰ param_remote_counters counts;
	param_remote_get_counters(&counts);
	PX4_INFO("set value requests received: %" PRIu32 ", set value responses sent: %" PRIu32,
		 counts.ਸੈੱਟ_ਮੁੱਲ_request_received, counts.ਸੈੱਟ_ਮੁੱਲ_response_ਭੇਜਿਆ);
	PX4_INFO("set value requests sent: %" PRIu32 ", set value responses received: %" PRIu32,
		 counts.ਸੈੱਟ_ਮੁੱਲ_request_ਭੇਜਿਆ, counts.ਸੈੱਟ_ਮੁੱਲ_response_received);
	PX4_INFO("resets received: %" PRIu32 ", set used requests sent: %" PRIu32,
		 counts.reset_received, counts.ਸੈੱਟ_ਵਰਤਿਆ_ਭੇਜਿਆ);
#ਜੇ_ਅੰਤ

}
