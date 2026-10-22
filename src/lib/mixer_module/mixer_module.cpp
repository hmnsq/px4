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

using فضاینام time_literals;


ساختار FunctionProvider {
	using Constructor = FunctionProviderBase * (*)(ثابت FunctionProviderBase::Context &context);
	constexpr FunctionProvider(OutputFunction min_func_, OutputFunction max_func_, Constructor constructor_)
		: min_func(min_func_), max_func(max_func_), constructor(constructor_) {}
	constexpr FunctionProvider(OutputFunction func, Constructor constructor_)
		: min_func(func), max_func(func), constructor(constructor_) {}

	OutputFunction min_func;
	OutputFunction max_func;
	Constructor constructor;
};

ایستا constexpr FunctionProvider all_function_providers[] = {
	// Providers higher up take precedence for subscription callback in case there are multiple
	{OutputFunction::Constant_Min, &FunctionConstantMin::allocate},
	{OutputFunction::Constant_Max, &FunctionConstantMax::allocate},
	{OutputFunction::Motor1, OutputFunction::MotorMax, &FunctionMotors::allocate},
	{OutputFunction::Servo1, OutputFunction::ServoMax, &FunctionServos::allocate},
	{OutputFunction::Peripheral_via_Actuator_Set1, OutputFunction::Peripheral_via_Actuator_Set6, &FunctionActuatorSet::allocate},
	{OutputFunction::Landing_Gear, &FunctionLandingGear::allocate},
	{OutputFunction::Landing_Gear_Wheel, &FunctionLandingGearWheel::allocate},
	{OutputFunction::Parachute, &FunctionParachute::allocate},
	{OutputFunction::Gripper, &FunctionGripper::allocate},
	{OutputFunction::RC_Roll, OutputFunction::RC_AUXMax, &FunctionManualRC::allocate},
	{OutputFunction::Gimbal_Roll, OutputFunction::Gimbal_Yaw, &FunctionGimbal::allocate},
	{OutputFunction::IC_Engine_Ignition, OutputFunction::IC_Engine_Starter, &FunctionICEControl::allocate},
};

MixingOutput::MixingOutput(ثابت نویسه *param_prefix, uint8_t max_رقم_outputs, OutputModuleInterface &interface,
			   SchedulingPolicy scheduling_policy, منطقی support_esc_calibration, منطقی ramp_up, ثابت uint8_t instance_start) :
	ModuleParams(&interface),
	_output_ramp_up(ramp_up),
	_scheduling_policy(scheduling_policy),
	_support_esc_calibration(support_esc_calibration),
	max_رقم_outputs_2(max_رقم_outputs < MAX_ACTUATORS ? max_رقم_outputs : MAX_ACTUATORS),
	_interface(interface),
	_control_latency_perf(perf_alloc(PC_ELAPSED, "control latency")),
	_param_prefix(param_prefix)
{
	/* Safely initialize armed flags */
	_armed.armed = نادرست;
	_armed.prearmed = نادرست;
	_armed.ready_to_arm = نادرست;
	_armed.lockdown = نادرست;
	_armed.termination = نادرست;
	_armed.in_esc_calibration_mode = نادرست;

	px4_sem_init(&_lock, 0, 1);

	initParamHandles(instance_start);

	برای (unsigned i = 0; i < MAX_ACTUATORS; i++) {
		failsafe_مقدار[i] = UINT16_MAX;
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

تهی MixingOutput::initParamHandles(ثابت uint8_t instance_start)
{
	نویسه param_name[17];

	برای (unsigned i = 0; i < max_رقم_outputs_2; ++i) {
		snprintf(param_name, اندازه(param_name), "%s_%s%d", _param_prefix, "FUNC", i + instance_start);
		_param_handles[i].function = param_find(param_name);
		snprintf(param_name, اندازه(param_name), "%s_%s%d", _param_prefix, "DIS", i + instance_start);
		_param_handles[i].disarmed = param_find(param_name);
		snprintf(param_name, اندازه(param_name), "%s_%s%d", _param_prefix, "MIN", i + instance_start);
		_param_handles[i].min = param_find(param_name);
		snprintf(param_name, اندازه(param_name), "%s_%s%d", _param_prefix, "CENT", i + instance_start);
		_param_handles[i].center = param_find(param_name);
		snprintf(param_name, اندازه(param_name), "%s_%s%d", _param_prefix, "MAX", i + instance_start);
		_param_handles[i].max = param_find(param_name);
		snprintf(param_name, اندازه(param_name), "%s_%s%d", _param_prefix, "FAIL", i + instance_start);
		_param_handles[i].failsafe = param_find(param_name);
	}

	snprintf(param_name, اندازه(param_name), "%s_%s", _param_prefix, "REV");
	_param_handle_rev_range = param_find(param_name);
}

تهی MixingOutput::printStatus() ثابت
{
	PX4_INFO("Param prefix: %s", _param_prefix);
	perf_print_counter(_control_latency_perf);

	اگر (_wq_switched) {
		PX4_INFO("Switched to rate_ctrl work queue");
	}

	PX4_INFO_RAW("Channel Configuration:\n");

	برای (unsigned i = 0; i < max_رقم_outputs_2; i++) {
		PX4_INFO_RAW("Channel %2d: func: %3d, value: %.2f, failsafe: %.2f, disarmed: %d, min: %d, max: %d, center: %d\n",
			     i, (صحیح)_function_assignment[i], (دوگانه)current_output_مقدار[i], (دوگانه)actual_Failsafe_مقدار(i),
			     disarmed_مقدار[i], min_مقدار[i], max_مقدار[i], center_مقدار[i]);
	}
}

تهی MixingOutput::updateParams()
{
	ModuleParams::updateParams();

	منطقی function_changed = نادرست;

	برای (unsigned i = 0; i < max_رقم_outputs_2; i++) {
		int32_t val;

		اگر (_param_handles[i].function != PARAM_INVALID && param_get(_param_handles[i].function, &val) == 0) {
			اگر (val != (int32_t)_function_assignment[i]) {
				function_changed = درست;
			}

			// we set _function_assignment[i] later to ensure _functions[i] is updated at the same time
		}

		اگر (_param_handles[i].disarmed != PARAM_INVALID && param_get(_param_handles[i].disarmed, &val) == 0) {
			disarmed_مقدار[i] = val;
		}

		اگر (_param_handles[i].min != PARAM_INVALID && param_get(_param_handles[i].min, &val) == 0) {
			min_مقدار[i] = val;
		}

		اگر (_param_handles[i].center != PARAM_INVALID && param_get(_param_handles[i].center, &val) == 0) {
			center_مقدار[i] = val;
		}

		اگر (_param_handles[i].max != PARAM_INVALID && param_get(_param_handles[i].max, &val) == 0) {
			max_مقدار[i] = val;
		}

		اگر (min_مقدار[i] > max_مقدار[i]) {
			uint16_t tmp = min_مقدار[i];
			min_مقدار[i] = max_مقدار[i];
			max_مقدار[i] = tmp;
		}


		اگر (_param_handles[i].failsafe != PARAM_INVALID && param_get(_param_handles[i].failsafe, &val) == 0) {
			failsafe_مقدار[i] = val;
		}
	}

	_reverse_output_mask = 0;
	int32_t rev_range_param;

	اگر (_param_handle_rev_range != PARAM_INVALID && param_get(_param_handle_rev_range, &rev_range_param) == 0) {
		_reverse_output_mask = rev_range_param;
	}

	اگر (function_changed) {
		_need_function_update = درست;
	}
}

تهی MixingOutput::cleanupFunctions()
{
	اگر (_subscription_callback) {
		_subscription_callback->unregisterCallback();
		_subscription_callback = nullptr;
	}

	برای (صحیح i = 0; i < MAX_ACTUATORS; ++i) {
		delete _function_allocated[i];
		_function_allocated[i] = nullptr;
		_functions[i] = nullptr;
	}
}

منطقی MixingOutput::updateSubscriptions(منطقی allow_wq_switch)
{
	اگر (!_need_function_update || _armed.armed) {
		بازگشت نادرست;
	}

	// must be locked to potentially change WorkQueue
	lock();

	_has_backup_schedule = نادرست;

	اگر (_scheduling_policy == SchedulingPolicy::Auto) {
		// first clear everything
		unregister();
		_interface.ScheduleClear();

		منطقی switch_requested = نادرست;

		// potentially switch work queue if we run motor outputs
		برای (unsigned i = 0; i < max_رقم_outputs_2; i++) {
			// read function directly from param, as _function_assignment[i] is updated later
			int32_t function;

			اگر (_param_handles[i].function != PARAM_INVALID && param_get(_param_handles[i].function, &function) == 0) {
				اگر (function >= (int32_t)OutputFunction::Motor1 && function <= (int32_t)OutputFunction::MotorMax) {
					switch_requested = درست;
				}
			}
		}

		اگر (allow_wq_switch && !_wq_switched && switch_requested) {
			اگر (_interface.ChangeWorkQueue(px4::wq_configurations::rate_ctrl)) {
				// let the new WQ handle the subscribe update
				_wq_switched = درست;
				_interface.ScheduleNow();
				unlock();
				بازگشت نادرست;
			}
		}
	}

	// Now update the functions
	PX4_DEBUG("updating functions");

	cleanupFunctions();

	ثابت FunctionProviderBase::Context context{_interface, _param_thr_mdl_fac.reference()};
	صحیح provider_indexes[MAX_ACTUATORS] {};
	صحیح next_provider = 0;
	صحیح subscription_callback_provider_index = INT_MAX;
	منطقی all_disabled = درست;

	برای (صحیح i = 0; i < max_رقم_outputs_2; ++i) {
		int32_t val;

		اگر (_param_handles[i].function != PARAM_INVALID && param_get(_param_handles[i].function, &val) == 0) {
			_function_assignment[i] = (OutputFunction)val;

		} وگرنه {
			_function_assignment[i] = OutputFunction::Disabled;
		}

		برای (صحیح p = 0; p < (صحیح)(اندازه(all_function_providers) / اندازه(all_function_providers[0])); ++p) {
			اگر (_function_assignment[i] >= all_function_providers[p].min_func &&
			    _function_assignment[i] <= all_function_providers[p].max_func) {
				all_disabled = نادرست;
				صحیح found_index = -1;

				برای (صحیح existing = 0; existing < next_provider; ++existing) {
					اگر (provider_indexes[existing] == p) {
						found_index = existing;
						بشکن;
					}
				}

				اگر (found_index >= 0) {
					_functions[i] = _function_allocated[found_index];

				} وگرنه {
					_function_allocated[next_provider] = all_function_providers[p].constructor(context);

					اگر (_function_allocated[next_provider]) {
						_functions[i] = _function_allocated[next_provider];
						provider_indexes[next_provider++] = p;

						// lowest provider takes precedence for scheduling
						اگر (p < subscription_callback_provider_index && _functions[i]->subscriptionCallback()) {
							subscription_callback_provider_index = p;
							_subscription_callback = _functions[i]->subscriptionCallback();
						}

					} وگرنه {
						PX4_ERR("function alloc failed");
					}
				}

				بشکن;
			}
		}
	}

	hrt_abstime fixed_rate_scheduling_interval = 4_ms; // schedule at 250Hz

	اگر (_max_topic_update_interval_us > fixed_rate_scheduling_interval) {
		fixed_rate_scheduling_interval = _max_topic_update_interval_us;
	}

	اگر (_scheduling_policy == SchedulingPolicy::Auto) {
		اگر (_subscription_callback) {
			اگر (_subscription_callback->registerCallback()) {
				PX4_DEBUG("Scheduling via callback");
				_has_backup_schedule = درست;
				_interface.ScheduleDelayed(50_ms);

			} وگرنه {
				PX4_ERR("registerCallback failed, scheduling at fixed rate");
				_interface.ScheduleOnInterval(fixed_rate_scheduling_interval);
			}

		} وگرنه اگر (all_disabled) {
			_interface.ScheduleOnInterval(_lowrate_schedule_interval);
			PX4_DEBUG("Scheduling at low rate");

		} وگرنه {
			_interface.ScheduleOnInterval(fixed_rate_scheduling_interval);
			PX4_DEBUG("Scheduling at fixed rate");
		}
	}

	setMaxTopicUpdateRate(_max_topic_update_interval_us);
	_need_function_update = نادرست;

	_actuator_test.reset();

	unlock();

	_interface.mixerChanged();

	بازگشت درست;
}

تهی MixingOutput::setMaxTopicUpdateRate(unsigned max_topic_update_interval_us)
{
	_max_topic_update_interval_us = max_topic_update_interval_us;

	اگر (_subscription_callback) {
		_subscription_callback->set_interval_us(_max_topic_update_interval_us);
	}
}

تهی MixingOutput::set_All_Min_مقادیر(uint16_t value)
{
	برای (unsigned i = 0; i < MAX_ACTUATORS; i++) {
		_param_handles[i].min = PARAM_INVALID;
		min_مقدار[i] = value;
	}
}

تهی MixingOutput::set_All_Center_مقادیر(uint16_t value)
{
	برای (unsigned i = 0; i < MAX_ACTUATORS; i++) {
		_param_handles[i].center = PARAM_INVALID;
		center_مقدار[i] = value;
	}
}

تهی MixingOutput::set_All_Max_مقادیر(uint16_t value)
{
	برای (unsigned i = 0; i < MAX_ACTUATORS; i++) {
		_param_handles[i].max = PARAM_INVALID;
		max_مقدار[i] = value;
	}
}

تهی MixingOutput::set_All_Failsafe_مقادیر(uint16_t value)
{
	برای (unsigned i = 0; i < MAX_ACTUATORS; i++) {
		_param_handles[i].failsafe = PARAM_INVALID;
		failsafe_مقدار[i] = value;
	}
}

تهی MixingOutput::set_All_Disarmed_مقادیر(uint16_t value)
{
	برای (unsigned i = 0; i < MAX_ACTUATORS; i++) {
		_param_handles[i].disarmed = PARAM_INVALID;
		disarmed_مقدار[i] = value;
	}
}

تهی MixingOutput::unregister()
{
	اگر (_subscription_callback) {
		_subscription_callback->unregisterCallback();
	}
}

منطقی MixingOutput::update()
{
	// check arming state
	اگر (_armed_sub.update(&_armed)) {
		_armed.in_esc_calibration_mode &= _support_esc_calibration;

		اگر (_ignore_lockdown) {
			_armed.lockdown = نادرست;
		}

		/* Update the armed status and check that we're not locked down.
		 * We also need to arm throttle for the ESC calibration. */
		_throttle_armed = (_armed.armed && !_armed.lockdown) || _armed.in_esc_calibration_mode;
	}

	// only used for sitl with lockstep
	منطقی has_updates = _subscription_callback && _subscription_callback->updated();

	// update topics
	برای (صحیح i = 0; i < MAX_ACTUATORS && _function_allocated[i]; ++i) {
		_function_allocated[i]->update();
	}

	اگر (_has_backup_schedule) {
		_interface.ScheduleDelayed(50_ms);
	}

	// check for actuator test
	_actuator_test.update(max_رقم_outputs_2, _param_thr_mdl_fac.get());

	// get output values
	اعشاری outputs[MAX_ACTUATORS];
	منطقی all_disabled = درست;
	ثابت uint32_t reversible_mask_prev = _reversible_mask;
	_reversible_mask = 0;

	برای (صحیح i = 0; i < max_رقم_outputs_2; ++i) {
		اگر (_functions[i]) {
			all_disabled = نادرست;

			اگر (_armed.armed || (_armed.prearmed && _functions[i]->allowPrearmControl())) {
				outputs[i] = _functions[i]->value(_function_assignment[i]);

			} وگرنه {
				outputs[i] = NAN;
			}

			_reversible_mask |= (uint32_t)_functions[i]->reversible(_function_assignment[i]) << i;

		} وگرنه {
			outputs[i] = NAN;
		}
	}

	اگر (_reversible_mask != reversible_mask_prev) {
		_interface.reversibleMaskChanged(_reversible_mask);
	}

	// Send output if any function mapped or one last disabling sample
	اگر (!all_disabled || !_was_all_disabled) {
		اگر (!_armed.armed && !_armed.kill) {
			_actuator_test.override_مقادیر(outputs, max_رقم_outputs_2);
		}

		limitAndUpdateOutputs(outputs, has_updates);
	}

	_was_all_disabled = all_disabled;

	بازگشت درست;
}

تهی
MixingOutput::limitAndUpdateOutputs(اعشاری outputs[MAX_ACTUATORS], منطقی has_updates)
{
	اگر (_armed.lockdown || _armed.kill) {
		// overwrite outputs in case of lockdown with disarmed values
		برای (size_t i = 0; i < max_رقم_outputs_2; i++) {
			current_output_مقدار[i] = disarmed_مقدار[i];
		}

	} وگرنه اگر (_armed.termination) {
		// Overwrite outputs with _failsafe_value when terminated
		برای (size_t i = 0; i < max_رقم_outputs_2; i++) {
			current_output_مقدار[i] = actual_Failsafe_مقدار(i);
		}

	} وگرنه {
		// the output limit call takes care of out of band errors, NaN and constrains
		output_limit_calc(_throttle_armed || _actuator_test.inTestMode(), max_رقم_outputs_2, outputs);
	}

	// We must calibrate the PWM and Oneshot ESCs to a consistent range of 1000-2000us (gets mapped to 125-250us for Oneshot)
	// Doing so makes calibrations consistent among different configurations and hence PWM minimum and maximum have a consistent effect
	// hence the defaults for these parameters also make most setups work out of the box
	اگر (_armed.in_esc_calibration_mode) {
		ایستا constexpr اعشاری PWM_CALIBRATION_کم = 1000.f;
		ایستا constexpr اعشاری PWM_CALIBRATION_بالا = 2000.f;

		برای (صحیح i = 0; i < max_رقم_outputs_2; i++) {
			اگر (fabsf(current_output_مقدار[i] - (اعشاری)min_مقدار[i]) < 0.5f) {
				current_output_مقدار[i] = PWM_CALIBRATION_کم;
			}

			اگر (fabsf(current_output_مقدار[i] - (اعشاری)max_مقدار[i]) < 0.5f) {
				current_output_مقدار[i] = PWM_CALIBRATION_بالا;
			}
		}
	}

	/* now return the outputs to the driver */
	اگر (_interface.updateOutputs(current_output_مقدار, max_رقم_outputs_2, has_updates)) {
		actuator_outputs_s actuator_outputs{};
		setAndPublishActuatorOutputs(max_رقم_outputs_2, actuator_outputs);

		updateLatencyPerfCounter(actuator_outputs);
	}
}

اعشاری MixingOutput::output_limit_calc_single(صحیح i, اعشاری value) ثابت
{
	// check for invalid / disabled channels
	اگر (!PX4_ISFINITE(value)) {
		بازگشت disarmed_مقدار[i];
	}

	اگر (_reverse_output_mask & (1 << i)) {
		value = -1.f * value;
	}

	اعشاری output = disarmed_مقدار[i];

	اگر (((_function_assignment[i] >= OutputFunction::Servo1
	      && _function_assignment[i] <= OutputFunction::ServoMax) || _function_assignment[i] == OutputFunction::Landing_Gear_Wheel
	     || (_function_assignment[i] >= OutputFunction::Gimbal_Roll
		 && _function_assignment[i] <= OutputFunction::Gimbal_Yaw))
	    && _param_handles[i].center != PARAM_INVALID
	    && center_مقدار[i] >= min_مقدار[i]
	    && center_مقدار[i] <= max_مقدار[i]) {
		output = math::interpolateNXY(value, {-1.f, 0.f, 1.f}, {(اعشاری)min_مقدار[i], (اعشاری)center_مقدار[i], (اعشاری)max_مقدار[i]});
	}

	// Everything except servos, or if center is not set
	وگرنه {
		output = math::interpolate(value, -1.f, 1.f, static_cast<اعشاری>(min_مقدار[i]), static_cast<اعشاری>(max_مقدار[i]));
	}

	بازگشت output;
}

تهی
MixingOutput::output_limit_calc(ثابت منطقی armed, ثابت صحیح رقم_channels, ثابت اعشاری output[MAX_ACTUATORS])
{
	// time to slowly ramp up the ESCs
	ایستا constexpr hrt_abstime RAMP_TIME_US = 500_ms;

	/* first evaluate state changes */
	انتخاب (_output_state) {
	حالت OutputLimitState::OFF:
		اگر (armed) {
			اگر (_output_ramp_up) {
				_output_state = OutputLimitState::RAMP;

			} وگرنه {
				_output_state = OutputLimitState::ON;
			}

			// reset arming time, used for ramp timing
			_output_time_armed = hrt_absolute_time();
		}

		بشکن;

	حالت OutputLimitState::RAMP:
		اگر (!armed) {
			_output_state = OutputLimitState::OFF;

		} وگرنه اگر (hrt_elapsed_time(&_output_time_armed) >= RAMP_TIME_US) {
			_output_state = OutputLimitState::ON;
		}

		بشکن;

	حالت OutputLimitState::ON:
		اگر (!armed) {
			_output_state = OutputLimitState::OFF;
		}

		بشکن;
	}

	/* if the system is pre-armed, the limit state is temporarily on,
	 * as some outputs are valid and the non-valid outputs have been
	 * set to NaN. This is not stored in the state machine though,
	 * as the throttle channels need to go through the ramp at
	 * regular arming time.
	 */
	auto local_limit_state = _output_state;

	اگر (isPrearmed()) {
		local_limit_state = OutputLimitState::ON;
	}

	// then set _current_output_value based on state
	انتخاب (local_limit_state) {
	حالت OutputLimitState::OFF:
		برای (صحیح i = 0; i < رقم_channels; i++) {
			current_output_مقدار[i] = disarmed_مقدار[i];
		}

		بشکن;

	حالت OutputLimitState::RAMP: {
			hrt_abstime diff = hrt_elapsed_time(&_output_time_armed);
			اعشاری progress = static_cast<اعشاری>(diff) / RAMP_TIME_US;

			اگر (progress > 1.f) {
				progress = 1.f;
			}

			برای (صحیح i = 0; i < رقم_channels; i++) {
				// Ramp from disarmed value to currently desired output that would apply without ramp
				اعشاری desired_output = output_limit_calc_single(i, output[i]);
				current_output_مقدار[i] = disarmed_مقدار[i] + progress * (desired_output - disarmed_مقدار[i]);
			}
		}
		بشکن;

	حالت OutputLimitState::ON:
		برای (صحیح i = 0; i < رقم_channels; i++) {
			current_output_مقدار[i] = output_limit_calc_single(i, output[i]);
		}

		بشکن;
	}
}

تهی
MixingOutput::setAndPublishActuatorOutputs(unsigned رقم_outputs, actuator_outputs_s &actuator_outputs)
{
	actuator_outputs.noutputs = رقم_outputs;

	برای (size_t i = 0; i < رقم_outputs; ++i) {
		actuator_outputs.output[i] = current_output_مقدار[i];
	}

	actuator_outputs.timestamp = hrt_absolute_time();
	_outputs_pub.publish(actuator_outputs);
}

تهی
MixingOutput::updateLatencyPerfCounter(ثابت actuator_outputs_s &actuator_outputs)
{
	// Just check the first function. It means we only get the latency if motors are assigned first, which is the default
	اگر (_function_allocated[0]) {
		hrt_abstime timestamp_sample;

		اگر (_function_allocated[0]->getLatestSampleTimestamp(timestamp_sample)) {
			perf_set_elapsed(_control_latency_perf, actuator_outputs.timestamp - timestamp_sample);
		}
	}
}

اعشاری MixingOutput::actual_Failsafe_مقدار(صحیح index) ثابت
{
	اعشاری value = 0;

	اگر (failsafe_مقدار[index] == UINT16_MAX) { // if set to default, use the one provided by the function
		اعشاری default_failsafe = NAN;

		اگر (_functions[index]) {
			default_failsafe = _functions[index]->default_Failsafe_مقدار(_function_assignment[index]);
		}

		value = output_limit_calc_single(index, default_failsafe);

	} وگرنه {
		value = static_cast<اعشاری>(failsafe_مقدار[index]);
	}

	بازگشت value;
}
