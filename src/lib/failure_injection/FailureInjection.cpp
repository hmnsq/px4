/****************************************************************************
 *
 *   Copyright (c) 2026 PX4 Development Team. All rights reserved.
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

#అన్తర్భావ "FailureInjection.hpp"

#ఒకవేళ defined(CONFIG_MODULES_FAILURE_INJECTION_MANAGER)

#అన్తర్భావ <parameters/param.h>
#అన్తర్భావ <uORB/topics/battery_status.h>
#అన్తర్భావ <uORB/topics/sensor_gnss.h>

నామాకాశ failure_injection
{

తార్కిక Config::update()
{
	failure_injection_s cfg;

	ఒకవేళ (_sub.update(&cfg)) {
		సమితి(cfg);
		తిరిగి నిజం;
	}

	తిరిగి అబద్ధం;
}

శూన్య Config::సమితి(స్థిరరాశి failure_injection_s &cfg)
{
	లెక్క_3 = (cfg.లెక్క <= failure_injection_s::MAX_FAILURES) ? cfg.లెక్క : failure_injection_s::MAX_FAILURES;

	కొరకు (uint8_t i = 0; i < లెక్క_3; i++) {
		_unit[i] = cfg.unit[i];
		instance_ముసుగు[i] = cfg.instance_ముసుగు_2[i];
		_failure_type[i] = static_cast<బహుళకం_2>(cfg.failure_type[i]);
	}
}

బహుళకం_2 Config::బహుళకం(uint8_t unit, uint8_t instance) స్థిరరాశి
{
	కొరకు (uint8_t i = 0; i < లెక్క_3; i++) {
		ఒకవేళ (_unit[i] != unit) {
			కొనసాగు;
		}

		// instance == 0 matches any instance of the unit; otherwise match the
		// 1-based instance against the bitmask (0xFFFF covers all instances).
		ఒకవేళ (instance == 0 || (instance <= 16 && (instance_ముసుగు[i] & (1u << (instance - 1))))) {
			తిరిగి _failure_type[i];
		}
	}

	తిరిగి బహుళకం_2::Ok;
}

తార్కిక process_battery(స్థిరరాశి Config &config, uint8_t instance, battery_స్థితి_s &battery_స్థితి)
{
	స్థిరరాశి బహుళకం_2 బహుళకం = config.బహుళకం(failure_injection_s::FAILURE_UNIT_SYSTEM_BATTERY, instance);
	స్థిర constexpr దశాంశం trigger_margin = 0.01f; // report remaining charge just below the threshold

	ఒకవేళ (బహుళకం == బహుళకం_2::Off) {
		// Suppress the publication so the pack reads disconnected.
		తిరిగి అబద్ధం;
	}

	ఒకవేళ (బహుళకం != బహుళకం_2::Wrong) {
		తిరిగి నిజం;
	}

	స్థిర స్థిరరాశి param_t level_handle = param_కనుగొను("SYS_FAIL_BAT_LVL");
	స్థిర స్థిరరాశి param_t దిగువ_thr_handle = param_కనుగొను("BAT_LOW_THR");
	స్థిర స్థిరరాశి param_t crit_thr_handle = param_కనుగొను("BAT_CRIT_THR");
	స్థిర స్థిరరాశి param_t emergen_thr_handle = param_కనుగొను("BAT_EMERGEN_THR");

	int32_t level = battery_స్థితి_s::WARNING_CRITICAL;
	param_get(level_handle, &level);

	param_t పరిమితి_handle = crit_thr_handle;

	ఎంపిక (level) {
	స్థితి battery_స్థితి_s::WARNING_దిగువ:
		పరిమితి_handle = దిగువ_thr_handle;
		ఆపు;

	స్థితి battery_స్థితి_s::WARNING_EMERGENCY:
		పరిమితి_handle = emergen_thr_handle;
		ఆపు;

	స్థితి battery_స్థితి_s::WARNING_CRITICAL:
	అప్రమేయ:
		level = battery_స్థితి_s::WARNING_CRITICAL;
		పరిమితి_handle = crit_thr_handle;
		ఆపు;
	}

	battery_స్థితి.warning = level;


	// Report the remaining charge just below the selected threshold so the
	// matching stage of the low-battery failsafe triggers.
	దశాంశం పరిమితి = 0.f;
	param_get(పరిమితి_handle, &పరిమితి);
	battery_స్థితి.మిగిలిన = (పరిమితి > trigger_margin) ? (పరిమితి - trigger_margin) : 0.f;

	తిరిగి నిజం;
}

తార్కిక process_gnss(స్థిరరాశి Config &config, uint8_t uorb_instance, sensor_gnss_s &sensor_gnss,
		  Stuck<sensor_gnss_s> &stuck)
{
	స్థిరరాశి బహుళకం_2 బహుళకం = config.బహుళకం(failure_injection_s::FAILURE_UNIT_SENSOR_GPS, uorb_instance + 1);

	// Off and Stuck are message-agnostic; run them first so the Stuck cache keeps the
	// uncorrupted sample and a later Stuck replays a healthy fix.
	ఒకవేళ (!process(బహుళకం, sensor_gnss, stuck)) {
		తిరిగి అబద్ధం;
	}

	ఒకవేళ (బహుళకం == బహుళకం_2::Wrong) {
		స్థిర స్థిరరాశి param_t fix_type_handle = param_కనుగొను("SYS_FAIL_GPS_WRG");

		int32_t fix_type = sensor_gnss_s::FIX_TYPE_2D;
		param_get(fix_type_handle, &fix_type);
		sensor_gnss.fix_type = (uint8_t)fix_type;

		స్థిర స్థిరరాశి param_t jamming_state_handle = param_కనుగొను("SYS_FAIL_GPS_JAM");

		int32_t jamming_state = sensor_gnss_s::JAMMING_STATE_UNKNOWN;
		param_get(jamming_state_handle, &jamming_state);

		ఒకవేళ (jamming_state != sensor_gnss_s::JAMMING_STATE_UNKNOWN) {
			sensor_gnss.jamming_state = (uint8_t)jamming_state;
		}
	}

	తిరిగి నిజం;
}

esc_స్థితి_s process_esc(స్థిరరాశి Config &config, స్థిరరాశి esc_స్థితి_s &స్థితి_2)
{
	esc_స్థితి_s ఫలితం = స్థితి_2;

	కొరకు (పూర్ణాంకం i = 0; i < ఫలితం.esc_లెక్క && i < esc_స్థితి_s::CONNECTED_ESC_MAX; i++) {
		స్థిరరాశి uint8_t function = ఫలితం.esc[i].actuator_function;

		ఒకవేళ (function < esc_report_s::ACTUATOR_FUNCTION_MOTOR1 || function > esc_report_s::ACTUATOR_FUNCTION_MOTOR_MAX) {
			కొనసాగు; // not a motor output
		}

		స్థిరరాశి uint8_t instance = function - esc_report_s::ACTUATOR_FUNCTION_MOTOR1 + 1; // 1-based ESC instance

		ఎంపిక (config.బహుళకం(failure_injection_s::FAILURE_UNIT_SYSTEM_ESC, instance)) {
		స్థితి బహుళకం_2::Off:
			ఫలితం.esc_online_flags &= ~(1u << i);
			ఫలితం.esc_armed_flags &= ~(1u << i);
			ఫలితం.esc[i] = esc_report_s{};
			ఫలితం.esc[i].actuator_function = function;
			ఆపు;

		స్థితి బహుళకం_2::Wrong:
			ఫలితం.esc[i].esc_voltage *= 0.1f;
			ఫలితం.esc[i].esc_current *= 0.1f;
			ఫలితం.esc[i].esc_rpm *= 10;
			ఆపు;

		అప్రమేయ:
			ఆపు;
		}
	}

	తిరిగి ఫలితం;
}

MotorFailureMasks process_motor(స్థిరరాశి Config &config)
{
	MotorFailureMasks masks{};

	కొరకు (పూర్ణాంకం i = 0; i < esc_స్థితి_s::CONNECTED_ESC_MAX; i++) {
		స్థిరరాశి uint16_t bit = 1u << i;

		ఎంపిక (config.బహుళకం(failure_injection_s::FAILURE_UNIT_SYSTEM_MOTOR, i + 1)) {
		స్థితి బహుళకం_2::Off:
			masks.failure_ముసుగు |= bit;
			ఆపు;

		స్థితి బహుళకం_2::Wrong:
			masks.stop_ముసుగు |= bit;
			ఆపు;

		అప్రమేయ:
			ఆపు;
		}
	}

	తిరిగి masks;
}

} // namespace failure_injection

#అయితే_ముగింపు // CONFIG_MODULES_FAILURE_INJECTION_MANAGER
