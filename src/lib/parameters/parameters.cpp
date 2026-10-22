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

#सचे defined(__PX4_NUTTX)
#अन्तर्भाव <nuttx/crc32.h>
#अञ्ञथा
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

#सचे defined(__PX4_NUTTX) && !defined(CONFIG_BUILD_FLAT)
#अन्तर्भाव <px4_platform/board_ctrl.h>
#अन्तर्भाव "parameters_ioctl.h"
#endif

#सचे defined(FLASH_BASED_PARAMS)
#अन्तर्भाव "flashparams/flashparams.h"
#अञ्ञथा
अन्तःस्थ स्थिर पूर्णाङ्क flash_param_save(param_filter_func filter) { पटिनिवत्त -1; }
अन्तःस्थ स्थिर पूर्णाङ्क flash_param_import() { पटिनिवत्त -1; }
#endif

स्थिर वर्ण *param_default_file = nullptr;
स्थिर वर्ण *param_backup_file = nullptr;

#अन्तर्भाव "autosave.h"
स्थिर ParamAutosave *autosave_instance {nullptr};

स्थिर px4::AtomicBitset<param_info_गणनम्> params_active;  // params found
स्थिर px4::AtomicBitset<param_info_गणनम्> params_unsaved;

// Params the save in progress is writing. They are moved out of params_unsaved
// before their values are read, so a param_set that lands while the save runs
// marks its param again and reaches the next save. Flash-param saves write only
// unsaved params, so nothing else would ever write that change.
स्थिर px4::AtomicBitset<param_info_गणनम्> params_saving;

स्थिर ConstLayer firmware_defaults;
स्थिर DynamicSparseLayer runtime_defaults{&firmware_defaults};
DynamicSparseLayer user_config{&runtime_defaults};

/** parameter update topic handle */
#सचे not defined(CONFIG_PARAM_REMOTE)
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
#सचे defined(CONFIG_PARAM_PRIMARY)
# include "parameters_primary.h"
#endif // CONFIG_PARAM_PRIMARY
#सचे defined(CONFIG_PARAM_REMOTE)
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
	param_find_perf = perf_alloc(PC_गणनम्, "param: find");
	param_get_perf = perf_alloc(PC_गणनम्, "param: get");
	param_set_perf = perf_alloc(PC_ELAPSED, "param: set");

#सचे defined(__PX4_NUTTX) && !defined(CONFIG_BUILD_FLAT)
	px4_register_boardct_ioctl(_PARAMIOCBASE, param_ioctl);
#endif

#सचे defined(CONFIG_PARAM_PRIMARY)
	param_primary_init();
#endif // CONFIG_PARAM_PRIMARY

#सचे defined(CONFIG_PARAM_REMOTE)
	param_remote_init();
#endif // CONFIG_PARAM_REMOTE

#सचे not defined(CONFIG_PARAM_REMOTE)
	autosave_instance = नूतन ParamAutosave();
#endif
}


शून्य
param_notify_changes()
{
// Don't send if this is a remote node. Only the primary
// sends out update notices
#सचे not defined(CONFIG_PARAM_REMOTE)
	parameter_update_s pup {};
	pup.instance = param_instance++;
	pup.get_गणनम्_2 = perf_event_गणनम्(param_get_perf);
	pup.set_गणनम् = perf_event_गणनम्(param_set_perf);
	pup.find_गणनम् = perf_event_गणनम्(param_find_perf);
	pup.export_गणनम् = perf_event_गणनम्(param_export_perf);
	pup.active = params_active.गणनम्();
	pup.changed = user_config.size();
	pup.custom_default = runtime_defaults.size();
	pup.timestamp = hrt_absolute_time();

	सचे (param_topic == nullptr) {
		param_topic = orb_advertise(ORB_ID(parameter_update), &pup);

	} अञ्ञथा {
		orb_publish(ORB_ID(parameter_update), param_topic, &pup);
	}

#endif
}

स्थिर param_t param_find_internal(ध्रुव वर्ण *name, तार्किक notification)
{
	perf_गणनम्(param_find_perf);

	param_t मध्यः;
	param_t front = 0;
	param_t last = param_info_गणनम्;

	/* perform a binary search of the known parameters */

	याव (front <= last) {
		मध्यः = front + (last - front) / 2;
		पूर्णाङ्क ret = strcmp(name, param_name(मध्यः));

		सचे (ret == 0) {
			सचे (notification) {
				param_set_used(मध्यः);
			}

			पटिनिवत्त मध्यः;

		} अञ्ञथा सचे (मध्यः == front) {
			/* An end point has been hit, but there has been no match */
			भङ्ग;

		} अञ्ञथा सचे (ret < 0) {
			last = मध्यः;

		} अञ्ञथा {
			front = मध्यः;
		}
	}

	/* not found */
	पटिनिवत्त PARAM_INVALID;
}

param_t param_find(ध्रुव वर्ण *name)
{
	पटिनिवत्त param_find_internal(name, सच्च);
}

param_t param_find_no_notification(ध्रुव वर्ण *name)
{
	पटिनिवत्त param_find_internal(name, मिच्छा);
}

unsigned param_गणनम्_used()
{
	पटिनिवत्त params_active.गणनम्();
}

param_t param_for_used_index(unsigned index)
{
	// walk all params and count used params
	सचे (index < param_info_गणनम्) {
		unsigned used_गणनम् = 0;

		अत्थाय (पूर्णाङ्क i = 0; i < params_active.size(); i++) {
			सचे (params_active[i]) {
				// we found the right used count,
				//  return the param value
				सचे (index == used_गणनम्) {
					पटिनिवत्त static_cast<param_t>(i);
				}

				used_गणनम्++;
			}
		}
	}

	पटिनिवत्त PARAM_INVALID;
}

पूर्णाङ्क param_get_used_index(param_t param)
{
	/* this tests for out of bounds and does a constant time lookup */
	सचे (!param_used(param)) {
		पटिनिवत्त -1;
	}

	/* walk all params and count, now knowing that it has a valid index */
	पूर्णाङ्क used_गणनम् = 0;

	अत्थाय (पूर्णाङ्क i = 0; i < params_active.size(); i++) {
		सचे (params_active[i]) {

			सचे (param == i) {
				पटिनिवत्त used_गणनम्;
			}

			used_गणनम्++;
		}
	}

	पटिनिवत्त -1;
}

तार्किक
param_मानम्_unsaved(param_t param)
{
	पटिनिवत्त handle_in_range(param) ? (params_unsaved[param] || params_saving[param]) : मिच्छा;
}

पूर्णाङ्क
param_get(param_t param, शून्य *val)
{
	perf_गणनम्(param_get_perf);

	सचे (!handle_in_range(param)) {
		PX4_ERR("get: param %" PRId16 " invalid", param);
		पटिनिवत्त PX4_ERROR;
	}

	सचे (!params_active[param]) {
		PX4_DEBUG("get: param %" PRId16 " (%s) not active", param, param_name(param));
	}

	पूर्णाङ्क result = PX4_ERROR;

	सचे (val) {

		auto retrieve_मानम् = user_config.get(param);

		विकल्प (param_type(param)) {
		पक्ष PARAM_TYPE_INT32:
			memcpy(val, &retrieve_मानम्.i, परिमाण(retrieve_मानम्.i));
			पटिनिवत्त PX4_OK;

		पक्ष PARAM_TYPE_FLOAT:
			memcpy(val, &retrieve_मानम्.f, परिमाण(retrieve_मानम्.f));
			पटिनिवत्त PX4_OK;
		}
	}

	पटिनिवत्त result;
}

पूर्णाङ्क
param_get_default_मानम्_internal(param_t param, शून्य *default_val)
{
	सचे (!handle_in_range(param)) {
		PX4_ERR("get default value: param %d invalid", param);
		पटिनिवत्त PX4_ERROR;
	}

	सचे (default_val) {
		विकल्प (param_type(param)) {
		पक्ष PARAM_TYPE_INT32: {
				int32_t val = runtime_defaults.get(param).i;
				memcpy(default_val, &val, परिमाण(val));
				पटिनिवत्त PX4_OK;
			}

		पक्ष PARAM_TYPE_FLOAT: {
				भिन्न val = runtime_defaults.get(param).f;
				memcpy(default_val, &val, परिमाण(val));
				पटिनिवत्त PX4_OK;
			}
		}
	}

	पटिनिवत्त PX4_ERROR;
}

पूर्णाङ्क
param_get_default_मानम्(param_t param, शून्य *default_val)
{
	सचे (!handle_in_range(param)) {
		पटिनिवत्त PX4_ERROR;
	}

	पूर्णाङ्क ret = param_get_default_मानम्_internal(param, default_val);
	पटिनिवत्त ret;
}

तार्किक param_मानम्_is_default(param_t param)
{
	सचे (!handle_in_range(param)) {
		पटिनिवत्त सच्च;
	}

	सचे (!user_config.contains(param)) {
		// if user config does not contain it, consider it default.
		पटिनिवत्त सच्च;

	} अञ्ञथा {
		// compare with default value
		ध्रुव param_मानम्_u user_config_मानम् = user_config.get(param);
		ध्रुव param_मानम्_u runtime_default_मानम् = runtime_defaults.get(param);

		विकल्प (param_type(param)) {
		पक्ष PARAM_TYPE_INT32: {
				पटिनिवत्त user_config_मानम्.i == runtime_default_मानम्.i;
			}

		पक्ष PARAM_TYPE_FLOAT: {
				पटिनिवत्त fabsf(user_config_मानम्.f - runtime_default_मानम्.f) < FLT_EPSILON;
			}
		}
	}

	पटिनिवत्त सच्च;
}

स्थिर शून्य
param_autosave()
{
	सचे (autosave_instance) {
		autosave_instance->request();
	}
}

शून्य
param_control_autosave(तार्किक enable)
{
	सचे (autosave_instance) {
		autosave_instance->enable(enable);
	}
}

स्थिर पूर्णाङ्क
param_set_internal(param_t param, ध्रुव शून्य *val, तार्किक mark_saved, तार्किक notify_changes, तार्किक update_remote = सच्च,
		   तार्किक autosave = सच्च)
{
	सचे (!handle_in_range(param)) {
		PX4_ERR("set invalid param %d", param);
		पटिनिवत्त PX4_ERROR;
	}

	सचे (val == nullptr) {
		PX4_ERR("set invalid value");
		पटिनिवत्त PX4_ERROR;
	}

	सचे (param_is_readonly(param)) {
		PX4_WARN("param %s is read-only", param_name(param));
		पटिनिवत्त PX4_ERROR;
	}

	पूर्णाङ्क result = -1;
	तार्किक param_changed = मिच्छा;
	perf_begin(param_set_perf);

	ध्रुव param_मानम्_u user_config_मानम् = user_config.get(param);
	param_मानम्_u new_मानम्{};

	विकल्प (param_type(param)) {
	पक्ष PARAM_TYPE_INT32:
		memcpy(&new_मानम्.i, val, परिमाण(new_मानम्.i));
		param_changed = user_config_मानम्.i != new_मानम्.i;
		भङ्ग;

	पक्ष PARAM_TYPE_FLOAT:
		memcpy(&new_मानम्.f, val, परिमाण(new_मानम्.f));
		param_changed = fabsf(user_config_मानम्.f - new_मानम्.f) > FLT_EPSILON;
		भङ्ग;

	स्वभाव: {
			PX4_ERR("param_set invalid param type for %s", param_name(param));
			भङ्ग;
		}
	}

	सचे (user_config.store(param, new_मानम्)) {
		सचे (param_changed) {
			params_unsaved.set(param, !mark_saved);
		}

		result = PX4_OK;

	} अञ्ञथा {
		PX4_ERR("param_set failed to store param %s", param_name(param));
		result = PX4_ERROR;
	}

	सचे ((result == PX4_OK) && param_changed && !mark_saved && autosave) { // this is false when importing parameters
		param_autosave();
	}

	// If this is the parameter server, make sure that the remote is updated
#सचे defined(CONFIG_PARAM_PRIMARY)

	सचे (param_changed && update_remote) {
		param_primary_set_मानम्(param, val);
	}

#endif

	// If this is the parameter remote, make sure that the primary is updated
#सचे defined(CONFIG_PARAM_REMOTE)

	सचे (param_changed && update_remote) {
		param_remote_set_मानम्(param, val);
	}

#endif

	perf_end(param_set_perf);

	/*
	 * If we set something, now that we have unlocked, go ahead and advertise that
	 * a thing has been set.
	 */
	सचे ((result == PX4_OK) && param_changed && notify_changes) {
		param_notify_changes();
	}

	पटिनिवत्त result;
}

#सचे defined(FLASH_BASED_PARAMS)
पूर्णाङ्क param_set_external(param_t param, ध्रुव शून्य *val, तार्किक mark_saved, तार्किक notify_changes)
{
	पटिनिवत्त param_set_internal(param, val, mark_saved, notify_changes);
}

शून्य param_get_external(param_t param, शून्य *val)
{
	param_get(param, val);
}
#endif

पूर्णाङ्क param_set(param_t param, ध्रुव शून्य *val)
{
	पटिनिवत्त param_set_internal(param, val, मिच्छा, सच्च);
}

पूर्णाङ्क param_set_no_notification(param_t param, ध्रुव शून्य *val)
{
	पटिनिवत्त param_set_internal(param, val, मिच्छा, मिच्छा);
}

पूर्णाङ्क param_set_no_remote_update(param_t param, ध्रुव शून्य *val, तार्किक notify)
{
	पटिनिवत्त param_set_internal(param, val, मिच्छा, notify, मिच्छा);
}

तार्किक param_used(param_t param)
{
	सचे (handle_in_range(param)) {
		पटिनिवत्त params_active[param];
	}

	पटिनिवत्त मिच्छा;
}

शून्य param_set_used(param_t param)
{
	सचे (handle_in_range(param)) {
#सचे defined(CONFIG_PARAM_REMOTE)

		सचे (!param_used(param)) {
			param_remote_set_used(param);
		}

#endif

		params_active.set(param, सच्च);
	}
}

पूर्णाङ्क param_set_default_मानम्(param_t param, ध्रुव शून्य *val)
{
	सचे (!handle_in_range(param)) {
		PX4_ERR("set default value invalid param %d", param);
		पटिनिवत्त PX4_ERROR;
	}

	सचे (val == nullptr) {
		PX4_ERR("set default value invalid value");
		पटिनिवत्त PX4_ERROR;
	}

	सचे (param_is_readonly(param)) {
		PX4_WARN("param %s is read-only", param_name(param));
		पटिनिवत्त PX4_ERROR;
	}

	पूर्णाङ्क result = PX4_ERROR;


	// check if param being set to default value
	तार्किक setting_to_static_default = मिच्छा;
	ध्रुव param_मानम्_u firmware_default_मानम् = firmware_defaults.get(param);

	विकल्प (param_type(param)) {
	पक्ष PARAM_TYPE_INT32: {
			int32_t new_मानम्;
			memcpy(&new_मानम्, val, परिमाण(new_मानम्));
			setting_to_static_default = firmware_default_मानम्.i == new_मानम्;
			भङ्ग;
		}

	पक्ष PARAM_TYPE_FLOAT: {
			भिन्न new_मानम्;
			memcpy(&new_मानम्, val, परिमाण(new_मानम्));
			setting_to_static_default = fabsf(firmware_default_मानम्.f - new_मानम्) <= FLT_EPSILON;
			भङ्ग;
		}
	}

	सचे (setting_to_static_default) {
		runtime_defaults.reset(param);

		result = PX4_OK;

	} अञ्ञथा {
		param_मानम्_u new_मानम्{};

		विकल्प (param_type(param)) {
		पक्ष PARAM_TYPE_INT32: {
				memcpy(&new_मानम्.i, val, परिमाण(new_मानम्.i));
				भङ्ग;
			}

		पक्ष PARAM_TYPE_FLOAT: {
				memcpy(&new_मानम्.f, val, परिमाण(new_मानम्.f));
				भङ्ग;
			}

		स्वभाव:
			भङ्ग;
		}

		सचे (runtime_defaults.store(param, new_मानम्)) {
			user_config.refresh(param);
			result = PX4_OK;

		} अञ्ञथा {
			result = PX4_ERROR;
		}
	}


	सचे ((result == PX4_OK) && param_used(param)) {
		// send notification if param is already in use
		param_notify_changes();
	}

	पटिनिवत्त result;
}

स्थिर पूर्णाङ्क param_reset_internal(param_t param, तार्किक notify = सच्च, तार्किक autosave = सच्च)
{
#सचे defined(CONFIG_PARAM_REMOTE)
	// Remote doesn't support reset
	पटिनिवत्त मिच्छा;
#endif

	सचे (param_is_readonly(param)) {
		पटिनिवत्त 0;  // silently skip — param_reset_all loops over all params
	}

	तार्किक param_found = user_config.contains(param);

	सचे (handle_in_range(param)) {
		user_config.reset(param);
	}

	सचे (param_found) {
		/* Persist the reset. Flash-param saves append only unsaved params,
		 * so a reset has to show up as a tombstone or replay keeps the
		 * previous override. */
		params_unsaved.set(param, सच्च);
	}

	सचे (autosave) {
		param_autosave();
	}

	सचे (param_found && notify) {
		param_notify_changes();
	}

#सचे defined(CONFIG_PARAM_PRIMARY)
	param_primary_reset(param);
#endif

	पटिनिवत्त param_found;
}

पूर्णाङ्क param_reset(param_t param) { पटिनिवत्त param_reset_internal(param, सच्च); }
पूर्णाङ्क param_reset_no_notification(param_t param) { पटिनिवत्त param_reset_internal(param, मिच्छा); }

स्थिर शून्य
param_reset_all_internal(तार्किक auto_save)
{
#सचे defined(CONFIG_PARAM_REMOTE)
	// Remote doesn't support reset
	पटिनिवत्त;
#endif

	अत्थाय (param_t param = 0; handle_in_range(param); param++) {
		param_reset_internal(param, मिच्छा, मिच्छा);
	}

	सचे (auto_save) {
		param_autosave();
	}

#सचे defined(CONFIG_PARAM_PRIMARY)
	param_primary_reset_all();
#endif

	param_notify_changes();
}

शून्य
param_reset_all()
{
	param_reset_all_internal(सच्च);
}

शून्य
param_reset_excludes(ध्रुव वर्ण *excludes[], पूर्णाङ्क अङ्कः_excludes)
{
	अत्थाय (param_t param = 0; handle_in_range(param); param++) {
		ध्रुव वर्ण *name = param_name(param);
		तार्किक exclude = मिच्छा;

		अत्थाय (पूर्णाङ्क index = 0; index < अङ्कः_excludes; index ++) {
			पूर्णाङ्क len = strlen(excludes[index]);

			सचे ((excludes[index][len - 1] == '*'
			     && strncmp(name, excludes[index], len - 1) == 0)
			    || strcmp(name, excludes[index]) == 0) {

				exclude = सच्च;
				भङ्ग;
			}
		}

		सचे (!exclude) {
			param_reset(param);
		}
	}
}

शून्य
param_reset_specific(ध्रुव वर्ण *resets[], पूर्णाङ्क अङ्कः_resets)
{
	अत्थाय (param_t param = 0; handle_in_range(param); param++) {
		ध्रुव वर्ण *name = param_name(param);
		तार्किक reset = मिच्छा;

		अत्थाय (पूर्णाङ्क index = 0; index < अङ्कः_resets; index++) {
			पूर्णाङ्क len = strlen(resets[index]);

			सचे ((resets[index][len - 1] == '*'
			     && strncmp(name, resets[index], len - 1) == 0)
			    || strcmp(name, resets[index]) == 0) {

				reset = सच्च;
				भङ्ग;
			}
		}

		सचे (reset) {
			param_reset(param);
		}
	}
}

पूर्णाङ्क
param_set_default_file(ध्रुव वर्ण *filename)
{
	सचे (filename && param_backup_file && strcmp(filename, param_backup_file) == 0) {
		PX4_ERR("default file can't be the same as the backup file %s", filename);
		पटिनिवत्त PX4_ERROR;
	}

#ifdef FLASH_BASED_PARAMS
	// the default for flash-based params is always the FLASH
	(शून्य)filename;
#अञ्ञथा

	सचे (param_default_file != nullptr) {
		// we assume this is not in use by some other thread
		मुक्त(param_default_file);
		param_default_file = nullptr;
	}

	सचे (filename) {
		param_default_file = strdup(filename);
	}

#endif /* FLASH_BASED_PARAMS */

	पटिनिवत्त 0;
}

ध्रुव वर्ण *param_get_default_file()
{
	पटिनिवत्त param_default_file;
}

पूर्णाङ्क param_set_backup_file(ध्रुव वर्ण *filename)
{
	सचे (filename && param_default_file && strcmp(filename, param_default_file) == 0) {
		PX4_ERR("backup file can't be the same as the default file %s", filename);
		पटिनिवत्त PX4_ERROR;
	}

	सचे (param_backup_file != nullptr) {
		// we assume this is not in use by some other thread
		मुक्त(param_backup_file);
		param_backup_file = nullptr;
	}

	सचे (filename) {
		param_backup_file = strdup(filename);

	} अञ्ञथा {
		param_backup_file = nullptr; // backup disabled
	}

	पटिनिवत्त 0;
}

ध्रुव वर्ण *param_get_backup_file()
{
	पटिनिवत्त param_backup_file;
}

स्थिर पूर्णाङ्क param_export_internal(पूर्णाङ्क fd, param_filter_func filter);
स्थिर पूर्णाङ्क param_verify(पूर्णाङ्क fd);

पूर्णाङ्क param_save_default(तार्किक blocking)
{
	PX4_DEBUG("param_save_default");

	// take the file lock
	सचे (blocking) {
		pthread_mutex_lock(&file_mutex);

	} अञ्ञथा {
		सचे (pthread_mutex_trylock(&file_mutex) != 0) {
			PX4_DEBUG("param_save_default: file lock failed (already locked)");
			पटिनिवत्त -EWOULDBLOCK;
		}
	}

	पूर्णाङ्क shutdown_lock_ret = px4_shutdown_lock();

	सचे (shutdown_lock_ret != 0) {
		PX4_ERR("px4_shutdown_lock() failed (%i)", shutdown_lock_ret);
	}

	पूर्णाङ्क res = PX4_ERROR;
	ध्रुव वर्ण *filename = param_get_default_file();

	अत्थाय (param_t param = 0; handle_in_range(param); param++) {
		सचे (params_unsaved[param]) {
			params_saving.set(param, सच्च);
			params_unsaved.set(param, मिच्छा);
		}
	}

	सचे (filename) {
		स्थिर constexpr पूर्णाङ्क MAX_ATTEMPTS = 3;

		अत्थाय (पूर्णाङ्क attempt = 1; attempt <= MAX_ATTEMPTS; attempt++) {
			// write parameters to file
			पूर्णाङ्क fd = ::open(filename, O_WRONLY | O_CREAT | O_TRUNC, PX4_O_MODE_666);

			सचे (fd > -1) {
				perf_begin(param_export_perf);
				res = param_export_internal(fd, nullptr);
				perf_end(param_export_perf);
				::close(fd);

				सचे (res == PX4_OK) {
					// reopen file to verify
					पूर्णाङ्क fd_verify = ::open(filename, O_RDONLY, PX4_O_MODE_666);
					res = param_verify(fd_verify) || lseek(fd_verify, 0, SEEK_SET) || param_verify(fd_verify);
					::close(fd_verify);
				}
			}

			सचे (res == PX4_OK) {
				भङ्ग;

			} अञ्ञथा {
				PX4_ERR("parameter export to %s failed (%d) attempt %d", filename, res, attempt);
				px4_usleep(10000); // wait at least 10 milliseconds before trying again
			}
		}

	} अञ्ञथा {
		perf_begin(param_export_perf);
		res = flash_param_save(nullptr);
		perf_end(param_export_perf);
	}

	सचे (res != PX4_OK) {
		PX4_ERR("param export failed (%d)", res);

		अत्थाय (param_t param = 0; handle_in_range(param); param++) {
			सचे (params_saving[param]) {
				params_unsaved.set(param, सच्च);
			}
		}
	}

	params_saving.reset();

	सचे (res == PX4_OK) {
		// backup file
		सचे (param_backup_file) {
			पूर्णाङ्क fd_backup_file = ::open(param_backup_file, O_WRONLY | O_CREAT | O_TRUNC, PX4_O_MODE_666);

			सचे (fd_backup_file > -1) {
				पूर्णाङ्क backup_export_ret = param_export_internal(fd_backup_file, nullptr);
				::close(fd_backup_file);

				सचे (backup_export_ret != 0) {
					PX4_ERR("backup parameter export to %s failed (%d)", param_backup_file, backup_export_ret);

				} अञ्ञथा {
					// verify export
					पूर्णाङ्क fd_verify = ::open(param_backup_file, O_RDONLY, PX4_O_MODE_666);
					param_verify(fd_verify);
					::close(fd_verify);
				}
			}
		}
	}

	pthread_mutex_unlock(&file_mutex);

	सचे (shutdown_lock_ret == 0) {
		px4_shutdown_unlock();
	}

	पटिनिवत्त res;
}

/**
 * @return 0 on success, 1 if all params have not yet been stored, -1 if device open failed, -2 if writing parameters failed
 */
पूर्णाङ्क
param_load_default()
{
	पूर्णाङ्क res = 0;
	ध्रुव वर्ण *filename = param_get_default_file();

	सचे (!filename) {
		पटिनिवत्त param_load(-1);
	}

	पूर्णाङ्क fd_load = ::open(filename, O_RDONLY);

	सचे (fd_load < 0) {
		/* no parameter file is OK, otherwise this is an error */
		सचे (errno != ENOENT) {
			PX4_ERR("open '%s' for reading failed", filename);
			पटिनिवत्त -1;
		}

		पटिनिवत्त 1;
	}

	पूर्णाङ्क result = param_load(fd_load);
	::close(fd_load);

	सचे (result == 1) {
		/* blank file: nothing stored yet */
		पटिनिवत्त 1;
	}

	सचे (result != 0) {
		PX4_ERR("error reading parameters from '%s'", filename);
		पटिनिवत्त -2;
	}

	पटिनिवत्त res;
}

स्थिर पूर्णाङ्क param_verify_callback(bson_decoder_t decoder, bson_ग्रन्थिः_t ग्रन्थिः_2)
{
	सचे (ग्रन्थिः_2->type == BSON_EOO) {
		पटिनिवत्त 0;
	}

	// find the parameter this node represents
	param_t param = param_find_no_notification(ग्रन्थिः_2->name);

	सचे (param == PARAM_INVALID) {
		PX4_ERR("verify: invalid parameter '%s'", ग्रन्थिः_2->name);
		पटिनिवत्त -1;
	}

	// handle verifying the parameter from the node
	विकल्प (ग्रन्थिः_2->type) {
	पक्ष BSON_INT32: {
			सचे (param_type(param) != PARAM_TYPE_INT32) {
				PX4_ERR("verify: invalid param type %d for '%s' (BSON_INT32)", param_type(param), ग्रन्थिः_2->name);
				पटिनिवत्त -1;
			}

			int32_t value;

			सचे (param_get(param, &value) == 0) {
				सचे (value == ग्रन्थिः_2->i32) {
					पटिनिवत्त 1; // valid

				} अञ्ञथा {
					PX4_ERR("verify: '%s' invalid BSON value %" PRIi32 "(expected %" PRIi32 ")", ग्रन्थिः_2->name, ग्रन्थिः_2->i32, value);
				}
			}
		}
		भङ्ग;

	पक्ष BSON_DOUBLE: {
			सचे (param_type(param) != PARAM_TYPE_FLOAT) {
				PX4_ERR("verify: invalid param type %d for '%s' (BSON_DOUBLE)", param_type(param), ग्रन्थिः_2->name);
				पटिनिवत्त -1;
			}

			भिन्न value;

			सचे (param_get(param, &value) == 0) {
				सचे (fabsf(value - (भिन्न)ग्रन्थिः_2->d) <= FLT_EPSILON) {
					पटिनिवत्त 1; // valid

				} अञ्ञथा {
					PX4_ERR("verify: '%s' invalid BSON value %.3f (expected %.3f)", ग्रन्थिः_2->name, ग्रन्थिः_2->d, (द्विगुण)value);
				}
			}
		}
		भङ्ग;

	स्वभाव:
		PX4_ERR("verify: '%s' invalid node type %d", ग्रन्थिः_2->name, ग्रन्थिः_2->type);
	}

	पटिनिवत्त -1;
}

स्थिर पूर्णाङ्क param_verify(पूर्णाङ्क fd)
{
	PX4_DEBUG("param_verify");

	सचे (fd < 0) {
		पटिनिवत्त -1;
	}

	सचे (lseek(fd, 0, SEEK_SET) != 0) {
		PX4_ERR("verify: seek failed");
		पटिनिवत्त -1;
	}

	bson_decoder_s decoder{};

	सचे (bson_decoder_init_file(&decoder, fd, param_verify_callback) == 0) {
		पूर्णाङ्क result = -1;

		करोहि {
			result = bson_decoder_next(&decoder);

		} याव (result > 0);

		सचे (result == 0) {
			सचे (decoder.total_document_size != decoder.total_decoded_size) {
				PX4_ERR("BSON document size (%" PRId32 ") doesn't match bytes decoded (%" PRId32 ")", decoder.total_document_size,
					decoder.total_decoded_size);

			} अञ्ञथा {
				पटिनिवत्त 0;
			}

		} अञ्ञथा सचे (result == -ENODATA) {
			PX4_ERR("verify: no BSON data");

		} अञ्ञथा {
			PX4_ERR("verify: failed (%d)", result);
		}
	}

	पटिनिवत्त -1;
}

पूर्णाङ्क
param_export(ध्रुव वर्ण *filename, param_filter_func filter)
{
	PX4_DEBUG("param_export");

	पूर्णाङ्क shutdown_lock_ret = px4_shutdown_lock();

	सचे (shutdown_lock_ret != 0) {
		PX4_ERR("px4_shutdown_lock() failed (%i)", shutdown_lock_ret);
	}

	// take the file lock
	सचे (pthread_mutex_trylock(&file_mutex) != 0) {
		PX4_ERR("param_export: file lock failed (already locked)");
		पटिनिवत्त PX4_ERROR;
	}

	पूर्णाङ्क fd = ::open(filename, O_RDWR | O_CREAT, PX4_O_MODE_666);
	पूर्णाङ्क result = PX4_ERROR;

	perf_begin(param_export_perf);

	सचे (fd > -1) {
		result = param_export_internal(fd, filter);

	} अञ्ञथा {
		result = flash_param_save(filter);
	}

	perf_end(param_export_perf);

	pthread_mutex_unlock(&file_mutex);

	सचे (shutdown_lock_ret == 0) {
		px4_shutdown_unlock();
	}

	पटिनिवत्त result;
}

// internal parameter export, caller is responsible for locking
स्थिर पूर्णाङ्क param_export_internal(पूर्णाङ्क fd, param_filter_func filter)
{
	PX4_DEBUG("param_export_internal");
	ध्रुव auto changed_params = user_config.containedAsBitset();

	पूर्णाङ्क result = -1;
	bson_encoder_s encoder{};
	uint8_t bson_buffer[256];

	सचे (lseek(fd, 0, SEEK_SET) != 0) {
		PX4_ERR("export seek failed %d", errno);
		पटिनिवत्त -1;
	}

	सचे (bson_encoder_init_buf_file(&encoder, fd, &bson_buffer, परिमाण(bson_buffer)) != 0) {
		गच्छ out;
	}

	अत्थाय (param_t param = 0; handle_in_range(param); param++) {
		सचे (!changed_params[param] || (filter && !filter(param))) {
			अनुवर्त;
		}

		ध्रुव param_मानम्_u runtime_default_मानम् = runtime_defaults.get(param);
		ध्रुव param_मानम्_u user_config_मानम् = user_config.get(param);

		// don't export default values
		विकल्प (param_type(param)) {
		पक्ष PARAM_TYPE_INT32:
			सचे (user_config_मानम्.i == runtime_default_मानम्.i) {
				PX4_DEBUG("skipping %s %" PRIi32 " export", param_name(param), runtime_default_मानम्.i);
				अनुवर्त;
			}

			भङ्ग;

		पक्ष PARAM_TYPE_FLOAT:
			सचे (fabsf(user_config_मानम्.f - runtime_default_मानम्.f) <= FLT_EPSILON) {
				PX4_DEBUG("skipping %s %.3f export", param_name(param), (द्विगुण)runtime_default_मानम्.f);
				अनुवर्त;
			}

			भङ्ग;
		}

		ध्रुव वर्ण *name = param_name(param);
		ध्रुव size_t size = param_size(param);

		/* append the appropriate BSON type object */
		विकल्प (param_type(param)) {
		पक्ष PARAM_TYPE_INT32: {
				ध्रुव int32_t i = user_config_मानम्.i;
				PX4_DEBUG("exporting: %s (%d) size: %lu val: %" PRIi32, name, param, (long unsigned पूर्णाङ्क)size, i);

				सचे (bson_encoder_append_int32(&encoder, name, i) != 0) {
					PX4_ERR("BSON append failed for '%s'", name);
					गच्छ out;
				}
			}
			भङ्ग;

		पक्ष PARAM_TYPE_FLOAT: {
				ध्रुव द्विगुण f = (द्विगुण)user_config_मानम्.f;
				PX4_DEBUG("exporting: %s (%d) size: %lu val: %.3f", name, param, (long unsigned पूर्णाङ्क)size, (द्विगुण)f);

				सचे (bson_encoder_append_double(&encoder, name, f) != 0) {
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

	सचे (result == 0) {
		सचे (bson_encoder_fini(&encoder) != PX4_OK) {
			PX4_ERR("BSON encoder finalize failed");
			result = -1;
		}
	}

	पटिनिवत्त result;
}

#सचे defined(FLASH_BASED_PARAMS)
// A file is never the flash backend's store, so what an import changes is unsaved until a delta carries it.
// As on the file backend, the import itself does not save.
स्थिर constexpr तार्किक file_import_is_saved = मिच्छा;
#अञ्ञथा
स्थिर constexpr तार्किक file_import_is_saved = सच्च;
#endif

स्थिर पूर्णाङ्क
param_import_callback(bson_decoder_t decoder, bson_ग्रन्थिः_t ग्रन्थिः_2)
{
	/*
	 * EOO means the end of the parameter object. (Currently not supporting
	 * nested BSON objects).
	 */
	सचे (ग्रन्थिः_2->type == BSON_EOO) {
		PX4_DEBUG("end of parameters");
		पटिनिवत्त 0;
	}

	// A tombstone has no value. The decoder leaves the previous node's in the union and the translations
	// read it without a type check.
	ध्रुव तार्किक tombstone = ग्रन्थिः_2->type == BSON_nullptr || ग्रन्थिः_2->type == BSON_UNDEFINED;

	// if we do param_set() directly in the translation, set PARAM_SKIP_IMPORT as return value and return here
	सचे (!tombstone && param_modify_on_import(ग्रन्थिः_2) == param_modify_on_import_ret::PARAM_SKIP_IMPORT) {
		पटिनिवत्त 1;
	}

	// Find the parameter this node represents.  If we don't know it, ignore the node.
	param_t param = param_find_no_notification(ग्रन्थिः_2->name);

	सचे (param == PARAM_INVALID) {
		PX4_WARN("ignoring unrecognised parameter '%s'", ग्रन्थिः_2->name);
		पटिनिवत्त 1;
	}

	// Handle setting the parameter from the node
	विकल्प (ग्रन्थिः_2->type) {
	पक्ष BSON_nullptr:
	पक्ष BSON_UNDEFINED:
		param_reset_internal(param, सच्च, मिच्छा);
		पटिनिवत्त 1;

	पक्ष BSON_INT32: {
			सचे (param_type(param) == PARAM_TYPE_INT32) {
				int32_t i = ग्रन्थिः_2->i32;
				param_set_internal(param, &i, file_import_is_saved, सच्च, सच्च, मिच्छा);
				PX4_DEBUG("Imported %s with value %" PRIi32, param_name(param), i);

			} अञ्ञथा {
				PX4_WARN("unexpected type for %s", ग्रन्थिः_2->name);
			}
		}
		भङ्ग;

	पक्ष BSON_DOUBLE: {
			सचे (param_type(param) == PARAM_TYPE_FLOAT) {
				भिन्न f = ग्रन्थिः_2->d;
				param_set_internal(param, &f, file_import_is_saved, सच्च, सच्च, मिच्छा);
				PX4_DEBUG("Imported %s with value %f", param_name(param), (द्विगुण)f);

			} अञ्ञथा {
				PX4_WARN("unexpected type for %s", ग्रन्थिः_2->name);
			}
		}
		भङ्ग;

	स्वभाव:
		PX4_ERR("import: unrecognised node type for '%s'", ग्रन्थिः_2->name);
	}

	// don't return zero, that means EOF
	पटिनिवत्त 1;
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

	सचे (fstat(fd, &st) == 0 && S_ISREG(st.st_mode) && st.st_size == 0) {
		पटिनिवत्त 1;
	}

	int32_t doc_size = 0;
	ssize_t len = पठ(fd, &doc_size, परिमाण(doc_size));

	सचे (len == 0 || (len == (ssize_t)परिमाण(doc_size) && (doc_size == 0 || doc_size == -1))) {
		पटिनिवत्त 1;
	}

	सचे (lseek(fd, 0, SEEK_SET) != 0) {
		PX4_ERR("import lseek failed (%d)", errno);
		पटिनिवत्त -1;
	}

	स्थिर constexpr पूर्णाङ्क MAX_ATTEMPTS = 3;

	अत्थाय (पूर्णाङ्क attempt = 1; attempt <= MAX_ATTEMPTS; attempt++) {
		bson_decoder_s decoder{};

		सचे (bson_decoder_init_file(&decoder, fd, param_import_callback) == 0) {
			पूर्णाङ्क result = -1;

			करोहि {
				result = bson_decoder_next(&decoder);

			} याव (result > 0);

			सचे (result == 0) {
				सचे (decoder.total_document_size == decoder.total_decoded_size) {
					PX4_INFO("BSON document size %" PRId32 " bytes, decoded %" PRId32 " bytes (INT32:%" PRIu16 ", FLOAT:%" PRIu16 ")",
						 decoder.total_document_size, decoder.total_decoded_size,
						 decoder.गणनम्_ग्रन्थिः_int32, decoder.गणनम्_ग्रन्थिः_double);

					पटिनिवत्त 0;

				} अञ्ञथा {
					PX4_ERR("BSON document size (%" PRId32 ") doesn't match bytes decoded (%" PRId32 ")",
						decoder.total_document_size, decoder.total_decoded_size);
				}

			} अञ्ञथा सचे (result == -ENODATA) {
				// silently retry as a precaution unless this is our last attempt
				सचे (attempt == MAX_ATTEMPTS) {
					PX4_DEBUG("BSON: no data");
					पटिनिवत्त 0;
				}

			} अञ्ञथा {
				PX4_ERR("param import failed (%d) attempt %d", result, attempt);
			}

		} अञ्ञथा {
			PX4_ERR("param import bson decoder init failed (attempt %d)", attempt);
		}

		सचे (attempt != MAX_ATTEMPTS) {
			सचे (lseek(fd, 0, SEEK_SET) != 0) {
				PX4_ERR("import lseek failed (%d)", errno);
			}

			px4_usleep(10000); // wait at least 10 milliseconds before trying again
		}
	}

	पटिनिवत्त -1;
}

#सचे defined(FLASH_BASED_PARAMS)
स्थिर पूर्णाङ्क flash_backend_locked(पूर्णाङ्क (*op)())
{
	/* Same order as param_save_default: mutex then shutdown lock. Autosave
	 * uses a trylock, so a compact holds it off the flash programming. */
	pthread_mutex_lock(&file_mutex);

	पूर्णाङ्क shutdown_lock_ret = px4_shutdown_lock();

	सचे (shutdown_lock_ret != 0) {
		PX4_ERR("px4_shutdown_lock() failed (%i)", shutdown_lock_ret);
	}

	पूर्णाङ्क result = op();

	pthread_mutex_unlock(&file_mutex);

	सचे (shutdown_lock_ret == 0) {
		px4_shutdown_unlock();
	}

	पटिनिवत्त result;
}

स्थिर पूर्णाङ्क flash_load_locked()
{
	/* Reset under the lock so a pending autosave cannot append tombstones
	 * of the just-cleared RAM before import/compact run. */
	param_reset_all_internal(मिच्छा);
	पूर्णाङ्क result = flash_param_import();

	सचे (result == 0 || result == 1) {
		params_unsaved.reset();
	}

	पटिनिवत्त result;
}
#endif

पूर्णाङ्क
param_import(पूर्णाङ्क fd)
{
	सचे (fd < 0) {
#सचे defined(FLASH_BASED_PARAMS)
		पटिनिवत्त flash_backend_locked(flash_param_import);
#अञ्ञथा
		पटिनिवत्त flash_param_import();
#endif
	}

	पटिनिवत्त param_import_internal(fd);
}

पूर्णाङ्क
param_load(पूर्णाङ्क fd)
{
	सचे (fd < 0) {
#सचे defined(FLASH_BASED_PARAMS)
		पटिनिवत्त flash_backend_locked(flash_load_locked);
#अञ्ञथा
		पटिनिवत्त PX4_ERROR;
#endif
	}

	param_reset_all_internal(मिच्छा);
	पूर्णाङ्क result = param_import_internal(fd);

#सचे !defined(FLASH_BASED_PARAMS)

	सचे (result >= 0) {
		params_unsaved.reset();
	}

#endif

	पटिनिवत्त result;
}

शून्य
param_foreach(शून्य (*func)(शून्य *arg, param_t param), शून्य *arg, तार्किक only_changed, तार्किक only_used)
{
	param_t	param;

	अत्थाय (param = 0; handle_in_range(param); param++) {

		/* if requested, skip unchanged values */
		सचे (only_changed && (!user_config.contains(param))) {
			अनुवर्त;
		}

		सचे (only_used && !param_used(param)) {
			अनुवर्त;
		}

		func(arg, param);
	}
}

uint32_t param_hash_check()
{
	uint32_t param_hash = 0;

	/* compute the CRC32 over all string param names and 4 byte values */
	अत्थाय (param_t param = 0; handle_in_range(param); param++) {
		सचे (!param_used(param) || param_is_volatile(param)) {
			अनुवर्त;
		}

		ध्रुव वर्ण *name = param_name(param);
		auto value = user_config.get(param).i;
		ध्रुव शून्य *val = (शून्य *)&value;
		param_hash = crc32part((ध्रुव uint8_t *)name, strlen(name), param_hash);
		param_hash = crc32part((ध्रुव uint8_t *)val, param_size(param), param_hash);
	}

	पटिनिवत्त param_hash;
}

शून्य param_print_status()
{
	PX4_INFO("summary: %d/%d (used/total)", param_गणनम्_used(), param_गणनम्());

#ifndef FLASH_BASED_PARAMS
	ध्रुव वर्ण *filename = param_get_default_file();

	सचे (filename != nullptr) {
		PX4_INFO("file: %s", param_get_default_file());
	}

	सचे (param_backup_file) {
		PX4_INFO("backup file: %s", param_backup_file);
	}

#endif /* FLASH_BASED_PARAMS */

	PX4_INFO("storage array: %d/%d elements (%zu bytes total)",
		 user_config.size(), firmware_defaults.size(), (size_t)user_config.byteSize());


	PX4_INFO("storage array (custom defaults): %d/%d elements (%zu bytes total)",
		 runtime_defaults.size(), firmware_defaults.size(), (size_t)runtime_defaults.byteSize());

	सचे (autosave_instance) {
		PX4_INFO("auto save: %s", autosave_instance->enabled() ? "on" : "off");

		hrt_abstime last_autosave = autosave_instance->lastAutosave();

		सचे (last_autosave > 0) {
			PX4_INFO("last auto save: %.3f seconds ago", hrt_elapsed_time(&last_autosave) * 1e-6);
		}
	}

	perf_print_counter(param_export_perf);
	perf_print_counter(param_find_perf);
	perf_print_counter(param_get_perf);
	perf_print_counter(param_set_perf);

#सचे defined(CONFIG_PARAM_PRIMARY)
	संरचना param_primary_counters counts;
	param_primary_get_counters(&counts);
	PX4_INFO("set value requests received: %" PRIu32 ", set value responses sent: %" PRIu32,
		 counts.set_मानम्_request_received, counts.set_मानम्_response_sent);
	PX4_INFO("set value requests sent: %" PRIu32 ", set value responses received: %" PRIu32,
		 counts.set_मानम्_request_sent, counts.set_मानम्_response_received);
	PX4_INFO("resets sent: %" PRIu32 ", set used requests received: %" PRIu32,
		 counts.reset_sent, counts.set_used_received);
#endif

#सचे defined(CONFIG_PARAM_REMOTE)
	संरचना param_remote_counters counts;
	param_remote_get_counters(&counts);
	PX4_INFO("set value requests received: %" PRIu32 ", set value responses sent: %" PRIu32,
		 counts.set_मानम्_request_received, counts.set_मानम्_response_sent);
	PX4_INFO("set value requests sent: %" PRIu32 ", set value responses received: %" PRIu32,
		 counts.set_मानम्_request_sent, counts.set_मानम्_response_received);
	PX4_INFO("resets received: %" PRIu32 ", set used requests sent: %" PRIu32,
		 counts.reset_received, counts.set_used_sent);
#endif

}
