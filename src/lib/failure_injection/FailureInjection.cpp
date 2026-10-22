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

#അന്തര്ഭാവ "FailureInjection.hpp"

#എങ്കിൽ defined(CONFIG_MODULES_FAILURE_INJECTION_MANAGER)

#അന്തര്ഭാവ <parameters/param.h>
#അന്തര്ഭാവ <uORB/topics/battery_status.h>
#അന്തര്ഭാവ <uORB/topics/sensor_gnss.h>

നാമാകാശ failure_injection
{

തർക്കം Config::update()
{
	failure_injection_s cfg;

	എങ്കിൽ (_sub.update(&cfg)) {
		set(cfg);
		തിരികെ സത്യം;
	}

	തിരികെ അസത്യം;
}

ശൂന്യം Config::set(സ്ഥിരം failure_injection_s &cfg)
{
	എണ്ണം_3 = (cfg.എണ്ണം <= failure_injection_s::MAX_FAILURES) ? cfg.എണ്ണം : failure_injection_s::MAX_FAILURES;

	വേണ്ടി (uint8_t i = 0; i < എണ്ണം_3; i++) {
		_unit[i] = cfg.unit[i];
		_instance_mask[i] = cfg.instance_mask[i];
		_failure_type[i] = static_cast<Mode>(cfg.failure_type[i]);
	}
}

Mode Config::mode(uint8_t unit, uint8_t instance) സ്ഥിരം
{
	വേണ്ടി (uint8_t i = 0; i < എണ്ണം_3; i++) {
		എങ്കിൽ (_unit[i] != unit) {
			തുടരുക;
		}

		// instance == 0 matches any instance of the unit; otherwise match the
		// 1-based instance against the bitmask (0xFFFF covers all instances).
		എങ്കിൽ (instance == 0 || (instance <= 16 && (_instance_mask[i] & (1u << (instance - 1))))) {
			തിരികെ _failure_type[i];
		}
	}

	തിരികെ Mode::Ok;
}

തർക്കം process_battery(സ്ഥിരം Config &config, uint8_t instance, battery_status_s &battery_status)
{
	സ്ഥിരം Mode mode = config.mode(failure_injection_s::FAILURE_UNIT_SYSTEM_BATTERY, instance);
	സ്ഥിതം constexpr ദശാംശം trigger_margin = 0.01f; // report remaining charge just below the threshold

	എങ്കിൽ (mode == Mode::Off) {
		// Suppress the publication so the pack reads disconnected.
		തിരികെ അസത്യം;
	}

	എങ്കിൽ (mode != Mode::Wrong) {
		തിരികെ സത്യം;
	}

	സ്ഥിതം സ്ഥിരം param_t level_handle = param_find("SYS_FAIL_BAT_LVL");
	സ്ഥിതം സ്ഥിരം param_t താഴെയുള്ള_thr_handle = param_find("BAT_LOW_THR");
	സ്ഥിതം സ്ഥിരം param_t crit_thr_handle = param_find("BAT_CRIT_THR");
	സ്ഥിതം സ്ഥിരം param_t emergen_thr_handle = param_find("BAT_EMERGEN_THR");

	int32_t level = battery_status_s::WARNING_CRITICAL;
	param_get(level_handle, &level);

	param_t threshold_handle = crit_thr_handle;

	തിരഞ്ഞെടുപ്പ് (level) {
	അവസ്ഥ battery_status_s::WARNING_താഴെയുള്ള:
		threshold_handle = താഴെയുള്ള_thr_handle;
		നിർത്തുക;

	അവസ്ഥ battery_status_s::WARNING_EMERGENCY:
		threshold_handle = emergen_thr_handle;
		നിർത്തുക;

	അവസ്ഥ battery_status_s::WARNING_CRITICAL:
	സ്ഥിരസ്ഥിതി:
		level = battery_status_s::WARNING_CRITICAL;
		threshold_handle = crit_thr_handle;
		നിർത്തുക;
	}

	battery_status.warning = level;


	// Report the remaining charge just below the selected threshold so the
	// matching stage of the low-battery failsafe triggers.
	ദശാംശം threshold = 0.f;
	param_get(threshold_handle, &threshold);
	battery_status.remaining = (threshold > trigger_margin) ? (threshold - trigger_margin) : 0.f;

	തിരികെ സത്യം;
}

തർക്കം process_gnss(സ്ഥിരം Config &config, uint8_t uorb_instance, sensor_gnss_s &sensor_gnss,
		  Stuck<sensor_gnss_s> &stuck)
{
	സ്ഥിരം Mode mode = config.mode(failure_injection_s::FAILURE_UNIT_SENSOR_GPS, uorb_instance + 1);

	// Off and Stuck are message-agnostic; run them first so the Stuck cache keeps the
	// uncorrupted sample and a later Stuck replays a healthy fix.
	എങ്കിൽ (!process(mode, sensor_gnss, stuck)) {
		തിരികെ അസത്യം;
	}

	എങ്കിൽ (mode == Mode::Wrong) {
		സ്ഥിതം സ്ഥിരം param_t fix_type_handle = param_find("SYS_FAIL_GPS_WRG");

		int32_t fix_type = sensor_gnss_s::FIX_TYPE_2D;
		param_get(fix_type_handle, &fix_type);
		sensor_gnss.fix_type = (uint8_t)fix_type;

		സ്ഥിതം സ്ഥിരം param_t jamming_state_handle = param_find("SYS_FAIL_GPS_JAM");

		int32_t jamming_state = sensor_gnss_s::JAMMING_STATE_UNKNOWN;
		param_get(jamming_state_handle, &jamming_state);

		എങ്കിൽ (jamming_state != sensor_gnss_s::JAMMING_STATE_UNKNOWN) {
			sensor_gnss.jamming_state = (uint8_t)jamming_state;
		}
	}

	തിരികെ സത്യം;
}

esc_status_s process_esc(സ്ഥിരം Config &config, സ്ഥിരം esc_status_s &status)
{
	esc_status_s result = status;

	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < result.esc_എണ്ണം && i < esc_status_s::CONNECTED_ESC_MAX; i++) {
		സ്ഥിരം uint8_t function = result.esc[i].actuator_function;

		എങ്കിൽ (function < esc_report_s::ACTUATOR_FUNCTION_MOTOR1 || function > esc_report_s::ACTUATOR_FUNCTION_MOTOR_MAX) {
			തുടരുക; // not a motor output
		}

		സ്ഥിരം uint8_t instance = function - esc_report_s::ACTUATOR_FUNCTION_MOTOR1 + 1; // 1-based ESC instance

		തിരഞ്ഞെടുപ്പ് (config.mode(failure_injection_s::FAILURE_UNIT_SYSTEM_ESC, instance)) {
		അവസ്ഥ Mode::Off:
			result.esc_online_flags &= ~(1u << i);
			result.esc_armed_flags &= ~(1u << i);
			result.esc[i] = esc_report_s{};
			result.esc[i].actuator_function = function;
			നിർത്തുക;

		അവസ്ഥ Mode::Wrong:
			result.esc[i].esc_voltage *= 0.1f;
			result.esc[i].esc_current *= 0.1f;
			result.esc[i].esc_rpm *= 10;
			നിർത്തുക;

		സ്ഥിരസ്ഥിതി:
			നിർത്തുക;
		}
	}

	തിരികെ result;
}

MotorFailureMasks process_motor(സ്ഥിരം Config &config)
{
	MotorFailureMasks masks{};

	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < esc_status_s::CONNECTED_ESC_MAX; i++) {
		സ്ഥിരം uint16_t bit = 1u << i;

		തിരഞ്ഞെടുപ്പ് (config.mode(failure_injection_s::FAILURE_UNIT_SYSTEM_MOTOR, i + 1)) {
		അവസ്ഥ Mode::Off:
			masks.failure_mask |= bit;
			നിർത്തുക;

		അവസ്ഥ Mode::Wrong:
			masks.stop_mask |= bit;
			നിർത്തുക;

		സ്ഥിരസ്ഥിതി:
			നിർത്തുക;
		}
	}

	തിരികെ masks;
}

} // namespace failure_injection

#endif // CONFIG_MODULES_FAILURE_INJECTION_MANAGER
