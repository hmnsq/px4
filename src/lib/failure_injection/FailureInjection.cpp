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

#ಅನ್ತರ್ಭಾವ "FailureInjection.hpp"

#ಆದರೆ defined(CONFIG_MODULES_FAILURE_INJECTION_MANAGER)

#ಅನ್ತರ್ಭಾವ <parameters/param.h>
#ಅನ್ತರ್ಭಾವ <uORB/topics/battery_status.h>
#ಅನ್ತರ್ಭಾವ <uORB/topics/sensor_gnss.h>

ನಾಮಾಕಾಶ failure_injection
{

ತಾರ್ಕಿಕ Config::update()
{
	failure_injection_s cfg;

	ಆದರೆ (_sub.update(&cfg)) {
		ಗಣ(cfg);
		ಹಿಂತಿರುಗಿ ನಿಜ;
	}

	ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}

ಶೂನ್ಯ Config::ಗಣ(ಸ್ಥಿರಾಂಕ failure_injection_s &cfg)
{
	ಎಣಿಕೆ_4 = (cfg.ಎಣಿಕೆ_2 <= failure_injection_s::MAX_FAILURES) ? cfg.ಎಣಿಕೆ_2 : failure_injection_s::MAX_FAILURES;

	ಗಾಗಿ (uint8_t i = 0; i < ಎಣಿಕೆ_4; i++) {
		_unit[i] = cfg.unit[i];
		instance_ಮುಖವಾಡ[i] = cfg.instance_ಮುಖವಾಡ_2[i];
		_failure_type[i] = static_cast<ಬಹುಲಕ_2>(cfg.failure_type[i]);
	}
}

ಬಹುಲಕ_2 Config::ಬಹುಲಕ(uint8_t unit, uint8_t instance) ಸ್ಥಿರಾಂಕ
{
	ಗಾಗಿ (uint8_t i = 0; i < ಎಣಿಕೆ_4; i++) {
		ಆದರೆ (_unit[i] != unit) {
			ಮುಂದುವರಿ;
		}

		// instance == 0 matches any instance of the unit; otherwise match the
		// 1-based instance against the bitmask (0xFFFF covers all instances).
		ಆದರೆ (instance == 0 || (instance <= 16 && (instance_ಮುಖವಾಡ[i] & (1u << (instance - 1))))) {
			ಹಿಂತಿರುಗಿ _failure_type[i];
		}
	}

	ಹಿಂತಿರುಗಿ ಬಹುಲಕ_2::Ok;
}

ತಾರ್ಕಿಕ process_battery(ಸ್ಥಿರಾಂಕ Config &config, uint8_t instance, battery_ಸ್ಥಿತಿ_s &battery_ಸ್ಥಿತಿ)
{
	ಸ್ಥಿರಾಂಕ ಬಹುಲಕ_2 ಬಹುಲಕ = config.ಬಹುಲಕ(failure_injection_s::FAILURE_UNIT_SYSTEM_BATTERY, instance);
	ಸ್ಥಿರ constexpr ದಶಮಾಂಶ trigger_margin = 0.01f; // report remaining charge just below the threshold

	ಆದರೆ (ಬಹುಲಕ == ಬಹುಲಕ_2::Off) {
		// Suppress the publication so the pack reads disconnected.
		ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
	}

	ಆದರೆ (ಬಹುಲಕ != ಬಹುಲಕ_2::Wrong) {
		ಹಿಂತಿರುಗಿ ನಿಜ;
	}

	ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ param_t level_handle = param_ಕಂಡುಹಿಡಿ("SYS_FAIL_BAT_LVL");
	ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ param_t ಕೆಳಗಿನ_thr_handle = param_ಕಂಡುಹಿಡಿ("BAT_LOW_THR");
	ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ param_t crit_thr_handle = param_ಕಂಡುಹಿಡಿ("BAT_CRIT_THR");
	ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ param_t emergen_thr_handle = param_ಕಂಡುಹಿಡಿ("BAT_EMERGEN_THR");

	int32_t level = battery_ಸ್ಥಿತಿ_s::WARNING_CRITICAL;
	param_get(level_handle, &level);

	param_t ಮಿತಿ_handle = crit_thr_handle;

	ಆಯ್ಕೆ (level) {
	ಸ್ಥಿತಿ battery_ಸ್ಥಿತಿ_s::WARNING_ಕೆಳಗಿನ:
		ಮಿತಿ_handle = ಕೆಳಗಿನ_thr_handle;
		ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ battery_ಸ್ಥಿತಿ_s::WARNING_EMERGENCY:
		ಮಿತಿ_handle = emergen_thr_handle;
		ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ battery_ಸ್ಥಿತಿ_s::WARNING_CRITICAL:
	ಪೂರ್ವನಿಯೋಜಿತ:
		level = battery_ಸ್ಥಿತಿ_s::WARNING_CRITICAL;
		ಮಿತಿ_handle = crit_thr_handle;
		ನಿಲ್ಲಿಸು;
	}

	battery_ಸ್ಥಿತಿ.warning = level;


	// Report the remaining charge just below the selected threshold so the
	// matching stage of the low-battery failsafe triggers.
	ದಶಮಾಂಶ ಮಿತಿ = 0.f;
	param_get(ಮಿತಿ_handle, &ಮಿತಿ);
	battery_ಸ್ಥಿತಿ.ಉಳಿದ = (ಮಿತಿ > trigger_margin) ? (ಮಿತಿ - trigger_margin) : 0.f;

	ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ process_gnss(ಸ್ಥಿರಾಂಕ Config &config, uint8_t uorb_instance, sensor_gnss_s &sensor_gnss,
		  Stuck<sensor_gnss_s> &stuck)
{
	ಸ್ಥಿರಾಂಕ ಬಹುಲಕ_2 ಬಹುಲಕ = config.ಬಹುಲಕ(failure_injection_s::FAILURE_UNIT_SENSOR_GPS, uorb_instance + 1);

	// Off and Stuck are message-agnostic; run them first so the Stuck cache keeps the
	// uncorrupted sample and a later Stuck replays a healthy fix.
	ಆದರೆ (!process(ಬಹುಲಕ, sensor_gnss, stuck)) {
		ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
	}

	ಆದರೆ (ಬಹುಲಕ == ಬಹುಲಕ_2::Wrong) {
		ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ param_t fix_type_handle = param_ಕಂಡುಹಿಡಿ("SYS_FAIL_GPS_WRG");

		int32_t fix_type = sensor_gnss_s::FIX_TYPE_2D;
		param_get(fix_type_handle, &fix_type);
		sensor_gnss.fix_type = (uint8_t)fix_type;

		ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ param_t jamming_state_handle = param_ಕಂಡುಹಿಡಿ("SYS_FAIL_GPS_JAM");

		int32_t jamming_state = sensor_gnss_s::JAMMING_STATE_UNKNOWN;
		param_get(jamming_state_handle, &jamming_state);

		ಆದರೆ (jamming_state != sensor_gnss_s::JAMMING_STATE_UNKNOWN) {
			sensor_gnss.jamming_state = (uint8_t)jamming_state;
		}
	}

	ಹಿಂತಿರುಗಿ ನಿಜ;
}

esc_ಸ್ಥಿತಿ_s process_esc(ಸ್ಥಿರಾಂಕ Config &config, ಸ್ಥಿರಾಂಕ esc_ಸ್ಥಿತಿ_s &ಸ್ಥಿತಿ_2)
{
	esc_ಸ್ಥಿತಿ_s ಫಲಿತಾಂಶ = ಸ್ಥಿತಿ_2;

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ಫಲಿತಾಂಶ.esc_ಎಣಿಕೆ && i < esc_ಸ್ಥಿತಿ_s::CONNECTED_ESC_MAX; i++) {
		ಸ್ಥಿರಾಂಕ uint8_t function = ಫಲಿತಾಂಶ.esc[i].actuator_function;

		ಆದರೆ (function < esc_report_s::ACTUATOR_FUNCTION_MOTOR1 || function > esc_report_s::ACTUATOR_FUNCTION_MOTOR_MAX) {
			ಮುಂದುವರಿ; // not a motor output
		}

		ಸ್ಥಿರಾಂಕ uint8_t instance = function - esc_report_s::ACTUATOR_FUNCTION_MOTOR1 + 1; // 1-based ESC instance

		ಆಯ್ಕೆ (config.ಬಹುಲಕ(failure_injection_s::FAILURE_UNIT_SYSTEM_ESC, instance)) {
		ಸ್ಥಿತಿ ಬಹುಲಕ_2::Off:
			ಫಲಿತಾಂಶ.esc_online_flags &= ~(1u << i);
			ಫಲಿತಾಂಶ.esc_armed_flags &= ~(1u << i);
			ಫಲಿತಾಂಶ.esc[i] = esc_report_s{};
			ಫಲಿತಾಂಶ.esc[i].actuator_function = function;
			ನಿಲ್ಲಿಸು;

		ಸ್ಥಿತಿ ಬಹುಲಕ_2::Wrong:
			ಫಲಿತಾಂಶ.esc[i].esc_voltage *= 0.1f;
			ಫಲಿತಾಂಶ.esc[i].esc_current *= 0.1f;
			ಫಲಿತಾಂಶ.esc[i].esc_rpm *= 10;
			ನಿಲ್ಲಿಸು;

		ಪೂರ್ವನಿಯೋಜಿತ:
			ನಿಲ್ಲಿಸು;
		}
	}

	ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

MotorFailureMasks process_motor(ಸ್ಥಿರಾಂಕ Config &config)
{
	MotorFailureMasks masks{};

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < esc_ಸ್ಥಿತಿ_s::CONNECTED_ESC_MAX; i++) {
		ಸ್ಥಿರಾಂಕ uint16_t bit = 1u << i;

		ಆಯ್ಕೆ (config.ಬಹುಲಕ(failure_injection_s::FAILURE_UNIT_SYSTEM_MOTOR, i + 1)) {
		ಸ್ಥಿತಿ ಬಹುಲಕ_2::Off:
			masks.failure_ಮುಖವಾಡ |= bit;
			ನಿಲ್ಲಿಸು;

		ಸ್ಥಿತಿ ಬಹುಲಕ_2::Wrong:
			masks.stop_ಮುಖವಾಡ |= bit;
			ನಿಲ್ಲಿಸು;

		ಪೂರ್ವನಿಯೋಜಿತ:
			ನಿಲ್ಲಿಸು;
		}
	}

	ಹಿಂತಿರುಗಿ masks;
}

} // namespace failure_injection

#ಆದರೆ_ಅಂತ್ಯ // CONFIG_MODULES_FAILURE_INJECTION_MANAGER
