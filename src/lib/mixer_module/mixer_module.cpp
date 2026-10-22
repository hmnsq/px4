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

#अन्तर्भाव "mixer_module.hpp"

#अन्तर्भाव <uORB/Publication.hpp>
#अन्तर्भाव <px4_platform_common/log.h>

using नामाकाश time_literals;


संरचना FunctionProvider {
	using Constructor = FunctionProviderBase * (*)(ध्रुव FunctionProviderBase::Context &context);
	constexpr FunctionProvider(OutputFunction min_func_, OutputFunction max_func_, Constructor constructor_)
		: min_func(min_func_), max_func(max_func_), constructor(constructor_) {}
	constexpr FunctionProvider(OutputFunction func, Constructor constructor_)
		: min_func(func), max_func(func), constructor(constructor_) {}

	OutputFunction min_func;
	OutputFunction max_func;
	Constructor constructor;
};

स्थिर constexpr FunctionProvider all_function_providers[] = {
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

MixingOutput::MixingOutput(ध्रुव वर्ण *param_prefix, uint8_t max_अङ्कः_outputs, OutputModuleInterface &interface,
			   SchedulingPolicy scheduling_policy, तार्किक support_esc_calibration, तार्किक ramp_up, ध्रुव uint8_t instance_start) :
	ModuleParams(&interface),
	_output_ramp_up(ramp_up),
	_scheduling_policy(scheduling_policy),
	_support_esc_calibration(support_esc_calibration),
	max_अङ्कः_outputs_2(max_अङ्कः_outputs < MAX_ACTUATORS ? max_अङ्कः_outputs : MAX_ACTUATORS),
	_interface(interface),
	_control_latency_perf(perf_alloc(PC_ELAPSED, "control latency")),
	_param_prefix(param_prefix)
{
	/* Safely initialize armed flags */
	_armed.armed = मिच्छ;
	_armed.prearmed = मिच्छ;
	_armed.ready_to_arm = मिच्छ;
	_armed.lockdown = मिच्छ;
	_armed.termination = मिच्छ;
	_armed.in_esc_calibration_mode = मिच्छ;

	px4_sem_init(&_lock, 0, 1);

	initParamHandles(instance_start);

	प्रति (unsigned i = 0; i < MAX_ACTUATORS; i++) {
		failsafe_मानम्[i] = UINT16_MAX;
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

शून्य MixingOutput::initParamHandles(ध्रुव uint8_t instance_start)
{
	वर्ण param_name[17];

	प्रति (unsigned i = 0; i < max_अङ्कः_outputs_2; ++i) {
		snprintf(param_name, परिमाण(param_name), "%s_%s%d", _param_prefix, "FUNC", i + instance_start);
		_param_handles[i].function = param_find(param_name);
		snprintf(param_name, परिमाण(param_name), "%s_%s%d", _param_prefix, "DIS", i + instance_start);
		_param_handles[i].disarmed = param_find(param_name);
		snprintf(param_name, परिमाण(param_name), "%s_%s%d", _param_prefix, "MIN", i + instance_start);
		_param_handles[i].min = param_find(param_name);
		snprintf(param_name, परिमाण(param_name), "%s_%s%d", _param_prefix, "CENT", i + instance_start);
		_param_handles[i].center = param_find(param_name);
		snprintf(param_name, परिमाण(param_name), "%s_%s%d", _param_prefix, "MAX", i + instance_start);
		_param_handles[i].max = param_find(param_name);
		snprintf(param_name, परिमाण(param_name), "%s_%s%d", _param_prefix, "FAIL", i + instance_start);
		_param_handles[i].failsafe = param_find(param_name);
	}

	snprintf(param_name, परिमाण(param_name), "%s_%s", _param_prefix, "REV");
	_param_handle_rev_range = param_find(param_name);
}

शून्य MixingOutput::printStatus() ध्रुव
{
	PX4_INFO("Param prefix: %s", _param_prefix);
	perf_print_counter(_control_latency_perf);

	जइ (_wq_switched) {
		PX4_INFO("Switched to rate_ctrl work queue");
	}

	PX4_INFO_RAW("Channel Configuration:\n");

	प्रति (unsigned i = 0; i < max_अङ्कः_outputs_2; i++) {
		PX4_INFO_RAW("Channel %2d: func: %3d, value: %.2f, failsafe: %.2f, disarmed: %d, min: %d, max: %d, center: %d\n",
			     i, (पूर्णाङ्क)_function_assignment[i], (द्विगुण)current_output_मानम्[i], (द्विगुण)actual_Failsafe_मानम्(i),
			     disarmed_मानम्[i], min_मानम्[i], max_मानम्[i], center_मानम्[i]);
	}
}

शून्य MixingOutput::updateParams()
{
	ModuleParams::updateParams();

	तार्किक function_changed = मिच्छ;

	प्रति (unsigned i = 0; i < max_अङ्कः_outputs_2; i++) {
		int32_t val;

		जइ (_param_handles[i].function != PARAM_INVALID && param_get(_param_handles[i].function, &val) == 0) {
			जइ (val != (int32_t)_function_assignment[i]) {
				function_changed = सच्च;
			}

			// we set _function_assignment[i] later to ensure _functions[i] is updated at the same time
		}

		जइ (_param_handles[i].disarmed != PARAM_INVALID && param_get(_param_handles[i].disarmed, &val) == 0) {
			disarmed_मानम्[i] = val;
		}

		जइ (_param_handles[i].min != PARAM_INVALID && param_get(_param_handles[i].min, &val) == 0) {
			min_मानम्[i] = val;
		}

		जइ (_param_handles[i].center != PARAM_INVALID && param_get(_param_handles[i].center, &val) == 0) {
			center_मानम्[i] = val;
		}

		जइ (_param_handles[i].max != PARAM_INVALID && param_get(_param_handles[i].max, &val) == 0) {
			max_मानम्[i] = val;
		}

		जइ (min_मानम्[i] > max_मानम्[i]) {
			uint16_t tmp = min_मानम्[i];
			min_मानम्[i] = max_मानम्[i];
			max_मानम्[i] = tmp;
		}


		जइ (_param_handles[i].failsafe != PARAM_INVALID && param_get(_param_handles[i].failsafe, &val) == 0) {
			failsafe_मानम्[i] = val;
		}
	}

	_reverse_output_mask = 0;
	int32_t rev_range_param;

	जइ (_param_handle_rev_range != PARAM_INVALID && param_get(_param_handle_rev_range, &rev_range_param) == 0) {
		_reverse_output_mask = rev_range_param;
	}

	जइ (function_changed) {
		_need_function_update = सच्च;
	}
}

शून्य MixingOutput::cleanupFunctions()
{
	जइ (_subscription_callback) {
		_subscription_callback->unregisterCallback();
		_subscription_callback = nullptr;
	}

	प्रति (पूर्णाङ्क i = 0; i < MAX_ACTUATORS; ++i) {
		delete _function_allocated[i];
		_function_allocated[i] = nullptr;
		_functions[i] = nullptr;
	}
}

तार्किक MixingOutput::updateSubscriptions(तार्किक allow_wq_switch)
{
	जइ (!_need_function_update || _armed.armed) {
		प्रत्यागम मिच्छ;
	}

	// must be locked to potentially change WorkQueue
	lock();

	_has_backup_schedule = मिच्छ;

	जइ (_scheduling_policy == SchedulingPolicy::Auto) {
		// first clear everything
		unregister();
		_interface.ScheduleClear();

		तार्किक switch_requested = मिच्छ;

		// potentially switch work queue if we run motor outputs
		प्रति (unsigned i = 0; i < max_अङ्कः_outputs_2; i++) {
			// read function directly from param, as _function_assignment[i] is updated later
			int32_t function;

			जइ (_param_handles[i].function != PARAM_INVALID && param_get(_param_handles[i].function, &function) == 0) {
				जइ (function >= (int32_t)OutputFunction::Motor1 && function <= (int32_t)OutputFunction::MotorMax) {
					switch_requested = सच्च;
				}
			}
		}

		जइ (allow_wq_switch && !_wq_switched && switch_requested) {
			जइ (_interface.ChangeWorkQueue(px4::wq_configurations::rate_ctrl)) {
				// let the new WQ handle the subscribe update
				_wq_switched = सच्च;
				_interface.ScheduleNow();
				unlock();
				प्रत्यागम मिच्छ;
			}
		}
	}

	// Now update the functions
	PX4_DEBUG("updating functions");

	cleanupFunctions();

	ध्रुव FunctionProviderBase::Context context{_interface, _param_thr_mdl_fac.reference()};
	पूर्णाङ्क provider_indexes[MAX_ACTUATORS] {};
	पूर्णाङ्क next_provider = 0;
	पूर्णाङ्क subscription_callback_provider_index = INT_MAX;
	तार्किक all_disabled = सच्च;

	प्रति (पूर्णाङ्क i = 0; i < max_अङ्कः_outputs_2; ++i) {
		int32_t val;

		जइ (_param_handles[i].function != PARAM_INVALID && param_get(_param_handles[i].function, &val) == 0) {
			_function_assignment[i] = (OutputFunction)val;

		} अण्णहा {
			_function_assignment[i] = OutputFunction::Disabled;
		}

		प्रति (पूर्णाङ्क p = 0; p < (पूर्णाङ्क)(परिमाण(all_function_providers) / परिमाण(all_function_providers[0])); ++p) {
			जइ (_function_assignment[i] >= all_function_providers[p].min_func &&
			    _function_assignment[i] <= all_function_providers[p].max_func) {
				all_disabled = मिच्छ;
				पूर्णाङ्क found_index = -1;

				प्रति (पूर्णाङ्क existing = 0; existing < next_provider; ++existing) {
					जइ (provider_indexes[existing] == p) {
						found_index = existing;
						भङ्ग;
					}
				}

				जइ (found_index >= 0) {
					_functions[i] = _function_allocated[found_index];

				} अण्णहा {
					_function_allocated[next_provider] = all_function_providers[p].constructor(context);

					जइ (_function_allocated[next_provider]) {
						_functions[i] = _function_allocated[next_provider];
						provider_indexes[next_provider++] = p;

						// lowest provider takes precedence for scheduling
						जइ (p < subscription_callback_provider_index && _functions[i]->subscriptionCallback()) {
							subscription_callback_provider_index = p;
							_subscription_callback = _functions[i]->subscriptionCallback();
						}

					} अण्णहा {
						PX4_ERR("function alloc failed");
					}
				}

				भङ्ग;
			}
		}
	}

	hrt_abstime fixed_rate_scheduling_interval = 4_ms; // schedule at 250Hz

	जइ (_max_topic_update_interval_us > fixed_rate_scheduling_interval) {
		fixed_rate_scheduling_interval = _max_topic_update_interval_us;
	}

	जइ (_scheduling_policy == SchedulingPolicy::Auto) {
		जइ (_subscription_callback) {
			जइ (_subscription_callback->registerCallback()) {
				PX4_DEBUG("Scheduling via callback");
				_has_backup_schedule = सच्च;
				_interface.ScheduleDelayed(50_ms);

			} अण्णहा {
				PX4_ERR("registerCallback failed, scheduling at fixed rate");
				_interface.ScheduleOnInterval(fixed_rate_scheduling_interval);
			}

		} अण्णहा जइ (all_disabled) {
			_interface.ScheduleOnInterval(_lowrate_schedule_interval);
			PX4_DEBUG("Scheduling at low rate");

		} अण्णहा {
			_interface.ScheduleOnInterval(fixed_rate_scheduling_interval);
			PX4_DEBUG("Scheduling at fixed rate");
		}
	}

	setMaxTopicUpdateRate(_max_topic_update_interval_us);
	_need_function_update = मिच्छ;

	_actuator_test.reset();

	unlock();

	_interface.mixerChanged();

	प्रत्यागम सच्च;
}

शून्य MixingOutput::setMaxTopicUpdateRate(unsigned max_topic_update_interval_us)
{
	_max_topic_update_interval_us = max_topic_update_interval_us;

	जइ (_subscription_callback) {
		_subscription_callback->set_interval_us(_max_topic_update_interval_us);
	}
}

शून्य MixingOutput::set_All_Min_मानानि(uint16_t value)
{
	प्रति (unsigned i = 0; i < MAX_ACTUATORS; i++) {
		_param_handles[i].min = PARAM_INVALID;
		min_मानम्[i] = value;
	}
}

शून्य MixingOutput::set_All_Center_मानानि(uint16_t value)
{
	प्रति (unsigned i = 0; i < MAX_ACTUATORS; i++) {
		_param_handles[i].center = PARAM_INVALID;
		center_मानम्[i] = value;
	}
}

शून्य MixingOutput::set_All_Max_मानानि(uint16_t value)
{
	प्रति (unsigned i = 0; i < MAX_ACTUATORS; i++) {
		_param_handles[i].max = PARAM_INVALID;
		max_मानम्[i] = value;
	}
}

शून्य MixingOutput::set_All_Failsafe_मानानि(uint16_t value)
{
	प्रति (unsigned i = 0; i < MAX_ACTUATORS; i++) {
		_param_handles[i].failsafe = PARAM_INVALID;
		failsafe_मानम्[i] = value;
	}
}

शून्य MixingOutput::set_All_Disarmed_मानानि(uint16_t value)
{
	प्रति (unsigned i = 0; i < MAX_ACTUATORS; i++) {
		_param_handles[i].disarmed = PARAM_INVALID;
		disarmed_मानम्[i] = value;
	}
}

शून्य MixingOutput::unregister()
{
	जइ (_subscription_callback) {
		_subscription_callback->unregisterCallback();
	}
}

तार्किक MixingOutput::update()
{
	// check arming state
	जइ (_armed_sub.update(&_armed)) {
		_armed.in_esc_calibration_mode &= _support_esc_calibration;

		जइ (_ignore_lockdown) {
			_armed.lockdown = मिच्छ;
		}

		/* Update the armed status and check that we're not locked down.
		 * We also need to arm throttle for the ESC calibration. */
		_throttle_armed = (_armed.armed && !_armed.lockdown) || _armed.in_esc_calibration_mode;
	}

	// only used for sitl with lockstep
	तार्किक has_updates = _subscription_callback && _subscription_callback->updated();

	// update topics
	प्रति (पूर्णाङ्क i = 0; i < MAX_ACTUATORS && _function_allocated[i]; ++i) {
		_function_allocated[i]->update();
	}

	जइ (_has_backup_schedule) {
		_interface.ScheduleDelayed(50_ms);
	}

	// check for actuator test
	_actuator_test.update(max_अङ्कः_outputs_2, _param_thr_mdl_fac.get());

	// get output values
	भिन्न outputs[MAX_ACTUATORS];
	तार्किक all_disabled = सच्च;
	ध्रुव uint32_t reversible_mask_prev = _reversible_mask;
	_reversible_mask = 0;

	प्रति (पूर्णाङ्क i = 0; i < max_अङ्कः_outputs_2; ++i) {
		जइ (_functions[i]) {
			all_disabled = मिच्छ;

			जइ (_armed.armed || (_armed.prearmed && _functions[i]->allowPrearmControl())) {
				outputs[i] = _functions[i]->value(_function_assignment[i]);

			} अण्णहा {
				outputs[i] = NAN;
			}

			_reversible_mask |= (uint32_t)_functions[i]->reversible(_function_assignment[i]) << i;

		} अण्णहा {
			outputs[i] = NAN;
		}
	}

	जइ (_reversible_mask != reversible_mask_prev) {
		_interface.reversibleMaskChanged(_reversible_mask);
	}

	// Send output if any function mapped or one last disabling sample
	जइ (!all_disabled || !_was_all_disabled) {
		जइ (!_armed.armed && !_armed.kill) {
			_actuator_test.override_मानानि(outputs, max_अङ्कः_outputs_2);
		}

		limitAndUpdateOutputs(outputs, has_updates);
	}

	_was_all_disabled = all_disabled;

	प्रत्यागम सच्च;
}

शून्य
MixingOutput::limitAndUpdateOutputs(भिन्न outputs[MAX_ACTUATORS], तार्किक has_updates)
{
	जइ (_armed.lockdown || _armed.kill) {
		// overwrite outputs in case of lockdown with disarmed values
		प्रति (size_t i = 0; i < max_अङ्कः_outputs_2; i++) {
			current_output_मानम्[i] = disarmed_मानम्[i];
		}

	} अण्णहा जइ (_armed.termination) {
		// Overwrite outputs with _failsafe_value when terminated
		प्रति (size_t i = 0; i < max_अङ्कः_outputs_2; i++) {
			current_output_मानम्[i] = actual_Failsafe_मानम्(i);
		}

	} अण्णहा {
		// the output limit call takes care of out of band errors, NaN and constrains
		output_limit_calc(_throttle_armed || _actuator_test.inTestMode(), max_अङ्कः_outputs_2, outputs);
	}

	// We must calibrate the PWM and Oneshot ESCs to a consistent range of 1000-2000us (gets mapped to 125-250us for Oneshot)
	// Doing so makes calibrations consistent among different configurations and hence PWM minimum and maximum have a consistent effect
	// hence the defaults for these parameters also make most setups work out of the box
	जइ (_armed.in_esc_calibration_mode) {
		स्थिर constexpr भिन्न PWM_CALIBRATION_नीचः = 1000.f;
		स्थिर constexpr भिन्न PWM_CALIBRATION_उच्चः = 2000.f;

		प्रति (पूर्णाङ्क i = 0; i < max_अङ्कः_outputs_2; i++) {
			जइ (fabsf(current_output_मानम्[i] - (भिन्न)min_मानम्[i]) < 0.5f) {
				current_output_मानम्[i] = PWM_CALIBRATION_नीचः;
			}

			जइ (fabsf(current_output_मानम्[i] - (भिन्न)max_मानम्[i]) < 0.5f) {
				current_output_मानम्[i] = PWM_CALIBRATION_उच्चः;
			}
		}
	}

	/* now return the outputs to the driver */
	जइ (_interface.updateOutputs(current_output_मानम्, max_अङ्कः_outputs_2, has_updates)) {
		actuator_outputs_s actuator_outputs{};
		setAndPublishActuatorOutputs(max_अङ्कः_outputs_2, actuator_outputs);

		updateLatencyPerfCounter(actuator_outputs);
	}
}

भिन्न MixingOutput::output_limit_calc_single(पूर्णाङ्क i, भिन्न value) ध्रुव
{
	// check for invalid / disabled channels
	जइ (!PX4_ISFINITE(value)) {
		प्रत्यागम disarmed_मानम्[i];
	}

	जइ (_reverse_output_mask & (1 << i)) {
		value = -1.f * value;
	}

	भिन्न output = disarmed_मानम्[i];

	जइ (((_function_assignment[i] >= OutputFunction::Servo1
	      && _function_assignment[i] <= OutputFunction::ServoMax) || _function_assignment[i] == OutputFunction::Landing_Gear_Wheel
	     || (_function_assignment[i] >= OutputFunction::Gimbal_Roll
		 && _function_assignment[i] <= OutputFunction::Gimbal_Yaw))
	    && _param_handles[i].center != PARAM_INVALID
	    && center_मानम्[i] >= min_मानम्[i]
	    && center_मानम्[i] <= max_मानम्[i]) {
		output = math::interpolateNXY(value, {-1.f, 0.f, 1.f}, {(भिन्न)min_मानम्[i], (भिन्न)center_मानम्[i], (भिन्न)max_मानम्[i]});
	}

	// Everything except servos, or if center is not set
	अण्णहा {
		output = math::interpolate(value, -1.f, 1.f, static_cast<भिन्न>(min_मानम्[i]), static_cast<भिन्न>(max_मानम्[i]));
	}

	प्रत्यागम output;
}

शून्य
MixingOutput::output_limit_calc(ध्रुव तार्किक armed, ध्रुव पूर्णाङ्क अङ्कः_channels, ध्रुव भिन्न output[MAX_ACTUATORS])
{
	// time to slowly ramp up the ESCs
	स्थिर constexpr hrt_abstime RAMP_TIME_US = 500_ms;

	/* first evaluate state changes */
	विकल्प (_output_state) {
	पक्ष OutputLimitState::OFF:
		जइ (armed) {
			जइ (_output_ramp_up) {
				_output_state = OutputLimitState::RAMP;

			} अण्णहा {
				_output_state = OutputLimitState::ON;
			}

			// reset arming time, used for ramp timing
			_output_time_armed = hrt_absolute_time();
		}

		भङ्ग;

	पक्ष OutputLimitState::RAMP:
		जइ (!armed) {
			_output_state = OutputLimitState::OFF;

		} अण्णहा जइ (hrt_elapsed_time(&_output_time_armed) >= RAMP_TIME_US) {
			_output_state = OutputLimitState::ON;
		}

		भङ्ग;

	पक्ष OutputLimitState::ON:
		जइ (!armed) {
			_output_state = OutputLimitState::OFF;
		}

		भङ्ग;
	}

	/* if the system is pre-armed, the limit state is temporarily on,
	 * as some outputs are valid and the non-valid outputs have been
	 * set to NaN. This is not stored in the state machine though,
	 * as the throttle channels need to go through the ramp at
	 * regular arming time.
	 */
	auto local_limit_state = _output_state;

	जइ (isPrearmed()) {
		local_limit_state = OutputLimitState::ON;
	}

	// then set _current_output_value based on state
	विकल्प (local_limit_state) {
	पक्ष OutputLimitState::OFF:
		प्रति (पूर्णाङ्क i = 0; i < अङ्कः_channels; i++) {
			current_output_मानम्[i] = disarmed_मानम्[i];
		}

		भङ्ग;

	पक्ष OutputLimitState::RAMP: {
			hrt_abstime diff = hrt_elapsed_time(&_output_time_armed);
			भिन्न progress = static_cast<भिन्न>(diff) / RAMP_TIME_US;

			जइ (progress > 1.f) {
				progress = 1.f;
			}

			प्रति (पूर्णाङ्क i = 0; i < अङ्कः_channels; i++) {
				// Ramp from disarmed value to currently desired output that would apply without ramp
				भिन्न desired_output = output_limit_calc_single(i, output[i]);
				current_output_मानम्[i] = disarmed_मानम्[i] + progress * (desired_output - disarmed_मानम्[i]);
			}
		}
		भङ्ग;

	पक्ष OutputLimitState::ON:
		प्रति (पूर्णाङ्क i = 0; i < अङ्कः_channels; i++) {
			current_output_मानम्[i] = output_limit_calc_single(i, output[i]);
		}

		भङ्ग;
	}
}

शून्य
MixingOutput::setAndPublishActuatorOutputs(unsigned अङ्कः_outputs, actuator_outputs_s &actuator_outputs)
{
	actuator_outputs.noutputs = अङ्कः_outputs;

	प्रति (size_t i = 0; i < अङ्कः_outputs; ++i) {
		actuator_outputs.output[i] = current_output_मानम्[i];
	}

	actuator_outputs.timestamp = hrt_absolute_time();
	_outputs_pub.publish(actuator_outputs);
}

शून्य
MixingOutput::updateLatencyPerfCounter(ध्रुव actuator_outputs_s &actuator_outputs)
{
	// Just check the first function. It means we only get the latency if motors are assigned first, which is the default
	जइ (_function_allocated[0]) {
		hrt_abstime timestamp_sample;

		जइ (_function_allocated[0]->getLatestSampleTimestamp(timestamp_sample)) {
			perf_set_elapsed(_control_latency_perf, actuator_outputs.timestamp - timestamp_sample);
		}
	}
}

भिन्न MixingOutput::actual_Failsafe_मानम्(पूर्णाङ्क index) ध्रुव
{
	भिन्न value = 0;

	जइ (failsafe_मानम्[index] == UINT16_MAX) { // if set to default, use the one provided by the function
		भिन्न default_failsafe = NAN;

		जइ (_functions[index]) {
			default_failsafe = _functions[index]->default_Failsafe_मानम्(_function_assignment[index]);
		}

		value = output_limit_calc_single(index, default_failsafe);

	} अण्णहा {
		value = static_cast<भिन्न>(failsafe_मानम्[index]);
	}

	प्रत्यागम value;
}
