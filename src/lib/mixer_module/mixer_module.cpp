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

#অন্তর্ভাব "mixer_module.hpp"

#অন্তর্ভাব <uORB/Publication.hpp>
#অন্তর্ভাব <px4_platform_common/log.h>

using নামাকাশ time_literals;


কাঠামো FunctionProvider {
	using Constructor = FunctionProviderBase * (*)(ধ্রুবক FunctionProviderBase::Context &context);
	constexpr FunctionProvider(OutputFunction min_func_, OutputFunction max_func_, Constructor constructor_)
		: min_func(min_func_), max_func(max_func_), constructor(constructor_) {}
	constexpr FunctionProvider(OutputFunction func, Constructor constructor_)
		: min_func(func), max_func(func), constructor(constructor_) {}

	OutputFunction min_func;
	OutputFunction max_func;
	Constructor constructor;
};

স্থির constexpr FunctionProvider all_function_providers[] = {
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

MixingOutput::MixingOutput(ধ্রুবক অক্ষর *param_prefix, uint8_t max_অঙ্ক_outputs, OutputModuleInterface &interface,
			   SchedulingPolicy scheduling_policy, যৌক্তিক support_esc_calibration, যৌক্তিক ramp_up, ধ্রুবক uint8_t instance_start) :
	ModuleParams(&interface),
	_output_ramp_up(ramp_up),
	_scheduling_policy(scheduling_policy),
	_support_esc_calibration(support_esc_calibration),
	max_অঙ্ক_outputs_2(max_অঙ্ক_outputs < MAX_ACTUATORS ? max_অঙ্ক_outputs : MAX_ACTUATORS),
	_interface(interface),
	_control_latency_perf(perf_alloc(PC_ELAPSED, "control latency")),
	_param_prefix(param_prefix)
{
	/* Safely initialize armed flags */
	_armed.armed = মিথ্যা;
	_armed.prearmed = মিথ্যা;
	_armed.ready_to_arm = মিথ্যা;
	_armed.lockdown = মিথ্যা;
	_armed.termination = মিথ্যা;
	_armed.in_esc_calibration_mode = মিথ্যা;

	px4_sem_init(&_lock, 0, 1);

	initParamHandles(instance_start);

	জন্য (unsigned i = 0; i < MAX_ACTUATORS; i++) {
		failsafe_মান[i] = UINT16_MAX;
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

শূন্য MixingOutput::initParamHandles(ধ্রুবক uint8_t instance_start)
{
	অক্ষর param_name[17];

	জন্য (unsigned i = 0; i < max_অঙ্ক_outputs_2; ++i) {
		snprintf(param_name, আকার(param_name), "%s_%s%d", _param_prefix, "FUNC", i + instance_start);
		_param_handles[i].function = param_find(param_name);
		snprintf(param_name, আকার(param_name), "%s_%s%d", _param_prefix, "DIS", i + instance_start);
		_param_handles[i].disarmed = param_find(param_name);
		snprintf(param_name, আকার(param_name), "%s_%s%d", _param_prefix, "MIN", i + instance_start);
		_param_handles[i].min = param_find(param_name);
		snprintf(param_name, আকার(param_name), "%s_%s%d", _param_prefix, "CENT", i + instance_start);
		_param_handles[i].center = param_find(param_name);
		snprintf(param_name, আকার(param_name), "%s_%s%d", _param_prefix, "MAX", i + instance_start);
		_param_handles[i].max = param_find(param_name);
		snprintf(param_name, আকার(param_name), "%s_%s%d", _param_prefix, "FAIL", i + instance_start);
		_param_handles[i].failsafe = param_find(param_name);
	}

	snprintf(param_name, আকার(param_name), "%s_%s", _param_prefix, "REV");
	_param_handle_rev_range = param_find(param_name);
}

শূন্য MixingOutput::printStatus() ধ্রুবক
{
	PX4_INFO("Param prefix: %s", _param_prefix);
	perf_print_গণক(_control_latency_perf);

	যদি (_wq_switched) {
		PX4_INFO("Switched to rate_ctrl work queue");
	}

	PX4_INFO_RAW("Channel Configuration:\n");

	জন্য (unsigned i = 0; i < max_অঙ্ক_outputs_2; i++) {
		PX4_INFO_RAW("Channel %2d: func: %3d, value: %.2f, failsafe: %.2f, disarmed: %d, min: %d, max: %d, center: %d\n",
			     i, (পূর্ণসংখ্যা)_function_assignment[i], (দ্বিগুণ)current_output_মান[i], (দ্বিগুণ)actual_Failsafe_মান(i),
			     disarmed_মান[i], min_মান[i], max_মান[i], center_মান[i]);
	}
}

শূন্য MixingOutput::updateParams()
{
	ModuleParams::updateParams();

	যৌক্তিক function_changed = মিথ্যা;

	জন্য (unsigned i = 0; i < max_অঙ্ক_outputs_2; i++) {
		int32_t val;

		যদি (_param_handles[i].function != PARAM_INVALID && param_get(_param_handles[i].function, &val) == 0) {
			যদি (val != (int32_t)_function_assignment[i]) {
				function_changed = সত্য;
			}

			// we set _function_assignment[i] later to ensure _functions[i] is updated at the same time
		}

		যদি (_param_handles[i].disarmed != PARAM_INVALID && param_get(_param_handles[i].disarmed, &val) == 0) {
			disarmed_মান[i] = val;
		}

		যদি (_param_handles[i].min != PARAM_INVALID && param_get(_param_handles[i].min, &val) == 0) {
			min_মান[i] = val;
		}

		যদি (_param_handles[i].center != PARAM_INVALID && param_get(_param_handles[i].center, &val) == 0) {
			center_মান[i] = val;
		}

		যদি (_param_handles[i].max != PARAM_INVALID && param_get(_param_handles[i].max, &val) == 0) {
			max_মান[i] = val;
		}

		যদি (min_মান[i] > max_মান[i]) {
			uint16_t tmp = min_মান[i];
			min_মান[i] = max_মান[i];
			max_মান[i] = tmp;
		}


		যদি (_param_handles[i].failsafe != PARAM_INVALID && param_get(_param_handles[i].failsafe, &val) == 0) {
			failsafe_মান[i] = val;
		}
	}

	_reverse_output_mask = 0;
	int32_t rev_range_param;

	যদি (_param_handle_rev_range != PARAM_INVALID && param_get(_param_handle_rev_range, &rev_range_param) == 0) {
		_reverse_output_mask = rev_range_param;
	}

	যদি (function_changed) {
		_need_function_update = সত্য;
	}
}

শূন্য MixingOutput::cleanupFunctions()
{
	যদি (_subscription_callback) {
		_subscription_callback->unregisterCallback();
		_subscription_callback = nullptr;
	}

	জন্য (পূর্ণসংখ্যা i = 0; i < MAX_ACTUATORS; ++i) {
		delete _function_allocated[i];
		_function_allocated[i] = nullptr;
		_functions[i] = nullptr;
	}
}

যৌক্তিক MixingOutput::updateSubscriptions(যৌক্তিক allow_wq_switch)
{
	যদি (!_need_function_update || _armed.armed) {
		ফেরত মিথ্যা;
	}

	// must be locked to potentially change WorkQueue
	lock();

	_has_backup_schedule = মিথ্যা;

	যদি (_scheduling_policy == SchedulingPolicy::Auto) {
		// first clear everything
		unregister();
		_interface.ScheduleClear();

		যৌক্তিক switch_requested = মিথ্যা;

		// potentially switch work queue if we run motor outputs
		জন্য (unsigned i = 0; i < max_অঙ্ক_outputs_2; i++) {
			// read function directly from param, as _function_assignment[i] is updated later
			int32_t function;

			যদি (_param_handles[i].function != PARAM_INVALID && param_get(_param_handles[i].function, &function) == 0) {
				যদি (function >= (int32_t)OutputFunction::Motor1 && function <= (int32_t)OutputFunction::MotorMax) {
					switch_requested = সত্য;
				}
			}
		}

		যদি (allow_wq_switch && !_wq_switched && switch_requested) {
			যদি (_interface.ChangeWorkQueue(px4::wq_configurations::rate_ctrl)) {
				// let the new WQ handle the subscribe update
				_wq_switched = সত্য;
				_interface.ScheduleNow();
				unlock();
				ফেরত মিথ্যা;
			}
		}
	}

	// Now update the functions
	PX4_DEBUG("updating functions");

	cleanupFunctions();

	ধ্রুবক FunctionProviderBase::Context context{_interface, _param_thr_mdl_fac.reference()};
	পূর্ণসংখ্যা provider_indexes[MAX_ACTUATORS] {};
	পূর্ণসংখ্যা পরবর্তী_provider = 0;
	পূর্ণসংখ্যা subscription_callback_provider_সূচক = INT_MAX;
	যৌক্তিক all_disabled = সত্য;

	জন্য (পূর্ণসংখ্যা i = 0; i < max_অঙ্ক_outputs_2; ++i) {
		int32_t val;

		যদি (_param_handles[i].function != PARAM_INVALID && param_get(_param_handles[i].function, &val) == 0) {
			_function_assignment[i] = (OutputFunction)val;

		} অন্যথা {
			_function_assignment[i] = OutputFunction::Disabled;
		}

		জন্য (পূর্ণসংখ্যা p = 0; p < (পূর্ণসংখ্যা)(আকার(all_function_providers) / আকার(all_function_providers[0])); ++p) {
			যদি (_function_assignment[i] >= all_function_providers[p].min_func &&
			    _function_assignment[i] <= all_function_providers[p].max_func) {
				all_disabled = মিথ্যা;
				পূর্ণসংখ্যা found_সূচক = -1;

				জন্য (পূর্ণসংখ্যা existing = 0; existing < পরবর্তী_provider; ++existing) {
					যদি (provider_indexes[existing] == p) {
						found_সূচক = existing;
						বিরতি;
					}
				}

				যদি (found_সূচক >= 0) {
					_functions[i] = _function_allocated[found_সূচক];

				} অন্যথা {
					_function_allocated[পরবর্তী_provider] = all_function_providers[p].constructor(context);

					যদি (_function_allocated[পরবর্তী_provider]) {
						_functions[i] = _function_allocated[পরবর্তী_provider];
						provider_indexes[পরবর্তী_provider++] = p;

						// lowest provider takes precedence for scheduling
						যদি (p < subscription_callback_provider_সূচক && _functions[i]->subscriptionCallback()) {
							subscription_callback_provider_সূচক = p;
							_subscription_callback = _functions[i]->subscriptionCallback();
						}

					} অন্যথা {
						PX4_ERR("function alloc failed");
					}
				}

				বিরতি;
			}
		}
	}

	hrt_abstime fixed_rate_scheduling_interval = 4_ms; // schedule at 250Hz

	যদি (_max_topic_update_interval_us > fixed_rate_scheduling_interval) {
		fixed_rate_scheduling_interval = _max_topic_update_interval_us;
	}

	যদি (_scheduling_policy == SchedulingPolicy::Auto) {
		যদি (_subscription_callback) {
			যদি (_subscription_callback->registerCallback()) {
				PX4_DEBUG("Scheduling via callback");
				_has_backup_schedule = সত্য;
				_interface.ScheduleDelayed(50_ms);

			} অন্যথা {
				PX4_ERR("registerCallback failed, scheduling at fixed rate");
				_interface.ScheduleOnInterval(fixed_rate_scheduling_interval);
			}

		} অন্যথা যদি (all_disabled) {
			_interface.ScheduleOnInterval(_lowrate_schedule_interval);
			PX4_DEBUG("Scheduling at low rate");

		} অন্যথা {
			_interface.ScheduleOnInterval(fixed_rate_scheduling_interval);
			PX4_DEBUG("Scheduling at fixed rate");
		}
	}

	setMaxTopicUpdateRate(_max_topic_update_interval_us);
	_need_function_update = মিথ্যা;

	_actuator_test.reset();

	unlock();

	_interface.mixerChanged();

	ফেরত সত্য;
}

শূন্য MixingOutput::setMaxTopicUpdateRate(unsigned max_topic_update_interval_us)
{
	_max_topic_update_interval_us = max_topic_update_interval_us;

	যদি (_subscription_callback) {
		_subscription_callback->set_interval_us(_max_topic_update_interval_us);
	}
}

শূন্য MixingOutput::set_All_Min_মানসমূহ(uint16_t value)
{
	জন্য (unsigned i = 0; i < MAX_ACTUATORS; i++) {
		_param_handles[i].min = PARAM_INVALID;
		min_মান[i] = value;
	}
}

শূন্য MixingOutput::set_All_Center_মানসমূহ(uint16_t value)
{
	জন্য (unsigned i = 0; i < MAX_ACTUATORS; i++) {
		_param_handles[i].center = PARAM_INVALID;
		center_মান[i] = value;
	}
}

শূন্য MixingOutput::set_All_Max_মানসমূহ(uint16_t value)
{
	জন্য (unsigned i = 0; i < MAX_ACTUATORS; i++) {
		_param_handles[i].max = PARAM_INVALID;
		max_মান[i] = value;
	}
}

শূন্য MixingOutput::set_All_Failsafe_মানসমূহ(uint16_t value)
{
	জন্য (unsigned i = 0; i < MAX_ACTUATORS; i++) {
		_param_handles[i].failsafe = PARAM_INVALID;
		failsafe_মান[i] = value;
	}
}

শূন্য MixingOutput::set_All_Disarmed_মানসমূহ(uint16_t value)
{
	জন্য (unsigned i = 0; i < MAX_ACTUATORS; i++) {
		_param_handles[i].disarmed = PARAM_INVALID;
		disarmed_মান[i] = value;
	}
}

শূন্য MixingOutput::unregister()
{
	যদি (_subscription_callback) {
		_subscription_callback->unregisterCallback();
	}
}

যৌক্তিক MixingOutput::update()
{
	// check arming state
	যদি (_armed_sub.update(&_armed)) {
		_armed.in_esc_calibration_mode &= _support_esc_calibration;

		যদি (_ignore_lockdown) {
			_armed.lockdown = মিথ্যা;
		}

		/* Update the armed status and check that we're not locked down.
		 * We also need to arm throttle for the ESC calibration. */
		_throttle_armed = (_armed.armed && !_armed.lockdown) || _armed.in_esc_calibration_mode;
	}

	// only used for sitl with lockstep
	যৌক্তিক has_updates = _subscription_callback && _subscription_callback->updated();

	// update topics
	জন্য (পূর্ণসংখ্যা i = 0; i < MAX_ACTUATORS && _function_allocated[i]; ++i) {
		_function_allocated[i]->update();
	}

	যদি (_has_backup_schedule) {
		_interface.ScheduleDelayed(50_ms);
	}

	// check for actuator test
	_actuator_test.update(max_অঙ্ক_outputs_2, _param_thr_mdl_fac.get());

	// get output values
	দশমিক outputs[MAX_ACTUATORS];
	যৌক্তিক all_disabled = সত্য;
	ধ্রুবক uint32_t reversible_mask_পূর্ব = _reversible_mask;
	_reversible_mask = 0;

	জন্য (পূর্ণসংখ্যা i = 0; i < max_অঙ্ক_outputs_2; ++i) {
		যদি (_functions[i]) {
			all_disabled = মিথ্যা;

			যদি (_armed.armed || (_armed.prearmed && _functions[i]->allowPrearmControl())) {
				outputs[i] = _functions[i]->value(_function_assignment[i]);

			} অন্যথা {
				outputs[i] = NAN;
			}

			_reversible_mask |= (uint32_t)_functions[i]->reversible(_function_assignment[i]) << i;

		} অন্যথা {
			outputs[i] = NAN;
		}
	}

	যদি (_reversible_mask != reversible_mask_পূর্ব) {
		_interface.reversibleMaskChanged(_reversible_mask);
	}

	// Send output if any function mapped or one last disabling sample
	যদি (!all_disabled || !_was_all_disabled) {
		যদি (!_armed.armed && !_armed.kill) {
			_actuator_test.override_মানসমূহ(outputs, max_অঙ্ক_outputs_2);
		}

		limitAndUpdateOutputs(outputs, has_updates);
	}

	_was_all_disabled = all_disabled;

	ফেরত সত্য;
}

শূন্য
MixingOutput::limitAndUpdateOutputs(দশমিক outputs[MAX_ACTUATORS], যৌক্তিক has_updates)
{
	যদি (_armed.lockdown || _armed.kill) {
		// overwrite outputs in case of lockdown with disarmed values
		জন্য (size_t i = 0; i < max_অঙ্ক_outputs_2; i++) {
			current_output_মান[i] = disarmed_মান[i];
		}

	} অন্যথা যদি (_armed.termination) {
		// Overwrite outputs with _failsafe_value when terminated
		জন্য (size_t i = 0; i < max_অঙ্ক_outputs_2; i++) {
			current_output_মান[i] = actual_Failsafe_মান(i);
		}

	} অন্যথা {
		// the output limit call takes care of out of band errors, NaN and constrains
		output_limit_calc(_throttle_armed || _actuator_test.inTestMode(), max_অঙ্ক_outputs_2, outputs);
	}

	// We must calibrate the PWM and Oneshot ESCs to a consistent range of 1000-2000us (gets mapped to 125-250us for Oneshot)
	// Doing so makes calibrations consistent among different configurations and hence PWM minimum and maximum have a consistent effect
	// hence the defaults for these parameters also make most setups work out of the box
	যদি (_armed.in_esc_calibration_mode) {
		স্থির constexpr দশমিক PWM_CALIBRATION_নিচের = 1000.f;
		স্থির constexpr দশমিক PWM_CALIBRATION_উপরের = 2000.f;

		জন্য (পূর্ণসংখ্যা i = 0; i < max_অঙ্ক_outputs_2; i++) {
			যদি (fabsf(current_output_মান[i] - (দশমিক)min_মান[i]) < 0.5f) {
				current_output_মান[i] = PWM_CALIBRATION_নিচের;
			}

			যদি (fabsf(current_output_মান[i] - (দশমিক)max_মান[i]) < 0.5f) {
				current_output_মান[i] = PWM_CALIBRATION_উপরের;
			}
		}
	}

	/* now return the outputs to the driver */
	যদি (_interface.updateOutputs(current_output_মান, max_অঙ্ক_outputs_2, has_updates)) {
		actuator_outputs_s actuator_outputs{};
		setAndPublishActuatorOutputs(max_অঙ্ক_outputs_2, actuator_outputs);

		update_Latency_Perf_গণক(actuator_outputs);
	}
}

দশমিক MixingOutput::output_limit_calc_single(পূর্ণসংখ্যা i, দশমিক value) ধ্রুবক
{
	// check for invalid / disabled channels
	যদি (!PX4_ISFINITE(value)) {
		ফেরত disarmed_মান[i];
	}

	যদি (_reverse_output_mask & (1 << i)) {
		value = -1.f * value;
	}

	দশমিক output = disarmed_মান[i];

	যদি (((_function_assignment[i] >= OutputFunction::Servo1
	      && _function_assignment[i] <= OutputFunction::ServoMax) || _function_assignment[i] == OutputFunction::Landing_Gear_Wheel
	     || (_function_assignment[i] >= OutputFunction::Gimbal_Roll
		 && _function_assignment[i] <= OutputFunction::Gimbal_Yaw))
	    && _param_handles[i].center != PARAM_INVALID
	    && center_মান[i] >= min_মান[i]
	    && center_মান[i] <= max_মান[i]) {
		output = math::interpolateNXY(value, {-1.f, 0.f, 1.f}, {(দশমিক)min_মান[i], (দশমিক)center_মান[i], (দশমিক)max_মান[i]});
	}

	// Everything except servos, or if center is not set
	অন্যথা {
		output = math::interpolate(value, -1.f, 1.f, static_cast<দশমিক>(min_মান[i]), static_cast<দশমিক>(max_মান[i]));
	}

	ফেরত output;
}

শূন্য
MixingOutput::output_limit_calc(ধ্রুবক যৌক্তিক armed, ধ্রুবক পূর্ণসংখ্যা অঙ্ক_channels, ধ্রুবক দশমিক output[MAX_ACTUATORS])
{
	// time to slowly ramp up the ESCs
	স্থির constexpr hrt_abstime RAMP_TIME_US = 500_ms;

	/* first evaluate state changes */
	নির্বাচন (_output_state) {
	ক্ষেত্র OutputLimitState::OFF:
		যদি (armed) {
			যদি (_output_ramp_up) {
				_output_state = OutputLimitState::RAMP;

			} অন্যথা {
				_output_state = OutputLimitState::ON;
			}

			// reset arming time, used for ramp timing
			_output_time_armed = hrt_absolute_time();
		}

		বিরতি;

	ক্ষেত্র OutputLimitState::RAMP:
		যদি (!armed) {
			_output_state = OutputLimitState::OFF;

		} অন্যথা যদি (hrt_elapsed_time(&_output_time_armed) >= RAMP_TIME_US) {
			_output_state = OutputLimitState::ON;
		}

		বিরতি;

	ক্ষেত্র OutputLimitState::ON:
		যদি (!armed) {
			_output_state = OutputLimitState::OFF;
		}

		বিরতি;
	}

	/* if the system is pre-armed, the limit state is temporarily on,
	 * as some outputs are valid and the non-valid outputs have been
	 * set to NaN. This is not stored in the state machine though,
	 * as the throttle channels need to go through the ramp at
	 * regular arming time.
	 */
	auto local_limit_state = _output_state;

	যদি (isPrearmed()) {
		local_limit_state = OutputLimitState::ON;
	}

	// then set _current_output_value based on state
	নির্বাচন (local_limit_state) {
	ক্ষেত্র OutputLimitState::OFF:
		জন্য (পূর্ণসংখ্যা i = 0; i < অঙ্ক_channels; i++) {
			current_output_মান[i] = disarmed_মান[i];
		}

		বিরতি;

	ক্ষেত্র OutputLimitState::RAMP: {
			hrt_abstime diff = hrt_elapsed_time(&_output_time_armed);
			দশমিক progress = static_cast<দশমিক>(diff) / RAMP_TIME_US;

			যদি (progress > 1.f) {
				progress = 1.f;
			}

			জন্য (পূর্ণসংখ্যা i = 0; i < অঙ্ক_channels; i++) {
				// Ramp from disarmed value to currently desired output that would apply without ramp
				দশমিক desired_output = output_limit_calc_single(i, output[i]);
				current_output_মান[i] = disarmed_মান[i] + progress * (desired_output - disarmed_মান[i]);
			}
		}
		বিরতি;

	ক্ষেত্র OutputLimitState::ON:
		জন্য (পূর্ণসংখ্যা i = 0; i < অঙ্ক_channels; i++) {
			current_output_মান[i] = output_limit_calc_single(i, output[i]);
		}

		বিরতি;
	}
}

শূন্য
MixingOutput::setAndPublishActuatorOutputs(unsigned অঙ্ক_outputs, actuator_outputs_s &actuator_outputs)
{
	actuator_outputs.noutputs = অঙ্ক_outputs;

	জন্য (size_t i = 0; i < অঙ্ক_outputs; ++i) {
		actuator_outputs.output[i] = current_output_মান[i];
	}

	actuator_outputs.timestamp = hrt_absolute_time();
	_outputs_pub.publish(actuator_outputs);
}

শূন্য
MixingOutput::update_Latency_Perf_গণক(ধ্রুবক actuator_outputs_s &actuator_outputs)
{
	// Just check the first function. It means we only get the latency if motors are assigned first, which is the default
	যদি (_function_allocated[0]) {
		hrt_abstime timestamp_sample;

		যদি (_function_allocated[0]->getLatestSampleTimestamp(timestamp_sample)) {
			perf_set_elapsed(_control_latency_perf, actuator_outputs.timestamp - timestamp_sample);
		}
	}
}

দশমিক MixingOutput::actual_Failsafe_মান(পূর্ণসংখ্যা সূচক) ধ্রুবক
{
	দশমিক value = 0;

	যদি (failsafe_মান[সূচক] == UINT16_MAX) { // if set to default, use the one provided by the function
		দশমিক default_failsafe = NAN;

		যদি (_functions[সূচক]) {
			default_failsafe = _functions[সূচক]->default_Failsafe_মান(_function_assignment[সূচক]);
		}

		value = output_limit_calc_single(সূচক, default_failsafe);

	} অন্যথা {
		value = static_cast<দশমিক>(failsafe_মান[সূচক]);
	}

	ফেরত value;
}
