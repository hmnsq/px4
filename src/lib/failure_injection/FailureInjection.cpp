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

#ضمن "FailureInjection.hpp"

#إذا defined(CONFIG_MODULES_FAILURE_INJECTION_MANAGER)

#ضمن <parameters/param.h>
#ضمن <uORB/topics/battery_status.h>
#ضمن <uORB/topics/sensor_gnss.h>

نطاق failure_injection
{

منطقي Config::update()
{
	failure_injection_s cfg;

	إذا (_sub.update(&cfg)) {
		set(cfg);
		أرجع true;
	}

	أرجع خطأ;
}

فراغ Config::set(ثابت failure_injection_s &cfg)
{
	گنتی_3 = (cfg.گنتی <= failure_injection_s::MAX_FAILURES) ? cfg.گنتی : failure_injection_s::MAX_FAILURES;

	لكل (uint8_t i = 0; i < گنتی_3; i++) {
		_unit[i] = cfg.unit[i];
		_instance_mask[i] = cfg.instance_mask[i];
		_failure_type[i] = static_cast<Mode>(cfg.failure_type[i]);
	}
}

Mode Config::mode(uint8_t unit, uint8_t instance) ثابت
{
	لكل (uint8_t i = 0; i < گنتی_3; i++) {
		إذا (_unit[i] != unit) {
			استمر;
		}

		// instance == 0 matches any instance of the unit; otherwise match the
		// 1-based instance against the bitmask (0xFFFF covers all instances).
		إذا (instance == 0 || (instance <= 16 && (_instance_mask[i] & (1u << (instance - 1))))) {
			أرجع _failure_type[i];
		}
	}

	أرجع Mode::Ok;
}

منطقي process_battery(ثابت Config &config, uint8_t instance, battery_status_s &battery_status)
{
	ثابت Mode mode = config.mode(failure_injection_s::FAILURE_UNIT_SYSTEM_BATTERY, instance);
	ساكن constexpr عائم trigger_margin = 0.01f; // report remaining charge just below the threshold

	إذا (mode == Mode::Off) {
		// Suppress the publication so the pack reads disconnected.
		أرجع خطأ;
	}

	إذا (mode != Mode::Wrong) {
		أرجع true;
	}

	ساكن ثابت param_t level_handle = param_find("SYS_FAIL_BAT_LVL");
	ساكن ثابت param_t کم_thr_handle = param_find("BAT_LOW_THR");
	ساكن ثابت param_t crit_thr_handle = param_find("BAT_CRIT_THR");
	ساكن ثابت param_t emergen_thr_handle = param_find("BAT_EMERGEN_THR");

	int32_t level = battery_status_s::WARNING_CRITICAL;
	param_get(level_handle, &level);

	param_t threshold_handle = crit_thr_handle;

	بدل (level) {
	حالة battery_status_s::WARNING_کم:
		threshold_handle = کم_thr_handle;
		اكسر;

	حالة battery_status_s::WARNING_EMERGENCY:
		threshold_handle = emergen_thr_handle;
		اكسر;

	حالة battery_status_s::WARNING_CRITICAL:
	افتراضي:
		level = battery_status_s::WARNING_CRITICAL;
		threshold_handle = crit_thr_handle;
		اكسر;
	}

	battery_status.warning = level;


	// Report the remaining charge just below the selected threshold so the
	// matching stage of the low-battery failsafe triggers.
	عائم threshold = 0.f;
	param_get(threshold_handle, &threshold);
	battery_status.remaining = (threshold > trigger_margin) ? (threshold - trigger_margin) : 0.f;

	أرجع true;
}

منطقي process_gnss(ثابت Config &config, uint8_t uorb_instance, sensor_gnss_s &sensor_gnss,
		  Stuck<sensor_gnss_s> &stuck)
{
	ثابت Mode mode = config.mode(failure_injection_s::FAILURE_UNIT_SENSOR_GPS, uorb_instance + 1);

	// Off and Stuck are message-agnostic; run them first so the Stuck cache keeps the
	// uncorrupted sample and a later Stuck replays a healthy fix.
	إذا (!process(mode, sensor_gnss, stuck)) {
		أرجع خطأ;
	}

	إذا (mode == Mode::Wrong) {
		ساكن ثابت param_t fix_type_handle = param_find("SYS_FAIL_GPS_WRG");

		int32_t fix_type = sensor_gnss_s::FIX_TYPE_2D;
		param_get(fix_type_handle, &fix_type);
		sensor_gnss.fix_type = (uint8_t)fix_type;

		ساكن ثابت param_t jamming_state_handle = param_find("SYS_FAIL_GPS_JAM");

		int32_t jamming_state = sensor_gnss_s::JAMMING_STATE_UNKNOWN;
		param_get(jamming_state_handle, &jamming_state);

		إذا (jamming_state != sensor_gnss_s::JAMMING_STATE_UNKNOWN) {
			sensor_gnss.jamming_state = (uint8_t)jamming_state;
		}
	}

	أرجع true;
}

esc_status_s process_esc(ثابت Config &config, ثابت esc_status_s &status)
{
	esc_status_s result = status;

	لكل (صحيح i = 0; i < result.esc_گنتی && i < esc_status_s::CONNECTED_ESC_MAX; i++) {
		ثابت uint8_t function = result.esc[i].actuator_function;

		إذا (function < esc_report_s::ACTUATOR_FUNCTION_MOTOR1 || function > esc_report_s::ACTUATOR_FUNCTION_MOTOR_MAX) {
			استمر; // not a motor output
		}

		ثابت uint8_t instance = function - esc_report_s::ACTUATOR_FUNCTION_MOTOR1 + 1; // 1-based ESC instance

		بدل (config.mode(failure_injection_s::FAILURE_UNIT_SYSTEM_ESC, instance)) {
		حالة Mode::Off:
			result.esc_online_flags &= ~(1u << i);
			result.esc_armed_flags &= ~(1u << i);
			result.esc[i] = esc_report_s{};
			result.esc[i].actuator_function = function;
			اكسر;

		حالة Mode::Wrong:
			result.esc[i].esc_voltage *= 0.1f;
			result.esc[i].esc_current *= 0.1f;
			result.esc[i].esc_rpm *= 10;
			اكسر;

		افتراضي:
			اكسر;
		}
	}

	أرجع result;
}

MotorFailureMasks process_motor(ثابت Config &config)
{
	MotorFailureMasks masks{};

	لكل (صحيح i = 0; i < esc_status_s::CONNECTED_ESC_MAX; i++) {
		ثابت uint16_t bit = 1u << i;

		بدل (config.mode(failure_injection_s::FAILURE_UNIT_SYSTEM_MOTOR, i + 1)) {
		حالة Mode::Off:
			masks.failure_mask |= bit;
			اكسر;

		حالة Mode::Wrong:
			masks.stop_mask |= bit;
			اكسر;

		افتراضي:
			اكسر;
		}
	}

	أرجع masks;
}

} // namespace failure_injection

#endif // CONFIG_MODULES_FAILURE_INJECTION_MANAGER
