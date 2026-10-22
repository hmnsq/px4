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

#ہدایت once

#شامل "actuator_test.hpp"

#شامل "functions/FunctionActuatorSet.hpp"
#شامل "functions/FunctionConstantMax.hpp"
#شامل "functions/FunctionConstantMin.hpp"
#شامل "functions/FunctionGimbal.hpp"
#شامل "functions/FunctionGripper.hpp"
#شامل "functions/FunctionICEngineControl.hpp"
#شامل "functions/FunctionLandingGear.hpp"
#شامل "functions/FunctionLandingGearWheel.hpp"
#شامل "functions/FunctionManualRC.hpp"
#شامل "functions/FunctionMotors.hpp"
#شامل "functions/FunctionParachute.hpp"
#شامل "functions/FunctionServos.hpp"

#شامل <board_config.h>
#شامل <drivers/drv_pwm_output.h>
#شامل <lib/perf/perf_counter.h>
#شامل <px4_platform_common/module_params.h>
#شامل <px4_platform_common/px4_work_queue/ScheduledWorkItem.hpp>
#شامل <uORB/Publication.hpp>
#شامل <uORB/PublicationMulti.hpp>
#شامل <uORB/Subscription.hpp>
#شامل <uORB/SubscriptionCallback.hpp>
#شامل <uORB/topics/actuator_armed.h>
#شامل <uORB/topics/actuator_outputs.h>
#شامل <uORB/topics/parameter_update.h>

استعمال نامخلا وقت_literals;

/**
 * @class OutputModuleInterface
 * Base class for an output module.
 */
جماعت OutputModuleInterface : عوامی px4::Scheduled_Work_شے, عوامی ModuleParams
{
عوامی:
	ساکن constexpr صحیح MAX_ACTUATORS = PWM_OUTPUT_MAX_CHANNELS;

	OutputModuleInterface(ثابت حرف *name, ثابت px4::wq_config_t &config) :
		px4::Scheduled_Work_شے(name, config),
		ModuleParams(خالی_اشاریہ)
	{}

	/**
	 * Callback to update the (physical) actuator outputs in the driver
	 * @param outputs individual actuator outputs in range [min, max] or failsafe/disarmed value
	 * @param num_outputs number of outputs (<= max_num_outputs)
	 * @param num_control_groups_updated number of actuator_control groups updated
	 * @return if true, the update got handled, and actuator_outputs can be published
	 */
	مجازی منطقی updateOutputs(اعشاری outputs[MAX_ACTUATORS], غیرعلامتی عدد_outputs, غیرعلامتی عدد_control_groups_updated) = 0;

	/** called whenever the mixer gets updated/reset */
	مجازی خالی mixer_بدلا() {}

	/**
	 * Called when the reversible output mask changes, before the new mask is used to compute outputs.
	 * Drivers whose output range depends on reversibility must update it here, not from their Run().
	 */
	مجازی خالی reversible_نقاب_بدلا(uint32_t reversible_نقاب_2) {}
};

/**
 * @class MixingOutput
 * This handles the mixing, arming/disarming and all subscriptions required for that.
 *
 * It can also drive the scheduling of the OutputModuleInterface (via uORB callbacks
 * to reduce output latency).
 */
جماعت MixingOutput : عوامی ModuleParams
{
عوامی:
	ساکن constexpr صحیح MAX_ACTUATORS = OutputModuleInterface::MAX_ACTUATORS;

	شمار جماعت SchedulingPolicy {
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
	MixingOutput(ثابت حرف *param_سابقہ, uint8_t max_عدد_outputs, OutputModuleInterface &interface,
		     SchedulingPolicy scheduling_policy,
		     منطقی support_esc_calibration, منطقی ramp_اوپر = سچ, ثابت uint8_t instance_آغاز = 1);

	~MixingOutput();

	خالی print_حالت_2() ثابت;

	/**
	 * Permanently disable an output function
	 */
	خالی disableFunction(صحیح اشاریہ)
	{
		_param_handles[اشاریہ].function = PARAM_INVALID;
		_need_function_update = سچ;
	}

	/**
	 * Check if a function is configured, i.e. not set to Disabled and initialized
	 */
	منطقی is_Function_سیٹ(صحیح اشاریہ) ثابت { واپس _functions[اشاریہ] != خالی_اشاریہ; }

	OutputFunction outputFunction(صحیح اشاریہ) ثابت { واپس _function_assignment[اشاریہ]; }

	منطقی isMotor(صحیح اشاریہ) ثابت { واپس is_Function_سیٹ(اشاریہ) && (_function_assignment[اشاریہ] >= OutputFunction::Motor1) && (_function_assignment[اشاریہ] <= OutputFunction::Motor12); }

	/**
	 * Call this regularly from Run(). It will call interface.updateOutputs().
	 * @return true if outputs were updated
	 */
	منطقی update();

	/**
	 * Check for subscription updates.
	 * Call this at the very end of Run() if allow_wq_switch
	 * @param allow_wq_switch if true
	 * @return true if subscriptions got changed
	 */
	منطقی updateSubscriptions(منطقی allow_wq_switch = جھوٹ);

	/**
	 * unregister uORB subscription callbacks
	 */
	خالی unregister();

	خالی سیٹ_Max_Topic_Update_شرح(غیرعلامتی max_topic_update_interval_us);

	ثابت actuator_armed_s &armed() ثابت { واپس _armed; }
	منطقی isActuatorTestRunning() ثابت { واپس _actuator_test.اندر_Test_منوال(); }

	خالی سیٹ_All_Failsafe_اقدار(uint16_t value);
	خالی سیٹ_All_Disarmed_اقدار(uint16_t value);
	خالی سیٹ_All_Min_اقدار(uint16_t value);
	خالی سیٹ_All_Center_اقدار(uint16_t value);
	خالی سیٹ_All_Max_اقدار(uint16_t value);

	/** Disarmed values: disarmedValue < minValue needs to hold */
	uint16_t &disarmed_قدر_2(صحیح اشاریہ) { واپس disarmed_قدر[اشاریہ]; }
	uint16_t &min_قدر_2(صحیح اشاریہ) { واپس min_قدر[اشاریہ]; }
	uint16_t &center_قدر_2(صحیح اشاریہ) { واپس center_قدر[اشاریہ]; }
	uint16_t &max_قدر_2(صحیح اشاریہ) { واپس max_قدر[اشاریہ]; }

	param_t functionParamHandle(صحیح اشاریہ) ثابت { واپس _param_handles[اشاریہ].function; }
	param_t disarmedParamHandle(صحیح اشاریہ) ثابت { واپس _param_handles[اشاریہ].disarmed; }
	param_t minParamHandle(صحیح اشاریہ) ثابت { واپس _param_handles[اشاریہ].min; }
	param_t centerParamHandle(صحیح اشاریہ) ثابت { واپس _param_handles[اشاریہ].center; }
	param_t maxParamHandle(صحیح اشاریہ) ثابت { واپس _param_handles[اشاریہ].max; }

	/**
	 * Returns the actual failsafe value taking into account the assigned function
	 */
	اعشاری actual_Failsafe_قدر(صحیح اشاریہ) ثابت;

	خالی سیٹ_Ignore_Lockdown(منطقی ignore_lockdown) { _ignore_lockdown = ignore_lockdown; }

	/**
	 * Set the maximum number of outputs. This can only be used to reduce the maximum.
	 */
	خالی سیٹ_Max_عدد_Outputs(uint8_t max_عدد_outputs) { اگر (max_عدد_outputs < max_عدد_outputs_2) { max_عدد_outputs_2 = max_عدد_outputs; } }

	ثابت حرف *param_سابقہ_3() ثابت { واپس param_سابقہ_2; }

	خالی سیٹ_Lowrate_Scheduling_Interval(hrt_abstime interval) { lowrate_جدول_وقت_interval = interval; }

	/**
	 * Get the bitmask of reversible outputs (motors only).
	 * This might change at any time, including while armed (motor failure recovery reverses a motor in
	 * flight). Drivers that must react to a change should override reversibleMaskChanged().
	 */
	uint32_t reversibleOutputs() ثابت { واپس reversible_نقاب; }

محفوظ:
	خالی updateParams() override;
	اعشاری output_حد_بندی_calc_single(صحیح i, اعشاری value) ثابت;

نجی:

	منطقی isPrearmed() ثابت
	{
		واپس (_armed.prearmed && !_armed.armed) || _armed.اندر_esc_calibration_منوال;
	}

	خالی سیٹ_And_Publish_Actuator_Outputs(غیرعلامتی عدد_outputs, actuator_outputs_s &actuator_outputs);
	خالی publish_Mixer_حالت(ثابت actuator_outputs_s &actuator_outputs);
	خالی update_Latency_Perf_شمار(ثابت actuator_outputs_s &actuator_outputs);

	خالی cleanupFunctions();

	خالی initParamHandles(ثابت uint8_t instance_آغاز);

	خالی حد_بندی_And_Update_Outputs(اعشاری outputs[MAX_ACTUATORS], منطقی has_updates);

	خالی output_حد_بندی_calc(ثابت منطقی armed, ثابت صحیح عدد_channels, ثابت اعشاری outputs[MAX_ACTUATORS]);

	ساخت ParamHandles {
		param_t function{PARAM_INVALID};
		param_t disarmed{PARAM_INVALID};
		param_t min{PARAM_INVALID};
		param_t center{PARAM_INVALID};
		param_t max{PARAM_INVALID};
		param_t failsafe{PARAM_INVALID};
	};

	خالی تالا() { کرو {} جب (px4_sem_wait(&_lock) != 0); }
	خالی unlock() { px4_sem_post(&_lock); }

	px4_sem_t _lock; /**< lock to protect access to work queue changes (includes ScheduleNow calls from another thread) */

	uint16_t failsafe_قدر[MAX_ACTUATORS] {};
	uint16_t disarmed_قدر[MAX_ACTUATORS] {};
	uint16_t min_قدر[MAX_ACTUATORS] {};
	uint16_t center_قدر[MAX_ACTUATORS] {};
	uint16_t max_قدر[MAX_ACTUATORS] {};
	اعشاری current_output_قدر[MAX_ACTUATORS] {}; ///< current output values (reordered)
	uint16_t الٹ_output_نقاب{0}; ///< reverses the interval [min, max] -> [max, min], NOT motor direction

	شمار جماعت Output_حد_بندی_State {
		OFF = 0,
		RAMP,
		ON
	} _output_state{Output_حد_بندی_State::OFF};

	hrt_abstime output_وقت_armed{0};
	ثابت منطقی output_ramp_اوپر; ///< if true, motors will ramp up from disarmed to min_output after arming

	uORB::Subscription _armed_sub{ORB_ID(actuator_armed)};

	uORB::PublicationMulti<actuator_outputs_s> _outputs_pub{ORB_ID(actuator_outputs)};

	actuator_armed_s _armed{};

	غیرعلامتی _max_topic_update_interval_us{0}; ///< max topic update interval (0=unlimited)

	منطقی _throttle_armed{جھوٹ};
	منطقی _ignore_lockdown{جھوٹ}; ///< if true, ignore the _armed.lockdown flag (for HIL outputs)

	ثابت SchedulingPolicy _scheduling_policy;
	ثابت منطقی _support_esc_calibration;

	منطقی _wq_switched{جھوٹ};
	uint8_t max_عدد_outputs_2;

	OutputModuleInterface &_interface;

	perf_شمار_t _control_latency_perf;

	Function_Provider_بنیاد *_function_allocated[MAX_ACTUATORS] {}; ///< unique allocated functions
	Function_Provider_بنیاد *_functions[MAX_ACTUATORS] {}; ///< currently assigned functions
	OutputFunction _function_assignment[MAX_ACTUATORS] {};
	منطقی _need_function_update{سچ};
	منطقی has_backup_جدول_وقت{جھوٹ};
	ثابت حرف *ثابت param_سابقہ_2;
	ParamHandles _param_handles[MAX_ACTUATORS];
	param_t _param_handle_rev_range{PARAM_INVALID};
	hrt_abstime lowrate_جدول_وقت_interval{300_ms};
	ActuatorTest _actuator_test{_function_assignment};
	uint32_t reversible_نقاب{0}; ///< per-output bits. If set, the output is configured to be reversible (motors only)
	منطقی _was_all_disabled{جھوٹ};

	uORB::Subscription_Callback_Work_شے *_subscription_callback{خالی_اشاریہ}; ///< current scheduling callback


	DEFINE_PARAMETERS(
		(ParamInt<px4::params::MC_AIRMODE>) _param_mc_airmode,   ///< multicopter air-mode
		(ParamFloat<px4::params::THR_MDL_FAC>) _param_thr_mdl_fac ///< thrust to motor control signal modelling factor
	)
};
