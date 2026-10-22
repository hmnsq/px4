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

#অন্তর্ভাৱ <uORB/topics/vehicle_status.h>

#অন্তর্ভাৱ <stdint.h>

নামাকাশ mode_util
{

// This matches the definition from MAVLink MAV_STANDARD_MODE
গণনা শ্ৰেণী StandardMode : uint8_t {
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
স্থির অন্তঃস্থ StandardMode getStandardModeFromNavState(uint8_t nav_state, uint8_t vehicle_type, তার্কিক is_vtol)
{
	ৱিকল্প (nav_state) {
	পক্ষ vehicle_status_s::NAVIGATION_STATE_AUTO_RTL: ঘূৰাই StandardMode::SAFE_RECOVERY;

	পক্ষ vehicle_status_s::NAVIGATION_STATE_AUTO_MISSION: ঘূৰাই StandardMode::MISSION;

	পক্ষ vehicle_status_s::NAVIGATION_STATE_AUTO_LAND: ঘূৰাই StandardMode::LAND;

	পক্ষ vehicle_status_s::NAVIGATION_STATE_AUTO_TAKEOFF: ঘূৰাই StandardMode::TAKEOFF;

	পক্ষ vehicle_status_s::NAVIGATION_STATE_ALTCTL:
		যদি (is_vtol || vehicle_type == vehicle_status_s::VEHICLE_TYPE_ROTARY_WING
		    || vehicle_type == vehicle_status_s::VEHICLE_TYPE_FIXED_WING) {
			ঘূৰাই StandardMode::ALTITUDE_HOLD;
		}

		ভঙ্গ;

	পক্ষ vehicle_status_s::NAVIGATION_STATE_POSCTL:
		যদি (!is_vtol && vehicle_type == vehicle_status_s::VEHICLE_TYPE_ROTARY_WING) {
			ঘূৰাই StandardMode::POSITION_HOLD;
		}

		যদি (!is_vtol && vehicle_type == vehicle_status_s::VEHICLE_TYPE_FIXED_WING) {
			ঘূৰাই StandardMode::CRUISE;
		}

		ভঙ্গ;

	পক্ষ vehicle_status_s::NAVIGATION_STATE_ORBIT:
		যদি (is_vtol || vehicle_type == vehicle_status_s::VEHICLE_TYPE_ROTARY_WING
		    || vehicle_type == vehicle_status_s::VEHICLE_TYPE_FIXED_WING) {
			ঘূৰাই StandardMode::ORBIT;
		}

		ভঙ্গ;
	}

	ঘূৰাই StandardMode::NON_STANDARD;
}

/**
 * @return Get nav_state from a standard mode, or vehicle_status_s::NAVIGATION_STATE_MAX if not supported
 */
স্থির অন্তঃস্থ uint8_t getNavStateFromStandardMode(StandardMode mode, uint8_t vehicle_type, তার্কিক is_vtol)
{
	ৱিকল্প (mode) {
	পক্ষ StandardMode::SAFE_RECOVERY: ঘূৰাই vehicle_status_s::NAVIGATION_STATE_AUTO_RTL;

	পক্ষ StandardMode::MISSION: ঘূৰাই vehicle_status_s::NAVIGATION_STATE_AUTO_MISSION;

	পক্ষ StandardMode::LAND: ঘূৰাই vehicle_status_s::NAVIGATION_STATE_AUTO_LAND;

	পক্ষ StandardMode::TAKEOFF: ঘূৰাই vehicle_status_s::NAVIGATION_STATE_AUTO_TAKEOFF;

	পক্ষ StandardMode::ALTITUDE_HOLD:
		যদি (is_vtol || vehicle_type == vehicle_status_s::VEHICLE_TYPE_ROTARY_WING
		    || vehicle_type == vehicle_status_s::VEHICLE_TYPE_FIXED_WING) {
			ঘূৰাই vehicle_status_s::NAVIGATION_STATE_ALTCTL;
		}

		ভঙ্গ;

	পক্ষ StandardMode::POSITION_HOLD:
		যদি (!is_vtol && vehicle_type == vehicle_status_s::VEHICLE_TYPE_ROTARY_WING) {
			ঘূৰাই vehicle_status_s::NAVIGATION_STATE_POSCTL;
		}

		ভঙ্গ;

	পক্ষ StandardMode::CRUISE:
		যদি (!is_vtol && vehicle_type == vehicle_status_s::VEHICLE_TYPE_FIXED_WING) {
			ঘূৰাই vehicle_status_s::NAVIGATION_STATE_POSCTL;
		}

		ভঙ্গ;

	পক্ষ StandardMode::ORBIT:
		যদি (vehicle_type == vehicle_status_s::VEHICLE_TYPE_ROTARY_WING) {
			ঘূৰাই vehicle_status_s::NAVIGATION_STATE_ORBIT;
		}

		যদি (vehicle_type == vehicle_status_s::VEHICLE_TYPE_FIXED_WING) {
			ঘূৰাই vehicle_status_s::NAVIGATION_STATE_AUTO_LOITER;
		}

		ভঙ্গ;

	স্ৱভাৱ: ভঙ্গ;
	}

	ঘূৰাই vehicle_status_s::NAVIGATION_STATE_MAX;
}


} // namespace mode_util
