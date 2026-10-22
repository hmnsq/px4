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
#ଅନ୍ତର୍ଭାଵ "param.h"
#ଅନ୍ତର୍ଭାଵ "param_translation.h"
#ଅନ୍ତର୍ଭାଵ <parameters/px4_parameters.hpp>
#ଅନ୍ତର୍ଭାଵ <lib/tinybson/tinybson.h>

#ଯଦି defined(__PX4_NUTTX)
#ଅନ୍ତର୍ଭାଵ <nuttx/crc32.h>
#ଅନ୍ୟଥା
#ଅନ୍ତର୍ଭାଵ <crc32.h>
#endif
#ଅନ୍ତର୍ଭାଵ <float.h>
#ଅନ୍ତର୍ଭାଵ <math.h>
#ଅନ୍ତର୍ଭାଵ <sys/stat.h>

#ଅନ୍ତର୍ଭାଵ <containers/Bitset.hpp>
#ଅନ୍ତର୍ଭାଵ <drivers/drv_hrt.h>
#ଅନ୍ତର୍ଭାଵ <lib/perf/perf_counter.h>
#ଅନ୍ତର୍ଭାଵ <px4_platform_common/px4_config.h>
#ଅନ୍ତର୍ଭାଵ <px4_platform_common/atomic_bitset.h>
#ଅନ୍ତର୍ଭାଵ <px4_platform_common/defines.h>
#ଅନ୍ତର୍ଭାଵ <px4_platform_common/posix.h>
#ଅନ୍ତର୍ଭାଵ <px4_platform_common/sem.h>
#ଅନ୍ତର୍ଭାଵ <px4_platform_common/shutdown.h>
#ଅନ୍ତର୍ଭାଵ <px4_platform_common/micro_hal.h>

using ନାମାକାଶ time_literals;

#ଅନ୍ତର୍ଭାଵ "uORB/uORB.h"
#ଅନ୍ତର୍ଭାଵ "uORB/topics/parameter_update.h"
#ଅନ୍ତର୍ଭାଵ <uORB/Subscription.hpp>

#ଅନ୍ତର୍ଭାଵ "ExhaustiveLayer.h"
#ଅନ୍ତର୍ଭାଵ "ConstLayer.h"
#ଅନ୍ତର୍ଭାଵ "DynamicSparseLayer.h"
#ଅନ୍ତର୍ଭାଵ "StaticSparseLayer.h"

#ଅନ୍ତର୍ଭାଵ "atomic_transaction.h"

/* Include functions common to user and kernel sides */
#ଅନ୍ତର୍ଭାଵ "parameters_common.cpp"

#ଯଦି defined(__PX4_NUTTX) && !defined(CONFIG_BUILD_FLAT)
#ଅନ୍ତର୍ଭାଵ <px4_platform/board_ctrl.h>
#ଅନ୍ତର୍ଭାଵ "parameters_ioctl.h"
#endif

#ଯଦି defined(FLASH_BASED_PARAMS)
#ଅନ୍ତର୍ଭାଵ "flashparams/flashparams.h"
#ଅନ୍ୟଥା
ଧାଡ଼ିଭିତରେ ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା flash_param_save(param_filter_func filter) { ଫେରନ୍ତୁ -1; }
ଧାଡ଼ିଭିତରେ ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା flash_param_import() { ଫେରନ୍ତୁ -1; }
#endif

ସ୍ଥିର ଅକ୍ଷର *param_default_file = nullptr;
ସ୍ଥିର ଅକ୍ଷର *param_backup_file = nullptr;

#ଅନ୍ତର୍ଭାଵ "autosave.h"
ସ୍ଥିର ParamAutosave *autosave_instance {nullptr};

ସ୍ଥିର px4::AtomicBitset<param_info_ଗଣନା> params_active;  // params found
ସ୍ଥିର px4::AtomicBitset<param_info_ଗଣନା> params_unsaved;

// Params the save in progress is writing. They are moved out of params_unsaved
// before their values are read, so a param_set that lands while the save runs
// marks its param again and reaches the next save. Flash-param saves write only
// unsaved params, so nothing else would ever write that change.
ସ୍ଥିର px4::AtomicBitset<param_info_ଗଣନା> params_saving;

ସ୍ଥିର ConstLayer firmware_defaults;
ସ୍ଥିର DynamicSparseLayer runtime_defaults{&firmware_defaults};
DynamicSparseLayer user_config{&runtime_defaults};

/** parameter update topic handle */
#ଯଦି not defined(CONFIG_PARAM_REMOTE)
ସ୍ଥିର orb_advert_t param_topic = nullptr;
ସ୍ଥିର unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା param_instance = 0;
#endif

ସ୍ଥିର perf_counter_t param_export_perf;
ସ୍ଥିର perf_counter_t param_find_perf;
ସ୍ଥିର perf_counter_t param_get_perf;
ସ୍ଥିର perf_counter_t param_set_perf;

ସ୍ଥିର pthread_mutex_t file_mutex  =
	PTHREAD_MUTEX_INITIALIZER; ///< this protects against concurrent param saves (file or flash access).

// Support for remote parameter node
#ଯଦି defined(CONFIG_PARAM_PRIMARY)
# include "parameters_primary.h"
#endif // CONFIG_PARAM_PRIMARY
#ଯଦି defined(CONFIG_PARAM_REMOTE)
# include "parameters_remote.h"
#endif // CONFIG_PARAM_REMOTE

ଶୂନ୍ୟ
param_init()
{
#ifdef __PX4_POSIX
	// Set up the recursive param mutex before any thread can access parameters.
	AtomicTransaction::initialize();
#endif

	param_export_perf = perf_alloc(PC_ELAPSED, "param: export");
	param_find_perf = perf_alloc(PC_ଗଣନା, "param: find");
	param_get_perf = perf_alloc(PC_ଗଣନା, "param: get");
	param_set_perf = perf_alloc(PC_ELAPSED, "param: set");

#ଯଦି defined(__PX4_NUTTX) && !defined(CONFIG_BUILD_FLAT)
	px4_register_boardct_ioctl(_PARAMIOCBASE, param_ioctl);
#endif

#ଯଦି defined(CONFIG_PARAM_PRIMARY)
	param_primary_init();
#endif // CONFIG_PARAM_PRIMARY

#ଯଦି defined(CONFIG_PARAM_REMOTE)
	param_remote_init();
#endif // CONFIG_PARAM_REMOTE

#ଯଦି not defined(CONFIG_PARAM_REMOTE)
	autosave_instance = ନୂତନ ParamAutosave();
#endif
}


ଶୂନ୍ୟ
param_notify_changes()
{
// Don't send if this is a remote node. Only the primary
// sends out update notices
#ଯଦି not defined(CONFIG_PARAM_REMOTE)
	parameter_update_s pup {};
	pup.instance = param_instance++;
	pup.get_ଗଣନା_2 = perf_event_ଗଣନା(param_get_perf);
	pup.set_ଗଣନା = perf_event_ଗଣନା(param_set_perf);
	pup.find_ଗଣନା = perf_event_ଗଣନା(param_find_perf);
	pup.export_ଗଣନା = perf_event_ଗଣନା(param_export_perf);
	pup.active = params_active.ଗଣନା_2();
	pup.changed = user_config.size();
	pup.custom_default = runtime_defaults.size();
	pup.timestamp = hrt_absolute_time();

	ଯଦି (param_topic == nullptr) {
		param_topic = orb_advertise(ORB_ID(parameter_update), &pup);

	} ଅନ୍ୟଥା {
		orb_publish(ORB_ID(parameter_update), param_topic, &pup);
	}

#endif
}

ସ୍ଥିର param_t param_find_internal(ଅଚଳ ଅକ୍ଷର *name, ତାର୍କିକ notification)
{
	perf_ଗଣନା(param_find_perf);

	param_t ମଧ୍ୟ;
	param_t front = 0;
	param_t last = param_info_ଗଣନା;

	/* perform a binary search of the known parameters */

	ଯେତେବେଳେ (front <= last) {
		ମଧ୍ୟ = front + (last - front) / 2;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret = strcmp(name, param_name(ମଧ୍ୟ));

		ଯଦି (ret == 0) {
			ଯଦି (notification) {
				param_set_used(ମଧ୍ୟ);
			}

			ଫେରନ୍ତୁ ମଧ୍ୟ;

		} ଅନ୍ୟଥା ଯଦି (ମଧ୍ୟ == front) {
			/* An end point has been hit, but there has been no match */
			ଭାଙ୍ଗ;

		} ଅନ୍ୟଥା ଯଦି (ret < 0) {
			last = ମଧ୍ୟ;

		} ଅନ୍ୟଥା {
			front = ମଧ୍ୟ;
		}
	}

	/* not found */
	ଫେରନ୍ତୁ PARAM_INVALID;
}

param_t param_find(ଅଚଳ ଅକ୍ଷର *name)
{
	ଫେରନ୍ତୁ param_find_internal(name, ସତ୍ୟ);
}

param_t param_find_no_notification(ଅଚଳ ଅକ୍ଷର *name)
{
	ଫେରନ୍ତୁ param_find_internal(name, ମିଥ୍ୟା);
}

unsigned param_ଗଣନା_used()
{
	ଫେରନ୍ତୁ params_active.ଗଣନା_2();
}

param_t param_for_used_index(unsigned index)
{
	// walk all params and count used params
	ଯଦି (index < param_info_ଗଣନା) {
		unsigned used_ଗଣନା = 0;

		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < params_active.size(); i++) {
			ଯଦି (params_active[i]) {
				// we found the right used count,
				//  return the param value
				ଯଦି (index == used_ଗଣନା) {
					ଫେରନ୍ତୁ static_cast<param_t>(i);
				}

				used_ଗଣନା++;
			}
		}
	}

	ଫେରନ୍ତୁ PARAM_INVALID;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା param_get_used_index(param_t param)
{
	/* this tests for out of bounds and does a constant time lookup */
	ଯଦି (!param_used(param)) {
		ଫେରନ୍ତୁ -1;
	}

	/* walk all params and count, now knowing that it has a valid index */
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା used_ଗଣନା = 0;

	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < params_active.size(); i++) {
		ଯଦି (params_active[i]) {

			ଯଦି (param == i) {
				ଫେରନ୍ତୁ used_ଗଣନା;
			}

			used_ଗଣନା++;
		}
	}

	ଫେରନ୍ତୁ -1;
}

ତାର୍କିକ
param_ମୂଲ୍ୟ_unsaved(param_t param)
{
	ଫେରନ୍ତୁ handle_in_range(param) ? (params_unsaved[param] || params_saving[param]) : ମିଥ୍ୟା;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା
param_get(param_t param, ଶୂନ୍ୟ *val)
{
	perf_ଗଣନା(param_get_perf);

	ଯଦି (!handle_in_range(param)) {
		PX4_ERR("get: param %" PRId16 " invalid", param);
		ଫେରନ୍ତୁ PX4_ERROR;
	}

	ଯଦି (!params_active[param]) {
		PX4_DEBUG("get: param %" PRId16 " (%s) not active", param, param_name(param));
	}

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା result = PX4_ERROR;

	ଯଦି (val) {

		auto retrieve_ମୂଲ୍ୟ = user_config.get(param);

		ବାଛ (param_type(param)) {
		ସ୍ଥିତି PARAM_TYPE_INT32:
			memcpy(val, &retrieve_ମୂଲ୍ୟ.i, ଆକାର(retrieve_ମୂଲ୍ୟ.i));
			ଫେରନ୍ତୁ PX4_OK;

		ସ୍ଥିତି PARAM_TYPE_FLOAT:
			memcpy(val, &retrieve_ମୂଲ୍ୟ.f, ଆକାର(retrieve_ମୂଲ୍ୟ.f));
			ଫେରନ୍ତୁ PX4_OK;
		}
	}

	ଫେରନ୍ତୁ result;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା
param_get_default_ମୂଲ୍ୟ_internal(param_t param, ଶୂନ୍ୟ *default_val)
{
	ଯଦି (!handle_in_range(param)) {
		PX4_ERR("get default value: param %d invalid", param);
		ଫେରନ୍ତୁ PX4_ERROR;
	}

	ଯଦି (default_val) {
		ବାଛ (param_type(param)) {
		ସ୍ଥିତି PARAM_TYPE_INT32: {
				int32_t val = runtime_defaults.get(param).i;
				memcpy(default_val, &val, ଆକାର(val));
				ଫେରନ୍ତୁ PX4_OK;
			}

		ସ୍ଥିତି PARAM_TYPE_FLOAT: {
				ଦଶମିକ val = runtime_defaults.get(param).f;
				memcpy(default_val, &val, ଆକାର(val));
				ଫେରନ୍ତୁ PX4_OK;
			}
		}
	}

	ଫେରନ୍ତୁ PX4_ERROR;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା
param_get_default_ମୂଲ୍ୟ(param_t param, ଶୂନ୍ୟ *default_val)
{
	ଯଦି (!handle_in_range(param)) {
		ଫେରନ୍ତୁ PX4_ERROR;
	}

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret = param_get_default_ମୂଲ୍ୟ_internal(param, default_val);
	ଫେରନ୍ତୁ ret;
}

ତାର୍କିକ param_ମୂଲ୍ୟ_is_default(param_t param)
{
	ଯଦି (!handle_in_range(param)) {
		ଫେରନ୍ତୁ ସତ୍ୟ;
	}

	ଯଦି (!user_config.contains(param)) {
		// if user config does not contain it, consider it default.
		ଫେରନ୍ତୁ ସତ୍ୟ;

	} ଅନ୍ୟଥା {
		// compare with default value
		ଅଚଳ param_ମୂଲ୍ୟ_u user_config_ମୂଲ୍ୟ = user_config.get(param);
		ଅଚଳ param_ମୂଲ୍ୟ_u runtime_default_ମୂଲ୍ୟ = runtime_defaults.get(param);

		ବାଛ (param_type(param)) {
		ସ୍ଥିତି PARAM_TYPE_INT32: {
				ଫେରନ୍ତୁ user_config_ମୂଲ୍ୟ.i == runtime_default_ମୂଲ୍ୟ.i;
			}

		ସ୍ଥିତି PARAM_TYPE_FLOAT: {
				ଫେରନ୍ତୁ fabsf(user_config_ମୂଲ୍ୟ.f - runtime_default_ମୂଲ୍ୟ.f) < FLT_EPSILON;
			}
		}
	}

	ଫେରନ୍ତୁ ସତ୍ୟ;
}

ସ୍ଥିର ଶୂନ୍ୟ
param_autosave()
{
	ଯଦି (autosave_instance) {
		autosave_instance->request();
	}
}

ଶୂନ୍ୟ
param_control_autosave(ତାର୍କିକ enable)
{
	ଯଦି (autosave_instance) {
		autosave_instance->enable(enable);
	}
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
param_set_internal(param_t param, ଅଚଳ ଶୂନ୍ୟ *val, ତାର୍କିକ mark_saved, ତାର୍କିକ notify_changes, ତାର୍କିକ update_remote = ସତ୍ୟ,
		   ତାର୍କିକ autosave = ସତ୍ୟ)
{
	ଯଦି (!handle_in_range(param)) {
		PX4_ERR("set invalid param %d", param);
		ଫେରନ୍ତୁ PX4_ERROR;
	}

	ଯଦି (val == nullptr) {
		PX4_ERR("set invalid value");
		ଫେରନ୍ତୁ PX4_ERROR;
	}

	ଯଦି (param_is_readonly(param)) {
		PX4_WARN("param %s is read-only", param_name(param));
		ଫେରନ୍ତୁ PX4_ERROR;
	}

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା result = -1;
	ତାର୍କିକ param_changed = ମିଥ୍ୟା;
	perf_begin(param_set_perf);

	ଅଚଳ param_ମୂଲ୍ୟ_u user_config_ମୂଲ୍ୟ = user_config.get(param);
	param_ମୂଲ୍ୟ_u new_ମୂଲ୍ୟ{};

	ବାଛ (param_type(param)) {
	ସ୍ଥିତି PARAM_TYPE_INT32:
		memcpy(&new_ମୂଲ୍ୟ.i, val, ଆକାର(new_ମୂଲ୍ୟ.i));
		param_changed = user_config_ମୂଲ୍ୟ.i != new_ମୂଲ୍ୟ.i;
		ଭାଙ୍ଗ;

	ସ୍ଥିତି PARAM_TYPE_FLOAT:
		memcpy(&new_ମୂଲ୍ୟ.f, val, ଆକାର(new_ମୂଲ୍ୟ.f));
		param_changed = fabsf(user_config_ମୂଲ୍ୟ.f - new_ମୂଲ୍ୟ.f) > FLT_EPSILON;
		ଭାଙ୍ଗ;

	ପୂର୍ବନିର୍ଦ୍ଧାରିତ: {
			PX4_ERR("param_set invalid param type for %s", param_name(param));
			ଭାଙ୍ଗ;
		}
	}

	ଯଦି (user_config.store(param, new_ମୂଲ୍ୟ)) {
		ଯଦି (param_changed) {
			params_unsaved.set(param, !mark_saved);
		}

		result = PX4_OK;

	} ଅନ୍ୟଥା {
		PX4_ERR("param_set failed to store param %s", param_name(param));
		result = PX4_ERROR;
	}

	ଯଦି ((result == PX4_OK) && param_changed && !mark_saved && autosave) { // this is false when importing parameters
		param_autosave();
	}

	// If this is the parameter server, make sure that the remote is updated
#ଯଦି defined(CONFIG_PARAM_PRIMARY)

	ଯଦି (param_changed && update_remote) {
		param_primary_set_ମୂଲ୍ୟ(param, val);
	}

#endif

	// If this is the parameter remote, make sure that the primary is updated
#ଯଦି defined(CONFIG_PARAM_REMOTE)

	ଯଦି (param_changed && update_remote) {
		param_remote_set_ମୂଲ୍ୟ(param, val);
	}

#endif

	perf_end(param_set_perf);

	/*
	 * If we set something, now that we have unlocked, go ahead and advertise that
	 * a thing has been set.
	 */
	ଯଦି ((result == PX4_OK) && param_changed && notify_changes) {
		param_notify_changes();
	}

	ଫେରନ୍ତୁ result;
}

#ଯଦି defined(FLASH_BASED_PARAMS)
ପୂର୍ଣ୍ଣସଂଖ୍ୟା param_set_external(param_t param, ଅଚଳ ଶୂନ୍ୟ *val, ତାର୍କିକ mark_saved, ତାର୍କିକ notify_changes)
{
	ଫେରନ୍ତୁ param_set_internal(param, val, mark_saved, notify_changes);
}

ଶୂନ୍ୟ param_get_external(param_t param, ଶୂନ୍ୟ *val)
{
	param_get(param, val);
}
#endif

ପୂର୍ଣ୍ଣସଂଖ୍ୟା param_set(param_t param, ଅଚଳ ଶୂନ୍ୟ *val)
{
	ଫେରନ୍ତୁ param_set_internal(param, val, ମିଥ୍ୟା, ସତ୍ୟ);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା param_set_no_notification(param_t param, ଅଚଳ ଶୂନ୍ୟ *val)
{
	ଫେରନ୍ତୁ param_set_internal(param, val, ମିଥ୍ୟା, ମିଥ୍ୟା);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା param_set_no_remote_update(param_t param, ଅଚଳ ଶୂନ୍ୟ *val, ତାର୍କିକ notify)
{
	ଫେରନ୍ତୁ param_set_internal(param, val, ମିଥ୍ୟା, notify, ମିଥ୍ୟା);
}

ତାର୍କିକ param_used(param_t param)
{
	ଯଦି (handle_in_range(param)) {
		ଫେରନ୍ତୁ params_active[param];
	}

	ଫେରନ୍ତୁ ମିଥ୍ୟା;
}

ଶୂନ୍ୟ param_set_used(param_t param)
{
	ଯଦି (handle_in_range(param)) {
#ଯଦି defined(CONFIG_PARAM_REMOTE)

		ଯଦି (!param_used(param)) {
			param_remote_set_used(param);
		}

#endif

		params_active.set(param, ସତ୍ୟ);
	}
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା param_set_default_ମୂଲ୍ୟ(param_t param, ଅଚଳ ଶୂନ୍ୟ *val)
{
	ଯଦି (!handle_in_range(param)) {
		PX4_ERR("set default value invalid param %d", param);
		ଫେରନ୍ତୁ PX4_ERROR;
	}

	ଯଦି (val == nullptr) {
		PX4_ERR("set default value invalid value");
		ଫେରନ୍ତୁ PX4_ERROR;
	}

	ଯଦି (param_is_readonly(param)) {
		PX4_WARN("param %s is read-only", param_name(param));
		ଫେରନ୍ତୁ PX4_ERROR;
	}

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା result = PX4_ERROR;


	// check if param being set to default value
	ତାର୍କିକ setting_to_static_default = ମିଥ୍ୟା;
	ଅଚଳ param_ମୂଲ୍ୟ_u firmware_default_ମୂଲ୍ୟ = firmware_defaults.get(param);

	ବାଛ (param_type(param)) {
	ସ୍ଥିତି PARAM_TYPE_INT32: {
			int32_t new_ମୂଲ୍ୟ;
			memcpy(&new_ମୂଲ୍ୟ, val, ଆକାର(new_ମୂଲ୍ୟ));
			setting_to_static_default = firmware_default_ମୂଲ୍ୟ.i == new_ମୂଲ୍ୟ;
			ଭାଙ୍ଗ;
		}

	ସ୍ଥିତି PARAM_TYPE_FLOAT: {
			ଦଶମିକ new_ମୂଲ୍ୟ;
			memcpy(&new_ମୂଲ୍ୟ, val, ଆକାର(new_ମୂଲ୍ୟ));
			setting_to_static_default = fabsf(firmware_default_ମୂଲ୍ୟ.f - new_ମୂଲ୍ୟ) <= FLT_EPSILON;
			ଭାଙ୍ଗ;
		}
	}

	ଯଦି (setting_to_static_default) {
		runtime_defaults.reset(param);

		result = PX4_OK;

	} ଅନ୍ୟଥା {
		param_ମୂଲ୍ୟ_u new_ମୂଲ୍ୟ{};

		ବାଛ (param_type(param)) {
		ସ୍ଥିତି PARAM_TYPE_INT32: {
				memcpy(&new_ମୂଲ୍ୟ.i, val, ଆକାର(new_ମୂଲ୍ୟ.i));
				ଭାଙ୍ଗ;
			}

		ସ୍ଥିତି PARAM_TYPE_FLOAT: {
				memcpy(&new_ମୂଲ୍ୟ.f, val, ଆକାର(new_ମୂଲ୍ୟ.f));
				ଭାଙ୍ଗ;
			}

		ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
			ଭାଙ୍ଗ;
		}

		ଯଦି (runtime_defaults.store(param, new_ମୂଲ୍ୟ)) {
			user_config.refresh(param);
			result = PX4_OK;

		} ଅନ୍ୟଥା {
			result = PX4_ERROR;
		}
	}


	ଯଦି ((result == PX4_OK) && param_used(param)) {
		// send notification if param is already in use
		param_notify_changes();
	}

	ଫେରନ୍ତୁ result;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା param_reset_internal(param_t param, ତାର୍କିକ notify = ସତ୍ୟ, ତାର୍କିକ autosave = ସତ୍ୟ)
{
#ଯଦି defined(CONFIG_PARAM_REMOTE)
	// Remote doesn't support reset
	ଫେରନ୍ତୁ ମିଥ୍ୟା;
#endif

	ଯଦି (param_is_readonly(param)) {
		ଫେରନ୍ତୁ 0;  // silently skip — param_reset_all loops over all params
	}

	ତାର୍କିକ param_found = user_config.contains(param);

	ଯଦି (handle_in_range(param)) {
		user_config.reset(param);
	}

	ଯଦି (param_found) {
		/* Persist the reset. Flash-param saves append only unsaved params,
		 * so a reset has to show up as a tombstone or replay keeps the
		 * previous override. */
		params_unsaved.set(param, ସତ୍ୟ);
	}

	ଯଦି (autosave) {
		param_autosave();
	}

	ଯଦି (param_found && notify) {
		param_notify_changes();
	}

#ଯଦି defined(CONFIG_PARAM_PRIMARY)
	param_primary_reset(param);
#endif

	ଫେରନ୍ତୁ param_found;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା param_reset(param_t param) { ଫେରନ୍ତୁ param_reset_internal(param, ସତ୍ୟ); }
ପୂର୍ଣ୍ଣସଂଖ୍ୟା param_reset_no_notification(param_t param) { ଫେରନ୍ତୁ param_reset_internal(param, ମିଥ୍ୟା); }

ସ୍ଥିର ଶୂନ୍ୟ
param_reset_all_internal(ତାର୍କିକ auto_save)
{
#ଯଦି defined(CONFIG_PARAM_REMOTE)
	// Remote doesn't support reset
	ଫେରନ୍ତୁ;
#endif

	ପାଇଁ (param_t param = 0; handle_in_range(param); param++) {
		param_reset_internal(param, ମିଥ୍ୟା, ମିଥ୍ୟା);
	}

	ଯଦି (auto_save) {
		param_autosave();
	}

#ଯଦି defined(CONFIG_PARAM_PRIMARY)
	param_primary_reset_all();
#endif

	param_notify_changes();
}

ଶୂନ୍ୟ
param_reset_all()
{
	param_reset_all_internal(ସତ୍ୟ);
}

ଶୂନ୍ୟ
param_reset_excludes(ଅଚଳ ଅକ୍ଷର *excludes[], ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ_excludes)
{
	ପାଇଁ (param_t param = 0; handle_in_range(param); param++) {
		ଅଚଳ ଅକ୍ଷର *name = param_name(param);
		ତାର୍କିକ exclude = ମିଥ୍ୟା;

		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା index = 0; index < ଅଙ୍କ_excludes; index ++) {
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା len = strlen(excludes[index]);

			ଯଦି ((excludes[index][len - 1] == '*'
			     && strncmp(name, excludes[index], len - 1) == 0)
			    || strcmp(name, excludes[index]) == 0) {

				exclude = ସତ୍ୟ;
				ଭାଙ୍ଗ;
			}
		}

		ଯଦି (!exclude) {
			param_reset(param);
		}
	}
}

ଶୂନ୍ୟ
param_reset_specific(ଅଚଳ ଅକ୍ଷର *resets[], ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ_resets)
{
	ପାଇଁ (param_t param = 0; handle_in_range(param); param++) {
		ଅଚଳ ଅକ୍ଷର *name = param_name(param);
		ତାର୍କିକ reset = ମିଥ୍ୟା;

		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା index = 0; index < ଅଙ୍କ_resets; index++) {
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା len = strlen(resets[index]);

			ଯଦି ((resets[index][len - 1] == '*'
			     && strncmp(name, resets[index], len - 1) == 0)
			    || strcmp(name, resets[index]) == 0) {

				reset = ସତ୍ୟ;
				ଭାଙ୍ଗ;
			}
		}

		ଯଦି (reset) {
			param_reset(param);
		}
	}
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା
param_set_default_file(ଅଚଳ ଅକ୍ଷର *filename)
{
	ଯଦି (filename && param_backup_file && strcmp(filename, param_backup_file) == 0) {
		PX4_ERR("default file can't be the same as the backup file %s", filename);
		ଫେରନ୍ତୁ PX4_ERROR;
	}

#ifdef FLASH_BASED_PARAMS
	// the default for flash-based params is always the FLASH
	(ଶୂନ୍ୟ)filename;
#ଅନ୍ୟଥା

	ଯଦି (param_default_file != nullptr) {
		// we assume this is not in use by some other thread
		ମୁକ୍ତ(param_default_file);
		param_default_file = nullptr;
	}

	ଯଦି (filename) {
		param_default_file = strdup(filename);
	}

#endif /* FLASH_BASED_PARAMS */

	ଫେରନ୍ତୁ 0;
}

ଅଚଳ ଅକ୍ଷର *param_get_default_file()
{
	ଫେରନ୍ତୁ param_default_file;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା param_set_backup_file(ଅଚଳ ଅକ୍ଷର *filename)
{
	ଯଦି (filename && param_default_file && strcmp(filename, param_default_file) == 0) {
		PX4_ERR("backup file can't be the same as the default file %s", filename);
		ଫେରନ୍ତୁ PX4_ERROR;
	}

	ଯଦି (param_backup_file != nullptr) {
		// we assume this is not in use by some other thread
		ମୁକ୍ତ(param_backup_file);
		param_backup_file = nullptr;
	}

	ଯଦି (filename) {
		param_backup_file = strdup(filename);

	} ଅନ୍ୟଥା {
		param_backup_file = nullptr; // backup disabled
	}

	ଫେରନ୍ତୁ 0;
}

ଅଚଳ ଅକ୍ଷର *param_get_backup_file()
{
	ଫେରନ୍ତୁ param_backup_file;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା param_export_internal(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, param_filter_func filter);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା param_verify(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା param_save_default(ତାର୍କିକ blocking)
{
	PX4_DEBUG("param_save_default");

	// take the file lock
	ଯଦି (blocking) {
		pthread_mutex_lock(&file_mutex);

	} ଅନ୍ୟଥା {
		ଯଦି (pthread_mutex_trylock(&file_mutex) != 0) {
			PX4_DEBUG("param_save_default: file lock failed (already locked)");
			ଫେରନ୍ତୁ -EWOULDBLOCK;
		}
	}

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା shutdown_lock_ret = px4_shutdown_lock();

	ଯଦି (shutdown_lock_ret != 0) {
		PX4_ERR("px4_shutdown_lock() failed (%i)", shutdown_lock_ret);
	}

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା res = PX4_ERROR;
	ଅଚଳ ଅକ୍ଷର *filename = param_get_default_file();

	ପାଇଁ (param_t param = 0; handle_in_range(param); param++) {
		ଯଦି (params_unsaved[param]) {
			params_saving.set(param, ସତ୍ୟ);
			params_unsaved.set(param, ମିଥ୍ୟା);
		}
	}

	ଯଦି (filename) {
		ସ୍ଥିର constexpr ପୂର୍ଣ୍ଣସଂଖ୍ୟା MAX_ATTEMPTS = 3;

		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା attempt = 1; attempt <= MAX_ATTEMPTS; attempt++) {
			// write parameters to file
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd = ::open(filename, O_WRONLY | O_CREAT | O_TRUNC, PX4_O_MODE_666);

			ଯଦି (fd > -1) {
				perf_begin(param_export_perf);
				res = param_export_internal(fd, nullptr);
				perf_end(param_export_perf);
				::close(fd);

				ଯଦି (res == PX4_OK) {
					// reopen file to verify
					ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd_verify = ::open(filename, O_RDONLY, PX4_O_MODE_666);
					res = param_verify(fd_verify) || lseek(fd_verify, 0, SEEK_SET) || param_verify(fd_verify);
					::close(fd_verify);
				}
			}

			ଯଦି (res == PX4_OK) {
				ଭାଙ୍ଗ;

			} ଅନ୍ୟଥା {
				PX4_ERR("parameter export to %s failed (%d) attempt %d", filename, res, attempt);
				px4_usleep(10000); // wait at least 10 milliseconds before trying again
			}
		}

	} ଅନ୍ୟଥା {
		perf_begin(param_export_perf);
		res = flash_param_save(nullptr);
		perf_end(param_export_perf);
	}

	ଯଦି (res != PX4_OK) {
		PX4_ERR("param export failed (%d)", res);

		ପାଇଁ (param_t param = 0; handle_in_range(param); param++) {
			ଯଦି (params_saving[param]) {
				params_unsaved.set(param, ସତ୍ୟ);
			}
		}
	}

	params_saving.reset();

	ଯଦି (res == PX4_OK) {
		// backup file
		ଯଦି (param_backup_file) {
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd_backup_file = ::open(param_backup_file, O_WRONLY | O_CREAT | O_TRUNC, PX4_O_MODE_666);

			ଯଦି (fd_backup_file > -1) {
				ପୂର୍ଣ୍ଣସଂଖ୍ୟା backup_export_ret = param_export_internal(fd_backup_file, nullptr);
				::close(fd_backup_file);

				ଯଦି (backup_export_ret != 0) {
					PX4_ERR("backup parameter export to %s failed (%d)", param_backup_file, backup_export_ret);

				} ଅନ୍ୟଥା {
					// verify export
					ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd_verify = ::open(param_backup_file, O_RDONLY, PX4_O_MODE_666);
					param_verify(fd_verify);
					::close(fd_verify);
				}
			}
		}
	}

	pthread_mutex_unlock(&file_mutex);

	ଯଦି (shutdown_lock_ret == 0) {
		px4_shutdown_unlock();
	}

	ଫେରନ୍ତୁ res;
}

/**
 * @return 0 on success, 1 if all params have not yet been stored, -1 if device open failed, -2 if writing parameters failed
 */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା
param_load_default()
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା res = 0;
	ଅଚଳ ଅକ୍ଷର *filename = param_get_default_file();

	ଯଦି (!filename) {
		ଫେରନ୍ତୁ param_load(-1);
	}

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd_load = ::open(filename, O_RDONLY);

	ଯଦି (fd_load < 0) {
		/* no parameter file is OK, otherwise this is an error */
		ଯଦି (errno != ENOENT) {
			PX4_ERR("open '%s' for reading failed", filename);
			ଫେରନ୍ତୁ -1;
		}

		ଫେରନ୍ତୁ 1;
	}

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା result = param_load(fd_load);
	::close(fd_load);

	ଯଦି (result == 1) {
		/* blank file: nothing stored yet */
		ଫେରନ୍ତୁ 1;
	}

	ଯଦି (result != 0) {
		PX4_ERR("error reading parameters from '%s'", filename);
		ଫେରନ୍ତୁ -2;
	}

	ଫେରନ୍ତୁ res;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା param_verify_callback(bson_decoder_t decoder, bson_ଗଣ୍ଠି_t ଗଣ୍ଠି_2)
{
	ଯଦି (ଗଣ୍ଠି_2->type == BSON_EOO) {
		ଫେରନ୍ତୁ 0;
	}

	// find the parameter this node represents
	param_t param = param_find_no_notification(ଗଣ୍ଠି_2->name);

	ଯଦି (param == PARAM_INVALID) {
		PX4_ERR("verify: invalid parameter '%s'", ଗଣ୍ଠି_2->name);
		ଫେରନ୍ତୁ -1;
	}

	// handle verifying the parameter from the node
	ବାଛ (ଗଣ୍ଠି_2->type) {
	ସ୍ଥିତି BSON_INT32: {
			ଯଦି (param_type(param) != PARAM_TYPE_INT32) {
				PX4_ERR("verify: invalid param type %d for '%s' (BSON_INT32)", param_type(param), ଗଣ୍ଠି_2->name);
				ଫେରନ୍ତୁ -1;
			}

			int32_t value;

			ଯଦି (param_get(param, &value) == 0) {
				ଯଦି (value == ଗଣ୍ଠି_2->i32) {
					ଫେରନ୍ତୁ 1; // valid

				} ଅନ୍ୟଥା {
					PX4_ERR("verify: '%s' invalid BSON value %" PRIi32 "(expected %" PRIi32 ")", ଗଣ୍ଠି_2->name, ଗଣ୍ଠି_2->i32, value);
				}
			}
		}
		ଭାଙ୍ଗ;

	ସ୍ଥିତି BSON_DOUBLE: {
			ଯଦି (param_type(param) != PARAM_TYPE_FLOAT) {
				PX4_ERR("verify: invalid param type %d for '%s' (BSON_DOUBLE)", param_type(param), ଗଣ୍ଠି_2->name);
				ଫେରନ୍ତୁ -1;
			}

			ଦଶମିକ value;

			ଯଦି (param_get(param, &value) == 0) {
				ଯଦି (fabsf(value - (ଦଶମିକ)ଗଣ୍ଠି_2->d) <= FLT_EPSILON) {
					ଫେରନ୍ତୁ 1; // valid

				} ଅନ୍ୟଥା {
					PX4_ERR("verify: '%s' invalid BSON value %.3f (expected %.3f)", ଗଣ୍ଠି_2->name, ଗଣ୍ଠି_2->d, (ଦ୍ୱିଗୁଣ)value);
				}
			}
		}
		ଭାଙ୍ଗ;

	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		PX4_ERR("verify: '%s' invalid node type %d", ଗଣ୍ଠି_2->name, ଗଣ୍ଠି_2->type);
	}

	ଫେରନ୍ତୁ -1;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା param_verify(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd)
{
	PX4_DEBUG("param_verify");

	ଯଦି (fd < 0) {
		ଫେରନ୍ତୁ -1;
	}

	ଯଦି (lseek(fd, 0, SEEK_SET) != 0) {
		PX4_ERR("verify: seek failed");
		ଫେରନ୍ତୁ -1;
	}

	bson_decoder_s decoder{};

	ଯଦି (bson_decoder_init_file(&decoder, fd, param_verify_callback) == 0) {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା result = -1;

		କର {
			result = bson_decoder_next(&decoder);

		} ଯେତେବେଳେ (result > 0);

		ଯଦି (result == 0) {
			ଯଦି (decoder.total_document_size != decoder.total_decoded_size) {
				PX4_ERR("BSON document size (%" PRId32 ") doesn't match bytes decoded (%" PRId32 ")", decoder.total_document_size,
					decoder.total_decoded_size);

			} ଅନ୍ୟଥା {
				ଫେରନ୍ତୁ 0;
			}

		} ଅନ୍ୟଥା ଯଦି (result == -ENODATA) {
			PX4_ERR("verify: no BSON data");

		} ଅନ୍ୟଥା {
			PX4_ERR("verify: failed (%d)", result);
		}
	}

	ଫେରନ୍ତୁ -1;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା
param_export(ଅଚଳ ଅକ୍ଷର *filename, param_filter_func filter)
{
	PX4_DEBUG("param_export");

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା shutdown_lock_ret = px4_shutdown_lock();

	ଯଦି (shutdown_lock_ret != 0) {
		PX4_ERR("px4_shutdown_lock() failed (%i)", shutdown_lock_ret);
	}

	// take the file lock
	ଯଦି (pthread_mutex_trylock(&file_mutex) != 0) {
		PX4_ERR("param_export: file lock failed (already locked)");
		ଫେରନ୍ତୁ PX4_ERROR;
	}

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd = ::open(filename, O_RDWR | O_CREAT, PX4_O_MODE_666);
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା result = PX4_ERROR;

	perf_begin(param_export_perf);

	ଯଦି (fd > -1) {
		result = param_export_internal(fd, filter);

	} ଅନ୍ୟଥା {
		result = flash_param_save(filter);
	}

	perf_end(param_export_perf);

	pthread_mutex_unlock(&file_mutex);

	ଯଦି (shutdown_lock_ret == 0) {
		px4_shutdown_unlock();
	}

	ଫେରନ୍ତୁ result;
}

// internal parameter export, caller is responsible for locking
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା param_export_internal(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, param_filter_func filter)
{
	PX4_DEBUG("param_export_internal");
	ଅଚଳ auto changed_params = user_config.containedAsBitset();

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା result = -1;
	bson_encoder_s encoder{};
	uint8_t bson_buffer[256];

	ଯଦି (lseek(fd, 0, SEEK_SET) != 0) {
		PX4_ERR("export seek failed %d", errno);
		ଫେରନ୍ତୁ -1;
	}

	ଯଦି (bson_encoder_init_buf_file(&encoder, fd, &bson_buffer, ଆକାର(bson_buffer)) != 0) {
		ଯାଅ out;
	}

	ପାଇଁ (param_t param = 0; handle_in_range(param); param++) {
		ଯଦି (!changed_params[param] || (filter && !filter(param))) {
			ଜାରି;
		}

		ଅଚଳ param_ମୂଲ୍ୟ_u runtime_default_ମୂଲ୍ୟ = runtime_defaults.get(param);
		ଅଚଳ param_ମୂଲ୍ୟ_u user_config_ମୂଲ୍ୟ = user_config.get(param);

		// don't export default values
		ବାଛ (param_type(param)) {
		ସ୍ଥିତି PARAM_TYPE_INT32:
			ଯଦି (user_config_ମୂଲ୍ୟ.i == runtime_default_ମୂଲ୍ୟ.i) {
				PX4_DEBUG("skipping %s %" PRIi32 " export", param_name(param), runtime_default_ମୂଲ୍ୟ.i);
				ଜାରି;
			}

			ଭାଙ୍ଗ;

		ସ୍ଥିତି PARAM_TYPE_FLOAT:
			ଯଦି (fabsf(user_config_ମୂଲ୍ୟ.f - runtime_default_ମୂଲ୍ୟ.f) <= FLT_EPSILON) {
				PX4_DEBUG("skipping %s %.3f export", param_name(param), (ଦ୍ୱିଗୁଣ)runtime_default_ମୂଲ୍ୟ.f);
				ଜାରି;
			}

			ଭାଙ୍ଗ;
		}

		ଅଚଳ ଅକ୍ଷର *name = param_name(param);
		ଅଚଳ size_t size = param_size(param);

		/* append the appropriate BSON type object */
		ବାଛ (param_type(param)) {
		ସ୍ଥିତି PARAM_TYPE_INT32: {
				ଅଚଳ int32_t i = user_config_ମୂଲ୍ୟ.i;
				PX4_DEBUG("exporting: %s (%d) size: %lu val: %" PRIi32, name, param, (long unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)size, i);

				ଯଦି (bson_encoder_append_int32(&encoder, name, i) != 0) {
					PX4_ERR("BSON append failed for '%s'", name);
					ଯାଅ out;
				}
			}
			ଭାଙ୍ଗ;

		ସ୍ଥିତି PARAM_TYPE_FLOAT: {
				ଅଚଳ ଦ୍ୱିଗୁଣ f = (ଦ୍ୱିଗୁଣ)user_config_ମୂଲ୍ୟ.f;
				PX4_DEBUG("exporting: %s (%d) size: %lu val: %.3f", name, param, (long unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)size, (ଦ୍ୱିଗୁଣ)f);

				ଯଦି (bson_encoder_append_double(&encoder, name, f) != 0) {
					PX4_ERR("BSON append failed for '%s'", name);
					ଯାଅ out;
				}
			}
			ଭାଙ୍ଗ;

		ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
			PX4_ERR("%s unrecognized parameter type %d, skipping export", name, param_type(param));
		}
	}

	result = 0;

out:

	ଯଦି (result == 0) {
		ଯଦି (bson_encoder_fini(&encoder) != PX4_OK) {
			PX4_ERR("BSON encoder finalize failed");
			result = -1;
		}
	}

	ଫେରନ୍ତୁ result;
}

#ଯଦି defined(FLASH_BASED_PARAMS)
// A file is never the flash backend's store, so what an import changes is unsaved until a delta carries it.
// As on the file backend, the import itself does not save.
ସ୍ଥିର constexpr ତାର୍କିକ file_import_is_saved = ମିଥ୍ୟା;
#ଅନ୍ୟଥା
ସ୍ଥିର constexpr ତାର୍କିକ file_import_is_saved = ସତ୍ୟ;
#endif

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
param_import_callback(bson_decoder_t decoder, bson_ଗଣ୍ଠି_t ଗଣ୍ଠି_2)
{
	/*
	 * EOO means the end of the parameter object. (Currently not supporting
	 * nested BSON objects).
	 */
	ଯଦି (ଗଣ୍ଠି_2->type == BSON_EOO) {
		PX4_DEBUG("end of parameters");
		ଫେରନ୍ତୁ 0;
	}

	// A tombstone has no value. The decoder leaves the previous node's in the union and the translations
	// read it without a type check.
	ଅଚଳ ତାର୍କିକ tombstone = ଗଣ୍ଠି_2->type == BSON_nullptr || ଗଣ୍ଠି_2->type == BSON_UNDEFINED;

	// if we do param_set() directly in the translation, set PARAM_SKIP_IMPORT as return value and return here
	ଯଦି (!tombstone && param_modify_on_import(ଗଣ୍ଠି_2) == param_modify_on_import_ret::PARAM_SKIP_IMPORT) {
		ଫେରନ୍ତୁ 1;
	}

	// Find the parameter this node represents.  If we don't know it, ignore the node.
	param_t param = param_find_no_notification(ଗଣ୍ଠି_2->name);

	ଯଦି (param == PARAM_INVALID) {
		PX4_WARN("ignoring unrecognised parameter '%s'", ଗଣ୍ଠି_2->name);
		ଫେରନ୍ତୁ 1;
	}

	// Handle setting the parameter from the node
	ବାଛ (ଗଣ୍ଠି_2->type) {
	ସ୍ଥିତି BSON_nullptr:
	ସ୍ଥିତି BSON_UNDEFINED:
		param_reset_internal(param, ସତ୍ୟ, ମିଥ୍ୟା);
		ଫେରନ୍ତୁ 1;

	ସ୍ଥିତି BSON_INT32: {
			ଯଦି (param_type(param) == PARAM_TYPE_INT32) {
				int32_t i = ଗଣ୍ଠି_2->i32;
				param_set_internal(param, &i, file_import_is_saved, ସତ୍ୟ, ସତ୍ୟ, ମିଥ୍ୟା);
				PX4_DEBUG("Imported %s with value %" PRIi32, param_name(param), i);

			} ଅନ୍ୟଥା {
				PX4_WARN("unexpected type for %s", ଗଣ୍ଠି_2->name);
			}
		}
		ଭାଙ୍ଗ;

	ସ୍ଥିତି BSON_DOUBLE: {
			ଯଦି (param_type(param) == PARAM_TYPE_FLOAT) {
				ଦଶମିକ f = ଗଣ୍ଠି_2->d;
				param_set_internal(param, &f, file_import_is_saved, ସତ୍ୟ, ସତ୍ୟ, ମିଥ୍ୟା);
				PX4_DEBUG("Imported %s with value %f", param_name(param), (ଦ୍ୱିଗୁଣ)f);

			} ଅନ୍ୟଥା {
				PX4_WARN("unexpected type for %s", ଗଣ୍ଠି_2->name);
			}
		}
		ଭାଙ୍ଗ;

	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		PX4_ERR("import: unrecognised node type for '%s'", ଗଣ୍ଠି_2->name);
	}

	// don't return zero, that means EOF
	ଫେରନ୍ତୁ 1;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
param_import_internal(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd)
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
	ସଂରଚନା stat st;

	ଯଦି (fstat(fd, &st) == 0 && S_ISREG(st.st_mode) && st.st_size == 0) {
		ଫେରନ୍ତୁ 1;
	}

	int32_t doc_size = 0;
	ssize_t len = ପଠ(fd, &doc_size, ଆକାର(doc_size));

	ଯଦି (len == 0 || (len == (ssize_t)ଆକାର(doc_size) && (doc_size == 0 || doc_size == -1))) {
		ଫେରନ୍ତୁ 1;
	}

	ଯଦି (lseek(fd, 0, SEEK_SET) != 0) {
		PX4_ERR("import lseek failed (%d)", errno);
		ଫେରନ୍ତୁ -1;
	}

	ସ୍ଥିର constexpr ପୂର୍ଣ୍ଣସଂଖ୍ୟା MAX_ATTEMPTS = 3;

	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା attempt = 1; attempt <= MAX_ATTEMPTS; attempt++) {
		bson_decoder_s decoder{};

		ଯଦି (bson_decoder_init_file(&decoder, fd, param_import_callback) == 0) {
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା result = -1;

			କର {
				result = bson_decoder_next(&decoder);

			} ଯେତେବେଳେ (result > 0);

			ଯଦି (result == 0) {
				ଯଦି (decoder.total_document_size == decoder.total_decoded_size) {
					PX4_INFO("BSON document size %" PRId32 " bytes, decoded %" PRId32 " bytes (INT32:%" PRIu16 ", FLOAT:%" PRIu16 ")",
						 decoder.total_document_size, decoder.total_decoded_size,
						 decoder.ଗଣନା_ଗଣ୍ଠି_int32, decoder.ଗଣନା_ଗଣ୍ଠି_double);

					ଫେରନ୍ତୁ 0;

				} ଅନ୍ୟଥା {
					PX4_ERR("BSON document size (%" PRId32 ") doesn't match bytes decoded (%" PRId32 ")",
						decoder.total_document_size, decoder.total_decoded_size);
				}

			} ଅନ୍ୟଥା ଯଦି (result == -ENODATA) {
				// silently retry as a precaution unless this is our last attempt
				ଯଦି (attempt == MAX_ATTEMPTS) {
					PX4_DEBUG("BSON: no data");
					ଫେରନ୍ତୁ 0;
				}

			} ଅନ୍ୟଥା {
				PX4_ERR("param import failed (%d) attempt %d", result, attempt);
			}

		} ଅନ୍ୟଥା {
			PX4_ERR("param import bson decoder init failed (attempt %d)", attempt);
		}

		ଯଦି (attempt != MAX_ATTEMPTS) {
			ଯଦି (lseek(fd, 0, SEEK_SET) != 0) {
				PX4_ERR("import lseek failed (%d)", errno);
			}

			px4_usleep(10000); // wait at least 10 milliseconds before trying again
		}
	}

	ଫେରନ୍ତୁ -1;
}

#ଯଦି defined(FLASH_BASED_PARAMS)
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା flash_backend_locked(ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*op)())
{
	/* Same order as param_save_default: mutex then shutdown lock. Autosave
	 * uses a trylock, so a compact holds it off the flash programming. */
	pthread_mutex_lock(&file_mutex);

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା shutdown_lock_ret = px4_shutdown_lock();

	ଯଦି (shutdown_lock_ret != 0) {
		PX4_ERR("px4_shutdown_lock() failed (%i)", shutdown_lock_ret);
	}

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା result = op();

	pthread_mutex_unlock(&file_mutex);

	ଯଦି (shutdown_lock_ret == 0) {
		px4_shutdown_unlock();
	}

	ଫେରନ୍ତୁ result;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା flash_load_locked()
{
	/* Reset under the lock so a pending autosave cannot append tombstones
	 * of the just-cleared RAM before import/compact run. */
	param_reset_all_internal(ମିଥ୍ୟା);
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା result = flash_param_import();

	ଯଦି (result == 0 || result == 1) {
		params_unsaved.reset();
	}

	ଫେରନ୍ତୁ result;
}
#endif

ପୂର୍ଣ୍ଣସଂଖ୍ୟା
param_import(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd)
{
	ଯଦି (fd < 0) {
#ଯଦି defined(FLASH_BASED_PARAMS)
		ଫେରନ୍ତୁ flash_backend_locked(flash_param_import);
#ଅନ୍ୟଥା
		ଫେରନ୍ତୁ flash_param_import();
#endif
	}

	ଫେରନ୍ତୁ param_import_internal(fd);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା
param_load(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd)
{
	ଯଦି (fd < 0) {
#ଯଦି defined(FLASH_BASED_PARAMS)
		ଫେରନ୍ତୁ flash_backend_locked(flash_load_locked);
#ଅନ୍ୟଥା
		ଫେରନ୍ତୁ PX4_ERROR;
#endif
	}

	param_reset_all_internal(ମିଥ୍ୟା);
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା result = param_import_internal(fd);

#ଯଦି !defined(FLASH_BASED_PARAMS)

	ଯଦି (result >= 0) {
		params_unsaved.reset();
	}

#endif

	ଫେରନ୍ତୁ result;
}

ଶୂନ୍ୟ
param_foreach(ଶୂନ୍ୟ (*func)(ଶୂନ୍ୟ *arg, param_t param), ଶୂନ୍ୟ *arg, ତାର୍କିକ only_changed, ତାର୍କିକ only_used)
{
	param_t	param;

	ପାଇଁ (param = 0; handle_in_range(param); param++) {

		/* if requested, skip unchanged values */
		ଯଦି (only_changed && (!user_config.contains(param))) {
			ଜାରି;
		}

		ଯଦି (only_used && !param_used(param)) {
			ଜାରି;
		}

		func(arg, param);
	}
}

uint32_t param_hash_check()
{
	uint32_t param_hash = 0;

	/* compute the CRC32 over all string param names and 4 byte values */
	ପାଇଁ (param_t param = 0; handle_in_range(param); param++) {
		ଯଦି (!param_used(param) || param_is_volatile(param)) {
			ଜାରି;
		}

		ଅଚଳ ଅକ୍ଷର *name = param_name(param);
		auto value = user_config.get(param).i;
		ଅଚଳ ଶୂନ୍ୟ *val = (ଶୂନ୍ୟ *)&value;
		param_hash = crc32part((ଅଚଳ uint8_t *)name, strlen(name), param_hash);
		param_hash = crc32part((ଅଚଳ uint8_t *)val, param_size(param), param_hash);
	}

	ଫେରନ୍ତୁ param_hash;
}

ଶୂନ୍ୟ param_print_status()
{
	PX4_INFO("summary: %d/%d (used/total)", param_ଗଣନା_used(), param_ଗଣନା());

#ifndef FLASH_BASED_PARAMS
	ଅଚଳ ଅକ୍ଷର *filename = param_get_default_file();

	ଯଦି (filename != nullptr) {
		PX4_INFO("file: %s", param_get_default_file());
	}

	ଯଦି (param_backup_file) {
		PX4_INFO("backup file: %s", param_backup_file);
	}

#endif /* FLASH_BASED_PARAMS */

	PX4_INFO("storage array: %d/%d elements (%zu bytes total)",
		 user_config.size(), firmware_defaults.size(), (size_t)user_config.byteSize());


	PX4_INFO("storage array (custom defaults): %d/%d elements (%zu bytes total)",
		 runtime_defaults.size(), firmware_defaults.size(), (size_t)runtime_defaults.byteSize());

	ଯଦି (autosave_instance) {
		PX4_INFO("auto save: %s", autosave_instance->enabled() ? "on" : "off");

		hrt_abstime last_autosave = autosave_instance->lastAutosave();

		ଯଦି (last_autosave > 0) {
			PX4_INFO("last auto save: %.3f seconds ago", hrt_elapsed_time(&last_autosave) * 1e-6);
		}
	}

	perf_print_counter(param_export_perf);
	perf_print_counter(param_find_perf);
	perf_print_counter(param_get_perf);
	perf_print_counter(param_set_perf);

#ଯଦି defined(CONFIG_PARAM_PRIMARY)
	ସଂରଚନା param_primary_counters counts;
	param_primary_get_counters(&counts);
	PX4_INFO("set value requests received: %" PRIu32 ", set value responses sent: %" PRIu32,
		 counts.set_ମୂଲ୍ୟ_request_received, counts.set_ମୂଲ୍ୟ_response_sent);
	PX4_INFO("set value requests sent: %" PRIu32 ", set value responses received: %" PRIu32,
		 counts.set_ମୂଲ୍ୟ_request_sent, counts.set_ମୂଲ୍ୟ_response_received);
	PX4_INFO("resets sent: %" PRIu32 ", set used requests received: %" PRIu32,
		 counts.reset_sent, counts.set_used_received);
#endif

#ଯଦି defined(CONFIG_PARAM_REMOTE)
	ସଂରଚନା param_remote_counters counts;
	param_remote_get_counters(&counts);
	PX4_INFO("set value requests received: %" PRIu32 ", set value responses sent: %" PRIu32,
		 counts.set_ମୂଲ୍ୟ_request_received, counts.set_ମୂଲ୍ୟ_response_sent);
	PX4_INFO("set value requests sent: %" PRIu32 ", set value responses received: %" PRIu32,
		 counts.set_ମୂଲ୍ୟ_request_sent, counts.set_ମୂଲ୍ୟ_response_received);
	PX4_INFO("resets received: %" PRIu32 ", set used requests sent: %" PRIu32,
		 counts.reset_received, counts.set_used_sent);
#endif

}
