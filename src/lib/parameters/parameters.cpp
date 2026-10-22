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
#অন্তর্ভাব "param.h"
#অন্তর্ভাব "param_translation.h"
#অন্তর্ভাব <parameters/px4_parameters.hpp>
#অন্তর্ভাব <lib/tinybson/tinybson.h>

#যদি defined(__PX4_NUTTX)
#অন্তর্ভাব <nuttx/crc32.h>
#অন্যথা
#অন্তর্ভাব <crc32.h>
#endif
#অন্তর্ভাব <float.h>
#অন্তর্ভাব <math.h>
#অন্তর্ভাব <sys/stat.h>

#অন্তর্ভাব <containers/Bitset.hpp>
#অন্তর্ভাব <drivers/drv_hrt.h>
#অন্তর্ভাব <lib/perf/perf_counter.h>
#অন্তর্ভাব <px4_platform_common/px4_config.h>
#অন্তর্ভাব <px4_platform_common/atomic_bitset.h>
#অন্তর্ভাব <px4_platform_common/defines.h>
#অন্তর্ভাব <px4_platform_common/posix.h>
#অন্তর্ভাব <px4_platform_common/sem.h>
#অন্তর্ভাব <px4_platform_common/shutdown.h>
#অন্তর্ভাব <px4_platform_common/micro_hal.h>

using নামাকাশ time_literals;

#অন্তর্ভাব "uORB/uORB.h"
#অন্তর্ভাব "uORB/topics/parameter_update.h"
#অন্তর্ভাব <uORB/Subscription.hpp>

#অন্তর্ভাব "ExhaustiveLayer.h"
#অন্তর্ভাব "ConstLayer.h"
#অন্তর্ভাব "DynamicSparseLayer.h"
#অন্তর্ভাব "StaticSparseLayer.h"

#অন্তর্ভাব "atomic_transaction.h"

/* Include functions common to user and kernel sides */
#অন্তর্ভাব "parameters_common.cpp"

#যদি defined(__PX4_NUTTX) && !defined(CONFIG_BUILD_FLAT)
#অন্তর্ভাব <px4_platform/board_ctrl.h>
#অন্তর্ভাব "parameters_ioctl.h"
#endif

#যদি defined(FLASH_BASED_PARAMS)
#অন্তর্ভাব "flashparams/flashparams.h"
#অন্যথা
অন্তঃস্থ স্থির পূর্ণাঙ্ক flash_param_save(param_filter_func filter) { প্রত্যাগম -1; }
অন্তঃস্থ স্থির পূর্ণাঙ্ক flash_param_import() { প্রত্যাগম -1; }
#endif

স্থির বর্ণ *param_default_file = nullptr;
স্থির বর্ণ *param_backup_file = nullptr;

#অন্তর্ভাব "autosave.h"
স্থির ParamAutosave *autosave_instance {nullptr};

স্থির px4::AtomicBitset<param_info_গুনতি> params_active;  // params found
স্থির px4::AtomicBitset<param_info_গুনতি> params_unsaved;

// Params the save in progress is writing. They are moved out of params_unsaved
// before their values are read, so a param_set that lands while the save runs
// marks its param again and reaches the next save. Flash-param saves write only
// unsaved params, so nothing else would ever write that change.
স্থির px4::AtomicBitset<param_info_গুনতি> params_saving;

স্থির ConstLayer firmware_defaults;
স্থির DynamicSparseLayer runtime_defaults{&firmware_defaults};
DynamicSparseLayer user_config{&runtime_defaults};

/** parameter update topic handle */
#যদি not defined(CONFIG_PARAM_REMOTE)
স্থির orb_advert_t param_topic = nullptr;
স্থির unsigned পূর্ণাঙ্ক param_instance = 0;
#endif

স্থির perf_counter_t param_export_perf;
স্থির perf_counter_t param_find_perf;
স্থির perf_counter_t param_get_perf;
স্থির perf_counter_t param_set_perf;

স্থির pthread_mutex_t file_mutex  =
	PTHREAD_MUTEX_INITIALIZER; ///< this protects against concurrent param saves (file or flash access).

// Support for remote parameter node
#যদি defined(CONFIG_PARAM_PRIMARY)
# include "parameters_primary.h"
#endif // CONFIG_PARAM_PRIMARY
#যদি defined(CONFIG_PARAM_REMOTE)
# include "parameters_remote.h"
#endif // CONFIG_PARAM_REMOTE

শূন্য
param_init()
{
#ifdef __PX4_POSIX
	// Set up the recursive param mutex before any thread can access parameters.
	AtomicTransaction::initialize();
#endif

	param_export_perf = perf_alloc(PC_ELAPSED, "param: export");
	param_find_perf = perf_alloc(PC_গুনতি, "param: find");
	param_get_perf = perf_alloc(PC_গুনতি, "param: get");
	param_set_perf = perf_alloc(PC_ELAPSED, "param: set");

#যদি defined(__PX4_NUTTX) && !defined(CONFIG_BUILD_FLAT)
	px4_register_boardct_ioctl(_PARAMIOCBASE, param_ioctl);
#endif

#যদি defined(CONFIG_PARAM_PRIMARY)
	param_primary_init();
#endif // CONFIG_PARAM_PRIMARY

#যদি defined(CONFIG_PARAM_REMOTE)
	param_remote_init();
#endif // CONFIG_PARAM_REMOTE

#যদি not defined(CONFIG_PARAM_REMOTE)
	autosave_instance = নূতন ParamAutosave();
#endif
}


শূন্য
param_notify_changes()
{
// Don't send if this is a remote node. Only the primary
// sends out update notices
#যদি not defined(CONFIG_PARAM_REMOTE)
	parameter_update_s pup {};
	pup.instance = param_instance++;
	pup.get_গুনতি_2 = perf_event_গুনতি(param_get_perf);
	pup.set_গুনতি = perf_event_গুনতি(param_set_perf);
	pup.find_গুনতি = perf_event_গুনতি(param_find_perf);
	pup.export_গুনতি = perf_event_গুনতি(param_export_perf);
	pup.active = params_active.গুনতি();
	pup.changed = user_config.size();
	pup.custom_default = runtime_defaults.size();
	pup.timestamp = hrt_absolute_time();

	যদি (param_topic == nullptr) {
		param_topic = orb_advertise(ORB_ID(parameter_update), &pup);

	} অন্যথা {
		orb_publish(ORB_ID(parameter_update), param_topic, &pup);
	}

#endif
}

স্থির param_t param_find_internal(ধ্রুব বর্ণ *name, তার্কিক notification)
{
	perf_গুনতি(param_find_perf);

	param_t মধ্য;
	param_t front = 0;
	param_t last = param_info_গুনতি;

	/* perform a binary search of the known parameters */

	যাবত্ (front <= last) {
		মধ্য = front + (last - front) / 2;
		পূর্ণাঙ্ক ret = strcmp(name, param_name(মধ্য));

		যদি (ret == 0) {
			যদি (notification) {
				param_set_used(মধ্য);
			}

			প্রত্যাগম মধ্য;

		} অন্যথা যদি (মধ্য == front) {
			/* An end point has been hit, but there has been no match */
			ভঙ্গ;

		} অন্যথা যদি (ret < 0) {
			last = মধ্য;

		} অন্যথা {
			front = মধ্য;
		}
	}

	/* not found */
	প্রত্যাগম PARAM_INVALID;
}

param_t param_find(ধ্রুব বর্ণ *name)
{
	প্রত্যাগম param_find_internal(name, সত্য);
}

param_t param_find_no_notification(ধ্রুব বর্ণ *name)
{
	প্রত্যাগম param_find_internal(name, অসত্য);
}

unsigned param_গুনতি_used()
{
	প্রত্যাগম params_active.গুনতি();
}

param_t param_for_used_index(unsigned index)
{
	// walk all params and count used params
	যদি (index < param_info_গুনতি) {
		unsigned used_গুনতি = 0;

		প্রতি (পূর্ণাঙ্ক i = 0; i < params_active.size(); i++) {
			যদি (params_active[i]) {
				// we found the right used count,
				//  return the param value
				যদি (index == used_গুনতি) {
					প্রত্যাগম static_cast<param_t>(i);
				}

				used_গুনতি++;
			}
		}
	}

	প্রত্যাগম PARAM_INVALID;
}

পূর্ণাঙ্ক param_get_used_index(param_t param)
{
	/* this tests for out of bounds and does a constant time lookup */
	যদি (!param_used(param)) {
		প্রত্যাগম -1;
	}

	/* walk all params and count, now knowing that it has a valid index */
	পূর্ণাঙ্ক used_গুনতি = 0;

	প্রতি (পূর্ণাঙ্ক i = 0; i < params_active.size(); i++) {
		যদি (params_active[i]) {

			যদি (param == i) {
				প্রত্যাগম used_গুনতি;
			}

			used_গুনতি++;
		}
	}

	প্রত্যাগম -1;
}

তার্কিক
param_মান_unsaved(param_t param)
{
	প্রত্যাগম handle_in_range(param) ? (params_unsaved[param] || params_saving[param]) : অসত্য;
}

পূর্ণাঙ্ক
param_get(param_t param, শূন্য *val)
{
	perf_গুনতি(param_get_perf);

	যদি (!handle_in_range(param)) {
		PX4_ERR("get: param %" PRId16 " invalid", param);
		প্রত্যাগম PX4_ERROR;
	}

	যদি (!params_active[param]) {
		PX4_DEBUG("get: param %" PRId16 " (%s) not active", param, param_name(param));
	}

	পূর্ণাঙ্ক result = PX4_ERROR;

	যদি (val) {

		auto retrieve_মান = user_config.get(param);

		বিকল্প (param_type(param)) {
		পক্ষ PARAM_TYPE_INT32:
			memcpy(val, &retrieve_মান.i, পরিমাণ(retrieve_মান.i));
			প্রত্যাগম PX4_OK;

		পক্ষ PARAM_TYPE_FLOAT:
			memcpy(val, &retrieve_মান.f, পরিমাণ(retrieve_মান.f));
			প্রত্যাগম PX4_OK;
		}
	}

	প্রত্যাগম result;
}

পূর্ণাঙ্ক
param_get_default_মান_internal(param_t param, শূন্য *default_val)
{
	যদি (!handle_in_range(param)) {
		PX4_ERR("get default value: param %d invalid", param);
		প্রত্যাগম PX4_ERROR;
	}

	যদি (default_val) {
		বিকল্প (param_type(param)) {
		পক্ষ PARAM_TYPE_INT32: {
				int32_t val = runtime_defaults.get(param).i;
				memcpy(default_val, &val, পরিমাণ(val));
				প্রত্যাগম PX4_OK;
			}

		পক্ষ PARAM_TYPE_FLOAT: {
				ভিন্ন val = runtime_defaults.get(param).f;
				memcpy(default_val, &val, পরিমাণ(val));
				প্রত্যাগম PX4_OK;
			}
		}
	}

	প্রত্যাগম PX4_ERROR;
}

পূর্ণাঙ্ক
param_get_default_মান(param_t param, শূন্য *default_val)
{
	যদি (!handle_in_range(param)) {
		প্রত্যাগম PX4_ERROR;
	}

	পূর্ণাঙ্ক ret = param_get_default_মান_internal(param, default_val);
	প্রত্যাগম ret;
}

তার্কিক param_মান_is_default(param_t param)
{
	যদি (!handle_in_range(param)) {
		প্রত্যাগম সত্য;
	}

	যদি (!user_config.contains(param)) {
		// if user config does not contain it, consider it default.
		প্রত্যাগম সত্য;

	} অন্যথা {
		// compare with default value
		ধ্রুব param_মান_u user_config_মান = user_config.get(param);
		ধ্রুব param_মান_u runtime_default_মান = runtime_defaults.get(param);

		বিকল্প (param_type(param)) {
		পক্ষ PARAM_TYPE_INT32: {
				প্রত্যাগম user_config_মান.i == runtime_default_মান.i;
			}

		পক্ষ PARAM_TYPE_FLOAT: {
				প্রত্যাগম fabsf(user_config_মান.f - runtime_default_মান.f) < FLT_EPSILON;
			}
		}
	}

	প্রত্যাগম সত্য;
}

স্থির শূন্য
param_autosave()
{
	যদি (autosave_instance) {
		autosave_instance->request();
	}
}

শূন্য
param_control_autosave(তার্কিক enable)
{
	যদি (autosave_instance) {
		autosave_instance->enable(enable);
	}
}

স্থির পূর্ণাঙ্ক
param_set_internal(param_t param, ধ্রুব শূন্য *val, তার্কিক mark_saved, তার্কিক notify_changes, তার্কিক update_remote = সত্য,
		   তার্কিক autosave = সত্য)
{
	যদি (!handle_in_range(param)) {
		PX4_ERR("set invalid param %d", param);
		প্রত্যাগম PX4_ERROR;
	}

	যদি (val == nullptr) {
		PX4_ERR("set invalid value");
		প্রত্যাগম PX4_ERROR;
	}

	যদি (param_is_readonly(param)) {
		PX4_WARN("param %s is read-only", param_name(param));
		প্রত্যাগম PX4_ERROR;
	}

	পূর্ণাঙ্ক result = -1;
	তার্কিক param_changed = অসত্য;
	perf_begin(param_set_perf);

	ধ্রুব param_মান_u user_config_মান = user_config.get(param);
	param_মান_u new_মান{};

	বিকল্প (param_type(param)) {
	পক্ষ PARAM_TYPE_INT32:
		memcpy(&new_মান.i, val, পরিমাণ(new_মান.i));
		param_changed = user_config_মান.i != new_মান.i;
		ভঙ্গ;

	পক্ষ PARAM_TYPE_FLOAT:
		memcpy(&new_মান.f, val, পরিমাণ(new_মান.f));
		param_changed = fabsf(user_config_মান.f - new_মান.f) > FLT_EPSILON;
		ভঙ্গ;

	স্বভাব: {
			PX4_ERR("param_set invalid param type for %s", param_name(param));
			ভঙ্গ;
		}
	}

	যদি (user_config.store(param, new_মান)) {
		যদি (param_changed) {
			params_unsaved.set(param, !mark_saved);
		}

		result = PX4_OK;

	} অন্যথা {
		PX4_ERR("param_set failed to store param %s", param_name(param));
		result = PX4_ERROR;
	}

	যদি ((result == PX4_OK) && param_changed && !mark_saved && autosave) { // this is false when importing parameters
		param_autosave();
	}

	// If this is the parameter server, make sure that the remote is updated
#যদি defined(CONFIG_PARAM_PRIMARY)

	যদি (param_changed && update_remote) {
		param_primary_set_মান(param, val);
	}

#endif

	// If this is the parameter remote, make sure that the primary is updated
#যদি defined(CONFIG_PARAM_REMOTE)

	যদি (param_changed && update_remote) {
		param_remote_set_মান(param, val);
	}

#endif

	perf_end(param_set_perf);

	/*
	 * If we set something, now that we have unlocked, go ahead and advertise that
	 * a thing has been set.
	 */
	যদি ((result == PX4_OK) && param_changed && notify_changes) {
		param_notify_changes();
	}

	প্রত্যাগম result;
}

#যদি defined(FLASH_BASED_PARAMS)
পূর্ণাঙ্ক param_set_external(param_t param, ধ্রুব শূন্য *val, তার্কিক mark_saved, তার্কিক notify_changes)
{
	প্রত্যাগম param_set_internal(param, val, mark_saved, notify_changes);
}

শূন্য param_get_external(param_t param, শূন্য *val)
{
	param_get(param, val);
}
#endif

পূর্ণাঙ্ক param_set(param_t param, ধ্রুব শূন্য *val)
{
	প্রত্যাগম param_set_internal(param, val, অসত্য, সত্য);
}

পূর্ণাঙ্ক param_set_no_notification(param_t param, ধ্রুব শূন্য *val)
{
	প্রত্যাগম param_set_internal(param, val, অসত্য, অসত্য);
}

পূর্ণাঙ্ক param_set_no_remote_update(param_t param, ধ্রুব শূন্য *val, তার্কিক notify)
{
	প্রত্যাগম param_set_internal(param, val, অসত্য, notify, অসত্য);
}

তার্কিক param_used(param_t param)
{
	যদি (handle_in_range(param)) {
		প্রত্যাগম params_active[param];
	}

	প্রত্যাগম অসত্য;
}

শূন্য param_set_used(param_t param)
{
	যদি (handle_in_range(param)) {
#যদি defined(CONFIG_PARAM_REMOTE)

		যদি (!param_used(param)) {
			param_remote_set_used(param);
		}

#endif

		params_active.set(param, সত্য);
	}
}

পূর্ণাঙ্ক param_set_default_মান(param_t param, ধ্রুব শূন্য *val)
{
	যদি (!handle_in_range(param)) {
		PX4_ERR("set default value invalid param %d", param);
		প্রত্যাগম PX4_ERROR;
	}

	যদি (val == nullptr) {
		PX4_ERR("set default value invalid value");
		প্রত্যাগম PX4_ERROR;
	}

	যদি (param_is_readonly(param)) {
		PX4_WARN("param %s is read-only", param_name(param));
		প্রত্যাগম PX4_ERROR;
	}

	পূর্ণাঙ্ক result = PX4_ERROR;


	// check if param being set to default value
	তার্কিক setting_to_static_default = অসত্য;
	ধ্রুব param_মান_u firmware_default_মান = firmware_defaults.get(param);

	বিকল্প (param_type(param)) {
	পক্ষ PARAM_TYPE_INT32: {
			int32_t new_মান;
			memcpy(&new_মান, val, পরিমাণ(new_মান));
			setting_to_static_default = firmware_default_মান.i == new_মান;
			ভঙ্গ;
		}

	পক্ষ PARAM_TYPE_FLOAT: {
			ভিন্ন new_মান;
			memcpy(&new_মান, val, পরিমাণ(new_মান));
			setting_to_static_default = fabsf(firmware_default_মান.f - new_মান) <= FLT_EPSILON;
			ভঙ্গ;
		}
	}

	যদি (setting_to_static_default) {
		runtime_defaults.reset(param);

		result = PX4_OK;

	} অন্যথা {
		param_মান_u new_মান{};

		বিকল্প (param_type(param)) {
		পক্ষ PARAM_TYPE_INT32: {
				memcpy(&new_মান.i, val, পরিমাণ(new_মান.i));
				ভঙ্গ;
			}

		পক্ষ PARAM_TYPE_FLOAT: {
				memcpy(&new_মান.f, val, পরিমাণ(new_মান.f));
				ভঙ্গ;
			}

		স্বভাব:
			ভঙ্গ;
		}

		যদি (runtime_defaults.store(param, new_মান)) {
			user_config.refresh(param);
			result = PX4_OK;

		} অন্যথা {
			result = PX4_ERROR;
		}
	}


	যদি ((result == PX4_OK) && param_used(param)) {
		// send notification if param is already in use
		param_notify_changes();
	}

	প্রত্যাগম result;
}

স্থির পূর্ণাঙ্ক param_reset_internal(param_t param, তার্কিক notify = সত্য, তার্কিক autosave = সত্য)
{
#যদি defined(CONFIG_PARAM_REMOTE)
	// Remote doesn't support reset
	প্রত্যাগম অসত্য;
#endif

	যদি (param_is_readonly(param)) {
		প্রত্যাগম 0;  // silently skip — param_reset_all loops over all params
	}

	তার্কিক param_found = user_config.contains(param);

	যদি (handle_in_range(param)) {
		user_config.reset(param);
	}

	যদি (param_found) {
		/* Persist the reset. Flash-param saves append only unsaved params,
		 * so a reset has to show up as a tombstone or replay keeps the
		 * previous override. */
		params_unsaved.set(param, সত্য);
	}

	যদি (autosave) {
		param_autosave();
	}

	যদি (param_found && notify) {
		param_notify_changes();
	}

#যদি defined(CONFIG_PARAM_PRIMARY)
	param_primary_reset(param);
#endif

	প্রত্যাগম param_found;
}

পূর্ণাঙ্ক param_reset(param_t param) { প্রত্যাগম param_reset_internal(param, সত্য); }
পূর্ণাঙ্ক param_reset_no_notification(param_t param) { প্রত্যাগম param_reset_internal(param, অসত্য); }

স্থির শূন্য
param_reset_all_internal(তার্কিক auto_save)
{
#যদি defined(CONFIG_PARAM_REMOTE)
	// Remote doesn't support reset
	প্রত্যাগম;
#endif

	প্রতি (param_t param = 0; handle_in_range(param); param++) {
		param_reset_internal(param, অসত্য, অসত্য);
	}

	যদি (auto_save) {
		param_autosave();
	}

#যদি defined(CONFIG_PARAM_PRIMARY)
	param_primary_reset_all();
#endif

	param_notify_changes();
}

শূন্য
param_reset_all()
{
	param_reset_all_internal(সত্য);
}

শূন্য
param_reset_excludes(ধ্রুব বর্ণ *excludes[], পূর্ণাঙ্ক অঙ্ক_excludes)
{
	প্রতি (param_t param = 0; handle_in_range(param); param++) {
		ধ্রুব বর্ণ *name = param_name(param);
		তার্কিক exclude = অসত্য;

		প্রতি (পূর্ণাঙ্ক index = 0; index < অঙ্ক_excludes; index ++) {
			পূর্ণাঙ্ক len = strlen(excludes[index]);

			যদি ((excludes[index][len - 1] == '*'
			     && strncmp(name, excludes[index], len - 1) == 0)
			    || strcmp(name, excludes[index]) == 0) {

				exclude = সত্য;
				ভঙ্গ;
			}
		}

		যদি (!exclude) {
			param_reset(param);
		}
	}
}

শূন্য
param_reset_specific(ধ্রুব বর্ণ *resets[], পূর্ণাঙ্ক অঙ্ক_resets)
{
	প্রতি (param_t param = 0; handle_in_range(param); param++) {
		ধ্রুব বর্ণ *name = param_name(param);
		তার্কিক reset = অসত্য;

		প্রতি (পূর্ণাঙ্ক index = 0; index < অঙ্ক_resets; index++) {
			পূর্ণাঙ্ক len = strlen(resets[index]);

			যদি ((resets[index][len - 1] == '*'
			     && strncmp(name, resets[index], len - 1) == 0)
			    || strcmp(name, resets[index]) == 0) {

				reset = সত্য;
				ভঙ্গ;
			}
		}

		যদি (reset) {
			param_reset(param);
		}
	}
}

পূর্ণাঙ্ক
param_set_default_file(ধ্রুব বর্ণ *filename)
{
	যদি (filename && param_backup_file && strcmp(filename, param_backup_file) == 0) {
		PX4_ERR("default file can't be the same as the backup file %s", filename);
		প্রত্যাগম PX4_ERROR;
	}

#ifdef FLASH_BASED_PARAMS
	// the default for flash-based params is always the FLASH
	(শূন্য)filename;
#অন্যথা

	যদি (param_default_file != nullptr) {
		// we assume this is not in use by some other thread
		মুক্ত(param_default_file);
		param_default_file = nullptr;
	}

	যদি (filename) {
		param_default_file = strdup(filename);
	}

#endif /* FLASH_BASED_PARAMS */

	প্রত্যাগম 0;
}

ধ্রুব বর্ণ *param_get_default_file()
{
	প্রত্যাগম param_default_file;
}

পূর্ণাঙ্ক param_set_backup_file(ধ্রুব বর্ণ *filename)
{
	যদি (filename && param_default_file && strcmp(filename, param_default_file) == 0) {
		PX4_ERR("backup file can't be the same as the default file %s", filename);
		প্রত্যাগম PX4_ERROR;
	}

	যদি (param_backup_file != nullptr) {
		// we assume this is not in use by some other thread
		মুক্ত(param_backup_file);
		param_backup_file = nullptr;
	}

	যদি (filename) {
		param_backup_file = strdup(filename);

	} অন্যথা {
		param_backup_file = nullptr; // backup disabled
	}

	প্রত্যাগম 0;
}

ধ্রুব বর্ণ *param_get_backup_file()
{
	প্রত্যাগম param_backup_file;
}

স্থির পূর্ণাঙ্ক param_export_internal(পূর্ণাঙ্ক fd, param_filter_func filter);
স্থির পূর্ণাঙ্ক param_verify(পূর্ণাঙ্ক fd);

পূর্ণাঙ্ক param_save_default(তার্কিক blocking)
{
	PX4_DEBUG("param_save_default");

	// take the file lock
	যদি (blocking) {
		pthread_mutex_lock(&file_mutex);

	} অন্যথা {
		যদি (pthread_mutex_trylock(&file_mutex) != 0) {
			PX4_DEBUG("param_save_default: file lock failed (already locked)");
			প্রত্যাগম -EWOULDBLOCK;
		}
	}

	পূর্ণাঙ্ক shutdown_lock_ret = px4_shutdown_lock();

	যদি (shutdown_lock_ret != 0) {
		PX4_ERR("px4_shutdown_lock() failed (%i)", shutdown_lock_ret);
	}

	পূর্ণাঙ্ক res = PX4_ERROR;
	ধ্রুব বর্ণ *filename = param_get_default_file();

	প্রতি (param_t param = 0; handle_in_range(param); param++) {
		যদি (params_unsaved[param]) {
			params_saving.set(param, সত্য);
			params_unsaved.set(param, অসত্য);
		}
	}

	যদি (filename) {
		স্থির constexpr পূর্ণাঙ্ক MAX_ATTEMPTS = 3;

		প্রতি (পূর্ণাঙ্ক attempt = 1; attempt <= MAX_ATTEMPTS; attempt++) {
			// write parameters to file
			পূর্ণাঙ্ক fd = ::open(filename, O_WRONLY | O_CREAT | O_TRUNC, PX4_O_MODE_666);

			যদি (fd > -1) {
				perf_begin(param_export_perf);
				res = param_export_internal(fd, nullptr);
				perf_end(param_export_perf);
				::close(fd);

				যদি (res == PX4_OK) {
					// reopen file to verify
					পূর্ণাঙ্ক fd_verify = ::open(filename, O_RDONLY, PX4_O_MODE_666);
					res = param_verify(fd_verify) || lseek(fd_verify, 0, SEEK_SET) || param_verify(fd_verify);
					::close(fd_verify);
				}
			}

			যদি (res == PX4_OK) {
				ভঙ্গ;

			} অন্যথা {
				PX4_ERR("parameter export to %s failed (%d) attempt %d", filename, res, attempt);
				px4_usleep(10000); // wait at least 10 milliseconds before trying again
			}
		}

	} অন্যথা {
		perf_begin(param_export_perf);
		res = flash_param_save(nullptr);
		perf_end(param_export_perf);
	}

	যদি (res != PX4_OK) {
		PX4_ERR("param export failed (%d)", res);

		প্রতি (param_t param = 0; handle_in_range(param); param++) {
			যদি (params_saving[param]) {
				params_unsaved.set(param, সত্য);
			}
		}
	}

	params_saving.reset();

	যদি (res == PX4_OK) {
		// backup file
		যদি (param_backup_file) {
			পূর্ণাঙ্ক fd_backup_file = ::open(param_backup_file, O_WRONLY | O_CREAT | O_TRUNC, PX4_O_MODE_666);

			যদি (fd_backup_file > -1) {
				পূর্ণাঙ্ক backup_export_ret = param_export_internal(fd_backup_file, nullptr);
				::close(fd_backup_file);

				যদি (backup_export_ret != 0) {
					PX4_ERR("backup parameter export to %s failed (%d)", param_backup_file, backup_export_ret);

				} অন্যথা {
					// verify export
					পূর্ণাঙ্ক fd_verify = ::open(param_backup_file, O_RDONLY, PX4_O_MODE_666);
					param_verify(fd_verify);
					::close(fd_verify);
				}
			}
		}
	}

	pthread_mutex_unlock(&file_mutex);

	যদি (shutdown_lock_ret == 0) {
		px4_shutdown_unlock();
	}

	প্রত্যাগম res;
}

/**
 * @return 0 on success, 1 if all params have not yet been stored, -1 if device open failed, -2 if writing parameters failed
 */
পূর্ণাঙ্ক
param_load_default()
{
	পূর্ণাঙ্ক res = 0;
	ধ্রুব বর্ণ *filename = param_get_default_file();

	যদি (!filename) {
		প্রত্যাগম param_load(-1);
	}

	পূর্ণাঙ্ক fd_load = ::open(filename, O_RDONLY);

	যদি (fd_load < 0) {
		/* no parameter file is OK, otherwise this is an error */
		যদি (errno != ENOENT) {
			PX4_ERR("open '%s' for reading failed", filename);
			প্রত্যাগম -1;
		}

		প্রত্যাগম 1;
	}

	পূর্ণাঙ্ক result = param_load(fd_load);
	::close(fd_load);

	যদি (result == 1) {
		/* blank file: nothing stored yet */
		প্রত্যাগম 1;
	}

	যদি (result != 0) {
		PX4_ERR("error reading parameters from '%s'", filename);
		প্রত্যাগম -2;
	}

	প্রত্যাগম res;
}

স্থির পূর্ণাঙ্ক param_verify_callback(bson_decoder_t decoder, bson_গ্রন্থি_t গ্রন্থি_2)
{
	যদি (গ্রন্থি_2->type == BSON_EOO) {
		প্রত্যাগম 0;
	}

	// find the parameter this node represents
	param_t param = param_find_no_notification(গ্রন্থি_2->name);

	যদি (param == PARAM_INVALID) {
		PX4_ERR("verify: invalid parameter '%s'", গ্রন্থি_2->name);
		প্রত্যাগম -1;
	}

	// handle verifying the parameter from the node
	বিকল্প (গ্রন্থি_2->type) {
	পক্ষ BSON_INT32: {
			যদি (param_type(param) != PARAM_TYPE_INT32) {
				PX4_ERR("verify: invalid param type %d for '%s' (BSON_INT32)", param_type(param), গ্রন্থি_2->name);
				প্রত্যাগম -1;
			}

			int32_t value;

			যদি (param_get(param, &value) == 0) {
				যদি (value == গ্রন্থি_2->i32) {
					প্রত্যাগম 1; // valid

				} অন্যথা {
					PX4_ERR("verify: '%s' invalid BSON value %" PRIi32 "(expected %" PRIi32 ")", গ্রন্থি_2->name, গ্রন্থি_2->i32, value);
				}
			}
		}
		ভঙ্গ;

	পক্ষ BSON_DOUBLE: {
			যদি (param_type(param) != PARAM_TYPE_FLOAT) {
				PX4_ERR("verify: invalid param type %d for '%s' (BSON_DOUBLE)", param_type(param), গ্রন্থি_2->name);
				প্রত্যাগম -1;
			}

			ভিন্ন value;

			যদি (param_get(param, &value) == 0) {
				যদি (fabsf(value - (ভিন্ন)গ্রন্থি_2->d) <= FLT_EPSILON) {
					প্রত্যাগম 1; // valid

				} অন্যথা {
					PX4_ERR("verify: '%s' invalid BSON value %.3f (expected %.3f)", গ্রন্থি_2->name, গ্রন্থি_2->d, (দ্বিগুণ)value);
				}
			}
		}
		ভঙ্গ;

	স্বভাব:
		PX4_ERR("verify: '%s' invalid node type %d", গ্রন্থি_2->name, গ্রন্থি_2->type);
	}

	প্রত্যাগম -1;
}

স্থির পূর্ণাঙ্ক param_verify(পূর্ণাঙ্ক fd)
{
	PX4_DEBUG("param_verify");

	যদি (fd < 0) {
		প্রত্যাগম -1;
	}

	যদি (lseek(fd, 0, SEEK_SET) != 0) {
		PX4_ERR("verify: seek failed");
		প্রত্যাগম -1;
	}

	bson_decoder_s decoder{};

	যদি (bson_decoder_init_file(&decoder, fd, param_verify_callback) == 0) {
		পূর্ণাঙ্ক result = -1;

		কুরু {
			result = bson_decoder_next(&decoder);

		} যাবত্ (result > 0);

		যদি (result == 0) {
			যদি (decoder.total_document_size != decoder.total_decoded_size) {
				PX4_ERR("BSON document size (%" PRId32 ") doesn't match bytes decoded (%" PRId32 ")", decoder.total_document_size,
					decoder.total_decoded_size);

			} অন্যথা {
				প্রত্যাগম 0;
			}

		} অন্যথা যদি (result == -ENODATA) {
			PX4_ERR("verify: no BSON data");

		} অন্যথা {
			PX4_ERR("verify: failed (%d)", result);
		}
	}

	প্রত্যাগম -1;
}

পূর্ণাঙ্ক
param_export(ধ্রুব বর্ণ *filename, param_filter_func filter)
{
	PX4_DEBUG("param_export");

	পূর্ণাঙ্ক shutdown_lock_ret = px4_shutdown_lock();

	যদি (shutdown_lock_ret != 0) {
		PX4_ERR("px4_shutdown_lock() failed (%i)", shutdown_lock_ret);
	}

	// take the file lock
	যদি (pthread_mutex_trylock(&file_mutex) != 0) {
		PX4_ERR("param_export: file lock failed (already locked)");
		প্রত্যাগম PX4_ERROR;
	}

	পূর্ণাঙ্ক fd = ::open(filename, O_RDWR | O_CREAT, PX4_O_MODE_666);
	পূর্ণাঙ্ক result = PX4_ERROR;

	perf_begin(param_export_perf);

	যদি (fd > -1) {
		result = param_export_internal(fd, filter);

	} অন্যথা {
		result = flash_param_save(filter);
	}

	perf_end(param_export_perf);

	pthread_mutex_unlock(&file_mutex);

	যদি (shutdown_lock_ret == 0) {
		px4_shutdown_unlock();
	}

	প্রত্যাগম result;
}

// internal parameter export, caller is responsible for locking
স্থির পূর্ণাঙ্ক param_export_internal(পূর্ণাঙ্ক fd, param_filter_func filter)
{
	PX4_DEBUG("param_export_internal");
	ধ্রুব auto changed_params = user_config.containedAsBitset();

	পূর্ণাঙ্ক result = -1;
	bson_encoder_s encoder{};
	uint8_t bson_buffer[256];

	যদি (lseek(fd, 0, SEEK_SET) != 0) {
		PX4_ERR("export seek failed %d", errno);
		প্রত্যাগম -1;
	}

	যদি (bson_encoder_init_buf_file(&encoder, fd, &bson_buffer, পরিমাণ(bson_buffer)) != 0) {
		গচ্ছ out;
	}

	প্রতি (param_t param = 0; handle_in_range(param); param++) {
		যদি (!changed_params[param] || (filter && !filter(param))) {
			অনুবর্ত;
		}

		ধ্রুব param_মান_u runtime_default_মান = runtime_defaults.get(param);
		ধ্রুব param_মান_u user_config_মান = user_config.get(param);

		// don't export default values
		বিকল্প (param_type(param)) {
		পক্ষ PARAM_TYPE_INT32:
			যদি (user_config_মান.i == runtime_default_মান.i) {
				PX4_DEBUG("skipping %s %" PRIi32 " export", param_name(param), runtime_default_মান.i);
				অনুবর্ত;
			}

			ভঙ্গ;

		পক্ষ PARAM_TYPE_FLOAT:
			যদি (fabsf(user_config_মান.f - runtime_default_মান.f) <= FLT_EPSILON) {
				PX4_DEBUG("skipping %s %.3f export", param_name(param), (দ্বিগুণ)runtime_default_মান.f);
				অনুবর্ত;
			}

			ভঙ্গ;
		}

		ধ্রুব বর্ণ *name = param_name(param);
		ধ্রুব size_t size = param_size(param);

		/* append the appropriate BSON type object */
		বিকল্প (param_type(param)) {
		পক্ষ PARAM_TYPE_INT32: {
				ধ্রুব int32_t i = user_config_মান.i;
				PX4_DEBUG("exporting: %s (%d) size: %lu val: %" PRIi32, name, param, (long unsigned পূর্ণাঙ্ক)size, i);

				যদি (bson_encoder_append_int32(&encoder, name, i) != 0) {
					PX4_ERR("BSON append failed for '%s'", name);
					গচ্ছ out;
				}
			}
			ভঙ্গ;

		পক্ষ PARAM_TYPE_FLOAT: {
				ধ্রুব দ্বিগুণ f = (দ্বিগুণ)user_config_মান.f;
				PX4_DEBUG("exporting: %s (%d) size: %lu val: %.3f", name, param, (long unsigned পূর্ণাঙ্ক)size, (দ্বিগুণ)f);

				যদি (bson_encoder_append_double(&encoder, name, f) != 0) {
					PX4_ERR("BSON append failed for '%s'", name);
					গচ্ছ out;
				}
			}
			ভঙ্গ;

		স্বভাব:
			PX4_ERR("%s unrecognized parameter type %d, skipping export", name, param_type(param));
		}
	}

	result = 0;

out:

	যদি (result == 0) {
		যদি (bson_encoder_fini(&encoder) != PX4_OK) {
			PX4_ERR("BSON encoder finalize failed");
			result = -1;
		}
	}

	প্রত্যাগম result;
}

#যদি defined(FLASH_BASED_PARAMS)
// A file is never the flash backend's store, so what an import changes is unsaved until a delta carries it.
// As on the file backend, the import itself does not save.
স্থির constexpr তার্কিক file_import_is_saved = অসত্য;
#অন্যথা
স্থির constexpr তার্কিক file_import_is_saved = সত্য;
#endif

স্থির পূর্ণাঙ্ক
param_import_callback(bson_decoder_t decoder, bson_গ্রন্থি_t গ্রন্থি_2)
{
	/*
	 * EOO means the end of the parameter object. (Currently not supporting
	 * nested BSON objects).
	 */
	যদি (গ্রন্থি_2->type == BSON_EOO) {
		PX4_DEBUG("end of parameters");
		প্রত্যাগম 0;
	}

	// A tombstone has no value. The decoder leaves the previous node's in the union and the translations
	// read it without a type check.
	ধ্রুব তার্কিক tombstone = গ্রন্থি_2->type == BSON_nullptr || গ্রন্থি_2->type == BSON_UNDEFINED;

	// if we do param_set() directly in the translation, set PARAM_SKIP_IMPORT as return value and return here
	যদি (!tombstone && param_modify_on_import(গ্রন্থি_2) == param_modify_on_import_ret::PARAM_SKIP_IMPORT) {
		প্রত্যাগম 1;
	}

	// Find the parameter this node represents.  If we don't know it, ignore the node.
	param_t param = param_find_no_notification(গ্রন্থি_2->name);

	যদি (param == PARAM_INVALID) {
		PX4_WARN("ignoring unrecognised parameter '%s'", গ্রন্থি_2->name);
		প্রত্যাগম 1;
	}

	// Handle setting the parameter from the node
	বিকল্প (গ্রন্থি_2->type) {
	পক্ষ BSON_nullptr:
	পক্ষ BSON_UNDEFINED:
		param_reset_internal(param, সত্য, অসত্য);
		প্রত্যাগম 1;

	পক্ষ BSON_INT32: {
			যদি (param_type(param) == PARAM_TYPE_INT32) {
				int32_t i = গ্রন্থি_2->i32;
				param_set_internal(param, &i, file_import_is_saved, সত্য, সত্য, অসত্য);
				PX4_DEBUG("Imported %s with value %" PRIi32, param_name(param), i);

			} অন্যথা {
				PX4_WARN("unexpected type for %s", গ্রন্থি_2->name);
			}
		}
		ভঙ্গ;

	পক্ষ BSON_DOUBLE: {
			যদি (param_type(param) == PARAM_TYPE_FLOAT) {
				ভিন্ন f = গ্রন্থি_2->d;
				param_set_internal(param, &f, file_import_is_saved, সত্য, সত্য, অসত্য);
				PX4_DEBUG("Imported %s with value %f", param_name(param), (দ্বিগুণ)f);

			} অন্যথা {
				PX4_WARN("unexpected type for %s", গ্রন্থি_2->name);
			}
		}
		ভঙ্গ;

	স্বভাব:
		PX4_ERR("import: unrecognised node type for '%s'", গ্রন্থি_2->name);
	}

	// don't return zero, that means EOF
	প্রত্যাগম 1;
}

স্থির পূর্ণাঙ্ক
param_import_internal(পূর্ণাঙ্ক fd)
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
	সংরচনা stat st;

	যদি (fstat(fd, &st) == 0 && S_ISREG(st.st_mode) && st.st_size == 0) {
		প্রত্যাগম 1;
	}

	int32_t doc_size = 0;
	ssize_t len = পঠ(fd, &doc_size, পরিমাণ(doc_size));

	যদি (len == 0 || (len == (ssize_t)পরিমাণ(doc_size) && (doc_size == 0 || doc_size == -1))) {
		প্রত্যাগম 1;
	}

	যদি (lseek(fd, 0, SEEK_SET) != 0) {
		PX4_ERR("import lseek failed (%d)", errno);
		প্রত্যাগম -1;
	}

	স্থির constexpr পূর্ণাঙ্ক MAX_ATTEMPTS = 3;

	প্রতি (পূর্ণাঙ্ক attempt = 1; attempt <= MAX_ATTEMPTS; attempt++) {
		bson_decoder_s decoder{};

		যদি (bson_decoder_init_file(&decoder, fd, param_import_callback) == 0) {
			পূর্ণাঙ্ক result = -1;

			কুরু {
				result = bson_decoder_next(&decoder);

			} যাবত্ (result > 0);

			যদি (result == 0) {
				যদি (decoder.total_document_size == decoder.total_decoded_size) {
					PX4_INFO("BSON document size %" PRId32 " bytes, decoded %" PRId32 " bytes (INT32:%" PRIu16 ", FLOAT:%" PRIu16 ")",
						 decoder.total_document_size, decoder.total_decoded_size,
						 decoder.গুনতি_গ্রন্থি_int32, decoder.গুনতি_গ্রন্থি_double);

					প্রত্যাগম 0;

				} অন্যথা {
					PX4_ERR("BSON document size (%" PRId32 ") doesn't match bytes decoded (%" PRId32 ")",
						decoder.total_document_size, decoder.total_decoded_size);
				}

			} অন্যথা যদি (result == -ENODATA) {
				// silently retry as a precaution unless this is our last attempt
				যদি (attempt == MAX_ATTEMPTS) {
					PX4_DEBUG("BSON: no data");
					প্রত্যাগম 0;
				}

			} অন্যথা {
				PX4_ERR("param import failed (%d) attempt %d", result, attempt);
			}

		} অন্যথা {
			PX4_ERR("param import bson decoder init failed (attempt %d)", attempt);
		}

		যদি (attempt != MAX_ATTEMPTS) {
			যদি (lseek(fd, 0, SEEK_SET) != 0) {
				PX4_ERR("import lseek failed (%d)", errno);
			}

			px4_usleep(10000); // wait at least 10 milliseconds before trying again
		}
	}

	প্রত্যাগম -1;
}

#যদি defined(FLASH_BASED_PARAMS)
স্থির পূর্ণাঙ্ক flash_backend_locked(পূর্ণাঙ্ক (*op)())
{
	/* Same order as param_save_default: mutex then shutdown lock. Autosave
	 * uses a trylock, so a compact holds it off the flash programming. */
	pthread_mutex_lock(&file_mutex);

	পূর্ণাঙ্ক shutdown_lock_ret = px4_shutdown_lock();

	যদি (shutdown_lock_ret != 0) {
		PX4_ERR("px4_shutdown_lock() failed (%i)", shutdown_lock_ret);
	}

	পূর্ণাঙ্ক result = op();

	pthread_mutex_unlock(&file_mutex);

	যদি (shutdown_lock_ret == 0) {
		px4_shutdown_unlock();
	}

	প্রত্যাগম result;
}

স্থির পূর্ণাঙ্ক flash_load_locked()
{
	/* Reset under the lock so a pending autosave cannot append tombstones
	 * of the just-cleared RAM before import/compact run. */
	param_reset_all_internal(অসত্য);
	পূর্ণাঙ্ক result = flash_param_import();

	যদি (result == 0 || result == 1) {
		params_unsaved.reset();
	}

	প্রত্যাগম result;
}
#endif

পূর্ণাঙ্ক
param_import(পূর্ণাঙ্ক fd)
{
	যদি (fd < 0) {
#যদি defined(FLASH_BASED_PARAMS)
		প্রত্যাগম flash_backend_locked(flash_param_import);
#অন্যথা
		প্রত্যাগম flash_param_import();
#endif
	}

	প্রত্যাগম param_import_internal(fd);
}

পূর্ণাঙ্ক
param_load(পূর্ণাঙ্ক fd)
{
	যদি (fd < 0) {
#যদি defined(FLASH_BASED_PARAMS)
		প্রত্যাগম flash_backend_locked(flash_load_locked);
#অন্যথা
		প্রত্যাগম PX4_ERROR;
#endif
	}

	param_reset_all_internal(অসত্য);
	পূর্ণাঙ্ক result = param_import_internal(fd);

#যদি !defined(FLASH_BASED_PARAMS)

	যদি (result >= 0) {
		params_unsaved.reset();
	}

#endif

	প্রত্যাগম result;
}

শূন্য
param_foreach(শূন্য (*func)(শূন্য *arg, param_t param), শূন্য *arg, তার্কিক only_changed, তার্কিক only_used)
{
	param_t	param;

	প্রতি (param = 0; handle_in_range(param); param++) {

		/* if requested, skip unchanged values */
		যদি (only_changed && (!user_config.contains(param))) {
			অনুবর্ত;
		}

		যদি (only_used && !param_used(param)) {
			অনুবর্ত;
		}

		func(arg, param);
	}
}

uint32_t param_hash_check()
{
	uint32_t param_hash = 0;

	/* compute the CRC32 over all string param names and 4 byte values */
	প্রতি (param_t param = 0; handle_in_range(param); param++) {
		যদি (!param_used(param) || param_is_volatile(param)) {
			অনুবর্ত;
		}

		ধ্রুব বর্ণ *name = param_name(param);
		auto value = user_config.get(param).i;
		ধ্রুব শূন্য *val = (শূন্য *)&value;
		param_hash = crc32part((ধ্রুব uint8_t *)name, strlen(name), param_hash);
		param_hash = crc32part((ধ্রুব uint8_t *)val, param_size(param), param_hash);
	}

	প্রত্যাগম param_hash;
}

শূন্য param_print_status()
{
	PX4_INFO("summary: %d/%d (used/total)", param_গুনতি_used(), param_গুনতি());

#ifndef FLASH_BASED_PARAMS
	ধ্রুব বর্ণ *filename = param_get_default_file();

	যদি (filename != nullptr) {
		PX4_INFO("file: %s", param_get_default_file());
	}

	যদি (param_backup_file) {
		PX4_INFO("backup file: %s", param_backup_file);
	}

#endif /* FLASH_BASED_PARAMS */

	PX4_INFO("storage array: %d/%d elements (%zu bytes total)",
		 user_config.size(), firmware_defaults.size(), (size_t)user_config.byteSize());


	PX4_INFO("storage array (custom defaults): %d/%d elements (%zu bytes total)",
		 runtime_defaults.size(), firmware_defaults.size(), (size_t)runtime_defaults.byteSize());

	যদি (autosave_instance) {
		PX4_INFO("auto save: %s", autosave_instance->enabled() ? "on" : "off");

		hrt_abstime last_autosave = autosave_instance->lastAutosave();

		যদি (last_autosave > 0) {
			PX4_INFO("last auto save: %.3f seconds ago", hrt_elapsed_time(&last_autosave) * 1e-6);
		}
	}

	perf_print_counter(param_export_perf);
	perf_print_counter(param_find_perf);
	perf_print_counter(param_get_perf);
	perf_print_counter(param_set_perf);

#যদি defined(CONFIG_PARAM_PRIMARY)
	সংরচনা param_primary_counters counts;
	param_primary_get_counters(&counts);
	PX4_INFO("set value requests received: %" PRIu32 ", set value responses sent: %" PRIu32,
		 counts.set_মান_request_received, counts.set_মান_response_sent);
	PX4_INFO("set value requests sent: %" PRIu32 ", set value responses received: %" PRIu32,
		 counts.set_মান_request_sent, counts.set_মান_response_received);
	PX4_INFO("resets sent: %" PRIu32 ", set used requests received: %" PRIu32,
		 counts.reset_sent, counts.set_used_received);
#endif

#যদি defined(CONFIG_PARAM_REMOTE)
	সংরচনা param_remote_counters counts;
	param_remote_get_counters(&counts);
	PX4_INFO("set value requests received: %" PRIu32 ", set value responses sent: %" PRIu32,
		 counts.set_মান_request_received, counts.set_মান_response_sent);
	PX4_INFO("set value requests sent: %" PRIu32 ", set value responses received: %" PRIu32,
		 counts.set_মান_request_sent, counts.set_মান_response_received);
	PX4_INFO("resets received: %" PRIu32 ", set used requests sent: %" PRIu32,
		 counts.reset_received, counts.set_used_sent);
#endif

}
