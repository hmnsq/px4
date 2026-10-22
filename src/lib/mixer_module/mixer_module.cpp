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

#ضمن "mixer_module.hpp"

#ضمن <uORB/Publication.hpp>
#ضمن <px4_platform_common/log.h>

using نطاق time_literals;


بنية FunctionProvider {
	using Constructor = FunctionProviderBase * (*)(ثابت FunctionProviderBase::Context &context);
	constexpr FunctionProvider(OutputFunction min_func_, OutputFunction max_func_, Constructor constructor_)
		: min_func(min_func_), max_func(max_func_), constructor(constructor_) {}
	constexpr FunctionProvider(OutputFunction func, Constructor constructor_)
		: min_func(func), max_func(func), constructor(constructor_) {}

	OutputFunction min_func;
	OutputFunction max_func;
	Constructor constructor;
};

ساكن constexpr FunctionProvider all_function_providers[] = {
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

MixingOutput::MixingOutput(ثابت حرف *param_prefix, uint8_t max_عدد_outputs, OutputModuleInterface &interface,
			   SchedulingPolicy scheduling_policy, منطقي support_esc_calibration, منطقي ramp_up, ثابت uint8_t instance_start) :
	ModuleParams(&interface),
	_output_ramp_up(ramp_up),
	_scheduling_policy(scheduling_policy),
	_support_esc_calibration(support_esc_calibration),
	max_عدد_outputs_2(max_عدد_outputs < MAX_ACTUATORS ? max_عدد_outputs : MAX_ACTUATORS),
	_interface(interface),
	_control_latency_perf(perf_alloc(PC_ELAPSED, "control latency")),
	_param_prefix(param_prefix)
{
	/* Safely initialize armed flags */
	_armed.armed = خطأ;
	_armed.prearmed = خطأ;
	_armed.ready_to_arm = خطأ;
	_armed.lockdown = خطأ;
	_armed.termination = خطأ;
	_armed.in_esc_calibration_mode = خطأ;

	px4_sem_init(&_lock, 0, 1);

	initParamHandles(instance_start);

	لكل (unsigned i = 0; i < MAX_ACTUATORS; i++) {
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

فراغ MixingOutput::initParamHandles(ثابت uint8_t instance_start)
{
	حرف param_name[17];

	لكل (unsigned i = 0; i < max_عدد_outputs_2; ++i) {
		snprintf(param_name, حجم(param_name), "%s_%s%d", _param_prefix, "FUNC", i + instance_start);
		_param_handles[i].function = param_find(param_name);
		snprintf(param_name, حجم(param_name), "%s_%s%d", _param_prefix, "DIS", i + instance_start);
		_param_handles[i].disarmed = param_find(param_name);
		snprintf(param_name, حجم(param_name), "%s_%s%d", _param_prefix, "MIN", i + instance_start);
		_param_handles[i].min = param_find(param_name);
		snprintf(param_name, حجم(param_name), "%s_%s%d", _param_prefix, "CENT", i + instance_start);
		_param_handles[i].center = param_find(param_name);
		snprintf(param_name, حجم(param_name), "%s_%s%d", _param_prefix, "MAX", i + instance_start);
		_param_handles[i].max = param_find(param_name);
		snprintf(param_name, حجم(param_name), "%s_%s%d", _param_prefix, "FAIL", i + instance_start);
		_param_handles[i].failsafe = param_find(param_name);
	}

	snprintf(param_name, حجم(param_name), "%s_%s", _param_prefix, "REV");
	_param_handle_rev_range = param_find(param_name);
}

فراغ MixingOutput::printStatus() ثابت
{
	PX4_INFO("Param prefix: %s", _param_prefix);
	perf_print_counter(_control_latency_perf);

	إذا (_wq_switched) {
		PX4_INFO("Switched to rate_ctrl work queue");
	}

	PX4_INFO_RAW("Channel Configuration:\n");

	لكل (unsigned i = 0; i < max_عدد_outputs_2; i++) {
		PX4_INFO_RAW("Channel %2d: func: %3d, value: %.2f, failsafe: %.2f, disarmed: %d, min: %d, max: %d, center: %d\n",
			     i, (صحيح)_function_assignment[i], (مزدوج)current_output_قدر[i], (مزدوج)actual_Failsafe_قدر(i),
			     disarmed_قدر[i], min_قدر[i], max_قدر[i], center_قدر[i]);
	}
}

فراغ MixingOutput::updateParams()
{
	ModuleParams::updateParams();

	منطقي function_changed = خطأ;

	لكل (unsigned i = 0; i < max_عدد_outputs_2; i++) {
		int32_t val;

		إذا (_param_handles[i].function != PARAM_INVALID && param_get(_param_handles[i].function, &val) == 0) {
			إذا (val != (int32_t)_function_assignment[i]) {
				function_changed = true;
			}

			// we set _function_assignment[i] later to ensure _functions[i] is updated at the same time
		}

		إذا (_param_handles[i].disarmed != PARAM_INVALID && param_get(_param_handles[i].disarmed, &val) == 0) {
			disarmed_قدر[i] = val;
		}

		إذا (_param_handles[i].min != PARAM_INVALID && param_get(_param_handles[i].min, &val) == 0) {
			min_قدر[i] = val;
		}

		إذا (_param_handles[i].center != PARAM_INVALID && param_get(_param_handles[i].center, &val) == 0) {
			center_قدر[i] = val;
		}

		إذا (_param_handles[i].max != PARAM_INVALID && param_get(_param_handles[i].max, &val) == 0) {
			max_قدر[i] = val;
		}

		إذا (min_قدر[i] > max_قدر[i]) {
			uint16_t tmp = min_قدر[i];
			min_قدر[i] = max_قدر[i];
			max_قدر[i] = tmp;
		}


		إذا (_param_handles[i].failsafe != PARAM_INVALID && param_get(_param_handles[i].failsafe, &val) == 0) {
			failsafe_قدر[i] = val;
		}
	}

	_reverse_output_mask = 0;
	int32_t rev_range_param;

	إذا (_param_handle_rev_range != PARAM_INVALID && param_get(_param_handle_rev_range, &rev_range_param) == 0) {
		_reverse_output_mask = rev_range_param;
	}

	إذا (function_changed) {
		_need_function_update = true;
	}
}

فراغ MixingOutput::cleanupFunctions()
{
	إذا (_subscription_callback) {
		_subscription_callback->unregisterCallback();
		_subscription_callback = nullptr;
	}

	لكل (صحيح i = 0; i < MAX_ACTUATORS; ++i) {
		delete _function_allocated[i];
		_function_allocated[i] = nullptr;
		_functions[i] = nullptr;
	}
}

منطقي MixingOutput::updateSubscriptions(منطقي allow_wq_switch)
{
	إذا (!_need_function_update || _armed.armed) {
		أرجع خطأ;
	}

	// must be locked to potentially change WorkQueue
	lock();

	_has_backup_schedule = خطأ;

	إذا (_scheduling_policy == SchedulingPolicy::Auto) {
		// first clear everything
		unregister();
		_interface.ScheduleClear();

		منطقي switch_requested = خطأ;

		// potentially switch work queue if we run motor outputs
		لكل (unsigned i = 0; i < max_عدد_outputs_2; i++) {
			// read function directly from param, as _function_assignment[i] is updated later
			int32_t function;

			إذا (_param_handles[i].function != PARAM_INVALID && param_get(_param_handles[i].function, &function) == 0) {
				إذا (function >= (int32_t)OutputFunction::Motor1 && function <= (int32_t)OutputFunction::MotorMax) {
					switch_requested = true;
				}
			}
		}

		إذا (allow_wq_switch && !_wq_switched && switch_requested) {
			إذا (_interface.ChangeWorkQueue(px4::wq_configurations::rate_ctrl)) {
				// let the new WQ handle the subscribe update
				_wq_switched = true;
				_interface.ScheduleNow();
				unlock();
				أرجع خطأ;
			}
		}
	}

	// Now update the functions
	PX4_DEBUG("updating functions");

	cleanupFunctions();

	ثابت FunctionProviderBase::Context context{_interface, _param_thr_mdl_fac.reference()};
	صحيح provider_indexes[MAX_ACTUATORS] {};
	صحيح next_provider = 0;
	صحيح subscription_callback_provider_index = INT_MAX;
	منطقي all_disabled = true;

	لكل (صحيح i = 0; i < max_عدد_outputs_2; ++i) {
		int32_t val;

		إذا (_param_handles[i].function != PARAM_INVALID && param_get(_param_handles[i].function, &val) == 0) {
			_function_assignment[i] = (OutputFunction)val;

		} وإلا {
			_function_assignment[i] = OutputFunction::Disabled;
		}

		لكل (صحيح p = 0; p < (صحيح)(حجم(all_function_providers) / حجم(all_function_providers[0])); ++p) {
			إذا (_function_assignment[i] >= all_function_providers[p].min_func &&
			    _function_assignment[i] <= all_function_providers[p].max_func) {
				all_disabled = خطأ;
				صحيح found_index = -1;

				لكل (صحيح existing = 0; existing < next_provider; ++existing) {
					إذا (provider_indexes[existing] == p) {
						found_index = existing;
						اكسر;
					}
				}

				إذا (found_index >= 0) {
					_functions[i] = _function_allocated[found_index];

				} وإلا {
					_function_allocated[next_provider] = all_function_providers[p].constructor(context);

					إذا (_function_allocated[next_provider]) {
						_functions[i] = _function_allocated[next_provider];
						provider_indexes[next_provider++] = p;

						// lowest provider takes precedence for scheduling
						إذا (p < subscription_callback_provider_index && _functions[i]->subscriptionCallback()) {
							subscription_callback_provider_index = p;
							_subscription_callback = _functions[i]->subscriptionCallback();
						}

					} وإلا {
						PX4_ERR("function alloc failed");
					}
				}

				اكسر;
			}
		}
	}

	hrt_abstime fixed_rate_scheduling_interval = 4_ms; // schedule at 250Hz

	إذا (_max_topic_update_interval_us > fixed_rate_scheduling_interval) {
		fixed_rate_scheduling_interval = _max_topic_update_interval_us;
	}

	إذا (_scheduling_policy == SchedulingPolicy::Auto) {
		إذا (_subscription_callback) {
			إذا (_subscription_callback->registerCallback()) {
				PX4_DEBUG("Scheduling via callback");
				_has_backup_schedule = true;
				_interface.ScheduleDelayed(50_ms);

			} وإلا {
				PX4_ERR("registerCallback failed, scheduling at fixed rate");
				_interface.ScheduleOnInterval(fixed_rate_scheduling_interval);
			}

		} وإلا إذا (all_disabled) {
			_interface.ScheduleOnInterval(_lowrate_schedule_interval);
			PX4_DEBUG("Scheduling at low rate");

		} وإلا {
			_interface.ScheduleOnInterval(fixed_rate_scheduling_interval);
			PX4_DEBUG("Scheduling at fixed rate");
		}
	}

	setMaxTopicUpdateRate(_max_topic_update_interval_us);
	_need_function_update = خطأ;

	_actuator_test.reset();

	unlock();

	_interface.mixerChanged();

	أرجع true;
}

فراغ MixingOutput::setMaxTopicUpdateRate(unsigned max_topic_update_interval_us)
{
	_max_topic_update_interval_us = max_topic_update_interval_us;

	إذا (_subscription_callback) {
		_subscription_callback->set_interval_us(_max_topic_update_interval_us);
	}
}

فراغ MixingOutput::set_All_Min_اقدار(uint16_t value)
{
	لكل (unsigned i = 0; i < MAX_ACTUATORS; i++) {
		_param_handles[i].min = PARAM_INVALID;
		min_قدر[i] = value;
	}
}

فراغ MixingOutput::set_All_Center_اقدار(uint16_t value)
{
	لكل (unsigned i = 0; i < MAX_ACTUATORS; i++) {
		_param_handles[i].center = PARAM_INVALID;
		center_قدر[i] = value;
	}
}

فراغ MixingOutput::set_All_Max_اقدار(uint16_t value)
{
	لكل (unsigned i = 0; i < MAX_ACTUATORS; i++) {
		_param_handles[i].max = PARAM_INVALID;
		max_قدر[i] = value;
	}
}

فراغ MixingOutput::set_All_Failsafe_اقدار(uint16_t value)
{
	لكل (unsigned i = 0; i < MAX_ACTUATORS; i++) {
		_param_handles[i].failsafe = PARAM_INVALID;
		failsafe_قدر[i] = value;
	}
}

فراغ MixingOutput::set_All_Disarmed_اقدار(uint16_t value)
{
	لكل (unsigned i = 0; i < MAX_ACTUATORS; i++) {
		_param_handles[i].disarmed = PARAM_INVALID;
		disarmed_قدر[i] = value;
	}
}

فراغ MixingOutput::unregister()
{
	إذا (_subscription_callback) {
		_subscription_callback->unregisterCallback();
	}
}

منطقي MixingOutput::update()
{
	// check arming state
	إذا (_armed_sub.update(&_armed)) {
		_armed.in_esc_calibration_mode &= _support_esc_calibration;

		إذا (_ignore_lockdown) {
			_armed.lockdown = خطأ;
		}

		/* Update the armed status and check that we're not locked down.
		 * We also need to arm throttle for the ESC calibration. */
		_throttle_armed = (_armed.armed && !_armed.lockdown) || _armed.in_esc_calibration_mode;
	}

	// only used for sitl with lockstep
	منطقي has_updates = _subscription_callback && _subscription_callback->updated();

	// update topics
	لكل (صحيح i = 0; i < MAX_ACTUATORS && _function_allocated[i]; ++i) {
		_function_allocated[i]->update();
	}

	إذا (_has_backup_schedule) {
		_interface.ScheduleDelayed(50_ms);
	}

	// check for actuator test
	_actuator_test.update(max_عدد_outputs_2, _param_thr_mdl_fac.get());

	// get output values
	عائم outputs[MAX_ACTUATORS];
	منطقي all_disabled = true;
	ثابت uint32_t reversible_mask_prev = _reversible_mask;
	_reversible_mask = 0;

	لكل (صحيح i = 0; i < max_عدد_outputs_2; ++i) {
		إذا (_functions[i]) {
			all_disabled = خطأ;

			إذا (_armed.armed || (_armed.prearmed && _functions[i]->allowPrearmControl())) {
				outputs[i] = _functions[i]->value(_function_assignment[i]);

			} وإلا {
				outputs[i] = NAN;
			}

			_reversible_mask |= (uint32_t)_functions[i]->reversible(_function_assignment[i]) << i;

		} وإلا {
			outputs[i] = NAN;
		}
	}

	إذا (_reversible_mask != reversible_mask_prev) {
		_interface.reversibleMaskChanged(_reversible_mask);
	}

	// Send output if any function mapped or one last disabling sample
	إذا (!all_disabled || !_was_all_disabled) {
		إذا (!_armed.armed && !_armed.kill) {
			_actuator_test.override_اقدار(outputs, max_عدد_outputs_2);
		}

		limitAndUpdateOutputs(outputs, has_updates);
	}

	_was_all_disabled = all_disabled;

	أرجع true;
}

فراغ
MixingOutput::limitAndUpdateOutputs(عائم outputs[MAX_ACTUATORS], منطقي has_updates)
{
	إذا (_armed.lockdown || _armed.kill) {
		// overwrite outputs in case of lockdown with disarmed values
		لكل (size_t i = 0; i < max_عدد_outputs_2; i++) {
			current_output_قدر[i] = disarmed_قدر[i];
		}

	} وإلا إذا (_armed.termination) {
		// Overwrite outputs with _failsafe_value when terminated
		لكل (size_t i = 0; i < max_عدد_outputs_2; i++) {
			current_output_قدر[i] = actual_Failsafe_قدر(i);
		}

	} وإلا {
		// the output limit call takes care of out of band errors, NaN and constrains
		output_limit_calc(_throttle_armed || _actuator_test.inTestMode(), max_عدد_outputs_2, outputs);
	}

	// We must calibrate the PWM and Oneshot ESCs to a consistent range of 1000-2000us (gets mapped to 125-250us for Oneshot)
	// Doing so makes calibrations consistent among different configurations and hence PWM minimum and maximum have a consistent effect
	// hence the defaults for these parameters also make most setups work out of the box
	إذا (_armed.in_esc_calibration_mode) {
		ساكن constexpr عائم PWM_CALIBRATION_کم = 1000.f;
		ساكن constexpr عائم PWM_CALIBRATION_بلند = 2000.f;

		لكل (صحيح i = 0; i < max_عدد_outputs_2; i++) {
			إذا (fabsf(current_output_قدر[i] - (عائم)min_قدر[i]) < 0.5f) {
				current_output_قدر[i] = PWM_CALIBRATION_کم;
			}

			إذا (fabsf(current_output_قدر[i] - (عائم)max_قدر[i]) < 0.5f) {
				current_output_قدر[i] = PWM_CALIBRATION_بلند;
			}
		}
	}

	/* now return the outputs to the driver */
	إذا (_interface.updateOutputs(current_output_قدر, max_عدد_outputs_2, has_updates)) {
		actuator_outputs_s actuator_outputs{};
		setAndPublishActuatorOutputs(max_عدد_outputs_2, actuator_outputs);

		updateLatencyPerfCounter(actuator_outputs);
	}
}

عائم MixingOutput::output_limit_calc_single(صحيح i, عائم value) ثابت
{
	// check for invalid / disabled channels
	إذا (!PX4_ISFINITE(value)) {
		أرجع disarmed_قدر[i];
	}

	إذا (_reverse_output_mask & (1 << i)) {
		value = -1.f * value;
	}

	عائم output = disarmed_قدر[i];

	إذا (((_function_assignment[i] >= OutputFunction::Servo1
	      && _function_assignment[i] <= OutputFunction::ServoMax) || _function_assignment[i] == OutputFunction::Landing_Gear_Wheel
	     || (_function_assignment[i] >= OutputFunction::Gimbal_Roll
		 && _function_assignment[i] <= OutputFunction::Gimbal_Yaw))
	    && _param_handles[i].center != PARAM_INVALID
	    && center_قدر[i] >= min_قدر[i]
	    && center_قدر[i] <= max_قدر[i]) {
		output = math::interpolateNXY(value, {-1.f, 0.f, 1.f}, {(عائم)min_قدر[i], (عائم)center_قدر[i], (عائم)max_قدر[i]});
	}

	// Everything except servos, or if center is not set
	وإلا {
		output = math::interpolate(value, -1.f, 1.f, static_cast<عائم>(min_قدر[i]), static_cast<عائم>(max_قدر[i]));
	}

	أرجع output;
}

فراغ
MixingOutput::output_limit_calc(ثابت منطقي armed, ثابت صحيح عدد_channels, ثابت عائم output[MAX_ACTUATORS])
{
	// time to slowly ramp up the ESCs
	ساكن constexpr hrt_abstime RAMP_TIME_US = 500_ms;

	/* first evaluate state changes */
	بدل (_output_state) {
	حالة OutputLimitState::OFF:
		إذا (armed) {
			إذا (_output_ramp_up) {
				_output_state = OutputLimitState::RAMP;

			} وإلا {
				_output_state = OutputLimitState::ON;
			}

			// reset arming time, used for ramp timing
			_output_time_armed = hrt_absolute_time();
		}

		اكسر;

	حالة OutputLimitState::RAMP:
		إذا (!armed) {
			_output_state = OutputLimitState::OFF;

		} وإلا إذا (hrt_elapsed_time(&_output_time_armed) >= RAMP_TIME_US) {
			_output_state = OutputLimitState::ON;
		}

		اكسر;

	حالة OutputLimitState::ON:
		إذا (!armed) {
			_output_state = OutputLimitState::OFF;
		}

		اكسر;
	}

	/* if the system is pre-armed, the limit state is temporarily on,
	 * as some outputs are valid and the non-valid outputs have been
	 * set to NaN. This is not stored in the state machine though,
	 * as the throttle channels need to go through the ramp at
	 * regular arming time.
	 */
	auto local_limit_state = _output_state;

	إذا (isPrearmed()) {
		local_limit_state = OutputLimitState::ON;
	}

	// then set _current_output_value based on state
	بدل (local_limit_state) {
	حالة OutputLimitState::OFF:
		لكل (صحيح i = 0; i < عدد_channels; i++) {
			current_output_قدر[i] = disarmed_قدر[i];
		}

		اكسر;

	حالة OutputLimitState::RAMP: {
			hrt_abstime diff = hrt_elapsed_time(&_output_time_armed);
			عائم progress = static_cast<عائم>(diff) / RAMP_TIME_US;

			إذا (progress > 1.f) {
				progress = 1.f;
			}

			لكل (صحيح i = 0; i < عدد_channels; i++) {
				// Ramp from disarmed value to currently desired output that would apply without ramp
				عائم desired_output = output_limit_calc_single(i, output[i]);
				current_output_قدر[i] = disarmed_قدر[i] + progress * (desired_output - disarmed_قدر[i]);
			}
		}
		اكسر;

	حالة OutputLimitState::ON:
		لكل (صحيح i = 0; i < عدد_channels; i++) {
			current_output_قدر[i] = output_limit_calc_single(i, output[i]);
		}

		اكسر;
	}
}

فراغ
MixingOutput::setAndPublishActuatorOutputs(unsigned عدد_outputs, actuator_outputs_s &actuator_outputs)
{
	actuator_outputs.noutputs = عدد_outputs;

	لكل (size_t i = 0; i < عدد_outputs; ++i) {
		actuator_outputs.output[i] = current_output_قدر[i];
	}

	actuator_outputs.timestamp = hrt_absolute_time();
	_outputs_pub.publish(actuator_outputs);
}

فراغ
MixingOutput::updateLatencyPerfCounter(ثابت actuator_outputs_s &actuator_outputs)
{
	// Just check the first function. It means we only get the latency if motors are assigned first, which is the default
	إذا (_function_allocated[0]) {
		hrt_abstime timestamp_sample;

		إذا (_function_allocated[0]->getLatestSampleTimestamp(timestamp_sample)) {
			perf_set_elapsed(_control_latency_perf, actuator_outputs.timestamp - timestamp_sample);
		}
	}
}

عائم MixingOutput::actual_Failsafe_قدر(صحيح index) ثابت
{
	عائم value = 0;

	إذا (failsafe_قدر[index] == UINT16_MAX) { // if set to default, use the one provided by the function
		عائم default_failsafe = NAN;

		إذا (_functions[index]) {
			default_failsafe = _functions[index]->default_Failsafe_قدر(_function_assignment[index]);
		}

		value = output_limit_calc_single(index, default_failsafe);

	} وإلا {
		value = static_cast<عائم>(failsafe_قدر[index]);
	}

	أرجع value;
}
