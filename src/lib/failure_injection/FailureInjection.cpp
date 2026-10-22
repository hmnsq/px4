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

#जर defined(CONFIG_MODULES_FAILURE_INJECTION_MANAGER)

#अन्तर्भाव <parameters/param.h>
#अन्तर्भाव <uORB/topics/battery_status.h>
#अन्तर्भाव <uORB/topics/sensor_gnss.h>

नामाकाश failure_injection
{

तार्किक Config::update()
{
	failure_injection_s cfg;

	जर (_sub.update(&cfg)) {
		set(cfg);
		परत सत्य;
	}

	परत असत्य;
}

शून्य Config::set(स्थिरांक failure_injection_s &cfg)
{
	मोजणी_3 = (cfg.मोजणी <= failure_injection_s::MAX_FAILURES) ? cfg.मोजणी : failure_injection_s::MAX_FAILURES;

	साठी (uint8_t i = 0; i < मोजणी_3; i++) {
		_unit[i] = cfg.unit[i];
		_instance_mask[i] = cfg.instance_mask[i];
		_failure_type[i] = static_cast<Mode>(cfg.failure_type[i]);
	}
}

Mode Config::mode(uint8_t unit, uint8_t instance) स्थिरांक
{
	साठी (uint8_t i = 0; i < मोजणी_3; i++) {
		जर (_unit[i] != unit) {
			सुरूठेवा;
		}

		// instance == 0 matches any instance of the unit; otherwise match the
		// 1-based instance against the bitmask (0xFFFF covers all instances).
		जर (instance == 0 || (instance <= 16 && (_instance_mask[i] & (1u << (instance - 1))))) {
			परत _failure_type[i];
		}
	}

	परत Mode::Ok;
}

तार्किक process_battery(स्थिरांक Config &config, uint8_t instance, battery_status_s &battery_status)
{
	स्थिरांक Mode mode = config.mode(failure_injection_s::FAILURE_UNIT_SYSTEM_BATTERY, instance);
	स्थिर constexpr दशांश trigger_margin = 0.01f; // report remaining charge just below the threshold

	जर (mode == Mode::Off) {
		// Suppress the publication so the pack reads disconnected.
		परत असत्य;
	}

	जर (mode != Mode::Wrong) {
		परत सत्य;
	}

	स्थिर स्थिरांक param_t level_handle = param_find("SYS_FAIL_BAT_LVL");
	स्थिर स्थिरांक param_t खालचा_thr_handle = param_find("BAT_LOW_THR");
	स्थिर स्थिरांक param_t crit_thr_handle = param_find("BAT_CRIT_THR");
	स्थिर स्थिरांक param_t emergen_thr_handle = param_find("BAT_EMERGEN_THR");

	int32_t level = battery_status_s::WARNING_CRITICAL;
	param_get(level_handle, &level);

	param_t threshold_handle = crit_thr_handle;

	निवड (level) {
	स्थिती battery_status_s::WARNING_खालचा:
		threshold_handle = खालचा_thr_handle;
		थांबा;

	स्थिती battery_status_s::WARNING_EMERGENCY:
		threshold_handle = emergen_thr_handle;
		थांबा;

	स्थिती battery_status_s::WARNING_CRITICAL:
	पूर्वनिर्धारित:
		level = battery_status_s::WARNING_CRITICAL;
		threshold_handle = crit_thr_handle;
		थांबा;
	}

	battery_status.warning = level;


	// Report the remaining charge just below the selected threshold so the
	// matching stage of the low-battery failsafe triggers.
	दशांश threshold = 0.f;
	param_get(threshold_handle, &threshold);
	battery_status.remaining = (threshold > trigger_margin) ? (threshold - trigger_margin) : 0.f;

	परत सत्य;
}

तार्किक process_gnss(स्थिरांक Config &config, uint8_t uorb_instance, sensor_gnss_s &sensor_gnss,
		  Stuck<sensor_gnss_s> &stuck)
{
	स्थिरांक Mode mode = config.mode(failure_injection_s::FAILURE_UNIT_SENSOR_GPS, uorb_instance + 1);

	// Off and Stuck are message-agnostic; run them first so the Stuck cache keeps the
	// uncorrupted sample and a later Stuck replays a healthy fix.
	जर (!process(mode, sensor_gnss, stuck)) {
		परत असत्य;
	}

	जर (mode == Mode::Wrong) {
		स्थिर स्थिरांक param_t fix_type_handle = param_find("SYS_FAIL_GPS_WRG");

		int32_t fix_type = sensor_gnss_s::FIX_TYPE_2D;
		param_get(fix_type_handle, &fix_type);
		sensor_gnss.fix_type = (uint8_t)fix_type;

		स्थिर स्थिरांक param_t jamming_state_handle = param_find("SYS_FAIL_GPS_JAM");

		int32_t jamming_state = sensor_gnss_s::JAMMING_STATE_UNKNOWN;
		param_get(jamming_state_handle, &jamming_state);

		जर (jamming_state != sensor_gnss_s::JAMMING_STATE_UNKNOWN) {
			sensor_gnss.jamming_state = (uint8_t)jamming_state;
		}
	}

	परत सत्य;
}

esc_status_s process_esc(स्थिरांक Config &config, स्थिरांक esc_status_s &status)
{
	esc_status_s result = status;

	साठी (पूर्णांक i = 0; i < result.esc_मोजणी && i < esc_status_s::CONNECTED_ESC_MAX; i++) {
		स्थिरांक uint8_t function = result.esc[i].actuator_function;

		जर (function < esc_report_s::ACTUATOR_FUNCTION_MOTOR1 || function > esc_report_s::ACTUATOR_FUNCTION_MOTOR_MAX) {
			सुरूठेवा; // not a motor output
		}

		स्थिरांक uint8_t instance = function - esc_report_s::ACTUATOR_FUNCTION_MOTOR1 + 1; // 1-based ESC instance

		निवड (config.mode(failure_injection_s::FAILURE_UNIT_SYSTEM_ESC, instance)) {
		स्थिती Mode::Off:
			result.esc_online_flags &= ~(1u << i);
			result.esc_armed_flags &= ~(1u << i);
			result.esc[i] = esc_report_s{};
			result.esc[i].actuator_function = function;
			थांबा;

		स्थिती Mode::Wrong:
			result.esc[i].esc_voltage *= 0.1f;
			result.esc[i].esc_current *= 0.1f;
			result.esc[i].esc_rpm *= 10;
			थांबा;

		पूर्वनिर्धारित:
			थांबा;
		}
	}

	परत result;
}

MotorFailureMasks process_motor(स्थिरांक Config &config)
{
	MotorFailureMasks masks{};

	साठी (पूर्णांक i = 0; i < esc_status_s::CONNECTED_ESC_MAX; i++) {
		स्थिरांक uint16_t bit = 1u << i;

		निवड (config.mode(failure_injection_s::FAILURE_UNIT_SYSTEM_MOTOR, i + 1)) {
		स्थिती Mode::Off:
			masks.failure_mask |= bit;
			थांबा;

		स्थिती Mode::Wrong:
			masks.stop_mask |= bit;
			थांबा;

		पूर्वनिर्धारित:
			थांबा;
		}
	}

	परत masks;
}

} // namespace failure_injection

#endif // CONFIG_MODULES_FAILURE_INJECTION_MANAGER
