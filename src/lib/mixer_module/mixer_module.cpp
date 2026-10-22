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

#అన్తర్భావ "mixer_module.hpp"

#అన్తర్భావ <uORB/Publication.hpp>
#అన్తర్భావ <px4_platform_common/log.h>

ఉపయోగించి నామాకాశ సమయం_literals;


నిర్మాణం FunctionProvider {
	ఉపయోగించి Constructor = Function_Provider_పునాది * (*)(స్థిరరాశి Function_Provider_పునాది::Context &context);
	constexpr FunctionProvider(OutputFunction min_func_, OutputFunction max_func_, Constructor constructor_)
		: min_func(min_func_), max_func(max_func_), constructor(constructor_) {}
	constexpr FunctionProvider(OutputFunction func, Constructor constructor_)
		: min_func(func), max_func(func), constructor(constructor_) {}

	OutputFunction min_func;
	OutputFunction max_func;
	Constructor constructor;
};

స్థిర constexpr FunctionProvider all_function_providers[] = {
	// Providers higher up take precedence for subscription callback in case there are multiple
	{OutputFunction::Constant_Min, &FunctionConstantMin::allocate},
	{OutputFunction::Constant_Max, &FunctionConstantMax::allocate},
	{OutputFunction::Motor1, OutputFunction::MotorMax, &FunctionMotors::allocate},
	{OutputFunction::Servo1, OutputFunction::ServoMax, &FunctionServos::allocate},
	{OutputFunction::Peripheral_ద్వారా_Actuator_Set1, OutputFunction::Peripheral_ద్వారా_Actuator_Set6, &Function_Actuator_సమితి::allocate},
	{OutputFunction::Landing_Gear, &FunctionLandingGear::allocate},
	{OutputFunction::Landing_Gear_Wheel, &FunctionLandingGearWheel::allocate},
	{OutputFunction::Parachute, &FunctionParachute::allocate},
	{OutputFunction::Gripper, &FunctionGripper::allocate},
	{OutputFunction::RC_Roll, OutputFunction::RC_AUXMax, &FunctionManualRC::allocate},
	{OutputFunction::Gimbal_Roll, OutputFunction::Gimbal_Yaw, &FunctionGimbal::allocate},
	{OutputFunction::IC_Engine_Ignition, OutputFunction::IC_Engine_Starter, &FunctionICEControl::allocate},
};

MixingOutput::MixingOutput(స్థిరరాశి అక్షరం *param_ఉపసర్గ, uint8_t max_అంకె_outputs, OutputModuleInterface &interface,
			   SchedulingPolicy scheduling_policy, తార్కిక support_esc_calibration, తార్కిక ramp_పైకి, స్థిరరాశి uint8_t instance_ప్రారంభం) :
	ModuleParams(&interface),
	output_ramp_పైకి(ramp_పైకి),
	_scheduling_policy(scheduling_policy),
	_support_esc_calibration(support_esc_calibration),
	max_అంకె_outputs_2(max_అంకె_outputs < MAX_ACTUATORS ? max_అంకె_outputs : MAX_ACTUATORS),
	_interface(interface),
	_control_latency_perf(perf_alloc(PC_ELAPSED, "control latency")),
	param_ఉపసర్గ_2(param_ఉపసర్గ)
{
	/* Safely initialize armed flags */
	_armed.armed = అబద్ధం;
	_armed.prearmed = అబద్ధం;
	_armed.ready_గమ్యం_arm = అబద్ధం;
	_armed.lockdown = అబద్ధం;
	_armed.termination = అబద్ధం;
	_armed.లోపల_esc_calibration_బహుళకం = అబద్ధం;

	px4_sem_init(&_lock, 0, 1);

	initParamHandles(instance_ప్రారంభం);

	కొరకు (గుర్తులేని i = 0; i < MAX_ACTUATORS; i++) {
		failsafe_విలువ[i] = UINT16_MAX;
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

శూన్య MixingOutput::initParamHandles(స్థిరరాశి uint8_t instance_ప్రారంభం)
{
	అక్షరం param_name[17];

	కొరకు (గుర్తులేని i = 0; i < max_అంకె_outputs_2; ++i) {
		snprintf(param_name, పరిమాణం(param_name), "%s_%s%d", param_ఉపసర్గ_2, "FUNC", i + instance_ప్రారంభం);
		_param_handles[i].function = param_కనుగొను(param_name);
		snprintf(param_name, పరిమాణం(param_name), "%s_%s%d", param_ఉపసర్గ_2, "DIS", i + instance_ప్రారంభం);
		_param_handles[i].disarmed = param_కనుగొను(param_name);
		snprintf(param_name, పరిమాణం(param_name), "%s_%s%d", param_ఉపసర్గ_2, "MIN", i + instance_ప్రారంభం);
		_param_handles[i].min = param_కనుగొను(param_name);
		snprintf(param_name, పరిమాణం(param_name), "%s_%s%d", param_ఉపసర్గ_2, "CENT", i + instance_ప్రారంభం);
		_param_handles[i].center = param_కనుగొను(param_name);
		snprintf(param_name, పరిమాణం(param_name), "%s_%s%d", param_ఉపసర్గ_2, "MAX", i + instance_ప్రారంభం);
		_param_handles[i].max = param_కనుగొను(param_name);
		snprintf(param_name, పరిమాణం(param_name), "%s_%s%d", param_ఉపసర్గ_2, "FAIL", i + instance_ప్రారంభం);
		_param_handles[i].failsafe = param_కనుగొను(param_name);
	}

	snprintf(param_name, పరిమాణం(param_name), "%s_%s", param_ఉపసర్గ_2, "REV");
	_param_handle_rev_range = param_కనుగొను(param_name);
}

శూన్య MixingOutput::print_స్థితి_2() స్థిరరాశి
{
	PX4_INFO("Param prefix: %s", param_ఉపసర్గ_2);
	perf_print_counter(_control_latency_perf);

	ఒకవేళ (_wq_switched) {
		PX4_INFO("Switched to rate_ctrl work queue");
	}

	PX4_INFO_RAW("Channel Configuration:\n");

	కొరకు (గుర్తులేని i = 0; i < max_అంకె_outputs_2; i++) {
		PX4_INFO_RAW("Channel %2d: func: %3d, value: %.2f, failsafe: %.2f, disarmed: %d, min: %d, max: %d, center: %d\n",
			     i, (పూర్ణాంకం)_function_assignment[i], (ద్విగుణ)current_output_విలువ[i], (ద్విగుణ)actual_Failsafe_విలువ(i),
			     disarmed_విలువ[i], min_విలువ[i], max_విలువ[i], center_విలువ[i]);
	}
}

శూన్య MixingOutput::updateParams()
{
	ModuleParams::updateParams();

	తార్కిక function_మారింది = అబద్ధం;

	కొరకు (గుర్తులేని i = 0; i < max_అంకె_outputs_2; i++) {
		int32_t val;

		ఒకవేళ (_param_handles[i].function != PARAM_INVALID && param_get(_param_handles[i].function, &val) == 0) {
			ఒకవేళ (val != (int32_t)_function_assignment[i]) {
				function_మారింది = నిజం;
			}

			// we set _function_assignment[i] later to ensure _functions[i] is updated at the same time
		}

		ఒకవేళ (_param_handles[i].disarmed != PARAM_INVALID && param_get(_param_handles[i].disarmed, &val) == 0) {
			disarmed_విలువ[i] = val;
		}

		ఒకవేళ (_param_handles[i].min != PARAM_INVALID && param_get(_param_handles[i].min, &val) == 0) {
			min_విలువ[i] = val;
		}

		ఒకవేళ (_param_handles[i].center != PARAM_INVALID && param_get(_param_handles[i].center, &val) == 0) {
			center_విలువ[i] = val;
		}

		ఒకవేళ (_param_handles[i].max != PARAM_INVALID && param_get(_param_handles[i].max, &val) == 0) {
			max_విలువ[i] = val;
		}

		ఒకవేళ (min_విలువ[i] > max_విలువ[i]) {
			uint16_t tmp = min_విలువ[i];
			min_విలువ[i] = max_విలువ[i];
			max_విలువ[i] = tmp;
		}


		ఒకవేళ (_param_handles[i].failsafe != PARAM_INVALID && param_get(_param_handles[i].failsafe, &val) == 0) {
			failsafe_విలువ[i] = val;
		}
	}

	తిరగేయి_output_ముసుగు = 0;
	int32_t rev_range_param;

	ఒకవేళ (_param_handle_rev_range != PARAM_INVALID && param_get(_param_handle_rev_range, &rev_range_param) == 0) {
		తిరగేయి_output_ముసుగు = rev_range_param;
	}

	ఒకవేళ (function_మారింది) {
		_need_function_update = నిజం;
	}
}

శూన్య MixingOutput::cleanupFunctions()
{
	ఒకవేళ (_subscription_callback) {
		_subscription_callback->unregisterCallback();
		_subscription_callback = శూన్యసూచిక;
	}

	కొరకు (పూర్ణాంకం i = 0; i < MAX_ACTUATORS; ++i) {
		తొలగించు _function_allocated[i];
		_function_allocated[i] = శూన్యసూచిక;
		_functions[i] = శూన్యసూచిక;
	}
}

తార్కిక MixingOutput::updateSubscriptions(తార్కిక allow_wq_switch)
{
	ఒకవేళ (!_need_function_update || _armed.armed) {
		తిరిగి అబద్ధం;
	}

	// must be locked to potentially change WorkQueue
	తాళం();

	has_backup_కాలపట్టిక = అబద్ధం;

	ఒకవేళ (_scheduling_policy == SchedulingPolicy::Auto) {
		// first clear everything
		unregister();
		_interface.కాలపట్టిక_Clear();

		తార్కిక switch_requested = అబద్ధం;

		// potentially switch work queue if we run motor outputs
		కొరకు (గుర్తులేని i = 0; i < max_అంకె_outputs_2; i++) {
			// read function directly from param, as _function_assignment[i] is updated later
			int32_t function;

			ఒకవేళ (_param_handles[i].function != PARAM_INVALID && param_get(_param_handles[i].function, &function) == 0) {
				ఒకవేళ (function >= (int32_t)OutputFunction::Motor1 && function <= (int32_t)OutputFunction::MotorMax) {
					switch_requested = నిజం;
				}
			}
		}

		ఒకవేళ (allow_wq_switch && !_wq_switched && switch_requested) {
			ఒకవేళ (_interface.Change_Work_వరుస(px4::wq_configurations::రేటు_ctrl)) {
				// let the new WQ handle the subscribe update
				_wq_switched = నిజం;
				_interface.కాలపట్టిక_Now();
				unlock();
				తిరిగి అబద్ధం;
			}
		}
	}

	// Now update the functions
	PX4_DEBUG("updating functions");

	cleanupFunctions();

	స్థిరరాశి Function_Provider_పునాది::Context context{_interface, _param_thr_mdl_fac.reference()};
	పూర్ణాంకం provider_indexes[MAX_ACTUATORS] {};
	పూర్ణాంకం తదుపరి_provider = 0;
	పూర్ణాంకం subscription_callback_provider_సూచీ = గరిష్ఠ_పూర్ణాంకం;
	తార్కిక all_disabled = నిజం;

	కొరకు (పూర్ణాంకం i = 0; i < max_అంకె_outputs_2; ++i) {
		int32_t val;

		ఒకవేళ (_param_handles[i].function != PARAM_INVALID && param_get(_param_handles[i].function, &val) == 0) {
			_function_assignment[i] = (OutputFunction)val;

		} లేకపోతే {
			_function_assignment[i] = OutputFunction::Disabled;
		}

		కొరకు (పూర్ణాంకం p = 0; p < (పూర్ణాంకం)(పరిమాణం(all_function_providers) / పరిమాణం(all_function_providers[0])); ++p) {
			ఒకవేళ (_function_assignment[i] >= all_function_providers[p].min_func &&
			    _function_assignment[i] <= all_function_providers[p].max_func) {
				all_disabled = అబద్ధం;
				పూర్ణాంకం దొరికింది_సూచీ = -1;

				కొరకు (పూర్ణాంకం existing = 0; existing < తదుపరి_provider; ++existing) {
					ఒకవేళ (provider_indexes[existing] == p) {
						దొరికింది_సూచీ = existing;
						ఆపు;
					}
				}

				ఒకవేళ (దొరికింది_సూచీ >= 0) {
					_functions[i] = _function_allocated[దొరికింది_సూచీ];

				} లేకపోతే {
					_function_allocated[తదుపరి_provider] = all_function_providers[p].constructor(context);

					ఒకవేళ (_function_allocated[తదుపరి_provider]) {
						_functions[i] = _function_allocated[తదుపరి_provider];
						provider_indexes[తదుపరి_provider++] = p;

						// lowest provider takes precedence for scheduling
						ఒకవేళ (p < subscription_callback_provider_సూచీ && _functions[i]->subscriptionCallback()) {
							subscription_callback_provider_సూచీ = p;
							_subscription_callback = _functions[i]->subscriptionCallback();
						}

					} లేకపోతే {
						PX4_ERR("function alloc failed");
					}
				}

				ఆపు;
			}
		}
	}

	hrt_abstime fixed_రేటు_scheduling_interval = 4_ms; // schedule at 250Hz

	ఒకవేళ (_max_topic_update_interval_us > fixed_రేటు_scheduling_interval) {
		fixed_రేటు_scheduling_interval = _max_topic_update_interval_us;
	}

	ఒకవేళ (_scheduling_policy == SchedulingPolicy::Auto) {
		ఒకవేళ (_subscription_callback) {
			ఒకవేళ (_subscription_callback->రిజిస్టరు_Callback()) {
				PX4_DEBUG("Scheduling via callback");
				has_backup_కాలపట్టిక = నిజం;
				_interface.కాలపట్టిక_Delayed(50_ms);

			} లేకపోతే {
				PX4_ERR("registerCallback failed, scheduling at fixed rate");
				_interface.కాలపట్టిక_On_Interval(fixed_రేటు_scheduling_interval);
			}

		} లేకపోతే ఒకవేళ (all_disabled) {
			_interface.కాలపట్టిక_On_Interval(lowrate_కాలపట్టిక_interval);
			PX4_DEBUG("Scheduling at low rate");

		} లేకపోతే {
			_interface.కాలపట్టిక_On_Interval(fixed_రేటు_scheduling_interval);
			PX4_DEBUG("Scheduling at fixed rate");
		}
	}

	సమితి_Max_Topic_Update_రేటు(_max_topic_update_interval_us);
	_need_function_update = అబద్ధం;

	_actuator_test.reset();

	unlock();

	_interface.mixer_మారింది();

	తిరిగి నిజం;
}

శూన్య MixingOutput::సమితి_Max_Topic_Update_రేటు(గుర్తులేని max_topic_update_interval_us)
{
	_max_topic_update_interval_us = max_topic_update_interval_us;

	ఒకవేళ (_subscription_callback) {
		_subscription_callback->సమితి_interval_us(_max_topic_update_interval_us);
	}
}

శూన్య MixingOutput::సమితి_All_Min_విలువలు(uint16_t value)
{
	కొరకు (గుర్తులేని i = 0; i < MAX_ACTUATORS; i++) {
		_param_handles[i].min = PARAM_INVALID;
		min_విలువ[i] = value;
	}
}

శూన్య MixingOutput::సమితి_All_Center_విలువలు(uint16_t value)
{
	కొరకు (గుర్తులేని i = 0; i < MAX_ACTUATORS; i++) {
		_param_handles[i].center = PARAM_INVALID;
		center_విలువ[i] = value;
	}
}

శూన్య MixingOutput::సమితి_All_Max_విలువలు(uint16_t value)
{
	కొరకు (గుర్తులేని i = 0; i < MAX_ACTUATORS; i++) {
		_param_handles[i].max = PARAM_INVALID;
		max_విలువ[i] = value;
	}
}

శూన్య MixingOutput::సమితి_All_Failsafe_విలువలు(uint16_t value)
{
	కొరకు (గుర్తులేని i = 0; i < MAX_ACTUATORS; i++) {
		_param_handles[i].failsafe = PARAM_INVALID;
		failsafe_విలువ[i] = value;
	}
}

శూన్య MixingOutput::సమితి_All_Disarmed_విలువలు(uint16_t value)
{
	కొరకు (గుర్తులేని i = 0; i < MAX_ACTUATORS; i++) {
		_param_handles[i].disarmed = PARAM_INVALID;
		disarmed_విలువ[i] = value;
	}
}

శూన్య MixingOutput::unregister()
{
	ఒకవేళ (_subscription_callback) {
		_subscription_callback->unregisterCallback();
	}
}

తార్కిక MixingOutput::update()
{
	// check arming state
	ఒకవేళ (_armed_sub.update(&_armed)) {
		_armed.లోపల_esc_calibration_బహుళకం &= _support_esc_calibration;

		ఒకవేళ (_ignore_lockdown) {
			_armed.lockdown = అబద్ధం;
		}

		/* Update the armed status and check that we're not locked down.
		 * We also need to arm throttle for the ESC calibration. */
		_throttle_armed = (_armed.armed && !_armed.lockdown) || _armed.లోపల_esc_calibration_బహుళకం;
	}

	// only used for sitl with lockstep
	తార్కిక has_updates = _subscription_callback && _subscription_callback->updated();

	// update topics
	కొరకు (పూర్ణాంకం i = 0; i < MAX_ACTUATORS && _function_allocated[i]; ++i) {
		_function_allocated[i]->update();
	}

	ఒకవేళ (has_backup_కాలపట్టిక) {
		_interface.కాలపట్టిక_Delayed(50_ms);
	}

	// check for actuator test
	_actuator_test.update(max_అంకె_outputs_2, _param_thr_mdl_fac.get());

	// get output values
	దశాంశం outputs[MAX_ACTUATORS];
	తార్కిక all_disabled = నిజం;
	స్థిరరాశి uint32_t reversible_ముసుగు_prev = reversible_ముసుగు;
	reversible_ముసుగు = 0;

	కొరకు (పూర్ణాంకం i = 0; i < max_అంకె_outputs_2; ++i) {
		ఒకవేళ (_functions[i]) {
			all_disabled = అబద్ధం;

			ఒకవేళ (_armed.armed || (_armed.prearmed && _functions[i]->allowPrearmControl())) {
				outputs[i] = _functions[i]->value(_function_assignment[i]);

			} లేకపోతే {
				outputs[i] = NAN;
			}

			reversible_ముసుగు |= (uint32_t)_functions[i]->reversible(_function_assignment[i]) << i;

		} లేకపోతే {
			outputs[i] = NAN;
		}
	}

	ఒకవేళ (reversible_ముసుగు != reversible_ముసుగు_prev) {
		_interface.reversible_ముసుగు_మారింది(reversible_ముసుగు);
	}

	// Send output if any function mapped or one last disabling sample
	ఒకవేళ (!all_disabled || !_was_all_disabled) {
		ఒకవేళ (!_armed.armed && !_armed.kill) {
			_actuator_test.override_విలువలు(outputs, max_అంకె_outputs_2);
		}

		హద్దు_And_Update_Outputs(outputs, has_updates);
	}

	_was_all_disabled = all_disabled;

	తిరిగి నిజం;
}

శూన్య
MixingOutput::హద్దు_And_Update_Outputs(దశాంశం outputs[MAX_ACTUATORS], తార్కిక has_updates)
{
	ఒకవేళ (_armed.lockdown || _armed.kill) {
		// overwrite outputs in case of lockdown with disarmed values
		కొరకు (size_t i = 0; i < max_అంకె_outputs_2; i++) {
			current_output_విలువ[i] = disarmed_విలువ[i];
		}

	} లేకపోతే ఒకవేళ (_armed.termination) {
		// Overwrite outputs with _failsafe_value when terminated
		కొరకు (size_t i = 0; i < max_అంకె_outputs_2; i++) {
			current_output_విలువ[i] = actual_Failsafe_విలువ(i);
		}

	} లేకపోతే {
		// the output limit call takes care of out of band errors, NaN and constrains
		output_హద్దు_calc(_throttle_armed || _actuator_test.లోపల_Test_బహుళకం(), max_అంకె_outputs_2, outputs);
	}

	// We must calibrate the PWM and Oneshot ESCs to a consistent range of 1000-2000us (gets mapped to 125-250us for Oneshot)
	// Doing so makes calibrations consistent among different configurations and hence PWM minimum and maximum have a consistent effect
	// hence the defaults for these parameters also make most setups work out of the box
	ఒకవేళ (_armed.లోపల_esc_calibration_బహుళకం) {
		స్థిర constexpr దశాంశం PWM_CALIBRATION_దిగువ = 1000.f;
		స్థిర constexpr దశాంశం PWM_CALIBRATION_ఎగువ = 2000.f;

		కొరకు (పూర్ణాంకం i = 0; i < max_అంకె_outputs_2; i++) {
			ఒకవేళ (fabsf(current_output_విలువ[i] - (దశాంశం)min_విలువ[i]) < 0.5f) {
				current_output_విలువ[i] = PWM_CALIBRATION_దిగువ;
			}

			ఒకవేళ (fabsf(current_output_విలువ[i] - (దశాంశం)max_విలువ[i]) < 0.5f) {
				current_output_విలువ[i] = PWM_CALIBRATION_ఎగువ;
			}
		}
	}

	/* now return the outputs to the driver */
	ఒకవేళ (_interface.updateOutputs(current_output_విలువ, max_అంకె_outputs_2, has_updates)) {
		actuator_outputs_s actuator_outputs{};
		సమితి_And_Publish_Actuator_Outputs(max_అంకె_outputs_2, actuator_outputs);

		updateLatencyPerfCounter(actuator_outputs);
	}
}

దశాంశం MixingOutput::output_హద్దు_calc_single(పూర్ణాంకం i, దశాంశం value) స్థిరరాశి
{
	// check for invalid / disabled channels
	ఒకవేళ (!PX4_ISFINITE(value)) {
		తిరిగి disarmed_విలువ[i];
	}

	ఒకవేళ (తిరగేయి_output_ముసుగు & (1 << i)) {
		value = -1.f * value;
	}

	దశాంశం output = disarmed_విలువ[i];

	ఒకవేళ (((_function_assignment[i] >= OutputFunction::Servo1
	      && _function_assignment[i] <= OutputFunction::ServoMax) || _function_assignment[i] == OutputFunction::Landing_Gear_Wheel
	     || (_function_assignment[i] >= OutputFunction::Gimbal_Roll
		 && _function_assignment[i] <= OutputFunction::Gimbal_Yaw))
	    && _param_handles[i].center != PARAM_INVALID
	    && center_విలువ[i] >= min_విలువ[i]
	    && center_విలువ[i] <= max_విలువ[i]) {
		output = math::interpolateNXY(value, {-1.f, 0.f, 1.f}, {(దశాంశం)min_విలువ[i], (దశాంశం)center_విలువ[i], (దశాంశం)max_విలువ[i]});
	}

	// Everything except servos, or if center is not set
	లేకపోతే {
		output = math::interpolate(value, -1.f, 1.f, static_cast<దశాంశం>(min_విలువ[i]), static_cast<దశాంశం>(max_విలువ[i]));
	}

	తిరిగి output;
}

శూన్య
MixingOutput::output_హద్దు_calc(స్థిరరాశి తార్కిక armed, స్థిరరాశి పూర్ణాంకం అంకె_channels, స్థిరరాశి దశాంశం output[MAX_ACTUATORS])
{
	// time to slowly ramp up the ESCs
	స్థిర constexpr hrt_abstime RAMP_సమయం_US = 500_ms;

	/* first evaluate state changes */
	ఎంపిక (_output_state) {
	స్థితి Output_హద్దు_State::OFF:
		ఒకవేళ (armed) {
			ఒకవేళ (output_ramp_పైకి) {
				_output_state = Output_హద్దు_State::RAMP;

			} లేకపోతే {
				_output_state = Output_హద్దు_State::ON;
			}

			// reset arming time, used for ramp timing
			output_సమయం_armed = hrt_absolute_సమయం();
		}

		ఆపు;

	స్థితి Output_హద్దు_State::RAMP:
		ఒకవేళ (!armed) {
			_output_state = Output_హద్దు_State::OFF;

		} లేకపోతే ఒకవేళ (hrt_elapsed_సమయం(&output_సమయం_armed) >= RAMP_సమయం_US) {
			_output_state = Output_హద్దు_State::ON;
		}

		ఆపు;

	స్థితి Output_హద్దు_State::ON:
		ఒకవేళ (!armed) {
			_output_state = Output_హద్దు_State::OFF;
		}

		ఆపు;
	}

	/* if the system is pre-armed, the limit state is temporarily on,
	 * as some outputs are valid and the non-valid outputs have been
	 * set to NaN. This is not stored in the state machine though,
	 * as the throttle channels need to go through the ramp at
	 * regular arming time.
	 */
	స్వయంచాలక local_హద్దు_state = _output_state;

	ఒకవేళ (isPrearmed()) {
		local_హద్దు_state = Output_హద్దు_State::ON;
	}

	// then set _current_output_value based on state
	ఎంపిక (local_హద్దు_state) {
	స్థితి Output_హద్దు_State::OFF:
		కొరకు (పూర్ణాంకం i = 0; i < అంకె_channels; i++) {
			current_output_విలువ[i] = disarmed_విలువ[i];
		}

		ఆపు;

	స్థితి Output_హద్దు_State::RAMP: {
			hrt_abstime diff = hrt_elapsed_సమయం(&output_సమయం_armed);
			దశాంశం పురోగతి = static_cast<దశాంశం>(diff) / RAMP_సమయం_US;

			ఒకవేళ (పురోగతి > 1.f) {
				పురోగతి = 1.f;
			}

			కొరకు (పూర్ణాంకం i = 0; i < అంకె_channels; i++) {
				// Ramp from disarmed value to currently desired output that would apply without ramp
				దశాంశం desired_output = output_హద్దు_calc_single(i, output[i]);
				current_output_విలువ[i] = disarmed_విలువ[i] + పురోగతి * (desired_output - disarmed_విలువ[i]);
			}
		}
		ఆపు;

	స్థితి Output_హద్దు_State::ON:
		కొరకు (పూర్ణాంకం i = 0; i < అంకె_channels; i++) {
			current_output_విలువ[i] = output_హద్దు_calc_single(i, output[i]);
		}

		ఆపు;
	}
}

శూన్య
MixingOutput::సమితి_And_Publish_Actuator_Outputs(గుర్తులేని అంకె_outputs, actuator_outputs_s &actuator_outputs)
{
	actuator_outputs.noutputs = అంకె_outputs;

	కొరకు (size_t i = 0; i < అంకె_outputs; ++i) {
		actuator_outputs.output[i] = current_output_విలువ[i];
	}

	actuator_outputs.timestamp = hrt_absolute_సమయం();
	_outputs_pub.publish(actuator_outputs);
}

శూన్య
MixingOutput::updateLatencyPerfCounter(స్థిరరాశి actuator_outputs_s &actuator_outputs)
{
	// Just check the first function. It means we only get the latency if motors are assigned first, which is the default
	ఒకవేళ (_function_allocated[0]) {
		hrt_abstime timestamp_sample;

		ఒకవేళ (_function_allocated[0]->getLatestSampleTimestamp(timestamp_sample)) {
			perf_సమితి_elapsed(_control_latency_perf, actuator_outputs.timestamp - timestamp_sample);
		}
	}
}

దశాంశం MixingOutput::actual_Failsafe_విలువ(పూర్ణాంకం సూచీ) స్థిరరాశి
{
	దశాంశం value = 0;

	ఒకవేళ (failsafe_విలువ[సూచీ] == UINT16_MAX) { // if set to default, use the one provided by the function
		దశాంశం default_failsafe = NAN;

		ఒకవేళ (_functions[సూచీ]) {
			default_failsafe = _functions[సూచీ]->default_Failsafe_విలువ(_function_assignment[సూచీ]);
		}

		value = output_హద్దు_calc_single(సూచీ, default_failsafe);

	} లేకపోతే {
		value = static_cast<దశాంశం>(failsafe_విలువ[సూచీ]);
	}

	తిరిగి value;
}
