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

#அந்தர்பாவ "FailureInjection.hpp"

#எனில் defined(CONFIG_MODULES_FAILURE_INJECTION_MANAGER)

#அந்தர்பாவ <parameters/param.h>
#அந்தர்பாவ <uORB/topics/battery_status.h>
#அந்தர்பாவ <uORB/topics/sensor_gnss.h>

நாமாகாஶ failure_injection
{

தருக்க Config::update()
{
	failure_injection_s cfg;

	எனில் (_sub.update(&cfg)) {
		set(cfg);
		திரும்பு உண்மை;
	}

	திரும்பு பொய்;
}

வெறுமை Config::set(மாறிலி failure_injection_s &cfg)
{
	மொத்தம்_3 = (cfg.மொத்தம் <= failure_injection_s::MAX_FAILURES) ? cfg.மொத்தம் : failure_injection_s::MAX_FAILURES;

	க்கு (uint8_t i = 0; i < மொத்தம்_3; i++) {
		_unit[i] = cfg.unit[i];
		_instance_mask[i] = cfg.instance_mask[i];
		_failure_type[i] = static_cast<Mode>(cfg.failure_type[i]);
	}
}

Mode Config::mode(uint8_t unit, uint8_t instance) மாறிலி
{
	க்கு (uint8_t i = 0; i < மொத்தம்_3; i++) {
		எனில் (_unit[i] != unit) {
			தொடர்;
		}

		// instance == 0 matches any instance of the unit; otherwise match the
		// 1-based instance against the bitmask (0xFFFF covers all instances).
		எனில் (instance == 0 || (instance <= 16 && (_instance_mask[i] & (1u << (instance - 1))))) {
			திரும்பு _failure_type[i];
		}
	}

	திரும்பு Mode::Ok;
}

தருக்க process_battery(மாறிலி Config &config, uint8_t instance, battery_status_s &battery_status)
{
	மாறிலி Mode mode = config.mode(failure_injection_s::FAILURE_UNIT_SYSTEM_BATTERY, instance);
	நிலையான constexpr தசம trigger_margin = 0.01f; // report remaining charge just below the threshold

	எனில் (mode == Mode::Off) {
		// Suppress the publication so the pack reads disconnected.
		திரும்பு பொய்;
	}

	எனில் (mode != Mode::Wrong) {
		திரும்பு உண்மை;
	}

	நிலையான மாறிலி param_t level_handle = param_find("SYS_FAIL_BAT_LVL");
	நிலையான மாறிலி param_t கீழ்_thr_handle = param_find("BAT_LOW_THR");
	நிலையான மாறிலி param_t crit_thr_handle = param_find("BAT_CRIT_THR");
	நிலையான மாறிலி param_t emergen_thr_handle = param_find("BAT_EMERGEN_THR");

	int32_t level = battery_status_s::WARNING_CRITICAL;
	param_get(level_handle, &level);

	param_t threshold_handle = crit_thr_handle;

	தேர்வு (level) {
	நிலை battery_status_s::WARNING_கீழ்:
		threshold_handle = கீழ்_thr_handle;
		நிறுத்து;

	நிலை battery_status_s::WARNING_EMERGENCY:
		threshold_handle = emergen_thr_handle;
		நிறுத்து;

	நிலை battery_status_s::WARNING_CRITICAL:
	இயல்பு:
		level = battery_status_s::WARNING_CRITICAL;
		threshold_handle = crit_thr_handle;
		நிறுத்து;
	}

	battery_status.warning = level;


	// Report the remaining charge just below the selected threshold so the
	// matching stage of the low-battery failsafe triggers.
	தசம threshold = 0.f;
	param_get(threshold_handle, &threshold);
	battery_status.remaining = (threshold > trigger_margin) ? (threshold - trigger_margin) : 0.f;

	திரும்பு உண்மை;
}

தருக்க process_gnss(மாறிலி Config &config, uint8_t uorb_instance, sensor_gnss_s &sensor_gnss,
		  Stuck<sensor_gnss_s> &stuck)
{
	மாறிலி Mode mode = config.mode(failure_injection_s::FAILURE_UNIT_SENSOR_GPS, uorb_instance + 1);

	// Off and Stuck are message-agnostic; run them first so the Stuck cache keeps the
	// uncorrupted sample and a later Stuck replays a healthy fix.
	எனில் (!process(mode, sensor_gnss, stuck)) {
		திரும்பு பொய்;
	}

	எனில் (mode == Mode::Wrong) {
		நிலையான மாறிலி param_t fix_type_handle = param_find("SYS_FAIL_GPS_WRG");

		int32_t fix_type = sensor_gnss_s::FIX_TYPE_2D;
		param_get(fix_type_handle, &fix_type);
		sensor_gnss.fix_type = (uint8_t)fix_type;

		நிலையான மாறிலி param_t jamming_state_handle = param_find("SYS_FAIL_GPS_JAM");

		int32_t jamming_state = sensor_gnss_s::JAMMING_STATE_UNKNOWN;
		param_get(jamming_state_handle, &jamming_state);

		எனில் (jamming_state != sensor_gnss_s::JAMMING_STATE_UNKNOWN) {
			sensor_gnss.jamming_state = (uint8_t)jamming_state;
		}
	}

	திரும்பு உண்மை;
}

esc_status_s process_esc(மாறிலி Config &config, மாறிலி esc_status_s &status)
{
	esc_status_s result = status;

	க்கு (முழு i = 0; i < result.esc_மொத்தம் && i < esc_status_s::CONNECTED_ESC_MAX; i++) {
		மாறிலி uint8_t function = result.esc[i].actuator_function;

		எனில் (function < esc_report_s::ACTUATOR_FUNCTION_MOTOR1 || function > esc_report_s::ACTUATOR_FUNCTION_MOTOR_MAX) {
			தொடர்; // not a motor output
		}

		மாறிலி uint8_t instance = function - esc_report_s::ACTUATOR_FUNCTION_MOTOR1 + 1; // 1-based ESC instance

		தேர்வு (config.mode(failure_injection_s::FAILURE_UNIT_SYSTEM_ESC, instance)) {
		நிலை Mode::Off:
			result.esc_online_flags &= ~(1u << i);
			result.esc_armed_flags &= ~(1u << i);
			result.esc[i] = esc_report_s{};
			result.esc[i].actuator_function = function;
			நிறுத்து;

		நிலை Mode::Wrong:
			result.esc[i].esc_voltage *= 0.1f;
			result.esc[i].esc_current *= 0.1f;
			result.esc[i].esc_rpm *= 10;
			நிறுத்து;

		இயல்பு:
			நிறுத்து;
		}
	}

	திரும்பு result;
}

MotorFailureMasks process_motor(மாறிலி Config &config)
{
	MotorFailureMasks masks{};

	க்கு (முழு i = 0; i < esc_status_s::CONNECTED_ESC_MAX; i++) {
		மாறிலி uint16_t bit = 1u << i;

		தேர்வு (config.mode(failure_injection_s::FAILURE_UNIT_SYSTEM_MOTOR, i + 1)) {
		நிலை Mode::Off:
			masks.failure_mask |= bit;
			நிறுத்து;

		நிலை Mode::Wrong:
			masks.stop_mask |= bit;
			நிறுத்து;

		இயல்பு:
			நிறுத்து;
		}
	}

	திரும்பு masks;
}

} // namespace failure_injection

#endif // CONFIG_MODULES_FAILURE_INJECTION_MANAGER
