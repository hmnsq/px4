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

#ਅਨ੍ਤਰ੍ਭਾਵ "mixer_module.hpp"

#ਅਨ੍ਤਰ੍ਭਾਵ <uORB/Publication.hpp>
#ਅਨ੍ਤਰ੍ਭਾਵ <px4_platform_common/log.h>

ਵਰਤੋਂ ਨਾਮਾਕਾਸ਼ ਸਮਾਂ_literals;


ਬਣਤਰ FunctionProvider {
	ਵਰਤੋਂ Constructor = Function_Provider_ਅਧਾਰ * (*)(ਅਚਲ Function_Provider_ਅਧਾਰ::Context &context);
	constexpr FunctionProvider(OutputFunction min_func_, OutputFunction max_func_, Constructor constructor_)
		: min_func(min_func_), max_func(max_func_), constructor(constructor_) {}
	constexpr FunctionProvider(OutputFunction func, Constructor constructor_)
		: min_func(func), max_func(func), constructor(constructor_) {}

	OutputFunction min_func;
	OutputFunction max_func;
	Constructor constructor;
};

ਸਥਿਰ constexpr FunctionProvider all_function_providers[] = {
	// Providers higher up take precedence for subscription callback in case there are multiple
	{OutputFunction::Constant_Min, &FunctionConstantMin::allocate},
	{OutputFunction::Constant_Max, &FunctionConstantMax::allocate},
	{OutputFunction::Motor1, OutputFunction::MotorMax, &FunctionMotors::allocate},
	{OutputFunction::Servo1, OutputFunction::ServoMax, &FunctionServos::allocate},
	{OutputFunction::Peripheral_ਰਾਹੀਂ_Actuator_Set1, OutputFunction::Peripheral_ਰਾਹੀਂ_Actuator_Set6, &Function_Actuator_ਸੈੱਟ::allocate},
	{OutputFunction::Landing_Gear, &FunctionLandingGear::allocate},
	{OutputFunction::Landing_Gear_Wheel, &FunctionLandingGearWheel::allocate},
	{OutputFunction::Parachute, &FunctionParachute::allocate},
	{OutputFunction::Gripper, &FunctionGripper::allocate},
	{OutputFunction::RC_Roll, OutputFunction::RC_AUXMax, &FunctionManualRC::allocate},
	{OutputFunction::Gimbal_Roll, OutputFunction::Gimbal_Yaw, &FunctionGimbal::allocate},
	{OutputFunction::IC_Engine_Ignition, OutputFunction::IC_Engine_Starter, &FunctionICEControl::allocate},
};

MixingOutput::MixingOutput(ਅਚਲ ਅੱਖਰ *param_ਅਗੇਤਰ, uint8_t max_ਅੰਕ_outputs, OutputModuleInterface &interface,
			   SchedulingPolicy scheduling_policy, ਤਾਰਕਿਕ support_esc_calibration, ਤਾਰਕਿਕ ramp_ਉੱਪਰ, ਅਚਲ uint8_t instance_ਸ਼ੁਰੂ) :
	ModuleParams(&interface),
	output_ramp_ਉੱਪਰ(ramp_ਉੱਪਰ),
	_scheduling_policy(scheduling_policy),
	_support_esc_calibration(support_esc_calibration),
	max_ਅੰਕ_outputs_2(max_ਅੰਕ_outputs < MAX_ACTUATORS ? max_ਅੰਕ_outputs : MAX_ACTUATORS),
	_interface(interface),
	_control_latency_perf(perf_alloc(PC_ELAPSED, "control latency")),
	param_ਅਗੇਤਰ_2(param_ਅਗੇਤਰ)
{
	/* Safely initialize armed flags */
	_armed.armed = ਝੂਠ;
	_armed.prearmed = ਝੂਠ;
	_armed.ready_ਮੰਜ਼ਿਲ_arm = ਝੂਠ;
	_armed.lockdown = ਝੂਠ;
	_armed.termination = ਝੂਠ;
	_armed.ਵਿੱਚ_esc_calibration_ਬਹੁਲਕ = ਝੂਠ;

	px4_sem_init(&_lock, 0, 1);

	initParamHandles(instance_ਸ਼ੁਰੂ);

	ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ i = 0; i < MAX_ACTUATORS; i++) {
		failsafe_ਮੁੱਲ[i] = UINT16_MAX;
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

ਖਾਲੀ MixingOutput::initParamHandles(ਅਚਲ uint8_t instance_ਸ਼ੁਰੂ)
{
	ਅੱਖਰ param_name[17];

	ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ i = 0; i < max_ਅੰਕ_outputs_2; ++i) {
		snprintf(param_name, ਆਕਾਰ(param_name), "%s_%s%d", param_ਅਗੇਤਰ_2, "FUNC", i + instance_ਸ਼ੁਰੂ);
		_param_handles[i].function = param_ਲੱਭੋ(param_name);
		snprintf(param_name, ਆਕਾਰ(param_name), "%s_%s%d", param_ਅਗੇਤਰ_2, "DIS", i + instance_ਸ਼ੁਰੂ);
		_param_handles[i].disarmed = param_ਲੱਭੋ(param_name);
		snprintf(param_name, ਆਕਾਰ(param_name), "%s_%s%d", param_ਅਗੇਤਰ_2, "MIN", i + instance_ਸ਼ੁਰੂ);
		_param_handles[i].min = param_ਲੱਭੋ(param_name);
		snprintf(param_name, ਆਕਾਰ(param_name), "%s_%s%d", param_ਅਗੇਤਰ_2, "CENT", i + instance_ਸ਼ੁਰੂ);
		_param_handles[i].center = param_ਲੱਭੋ(param_name);
		snprintf(param_name, ਆਕਾਰ(param_name), "%s_%s%d", param_ਅਗੇਤਰ_2, "MAX", i + instance_ਸ਼ੁਰੂ);
		_param_handles[i].max = param_ਲੱਭੋ(param_name);
		snprintf(param_name, ਆਕਾਰ(param_name), "%s_%s%d", param_ਅਗੇਤਰ_2, "FAIL", i + instance_ਸ਼ੁਰੂ);
		_param_handles[i].failsafe = param_ਲੱਭੋ(param_name);
	}

	snprintf(param_name, ਆਕਾਰ(param_name), "%s_%s", param_ਅਗੇਤਰ_2, "REV");
	_param_handle_rev_range = param_ਲੱਭੋ(param_name);
}

ਖਾਲੀ MixingOutput::print_ਸਥਿਤੀ_2() ਅਚਲ
{
	PX4_INFO("Param prefix: %s", param_ਅਗੇਤਰ_2);
	perf_print_counter(_control_latency_perf);

	ਜੇ (_wq_switched) {
		PX4_INFO("Switched to rate_ctrl work queue");
	}

	PX4_INFO_RAW("Channel Configuration:\n");

	ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ i = 0; i < max_ਅੰਕ_outputs_2; i++) {
		PX4_INFO_RAW("Channel %2d: func: %3d, value: %.2f, failsafe: %.2f, disarmed: %d, min: %d, max: %d, center: %d\n",
			     i, (ਪੂਰਨ)_function_assignment[i], (ਦੁੱਗਣਾ)current_output_ਮੁੱਲ[i], (ਦੁੱਗਣਾ)actual_Failsafe_ਮੁੱਲ(i),
			     disarmed_ਮੁੱਲ[i], min_ਮੁੱਲ[i], max_ਮੁੱਲ[i], center_ਮੁੱਲ[i]);
	}
}

ਖਾਲੀ MixingOutput::updateParams()
{
	ModuleParams::updateParams();

	ਤਾਰਕਿਕ function_ਬਦਲਿਆ = ਝੂਠ;

	ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ i = 0; i < max_ਅੰਕ_outputs_2; i++) {
		int32_t val;

		ਜੇ (_param_handles[i].function != PARAM_INVALID && param_get(_param_handles[i].function, &val) == 0) {
			ਜੇ (val != (int32_t)_function_assignment[i]) {
				function_ਬਦਲਿਆ = ਸੱਚ;
			}

			// we set _function_assignment[i] later to ensure _functions[i] is updated at the same time
		}

		ਜੇ (_param_handles[i].disarmed != PARAM_INVALID && param_get(_param_handles[i].disarmed, &val) == 0) {
			disarmed_ਮੁੱਲ[i] = val;
		}

		ਜੇ (_param_handles[i].min != PARAM_INVALID && param_get(_param_handles[i].min, &val) == 0) {
			min_ਮੁੱਲ[i] = val;
		}

		ਜੇ (_param_handles[i].center != PARAM_INVALID && param_get(_param_handles[i].center, &val) == 0) {
			center_ਮੁੱਲ[i] = val;
		}

		ਜੇ (_param_handles[i].max != PARAM_INVALID && param_get(_param_handles[i].max, &val) == 0) {
			max_ਮੁੱਲ[i] = val;
		}

		ਜੇ (min_ਮੁੱਲ[i] > max_ਮੁੱਲ[i]) {
			uint16_t tmp = min_ਮੁੱਲ[i];
			min_ਮੁੱਲ[i] = max_ਮੁੱਲ[i];
			max_ਮੁੱਲ[i] = tmp;
		}


		ਜੇ (_param_handles[i].failsafe != PARAM_INVALID && param_get(_param_handles[i].failsafe, &val) == 0) {
			failsafe_ਮੁੱਲ[i] = val;
		}
	}

	ਉਲਟ_output_ਨਕਾਬ = 0;
	int32_t rev_range_param;

	ਜੇ (_param_handle_rev_range != PARAM_INVALID && param_get(_param_handle_rev_range, &rev_range_param) == 0) {
		ਉਲਟ_output_ਨਕਾਬ = rev_range_param;
	}

	ਜੇ (function_ਬਦਲਿਆ) {
		_need_function_update = ਸੱਚ;
	}
}

ਖਾਲੀ MixingOutput::cleanupFunctions()
{
	ਜੇ (_subscription_callback) {
		_subscription_callback->unregisterCallback();
		_subscription_callback = ਖਾਲੀ_ਸੂਚਕ;
	}

	ਲਈ (ਪੂਰਨ i = 0; i < MAX_ACTUATORS; ++i) {
		ਮਿਟਾਓ _function_allocated[i];
		_function_allocated[i] = ਖਾਲੀ_ਸੂਚਕ;
		_functions[i] = ਖਾਲੀ_ਸੂਚਕ;
	}
}

ਤਾਰਕਿਕ MixingOutput::updateSubscriptions(ਤਾਰਕਿਕ allow_wq_switch)
{
	ਜੇ (!_need_function_update || _armed.armed) {
		ਵਾਪਸ ਝੂਠ;
	}

	// must be locked to potentially change WorkQueue
	ਤਾਲਾ();

	has_backup_ਸਮਾਂ_ਸਾਰਣੀ = ਝੂਠ;

	ਜੇ (_scheduling_policy == SchedulingPolicy::Auto) {
		// first clear everything
		unregister();
		_interface.ਸਮਾਂ_ਸਾਰਣੀ_Clear();

		ਤਾਰਕਿਕ switch_requested = ਝੂਠ;

		// potentially switch work queue if we run motor outputs
		ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ i = 0; i < max_ਅੰਕ_outputs_2; i++) {
			// read function directly from param, as _function_assignment[i] is updated later
			int32_t function;

			ਜੇ (_param_handles[i].function != PARAM_INVALID && param_get(_param_handles[i].function, &function) == 0) {
				ਜੇ (function >= (int32_t)OutputFunction::Motor1 && function <= (int32_t)OutputFunction::MotorMax) {
					switch_requested = ਸੱਚ;
				}
			}
		}

		ਜੇ (allow_wq_switch && !_wq_switched && switch_requested) {
			ਜੇ (_interface.Change_Work_ਕਤਾਰ(px4::wq_configurations::ਦਰ_ctrl)) {
				// let the new WQ handle the subscribe update
				_wq_switched = ਸੱਚ;
				_interface.ਸਮਾਂ_ਸਾਰਣੀ_Now();
				unlock();
				ਵਾਪਸ ਝੂਠ;
			}
		}
	}

	// Now update the functions
	PX4_DEBUG("updating functions");

	cleanupFunctions();

	ਅਚਲ Function_Provider_ਅਧਾਰ::Context context{_interface, _param_thr_mdl_fac.reference()};
	ਪੂਰਨ provider_indexes[MAX_ACTUATORS] {};
	ਪੂਰਨ ਅਗਲਾ_provider = 0;
	ਪੂਰਨ subscription_callback_provider_ਸੂਚਕਾਂਕ = ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ;
	ਤਾਰਕਿਕ all_disabled = ਸੱਚ;

	ਲਈ (ਪੂਰਨ i = 0; i < max_ਅੰਕ_outputs_2; ++i) {
		int32_t val;

		ਜੇ (_param_handles[i].function != PARAM_INVALID && param_get(_param_handles[i].function, &val) == 0) {
			_function_assignment[i] = (OutputFunction)val;

		} ਨਹੀਂਤਾਂ {
			_function_assignment[i] = OutputFunction::Disabled;
		}

		ਲਈ (ਪੂਰਨ p = 0; p < (ਪੂਰਨ)(ਆਕਾਰ(all_function_providers) / ਆਕਾਰ(all_function_providers[0])); ++p) {
			ਜੇ (_function_assignment[i] >= all_function_providers[p].min_func &&
			    _function_assignment[i] <= all_function_providers[p].max_func) {
				all_disabled = ਝੂਠ;
				ਪੂਰਨ ਮਿਲਿਆ_ਸੂਚਕਾਂਕ = -1;

				ਲਈ (ਪੂਰਨ existing = 0; existing < ਅਗਲਾ_provider; ++existing) {
					ਜੇ (provider_indexes[existing] == p) {
						ਮਿਲਿਆ_ਸੂਚਕਾਂਕ = existing;
						ਰੋਕੋ;
					}
				}

				ਜੇ (ਮਿਲਿਆ_ਸੂਚਕਾਂਕ >= 0) {
					_functions[i] = _function_allocated[ਮਿਲਿਆ_ਸੂਚਕਾਂਕ];

				} ਨਹੀਂਤਾਂ {
					_function_allocated[ਅਗਲਾ_provider] = all_function_providers[p].constructor(context);

					ਜੇ (_function_allocated[ਅਗਲਾ_provider]) {
						_functions[i] = _function_allocated[ਅਗਲਾ_provider];
						provider_indexes[ਅਗਲਾ_provider++] = p;

						// lowest provider takes precedence for scheduling
						ਜੇ (p < subscription_callback_provider_ਸੂਚਕਾਂਕ && _functions[i]->subscriptionCallback()) {
							subscription_callback_provider_ਸੂਚਕਾਂਕ = p;
							_subscription_callback = _functions[i]->subscriptionCallback();
						}

					} ਨਹੀਂਤਾਂ {
						PX4_ERR("function alloc failed");
					}
				}

				ਰੋਕੋ;
			}
		}
	}

	hrt_abstime fixed_ਦਰ_scheduling_interval = 4_ms; // schedule at 250Hz

	ਜੇ (_max_topic_update_interval_us > fixed_ਦਰ_scheduling_interval) {
		fixed_ਦਰ_scheduling_interval = _max_topic_update_interval_us;
	}

	ਜੇ (_scheduling_policy == SchedulingPolicy::Auto) {
		ਜੇ (_subscription_callback) {
			ਜੇ (_subscription_callback->ਰਜਿਸਟਰ_Callback()) {
				PX4_DEBUG("Scheduling via callback");
				has_backup_ਸਮਾਂ_ਸਾਰਣੀ = ਸੱਚ;
				_interface.ਸਮਾਂ_ਸਾਰਣੀ_Delayed(50_ms);

			} ਨਹੀਂਤਾਂ {
				PX4_ERR("registerCallback failed, scheduling at fixed rate");
				_interface.ਸਮਾਂ_ਸਾਰਣੀ_On_Interval(fixed_ਦਰ_scheduling_interval);
			}

		} ਨਹੀਂਤਾਂ ਜੇ (all_disabled) {
			_interface.ਸਮਾਂ_ਸਾਰਣੀ_On_Interval(lowrate_ਸਮਾਂ_ਸਾਰਣੀ_interval);
			PX4_DEBUG("Scheduling at low rate");

		} ਨਹੀਂਤਾਂ {
			_interface.ਸਮਾਂ_ਸਾਰਣੀ_On_Interval(fixed_ਦਰ_scheduling_interval);
			PX4_DEBUG("Scheduling at fixed rate");
		}
	}

	ਸੈੱਟ_Max_Topic_Update_ਦਰ(_max_topic_update_interval_us);
	_need_function_update = ਝੂਠ;

	_actuator_test.reset();

	unlock();

	_interface.mixer_ਬਦਲਿਆ();

	ਵਾਪਸ ਸੱਚ;
}

ਖਾਲੀ MixingOutput::ਸੈੱਟ_Max_Topic_Update_ਦਰ(ਬਿਨਾਨਿਸ਼ਾਨੀ max_topic_update_interval_us)
{
	_max_topic_update_interval_us = max_topic_update_interval_us;

	ਜੇ (_subscription_callback) {
		_subscription_callback->ਸੈੱਟ_interval_us(_max_topic_update_interval_us);
	}
}

ਖਾਲੀ MixingOutput::ਸੈੱਟ_All_Min_ਮੁੱਲਾਂ(uint16_t value)
{
	ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ i = 0; i < MAX_ACTUATORS; i++) {
		_param_handles[i].min = PARAM_INVALID;
		min_ਮੁੱਲ[i] = value;
	}
}

ਖਾਲੀ MixingOutput::ਸੈੱਟ_All_Center_ਮੁੱਲਾਂ(uint16_t value)
{
	ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ i = 0; i < MAX_ACTUATORS; i++) {
		_param_handles[i].center = PARAM_INVALID;
		center_ਮੁੱਲ[i] = value;
	}
}

ਖਾਲੀ MixingOutput::ਸੈੱਟ_All_Max_ਮੁੱਲਾਂ(uint16_t value)
{
	ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ i = 0; i < MAX_ACTUATORS; i++) {
		_param_handles[i].max = PARAM_INVALID;
		max_ਮੁੱਲ[i] = value;
	}
}

ਖਾਲੀ MixingOutput::ਸੈੱਟ_All_Failsafe_ਮੁੱਲਾਂ(uint16_t value)
{
	ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ i = 0; i < MAX_ACTUATORS; i++) {
		_param_handles[i].failsafe = PARAM_INVALID;
		failsafe_ਮੁੱਲ[i] = value;
	}
}

ਖਾਲੀ MixingOutput::ਸੈੱਟ_All_Disarmed_ਮੁੱਲਾਂ(uint16_t value)
{
	ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ i = 0; i < MAX_ACTUATORS; i++) {
		_param_handles[i].disarmed = PARAM_INVALID;
		disarmed_ਮੁੱਲ[i] = value;
	}
}

ਖਾਲੀ MixingOutput::unregister()
{
	ਜੇ (_subscription_callback) {
		_subscription_callback->unregisterCallback();
	}
}

ਤਾਰਕਿਕ MixingOutput::update()
{
	// check arming state
	ਜੇ (_armed_sub.update(&_armed)) {
		_armed.ਵਿੱਚ_esc_calibration_ਬਹੁਲਕ &= _support_esc_calibration;

		ਜੇ (_ignore_lockdown) {
			_armed.lockdown = ਝੂਠ;
		}

		/* Update the armed status and check that we're not locked down.
		 * We also need to arm throttle for the ESC calibration. */
		_throttle_armed = (_armed.armed && !_armed.lockdown) || _armed.ਵਿੱਚ_esc_calibration_ਬਹੁਲਕ;
	}

	// only used for sitl with lockstep
	ਤਾਰਕਿਕ has_updates = _subscription_callback && _subscription_callback->updated();

	// update topics
	ਲਈ (ਪੂਰਨ i = 0; i < MAX_ACTUATORS && _function_allocated[i]; ++i) {
		_function_allocated[i]->update();
	}

	ਜੇ (has_backup_ਸਮਾਂ_ਸਾਰਣੀ) {
		_interface.ਸਮਾਂ_ਸਾਰਣੀ_Delayed(50_ms);
	}

	// check for actuator test
	_actuator_test.update(max_ਅੰਕ_outputs_2, _param_thr_mdl_fac.get());

	// get output values
	ਦਸ਼ਮਲਵ outputs[MAX_ACTUATORS];
	ਤਾਰਕਿਕ all_disabled = ਸੱਚ;
	ਅਚਲ uint32_t reversible_ਨਕਾਬ_prev = reversible_ਨਕਾਬ;
	reversible_ਨਕਾਬ = 0;

	ਲਈ (ਪੂਰਨ i = 0; i < max_ਅੰਕ_outputs_2; ++i) {
		ਜੇ (_functions[i]) {
			all_disabled = ਝੂਠ;

			ਜੇ (_armed.armed || (_armed.prearmed && _functions[i]->allowPrearmControl())) {
				outputs[i] = _functions[i]->value(_function_assignment[i]);

			} ਨਹੀਂਤਾਂ {
				outputs[i] = NAN;
			}

			reversible_ਨਕਾਬ |= (uint32_t)_functions[i]->reversible(_function_assignment[i]) << i;

		} ਨਹੀਂਤਾਂ {
			outputs[i] = NAN;
		}
	}

	ਜੇ (reversible_ਨਕਾਬ != reversible_ਨਕਾਬ_prev) {
		_interface.reversible_ਨਕਾਬ_ਬਦਲਿਆ(reversible_ਨਕਾਬ);
	}

	// Send output if any function mapped or one last disabling sample
	ਜੇ (!all_disabled || !_was_all_disabled) {
		ਜੇ (!_armed.armed && !_armed.kill) {
			_actuator_test.override_ਮੁੱਲਾਂ(outputs, max_ਅੰਕ_outputs_2);
		}

		ਹੱਦਬੰਦੀ_And_Update_Outputs(outputs, has_updates);
	}

	_was_all_disabled = all_disabled;

	ਵਾਪਸ ਸੱਚ;
}

ਖਾਲੀ
MixingOutput::ਹੱਦਬੰਦੀ_And_Update_Outputs(ਦਸ਼ਮਲਵ outputs[MAX_ACTUATORS], ਤਾਰਕਿਕ has_updates)
{
	ਜੇ (_armed.lockdown || _armed.kill) {
		// overwrite outputs in case of lockdown with disarmed values
		ਲਈ (size_t i = 0; i < max_ਅੰਕ_outputs_2; i++) {
			current_output_ਮੁੱਲ[i] = disarmed_ਮੁੱਲ[i];
		}

	} ਨਹੀਂਤਾਂ ਜੇ (_armed.termination) {
		// Overwrite outputs with _failsafe_value when terminated
		ਲਈ (size_t i = 0; i < max_ਅੰਕ_outputs_2; i++) {
			current_output_ਮੁੱਲ[i] = actual_Failsafe_ਮੁੱਲ(i);
		}

	} ਨਹੀਂਤਾਂ {
		// the output limit call takes care of out of band errors, NaN and constrains
		output_ਹੱਦਬੰਦੀ_calc(_throttle_armed || _actuator_test.ਵਿੱਚ_Test_ਬਹੁਲਕ(), max_ਅੰਕ_outputs_2, outputs);
	}

	// We must calibrate the PWM and Oneshot ESCs to a consistent range of 1000-2000us (gets mapped to 125-250us for Oneshot)
	// Doing so makes calibrations consistent among different configurations and hence PWM minimum and maximum have a consistent effect
	// hence the defaults for these parameters also make most setups work out of the box
	ਜੇ (_armed.ਵਿੱਚ_esc_calibration_ਬਹੁਲਕ) {
		ਸਥਿਰ constexpr ਦਸ਼ਮਲਵ PWM_CALIBRATION_ਨੀਵਾਂ = 1000.f;
		ਸਥਿਰ constexpr ਦਸ਼ਮਲਵ PWM_CALIBRATION_ਉੱਚਾ = 2000.f;

		ਲਈ (ਪੂਰਨ i = 0; i < max_ਅੰਕ_outputs_2; i++) {
			ਜੇ (fabsf(current_output_ਮੁੱਲ[i] - (ਦਸ਼ਮਲਵ)min_ਮੁੱਲ[i]) < 0.5f) {
				current_output_ਮੁੱਲ[i] = PWM_CALIBRATION_ਨੀਵਾਂ;
			}

			ਜੇ (fabsf(current_output_ਮੁੱਲ[i] - (ਦਸ਼ਮਲਵ)max_ਮੁੱਲ[i]) < 0.5f) {
				current_output_ਮੁੱਲ[i] = PWM_CALIBRATION_ਉੱਚਾ;
			}
		}
	}

	/* now return the outputs to the driver */
	ਜੇ (_interface.updateOutputs(current_output_ਮੁੱਲ, max_ਅੰਕ_outputs_2, has_updates)) {
		actuator_outputs_s actuator_outputs{};
		ਸੈੱਟ_And_Publish_Actuator_Outputs(max_ਅੰਕ_outputs_2, actuator_outputs);

		updateLatencyPerfCounter(actuator_outputs);
	}
}

ਦਸ਼ਮਲਵ MixingOutput::output_ਹੱਦਬੰਦੀ_calc_single(ਪੂਰਨ i, ਦਸ਼ਮਲਵ value) ਅਚਲ
{
	// check for invalid / disabled channels
	ਜੇ (!PX4_ISFINITE(value)) {
		ਵਾਪਸ disarmed_ਮੁੱਲ[i];
	}

	ਜੇ (ਉਲਟ_output_ਨਕਾਬ & (1 << i)) {
		value = -1.f * value;
	}

	ਦਸ਼ਮਲਵ output = disarmed_ਮੁੱਲ[i];

	ਜੇ (((_function_assignment[i] >= OutputFunction::Servo1
	      && _function_assignment[i] <= OutputFunction::ServoMax) || _function_assignment[i] == OutputFunction::Landing_Gear_Wheel
	     || (_function_assignment[i] >= OutputFunction::Gimbal_Roll
		 && _function_assignment[i] <= OutputFunction::Gimbal_Yaw))
	    && _param_handles[i].center != PARAM_INVALID
	    && center_ਮੁੱਲ[i] >= min_ਮੁੱਲ[i]
	    && center_ਮੁੱਲ[i] <= max_ਮੁੱਲ[i]) {
		output = math::interpolateNXY(value, {-1.f, 0.f, 1.f}, {(ਦਸ਼ਮਲਵ)min_ਮੁੱਲ[i], (ਦਸ਼ਮਲਵ)center_ਮੁੱਲ[i], (ਦਸ਼ਮਲਵ)max_ਮੁੱਲ[i]});
	}

	// Everything except servos, or if center is not set
	ਨਹੀਂਤਾਂ {
		output = math::interpolate(value, -1.f, 1.f, static_cast<ਦਸ਼ਮਲਵ>(min_ਮੁੱਲ[i]), static_cast<ਦਸ਼ਮਲਵ>(max_ਮੁੱਲ[i]));
	}

	ਵਾਪਸ output;
}

ਖਾਲੀ
MixingOutput::output_ਹੱਦਬੰਦੀ_calc(ਅਚਲ ਤਾਰਕਿਕ armed, ਅਚਲ ਪੂਰਨ ਅੰਕ_channels, ਅਚਲ ਦਸ਼ਮਲਵ output[MAX_ACTUATORS])
{
	// time to slowly ramp up the ESCs
	ਸਥਿਰ constexpr hrt_abstime RAMP_ਸਮਾਂ_US = 500_ms;

	/* first evaluate state changes */
	ਚੋਣ (_output_state) {
	ਹਾਲਤ Output_ਹੱਦਬੰਦੀ_State::OFF:
		ਜੇ (armed) {
			ਜੇ (output_ramp_ਉੱਪਰ) {
				_output_state = Output_ਹੱਦਬੰਦੀ_State::RAMP;

			} ਨਹੀਂਤਾਂ {
				_output_state = Output_ਹੱਦਬੰਦੀ_State::ON;
			}

			// reset arming time, used for ramp timing
			output_ਸਮਾਂ_armed = hrt_absolute_ਸਮਾਂ();
		}

		ਰੋਕੋ;

	ਹਾਲਤ Output_ਹੱਦਬੰਦੀ_State::RAMP:
		ਜੇ (!armed) {
			_output_state = Output_ਹੱਦਬੰਦੀ_State::OFF;

		} ਨਹੀਂਤਾਂ ਜੇ (hrt_elapsed_ਸਮਾਂ(&output_ਸਮਾਂ_armed) >= RAMP_ਸਮਾਂ_US) {
			_output_state = Output_ਹੱਦਬੰਦੀ_State::ON;
		}

		ਰੋਕੋ;

	ਹਾਲਤ Output_ਹੱਦਬੰਦੀ_State::ON:
		ਜੇ (!armed) {
			_output_state = Output_ਹੱਦਬੰਦੀ_State::OFF;
		}

		ਰੋਕੋ;
	}

	/* if the system is pre-armed, the limit state is temporarily on,
	 * as some outputs are valid and the non-valid outputs have been
	 * set to NaN. This is not stored in the state machine though,
	 * as the throttle channels need to go through the ramp at
	 * regular arming time.
	 */
	ਸਵੈਚਾਲਿਤ local_ਹੱਦਬੰਦੀ_state = _output_state;

	ਜੇ (isPrearmed()) {
		local_ਹੱਦਬੰਦੀ_state = Output_ਹੱਦਬੰਦੀ_State::ON;
	}

	// then set _current_output_value based on state
	ਚੋਣ (local_ਹੱਦਬੰਦੀ_state) {
	ਹਾਲਤ Output_ਹੱਦਬੰਦੀ_State::OFF:
		ਲਈ (ਪੂਰਨ i = 0; i < ਅੰਕ_channels; i++) {
			current_output_ਮੁੱਲ[i] = disarmed_ਮੁੱਲ[i];
		}

		ਰੋਕੋ;

	ਹਾਲਤ Output_ਹੱਦਬੰਦੀ_State::RAMP: {
			hrt_abstime diff = hrt_elapsed_ਸਮਾਂ(&output_ਸਮਾਂ_armed);
			ਦਸ਼ਮਲਵ ਤਰੱਕੀ = static_cast<ਦਸ਼ਮਲਵ>(diff) / RAMP_ਸਮਾਂ_US;

			ਜੇ (ਤਰੱਕੀ > 1.f) {
				ਤਰੱਕੀ = 1.f;
			}

			ਲਈ (ਪੂਰਨ i = 0; i < ਅੰਕ_channels; i++) {
				// Ramp from disarmed value to currently desired output that would apply without ramp
				ਦਸ਼ਮਲਵ desired_output = output_ਹੱਦਬੰਦੀ_calc_single(i, output[i]);
				current_output_ਮੁੱਲ[i] = disarmed_ਮੁੱਲ[i] + ਤਰੱਕੀ * (desired_output - disarmed_ਮੁੱਲ[i]);
			}
		}
		ਰੋਕੋ;

	ਹਾਲਤ Output_ਹੱਦਬੰਦੀ_State::ON:
		ਲਈ (ਪੂਰਨ i = 0; i < ਅੰਕ_channels; i++) {
			current_output_ਮੁੱਲ[i] = output_ਹੱਦਬੰਦੀ_calc_single(i, output[i]);
		}

		ਰੋਕੋ;
	}
}

ਖਾਲੀ
MixingOutput::ਸੈੱਟ_And_Publish_Actuator_Outputs(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੰਕ_outputs, actuator_outputs_s &actuator_outputs)
{
	actuator_outputs.noutputs = ਅੰਕ_outputs;

	ਲਈ (size_t i = 0; i < ਅੰਕ_outputs; ++i) {
		actuator_outputs.output[i] = current_output_ਮੁੱਲ[i];
	}

	actuator_outputs.timestamp = hrt_absolute_ਸਮਾਂ();
	_outputs_pub.publish(actuator_outputs);
}

ਖਾਲੀ
MixingOutput::updateLatencyPerfCounter(ਅਚਲ actuator_outputs_s &actuator_outputs)
{
	// Just check the first function. It means we only get the latency if motors are assigned first, which is the default
	ਜੇ (_function_allocated[0]) {
		hrt_abstime timestamp_sample;

		ਜੇ (_function_allocated[0]->getLatestSampleTimestamp(timestamp_sample)) {
			perf_ਸੈੱਟ_elapsed(_control_latency_perf, actuator_outputs.timestamp - timestamp_sample);
		}
	}
}

ਦਸ਼ਮਲਵ MixingOutput::actual_Failsafe_ਮੁੱਲ(ਪੂਰਨ ਸੂਚਕਾਂਕ) ਅਚਲ
{
	ਦਸ਼ਮਲਵ value = 0;

	ਜੇ (failsafe_ਮੁੱਲ[ਸੂਚਕਾਂਕ] == UINT16_MAX) { // if set to default, use the one provided by the function
		ਦਸ਼ਮਲਵ default_failsafe = NAN;

		ਜੇ (_functions[ਸੂਚਕਾਂਕ]) {
			default_failsafe = _functions[ਸੂਚਕਾਂਕ]->default_Failsafe_ਮੁੱਲ(_function_assignment[ਸੂਚਕਾਂਕ]);
		}

		value = output_ਹੱਦਬੰਦੀ_calc_single(ਸੂਚਕਾਂਕ, default_failsafe);

	} ਨਹੀਂਤਾਂ {
		value = static_cast<ਦਸ਼ਮਲਵ>(failsafe_ਮੁੱਲ[ਸੂਚਕਾਂਕ]);
	}

	ਵਾਪਸ value;
}
