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
#அந்தர்பாவ "param.h"
#அந்தர்பாவ "param_translation.h"
#அந்தர்பாவ <parameters/px4_parameters.hpp>
#அந்தர்பாவ <lib/tinybson/tinybson.h>

#எனில் defined(__PX4_NUTTX)
#அந்தர்பாவ <nuttx/crc32.h>
#இல்லையெனில்
#அந்தர்பாவ <crc32.h>
#endif
#அந்தர்பாவ <float.h>
#அந்தர்பாவ <math.h>
#அந்தர்பாவ <sys/stat.h>

#அந்தர்பாவ <containers/Bitset.hpp>
#அந்தர்பாவ <drivers/drv_hrt.h>
#அந்தர்பாவ <lib/perf/perf_counter.h>
#அந்தர்பாவ <px4_platform_common/px4_config.h>
#அந்தர்பாவ <px4_platform_common/atomic_bitset.h>
#அந்தர்பாவ <px4_platform_common/defines.h>
#அந்தர்பாவ <px4_platform_common/posix.h>
#அந்தர்பாவ <px4_platform_common/sem.h>
#அந்தர்பாவ <px4_platform_common/shutdown.h>
#அந்தர்பாவ <px4_platform_common/micro_hal.h>

using நாமாகாஶ time_literals;

#அந்தர்பாவ "uORB/uORB.h"
#அந்தர்பாவ "uORB/topics/parameter_update.h"
#அந்தர்பாவ <uORB/Subscription.hpp>

#அந்தர்பாவ "ExhaustiveLayer.h"
#அந்தர்பாவ "ConstLayer.h"
#அந்தர்பாவ "DynamicSparseLayer.h"
#அந்தர்பாவ "StaticSparseLayer.h"

#அந்தர்பாவ "atomic_transaction.h"

/* Include functions common to user and kernel sides */
#அந்தர்பாவ "parameters_common.cpp"

#எனில் defined(__PX4_NUTTX) && !defined(CONFIG_BUILD_FLAT)
#அந்தர்பாவ <px4_platform/board_ctrl.h>
#அந்தர்பாவ "parameters_ioctl.h"
#endif

#எனில் defined(FLASH_BASED_PARAMS)
#அந்தர்பாவ "flashparams/flashparams.h"
#இல்லையெனில்
உள்வரி நிலையான முழு flash_param_save(param_filter_func filter) { திரும்பு -1; }
உள்வரி நிலையான முழு flash_param_import() { திரும்பு -1; }
#endif

நிலையான எழுத்து *param_default_file = nullptr;
நிலையான எழுத்து *param_backup_file = nullptr;

#அந்தர்பாவ "autosave.h"
நிலையான ParamAutosave *autosave_instance {nullptr};

நிலையான px4::AtomicBitset<param_info_மொத்தம்> params_active;  // params found
நிலையான px4::AtomicBitset<param_info_மொத்தம்> params_unsaved;

// Params the save in progress is writing. They are moved out of params_unsaved
// before their values are read, so a param_set that lands while the save runs
// marks its param again and reaches the next save. Flash-param saves write only
// unsaved params, so nothing else would ever write that change.
நிலையான px4::AtomicBitset<param_info_மொத்தம்> params_saving;

நிலையான ConstLayer firmware_defaults;
நிலையான DynamicSparseLayer runtime_defaults{&firmware_defaults};
DynamicSparseLayer user_config{&runtime_defaults};

/** parameter update topic handle */
#எனில் not defined(CONFIG_PARAM_REMOTE)
நிலையான orb_advert_t param_topic = nullptr;
நிலையான unsigned முழு param_instance = 0;
#endif

நிலையான perf_counter_t param_export_perf;
நிலையான perf_counter_t param_find_perf;
நிலையான perf_counter_t param_get_perf;
நிலையான perf_counter_t param_set_perf;

நிலையான pthread_mutex_t file_mutex  =
	PTHREAD_MUTEX_INITIALIZER; ///< this protects against concurrent param saves (file or flash access).

// Support for remote parameter node
#எனில் defined(CONFIG_PARAM_PRIMARY)
# include "parameters_primary.h"
#endif // CONFIG_PARAM_PRIMARY
#எனில் defined(CONFIG_PARAM_REMOTE)
# include "parameters_remote.h"
#endif // CONFIG_PARAM_REMOTE

வெறுமை
param_init()
{
#ifdef __PX4_POSIX
	// Set up the recursive param mutex before any thread can access parameters.
	AtomicTransaction::initialize();
#endif

	param_export_perf = perf_alloc(PC_ELAPSED, "param: export");
	param_find_perf = perf_alloc(PC_மொத்தம், "param: find");
	param_get_perf = perf_alloc(PC_மொத்தம், "param: get");
	param_set_perf = perf_alloc(PC_ELAPSED, "param: set");

#எனில் defined(__PX4_NUTTX) && !defined(CONFIG_BUILD_FLAT)
	px4_register_boardct_ioctl(_PARAMIOCBASE, param_ioctl);
#endif

#எனில் defined(CONFIG_PARAM_PRIMARY)
	param_primary_init();
#endif // CONFIG_PARAM_PRIMARY

#எனில் defined(CONFIG_PARAM_REMOTE)
	param_remote_init();
#endif // CONFIG_PARAM_REMOTE

#எனில் not defined(CONFIG_PARAM_REMOTE)
	autosave_instance = புதிய ParamAutosave();
#endif
}


வெறுமை
param_notify_changes()
{
// Don't send if this is a remote node. Only the primary
// sends out update notices
#எனில் not defined(CONFIG_PARAM_REMOTE)
	parameter_update_s pup {};
	pup.instance = param_instance++;
	pup.get_மொத்தம்_2 = perf_event_மொத்தம்(param_get_perf);
	pup.set_மொத்தம் = perf_event_மொத்தம்(param_set_perf);
	pup.find_மொத்தம் = perf_event_மொத்தம்(param_find_perf);
	pup.export_மொத்தம் = perf_event_மொத்தம்(param_export_perf);
	pup.active = params_active.மொத்தம்();
	pup.changed = user_config.size();
	pup.custom_default = runtime_defaults.size();
	pup.timestamp = hrt_absolute_time();

	எனில் (param_topic == nullptr) {
		param_topic = orb_advertise(ORB_ID(parameter_update), &pup);

	} இல்லையெனில் {
		orb_publish(ORB_ID(parameter_update), param_topic, &pup);
	}

#endif
}

நிலையான param_t param_find_internal(மாறிலி எழுத்து *name, தருக்க notification)
{
	perf_மொத்தம்(param_find_perf);

	param_t நடு;
	param_t front = 0;
	param_t last = param_info_மொத்தம்;

	/* perform a binary search of the known parameters */

	போது (front <= last) {
		நடு = front + (last - front) / 2;
		முழு ret = strcmp(name, param_name(நடு));

		எனில் (ret == 0) {
			எனில் (notification) {
				param_set_used(நடு);
			}

			திரும்பு நடு;

		} இல்லையெனில் எனில் (நடு == front) {
			/* An end point has been hit, but there has been no match */
			நிறுத்து;

		} இல்லையெனில் எனில் (ret < 0) {
			last = நடு;

		} இல்லையெனில் {
			front = நடு;
		}
	}

	/* not found */
	திரும்பு PARAM_INVALID;
}

param_t param_find(மாறிலி எழுத்து *name)
{
	திரும்பு param_find_internal(name, உண்மை);
}

param_t param_find_no_notification(மாறிலி எழுத்து *name)
{
	திரும்பு param_find_internal(name, பொய்);
}

unsigned param_மொத்தம்_used()
{
	திரும்பு params_active.மொத்தம்();
}

param_t param_for_used_index(unsigned index)
{
	// walk all params and count used params
	எனில் (index < param_info_மொத்தம்) {
		unsigned used_மொத்தம் = 0;

		க்கு (முழு i = 0; i < params_active.size(); i++) {
			எனில் (params_active[i]) {
				// we found the right used count,
				//  return the param value
				எனில் (index == used_மொத்தம்) {
					திரும்பு static_cast<param_t>(i);
				}

				used_மொத்தம்++;
			}
		}
	}

	திரும்பு PARAM_INVALID;
}

முழு param_get_used_index(param_t param)
{
	/* this tests for out of bounds and does a constant time lookup */
	எனில் (!param_used(param)) {
		திரும்பு -1;
	}

	/* walk all params and count, now knowing that it has a valid index */
	முழு used_மொத்தம் = 0;

	க்கு (முழு i = 0; i < params_active.size(); i++) {
		எனில் (params_active[i]) {

			எனில் (param == i) {
				திரும்பு used_மொத்தம்;
			}

			used_மொத்தம்++;
		}
	}

	திரும்பு -1;
}

தருக்க
param_மதிப்பு_unsaved(param_t param)
{
	திரும்பு handle_in_range(param) ? (params_unsaved[param] || params_saving[param]) : பொய்;
}

முழு
param_get(param_t param, வெறுமை *val)
{
	perf_மொத்தம்(param_get_perf);

	எனில் (!handle_in_range(param)) {
		PX4_ERR("get: param %" PRId16 " invalid", param);
		திரும்பு PX4_ERROR;
	}

	எனில் (!params_active[param]) {
		PX4_DEBUG("get: param %" PRId16 " (%s) not active", param, param_name(param));
	}

	முழு result = PX4_ERROR;

	எனில் (val) {

		auto retrieve_மதிப்பு = user_config.get(param);

		தேர்வு (param_type(param)) {
		நிலை PARAM_TYPE_INT32:
			memcpy(val, &retrieve_மதிப்பு.i, அளவு(retrieve_மதிப்பு.i));
			திரும்பு PX4_OK;

		நிலை PARAM_TYPE_FLOAT:
			memcpy(val, &retrieve_மதிப்பு.f, அளவு(retrieve_மதிப்பு.f));
			திரும்பு PX4_OK;
		}
	}

	திரும்பு result;
}

முழு
param_get_default_மதிப்பு_internal(param_t param, வெறுமை *default_val)
{
	எனில் (!handle_in_range(param)) {
		PX4_ERR("get default value: param %d invalid", param);
		திரும்பு PX4_ERROR;
	}

	எனில் (default_val) {
		தேர்வு (param_type(param)) {
		நிலை PARAM_TYPE_INT32: {
				int32_t val = runtime_defaults.get(param).i;
				memcpy(default_val, &val, அளவு(val));
				திரும்பு PX4_OK;
			}

		நிலை PARAM_TYPE_FLOAT: {
				தசம val = runtime_defaults.get(param).f;
				memcpy(default_val, &val, அளவு(val));
				திரும்பு PX4_OK;
			}
		}
	}

	திரும்பு PX4_ERROR;
}

முழு
param_get_default_மதிப்பு(param_t param, வெறுமை *default_val)
{
	எனில் (!handle_in_range(param)) {
		திரும்பு PX4_ERROR;
	}

	முழு ret = param_get_default_மதிப்பு_internal(param, default_val);
	திரும்பு ret;
}

தருக்க param_மதிப்பு_is_default(param_t param)
{
	எனில் (!handle_in_range(param)) {
		திரும்பு உண்மை;
	}

	எனில் (!user_config.contains(param)) {
		// if user config does not contain it, consider it default.
		திரும்பு உண்மை;

	} இல்லையெனில் {
		// compare with default value
		மாறிலி param_மதிப்பு_u user_config_மதிப்பு = user_config.get(param);
		மாறிலி param_மதிப்பு_u runtime_default_மதிப்பு = runtime_defaults.get(param);

		தேர்வு (param_type(param)) {
		நிலை PARAM_TYPE_INT32: {
				திரும்பு user_config_மதிப்பு.i == runtime_default_மதிப்பு.i;
			}

		நிலை PARAM_TYPE_FLOAT: {
				திரும்பு fabsf(user_config_மதிப்பு.f - runtime_default_மதிப்பு.f) < FLT_EPSILON;
			}
		}
	}

	திரும்பு உண்மை;
}

நிலையான வெறுமை
param_autosave()
{
	எனில் (autosave_instance) {
		autosave_instance->request();
	}
}

வெறுமை
param_control_autosave(தருக்க enable)
{
	எனில் (autosave_instance) {
		autosave_instance->enable(enable);
	}
}

நிலையான முழு
param_set_internal(param_t param, மாறிலி வெறுமை *val, தருக்க mark_saved, தருக்க notify_changes, தருக்க update_remote = உண்மை,
		   தருக்க autosave = உண்மை)
{
	எனில் (!handle_in_range(param)) {
		PX4_ERR("set invalid param %d", param);
		திரும்பு PX4_ERROR;
	}

	எனில் (val == nullptr) {
		PX4_ERR("set invalid value");
		திரும்பு PX4_ERROR;
	}

	எனில் (param_is_readonly(param)) {
		PX4_WARN("param %s is read-only", param_name(param));
		திரும்பு PX4_ERROR;
	}

	முழு result = -1;
	தருக்க param_changed = பொய்;
	perf_begin(param_set_perf);

	மாறிலி param_மதிப்பு_u user_config_மதிப்பு = user_config.get(param);
	param_மதிப்பு_u new_மதிப்பு{};

	தேர்வு (param_type(param)) {
	நிலை PARAM_TYPE_INT32:
		memcpy(&new_மதிப்பு.i, val, அளவு(new_மதிப்பு.i));
		param_changed = user_config_மதிப்பு.i != new_மதிப்பு.i;
		நிறுத்து;

	நிலை PARAM_TYPE_FLOAT:
		memcpy(&new_மதிப்பு.f, val, அளவு(new_மதிப்பு.f));
		param_changed = fabsf(user_config_மதிப்பு.f - new_மதிப்பு.f) > FLT_EPSILON;
		நிறுத்து;

	இயல்பு: {
			PX4_ERR("param_set invalid param type for %s", param_name(param));
			நிறுத்து;
		}
	}

	எனில் (user_config.store(param, new_மதிப்பு)) {
		எனில் (param_changed) {
			params_unsaved.set(param, !mark_saved);
		}

		result = PX4_OK;

	} இல்லையெனில் {
		PX4_ERR("param_set failed to store param %s", param_name(param));
		result = PX4_ERROR;
	}

	எனில் ((result == PX4_OK) && param_changed && !mark_saved && autosave) { // this is false when importing parameters
		param_autosave();
	}

	// If this is the parameter server, make sure that the remote is updated
#எனில் defined(CONFIG_PARAM_PRIMARY)

	எனில் (param_changed && update_remote) {
		param_primary_set_மதிப்பு(param, val);
	}

#endif

	// If this is the parameter remote, make sure that the primary is updated
#எனில் defined(CONFIG_PARAM_REMOTE)

	எனில் (param_changed && update_remote) {
		param_remote_set_மதிப்பு(param, val);
	}

#endif

	perf_end(param_set_perf);

	/*
	 * If we set something, now that we have unlocked, go ahead and advertise that
	 * a thing has been set.
	 */
	எனில் ((result == PX4_OK) && param_changed && notify_changes) {
		param_notify_changes();
	}

	திரும்பு result;
}

#எனில் defined(FLASH_BASED_PARAMS)
முழு param_set_external(param_t param, மாறிலி வெறுமை *val, தருக்க mark_saved, தருக்க notify_changes)
{
	திரும்பு param_set_internal(param, val, mark_saved, notify_changes);
}

வெறுமை param_get_external(param_t param, வெறுமை *val)
{
	param_get(param, val);
}
#endif

முழு param_set(param_t param, மாறிலி வெறுமை *val)
{
	திரும்பு param_set_internal(param, val, பொய், உண்மை);
}

முழு param_set_no_notification(param_t param, மாறிலி வெறுமை *val)
{
	திரும்பு param_set_internal(param, val, பொய், பொய்);
}

முழு param_set_no_remote_update(param_t param, மாறிலி வெறுமை *val, தருக்க notify)
{
	திரும்பு param_set_internal(param, val, பொய், notify, பொய்);
}

தருக்க param_used(param_t param)
{
	எனில் (handle_in_range(param)) {
		திரும்பு params_active[param];
	}

	திரும்பு பொய்;
}

வெறுமை param_set_used(param_t param)
{
	எனில் (handle_in_range(param)) {
#எனில் defined(CONFIG_PARAM_REMOTE)

		எனில் (!param_used(param)) {
			param_remote_set_used(param);
		}

#endif

		params_active.set(param, உண்மை);
	}
}

முழு param_set_default_மதிப்பு(param_t param, மாறிலி வெறுமை *val)
{
	எனில் (!handle_in_range(param)) {
		PX4_ERR("set default value invalid param %d", param);
		திரும்பு PX4_ERROR;
	}

	எனில் (val == nullptr) {
		PX4_ERR("set default value invalid value");
		திரும்பு PX4_ERROR;
	}

	எனில் (param_is_readonly(param)) {
		PX4_WARN("param %s is read-only", param_name(param));
		திரும்பு PX4_ERROR;
	}

	முழு result = PX4_ERROR;


	// check if param being set to default value
	தருக்க setting_to_static_default = பொய்;
	மாறிலி param_மதிப்பு_u firmware_default_மதிப்பு = firmware_defaults.get(param);

	தேர்வு (param_type(param)) {
	நிலை PARAM_TYPE_INT32: {
			int32_t new_மதிப்பு;
			memcpy(&new_மதிப்பு, val, அளவு(new_மதிப்பு));
			setting_to_static_default = firmware_default_மதிப்பு.i == new_மதிப்பு;
			நிறுத்து;
		}

	நிலை PARAM_TYPE_FLOAT: {
			தசம new_மதிப்பு;
			memcpy(&new_மதிப்பு, val, அளவு(new_மதிப்பு));
			setting_to_static_default = fabsf(firmware_default_மதிப்பு.f - new_மதிப்பு) <= FLT_EPSILON;
			நிறுத்து;
		}
	}

	எனில் (setting_to_static_default) {
		runtime_defaults.reset(param);

		result = PX4_OK;

	} இல்லையெனில் {
		param_மதிப்பு_u new_மதிப்பு{};

		தேர்வு (param_type(param)) {
		நிலை PARAM_TYPE_INT32: {
				memcpy(&new_மதிப்பு.i, val, அளவு(new_மதிப்பு.i));
				நிறுத்து;
			}

		நிலை PARAM_TYPE_FLOAT: {
				memcpy(&new_மதிப்பு.f, val, அளவு(new_மதிப்பு.f));
				நிறுத்து;
			}

		இயல்பு:
			நிறுத்து;
		}

		எனில் (runtime_defaults.store(param, new_மதிப்பு)) {
			user_config.refresh(param);
			result = PX4_OK;

		} இல்லையெனில் {
			result = PX4_ERROR;
		}
	}


	எனில் ((result == PX4_OK) && param_used(param)) {
		// send notification if param is already in use
		param_notify_changes();
	}

	திரும்பு result;
}

நிலையான முழு param_reset_internal(param_t param, தருக்க notify = உண்மை, தருக்க autosave = உண்மை)
{
#எனில் defined(CONFIG_PARAM_REMOTE)
	// Remote doesn't support reset
	திரும்பு பொய்;
#endif

	எனில் (param_is_readonly(param)) {
		திரும்பு 0;  // silently skip — param_reset_all loops over all params
	}

	தருக்க param_found = user_config.contains(param);

	எனில் (handle_in_range(param)) {
		user_config.reset(param);
	}

	எனில் (param_found) {
		/* Persist the reset. Flash-param saves append only unsaved params,
		 * so a reset has to show up as a tombstone or replay keeps the
		 * previous override. */
		params_unsaved.set(param, உண்மை);
	}

	எனில் (autosave) {
		param_autosave();
	}

	எனில் (param_found && notify) {
		param_notify_changes();
	}

#எனில் defined(CONFIG_PARAM_PRIMARY)
	param_primary_reset(param);
#endif

	திரும்பு param_found;
}

முழு param_reset(param_t param) { திரும்பு param_reset_internal(param, உண்மை); }
முழு param_reset_no_notification(param_t param) { திரும்பு param_reset_internal(param, பொய்); }

நிலையான வெறுமை
param_reset_all_internal(தருக்க auto_save)
{
#எனில் defined(CONFIG_PARAM_REMOTE)
	// Remote doesn't support reset
	திரும்பு;
#endif

	க்கு (param_t param = 0; handle_in_range(param); param++) {
		param_reset_internal(param, பொய், பொய்);
	}

	எனில் (auto_save) {
		param_autosave();
	}

#எனில் defined(CONFIG_PARAM_PRIMARY)
	param_primary_reset_all();
#endif

	param_notify_changes();
}

வெறுமை
param_reset_all()
{
	param_reset_all_internal(உண்மை);
}

வெறுமை
param_reset_excludes(மாறிலி எழுத்து *excludes[], முழு இலக்கம்_excludes)
{
	க்கு (param_t param = 0; handle_in_range(param); param++) {
		மாறிலி எழுத்து *name = param_name(param);
		தருக்க exclude = பொய்;

		க்கு (முழு index = 0; index < இலக்கம்_excludes; index ++) {
			முழு len = strlen(excludes[index]);

			எனில் ((excludes[index][len - 1] == '*'
			     && strncmp(name, excludes[index], len - 1) == 0)
			    || strcmp(name, excludes[index]) == 0) {

				exclude = உண்மை;
				நிறுத்து;
			}
		}

		எனில் (!exclude) {
			param_reset(param);
		}
	}
}

வெறுமை
param_reset_specific(மாறிலி எழுத்து *resets[], முழு இலக்கம்_resets)
{
	க்கு (param_t param = 0; handle_in_range(param); param++) {
		மாறிலி எழுத்து *name = param_name(param);
		தருக்க reset = பொய்;

		க்கு (முழு index = 0; index < இலக்கம்_resets; index++) {
			முழு len = strlen(resets[index]);

			எனில் ((resets[index][len - 1] == '*'
			     && strncmp(name, resets[index], len - 1) == 0)
			    || strcmp(name, resets[index]) == 0) {

				reset = உண்மை;
				நிறுத்து;
			}
		}

		எனில் (reset) {
			param_reset(param);
		}
	}
}

முழு
param_set_default_file(மாறிலி எழுத்து *filename)
{
	எனில் (filename && param_backup_file && strcmp(filename, param_backup_file) == 0) {
		PX4_ERR("default file can't be the same as the backup file %s", filename);
		திரும்பு PX4_ERROR;
	}

#ifdef FLASH_BASED_PARAMS
	// the default for flash-based params is always the FLASH
	(வெறுமை)filename;
#இல்லையெனில்

	எனில் (param_default_file != nullptr) {
		// we assume this is not in use by some other thread
		விடுவி(param_default_file);
		param_default_file = nullptr;
	}

	எனில் (filename) {
		param_default_file = strdup(filename);
	}

#endif /* FLASH_BASED_PARAMS */

	திரும்பு 0;
}

மாறிலி எழுத்து *param_get_default_file()
{
	திரும்பு param_default_file;
}

முழு param_set_backup_file(மாறிலி எழுத்து *filename)
{
	எனில் (filename && param_default_file && strcmp(filename, param_default_file) == 0) {
		PX4_ERR("backup file can't be the same as the default file %s", filename);
		திரும்பு PX4_ERROR;
	}

	எனில் (param_backup_file != nullptr) {
		// we assume this is not in use by some other thread
		விடுவி(param_backup_file);
		param_backup_file = nullptr;
	}

	எனில் (filename) {
		param_backup_file = strdup(filename);

	} இல்லையெனில் {
		param_backup_file = nullptr; // backup disabled
	}

	திரும்பு 0;
}

மாறிலி எழுத்து *param_get_backup_file()
{
	திரும்பு param_backup_file;
}

நிலையான முழு param_export_internal(முழு fd, param_filter_func filter);
நிலையான முழு param_verify(முழு fd);

முழு param_save_default(தருக்க blocking)
{
	PX4_DEBUG("param_save_default");

	// take the file lock
	எனில் (blocking) {
		pthread_mutex_lock(&file_mutex);

	} இல்லையெனில் {
		எனில் (pthread_mutex_trylock(&file_mutex) != 0) {
			PX4_DEBUG("param_save_default: file lock failed (already locked)");
			திரும்பு -EWOULDBLOCK;
		}
	}

	முழு shutdown_lock_ret = px4_shutdown_lock();

	எனில் (shutdown_lock_ret != 0) {
		PX4_ERR("px4_shutdown_lock() failed (%i)", shutdown_lock_ret);
	}

	முழு res = PX4_ERROR;
	மாறிலி எழுத்து *filename = param_get_default_file();

	க்கு (param_t param = 0; handle_in_range(param); param++) {
		எனில் (params_unsaved[param]) {
			params_saving.set(param, உண்மை);
			params_unsaved.set(param, பொய்);
		}
	}

	எனில் (filename) {
		நிலையான constexpr முழு MAX_ATTEMPTS = 3;

		க்கு (முழு attempt = 1; attempt <= MAX_ATTEMPTS; attempt++) {
			// write parameters to file
			முழு fd = ::open(filename, O_WRONLY | O_CREAT | O_TRUNC, PX4_O_MODE_666);

			எனில் (fd > -1) {
				perf_begin(param_export_perf);
				res = param_export_internal(fd, nullptr);
				perf_end(param_export_perf);
				::close(fd);

				எனில் (res == PX4_OK) {
					// reopen file to verify
					முழு fd_verify = ::open(filename, O_RDONLY, PX4_O_MODE_666);
					res = param_verify(fd_verify) || lseek(fd_verify, 0, SEEK_SET) || param_verify(fd_verify);
					::close(fd_verify);
				}
			}

			எனில் (res == PX4_OK) {
				நிறுத்து;

			} இல்லையெனில் {
				PX4_ERR("parameter export to %s failed (%d) attempt %d", filename, res, attempt);
				px4_usleep(10000); // wait at least 10 milliseconds before trying again
			}
		}

	} இல்லையெனில் {
		perf_begin(param_export_perf);
		res = flash_param_save(nullptr);
		perf_end(param_export_perf);
	}

	எனில் (res != PX4_OK) {
		PX4_ERR("param export failed (%d)", res);

		க்கு (param_t param = 0; handle_in_range(param); param++) {
			எனில் (params_saving[param]) {
				params_unsaved.set(param, உண்மை);
			}
		}
	}

	params_saving.reset();

	எனில் (res == PX4_OK) {
		// backup file
		எனில் (param_backup_file) {
			முழு fd_backup_file = ::open(param_backup_file, O_WRONLY | O_CREAT | O_TRUNC, PX4_O_MODE_666);

			எனில் (fd_backup_file > -1) {
				முழு backup_export_ret = param_export_internal(fd_backup_file, nullptr);
				::close(fd_backup_file);

				எனில் (backup_export_ret != 0) {
					PX4_ERR("backup parameter export to %s failed (%d)", param_backup_file, backup_export_ret);

				} இல்லையெனில் {
					// verify export
					முழு fd_verify = ::open(param_backup_file, O_RDONLY, PX4_O_MODE_666);
					param_verify(fd_verify);
					::close(fd_verify);
				}
			}
		}
	}

	pthread_mutex_unlock(&file_mutex);

	எனில் (shutdown_lock_ret == 0) {
		px4_shutdown_unlock();
	}

	திரும்பு res;
}

/**
 * @return 0 on success, 1 if all params have not yet been stored, -1 if device open failed, -2 if writing parameters failed
 */
முழு
param_load_default()
{
	முழு res = 0;
	மாறிலி எழுத்து *filename = param_get_default_file();

	எனில் (!filename) {
		திரும்பு param_load(-1);
	}

	முழு fd_load = ::open(filename, O_RDONLY);

	எனில் (fd_load < 0) {
		/* no parameter file is OK, otherwise this is an error */
		எனில் (errno != ENOENT) {
			PX4_ERR("open '%s' for reading failed", filename);
			திரும்பு -1;
		}

		திரும்பு 1;
	}

	முழு result = param_load(fd_load);
	::close(fd_load);

	எனில் (result == 1) {
		/* blank file: nothing stored yet */
		திரும்பு 1;
	}

	எனில் (result != 0) {
		PX4_ERR("error reading parameters from '%s'", filename);
		திரும்பு -2;
	}

	திரும்பு res;
}

நிலையான முழு param_verify_callback(bson_decoder_t decoder, bson_முனை_t முனை_2)
{
	எனில் (முனை_2->type == BSON_EOO) {
		திரும்பு 0;
	}

	// find the parameter this node represents
	param_t param = param_find_no_notification(முனை_2->name);

	எனில் (param == PARAM_INVALID) {
		PX4_ERR("verify: invalid parameter '%s'", முனை_2->name);
		திரும்பு -1;
	}

	// handle verifying the parameter from the node
	தேர்வு (முனை_2->type) {
	நிலை BSON_INT32: {
			எனில் (param_type(param) != PARAM_TYPE_INT32) {
				PX4_ERR("verify: invalid param type %d for '%s' (BSON_INT32)", param_type(param), முனை_2->name);
				திரும்பு -1;
			}

			int32_t value;

			எனில் (param_get(param, &value) == 0) {
				எனில் (value == முனை_2->i32) {
					திரும்பு 1; // valid

				} இல்லையெனில் {
					PX4_ERR("verify: '%s' invalid BSON value %" PRIi32 "(expected %" PRIi32 ")", முனை_2->name, முனை_2->i32, value);
				}
			}
		}
		நிறுத்து;

	நிலை BSON_DOUBLE: {
			எனில் (param_type(param) != PARAM_TYPE_FLOAT) {
				PX4_ERR("verify: invalid param type %d for '%s' (BSON_DOUBLE)", param_type(param), முனை_2->name);
				திரும்பு -1;
			}

			தசம value;

			எனில் (param_get(param, &value) == 0) {
				எனில் (fabsf(value - (தசம)முனை_2->d) <= FLT_EPSILON) {
					திரும்பு 1; // valid

				} இல்லையெனில் {
					PX4_ERR("verify: '%s' invalid BSON value %.3f (expected %.3f)", முனை_2->name, முனை_2->d, (இரட்டை)value);
				}
			}
		}
		நிறுத்து;

	இயல்பு:
		PX4_ERR("verify: '%s' invalid node type %d", முனை_2->name, முனை_2->type);
	}

	திரும்பு -1;
}

நிலையான முழு param_verify(முழு fd)
{
	PX4_DEBUG("param_verify");

	எனில் (fd < 0) {
		திரும்பு -1;
	}

	எனில் (lseek(fd, 0, SEEK_SET) != 0) {
		PX4_ERR("verify: seek failed");
		திரும்பு -1;
	}

	bson_decoder_s decoder{};

	எனில் (bson_decoder_init_file(&decoder, fd, param_verify_callback) == 0) {
		முழு result = -1;

		செய் {
			result = bson_decoder_next(&decoder);

		} போது (result > 0);

		எனில் (result == 0) {
			எனில் (decoder.total_document_size != decoder.total_decoded_size) {
				PX4_ERR("BSON document size (%" PRId32 ") doesn't match bytes decoded (%" PRId32 ")", decoder.total_document_size,
					decoder.total_decoded_size);

			} இல்லையெனில் {
				திரும்பு 0;
			}

		} இல்லையெனில் எனில் (result == -ENODATA) {
			PX4_ERR("verify: no BSON data");

		} இல்லையெனில் {
			PX4_ERR("verify: failed (%d)", result);
		}
	}

	திரும்பு -1;
}

முழு
param_export(மாறிலி எழுத்து *filename, param_filter_func filter)
{
	PX4_DEBUG("param_export");

	முழு shutdown_lock_ret = px4_shutdown_lock();

	எனில் (shutdown_lock_ret != 0) {
		PX4_ERR("px4_shutdown_lock() failed (%i)", shutdown_lock_ret);
	}

	// take the file lock
	எனில் (pthread_mutex_trylock(&file_mutex) != 0) {
		PX4_ERR("param_export: file lock failed (already locked)");
		திரும்பு PX4_ERROR;
	}

	முழு fd = ::open(filename, O_RDWR | O_CREAT, PX4_O_MODE_666);
	முழு result = PX4_ERROR;

	perf_begin(param_export_perf);

	எனில் (fd > -1) {
		result = param_export_internal(fd, filter);

	} இல்லையெனில் {
		result = flash_param_save(filter);
	}

	perf_end(param_export_perf);

	pthread_mutex_unlock(&file_mutex);

	எனில் (shutdown_lock_ret == 0) {
		px4_shutdown_unlock();
	}

	திரும்பு result;
}

// internal parameter export, caller is responsible for locking
நிலையான முழு param_export_internal(முழு fd, param_filter_func filter)
{
	PX4_DEBUG("param_export_internal");
	மாறிலி auto changed_params = user_config.containedAsBitset();

	முழு result = -1;
	bson_encoder_s encoder{};
	uint8_t bson_buffer[256];

	எனில் (lseek(fd, 0, SEEK_SET) != 0) {
		PX4_ERR("export seek failed %d", errno);
		திரும்பு -1;
	}

	எனில் (bson_encoder_init_buf_file(&encoder, fd, &bson_buffer, அளவு(bson_buffer)) != 0) {
		செல் out;
	}

	க்கு (param_t param = 0; handle_in_range(param); param++) {
		எனில் (!changed_params[param] || (filter && !filter(param))) {
			தொடர்;
		}

		மாறிலி param_மதிப்பு_u runtime_default_மதிப்பு = runtime_defaults.get(param);
		மாறிலி param_மதிப்பு_u user_config_மதிப்பு = user_config.get(param);

		// don't export default values
		தேர்வு (param_type(param)) {
		நிலை PARAM_TYPE_INT32:
			எனில் (user_config_மதிப்பு.i == runtime_default_மதிப்பு.i) {
				PX4_DEBUG("skipping %s %" PRIi32 " export", param_name(param), runtime_default_மதிப்பு.i);
				தொடர்;
			}

			நிறுத்து;

		நிலை PARAM_TYPE_FLOAT:
			எனில் (fabsf(user_config_மதிப்பு.f - runtime_default_மதிப்பு.f) <= FLT_EPSILON) {
				PX4_DEBUG("skipping %s %.3f export", param_name(param), (இரட்டை)runtime_default_மதிப்பு.f);
				தொடர்;
			}

			நிறுத்து;
		}

		மாறிலி எழுத்து *name = param_name(param);
		மாறிலி size_t size = param_size(param);

		/* append the appropriate BSON type object */
		தேர்வு (param_type(param)) {
		நிலை PARAM_TYPE_INT32: {
				மாறிலி int32_t i = user_config_மதிப்பு.i;
				PX4_DEBUG("exporting: %s (%d) size: %lu val: %" PRIi32, name, param, (long unsigned முழு)size, i);

				எனில் (bson_encoder_append_int32(&encoder, name, i) != 0) {
					PX4_ERR("BSON append failed for '%s'", name);
					செல் out;
				}
			}
			நிறுத்து;

		நிலை PARAM_TYPE_FLOAT: {
				மாறிலி இரட்டை f = (இரட்டை)user_config_மதிப்பு.f;
				PX4_DEBUG("exporting: %s (%d) size: %lu val: %.3f", name, param, (long unsigned முழு)size, (இரட்டை)f);

				எனில் (bson_encoder_append_double(&encoder, name, f) != 0) {
					PX4_ERR("BSON append failed for '%s'", name);
					செல் out;
				}
			}
			நிறுத்து;

		இயல்பு:
			PX4_ERR("%s unrecognized parameter type %d, skipping export", name, param_type(param));
		}
	}

	result = 0;

out:

	எனில் (result == 0) {
		எனில் (bson_encoder_fini(&encoder) != PX4_OK) {
			PX4_ERR("BSON encoder finalize failed");
			result = -1;
		}
	}

	திரும்பு result;
}

#எனில் defined(FLASH_BASED_PARAMS)
// A file is never the flash backend's store, so what an import changes is unsaved until a delta carries it.
// As on the file backend, the import itself does not save.
நிலையான constexpr தருக்க file_import_is_saved = பொய்;
#இல்லையெனில்
நிலையான constexpr தருக்க file_import_is_saved = உண்மை;
#endif

நிலையான முழு
param_import_callback(bson_decoder_t decoder, bson_முனை_t முனை_2)
{
	/*
	 * EOO means the end of the parameter object. (Currently not supporting
	 * nested BSON objects).
	 */
	எனில் (முனை_2->type == BSON_EOO) {
		PX4_DEBUG("end of parameters");
		திரும்பு 0;
	}

	// A tombstone has no value. The decoder leaves the previous node's in the union and the translations
	// read it without a type check.
	மாறிலி தருக்க tombstone = முனை_2->type == BSON_nullptr || முனை_2->type == BSON_UNDEFINED;

	// if we do param_set() directly in the translation, set PARAM_SKIP_IMPORT as return value and return here
	எனில் (!tombstone && param_modify_on_import(முனை_2) == param_modify_on_import_ret::PARAM_SKIP_IMPORT) {
		திரும்பு 1;
	}

	// Find the parameter this node represents.  If we don't know it, ignore the node.
	param_t param = param_find_no_notification(முனை_2->name);

	எனில் (param == PARAM_INVALID) {
		PX4_WARN("ignoring unrecognised parameter '%s'", முனை_2->name);
		திரும்பு 1;
	}

	// Handle setting the parameter from the node
	தேர்வு (முனை_2->type) {
	நிலை BSON_nullptr:
	நிலை BSON_UNDEFINED:
		param_reset_internal(param, உண்மை, பொய்);
		திரும்பு 1;

	நிலை BSON_INT32: {
			எனில் (param_type(param) == PARAM_TYPE_INT32) {
				int32_t i = முனை_2->i32;
				param_set_internal(param, &i, file_import_is_saved, உண்மை, உண்மை, பொய்);
				PX4_DEBUG("Imported %s with value %" PRIi32, param_name(param), i);

			} இல்லையெனில் {
				PX4_WARN("unexpected type for %s", முனை_2->name);
			}
		}
		நிறுத்து;

	நிலை BSON_DOUBLE: {
			எனில் (param_type(param) == PARAM_TYPE_FLOAT) {
				தசம f = முனை_2->d;
				param_set_internal(param, &f, file_import_is_saved, உண்மை, உண்மை, பொய்);
				PX4_DEBUG("Imported %s with value %f", param_name(param), (இரட்டை)f);

			} இல்லையெனில் {
				PX4_WARN("unexpected type for %s", முனை_2->name);
			}
		}
		நிறுத்து;

	இயல்பு:
		PX4_ERR("import: unrecognised node type for '%s'", முனை_2->name);
	}

	// don't return zero, that means EOF
	திரும்பு 1;
}

நிலையான முழு
param_import_internal(முழு fd)
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
	கட்டமைப்பு stat st;

	எனில் (fstat(fd, &st) == 0 && S_ISREG(st.st_mode) && st.st_size == 0) {
		திரும்பு 1;
	}

	int32_t doc_size = 0;
	ssize_t len = பட(fd, &doc_size, அளவு(doc_size));

	எனில் (len == 0 || (len == (ssize_t)அளவு(doc_size) && (doc_size == 0 || doc_size == -1))) {
		திரும்பு 1;
	}

	எனில் (lseek(fd, 0, SEEK_SET) != 0) {
		PX4_ERR("import lseek failed (%d)", errno);
		திரும்பு -1;
	}

	நிலையான constexpr முழு MAX_ATTEMPTS = 3;

	க்கு (முழு attempt = 1; attempt <= MAX_ATTEMPTS; attempt++) {
		bson_decoder_s decoder{};

		எனில் (bson_decoder_init_file(&decoder, fd, param_import_callback) == 0) {
			முழு result = -1;

			செய் {
				result = bson_decoder_next(&decoder);

			} போது (result > 0);

			எனில் (result == 0) {
				எனில் (decoder.total_document_size == decoder.total_decoded_size) {
					PX4_INFO("BSON document size %" PRId32 " bytes, decoded %" PRId32 " bytes (INT32:%" PRIu16 ", FLOAT:%" PRIu16 ")",
						 decoder.total_document_size, decoder.total_decoded_size,
						 decoder.மொத்தம்_முனை_int32, decoder.மொத்தம்_முனை_double);

					திரும்பு 0;

				} இல்லையெனில் {
					PX4_ERR("BSON document size (%" PRId32 ") doesn't match bytes decoded (%" PRId32 ")",
						decoder.total_document_size, decoder.total_decoded_size);
				}

			} இல்லையெனில் எனில் (result == -ENODATA) {
				// silently retry as a precaution unless this is our last attempt
				எனில் (attempt == MAX_ATTEMPTS) {
					PX4_DEBUG("BSON: no data");
					திரும்பு 0;
				}

			} இல்லையெனில் {
				PX4_ERR("param import failed (%d) attempt %d", result, attempt);
			}

		} இல்லையெனில் {
			PX4_ERR("param import bson decoder init failed (attempt %d)", attempt);
		}

		எனில் (attempt != MAX_ATTEMPTS) {
			எனில் (lseek(fd, 0, SEEK_SET) != 0) {
				PX4_ERR("import lseek failed (%d)", errno);
			}

			px4_usleep(10000); // wait at least 10 milliseconds before trying again
		}
	}

	திரும்பு -1;
}

#எனில் defined(FLASH_BASED_PARAMS)
நிலையான முழு flash_backend_locked(முழு (*op)())
{
	/* Same order as param_save_default: mutex then shutdown lock. Autosave
	 * uses a trylock, so a compact holds it off the flash programming. */
	pthread_mutex_lock(&file_mutex);

	முழு shutdown_lock_ret = px4_shutdown_lock();

	எனில் (shutdown_lock_ret != 0) {
		PX4_ERR("px4_shutdown_lock() failed (%i)", shutdown_lock_ret);
	}

	முழு result = op();

	pthread_mutex_unlock(&file_mutex);

	எனில் (shutdown_lock_ret == 0) {
		px4_shutdown_unlock();
	}

	திரும்பு result;
}

நிலையான முழு flash_load_locked()
{
	/* Reset under the lock so a pending autosave cannot append tombstones
	 * of the just-cleared RAM before import/compact run. */
	param_reset_all_internal(பொய்);
	முழு result = flash_param_import();

	எனில் (result == 0 || result == 1) {
		params_unsaved.reset();
	}

	திரும்பு result;
}
#endif

முழு
param_import(முழு fd)
{
	எனில் (fd < 0) {
#எனில் defined(FLASH_BASED_PARAMS)
		திரும்பு flash_backend_locked(flash_param_import);
#இல்லையெனில்
		திரும்பு flash_param_import();
#endif
	}

	திரும்பு param_import_internal(fd);
}

முழு
param_load(முழு fd)
{
	எனில் (fd < 0) {
#எனில் defined(FLASH_BASED_PARAMS)
		திரும்பு flash_backend_locked(flash_load_locked);
#இல்லையெனில்
		திரும்பு PX4_ERROR;
#endif
	}

	param_reset_all_internal(பொய்);
	முழு result = param_import_internal(fd);

#எனில் !defined(FLASH_BASED_PARAMS)

	எனில் (result >= 0) {
		params_unsaved.reset();
	}

#endif

	திரும்பு result;
}

வெறுமை
param_foreach(வெறுமை (*func)(வெறுமை *arg, param_t param), வெறுமை *arg, தருக்க only_changed, தருக்க only_used)
{
	param_t	param;

	க்கு (param = 0; handle_in_range(param); param++) {

		/* if requested, skip unchanged values */
		எனில் (only_changed && (!user_config.contains(param))) {
			தொடர்;
		}

		எனில் (only_used && !param_used(param)) {
			தொடர்;
		}

		func(arg, param);
	}
}

uint32_t param_hash_check()
{
	uint32_t param_hash = 0;

	/* compute the CRC32 over all string param names and 4 byte values */
	க்கு (param_t param = 0; handle_in_range(param); param++) {
		எனில் (!param_used(param) || param_is_volatile(param)) {
			தொடர்;
		}

		மாறிலி எழுத்து *name = param_name(param);
		auto value = user_config.get(param).i;
		மாறிலி வெறுமை *val = (வெறுமை *)&value;
		param_hash = crc32part((மாறிலி uint8_t *)name, strlen(name), param_hash);
		param_hash = crc32part((மாறிலி uint8_t *)val, param_size(param), param_hash);
	}

	திரும்பு param_hash;
}

வெறுமை param_print_status()
{
	PX4_INFO("summary: %d/%d (used/total)", param_மொத்தம்_used(), param_மொத்தம்());

#ifndef FLASH_BASED_PARAMS
	மாறிலி எழுத்து *filename = param_get_default_file();

	எனில் (filename != nullptr) {
		PX4_INFO("file: %s", param_get_default_file());
	}

	எனில் (param_backup_file) {
		PX4_INFO("backup file: %s", param_backup_file);
	}

#endif /* FLASH_BASED_PARAMS */

	PX4_INFO("storage array: %d/%d elements (%zu bytes total)",
		 user_config.size(), firmware_defaults.size(), (size_t)user_config.byteSize());


	PX4_INFO("storage array (custom defaults): %d/%d elements (%zu bytes total)",
		 runtime_defaults.size(), firmware_defaults.size(), (size_t)runtime_defaults.byteSize());

	எனில் (autosave_instance) {
		PX4_INFO("auto save: %s", autosave_instance->enabled() ? "on" : "off");

		hrt_abstime last_autosave = autosave_instance->lastAutosave();

		எனில் (last_autosave > 0) {
			PX4_INFO("last auto save: %.3f seconds ago", hrt_elapsed_time(&last_autosave) * 1e-6);
		}
	}

	perf_print_counter(param_export_perf);
	perf_print_counter(param_find_perf);
	perf_print_counter(param_get_perf);
	perf_print_counter(param_set_perf);

#எனில் defined(CONFIG_PARAM_PRIMARY)
	கட்டமைப்பு param_primary_counters counts;
	param_primary_get_counters(&counts);
	PX4_INFO("set value requests received: %" PRIu32 ", set value responses sent: %" PRIu32,
		 counts.set_மதிப்பு_request_received, counts.set_மதிப்பு_response_sent);
	PX4_INFO("set value requests sent: %" PRIu32 ", set value responses received: %" PRIu32,
		 counts.set_மதிப்பு_request_sent, counts.set_மதிப்பு_response_received);
	PX4_INFO("resets sent: %" PRIu32 ", set used requests received: %" PRIu32,
		 counts.reset_sent, counts.set_used_received);
#endif

#எனில் defined(CONFIG_PARAM_REMOTE)
	கட்டமைப்பு param_remote_counters counts;
	param_remote_get_counters(&counts);
	PX4_INFO("set value requests received: %" PRIu32 ", set value responses sent: %" PRIu32,
		 counts.set_மதிப்பு_request_received, counts.set_மதிப்பு_response_sent);
	PX4_INFO("set value requests sent: %" PRIu32 ", set value responses received: %" PRIu32,
		 counts.set_மதிப்பு_request_sent, counts.set_மதிப்பு_response_received);
	PX4_INFO("resets received: %" PRIu32 ", set used requests sent: %" PRIu32,
		 counts.reset_received, counts.set_used_sent);
#endif

}
