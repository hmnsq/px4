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

তার্কিক Config::update()
{
	failure_injection_s cfg;

	যদি (_sub.update(&cfg)) {
		set(cfg);
		প্রত্যাগম সত্য;
	}

	প্রত্যাগম অসত্য;
}

শূন্য Config::set(ধ্রুব failure_injection_s &cfg)
{
	গুনতি_3 = (cfg.গুনতি <= failure_injection_s::MAX_FAILURES) ? cfg.গুনতি : failure_injection_s::MAX_FAILURES;

	প্রতি (uint8_t i = 0; i < গুনতি_3; i++) {
		_unit[i] = cfg.unit[i];
		_instance_mask[i] = cfg.instance_mask[i];
		_failure_type[i] = static_cast<Mode>(cfg.failure_type[i]);
	}
}

Mode Config::mode(uint8_t unit, uint8_t instance) ধ্রুব
{
	প্রতি (uint8_t i = 0; i < গুনতি_3; i++) {
		যদি (_unit[i] != unit) {
			অনুবর্ত;
		}

		// instance == 0 matches any instance of the unit; otherwise match the
		// 1-based instance against the bitmask (0xFFFF covers all instances).
		যদি (instance == 0 || (instance <= 16 && (_instance_mask[i] & (1u << (instance - 1))))) {
			প্রত্যাগম _failure_type[i];
		}
	}

	প্রত্যাগম Mode::Ok;
}

তার্কিক process_battery(ধ্রুব Config &config, uint8_t instance, battery_status_s &battery_status)
{
	ধ্রুব Mode mode = config.mode(failure_injection_s::FAILURE_UNIT_SYSTEM_BATTERY, instance);
	স্থির constexpr ভিন্ন trigger_margin = 0.01f; // report remaining charge just below the threshold

	যদি (mode == Mode::Off) {
		// Suppress the publication so the pack reads disconnected.
		প্রত্যাগম অসত্য;
	}

	যদি (mode != Mode::Wrong) {
		প্রত্যাগম সত্য;
	}

	স্থির ধ্রুব param_t level_handle = param_find("SYS_FAIL_BAT_LVL");
	স্থির ধ্রুব param_t নিচের_thr_handle = param_find("BAT_LOW_THR");
	স্থির ধ্রুব param_t crit_thr_handle = param_find("BAT_CRIT_THR");
	স্থির ধ্রুব param_t emergen_thr_handle = param_find("BAT_EMERGEN_THR");

	int32_t level = battery_status_s::WARNING_CRITICAL;
	param_get(level_handle, &level);

	param_t threshold_handle = crit_thr_handle;

	বিকল্প (level) {
	পক্ষ battery_status_s::WARNING_নিচের:
		threshold_handle = নিচের_thr_handle;
		ভঙ্গ;

	পক্ষ battery_status_s::WARNING_EMERGENCY:
		threshold_handle = emergen_thr_handle;
		ভঙ্গ;

	পক্ষ battery_status_s::WARNING_CRITICAL:
	স্বভাব:
		level = battery_status_s::WARNING_CRITICAL;
		threshold_handle = crit_thr_handle;
		ভঙ্গ;
	}

	battery_status.warning = level;


	// Report the remaining charge just below the selected threshold so the
	// matching stage of the low-battery failsafe triggers.
	ভিন্ন threshold = 0.f;
	param_get(threshold_handle, &threshold);
	battery_status.remaining = (threshold > trigger_margin) ? (threshold - trigger_margin) : 0.f;

	প্রত্যাগম সত্য;
}

তার্কিক process_gnss(ধ্রুব Config &config, uint8_t uorb_instance, sensor_gnss_s &sensor_gnss,
		  Stuck<sensor_gnss_s> &stuck)
{
	ধ্রুব Mode mode = config.mode(failure_injection_s::FAILURE_UNIT_SENSOR_GPS, uorb_instance + 1);

	// Off and Stuck are message-agnostic; run them first so the Stuck cache keeps the
	// uncorrupted sample and a later Stuck replays a healthy fix.
	যদি (!process(mode, sensor_gnss, stuck)) {
		প্রত্যাগম অসত্য;
	}

	যদি (mode == Mode::Wrong) {
		স্থির ধ্রুব param_t fix_type_handle = param_find("SYS_FAIL_GPS_WRG");

		int32_t fix_type = sensor_gnss_s::FIX_TYPE_2D;
		param_get(fix_type_handle, &fix_type);
		sensor_gnss.fix_type = (uint8_t)fix_type;

		স্থির ধ্রুব param_t jamming_state_handle = param_find("SYS_FAIL_GPS_JAM");

		int32_t jamming_state = sensor_gnss_s::JAMMING_STATE_UNKNOWN;
		param_get(jamming_state_handle, &jamming_state);

		যদি (jamming_state != sensor_gnss_s::JAMMING_STATE_UNKNOWN) {
			sensor_gnss.jamming_state = (uint8_t)jamming_state;
		}
	}

	প্রত্যাগম সত্য;
}

esc_status_s process_esc(ধ্রুব Config &config, ধ্রুব esc_status_s &status)
{
	esc_status_s result = status;

	প্রতি (পূর্ণাঙ্ক i = 0; i < result.esc_গুনতি && i < esc_status_s::CONNECTED_ESC_MAX; i++) {
		ধ্রুব uint8_t function = result.esc[i].actuator_function;

		যদি (function < esc_report_s::ACTUATOR_FUNCTION_MOTOR1 || function > esc_report_s::ACTUATOR_FUNCTION_MOTOR_MAX) {
			অনুবর্ত; // not a motor output
		}

		ধ্রুব uint8_t instance = function - esc_report_s::ACTUATOR_FUNCTION_MOTOR1 + 1; // 1-based ESC instance

		বিকল্প (config.mode(failure_injection_s::FAILURE_UNIT_SYSTEM_ESC, instance)) {
		পক্ষ Mode::Off:
			result.esc_online_flags &= ~(1u << i);
			result.esc_armed_flags &= ~(1u << i);
			result.esc[i] = esc_report_s{};
			result.esc[i].actuator_function = function;
			ভঙ্গ;

		পক্ষ Mode::Wrong:
			result.esc[i].esc_voltage *= 0.1f;
			result.esc[i].esc_current *= 0.1f;
			result.esc[i].esc_rpm *= 10;
			ভঙ্গ;

		স্বভাব:
			ভঙ্গ;
		}
	}

	প্রত্যাগম result;
}

MotorFailureMasks process_motor(ধ্রুব Config &config)
{
	MotorFailureMasks masks{};

	প্রতি (পূর্ণাঙ্ক i = 0; i < esc_status_s::CONNECTED_ESC_MAX; i++) {
		ধ্রুব uint16_t bit = 1u << i;

		বিকল্প (config.mode(failure_injection_s::FAILURE_UNIT_SYSTEM_MOTOR, i + 1)) {
		পক্ষ Mode::Off:
			masks.failure_mask |= bit;
			ভঙ্গ;

		পক্ষ Mode::Wrong:
			masks.stop_mask |= bit;
			ভঙ্গ;

		স্বভাব:
			ভঙ্গ;
		}
	}

	প্রত্যাগম masks;
}

} // namespace failure_injection

#endif // CONFIG_MODULES_FAILURE_INJECTION_MANAGER
