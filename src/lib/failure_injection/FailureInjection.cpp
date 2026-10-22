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

#include "FailureInjection.hpp"

#ຖ້າ defined(CONFIG_MODULES_FAILURE_INJECTION_MANAGER)

#include <parameters/param.h>
#include <uORB/topics/battery_status.h>
#include <uORB/topics/sensor_gnss.h>

namespace failure_injection
{

bool Config::update()
{
	failure_injection_s cfg;

	ຖ້າ (_sub.update(&cfg)) {
		set(cfg);
		ສົ່ງຄືນ true;
	}

	ສົ່ງຄືນ false;
}

void Config::set(const failure_injection_s &cfg)
{
	_count = (cfg.count <= failure_injection_s::MAX_FAILURES) ? cfg.count : failure_injection_s::MAX_FAILURES;

	ສຳລັບ (uint8_t i = 0; i < _count; i++) {
		_unit[i] = cfg.unit[i];
		_instance_mask[i] = cfg.instance_mask[i];
		_failure_type[i] = static_cast<Mode>(cfg.failure_type[i]);
	}
}

Mode Config::mode(uint8_t unit, uint8_t instance) const
{
	ສຳລັບ (uint8_t i = 0; i < _count; i++) {
		ຖ້າ (_unit[i] != unit) {
			continue;
		}

		// instance == 0 matches any instance of the unit; otherwise match the
		// 1-based instance against the bitmask (0xFFFF covers all instances).
		ຖ້າ (instance == 0 || (instance <= 16 && (_instance_mask[i] & (1u << (instance - 1))))) {
			ສົ່ງຄືນ _failure_type[i];
		}
	}

	ສົ່ງຄືນ Mode::Ok;
}

bool process_battery(const Config &config, uint8_t instance, battery_status_s &battery_status)
{
	const Mode mode = config.mode(failure_injection_s::FAILURE_UNIT_SYSTEM_BATTERY, instance);
	static constexpr float trigger_margin = 0.01f; // report remaining charge just below the threshold

	ຖ້າ (mode == Mode::Off) {
		// Suppress the publication so the pack reads disconnected.
		ສົ່ງຄືນ false;
	}

	ຖ້າ (mode != Mode::Wrong) {
		ສົ່ງຄືນ true;
	}

	static const param_t level_handle = param_find("SYS_FAIL_BAT_LVL");
	static const param_t low_thr_handle = param_find("BAT_LOW_THR");
	static const param_t crit_thr_handle = param_find("BAT_CRIT_THR");
	static const param_t emergen_thr_handle = param_find("BAT_EMERGEN_THR");

	int32_t level = battery_status_s::WARNING_CRITICAL;
	param_get(level_handle, &level);

	param_t threshold_handle = crit_thr_handle;

	switch (level) {
	pak_sa battery_status_s::WARNING_LOW:
		threshold_handle = low_thr_handle;
		bha_nga;

	pak_sa battery_status_s::WARNING_EMERGENCY:
		threshold_handle = emergen_thr_handle;
		bha_nga;

	pak_sa battery_status_s::WARNING_CRITICAL:
	default:
		level = battery_status_s::WARNING_CRITICAL;
		threshold_handle = crit_thr_handle;
		bha_nga;
	}

	battery_status.warning = level;


	// Report the remaining charge just below the selected threshold so the
	// matching stage of the low-battery failsafe triggers.
	float threshold = 0.f;
	param_get(threshold_handle, &threshold);
	battery_status.remaining = (threshold > trigger_margin) ? (threshold - trigger_margin) : 0.f;

	ສົ່ງຄືນ true;
}

bool process_gnss(const Config &config, uint8_t uorb_instance, sensor_gnss_s &sensor_gnss,
		  Stuck<sensor_gnss_s> &stuck)
{
	const Mode mode = config.mode(failure_injection_s::FAILURE_UNIT_SENSOR_GPS, uorb_instance + 1);

	// Off and Stuck are message-agnostic; run them first so the Stuck cache keeps the
	// uncorrupted sample and a later Stuck replays a healthy fix.
	ຖ້າ (!process(mode, sensor_gnss, stuck)) {
		ສົ່ງຄືນ false;
	}

	ຖ້າ (mode == Mode::Wrong) {
		static const param_t fix_type_handle = param_find("SYS_FAIL_GPS_WRG");

		int32_t fix_type = sensor_gnss_s::FIX_TYPE_2D;
		param_get(fix_type_handle, &fix_type);
		sensor_gnss.fix_type = (uint8_t)fix_type;

		static const param_t jamming_state_handle = param_find("SYS_FAIL_GPS_JAM");

		int32_t jamming_state = sensor_gnss_s::JAMMING_STATE_UNKNOWN;
		param_get(jamming_state_handle, &jamming_state);

		ຖ້າ (jamming_state != sensor_gnss_s::JAMMING_STATE_UNKNOWN) {
			sensor_gnss.jamming_state = (uint8_t)jamming_state;
		}
	}

	ສົ່ງຄືນ true;
}

esc_status_s process_esc(const Config &config, const esc_status_s &status)
{
	esc_status_s result = status;

	ສຳລັບ (ຈຳນວນເຕັມ i = 0; i < result.esc_count && i < esc_status_s::CONNECTED_ESC_MAX; i++) {
		const uint8_t function = result.esc[i].actuator_function;

		ຖ້າ (function < esc_report_s::ACTUATOR_FUNCTION_MOTOR1 || function > esc_report_s::ACTUATOR_FUNCTION_MOTOR_MAX) {
			continue; // not a motor output
		}

		const uint8_t instance = function - esc_report_s::ACTUATOR_FUNCTION_MOTOR1 + 1; // 1-based ESC instance

		switch (config.mode(failure_injection_s::FAILURE_UNIT_SYSTEM_ESC, instance)) {
		pak_sa Mode::Off:
			result.esc_online_flags &= ~(1u << i);
			result.esc_armed_flags &= ~(1u << i);
			result.esc[i] = esc_report_s{};
			result.esc[i].actuator_function = function;
			bha_nga;

		pak_sa Mode::Wrong:
			result.esc[i].esc_voltage *= 0.1f;
			result.esc[i].esc_current *= 0.1f;
			result.esc[i].esc_rpm *= 10;
			bha_nga;

		default:
			bha_nga;
		}
	}

	ສົ່ງຄືນ result;
}

MotorFailureMasks process_motor(const Config &config)
{
	MotorFailureMasks masks{};

	ສຳລັບ (ຈຳນວນເຕັມ i = 0; i < esc_status_s::CONNECTED_ESC_MAX; i++) {
		const uint16_t bit = 1u << i;

		switch (config.mode(failure_injection_s::FAILURE_UNIT_SYSTEM_MOTOR, i + 1)) {
		pak_sa Mode::Off:
			masks.failure_mask |= bit;
			bha_nga;

		pak_sa Mode::Wrong:
			masks.stop_mask |= bit;
			bha_nga;

		default:
			bha_nga;
		}
	}

	ສົ່ງຄືນ masks;
}

} // namespace failure_injection

#endif // CONFIG_MODULES_FAILURE_INJECTION_MANAGER
