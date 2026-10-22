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

#నిర్వచించు PARAM_IMPLEMENTATION
#అన్తర్భావ "param.h"
#అన్తర్భావ "param_translation.h"
#అన్తర్భావ <parameters/px4_parameters.hpp>
#అన్తర్భావ <lib/tinybson/tinybson.h>

#ఒకవేళ defined(__PX4_NUTTX)
#అన్తర్భావ <nuttx/crc32.h>
#లేకపోతే
#అన్తర్భావ <crc32.h>
#అయితే_ముగింపు
#అన్తర్భావ <float.h>
#అన్తర్భావ <math.h>
#అన్తర్భావ <sys/stat.h>

#అన్తర్భావ <containers/Bitset.hpp>
#అన్తర్భావ <drivers/drv_hrt.h>
#అన్తర్భావ <lib/perf/perf_counter.h>
#అన్తర్భావ <px4_platform_common/px4_config.h>
#అన్తర్భావ <px4_platform_common/atomic_bitset.h>
#అన్తర్భావ <px4_platform_common/defines.h>
#అన్తర్భావ <px4_platform_common/posix.h>
#అన్తర్భావ <px4_platform_common/sem.h>
#అన్తర్భావ <px4_platform_common/shutdown.h>
#అన్తర్భావ <px4_platform_common/micro_hal.h>

ఉపయోగించి నామాకాశ సమయం_literals;

#అన్తర్భావ "uORB/uORB.h"
#అన్తర్భావ "uORB/topics/parameter_update.h"
#అన్తర్భావ <uORB/Subscription.hpp>

#అన్తర్భావ "ExhaustiveLayer.h"
#అన్తర్భావ "ConstLayer.h"
#అన్తర్భావ "DynamicSparseLayer.h"
#అన్తర్భావ "StaticSparseLayer.h"

#అన్తర్భావ "atomic_transaction.h"

/* Include functions common to user and kernel sides */
#అన్తర్భావ "parameters_common.cpp"

#ఒకవేళ defined(__PX4_NUTTX) && !defined(CONFIG_BUILD_FLAT)
#అన్తర్భావ <px4_platform/board_ctrl.h>
#అన్తర్భావ "parameters_ioctl.h"
#అయితే_ముగింపు

#ఒకవేళ defined(FLASH_BASED_PARAMS)
#అన్తర్భావ "flashparams/flashparams.h"
#లేకపోతే
అంతఃవరుస స్థిర పూర్ణాంకం flash_param_save(param_filter_func filter) { తిరిగి -1; }
అంతఃవరుస స్థిర పూర్ణాంకం flash_param_import() { తిరిగి -1; }
#అయితే_ముగింపు

స్థిర అక్షరం *param_default_file = శూన్యసూచిక;
స్థిర అక్షరం *param_backup_file = శూన్యసూచిక;

#అన్తర్భావ "autosave.h"
స్థిర ParamAutosave *autosave_instance {శూన్యసూచిక};

స్థిర px4::AtomicBitset<param_info_లెక్క> params_చురుకైన;  // params found
స్థిర px4::AtomicBitset<param_info_లెక్క> params_unsaved;

// Params the save in progress is writing. They are moved out of params_unsaved
// before their values are read, so a param_set that lands while the save runs
// marks its param again and reaches the next save. Flash-param saves write only
// unsaved params, so nothing else would ever write that change.
స్థిర px4::AtomicBitset<param_info_లెక్క> params_saving;

స్థిర ConstLayer firmware_defaults;
స్థిర DynamicSparseLayer runtime_defaults{&firmware_defaults};
DynamicSparseLayer user_config{&runtime_defaults};

/** parameter update topic handle */
#ఒకవేళ not defined(CONFIG_PARAM_REMOTE)
స్థిర orb_advert_t param_topic = శూన్యసూచిక;
స్థిర గుర్తులేని పూర్ణాంకం param_instance = 0;
#అయితే_ముగింపు

స్థిర perf_counter_t param_export_perf;
స్థిర perf_counter_t param_కనుగొను_perf;
స్థిర perf_counter_t param_get_perf;
స్థిర perf_counter_t param_సమితి_perf;

స్థిర pthread_mutex_t file_mutex  =
	PTHREAD_MUTEX_INITIALIZER; ///< this protects against concurrent param saves (file or flash access).

// Support for remote parameter node
#ఒకవేళ defined(CONFIG_PARAM_PRIMARY)
# include "parameters_primary.h"
#అయితే_ముగింపు // CONFIG_PARAM_PRIMARY
#ఒకవేళ defined(CONFIG_PARAM_REMOTE)
# include "parameters_remote.h"
#అయితే_ముగింపు // CONFIG_PARAM_REMOTE

శూన్య
param_init()
{
#నిర్వచితమైతే __PX4_POSIX
	// Set up the recursive param mutex before any thread can access parameters.
	AtomicTransaction::initialize();
#అయితే_ముగింపు

	param_export_perf = perf_alloc(PC_ELAPSED, "param: export");
	param_కనుగొను_perf = perf_alloc(PC_లెక్క, "param: find");
	param_get_perf = perf_alloc(PC_లెక్క, "param: get");
	param_సమితి_perf = perf_alloc(PC_ELAPSED, "param: set");

#ఒకవేళ defined(__PX4_NUTTX) && !defined(CONFIG_BUILD_FLAT)
	px4_రిజిస్టరు_boardct_ioctl(_PARAMIOCBASE, param_ioctl);
#అయితే_ముగింపు

#ఒకవేళ defined(CONFIG_PARAM_PRIMARY)
	param_primary_init();
#అయితే_ముగింపు // CONFIG_PARAM_PRIMARY

#ఒకవేళ defined(CONFIG_PARAM_REMOTE)
	param_remote_init();
#అయితే_ముగింపు // CONFIG_PARAM_REMOTE

#ఒకవేళ not defined(CONFIG_PARAM_REMOTE)
	autosave_instance = కొత్త ParamAutosave();
#అయితే_ముగింపు
}


శూన్య
param_notify_changes()
{
// Don't send if this is a remote node. Only the primary
// sends out update notices
#ఒకవేళ not defined(CONFIG_PARAM_REMOTE)
	parameter_update_s pup {};
	pup.instance = param_instance++;
	pup.get_లెక్క_2 = perf_event_లెక్క(param_get_perf);
	pup.సమితి_లెక్క = perf_event_లెక్క(param_సమితి_perf);
	pup.కనుగొను_లెక్క = perf_event_లెక్క(param_కనుగొను_perf);
	pup.export_లెక్క = perf_event_లెక్క(param_export_perf);
	pup.చురుకైన_2 = params_చురుకైన.లెక్క();
	pup.మారింది = user_config.కొలత();
	pup.custom_default = runtime_defaults.కొలత();
	pup.timestamp = hrt_absolute_సమయం();

	ఒకవేళ (param_topic == శూన్యసూచిక) {
		param_topic = orb_advertise(ORB_ID(parameter_update), &pup);

	} లేకపోతే {
		orb_publish(ORB_ID(parameter_update), param_topic, &pup);
	}

#అయితే_ముగింపు
}

స్థిర param_t param_కనుగొను_అంతర్గత(స్థిరరాశి అక్షరం *name, తార్కిక notification)
{
	perf_లెక్క(param_కనుగొను_perf);

	param_t మధ్య;
	param_t front = 0;
	param_t చివరి = param_info_లెక్క;

	/* perform a binary search of the known parameters */

	అయితే (front <= చివరి) {
		మధ్య = front + (చివరి - front) / 2;
		పూర్ణాంకం ret = strcmp(name, param_name(మధ్య));

		ఒకవేళ (ret == 0) {
			ఒకవేళ (notification) {
				param_సమితి_వాడిన(మధ్య);
			}

			తిరిగి మధ్య;

		} లేకపోతే ఒకవేళ (మధ్య == front) {
			/* An end point has been hit, but there has been no match */
			ఆపు;

		} లేకపోతే ఒకవేళ (ret < 0) {
			చివరి = మధ్య;

		} లేకపోతే {
			front = మధ్య;
		}
	}

	/* not found */
	తిరిగి PARAM_INVALID;
}

param_t param_కనుగొను(స్థిరరాశి అక్షరం *name)
{
	తిరిగి param_కనుగొను_అంతర్గత(name, నిజం);
}

param_t param_కనుగొను_no_notification(స్థిరరాశి అక్షరం *name)
{
	తిరిగి param_కనుగొను_అంతర్గత(name, అబద్ధం);
}

గుర్తులేని param_లెక్క_వాడిన()
{
	తిరిగి params_చురుకైన.లెక్క();
}

param_t param_for_వాడిన_సూచీ(గుర్తులేని సూచీ)
{
	// walk all params and count used params
	ఒకవేళ (సూచీ < param_info_లెక్క) {
		గుర్తులేని వాడిన_లెక్క = 0;

		కొరకు (పూర్ణాంకం i = 0; i < params_చురుకైన.కొలత(); i++) {
			ఒకవేళ (params_చురుకైన[i]) {
				// we found the right used count,
				//  return the param value
				ఒకవేళ (సూచీ == వాడిన_లెక్క) {
					తిరిగి static_cast<param_t>(i);
				}

				వాడిన_లెక్క++;
			}
		}
	}

	తిరిగి PARAM_INVALID;
}

పూర్ణాంకం param_get_వాడిన_సూచీ(param_t param)
{
	/* this tests for out of bounds and does a constant time lookup */
	ఒకవేళ (!param_వాడిన(param)) {
		తిరిగి -1;
	}

	/* walk all params and count, now knowing that it has a valid index */
	పూర్ణాంకం వాడిన_లెక్క = 0;

	కొరకు (పూర్ణాంకం i = 0; i < params_చురుకైన.కొలత(); i++) {
		ఒకవేళ (params_చురుకైన[i]) {

			ఒకవేళ (param == i) {
				తిరిగి వాడిన_లెక్క;
			}

			వాడిన_లెక్క++;
		}
	}

	తిరిగి -1;
}

తార్కిక
param_విలువ_unsaved(param_t param)
{
	తిరిగి handle_లోపల_range(param) ? (params_unsaved[param] || params_saving[param]) : అబద్ధం;
}

పూర్ణాంకం
param_get(param_t param, శూన్య *val)
{
	perf_లెక్క(param_get_perf);

	ఒకవేళ (!handle_లోపల_range(param)) {
		PX4_ERR("get: param %" PRId16 " invalid", param);
		తిరిగి PX4_ERROR;
	}

	ఒకవేళ (!params_చురుకైన[param]) {
		PX4_DEBUG("get: param %" PRId16 " (%s) not active", param, param_name(param));
	}

	పూర్ణాంకం ఫలితం = PX4_ERROR;

	ఒకవేళ (val) {

		స్వయంచాలక retrieve_విలువ = user_config.get(param);

		ఎంపిక (param_type(param)) {
		స్థితి PARAM_TYPE_INT32:
			జ్ఞాపకం_నకలు(val, &retrieve_విలువ.i, పరిమాణం(retrieve_విలువ.i));
			తిరిగి PX4_OK;

		స్థితి PARAM_TYPE_FLOAT:
			జ్ఞాపకం_నకలు(val, &retrieve_విలువ.f, పరిమాణం(retrieve_విలువ.f));
			తిరిగి PX4_OK;
		}
	}

	తిరిగి ఫలితం;
}

పూర్ణాంకం
param_get_default_విలువ_అంతర్గత(param_t param, శూన్య *default_val)
{
	ఒకవేళ (!handle_లోపల_range(param)) {
		PX4_ERR("get default value: param %d invalid", param);
		తిరిగి PX4_ERROR;
	}

	ఒకవేళ (default_val) {
		ఎంపిక (param_type(param)) {
		స్థితి PARAM_TYPE_INT32: {
				int32_t val = runtime_defaults.get(param).i;
				జ్ఞాపకం_నకలు(default_val, &val, పరిమాణం(val));
				తిరిగి PX4_OK;
			}

		స్థితి PARAM_TYPE_FLOAT: {
				దశాంశం val = runtime_defaults.get(param).f;
				జ్ఞాపకం_నకలు(default_val, &val, పరిమాణం(val));
				తిరిగి PX4_OK;
			}
		}
	}

	తిరిగి PX4_ERROR;
}

పూర్ణాంకం
param_get_default_విలువ(param_t param, శూన్య *default_val)
{
	ఒకవేళ (!handle_లోపల_range(param)) {
		తిరిగి PX4_ERROR;
	}

	పూర్ణాంకం ret = param_get_default_విలువ_అంతర్గత(param, default_val);
	తిరిగి ret;
}

తార్కిక param_విలువ_is_default(param_t param)
{
	ఒకవేళ (!handle_లోపల_range(param)) {
		తిరిగి నిజం;
	}

	ఒకవేళ (!user_config.కలిగియుంది(param)) {
		// if user config does not contain it, consider it default.
		తిరిగి నిజం;

	} లేకపోతే {
		// compare with default value
		స్థిరరాశి param_విలువ_u user_config_విలువ = user_config.get(param);
		స్థిరరాశి param_విలువ_u runtime_default_విలువ = runtime_defaults.get(param);

		ఎంపిక (param_type(param)) {
		స్థితి PARAM_TYPE_INT32: {
				తిరిగి user_config_విలువ.i == runtime_default_విలువ.i;
			}

		స్థితి PARAM_TYPE_FLOAT: {
				తిరిగి fabsf(user_config_విలువ.f - runtime_default_విలువ.f) < FLT_EPSILON;
			}
		}
	}

	తిరిగి నిజం;
}

స్థిర శూన్య
param_autosave()
{
	ఒకవేళ (autosave_instance) {
		autosave_instance->request();
	}
}

శూన్య
param_control_autosave(తార్కిక enable)
{
	ఒకవేళ (autosave_instance) {
		autosave_instance->enable(enable);
	}
}

స్థిర పూర్ణాంకం
param_సమితి_అంతర్గత(param_t param, స్థిరరాశి శూన్య *val, తార్కిక mark_saved, తార్కిక notify_changes, తార్కిక update_remote = నిజం,
		   తార్కిక autosave = నిజం)
{
	ఒకవేళ (!handle_లోపల_range(param)) {
		PX4_ERR("set invalid param %d", param);
		తిరిగి PX4_ERROR;
	}

	ఒకవేళ (val == శూన్యసూచిక) {
		PX4_ERR("set invalid value");
		తిరిగి PX4_ERROR;
	}

	ఒకవేళ (param_is_readonly(param)) {
		PX4_WARN("param %s is read-only", param_name(param));
		తిరిగి PX4_ERROR;
	}

	పూర్ణాంకం ఫలితం = -1;
	తార్కిక param_మారింది = అబద్ధం;
	perf_begin(param_సమితి_perf);

	స్థిరరాశి param_విలువ_u user_config_విలువ = user_config.get(param);
	param_విలువ_u new_విలువ{};

	ఎంపిక (param_type(param)) {
	స్థితి PARAM_TYPE_INT32:
		జ్ఞాపకం_నకలు(&new_విలువ.i, val, పరిమాణం(new_విలువ.i));
		param_మారింది = user_config_విలువ.i != new_విలువ.i;
		ఆపు;

	స్థితి PARAM_TYPE_FLOAT:
		జ్ఞాపకం_నకలు(&new_విలువ.f, val, పరిమాణం(new_విలువ.f));
		param_మారింది = fabsf(user_config_విలువ.f - new_విలువ.f) > FLT_EPSILON;
		ఆపు;

	అప్రమేయ: {
			PX4_ERR("param_set invalid param type for %s", param_name(param));
			ఆపు;
		}
	}

	ఒకవేళ (user_config.store(param, new_విలువ)) {
		ఒకవేళ (param_మారింది) {
			params_unsaved.సమితి(param, !mark_saved);
		}

		ఫలితం = PX4_OK;

	} లేకపోతే {
		PX4_ERR("param_set failed to store param %s", param_name(param));
		ఫలితం = PX4_ERROR;
	}

	ఒకవేళ ((ఫలితం == PX4_OK) && param_మారింది && !mark_saved && autosave) { // this is false when importing parameters
		param_autosave();
	}

	// If this is the parameter server, make sure that the remote is updated
#ఒకవేళ defined(CONFIG_PARAM_PRIMARY)

	ఒకవేళ (param_మారింది && update_remote) {
		param_primary_సమితి_విలువ(param, val);
	}

#అయితే_ముగింపు

	// If this is the parameter remote, make sure that the primary is updated
#ఒకవేళ defined(CONFIG_PARAM_REMOTE)

	ఒకవేళ (param_మారింది && update_remote) {
		param_remote_సమితి_విలువ(param, val);
	}

#అయితే_ముగింపు

	perf_చివర(param_సమితి_perf);

	/*
	 * If we set something, now that we have unlocked, go ahead and advertise that
	 * a thing has been set.
	 */
	ఒకవేళ ((ఫలితం == PX4_OK) && param_మారింది && notify_changes) {
		param_notify_changes();
	}

	తిరిగి ఫలితం;
}

#ఒకవేళ defined(FLASH_BASED_PARAMS)
పూర్ణాంకం param_సమితి_external(param_t param, స్థిరరాశి శూన్య *val, తార్కిక mark_saved, తార్కిక notify_changes)
{
	తిరిగి param_సమితి_అంతర్గత(param, val, mark_saved, notify_changes);
}

శూన్య param_get_external(param_t param, శూన్య *val)
{
	param_get(param, val);
}
#అయితే_ముగింపు

పూర్ణాంకం param_సమితి(param_t param, స్థిరరాశి శూన్య *val)
{
	తిరిగి param_సమితి_అంతర్గత(param, val, అబద్ధం, నిజం);
}

పూర్ణాంకం param_సమితి_no_notification(param_t param, స్థిరరాశి శూన్య *val)
{
	తిరిగి param_సమితి_అంతర్గత(param, val, అబద్ధం, అబద్ధం);
}

పూర్ణాంకం param_సమితి_no_remote_update(param_t param, స్థిరరాశి శూన్య *val, తార్కిక notify)
{
	తిరిగి param_సమితి_అంతర్గత(param, val, అబద్ధం, notify, అబద్ధం);
}

తార్కిక param_వాడిన(param_t param)
{
	ఒకవేళ (handle_లోపల_range(param)) {
		తిరిగి params_చురుకైన[param];
	}

	తిరిగి అబద్ధం;
}

శూన్య param_సమితి_వాడిన(param_t param)
{
	ఒకవేళ (handle_లోపల_range(param)) {
#ఒకవేళ defined(CONFIG_PARAM_REMOTE)

		ఒకవేళ (!param_వాడిన(param)) {
			param_remote_సమితి_వాడిన(param);
		}

#అయితే_ముగింపు

		params_చురుకైన.సమితి(param, నిజం);
	}
}

పూర్ణాంకం param_సమితి_default_విలువ(param_t param, స్థిరరాశి శూన్య *val)
{
	ఒకవేళ (!handle_లోపల_range(param)) {
		PX4_ERR("set default value invalid param %d", param);
		తిరిగి PX4_ERROR;
	}

	ఒకవేళ (val == శూన్యసూచిక) {
		PX4_ERR("set default value invalid value");
		తిరిగి PX4_ERROR;
	}

	ఒకవేళ (param_is_readonly(param)) {
		PX4_WARN("param %s is read-only", param_name(param));
		తిరిగి PX4_ERROR;
	}

	పూర్ణాంకం ఫలితం = PX4_ERROR;


	// check if param being set to default value
	తార్కిక setting_గమ్యం_static_default = అబద్ధం;
	స్థిరరాశి param_విలువ_u firmware_default_విలువ = firmware_defaults.get(param);

	ఎంపిక (param_type(param)) {
	స్థితి PARAM_TYPE_INT32: {
			int32_t new_విలువ;
			జ్ఞాపకం_నకలు(&new_విలువ, val, పరిమాణం(new_విలువ));
			setting_గమ్యం_static_default = firmware_default_విలువ.i == new_విలువ;
			ఆపు;
		}

	స్థితి PARAM_TYPE_FLOAT: {
			దశాంశం new_విలువ;
			జ్ఞాపకం_నకలు(&new_విలువ, val, పరిమాణం(new_విలువ));
			setting_గమ్యం_static_default = fabsf(firmware_default_విలువ.f - new_విలువ) <= FLT_EPSILON;
			ఆపు;
		}
	}

	ఒకవేళ (setting_గమ్యం_static_default) {
		runtime_defaults.reset(param);

		ఫలితం = PX4_OK;

	} లేకపోతే {
		param_విలువ_u new_విలువ{};

		ఎంపిక (param_type(param)) {
		స్థితి PARAM_TYPE_INT32: {
				జ్ఞాపకం_నకలు(&new_విలువ.i, val, పరిమాణం(new_విలువ.i));
				ఆపు;
			}

		స్థితి PARAM_TYPE_FLOAT: {
				జ్ఞాపకం_నకలు(&new_విలువ.f, val, పరిమాణం(new_విలువ.f));
				ఆపు;
			}

		అప్రమేయ:
			ఆపు;
		}

		ఒకవేళ (runtime_defaults.store(param, new_విలువ)) {
			user_config.refresh(param);
			ఫలితం = PX4_OK;

		} లేకపోతే {
			ఫలితం = PX4_ERROR;
		}
	}


	ఒకవేళ ((ఫలితం == PX4_OK) && param_వాడిన(param)) {
		// send notification if param is already in use
		param_notify_changes();
	}

	తిరిగి ఫలితం;
}

స్థిర పూర్ణాంకం param_reset_అంతర్గత(param_t param, తార్కిక notify = నిజం, తార్కిక autosave = నిజం)
{
#ఒకవేళ defined(CONFIG_PARAM_REMOTE)
	// Remote doesn't support reset
	తిరిగి అబద్ధం;
#అయితే_ముగింపు

	ఒకవేళ (param_is_readonly(param)) {
		తిరిగి 0;  // silently skip — param_reset_all loops over all params
	}

	తార్కిక param_దొరికింది = user_config.కలిగియుంది(param);

	ఒకవేళ (handle_లోపల_range(param)) {
		user_config.reset(param);
	}

	ఒకవేళ (param_దొరికింది) {
		/* Persist the reset. Flash-param saves append only unsaved params,
		 * so a reset has to show up as a tombstone or replay keeps the
		 * previous override. */
		params_unsaved.సమితి(param, నిజం);
	}

	ఒకవేళ (autosave) {
		param_autosave();
	}

	ఒకవేళ (param_దొరికింది && notify) {
		param_notify_changes();
	}

#ఒకవేళ defined(CONFIG_PARAM_PRIMARY)
	param_primary_reset(param);
#అయితే_ముగింపు

	తిరిగి param_దొరికింది;
}

పూర్ణాంకం param_reset(param_t param) { తిరిగి param_reset_అంతర్గత(param, నిజం); }
పూర్ణాంకం param_reset_no_notification(param_t param) { తిరిగి param_reset_అంతర్గత(param, అబద్ధం); }

స్థిర శూన్య
param_reset_all_అంతర్గత(తార్కిక auto_save)
{
#ఒకవేళ defined(CONFIG_PARAM_REMOTE)
	// Remote doesn't support reset
	తిరిగి;
#అయితే_ముగింపు

	కొరకు (param_t param = 0; handle_లోపల_range(param); param++) {
		param_reset_అంతర్గత(param, అబద్ధం, అబద్ధం);
	}

	ఒకవేళ (auto_save) {
		param_autosave();
	}

#ఒకవేళ defined(CONFIG_PARAM_PRIMARY)
	param_primary_reset_all();
#అయితే_ముగింపు

	param_notify_changes();
}

శూన్య
param_reset_all()
{
	param_reset_all_అంతర్గత(నిజం);
}

శూన్య
param_reset_excludes(స్థిరరాశి అక్షరం *excludes[], పూర్ణాంకం అంకె_excludes)
{
	కొరకు (param_t param = 0; handle_లోపల_range(param); param++) {
		స్థిరరాశి అక్షరం *name = param_name(param);
		తార్కిక exclude = అబద్ధం;

		కొరకు (పూర్ణాంకం సూచీ = 0; సూచీ < అంకె_excludes; సూచీ ++) {
			పూర్ణాంకం len = అక్షరశ్రేణి_పొడవు(excludes[సూచీ]);

			ఒకవేళ ((excludes[సూచీ][len - 1] == '*'
			     && అక్షరశ్రేణి_పోలిక(name, excludes[సూచీ], len - 1) == 0)
			    || strcmp(name, excludes[సూచీ]) == 0) {

				exclude = నిజం;
				ఆపు;
			}
		}

		ఒకవేళ (!exclude) {
			param_reset(param);
		}
	}
}

శూన్య
param_reset_specific(స్థిరరాశి అక్షరం *resets[], పూర్ణాంకం అంకె_resets)
{
	కొరకు (param_t param = 0; handle_లోపల_range(param); param++) {
		స్థిరరాశి అక్షరం *name = param_name(param);
		తార్కిక reset = అబద్ధం;

		కొరకు (పూర్ణాంకం సూచీ = 0; సూచీ < అంకె_resets; సూచీ++) {
			పూర్ణాంకం len = అక్షరశ్రేణి_పొడవు(resets[సూచీ]);

			ఒకవేళ ((resets[సూచీ][len - 1] == '*'
			     && అక్షరశ్రేణి_పోలిక(name, resets[సూచీ], len - 1) == 0)
			    || strcmp(name, resets[సూచీ]) == 0) {

				reset = నిజం;
				ఆపు;
			}
		}

		ఒకవేళ (reset) {
			param_reset(param);
		}
	}
}

పూర్ణాంకం
param_సమితి_default_file(స్థిరరాశి అక్షరం *filename)
{
	ఒకవేళ (filename && param_backup_file && strcmp(filename, param_backup_file) == 0) {
		PX4_ERR("default file can't be the same as the backup file %s", filename);
		తిరిగి PX4_ERROR;
	}

#నిర్వచితమైతే FLASH_BASED_PARAMS
	// the default for flash-based params is always the FLASH
	(శూన్య)filename;
#లేకపోతే

	ఒకవేళ (param_default_file != శూన్యసూచిక) {
		// we assume this is not in use by some other thread
		విడుదల(param_default_file);
		param_default_file = శూన్యసూచిక;
	}

	ఒకవేళ (filename) {
		param_default_file = strdup(filename);
	}

#అయితే_ముగింపు /* FLASH_BASED_PARAMS */

	తిరిగి 0;
}

స్థిరరాశి అక్షరం *param_get_default_file()
{
	తిరిగి param_default_file;
}

పూర్ణాంకం param_సమితి_backup_file(స్థిరరాశి అక్షరం *filename)
{
	ఒకవేళ (filename && param_default_file && strcmp(filename, param_default_file) == 0) {
		PX4_ERR("backup file can't be the same as the default file %s", filename);
		తిరిగి PX4_ERROR;
	}

	ఒకవేళ (param_backup_file != శూన్యసూచిక) {
		// we assume this is not in use by some other thread
		విడుదల(param_backup_file);
		param_backup_file = శూన్యసూచిక;
	}

	ఒకవేళ (filename) {
		param_backup_file = strdup(filename);

	} లేకపోతే {
		param_backup_file = శూన్యసూచిక; // backup disabled
	}

	తిరిగి 0;
}

స్థిరరాశి అక్షరం *param_get_backup_file()
{
	తిరిగి param_backup_file;
}

స్థిర పూర్ణాంకం param_export_అంతర్గత(పూర్ణాంకం fd, param_filter_func filter);
స్థిర పూర్ణాంకం param_verify(పూర్ణాంకం fd);

పూర్ణాంకం param_save_default(తార్కిక blocking)
{
	PX4_DEBUG("param_save_default");

	// take the file lock
	ఒకవేళ (blocking) {
		pthread_mutex_lock(&file_mutex);

	} లేకపోతే {
		ఒకవేళ (pthread_mutex_trylock(&file_mutex) != 0) {
			PX4_DEBUG("param_save_default: file lock failed (already locked)");
			తిరిగి -EWOULDBLOCK;
		}
	}

	పూర్ణాంకం shutdown_తాళం_ret = px4_shutdown_తాళం();

	ఒకవేళ (shutdown_తాళం_ret != 0) {
		PX4_ERR("px4_shutdown_lock() failed (%i)", shutdown_తాళం_ret);
	}

	పూర్ణాంకం res = PX4_ERROR;
	స్థిరరాశి అక్షరం *filename = param_get_default_file();

	కొరకు (param_t param = 0; handle_లోపల_range(param); param++) {
		ఒకవేళ (params_unsaved[param]) {
			params_saving.సమితి(param, నిజం);
			params_unsaved.సమితి(param, అబద్ధం);
		}
	}

	ఒకవేళ (filename) {
		స్థిర constexpr పూర్ణాంకం MAX_ATTEMPTS = 3;

		కొరకు (పూర్ణాంకం attempt = 1; attempt <= MAX_ATTEMPTS; attempt++) {
			// write parameters to file
			పూర్ణాంకం fd = ::open(filename, O_WRONLY | O_CREAT | O_TRUNC, PX4_O_బహుళకం_666);

			ఒకవేళ (fd > -1) {
				perf_begin(param_export_perf);
				res = param_export_అంతర్గత(fd, శూన్యసూచిక);
				perf_చివర(param_export_perf);
				::close(fd);

				ఒకవేళ (res == PX4_OK) {
					// reopen file to verify
					పూర్ణాంకం fd_verify = ::open(filename, O_RDONLY, PX4_O_బహుళకం_666);
					res = param_verify(fd_verify) || lseek(fd_verify, 0, SEEK_SET) || param_verify(fd_verify);
					::close(fd_verify);
				}
			}

			ఒకవేళ (res == PX4_OK) {
				ఆపు;

			} లేకపోతే {
				PX4_ERR("parameter export to %s failed (%d) attempt %d", filename, res, attempt);
				px4_usleep(10000); // wait at least 10 milliseconds before trying again
			}
		}

	} లేకపోతే {
		perf_begin(param_export_perf);
		res = flash_param_save(శూన్యసూచిక);
		perf_చివర(param_export_perf);
	}

	ఒకవేళ (res != PX4_OK) {
		PX4_ERR("param export failed (%d)", res);

		కొరకు (param_t param = 0; handle_లోపల_range(param); param++) {
			ఒకవేళ (params_saving[param]) {
				params_unsaved.సమితి(param, నిజం);
			}
		}
	}

	params_saving.reset();

	ఒకవేళ (res == PX4_OK) {
		// backup file
		ఒకవేళ (param_backup_file) {
			పూర్ణాంకం fd_backup_file = ::open(param_backup_file, O_WRONLY | O_CREAT | O_TRUNC, PX4_O_బహుళకం_666);

			ఒకవేళ (fd_backup_file > -1) {
				పూర్ణాంకం backup_export_ret = param_export_అంతర్గత(fd_backup_file, శూన్యసూచిక);
				::close(fd_backup_file);

				ఒకవేళ (backup_export_ret != 0) {
					PX4_ERR("backup parameter export to %s failed (%d)", param_backup_file, backup_export_ret);

				} లేకపోతే {
					// verify export
					పూర్ణాంకం fd_verify = ::open(param_backup_file, O_RDONLY, PX4_O_బహుళకం_666);
					param_verify(fd_verify);
					::close(fd_verify);
				}
			}
		}
	}

	pthread_mutex_unlock(&file_mutex);

	ఒకవేళ (shutdown_తాళం_ret == 0) {
		px4_shutdown_unlock();
	}

	తిరిగి res;
}

/**
 * @return 0 on success, 1 if all params have not yet been stored, -1 if device open failed, -2 if writing parameters failed
 */
పూర్ణాంకం
param_ఎక్కించు_default()
{
	పూర్ణాంకం res = 0;
	స్థిరరాశి అక్షరం *filename = param_get_default_file();

	ఒకవేళ (!filename) {
		తిరిగి param_ఎక్కించు(-1);
	}

	పూర్ణాంకం fd_ఎక్కించు = ::open(filename, O_RDONLY);

	ఒకవేళ (fd_ఎక్కించు < 0) {
		/* no parameter file is OK, otherwise this is an error */
		ఒకవేళ (errno != ENOENT) {
			PX4_ERR("open '%s' for reading failed", filename);
			తిరిగి -1;
		}

		తిరిగి 1;
	}

	పూర్ణాంకం ఫలితం = param_ఎక్కించు(fd_ఎక్కించు);
	::close(fd_ఎక్కించు);

	ఒకవేళ (ఫలితం == 1) {
		/* blank file: nothing stored yet */
		తిరిగి 1;
	}

	ఒకవేళ (ఫలితం != 0) {
		PX4_ERR("error reading parameters from '%s'", filename);
		తిరిగి -2;
	}

	తిరిగి res;
}

స్థిర పూర్ణాంకం param_verify_callback(bson_decoder_t decoder, bson_కణుపు_t కణుపు_2)
{
	ఒకవేళ (కణుపు_2->type == BSON_EOO) {
		తిరిగి 0;
	}

	// find the parameter this node represents
	param_t param = param_కనుగొను_no_notification(కణుపు_2->name);

	ఒకవేళ (param == PARAM_INVALID) {
		PX4_ERR("verify: invalid parameter '%s'", కణుపు_2->name);
		తిరిగి -1;
	}

	// handle verifying the parameter from the node
	ఎంపిక (కణుపు_2->type) {
	స్థితి BSON_INT32: {
			ఒకవేళ (param_type(param) != PARAM_TYPE_INT32) {
				PX4_ERR("verify: invalid param type %d for '%s' (BSON_INT32)", param_type(param), కణుపు_2->name);
				తిరిగి -1;
			}

			int32_t value;

			ఒకవేళ (param_get(param, &value) == 0) {
				ఒకవేళ (value == కణుపు_2->i32) {
					తిరిగి 1; // valid

				} లేకపోతే {
					PX4_ERR("verify: '%s' invalid BSON value %" PRIi32 "(expected %" PRIi32 ")", కణుపు_2->name, కణుపు_2->i32, value);
				}
			}
		}
		ఆపు;

	స్థితి BSON_DOUBLE: {
			ఒకవేళ (param_type(param) != PARAM_TYPE_FLOAT) {
				PX4_ERR("verify: invalid param type %d for '%s' (BSON_DOUBLE)", param_type(param), కణుపు_2->name);
				తిరిగి -1;
			}

			దశాంశం value;

			ఒకవేళ (param_get(param, &value) == 0) {
				ఒకవేళ (fabsf(value - (దశాంశం)కణుపు_2->d) <= FLT_EPSILON) {
					తిరిగి 1; // valid

				} లేకపోతే {
					PX4_ERR("verify: '%s' invalid BSON value %.3f (expected %.3f)", కణుపు_2->name, కణుపు_2->d, (ద్విగుణ)value);
				}
			}
		}
		ఆపు;

	అప్రమేయ:
		PX4_ERR("verify: '%s' invalid node type %d", కణుపు_2->name, కణుపు_2->type);
	}

	తిరిగి -1;
}

స్థిర పూర్ణాంకం param_verify(పూర్ణాంకం fd)
{
	PX4_DEBUG("param_verify");

	ఒకవేళ (fd < 0) {
		తిరిగి -1;
	}

	ఒకవేళ (lseek(fd, 0, SEEK_SET) != 0) {
		PX4_ERR("verify: seek failed");
		తిరిగి -1;
	}

	bson_decoder_s decoder{};

	ఒకవేళ (bson_decoder_init_file(&decoder, fd, param_verify_callback) == 0) {
		పూర్ణాంకం ఫలితం = -1;

		చేయి {
			ఫలితం = bson_decoder_తదుపరి(&decoder);

		} అయితే (ఫలితం > 0);

		ఒకవేళ (ఫలితం == 0) {
			ఒకవేళ (decoder.మొత్తం_document_కొలత != decoder.మొత్తం_decoded_కొలత) {
				PX4_ERR("BSON document size (%" PRId32 ") doesn't match bytes decoded (%" PRId32 ")", decoder.మొత్తం_document_కొలత,
					decoder.మొత్తం_decoded_కొలత);

			} లేకపోతే {
				తిరిగి 0;
			}

		} లేకపోతే ఒకవేళ (ఫలితం == -ENODATA) {
			PX4_ERR("verify: no BSON data");

		} లేకపోతే {
			PX4_ERR("verify: failed (%d)", ఫలితం);
		}
	}

	తిరిగి -1;
}

పూర్ణాంకం
param_export(స్థిరరాశి అక్షరం *filename, param_filter_func filter)
{
	PX4_DEBUG("param_export");

	పూర్ణాంకం shutdown_తాళం_ret = px4_shutdown_తాళం();

	ఒకవేళ (shutdown_తాళం_ret != 0) {
		PX4_ERR("px4_shutdown_lock() failed (%i)", shutdown_తాళం_ret);
	}

	// take the file lock
	ఒకవేళ (pthread_mutex_trylock(&file_mutex) != 0) {
		PX4_ERR("param_export: file lock failed (already locked)");
		తిరిగి PX4_ERROR;
	}

	పూర్ణాంకం fd = ::open(filename, O_RDWR | O_CREAT, PX4_O_బహుళకం_666);
	పూర్ణాంకం ఫలితం = PX4_ERROR;

	perf_begin(param_export_perf);

	ఒకవేళ (fd > -1) {
		ఫలితం = param_export_అంతర్గత(fd, filter);

	} లేకపోతే {
		ఫలితం = flash_param_save(filter);
	}

	perf_చివర(param_export_perf);

	pthread_mutex_unlock(&file_mutex);

	ఒకవేళ (shutdown_తాళం_ret == 0) {
		px4_shutdown_unlock();
	}

	తిరిగి ఫలితం;
}

// internal parameter export, caller is responsible for locking
స్థిర పూర్ణాంకం param_export_అంతర్గత(పూర్ణాంకం fd, param_filter_func filter)
{
	PX4_DEBUG("param_export_internal");
	స్థిరరాశి స్వయంచాలక మారింది_params = user_config.containedAsBitset();

	పూర్ణాంకం ఫలితం = -1;
	bson_encoder_s encoder{};
	uint8_t bson_బఫరు[256];

	ఒకవేళ (lseek(fd, 0, SEEK_SET) != 0) {
		PX4_ERR("export seek failed %d", errno);
		తిరిగి -1;
	}

	ఒకవేళ (bson_encoder_init_buf_file(&encoder, fd, &bson_బఫరు, పరిమాణం(bson_బఫరు)) != 0) {
		వెళ్ళు బహిర్గతం;
	}

	కొరకు (param_t param = 0; handle_లోపల_range(param); param++) {
		ఒకవేళ (!మారింది_params[param] || (filter && !filter(param))) {
			కొనసాగు;
		}

		స్థిరరాశి param_విలువ_u runtime_default_విలువ = runtime_defaults.get(param);
		స్థిరరాశి param_విలువ_u user_config_విలువ = user_config.get(param);

		// don't export default values
		ఎంపిక (param_type(param)) {
		స్థితి PARAM_TYPE_INT32:
			ఒకవేళ (user_config_విలువ.i == runtime_default_విలువ.i) {
				PX4_DEBUG("skipping %s %" PRIi32 " export", param_name(param), runtime_default_విలువ.i);
				కొనసాగు;
			}

			ఆపు;

		స్థితి PARAM_TYPE_FLOAT:
			ఒకవేళ (fabsf(user_config_విలువ.f - runtime_default_విలువ.f) <= FLT_EPSILON) {
				PX4_DEBUG("skipping %s %.3f export", param_name(param), (ద్విగుణ)runtime_default_విలువ.f);
				కొనసాగు;
			}

			ఆపు;
		}

		స్థిరరాశి అక్షరం *name = param_name(param);
		స్థిరరాశి size_t కొలత = param_కొలత(param);

		/* append the appropriate BSON type object */
		ఎంపిక (param_type(param)) {
		స్థితి PARAM_TYPE_INT32: {
				స్థిరరాశి int32_t i = user_config_విలువ.i;
				PX4_DEBUG("exporting: %s (%d) size: %lu val: %" PRIi32, name, param, (దీర్ఘ గుర్తులేని పూర్ణాంకం)కొలత, i);

				ఒకవేళ (bson_encoder_append_int32(&encoder, name, i) != 0) {
					PX4_ERR("BSON append failed for '%s'", name);
					వెళ్ళు బహిర్గతం;
				}
			}
			ఆపు;

		స్థితి PARAM_TYPE_FLOAT: {
				స్థిరరాశి ద్విగుణ f = (ద్విగుణ)user_config_విలువ.f;
				PX4_DEBUG("exporting: %s (%d) size: %lu val: %.3f", name, param, (దీర్ఘ గుర్తులేని పూర్ణాంకం)కొలత, (ద్విగుణ)f);

				ఒకవేళ (bson_encoder_append_double(&encoder, name, f) != 0) {
					PX4_ERR("BSON append failed for '%s'", name);
					వెళ్ళు బహిర్గతం;
				}
			}
			ఆపు;

		అప్రమేయ:
			PX4_ERR("%s unrecognized parameter type %d, skipping export", name, param_type(param));
		}
	}

	ఫలితం = 0;

బహిర్గతం:

	ఒకవేళ (ఫలితం == 0) {
		ఒకవేళ (bson_encoder_fini(&encoder) != PX4_OK) {
			PX4_ERR("BSON encoder finalize failed");
			ఫలితం = -1;
		}
	}

	తిరిగి ఫలితం;
}

#ఒకవేళ defined(FLASH_BASED_PARAMS)
// A file is never the flash backend's store, so what an import changes is unsaved until a delta carries it.
// As on the file backend, the import itself does not save.
స్థిర constexpr తార్కిక file_import_is_saved = అబద్ధం;
#లేకపోతే
స్థిర constexpr తార్కిక file_import_is_saved = నిజం;
#అయితే_ముగింపు

స్థిర పూర్ణాంకం
param_import_callback(bson_decoder_t decoder, bson_కణుపు_t కణుపు_2)
{
	/*
	 * EOO means the end of the parameter object. (Currently not supporting
	 * nested BSON objects).
	 */
	ఒకవేళ (కణుపు_2->type == BSON_EOO) {
		PX4_DEBUG("end of parameters");
		తిరిగి 0;
	}

	// A tombstone has no value. The decoder leaves the previous node's in the union and the translations
	// read it without a type check.
	స్థిరరాశి తార్కిక tombstone = కణుపు_2->type == BSON_nullptr || కణుపు_2->type == BSON_UNDEFINED;

	// if we do param_set() directly in the translation, set PARAM_SKIP_IMPORT as return value and return here
	ఒకవేళ (!tombstone && param_modify_on_import(కణుపు_2) == param_modify_on_import_ret::PARAM_SKIP_IMPORT) {
		తిరిగి 1;
	}

	// Find the parameter this node represents.  If we don't know it, ignore the node.
	param_t param = param_కనుగొను_no_notification(కణుపు_2->name);

	ఒకవేళ (param == PARAM_INVALID) {
		PX4_WARN("ignoring unrecognised parameter '%s'", కణుపు_2->name);
		తిరిగి 1;
	}

	// Handle setting the parameter from the node
	ఎంపిక (కణుపు_2->type) {
	స్థితి BSON_nullptr:
	స్థితి BSON_UNDEFINED:
		param_reset_అంతర్గత(param, నిజం, అబద్ధం);
		తిరిగి 1;

	స్థితి BSON_INT32: {
			ఒకవేళ (param_type(param) == PARAM_TYPE_INT32) {
				int32_t i = కణుపు_2->i32;
				param_సమితి_అంతర్గత(param, &i, file_import_is_saved, నిజం, నిజం, అబద్ధం);
				PX4_DEBUG("Imported %s with value %" PRIi32, param_name(param), i);

			} లేకపోతే {
				PX4_WARN("unexpected type for %s", కణుపు_2->name);
			}
		}
		ఆపు;

	స్థితి BSON_DOUBLE: {
			ఒకవేళ (param_type(param) == PARAM_TYPE_FLOAT) {
				దశాంశం f = కణుపు_2->d;
				param_సమితి_అంతర్గత(param, &f, file_import_is_saved, నిజం, నిజం, అబద్ధం);
				PX4_DEBUG("Imported %s with value %f", param_name(param), (ద్విగుణ)f);

			} లేకపోతే {
				PX4_WARN("unexpected type for %s", కణుపు_2->name);
			}
		}
		ఆపు;

	అప్రమేయ:
		PX4_ERR("import: unrecognised node type for '%s'", కణుపు_2->name);
	}

	// don't return zero, that means EOF
	తిరిగి 1;
}

స్థిర పూర్ణాంకం
param_import_అంతర్గత(పూర్ణాంకం fd)
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
	నిర్మాణం stat st;

	ఒకవేళ (fstat(fd, &st) == 0 && S_ISREG(st.st_mode) && st.st_size == 0) {
		తిరిగి 1;
	}

	int32_t doc_కొలత = 0;
	ssize_t len = పఠ(fd, &doc_కొలత, పరిమాణం(doc_కొలత));

	ఒకవేళ (len == 0 || (len == (ssize_t)పరిమాణం(doc_కొలత) && (doc_కొలత == 0 || doc_కొలత == -1))) {
		తిరిగి 1;
	}

	ఒకవేళ (lseek(fd, 0, SEEK_SET) != 0) {
		PX4_ERR("import lseek failed (%d)", errno);
		తిరిగి -1;
	}

	స్థిర constexpr పూర్ణాంకం MAX_ATTEMPTS = 3;

	కొరకు (పూర్ణాంకం attempt = 1; attempt <= MAX_ATTEMPTS; attempt++) {
		bson_decoder_s decoder{};

		ఒకవేళ (bson_decoder_init_file(&decoder, fd, param_import_callback) == 0) {
			పూర్ణాంకం ఫలితం = -1;

			చేయి {
				ఫలితం = bson_decoder_తదుపరి(&decoder);

			} అయితే (ఫలితం > 0);

			ఒకవేళ (ఫలితం == 0) {
				ఒకవేళ (decoder.మొత్తం_document_కొలత == decoder.మొత్తం_decoded_కొలత) {
					PX4_INFO("BSON document size %" PRId32 " bytes, decoded %" PRId32 " bytes (INT32:%" PRIu16 ", FLOAT:%" PRIu16 ")",
						 decoder.మొత్తం_document_కొలత, decoder.మొత్తం_decoded_కొలత,
						 decoder.లెక్క_కణుపు_int32, decoder.లెక్క_కణుపు_double);

					తిరిగి 0;

				} లేకపోతే {
					PX4_ERR("BSON document size (%" PRId32 ") doesn't match bytes decoded (%" PRId32 ")",
						decoder.మొత్తం_document_కొలత, decoder.మొత్తం_decoded_కొలత);
				}

			} లేకపోతే ఒకవేళ (ఫలితం == -ENODATA) {
				// silently retry as a precaution unless this is our last attempt
				ఒకవేళ (attempt == MAX_ATTEMPTS) {
					PX4_DEBUG("BSON: no data");
					తిరిగి 0;
				}

			} లేకపోతే {
				PX4_ERR("param import failed (%d) attempt %d", ఫలితం, attempt);
			}

		} లేకపోతే {
			PX4_ERR("param import bson decoder init failed (attempt %d)", attempt);
		}

		ఒకవేళ (attempt != MAX_ATTEMPTS) {
			ఒకవేళ (lseek(fd, 0, SEEK_SET) != 0) {
				PX4_ERR("import lseek failed (%d)", errno);
			}

			px4_usleep(10000); // wait at least 10 milliseconds before trying again
		}
	}

	తిరిగి -1;
}

#ఒకవేళ defined(FLASH_BASED_PARAMS)
స్థిర పూర్ణాంకం flash_backend_locked(పూర్ణాంకం (*op)())
{
	/* Same order as param_save_default: mutex then shutdown lock. Autosave
	 * uses a trylock, so a compact holds it off the flash programming. */
	pthread_mutex_lock(&file_mutex);

	పూర్ణాంకం shutdown_తాళం_ret = px4_shutdown_తాళం();

	ఒకవేళ (shutdown_తాళం_ret != 0) {
		PX4_ERR("px4_shutdown_lock() failed (%i)", shutdown_తాళం_ret);
	}

	పూర్ణాంకం ఫలితం = op();

	pthread_mutex_unlock(&file_mutex);

	ఒకవేళ (shutdown_తాళం_ret == 0) {
		px4_shutdown_unlock();
	}

	తిరిగి ఫలితం;
}

స్థిర పూర్ణాంకం flash_ఎక్కించు_locked()
{
	/* Reset under the lock so a pending autosave cannot append tombstones
	 * of the just-cleared RAM before import/compact run. */
	param_reset_all_అంతర్గత(అబద్ధం);
	పూర్ణాంకం ఫలితం = flash_param_import();

	ఒకవేళ (ఫలితం == 0 || ఫలితం == 1) {
		params_unsaved.reset();
	}

	తిరిగి ఫలితం;
}
#అయితే_ముగింపు

పూర్ణాంకం
param_import(పూర్ణాంకం fd)
{
	ఒకవేళ (fd < 0) {
#ఒకవేళ defined(FLASH_BASED_PARAMS)
		తిరిగి flash_backend_locked(flash_param_import);
#లేకపోతే
		తిరిగి flash_param_import();
#అయితే_ముగింపు
	}

	తిరిగి param_import_అంతర్గత(fd);
}

పూర్ణాంకం
param_ఎక్కించు(పూర్ణాంకం fd)
{
	ఒకవేళ (fd < 0) {
#ఒకవేళ defined(FLASH_BASED_PARAMS)
		తిరిగి flash_backend_locked(flash_ఎక్కించు_locked);
#లేకపోతే
		తిరిగి PX4_ERROR;
#అయితే_ముగింపు
	}

	param_reset_all_అంతర్గత(అబద్ధం);
	పూర్ణాంకం ఫలితం = param_import_అంతర్గత(fd);

#ఒకవేళ !defined(FLASH_BASED_PARAMS)

	ఒకవేళ (ఫలితం >= 0) {
		params_unsaved.reset();
	}

#అయితే_ముగింపు

	తిరిగి ఫలితం;
}

శూన్య
param_foreach(శూన్య (*func)(శూన్య *arg, param_t param), శూన్య *arg, తార్కిక only_మారింది, తార్కిక only_వాడిన)
{
	param_t	param;

	కొరకు (param = 0; handle_లోపల_range(param); param++) {

		/* if requested, skip unchanged values */
		ఒకవేళ (only_మారింది && (!user_config.కలిగియుంది(param))) {
			కొనసాగు;
		}

		ఒకవేళ (only_వాడిన && !param_వాడిన(param)) {
			కొనసాగు;
		}

		func(arg, param);
	}
}

uint32_t param_hash_check()
{
	uint32_t param_hash = 0;

	/* compute the CRC32 over all string param names and 4 byte values */
	కొరకు (param_t param = 0; handle_లోపల_range(param); param++) {
		ఒకవేళ (!param_వాడిన(param) || param_is_volatile(param)) {
			కొనసాగు;
		}

		స్థిరరాశి అక్షరం *name = param_name(param);
		స్వయంచాలక value = user_config.get(param).i;
		స్థిరరాశి శూన్య *val = (శూన్య *)&value;
		param_hash = crc32part((స్థిరరాశి uint8_t *)name, అక్షరశ్రేణి_పొడవు(name), param_hash);
		param_hash = crc32part((స్థిరరాశి uint8_t *)val, param_కొలత(param), param_hash);
	}

	తిరిగి param_hash;
}

శూన్య param_print_స్థితి()
{
	PX4_INFO("summary: %d/%d (used/total)", param_లెక్క_వాడిన(), param_లెక్క());

#నిర్వచితం_కాకపోతే FLASH_BASED_PARAMS
	స్థిరరాశి అక్షరం *filename = param_get_default_file();

	ఒకవేళ (filename != శూన్యసూచిక) {
		PX4_INFO("file: %s", param_get_default_file());
	}

	ఒకవేళ (param_backup_file) {
		PX4_INFO("backup file: %s", param_backup_file);
	}

#అయితే_ముగింపు /* FLASH_BASED_PARAMS */

	PX4_INFO("storage array: %d/%d elements (%zu bytes total)",
		 user_config.కొలత(), firmware_defaults.కొలత(), (size_t)user_config.byte_కొలత());


	PX4_INFO("storage array (custom defaults): %d/%d elements (%zu bytes total)",
		 runtime_defaults.కొలత(), firmware_defaults.కొలత(), (size_t)runtime_defaults.byte_కొలత());

	ఒకవేళ (autosave_instance) {
		PX4_INFO("auto save: %s", autosave_instance->enabled() ? "on" : "off");

		hrt_abstime చివరి_autosave = autosave_instance->చివరి_Autosave();

		ఒకవేళ (చివరి_autosave > 0) {
			PX4_INFO("last auto save: %.3f seconds ago", hrt_elapsed_సమయం(&చివరి_autosave) * 1e-6);
		}
	}

	perf_print_counter(param_export_perf);
	perf_print_counter(param_కనుగొను_perf);
	perf_print_counter(param_get_perf);
	perf_print_counter(param_సమితి_perf);

#ఒకవేళ defined(CONFIG_PARAM_PRIMARY)
	నిర్మాణం param_primary_counters counts;
	param_primary_get_counters(&counts);
	PX4_INFO("set value requests received: %" PRIu32 ", set value responses sent: %" PRIu32,
		 counts.సమితి_విలువ_request_received, counts.సమితి_విలువ_response_పంపిన);
	PX4_INFO("set value requests sent: %" PRIu32 ", set value responses received: %" PRIu32,
		 counts.సమితి_విలువ_request_పంపిన, counts.సమితి_విలువ_response_received);
	PX4_INFO("resets sent: %" PRIu32 ", set used requests received: %" PRIu32,
		 counts.reset_పంపిన, counts.సమితి_వాడిన_received);
#అయితే_ముగింపు

#ఒకవేళ defined(CONFIG_PARAM_REMOTE)
	నిర్మాణం param_remote_counters counts;
	param_remote_get_counters(&counts);
	PX4_INFO("set value requests received: %" PRIu32 ", set value responses sent: %" PRIu32,
		 counts.సమితి_విలువ_request_received, counts.సమితి_విలువ_response_పంపిన);
	PX4_INFO("set value requests sent: %" PRIu32 ", set value responses received: %" PRIu32,
		 counts.సమితి_విలువ_request_పంపిన, counts.సమితి_విలువ_response_received);
	PX4_INFO("resets received: %" PRIu32 ", set used requests sent: %" PRIu32,
		 counts.reset_received, counts.సమితి_వాడిన_పంపిన);
#అయితే_ముగింపు

}
