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

#ಯದಿ defined(CONFIG_MODULES_FAILURE_INJECTION_MANAGER)

#ಅನ್ತರ್ಭಾವ <parameters/param.h>
#ಅನ್ತರ್ಭಾವ <uORB/topics/battery_status.h>
#ಅನ್ತರ್ಭಾವ <uORB/topics/sensor_gnss.h>

ನಾಮಾಕಾಶ failure_injection
{

ತಾರ್ಕಿಕ Config::update()
{
	failure_injection_s cfg;

	ಯದಿ (_sub.update(&cfg)) {
		set(cfg);
		ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
	}

	ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
}

ಶೂನ್ಯ Config::set(ಧ್ರುವ failure_injection_s &cfg)
{
	ಎಣಿಕೆ_3 = (cfg.ಎಣಿಕೆ <= failure_injection_s::MAX_FAILURES) ? cfg.ಎಣಿಕೆ : failure_injection_s::MAX_FAILURES;

	ಪ್ರತಿ (uint8_t i = 0; i < ಎಣಿಕೆ_3; i++) {
		_unit[i] = cfg.unit[i];
		_instance_mask[i] = cfg.instance_mask[i];
		_failure_type[i] = static_cast<Mode>(cfg.failure_type[i]);
	}
}

Mode Config::mode(uint8_t unit, uint8_t instance) ಧ್ರುವ
{
	ಪ್ರತಿ (uint8_t i = 0; i < ಎಣಿಕೆ_3; i++) {
		ಯದಿ (_unit[i] != unit) {
			ಅನುವರ್ತ;
		}

		// instance == 0 matches any instance of the unit; otherwise match the
		// 1-based instance against the bitmask (0xFFFF covers all instances).
		ಯದಿ (instance == 0 || (instance <= 16 && (_instance_mask[i] & (1u << (instance - 1))))) {
			ಪ್ರತ್ಯಾಗಮ _failure_type[i];
		}
	}

	ಪ್ರತ್ಯಾಗಮ Mode::Ok;
}

ತಾರ್ಕಿಕ process_battery(ಧ್ರುವ Config &config, uint8_t instance, battery_status_s &battery_status)
{
	ಧ್ರುವ Mode mode = config.mode(failure_injection_s::FAILURE_UNIT_SYSTEM_BATTERY, instance);
	ಸ್ಥಿರ constexpr ಭಿನ್ನ trigger_margin = 0.01f; // report remaining charge just below the threshold

	ಯದಿ (mode == Mode::Off) {
		// Suppress the publication so the pack reads disconnected.
		ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
	}

	ಯದಿ (mode != Mode::Wrong) {
		ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
	}

	ಸ್ಥಿರ ಧ್ರುವ param_t level_handle = param_find("SYS_FAIL_BAT_LVL");
	ಸ್ಥಿರ ಧ್ರುವ param_t ಕೆಳಗಿನ_thr_handle = param_find("BAT_LOW_THR");
	ಸ್ಥಿರ ಧ್ರುವ param_t crit_thr_handle = param_find("BAT_CRIT_THR");
	ಸ್ಥಿರ ಧ್ರುವ param_t emergen_thr_handle = param_find("BAT_EMERGEN_THR");

	int32_t level = battery_status_s::WARNING_CRITICAL;
	param_get(level_handle, &level);

	param_t threshold_handle = crit_thr_handle;

	ವಿಕಲ್ಪ (level) {
	ಪಕ್ಷ battery_status_s::WARNING_ಕೆಳಗಿನ:
		threshold_handle = ಕೆಳಗಿನ_thr_handle;
		ಭಙ್ಗ;

	ಪಕ್ಷ battery_status_s::WARNING_EMERGENCY:
		threshold_handle = emergen_thr_handle;
		ಭಙ್ಗ;

	ಪಕ್ಷ battery_status_s::WARNING_CRITICAL:
	ಸ್ವಭಾವ:
		level = battery_status_s::WARNING_CRITICAL;
		threshold_handle = crit_thr_handle;
		ಭಙ್ಗ;
	}

	battery_status.warning = level;


	// Report the remaining charge just below the selected threshold so the
	// matching stage of the low-battery failsafe triggers.
	ಭಿನ್ನ threshold = 0.f;
	param_get(threshold_handle, &threshold);
	battery_status.remaining = (threshold > trigger_margin) ? (threshold - trigger_margin) : 0.f;

	ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

ತಾರ್ಕಿಕ process_gnss(ಧ್ರುವ Config &config, uint8_t uorb_instance, sensor_gnss_s &sensor_gnss,
		  Stuck<sensor_gnss_s> &stuck)
{
	ಧ್ರುವ Mode mode = config.mode(failure_injection_s::FAILURE_UNIT_SENSOR_GPS, uorb_instance + 1);

	// Off and Stuck are message-agnostic; run them first so the Stuck cache keeps the
	// uncorrupted sample and a later Stuck replays a healthy fix.
	ಯದಿ (!process(mode, sensor_gnss, stuck)) {
		ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
	}

	ಯದಿ (mode == Mode::Wrong) {
		ಸ್ಥಿರ ಧ್ರುವ param_t fix_type_handle = param_find("SYS_FAIL_GPS_WRG");

		int32_t fix_type = sensor_gnss_s::FIX_TYPE_2D;
		param_get(fix_type_handle, &fix_type);
		sensor_gnss.fix_type = (uint8_t)fix_type;

		ಸ್ಥಿರ ಧ್ರುವ param_t jamming_state_handle = param_find("SYS_FAIL_GPS_JAM");

		int32_t jamming_state = sensor_gnss_s::JAMMING_STATE_UNKNOWN;
		param_get(jamming_state_handle, &jamming_state);

		ಯದಿ (jamming_state != sensor_gnss_s::JAMMING_STATE_UNKNOWN) {
			sensor_gnss.jamming_state = (uint8_t)jamming_state;
		}
	}

	ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

esc_status_s process_esc(ಧ್ರುವ Config &config, ಧ್ರುವ esc_status_s &status)
{
	esc_status_s result = status;

	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < result.esc_ಎಣಿಕೆ && i < esc_status_s::CONNECTED_ESC_MAX; i++) {
		ಧ್ರುವ uint8_t function = result.esc[i].actuator_function;

		ಯದಿ (function < esc_report_s::ACTUATOR_FUNCTION_MOTOR1 || function > esc_report_s::ACTUATOR_FUNCTION_MOTOR_MAX) {
			ಅನುವರ್ತ; // not a motor output
		}

		ಧ್ರುವ uint8_t instance = function - esc_report_s::ACTUATOR_FUNCTION_MOTOR1 + 1; // 1-based ESC instance

		ವಿಕಲ್ಪ (config.mode(failure_injection_s::FAILURE_UNIT_SYSTEM_ESC, instance)) {
		ಪಕ್ಷ Mode::Off:
			result.esc_online_flags &= ~(1u << i);
			result.esc_armed_flags &= ~(1u << i);
			result.esc[i] = esc_report_s{};
			result.esc[i].actuator_function = function;
			ಭಙ್ಗ;

		ಪಕ್ಷ Mode::Wrong:
			result.esc[i].esc_voltage *= 0.1f;
			result.esc[i].esc_current *= 0.1f;
			result.esc[i].esc_rpm *= 10;
			ಭಙ್ಗ;

		ಸ್ವಭಾವ:
			ಭಙ್ಗ;
		}
	}

	ಪ್ರತ್ಯಾಗಮ result;
}

MotorFailureMasks process_motor(ಧ್ರುವ Config &config)
{
	MotorFailureMasks masks{};

	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < esc_status_s::CONNECTED_ESC_MAX; i++) {
		ಧ್ರುವ uint16_t bit = 1u << i;

		ವಿಕಲ್ಪ (config.mode(failure_injection_s::FAILURE_UNIT_SYSTEM_MOTOR, i + 1)) {
		ಪಕ್ಷ Mode::Off:
			masks.failure_mask |= bit;
			ಭಙ್ಗ;

		ಪಕ್ಷ Mode::Wrong:
			masks.stop_mask |= bit;
			ಭಙ್ಗ;

		ಸ್ವಭಾವ:
			ಭಙ್ಗ;
		}
	}

	ಪ್ರತ್ಯಾಗಮ masks;
}

} // namespace failure_injection

#endif // CONFIG_MODULES_FAILURE_INJECTION_MANAGER
