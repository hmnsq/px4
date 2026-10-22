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
#അന്തര്ഭാവ "param.h"
#അന്തര്ഭാവ "param_translation.h"
#അന്തര്ഭാവ <parameters/px4_parameters.hpp>
#അന്തര്ഭാവ <lib/tinybson/tinybson.h>

#എങ്കിൽ defined(__PX4_NUTTX)
#അന്തര്ഭാവ <nuttx/crc32.h>
#അല്ലെങ്കിൽ
#അന്തര്ഭാവ <crc32.h>
#endif
#അന്തര്ഭാവ <float.h>
#അന്തര്ഭാവ <math.h>
#അന്തര്ഭാവ <sys/stat.h>

#അന്തര്ഭാവ <containers/Bitset.hpp>
#അന്തര്ഭാവ <drivers/drv_hrt.h>
#അന്തര്ഭാവ <lib/perf/perf_counter.h>
#അന്തര്ഭാവ <px4_platform_common/px4_config.h>
#അന്തര്ഭാവ <px4_platform_common/atomic_bitset.h>
#അന്തര്ഭാവ <px4_platform_common/defines.h>
#അന്തര്ഭാവ <px4_platform_common/posix.h>
#അന്തര്ഭാവ <px4_platform_common/sem.h>
#അന്തര്ഭാവ <px4_platform_common/shutdown.h>
#അന്തര്ഭാവ <px4_platform_common/micro_hal.h>

using നാമാകാശ time_literals;

#അന്തര്ഭാവ "uORB/uORB.h"
#അന്തര്ഭാവ "uORB/topics/parameter_update.h"
#അന്തര്ഭാവ <uORB/Subscription.hpp>

#അന്തര്ഭാവ "ExhaustiveLayer.h"
#അന്തര്ഭാവ "ConstLayer.h"
#അന്തര്ഭാവ "DynamicSparseLayer.h"
#അന്തര്ഭാവ "StaticSparseLayer.h"

#അന്തര്ഭാവ "atomic_transaction.h"

/* Include functions common to user and kernel sides */
#അന്തര്ഭാവ "parameters_common.cpp"

#എങ്കിൽ defined(__PX4_NUTTX) && !defined(CONFIG_BUILD_FLAT)
#അന്തര്ഭാവ <px4_platform/board_ctrl.h>
#അന്തര്ഭാവ "parameters_ioctl.h"
#endif

#എങ്കിൽ defined(FLASH_BASED_PARAMS)
#അന്തര്ഭാവ "flashparams/flashparams.h"
#അല്ലെങ്കിൽ
വരിയിൽ സ്ഥിതം പൂർണ്ണസംഖ്യ flash_param_save(param_filter_func filter) { തിരികെ -1; }
വരിയിൽ സ്ഥിതം പൂർണ്ണസംഖ്യ flash_param_import() { തിരികെ -1; }
#endif

സ്ഥിതം അക്ഷരം *param_default_file = nullptr;
സ്ഥിതം അക്ഷരം *param_backup_file = nullptr;

#അന്തര്ഭാവ "autosave.h"
സ്ഥിതം ParamAutosave *autosave_instance {nullptr};

സ്ഥിതം px4::AtomicBitset<param_info_എണ്ണം> params_active;  // params found
സ്ഥിതം px4::AtomicBitset<param_info_എണ്ണം> params_unsaved;

// Params the save in progress is writing. They are moved out of params_unsaved
// before their values are read, so a param_set that lands while the save runs
// marks its param again and reaches the next save. Flash-param saves write only
// unsaved params, so nothing else would ever write that change.
സ്ഥിതം px4::AtomicBitset<param_info_എണ്ണം> params_saving;

സ്ഥിതം ConstLayer firmware_defaults;
സ്ഥിതം DynamicSparseLayer runtime_defaults{&firmware_defaults};
DynamicSparseLayer user_config{&runtime_defaults};

/** parameter update topic handle */
#എങ്കിൽ not defined(CONFIG_PARAM_REMOTE)
സ്ഥിതം orb_advert_t param_topic = nullptr;
സ്ഥിതം unsigned പൂർണ്ണസംഖ്യ param_instance = 0;
#endif

സ്ഥിതം perf_counter_t param_export_perf;
സ്ഥിതം perf_counter_t param_find_perf;
സ്ഥിതം perf_counter_t param_get_perf;
സ്ഥിതം perf_counter_t param_set_perf;

സ്ഥിതം pthread_mutex_t file_mutex  =
	PTHREAD_MUTEX_INITIALIZER; ///< this protects against concurrent param saves (file or flash access).

// Support for remote parameter node
#എങ്കിൽ defined(CONFIG_PARAM_PRIMARY)
# include "parameters_primary.h"
#endif // CONFIG_PARAM_PRIMARY
#എങ്കിൽ defined(CONFIG_PARAM_REMOTE)
# include "parameters_remote.h"
#endif // CONFIG_PARAM_REMOTE

ശൂന്യം
param_init()
{
#ifdef __PX4_POSIX
	// Set up the recursive param mutex before any thread can access parameters.
	AtomicTransaction::initialize();
#endif

	param_export_perf = perf_alloc(PC_ELAPSED, "param: export");
	param_find_perf = perf_alloc(PC_എണ്ണം, "param: find");
	param_get_perf = perf_alloc(PC_എണ്ണം, "param: get");
	param_set_perf = perf_alloc(PC_ELAPSED, "param: set");

#എങ്കിൽ defined(__PX4_NUTTX) && !defined(CONFIG_BUILD_FLAT)
	px4_register_boardct_ioctl(_PARAMIOCBASE, param_ioctl);
#endif

#എങ്കിൽ defined(CONFIG_PARAM_PRIMARY)
	param_primary_init();
#endif // CONFIG_PARAM_PRIMARY

#എങ്കിൽ defined(CONFIG_PARAM_REMOTE)
	param_remote_init();
#endif // CONFIG_PARAM_REMOTE

#എങ്കിൽ not defined(CONFIG_PARAM_REMOTE)
	autosave_instance = പുതിയ ParamAutosave();
#endif
}


ശൂന്യം
param_notify_changes()
{
// Don't send if this is a remote node. Only the primary
// sends out update notices
#എങ്കിൽ not defined(CONFIG_PARAM_REMOTE)
	parameter_update_s pup {};
	pup.instance = param_instance++;
	pup.get_എണ്ണം_2 = perf_event_എണ്ണം(param_get_perf);
	pup.set_എണ്ണം = perf_event_എണ്ണം(param_set_perf);
	pup.find_എണ്ണം = perf_event_എണ്ണം(param_find_perf);
	pup.export_എണ്ണം = perf_event_എണ്ണം(param_export_perf);
	pup.active = params_active.എണ്ണം();
	pup.changed = user_config.size();
	pup.custom_default = runtime_defaults.size();
	pup.timestamp = hrt_absolute_time();

	എങ്കിൽ (param_topic == nullptr) {
		param_topic = orb_advertise(ORB_ID(parameter_update), &pup);

	} അല്ലെങ്കിൽ {
		orb_publish(ORB_ID(parameter_update), param_topic, &pup);
	}

#endif
}

സ്ഥിതം param_t param_find_internal(സ്ഥിരം അക്ഷരം *name, തർക്കം notification)
{
	perf_എണ്ണം(param_find_perf);

	param_t നടുവ്;
	param_t front = 0;
	param_t last = param_info_എണ്ണം;

	/* perform a binary search of the known parameters */

	ആകുമ്പോൾ (front <= last) {
		നടുവ് = front + (last - front) / 2;
		പൂർണ്ണസംഖ്യ ret = strcmp(name, param_name(നടുവ്));

		എങ്കിൽ (ret == 0) {
			എങ്കിൽ (notification) {
				param_set_used(നടുവ്);
			}

			തിരികെ നടുവ്;

		} അല്ലെങ്കിൽ എങ്കിൽ (നടുവ് == front) {
			/* An end point has been hit, but there has been no match */
			നിർത്തുക;

		} അല്ലെങ്കിൽ എങ്കിൽ (ret < 0) {
			last = നടുവ്;

		} അല്ലെങ്കിൽ {
			front = നടുവ്;
		}
	}

	/* not found */
	തിരികെ PARAM_INVALID;
}

param_t param_find(സ്ഥിരം അക്ഷരം *name)
{
	തിരികെ param_find_internal(name, സത്യം);
}

param_t param_find_no_notification(സ്ഥിരം അക്ഷരം *name)
{
	തിരികെ param_find_internal(name, അസത്യം);
}

unsigned param_എണ്ണം_used()
{
	തിരികെ params_active.എണ്ണം();
}

param_t param_for_used_index(unsigned index)
{
	// walk all params and count used params
	എങ്കിൽ (index < param_info_എണ്ണം) {
		unsigned used_എണ്ണം = 0;

		വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < params_active.size(); i++) {
			എങ്കിൽ (params_active[i]) {
				// we found the right used count,
				//  return the param value
				എങ്കിൽ (index == used_എണ്ണം) {
					തിരികെ static_cast<param_t>(i);
				}

				used_എണ്ണം++;
			}
		}
	}

	തിരികെ PARAM_INVALID;
}

പൂർണ്ണസംഖ്യ param_get_used_index(param_t param)
{
	/* this tests for out of bounds and does a constant time lookup */
	എങ്കിൽ (!param_used(param)) {
		തിരികെ -1;
	}

	/* walk all params and count, now knowing that it has a valid index */
	പൂർണ്ണസംഖ്യ used_എണ്ണം = 0;

	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < params_active.size(); i++) {
		എങ്കിൽ (params_active[i]) {

			എങ്കിൽ (param == i) {
				തിരികെ used_എണ്ണം;
			}

			used_എണ്ണം++;
		}
	}

	തിരികെ -1;
}

തർക്കം
param_മൂല്യം_unsaved(param_t param)
{
	തിരികെ handle_in_range(param) ? (params_unsaved[param] || params_saving[param]) : അസത്യം;
}

പൂർണ്ണസംഖ്യ
param_get(param_t param, ശൂന്യം *val)
{
	perf_എണ്ണം(param_get_perf);

	എങ്കിൽ (!handle_in_range(param)) {
		PX4_ERR("get: param %" PRId16 " invalid", param);
		തിരികെ PX4_ERROR;
	}

	എങ്കിൽ (!params_active[param]) {
		PX4_DEBUG("get: param %" PRId16 " (%s) not active", param, param_name(param));
	}

	പൂർണ്ണസംഖ്യ result = PX4_ERROR;

	എങ്കിൽ (val) {

		auto retrieve_മൂല്യം = user_config.get(param);

		തിരഞ്ഞെടുപ്പ് (param_type(param)) {
		അവസ്ഥ PARAM_TYPE_INT32:
			memcpy(val, &retrieve_മൂല്യം.i, വലുപ്പം(retrieve_മൂല്യം.i));
			തിരികെ PX4_OK;

		അവസ്ഥ PARAM_TYPE_FLOAT:
			memcpy(val, &retrieve_മൂല്യം.f, വലുപ്പം(retrieve_മൂല്യം.f));
			തിരികെ PX4_OK;
		}
	}

	തിരികെ result;
}

പൂർണ്ണസംഖ്യ
param_get_default_മൂല്യം_internal(param_t param, ശൂന്യം *default_val)
{
	എങ്കിൽ (!handle_in_range(param)) {
		PX4_ERR("get default value: param %d invalid", param);
		തിരികെ PX4_ERROR;
	}

	എങ്കിൽ (default_val) {
		തിരഞ്ഞെടുപ്പ് (param_type(param)) {
		അവസ്ഥ PARAM_TYPE_INT32: {
				int32_t val = runtime_defaults.get(param).i;
				memcpy(default_val, &val, വലുപ്പം(val));
				തിരികെ PX4_OK;
			}

		അവസ്ഥ PARAM_TYPE_FLOAT: {
				ദശാംശം val = runtime_defaults.get(param).f;
				memcpy(default_val, &val, വലുപ്പം(val));
				തിരികെ PX4_OK;
			}
		}
	}

	തിരികെ PX4_ERROR;
}

പൂർണ്ണസംഖ്യ
param_get_default_മൂല്യം(param_t param, ശൂന്യം *default_val)
{
	എങ്കിൽ (!handle_in_range(param)) {
		തിരികെ PX4_ERROR;
	}

	പൂർണ്ണസംഖ്യ ret = param_get_default_മൂല്യം_internal(param, default_val);
	തിരികെ ret;
}

തർക്കം param_മൂല്യം_is_default(param_t param)
{
	എങ്കിൽ (!handle_in_range(param)) {
		തിരികെ സത്യം;
	}

	എങ്കിൽ (!user_config.contains(param)) {
		// if user config does not contain it, consider it default.
		തിരികെ സത്യം;

	} അല്ലെങ്കിൽ {
		// compare with default value
		സ്ഥിരം param_മൂല്യം_u user_config_മൂല്യം = user_config.get(param);
		സ്ഥിരം param_മൂല്യം_u runtime_default_മൂല്യം = runtime_defaults.get(param);

		തിരഞ്ഞെടുപ്പ് (param_type(param)) {
		അവസ്ഥ PARAM_TYPE_INT32: {
				തിരികെ user_config_മൂല്യം.i == runtime_default_മൂല്യം.i;
			}

		അവസ്ഥ PARAM_TYPE_FLOAT: {
				തിരികെ fabsf(user_config_മൂല്യം.f - runtime_default_മൂല്യം.f) < FLT_EPSILON;
			}
		}
	}

	തിരികെ സത്യം;
}

സ്ഥിതം ശൂന്യം
param_autosave()
{
	എങ്കിൽ (autosave_instance) {
		autosave_instance->request();
	}
}

ശൂന്യം
param_control_autosave(തർക്കം enable)
{
	എങ്കിൽ (autosave_instance) {
		autosave_instance->enable(enable);
	}
}

സ്ഥിതം പൂർണ്ണസംഖ്യ
param_set_internal(param_t param, സ്ഥിരം ശൂന്യം *val, തർക്കം mark_saved, തർക്കം notify_changes, തർക്കം update_remote = സത്യം,
		   തർക്കം autosave = സത്യം)
{
	എങ്കിൽ (!handle_in_range(param)) {
		PX4_ERR("set invalid param %d", param);
		തിരികെ PX4_ERROR;
	}

	എങ്കിൽ (val == nullptr) {
		PX4_ERR("set invalid value");
		തിരികെ PX4_ERROR;
	}

	എങ്കിൽ (param_is_readonly(param)) {
		PX4_WARN("param %s is read-only", param_name(param));
		തിരികെ PX4_ERROR;
	}

	പൂർണ്ണസംഖ്യ result = -1;
	തർക്കം param_changed = അസത്യം;
	perf_begin(param_set_perf);

	സ്ഥിരം param_മൂല്യം_u user_config_മൂല്യം = user_config.get(param);
	param_മൂല്യം_u new_മൂല്യം{};

	തിരഞ്ഞെടുപ്പ് (param_type(param)) {
	അവസ്ഥ PARAM_TYPE_INT32:
		memcpy(&new_മൂല്യം.i, val, വലുപ്പം(new_മൂല്യം.i));
		param_changed = user_config_മൂല്യം.i != new_മൂല്യം.i;
		നിർത്തുക;

	അവസ്ഥ PARAM_TYPE_FLOAT:
		memcpy(&new_മൂല്യം.f, val, വലുപ്പം(new_മൂല്യം.f));
		param_changed = fabsf(user_config_മൂല്യം.f - new_മൂല്യം.f) > FLT_EPSILON;
		നിർത്തുക;

	സ്ഥിരസ്ഥിതി: {
			PX4_ERR("param_set invalid param type for %s", param_name(param));
			നിർത്തുക;
		}
	}

	എങ്കിൽ (user_config.store(param, new_മൂല്യം)) {
		എങ്കിൽ (param_changed) {
			params_unsaved.set(param, !mark_saved);
		}

		result = PX4_OK;

	} അല്ലെങ്കിൽ {
		PX4_ERR("param_set failed to store param %s", param_name(param));
		result = PX4_ERROR;
	}

	എങ്കിൽ ((result == PX4_OK) && param_changed && !mark_saved && autosave) { // this is false when importing parameters
		param_autosave();
	}

	// If this is the parameter server, make sure that the remote is updated
#എങ്കിൽ defined(CONFIG_PARAM_PRIMARY)

	എങ്കിൽ (param_changed && update_remote) {
		param_primary_set_മൂല്യം(param, val);
	}

#endif

	// If this is the parameter remote, make sure that the primary is updated
#എങ്കിൽ defined(CONFIG_PARAM_REMOTE)

	എങ്കിൽ (param_changed && update_remote) {
		param_remote_set_മൂല്യം(param, val);
	}

#endif

	perf_end(param_set_perf);

	/*
	 * If we set something, now that we have unlocked, go ahead and advertise that
	 * a thing has been set.
	 */
	എങ്കിൽ ((result == PX4_OK) && param_changed && notify_changes) {
		param_notify_changes();
	}

	തിരികെ result;
}

#എങ്കിൽ defined(FLASH_BASED_PARAMS)
പൂർണ്ണസംഖ്യ param_set_external(param_t param, സ്ഥിരം ശൂന്യം *val, തർക്കം mark_saved, തർക്കം notify_changes)
{
	തിരികെ param_set_internal(param, val, mark_saved, notify_changes);
}

ശൂന്യം param_get_external(param_t param, ശൂന്യം *val)
{
	param_get(param, val);
}
#endif

പൂർണ്ണസംഖ്യ param_set(param_t param, സ്ഥിരം ശൂന്യം *val)
{
	തിരികെ param_set_internal(param, val, അസത്യം, സത്യം);
}

പൂർണ്ണസംഖ്യ param_set_no_notification(param_t param, സ്ഥിരം ശൂന്യം *val)
{
	തിരികെ param_set_internal(param, val, അസത്യം, അസത്യം);
}

പൂർണ്ണസംഖ്യ param_set_no_remote_update(param_t param, സ്ഥിരം ശൂന്യം *val, തർക്കം notify)
{
	തിരികെ param_set_internal(param, val, അസത്യം, notify, അസത്യം);
}

തർക്കം param_used(param_t param)
{
	എങ്കിൽ (handle_in_range(param)) {
		തിരികെ params_active[param];
	}

	തിരികെ അസത്യം;
}

ശൂന്യം param_set_used(param_t param)
{
	എങ്കിൽ (handle_in_range(param)) {
#എങ്കിൽ defined(CONFIG_PARAM_REMOTE)

		എങ്കിൽ (!param_used(param)) {
			param_remote_set_used(param);
		}

#endif

		params_active.set(param, സത്യം);
	}
}

പൂർണ്ണസംഖ്യ param_set_default_മൂല്യം(param_t param, സ്ഥിരം ശൂന്യം *val)
{
	എങ്കിൽ (!handle_in_range(param)) {
		PX4_ERR("set default value invalid param %d", param);
		തിരികെ PX4_ERROR;
	}

	എങ്കിൽ (val == nullptr) {
		PX4_ERR("set default value invalid value");
		തിരികെ PX4_ERROR;
	}

	എങ്കിൽ (param_is_readonly(param)) {
		PX4_WARN("param %s is read-only", param_name(param));
		തിരികെ PX4_ERROR;
	}

	പൂർണ്ണസംഖ്യ result = PX4_ERROR;


	// check if param being set to default value
	തർക്കം setting_to_static_default = അസത്യം;
	സ്ഥിരം param_മൂല്യം_u firmware_default_മൂല്യം = firmware_defaults.get(param);

	തിരഞ്ഞെടുപ്പ് (param_type(param)) {
	അവസ്ഥ PARAM_TYPE_INT32: {
			int32_t new_മൂല്യം;
			memcpy(&new_മൂല്യം, val, വലുപ്പം(new_മൂല്യം));
			setting_to_static_default = firmware_default_മൂല്യം.i == new_മൂല്യം;
			നിർത്തുക;
		}

	അവസ്ഥ PARAM_TYPE_FLOAT: {
			ദശാംശം new_മൂല്യം;
			memcpy(&new_മൂല്യം, val, വലുപ്പം(new_മൂല്യം));
			setting_to_static_default = fabsf(firmware_default_മൂല്യം.f - new_മൂല്യം) <= FLT_EPSILON;
			നിർത്തുക;
		}
	}

	എങ്കിൽ (setting_to_static_default) {
		runtime_defaults.reset(param);

		result = PX4_OK;

	} അല്ലെങ്കിൽ {
		param_മൂല്യം_u new_മൂല്യം{};

		തിരഞ്ഞെടുപ്പ് (param_type(param)) {
		അവസ്ഥ PARAM_TYPE_INT32: {
				memcpy(&new_മൂല്യം.i, val, വലുപ്പം(new_മൂല്യം.i));
				നിർത്തുക;
			}

		അവസ്ഥ PARAM_TYPE_FLOAT: {
				memcpy(&new_മൂല്യം.f, val, വലുപ്പം(new_മൂല്യം.f));
				നിർത്തുക;
			}

		സ്ഥിരസ്ഥിതി:
			നിർത്തുക;
		}

		എങ്കിൽ (runtime_defaults.store(param, new_മൂല്യം)) {
			user_config.refresh(param);
			result = PX4_OK;

		} അല്ലെങ്കിൽ {
			result = PX4_ERROR;
		}
	}


	എങ്കിൽ ((result == PX4_OK) && param_used(param)) {
		// send notification if param is already in use
		param_notify_changes();
	}

	തിരികെ result;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ param_reset_internal(param_t param, തർക്കം notify = സത്യം, തർക്കം autosave = സത്യം)
{
#എങ്കിൽ defined(CONFIG_PARAM_REMOTE)
	// Remote doesn't support reset
	തിരികെ അസത്യം;
#endif

	എങ്കിൽ (param_is_readonly(param)) {
		തിരികെ 0;  // silently skip — param_reset_all loops over all params
	}

	തർക്കം param_found = user_config.contains(param);

	എങ്കിൽ (handle_in_range(param)) {
		user_config.reset(param);
	}

	എങ്കിൽ (param_found) {
		/* Persist the reset. Flash-param saves append only unsaved params,
		 * so a reset has to show up as a tombstone or replay keeps the
		 * previous override. */
		params_unsaved.set(param, സത്യം);
	}

	എങ്കിൽ (autosave) {
		param_autosave();
	}

	എങ്കിൽ (param_found && notify) {
		param_notify_changes();
	}

#എങ്കിൽ defined(CONFIG_PARAM_PRIMARY)
	param_primary_reset(param);
#endif

	തിരികെ param_found;
}

പൂർണ്ണസംഖ്യ param_reset(param_t param) { തിരികെ param_reset_internal(param, സത്യം); }
പൂർണ്ണസംഖ്യ param_reset_no_notification(param_t param) { തിരികെ param_reset_internal(param, അസത്യം); }

സ്ഥിതം ശൂന്യം
param_reset_all_internal(തർക്കം auto_save)
{
#എങ്കിൽ defined(CONFIG_PARAM_REMOTE)
	// Remote doesn't support reset
	തിരികെ;
#endif

	വേണ്ടി (param_t param = 0; handle_in_range(param); param++) {
		param_reset_internal(param, അസത്യം, അസത്യം);
	}

	എങ്കിൽ (auto_save) {
		param_autosave();
	}

#എങ്കിൽ defined(CONFIG_PARAM_PRIMARY)
	param_primary_reset_all();
#endif

	param_notify_changes();
}

ശൂന്യം
param_reset_all()
{
	param_reset_all_internal(സത്യം);
}

ശൂന്യം
param_reset_excludes(സ്ഥിരം അക്ഷരം *excludes[], പൂർണ്ണസംഖ്യ അക്കം_excludes)
{
	വേണ്ടി (param_t param = 0; handle_in_range(param); param++) {
		സ്ഥിരം അക്ഷരം *name = param_name(param);
		തർക്കം exclude = അസത്യം;

		വേണ്ടി (പൂർണ്ണസംഖ്യ index = 0; index < അക്കം_excludes; index ++) {
			പൂർണ്ണസംഖ്യ len = strlen(excludes[index]);

			എങ്കിൽ ((excludes[index][len - 1] == '*'
			     && strncmp(name, excludes[index], len - 1) == 0)
			    || strcmp(name, excludes[index]) == 0) {

				exclude = സത്യം;
				നിർത്തുക;
			}
		}

		എങ്കിൽ (!exclude) {
			param_reset(param);
		}
	}
}

ശൂന്യം
param_reset_specific(സ്ഥിരം അക്ഷരം *resets[], പൂർണ്ണസംഖ്യ അക്കം_resets)
{
	വേണ്ടി (param_t param = 0; handle_in_range(param); param++) {
		സ്ഥിരം അക്ഷരം *name = param_name(param);
		തർക്കം reset = അസത്യം;

		വേണ്ടി (പൂർണ്ണസംഖ്യ index = 0; index < അക്കം_resets; index++) {
			പൂർണ്ണസംഖ്യ len = strlen(resets[index]);

			എങ്കിൽ ((resets[index][len - 1] == '*'
			     && strncmp(name, resets[index], len - 1) == 0)
			    || strcmp(name, resets[index]) == 0) {

				reset = സത്യം;
				നിർത്തുക;
			}
		}

		എങ്കിൽ (reset) {
			param_reset(param);
		}
	}
}

പൂർണ്ണസംഖ്യ
param_set_default_file(സ്ഥിരം അക്ഷരം *filename)
{
	എങ്കിൽ (filename && param_backup_file && strcmp(filename, param_backup_file) == 0) {
		PX4_ERR("default file can't be the same as the backup file %s", filename);
		തിരികെ PX4_ERROR;
	}

#ifdef FLASH_BASED_PARAMS
	// the default for flash-based params is always the FLASH
	(ശൂന്യം)filename;
#അല്ലെങ്കിൽ

	എങ്കിൽ (param_default_file != nullptr) {
		// we assume this is not in use by some other thread
		സ്വതന്ത്രം(param_default_file);
		param_default_file = nullptr;
	}

	എങ്കിൽ (filename) {
		param_default_file = strdup(filename);
	}

#endif /* FLASH_BASED_PARAMS */

	തിരികെ 0;
}

സ്ഥിരം അക്ഷരം *param_get_default_file()
{
	തിരികെ param_default_file;
}

പൂർണ്ണസംഖ്യ param_set_backup_file(സ്ഥിരം അക്ഷരം *filename)
{
	എങ്കിൽ (filename && param_default_file && strcmp(filename, param_default_file) == 0) {
		PX4_ERR("backup file can't be the same as the default file %s", filename);
		തിരികെ PX4_ERROR;
	}

	എങ്കിൽ (param_backup_file != nullptr) {
		// we assume this is not in use by some other thread
		സ്വതന്ത്രം(param_backup_file);
		param_backup_file = nullptr;
	}

	എങ്കിൽ (filename) {
		param_backup_file = strdup(filename);

	} അല്ലെങ്കിൽ {
		param_backup_file = nullptr; // backup disabled
	}

	തിരികെ 0;
}

സ്ഥിരം അക്ഷരം *param_get_backup_file()
{
	തിരികെ param_backup_file;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ param_export_internal(പൂർണ്ണസംഖ്യ fd, param_filter_func filter);
സ്ഥിതം പൂർണ്ണസംഖ്യ param_verify(പൂർണ്ണസംഖ്യ fd);

പൂർണ്ണസംഖ്യ param_save_default(തർക്കം blocking)
{
	PX4_DEBUG("param_save_default");

	// take the file lock
	എങ്കിൽ (blocking) {
		pthread_mutex_lock(&file_mutex);

	} അല്ലെങ്കിൽ {
		എങ്കിൽ (pthread_mutex_trylock(&file_mutex) != 0) {
			PX4_DEBUG("param_save_default: file lock failed (already locked)");
			തിരികെ -EWOULDBLOCK;
		}
	}

	പൂർണ്ണസംഖ്യ shutdown_lock_ret = px4_shutdown_lock();

	എങ്കിൽ (shutdown_lock_ret != 0) {
		PX4_ERR("px4_shutdown_lock() failed (%i)", shutdown_lock_ret);
	}

	പൂർണ്ണസംഖ്യ res = PX4_ERROR;
	സ്ഥിരം അക്ഷരം *filename = param_get_default_file();

	വേണ്ടി (param_t param = 0; handle_in_range(param); param++) {
		എങ്കിൽ (params_unsaved[param]) {
			params_saving.set(param, സത്യം);
			params_unsaved.set(param, അസത്യം);
		}
	}

	എങ്കിൽ (filename) {
		സ്ഥിതം constexpr പൂർണ്ണസംഖ്യ MAX_ATTEMPTS = 3;

		വേണ്ടി (പൂർണ്ണസംഖ്യ attempt = 1; attempt <= MAX_ATTEMPTS; attempt++) {
			// write parameters to file
			പൂർണ്ണസംഖ്യ fd = ::open(filename, O_WRONLY | O_CREAT | O_TRUNC, PX4_O_MODE_666);

			എങ്കിൽ (fd > -1) {
				perf_begin(param_export_perf);
				res = param_export_internal(fd, nullptr);
				perf_end(param_export_perf);
				::close(fd);

				എങ്കിൽ (res == PX4_OK) {
					// reopen file to verify
					പൂർണ്ണസംഖ്യ fd_verify = ::open(filename, O_RDONLY, PX4_O_MODE_666);
					res = param_verify(fd_verify) || lseek(fd_verify, 0, SEEK_SET) || param_verify(fd_verify);
					::close(fd_verify);
				}
			}

			എങ്കിൽ (res == PX4_OK) {
				നിർത്തുക;

			} അല്ലെങ്കിൽ {
				PX4_ERR("parameter export to %s failed (%d) attempt %d", filename, res, attempt);
				px4_usleep(10000); // wait at least 10 milliseconds before trying again
			}
		}

	} അല്ലെങ്കിൽ {
		perf_begin(param_export_perf);
		res = flash_param_save(nullptr);
		perf_end(param_export_perf);
	}

	എങ്കിൽ (res != PX4_OK) {
		PX4_ERR("param export failed (%d)", res);

		വേണ്ടി (param_t param = 0; handle_in_range(param); param++) {
			എങ്കിൽ (params_saving[param]) {
				params_unsaved.set(param, സത്യം);
			}
		}
	}

	params_saving.reset();

	എങ്കിൽ (res == PX4_OK) {
		// backup file
		എങ്കിൽ (param_backup_file) {
			പൂർണ്ണസംഖ്യ fd_backup_file = ::open(param_backup_file, O_WRONLY | O_CREAT | O_TRUNC, PX4_O_MODE_666);

			എങ്കിൽ (fd_backup_file > -1) {
				പൂർണ്ണസംഖ്യ backup_export_ret = param_export_internal(fd_backup_file, nullptr);
				::close(fd_backup_file);

				എങ്കിൽ (backup_export_ret != 0) {
					PX4_ERR("backup parameter export to %s failed (%d)", param_backup_file, backup_export_ret);

				} അല്ലെങ്കിൽ {
					// verify export
					പൂർണ്ണസംഖ്യ fd_verify = ::open(param_backup_file, O_RDONLY, PX4_O_MODE_666);
					param_verify(fd_verify);
					::close(fd_verify);
				}
			}
		}
	}

	pthread_mutex_unlock(&file_mutex);

	എങ്കിൽ (shutdown_lock_ret == 0) {
		px4_shutdown_unlock();
	}

	തിരികെ res;
}

/**
 * @return 0 on success, 1 if all params have not yet been stored, -1 if device open failed, -2 if writing parameters failed
 */
പൂർണ്ണസംഖ്യ
param_load_default()
{
	പൂർണ്ണസംഖ്യ res = 0;
	സ്ഥിരം അക്ഷരം *filename = param_get_default_file();

	എങ്കിൽ (!filename) {
		തിരികെ param_load(-1);
	}

	പൂർണ്ണസംഖ്യ fd_load = ::open(filename, O_RDONLY);

	എങ്കിൽ (fd_load < 0) {
		/* no parameter file is OK, otherwise this is an error */
		എങ്കിൽ (errno != ENOENT) {
			PX4_ERR("open '%s' for reading failed", filename);
			തിരികെ -1;
		}

		തിരികെ 1;
	}

	പൂർണ്ണസംഖ്യ result = param_load(fd_load);
	::close(fd_load);

	എങ്കിൽ (result == 1) {
		/* blank file: nothing stored yet */
		തിരികെ 1;
	}

	എങ്കിൽ (result != 0) {
		PX4_ERR("error reading parameters from '%s'", filename);
		തിരികെ -2;
	}

	തിരികെ res;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ param_verify_callback(bson_decoder_t decoder, bson_കണ്ണി_t കണ്ണി_2)
{
	എങ്കിൽ (കണ്ണി_2->type == BSON_EOO) {
		തിരികെ 0;
	}

	// find the parameter this node represents
	param_t param = param_find_no_notification(കണ്ണി_2->name);

	എങ്കിൽ (param == PARAM_INVALID) {
		PX4_ERR("verify: invalid parameter '%s'", കണ്ണി_2->name);
		തിരികെ -1;
	}

	// handle verifying the parameter from the node
	തിരഞ്ഞെടുപ്പ് (കണ്ണി_2->type) {
	അവസ്ഥ BSON_INT32: {
			എങ്കിൽ (param_type(param) != PARAM_TYPE_INT32) {
				PX4_ERR("verify: invalid param type %d for '%s' (BSON_INT32)", param_type(param), കണ്ണി_2->name);
				തിരികെ -1;
			}

			int32_t value;

			എങ്കിൽ (param_get(param, &value) == 0) {
				എങ്കിൽ (value == കണ്ണി_2->i32) {
					തിരികെ 1; // valid

				} അല്ലെങ്കിൽ {
					PX4_ERR("verify: '%s' invalid BSON value %" PRIi32 "(expected %" PRIi32 ")", കണ്ണി_2->name, കണ്ണി_2->i32, value);
				}
			}
		}
		നിർത്തുക;

	അവസ്ഥ BSON_DOUBLE: {
			എങ്കിൽ (param_type(param) != PARAM_TYPE_FLOAT) {
				PX4_ERR("verify: invalid param type %d for '%s' (BSON_DOUBLE)", param_type(param), കണ്ണി_2->name);
				തിരികെ -1;
			}

			ദശാംശം value;

			എങ്കിൽ (param_get(param, &value) == 0) {
				എങ്കിൽ (fabsf(value - (ദശാംശം)കണ്ണി_2->d) <= FLT_EPSILON) {
					തിരികെ 1; // valid

				} അല്ലെങ്കിൽ {
					PX4_ERR("verify: '%s' invalid BSON value %.3f (expected %.3f)", കണ്ണി_2->name, കണ്ണി_2->d, (ഇരട്ട)value);
				}
			}
		}
		നിർത്തുക;

	സ്ഥിരസ്ഥിതി:
		PX4_ERR("verify: '%s' invalid node type %d", കണ്ണി_2->name, കണ്ണി_2->type);
	}

	തിരികെ -1;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ param_verify(പൂർണ്ണസംഖ്യ fd)
{
	PX4_DEBUG("param_verify");

	എങ്കിൽ (fd < 0) {
		തിരികെ -1;
	}

	എങ്കിൽ (lseek(fd, 0, SEEK_SET) != 0) {
		PX4_ERR("verify: seek failed");
		തിരികെ -1;
	}

	bson_decoder_s decoder{};

	എങ്കിൽ (bson_decoder_init_file(&decoder, fd, param_verify_callback) == 0) {
		പൂർണ്ണസംഖ്യ result = -1;

		ചെയ്യുക {
			result = bson_decoder_next(&decoder);

		} ആകുമ്പോൾ (result > 0);

		എങ്കിൽ (result == 0) {
			എങ്കിൽ (decoder.total_document_size != decoder.total_decoded_size) {
				PX4_ERR("BSON document size (%" PRId32 ") doesn't match bytes decoded (%" PRId32 ")", decoder.total_document_size,
					decoder.total_decoded_size);

			} അല്ലെങ്കിൽ {
				തിരികെ 0;
			}

		} അല്ലെങ്കിൽ എങ്കിൽ (result == -ENODATA) {
			PX4_ERR("verify: no BSON data");

		} അല്ലെങ്കിൽ {
			PX4_ERR("verify: failed (%d)", result);
		}
	}

	തിരികെ -1;
}

പൂർണ്ണസംഖ്യ
param_export(സ്ഥിരം അക്ഷരം *filename, param_filter_func filter)
{
	PX4_DEBUG("param_export");

	പൂർണ്ണസംഖ്യ shutdown_lock_ret = px4_shutdown_lock();

	എങ്കിൽ (shutdown_lock_ret != 0) {
		PX4_ERR("px4_shutdown_lock() failed (%i)", shutdown_lock_ret);
	}

	// take the file lock
	എങ്കിൽ (pthread_mutex_trylock(&file_mutex) != 0) {
		PX4_ERR("param_export: file lock failed (already locked)");
		തിരികെ PX4_ERROR;
	}

	പൂർണ്ണസംഖ്യ fd = ::open(filename, O_RDWR | O_CREAT, PX4_O_MODE_666);
	പൂർണ്ണസംഖ്യ result = PX4_ERROR;

	perf_begin(param_export_perf);

	എങ്കിൽ (fd > -1) {
		result = param_export_internal(fd, filter);

	} അല്ലെങ്കിൽ {
		result = flash_param_save(filter);
	}

	perf_end(param_export_perf);

	pthread_mutex_unlock(&file_mutex);

	എങ്കിൽ (shutdown_lock_ret == 0) {
		px4_shutdown_unlock();
	}

	തിരികെ result;
}

// internal parameter export, caller is responsible for locking
സ്ഥിതം പൂർണ്ണസംഖ്യ param_export_internal(പൂർണ്ണസംഖ്യ fd, param_filter_func filter)
{
	PX4_DEBUG("param_export_internal");
	സ്ഥിരം auto changed_params = user_config.containedAsBitset();

	പൂർണ്ണസംഖ്യ result = -1;
	bson_encoder_s encoder{};
	uint8_t bson_buffer[256];

	എങ്കിൽ (lseek(fd, 0, SEEK_SET) != 0) {
		PX4_ERR("export seek failed %d", errno);
		തിരികെ -1;
	}

	എങ്കിൽ (bson_encoder_init_buf_file(&encoder, fd, &bson_buffer, വലുപ്പം(bson_buffer)) != 0) {
		പോകുക out;
	}

	വേണ്ടി (param_t param = 0; handle_in_range(param); param++) {
		എങ്കിൽ (!changed_params[param] || (filter && !filter(param))) {
			തുടരുക;
		}

		സ്ഥിരം param_മൂല്യം_u runtime_default_മൂല്യം = runtime_defaults.get(param);
		സ്ഥിരം param_മൂല്യം_u user_config_മൂല്യം = user_config.get(param);

		// don't export default values
		തിരഞ്ഞെടുപ്പ് (param_type(param)) {
		അവസ്ഥ PARAM_TYPE_INT32:
			എങ്കിൽ (user_config_മൂല്യം.i == runtime_default_മൂല്യം.i) {
				PX4_DEBUG("skipping %s %" PRIi32 " export", param_name(param), runtime_default_മൂല്യം.i);
				തുടരുക;
			}

			നിർത്തുക;

		അവസ്ഥ PARAM_TYPE_FLOAT:
			എങ്കിൽ (fabsf(user_config_മൂല്യം.f - runtime_default_മൂല്യം.f) <= FLT_EPSILON) {
				PX4_DEBUG("skipping %s %.3f export", param_name(param), (ഇരട്ട)runtime_default_മൂല്യം.f);
				തുടരുക;
			}

			നിർത്തുക;
		}

		സ്ഥിരം അക്ഷരം *name = param_name(param);
		സ്ഥിരം size_t size = param_size(param);

		/* append the appropriate BSON type object */
		തിരഞ്ഞെടുപ്പ് (param_type(param)) {
		അവസ്ഥ PARAM_TYPE_INT32: {
				സ്ഥിരം int32_t i = user_config_മൂല്യം.i;
				PX4_DEBUG("exporting: %s (%d) size: %lu val: %" PRIi32, name, param, (long unsigned പൂർണ്ണസംഖ്യ)size, i);

				എങ്കിൽ (bson_encoder_append_int32(&encoder, name, i) != 0) {
					PX4_ERR("BSON append failed for '%s'", name);
					പോകുക out;
				}
			}
			നിർത്തുക;

		അവസ്ഥ PARAM_TYPE_FLOAT: {
				സ്ഥിരം ഇരട്ട f = (ഇരട്ട)user_config_മൂല്യം.f;
				PX4_DEBUG("exporting: %s (%d) size: %lu val: %.3f", name, param, (long unsigned പൂർണ്ണസംഖ്യ)size, (ഇരട്ട)f);

				എങ്കിൽ (bson_encoder_append_double(&encoder, name, f) != 0) {
					PX4_ERR("BSON append failed for '%s'", name);
					പോകുക out;
				}
			}
			നിർത്തുക;

		സ്ഥിരസ്ഥിതി:
			PX4_ERR("%s unrecognized parameter type %d, skipping export", name, param_type(param));
		}
	}

	result = 0;

out:

	എങ്കിൽ (result == 0) {
		എങ്കിൽ (bson_encoder_fini(&encoder) != PX4_OK) {
			PX4_ERR("BSON encoder finalize failed");
			result = -1;
		}
	}

	തിരികെ result;
}

#എങ്കിൽ defined(FLASH_BASED_PARAMS)
// A file is never the flash backend's store, so what an import changes is unsaved until a delta carries it.
// As on the file backend, the import itself does not save.
സ്ഥിതം constexpr തർക്കം file_import_is_saved = അസത്യം;
#അല്ലെങ്കിൽ
സ്ഥിതം constexpr തർക്കം file_import_is_saved = സത്യം;
#endif

സ്ഥിതം പൂർണ്ണസംഖ്യ
param_import_callback(bson_decoder_t decoder, bson_കണ്ണി_t കണ്ണി_2)
{
	/*
	 * EOO means the end of the parameter object. (Currently not supporting
	 * nested BSON objects).
	 */
	എങ്കിൽ (കണ്ണി_2->type == BSON_EOO) {
		PX4_DEBUG("end of parameters");
		തിരികെ 0;
	}

	// A tombstone has no value. The decoder leaves the previous node's in the union and the translations
	// read it without a type check.
	സ്ഥിരം തർക്കം tombstone = കണ്ണി_2->type == BSON_nullptr || കണ്ണി_2->type == BSON_UNDEFINED;

	// if we do param_set() directly in the translation, set PARAM_SKIP_IMPORT as return value and return here
	എങ്കിൽ (!tombstone && param_modify_on_import(കണ്ണി_2) == param_modify_on_import_ret::PARAM_SKIP_IMPORT) {
		തിരികെ 1;
	}

	// Find the parameter this node represents.  If we don't know it, ignore the node.
	param_t param = param_find_no_notification(കണ്ണി_2->name);

	എങ്കിൽ (param == PARAM_INVALID) {
		PX4_WARN("ignoring unrecognised parameter '%s'", കണ്ണി_2->name);
		തിരികെ 1;
	}

	// Handle setting the parameter from the node
	തിരഞ്ഞെടുപ്പ് (കണ്ണി_2->type) {
	അവസ്ഥ BSON_nullptr:
	അവസ്ഥ BSON_UNDEFINED:
		param_reset_internal(param, സത്യം, അസത്യം);
		തിരികെ 1;

	അവസ്ഥ BSON_INT32: {
			എങ്കിൽ (param_type(param) == PARAM_TYPE_INT32) {
				int32_t i = കണ്ണി_2->i32;
				param_set_internal(param, &i, file_import_is_saved, സത്യം, സത്യം, അസത്യം);
				PX4_DEBUG("Imported %s with value %" PRIi32, param_name(param), i);

			} അല്ലെങ്കിൽ {
				PX4_WARN("unexpected type for %s", കണ്ണി_2->name);
			}
		}
		നിർത്തുക;

	അവസ്ഥ BSON_DOUBLE: {
			എങ്കിൽ (param_type(param) == PARAM_TYPE_FLOAT) {
				ദശാംശം f = കണ്ണി_2->d;
				param_set_internal(param, &f, file_import_is_saved, സത്യം, സത്യം, അസത്യം);
				PX4_DEBUG("Imported %s with value %f", param_name(param), (ഇരട്ട)f);

			} അല്ലെങ്കിൽ {
				PX4_WARN("unexpected type for %s", കണ്ണി_2->name);
			}
		}
		നിർത്തുക;

	സ്ഥിരസ്ഥിതി:
		PX4_ERR("import: unrecognised node type for '%s'", കണ്ണി_2->name);
	}

	// don't return zero, that means EOF
	തിരികെ 1;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ
param_import_internal(പൂർണ്ണസംഖ്യ fd)
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
	ഘടന stat st;

	എങ്കിൽ (fstat(fd, &st) == 0 && S_ISREG(st.st_mode) && st.st_size == 0) {
		തിരികെ 1;
	}

	int32_t doc_size = 0;
	ssize_t len = പഠ(fd, &doc_size, വലുപ്പം(doc_size));

	എങ്കിൽ (len == 0 || (len == (ssize_t)വലുപ്പം(doc_size) && (doc_size == 0 || doc_size == -1))) {
		തിരികെ 1;
	}

	എങ്കിൽ (lseek(fd, 0, SEEK_SET) != 0) {
		PX4_ERR("import lseek failed (%d)", errno);
		തിരികെ -1;
	}

	സ്ഥിതം constexpr പൂർണ്ണസംഖ്യ MAX_ATTEMPTS = 3;

	വേണ്ടി (പൂർണ്ണസംഖ്യ attempt = 1; attempt <= MAX_ATTEMPTS; attempt++) {
		bson_decoder_s decoder{};

		എങ്കിൽ (bson_decoder_init_file(&decoder, fd, param_import_callback) == 0) {
			പൂർണ്ണസംഖ്യ result = -1;

			ചെയ്യുക {
				result = bson_decoder_next(&decoder);

			} ആകുമ്പോൾ (result > 0);

			എങ്കിൽ (result == 0) {
				എങ്കിൽ (decoder.total_document_size == decoder.total_decoded_size) {
					PX4_INFO("BSON document size %" PRId32 " bytes, decoded %" PRId32 " bytes (INT32:%" PRIu16 ", FLOAT:%" PRIu16 ")",
						 decoder.total_document_size, decoder.total_decoded_size,
						 decoder.എണ്ണം_കണ്ണി_int32, decoder.എണ്ണം_കണ്ണി_double);

					തിരികെ 0;

				} അല്ലെങ്കിൽ {
					PX4_ERR("BSON document size (%" PRId32 ") doesn't match bytes decoded (%" PRId32 ")",
						decoder.total_document_size, decoder.total_decoded_size);
				}

			} അല്ലെങ്കിൽ എങ്കിൽ (result == -ENODATA) {
				// silently retry as a precaution unless this is our last attempt
				എങ്കിൽ (attempt == MAX_ATTEMPTS) {
					PX4_DEBUG("BSON: no data");
					തിരികെ 0;
				}

			} അല്ലെങ്കിൽ {
				PX4_ERR("param import failed (%d) attempt %d", result, attempt);
			}

		} അല്ലെങ്കിൽ {
			PX4_ERR("param import bson decoder init failed (attempt %d)", attempt);
		}

		എങ്കിൽ (attempt != MAX_ATTEMPTS) {
			എങ്കിൽ (lseek(fd, 0, SEEK_SET) != 0) {
				PX4_ERR("import lseek failed (%d)", errno);
			}

			px4_usleep(10000); // wait at least 10 milliseconds before trying again
		}
	}

	തിരികെ -1;
}

#എങ്കിൽ defined(FLASH_BASED_PARAMS)
സ്ഥിതം പൂർണ്ണസംഖ്യ flash_backend_locked(പൂർണ്ണസംഖ്യ (*op)())
{
	/* Same order as param_save_default: mutex then shutdown lock. Autosave
	 * uses a trylock, so a compact holds it off the flash programming. */
	pthread_mutex_lock(&file_mutex);

	പൂർണ്ണസംഖ്യ shutdown_lock_ret = px4_shutdown_lock();

	എങ്കിൽ (shutdown_lock_ret != 0) {
		PX4_ERR("px4_shutdown_lock() failed (%i)", shutdown_lock_ret);
	}

	പൂർണ്ണസംഖ്യ result = op();

	pthread_mutex_unlock(&file_mutex);

	എങ്കിൽ (shutdown_lock_ret == 0) {
		px4_shutdown_unlock();
	}

	തിരികെ result;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ flash_load_locked()
{
	/* Reset under the lock so a pending autosave cannot append tombstones
	 * of the just-cleared RAM before import/compact run. */
	param_reset_all_internal(അസത്യം);
	പൂർണ്ണസംഖ്യ result = flash_param_import();

	എങ്കിൽ (result == 0 || result == 1) {
		params_unsaved.reset();
	}

	തിരികെ result;
}
#endif

പൂർണ്ണസംഖ്യ
param_import(പൂർണ്ണസംഖ്യ fd)
{
	എങ്കിൽ (fd < 0) {
#എങ്കിൽ defined(FLASH_BASED_PARAMS)
		തിരികെ flash_backend_locked(flash_param_import);
#അല്ലെങ്കിൽ
		തിരികെ flash_param_import();
#endif
	}

	തിരികെ param_import_internal(fd);
}

പൂർണ്ണസംഖ്യ
param_load(പൂർണ്ണസംഖ്യ fd)
{
	എങ്കിൽ (fd < 0) {
#എങ്കിൽ defined(FLASH_BASED_PARAMS)
		തിരികെ flash_backend_locked(flash_load_locked);
#അല്ലെങ്കിൽ
		തിരികെ PX4_ERROR;
#endif
	}

	param_reset_all_internal(അസത്യം);
	പൂർണ്ണസംഖ്യ result = param_import_internal(fd);

#എങ്കിൽ !defined(FLASH_BASED_PARAMS)

	എങ്കിൽ (result >= 0) {
		params_unsaved.reset();
	}

#endif

	തിരികെ result;
}

ശൂന്യം
param_foreach(ശൂന്യം (*func)(ശൂന്യം *arg, param_t param), ശൂന്യം *arg, തർക്കം only_changed, തർക്കം only_used)
{
	param_t	param;

	വേണ്ടി (param = 0; handle_in_range(param); param++) {

		/* if requested, skip unchanged values */
		എങ്കിൽ (only_changed && (!user_config.contains(param))) {
			തുടരുക;
		}

		എങ്കിൽ (only_used && !param_used(param)) {
			തുടരുക;
		}

		func(arg, param);
	}
}

uint32_t param_hash_check()
{
	uint32_t param_hash = 0;

	/* compute the CRC32 over all string param names and 4 byte values */
	വേണ്ടി (param_t param = 0; handle_in_range(param); param++) {
		എങ്കിൽ (!param_used(param) || param_is_volatile(param)) {
			തുടരുക;
		}

		സ്ഥിരം അക്ഷരം *name = param_name(param);
		auto value = user_config.get(param).i;
		സ്ഥിരം ശൂന്യം *val = (ശൂന്യം *)&value;
		param_hash = crc32part((സ്ഥിരം uint8_t *)name, strlen(name), param_hash);
		param_hash = crc32part((സ്ഥിരം uint8_t *)val, param_size(param), param_hash);
	}

	തിരികെ param_hash;
}

ശൂന്യം param_print_status()
{
	PX4_INFO("summary: %d/%d (used/total)", param_എണ്ണം_used(), param_എണ്ണം());

#ifndef FLASH_BASED_PARAMS
	സ്ഥിരം അക്ഷരം *filename = param_get_default_file();

	എങ്കിൽ (filename != nullptr) {
		PX4_INFO("file: %s", param_get_default_file());
	}

	എങ്കിൽ (param_backup_file) {
		PX4_INFO("backup file: %s", param_backup_file);
	}

#endif /* FLASH_BASED_PARAMS */

	PX4_INFO("storage array: %d/%d elements (%zu bytes total)",
		 user_config.size(), firmware_defaults.size(), (size_t)user_config.byteSize());


	PX4_INFO("storage array (custom defaults): %d/%d elements (%zu bytes total)",
		 runtime_defaults.size(), firmware_defaults.size(), (size_t)runtime_defaults.byteSize());

	എങ്കിൽ (autosave_instance) {
		PX4_INFO("auto save: %s", autosave_instance->enabled() ? "on" : "off");

		hrt_abstime last_autosave = autosave_instance->lastAutosave();

		എങ്കിൽ (last_autosave > 0) {
			PX4_INFO("last auto save: %.3f seconds ago", hrt_elapsed_time(&last_autosave) * 1e-6);
		}
	}

	perf_print_counter(param_export_perf);
	perf_print_counter(param_find_perf);
	perf_print_counter(param_get_perf);
	perf_print_counter(param_set_perf);

#എങ്കിൽ defined(CONFIG_PARAM_PRIMARY)
	ഘടന param_primary_counters counts;
	param_primary_get_counters(&counts);
	PX4_INFO("set value requests received: %" PRIu32 ", set value responses sent: %" PRIu32,
		 counts.set_മൂല്യം_request_received, counts.set_മൂല്യം_response_sent);
	PX4_INFO("set value requests sent: %" PRIu32 ", set value responses received: %" PRIu32,
		 counts.set_മൂല്യം_request_sent, counts.set_മൂല്യം_response_received);
	PX4_INFO("resets sent: %" PRIu32 ", set used requests received: %" PRIu32,
		 counts.reset_sent, counts.set_used_received);
#endif

#എങ്കിൽ defined(CONFIG_PARAM_REMOTE)
	ഘടന param_remote_counters counts;
	param_remote_get_counters(&counts);
	PX4_INFO("set value requests received: %" PRIu32 ", set value responses sent: %" PRIu32,
		 counts.set_മൂല്യം_request_received, counts.set_മൂല്യം_response_sent);
	PX4_INFO("set value requests sent: %" PRIu32 ", set value responses received: %" PRIu32,
		 counts.set_മൂല്യം_request_sent, counts.set_മൂല്യം_response_received);
	PX4_INFO("resets received: %" PRIu32 ", set used requests sent: %" PRIu32,
		 counts.reset_received, counts.set_used_sent);
#endif

}
