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

#जर defined(__PX4_NUTTX)
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

#जर defined(__PX4_NUTTX) && !defined(CONFIG_BUILD_FLAT)
#अन्तर्भाव <px4_platform/board_ctrl.h>
#अन्तर्भाव "parameters_ioctl.h"
#endif

#जर defined(FLASH_BASED_PARAMS)
#अन्तर्भाव "flashparams/flashparams.h"
#अन्यथा
ओळीत स्थिर पूर्णांक flash_param_save(param_filter_func filter) { परत -1; }
ओळीत स्थिर पूर्णांक flash_param_import() { परत -1; }
#endif

स्थिर अक्षर *param_default_file = nullptr;
स्थिर अक्षर *param_backup_file = nullptr;

#अन्तर्भाव "autosave.h"
स्थिर ParamAutosave *autosave_instance {nullptr};

स्थिर px4::AtomicBitset<param_info_मोजणी> params_active;  // params found
स्थिर px4::AtomicBitset<param_info_मोजणी> params_unsaved;

// Params the save in progress is writing. They are moved out of params_unsaved
// before their values are read, so a param_set that lands while the save runs
// marks its param again and reaches the next save. Flash-param saves write only
// unsaved params, so nothing else would ever write that change.
स्थिर px4::AtomicBitset<param_info_मोजणी> params_saving;

स्थिर ConstLayer firmware_defaults;
स्थिर DynamicSparseLayer runtime_defaults{&firmware_defaults};
DynamicSparseLayer user_config{&runtime_defaults};

/** parameter update topic handle */
#जर not defined(CONFIG_PARAM_REMOTE)
स्थिर orb_advert_t param_topic = nullptr;
स्थिर अचिन्हित पूर्णांक param_instance = 0;
#endif

स्थिर perf_counter_t param_export_perf;
स्थिर perf_counter_t param_find_perf;
स्थिर perf_counter_t param_get_perf;
स्थिर perf_counter_t param_set_perf;

स्थिर pthread_mutex_t file_mutex  =
	PTHREAD_MUTEX_INITIALIZER; ///< this protects against concurrent param saves (file or flash access).

// Support for remote parameter node
#जर defined(CONFIG_PARAM_PRIMARY)
# include "parameters_primary.h"
#endif // CONFIG_PARAM_PRIMARY
#जर defined(CONFIG_PARAM_REMOTE)
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
	param_find_perf = perf_alloc(PC_मोजणी, "param: find");
	param_get_perf = perf_alloc(PC_मोजणी, "param: get");
	param_set_perf = perf_alloc(PC_ELAPSED, "param: set");

#जर defined(__PX4_NUTTX) && !defined(CONFIG_BUILD_FLAT)
	px4_register_boardct_ioctl(_PARAMIOCBASE, param_ioctl);
#endif

#जर defined(CONFIG_PARAM_PRIMARY)
	param_primary_init();
#endif // CONFIG_PARAM_PRIMARY

#जर defined(CONFIG_PARAM_REMOTE)
	param_remote_init();
#endif // CONFIG_PARAM_REMOTE

#जर not defined(CONFIG_PARAM_REMOTE)
	autosave_instance = नवीन ParamAutosave();
#endif
}


शून्य
param_notify_changes()
{
// Don't send if this is a remote node. Only the primary
// sends out update notices
#जर not defined(CONFIG_PARAM_REMOTE)
	parameter_update_s pup {};
	pup.instance = param_instance++;
	pup.get_मोजणी_2 = perf_event_मोजणी(param_get_perf);
	pup.set_मोजणी = perf_event_मोजणी(param_set_perf);
	pup.find_मोजणी = perf_event_मोजणी(param_find_perf);
	pup.export_मोजणी = perf_event_मोजणी(param_export_perf);
	pup.active = params_active.मोजणी();
	pup.changed = user_config.size();
	pup.custom_default = runtime_defaults.size();
	pup.timestamp = hrt_absolute_time();

	जर (param_topic == nullptr) {
		param_topic = orb_advertise(ORB_ID(parameter_update), &pup);

	} अन्यथा {
		orb_publish(ORB_ID(parameter_update), param_topic, &pup);
	}

#endif
}

स्थिर param_t param_find_internal(स्थिरांक अक्षर *name, तार्किक notification)
{
	perf_मोजणी(param_find_perf);

	param_t मधला;
	param_t front = 0;
	param_t last = param_info_मोजणी;

	/* perform a binary search of the known parameters */

	पर्यंत (front <= last) {
		मधला = front + (last - front) / 2;
		पूर्णांक ret = म_भेद(name, param_name(मधला));

		जर (ret == 0) {
			जर (notification) {
				param_set_used(मधला);
			}

			परत मधला;

		} अन्यथा जर (मधला == front) {
			/* An end point has been hit, but there has been no match */
			थांबा;

		} अन्यथा जर (ret < 0) {
			last = मधला;

		} अन्यथा {
			front = मधला;
		}
	}

	/* not found */
	परत PARAM_INVALID;
}

param_t param_find(स्थिरांक अक्षर *name)
{
	परत param_find_internal(name, सत्य);
}

param_t param_find_no_notification(स्थिरांक अक्षर *name)
{
	परत param_find_internal(name, असत्य);
}

अचिन्हित param_मोजणी_used()
{
	परत params_active.मोजणी();
}

param_t param_for_used_index(अचिन्हित index)
{
	// walk all params and count used params
	जर (index < param_info_मोजणी) {
		अचिन्हित used_मोजणी = 0;

		साठी (पूर्णांक i = 0; i < params_active.size(); i++) {
			जर (params_active[i]) {
				// we found the right used count,
				//  return the param value
				जर (index == used_मोजणी) {
					परत static_cast<param_t>(i);
				}

				used_मोजणी++;
			}
		}
	}

	परत PARAM_INVALID;
}

पूर्णांक param_get_used_index(param_t param)
{
	/* this tests for out of bounds and does a constant time lookup */
	जर (!param_used(param)) {
		परत -1;
	}

	/* walk all params and count, now knowing that it has a valid index */
	पूर्णांक used_मोजणी = 0;

	साठी (पूर्णांक i = 0; i < params_active.size(); i++) {
		जर (params_active[i]) {

			जर (param == i) {
				परत used_मोजणी;
			}

			used_मोजणी++;
		}
	}

	परत -1;
}

तार्किक
param_मूल्य_unsaved(param_t param)
{
	परत handle_in_range(param) ? (params_unsaved[param] || params_saving[param]) : असत्य;
}

पूर्णांक
param_get(param_t param, शून्य *val)
{
	perf_मोजणी(param_get_perf);

	जर (!handle_in_range(param)) {
		PX4_ERR("get: param %" PRId16 " invalid", param);
		परत PX4_ERROR;
	}

	जर (!params_active[param]) {
		PX4_DEBUG("get: param %" PRId16 " (%s) not active", param, param_name(param));
	}

	पूर्णांक result = PX4_ERROR;

	जर (val) {

		स्वतः retrieve_मूल्य = user_config.get(param);

		निवड (param_type(param)) {
		स्थिती PARAM_TYPE_INT32:
			स_नकल(val, &retrieve_मूल्य.i, आकार(retrieve_मूल्य.i));
			परत PX4_OK;

		स्थिती PARAM_TYPE_FLOAT:
			स_नकल(val, &retrieve_मूल्य.f, आकार(retrieve_मूल्य.f));
			परत PX4_OK;
		}
	}

	परत result;
}

पूर्णांक
param_get_default_मूल्य_internal(param_t param, शून्य *default_val)
{
	जर (!handle_in_range(param)) {
		PX4_ERR("get default value: param %d invalid", param);
		परत PX4_ERROR;
	}

	जर (default_val) {
		निवड (param_type(param)) {
		स्थिती PARAM_TYPE_INT32: {
				int32_t val = runtime_defaults.get(param).i;
				स_नकल(default_val, &val, आकार(val));
				परत PX4_OK;
			}

		स्थिती PARAM_TYPE_FLOAT: {
				दशांश val = runtime_defaults.get(param).f;
				स_नकल(default_val, &val, आकार(val));
				परत PX4_OK;
			}
		}
	}

	परत PX4_ERROR;
}

पूर्णांक
param_get_default_मूल्य(param_t param, शून्य *default_val)
{
	जर (!handle_in_range(param)) {
		परत PX4_ERROR;
	}

	पूर्णांक ret = param_get_default_मूल्य_internal(param, default_val);
	परत ret;
}

तार्किक param_मूल्य_is_default(param_t param)
{
	जर (!handle_in_range(param)) {
		परत सत्य;
	}

	जर (!user_config.contains(param)) {
		// if user config does not contain it, consider it default.
		परत सत्य;

	} अन्यथा {
		// compare with default value
		स्थिरांक param_मूल्य_u user_config_मूल्य = user_config.get(param);
		स्थिरांक param_मूल्य_u runtime_default_मूल्य = runtime_defaults.get(param);

		निवड (param_type(param)) {
		स्थिती PARAM_TYPE_INT32: {
				परत user_config_मूल्य.i == runtime_default_मूल्य.i;
			}

		स्थिती PARAM_TYPE_FLOAT: {
				परत fabsf(user_config_मूल्य.f - runtime_default_मूल्य.f) < भग्न_अंतर;
			}
		}
	}

	परत सत्य;
}

स्थिर शून्य
param_autosave()
{
	जर (autosave_instance) {
		autosave_instance->request();
	}
}

शून्य
param_control_autosave(तार्किक enable)
{
	जर (autosave_instance) {
		autosave_instance->enable(enable);
	}
}

स्थिर पूर्णांक
param_set_internal(param_t param, स्थिरांक शून्य *val, तार्किक mark_saved, तार्किक notify_changes, तार्किक update_remote = सत्य,
		   तार्किक autosave = सत्य)
{
	जर (!handle_in_range(param)) {
		PX4_ERR("set invalid param %d", param);
		परत PX4_ERROR;
	}

	जर (val == nullptr) {
		PX4_ERR("set invalid value");
		परत PX4_ERROR;
	}

	जर (param_is_readonly(param)) {
		PX4_WARN("param %s is read-only", param_name(param));
		परत PX4_ERROR;
	}

	पूर्णांक result = -1;
	तार्किक param_changed = असत्य;
	perf_begin(param_set_perf);

	स्थिरांक param_मूल्य_u user_config_मूल्य = user_config.get(param);
	param_मूल्य_u new_मूल्य{};

	निवड (param_type(param)) {
	स्थिती PARAM_TYPE_INT32:
		स_नकल(&new_मूल्य.i, val, आकार(new_मूल्य.i));
		param_changed = user_config_मूल्य.i != new_मूल्य.i;
		थांबा;

	स्थिती PARAM_TYPE_FLOAT:
		स_नकल(&new_मूल्य.f, val, आकार(new_मूल्य.f));
		param_changed = fabsf(user_config_मूल्य.f - new_मूल्य.f) > भग्न_अंतर;
		थांबा;

	पूर्वनिर्धारित: {
			PX4_ERR("param_set invalid param type for %s", param_name(param));
			थांबा;
		}
	}

	जर (user_config.store(param, new_मूल्य)) {
		जर (param_changed) {
			params_unsaved.set(param, !mark_saved);
		}

		result = PX4_OK;

	} अन्यथा {
		PX4_ERR("param_set failed to store param %s", param_name(param));
		result = PX4_ERROR;
	}

	जर ((result == PX4_OK) && param_changed && !mark_saved && autosave) { // this is false when importing parameters
		param_autosave();
	}

	// If this is the parameter server, make sure that the remote is updated
#जर defined(CONFIG_PARAM_PRIMARY)

	जर (param_changed && update_remote) {
		param_primary_set_मूल्य(param, val);
	}

#endif

	// If this is the parameter remote, make sure that the primary is updated
#जर defined(CONFIG_PARAM_REMOTE)

	जर (param_changed && update_remote) {
		param_remote_set_मूल्य(param, val);
	}

#endif

	perf_end(param_set_perf);

	/*
	 * If we set something, now that we have unlocked, go ahead and advertise that
	 * a thing has been set.
	 */
	जर ((result == PX4_OK) && param_changed && notify_changes) {
		param_notify_changes();
	}

	परत result;
}

#जर defined(FLASH_BASED_PARAMS)
पूर्णांक param_set_external(param_t param, स्थिरांक शून्य *val, तार्किक mark_saved, तार्किक notify_changes)
{
	परत param_set_internal(param, val, mark_saved, notify_changes);
}

शून्य param_get_external(param_t param, शून्य *val)
{
	param_get(param, val);
}
#endif

पूर्णांक param_set(param_t param, स्थिरांक शून्य *val)
{
	परत param_set_internal(param, val, असत्य, सत्य);
}

पूर्णांक param_set_no_notification(param_t param, स्थिरांक शून्य *val)
{
	परत param_set_internal(param, val, असत्य, असत्य);
}

पूर्णांक param_set_no_remote_update(param_t param, स्थिरांक शून्य *val, तार्किक notify)
{
	परत param_set_internal(param, val, असत्य, notify, असत्य);
}

तार्किक param_used(param_t param)
{
	जर (handle_in_range(param)) {
		परत params_active[param];
	}

	परत असत्य;
}

शून्य param_set_used(param_t param)
{
	जर (handle_in_range(param)) {
#जर defined(CONFIG_PARAM_REMOTE)

		जर (!param_used(param)) {
			param_remote_set_used(param);
		}

#endif

		params_active.set(param, सत्य);
	}
}

पूर्णांक param_set_default_मूल्य(param_t param, स्थिरांक शून्य *val)
{
	जर (!handle_in_range(param)) {
		PX4_ERR("set default value invalid param %d", param);
		परत PX4_ERROR;
	}

	जर (val == nullptr) {
		PX4_ERR("set default value invalid value");
		परत PX4_ERROR;
	}

	जर (param_is_readonly(param)) {
		PX4_WARN("param %s is read-only", param_name(param));
		परत PX4_ERROR;
	}

	पूर्णांक result = PX4_ERROR;


	// check if param being set to default value
	तार्किक setting_to_static_default = असत्य;
	स्थिरांक param_मूल्य_u firmware_default_मूल्य = firmware_defaults.get(param);

	निवड (param_type(param)) {
	स्थिती PARAM_TYPE_INT32: {
			int32_t new_मूल्य;
			स_नकल(&new_मूल्य, val, आकार(new_मूल्य));
			setting_to_static_default = firmware_default_मूल्य.i == new_मूल्य;
			थांबा;
		}

	स्थिती PARAM_TYPE_FLOAT: {
			दशांश new_मूल्य;
			स_नकल(&new_मूल्य, val, आकार(new_मूल्य));
			setting_to_static_default = fabsf(firmware_default_मूल्य.f - new_मूल्य) <= भग्न_अंतर;
			थांबा;
		}
	}

	जर (setting_to_static_default) {
		runtime_defaults.reset(param);

		result = PX4_OK;

	} अन्यथा {
		param_मूल्य_u new_मूल्य{};

		निवड (param_type(param)) {
		स्थिती PARAM_TYPE_INT32: {
				स_नकल(&new_मूल्य.i, val, आकार(new_मूल्य.i));
				थांबा;
			}

		स्थिती PARAM_TYPE_FLOAT: {
				स_नकल(&new_मूल्य.f, val, आकार(new_मूल्य.f));
				थांबा;
			}

		पूर्वनिर्धारित:
			थांबा;
		}

		जर (runtime_defaults.store(param, new_मूल्य)) {
			user_config.refresh(param);
			result = PX4_OK;

		} अन्यथा {
			result = PX4_ERROR;
		}
	}


	जर ((result == PX4_OK) && param_used(param)) {
		// send notification if param is already in use
		param_notify_changes();
	}

	परत result;
}

स्थिर पूर्णांक param_reset_internal(param_t param, तार्किक notify = सत्य, तार्किक autosave = सत्य)
{
#जर defined(CONFIG_PARAM_REMOTE)
	// Remote doesn't support reset
	परत असत्य;
#endif

	जर (param_is_readonly(param)) {
		परत 0;  // silently skip — param_reset_all loops over all params
	}

	तार्किक param_found = user_config.contains(param);

	जर (handle_in_range(param)) {
		user_config.reset(param);
	}

	जर (param_found) {
		/* Persist the reset. Flash-param saves append only unsaved params,
		 * so a reset has to show up as a tombstone or replay keeps the
		 * previous override. */
		params_unsaved.set(param, सत्य);
	}

	जर (autosave) {
		param_autosave();
	}

	जर (param_found && notify) {
		param_notify_changes();
	}

#जर defined(CONFIG_PARAM_PRIMARY)
	param_primary_reset(param);
#endif

	परत param_found;
}

पूर्णांक param_reset(param_t param) { परत param_reset_internal(param, सत्य); }
पूर्णांक param_reset_no_notification(param_t param) { परत param_reset_internal(param, असत्य); }

स्थिर शून्य
param_reset_all_internal(तार्किक auto_save)
{
#जर defined(CONFIG_PARAM_REMOTE)
	// Remote doesn't support reset
	परत;
#endif

	साठी (param_t param = 0; handle_in_range(param); param++) {
		param_reset_internal(param, असत्य, असत्य);
	}

	जर (auto_save) {
		param_autosave();
	}

#जर defined(CONFIG_PARAM_PRIMARY)
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
param_reset_excludes(स्थिरांक अक्षर *excludes[], पूर्णांक अंक_excludes)
{
	साठी (param_t param = 0; handle_in_range(param); param++) {
		स्थिरांक अक्षर *name = param_name(param);
		तार्किक exclude = असत्य;

		साठी (पूर्णांक index = 0; index < अंक_excludes; index ++) {
			पूर्णांक len = म_माप(excludes[index]);

			जर ((excludes[index][len - 1] == '*'
			     && म_भेदन(name, excludes[index], len - 1) == 0)
			    || म_भेद(name, excludes[index]) == 0) {

				exclude = सत्य;
				थांबा;
			}
		}

		जर (!exclude) {
			param_reset(param);
		}
	}
}

शून्य
param_reset_specific(स्थिरांक अक्षर *resets[], पूर्णांक अंक_resets)
{
	साठी (param_t param = 0; handle_in_range(param); param++) {
		स्थिरांक अक्षर *name = param_name(param);
		तार्किक reset = असत्य;

		साठी (पूर्णांक index = 0; index < अंक_resets; index++) {
			पूर्णांक len = म_माप(resets[index]);

			जर ((resets[index][len - 1] == '*'
			     && म_भेदन(name, resets[index], len - 1) == 0)
			    || म_भेद(name, resets[index]) == 0) {

				reset = सत्य;
				थांबा;
			}
		}

		जर (reset) {
			param_reset(param);
		}
	}
}

पूर्णांक
param_set_default_file(स्थिरांक अक्षर *filename)
{
	जर (filename && param_backup_file && म_भेद(filename, param_backup_file) == 0) {
		PX4_ERR("default file can't be the same as the backup file %s", filename);
		परत PX4_ERROR;
	}

#ifdef FLASH_BASED_PARAMS
	// the default for flash-based params is always the FLASH
	(शून्य)filename;
#अन्यथा

	जर (param_default_file != nullptr) {
		// we assume this is not in use by some other thread
		मुक्त(param_default_file);
		param_default_file = nullptr;
	}

	जर (filename) {
		param_default_file = strdup(filename);
	}

#endif /* FLASH_BASED_PARAMS */

	परत 0;
}

स्थिरांक अक्षर *param_get_default_file()
{
	परत param_default_file;
}

पूर्णांक param_set_backup_file(स्थिरांक अक्षर *filename)
{
	जर (filename && param_default_file && म_भेद(filename, param_default_file) == 0) {
		PX4_ERR("backup file can't be the same as the default file %s", filename);
		परत PX4_ERROR;
	}

	जर (param_backup_file != nullptr) {
		// we assume this is not in use by some other thread
		मुक्त(param_backup_file);
		param_backup_file = nullptr;
	}

	जर (filename) {
		param_backup_file = strdup(filename);

	} अन्यथा {
		param_backup_file = nullptr; // backup disabled
	}

	परत 0;
}

स्थिरांक अक्षर *param_get_backup_file()
{
	परत param_backup_file;
}

स्थिर पूर्णांक param_export_internal(पूर्णांक fd, param_filter_func filter);
स्थिर पूर्णांक param_verify(पूर्णांक fd);

पूर्णांक param_save_default(तार्किक blocking)
{
	PX4_DEBUG("param_save_default");

	// take the file lock
	जर (blocking) {
		pthread_mutex_lock(&file_mutex);

	} अन्यथा {
		जर (pthread_mutex_trylock(&file_mutex) != 0) {
			PX4_DEBUG("param_save_default: file lock failed (already locked)");
			परत -EWOULDBLOCK;
		}
	}

	पूर्णांक shutdown_lock_ret = px4_shutdown_lock();

	जर (shutdown_lock_ret != 0) {
		PX4_ERR("px4_shutdown_lock() failed (%i)", shutdown_lock_ret);
	}

	पूर्णांक res = PX4_ERROR;
	स्थिरांक अक्षर *filename = param_get_default_file();

	साठी (param_t param = 0; handle_in_range(param); param++) {
		जर (params_unsaved[param]) {
			params_saving.set(param, सत्य);
			params_unsaved.set(param, असत्य);
		}
	}

	जर (filename) {
		स्थिर constexpr पूर्णांक MAX_ATTEMPTS = 3;

		साठी (पूर्णांक attempt = 1; attempt <= MAX_ATTEMPTS; attempt++) {
			// write parameters to file
			पूर्णांक fd = ::खोलो(filename, O_WRONLY | O_CREAT | O_TRUNC, PX4_O_MODE_666);

			जर (fd > -1) {
				perf_begin(param_export_perf);
				res = param_export_internal(fd, nullptr);
				perf_end(param_export_perf);
				::बंद(fd);

				जर (res == PX4_OK) {
					// reopen file to verify
					पूर्णांक fd_verify = ::खोलो(filename, O_RDONLY, PX4_O_MODE_666);
					res = param_verify(fd_verify) || lseek(fd_verify, 0, शुरू_से) || param_verify(fd_verify);
					::बंद(fd_verify);
				}
			}

			जर (res == PX4_OK) {
				थांबा;

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

	जर (res != PX4_OK) {
		PX4_ERR("param export failed (%d)", res);

		साठी (param_t param = 0; handle_in_range(param); param++) {
			जर (params_saving[param]) {
				params_unsaved.set(param, सत्य);
			}
		}
	}

	params_saving.reset();

	जर (res == PX4_OK) {
		// backup file
		जर (param_backup_file) {
			पूर्णांक fd_backup_file = ::खोलो(param_backup_file, O_WRONLY | O_CREAT | O_TRUNC, PX4_O_MODE_666);

			जर (fd_backup_file > -1) {
				पूर्णांक backup_export_ret = param_export_internal(fd_backup_file, nullptr);
				::बंद(fd_backup_file);

				जर (backup_export_ret != 0) {
					PX4_ERR("backup parameter export to %s failed (%d)", param_backup_file, backup_export_ret);

				} अन्यथा {
					// verify export
					पूर्णांक fd_verify = ::खोलो(param_backup_file, O_RDONLY, PX4_O_MODE_666);
					param_verify(fd_verify);
					::बंद(fd_verify);
				}
			}
		}
	}

	pthread_mutex_unlock(&file_mutex);

	जर (shutdown_lock_ret == 0) {
		px4_shutdown_unlock();
	}

	परत res;
}

/**
 * @return 0 on success, 1 if all params have not yet been stored, -1 if device open failed, -2 if writing parameters failed
 */
पूर्णांक
param_load_default()
{
	पूर्णांक res = 0;
	स्थिरांक अक्षर *filename = param_get_default_file();

	जर (!filename) {
		परत param_load(-1);
	}

	पूर्णांक fd_load = ::खोलो(filename, O_RDONLY);

	जर (fd_load < 0) {
		/* no parameter file is OK, otherwise this is an error */
		जर (त्रुटि_सं != ENOENT) {
			PX4_ERR("open '%s' for reading failed", filename);
			परत -1;
		}

		परत 1;
	}

	पूर्णांक result = param_load(fd_load);
	::बंद(fd_load);

	जर (result == 1) {
		/* blank file: nothing stored yet */
		परत 1;
	}

	जर (result != 0) {
		PX4_ERR("error reading parameters from '%s'", filename);
		परत -2;
	}

	परत res;
}

स्थिर पूर्णांक param_verify_callback(bson_decoder_t decoder, bson_गाठ_t गाठ_2)
{
	जर (गाठ_2->type == BSON_EOO) {
		परत 0;
	}

	// find the parameter this node represents
	param_t param = param_find_no_notification(गाठ_2->name);

	जर (param == PARAM_INVALID) {
		PX4_ERR("verify: invalid parameter '%s'", गाठ_2->name);
		परत -1;
	}

	// handle verifying the parameter from the node
	निवड (गाठ_2->type) {
	स्थिती BSON_INT32: {
			जर (param_type(param) != PARAM_TYPE_INT32) {
				PX4_ERR("verify: invalid param type %d for '%s' (BSON_INT32)", param_type(param), गाठ_2->name);
				परत -1;
			}

			int32_t value;

			जर (param_get(param, &value) == 0) {
				जर (value == गाठ_2->i32) {
					परत 1; // valid

				} अन्यथा {
					PX4_ERR("verify: '%s' invalid BSON value %" PRIi32 "(expected %" PRIi32 ")", गाठ_2->name, गाठ_2->i32, value);
				}
			}
		}
		थांबा;

	स्थिती BSON_DOUBLE: {
			जर (param_type(param) != PARAM_TYPE_FLOAT) {
				PX4_ERR("verify: invalid param type %d for '%s' (BSON_DOUBLE)", param_type(param), गाठ_2->name);
				परत -1;
			}

			दशांश value;

			जर (param_get(param, &value) == 0) {
				जर (fabsf(value - (दशांश)गाठ_2->d) <= भग्न_अंतर) {
					परत 1; // valid

				} अन्यथा {
					PX4_ERR("verify: '%s' invalid BSON value %.3f (expected %.3f)", गाठ_2->name, गाठ_2->d, (द्विगुण)value);
				}
			}
		}
		थांबा;

	पूर्वनिर्धारित:
		PX4_ERR("verify: '%s' invalid node type %d", गाठ_2->name, गाठ_2->type);
	}

	परत -1;
}

स्थिर पूर्णांक param_verify(पूर्णांक fd)
{
	PX4_DEBUG("param_verify");

	जर (fd < 0) {
		परत -1;
	}

	जर (lseek(fd, 0, शुरू_से) != 0) {
		PX4_ERR("verify: seek failed");
		परत -1;
	}

	bson_decoder_s decoder{};

	जर (bson_decoder_init_file(&decoder, fd, param_verify_callback) == 0) {
		पूर्णांक result = -1;

		करा {
			result = bson_decoder_next(&decoder);

		} पर्यंत (result > 0);

		जर (result == 0) {
			जर (decoder.total_document_size != decoder.total_decoded_size) {
				PX4_ERR("BSON document size (%" PRId32 ") doesn't match bytes decoded (%" PRId32 ")", decoder.total_document_size,
					decoder.total_decoded_size);

			} अन्यथा {
				परत 0;
			}

		} अन्यथा जर (result == -ENODATA) {
			PX4_ERR("verify: no BSON data");

		} अन्यथा {
			PX4_ERR("verify: failed (%d)", result);
		}
	}

	परत -1;
}

पूर्णांक
param_export(स्थिरांक अक्षर *filename, param_filter_func filter)
{
	PX4_DEBUG("param_export");

	पूर्णांक shutdown_lock_ret = px4_shutdown_lock();

	जर (shutdown_lock_ret != 0) {
		PX4_ERR("px4_shutdown_lock() failed (%i)", shutdown_lock_ret);
	}

	// take the file lock
	जर (pthread_mutex_trylock(&file_mutex) != 0) {
		PX4_ERR("param_export: file lock failed (already locked)");
		परत PX4_ERROR;
	}

	पूर्णांक fd = ::खोलो(filename, O_RDWR | O_CREAT, PX4_O_MODE_666);
	पूर्णांक result = PX4_ERROR;

	perf_begin(param_export_perf);

	जर (fd > -1) {
		result = param_export_internal(fd, filter);

	} अन्यथा {
		result = flash_param_save(filter);
	}

	perf_end(param_export_perf);

	pthread_mutex_unlock(&file_mutex);

	जर (shutdown_lock_ret == 0) {
		px4_shutdown_unlock();
	}

	परत result;
}

// internal parameter export, caller is responsible for locking
स्थिर पूर्णांक param_export_internal(पूर्णांक fd, param_filter_func filter)
{
	PX4_DEBUG("param_export_internal");
	स्थिरांक स्वतः changed_params = user_config.containedAsBitset();

	पूर्णांक result = -1;
	bson_encoder_s encoder{};
	uint8_t bson_buffer[256];

	जर (lseek(fd, 0, शुरू_से) != 0) {
		PX4_ERR("export seek failed %d", त्रुटि_सं);
		परत -1;
	}

	जर (bson_encoder_init_buf_file(&encoder, fd, &bson_buffer, आकार(bson_buffer)) != 0) {
		जा out;
	}

	साठी (param_t param = 0; handle_in_range(param); param++) {
		जर (!changed_params[param] || (filter && !filter(param))) {
			सुरूठेवा;
		}

		स्थिरांक param_मूल्य_u runtime_default_मूल्य = runtime_defaults.get(param);
		स्थिरांक param_मूल्य_u user_config_मूल्य = user_config.get(param);

		// don't export default values
		निवड (param_type(param)) {
		स्थिती PARAM_TYPE_INT32:
			जर (user_config_मूल्य.i == runtime_default_मूल्य.i) {
				PX4_DEBUG("skipping %s %" PRIi32 " export", param_name(param), runtime_default_मूल्य.i);
				सुरूठेवा;
			}

			थांबा;

		स्थिती PARAM_TYPE_FLOAT:
			जर (fabsf(user_config_मूल्य.f - runtime_default_मूल्य.f) <= भग्न_अंतर) {
				PX4_DEBUG("skipping %s %.3f export", param_name(param), (द्विगुण)runtime_default_मूल्य.f);
				सुरूठेवा;
			}

			थांबा;
		}

		स्थिरांक अक्षर *name = param_name(param);
		स्थिरांक माप_प्रकार size = param_size(param);

		/* append the appropriate BSON type object */
		निवड (param_type(param)) {
		स्थिती PARAM_TYPE_INT32: {
				स्थिरांक int32_t i = user_config_मूल्य.i;
				PX4_DEBUG("exporting: %s (%d) size: %lu val: %" PRIi32, name, param, (दीर्घ अचिन्हित पूर्णांक)size, i);

				जर (bson_encoder_append_int32(&encoder, name, i) != 0) {
					PX4_ERR("BSON append failed for '%s'", name);
					जा out;
				}
			}
			थांबा;

		स्थिती PARAM_TYPE_FLOAT: {
				स्थिरांक द्विगुण f = (द्विगुण)user_config_मूल्य.f;
				PX4_DEBUG("exporting: %s (%d) size: %lu val: %.3f", name, param, (दीर्घ अचिन्हित पूर्णांक)size, (द्विगुण)f);

				जर (bson_encoder_append_double(&encoder, name, f) != 0) {
					PX4_ERR("BSON append failed for '%s'", name);
					जा out;
				}
			}
			थांबा;

		पूर्वनिर्धारित:
			PX4_ERR("%s unrecognized parameter type %d, skipping export", name, param_type(param));
		}
	}

	result = 0;

out:

	जर (result == 0) {
		जर (bson_encoder_fini(&encoder) != PX4_OK) {
			PX4_ERR("BSON encoder finalize failed");
			result = -1;
		}
	}

	परत result;
}

#जर defined(FLASH_BASED_PARAMS)
// A file is never the flash backend's store, so what an import changes is unsaved until a delta carries it.
// As on the file backend, the import itself does not save.
स्थिर constexpr तार्किक file_import_is_saved = असत्य;
#अन्यथा
स्थिर constexpr तार्किक file_import_is_saved = सत्य;
#endif

स्थिर पूर्णांक
param_import_callback(bson_decoder_t decoder, bson_गाठ_t गाठ_2)
{
	/*
	 * EOO means the end of the parameter object. (Currently not supporting
	 * nested BSON objects).
	 */
	जर (गाठ_2->type == BSON_EOO) {
		PX4_DEBUG("end of parameters");
		परत 0;
	}

	// A tombstone has no value. The decoder leaves the previous node's in the union and the translations
	// read it without a type check.
	स्थिरांक तार्किक tombstone = गाठ_2->type == BSON_nullptr || गाठ_2->type == BSON_UNDEFINED;

	// if we do param_set() directly in the translation, set PARAM_SKIP_IMPORT as return value and return here
	जर (!tombstone && param_modify_on_import(गाठ_2) == param_modify_on_import_ret::PARAM_SKIP_IMPORT) {
		परत 1;
	}

	// Find the parameter this node represents.  If we don't know it, ignore the node.
	param_t param = param_find_no_notification(गाठ_2->name);

	जर (param == PARAM_INVALID) {
		PX4_WARN("ignoring unrecognised parameter '%s'", गाठ_2->name);
		परत 1;
	}

	// Handle setting the parameter from the node
	निवड (गाठ_2->type) {
	स्थिती BSON_nullptr:
	स्थिती BSON_UNDEFINED:
		param_reset_internal(param, सत्य, असत्य);
		परत 1;

	स्थिती BSON_INT32: {
			जर (param_type(param) == PARAM_TYPE_INT32) {
				int32_t i = गाठ_2->i32;
				param_set_internal(param, &i, file_import_is_saved, सत्य, सत्य, असत्य);
				PX4_DEBUG("Imported %s with value %" PRIi32, param_name(param), i);

			} अन्यथा {
				PX4_WARN("unexpected type for %s", गाठ_2->name);
			}
		}
		थांबा;

	स्थिती BSON_DOUBLE: {
			जर (param_type(param) == PARAM_TYPE_FLOAT) {
				दशांश f = गाठ_2->d;
				param_set_internal(param, &f, file_import_is_saved, सत्य, सत्य, असत्य);
				PX4_DEBUG("Imported %s with value %f", param_name(param), (द्विगुण)f);

			} अन्यथा {
				PX4_WARN("unexpected type for %s", गाठ_2->name);
			}
		}
		थांबा;

	पूर्वनिर्धारित:
		PX4_ERR("import: unrecognised node type for '%s'", गाठ_2->name);
	}

	// don't return zero, that means EOF
	परत 1;
}

स्थिर पूर्णांक
param_import_internal(पूर्णांक fd)
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

	जर (ख_स्थिति(fd, &st) == 0 && S_ISREG(st.st_mode) && st.st_size == 0) {
		परत 1;
	}

	int32_t doc_size = 0;
	ssize_t len = पठ(fd, &doc_size, आकार(doc_size));

	जर (len == 0 || (len == (ssize_t)आकार(doc_size) && (doc_size == 0 || doc_size == -1))) {
		परत 1;
	}

	जर (lseek(fd, 0, शुरू_से) != 0) {
		PX4_ERR("import lseek failed (%d)", त्रुटि_सं);
		परत -1;
	}

	स्थिर constexpr पूर्णांक MAX_ATTEMPTS = 3;

	साठी (पूर्णांक attempt = 1; attempt <= MAX_ATTEMPTS; attempt++) {
		bson_decoder_s decoder{};

		जर (bson_decoder_init_file(&decoder, fd, param_import_callback) == 0) {
			पूर्णांक result = -1;

			करा {
				result = bson_decoder_next(&decoder);

			} पर्यंत (result > 0);

			जर (result == 0) {
				जर (decoder.total_document_size == decoder.total_decoded_size) {
					PX4_INFO("BSON document size %" PRId32 " bytes, decoded %" PRId32 " bytes (INT32:%" PRIu16 ", FLOAT:%" PRIu16 ")",
						 decoder.total_document_size, decoder.total_decoded_size,
						 decoder.मोजणी_गाठ_int32, decoder.मोजणी_गाठ_double);

					परत 0;

				} अन्यथा {
					PX4_ERR("BSON document size (%" PRId32 ") doesn't match bytes decoded (%" PRId32 ")",
						decoder.total_document_size, decoder.total_decoded_size);
				}

			} अन्यथा जर (result == -ENODATA) {
				// silently retry as a precaution unless this is our last attempt
				जर (attempt == MAX_ATTEMPTS) {
					PX4_DEBUG("BSON: no data");
					परत 0;
				}

			} अन्यथा {
				PX4_ERR("param import failed (%d) attempt %d", result, attempt);
			}

		} अन्यथा {
			PX4_ERR("param import bson decoder init failed (attempt %d)", attempt);
		}

		जर (attempt != MAX_ATTEMPTS) {
			जर (lseek(fd, 0, शुरू_से) != 0) {
				PX4_ERR("import lseek failed (%d)", त्रुटि_सं);
			}

			px4_usleep(10000); // wait at least 10 milliseconds before trying again
		}
	}

	परत -1;
}

#जर defined(FLASH_BASED_PARAMS)
स्थिर पूर्णांक flash_backend_locked(पूर्णांक (*op)())
{
	/* Same order as param_save_default: mutex then shutdown lock. Autosave
	 * uses a trylock, so a compact holds it off the flash programming. */
	pthread_mutex_lock(&file_mutex);

	पूर्णांक shutdown_lock_ret = px4_shutdown_lock();

	जर (shutdown_lock_ret != 0) {
		PX4_ERR("px4_shutdown_lock() failed (%i)", shutdown_lock_ret);
	}

	पूर्णांक result = op();

	pthread_mutex_unlock(&file_mutex);

	जर (shutdown_lock_ret == 0) {
		px4_shutdown_unlock();
	}

	परत result;
}

स्थिर पूर्णांक flash_load_locked()
{
	/* Reset under the lock so a pending autosave cannot append tombstones
	 * of the just-cleared RAM before import/compact run. */
	param_reset_all_internal(असत्य);
	पूर्णांक result = flash_param_import();

	जर (result == 0 || result == 1) {
		params_unsaved.reset();
	}

	परत result;
}
#endif

पूर्णांक
param_import(पूर्णांक fd)
{
	जर (fd < 0) {
#जर defined(FLASH_BASED_PARAMS)
		परत flash_backend_locked(flash_param_import);
#अन्यथा
		परत flash_param_import();
#endif
	}

	परत param_import_internal(fd);
}

पूर्णांक
param_load(पूर्णांक fd)
{
	जर (fd < 0) {
#जर defined(FLASH_BASED_PARAMS)
		परत flash_backend_locked(flash_load_locked);
#अन्यथा
		परत PX4_ERROR;
#endif
	}

	param_reset_all_internal(असत्य);
	पूर्णांक result = param_import_internal(fd);

#जर !defined(FLASH_BASED_PARAMS)

	जर (result >= 0) {
		params_unsaved.reset();
	}

#endif

	परत result;
}

शून्य
param_foreach(शून्य (*func)(शून्य *arg, param_t param), शून्य *arg, तार्किक only_changed, तार्किक only_used)
{
	param_t	param;

	साठी (param = 0; handle_in_range(param); param++) {

		/* if requested, skip unchanged values */
		जर (only_changed && (!user_config.contains(param))) {
			सुरूठेवा;
		}

		जर (only_used && !param_used(param)) {
			सुरूठेवा;
		}

		func(arg, param);
	}
}

uint32_t param_hash_check()
{
	uint32_t param_hash = 0;

	/* compute the CRC32 over all string param names and 4 byte values */
	साठी (param_t param = 0; handle_in_range(param); param++) {
		जर (!param_used(param) || param_is_volatile(param)) {
			सुरूठेवा;
		}

		स्थिरांक अक्षर *name = param_name(param);
		स्वतः value = user_config.get(param).i;
		स्थिरांक शून्य *val = (शून्य *)&value;
		param_hash = crc32part((स्थिरांक uint8_t *)name, म_माप(name), param_hash);
		param_hash = crc32part((स्थिरांक uint8_t *)val, param_size(param), param_hash);
	}

	परत param_hash;
}

शून्य param_print_status()
{
	PX4_INFO("summary: %d/%d (used/total)", param_मोजणी_used(), param_मोजणी());

#ifndef FLASH_BASED_PARAMS
	स्थिरांक अक्षर *filename = param_get_default_file();

	जर (filename != nullptr) {
		PX4_INFO("file: %s", param_get_default_file());
	}

	जर (param_backup_file) {
		PX4_INFO("backup file: %s", param_backup_file);
	}

#endif /* FLASH_BASED_PARAMS */

	PX4_INFO("storage array: %d/%d elements (%zu bytes total)",
		 user_config.size(), firmware_defaults.size(), (माप_प्रकार)user_config.byteSize());


	PX4_INFO("storage array (custom defaults): %d/%d elements (%zu bytes total)",
		 runtime_defaults.size(), firmware_defaults.size(), (माप_प्रकार)runtime_defaults.byteSize());

	जर (autosave_instance) {
		PX4_INFO("auto save: %s", autosave_instance->enabled() ? "on" : "off");

		hrt_abstime last_autosave = autosave_instance->lastAutosave();

		जर (last_autosave > 0) {
			PX4_INFO("last auto save: %.3f seconds ago", hrt_elapsed_time(&last_autosave) * 1e-6);
		}
	}

	perf_print_counter(param_export_perf);
	perf_print_counter(param_find_perf);
	perf_print_counter(param_get_perf);
	perf_print_counter(param_set_perf);

#जर defined(CONFIG_PARAM_PRIMARY)
	संरचना param_primary_counters counts;
	param_primary_get_counters(&counts);
	PX4_INFO("set value requests received: %" PRIu32 ", set value responses sent: %" PRIu32,
		 counts.set_मूल्य_request_received, counts.set_मूल्य_response_sent);
	PX4_INFO("set value requests sent: %" PRIu32 ", set value responses received: %" PRIu32,
		 counts.set_मूल्य_request_sent, counts.set_मूल्य_response_received);
	PX4_INFO("resets sent: %" PRIu32 ", set used requests received: %" PRIu32,
		 counts.reset_sent, counts.set_used_received);
#endif

#जर defined(CONFIG_PARAM_REMOTE)
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
