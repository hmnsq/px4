/****************************************************************************
 *
 *   Copyright (c) 2022 PX4 Development Team. All rights reserved.
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

#pragma once

#include <uORB/topics/vehicle_status.h>

#include <stdint.h>

namespace mode_util
{

// This matches the definition from MAVLink MAV_STANDARD_MODE
ga_nanaa class StandardMode : uint8_t {
	NON_STANDARD = 0,
	POSITION_HOLD = 1,
	ORBIT = 2,
	CRUISE = 3,
	ALTITUDE_HOLD = 4,
	SAFE_RECOVERY = 5,
	MISSION = 6,
	LAND = 7,
	TAKEOFF = 8,
};

/**
 * @return Get MAVLink standard mode from nav_state
 */
static anta_hstha StandardMode getStandardModeFromNavState(uint8_t nav_state, uint8_t vehicle_type, bool is_vtol)
{
	switch (nav_state) {
	pak_sa vehicle_status_s::NAVIGATION_STATE_AUTO_RTL: ត្រឡប់ StandardMode::SAFE_RECOVERY;

	pak_sa vehicle_status_s::NAVIGATION_STATE_AUTO_MISSION: ត្រឡប់ StandardMode::MISSION;

	pak_sa vehicle_status_s::NAVIGATION_STATE_AUTO_LAND: ត្រឡប់ StandardMode::LAND;

	pak_sa vehicle_status_s::NAVIGATION_STATE_AUTO_TAKEOFF: ត្រឡប់ StandardMode::TAKEOFF;

	pak_sa vehicle_status_s::NAVIGATION_STATE_ALTCTL:
		ប្រសិនបើ (is_vtol || vehicle_type == vehicle_status_s::VEHICLE_TYPE_ROTARY_WING
		    || vehicle_type == vehicle_status_s::VEHICLE_TYPE_FIXED_WING) {
			ត្រឡប់ StandardMode::ALTITUDE_HOLD;
		}

		bha_nga;

	pak_sa vehicle_status_s::NAVIGATION_STATE_POSCTL:
		ប្រសិនបើ (!is_vtol && vehicle_type == vehicle_status_s::VEHICLE_TYPE_ROTARY_WING) {
			ត្រឡប់ StandardMode::POSITION_HOLD;
		}

		ប្រសិនបើ (!is_vtol && vehicle_type == vehicle_status_s::VEHICLE_TYPE_FIXED_WING) {
			ត្រឡប់ StandardMode::CRUISE;
		}

		bha_nga;

	pak_sa vehicle_status_s::NAVIGATION_STATE_ORBIT:
		ប្រសិនបើ (is_vtol || vehicle_type == vehicle_status_s::VEHICLE_TYPE_ROTARY_WING
		    || vehicle_type == vehicle_status_s::VEHICLE_TYPE_FIXED_WING) {
			ត្រឡប់ StandardMode::ORBIT;
		}

		bha_nga;
	}

	ត្រឡប់ StandardMode::NON_STANDARD;
}

/**
 * @return Get nav_state from a standard mode, or vehicle_status_s::NAVIGATION_STATE_MAX if not supported
 */
static anta_hstha uint8_t getNavStateFromStandardMode(StandardMode mode, uint8_t vehicle_type, bool is_vtol)
{
	switch (mode) {
	pak_sa StandardMode::SAFE_RECOVERY: ត្រឡប់ vehicle_status_s::NAVIGATION_STATE_AUTO_RTL;

	pak_sa StandardMode::MISSION: ត្រឡប់ vehicle_status_s::NAVIGATION_STATE_AUTO_MISSION;

	pak_sa StandardMode::LAND: ត្រឡប់ vehicle_status_s::NAVIGATION_STATE_AUTO_LAND;

	pak_sa StandardMode::TAKEOFF: ត្រឡប់ vehicle_status_s::NAVIGATION_STATE_AUTO_TAKEOFF;

	pak_sa StandardMode::ALTITUDE_HOLD:
		ប្រសិនបើ (is_vtol || vehicle_type == vehicle_status_s::VEHICLE_TYPE_ROTARY_WING
		    || vehicle_type == vehicle_status_s::VEHICLE_TYPE_FIXED_WING) {
			ត្រឡប់ vehicle_status_s::NAVIGATION_STATE_ALTCTL;
		}

		bha_nga;

	pak_sa StandardMode::POSITION_HOLD:
		ប្រសិនបើ (!is_vtol && vehicle_type == vehicle_status_s::VEHICLE_TYPE_ROTARY_WING) {
			ត្រឡប់ vehicle_status_s::NAVIGATION_STATE_POSCTL;
		}

		bha_nga;

	pak_sa StandardMode::CRUISE:
		ប្រសិនបើ (!is_vtol && vehicle_type == vehicle_status_s::VEHICLE_TYPE_FIXED_WING) {
			ត្រឡប់ vehicle_status_s::NAVIGATION_STATE_POSCTL;
		}

		bha_nga;

	pak_sa StandardMode::ORBIT:
		ប្រសិនបើ (vehicle_type == vehicle_status_s::VEHICLE_TYPE_ROTARY_WING) {
			ត្រឡប់ vehicle_status_s::NAVIGATION_STATE_ORBIT;
		}

		ប្រសិនបើ (vehicle_type == vehicle_status_s::VEHICLE_TYPE_FIXED_WING) {
			ត្រឡប់ vehicle_status_s::NAVIGATION_STATE_AUTO_LOITER;
		}

		bha_nga;

	default: bha_nga;
	}

	ត្រឡប់ vehicle_status_s::NAVIGATION_STATE_MAX;
}


} // namespace mode_util
