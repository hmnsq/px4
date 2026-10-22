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

#અન્તર્ભાવ "mixer_module.hpp"

#અન્તર્ભાવ <uORB/Publication.hpp>
#અન્તર્ભાવ <px4_platform_common/log.h>

using નામાકાશ time_literals;


સંરચના FunctionProvider {
	using Constructor = FunctionProviderBase * (*)(અચલ FunctionProviderBase::Context &context);
	constexpr FunctionProvider(OutputFunction min_func_, OutputFunction max_func_, Constructor constructor_)
		: min_func(min_func_), max_func(max_func_), constructor(constructor_) {}
	constexpr FunctionProvider(OutputFunction func, Constructor constructor_)
		: min_func(func), max_func(func), constructor(constructor_) {}

	OutputFunction min_func;
	OutputFunction max_func;
	Constructor constructor;
};

સ્થિર constexpr FunctionProvider all_function_providers[] = {
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

MixingOutput::MixingOutput(અચલ અક્ષર *param_prefix, uint8_t max_અંક_outputs, OutputModuleInterface &interface,
			   SchedulingPolicy scheduling_policy, તાર્કિક support_esc_calibration, તાર્કિક ramp_up, અચલ uint8_t instance_start) :
	ModuleParams(&interface),
	_output_ramp_up(ramp_up),
	_scheduling_policy(scheduling_policy),
	_support_esc_calibration(support_esc_calibration),
	max_અંક_outputs_2(max_અંક_outputs < MAX_ACTUATORS ? max_અંક_outputs : MAX_ACTUATORS),
	_interface(interface),
	_control_latency_perf(perf_alloc(PC_ELAPSED, "control latency")),
	_param_prefix(param_prefix)
{
	/* Safely initialize armed flags */
	_armed.armed = ખોટું;
	_armed.prearmed = ખોટું;
	_armed.ready_to_arm = ખોટું;
	_armed.lockdown = ખોટું;
	_armed.termination = ખોટું;
	_armed.in_esc_calibration_mode = ખોટું;

	px4_sem_init(&_lock, 0, 1);

	initParamHandles(instance_start);

	માટે (unsigned i = 0; i < MAX_ACTUATORS; i++) {
		failsafe_કિંમત[i] = UINT16_MAX;
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

શૂન્ય MixingOutput::initParamHandles(અચલ uint8_t instance_start)
{
	અક્ષર param_name[17];

	માટે (unsigned i = 0; i < max_અંક_outputs_2; ++i) {
		snprintf(param_name, કદ(param_name), "%s_%s%d", _param_prefix, "FUNC", i + instance_start);
		_param_handles[i].function = param_find(param_name);
		snprintf(param_name, કદ(param_name), "%s_%s%d", _param_prefix, "DIS", i + instance_start);
		_param_handles[i].disarmed = param_find(param_name);
		snprintf(param_name, કદ(param_name), "%s_%s%d", _param_prefix, "MIN", i + instance_start);
		_param_handles[i].min = param_find(param_name);
		snprintf(param_name, કદ(param_name), "%s_%s%d", _param_prefix, "CENT", i + instance_start);
		_param_handles[i].center = param_find(param_name);
		snprintf(param_name, કદ(param_name), "%s_%s%d", _param_prefix, "MAX", i + instance_start);
		_param_handles[i].max = param_find(param_name);
		snprintf(param_name, કદ(param_name), "%s_%s%d", _param_prefix, "FAIL", i + instance_start);
		_param_handles[i].failsafe = param_find(param_name);
	}

	snprintf(param_name, કદ(param_name), "%s_%s", _param_prefix, "REV");
	_param_handle_rev_range = param_find(param_name);
}

શૂન્ય MixingOutput::printStatus() અચલ
{
	PX4_INFO("Param prefix: %s", _param_prefix);
	perf_print_counter(_control_latency_perf);

	જો (_wq_switched) {
		PX4_INFO("Switched to rate_ctrl work queue");
	}

	PX4_INFO_RAW("Channel Configuration:\n");

	માટે (unsigned i = 0; i < max_અંક_outputs_2; i++) {
		PX4_INFO_RAW("Channel %2d: func: %3d, value: %.2f, failsafe: %.2f, disarmed: %d, min: %d, max: %d, center: %d\n",
			     i, (પૂર્ણાંક)_function_assignment[i], (દ્વિગુણ)current_output_કિંમત[i], (દ્વિગુણ)actual_Failsafe_કિંમત(i),
			     disarmed_કિંમત[i], min_કિંમત[i], max_કિંમત[i], center_કિંમત[i]);
	}
}

શૂન્ય MixingOutput::updateParams()
{
	ModuleParams::updateParams();

	તાર્કિક function_changed = ખોટું;

	માટે (unsigned i = 0; i < max_અંક_outputs_2; i++) {
		int32_t val;

		જો (_param_handles[i].function != PARAM_INVALID && param_get(_param_handles[i].function, &val) == 0) {
			જો (val != (int32_t)_function_assignment[i]) {
				function_changed = સાચું;
			}

			// we set _function_assignment[i] later to ensure _functions[i] is updated at the same time
		}

		જો (_param_handles[i].disarmed != PARAM_INVALID && param_get(_param_handles[i].disarmed, &val) == 0) {
			disarmed_કિંમત[i] = val;
		}

		જો (_param_handles[i].min != PARAM_INVALID && param_get(_param_handles[i].min, &val) == 0) {
			min_કિંમત[i] = val;
		}

		જો (_param_handles[i].center != PARAM_INVALID && param_get(_param_handles[i].center, &val) == 0) {
			center_કિંમત[i] = val;
		}

		જો (_param_handles[i].max != PARAM_INVALID && param_get(_param_handles[i].max, &val) == 0) {
			max_કિંમત[i] = val;
		}

		જો (min_કિંમત[i] > max_કિંમત[i]) {
			uint16_t tmp = min_કિંમત[i];
			min_કિંમત[i] = max_કિંમત[i];
			max_કિંમત[i] = tmp;
		}


		જો (_param_handles[i].failsafe != PARAM_INVALID && param_get(_param_handles[i].failsafe, &val) == 0) {
			failsafe_કિંમત[i] = val;
		}
	}

	_reverse_output_mask = 0;
	int32_t rev_range_param;

	જો (_param_handle_rev_range != PARAM_INVALID && param_get(_param_handle_rev_range, &rev_range_param) == 0) {
		_reverse_output_mask = rev_range_param;
	}

	જો (function_changed) {
		_need_function_update = સાચું;
	}
}

શૂન્ય MixingOutput::cleanupFunctions()
{
	જો (_subscription_callback) {
		_subscription_callback->unregisterCallback();
		_subscription_callback = nullptr;
	}

	માટે (પૂર્ણાંક i = 0; i < MAX_ACTUATORS; ++i) {
		delete _function_allocated[i];
		_function_allocated[i] = nullptr;
		_functions[i] = nullptr;
	}
}

તાર્કિક MixingOutput::updateSubscriptions(તાર્કિક allow_wq_switch)
{
	જો (!_need_function_update || _armed.armed) {
		પાછા ખોટું;
	}

	// must be locked to potentially change WorkQueue
	lock();

	_has_backup_schedule = ખોટું;

	જો (_scheduling_policy == SchedulingPolicy::Auto) {
		// first clear everything
		unregister();
		_interface.ScheduleClear();

		તાર્કિક switch_requested = ખોટું;

		// potentially switch work queue if we run motor outputs
		માટે (unsigned i = 0; i < max_અંક_outputs_2; i++) {
			// read function directly from param, as _function_assignment[i] is updated later
			int32_t function;

			જો (_param_handles[i].function != PARAM_INVALID && param_get(_param_handles[i].function, &function) == 0) {
				જો (function >= (int32_t)OutputFunction::Motor1 && function <= (int32_t)OutputFunction::MotorMax) {
					switch_requested = સાચું;
				}
			}
		}

		જો (allow_wq_switch && !_wq_switched && switch_requested) {
			જો (_interface.ChangeWorkQueue(px4::wq_configurations::rate_ctrl)) {
				// let the new WQ handle the subscribe update
				_wq_switched = સાચું;
				_interface.ScheduleNow();
				unlock();
				પાછા ખોટું;
			}
		}
	}

	// Now update the functions
	PX4_DEBUG("updating functions");

	cleanupFunctions();

	અચલ FunctionProviderBase::Context context{_interface, _param_thr_mdl_fac.reference()};
	પૂર્ણાંક provider_indexes[MAX_ACTUATORS] {};
	પૂર્ણાંક next_provider = 0;
	પૂર્ણાંક subscription_callback_provider_index = INT_MAX;
	તાર્કિક all_disabled = સાચું;

	માટે (પૂર્ણાંક i = 0; i < max_અંક_outputs_2; ++i) {
		int32_t val;

		જો (_param_handles[i].function != PARAM_INVALID && param_get(_param_handles[i].function, &val) == 0) {
			_function_assignment[i] = (OutputFunction)val;

		} નહીંતર {
			_function_assignment[i] = OutputFunction::Disabled;
		}

		માટે (પૂર્ણાંક p = 0; p < (પૂર્ણાંક)(કદ(all_function_providers) / કદ(all_function_providers[0])); ++p) {
			જો (_function_assignment[i] >= all_function_providers[p].min_func &&
			    _function_assignment[i] <= all_function_providers[p].max_func) {
				all_disabled = ખોટું;
				પૂર્ણાંક found_index = -1;

				માટે (પૂર્ણાંક existing = 0; existing < next_provider; ++existing) {
					જો (provider_indexes[existing] == p) {
						found_index = existing;
						તોડો;
					}
				}

				જો (found_index >= 0) {
					_functions[i] = _function_allocated[found_index];

				} નહીંતર {
					_function_allocated[next_provider] = all_function_providers[p].constructor(context);

					જો (_function_allocated[next_provider]) {
						_functions[i] = _function_allocated[next_provider];
						provider_indexes[next_provider++] = p;

						// lowest provider takes precedence for scheduling
						જો (p < subscription_callback_provider_index && _functions[i]->subscriptionCallback()) {
							subscription_callback_provider_index = p;
							_subscription_callback = _functions[i]->subscriptionCallback();
						}

					} નહીંતર {
						PX4_ERR("function alloc failed");
					}
				}

				તોડો;
			}
		}
	}

	hrt_abstime fixed_rate_scheduling_interval = 4_ms; // schedule at 250Hz

	જો (_max_topic_update_interval_us > fixed_rate_scheduling_interval) {
		fixed_rate_scheduling_interval = _max_topic_update_interval_us;
	}

	જો (_scheduling_policy == SchedulingPolicy::Auto) {
		જો (_subscription_callback) {
			જો (_subscription_callback->registerCallback()) {
				PX4_DEBUG("Scheduling via callback");
				_has_backup_schedule = સાચું;
				_interface.ScheduleDelayed(50_ms);

			} નહીંતર {
				PX4_ERR("registerCallback failed, scheduling at fixed rate");
				_interface.ScheduleOnInterval(fixed_rate_scheduling_interval);
			}

		} નહીંતર જો (all_disabled) {
			_interface.ScheduleOnInterval(_lowrate_schedule_interval);
			PX4_DEBUG("Scheduling at low rate");

		} નહીંતર {
			_interface.ScheduleOnInterval(fixed_rate_scheduling_interval);
			PX4_DEBUG("Scheduling at fixed rate");
		}
	}

	setMaxTopicUpdateRate(_max_topic_update_interval_us);
	_need_function_update = ખોટું;

	_actuator_test.reset();

	unlock();

	_interface.mixerChanged();

	પાછા સાચું;
}

શૂન્ય MixingOutput::setMaxTopicUpdateRate(unsigned max_topic_update_interval_us)
{
	_max_topic_update_interval_us = max_topic_update_interval_us;

	જો (_subscription_callback) {
		_subscription_callback->set_interval_us(_max_topic_update_interval_us);
	}
}

શૂન્ય MixingOutput::set_All_Min_કિંમતો(uint16_t value)
{
	માટે (unsigned i = 0; i < MAX_ACTUATORS; i++) {
		_param_handles[i].min = PARAM_INVALID;
		min_કિંમત[i] = value;
	}
}

શૂન્ય MixingOutput::set_All_Center_કિંમતો(uint16_t value)
{
	માટે (unsigned i = 0; i < MAX_ACTUATORS; i++) {
		_param_handles[i].center = PARAM_INVALID;
		center_કિંમત[i] = value;
	}
}

શૂન્ય MixingOutput::set_All_Max_કિંમતો(uint16_t value)
{
	માટે (unsigned i = 0; i < MAX_ACTUATORS; i++) {
		_param_handles[i].max = PARAM_INVALID;
		max_કિંમત[i] = value;
	}
}

શૂન્ય MixingOutput::set_All_Failsafe_કિંમતો(uint16_t value)
{
	માટે (unsigned i = 0; i < MAX_ACTUATORS; i++) {
		_param_handles[i].failsafe = PARAM_INVALID;
		failsafe_કિંમત[i] = value;
	}
}

શૂન્ય MixingOutput::set_All_Disarmed_કિંમતો(uint16_t value)
{
	માટે (unsigned i = 0; i < MAX_ACTUATORS; i++) {
		_param_handles[i].disarmed = PARAM_INVALID;
		disarmed_કિંમત[i] = value;
	}
}

શૂન્ય MixingOutput::unregister()
{
	જો (_subscription_callback) {
		_subscription_callback->unregisterCallback();
	}
}

તાર્કિક MixingOutput::update()
{
	// check arming state
	જો (_armed_sub.update(&_armed)) {
		_armed.in_esc_calibration_mode &= _support_esc_calibration;

		જો (_ignore_lockdown) {
			_armed.lockdown = ખોટું;
		}

		/* Update the armed status and check that we're not locked down.
		 * We also need to arm throttle for the ESC calibration. */
		_throttle_armed = (_armed.armed && !_armed.lockdown) || _armed.in_esc_calibration_mode;
	}

	// only used for sitl with lockstep
	તાર્કિક has_updates = _subscription_callback && _subscription_callback->updated();

	// update topics
	માટે (પૂર્ણાંક i = 0; i < MAX_ACTUATORS && _function_allocated[i]; ++i) {
		_function_allocated[i]->update();
	}

	જો (_has_backup_schedule) {
		_interface.ScheduleDelayed(50_ms);
	}

	// check for actuator test
	_actuator_test.update(max_અંક_outputs_2, _param_thr_mdl_fac.get());

	// get output values
	દશાંશ outputs[MAX_ACTUATORS];
	તાર્કિક all_disabled = સાચું;
	અચલ uint32_t reversible_mask_prev = _reversible_mask;
	_reversible_mask = 0;

	માટે (પૂર્ણાંક i = 0; i < max_અંક_outputs_2; ++i) {
		જો (_functions[i]) {
			all_disabled = ખોટું;

			જો (_armed.armed || (_armed.prearmed && _functions[i]->allowPrearmControl())) {
				outputs[i] = _functions[i]->value(_function_assignment[i]);

			} નહીંતર {
				outputs[i] = NAN;
			}

			_reversible_mask |= (uint32_t)_functions[i]->reversible(_function_assignment[i]) << i;

		} નહીંતર {
			outputs[i] = NAN;
		}
	}

	જો (_reversible_mask != reversible_mask_prev) {
		_interface.reversibleMaskChanged(_reversible_mask);
	}

	// Send output if any function mapped or one last disabling sample
	જો (!all_disabled || !_was_all_disabled) {
		જો (!_armed.armed && !_armed.kill) {
			_actuator_test.override_કિંમતો(outputs, max_અંક_outputs_2);
		}

		limitAndUpdateOutputs(outputs, has_updates);
	}

	_was_all_disabled = all_disabled;

	પાછા સાચું;
}

શૂન્ય
MixingOutput::limitAndUpdateOutputs(દશાંશ outputs[MAX_ACTUATORS], તાર્કિક has_updates)
{
	જો (_armed.lockdown || _armed.kill) {
		// overwrite outputs in case of lockdown with disarmed values
		માટે (size_t i = 0; i < max_અંક_outputs_2; i++) {
			current_output_કિંમત[i] = disarmed_કિંમત[i];
		}

	} નહીંતર જો (_armed.termination) {
		// Overwrite outputs with _failsafe_value when terminated
		માટે (size_t i = 0; i < max_અંક_outputs_2; i++) {
			current_output_કિંમત[i] = actual_Failsafe_કિંમત(i);
		}

	} નહીંતર {
		// the output limit call takes care of out of band errors, NaN and constrains
		output_limit_calc(_throttle_armed || _actuator_test.inTestMode(), max_અંક_outputs_2, outputs);
	}

	// We must calibrate the PWM and Oneshot ESCs to a consistent range of 1000-2000us (gets mapped to 125-250us for Oneshot)
	// Doing so makes calibrations consistent among different configurations and hence PWM minimum and maximum have a consistent effect
	// hence the defaults for these parameters also make most setups work out of the box
	જો (_armed.in_esc_calibration_mode) {
		સ્થિર constexpr દશાંશ PWM_CALIBRATION_નીચલું = 1000.f;
		સ્થિર constexpr દશાંશ PWM_CALIBRATION_ઉપલું = 2000.f;

		માટે (પૂર્ણાંક i = 0; i < max_અંક_outputs_2; i++) {
			જો (fabsf(current_output_કિંમત[i] - (દશાંશ)min_કિંમત[i]) < 0.5f) {
				current_output_કિંમત[i] = PWM_CALIBRATION_નીચલું;
			}

			જો (fabsf(current_output_કિંમત[i] - (દશાંશ)max_કિંમત[i]) < 0.5f) {
				current_output_કિંમત[i] = PWM_CALIBRATION_ઉપલું;
			}
		}
	}

	/* now return the outputs to the driver */
	જો (_interface.updateOutputs(current_output_કિંમત, max_અંક_outputs_2, has_updates)) {
		actuator_outputs_s actuator_outputs{};
		setAndPublishActuatorOutputs(max_અંક_outputs_2, actuator_outputs);

		updateLatencyPerfCounter(actuator_outputs);
	}
}

દશાંશ MixingOutput::output_limit_calc_single(પૂર્ણાંક i, દશાંશ value) અચલ
{
	// check for invalid / disabled channels
	જો (!PX4_ISFINITE(value)) {
		પાછા disarmed_કિંમત[i];
	}

	જો (_reverse_output_mask & (1 << i)) {
		value = -1.f * value;
	}

	દશાંશ output = disarmed_કિંમત[i];

	જો (((_function_assignment[i] >= OutputFunction::Servo1
	      && _function_assignment[i] <= OutputFunction::ServoMax) || _function_assignment[i] == OutputFunction::Landing_Gear_Wheel
	     || (_function_assignment[i] >= OutputFunction::Gimbal_Roll
		 && _function_assignment[i] <= OutputFunction::Gimbal_Yaw))
	    && _param_handles[i].center != PARAM_INVALID
	    && center_કિંમત[i] >= min_કિંમત[i]
	    && center_કિંમત[i] <= max_કિંમત[i]) {
		output = math::interpolateNXY(value, {-1.f, 0.f, 1.f}, {(દશાંશ)min_કિંમત[i], (દશાંશ)center_કિંમત[i], (દશાંશ)max_કિંમત[i]});
	}

	// Everything except servos, or if center is not set
	નહીંતર {
		output = math::interpolate(value, -1.f, 1.f, static_cast<દશાંશ>(min_કિંમત[i]), static_cast<દશાંશ>(max_કિંમત[i]));
	}

	પાછા output;
}

શૂન્ય
MixingOutput::output_limit_calc(અચલ તાર્કિક armed, અચલ પૂર્ણાંક અંક_channels, અચલ દશાંશ output[MAX_ACTUATORS])
{
	// time to slowly ramp up the ESCs
	સ્થિર constexpr hrt_abstime RAMP_TIME_US = 500_ms;

	/* first evaluate state changes */
	પસંદગી (_output_state) {
	સ્થિતિ OutputLimitState::OFF:
		જો (armed) {
			જો (_output_ramp_up) {
				_output_state = OutputLimitState::RAMP;

			} નહીંતર {
				_output_state = OutputLimitState::ON;
			}

			// reset arming time, used for ramp timing
			_output_time_armed = hrt_absolute_time();
		}

		તોડો;

	સ્થિતિ OutputLimitState::RAMP:
		જો (!armed) {
			_output_state = OutputLimitState::OFF;

		} નહીંતર જો (hrt_elapsed_time(&_output_time_armed) >= RAMP_TIME_US) {
			_output_state = OutputLimitState::ON;
		}

		તોડો;

	સ્થિતિ OutputLimitState::ON:
		જો (!armed) {
			_output_state = OutputLimitState::OFF;
		}

		તોડો;
	}

	/* if the system is pre-armed, the limit state is temporarily on,
	 * as some outputs are valid and the non-valid outputs have been
	 * set to NaN. This is not stored in the state machine though,
	 * as the throttle channels need to go through the ramp at
	 * regular arming time.
	 */
	auto local_limit_state = _output_state;

	જો (isPrearmed()) {
		local_limit_state = OutputLimitState::ON;
	}

	// then set _current_output_value based on state
	પસંદગી (local_limit_state) {
	સ્થિતિ OutputLimitState::OFF:
		માટે (પૂર્ણાંક i = 0; i < અંક_channels; i++) {
			current_output_કિંમત[i] = disarmed_કિંમત[i];
		}

		તોડો;

	સ્થિતિ OutputLimitState::RAMP: {
			hrt_abstime diff = hrt_elapsed_time(&_output_time_armed);
			દશાંશ progress = static_cast<દશાંશ>(diff) / RAMP_TIME_US;

			જો (progress > 1.f) {
				progress = 1.f;
			}

			માટે (પૂર્ણાંક i = 0; i < અંક_channels; i++) {
				// Ramp from disarmed value to currently desired output that would apply without ramp
				દશાંશ desired_output = output_limit_calc_single(i, output[i]);
				current_output_કિંમત[i] = disarmed_કિંમત[i] + progress * (desired_output - disarmed_કિંમત[i]);
			}
		}
		તોડો;

	સ્થિતિ OutputLimitState::ON:
		માટે (પૂર્ણાંક i = 0; i < અંક_channels; i++) {
			current_output_કિંમત[i] = output_limit_calc_single(i, output[i]);
		}

		તોડો;
	}
}

શૂન્ય
MixingOutput::setAndPublishActuatorOutputs(unsigned અંક_outputs, actuator_outputs_s &actuator_outputs)
{
	actuator_outputs.noutputs = અંક_outputs;

	માટે (size_t i = 0; i < અંક_outputs; ++i) {
		actuator_outputs.output[i] = current_output_કિંમત[i];
	}

	actuator_outputs.timestamp = hrt_absolute_time();
	_outputs_pub.publish(actuator_outputs);
}

શૂન્ય
MixingOutput::updateLatencyPerfCounter(અચલ actuator_outputs_s &actuator_outputs)
{
	// Just check the first function. It means we only get the latency if motors are assigned first, which is the default
	જો (_function_allocated[0]) {
		hrt_abstime timestamp_sample;

		જો (_function_allocated[0]->getLatestSampleTimestamp(timestamp_sample)) {
			perf_set_elapsed(_control_latency_perf, actuator_outputs.timestamp - timestamp_sample);
		}
	}
}

દશાંશ MixingOutput::actual_Failsafe_કિંમત(પૂર્ણાંક index) અચલ
{
	દશાંશ value = 0;

	જો (failsafe_કિંમત[index] == UINT16_MAX) { // if set to default, use the one provided by the function
		દશાંશ default_failsafe = NAN;

		જો (_functions[index]) {
			default_failsafe = _functions[index]->default_Failsafe_કિંમત(_function_assignment[index]);
		}

		value = output_limit_calc_single(index, default_failsafe);

	} નહીંતર {
		value = static_cast<દશાંશ>(failsafe_કિંમત[index]);
	}

	પાછા value;
}
