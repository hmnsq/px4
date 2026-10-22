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
	using Constructor = FunctionProviderBase * (*)(स्थिरांक FunctionProviderBase::Context &context);
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

MixingOutput::MixingOutput(स्थिरांक अक्षर *param_prefix, uint8_t max_अंक_outputs, OutputModuleInterface &interface,
			   SchedulingPolicy scheduling_policy, तार्किक support_esc_calibration, तार्किक ramp_up, स्थिरांक uint8_t instance_start) :
	ModuleParams(&interface),
	_output_ramp_up(ramp_up),
	_scheduling_policy(scheduling_policy),
	_support_esc_calibration(support_esc_calibration),
	max_अंक_outputs_2(max_अंक_outputs < MAX_ACTUATORS ? max_अंक_outputs : MAX_ACTUATORS),
	_interface(interface),
	_control_latency_perf(perf_alloc(PC_ELAPSED, "control latency")),
	_param_prefix(param_prefix)
{
	/* Safely initialize armed flags */
	_armed.armed = असत्य;
	_armed.prearmed = असत्य;
	_armed.ready_to_arm = असत्य;
	_armed.lockdown = असत्य;
	_armed.termination = असत्य;
	_armed.in_esc_calibration_mode = असत्य;

	px4_sem_init(&_lock, 0, 1);

	initParamHandles(instance_start);

	साठी (अचिन्हित i = 0; i < MAX_ACTUATORS; i++) {
		failsafe_मूल्य[i] = UINT16_MAX;
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

शून्य MixingOutput::initParamHandles(स्थिरांक uint8_t instance_start)
{
	अक्षर param_name[17];

	साठी (अचिन्हित i = 0; i < max_अंक_outputs_2; ++i) {
		snprintf(param_name, आकार(param_name), "%s_%s%d", _param_prefix, "FUNC", i + instance_start);
		_param_handles[i].function = param_find(param_name);
		snprintf(param_name, आकार(param_name), "%s_%s%d", _param_prefix, "DIS", i + instance_start);
		_param_handles[i].disarmed = param_find(param_name);
		snprintf(param_name, आकार(param_name), "%s_%s%d", _param_prefix, "MIN", i + instance_start);
		_param_handles[i].min = param_find(param_name);
		snprintf(param_name, आकार(param_name), "%s_%s%d", _param_prefix, "CENT", i + instance_start);
		_param_handles[i].center = param_find(param_name);
		snprintf(param_name, आकार(param_name), "%s_%s%d", _param_prefix, "MAX", i + instance_start);
		_param_handles[i].max = param_find(param_name);
		snprintf(param_name, आकार(param_name), "%s_%s%d", _param_prefix, "FAIL", i + instance_start);
		_param_handles[i].failsafe = param_find(param_name);
	}

	snprintf(param_name, आकार(param_name), "%s_%s", _param_prefix, "REV");
	_param_handle_rev_range = param_find(param_name);
}

शून्य MixingOutput::printStatus() स्थिरांक
{
	PX4_INFO("Param prefix: %s", _param_prefix);
	perf_print_counter(_control_latency_perf);

	जर (_wq_switched) {
		PX4_INFO("Switched to rate_ctrl work queue");
	}

	PX4_INFO_RAW("Channel Configuration:\n");

	साठी (अचिन्हित i = 0; i < max_अंक_outputs_2; i++) {
		PX4_INFO_RAW("Channel %2d: func: %3d, value: %.2f, failsafe: %.2f, disarmed: %d, min: %d, max: %d, center: %d\n",
			     i, (पूर्णांक)_function_assignment[i], (द्विगुण)current_output_मूल्य[i], (द्विगुण)actual_Failsafe_मूल्य(i),
			     disarmed_मूल्य[i], min_मूल्य[i], max_मूल्य[i], center_मूल्य[i]);
	}
}

शून्य MixingOutput::updateParams()
{
	ModuleParams::updateParams();

	तार्किक function_changed = असत्य;

	साठी (अचिन्हित i = 0; i < max_अंक_outputs_2; i++) {
		int32_t val;

		जर (_param_handles[i].function != PARAM_INVALID && param_get(_param_handles[i].function, &val) == 0) {
			जर (val != (int32_t)_function_assignment[i]) {
				function_changed = सत्य;
			}

			// we set _function_assignment[i] later to ensure _functions[i] is updated at the same time
		}

		जर (_param_handles[i].disarmed != PARAM_INVALID && param_get(_param_handles[i].disarmed, &val) == 0) {
			disarmed_मूल्य[i] = val;
		}

		जर (_param_handles[i].min != PARAM_INVALID && param_get(_param_handles[i].min, &val) == 0) {
			min_मूल्य[i] = val;
		}

		जर (_param_handles[i].center != PARAM_INVALID && param_get(_param_handles[i].center, &val) == 0) {
			center_मूल्य[i] = val;
		}

		जर (_param_handles[i].max != PARAM_INVALID && param_get(_param_handles[i].max, &val) == 0) {
			max_मूल्य[i] = val;
		}

		जर (min_मूल्य[i] > max_मूल्य[i]) {
			uint16_t tmp = min_मूल्य[i];
			min_मूल्य[i] = max_मूल्य[i];
			max_मूल्य[i] = tmp;
		}


		जर (_param_handles[i].failsafe != PARAM_INVALID && param_get(_param_handles[i].failsafe, &val) == 0) {
			failsafe_मूल्य[i] = val;
		}
	}

	_reverse_output_mask = 0;
	int32_t rev_range_param;

	जर (_param_handle_rev_range != PARAM_INVALID && param_get(_param_handle_rev_range, &rev_range_param) == 0) {
		_reverse_output_mask = rev_range_param;
	}

	जर (function_changed) {
		_need_function_update = सत्य;
	}
}

शून्य MixingOutput::cleanupFunctions()
{
	जर (_subscription_callback) {
		_subscription_callback->unregisterCallback();
		_subscription_callback = nullptr;
	}

	साठी (पूर्णांक i = 0; i < MAX_ACTUATORS; ++i) {
		delete _function_allocated[i];
		_function_allocated[i] = nullptr;
		_functions[i] = nullptr;
	}
}

तार्किक MixingOutput::updateSubscriptions(तार्किक allow_wq_switch)
{
	जर (!_need_function_update || _armed.armed) {
		परत असत्य;
	}

	// must be locked to potentially change WorkQueue
	lock();

	_has_backup_schedule = असत्य;

	जर (_scheduling_policy == SchedulingPolicy::Auto) {
		// first clear everything
		unregister();
		_interface.ScheduleClear();

		तार्किक switch_requested = असत्य;

		// potentially switch work queue if we run motor outputs
		साठी (अचिन्हित i = 0; i < max_अंक_outputs_2; i++) {
			// read function directly from param, as _function_assignment[i] is updated later
			int32_t function;

			जर (_param_handles[i].function != PARAM_INVALID && param_get(_param_handles[i].function, &function) == 0) {
				जर (function >= (int32_t)OutputFunction::Motor1 && function <= (int32_t)OutputFunction::MotorMax) {
					switch_requested = सत्य;
				}
			}
		}

		जर (allow_wq_switch && !_wq_switched && switch_requested) {
			जर (_interface.ChangeWorkQueue(px4::wq_configurations::rate_ctrl)) {
				// let the new WQ handle the subscribe update
				_wq_switched = सत्य;
				_interface.ScheduleNow();
				unlock();
				परत असत्य;
			}
		}
	}

	// Now update the functions
	PX4_DEBUG("updating functions");

	cleanupFunctions();

	स्थिरांक FunctionProviderBase::Context context{_interface, _param_thr_mdl_fac.reference()};
	पूर्णांक provider_indexes[MAX_ACTUATORS] {};
	पूर्णांक next_provider = 0;
	पूर्णांक subscription_callback_provider_index = पूर्णांक_उच्च;
	तार्किक all_disabled = सत्य;

	साठी (पूर्णांक i = 0; i < max_अंक_outputs_2; ++i) {
		int32_t val;

		जर (_param_handles[i].function != PARAM_INVALID && param_get(_param_handles[i].function, &val) == 0) {
			_function_assignment[i] = (OutputFunction)val;

		} अन्यथा {
			_function_assignment[i] = OutputFunction::Disabled;
		}

		साठी (पूर्णांक p = 0; p < (पूर्णांक)(आकार(all_function_providers) / आकार(all_function_providers[0])); ++p) {
			जर (_function_assignment[i] >= all_function_providers[p].min_func &&
			    _function_assignment[i] <= all_function_providers[p].max_func) {
				all_disabled = असत्य;
				पूर्णांक found_index = -1;

				साठी (पूर्णांक existing = 0; existing < next_provider; ++existing) {
					जर (provider_indexes[existing] == p) {
						found_index = existing;
						थांबा;
					}
				}

				जर (found_index >= 0) {
					_functions[i] = _function_allocated[found_index];

				} अन्यथा {
					_function_allocated[next_provider] = all_function_providers[p].constructor(context);

					जर (_function_allocated[next_provider]) {
						_functions[i] = _function_allocated[next_provider];
						provider_indexes[next_provider++] = p;

						// lowest provider takes precedence for scheduling
						जर (p < subscription_callback_provider_index && _functions[i]->subscriptionCallback()) {
							subscription_callback_provider_index = p;
							_subscription_callback = _functions[i]->subscriptionCallback();
						}

					} अन्यथा {
						PX4_ERR("function alloc failed");
					}
				}

				थांबा;
			}
		}
	}

	hrt_abstime fixed_rate_scheduling_interval = 4_ms; // schedule at 250Hz

	जर (_max_topic_update_interval_us > fixed_rate_scheduling_interval) {
		fixed_rate_scheduling_interval = _max_topic_update_interval_us;
	}

	जर (_scheduling_policy == SchedulingPolicy::Auto) {
		जर (_subscription_callback) {
			जर (_subscription_callback->registerCallback()) {
				PX4_DEBUG("Scheduling via callback");
				_has_backup_schedule = सत्य;
				_interface.ScheduleDelayed(50_ms);

			} अन्यथा {
				PX4_ERR("registerCallback failed, scheduling at fixed rate");
				_interface.ScheduleOnInterval(fixed_rate_scheduling_interval);
			}

		} अन्यथा जर (all_disabled) {
			_interface.ScheduleOnInterval(_lowrate_schedule_interval);
			PX4_DEBUG("Scheduling at low rate");

		} अन्यथा {
			_interface.ScheduleOnInterval(fixed_rate_scheduling_interval);
			PX4_DEBUG("Scheduling at fixed rate");
		}
	}

	setMaxTopicUpdateRate(_max_topic_update_interval_us);
	_need_function_update = असत्य;

	_actuator_test.reset();

	unlock();

	_interface.mixerChanged();

	परत सत्य;
}

शून्य MixingOutput::setMaxTopicUpdateRate(अचिन्हित max_topic_update_interval_us)
{
	_max_topic_update_interval_us = max_topic_update_interval_us;

	जर (_subscription_callback) {
		_subscription_callback->set_interval_us(_max_topic_update_interval_us);
	}
}

शून्य MixingOutput::set_All_Min_मूल्ये(uint16_t value)
{
	साठी (अचिन्हित i = 0; i < MAX_ACTUATORS; i++) {
		_param_handles[i].min = PARAM_INVALID;
		min_मूल्य[i] = value;
	}
}

शून्य MixingOutput::set_All_Center_मूल्ये(uint16_t value)
{
	साठी (अचिन्हित i = 0; i < MAX_ACTUATORS; i++) {
		_param_handles[i].center = PARAM_INVALID;
		center_मूल्य[i] = value;
	}
}

शून्य MixingOutput::set_All_Max_मूल्ये(uint16_t value)
{
	साठी (अचिन्हित i = 0; i < MAX_ACTUATORS; i++) {
		_param_handles[i].max = PARAM_INVALID;
		max_मूल्य[i] = value;
	}
}

शून्य MixingOutput::set_All_Failsafe_मूल्ये(uint16_t value)
{
	साठी (अचिन्हित i = 0; i < MAX_ACTUATORS; i++) {
		_param_handles[i].failsafe = PARAM_INVALID;
		failsafe_मूल्य[i] = value;
	}
}

शून्य MixingOutput::set_All_Disarmed_मूल्ये(uint16_t value)
{
	साठी (अचिन्हित i = 0; i < MAX_ACTUATORS; i++) {
		_param_handles[i].disarmed = PARAM_INVALID;
		disarmed_मूल्य[i] = value;
	}
}

शून्य MixingOutput::unregister()
{
	जर (_subscription_callback) {
		_subscription_callback->unregisterCallback();
	}
}

तार्किक MixingOutput::update()
{
	// check arming state
	जर (_armed_sub.update(&_armed)) {
		_armed.in_esc_calibration_mode &= _support_esc_calibration;

		जर (_ignore_lockdown) {
			_armed.lockdown = असत्य;
		}

		/* Update the armed status and check that we're not locked down.
		 * We also need to arm throttle for the ESC calibration. */
		_throttle_armed = (_armed.armed && !_armed.lockdown) || _armed.in_esc_calibration_mode;
	}

	// only used for sitl with lockstep
	तार्किक has_updates = _subscription_callback && _subscription_callback->updated();

	// update topics
	साठी (पूर्णांक i = 0; i < MAX_ACTUATORS && _function_allocated[i]; ++i) {
		_function_allocated[i]->update();
	}

	जर (_has_backup_schedule) {
		_interface.ScheduleDelayed(50_ms);
	}

	// check for actuator test
	_actuator_test.update(max_अंक_outputs_2, _param_thr_mdl_fac.get());

	// get output values
	दशांश outputs[MAX_ACTUATORS];
	तार्किक all_disabled = सत्य;
	स्थिरांक uint32_t reversible_mask_prev = _reversible_mask;
	_reversible_mask = 0;

	साठी (पूर्णांक i = 0; i < max_अंक_outputs_2; ++i) {
		जर (_functions[i]) {
			all_disabled = असत्य;

			जर (_armed.armed || (_armed.prearmed && _functions[i]->allowPrearmControl())) {
				outputs[i] = _functions[i]->value(_function_assignment[i]);

			} अन्यथा {
				outputs[i] = न_अंक;
			}

			_reversible_mask |= (uint32_t)_functions[i]->reversible(_function_assignment[i]) << i;

		} अन्यथा {
			outputs[i] = न_अंक;
		}
	}

	जर (_reversible_mask != reversible_mask_prev) {
		_interface.reversibleMaskChanged(_reversible_mask);
	}

	// Send output if any function mapped or one last disabling sample
	जर (!all_disabled || !_was_all_disabled) {
		जर (!_armed.armed && !_armed.समाप्त) {
			_actuator_test.override_मूल्ये(outputs, max_अंक_outputs_2);
		}

		limitAndUpdateOutputs(outputs, has_updates);
	}

	_was_all_disabled = all_disabled;

	परत सत्य;
}

शून्य
MixingOutput::limitAndUpdateOutputs(दशांश outputs[MAX_ACTUATORS], तार्किक has_updates)
{
	जर (_armed.lockdown || _armed.समाप्त) {
		// overwrite outputs in case of lockdown with disarmed values
		साठी (माप_प्रकार i = 0; i < max_अंक_outputs_2; i++) {
			current_output_मूल्य[i] = disarmed_मूल्य[i];
		}

	} अन्यथा जर (_armed.termination) {
		// Overwrite outputs with _failsafe_value when terminated
		साठी (माप_प्रकार i = 0; i < max_अंक_outputs_2; i++) {
			current_output_मूल्य[i] = actual_Failsafe_मूल्य(i);
		}

	} अन्यथा {
		// the output limit call takes care of out of band errors, NaN and constrains
		output_limit_calc(_throttle_armed || _actuator_test.inTestMode(), max_अंक_outputs_2, outputs);
	}

	// We must calibrate the PWM and Oneshot ESCs to a consistent range of 1000-2000us (gets mapped to 125-250us for Oneshot)
	// Doing so makes calibrations consistent among different configurations and hence PWM minimum and maximum have a consistent effect
	// hence the defaults for these parameters also make most setups work out of the box
	जर (_armed.in_esc_calibration_mode) {
		स्थिर constexpr दशांश PWM_CALIBRATION_खालचा = 1000.f;
		स्थिर constexpr दशांश PWM_CALIBRATION_वरचा = 2000.f;

		साठी (पूर्णांक i = 0; i < max_अंक_outputs_2; i++) {
			जर (fabsf(current_output_मूल्य[i] - (दशांश)min_मूल्य[i]) < 0.5f) {
				current_output_मूल्य[i] = PWM_CALIBRATION_खालचा;
			}

			जर (fabsf(current_output_मूल्य[i] - (दशांश)max_मूल्य[i]) < 0.5f) {
				current_output_मूल्य[i] = PWM_CALIBRATION_वरचा;
			}
		}
	}

	/* now return the outputs to the driver */
	जर (_interface.updateOutputs(current_output_मूल्य, max_अंक_outputs_2, has_updates)) {
		actuator_outputs_s actuator_outputs{};
		setAndPublishActuatorOutputs(max_अंक_outputs_2, actuator_outputs);

		updateLatencyPerfCounter(actuator_outputs);
	}
}

दशांश MixingOutput::output_limit_calc_single(पूर्णांक i, दशांश value) स्थिरांक
{
	// check for invalid / disabled channels
	जर (!PX4_ISFINITE(value)) {
		परत disarmed_मूल्य[i];
	}

	जर (_reverse_output_mask & (1 << i)) {
		value = -1.f * value;
	}

	दशांश output = disarmed_मूल्य[i];

	जर (((_function_assignment[i] >= OutputFunction::Servo1
	      && _function_assignment[i] <= OutputFunction::ServoMax) || _function_assignment[i] == OutputFunction::Landing_Gear_Wheel
	     || (_function_assignment[i] >= OutputFunction::Gimbal_Roll
		 && _function_assignment[i] <= OutputFunction::Gimbal_Yaw))
	    && _param_handles[i].center != PARAM_INVALID
	    && center_मूल्य[i] >= min_मूल्य[i]
	    && center_मूल्य[i] <= max_मूल्य[i]) {
		output = math::interpolateNXY(value, {-1.f, 0.f, 1.f}, {(दशांश)min_मूल्य[i], (दशांश)center_मूल्य[i], (दशांश)max_मूल्य[i]});
	}

	// Everything except servos, or if center is not set
	अन्यथा {
		output = math::interpolate(value, -1.f, 1.f, static_cast<दशांश>(min_मूल्य[i]), static_cast<दशांश>(max_मूल्य[i]));
	}

	परत output;
}

शून्य
MixingOutput::output_limit_calc(स्थिरांक तार्किक armed, स्थिरांक पूर्णांक अंक_channels, स्थिरांक दशांश output[MAX_ACTUATORS])
{
	// time to slowly ramp up the ESCs
	स्थिर constexpr hrt_abstime RAMP_TIME_US = 500_ms;

	/* first evaluate state changes */
	निवड (_output_state) {
	स्थिती OutputLimitState::OFF:
		जर (armed) {
			जर (_output_ramp_up) {
				_output_state = OutputLimitState::RAMP;

			} अन्यथा {
				_output_state = OutputLimitState::ON;
			}

			// reset arming time, used for ramp timing
			_output_time_armed = hrt_absolute_time();
		}

		थांबा;

	स्थिती OutputLimitState::RAMP:
		जर (!armed) {
			_output_state = OutputLimitState::OFF;

		} अन्यथा जर (hrt_elapsed_time(&_output_time_armed) >= RAMP_TIME_US) {
			_output_state = OutputLimitState::ON;
		}

		थांबा;

	स्थिती OutputLimitState::ON:
		जर (!armed) {
			_output_state = OutputLimitState::OFF;
		}

		थांबा;
	}

	/* if the system is pre-armed, the limit state is temporarily on,
	 * as some outputs are valid and the non-valid outputs have been
	 * set to NaN. This is not stored in the state machine though,
	 * as the throttle channels need to go through the ramp at
	 * regular arming time.
	 */
	स्वतः local_limit_state = _output_state;

	जर (isPrearmed()) {
		local_limit_state = OutputLimitState::ON;
	}

	// then set _current_output_value based on state
	निवड (local_limit_state) {
	स्थिती OutputLimitState::OFF:
		साठी (पूर्णांक i = 0; i < अंक_channels; i++) {
			current_output_मूल्य[i] = disarmed_मूल्य[i];
		}

		थांबा;

	स्थिती OutputLimitState::RAMP: {
			hrt_abstime diff = hrt_elapsed_time(&_output_time_armed);
			दशांश progress = static_cast<दशांश>(diff) / RAMP_TIME_US;

			जर (progress > 1.f) {
				progress = 1.f;
			}

			साठी (पूर्णांक i = 0; i < अंक_channels; i++) {
				// Ramp from disarmed value to currently desired output that would apply without ramp
				दशांश desired_output = output_limit_calc_single(i, output[i]);
				current_output_मूल्य[i] = disarmed_मूल्य[i] + progress * (desired_output - disarmed_मूल्य[i]);
			}
		}
		थांबा;

	स्थिती OutputLimitState::ON:
		साठी (पूर्णांक i = 0; i < अंक_channels; i++) {
			current_output_मूल्य[i] = output_limit_calc_single(i, output[i]);
		}

		थांबा;
	}
}

शून्य
MixingOutput::setAndPublishActuatorOutputs(अचिन्हित अंक_outputs, actuator_outputs_s &actuator_outputs)
{
	actuator_outputs.noutputs = अंक_outputs;

	साठी (माप_प्रकार i = 0; i < अंक_outputs; ++i) {
		actuator_outputs.output[i] = current_output_मूल्य[i];
	}

	actuator_outputs.timestamp = hrt_absolute_time();
	_outputs_pub.publish(actuator_outputs);
}

शून्य
MixingOutput::updateLatencyPerfCounter(स्थिरांक actuator_outputs_s &actuator_outputs)
{
	// Just check the first function. It means we only get the latency if motors are assigned first, which is the default
	जर (_function_allocated[0]) {
		hrt_abstime timestamp_sample;

		जर (_function_allocated[0]->getLatestSampleTimestamp(timestamp_sample)) {
			perf_set_elapsed(_control_latency_perf, actuator_outputs.timestamp - timestamp_sample);
		}
	}
}

दशांश MixingOutput::actual_Failsafe_मूल्य(पूर्णांक index) स्थिरांक
{
	दशांश value = 0;

	जर (failsafe_मूल्य[index] == UINT16_MAX) { // if set to default, use the one provided by the function
		दशांश default_failsafe = न_अंक;

		जर (_functions[index]) {
			default_failsafe = _functions[index]->default_Failsafe_मूल्य(_function_assignment[index]);
		}

		value = output_limit_calc_single(index, default_failsafe);

	} अन्यथा {
		value = static_cast<दशांश>(failsafe_मूल्य[index]);
	}

	परत value;
}
