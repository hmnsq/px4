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
#અન્તર્ભાવ "param.h"
#અન્તર્ભાવ "param_translation.h"
#અન્તર્ભાવ <parameters/px4_parameters.hpp>
#અન્તર્ભાવ <lib/tinybson/tinybson.h>

#જો defined(__PX4_NUTTX)
#અન્તર્ભાવ <nuttx/crc32.h>
#નહીંતર
#અન્તર્ભાવ <crc32.h>
#endif
#અન્તર્ભાવ <float.h>
#અન્તર્ભાવ <math.h>
#અન્તર્ભાવ <sys/stat.h>

#અન્તર્ભાવ <containers/Bitset.hpp>
#અન્તર્ભાવ <drivers/drv_hrt.h>
#અન્તર્ભાવ <lib/perf/perf_counter.h>
#અન્તર્ભાવ <px4_platform_common/px4_config.h>
#અન્તર્ભાવ <px4_platform_common/atomic_bitset.h>
#અન્તર્ભાવ <px4_platform_common/defines.h>
#અન્તર્ભાવ <px4_platform_common/posix.h>
#અન્તર્ભાવ <px4_platform_common/sem.h>
#અન્તર્ભાવ <px4_platform_common/shutdown.h>
#અન્તર્ભાવ <px4_platform_common/micro_hal.h>

using નામાકાશ time_literals;

#અન્તર્ભાવ "uORB/uORB.h"
#અન્તર્ભાવ "uORB/topics/parameter_update.h"
#અન્તર્ભાવ <uORB/Subscription.hpp>

#અન્તર્ભાવ "ExhaustiveLayer.h"
#અન્તર્ભાવ "ConstLayer.h"
#અન્તર્ભાવ "DynamicSparseLayer.h"
#અન્તર્ભાવ "StaticSparseLayer.h"

#અન્તર્ભાવ "atomic_transaction.h"

/* Include functions common to user and kernel sides */
#અન્તર્ભાવ "parameters_common.cpp"

#જો defined(__PX4_NUTTX) && !defined(CONFIG_BUILD_FLAT)
#અન્તર્ભાવ <px4_platform/board_ctrl.h>
#અન્તર્ભાવ "parameters_ioctl.h"
#endif

#જો defined(FLASH_BASED_PARAMS)
#અન્તર્ભાવ "flashparams/flashparams.h"
#નહીંતર
અંતઃરેખ સ્થિર પૂર્ણાંક flash_param_save(param_filter_func filter) { પાછા -1; }
અંતઃરેખ સ્થિર પૂર્ણાંક flash_param_import() { પાછા -1; }
#endif

સ્થિર અક્ષર *param_default_file = nullptr;
સ્થિર અક્ષર *param_backup_file = nullptr;

#અન્તર્ભાવ "autosave.h"
સ્થિર ParamAutosave *autosave_instance {nullptr};

સ્થિર px4::AtomicBitset<param_info_ગણતરી> params_active;  // params found
સ્થિર px4::AtomicBitset<param_info_ગણતરી> params_unsaved;

// Params the save in progress is writing. They are moved out of params_unsaved
// before their values are read, so a param_set that lands while the save runs
// marks its param again and reaches the next save. Flash-param saves write only
// unsaved params, so nothing else would ever write that change.
સ્થિર px4::AtomicBitset<param_info_ગણતરી> params_saving;

સ્થિર ConstLayer firmware_defaults;
સ્થિર DynamicSparseLayer runtime_defaults{&firmware_defaults};
DynamicSparseLayer user_config{&runtime_defaults};

/** parameter update topic handle */
#જો not defined(CONFIG_PARAM_REMOTE)
સ્થિર orb_advert_t param_topic = nullptr;
સ્થિર unsigned પૂર્ણાંક param_instance = 0;
#endif

સ્થિર perf_counter_t param_export_perf;
સ્થિર perf_counter_t param_find_perf;
સ્થિર perf_counter_t param_get_perf;
સ્થિર perf_counter_t param_set_perf;

સ્થિર pthread_mutex_t file_mutex  =
	PTHREAD_MUTEX_INITIALIZER; ///< this protects against concurrent param saves (file or flash access).

// Support for remote parameter node
#જો defined(CONFIG_PARAM_PRIMARY)
# include "parameters_primary.h"
#endif // CONFIG_PARAM_PRIMARY
#જો defined(CONFIG_PARAM_REMOTE)
# include "parameters_remote.h"
#endif // CONFIG_PARAM_REMOTE

શૂન્ય
param_init()
{
#ifdef __PX4_POSIX
	// Set up the recursive param mutex before any thread can access parameters.
	AtomicTransaction::initialize();
#endif

	param_export_perf = perf_alloc(PC_ELAPSED, "param: export");
	param_find_perf = perf_alloc(PC_ગણતરી, "param: find");
	param_get_perf = perf_alloc(PC_ગણતરી, "param: get");
	param_set_perf = perf_alloc(PC_ELAPSED, "param: set");

#જો defined(__PX4_NUTTX) && !defined(CONFIG_BUILD_FLAT)
	px4_register_boardct_ioctl(_PARAMIOCBASE, param_ioctl);
#endif

#જો defined(CONFIG_PARAM_PRIMARY)
	param_primary_init();
#endif // CONFIG_PARAM_PRIMARY

#જો defined(CONFIG_PARAM_REMOTE)
	param_remote_init();
#endif // CONFIG_PARAM_REMOTE

#જો not defined(CONFIG_PARAM_REMOTE)
	autosave_instance = નવું ParamAutosave();
#endif
}


શૂન્ય
param_notify_changes()
{
// Don't send if this is a remote node. Only the primary
// sends out update notices
#જો not defined(CONFIG_PARAM_REMOTE)
	parameter_update_s pup {};
	pup.instance = param_instance++;
	pup.get_ગણતરી_2 = perf_event_ગણતરી(param_get_perf);
	pup.set_ગણતરી = perf_event_ગણતરી(param_set_perf);
	pup.find_ગણતરી = perf_event_ગણતરી(param_find_perf);
	pup.export_ગણતરી = perf_event_ગણતરી(param_export_perf);
	pup.active = params_active.ગણતરી();
	pup.changed = user_config.size();
	pup.custom_default = runtime_defaults.size();
	pup.timestamp = hrt_absolute_time();

	જો (param_topic == nullptr) {
		param_topic = orb_advertise(ORB_ID(parameter_update), &pup);

	} નહીંતર {
		orb_publish(ORB_ID(parameter_update), param_topic, &pup);
	}

#endif
}

સ્થિર param_t param_find_internal(અચલ અક્ષર *name, તાર્કિક notification)
{
	perf_ગણતરી(param_find_perf);

	param_t મધ્ય;
	param_t front = 0;
	param_t last = param_info_ગણતરી;

	/* perform a binary search of the known parameters */

	જ્યારે (front <= last) {
		મધ્ય = front + (last - front) / 2;
		પૂર્ણાંક ret = strcmp(name, param_name(મધ્ય));

		જો (ret == 0) {
			જો (notification) {
				param_set_used(મધ્ય);
			}

			પાછા મધ્ય;

		} નહીંતર જો (મધ્ય == front) {
			/* An end point has been hit, but there has been no match */
			તોડો;

		} નહીંતર જો (ret < 0) {
			last = મધ્ય;

		} નહીંતર {
			front = મધ્ય;
		}
	}

	/* not found */
	પાછા PARAM_INVALID;
}

param_t param_find(અચલ અક્ષર *name)
{
	પાછા param_find_internal(name, સાચું);
}

param_t param_find_no_notification(અચલ અક્ષર *name)
{
	પાછા param_find_internal(name, ખોટું);
}

unsigned param_ગણતરી_used()
{
	પાછા params_active.ગણતરી();
}

param_t param_for_used_index(unsigned index)
{
	// walk all params and count used params
	જો (index < param_info_ગણતરી) {
		unsigned used_ગણતરી = 0;

		માટે (પૂર્ણાંક i = 0; i < params_active.size(); i++) {
			જો (params_active[i]) {
				// we found the right used count,
				//  return the param value
				જો (index == used_ગણતરી) {
					પાછા static_cast<param_t>(i);
				}

				used_ગણતરી++;
			}
		}
	}

	પાછા PARAM_INVALID;
}

પૂર્ણાંક param_get_used_index(param_t param)
{
	/* this tests for out of bounds and does a constant time lookup */
	જો (!param_used(param)) {
		પાછા -1;
	}

	/* walk all params and count, now knowing that it has a valid index */
	પૂર્ણાંક used_ગણતરી = 0;

	માટે (પૂર્ણાંક i = 0; i < params_active.size(); i++) {
		જો (params_active[i]) {

			જો (param == i) {
				પાછા used_ગણતરી;
			}

			used_ગણતરી++;
		}
	}

	પાછા -1;
}

તાર્કિક
param_કિંમત_unsaved(param_t param)
{
	પાછા handle_in_range(param) ? (params_unsaved[param] || params_saving[param]) : ખોટું;
}

પૂર્ણાંક
param_get(param_t param, શૂન્ય *val)
{
	perf_ગણતરી(param_get_perf);

	જો (!handle_in_range(param)) {
		PX4_ERR("get: param %" PRId16 " invalid", param);
		પાછા PX4_ERROR;
	}

	જો (!params_active[param]) {
		PX4_DEBUG("get: param %" PRId16 " (%s) not active", param, param_name(param));
	}

	પૂર્ણાંક result = PX4_ERROR;

	જો (val) {

		auto retrieve_કિંમત = user_config.get(param);

		પસંદગી (param_type(param)) {
		સ્થિતિ PARAM_TYPE_INT32:
			memcpy(val, &retrieve_કિંમત.i, કદ(retrieve_કિંમત.i));
			પાછા PX4_OK;

		સ્થિતિ PARAM_TYPE_FLOAT:
			memcpy(val, &retrieve_કિંમત.f, કદ(retrieve_કિંમત.f));
			પાછા PX4_OK;
		}
	}

	પાછા result;
}

પૂર્ણાંક
param_get_default_કિંમત_internal(param_t param, શૂન્ય *default_val)
{
	જો (!handle_in_range(param)) {
		PX4_ERR("get default value: param %d invalid", param);
		પાછા PX4_ERROR;
	}

	જો (default_val) {
		પસંદગી (param_type(param)) {
		સ્થિતિ PARAM_TYPE_INT32: {
				int32_t val = runtime_defaults.get(param).i;
				memcpy(default_val, &val, કદ(val));
				પાછા PX4_OK;
			}

		સ્થિતિ PARAM_TYPE_FLOAT: {
				દશાંશ val = runtime_defaults.get(param).f;
				memcpy(default_val, &val, કદ(val));
				પાછા PX4_OK;
			}
		}
	}

	પાછા PX4_ERROR;
}

પૂર્ણાંક
param_get_default_કિંમત(param_t param, શૂન્ય *default_val)
{
	જો (!handle_in_range(param)) {
		પાછા PX4_ERROR;
	}

	પૂર્ણાંક ret = param_get_default_કિંમત_internal(param, default_val);
	પાછા ret;
}

તાર્કિક param_કિંમત_is_default(param_t param)
{
	જો (!handle_in_range(param)) {
		પાછા સાચું;
	}

	જો (!user_config.contains(param)) {
		// if user config does not contain it, consider it default.
		પાછા સાચું;

	} નહીંતર {
		// compare with default value
		અચલ param_કિંમત_u user_config_કિંમત = user_config.get(param);
		અચલ param_કિંમત_u runtime_default_કિંમત = runtime_defaults.get(param);

		પસંદગી (param_type(param)) {
		સ્થિતિ PARAM_TYPE_INT32: {
				પાછા user_config_કિંમત.i == runtime_default_કિંમત.i;
			}

		સ્થિતિ PARAM_TYPE_FLOAT: {
				પાછા fabsf(user_config_કિંમત.f - runtime_default_કિંમત.f) < FLT_EPSILON;
			}
		}
	}

	પાછા સાચું;
}

સ્થિર શૂન્ય
param_autosave()
{
	જો (autosave_instance) {
		autosave_instance->request();
	}
}

શૂન્ય
param_control_autosave(તાર્કિક enable)
{
	જો (autosave_instance) {
		autosave_instance->enable(enable);
	}
}

સ્થિર પૂર્ણાંક
param_set_internal(param_t param, અચલ શૂન્ય *val, તાર્કિક mark_saved, તાર્કિક notify_changes, તાર્કિક update_remote = સાચું,
		   તાર્કિક autosave = સાચું)
{
	જો (!handle_in_range(param)) {
		PX4_ERR("set invalid param %d", param);
		પાછા PX4_ERROR;
	}

	જો (val == nullptr) {
		PX4_ERR("set invalid value");
		પાછા PX4_ERROR;
	}

	જો (param_is_readonly(param)) {
		PX4_WARN("param %s is read-only", param_name(param));
		પાછા PX4_ERROR;
	}

	પૂર્ણાંક result = -1;
	તાર્કિક param_changed = ખોટું;
	perf_begin(param_set_perf);

	અચલ param_કિંમત_u user_config_કિંમત = user_config.get(param);
	param_કિંમત_u new_કિંમત{};

	પસંદગી (param_type(param)) {
	સ્થિતિ PARAM_TYPE_INT32:
		memcpy(&new_કિંમત.i, val, કદ(new_કિંમત.i));
		param_changed = user_config_કિંમત.i != new_કિંમત.i;
		તોડો;

	સ્થિતિ PARAM_TYPE_FLOAT:
		memcpy(&new_કિંમત.f, val, કદ(new_કિંમત.f));
		param_changed = fabsf(user_config_કિંમત.f - new_કિંમત.f) > FLT_EPSILON;
		તોડો;

	મૂળભૂત: {
			PX4_ERR("param_set invalid param type for %s", param_name(param));
			તોડો;
		}
	}

	જો (user_config.store(param, new_કિંમત)) {
		જો (param_changed) {
			params_unsaved.set(param, !mark_saved);
		}

		result = PX4_OK;

	} નહીંતર {
		PX4_ERR("param_set failed to store param %s", param_name(param));
		result = PX4_ERROR;
	}

	જો ((result == PX4_OK) && param_changed && !mark_saved && autosave) { // this is false when importing parameters
		param_autosave();
	}

	// If this is the parameter server, make sure that the remote is updated
#જો defined(CONFIG_PARAM_PRIMARY)

	જો (param_changed && update_remote) {
		param_primary_set_કિંમત(param, val);
	}

#endif

	// If this is the parameter remote, make sure that the primary is updated
#જો defined(CONFIG_PARAM_REMOTE)

	જો (param_changed && update_remote) {
		param_remote_set_કિંમત(param, val);
	}

#endif

	perf_end(param_set_perf);

	/*
	 * If we set something, now that we have unlocked, go ahead and advertise that
	 * a thing has been set.
	 */
	જો ((result == PX4_OK) && param_changed && notify_changes) {
		param_notify_changes();
	}

	પાછા result;
}

#જો defined(FLASH_BASED_PARAMS)
પૂર્ણાંક param_set_external(param_t param, અચલ શૂન્ય *val, તાર્કિક mark_saved, તાર્કિક notify_changes)
{
	પાછા param_set_internal(param, val, mark_saved, notify_changes);
}

શૂન્ય param_get_external(param_t param, શૂન્ય *val)
{
	param_get(param, val);
}
#endif

પૂર્ણાંક param_set(param_t param, અચલ શૂન્ય *val)
{
	પાછા param_set_internal(param, val, ખોટું, સાચું);
}

પૂર્ણાંક param_set_no_notification(param_t param, અચલ શૂન્ય *val)
{
	પાછા param_set_internal(param, val, ખોટું, ખોટું);
}

પૂર્ણાંક param_set_no_remote_update(param_t param, અચલ શૂન્ય *val, તાર્કિક notify)
{
	પાછા param_set_internal(param, val, ખોટું, notify, ખોટું);
}

તાર્કિક param_used(param_t param)
{
	જો (handle_in_range(param)) {
		પાછા params_active[param];
	}

	પાછા ખોટું;
}

શૂન્ય param_set_used(param_t param)
{
	જો (handle_in_range(param)) {
#જો defined(CONFIG_PARAM_REMOTE)

		જો (!param_used(param)) {
			param_remote_set_used(param);
		}

#endif

		params_active.set(param, સાચું);
	}
}

પૂર્ણાંક param_set_default_કિંમત(param_t param, અચલ શૂન્ય *val)
{
	જો (!handle_in_range(param)) {
		PX4_ERR("set default value invalid param %d", param);
		પાછા PX4_ERROR;
	}

	જો (val == nullptr) {
		PX4_ERR("set default value invalid value");
		પાછા PX4_ERROR;
	}

	જો (param_is_readonly(param)) {
		PX4_WARN("param %s is read-only", param_name(param));
		પાછા PX4_ERROR;
	}

	પૂર્ણાંક result = PX4_ERROR;


	// check if param being set to default value
	તાર્કિક setting_to_static_default = ખોટું;
	અચલ param_કિંમત_u firmware_default_કિંમત = firmware_defaults.get(param);

	પસંદગી (param_type(param)) {
	સ્થિતિ PARAM_TYPE_INT32: {
			int32_t new_કિંમત;
			memcpy(&new_કિંમત, val, કદ(new_કિંમત));
			setting_to_static_default = firmware_default_કિંમત.i == new_કિંમત;
			તોડો;
		}

	સ્થિતિ PARAM_TYPE_FLOAT: {
			દશાંશ new_કિંમત;
			memcpy(&new_કિંમત, val, કદ(new_કિંમત));
			setting_to_static_default = fabsf(firmware_default_કિંમત.f - new_કિંમત) <= FLT_EPSILON;
			તોડો;
		}
	}

	જો (setting_to_static_default) {
		runtime_defaults.reset(param);

		result = PX4_OK;

	} નહીંતર {
		param_કિંમત_u new_કિંમત{};

		પસંદગી (param_type(param)) {
		સ્થિતિ PARAM_TYPE_INT32: {
				memcpy(&new_કિંમત.i, val, કદ(new_કિંમત.i));
				તોડો;
			}

		સ્થિતિ PARAM_TYPE_FLOAT: {
				memcpy(&new_કિંમત.f, val, કદ(new_કિંમત.f));
				તોડો;
			}

		મૂળભૂત:
			તોડો;
		}

		જો (runtime_defaults.store(param, new_કિંમત)) {
			user_config.refresh(param);
			result = PX4_OK;

		} નહીંતર {
			result = PX4_ERROR;
		}
	}


	જો ((result == PX4_OK) && param_used(param)) {
		// send notification if param is already in use
		param_notify_changes();
	}

	પાછા result;
}

સ્થિર પૂર્ણાંક param_reset_internal(param_t param, તાર્કિક notify = સાચું, તાર્કિક autosave = સાચું)
{
#જો defined(CONFIG_PARAM_REMOTE)
	// Remote doesn't support reset
	પાછા ખોટું;
#endif

	જો (param_is_readonly(param)) {
		પાછા 0;  // silently skip — param_reset_all loops over all params
	}

	તાર્કિક param_found = user_config.contains(param);

	જો (handle_in_range(param)) {
		user_config.reset(param);
	}

	જો (param_found) {
		/* Persist the reset. Flash-param saves append only unsaved params,
		 * so a reset has to show up as a tombstone or replay keeps the
		 * previous override. */
		params_unsaved.set(param, સાચું);
	}

	જો (autosave) {
		param_autosave();
	}

	જો (param_found && notify) {
		param_notify_changes();
	}

#જો defined(CONFIG_PARAM_PRIMARY)
	param_primary_reset(param);
#endif

	પાછા param_found;
}

પૂર્ણાંક param_reset(param_t param) { પાછા param_reset_internal(param, સાચું); }
પૂર્ણાંક param_reset_no_notification(param_t param) { પાછા param_reset_internal(param, ખોટું); }

સ્થિર શૂન્ય
param_reset_all_internal(તાર્કિક auto_save)
{
#જો defined(CONFIG_PARAM_REMOTE)
	// Remote doesn't support reset
	પાછા;
#endif

	માટે (param_t param = 0; handle_in_range(param); param++) {
		param_reset_internal(param, ખોટું, ખોટું);
	}

	જો (auto_save) {
		param_autosave();
	}

#જો defined(CONFIG_PARAM_PRIMARY)
	param_primary_reset_all();
#endif

	param_notify_changes();
}

શૂન્ય
param_reset_all()
{
	param_reset_all_internal(સાચું);
}

શૂન્ય
param_reset_excludes(અચલ અક્ષર *excludes[], પૂર્ણાંક અંક_excludes)
{
	માટે (param_t param = 0; handle_in_range(param); param++) {
		અચલ અક્ષર *name = param_name(param);
		તાર્કિક exclude = ખોટું;

		માટે (પૂર્ણાંક index = 0; index < અંક_excludes; index ++) {
			પૂર્ણાંક len = strlen(excludes[index]);

			જો ((excludes[index][len - 1] == '*'
			     && strncmp(name, excludes[index], len - 1) == 0)
			    || strcmp(name, excludes[index]) == 0) {

				exclude = સાચું;
				તોડો;
			}
		}

		જો (!exclude) {
			param_reset(param);
		}
	}
}

શૂન્ય
param_reset_specific(અચલ અક્ષર *resets[], પૂર્ણાંક અંક_resets)
{
	માટે (param_t param = 0; handle_in_range(param); param++) {
		અચલ અક્ષર *name = param_name(param);
		તાર્કિક reset = ખોટું;

		માટે (પૂર્ણાંક index = 0; index < અંક_resets; index++) {
			પૂર્ણાંક len = strlen(resets[index]);

			જો ((resets[index][len - 1] == '*'
			     && strncmp(name, resets[index], len - 1) == 0)
			    || strcmp(name, resets[index]) == 0) {

				reset = સાચું;
				તોડો;
			}
		}

		જો (reset) {
			param_reset(param);
		}
	}
}

પૂર્ણાંક
param_set_default_file(અચલ અક્ષર *filename)
{
	જો (filename && param_backup_file && strcmp(filename, param_backup_file) == 0) {
		PX4_ERR("default file can't be the same as the backup file %s", filename);
		પાછા PX4_ERROR;
	}

#ifdef FLASH_BASED_PARAMS
	// the default for flash-based params is always the FLASH
	(શૂન્ય)filename;
#નહીંતર

	જો (param_default_file != nullptr) {
		// we assume this is not in use by some other thread
		મુક્ત(param_default_file);
		param_default_file = nullptr;
	}

	જો (filename) {
		param_default_file = strdup(filename);
	}

#endif /* FLASH_BASED_PARAMS */

	પાછા 0;
}

અચલ અક્ષર *param_get_default_file()
{
	પાછા param_default_file;
}

પૂર્ણાંક param_set_backup_file(અચલ અક્ષર *filename)
{
	જો (filename && param_default_file && strcmp(filename, param_default_file) == 0) {
		PX4_ERR("backup file can't be the same as the default file %s", filename);
		પાછા PX4_ERROR;
	}

	જો (param_backup_file != nullptr) {
		// we assume this is not in use by some other thread
		મુક્ત(param_backup_file);
		param_backup_file = nullptr;
	}

	જો (filename) {
		param_backup_file = strdup(filename);

	} નહીંતર {
		param_backup_file = nullptr; // backup disabled
	}

	પાછા 0;
}

અચલ અક્ષર *param_get_backup_file()
{
	પાછા param_backup_file;
}

સ્થિર પૂર્ણાંક param_export_internal(પૂર્ણાંક fd, param_filter_func filter);
સ્થિર પૂર્ણાંક param_verify(પૂર્ણાંક fd);

પૂર્ણાંક param_save_default(તાર્કિક blocking)
{
	PX4_DEBUG("param_save_default");

	// take the file lock
	જો (blocking) {
		pthread_mutex_lock(&file_mutex);

	} નહીંતર {
		જો (pthread_mutex_trylock(&file_mutex) != 0) {
			PX4_DEBUG("param_save_default: file lock failed (already locked)");
			પાછા -EWOULDBLOCK;
		}
	}

	પૂર્ણાંક shutdown_lock_ret = px4_shutdown_lock();

	જો (shutdown_lock_ret != 0) {
		PX4_ERR("px4_shutdown_lock() failed (%i)", shutdown_lock_ret);
	}

	પૂર્ણાંક res = PX4_ERROR;
	અચલ અક્ષર *filename = param_get_default_file();

	માટે (param_t param = 0; handle_in_range(param); param++) {
		જો (params_unsaved[param]) {
			params_saving.set(param, સાચું);
			params_unsaved.set(param, ખોટું);
		}
	}

	જો (filename) {
		સ્થિર constexpr પૂર્ણાંક MAX_ATTEMPTS = 3;

		માટે (પૂર્ણાંક attempt = 1; attempt <= MAX_ATTEMPTS; attempt++) {
			// write parameters to file
			પૂર્ણાંક fd = ::open(filename, O_WRONLY | O_CREAT | O_TRUNC, PX4_O_MODE_666);

			જો (fd > -1) {
				perf_begin(param_export_perf);
				res = param_export_internal(fd, nullptr);
				perf_end(param_export_perf);
				::close(fd);

				જો (res == PX4_OK) {
					// reopen file to verify
					પૂર્ણાંક fd_verify = ::open(filename, O_RDONLY, PX4_O_MODE_666);
					res = param_verify(fd_verify) || lseek(fd_verify, 0, SEEK_SET) || param_verify(fd_verify);
					::close(fd_verify);
				}
			}

			જો (res == PX4_OK) {
				તોડો;

			} નહીંતર {
				PX4_ERR("parameter export to %s failed (%d) attempt %d", filename, res, attempt);
				px4_usleep(10000); // wait at least 10 milliseconds before trying again
			}
		}

	} નહીંતર {
		perf_begin(param_export_perf);
		res = flash_param_save(nullptr);
		perf_end(param_export_perf);
	}

	જો (res != PX4_OK) {
		PX4_ERR("param export failed (%d)", res);

		માટે (param_t param = 0; handle_in_range(param); param++) {
			જો (params_saving[param]) {
				params_unsaved.set(param, સાચું);
			}
		}
	}

	params_saving.reset();

	જો (res == PX4_OK) {
		// backup file
		જો (param_backup_file) {
			પૂર્ણાંક fd_backup_file = ::open(param_backup_file, O_WRONLY | O_CREAT | O_TRUNC, PX4_O_MODE_666);

			જો (fd_backup_file > -1) {
				પૂર્ણાંક backup_export_ret = param_export_internal(fd_backup_file, nullptr);
				::close(fd_backup_file);

				જો (backup_export_ret != 0) {
					PX4_ERR("backup parameter export to %s failed (%d)", param_backup_file, backup_export_ret);

				} નહીંતર {
					// verify export
					પૂર્ણાંક fd_verify = ::open(param_backup_file, O_RDONLY, PX4_O_MODE_666);
					param_verify(fd_verify);
					::close(fd_verify);
				}
			}
		}
	}

	pthread_mutex_unlock(&file_mutex);

	જો (shutdown_lock_ret == 0) {
		px4_shutdown_unlock();
	}

	પાછા res;
}

/**
 * @return 0 on success, 1 if all params have not yet been stored, -1 if device open failed, -2 if writing parameters failed
 */
પૂર્ણાંક
param_load_default()
{
	પૂર્ણાંક res = 0;
	અચલ અક્ષર *filename = param_get_default_file();

	જો (!filename) {
		પાછા param_load(-1);
	}

	પૂર્ણાંક fd_load = ::open(filename, O_RDONLY);

	જો (fd_load < 0) {
		/* no parameter file is OK, otherwise this is an error */
		જો (errno != ENOENT) {
			PX4_ERR("open '%s' for reading failed", filename);
			પાછા -1;
		}

		પાછા 1;
	}

	પૂર્ણાંક result = param_load(fd_load);
	::close(fd_load);

	જો (result == 1) {
		/* blank file: nothing stored yet */
		પાછા 1;
	}

	જો (result != 0) {
		PX4_ERR("error reading parameters from '%s'", filename);
		પાછા -2;
	}

	પાછા res;
}

સ્થિર પૂર્ણાંક param_verify_callback(bson_decoder_t decoder, bson_ગાંઠ_t ગાંઠ_2)
{
	જો (ગાંઠ_2->type == BSON_EOO) {
		પાછા 0;
	}

	// find the parameter this node represents
	param_t param = param_find_no_notification(ગાંઠ_2->name);

	જો (param == PARAM_INVALID) {
		PX4_ERR("verify: invalid parameter '%s'", ગાંઠ_2->name);
		પાછા -1;
	}

	// handle verifying the parameter from the node
	પસંદગી (ગાંઠ_2->type) {
	સ્થિતિ BSON_INT32: {
			જો (param_type(param) != PARAM_TYPE_INT32) {
				PX4_ERR("verify: invalid param type %d for '%s' (BSON_INT32)", param_type(param), ગાંઠ_2->name);
				પાછા -1;
			}

			int32_t value;

			જો (param_get(param, &value) == 0) {
				જો (value == ગાંઠ_2->i32) {
					પાછા 1; // valid

				} નહીંતર {
					PX4_ERR("verify: '%s' invalid BSON value %" PRIi32 "(expected %" PRIi32 ")", ગાંઠ_2->name, ગાંઠ_2->i32, value);
				}
			}
		}
		તોડો;

	સ્થિતિ BSON_DOUBLE: {
			જો (param_type(param) != PARAM_TYPE_FLOAT) {
				PX4_ERR("verify: invalid param type %d for '%s' (BSON_DOUBLE)", param_type(param), ગાંઠ_2->name);
				પાછા -1;
			}

			દશાંશ value;

			જો (param_get(param, &value) == 0) {
				જો (fabsf(value - (દશાંશ)ગાંઠ_2->d) <= FLT_EPSILON) {
					પાછા 1; // valid

				} નહીંતર {
					PX4_ERR("verify: '%s' invalid BSON value %.3f (expected %.3f)", ગાંઠ_2->name, ગાંઠ_2->d, (દ્વિગુણ)value);
				}
			}
		}
		તોડો;

	મૂળભૂત:
		PX4_ERR("verify: '%s' invalid node type %d", ગાંઠ_2->name, ગાંઠ_2->type);
	}

	પાછા -1;
}

સ્થિર પૂર્ણાંક param_verify(પૂર્ણાંક fd)
{
	PX4_DEBUG("param_verify");

	જો (fd < 0) {
		પાછા -1;
	}

	જો (lseek(fd, 0, SEEK_SET) != 0) {
		PX4_ERR("verify: seek failed");
		પાછા -1;
	}

	bson_decoder_s decoder{};

	જો (bson_decoder_init_file(&decoder, fd, param_verify_callback) == 0) {
		પૂર્ણાંક result = -1;

		કરો {
			result = bson_decoder_next(&decoder);

		} જ્યારે (result > 0);

		જો (result == 0) {
			જો (decoder.total_document_size != decoder.total_decoded_size) {
				PX4_ERR("BSON document size (%" PRId32 ") doesn't match bytes decoded (%" PRId32 ")", decoder.total_document_size,
					decoder.total_decoded_size);

			} નહીંતર {
				પાછા 0;
			}

		} નહીંતર જો (result == -ENODATA) {
			PX4_ERR("verify: no BSON data");

		} નહીંતર {
			PX4_ERR("verify: failed (%d)", result);
		}
	}

	પાછા -1;
}

પૂર્ણાંક
param_export(અચલ અક્ષર *filename, param_filter_func filter)
{
	PX4_DEBUG("param_export");

	પૂર્ણાંક shutdown_lock_ret = px4_shutdown_lock();

	જો (shutdown_lock_ret != 0) {
		PX4_ERR("px4_shutdown_lock() failed (%i)", shutdown_lock_ret);
	}

	// take the file lock
	જો (pthread_mutex_trylock(&file_mutex) != 0) {
		PX4_ERR("param_export: file lock failed (already locked)");
		પાછા PX4_ERROR;
	}

	પૂર્ણાંક fd = ::open(filename, O_RDWR | O_CREAT, PX4_O_MODE_666);
	પૂર્ણાંક result = PX4_ERROR;

	perf_begin(param_export_perf);

	જો (fd > -1) {
		result = param_export_internal(fd, filter);

	} નહીંતર {
		result = flash_param_save(filter);
	}

	perf_end(param_export_perf);

	pthread_mutex_unlock(&file_mutex);

	જો (shutdown_lock_ret == 0) {
		px4_shutdown_unlock();
	}

	પાછા result;
}

// internal parameter export, caller is responsible for locking
સ્થિર પૂર્ણાંક param_export_internal(પૂર્ણાંક fd, param_filter_func filter)
{
	PX4_DEBUG("param_export_internal");
	અચલ auto changed_params = user_config.containedAsBitset();

	પૂર્ણાંક result = -1;
	bson_encoder_s encoder{};
	uint8_t bson_buffer[256];

	જો (lseek(fd, 0, SEEK_SET) != 0) {
		PX4_ERR("export seek failed %d", errno);
		પાછા -1;
	}

	જો (bson_encoder_init_buf_file(&encoder, fd, &bson_buffer, કદ(bson_buffer)) != 0) {
		જાઓ out;
	}

	માટે (param_t param = 0; handle_in_range(param); param++) {
		જો (!changed_params[param] || (filter && !filter(param))) {
			ચાલુ;
		}

		અચલ param_કિંમત_u runtime_default_કિંમત = runtime_defaults.get(param);
		અચલ param_કિંમત_u user_config_કિંમત = user_config.get(param);

		// don't export default values
		પસંદગી (param_type(param)) {
		સ્થિતિ PARAM_TYPE_INT32:
			જો (user_config_કિંમત.i == runtime_default_કિંમત.i) {
				PX4_DEBUG("skipping %s %" PRIi32 " export", param_name(param), runtime_default_કિંમત.i);
				ચાલુ;
			}

			તોડો;

		સ્થિતિ PARAM_TYPE_FLOAT:
			જો (fabsf(user_config_કિંમત.f - runtime_default_કિંમત.f) <= FLT_EPSILON) {
				PX4_DEBUG("skipping %s %.3f export", param_name(param), (દ્વિગુણ)runtime_default_કિંમત.f);
				ચાલુ;
			}

			તોડો;
		}

		અચલ અક્ષર *name = param_name(param);
		અચલ size_t size = param_size(param);

		/* append the appropriate BSON type object */
		પસંદગી (param_type(param)) {
		સ્થિતિ PARAM_TYPE_INT32: {
				અચલ int32_t i = user_config_કિંમત.i;
				PX4_DEBUG("exporting: %s (%d) size: %lu val: %" PRIi32, name, param, (long unsigned પૂર્ણાંક)size, i);

				જો (bson_encoder_append_int32(&encoder, name, i) != 0) {
					PX4_ERR("BSON append failed for '%s'", name);
					જાઓ out;
				}
			}
			તોડો;

		સ્થિતિ PARAM_TYPE_FLOAT: {
				અચલ દ્વિગુણ f = (દ્વિગુણ)user_config_કિંમત.f;
				PX4_DEBUG("exporting: %s (%d) size: %lu val: %.3f", name, param, (long unsigned પૂર્ણાંક)size, (દ્વિગુણ)f);

				જો (bson_encoder_append_double(&encoder, name, f) != 0) {
					PX4_ERR("BSON append failed for '%s'", name);
					જાઓ out;
				}
			}
			તોડો;

		મૂળભૂત:
			PX4_ERR("%s unrecognized parameter type %d, skipping export", name, param_type(param));
		}
	}

	result = 0;

out:

	જો (result == 0) {
		જો (bson_encoder_fini(&encoder) != PX4_OK) {
			PX4_ERR("BSON encoder finalize failed");
			result = -1;
		}
	}

	પાછા result;
}

#જો defined(FLASH_BASED_PARAMS)
// A file is never the flash backend's store, so what an import changes is unsaved until a delta carries it.
// As on the file backend, the import itself does not save.
સ્થિર constexpr તાર્કિક file_import_is_saved = ખોટું;
#નહીંતર
સ્થિર constexpr તાર્કિક file_import_is_saved = સાચું;
#endif

સ્થિર પૂર્ણાંક
param_import_callback(bson_decoder_t decoder, bson_ગાંઠ_t ગાંઠ_2)
{
	/*
	 * EOO means the end of the parameter object. (Currently not supporting
	 * nested BSON objects).
	 */
	જો (ગાંઠ_2->type == BSON_EOO) {
		PX4_DEBUG("end of parameters");
		પાછા 0;
	}

	// A tombstone has no value. The decoder leaves the previous node's in the union and the translations
	// read it without a type check.
	અચલ તાર્કિક tombstone = ગાંઠ_2->type == BSON_nullptr || ગાંઠ_2->type == BSON_UNDEFINED;

	// if we do param_set() directly in the translation, set PARAM_SKIP_IMPORT as return value and return here
	જો (!tombstone && param_modify_on_import(ગાંઠ_2) == param_modify_on_import_ret::PARAM_SKIP_IMPORT) {
		પાછા 1;
	}

	// Find the parameter this node represents.  If we don't know it, ignore the node.
	param_t param = param_find_no_notification(ગાંઠ_2->name);

	જો (param == PARAM_INVALID) {
		PX4_WARN("ignoring unrecognised parameter '%s'", ગાંઠ_2->name);
		પાછા 1;
	}

	// Handle setting the parameter from the node
	પસંદગી (ગાંઠ_2->type) {
	સ્થિતિ BSON_nullptr:
	સ્થિતિ BSON_UNDEFINED:
		param_reset_internal(param, સાચું, ખોટું);
		પાછા 1;

	સ્થિતિ BSON_INT32: {
			જો (param_type(param) == PARAM_TYPE_INT32) {
				int32_t i = ગાંઠ_2->i32;
				param_set_internal(param, &i, file_import_is_saved, સાચું, સાચું, ખોટું);
				PX4_DEBUG("Imported %s with value %" PRIi32, param_name(param), i);

			} નહીંતર {
				PX4_WARN("unexpected type for %s", ગાંઠ_2->name);
			}
		}
		તોડો;

	સ્થિતિ BSON_DOUBLE: {
			જો (param_type(param) == PARAM_TYPE_FLOAT) {
				દશાંશ f = ગાંઠ_2->d;
				param_set_internal(param, &f, file_import_is_saved, સાચું, સાચું, ખોટું);
				PX4_DEBUG("Imported %s with value %f", param_name(param), (દ્વિગુણ)f);

			} નહીંતર {
				PX4_WARN("unexpected type for %s", ગાંઠ_2->name);
			}
		}
		તોડો;

	મૂળભૂત:
		PX4_ERR("import: unrecognised node type for '%s'", ગાંઠ_2->name);
	}

	// don't return zero, that means EOF
	પાછા 1;
}

સ્થિર પૂર્ણાંક
param_import_internal(પૂર્ણાંક fd)
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
	સંરચના stat st;

	જો (fstat(fd, &st) == 0 && S_ISREG(st.st_mode) && st.st_size == 0) {
		પાછા 1;
	}

	int32_t doc_size = 0;
	ssize_t len = પઠ(fd, &doc_size, કદ(doc_size));

	જો (len == 0 || (len == (ssize_t)કદ(doc_size) && (doc_size == 0 || doc_size == -1))) {
		પાછા 1;
	}

	જો (lseek(fd, 0, SEEK_SET) != 0) {
		PX4_ERR("import lseek failed (%d)", errno);
		પાછા -1;
	}

	સ્થિર constexpr પૂર્ણાંક MAX_ATTEMPTS = 3;

	માટે (પૂર્ણાંક attempt = 1; attempt <= MAX_ATTEMPTS; attempt++) {
		bson_decoder_s decoder{};

		જો (bson_decoder_init_file(&decoder, fd, param_import_callback) == 0) {
			પૂર્ણાંક result = -1;

			કરો {
				result = bson_decoder_next(&decoder);

			} જ્યારે (result > 0);

			જો (result == 0) {
				જો (decoder.total_document_size == decoder.total_decoded_size) {
					PX4_INFO("BSON document size %" PRId32 " bytes, decoded %" PRId32 " bytes (INT32:%" PRIu16 ", FLOAT:%" PRIu16 ")",
						 decoder.total_document_size, decoder.total_decoded_size,
						 decoder.ગણતરી_ગાંઠ_int32, decoder.ગણતરી_ગાંઠ_double);

					પાછા 0;

				} નહીંતર {
					PX4_ERR("BSON document size (%" PRId32 ") doesn't match bytes decoded (%" PRId32 ")",
						decoder.total_document_size, decoder.total_decoded_size);
				}

			} નહીંતર જો (result == -ENODATA) {
				// silently retry as a precaution unless this is our last attempt
				જો (attempt == MAX_ATTEMPTS) {
					PX4_DEBUG("BSON: no data");
					પાછા 0;
				}

			} નહીંતર {
				PX4_ERR("param import failed (%d) attempt %d", result, attempt);
			}

		} નહીંતર {
			PX4_ERR("param import bson decoder init failed (attempt %d)", attempt);
		}

		જો (attempt != MAX_ATTEMPTS) {
			જો (lseek(fd, 0, SEEK_SET) != 0) {
				PX4_ERR("import lseek failed (%d)", errno);
			}

			px4_usleep(10000); // wait at least 10 milliseconds before trying again
		}
	}

	પાછા -1;
}

#જો defined(FLASH_BASED_PARAMS)
સ્થિર પૂર્ણાંક flash_backend_locked(પૂર્ણાંક (*op)())
{
	/* Same order as param_save_default: mutex then shutdown lock. Autosave
	 * uses a trylock, so a compact holds it off the flash programming. */
	pthread_mutex_lock(&file_mutex);

	પૂર્ણાંક shutdown_lock_ret = px4_shutdown_lock();

	જો (shutdown_lock_ret != 0) {
		PX4_ERR("px4_shutdown_lock() failed (%i)", shutdown_lock_ret);
	}

	પૂર્ણાંક result = op();

	pthread_mutex_unlock(&file_mutex);

	જો (shutdown_lock_ret == 0) {
		px4_shutdown_unlock();
	}

	પાછા result;
}

સ્થિર પૂર્ણાંક flash_load_locked()
{
	/* Reset under the lock so a pending autosave cannot append tombstones
	 * of the just-cleared RAM before import/compact run. */
	param_reset_all_internal(ખોટું);
	પૂર્ણાંક result = flash_param_import();

	જો (result == 0 || result == 1) {
		params_unsaved.reset();
	}

	પાછા result;
}
#endif

પૂર્ણાંક
param_import(પૂર્ણાંક fd)
{
	જો (fd < 0) {
#જો defined(FLASH_BASED_PARAMS)
		પાછા flash_backend_locked(flash_param_import);
#નહીંતર
		પાછા flash_param_import();
#endif
	}

	પાછા param_import_internal(fd);
}

પૂર્ણાંક
param_load(પૂર્ણાંક fd)
{
	જો (fd < 0) {
#જો defined(FLASH_BASED_PARAMS)
		પાછા flash_backend_locked(flash_load_locked);
#નહીંતર
		પાછા PX4_ERROR;
#endif
	}

	param_reset_all_internal(ખોટું);
	પૂર્ણાંક result = param_import_internal(fd);

#જો !defined(FLASH_BASED_PARAMS)

	જો (result >= 0) {
		params_unsaved.reset();
	}

#endif

	પાછા result;
}

શૂન્ય
param_foreach(શૂન્ય (*func)(શૂન્ય *arg, param_t param), શૂન્ય *arg, તાર્કિક only_changed, તાર્કિક only_used)
{
	param_t	param;

	માટે (param = 0; handle_in_range(param); param++) {

		/* if requested, skip unchanged values */
		જો (only_changed && (!user_config.contains(param))) {
			ચાલુ;
		}

		જો (only_used && !param_used(param)) {
			ચાલુ;
		}

		func(arg, param);
	}
}

uint32_t param_hash_check()
{
	uint32_t param_hash = 0;

	/* compute the CRC32 over all string param names and 4 byte values */
	માટે (param_t param = 0; handle_in_range(param); param++) {
		જો (!param_used(param) || param_is_volatile(param)) {
			ચાલુ;
		}

		અચલ અક્ષર *name = param_name(param);
		auto value = user_config.get(param).i;
		અચલ શૂન્ય *val = (શૂન્ય *)&value;
		param_hash = crc32part((અચલ uint8_t *)name, strlen(name), param_hash);
		param_hash = crc32part((અચલ uint8_t *)val, param_size(param), param_hash);
	}

	પાછા param_hash;
}

શૂન્ય param_print_status()
{
	PX4_INFO("summary: %d/%d (used/total)", param_ગણતરી_used(), param_ગણતરી());

#ifndef FLASH_BASED_PARAMS
	અચલ અક્ષર *filename = param_get_default_file();

	જો (filename != nullptr) {
		PX4_INFO("file: %s", param_get_default_file());
	}

	જો (param_backup_file) {
		PX4_INFO("backup file: %s", param_backup_file);
	}

#endif /* FLASH_BASED_PARAMS */

	PX4_INFO("storage array: %d/%d elements (%zu bytes total)",
		 user_config.size(), firmware_defaults.size(), (size_t)user_config.byteSize());


	PX4_INFO("storage array (custom defaults): %d/%d elements (%zu bytes total)",
		 runtime_defaults.size(), firmware_defaults.size(), (size_t)runtime_defaults.byteSize());

	જો (autosave_instance) {
		PX4_INFO("auto save: %s", autosave_instance->enabled() ? "on" : "off");

		hrt_abstime last_autosave = autosave_instance->lastAutosave();

		જો (last_autosave > 0) {
			PX4_INFO("last auto save: %.3f seconds ago", hrt_elapsed_time(&last_autosave) * 1e-6);
		}
	}

	perf_print_counter(param_export_perf);
	perf_print_counter(param_find_perf);
	perf_print_counter(param_get_perf);
	perf_print_counter(param_set_perf);

#જો defined(CONFIG_PARAM_PRIMARY)
	સંરચના param_primary_counters counts;
	param_primary_get_counters(&counts);
	PX4_INFO("set value requests received: %" PRIu32 ", set value responses sent: %" PRIu32,
		 counts.set_કિંમત_request_received, counts.set_કિંમત_response_sent);
	PX4_INFO("set value requests sent: %" PRIu32 ", set value responses received: %" PRIu32,
		 counts.set_કિંમત_request_sent, counts.set_કિંમત_response_received);
	PX4_INFO("resets sent: %" PRIu32 ", set used requests received: %" PRIu32,
		 counts.reset_sent, counts.set_used_received);
#endif

#જો defined(CONFIG_PARAM_REMOTE)
	સંરચના param_remote_counters counts;
	param_remote_get_counters(&counts);
	PX4_INFO("set value requests received: %" PRIu32 ", set value responses sent: %" PRIu32,
		 counts.set_કિંમત_request_received, counts.set_કિંમત_response_sent);
	PX4_INFO("set value requests sent: %" PRIu32 ", set value responses received: %" PRIu32,
		 counts.set_કિંમત_request_sent, counts.set_કિંમત_response_received);
	PX4_INFO("resets received: %" PRIu32 ", set used requests sent: %" PRIu32,
		 counts.reset_received, counts.set_used_sent);
#endif

}
