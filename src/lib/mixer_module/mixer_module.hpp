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

#ضمن "actuator_test.hpp"

#ضمن "functions/FunctionActuatorSet.hpp"
#ضمن "functions/FunctionConstantMax.hpp"
#ضمن "functions/FunctionConstantMin.hpp"
#ضمن "functions/FunctionGimbal.hpp"
#ضمن "functions/FunctionGripper.hpp"
#ضمن "functions/FunctionICEngineControl.hpp"
#ضمن "functions/FunctionLandingGear.hpp"
#ضمن "functions/FunctionLandingGearWheel.hpp"
#ضمن "functions/FunctionManualRC.hpp"
#ضمن "functions/FunctionMotors.hpp"
#ضمن "functions/FunctionParachute.hpp"
#ضمن "functions/FunctionServos.hpp"

#ضمن <board_config.h>
#ضمن <drivers/drv_pwm_output.h>
#ضمن <lib/perf/perf_counter.h>
#ضمن <px4_platform_common/module_params.h>
#ضمن <px4_platform_common/px4_work_queue/ScheduledWorkItem.hpp>
#ضمن <uORB/Publication.hpp>
#ضمن <uORB/PublicationMulti.hpp>
#ضمن <uORB/Subscription.hpp>
#ضمن <uORB/SubscriptionCallback.hpp>
#ضمن <uORB/topics/actuator_armed.h>
#ضمن <uORB/topics/actuator_outputs.h>
#ضمن <uORB/topics/parameter_update.h>

using نطاق time_literals;

/**
 * @class OutputModuleInterface
 * Base class for an output module.
 */
صنف OutputModuleInterface : عام px4::ScheduledWorkItem, عام ModuleParams
{
عام:
	ساكن constexpr صحيح MAX_ACTUATORS = PWM_OUTPUT_MAX_CHANNELS;

	OutputModuleInterface(ثابت حرف *name, ثابت px4::wq_config_t &config) :
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
	virtual منطقي updateOutputs(عائم outputs[MAX_ACTUATORS], unsigned ژمارە_outputs, unsigned ژمارە_control_groups_updated) = 0;

	/** called whenever the mixer gets updated/reset */
	virtual فراغ mixerChanged() {}

	/**
	 * Called when the reversible output mask changes, before the new mask is used to compute outputs.
	 * Drivers whose output range depends on reversibility must update it here, not from their Run().
	 */
	virtual فراغ reversibleMaskChanged(uint32_t reversible_mask) {}
};

/**
 * @class MixingOutput
 * This handles the mixing, arming/disarming and all subscriptions required for that.
 *
 * It can also drive the scheduling of the OutputModuleInterface (via uORB callbacks
 * to reduce output latency).
 */
صنف MixingOutput : عام ModuleParams
{
عام:
	ساكن constexpr صحيح MAX_ACTUATORS = OutputModuleInterface::MAX_ACTUATORS;

	تعداد صنف SchedulingPolicy {
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
	MixingOutput(ثابت حرف *param_prefix, uint8_t max_ژمارە_outputs, OutputModuleInterface &interface,
		     SchedulingPolicy scheduling_policy,
		     منطقي support_esc_calibration, منطقي ramp_up = true, ثابت uint8_t instance_start = 1);

	~MixingOutput();

	فراغ printStatus() ثابت;

	/**
	 * Permanently disable an output function
	 */
	فراغ disableFunction(صحيح index)
	{
		_param_handles[index].function = PARAM_INVALID;
		_need_function_update = true;
	}

	/**
	 * Check if a function is configured, i.e. not set to Disabled and initialized
	 */
	منطقي isFunctionSet(صحيح index) ثابت { أرجع _functions[index] != nullptr; }

	OutputFunction outputFunction(صحيح index) ثابت { أرجع _function_assignment[index]; }

	منطقي isMotor(صحيح index) ثابت { أرجع isFunctionSet(index) && (_function_assignment[index] >= OutputFunction::Motor1) && (_function_assignment[index] <= OutputFunction::Motor12); }

	/**
	 * Call this regularly from Run(). It will call interface.updateOutputs().
	 * @return true if outputs were updated
	 */
	منطقي update();

	/**
	 * Check for subscription updates.
	 * Call this at the very end of Run() if allow_wq_switch
	 * @param allow_wq_switch if true
	 * @return true if subscriptions got changed
	 */
	منطقي updateSubscriptions(منطقي allow_wq_switch = خطأ);

	/**
	 * unregister uORB subscription callbacks
	 */
	فراغ unregister();

	فراغ setMaxTopicUpdateRate(unsigned max_topic_update_interval_us);

	ثابت actuator_armed_s &armed() ثابت { أرجع _armed; }
	منطقي isActuatorTestRunning() ثابت { أرجع _actuator_test.inTestMode(); }

	فراغ set_All_Failsafe_بەهاکان(uint16_t value);
	فراغ set_All_Disarmed_بەهاکان(uint16_t value);
	فراغ set_All_Min_بەهاکان(uint16_t value);
	فراغ set_All_Center_بەهاکان(uint16_t value);
	فراغ set_All_Max_بەهاکان(uint16_t value);

	/** Disarmed values: disarmedValue < minValue needs to hold */
	uint16_t &disarmed_بەها_2(صحيح index) { أرجع disarmed_بەها[index]; }
	uint16_t &min_بەها_2(صحيح index) { أرجع min_بەها[index]; }
	uint16_t &center_بەها_2(صحيح index) { أرجع center_بەها[index]; }
	uint16_t &max_بەها_2(صحيح index) { أرجع max_بەها[index]; }

	param_t functionParamHandle(صحيح index) ثابت { أرجع _param_handles[index].function; }
	param_t disarmedParamHandle(صحيح index) ثابت { أرجع _param_handles[index].disarmed; }
	param_t minParamHandle(صحيح index) ثابت { أرجع _param_handles[index].min; }
	param_t centerParamHandle(صحيح index) ثابت { أرجع _param_handles[index].center; }
	param_t maxParamHandle(صحيح index) ثابت { أرجع _param_handles[index].max; }

	/**
	 * Returns the actual failsafe value taking into account the assigned function
	 */
	عائم actual_Failsafe_بەها(صحيح index) ثابت;

	فراغ setIgnoreLockdown(منطقي ignore_lockdown) { _ignore_lockdown = ignore_lockdown; }

	/**
	 * Set the maximum number of outputs. This can only be used to reduce the maximum.
	 */
	فراغ set_Max_ژمارە_Outputs(uint8_t max_ژمارە_outputs) { إذا (max_ژمارە_outputs < max_ژمارە_outputs_2) { max_ژمارە_outputs_2 = max_ژمارە_outputs; } }

	ثابت حرف *paramPrefix() ثابت { أرجع _param_prefix; }

	فراغ setLowrateSchedulingInterval(hrt_abstime interval) { _lowrate_schedule_interval = interval; }

	/**
	 * Get the bitmask of reversible outputs (motors only).
	 * This might change at any time, including while armed (motor failure recovery reverses a motor in
	 * flight). Drivers that must react to a change should override reversibleMaskChanged().
	 */
	uint32_t reversibleOutputs() ثابت { أرجع _reversible_mask; }

محمي:
	فراغ updateParams() override;
	عائم output_limit_calc_single(صحيح i, عائم value) ثابت;

خاص:

	منطقي isPrearmed() ثابت
	{
		أرجع (_armed.prearmed && !_armed.armed) || _armed.in_esc_calibration_mode;
	}

	فراغ setAndPublishActuatorOutputs(unsigned ژمارە_outputs, actuator_outputs_s &actuator_outputs);
	فراغ publishMixerStatus(ثابت actuator_outputs_s &actuator_outputs);
	فراغ updateLatencyPerfCounter(ثابت actuator_outputs_s &actuator_outputs);

	فراغ cleanupFunctions();

	فراغ initParamHandles(ثابت uint8_t instance_start);

	فراغ limitAndUpdateOutputs(عائم outputs[MAX_ACTUATORS], منطقي has_updates);

	فراغ output_limit_calc(ثابت منطقي armed, ثابت صحيح ژمارە_channels, ثابت عائم outputs[MAX_ACTUATORS]);

	بنية ParamHandles {
		param_t function{PARAM_INVALID};
		param_t disarmed{PARAM_INVALID};
		param_t min{PARAM_INVALID};
		param_t center{PARAM_INVALID};
		param_t max{PARAM_INVALID};
		param_t failsafe{PARAM_INVALID};
	};

	فراغ lock() { افعل {} طالما (px4_sem_wait(&_lock) != 0); }
	فراغ unlock() { px4_sem_post(&_lock); }

	px4_sem_t _lock; /**< lock to protect access to work queue changes (includes ScheduleNow calls from another thread) */

	uint16_t failsafe_بەها[MAX_ACTUATORS] {};
	uint16_t disarmed_بەها[MAX_ACTUATORS] {};
	uint16_t min_بەها[MAX_ACTUATORS] {};
	uint16_t center_بەها[MAX_ACTUATORS] {};
	uint16_t max_بەها[MAX_ACTUATORS] {};
	عائم current_output_بەها[MAX_ACTUATORS] {}; ///< current output values (reordered)
	uint16_t _reverse_output_mask{0}; ///< reverses the interval [min, max] -> [max, min], NOT motor direction

	تعداد صنف OutputLimitState {
		OFF = 0,
		RAMP,
		ON
	} _output_state{OutputLimitState::OFF};

	hrt_abstime _output_time_armed{0};
	ثابت منطقي _output_ramp_up; ///< if true, motors will ramp up from disarmed to min_output after arming

	uORB::Subscription _armed_sub{ORB_ID(actuator_armed)};

	uORB::PublicationMulti<actuator_outputs_s> _outputs_pub{ORB_ID(actuator_outputs)};

	actuator_armed_s _armed{};

	unsigned _max_topic_update_interval_us{0}; ///< max topic update interval (0=unlimited)

	منطقي _throttle_armed{خطأ};
	منطقي _ignore_lockdown{خطأ}; ///< if true, ignore the _armed.lockdown flag (for HIL outputs)

	ثابت SchedulingPolicy _scheduling_policy;
	ثابت منطقي _support_esc_calibration;

	منطقي _wq_switched{خطأ};
	uint8_t max_ژمارە_outputs_2;

	OutputModuleInterface &_interface;

	perf_counter_t _control_latency_perf;

	FunctionProviderBase *_function_allocated[MAX_ACTUATORS] {}; ///< unique allocated functions
	FunctionProviderBase *_functions[MAX_ACTUATORS] {}; ///< currently assigned functions
	OutputFunction _function_assignment[MAX_ACTUATORS] {};
	منطقي _need_function_update{true};
	منطقي _has_backup_schedule{خطأ};
	ثابت حرف *ثابت _param_prefix;
	ParamHandles _param_handles[MAX_ACTUATORS];
	param_t _param_handle_rev_range{PARAM_INVALID};
	hrt_abstime _lowrate_schedule_interval{300_ms};
	ActuatorTest _actuator_test{_function_assignment};
	uint32_t _reversible_mask{0}; ///< per-output bits. If set, the output is configured to be reversible (motors only)
	منطقي _was_all_disabled{خطأ};

	uORB::SubscriptionCallbackWorkItem *_subscription_callback{nullptr}; ///< current scheduling callback


	DEFINE_PARAMETERS(
		(ParamInt<px4::params::MC_AIRMODE>) _param_mc_airmode,   ///< multicopter air-mode
		(ParamFloat<px4::params::THR_MDL_FAC>) _param_thr_mdl_fac ///< thrust to motor control signal modelling factor
	)
};
