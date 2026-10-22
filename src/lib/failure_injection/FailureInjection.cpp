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

#અન્તર્ભાવ "FailureInjection.hpp"

#જો defined(CONFIG_MODULES_FAILURE_INJECTION_MANAGER)

#અન્તર્ભાવ <parameters/param.h>
#અન્તર્ભાવ <uORB/topics/battery_status.h>
#અન્તર્ભાવ <uORB/topics/sensor_gnss.h>

નામાકાશ failure_injection
{

તાર્કિક Config::update()
{
	failure_injection_s cfg;

	જો (_sub.update(&cfg)) {
		set(cfg);
		પાછા સાચું;
	}

	પાછા ખોટું;
}

શૂન્ય Config::set(અચલ failure_injection_s &cfg)
{
	ગણતરી_3 = (cfg.ગણતરી <= failure_injection_s::MAX_FAILURES) ? cfg.ગણતરી : failure_injection_s::MAX_FAILURES;

	માટે (uint8_t i = 0; i < ગણતરી_3; i++) {
		_unit[i] = cfg.unit[i];
		_instance_mask[i] = cfg.instance_mask[i];
		_failure_type[i] = static_cast<Mode>(cfg.failure_type[i]);
	}
}

Mode Config::mode(uint8_t unit, uint8_t instance) અચલ
{
	માટે (uint8_t i = 0; i < ગણતરી_3; i++) {
		જો (_unit[i] != unit) {
			ચાલુ;
		}

		// instance == 0 matches any instance of the unit; otherwise match the
		// 1-based instance against the bitmask (0xFFFF covers all instances).
		જો (instance == 0 || (instance <= 16 && (_instance_mask[i] & (1u << (instance - 1))))) {
			પાછા _failure_type[i];
		}
	}

	પાછા Mode::Ok;
}

તાર્કિક process_battery(અચલ Config &config, uint8_t instance, battery_status_s &battery_status)
{
	અચલ Mode mode = config.mode(failure_injection_s::FAILURE_UNIT_SYSTEM_BATTERY, instance);
	સ્થિર constexpr દશાંશ trigger_margin = 0.01f; // report remaining charge just below the threshold

	જો (mode == Mode::Off) {
		// Suppress the publication so the pack reads disconnected.
		પાછા ખોટું;
	}

	જો (mode != Mode::Wrong) {
		પાછા સાચું;
	}

	સ્થિર અચલ param_t level_handle = param_find("SYS_FAIL_BAT_LVL");
	સ્થિર અચલ param_t નીચલું_thr_handle = param_find("BAT_LOW_THR");
	સ્થિર અચલ param_t crit_thr_handle = param_find("BAT_CRIT_THR");
	સ્થિર અચલ param_t emergen_thr_handle = param_find("BAT_EMERGEN_THR");

	int32_t level = battery_status_s::WARNING_CRITICAL;
	param_get(level_handle, &level);

	param_t threshold_handle = crit_thr_handle;

	પસંદગી (level) {
	સ્થિતિ battery_status_s::WARNING_નીચલું:
		threshold_handle = નીચલું_thr_handle;
		તોડો;

	સ્થિતિ battery_status_s::WARNING_EMERGENCY:
		threshold_handle = emergen_thr_handle;
		તોડો;

	સ્થિતિ battery_status_s::WARNING_CRITICAL:
	મૂળભૂત:
		level = battery_status_s::WARNING_CRITICAL;
		threshold_handle = crit_thr_handle;
		તોડો;
	}

	battery_status.warning = level;


	// Report the remaining charge just below the selected threshold so the
	// matching stage of the low-battery failsafe triggers.
	દશાંશ threshold = 0.f;
	param_get(threshold_handle, &threshold);
	battery_status.remaining = (threshold > trigger_margin) ? (threshold - trigger_margin) : 0.f;

	પાછા સાચું;
}

તાર્કિક process_gnss(અચલ Config &config, uint8_t uorb_instance, sensor_gnss_s &sensor_gnss,
		  Stuck<sensor_gnss_s> &stuck)
{
	અચલ Mode mode = config.mode(failure_injection_s::FAILURE_UNIT_SENSOR_GPS, uorb_instance + 1);

	// Off and Stuck are message-agnostic; run them first so the Stuck cache keeps the
	// uncorrupted sample and a later Stuck replays a healthy fix.
	જો (!process(mode, sensor_gnss, stuck)) {
		પાછા ખોટું;
	}

	જો (mode == Mode::Wrong) {
		સ્થિર અચલ param_t fix_type_handle = param_find("SYS_FAIL_GPS_WRG");

		int32_t fix_type = sensor_gnss_s::FIX_TYPE_2D;
		param_get(fix_type_handle, &fix_type);
		sensor_gnss.fix_type = (uint8_t)fix_type;

		સ્થિર અચલ param_t jamming_state_handle = param_find("SYS_FAIL_GPS_JAM");

		int32_t jamming_state = sensor_gnss_s::JAMMING_STATE_UNKNOWN;
		param_get(jamming_state_handle, &jamming_state);

		જો (jamming_state != sensor_gnss_s::JAMMING_STATE_UNKNOWN) {
			sensor_gnss.jamming_state = (uint8_t)jamming_state;
		}
	}

	પાછા સાચું;
}

esc_status_s process_esc(અચલ Config &config, અચલ esc_status_s &status)
{
	esc_status_s result = status;

	માટે (પૂર્ણાંક i = 0; i < result.esc_ગણતરી && i < esc_status_s::CONNECTED_ESC_MAX; i++) {
		અચલ uint8_t function = result.esc[i].actuator_function;

		જો (function < esc_report_s::ACTUATOR_FUNCTION_MOTOR1 || function > esc_report_s::ACTUATOR_FUNCTION_MOTOR_MAX) {
			ચાલુ; // not a motor output
		}

		અચલ uint8_t instance = function - esc_report_s::ACTUATOR_FUNCTION_MOTOR1 + 1; // 1-based ESC instance

		પસંદગી (config.mode(failure_injection_s::FAILURE_UNIT_SYSTEM_ESC, instance)) {
		સ્થિતિ Mode::Off:
			result.esc_online_flags &= ~(1u << i);
			result.esc_armed_flags &= ~(1u << i);
			result.esc[i] = esc_report_s{};
			result.esc[i].actuator_function = function;
			તોડો;

		સ્થિતિ Mode::Wrong:
			result.esc[i].esc_voltage *= 0.1f;
			result.esc[i].esc_current *= 0.1f;
			result.esc[i].esc_rpm *= 10;
			તોડો;

		મૂળભૂત:
			તોડો;
		}
	}

	પાછા result;
}

MotorFailureMasks process_motor(અચલ Config &config)
{
	MotorFailureMasks masks{};

	માટે (પૂર્ણાંક i = 0; i < esc_status_s::CONNECTED_ESC_MAX; i++) {
		અચલ uint16_t bit = 1u << i;

		પસંદગી (config.mode(failure_injection_s::FAILURE_UNIT_SYSTEM_MOTOR, i + 1)) {
		સ્થિતિ Mode::Off:
			masks.failure_mask |= bit;
			તોડો;

		સ્થિતિ Mode::Wrong:
			masks.stop_mask |= bit;
			તોડો;

		મૂળભૂત:
			તોડો;
		}
	}

	પાછા masks;
}

} // namespace failure_injection

#endif // CONFIG_MODULES_FAILURE_INJECTION_MANAGER
