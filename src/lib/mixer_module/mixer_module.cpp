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

#شامل "mixer_module.hpp"

#شامل <uORB/Publication.hpp>
#شامل <px4_platform_common/log.h>

استعمال نامخلا وقت_literals;


ساخت FunctionProvider {
	استعمال Constructor = Function_Provider_بنیاد * (*)(ثابت Function_Provider_بنیاد::Context &context);
	constexpr FunctionProvider(OutputFunction min_func_, OutputFunction max_func_, Constructor constructor_)
		: min_func(min_func_), max_func(max_func_), constructor(constructor_) {}
	constexpr FunctionProvider(OutputFunction func, Constructor constructor_)
		: min_func(func), max_func(func), constructor(constructor_) {}

	OutputFunction min_func;
	OutputFunction max_func;
	Constructor constructor;
};

ساکن constexpr FunctionProvider all_function_providers[] = {
	// Providers higher up take precedence for subscription callback in case there are multiple
	{OutputFunction::Constant_Min, &FunctionConstantMin::allocate},
	{OutputFunction::Constant_Max, &FunctionConstantMax::allocate},
	{OutputFunction::Motor1, OutputFunction::MotorMax, &FunctionMotors::allocate},
	{OutputFunction::Servo1, OutputFunction::ServoMax, &FunctionServos::allocate},
	{OutputFunction::Peripheral_بذریعہ_Actuator_Set1, OutputFunction::Peripheral_بذریعہ_Actuator_Set6, &Function_Actuator_سیٹ::allocate},
	{OutputFunction::Landing_Gear, &FunctionLandingGear::allocate},
	{OutputFunction::Landing_Gear_Wheel, &FunctionLandingGearWheel::allocate},
	{OutputFunction::Parachute, &FunctionParachute::allocate},
	{OutputFunction::Gripper, &FunctionGripper::allocate},
	{OutputFunction::RC_Roll, OutputFunction::RC_AUXMax, &FunctionManualRC::allocate},
	{OutputFunction::Gimbal_Roll, OutputFunction::Gimbal_Yaw, &FunctionGimbal::allocate},
	{OutputFunction::IC_Engine_Ignition, OutputFunction::IC_Engine_Starter, &FunctionICEControl::allocate},
};

MixingOutput::MixingOutput(ثابت حرف *param_سابقہ, uint8_t max_عدد_outputs, OutputModuleInterface &interface,
			   SchedulingPolicy scheduling_policy, منطقی support_esc_calibration, منطقی ramp_اوپر, ثابت uint8_t instance_آغاز) :
	ModuleParams(&interface),
	output_ramp_اوپر(ramp_اوپر),
	_scheduling_policy(scheduling_policy),
	_support_esc_calibration(support_esc_calibration),
	max_عدد_outputs_2(max_عدد_outputs < MAX_ACTUATORS ? max_عدد_outputs : MAX_ACTUATORS),
	_interface(interface),
	_control_latency_perf(perf_alloc(PC_ELAPSED, "control latency")),
	param_سابقہ_2(param_سابقہ)
{
	/* Safely initialize armed flags */
	_armed.armed = جھوٹ;
	_armed.prearmed = جھوٹ;
	_armed.ready_مقصد_arm = جھوٹ;
	_armed.lockdown = جھوٹ;
	_armed.termination = جھوٹ;
	_armed.اندر_esc_calibration_منوال = جھوٹ;

	px4_sem_init(&_lock, 0, 1);

	initParamHandles(instance_آغاز);

	برائے (غیرعلامتی i = 0; i < MAX_ACTUATORS; i++) {
		failsafe_قدر[i] = UINT16_MAX;
	}

	updateParams();

	_outputs_pub.advertise();
}

MixingOutput::~MixingOutput()
{
	perf_free(_control_latency_perf);
	px4_sem_destroy(&_lock);

	cleanupFunctions();

	_outputs_pub.unadvertise();
}

خالی MixingOutput::initParamHandles(ثابت uint8_t instance_آغاز)
{
	حرف param_name[17];

	برائے (غیرعلامتی i = 0; i < max_عدد_outputs_2; ++i) {
		snprintf(param_name, حجم(param_name), "%s_%s%d", param_سابقہ_2, "FUNC", i + instance_آغاز);
		_param_handles[i].function = param_ڈھونڈو(param_name);
		snprintf(param_name, حجم(param_name), "%s_%s%d", param_سابقہ_2, "DIS", i + instance_آغاز);
		_param_handles[i].disarmed = param_ڈھونڈو(param_name);
		snprintf(param_name, حجم(param_name), "%s_%s%d", param_سابقہ_2, "MIN", i + instance_آغاز);
		_param_handles[i].min = param_ڈھونڈو(param_name);
		snprintf(param_name, حجم(param_name), "%s_%s%d", param_سابقہ_2, "CENT", i + instance_آغاز);
		_param_handles[i].center = param_ڈھونڈو(param_name);
		snprintf(param_name, حجم(param_name), "%s_%s%d", param_سابقہ_2, "MAX", i + instance_آغاز);
		_param_handles[i].max = param_ڈھونڈو(param_name);
		snprintf(param_name, حجم(param_name), "%s_%s%d", param_سابقہ_2, "FAIL", i + instance_آغاز);
		_param_handles[i].failsafe = param_ڈھونڈو(param_name);
	}

	snprintf(param_name, حجم(param_name), "%s_%s", param_سابقہ_2, "REV");
	_param_handle_rev_range = param_ڈھونڈو(param_name);
}

خالی MixingOutput::print_حالت_2() ثابت
{
	PX4_INFO("Param prefix: %s", param_سابقہ_2);
	perf_print_شمار(_control_latency_perf);

	اگر (_wq_switched) {
		PX4_INFO("Switched to rate_ctrl work queue");
	}

	PX4_INFO_RAW("Channel Configuration:\n");

	برائے (غیرعلامتی i = 0; i < max_عدد_outputs_2; i++) {
		PX4_INFO_RAW("Channel %2d: func: %3d, value: %.2f, failsafe: %.2f, disarmed: %d, min: %d, max: %d, center: %d\n",
			     i, (صحیح)_function_assignment[i], (دوہرا)current_output_قدر[i], (دوہرا)actual_Failsafe_قدر(i),
			     disarmed_قدر[i], min_قدر[i], max_قدر[i], center_قدر[i]);
	}
}

خالی MixingOutput::updateParams()
{
	ModuleParams::updateParams();

	منطقی function_بدلا = جھوٹ;

	برائے (غیرعلامتی i = 0; i < max_عدد_outputs_2; i++) {
		int32_t val;

		اگر (_param_handles[i].function != PARAM_INVALID && param_get(_param_handles[i].function, &val) == 0) {
			اگر (val != (int32_t)_function_assignment[i]) {
				function_بدلا = سچ;
			}

			// we set _function_assignment[i] later to ensure _functions[i] is updated at the same time
		}

		اگر (_param_handles[i].disarmed != PARAM_INVALID && param_get(_param_handles[i].disarmed, &val) == 0) {
			disarmed_قدر[i] = val;
		}

		اگر (_param_handles[i].min != PARAM_INVALID && param_get(_param_handles[i].min, &val) == 0) {
			min_قدر[i] = val;
		}

		اگر (_param_handles[i].center != PARAM_INVALID && param_get(_param_handles[i].center, &val) == 0) {
			center_قدر[i] = val;
		}

		اگر (_param_handles[i].max != PARAM_INVALID && param_get(_param_handles[i].max, &val) == 0) {
			max_قدر[i] = val;
		}

		اگر (min_قدر[i] > max_قدر[i]) {
			uint16_t tmp = min_قدر[i];
			min_قدر[i] = max_قدر[i];
			max_قدر[i] = tmp;
		}


		اگر (_param_handles[i].failsafe != PARAM_INVALID && param_get(_param_handles[i].failsafe, &val) == 0) {
			failsafe_قدر[i] = val;
		}
	}

	الٹ_output_نقاب = 0;
	int32_t rev_range_param;

	اگر (_param_handle_rev_range != PARAM_INVALID && param_get(_param_handle_rev_range, &rev_range_param) == 0) {
		الٹ_output_نقاب = rev_range_param;
	}

	اگر (function_بدلا) {
		_need_function_update = سچ;
	}
}

خالی MixingOutput::cleanupFunctions()
{
	اگر (_subscription_callback) {
		_subscription_callback->unregisterCallback();
		_subscription_callback = خالی_اشاریہ;
	}

	برائے (صحیح i = 0; i < MAX_ACTUATORS; ++i) {
		حذف _function_allocated[i];
		_function_allocated[i] = خالی_اشاریہ;
		_functions[i] = خالی_اشاریہ;
	}
}

منطقی MixingOutput::updateSubscriptions(منطقی allow_wq_switch)
{
	اگر (!_need_function_update || _armed.armed) {
		واپس جھوٹ;
	}

	// must be locked to potentially change WorkQueue
	تالا();

	has_backup_جدول_وقت = جھوٹ;

	اگر (_scheduling_policy == SchedulingPolicy::Auto) {
		// first clear everything
		unregister();
		_interface.جدول_وقت_Clear();

		منطقی switch_requested = جھوٹ;

		// potentially switch work queue if we run motor outputs
		برائے (غیرعلامتی i = 0; i < max_عدد_outputs_2; i++) {
			// read function directly from param, as _function_assignment[i] is updated later
			int32_t function;

			اگر (_param_handles[i].function != PARAM_INVALID && param_get(_param_handles[i].function, &function) == 0) {
				اگر (function >= (int32_t)OutputFunction::Motor1 && function <= (int32_t)OutputFunction::MotorMax) {
					switch_requested = سچ;
				}
			}
		}

		اگر (allow_wq_switch && !_wq_switched && switch_requested) {
			اگر (_interface.Change_Work_قطار(px4::wq_configurations::شرح_ctrl)) {
				// let the new WQ handle the subscribe update
				_wq_switched = سچ;
				_interface.جدول_وقت_Now();
				unlock();
				واپس جھوٹ;
			}
		}
	}

	// Now update the functions
	PX4_DEBUG("updating functions");

	cleanupFunctions();

	ثابت Function_Provider_بنیاد::Context context{_interface, _param_thr_mdl_fac.reference()};
	صحیح provider_indexes[MAX_ACTUATORS] {};
	صحیح اگلا_provider = 0;
	صحیح subscription_callback_provider_اشاریہ = زیادہ_سے_زیادہ_عدد;
	منطقی all_disabled = سچ;

	برائے (صحیح i = 0; i < max_عدد_outputs_2; ++i) {
		int32_t val;

		اگر (_param_handles[i].function != PARAM_INVALID && param_get(_param_handles[i].function, &val) == 0) {
			_function_assignment[i] = (OutputFunction)val;

		} ورنہ {
			_function_assignment[i] = OutputFunction::Disabled;
		}

		برائے (صحیح p = 0; p < (صحیح)(حجم(all_function_providers) / حجم(all_function_providers[0])); ++p) {
			اگر (_function_assignment[i] >= all_function_providers[p].min_func &&
			    _function_assignment[i] <= all_function_providers[p].max_func) {
				all_disabled = جھوٹ;
				صحیح ملا_اشاریہ = -1;

				برائے (صحیح existing = 0; existing < اگلا_provider; ++existing) {
					اگر (provider_indexes[existing] == p) {
						ملا_اشاریہ = existing;
						توڑو;
					}
				}

				اگر (ملا_اشاریہ >= 0) {
					_functions[i] = _function_allocated[ملا_اشاریہ];

				} ورنہ {
					_function_allocated[اگلا_provider] = all_function_providers[p].constructor(context);

					اگر (_function_allocated[اگلا_provider]) {
						_functions[i] = _function_allocated[اگلا_provider];
						provider_indexes[اگلا_provider++] = p;

						// lowest provider takes precedence for scheduling
						اگر (p < subscription_callback_provider_اشاریہ && _functions[i]->subscriptionCallback()) {
							subscription_callback_provider_اشاریہ = p;
							_subscription_callback = _functions[i]->subscriptionCallback();
						}

					} ورنہ {
						PX4_ERR("function alloc failed");
					}
				}

				توڑو;
			}
		}
	}

	hrt_abstime fixed_شرح_scheduling_interval = 4_ms; // schedule at 250Hz

	اگر (_max_topic_update_interval_us > fixed_شرح_scheduling_interval) {
		fixed_شرح_scheduling_interval = _max_topic_update_interval_us;
	}

	اگر (_scheduling_policy == SchedulingPolicy::Auto) {
		اگر (_subscription_callback) {
			اگر (_subscription_callback->رجسٹر_Callback()) {
				PX4_DEBUG("Scheduling via callback");
				has_backup_جدول_وقت = سچ;
				_interface.جدول_وقت_Delayed(50_ms);

			} ورنہ {
				PX4_ERR("registerCallback failed, scheduling at fixed rate");
				_interface.جدول_وقت_On_Interval(fixed_شرح_scheduling_interval);
			}

		} ورنہ اگر (all_disabled) {
			_interface.جدول_وقت_On_Interval(lowrate_جدول_وقت_interval);
			PX4_DEBUG("Scheduling at low rate");

		} ورنہ {
			_interface.جدول_وقت_On_Interval(fixed_شرح_scheduling_interval);
			PX4_DEBUG("Scheduling at fixed rate");
		}
	}

	سیٹ_Max_Topic_Update_شرح(_max_topic_update_interval_us);
	_need_function_update = جھوٹ;

	_actuator_test.reset();

	unlock();

	_interface.mixer_بدلا();

	واپس سچ;
}

خالی MixingOutput::سیٹ_Max_Topic_Update_شرح(غیرعلامتی max_topic_update_interval_us)
{
	_max_topic_update_interval_us = max_topic_update_interval_us;

	اگر (_subscription_callback) {
		_subscription_callback->سیٹ_interval_us(_max_topic_update_interval_us);
	}
}

خالی MixingOutput::سیٹ_All_Min_اقدار(uint16_t value)
{
	برائے (غیرعلامتی i = 0; i < MAX_ACTUATORS; i++) {
		_param_handles[i].min = PARAM_INVALID;
		min_قدر[i] = value;
	}
}

خالی MixingOutput::سیٹ_All_Center_اقدار(uint16_t value)
{
	برائے (غیرعلامتی i = 0; i < MAX_ACTUATORS; i++) {
		_param_handles[i].center = PARAM_INVALID;
		center_قدر[i] = value;
	}
}

خالی MixingOutput::سیٹ_All_Max_اقدار(uint16_t value)
{
	برائے (غیرعلامتی i = 0; i < MAX_ACTUATORS; i++) {
		_param_handles[i].max = PARAM_INVALID;
		max_قدر[i] = value;
	}
}

خالی MixingOutput::سیٹ_All_Failsafe_اقدار(uint16_t value)
{
	برائے (غیرعلامتی i = 0; i < MAX_ACTUATORS; i++) {
		_param_handles[i].failsafe = PARAM_INVALID;
		failsafe_قدر[i] = value;
	}
}

خالی MixingOutput::سیٹ_All_Disarmed_اقدار(uint16_t value)
{
	برائے (غیرعلامتی i = 0; i < MAX_ACTUATORS; i++) {
		_param_handles[i].disarmed = PARAM_INVALID;
		disarmed_قدر[i] = value;
	}
}

خالی MixingOutput::unregister()
{
	اگر (_subscription_callback) {
		_subscription_callback->unregisterCallback();
	}
}

منطقی MixingOutput::update()
{
	// check arming state
	اگر (_armed_sub.update(&_armed)) {
		_armed.اندر_esc_calibration_منوال &= _support_esc_calibration;

		اگر (_ignore_lockdown) {
			_armed.lockdown = جھوٹ;
		}

		/* Update the armed status and check that we're not locked down.
		 * We also need to arm throttle for the ESC calibration. */
		_throttle_armed = (_armed.armed && !_armed.lockdown) || _armed.اندر_esc_calibration_منوال;
	}

	// only used for sitl with lockstep
	منطقی has_updates = _subscription_callback && _subscription_callback->updated();

	// update topics
	برائے (صحیح i = 0; i < MAX_ACTUATORS && _function_allocated[i]; ++i) {
		_function_allocated[i]->update();
	}

	اگر (has_backup_جدول_وقت) {
		_interface.جدول_وقت_Delayed(50_ms);
	}

	// check for actuator test
	_actuator_test.update(max_عدد_outputs_2, _param_thr_mdl_fac.get());

	// get output values
	اعشاری outputs[MAX_ACTUATORS];
	منطقی all_disabled = سچ;
	ثابت uint32_t reversible_نقاب_پچھلا = reversible_نقاب;
	reversible_نقاب = 0;

	برائے (صحیح i = 0; i < max_عدد_outputs_2; ++i) {
		اگر (_functions[i]) {
			all_disabled = جھوٹ;

			اگر (_armed.armed || (_armed.prearmed && _functions[i]->allowPrearmControl())) {
				outputs[i] = _functions[i]->value(_function_assignment[i]);

			} ورنہ {
				outputs[i] = NAN;
			}

			reversible_نقاب |= (uint32_t)_functions[i]->reversible(_function_assignment[i]) << i;

		} ورنہ {
			outputs[i] = NAN;
		}
	}

	اگر (reversible_نقاب != reversible_نقاب_پچھلا) {
		_interface.reversible_نقاب_بدلا(reversible_نقاب);
	}

	// Send output if any function mapped or one last disabling sample
	اگر (!all_disabled || !_was_all_disabled) {
		اگر (!_armed.armed && !_armed.kill) {
			_actuator_test.override_اقدار(outputs, max_عدد_outputs_2);
		}

		حد_بندی_And_Update_Outputs(outputs, has_updates);
	}

	_was_all_disabled = all_disabled;

	واپس سچ;
}

خالی
MixingOutput::حد_بندی_And_Update_Outputs(اعشاری outputs[MAX_ACTUATORS], منطقی has_updates)
{
	اگر (_armed.lockdown || _armed.kill) {
		// overwrite outputs in case of lockdown with disarmed values
		برائے (size_t i = 0; i < max_عدد_outputs_2; i++) {
			current_output_قدر[i] = disarmed_قدر[i];
		}

	} ورنہ اگر (_armed.termination) {
		// Overwrite outputs with _failsafe_value when terminated
		برائے (size_t i = 0; i < max_عدد_outputs_2; i++) {
			current_output_قدر[i] = actual_Failsafe_قدر(i);
		}

	} ورنہ {
		// the output limit call takes care of out of band errors, NaN and constrains
		output_حد_بندی_calc(_throttle_armed || _actuator_test.اندر_Test_منوال(), max_عدد_outputs_2, outputs);
	}

	// We must calibrate the PWM and Oneshot ESCs to a consistent range of 1000-2000us (gets mapped to 125-250us for Oneshot)
	// Doing so makes calibrations consistent among different configurations and hence PWM minimum and maximum have a consistent effect
	// hence the defaults for these parameters also make most setups work out of the box
	اگر (_armed.اندر_esc_calibration_منوال) {
		ساکن constexpr اعشاری PWM_CALIBRATION_کم = 1000.f;
		ساکن constexpr اعشاری PWM_CALIBRATION_بلند = 2000.f;

		برائے (صحیح i = 0; i < max_عدد_outputs_2; i++) {
			اگر (fabsf(current_output_قدر[i] - (اعشاری)min_قدر[i]) < 0.5f) {
				current_output_قدر[i] = PWM_CALIBRATION_کم;
			}

			اگر (fabsf(current_output_قدر[i] - (اعشاری)max_قدر[i]) < 0.5f) {
				current_output_قدر[i] = PWM_CALIBRATION_بلند;
			}
		}
	}

	/* now return the outputs to the driver */
	اگر (_interface.updateOutputs(current_output_قدر, max_عدد_outputs_2, has_updates)) {
		actuator_outputs_s actuator_outputs{};
		سیٹ_And_Publish_Actuator_Outputs(max_عدد_outputs_2, actuator_outputs);

		update_Latency_Perf_شمار(actuator_outputs);
	}
}

اعشاری MixingOutput::output_حد_بندی_calc_single(صحیح i, اعشاری value) ثابت
{
	// check for invalid / disabled channels
	اگر (!PX4_ISFINITE(value)) {
		واپس disarmed_قدر[i];
	}

	اگر (الٹ_output_نقاب & (1 << i)) {
		value = -1.f * value;
	}

	اعشاری output = disarmed_قدر[i];

	اگر (((_function_assignment[i] >= OutputFunction::Servo1
	      && _function_assignment[i] <= OutputFunction::ServoMax) || _function_assignment[i] == OutputFunction::Landing_Gear_Wheel
	     || (_function_assignment[i] >= OutputFunction::Gimbal_Roll
		 && _function_assignment[i] <= OutputFunction::Gimbal_Yaw))
	    && _param_handles[i].center != PARAM_INVALID
	    && center_قدر[i] >= min_قدر[i]
	    && center_قدر[i] <= max_قدر[i]) {
		output = math::interpolateNXY(value, {-1.f, 0.f, 1.f}, {(اعشاری)min_قدر[i], (اعشاری)center_قدر[i], (اعشاری)max_قدر[i]});
	}

	// Everything except servos, or if center is not set
	ورنہ {
		output = math::interpolate(value, -1.f, 1.f, static_cast<اعشاری>(min_قدر[i]), static_cast<اعشاری>(max_قدر[i]));
	}

	واپس output;
}

خالی
MixingOutput::output_حد_بندی_calc(ثابت منطقی armed, ثابت صحیح عدد_channels, ثابت اعشاری output[MAX_ACTUATORS])
{
	// time to slowly ramp up the ESCs
	ساکن constexpr hrt_abstime RAMP_وقت_US = 500_ms;

	/* first evaluate state changes */
	انتخاب (_output_state) {
	حالت Output_حد_بندی_State::OFF:
		اگر (armed) {
			اگر (output_ramp_اوپر) {
				_output_state = Output_حد_بندی_State::RAMP;

			} ورنہ {
				_output_state = Output_حد_بندی_State::ON;
			}

			// reset arming time, used for ramp timing
			output_وقت_armed = hrt_absolute_وقت();
		}

		توڑو;

	حالت Output_حد_بندی_State::RAMP:
		اگر (!armed) {
			_output_state = Output_حد_بندی_State::OFF;

		} ورنہ اگر (hrt_elapsed_وقت(&output_وقت_armed) >= RAMP_وقت_US) {
			_output_state = Output_حد_بندی_State::ON;
		}

		توڑو;

	حالت Output_حد_بندی_State::ON:
		اگر (!armed) {
			_output_state = Output_حد_بندی_State::OFF;
		}

		توڑو;
	}

	/* if the system is pre-armed, the limit state is temporarily on,
	 * as some outputs are valid and the non-valid outputs have been
	 * set to NaN. This is not stored in the state machine though,
	 * as the throttle channels need to go through the ramp at
	 * regular arming time.
	 */
	خودکار local_حد_بندی_state = _output_state;

	اگر (isPrearmed()) {
		local_حد_بندی_state = Output_حد_بندی_State::ON;
	}

	// then set _current_output_value based on state
	انتخاب (local_حد_بندی_state) {
	حالت Output_حد_بندی_State::OFF:
		برائے (صحیح i = 0; i < عدد_channels; i++) {
			current_output_قدر[i] = disarmed_قدر[i];
		}

		توڑو;

	حالت Output_حد_بندی_State::RAMP: {
			hrt_abstime diff = hrt_elapsed_وقت(&output_وقت_armed);
			اعشاری پیش_رفت = static_cast<اعشاری>(diff) / RAMP_وقت_US;

			اگر (پیش_رفت > 1.f) {
				پیش_رفت = 1.f;
			}

			برائے (صحیح i = 0; i < عدد_channels; i++) {
				// Ramp from disarmed value to currently desired output that would apply without ramp
				اعشاری desired_output = output_حد_بندی_calc_single(i, output[i]);
				current_output_قدر[i] = disarmed_قدر[i] + پیش_رفت * (desired_output - disarmed_قدر[i]);
			}
		}
		توڑو;

	حالت Output_حد_بندی_State::ON:
		برائے (صحیح i = 0; i < عدد_channels; i++) {
			current_output_قدر[i] = output_حد_بندی_calc_single(i, output[i]);
		}

		توڑو;
	}
}

خالی
MixingOutput::سیٹ_And_Publish_Actuator_Outputs(غیرعلامتی عدد_outputs, actuator_outputs_s &actuator_outputs)
{
	actuator_outputs.noutputs = عدد_outputs;

	برائے (size_t i = 0; i < عدد_outputs; ++i) {
		actuator_outputs.output[i] = current_output_قدر[i];
	}

	actuator_outputs.timestamp = hrt_absolute_وقت();
	_outputs_pub.publish(actuator_outputs);
}

خالی
MixingOutput::update_Latency_Perf_شمار(ثابت actuator_outputs_s &actuator_outputs)
{
	// Just check the first function. It means we only get the latency if motors are assigned first, which is the default
	اگر (_function_allocated[0]) {
		hrt_abstime timestamp_sample;

		اگر (_function_allocated[0]->getLatestSampleTimestamp(timestamp_sample)) {
			perf_سیٹ_elapsed(_control_latency_perf, actuator_outputs.timestamp - timestamp_sample);
		}
	}
}

اعشاری MixingOutput::actual_Failsafe_قدر(صحیح اشاریہ) ثابت
{
	اعشاری value = 0;

	اگر (failsafe_قدر[اشاریہ] == UINT16_MAX) { // if set to default, use the one provided by the function
		اعشاری default_failsafe = NAN;

		اگر (_functions[اشاریہ]) {
			default_failsafe = _functions[اشاریہ]->default_Failsafe_قدر(_function_assignment[اشاریہ]);
		}

		value = output_حد_بندی_calc_single(اشاریہ, default_failsafe);

	} ورنہ {
		value = static_cast<اعشاری>(failsafe_قدر[اشاریہ]);
	}

	واپس value;
}
