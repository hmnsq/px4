/****************************************************************************
 *
 *   Copyright (c) 2019 PX4 Development Team. All rights reserved.
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

#pragma once

#ଅନ୍ତର୍ଭାଵ "actuator_test.hpp"

#ଅନ୍ତର୍ଭାଵ "functions/FunctionActuatorSet.hpp"
#ଅନ୍ତର୍ଭାଵ "functions/FunctionConstantMax.hpp"
#ଅନ୍ତର୍ଭାଵ "functions/FunctionConstantMin.hpp"
#ଅନ୍ତର୍ଭାଵ "functions/FunctionGimbal.hpp"
#ଅନ୍ତର୍ଭାଵ "functions/FunctionGripper.hpp"
#ଅନ୍ତର୍ଭାଵ "functions/FunctionICEngineControl.hpp"
#ଅନ୍ତର୍ଭାଵ "functions/FunctionLandingGear.hpp"
#ଅନ୍ତର୍ଭାଵ "functions/FunctionLandingGearWheel.hpp"
#ଅନ୍ତର୍ଭାଵ "functions/FunctionManualRC.hpp"
#ଅନ୍ତର୍ଭାଵ "functions/FunctionMotors.hpp"
#ଅନ୍ତର୍ଭାଵ "functions/FunctionParachute.hpp"
#ଅନ୍ତର୍ଭାଵ "functions/FunctionServos.hpp"

#ଅନ୍ତର୍ଭାଵ <board_config.h>
#ଅନ୍ତର୍ଭାଵ <drivers/drv_pwm_output.h>
#ଅନ୍ତର୍ଭାଵ <lib/perf/perf_counter.h>
#ଅନ୍ତର୍ଭାଵ <px4_platform_common/module_params.h>
#ଅନ୍ତର୍ଭାଵ <px4_platform_common/px4_work_queue/ScheduledWorkItem.hpp>
#ଅନ୍ତର୍ଭାଵ <uORB/Publication.hpp>
#ଅନ୍ତର୍ଭାଵ <uORB/PublicationMulti.hpp>
#ଅନ୍ତର୍ଭାଵ <uORB/Subscription.hpp>
#ଅନ୍ତର୍ଭାଵ <uORB/SubscriptionCallback.hpp>
#ଅନ୍ତର୍ଭାଵ <uORB/topics/actuator_armed.h>
#ଅନ୍ତର୍ଭାଵ <uORB/topics/actuator_outputs.h>
#ଅନ୍ତର୍ଭାଵ <uORB/topics/parameter_update.h>

using ନାମାକାଶ time_literals;

/**
 * @class OutputModuleInterface
 * Base class for an output module.
 */
ଶ୍ରେଣୀ OutputModuleInterface : ସର୍ବସାଧାରଣ px4::ScheduledWorkItem, ସର୍ବସାଧାରଣ ModuleParams
{
ସର୍ବସାଧାରଣ:
	ସ୍ଥିର constexpr ପୂର୍ଣ୍ଣସଂଖ୍ୟା MAX_ACTUATORS = PWM_OUTPUT_MAX_CHANNELS;

	OutputModuleInterface(ଅଚଳ ଅକ୍ଷର *name, ଅଚଳ px4::wq_config_t &config) :
		px4::ScheduledWorkItem(name, config),
		ModuleParams(nullptr)
	{}

	/**
	 * Callback to update the (physical) actuator outputs in the driver
	 * @param outputs individual actuator outputs in range [min, max] or failsafe/disarmed value
	 * @param num_outputs number of outputs (<= max_num_outputs)
	 * @param num_control_groups_updated number of actuator_control groups updated
	 * @return if true, the update got handled, and actuator_outputs can be published
	 */
	ଆଭାସୀ ତାର୍କିକ updateOutputs(ଦଶମିକ outputs[MAX_ACTUATORS], unsigned ଅଙ୍କ_outputs, unsigned ଅଙ୍କ_control_groups_updated) = 0;

	/** called whenever the mixer gets updated/reset */
	ଆଭାସୀ ଶୂନ୍ୟ mixerChanged() {}

	/**
	 * Called when the reversible output mask changes, before the new mask is used to compute outputs.
	 * Drivers whose output range depends on reversibility must update it here, not from their Run().
	 */
	ଆଭାସୀ ଶୂନ୍ୟ reversibleMaskChanged(uint32_t reversible_mask) {}
};

/**
 * @class MixingOutput
 * This handles the mixing, arming/disarming and all subscriptions required for that.
 *
 * It can also drive the scheduling of the OutputModuleInterface (via uORB callbacks
 * to reduce output latency).
 */
ଶ୍ରେଣୀ MixingOutput : ସର୍ବସାଧାରଣ ModuleParams
{
ସର୍ବସାଧାରଣ:
	ସ୍ଥିର constexpr ପୂର୍ଣ୍ଣସଂଖ୍ୟା MAX_ACTUATORS = OutputModuleInterface::MAX_ACTUATORS;

	ଗଣନା ଶ୍ରେଣୀ SchedulingPolicy {
		Disabled, ///< Do not drive scheduling (the module needs to call ScheduleOnInterval() for example)
		Auto ///< Drive scheduling based on subscribed actuator controls topics (via uORB callbacks)
	};

	/**
	 * Constructor
	 * @param param_prefix for min/max/etc. params, e.g. "PWM_MAIN". This needs to match 'param_prefix' in the module.yaml
	 * @param max_num_outputs maximum number of supported outputs
	 * @param interface Parent module for scheduling, parameter updates and callbacks
	 * @param scheduling_policy
	 * @param support_esc_calibration true if the output module supports ESC calibration via max, then min setting
	 * @param ramp_up true if motor ramp up from disarmed to min upon arming is wanted
	 */
	MixingOutput(ଅଚଳ ଅକ୍ଷର *param_prefix, uint8_t max_ଅଙ୍କ_outputs, OutputModuleInterface &interface,
		     SchedulingPolicy scheduling_policy,
		     ତାର୍କିକ support_esc_calibration, ତାର୍କିକ ramp_up = ସତ୍ୟ, ଅଚଳ uint8_t instance_start = 1);

	~MixingOutput();

	ଶୂନ୍ୟ printStatus() ଅଚଳ;

	/**
	 * Permanently disable an output function
	 */
	ଶୂନ୍ୟ disableFunction(ପୂର୍ଣ୍ଣସଂଖ୍ୟା index)
	{
		_param_handles[index].function = PARAM_INVALID;
		_need_function_update = ସତ୍ୟ;
	}

	/**
	 * Check if a function is configured, i.e. not set to Disabled and initialized
	 */
	ତାର୍କିକ isFunctionSet(ପୂର୍ଣ୍ଣସଂଖ୍ୟା index) ଅଚଳ { ଫେରନ୍ତୁ _functions[index] != nullptr; }

	OutputFunction outputFunction(ପୂର୍ଣ୍ଣସଂଖ୍ୟା index) ଅଚଳ { ଫେରନ୍ତୁ _function_assignment[index]; }

	ତାର୍କିକ isMotor(ପୂର୍ଣ୍ଣସଂଖ୍ୟା index) ଅଚଳ { ଫେରନ୍ତୁ isFunctionSet(index) && (_function_assignment[index] >= OutputFunction::Motor1) && (_function_assignment[index] <= OutputFunction::Motor12); }

	/**
	 * Call this regularly from Run(). It will call interface.updateOutputs().
	 * @return true if outputs were updated
	 */
	ତାର୍କିକ update();

	/**
	 * Check for subscription updates.
	 * Call this at the very end of Run() if allow_wq_switch
	 * @param allow_wq_switch if true
	 * @return true if subscriptions got changed
	 */
	ତାର୍କିକ updateSubscriptions(ତାର୍କିକ allow_wq_switch = ମିଥ୍ୟା);

	/**
	 * unregister uORB subscription callbacks
	 */
	ଶୂନ୍ୟ unregister();

	ଶୂନ୍ୟ setMaxTopicUpdateRate(unsigned max_topic_update_interval_us);

	ଅଚଳ actuator_armed_s &armed() ଅଚଳ { ଫେରନ୍ତୁ _armed; }
	ତାର୍କିକ isActuatorTestRunning() ଅଚଳ { ଫେରନ୍ତୁ _actuator_test.inTestMode(); }

	ଶୂନ୍ୟ set_All_Failsafe_ମୂଲ୍ୟଗୁଡ଼ିକ(uint16_t value);
	ଶୂନ୍ୟ set_All_Disarmed_ମୂଲ୍ୟଗୁଡ଼ିକ(uint16_t value);
	ଶୂନ୍ୟ set_All_Min_ମୂଲ୍ୟଗୁଡ଼ିକ(uint16_t value);
	ଶୂନ୍ୟ set_All_Center_ମୂଲ୍ୟଗୁଡ଼ିକ(uint16_t value);
	ଶୂନ୍ୟ set_All_Max_ମୂଲ୍ୟଗୁଡ଼ିକ(uint16_t value);

	/** Disarmed values: disarmedValue < minValue needs to hold */
	uint16_t &disarmed_ମୂଲ୍ୟ_2(ପୂର୍ଣ୍ଣସଂଖ୍ୟା index) { ଫେରନ୍ତୁ disarmed_ମୂଲ୍ୟ[index]; }
	uint16_t &min_ମୂଲ୍ୟ_2(ପୂର୍ଣ୍ଣସଂଖ୍ୟା index) { ଫେରନ୍ତୁ min_ମୂଲ୍ୟ[index]; }
	uint16_t &center_ମୂଲ୍ୟ_2(ପୂର୍ଣ୍ଣସଂଖ୍ୟା index) { ଫେରନ୍ତୁ center_ମୂଲ୍ୟ[index]; }
	uint16_t &max_ମୂଲ୍ୟ_2(ପୂର୍ଣ୍ଣସଂଖ୍ୟା index) { ଫେରନ୍ତୁ max_ମୂଲ୍ୟ[index]; }

	param_t functionParamHandle(ପୂର୍ଣ୍ଣସଂଖ୍ୟା index) ଅଚଳ { ଫେରନ୍ତୁ _param_handles[index].function; }
	param_t disarmedParamHandle(ପୂର୍ଣ୍ଣସଂଖ୍ୟା index) ଅଚଳ { ଫେରନ୍ତୁ _param_handles[index].disarmed; }
	param_t minParamHandle(ପୂର୍ଣ୍ଣସଂଖ୍ୟା index) ଅଚଳ { ଫେରନ୍ତୁ _param_handles[index].min; }
	param_t centerParamHandle(ପୂର୍ଣ୍ଣସଂଖ୍ୟା index) ଅଚଳ { ଫେରନ୍ତୁ _param_handles[index].center; }
	param_t maxParamHandle(ପୂର୍ଣ୍ଣସଂଖ୍ୟା index) ଅଚଳ { ଫେରନ୍ତୁ _param_handles[index].max; }

	/**
	 * Returns the actual failsafe value taking into account the assigned function
	 */
	ଦଶମିକ actual_Failsafe_ମୂଲ୍ୟ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା index) ଅଚଳ;

	ଶୂନ୍ୟ setIgnoreLockdown(ତାର୍କିକ ignore_lockdown) { _ignore_lockdown = ignore_lockdown; }

	/**
	 * Set the maximum number of outputs. This can only be used to reduce the maximum.
	 */
	ଶୂନ୍ୟ set_Max_ଅଙ୍କ_Outputs(uint8_t max_ଅଙ୍କ_outputs) { ଯଦି (max_ଅଙ୍କ_outputs < max_ଅଙ୍କ_outputs_2) { max_ଅଙ୍କ_outputs_2 = max_ଅଙ୍କ_outputs; } }

	ଅଚଳ ଅକ୍ଷର *paramPrefix() ଅଚଳ { ଫେରନ୍ତୁ _param_prefix; }

	ଶୂନ୍ୟ setLowrateSchedulingInterval(hrt_abstime interval) { _lowrate_schedule_interval = interval; }

	/**
	 * Get the bitmask of reversible outputs (motors only).
	 * This might change at any time, including while armed (motor failure recovery reverses a motor in
	 * flight). Drivers that must react to a change should override reversibleMaskChanged().
	 */
	uint32_t reversibleOutputs() ଅଚଳ { ଫେରନ୍ତୁ _reversible_mask; }

ସୁରକ୍ଷିତ:
	ଶୂନ୍ୟ updateParams() override;
	ଦଶମିକ output_limit_calc_single(ପୂର୍ଣ୍ଣସଂଖ୍ୟା i, ଦଶମିକ value) ଅଚଳ;

ବ୍ୟକ୍ତିଗତ:

	ତାର୍କିକ isPrearmed() ଅଚଳ
	{
		ଫେରନ୍ତୁ (_armed.prearmed && !_armed.armed) || _armed.in_esc_calibration_mode;
	}

	ଶୂନ୍ୟ setAndPublishActuatorOutputs(unsigned ଅଙ୍କ_outputs, actuator_outputs_s &actuator_outputs);
	ଶୂନ୍ୟ publishMixerStatus(ଅଚଳ actuator_outputs_s &actuator_outputs);
	ଶୂନ୍ୟ updateLatencyPerfCounter(ଅଚଳ actuator_outputs_s &actuator_outputs);

	ଶୂନ୍ୟ cleanupFunctions();

	ଶୂନ୍ୟ initParamHandles(ଅଚଳ uint8_t instance_start);

	ଶୂନ୍ୟ limitAndUpdateOutputs(ଦଶମିକ outputs[MAX_ACTUATORS], ତାର୍କିକ has_updates);

	ଶୂନ୍ୟ output_limit_calc(ଅଚଳ ତାର୍କିକ armed, ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ_channels, ଅଚଳ ଦଶମିକ outputs[MAX_ACTUATORS]);

	ସଂରଚନା ParamHandles {
		param_t function{PARAM_INVALID};
		param_t disarmed{PARAM_INVALID};
		param_t min{PARAM_INVALID};
		param_t center{PARAM_INVALID};
		param_t max{PARAM_INVALID};
		param_t failsafe{PARAM_INVALID};
	};

	ଶୂନ୍ୟ lock() { କର {} ଯେତେବେଳେ (px4_sem_wait(&_lock) != 0); }
	ଶୂନ୍ୟ unlock() { px4_sem_post(&_lock); }

	px4_sem_t _lock; /**< lock to protect access to work queue changes (includes ScheduleNow calls from another thread) */

	uint16_t failsafe_ମୂଲ୍ୟ[MAX_ACTUATORS] {};
	uint16_t disarmed_ମୂଲ୍ୟ[MAX_ACTUATORS] {};
	uint16_t min_ମୂଲ୍ୟ[MAX_ACTUATORS] {};
	uint16_t center_ମୂଲ୍ୟ[MAX_ACTUATORS] {};
	uint16_t max_ମୂଲ୍ୟ[MAX_ACTUATORS] {};
	ଦଶମିକ current_output_ମୂଲ୍ୟ[MAX_ACTUATORS] {}; ///< current output values (reordered)
	uint16_t _reverse_output_mask{0}; ///< reverses the interval [min, max] -> [max, min], NOT motor direction

	ଗଣନା ଶ୍ରେଣୀ OutputLimitState {
		OFF = 0,
		RAMP,
		ON
	} _output_state{OutputLimitState::OFF};

	hrt_abstime _output_time_armed{0};
	ଅଚଳ ତାର୍କିକ _output_ramp_up; ///< if true, motors will ramp up from disarmed to min_output after arming

	uORB::Subscription _armed_sub{ORB_ID(actuator_armed)};

	uORB::PublicationMulti<actuator_outputs_s> _outputs_pub{ORB_ID(actuator_outputs)};

	actuator_armed_s _armed{};

	unsigned _max_topic_update_interval_us{0}; ///< max topic update interval (0=unlimited)

	ତାର୍କିକ _throttle_armed{ମିଥ୍ୟା};
	ତାର୍କିକ _ignore_lockdown{ମିଥ୍ୟା}; ///< if true, ignore the _armed.lockdown flag (for HIL outputs)

	ଅଚଳ SchedulingPolicy _scheduling_policy;
	ଅଚଳ ତାର୍କିକ _support_esc_calibration;

	ତାର୍କିକ _wq_switched{ମିଥ୍ୟା};
	uint8_t max_ଅଙ୍କ_outputs_2;

	OutputModuleInterface &_interface;

	perf_counter_t _control_latency_perf;

	FunctionProviderBase *_function_allocated[MAX_ACTUATORS] {}; ///< unique allocated functions
	FunctionProviderBase *_functions[MAX_ACTUATORS] {}; ///< currently assigned functions
	OutputFunction _function_assignment[MAX_ACTUATORS] {};
	ତାର୍କିକ _need_function_update{ସତ୍ୟ};
	ତାର୍କିକ _has_backup_schedule{ମିଥ୍ୟା};
	ଅଚଳ ଅକ୍ଷର *ଅଚଳ _param_prefix;
	ParamHandles _param_handles[MAX_ACTUATORS];
	param_t _param_handle_rev_range{PARAM_INVALID};
	hrt_abstime _lowrate_schedule_interval{300_ms};
	ActuatorTest _actuator_test{_function_assignment};
	uint32_t _reversible_mask{0}; ///< per-output bits. If set, the output is configured to be reversible (motors only)
	ତାର୍କିକ _was_all_disabled{ମିଥ୍ୟା};

	uORB::SubscriptionCallbackWorkItem *_subscription_callback{nullptr}; ///< current scheduling callback


	DEFINE_PARAMETERS(
		(ParamInt<px4::params::MC_AIRMODE>) _param_mc_airmode,   ///< multicopter air-mode
		(ParamFloat<px4::params::THR_MDL_FAC>) _param_thr_mdl_fac ///< thrust to motor control signal modelling factor
	)
};
