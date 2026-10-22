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

#অন্তর্ভাব "FailureInjection.hpp"

#যদি defined(CONFIG_MODULES_FAILURE_INJECTION_MANAGER)

#অন্তর্ভাব <parameters/param.h>
#অন্তর্ভাব <uORB/topics/battery_status.h>
#অন্তর্ভাব <uORB/topics/sensor_gnss.h>

নামাকাশ failure_injection
{

যৌক্তিক Config::update()
{
	failure_injection_s cfg;

	যদি (_sub.update(&cfg)) {
		set(cfg);
		ফেরত সত্য;
	}

	ফেরত মিথ্যা;
}

শূন্য Config::set(ধ্রুবক failure_injection_s &cfg)
{
	গুনতি_3 = (cfg.গুনতি <= failure_injection_s::MAX_FAILURES) ? cfg.গুনতি : failure_injection_s::MAX_FAILURES;

	জন্য (uint8_t i = 0; i < গুনতি_3; i++) {
		_unit[i] = cfg.unit[i];
		_instance_mask[i] = cfg.instance_mask[i];
		_failure_type[i] = static_cast<Mode>(cfg.failure_type[i]);
	}
}

Mode Config::mode(uint8_t unit, uint8_t instance) ধ্রুবক
{
	জন্য (uint8_t i = 0; i < গুনতি_3; i++) {
		যদি (_unit[i] != unit) {
			চালু;
		}

		// instance == 0 matches any instance of the unit; otherwise match the
		// 1-based instance against the bitmask (0xFFFF covers all instances).
		যদি (instance == 0 || (instance <= 16 && (_instance_mask[i] & (1u << (instance - 1))))) {
			ফেরত _failure_type[i];
		}
	}

	ফেরত Mode::Ok;
}

যৌক্তিক process_battery(ধ্রুবক Config &config, uint8_t instance, battery_status_s &battery_status)
{
	ধ্রুবক Mode mode = config.mode(failure_injection_s::FAILURE_UNIT_SYSTEM_BATTERY, instance);
	স্থির constexpr দশমিক trigger_margin = 0.01f; // report remaining charge just below the threshold

	যদি (mode == Mode::Off) {
		// Suppress the publication so the pack reads disconnected.
		ফেরত মিথ্যা;
	}

	যদি (mode != Mode::Wrong) {
		ফেরত সত্য;
	}

	স্থির ধ্রুবক param_t level_handle = param_find("SYS_FAIL_BAT_LVL");
	স্থির ধ্রুবক param_t নিচের_thr_handle = param_find("BAT_LOW_THR");
	স্থির ধ্রুবক param_t crit_thr_handle = param_find("BAT_CRIT_THR");
	স্থির ধ্রুবক param_t emergen_thr_handle = param_find("BAT_EMERGEN_THR");

	int32_t level = battery_status_s::WARNING_CRITICAL;
	param_get(level_handle, &level);

	param_t threshold_handle = crit_thr_handle;

	নির্বাচন (level) {
	ক্ষেত্র battery_status_s::WARNING_নিচের:
		threshold_handle = নিচের_thr_handle;
		বিরতি;

	ক্ষেত্র battery_status_s::WARNING_EMERGENCY:
		threshold_handle = emergen_thr_handle;
		বিরতি;

	ক্ষেত্র battery_status_s::WARNING_CRITICAL:
	পূর্বনির্ধারিত:
		level = battery_status_s::WARNING_CRITICAL;
		threshold_handle = crit_thr_handle;
		বিরতি;
	}

	battery_status.warning = level;


	// Report the remaining charge just below the selected threshold so the
	// matching stage of the low-battery failsafe triggers.
	দশমিক threshold = 0.f;
	param_get(threshold_handle, &threshold);
	battery_status.remaining = (threshold > trigger_margin) ? (threshold - trigger_margin) : 0.f;

	ফেরত সত্য;
}

যৌক্তিক process_gnss(ধ্রুবক Config &config, uint8_t uorb_instance, sensor_gnss_s &sensor_gnss,
		  Stuck<sensor_gnss_s> &stuck)
{
	ধ্রুবক Mode mode = config.mode(failure_injection_s::FAILURE_UNIT_SENSOR_GPS, uorb_instance + 1);

	// Off and Stuck are message-agnostic; run them first so the Stuck cache keeps the
	// uncorrupted sample and a later Stuck replays a healthy fix.
	যদি (!process(mode, sensor_gnss, stuck)) {
		ফেরত মিথ্যা;
	}

	যদি (mode == Mode::Wrong) {
		স্থির ধ্রুবক param_t fix_type_handle = param_find("SYS_FAIL_GPS_WRG");

		int32_t fix_type = sensor_gnss_s::FIX_TYPE_2D;
		param_get(fix_type_handle, &fix_type);
		sensor_gnss.fix_type = (uint8_t)fix_type;

		স্থির ধ্রুবক param_t jamming_state_handle = param_find("SYS_FAIL_GPS_JAM");

		int32_t jamming_state = sensor_gnss_s::JAMMING_STATE_UNKNOWN;
		param_get(jamming_state_handle, &jamming_state);

		যদি (jamming_state != sensor_gnss_s::JAMMING_STATE_UNKNOWN) {
			sensor_gnss.jamming_state = (uint8_t)jamming_state;
		}
	}

	ফেরত সত্য;
}

esc_status_s process_esc(ধ্রুবক Config &config, ধ্রুবক esc_status_s &status)
{
	esc_status_s ফলাফল = status;

	জন্য (পূর্ণসংখ্যা i = 0; i < ফলাফল.esc_গুনতি && i < esc_status_s::CONNECTED_ESC_MAX; i++) {
		ধ্রুবক uint8_t function = ফলাফল.esc[i].actuator_function;

		যদি (function < esc_report_s::ACTUATOR_FUNCTION_MOTOR1 || function > esc_report_s::ACTUATOR_FUNCTION_MOTOR_MAX) {
			চালু; // not a motor output
		}

		ধ্রুবক uint8_t instance = function - esc_report_s::ACTUATOR_FUNCTION_MOTOR1 + 1; // 1-based ESC instance

		নির্বাচন (config.mode(failure_injection_s::FAILURE_UNIT_SYSTEM_ESC, instance)) {
		ক্ষেত্র Mode::Off:
			ফলাফল.esc_online_flags &= ~(1u << i);
			ফলাফল.esc_armed_flags &= ~(1u << i);
			ফলাফল.esc[i] = esc_report_s{};
			ফলাফল.esc[i].actuator_function = function;
			বিরতি;

		ক্ষেত্র Mode::Wrong:
			ফলাফল.esc[i].esc_voltage *= 0.1f;
			ফলাফল.esc[i].esc_current *= 0.1f;
			ফলাফল.esc[i].esc_rpm *= 10;
			বিরতি;

		পূর্বনির্ধারিত:
			বিরতি;
		}
	}

	ফেরত ফলাফল;
}

MotorFailureMasks process_motor(ধ্রুবক Config &config)
{
	MotorFailureMasks masks{};

	জন্য (পূর্ণসংখ্যা i = 0; i < esc_status_s::CONNECTED_ESC_MAX; i++) {
		ধ্রুবক uint16_t bit = 1u << i;

		নির্বাচন (config.mode(failure_injection_s::FAILURE_UNIT_SYSTEM_MOTOR, i + 1)) {
		ক্ষেত্র Mode::Off:
			masks.failure_mask |= bit;
			বিরতি;

		ক্ষেত্র Mode::Wrong:
			masks.stop_mask |= bit;
			বিরতি;

		পূর্বনির্ধারিত:
			বিরতি;
		}
	}

	ফেরত masks;
}

} // namespace failure_injection

#endif // CONFIG_MODULES_FAILURE_INJECTION_MANAGER
