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

#परिभाषय PARAM_IMPLEMENTATION
#अन्तर्भाव "param.h"
#अन्तर्भाव "param_translation.h"
#अन्तर्भाव <parameters/px4_parameters.hpp>
#अन्तर्भाव <lib/tinybson/tinybson.h>

#यदि defined(__PX4_NUTTX)
#अन्तर्भाव <nuttx/crc32.h>
#अन्यथा
#अन्तर्भाव <crc32.h>
#यदि_अन्तः
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

उपयुज्य नामाकाश time_literals;

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
#यदि_अन्तः

#यदि defined(FLASH_BASED_PARAMS)
#अन्तर्भाव "flashparams/flashparams.h"
#अन्यथा
अन्तःस्थ स्थिर पूर्णाङ्क flash_param_save(param_filter_func filter) { प्रत्यागम -1; }
अन्तःस्थ स्थिर पूर्णाङ्क flash_param_import() { प्रत्यागम -1; }
#यदि_अन्तः

स्थिर वर्ण *param_default_file = शून्यसूचकः;
स्थिर वर्ण *param_backup_file = शून्यसूचकः;

#अन्तर्भाव "autosave.h"
स्थिर ParamAutosave *autosave_instance {शून्यसूचकः};

स्थिर px4::AtomicBitset<param_info_गणनम्> params_सक्रिय;  // params found
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
#यदि not defined(CONFIG_PARAM_REMOTE)
स्थिर orb_advert_t param_topic = शून्यसूचकः;
स्थिर अचिह्नित पूर्णाङ्क param_instance = 0;
#यदि_अन्तः

स्थिर perf_गणक_t param_export_perf;
स्थिर perf_गणक_t param_find_perf;
स्थिर perf_गणक_t param_get_perf;
स्थिर perf_गणक_t param_समुच्चयः_perf;

स्थिर pthread_mutex_t file_mutex  =
	PTHREAD_MUTEX_INITIALIZER; ///< this protects against concurrent param saves (file or flash access).

// Support for remote parameter node
#यदि defined(CONFIG_PARAM_PRIMARY)
# include "parameters_primary.h"
#यदि_अन्तः // CONFIG_PARAM_PRIMARY
#यदि defined(CONFIG_PARAM_REMOTE)
# include "parameters_remote.h"
#यदि_अन्तः // CONFIG_PARAM_REMOTE

शून्य
param_init()
{
#यदि_परिभाषितम् __PX4_POSIX
	// Set up the recursive param mutex before any thread can access parameters.
	AtomicTransaction::initialize();
#यदि_अन्तः

	param_export_perf = perf_alloc(PC_ELAPSED, "param: export");
	param_find_perf = perf_alloc(PC_गणनम्, "param: find");
	param_get_perf = perf_alloc(PC_गणनम्, "param: get");
	param_समुच्चयः_perf = perf_alloc(PC_ELAPSED, "param: set");

#यदि defined(__PX4_NUTTX) && !defined(CONFIG_BUILD_FLAT)
	px4_पञ्जिकाकोशः_boardct_ioctl(_PARAMIOCBASE, param_ioctl);
#यदि_अन्तः

#यदि defined(CONFIG_PARAM_PRIMARY)
	param_primary_init();
#यदि_अन्तः // CONFIG_PARAM_PRIMARY

#यदि defined(CONFIG_PARAM_REMOTE)
	param_remote_init();
#यदि_अन्तः // CONFIG_PARAM_REMOTE

#यदि not defined(CONFIG_PARAM_REMOTE)
	autosave_instance = नूतन ParamAutosave();
#यदि_अन्तः
}


शून्य
param_notify_changes()
{
// Don't send if this is a remote node. Only the primary
// sends out update notices
#यदि not defined(CONFIG_PARAM_REMOTE)
	parameter_update_s pup {};
	pup.instance = param_instance++;
	pup.get_गणनम्_2 = perf_event_गणनम्(param_get_perf);
	pup.समुच्चयः_गणनम् = perf_event_गणनम्(param_समुच्चयः_perf);
	pup.find_गणनम् = perf_event_गणनम्(param_find_perf);
	pup.export_गणनम् = perf_event_गणनम्(param_export_perf);
	pup.सक्रिय_2 = params_सक्रिय.गणनम्();
	pup.परिवर्तितम् = user_config.आकार();
	pup.custom_default = runtime_defaults.आकार();
	pup.timestamp = hrt_absolute_time();

	यदि (param_topic == शून्यसूचकः) {
		param_topic = orb_advertise(ORB_ID(parameter_update), &pup);

	} अन्यथा {
		orb_publish(ORB_ID(parameter_update), param_topic, &pup);
	}

#यदि_अन्तः
}

स्थिर param_t param_find_आन्तरिकम्(ध्रुव वर्ण *name, तार्किक notification)
{
	perf_गणनम्(param_find_perf);

	param_t मध्यः;
	param_t front = 0;
	param_t चरमः = param_info_गणनम्;

	/* perform a binary search of the known parameters */

	यावत् (front <= चरमः) {
		मध्यः = front + (चरमः - front) / 2;
		पूर्णाङ्क ret = वाक्यं_तुलय(name, param_name(मध्यः));

		यदि (ret == 0) {
			यदि (notification) {
				param_समुच्चयः_प्रयुक्तम्(मध्यः);
			}

			प्रत्यागम मध्यः;

		} अन्यथा यदि (मध्यः == front) {
			/* An end point has been hit, but there has been no match */
			भङ्ग;

		} अन्यथा यदि (ret < 0) {
			चरमः = मध्यः;

		} अन्यथा {
			front = मध्यः;
		}
	}

	/* not found */
	प्रत्यागम PARAM_INVALID;
}

param_t param_find(ध्रुव वर्ण *name)
{
	प्रत्यागम param_find_आन्तरिकम्(name, सत्य);
}

param_t param_find_no_notification(ध्रुव वर्ण *name)
{
	प्रत्यागम param_find_आन्तरिकम्(name, असत्य);
}

अचिह्नित param_गणनम्_प्रयुक्तम्()
{
	प्रत्यागम params_सक्रिय.गणनम्();
}

param_t param_for_प्रयुक्तम्_सूचकांक(अचिह्नित सूचकांक)
{
	// walk all params and count used params
	यदि (सूचकांक < param_info_गणनम्) {
		अचिह्नित प्रयुक्तम्_गणनम् = 0;

		प्रति (पूर्णाङ्क i = 0; i < params_सक्रिय.आकार(); i++) {
			यदि (params_सक्रिय[i]) {
				// we found the right used count,
				//  return the param value
				यदि (सूचकांक == प्रयुक्तम्_गणनम्) {
					प्रत्यागम static_cast<param_t>(i);
				}

				प्रयुक्तम्_गणनम्++;
			}
		}
	}

	प्रत्यागम PARAM_INVALID;
}

पूर्णाङ्क param_get_प्रयुक्तम्_सूचकांक(param_t param)
{
	/* this tests for out of bounds and does a constant time lookup */
	यदि (!param_प्रयुक्तम्(param)) {
		प्रत्यागम -1;
	}

	/* walk all params and count, now knowing that it has a valid index */
	पूर्णाङ्क प्रयुक्तम्_गणनम् = 0;

	प्रति (पूर्णाङ्क i = 0; i < params_सक्रिय.आकार(); i++) {
		यदि (params_सक्रिय[i]) {

			यदि (param == i) {
				प्रत्यागम प्रयुक्तम्_गणनम्;
			}

			प्रयुक्तम्_गणनम्++;
		}
	}

	प्रत्यागम -1;
}

तार्किक
param_मानम्_unsaved(param_t param)
{
	प्रत्यागम handle_अन्तर्_range(param) ? (params_unsaved[param] || params_saving[param]) : असत्य;
}

पूर्णाङ्क
param_get(param_t param, शून्य *val)
{
	perf_गणनम्(param_get_perf);

	यदि (!handle_अन्तर्_range(param)) {
		PX4_ERR("get: param %" PRId16 " invalid", param);
		प्रत्यागम PX4_ERROR;
	}

	यदि (!params_सक्रिय[param]) {
		PX4_DEBUG("get: param %" PRId16 " (%s) not active", param, param_name(param));
	}

	पूर्णाङ्क परिणाम = PX4_ERROR;

	यदि (val) {

		स्वचालित retrieve_मानम् = user_config.get(param);

		विकल्प (param_type(param)) {
		पक्ष PARAM_TYPE_INT32:
			स्मृतिं_प्रतिलिख(val, &retrieve_मानम्.i, परिमाण(retrieve_मानम्.i));
			प्रत्यागम PX4_OK;

		पक्ष PARAM_TYPE_FLOAT:
			स्मृतिं_प्रतिलिख(val, &retrieve_मानम्.f, परिमाण(retrieve_मानम्.f));
			प्रत्यागम PX4_OK;
		}
	}

	प्रत्यागम परिणाम;
}

पूर्णाङ्क
param_get_default_मानम्_आन्तरिकम्(param_t param, शून्य *default_val)
{
	यदि (!handle_अन्तर्_range(param)) {
		PX4_ERR("get default value: param %d invalid", param);
		प्रत्यागम PX4_ERROR;
	}

	यदि (default_val) {
		विकल्प (param_type(param)) {
		पक्ष PARAM_TYPE_INT32: {
				int32_t val = runtime_defaults.get(param).i;
				स्मृतिं_प्रतिलिख(default_val, &val, परिमाण(val));
				प्रत्यागम PX4_OK;
			}

		पक्ष PARAM_TYPE_FLOAT: {
				भिन्न val = runtime_defaults.get(param).f;
				स्मृतिं_प्रतिलिख(default_val, &val, परिमाण(val));
				प्रत्यागम PX4_OK;
			}
		}
	}

	प्रत्यागम PX4_ERROR;
}

पूर्णाङ्क
param_get_default_मानम्(param_t param, शून्य *default_val)
{
	यदि (!handle_अन्तर्_range(param)) {
		प्रत्यागम PX4_ERROR;
	}

	पूर्णाङ्क ret = param_get_default_मानम्_आन्तरिकम्(param, default_val);
	प्रत्यागम ret;
}

तार्किक param_मानम्_is_default(param_t param)
{
	यदि (!handle_अन्तर्_range(param)) {
		प्रत्यागम सत्य;
	}

	यदि (!user_config.अन्तर्भवति(param)) {
		// if user config does not contain it, consider it default.
		प्रत्यागम सत्य;

	} अन्यथा {
		// compare with default value
		ध्रुव param_मानम्_u user_config_मानम् = user_config.get(param);
		ध्रुव param_मानम्_u runtime_default_मानम् = runtime_defaults.get(param);

		विकल्प (param_type(param)) {
		पक्ष PARAM_TYPE_INT32: {
				प्रत्यागम user_config_मानम्.i == runtime_default_मानम्.i;
			}

		पक्ष PARAM_TYPE_FLOAT: {
				प्रत्यागम fabsf(user_config_मानम्.f - runtime_default_मानम्.f) < FLT_EPSILON;
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
param_समुच्चयः_आन्तरिकम्(param_t param, ध्रुव शून्य *val, तार्किक mark_saved, तार्किक notify_changes, तार्किक update_remote = सत्य,
		   तार्किक autosave = सत्य)
{
	यदि (!handle_अन्तर्_range(param)) {
		PX4_ERR("set invalid param %d", param);
		प्रत्यागम PX4_ERROR;
	}

	यदि (val == शून्यसूचकः) {
		PX4_ERR("set invalid value");
		प्रत्यागम PX4_ERROR;
	}

	यदि (param_is_readonly(param)) {
		PX4_WARN("param %s is read-only", param_name(param));
		प्रत्यागम PX4_ERROR;
	}

	पूर्णाङ्क परिणाम = -1;
	तार्किक param_परिवर्तितम् = असत्य;
	perf_begin(param_समुच्चयः_perf);

	ध्रुव param_मानम्_u user_config_मानम् = user_config.get(param);
	param_मानम्_u new_मानम्{};

	विकल्प (param_type(param)) {
	पक्ष PARAM_TYPE_INT32:
		स्मृतिं_प्रतिलिख(&new_मानम्.i, val, परिमाण(new_मानम्.i));
		param_परिवर्तितम् = user_config_मानम्.i != new_मानम्.i;
		भङ्ग;

	पक्ष PARAM_TYPE_FLOAT:
		स्मृतिं_प्रतिलिख(&new_मानम्.f, val, परिमाण(new_मानम्.f));
		param_परिवर्तितम् = fabsf(user_config_मानम्.f - new_मानम्.f) > FLT_EPSILON;
		भङ्ग;

	स्वभाव: {
			PX4_ERR("param_set invalid param type for %s", param_name(param));
			भङ्ग;
		}
	}

	यदि (user_config.store(param, new_मानम्)) {
		यदि (param_परिवर्तितम्) {
			params_unsaved.समुच्चयः(param, !mark_saved);
		}

		परिणाम = PX4_OK;

	} अन्यथा {
		PX4_ERR("param_set failed to store param %s", param_name(param));
		परिणाम = PX4_ERROR;
	}

	यदि ((परिणाम == PX4_OK) && param_परिवर्तितम् && !mark_saved && autosave) { // this is false when importing parameters
		param_autosave();
	}

	// If this is the parameter server, make sure that the remote is updated
#यदि defined(CONFIG_PARAM_PRIMARY)

	यदि (param_परिवर्तितम् && update_remote) {
		param_primary_समुच्चयः_मानम्(param, val);
	}

#यदि_अन्तः

	// If this is the parameter remote, make sure that the primary is updated
#यदि defined(CONFIG_PARAM_REMOTE)

	यदि (param_परिवर्तितम् && update_remote) {
		param_remote_समुच्चयः_मानम्(param, val);
	}

#यदि_अन्तः

	perf_अन्तः(param_समुच्चयः_perf);

	/*
	 * If we set something, now that we have unlocked, go ahead and advertise that
	 * a thing has been set.
	 */
	यदि ((परिणाम == PX4_OK) && param_परिवर्तितम् && notify_changes) {
		param_notify_changes();
	}

	प्रत्यागम परिणाम;
}

#यदि defined(FLASH_BASED_PARAMS)
पूर्णाङ्क param_समुच्चयः_external(param_t param, ध्रुव शून्य *val, तार्किक mark_saved, तार्किक notify_changes)
{
	प्रत्यागम param_समुच्चयः_आन्तरिकम्(param, val, mark_saved, notify_changes);
}

शून्य param_get_external(param_t param, शून्य *val)
{
	param_get(param, val);
}
#यदि_अन्तः

पूर्णाङ्क param_समुच्चयः(param_t param, ध्रुव शून्य *val)
{
	प्रत्यागम param_समुच्चयः_आन्तरिकम्(param, val, असत्य, सत्य);
}

पूर्णाङ्क param_समुच्चयः_no_notification(param_t param, ध्रुव शून्य *val)
{
	प्रत्यागम param_समुच्चयः_आन्तरिकम्(param, val, असत्य, असत्य);
}

पूर्णाङ्क param_समुच्चयः_no_remote_update(param_t param, ध्रुव शून्य *val, तार्किक notify)
{
	प्रत्यागम param_समुच्चयः_आन्तरिकम्(param, val, असत्य, notify, असत्य);
}

तार्किक param_प्रयुक्तम्(param_t param)
{
	यदि (handle_अन्तर्_range(param)) {
		प्रत्यागम params_सक्रिय[param];
	}

	प्रत्यागम असत्य;
}

शून्य param_समुच्चयः_प्रयुक्तम्(param_t param)
{
	यदि (handle_अन्तर्_range(param)) {
#यदि defined(CONFIG_PARAM_REMOTE)

		यदि (!param_प्रयुक्तम्(param)) {
			param_remote_समुच्चयः_प्रयुक्तम्(param);
		}

#यदि_अन्तः

		params_सक्रिय.समुच्चयः(param, सत्य);
	}
}

पूर्णाङ्क param_समुच्चयः_default_मानम्(param_t param, ध्रुव शून्य *val)
{
	यदि (!handle_अन्तर्_range(param)) {
		PX4_ERR("set default value invalid param %d", param);
		प्रत्यागम PX4_ERROR;
	}

	यदि (val == शून्यसूचकः) {
		PX4_ERR("set default value invalid value");
		प्रत्यागम PX4_ERROR;
	}

	यदि (param_is_readonly(param)) {
		PX4_WARN("param %s is read-only", param_name(param));
		प्रत्यागम PX4_ERROR;
	}

	पूर्णाङ्क परिणाम = PX4_ERROR;


	// check if param being set to default value
	तार्किक setting_गन्तव्यम्_static_default = असत्य;
	ध्रुव param_मानम्_u firmware_default_मानम् = firmware_defaults.get(param);

	विकल्प (param_type(param)) {
	पक्ष PARAM_TYPE_INT32: {
			int32_t new_मानम्;
			स्मृतिं_प्रतिलिख(&new_मानम्, val, परिमाण(new_मानम्));
			setting_गन्तव्यम्_static_default = firmware_default_मानम्.i == new_मानम्;
			भङ्ग;
		}

	पक्ष PARAM_TYPE_FLOAT: {
			भिन्न new_मानम्;
			स्मृतिं_प्रतिलिख(&new_मानम्, val, परिमाण(new_मानम्));
			setting_गन्तव्यम्_static_default = fabsf(firmware_default_मानम्.f - new_मानम्) <= FLT_EPSILON;
			भङ्ग;
		}
	}

	यदि (setting_गन्तव्यम्_static_default) {
		runtime_defaults.reset(param);

		परिणाम = PX4_OK;

	} अन्यथा {
		param_मानम्_u new_मानम्{};

		विकल्प (param_type(param)) {
		पक्ष PARAM_TYPE_INT32: {
				स्मृतिं_प्रतिलिख(&new_मानम्.i, val, परिमाण(new_मानम्.i));
				भङ्ग;
			}

		पक्ष PARAM_TYPE_FLOAT: {
				स्मृतिं_प्रतिलिख(&new_मानम्.f, val, परिमाण(new_मानम्.f));
				भङ्ग;
			}

		स्वभाव:
			भङ्ग;
		}

		यदि (runtime_defaults.store(param, new_मानम्)) {
			user_config.refresh(param);
			परिणाम = PX4_OK;

		} अन्यथा {
			परिणाम = PX4_ERROR;
		}
	}


	यदि ((परिणाम == PX4_OK) && param_प्रयुक्तम्(param)) {
		// send notification if param is already in use
		param_notify_changes();
	}

	प्रत्यागम परिणाम;
}

स्थिर पूर्णाङ्क param_reset_आन्तरिकम्(param_t param, तार्किक notify = सत्य, तार्किक autosave = सत्य)
{
#यदि defined(CONFIG_PARAM_REMOTE)
	// Remote doesn't support reset
	प्रत्यागम असत्य;
#यदि_अन्तः

	यदि (param_is_readonly(param)) {
		प्रत्यागम 0;  // silently skip — param_reset_all loops over all params
	}

	तार्किक param_प्राप्तम् = user_config.अन्तर्भवति(param);

	यदि (handle_अन्तर्_range(param)) {
		user_config.reset(param);
	}

	यदि (param_प्राप्तम्) {
		/* Persist the reset. Flash-param saves append only unsaved params,
		 * so a reset has to show up as a tombstone or replay keeps the
		 * previous override. */
		params_unsaved.समुच्चयः(param, सत्य);
	}

	यदि (autosave) {
		param_autosave();
	}

	यदि (param_प्राप्तम् && notify) {
		param_notify_changes();
	}

#यदि defined(CONFIG_PARAM_PRIMARY)
	param_primary_reset(param);
#यदि_अन्तः

	प्रत्यागम param_प्राप्तम्;
}

पूर्णाङ्क param_reset(param_t param) { प्रत्यागम param_reset_आन्तरिकम्(param, सत्य); }
पूर्णाङ्क param_reset_no_notification(param_t param) { प्रत्यागम param_reset_आन्तरिकम्(param, असत्य); }

स्थिर शून्य
param_reset_all_आन्तरिकम्(तार्किक auto_save)
{
#यदि defined(CONFIG_PARAM_REMOTE)
	// Remote doesn't support reset
	प्रत्यागम;
#यदि_अन्तः

	प्रति (param_t param = 0; handle_अन्तर्_range(param); param++) {
		param_reset_आन्तरिकम्(param, असत्य, असत्य);
	}

	यदि (auto_save) {
		param_autosave();
	}

#यदि defined(CONFIG_PARAM_PRIMARY)
	param_primary_reset_all();
#यदि_अन्तः

	param_notify_changes();
}

शून्य
param_reset_all()
{
	param_reset_all_आन्तरिकम्(सत्य);
}

शून्य
param_reset_excludes(ध्रुव वर्ण *excludes[], पूर्णाङ्क अङ्कः_excludes)
{
	प्रति (param_t param = 0; handle_अन्तर्_range(param); param++) {
		ध्रुव वर्ण *name = param_name(param);
		तार्किक exclude = असत्य;

		प्रति (पूर्णाङ्क सूचकांक = 0; सूचकांक < अङ्कः_excludes; सूचकांक ++) {
			पूर्णाङ्क len = वाक्यदैर्घ्यम्(excludes[सूचकांक]);

			यदि ((excludes[सूचकांक][len - 1] == '*'
			     && वाक्ये_तुलय(name, excludes[सूचकांक], len - 1) == 0)
			    || वाक्यं_तुलय(name, excludes[सूचकांक]) == 0) {

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
param_reset_specific(ध्रुव वर्ण *resets[], पूर्णाङ्क अङ्कः_resets)
{
	प्रति (param_t param = 0; handle_अन्तर्_range(param); param++) {
		ध्रुव वर्ण *name = param_name(param);
		तार्किक reset = असत्य;

		प्रति (पूर्णाङ्क सूचकांक = 0; सूचकांक < अङ्कः_resets; सूचकांक++) {
			पूर्णाङ्क len = वाक्यदैर्घ्यम्(resets[सूचकांक]);

			यदि ((resets[सूचकांक][len - 1] == '*'
			     && वाक्ये_तुलय(name, resets[सूचकांक], len - 1) == 0)
			    || वाक्यं_तुलय(name, resets[सूचकांक]) == 0) {

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
param_समुच्चयः_default_file(ध्रुव वर्ण *filename)
{
	यदि (filename && param_backup_file && वाक्यं_तुलय(filename, param_backup_file) == 0) {
		PX4_ERR("default file can't be the same as the backup file %s", filename);
		प्रत्यागम PX4_ERROR;
	}

#यदि_परिभाषितम् FLASH_BASED_PARAMS
	// the default for flash-based params is always the FLASH
	(शून्य)filename;
#अन्यथा

	यदि (param_default_file != शून्यसूचकः) {
		// we assume this is not in use by some other thread
		मुक्त(param_default_file);
		param_default_file = शून्यसूचकः;
	}

	यदि (filename) {
		param_default_file = strdup(filename);
	}

#यदि_अन्तः /* FLASH_BASED_PARAMS */

	प्रत्यागम 0;
}

ध्रुव वर्ण *param_get_default_file()
{
	प्रत्यागम param_default_file;
}

पूर्णाङ्क param_समुच्चयः_backup_file(ध्रुव वर्ण *filename)
{
	यदि (filename && param_default_file && वाक्यं_तुलय(filename, param_default_file) == 0) {
		PX4_ERR("backup file can't be the same as the default file %s", filename);
		प्रत्यागम PX4_ERROR;
	}

	यदि (param_backup_file != शून्यसूचकः) {
		// we assume this is not in use by some other thread
		मुक्त(param_backup_file);
		param_backup_file = शून्यसूचकः;
	}

	यदि (filename) {
		param_backup_file = strdup(filename);

	} अन्यथा {
		param_backup_file = शून्यसूचकः; // backup disabled
	}

	प्रत्यागम 0;
}

ध्रुव वर्ण *param_get_backup_file()
{
	प्रत्यागम param_backup_file;
}

स्थिर पूर्णाङ्क param_export_आन्तरिकम्(पूर्णाङ्क fd, param_filter_func filter);
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

	पूर्णाङ्क shutdown_तालकम्_ret = px4_shutdown_तालकम्();

	यदि (shutdown_तालकम्_ret != 0) {
		PX4_ERR("px4_shutdown_lock() failed (%i)", shutdown_तालकम्_ret);
	}

	पूर्णाङ्क res = PX4_ERROR;
	ध्रुव वर्ण *filename = param_get_default_file();

	प्रति (param_t param = 0; handle_अन्तर्_range(param); param++) {
		यदि (params_unsaved[param]) {
			params_saving.समुच्चयः(param, सत्य);
			params_unsaved.समुच्चयः(param, असत्य);
		}
	}

	यदि (filename) {
		स्थिर constexpr पूर्णाङ्क MAX_ATTEMPTS = 3;

		प्रति (पूर्णाङ्क attempt = 1; attempt <= MAX_ATTEMPTS; attempt++) {
			// write parameters to file
			पूर्णाङ्क fd = ::उद्घाटय(filename, O_WRONLY | O_CREAT | O_TRUNC, PX4_O_बहुलकम्_666);

			यदि (fd > -1) {
				perf_begin(param_export_perf);
				res = param_export_आन्तरिकम्(fd, शून्यसूचकः);
				perf_अन्तः(param_export_perf);
				::close(fd);

				यदि (res == PX4_OK) {
					// reopen file to verify
					पूर्णाङ्क fd_verify = ::उद्घाटय(filename, O_RDONLY, PX4_O_बहुलकम्_666);
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
		res = flash_param_save(शून्यसूचकः);
		perf_अन्तः(param_export_perf);
	}

	यदि (res != PX4_OK) {
		PX4_ERR("param export failed (%d)", res);

		प्रति (param_t param = 0; handle_अन्तर्_range(param); param++) {
			यदि (params_saving[param]) {
				params_unsaved.समुच्चयः(param, सत्य);
			}
		}
	}

	params_saving.reset();

	यदि (res == PX4_OK) {
		// backup file
		यदि (param_backup_file) {
			पूर्णाङ्क fd_backup_file = ::उद्घाटय(param_backup_file, O_WRONLY | O_CREAT | O_TRUNC, PX4_O_बहुलकम्_666);

			यदि (fd_backup_file > -1) {
				पूर्णाङ्क backup_export_ret = param_export_आन्तरिकम्(fd_backup_file, शून्यसूचकः);
				::close(fd_backup_file);

				यदि (backup_export_ret != 0) {
					PX4_ERR("backup parameter export to %s failed (%d)", param_backup_file, backup_export_ret);

				} अन्यथा {
					// verify export
					पूर्णाङ्क fd_verify = ::उद्घाटय(param_backup_file, O_RDONLY, PX4_O_बहुलकम्_666);
					param_verify(fd_verify);
					::close(fd_verify);
				}
			}
		}
	}

	pthread_mutex_unlock(&file_mutex);

	यदि (shutdown_तालकम्_ret == 0) {
		px4_shutdown_unlock();
	}

	प्रत्यागम res;
}

/**
 * @return 0 on success, 1 if all params have not yet been stored, -1 if device open failed, -2 if writing parameters failed
 */
पूर्णाङ्क
param_आरोपय_default()
{
	पूर्णाङ्क res = 0;
	ध्रुव वर्ण *filename = param_get_default_file();

	यदि (!filename) {
		प्रत्यागम param_आरोपय(-1);
	}

	पूर्णाङ्क fd_आरोपय = ::उद्घाटय(filename, O_RDONLY);

	यदि (fd_आरोपय < 0) {
		/* no parameter file is OK, otherwise this is an error */
		यदि (errno != ENOENT) {
			PX4_ERR("open '%s' for reading failed", filename);
			प्रत्यागम -1;
		}

		प्रत्यागम 1;
	}

	पूर्णाङ्क परिणाम = param_आरोपय(fd_आरोपय);
	::close(fd_आरोपय);

	यदि (परिणाम == 1) {
		/* blank file: nothing stored yet */
		प्रत्यागम 1;
	}

	यदि (परिणाम != 0) {
		PX4_ERR("error reading parameters from '%s'", filename);
		प्रत्यागम -2;
	}

	प्रत्यागम res;
}

स्थिर पूर्णाङ्क param_verify_callback(bson_decoder_t decoder, bson_ग्रन्थिः_t ग्रन्थिः_2)
{
	यदि (ग्रन्थिः_2->type == BSON_EOO) {
		प्रत्यागम 0;
	}

	// find the parameter this node represents
	param_t param = param_find_no_notification(ग्रन्थिः_2->name);

	यदि (param == PARAM_INVALID) {
		PX4_ERR("verify: invalid parameter '%s'", ग्रन्थिः_2->name);
		प्रत्यागम -1;
	}

	// handle verifying the parameter from the node
	विकल्प (ग्रन्थिः_2->type) {
	पक्ष BSON_INT32: {
			यदि (param_type(param) != PARAM_TYPE_INT32) {
				PX4_ERR("verify: invalid param type %d for '%s' (BSON_INT32)", param_type(param), ग्रन्थिः_2->name);
				प्रत्यागम -1;
			}

			int32_t value;

			यदि (param_get(param, &value) == 0) {
				यदि (value == ग्रन्थिः_2->i32) {
					प्रत्यागम 1; // valid

				} अन्यथा {
					PX4_ERR("verify: '%s' invalid BSON value %" PRIi32 "(expected %" PRIi32 ")", ग्रन्थिः_2->name, ग्रन्थिः_2->i32, value);
				}
			}
		}
		भङ्ग;

	पक्ष BSON_DOUBLE: {
			यदि (param_type(param) != PARAM_TYPE_FLOAT) {
				PX4_ERR("verify: invalid param type %d for '%s' (BSON_DOUBLE)", param_type(param), ग्रन्थिः_2->name);
				प्रत्यागम -1;
			}

			भिन्न value;

			यदि (param_get(param, &value) == 0) {
				यदि (fabsf(value - (भिन्न)ग्रन्थिः_2->d) <= FLT_EPSILON) {
					प्रत्यागम 1; // valid

				} अन्यथा {
					PX4_ERR("verify: '%s' invalid BSON value %.3f (expected %.3f)", ग्रन्थिः_2->name, ग्रन्थिः_2->d, (द्विगुण)value);
				}
			}
		}
		भङ्ग;

	स्वभाव:
		PX4_ERR("verify: '%s' invalid node type %d", ग्रन्थिः_2->name, ग्रन्थिः_2->type);
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
		पूर्णाङ्क परिणाम = -1;

		कुरु {
			परिणाम = bson_decoder_अग्र(&decoder);

		} यावत् (परिणाम > 0);

		यदि (परिणाम == 0) {
			यदि (decoder.योग_document_आकार != decoder.योग_decoded_आकार) {
				PX4_ERR("BSON document size (%" PRId32 ") doesn't match bytes decoded (%" PRId32 ")", decoder.योग_document_आकार,
					decoder.योग_decoded_आकार);

			} अन्यथा {
				प्रत्यागम 0;
			}

		} अन्यथा यदि (परिणाम == -ENODATA) {
			PX4_ERR("verify: no BSON data");

		} अन्यथा {
			PX4_ERR("verify: failed (%d)", परिणाम);
		}
	}

	प्रत्यागम -1;
}

पूर्णाङ्क
param_export(ध्रुव वर्ण *filename, param_filter_func filter)
{
	PX4_DEBUG("param_export");

	पूर्णाङ्क shutdown_तालकम्_ret = px4_shutdown_तालकम्();

	यदि (shutdown_तालकम्_ret != 0) {
		PX4_ERR("px4_shutdown_lock() failed (%i)", shutdown_तालकम्_ret);
	}

	// take the file lock
	यदि (pthread_mutex_trylock(&file_mutex) != 0) {
		PX4_ERR("param_export: file lock failed (already locked)");
		प्रत्यागम PX4_ERROR;
	}

	पूर्णाङ्क fd = ::उद्घाटय(filename, O_RDWR | O_CREAT, PX4_O_बहुलकम्_666);
	पूर्णाङ्क परिणाम = PX4_ERROR;

	perf_begin(param_export_perf);

	यदि (fd > -1) {
		परिणाम = param_export_आन्तरिकम्(fd, filter);

	} अन्यथा {
		परिणाम = flash_param_save(filter);
	}

	perf_अन्तः(param_export_perf);

	pthread_mutex_unlock(&file_mutex);

	यदि (shutdown_तालकम्_ret == 0) {
		px4_shutdown_unlock();
	}

	प्रत्यागम परिणाम;
}

// internal parameter export, caller is responsible for locking
स्थिर पूर्णाङ्क param_export_आन्तरिकम्(पूर्णाङ्क fd, param_filter_func filter)
{
	PX4_DEBUG("param_export_internal");
	ध्रुव स्वचालित परिवर्तितम्_params = user_config.containedAsBitset();

	पूर्णाङ्क परिणाम = -1;
	bson_encoder_s encoder{};
	uint8_t bson_बफर[256];

	यदि (lseek(fd, 0, SEEK_SET) != 0) {
		PX4_ERR("export seek failed %d", errno);
		प्रत्यागम -1;
	}

	यदि (bson_encoder_init_buf_file(&encoder, fd, &bson_बफर, परिमाण(bson_बफर)) != 0) {
		गच्छ बहिर्गमनम्;
	}

	प्रति (param_t param = 0; handle_अन्तर्_range(param); param++) {
		यदि (!परिवर्तितम्_params[param] || (filter && !filter(param))) {
			अनुवर्त;
		}

		ध्रुव param_मानम्_u runtime_default_मानम् = runtime_defaults.get(param);
		ध्रुव param_मानम्_u user_config_मानम् = user_config.get(param);

		// don't export default values
		विकल्प (param_type(param)) {
		पक्ष PARAM_TYPE_INT32:
			यदि (user_config_मानम्.i == runtime_default_मानम्.i) {
				PX4_DEBUG("skipping %s %" PRIi32 " export", param_name(param), runtime_default_मानम्.i);
				अनुवर्त;
			}

			भङ्ग;

		पक्ष PARAM_TYPE_FLOAT:
			यदि (fabsf(user_config_मानम्.f - runtime_default_मानम्.f) <= FLT_EPSILON) {
				PX4_DEBUG("skipping %s %.3f export", param_name(param), (द्विगुण)runtime_default_मानम्.f);
				अनुवर्त;
			}

			भङ्ग;
		}

		ध्रुव वर्ण *name = param_name(param);
		ध्रुव size_t आकार = param_आकार(param);

		/* append the appropriate BSON type object */
		विकल्प (param_type(param)) {
		पक्ष PARAM_TYPE_INT32: {
				ध्रुव int32_t i = user_config_मानम्.i;
				PX4_DEBUG("exporting: %s (%d) size: %lu val: %" PRIi32, name, param, (दीर्घ अचिह्नित पूर्णाङ्क)आकार, i);

				यदि (bson_encoder_append_int32(&encoder, name, i) != 0) {
					PX4_ERR("BSON append failed for '%s'", name);
					गच्छ बहिर्गमनम्;
				}
			}
			भङ्ग;

		पक्ष PARAM_TYPE_FLOAT: {
				ध्रुव द्विगुण f = (द्विगुण)user_config_मानम्.f;
				PX4_DEBUG("exporting: %s (%d) size: %lu val: %.3f", name, param, (दीर्घ अचिह्नित पूर्णाङ्क)आकार, (द्विगुण)f);

				यदि (bson_encoder_append_double(&encoder, name, f) != 0) {
					PX4_ERR("BSON append failed for '%s'", name);
					गच्छ बहिर्गमनम्;
				}
			}
			भङ्ग;

		स्वभाव:
			PX4_ERR("%s unrecognized parameter type %d, skipping export", name, param_type(param));
		}
	}

	परिणाम = 0;

बहिर्गमनम्:

	यदि (परिणाम == 0) {
		यदि (bson_encoder_fini(&encoder) != PX4_OK) {
			PX4_ERR("BSON encoder finalize failed");
			परिणाम = -1;
		}
	}

	प्रत्यागम परिणाम;
}

#यदि defined(FLASH_BASED_PARAMS)
// A file is never the flash backend's store, so what an import changes is unsaved until a delta carries it.
// As on the file backend, the import itself does not save.
स्थिर constexpr तार्किक file_import_is_saved = असत्य;
#अन्यथा
स्थिर constexpr तार्किक file_import_is_saved = सत्य;
#यदि_अन्तः

स्थिर पूर्णाङ्क
param_import_callback(bson_decoder_t decoder, bson_ग्रन्थिः_t ग्रन्थिः_2)
{
	/*
	 * EOO means the end of the parameter object. (Currently not supporting
	 * nested BSON objects).
	 */
	यदि (ग्रन्थिः_2->type == BSON_EOO) {
		PX4_DEBUG("end of parameters");
		प्रत्यागम 0;
	}

	// A tombstone has no value. The decoder leaves the previous node's in the union and the translations
	// read it without a type check.
	ध्रुव तार्किक tombstone = ग्रन्थिः_2->type == BSON_nullptr || ग्रन्थिः_2->type == BSON_UNDEFINED;

	// if we do param_set() directly in the translation, set PARAM_SKIP_IMPORT as return value and return here
	यदि (!tombstone && param_modify_on_import(ग्रन्थिः_2) == param_modify_on_import_ret::PARAM_SKIP_IMPORT) {
		प्रत्यागम 1;
	}

	// Find the parameter this node represents.  If we don't know it, ignore the node.
	param_t param = param_find_no_notification(ग्रन्थिः_2->name);

	यदि (param == PARAM_INVALID) {
		PX4_WARN("ignoring unrecognised parameter '%s'", ग्रन्थिः_2->name);
		प्रत्यागम 1;
	}

	// Handle setting the parameter from the node
	विकल्प (ग्रन्थिः_2->type) {
	पक्ष BSON_nullptr:
	पक्ष BSON_UNDEFINED:
		param_reset_आन्तरिकम्(param, सत्य, असत्य);
		प्रत्यागम 1;

	पक्ष BSON_INT32: {
			यदि (param_type(param) == PARAM_TYPE_INT32) {
				int32_t i = ग्रन्थिः_2->i32;
				param_समुच्चयः_आन्तरिकम्(param, &i, file_import_is_saved, सत्य, सत्य, असत्य);
				PX4_DEBUG("Imported %s with value %" PRIi32, param_name(param), i);

			} अन्यथा {
				PX4_WARN("unexpected type for %s", ग्रन्थिः_2->name);
			}
		}
		भङ्ग;

	पक्ष BSON_DOUBLE: {
			यदि (param_type(param) == PARAM_TYPE_FLOAT) {
				भिन्न f = ग्रन्थिः_2->d;
				param_समुच्चयः_आन्तरिकम्(param, &f, file_import_is_saved, सत्य, सत्य, असत्य);
				PX4_DEBUG("Imported %s with value %f", param_name(param), (द्विगुण)f);

			} अन्यथा {
				PX4_WARN("unexpected type for %s", ग्रन्थिः_2->name);
			}
		}
		भङ्ग;

	स्वभाव:
		PX4_ERR("import: unrecognised node type for '%s'", ग्रन्थिः_2->name);
	}

	// don't return zero, that means EOF
	प्रत्यागम 1;
}

स्थिर पूर्णाङ्क
param_import_आन्तरिकम्(पूर्णाङ्क fd)
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

	int32_t doc_आकार = 0;
	ssize_t len = पठ(fd, &doc_आकार, परिमाण(doc_आकार));

	यदि (len == 0 || (len == (ssize_t)परिमाण(doc_आकार) && (doc_आकार == 0 || doc_आकार == -1))) {
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
			पूर्णाङ्क परिणाम = -1;

			कुरु {
				परिणाम = bson_decoder_अग्र(&decoder);

			} यावत् (परिणाम > 0);

			यदि (परिणाम == 0) {
				यदि (decoder.योग_document_आकार == decoder.योग_decoded_आकार) {
					PX4_INFO("BSON document size %" PRId32 " bytes, decoded %" PRId32 " bytes (INT32:%" PRIu16 ", FLOAT:%" PRIu16 ")",
						 decoder.योग_document_आकार, decoder.योग_decoded_आकार,
						 decoder.गणनम्_ग्रन्थिः_int32, decoder.गणनम्_ग्रन्थिः_double);

					प्रत्यागम 0;

				} अन्यथा {
					PX4_ERR("BSON document size (%" PRId32 ") doesn't match bytes decoded (%" PRId32 ")",
						decoder.योग_document_आकार, decoder.योग_decoded_आकार);
				}

			} अन्यथा यदि (परिणाम == -ENODATA) {
				// silently retry as a precaution unless this is our last attempt
				यदि (attempt == MAX_ATTEMPTS) {
					PX4_DEBUG("BSON: no data");
					प्रत्यागम 0;
				}

			} अन्यथा {
				PX4_ERR("param import failed (%d) attempt %d", परिणाम, attempt);
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

	पूर्णाङ्क shutdown_तालकम्_ret = px4_shutdown_तालकम्();

	यदि (shutdown_तालकम्_ret != 0) {
		PX4_ERR("px4_shutdown_lock() failed (%i)", shutdown_तालकम्_ret);
	}

	पूर्णाङ्क परिणाम = op();

	pthread_mutex_unlock(&file_mutex);

	यदि (shutdown_तालकम्_ret == 0) {
		px4_shutdown_unlock();
	}

	प्रत्यागम परिणाम;
}

स्थिर पूर्णाङ्क flash_आरोपय_locked()
{
	/* Reset under the lock so a pending autosave cannot append tombstones
	 * of the just-cleared RAM before import/compact run. */
	param_reset_all_आन्तरिकम्(असत्य);
	पूर्णाङ्क परिणाम = flash_param_import();

	यदि (परिणाम == 0 || परिणाम == 1) {
		params_unsaved.reset();
	}

	प्रत्यागम परिणाम;
}
#यदि_अन्तः

पूर्णाङ्क
param_import(पूर्णाङ्क fd)
{
	यदि (fd < 0) {
#यदि defined(FLASH_BASED_PARAMS)
		प्रत्यागम flash_backend_locked(flash_param_import);
#अन्यथा
		प्रत्यागम flash_param_import();
#यदि_अन्तः
	}

	प्रत्यागम param_import_आन्तरिकम्(fd);
}

पूर्णाङ्क
param_आरोपय(पूर्णाङ्क fd)
{
	यदि (fd < 0) {
#यदि defined(FLASH_BASED_PARAMS)
		प्रत्यागम flash_backend_locked(flash_आरोपय_locked);
#अन्यथा
		प्रत्यागम PX4_ERROR;
#यदि_अन्तः
	}

	param_reset_all_आन्तरिकम्(असत्य);
	पूर्णाङ्क परिणाम = param_import_आन्तरिकम्(fd);

#यदि !defined(FLASH_BASED_PARAMS)

	यदि (परिणाम >= 0) {
		params_unsaved.reset();
	}

#यदि_अन्तः

	प्रत्यागम परिणाम;
}

शून्य
param_foreach(शून्य (*func)(शून्य *arg, param_t param), शून्य *arg, तार्किक only_परिवर्तितम्, तार्किक only_प्रयुक्तम्)
{
	param_t	param;

	प्रति (param = 0; handle_अन्तर्_range(param); param++) {

		/* if requested, skip unchanged values */
		यदि (only_परिवर्तितम् && (!user_config.अन्तर्भवति(param))) {
			अनुवर्त;
		}

		यदि (only_प्रयुक्तम् && !param_प्रयुक्तम्(param)) {
			अनुवर्त;
		}

		func(arg, param);
	}
}

uint32_t param_hash_check()
{
	uint32_t param_hash = 0;

	/* compute the CRC32 over all string param names and 4 byte values */
	प्रति (param_t param = 0; handle_अन्तर्_range(param); param++) {
		यदि (!param_प्रयुक्तम्(param) || param_is_volatile(param)) {
			अनुवर्त;
		}

		ध्रुव वर्ण *name = param_name(param);
		स्वचालित value = user_config.get(param).i;
		ध्रुव शून्य *val = (शून्य *)&value;
		param_hash = crc32part((ध्रुव uint8_t *)name, वाक्यदैर्घ्यम्(name), param_hash);
		param_hash = crc32part((ध्रुव uint8_t *)val, param_आकार(param), param_hash);
	}

	प्रत्यागम param_hash;
}

शून्य param_print_स्थितिः()
{
	PX4_INFO("summary: %d/%d (used/total)", param_गणनम्_प्रयुक्तम्(), param_गणनम्());

#यदि_अपरिभाषितम् FLASH_BASED_PARAMS
	ध्रुव वर्ण *filename = param_get_default_file();

	यदि (filename != शून्यसूचकः) {
		PX4_INFO("file: %s", param_get_default_file());
	}

	यदि (param_backup_file) {
		PX4_INFO("backup file: %s", param_backup_file);
	}

#यदि_अन्तः /* FLASH_BASED_PARAMS */

	PX4_INFO("storage array: %d/%d elements (%zu bytes total)",
		 user_config.आकार(), firmware_defaults.आकार(), (size_t)user_config.byte_आकार());


	PX4_INFO("storage array (custom defaults): %d/%d elements (%zu bytes total)",
		 runtime_defaults.आकार(), firmware_defaults.आकार(), (size_t)runtime_defaults.byte_आकार());

	यदि (autosave_instance) {
		PX4_INFO("auto save: %s", autosave_instance->enabled() ? "on" : "off");

		hrt_abstime चरमः_autosave = autosave_instance->चरमः_Autosave();

		यदि (चरमः_autosave > 0) {
			PX4_INFO("last auto save: %.3f seconds ago", hrt_elapsed_time(&चरमः_autosave) * 1e-6);
		}
	}

	perf_print_गणक(param_export_perf);
	perf_print_गणक(param_find_perf);
	perf_print_गणक(param_get_perf);
	perf_print_गणक(param_समुच्चयः_perf);

#यदि defined(CONFIG_PARAM_PRIMARY)
	संरचना param_primary_counters counts;
	param_primary_get_counters(&counts);
	PX4_INFO("set value requests received: %" PRIu32 ", set value responses sent: %" PRIu32,
		 counts.समुच्चयः_मानम्_request_received, counts.समुच्चयः_मानम्_response_प्रेषितम्);
	PX4_INFO("set value requests sent: %" PRIu32 ", set value responses received: %" PRIu32,
		 counts.समुच्चयः_मानम्_request_प्रेषितम्, counts.समुच्चयः_मानम्_response_received);
	PX4_INFO("resets sent: %" PRIu32 ", set used requests received: %" PRIu32,
		 counts.reset_प्रेषितम्, counts.समुच्चयः_प्रयुक्तम्_received);
#यदि_अन्तः

#यदि defined(CONFIG_PARAM_REMOTE)
	संरचना param_remote_counters counts;
	param_remote_get_counters(&counts);
	PX4_INFO("set value requests received: %" PRIu32 ", set value responses sent: %" PRIu32,
		 counts.समुच्चयः_मानम्_request_received, counts.समुच्चयः_मानम्_response_प्रेषितम्);
	PX4_INFO("set value requests sent: %" PRIu32 ", set value responses received: %" PRIu32,
		 counts.समुच्चयः_मानम्_request_प्रेषितम्, counts.समुच्चयः_मानम्_response_received);
	PX4_INFO("resets received: %" PRIu32 ", set used requests sent: %" PRIu32,
		 counts.reset_received, counts.समुच्चयः_प्रयुक्तम्_प्रेषितम्);
#यदि_अन्तः

}
