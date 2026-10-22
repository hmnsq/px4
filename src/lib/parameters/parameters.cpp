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
#अन्तर्भाव "param.h"
#अन्तर्भाव "param_translation.h"
#अन्तर्भाव <parameters/px4_parameters.hpp>
#अन्तर्भाव <lib/tinybson/tinybson.h>

#यदि defined(__PX4_NUTTX)
#अन्तर्भाव <nuttx/crc32.h>
#अन्यथा
#अन्तर्भाव <crc32.h>
#endif
#अन्तर्भाव <float.h>
#अन्तर्भाव <math.h>
#अन्तर्भाव <sys/stat.h>

#अन्तर्भाव <containers/Bitset.hpp>
#अन्तर्भाव <drivers/drv_hrt.h>
#अन्तर्भाव <lib/perf/perf_counter.h>
#अन्तर्भाव <px4_platform_common/px4_config.h>
#अन्तर्भाव <px4_platform_common/atomic_bitset.h>
#अन्तर्भाव <px4_platform_common/defines.h>
#अन्तर्भाव <px4_platform_common/posix.h>
#अन्तर्भाव <px4_platform_common/sem.h>
#अन्तर्भाव <px4_platform_common/shutdown.h>
#अन्तर्भाव <px4_platform_common/micro_hal.h>

using नामाकाश time_literals;

#अन्तर्भाव "uORB/uORB.h"
#अन्तर्भाव "uORB/topics/parameter_update.h"
#अन्तर्भाव <uORB/Subscription.hpp>

#अन्तर्भाव "ExhaustiveLayer.h"
#अन्तर्भाव "ConstLayer.h"
#अन्तर्भाव "DynamicSparseLayer.h"
#अन्तर्भाव "StaticSparseLayer.h"

#अन्तर्भाव "atomic_transaction.h"

/* Include functions common to user and kernel sides */
#अन्तर्भाव "parameters_common.cpp"

#यदि defined(__PX4_NUTTX) && !defined(CONFIG_BUILD_FLAT)
#अन्तर्भाव <px4_platform/board_ctrl.h>
#अन्तर्भाव "parameters_ioctl.h"
#endif

#यदि defined(FLASH_BASED_PARAMS)
#अन्तर्भाव "flashparams/flashparams.h"
#अन्यथा
अन्तःस्थ स्थिर पूर्णाङ्क flash_param_save(param_filter_func filter) { प्रत्यागम -1; }
अन्तःस्थ स्थिर पूर्णाङ्क flash_param_import() { प्रत्यागम -1; }
#endif

स्थिर वर्ण *param_default_file = nullptr;
स्थिर वर्ण *param_backup_file = nullptr;

#अन्तर्भाव "autosave.h"
स्थिर ParamAutosave *autosave_instance {nullptr};

स्थिर px4::AtomicBitset<param_info_गिनती> params_active;  // params found
स्थिर px4::AtomicBitset<param_info_गिनती> params_unsaved;

// Params the save in progress is writing. They are moved out of params_unsaved
// before their values are read, so a param_set that lands while the save runs
// marks its param again and reaches the next save. Flash-param saves write only
// unsaved params, so nothing else would ever write that change.
स्थिर px4::AtomicBitset<param_info_गिनती> params_saving;

स्थिर ConstLayer firmware_defaults;
स्थिर DynamicSparseLayer runtime_defaults{&firmware_defaults};
DynamicSparseLayer user_config{&runtime_defaults};

/** parameter update topic handle */
#यदि not defined(CONFIG_PARAM_REMOTE)
स्थिर orb_advert_t param_topic = nullptr;
स्थिर unsigned पूर्णाङ्क param_instance = 0;
#endif

स्थिर perf_counter_t param_export_perf;
स्थिर perf_counter_t param_find_perf;
स्थिर perf_counter_t param_get_perf;
स्थिर perf_counter_t param_set_perf;

स्थिर pthread_mutex_t file_mutex  =
	PTHREAD_MUTEX_INITIALIZER; ///< this protects against concurrent param saves (file or flash access).

// Support for remote parameter node
#यदि defined(CONFIG_PARAM_PRIMARY)
# include "parameters_primary.h"
#endif // CONFIG_PARAM_PRIMARY
#यदि defined(CONFIG_PARAM_REMOTE)
# include "parameters_remote.h"
#endif // CONFIG_PARAM_REMOTE

शून्य
param_init()
{
#ifdef __PX4_POSIX
	// Set up the recursive param mutex before any thread can access parameters.
	AtomicTransaction::initialize();
#endif

	param_export_perf = perf_alloc(PC_ELAPSED, "param: export");
	param_find_perf = perf_alloc(PC_गिनती, "param: find");
	param_get_perf = perf_alloc(PC_गिनती, "param: get");
	param_set_perf = perf_alloc(PC_ELAPSED, "param: set");

#यदि defined(__PX4_NUTTX) && !defined(CONFIG_BUILD_FLAT)
	px4_register_boardct_ioctl(_PARAMIOCBASE, param_ioctl);
#endif

#यदि defined(CONFIG_PARAM_PRIMARY)
	param_primary_init();
#endif // CONFIG_PARAM_PRIMARY

#यदि defined(CONFIG_PARAM_REMOTE)
	param_remote_init();
#endif // CONFIG_PARAM_REMOTE

#यदि not defined(CONFIG_PARAM_REMOTE)
	autosave_instance = नूतन ParamAutosave();
#endif
}


शून्य
param_notify_changes()
{
// Don't send if this is a remote node. Only the primary
// sends out update notices
#यदि not defined(CONFIG_PARAM_REMOTE)
	parameter_update_s pup {};
	pup.instance = param_instance++;
	pup.get_गिनती_2 = perf_event_गिनती(param_get_perf);
	pup.set_गिनती = perf_event_गिनती(param_set_perf);
	pup.find_गिनती = perf_event_गिनती(param_find_perf);
	pup.export_गिनती = perf_event_गिनती(param_export_perf);
	pup.active = params_active.गिनती();
	pup.changed = user_config.size();
	pup.custom_default = runtime_defaults.size();
	pup.timestamp = hrt_absolute_time();

	यदि (param_topic == nullptr) {
		param_topic = orb_advertise(ORB_ID(parameter_update), &pup);

	} अन्यथा {
		orb_publish(ORB_ID(parameter_update), param_topic, &pup);
	}

#endif
}

स्थिर param_t param_find_internal(ध्रुव वर्ण *name, तार्किक notification)
{
	perf_गिनती(param_find_perf);

	param_t मध्य;
	param_t front = 0;
	param_t last = param_info_गिनती;

	/* perform a binary search of the known parameters */

	यावत् (front <= last) {
		मध्य = front + (last - front) / 2;
		पूर्णाङ्क ret = strcmp(name, param_name(मध्य));

		यदि (ret == 0) {
			यदि (notification) {
				param_set_used(मध्य);
			}

			प्रत्यागम मध्य;

		} अन्यथा यदि (मध्य == front) {
			/* An end point has been hit, but there has been no match */
			भङ्ग;

		} अन्यथा यदि (ret < 0) {
			last = मध्य;

		} अन्यथा {
			front = मध्य;
		}
	}

	/* not found */
	प्रत्यागम PARAM_INVALID;
}

param_t param_find(ध्रुव वर्ण *name)
{
	प्रत्यागम param_find_internal(name, सत्य);
}

param_t param_find_no_notification(ध्रुव वर्ण *name)
{
	प्रत्यागम param_find_internal(name, असत्य);
}

unsigned param_गिनती_used()
{
	प्रत्यागम params_active.गिनती();
}

param_t param_for_used_index(unsigned index)
{
	// walk all params and count used params
	यदि (index < param_info_गिनती) {
		unsigned used_गिनती = 0;

		प्रति (पूर्णाङ्क i = 0; i < params_active.size(); i++) {
			यदि (params_active[i]) {
				// we found the right used count,
				//  return the param value
				यदि (index == used_गिनती) {
					प्रत्यागम static_cast<param_t>(i);
				}

				used_गिनती++;
			}
		}
	}

	प्रत्यागम PARAM_INVALID;
}

पूर्णाङ्क param_get_used_index(param_t param)
{
	/* this tests for out of bounds and does a constant time lookup */
	यदि (!param_used(param)) {
		प्रत्यागम -1;
	}

	/* walk all params and count, now knowing that it has a valid index */
	पूर्णाङ्क used_गिनती = 0;

	प्रति (पूर्णाङ्क i = 0; i < params_active.size(); i++) {
		यदि (params_active[i]) {

			यदि (param == i) {
				प्रत्यागम used_गिनती;
			}

			used_गिनती++;
		}
	}

	प्रत्यागम -1;
}

तार्किक
param_मूल्य_unsaved(param_t param)
{
	प्रत्यागम handle_in_range(param) ? (params_unsaved[param] || params_saving[param]) : असत्य;
}

पूर्णाङ्क
param_get(param_t param, शून्य *val)
{
	perf_गिनती(param_get_perf);

	यदि (!handle_in_range(param)) {
		PX4_ERR("get: param %" PRId16 " invalid", param);
		प्रत्यागम PX4_ERROR;
	}

	यदि (!params_active[param]) {
		PX4_DEBUG("get: param %" PRId16 " (%s) not active", param, param_name(param));
	}

	पूर्णाङ्क result = PX4_ERROR;

	यदि (val) {

		auto retrieve_मूल्य = user_config.get(param);

		विकल्प (param_type(param)) {
		पक्ष PARAM_TYPE_INT32:
			memcpy(val, &retrieve_मूल्य.i, परिमाण(retrieve_मूल्य.i));
			प्रत्यागम PX4_OK;

		पक्ष PARAM_TYPE_FLOAT:
			memcpy(val, &retrieve_मूल्य.f, परिमाण(retrieve_मूल्य.f));
			प्रत्यागम PX4_OK;
		}
	}

	प्रत्यागम result;
}

पूर्णाङ्क
param_get_default_मूल्य_internal(param_t param, शून्य *default_val)
{
	यदि (!handle_in_range(param)) {
		PX4_ERR("get default value: param %d invalid", param);
		प्रत्यागम PX4_ERROR;
	}

	यदि (default_val) {
		विकल्प (param_type(param)) {
		पक्ष PARAM_TYPE_INT32: {
				int32_t val = runtime_defaults.get(param).i;
				memcpy(default_val, &val, परिमाण(val));
				प्रत्यागम PX4_OK;
			}

		पक्ष PARAM_TYPE_FLOAT: {
				भिन्न val = runtime_defaults.get(param).f;
				memcpy(default_val, &val, परिमाण(val));
				प्रत्यागम PX4_OK;
			}
		}
	}

	प्रत्यागम PX4_ERROR;
}

पूर्णाङ्क
param_get_default_मूल्य(param_t param, शून्य *default_val)
{
	यदि (!handle_in_range(param)) {
		प्रत्यागम PX4_ERROR;
	}

	पूर्णाङ्क ret = param_get_default_मूल्य_internal(param, default_val);
	प्रत्यागम ret;
}

तार्किक param_मूल्य_is_default(param_t param)
{
	यदि (!handle_in_range(param)) {
		प्रत्यागम सत्य;
	}

	यदि (!user_config.contains(param)) {
		// if user config does not contain it, consider it default.
		प्रत्यागम सत्य;

	} अन्यथा {
		// compare with default value
		ध्रुव param_मूल्य_u user_config_मूल्य = user_config.get(param);
		ध्रुव param_मूल्य_u runtime_default_मूल्य = runtime_defaults.get(param);

		विकल्प (param_type(param)) {
		पक्ष PARAM_TYPE_INT32: {
				प्रत्यागम user_config_मूल्य.i == runtime_default_मूल्य.i;
			}

		पक्ष PARAM_TYPE_FLOAT: {
				प्रत्यागम fabsf(user_config_मूल्य.f - runtime_default_मूल्य.f) < FLT_EPSILON;
			}
		}
	}

	प्रत्यागम सत्य;
}

स्थिर शून्य
param_autosave()
{
	यदि (autosave_instance) {
		autosave_instance->request();
	}
}

शून्य
param_control_autosave(तार्किक enable)
{
	यदि (autosave_instance) {
		autosave_instance->enable(enable);
	}
}

स्थिर पूर्णाङ्क
param_set_internal(param_t param, ध्रुव शून्य *val, तार्किक mark_saved, तार्किक notify_changes, तार्किक update_remote = सत्य,
		   तार्किक autosave = सत्य)
{
	यदि (!handle_in_range(param)) {
		PX4_ERR("set invalid param %d", param);
		प्रत्यागम PX4_ERROR;
	}

	यदि (val == nullptr) {
		PX4_ERR("set invalid value");
		प्रत्यागम PX4_ERROR;
	}

	यदि (param_is_readonly(param)) {
		PX4_WARN("param %s is read-only", param_name(param));
		प्रत्यागम PX4_ERROR;
	}

	पूर्णाङ्क result = -1;
	तार्किक param_changed = असत्य;
	perf_begin(param_set_perf);

	ध्रुव param_मूल्य_u user_config_मूल्य = user_config.get(param);
	param_मूल्य_u new_मूल्य{};

	विकल्प (param_type(param)) {
	पक्ष PARAM_TYPE_INT32:
		memcpy(&new_मूल्य.i, val, परिमाण(new_मूल्य.i));
		param_changed = user_config_मूल्य.i != new_मूल्य.i;
		भङ्ग;

	पक्ष PARAM_TYPE_FLOAT:
		memcpy(&new_मूल्य.f, val, परिमाण(new_मूल्य.f));
		param_changed = fabsf(user_config_मूल्य.f - new_मूल्य.f) > FLT_EPSILON;
		भङ्ग;

	स्वभाव: {
			PX4_ERR("param_set invalid param type for %s", param_name(param));
			भङ्ग;
		}
	}

	यदि (user_config.store(param, new_मूल्य)) {
		यदि (param_changed) {
			params_unsaved.set(param, !mark_saved);
		}

		result = PX4_OK;

	} अन्यथा {
		PX4_ERR("param_set failed to store param %s", param_name(param));
		result = PX4_ERROR;
	}

	यदि ((result == PX4_OK) && param_changed && !mark_saved && autosave) { // this is false when importing parameters
		param_autosave();
	}

	// If this is the parameter server, make sure that the remote is updated
#यदि defined(CONFIG_PARAM_PRIMARY)

	यदि (param_changed && update_remote) {
		param_primary_set_मूल्य(param, val);
	}

#endif

	// If this is the parameter remote, make sure that the primary is updated
#यदि defined(CONFIG_PARAM_REMOTE)

	यदि (param_changed && update_remote) {
		param_remote_set_मूल्य(param, val);
	}

#endif

	perf_end(param_set_perf);

	/*
	 * If we set something, now that we have unlocked, go ahead and advertise that
	 * a thing has been set.
	 */
	यदि ((result == PX4_OK) && param_changed && notify_changes) {
		param_notify_changes();
	}

	प्रत्यागम result;
}

#यदि defined(FLASH_BASED_PARAMS)
पूर्णाङ्क param_set_external(param_t param, ध्रुव शून्य *val, तार्किक mark_saved, तार्किक notify_changes)
{
	प्रत्यागम param_set_internal(param, val, mark_saved, notify_changes);
}

शून्य param_get_external(param_t param, शून्य *val)
{
	param_get(param, val);
}
#endif

पूर्णाङ्क param_set(param_t param, ध्रुव शून्य *val)
{
	प्रत्यागम param_set_internal(param, val, असत्य, सत्य);
}

पूर्णाङ्क param_set_no_notification(param_t param, ध्रुव शून्य *val)
{
	प्रत्यागम param_set_internal(param, val, असत्य, असत्य);
}

पूर्णाङ्क param_set_no_remote_update(param_t param, ध्रुव शून्य *val, तार्किक notify)
{
	प्रत्यागम param_set_internal(param, val, असत्य, notify, असत्य);
}

तार्किक param_used(param_t param)
{
	यदि (handle_in_range(param)) {
		प्रत्यागम params_active[param];
	}

	प्रत्यागम असत्य;
}

शून्य param_set_used(param_t param)
{
	यदि (handle_in_range(param)) {
#यदि defined(CONFIG_PARAM_REMOTE)

		यदि (!param_used(param)) {
			param_remote_set_used(param);
		}

#endif

		params_active.set(param, सत्य);
	}
}

पूर्णाङ्क param_set_default_मूल्य(param_t param, ध्रुव शून्य *val)
{
	यदि (!handle_in_range(param)) {
		PX4_ERR("set default value invalid param %d", param);
		प्रत्यागम PX4_ERROR;
	}

	यदि (val == nullptr) {
		PX4_ERR("set default value invalid value");
		प्रत्यागम PX4_ERROR;
	}

	यदि (param_is_readonly(param)) {
		PX4_WARN("param %s is read-only", param_name(param));
		प्रत्यागम PX4_ERROR;
	}

	पूर्णाङ्क result = PX4_ERROR;


	// check if param being set to default value
	तार्किक setting_to_static_default = असत्य;
	ध्रुव param_मूल्य_u firmware_default_मूल्य = firmware_defaults.get(param);

	विकल्प (param_type(param)) {
	पक्ष PARAM_TYPE_INT32: {
			int32_t new_मूल्य;
			memcpy(&new_मूल्य, val, परिमाण(new_मूल्य));
			setting_to_static_default = firmware_default_मूल्य.i == new_मूल्य;
			भङ्ग;
		}

	पक्ष PARAM_TYPE_FLOAT: {
			भिन्न new_मूल्य;
			memcpy(&new_मूल्य, val, परिमाण(new_मूल्य));
			setting_to_static_default = fabsf(firmware_default_मूल्य.f - new_मूल्य) <= FLT_EPSILON;
			भङ्ग;
		}
	}

	यदि (setting_to_static_default) {
		runtime_defaults.reset(param);

		result = PX4_OK;

	} अन्यथा {
		param_मूल्य_u new_मूल्य{};

		विकल्प (param_type(param)) {
		पक्ष PARAM_TYPE_INT32: {
				memcpy(&new_मूल्य.i, val, परिमाण(new_मूल्य.i));
				भङ्ग;
			}

		पक्ष PARAM_TYPE_FLOAT: {
				memcpy(&new_मूल्य.f, val, परिमाण(new_मूल्य.f));
				भङ्ग;
			}

		स्वभाव:
			भङ्ग;
		}

		यदि (runtime_defaults.store(param, new_मूल्य)) {
			user_config.refresh(param);
			result = PX4_OK;

		} अन्यथा {
			result = PX4_ERROR;
		}
	}


	यदि ((result == PX4_OK) && param_used(param)) {
		// send notification if param is already in use
		param_notify_changes();
	}

	प्रत्यागम result;
}

स्थिर पूर्णाङ्क param_reset_internal(param_t param, तार्किक notify = सत्य, तार्किक autosave = सत्य)
{
#यदि defined(CONFIG_PARAM_REMOTE)
	// Remote doesn't support reset
	प्रत्यागम असत्य;
#endif

	यदि (param_is_readonly(param)) {
		प्रत्यागम 0;  // silently skip — param_reset_all loops over all params
	}

	तार्किक param_found = user_config.contains(param);

	यदि (handle_in_range(param)) {
		user_config.reset(param);
	}

	यदि (param_found) {
		/* Persist the reset. Flash-param saves append only unsaved params,
		 * so a reset has to show up as a tombstone or replay keeps the
		 * previous override. */
		params_unsaved.set(param, सत्य);
	}

	यदि (autosave) {
		param_autosave();
	}

	यदि (param_found && notify) {
		param_notify_changes();
	}

#यदि defined(CONFIG_PARAM_PRIMARY)
	param_primary_reset(param);
#endif

	प्रत्यागम param_found;
}

पूर्णाङ्क param_reset(param_t param) { प्रत्यागम param_reset_internal(param, सत्य); }
पूर्णाङ्क param_reset_no_notification(param_t param) { प्रत्यागम param_reset_internal(param, असत्य); }

स्थिर शून्य
param_reset_all_internal(तार्किक auto_save)
{
#यदि defined(CONFIG_PARAM_REMOTE)
	// Remote doesn't support reset
	प्रत्यागम;
#endif

	प्रति (param_t param = 0; handle_in_range(param); param++) {
		param_reset_internal(param, असत्य, असत्य);
	}

	यदि (auto_save) {
		param_autosave();
	}

#यदि defined(CONFIG_PARAM_PRIMARY)
	param_primary_reset_all();
#endif

	param_notify_changes();
}

शून्य
param_reset_all()
{
	param_reset_all_internal(सत्य);
}

शून्य
param_reset_excludes(ध्रुव वर्ण *excludes[], पूर्णाङ्क अंक_excludes)
{
	प्रति (param_t param = 0; handle_in_range(param); param++) {
		ध्रुव वर्ण *name = param_name(param);
		तार्किक exclude = असत्य;

		प्रति (पूर्णाङ्क index = 0; index < अंक_excludes; index ++) {
			पूर्णाङ्क len = strlen(excludes[index]);

			यदि ((excludes[index][len - 1] == '*'
			     && strncmp(name, excludes[index], len - 1) == 0)
			    || strcmp(name, excludes[index]) == 0) {

				exclude = सत्य;
				भङ्ग;
			}
		}

		यदि (!exclude) {
			param_reset(param);
		}
	}
}

शून्य
param_reset_specific(ध्रुव वर्ण *resets[], पूर्णाङ्क अंक_resets)
{
	प्रति (param_t param = 0; handle_in_range(param); param++) {
		ध्रुव वर्ण *name = param_name(param);
		तार्किक reset = असत्य;

		प्रति (पूर्णाङ्क index = 0; index < अंक_resets; index++) {
			पूर्णाङ्क len = strlen(resets[index]);

			यदि ((resets[index][len - 1] == '*'
			     && strncmp(name, resets[index], len - 1) == 0)
			    || strcmp(name, resets[index]) == 0) {

				reset = सत्य;
				भङ्ग;
			}
		}

		यदि (reset) {
			param_reset(param);
		}
	}
}

पूर्णाङ्क
param_set_default_file(ध्रुव वर्ण *filename)
{
	यदि (filename && param_backup_file && strcmp(filename, param_backup_file) == 0) {
		PX4_ERR("default file can't be the same as the backup file %s", filename);
		प्रत्यागम PX4_ERROR;
	}

#ifdef FLASH_BASED_PARAMS
	// the default for flash-based params is always the FLASH
	(शून्य)filename;
#अन्यथा

	यदि (param_default_file != nullptr) {
		// we assume this is not in use by some other thread
		मुक्त(param_default_file);
		param_default_file = nullptr;
	}

	यदि (filename) {
		param_default_file = strdup(filename);
	}

#endif /* FLASH_BASED_PARAMS */

	प्रत्यागम 0;
}

ध्रुव वर्ण *param_get_default_file()
{
	प्रत्यागम param_default_file;
}

पूर्णाङ्क param_set_backup_file(ध्रुव वर्ण *filename)
{
	यदि (filename && param_default_file && strcmp(filename, param_default_file) == 0) {
		PX4_ERR("backup file can't be the same as the default file %s", filename);
		प्रत्यागम PX4_ERROR;
	}

	यदि (param_backup_file != nullptr) {
		// we assume this is not in use by some other thread
		मुक्त(param_backup_file);
		param_backup_file = nullptr;
	}

	यदि (filename) {
		param_backup_file = strdup(filename);

	} अन्यथा {
		param_backup_file = nullptr; // backup disabled
	}

	प्रत्यागम 0;
}

ध्रुव वर्ण *param_get_backup_file()
{
	प्रत्यागम param_backup_file;
}

स्थिर पूर्णाङ्क param_export_internal(पूर्णाङ्क fd, param_filter_func filter);
स्थिर पूर्णाङ्क param_verify(पूर्णाङ्क fd);

पूर्णाङ्क param_save_default(तार्किक blocking)
{
	PX4_DEBUG("param_save_default");

	// take the file lock
	यदि (blocking) {
		pthread_mutex_lock(&file_mutex);

	} अन्यथा {
		यदि (pthread_mutex_trylock(&file_mutex) != 0) {
			PX4_DEBUG("param_save_default: file lock failed (already locked)");
			प्रत्यागम -EWOULDBLOCK;
		}
	}

	पूर्णाङ्क shutdown_lock_ret = px4_shutdown_lock();

	यदि (shutdown_lock_ret != 0) {
		PX4_ERR("px4_shutdown_lock() failed (%i)", shutdown_lock_ret);
	}

	पूर्णाङ्क res = PX4_ERROR;
	ध्रुव वर्ण *filename = param_get_default_file();

	प्रति (param_t param = 0; handle_in_range(param); param++) {
		यदि (params_unsaved[param]) {
			params_saving.set(param, सत्य);
			params_unsaved.set(param, असत्य);
		}
	}

	यदि (filename) {
		स्थिर constexpr पूर्णाङ्क MAX_ATTEMPTS = 3;

		प्रति (पूर्णाङ्क attempt = 1; attempt <= MAX_ATTEMPTS; attempt++) {
			// write parameters to file
			पूर्णाङ्क fd = ::open(filename, O_WRONLY | O_CREAT | O_TRUNC, PX4_O_MODE_666);

			यदि (fd > -1) {
				perf_begin(param_export_perf);
				res = param_export_internal(fd, nullptr);
				perf_end(param_export_perf);
				::close(fd);

				यदि (res == PX4_OK) {
					// reopen file to verify
					पूर्णाङ्क fd_verify = ::open(filename, O_RDONLY, PX4_O_MODE_666);
					res = param_verify(fd_verify) || lseek(fd_verify, 0, SEEK_SET) || param_verify(fd_verify);
					::close(fd_verify);
				}
			}

			यदि (res == PX4_OK) {
				भङ्ग;

			} अन्यथा {
				PX4_ERR("parameter export to %s failed (%d) attempt %d", filename, res, attempt);
				px4_usleep(10000); // wait at least 10 milliseconds before trying again
			}
		}

	} अन्यथा {
		perf_begin(param_export_perf);
		res = flash_param_save(nullptr);
		perf_end(param_export_perf);
	}

	यदि (res != PX4_OK) {
		PX4_ERR("param export failed (%d)", res);

		प्रति (param_t param = 0; handle_in_range(param); param++) {
			यदि (params_saving[param]) {
				params_unsaved.set(param, सत्य);
			}
		}
	}

	params_saving.reset();

	यदि (res == PX4_OK) {
		// backup file
		यदि (param_backup_file) {
			पूर्णाङ्क fd_backup_file = ::open(param_backup_file, O_WRONLY | O_CREAT | O_TRUNC, PX4_O_MODE_666);

			यदि (fd_backup_file > -1) {
				पूर्णाङ्क backup_export_ret = param_export_internal(fd_backup_file, nullptr);
				::close(fd_backup_file);

				यदि (backup_export_ret != 0) {
					PX4_ERR("backup parameter export to %s failed (%d)", param_backup_file, backup_export_ret);

				} अन्यथा {
					// verify export
					पूर्णाङ्क fd_verify = ::open(param_backup_file, O_RDONLY, PX4_O_MODE_666);
					param_verify(fd_verify);
					::close(fd_verify);
				}
			}
		}
	}

	pthread_mutex_unlock(&file_mutex);

	यदि (shutdown_lock_ret == 0) {
		px4_shutdown_unlock();
	}

	प्रत्यागम res;
}

/**
 * @return 0 on success, 1 if all params have not yet been stored, -1 if device open failed, -2 if writing parameters failed
 */
पूर्णाङ्क
param_load_default()
{
	पूर्णाङ्क res = 0;
	ध्रुव वर्ण *filename = param_get_default_file();

	यदि (!filename) {
		प्रत्यागम param_load(-1);
	}

	पूर्णाङ्क fd_load = ::open(filename, O_RDONLY);

	यदि (fd_load < 0) {
		/* no parameter file is OK, otherwise this is an error */
		यदि (errno != ENOENT) {
			PX4_ERR("open '%s' for reading failed", filename);
			प्रत्यागम -1;
		}

		प्रत्यागम 1;
	}

	पूर्णाङ्क result = param_load(fd_load);
	::close(fd_load);

	यदि (result == 1) {
		/* blank file: nothing stored yet */
		प्रत्यागम 1;
	}

	यदि (result != 0) {
		PX4_ERR("error reading parameters from '%s'", filename);
		प्रत्यागम -2;
	}

	प्रत्यागम res;
}

स्थिर पूर्णाङ्क param_verify_callback(bson_decoder_t decoder, bson_गाँठ_t गाँठ_2)
{
	यदि (गाँठ_2->type == BSON_EOO) {
		प्रत्यागम 0;
	}

	// find the parameter this node represents
	param_t param = param_find_no_notification(गाँठ_2->name);

	यदि (param == PARAM_INVALID) {
		PX4_ERR("verify: invalid parameter '%s'", गाँठ_2->name);
		प्रत्यागम -1;
	}

	// handle verifying the parameter from the node
	विकल्प (गाँठ_2->type) {
	पक्ष BSON_INT32: {
			यदि (param_type(param) != PARAM_TYPE_INT32) {
				PX4_ERR("verify: invalid param type %d for '%s' (BSON_INT32)", param_type(param), गाँठ_2->name);
				प्रत्यागम -1;
			}

			int32_t value;

			यदि (param_get(param, &value) == 0) {
				यदि (value == गाँठ_2->i32) {
					प्रत्यागम 1; // valid

				} अन्यथा {
					PX4_ERR("verify: '%s' invalid BSON value %" PRIi32 "(expected %" PRIi32 ")", गाँठ_2->name, गाँठ_2->i32, value);
				}
			}
		}
		भङ्ग;

	पक्ष BSON_DOUBLE: {
			यदि (param_type(param) != PARAM_TYPE_FLOAT) {
				PX4_ERR("verify: invalid param type %d for '%s' (BSON_DOUBLE)", param_type(param), गाँठ_2->name);
				प्रत्यागम -1;
			}

			भिन्न value;

			यदि (param_get(param, &value) == 0) {
				यदि (fabsf(value - (भिन्न)गाँठ_2->d) <= FLT_EPSILON) {
					प्रत्यागम 1; // valid

				} अन्यथा {
					PX4_ERR("verify: '%s' invalid BSON value %.3f (expected %.3f)", गाँठ_2->name, गाँठ_2->d, (द्विगुण)value);
				}
			}
		}
		भङ्ग;

	स्वभाव:
		PX4_ERR("verify: '%s' invalid node type %d", गाँठ_2->name, गाँठ_2->type);
	}

	प्रत्यागम -1;
}

स्थिर पूर्णाङ्क param_verify(पूर्णाङ्क fd)
{
	PX4_DEBUG("param_verify");

	यदि (fd < 0) {
		प्रत्यागम -1;
	}

	यदि (lseek(fd, 0, SEEK_SET) != 0) {
		PX4_ERR("verify: seek failed");
		प्रत्यागम -1;
	}

	bson_decoder_s decoder{};

	यदि (bson_decoder_init_file(&decoder, fd, param_verify_callback) == 0) {
		पूर्णाङ्क result = -1;

		कुरु {
			result = bson_decoder_next(&decoder);

		} यावत् (result > 0);

		यदि (result == 0) {
			यदि (decoder.total_document_size != decoder.total_decoded_size) {
				PX4_ERR("BSON document size (%" PRId32 ") doesn't match bytes decoded (%" PRId32 ")", decoder.total_document_size,
					decoder.total_decoded_size);

			} अन्यथा {
				प्रत्यागम 0;
			}

		} अन्यथा यदि (result == -ENODATA) {
			PX4_ERR("verify: no BSON data");

		} अन्यथा {
			PX4_ERR("verify: failed (%d)", result);
		}
	}

	प्रत्यागम -1;
}

पूर्णाङ्क
param_export(ध्रुव वर्ण *filename, param_filter_func filter)
{
	PX4_DEBUG("param_export");

	पूर्णाङ्क shutdown_lock_ret = px4_shutdown_lock();

	यदि (shutdown_lock_ret != 0) {
		PX4_ERR("px4_shutdown_lock() failed (%i)", shutdown_lock_ret);
	}

	// take the file lock
	यदि (pthread_mutex_trylock(&file_mutex) != 0) {
		PX4_ERR("param_export: file lock failed (already locked)");
		प्रत्यागम PX4_ERROR;
	}

	पूर्णाङ्क fd = ::open(filename, O_RDWR | O_CREAT, PX4_O_MODE_666);
	पूर्णाङ्क result = PX4_ERROR;

	perf_begin(param_export_perf);

	यदि (fd > -1) {
		result = param_export_internal(fd, filter);

	} अन्यथा {
		result = flash_param_save(filter);
	}

	perf_end(param_export_perf);

	pthread_mutex_unlock(&file_mutex);

	यदि (shutdown_lock_ret == 0) {
		px4_shutdown_unlock();
	}

	प्रत्यागम result;
}

// internal parameter export, caller is responsible for locking
स्थिर पूर्णाङ्क param_export_internal(पूर्णाङ्क fd, param_filter_func filter)
{
	PX4_DEBUG("param_export_internal");
	ध्रुव auto changed_params = user_config.containedAsBitset();

	पूर्णाङ्क result = -1;
	bson_encoder_s encoder{};
	uint8_t bson_buffer[256];

	यदि (lseek(fd, 0, SEEK_SET) != 0) {
		PX4_ERR("export seek failed %d", errno);
		प्रत्यागम -1;
	}

	यदि (bson_encoder_init_buf_file(&encoder, fd, &bson_buffer, परिमाण(bson_buffer)) != 0) {
		गच्छ out;
	}

	प्रति (param_t param = 0; handle_in_range(param); param++) {
		यदि (!changed_params[param] || (filter && !filter(param))) {
			अनुवर्त;
		}

		ध्रुव param_मूल्य_u runtime_default_मूल्य = runtime_defaults.get(param);
		ध्रुव param_मूल्य_u user_config_मूल्य = user_config.get(param);

		// don't export default values
		विकल्प (param_type(param)) {
		पक्ष PARAM_TYPE_INT32:
			यदि (user_config_मूल्य.i == runtime_default_मूल्य.i) {
				PX4_DEBUG("skipping %s %" PRIi32 " export", param_name(param), runtime_default_मूल्य.i);
				अनुवर्त;
			}

			भङ्ग;

		पक्ष PARAM_TYPE_FLOAT:
			यदि (fabsf(user_config_मूल्य.f - runtime_default_मूल्य.f) <= FLT_EPSILON) {
				PX4_DEBUG("skipping %s %.3f export", param_name(param), (द्विगुण)runtime_default_मूल्य.f);
				अनुवर्त;
			}

			भङ्ग;
		}

		ध्रुव वर्ण *name = param_name(param);
		ध्रुव size_t size = param_size(param);

		/* append the appropriate BSON type object */
		विकल्प (param_type(param)) {
		पक्ष PARAM_TYPE_INT32: {
				ध्रुव int32_t i = user_config_मूल्य.i;
				PX4_DEBUG("exporting: %s (%d) size: %lu val: %" PRIi32, name, param, (long unsigned पूर्णाङ्क)size, i);

				यदि (bson_encoder_append_int32(&encoder, name, i) != 0) {
					PX4_ERR("BSON append failed for '%s'", name);
					गच्छ out;
				}
			}
			भङ्ग;

		पक्ष PARAM_TYPE_FLOAT: {
				ध्रुव द्विगुण f = (द्विगुण)user_config_मूल्य.f;
				PX4_DEBUG("exporting: %s (%d) size: %lu val: %.3f", name, param, (long unsigned पूर्णाङ्क)size, (द्विगुण)f);

				यदि (bson_encoder_append_double(&encoder, name, f) != 0) {
					PX4_ERR("BSON append failed for '%s'", name);
					गच्छ out;
				}
			}
			भङ्ग;

		स्वभाव:
			PX4_ERR("%s unrecognized parameter type %d, skipping export", name, param_type(param));
		}
	}

	result = 0;

out:

	यदि (result == 0) {
		यदि (bson_encoder_fini(&encoder) != PX4_OK) {
			PX4_ERR("BSON encoder finalize failed");
			result = -1;
		}
	}

	प्रत्यागम result;
}

#यदि defined(FLASH_BASED_PARAMS)
// A file is never the flash backend's store, so what an import changes is unsaved until a delta carries it.
// As on the file backend, the import itself does not save.
स्थिर constexpr तार्किक file_import_is_saved = असत्य;
#अन्यथा
स्थिर constexpr तार्किक file_import_is_saved = सत्य;
#endif

स्थिर पूर्णाङ्क
param_import_callback(bson_decoder_t decoder, bson_गाँठ_t गाँठ_2)
{
	/*
	 * EOO means the end of the parameter object. (Currently not supporting
	 * nested BSON objects).
	 */
	यदि (गाँठ_2->type == BSON_EOO) {
		PX4_DEBUG("end of parameters");
		प्रत्यागम 0;
	}

	// A tombstone has no value. The decoder leaves the previous node's in the union and the translations
	// read it without a type check.
	ध्रुव तार्किक tombstone = गाँठ_2->type == BSON_nullptr || गाँठ_2->type == BSON_UNDEFINED;

	// if we do param_set() directly in the translation, set PARAM_SKIP_IMPORT as return value and return here
	यदि (!tombstone && param_modify_on_import(गाँठ_2) == param_modify_on_import_ret::PARAM_SKIP_IMPORT) {
		प्रत्यागम 1;
	}

	// Find the parameter this node represents.  If we don't know it, ignore the node.
	param_t param = param_find_no_notification(गाँठ_2->name);

	यदि (param == PARAM_INVALID) {
		PX4_WARN("ignoring unrecognised parameter '%s'", गाँठ_2->name);
		प्रत्यागम 1;
	}

	// Handle setting the parameter from the node
	विकल्प (गाँठ_2->type) {
	पक्ष BSON_nullptr:
	पक्ष BSON_UNDEFINED:
		param_reset_internal(param, सत्य, असत्य);
		प्रत्यागम 1;

	पक्ष BSON_INT32: {
			यदि (param_type(param) == PARAM_TYPE_INT32) {
				int32_t i = गाँठ_2->i32;
				param_set_internal(param, &i, file_import_is_saved, सत्य, सत्य, असत्य);
				PX4_DEBUG("Imported %s with value %" PRIi32, param_name(param), i);

			} अन्यथा {
				PX4_WARN("unexpected type for %s", गाँठ_2->name);
			}
		}
		भङ्ग;

	पक्ष BSON_DOUBLE: {
			यदि (param_type(param) == PARAM_TYPE_FLOAT) {
				भिन्न f = गाँठ_2->d;
				param_set_internal(param, &f, file_import_is_saved, सत्य, सत्य, असत्य);
				PX4_DEBUG("Imported %s with value %f", param_name(param), (द्विगुण)f);

			} अन्यथा {
				PX4_WARN("unexpected type for %s", गाँठ_2->name);
			}
		}
		भङ्ग;

	स्वभाव:
		PX4_ERR("import: unrecognised node type for '%s'", गाँठ_2->name);
	}

	// don't return zero, that means EOF
	प्रत्यागम 1;
}

स्थिर पूर्णाङ्क
param_import_internal(पूर्णाङ्क fd)
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
	संरचना stat st;

	यदि (fstat(fd, &st) == 0 && S_ISREG(st.st_mode) && st.st_size == 0) {
		प्रत्यागम 1;
	}

	int32_t doc_size = 0;
	ssize_t len = पठ(fd, &doc_size, परिमाण(doc_size));

	यदि (len == 0 || (len == (ssize_t)परिमाण(doc_size) && (doc_size == 0 || doc_size == -1))) {
		प्रत्यागम 1;
	}

	यदि (lseek(fd, 0, SEEK_SET) != 0) {
		PX4_ERR("import lseek failed (%d)", errno);
		प्रत्यागम -1;
	}

	स्थिर constexpr पूर्णाङ्क MAX_ATTEMPTS = 3;

	प्रति (पूर्णाङ्क attempt = 1; attempt <= MAX_ATTEMPTS; attempt++) {
		bson_decoder_s decoder{};

		यदि (bson_decoder_init_file(&decoder, fd, param_import_callback) == 0) {
			पूर्णाङ्क result = -1;

			कुरु {
				result = bson_decoder_next(&decoder);

			} यावत् (result > 0);

			यदि (result == 0) {
				यदि (decoder.total_document_size == decoder.total_decoded_size) {
					PX4_INFO("BSON document size %" PRId32 " bytes, decoded %" PRId32 " bytes (INT32:%" PRIu16 ", FLOAT:%" PRIu16 ")",
						 decoder.total_document_size, decoder.total_decoded_size,
						 decoder.गिनती_गाँठ_int32, decoder.गिनती_गाँठ_double);

					प्रत्यागम 0;

				} अन्यथा {
					PX4_ERR("BSON document size (%" PRId32 ") doesn't match bytes decoded (%" PRId32 ")",
						decoder.total_document_size, decoder.total_decoded_size);
				}

			} अन्यथा यदि (result == -ENODATA) {
				// silently retry as a precaution unless this is our last attempt
				यदि (attempt == MAX_ATTEMPTS) {
					PX4_DEBUG("BSON: no data");
					प्रत्यागम 0;
				}

			} अन्यथा {
				PX4_ERR("param import failed (%d) attempt %d", result, attempt);
			}

		} अन्यथा {
			PX4_ERR("param import bson decoder init failed (attempt %d)", attempt);
		}

		यदि (attempt != MAX_ATTEMPTS) {
			यदि (lseek(fd, 0, SEEK_SET) != 0) {
				PX4_ERR("import lseek failed (%d)", errno);
			}

			px4_usleep(10000); // wait at least 10 milliseconds before trying again
		}
	}

	प्रत्यागम -1;
}

#यदि defined(FLASH_BASED_PARAMS)
स्थिर पूर्णाङ्क flash_backend_locked(पूर्णाङ्क (*op)())
{
	/* Same order as param_save_default: mutex then shutdown lock. Autosave
	 * uses a trylock, so a compact holds it off the flash programming. */
	pthread_mutex_lock(&file_mutex);

	पूर्णाङ्क shutdown_lock_ret = px4_shutdown_lock();

	यदि (shutdown_lock_ret != 0) {
		PX4_ERR("px4_shutdown_lock() failed (%i)", shutdown_lock_ret);
	}

	पूर्णाङ्क result = op();

	pthread_mutex_unlock(&file_mutex);

	यदि (shutdown_lock_ret == 0) {
		px4_shutdown_unlock();
	}

	प्रत्यागम result;
}

स्थिर पूर्णाङ्क flash_load_locked()
{
	/* Reset under the lock so a pending autosave cannot append tombstones
	 * of the just-cleared RAM before import/compact run. */
	param_reset_all_internal(असत्य);
	पूर्णाङ्क result = flash_param_import();

	यदि (result == 0 || result == 1) {
		params_unsaved.reset();
	}

	प्रत्यागम result;
}
#endif

पूर्णाङ्क
param_import(पूर्णाङ्क fd)
{
	यदि (fd < 0) {
#यदि defined(FLASH_BASED_PARAMS)
		प्रत्यागम flash_backend_locked(flash_param_import);
#अन्यथा
		प्रत्यागम flash_param_import();
#endif
	}

	प्रत्यागम param_import_internal(fd);
}

पूर्णाङ्क
param_load(पूर्णाङ्क fd)
{
	यदि (fd < 0) {
#यदि defined(FLASH_BASED_PARAMS)
		प्रत्यागम flash_backend_locked(flash_load_locked);
#अन्यथा
		प्रत्यागम PX4_ERROR;
#endif
	}

	param_reset_all_internal(असत्य);
	पूर्णाङ्क result = param_import_internal(fd);

#यदि !defined(FLASH_BASED_PARAMS)

	यदि (result >= 0) {
		params_unsaved.reset();
	}

#endif

	प्रत्यागम result;
}

शून्य
param_foreach(शून्य (*func)(शून्य *arg, param_t param), शून्य *arg, तार्किक only_changed, तार्किक only_used)
{
	param_t	param;

	प्रति (param = 0; handle_in_range(param); param++) {

		/* if requested, skip unchanged values */
		यदि (only_changed && (!user_config.contains(param))) {
			अनुवर्त;
		}

		यदि (only_used && !param_used(param)) {
			अनुवर्त;
		}

		func(arg, param);
	}
}

uint32_t param_hash_check()
{
	uint32_t param_hash = 0;

	/* compute the CRC32 over all string param names and 4 byte values */
	प्रति (param_t param = 0; handle_in_range(param); param++) {
		यदि (!param_used(param) || param_is_volatile(param)) {
			अनुवर्त;
		}

		ध्रुव वर्ण *name = param_name(param);
		auto value = user_config.get(param).i;
		ध्रुव शून्य *val = (शून्य *)&value;
		param_hash = crc32part((ध्रुव uint8_t *)name, strlen(name), param_hash);
		param_hash = crc32part((ध्रुव uint8_t *)val, param_size(param), param_hash);
	}

	प्रत्यागम param_hash;
}

शून्य param_print_status()
{
	PX4_INFO("summary: %d/%d (used/total)", param_गिनती_used(), param_गिनती());

#ifndef FLASH_BASED_PARAMS
	ध्रुव वर्ण *filename = param_get_default_file();

	यदि (filename != nullptr) {
		PX4_INFO("file: %s", param_get_default_file());
	}

	यदि (param_backup_file) {
		PX4_INFO("backup file: %s", param_backup_file);
	}

#endif /* FLASH_BASED_PARAMS */

	PX4_INFO("storage array: %d/%d elements (%zu bytes total)",
		 user_config.size(), firmware_defaults.size(), (size_t)user_config.byteSize());


	PX4_INFO("storage array (custom defaults): %d/%d elements (%zu bytes total)",
		 runtime_defaults.size(), firmware_defaults.size(), (size_t)runtime_defaults.byteSize());

	यदि (autosave_instance) {
		PX4_INFO("auto save: %s", autosave_instance->enabled() ? "on" : "off");

		hrt_abstime last_autosave = autosave_instance->lastAutosave();

		यदि (last_autosave > 0) {
			PX4_INFO("last auto save: %.3f seconds ago", hrt_elapsed_time(&last_autosave) * 1e-6);
		}
	}

	perf_print_counter(param_export_perf);
	perf_print_counter(param_find_perf);
	perf_print_counter(param_get_perf);
	perf_print_counter(param_set_perf);

#यदि defined(CONFIG_PARAM_PRIMARY)
	संरचना param_primary_counters counts;
	param_primary_get_counters(&counts);
	PX4_INFO("set value requests received: %" PRIu32 ", set value responses sent: %" PRIu32,
		 counts.set_मूल्य_request_received, counts.set_मूल्य_response_sent);
	PX4_INFO("set value requests sent: %" PRIu32 ", set value responses received: %" PRIu32,
		 counts.set_मूल्य_request_sent, counts.set_मूल्य_response_received);
	PX4_INFO("resets sent: %" PRIu32 ", set used requests received: %" PRIu32,
		 counts.reset_sent, counts.set_used_received);
#endif

#यदि defined(CONFIG_PARAM_REMOTE)
	संरचना param_remote_counters counts;
	param_remote_get_counters(&counts);
	PX4_INFO("set value requests received: %" PRIu32 ", set value responses sent: %" PRIu32,
		 counts.set_मूल्य_request_received, counts.set_मूल्य_response_sent);
	PX4_INFO("set value requests sent: %" PRIu32 ", set value responses received: %" PRIu32,
		 counts.set_मूल्य_request_sent, counts.set_मूल्य_response_received);
	PX4_INFO("resets received: %" PRIu32 ", set used requests sent: %" PRIu32,
		 counts.reset_received, counts.set_used_sent);
#endif

}
