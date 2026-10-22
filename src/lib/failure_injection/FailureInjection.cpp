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

#ਅਨ੍ਤਰ੍ਭਾਵ "FailureInjection.hpp"

#ਜੇ defined(CONFIG_MODULES_FAILURE_INJECTION_MANAGER)

#ਅਨ੍ਤਰ੍ਭਾਵ <parameters/param.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <uORB/topics/battery_status.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <uORB/topics/sensor_gnss.h>

ਨਾਮਾਕਾਸ਼ failure_injection
{

ਤਾਰਕਿਕ Config::update()
{
	failure_injection_s cfg;

	ਜੇ (_sub.update(&cfg)) {
		ਸੈੱਟ(cfg);
		ਵਾਪਸ ਸੱਚ;
	}

	ਵਾਪਸ ਝੂਠ;
}

ਖਾਲੀ Config::ਸੈੱਟ(ਅਚਲ failure_injection_s &cfg)
{
	ਗਿਣਤੀ_4 = (cfg.ਗਿਣਤੀ_2 <= failure_injection_s::MAX_FAILURES) ? cfg.ਗਿਣਤੀ_2 : failure_injection_s::MAX_FAILURES;

	ਲਈ (uint8_t i = 0; i < ਗਿਣਤੀ_4; i++) {
		_unit[i] = cfg.unit[i];
		instance_ਨਕਾਬ[i] = cfg.instance_ਨਕਾਬ_2[i];
		_failure_type[i] = static_cast<ਬਹੁਲਕ_2>(cfg.failure_type[i]);
	}
}

ਬਹੁਲਕ_2 Config::ਬਹੁਲਕ(uint8_t unit, uint8_t instance) ਅਚਲ
{
	ਲਈ (uint8_t i = 0; i < ਗਿਣਤੀ_4; i++) {
		ਜੇ (_unit[i] != unit) {
			ਜਾਰੀ;
		}

		// instance == 0 matches any instance of the unit; otherwise match the
		// 1-based instance against the bitmask (0xFFFF covers all instances).
		ਜੇ (instance == 0 || (instance <= 16 && (instance_ਨਕਾਬ[i] & (1u << (instance - 1))))) {
			ਵਾਪਸ _failure_type[i];
		}
	}

	ਵਾਪਸ ਬਹੁਲਕ_2::Ok;
}

ਤਾਰਕਿਕ process_battery(ਅਚਲ Config &config, uint8_t instance, battery_ਸਥਿਤੀ_s &battery_ਸਥਿਤੀ)
{
	ਅਚਲ ਬਹੁਲਕ_2 ਬਹੁਲਕ = config.ਬਹੁਲਕ(failure_injection_s::FAILURE_UNIT_SYSTEM_BATTERY, instance);
	ਸਥਿਰ constexpr ਦਸ਼ਮਲਵ trigger_margin = 0.01f; // report remaining charge just below the threshold

	ਜੇ (ਬਹੁਲਕ == ਬਹੁਲਕ_2::Off) {
		// Suppress the publication so the pack reads disconnected.
		ਵਾਪਸ ਝੂਠ;
	}

	ਜੇ (ਬਹੁਲਕ != ਬਹੁਲਕ_2::Wrong) {
		ਵਾਪਸ ਸੱਚ;
	}

	ਸਥਿਰ ਅਚਲ param_t level_handle = param_ਲੱਭੋ("SYS_FAIL_BAT_LVL");
	ਸਥਿਰ ਅਚਲ param_t ਨੀਵਾਂ_thr_handle = param_ਲੱਭੋ("BAT_LOW_THR");
	ਸਥਿਰ ਅਚਲ param_t crit_thr_handle = param_ਲੱਭੋ("BAT_CRIT_THR");
	ਸਥਿਰ ਅਚਲ param_t emergen_thr_handle = param_ਲੱਭੋ("BAT_EMERGEN_THR");

	int32_t level = battery_ਸਥਿਤੀ_s::WARNING_CRITICAL;
	param_get(level_handle, &level);

	param_t ਹੱਦ_handle = crit_thr_handle;

	ਚੋਣ (level) {
	ਹਾਲਤ battery_ਸਥਿਤੀ_s::WARNING_ਨੀਵਾਂ:
		ਹੱਦ_handle = ਨੀਵਾਂ_thr_handle;
		ਰੋਕੋ;

	ਹਾਲਤ battery_ਸਥਿਤੀ_s::WARNING_EMERGENCY:
		ਹੱਦ_handle = emergen_thr_handle;
		ਰੋਕੋ;

	ਹਾਲਤ battery_ਸਥਿਤੀ_s::WARNING_CRITICAL:
	ਮੂਲ:
		level = battery_ਸਥਿਤੀ_s::WARNING_CRITICAL;
		ਹੱਦ_handle = crit_thr_handle;
		ਰੋਕੋ;
	}

	battery_ਸਥਿਤੀ.warning = level;


	// Report the remaining charge just below the selected threshold so the
	// matching stage of the low-battery failsafe triggers.
	ਦਸ਼ਮਲਵ ਹੱਦ = 0.f;
	param_get(ਹੱਦ_handle, &ਹੱਦ);
	battery_ਸਥਿਤੀ.ਬਚਿਆ = (ਹੱਦ > trigger_margin) ? (ਹੱਦ - trigger_margin) : 0.f;

	ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ process_gnss(ਅਚਲ Config &config, uint8_t uorb_instance, sensor_gnss_s &sensor_gnss,
		  Stuck<sensor_gnss_s> &stuck)
{
	ਅਚਲ ਬਹੁਲਕ_2 ਬਹੁਲਕ = config.ਬਹੁਲਕ(failure_injection_s::FAILURE_UNIT_SENSOR_GPS, uorb_instance + 1);

	// Off and Stuck are message-agnostic; run them first so the Stuck cache keeps the
	// uncorrupted sample and a later Stuck replays a healthy fix.
	ਜੇ (!process(ਬਹੁਲਕ, sensor_gnss, stuck)) {
		ਵਾਪਸ ਝੂਠ;
	}

	ਜੇ (ਬਹੁਲਕ == ਬਹੁਲਕ_2::Wrong) {
		ਸਥਿਰ ਅਚਲ param_t fix_type_handle = param_ਲੱਭੋ("SYS_FAIL_GPS_WRG");

		int32_t fix_type = sensor_gnss_s::FIX_TYPE_2D;
		param_get(fix_type_handle, &fix_type);
		sensor_gnss.fix_type = (uint8_t)fix_type;

		ਸਥਿਰ ਅਚਲ param_t jamming_state_handle = param_ਲੱਭੋ("SYS_FAIL_GPS_JAM");

		int32_t jamming_state = sensor_gnss_s::JAMMING_STATE_UNKNOWN;
		param_get(jamming_state_handle, &jamming_state);

		ਜੇ (jamming_state != sensor_gnss_s::JAMMING_STATE_UNKNOWN) {
			sensor_gnss.jamming_state = (uint8_t)jamming_state;
		}
	}

	ਵਾਪਸ ਸੱਚ;
}

esc_ਸਥਿਤੀ_s process_esc(ਅਚਲ Config &config, ਅਚਲ esc_ਸਥਿਤੀ_s &ਸਥਿਤੀ)
{
	esc_ਸਥਿਤੀ_s ਨਤੀਜਾ = ਸਥਿਤੀ;

	ਲਈ (ਪੂਰਨ i = 0; i < ਨਤੀਜਾ.esc_ਗਿਣਤੀ && i < esc_ਸਥਿਤੀ_s::CONNECTED_ESC_MAX; i++) {
		ਅਚਲ uint8_t function = ਨਤੀਜਾ.esc[i].actuator_function;

		ਜੇ (function < esc_report_s::ACTUATOR_FUNCTION_MOTOR1 || function > esc_report_s::ACTUATOR_FUNCTION_MOTOR_MAX) {
			ਜਾਰੀ; // not a motor output
		}

		ਅਚਲ uint8_t instance = function - esc_report_s::ACTUATOR_FUNCTION_MOTOR1 + 1; // 1-based ESC instance

		ਚੋਣ (config.ਬਹੁਲਕ(failure_injection_s::FAILURE_UNIT_SYSTEM_ESC, instance)) {
		ਹਾਲਤ ਬਹੁਲਕ_2::Off:
			ਨਤੀਜਾ.esc_online_flags &= ~(1u << i);
			ਨਤੀਜਾ.esc_armed_flags &= ~(1u << i);
			ਨਤੀਜਾ.esc[i] = esc_report_s{};
			ਨਤੀਜਾ.esc[i].actuator_function = function;
			ਰੋਕੋ;

		ਹਾਲਤ ਬਹੁਲਕ_2::Wrong:
			ਨਤੀਜਾ.esc[i].esc_voltage *= 0.1f;
			ਨਤੀਜਾ.esc[i].esc_current *= 0.1f;
			ਨਤੀਜਾ.esc[i].esc_rpm *= 10;
			ਰੋਕੋ;

		ਮੂਲ:
			ਰੋਕੋ;
		}
	}

	ਵਾਪਸ ਨਤੀਜਾ;
}

MotorFailureMasks process_motor(ਅਚਲ Config &config)
{
	MotorFailureMasks masks{};

	ਲਈ (ਪੂਰਨ i = 0; i < esc_ਸਥਿਤੀ_s::CONNECTED_ESC_MAX; i++) {
		ਅਚਲ uint16_t bit = 1u << i;

		ਚੋਣ (config.ਬਹੁਲਕ(failure_injection_s::FAILURE_UNIT_SYSTEM_MOTOR, i + 1)) {
		ਹਾਲਤ ਬਹੁਲਕ_2::Off:
			masks.failure_ਨਕਾਬ |= bit;
			ਰੋਕੋ;

		ਹਾਲਤ ਬਹੁਲਕ_2::Wrong:
			masks.stop_ਨਕਾਬ |= bit;
			ਰੋਕੋ;

		ਮੂਲ:
			ਰੋਕੋ;
		}
	}

	ਵਾਪਸ masks;
}

} // namespace failure_injection

#ਜੇ_ਅੰਤ // CONFIG_MODULES_FAILURE_INJECTION_MANAGER
