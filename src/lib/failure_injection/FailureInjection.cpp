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

#अन्तर्भाव "FailureInjection.hpp"

#यदि defined(CONFIG_MODULES_FAILURE_INJECTION_MANAGER)

#अन्तर्भाव <parameters/param.h>
#अन्तर्भाव <uORB/topics/battery_status.h>
#अन्तर्भाव <uORB/topics/sensor_gnss.h>

नामाकाश failure_injection
{

तार्किक Config::update()
{
	failure_injection_s cfg;

	यदि (_sub.update(&cfg)) {
		समुच्चयः(cfg);
		प्रत्यागम सत्य;
	}

	प्रत्यागम असत्य;
}

शून्य Config::समुच्चयः(ध्रुव failure_injection_s &cfg)
{
	गणनम्_3 = (cfg.गणनम् <= failure_injection_s::MAX_FAILURES) ? cfg.गणनम् : failure_injection_s::MAX_FAILURES;

	प्रति (uint8_t i = 0; i < गणनम्_3; i++) {
		_unit[i] = cfg.unit[i];
		instance_आवरणम्[i] = cfg.instance_आवरणम्_2[i];
		_failure_type[i] = static_cast<बहुलकम्_2>(cfg.failure_type[i]);
	}
}

बहुलकम्_2 Config::बहुलकम्(uint8_t unit, uint8_t instance) ध्रुव
{
	प्रति (uint8_t i = 0; i < गणनम्_3; i++) {
		यदि (_unit[i] != unit) {
			अनुवर्त;
		}

		// instance == 0 matches any instance of the unit; otherwise match the
		// 1-based instance against the bitmask (0xFFFF covers all instances).
		यदि (instance == 0 || (instance <= 16 && (instance_आवरणम्[i] & (1u << (instance - 1))))) {
			प्रत्यागम _failure_type[i];
		}
	}

	प्रत्यागम बहुलकम्_2::Ok;
}

तार्किक process_battery(ध्रुव Config &config, uint8_t instance, battery_स्थितिः_s &battery_स्थितिः)
{
	ध्रुव बहुलकम्_2 बहुलकम् = config.बहुलकम्(failure_injection_s::FAILURE_UNIT_SYSTEM_BATTERY, instance);
	स्थिर constexpr भिन्न trigger_margin = 0.01f; // report remaining charge just below the threshold

	यदि (बहुलकम् == बहुलकम्_2::Off) {
		// Suppress the publication so the pack reads disconnected.
		प्रत्यागम असत्य;
	}

	यदि (बहुलकम् != बहुलकम्_2::Wrong) {
		प्रत्यागम सत्य;
	}

	स्थिर ध्रुव param_t level_handle = param_find("SYS_FAIL_BAT_LVL");
	स्थिर ध्रुव param_t नीचः_thr_handle = param_find("BAT_LOW_THR");
	स्थिर ध्रुव param_t crit_thr_handle = param_find("BAT_CRIT_THR");
	स्थिर ध्रुव param_t emergen_thr_handle = param_find("BAT_EMERGEN_THR");

	int32_t level = battery_स्थितिः_s::WARNING_CRITICAL;
	param_get(level_handle, &level);

	param_t देहली_handle = crit_thr_handle;

	विकल्प (level) {
	पक्ष battery_स्थितिः_s::WARNING_नीचः:
		देहली_handle = नीचः_thr_handle;
		भङ्ग;

	पक्ष battery_स्थितिः_s::WARNING_EMERGENCY:
		देहली_handle = emergen_thr_handle;
		भङ्ग;

	पक्ष battery_स्थितिः_s::WARNING_CRITICAL:
	स्वभाव:
		level = battery_स्थितिः_s::WARNING_CRITICAL;
		देहली_handle = crit_thr_handle;
		भङ्ग;
	}

	battery_स्थितिः.warning = level;


	// Report the remaining charge just below the selected threshold so the
	// matching stage of the low-battery failsafe triggers.
	भिन्न देहली = 0.f;
	param_get(देहली_handle, &देहली);
	battery_स्थितिः.remaining = (देहली > trigger_margin) ? (देहली - trigger_margin) : 0.f;

	प्रत्यागम सत्य;
}

तार्किक process_gnss(ध्रुव Config &config, uint8_t uorb_instance, sensor_gnss_s &sensor_gnss,
		  Stuck<sensor_gnss_s> &stuck)
{
	ध्रुव बहुलकम्_2 बहुलकम् = config.बहुलकम्(failure_injection_s::FAILURE_UNIT_SENSOR_GPS, uorb_instance + 1);

	// Off and Stuck are message-agnostic; run them first so the Stuck cache keeps the
	// uncorrupted sample and a later Stuck replays a healthy fix.
	यदि (!process(बहुलकम्, sensor_gnss, stuck)) {
		प्रत्यागम असत्य;
	}

	यदि (बहुलकम् == बहुलकम्_2::Wrong) {
		स्थिर ध्रुव param_t fix_type_handle = param_find("SYS_FAIL_GPS_WRG");

		int32_t fix_type = sensor_gnss_s::FIX_TYPE_2D;
		param_get(fix_type_handle, &fix_type);
		sensor_gnss.fix_type = (uint8_t)fix_type;

		स्थिर ध्रुव param_t jamming_state_handle = param_find("SYS_FAIL_GPS_JAM");

		int32_t jamming_state = sensor_gnss_s::JAMMING_STATE_UNKNOWN;
		param_get(jamming_state_handle, &jamming_state);

		यदि (jamming_state != sensor_gnss_s::JAMMING_STATE_UNKNOWN) {
			sensor_gnss.jamming_state = (uint8_t)jamming_state;
		}
	}

	प्रत्यागम सत्य;
}

esc_स्थितिः_s process_esc(ध्रुव Config &config, ध्रुव esc_स्थितिः_s &स्थितिः)
{
	esc_स्थितिः_s परिणाम = स्थितिः;

	प्रति (पूर्णाङ्क i = 0; i < परिणाम.esc_गणनम् && i < esc_स्थितिः_s::CONNECTED_ESC_MAX; i++) {
		ध्रुव uint8_t function = परिणाम.esc[i].actuator_function;

		यदि (function < esc_report_s::ACTUATOR_FUNCTION_MOTOR1 || function > esc_report_s::ACTUATOR_FUNCTION_MOTOR_MAX) {
			अनुवर्त; // not a motor output
		}

		ध्रुव uint8_t instance = function - esc_report_s::ACTUATOR_FUNCTION_MOTOR1 + 1; // 1-based ESC instance

		विकल्प (config.बहुलकम्(failure_injection_s::FAILURE_UNIT_SYSTEM_ESC, instance)) {
		पक्ष बहुलकम्_2::Off:
			परिणाम.esc_online_flags &= ~(1u << i);
			परिणाम.esc_armed_flags &= ~(1u << i);
			परिणाम.esc[i] = esc_report_s{};
			परिणाम.esc[i].actuator_function = function;
			भङ्ग;

		पक्ष बहुलकम्_2::Wrong:
			परिणाम.esc[i].esc_voltage *= 0.1f;
			परिणाम.esc[i].esc_current *= 0.1f;
			परिणाम.esc[i].esc_rpm *= 10;
			भङ्ग;

		स्वभाव:
			भङ्ग;
		}
	}

	प्रत्यागम परिणाम;
}

MotorFailureMasks process_motor(ध्रुव Config &config)
{
	MotorFailureMasks masks{};

	प्रति (पूर्णाङ्क i = 0; i < esc_स्थितिः_s::CONNECTED_ESC_MAX; i++) {
		ध्रुव uint16_t bit = 1u << i;

		विकल्प (config.बहुलकम्(failure_injection_s::FAILURE_UNIT_SYSTEM_MOTOR, i + 1)) {
		पक्ष बहुलकम्_2::Off:
			masks.failure_आवरणम् |= bit;
			भङ्ग;

		पक्ष बहुलकम्_2::Wrong:
			masks.stop_आवरणम् |= bit;
			भङ्ग;

		स्वभाव:
			भङ्ग;
		}
	}

	प्रत्यागम masks;
}

} // namespace failure_injection

#यदि_अन्तः // CONFIG_MODULES_FAILURE_INJECTION_MANAGER
