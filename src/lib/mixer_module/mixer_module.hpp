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

#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ "actuator_test.hpp"

#ਅਨ੍ਤਰ੍ਭਾਵ "functions/FunctionActuatorSet.hpp"
#ਅਨ੍ਤਰ੍ਭਾਵ "functions/FunctionConstantMax.hpp"
#ਅਨ੍ਤਰ੍ਭਾਵ "functions/FunctionConstantMin.hpp"
#ਅਨ੍ਤਰ੍ਭਾਵ "functions/FunctionGimbal.hpp"
#ਅਨ੍ਤਰ੍ਭਾਵ "functions/FunctionGripper.hpp"
#ਅਨ੍ਤਰ੍ਭਾਵ "functions/FunctionICEngineControl.hpp"
#ਅਨ੍ਤਰ੍ਭਾਵ "functions/FunctionLandingGear.hpp"
#ਅਨ੍ਤਰ੍ਭਾਵ "functions/FunctionLandingGearWheel.hpp"
#ਅਨ੍ਤਰ੍ਭਾਵ "functions/FunctionManualRC.hpp"
#ਅਨ੍ਤਰ੍ਭਾਵ "functions/FunctionMotors.hpp"
#ਅਨ੍ਤਰ੍ਭਾਵ "functions/FunctionParachute.hpp"
#ਅਨ੍ਤਰ੍ਭਾਵ "functions/FunctionServos.hpp"

#ਅਨ੍ਤਰ੍ਭਾਵ <board_config.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <drivers/drv_pwm_output.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <lib/perf/perf_counter.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <px4_platform_common/module_params.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <px4_platform_common/px4_work_queue/ScheduledWorkItem.hpp>
#ਅਨ੍ਤਰ੍ਭਾਵ <uORB/Publication.hpp>
#ਅਨ੍ਤਰ੍ਭਾਵ <uORB/PublicationMulti.hpp>
#ਅਨ੍ਤਰ੍ਭਾਵ <uORB/Subscription.hpp>
#ਅਨ੍ਤਰ੍ਭਾਵ <uORB/SubscriptionCallback.hpp>
#ਅਨ੍ਤਰ੍ਭਾਵ <uORB/topics/actuator_armed.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <uORB/topics/actuator_outputs.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <uORB/topics/parameter_update.h>

ਵਰਤੋਂ ਨਾਮਾਕਾਸ਼ ਸਮਾਂ_literals;

/**
 * @class OutputModuleInterface
 * Base class for an output module.
 */
ਵਰਗ OutputModuleInterface : ਜਨਤਕ px4::Scheduled_Work_ਵਸਤੂ, ਜਨਤਕ ModuleParams
{
ਜਨਤਕ:
	ਸਥਿਰ constexpr ਪੂਰਨ MAX_ACTUATORS = PWM_OUTPUT_MAX_CHANNELS;

	OutputModuleInterface(ਅਚਲ ਅੱਖਰ *name, ਅਚਲ px4::wq_config_t &config) :
		px4::Scheduled_Work_ਵਸਤੂ(name, config),
		ModuleParams(ਖਾਲੀ_ਸੂਚਕ)
	{}

	/**
	 * Callback to update the (physical) actuator outputs in the driver
	 * @param outputs individual actuator outputs in range [min, max] or failsafe/disarmed value
	 * @param num_outputs number of outputs (<= max_num_outputs)
	 * @param num_control_groups_updated number of actuator_control groups updated
	 * @return if true, the update got handled, and actuator_outputs can be published
	 */
	ਆਭਾਸੀ ਤਾਰਕਿਕ updateOutputs(ਦਸ਼ਮਲਵ outputs[MAX_ACTUATORS], ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੰਕ_outputs, ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੰਕ_control_groups_updated) = 0;

	/** called whenever the mixer gets updated/reset */
	ਆਭਾਸੀ ਖਾਲੀ mixer_ਬਦਲਿਆ() {}

	/**
	 * Called when the reversible output mask changes, before the new mask is used to compute outputs.
	 * Drivers whose output range depends on reversibility must update it here, not from their Run().
	 */
	ਆਭਾਸੀ ਖਾਲੀ reversible_ਨਕਾਬ_ਬਦਲਿਆ(uint32_t reversible_ਨਕਾਬ_2) {}
};

/**
 * @class MixingOutput
 * This handles the mixing, arming/disarming and all subscriptions required for that.
 *
 * It can also drive the scheduling of the OutputModuleInterface (via uORB callbacks
 * to reduce output latency).
 */
ਵਰਗ MixingOutput : ਜਨਤਕ ModuleParams
{
ਜਨਤਕ:
	ਸਥਿਰ constexpr ਪੂਰਨ MAX_ACTUATORS = OutputModuleInterface::MAX_ACTUATORS;

	ਗਿਣਤੀ ਵਰਗ SchedulingPolicy {
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
	MixingOutput(ਅਚਲ ਅੱਖਰ *param_ਅਗੇਤਰ, uint8_t max_ਅੰਕ_outputs, OutputModuleInterface &interface,
		     SchedulingPolicy scheduling_policy,
		     ਤਾਰਕਿਕ support_esc_calibration, ਤਾਰਕਿਕ ramp_ਉੱਪਰ = ਸੱਚ, ਅਚਲ uint8_t instance_ਸ਼ੁਰੂ = 1);

	~MixingOutput();

	ਖਾਲੀ print_ਸਥਿਤੀ_2() ਅਚਲ;

	/**
	 * Permanently disable an output function
	 */
	ਖਾਲੀ disableFunction(ਪੂਰਨ ਸੂਚਕਾਂਕ)
	{
		_param_handles[ਸੂਚਕਾਂਕ].function = PARAM_INVALID;
		_need_function_update = ਸੱਚ;
	}

	/**
	 * Check if a function is configured, i.e. not set to Disabled and initialized
	 */
	ਤਾਰਕਿਕ is_Function_ਸੈੱਟ(ਪੂਰਨ ਸੂਚਕਾਂਕ) ਅਚਲ { ਵਾਪਸ _functions[ਸੂਚਕਾਂਕ] != ਖਾਲੀ_ਸੂਚਕ; }

	OutputFunction outputFunction(ਪੂਰਨ ਸੂਚਕਾਂਕ) ਅਚਲ { ਵਾਪਸ _function_assignment[ਸੂਚਕਾਂਕ]; }

	ਤਾਰਕਿਕ isMotor(ਪੂਰਨ ਸੂਚਕਾਂਕ) ਅਚਲ { ਵਾਪਸ is_Function_ਸੈੱਟ(ਸੂਚਕਾਂਕ) && (_function_assignment[ਸੂਚਕਾਂਕ] >= OutputFunction::Motor1) && (_function_assignment[ਸੂਚਕਾਂਕ] <= OutputFunction::Motor12); }

	/**
	 * Call this regularly from Run(). It will call interface.updateOutputs().
	 * @return true if outputs were updated
	 */
	ਤਾਰਕਿਕ update();

	/**
	 * Check for subscription updates.
	 * Call this at the very end of Run() if allow_wq_switch
	 * @param allow_wq_switch if true
	 * @return true if subscriptions got changed
	 */
	ਤਾਰਕਿਕ updateSubscriptions(ਤਾਰਕਿਕ allow_wq_switch = ਝੂਠ);

	/**
	 * unregister uORB subscription callbacks
	 */
	ਖਾਲੀ unregister();

	ਖਾਲੀ ਸੈੱਟ_Max_Topic_Update_ਦਰ(ਬਿਨਾਨਿਸ਼ਾਨੀ max_topic_update_interval_us);

	ਅਚਲ actuator_armed_s &armed() ਅਚਲ { ਵਾਪਸ _armed; }
	ਤਾਰਕਿਕ isActuatorTestRunning() ਅਚਲ { ਵਾਪਸ _actuator_test.ਵਿੱਚ_Test_ਬਹੁਲਕ(); }

	ਖਾਲੀ ਸੈੱਟ_All_Failsafe_ਮੁੱਲਾਂ(uint16_t value);
	ਖਾਲੀ ਸੈੱਟ_All_Disarmed_ਮੁੱਲਾਂ(uint16_t value);
	ਖਾਲੀ ਸੈੱਟ_All_Min_ਮੁੱਲਾਂ(uint16_t value);
	ਖਾਲੀ ਸੈੱਟ_All_Center_ਮੁੱਲਾਂ(uint16_t value);
	ਖਾਲੀ ਸੈੱਟ_All_Max_ਮੁੱਲਾਂ(uint16_t value);

	/** Disarmed values: disarmedValue < minValue needs to hold */
	uint16_t &disarmed_ਮੁੱਲ_2(ਪੂਰਨ ਸੂਚਕਾਂਕ) { ਵਾਪਸ disarmed_ਮੁੱਲ[ਸੂਚਕਾਂਕ]; }
	uint16_t &min_ਮੁੱਲ_2(ਪੂਰਨ ਸੂਚਕਾਂਕ) { ਵਾਪਸ min_ਮੁੱਲ[ਸੂਚਕਾਂਕ]; }
	uint16_t &center_ਮੁੱਲ_2(ਪੂਰਨ ਸੂਚਕਾਂਕ) { ਵਾਪਸ center_ਮੁੱਲ[ਸੂਚਕਾਂਕ]; }
	uint16_t &max_ਮੁੱਲ_2(ਪੂਰਨ ਸੂਚਕਾਂਕ) { ਵਾਪਸ max_ਮੁੱਲ[ਸੂਚਕਾਂਕ]; }

	param_t functionParamHandle(ਪੂਰਨ ਸੂਚਕਾਂਕ) ਅਚਲ { ਵਾਪਸ _param_handles[ਸੂਚਕਾਂਕ].function; }
	param_t disarmedParamHandle(ਪੂਰਨ ਸੂਚਕਾਂਕ) ਅਚਲ { ਵਾਪਸ _param_handles[ਸੂਚਕਾਂਕ].disarmed; }
	param_t minParamHandle(ਪੂਰਨ ਸੂਚਕਾਂਕ) ਅਚਲ { ਵਾਪਸ _param_handles[ਸੂਚਕਾਂਕ].min; }
	param_t centerParamHandle(ਪੂਰਨ ਸੂਚਕਾਂਕ) ਅਚਲ { ਵਾਪਸ _param_handles[ਸੂਚਕਾਂਕ].center; }
	param_t maxParamHandle(ਪੂਰਨ ਸੂਚਕਾਂਕ) ਅਚਲ { ਵਾਪਸ _param_handles[ਸੂਚਕਾਂਕ].max; }

	/**
	 * Returns the actual failsafe value taking into account the assigned function
	 */
	ਦਸ਼ਮਲਵ actual_Failsafe_ਮੁੱਲ(ਪੂਰਨ ਸੂਚਕਾਂਕ) ਅਚਲ;

	ਖਾਲੀ ਸੈੱਟ_Ignore_Lockdown(ਤਾਰਕਿਕ ignore_lockdown) { _ignore_lockdown = ignore_lockdown; }

	/**
	 * Set the maximum number of outputs. This can only be used to reduce the maximum.
	 */
	ਖਾਲੀ ਸੈੱਟ_Max_ਅੰਕ_Outputs(uint8_t max_ਅੰਕ_outputs) { ਜੇ (max_ਅੰਕ_outputs < max_ਅੰਕ_outputs_2) { max_ਅੰਕ_outputs_2 = max_ਅੰਕ_outputs; } }

	ਅਚਲ ਅੱਖਰ *param_ਅਗੇਤਰ_3() ਅਚਲ { ਵਾਪਸ param_ਅਗੇਤਰ_2; }

	ਖਾਲੀ ਸੈੱਟ_Lowrate_Scheduling_Interval(hrt_abstime interval) { lowrate_ਸਮਾਂ_ਸਾਰਣੀ_interval = interval; }

	/**
	 * Get the bitmask of reversible outputs (motors only).
	 * This might change at any time, including while armed (motor failure recovery reverses a motor in
	 * flight). Drivers that must react to a change should override reversibleMaskChanged().
	 */
	uint32_t reversibleOutputs() ਅਚਲ { ਵਾਪਸ reversible_ਨਕਾਬ; }

ਸੁਰੱਖਿਅਤ:
	ਖਾਲੀ updateParams() override;
	ਦਸ਼ਮਲਵ output_ਹੱਦਬੰਦੀ_calc_single(ਪੂਰਨ i, ਦਸ਼ਮਲਵ value) ਅਚਲ;

ਨਿਜੀ:

	ਤਾਰਕਿਕ isPrearmed() ਅਚਲ
	{
		ਵਾਪਸ (_armed.prearmed && !_armed.armed) || _armed.ਵਿੱਚ_esc_calibration_ਬਹੁਲਕ;
	}

	ਖਾਲੀ ਸੈੱਟ_And_Publish_Actuator_Outputs(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੰਕ_outputs, actuator_outputs_s &actuator_outputs);
	ਖਾਲੀ publish_Mixer_ਸਥਿਤੀ(ਅਚਲ actuator_outputs_s &actuator_outputs);
	ਖਾਲੀ updateLatencyPerfCounter(ਅਚਲ actuator_outputs_s &actuator_outputs);

	ਖਾਲੀ cleanupFunctions();

	ਖਾਲੀ initParamHandles(ਅਚਲ uint8_t instance_ਸ਼ੁਰੂ);

	ਖਾਲੀ ਹੱਦਬੰਦੀ_And_Update_Outputs(ਦਸ਼ਮਲਵ outputs[MAX_ACTUATORS], ਤਾਰਕਿਕ has_updates);

	ਖਾਲੀ output_ਹੱਦਬੰਦੀ_calc(ਅਚਲ ਤਾਰਕਿਕ armed, ਅਚਲ ਪੂਰਨ ਅੰਕ_channels, ਅਚਲ ਦਸ਼ਮਲਵ outputs[MAX_ACTUATORS]);

	ਬਣਤਰ ParamHandles {
		param_t function{PARAM_INVALID};
		param_t disarmed{PARAM_INVALID};
		param_t min{PARAM_INVALID};
		param_t center{PARAM_INVALID};
		param_t max{PARAM_INVALID};
		param_t failsafe{PARAM_INVALID};
	};

	ਖਾਲੀ ਤਾਲਾ() { ਕਰੋ {} ਜਦੋਂ (px4_sem_wait(&_lock) != 0); }
	ਖਾਲੀ unlock() { px4_sem_post(&_lock); }

	px4_sem_t _lock; /**< lock to protect access to work queue changes (includes ScheduleNow calls from another thread) */

	uint16_t failsafe_ਮੁੱਲ[MAX_ACTUATORS] {};
	uint16_t disarmed_ਮੁੱਲ[MAX_ACTUATORS] {};
	uint16_t min_ਮੁੱਲ[MAX_ACTUATORS] {};
	uint16_t center_ਮੁੱਲ[MAX_ACTUATORS] {};
	uint16_t max_ਮੁੱਲ[MAX_ACTUATORS] {};
	ਦਸ਼ਮਲਵ current_output_ਮੁੱਲ[MAX_ACTUATORS] {}; ///< current output values (reordered)
	uint16_t ਉਲਟ_output_ਨਕਾਬ{0}; ///< reverses the interval [min, max] -> [max, min], NOT motor direction

	ਗਿਣਤੀ ਵਰਗ Output_ਹੱਦਬੰਦੀ_State {
		OFF = 0,
		RAMP,
		ON
	} _output_state{Output_ਹੱਦਬੰਦੀ_State::OFF};

	hrt_abstime output_ਸਮਾਂ_armed{0};
	ਅਚਲ ਤਾਰਕਿਕ output_ramp_ਉੱਪਰ; ///< if true, motors will ramp up from disarmed to min_output after arming

	uORB::Subscription _armed_sub{ORB_ID(actuator_armed)};

	uORB::PublicationMulti<actuator_outputs_s> _outputs_pub{ORB_ID(actuator_outputs)};

	actuator_armed_s _armed{};

	ਬਿਨਾਨਿਸ਼ਾਨੀ _max_topic_update_interval_us{0}; ///< max topic update interval (0=unlimited)

	ਤਾਰਕਿਕ _throttle_armed{ਝੂਠ};
	ਤਾਰਕਿਕ _ignore_lockdown{ਝੂਠ}; ///< if true, ignore the _armed.lockdown flag (for HIL outputs)

	ਅਚਲ SchedulingPolicy _scheduling_policy;
	ਅਚਲ ਤਾਰਕਿਕ _support_esc_calibration;

	ਤਾਰਕਿਕ _wq_switched{ਝੂਠ};
	uint8_t max_ਅੰਕ_outputs_2;

	OutputModuleInterface &_interface;

	perf_counter_t _control_latency_perf;

	Function_Provider_ਅਧਾਰ *_function_allocated[MAX_ACTUATORS] {}; ///< unique allocated functions
	Function_Provider_ਅਧਾਰ *_functions[MAX_ACTUATORS] {}; ///< currently assigned functions
	OutputFunction _function_assignment[MAX_ACTUATORS] {};
	ਤਾਰਕਿਕ _need_function_update{ਸੱਚ};
	ਤਾਰਕਿਕ has_backup_ਸਮਾਂ_ਸਾਰਣੀ{ਝੂਠ};
	ਅਚਲ ਅੱਖਰ *ਅਚਲ param_ਅਗੇਤਰ_2;
	ParamHandles _param_handles[MAX_ACTUATORS];
	param_t _param_handle_rev_range{PARAM_INVALID};
	hrt_abstime lowrate_ਸਮਾਂ_ਸਾਰਣੀ_interval{300_ms};
	ActuatorTest _actuator_test{_function_assignment};
	uint32_t reversible_ਨਕਾਬ{0}; ///< per-output bits. If set, the output is configured to be reversible (motors only)
	ਤਾਰਕਿਕ _was_all_disabled{ਝੂਠ};

	uORB::Subscription_Callback_Work_ਵਸਤੂ *_subscription_callback{ਖਾਲੀ_ਸੂਚਕ}; ///< current scheduling callback


	DEFINE_PARAMETERS(
		(ParamInt<px4::params::MC_AIRMODE>) _param_mc_airmode,   ///< multicopter air-mode
		(ParamFloat<px4::params::THR_MDL_FAC>) _param_thr_mdl_fac ///< thrust to motor control signal modelling factor
	)
};
