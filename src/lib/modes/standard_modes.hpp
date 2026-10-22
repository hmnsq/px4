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

#അന്തര്ഭാവ <uORB/topics/vehicle_status.h>

#അന്തര്ഭാവ <stdint.h>

നാമാകാശ mode_util
{

// This matches the definition from MAVLink MAV_STANDARD_MODE
എണ്ണൽ വർഗ്ഗം StandardMode : uint8_t {
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
സ്ഥിതം വരിയിൽ StandardMode getStandardModeFromNavState(uint8_t nav_state, uint8_t vehicle_type, തർക്കം is_vtol)
{
	തിരഞ്ഞെടുപ്പ് (nav_state) {
	അവസ്ഥ vehicle_status_s::NAVIGATION_STATE_AUTO_RTL: തിരികെ StandardMode::SAFE_RECOVERY;

	അവസ്ഥ vehicle_status_s::NAVIGATION_STATE_AUTO_MISSION: തിരികെ StandardMode::MISSION;

	അവസ്ഥ vehicle_status_s::NAVIGATION_STATE_AUTO_LAND: തിരികെ StandardMode::LAND;

	അവസ്ഥ vehicle_status_s::NAVIGATION_STATE_AUTO_TAKEOFF: തിരികെ StandardMode::TAKEOFF;

	അവസ്ഥ vehicle_status_s::NAVIGATION_STATE_ALTCTL:
		എങ്കിൽ (is_vtol || vehicle_type == vehicle_status_s::VEHICLE_TYPE_ROTARY_WING
		    || vehicle_type == vehicle_status_s::VEHICLE_TYPE_FIXED_WING) {
			തിരികെ StandardMode::ALTITUDE_HOLD;
		}

		നിർത്തുക;

	അവസ്ഥ vehicle_status_s::NAVIGATION_STATE_POSCTL:
		എങ്കിൽ (!is_vtol && vehicle_type == vehicle_status_s::VEHICLE_TYPE_ROTARY_WING) {
			തിരികെ StandardMode::POSITION_HOLD;
		}

		എങ്കിൽ (!is_vtol && vehicle_type == vehicle_status_s::VEHICLE_TYPE_FIXED_WING) {
			തിരികെ StandardMode::CRUISE;
		}

		നിർത്തുക;

	അവസ്ഥ vehicle_status_s::NAVIGATION_STATE_ORBIT:
		എങ്കിൽ (is_vtol || vehicle_type == vehicle_status_s::VEHICLE_TYPE_ROTARY_WING
		    || vehicle_type == vehicle_status_s::VEHICLE_TYPE_FIXED_WING) {
			തിരികെ StandardMode::ORBIT;
		}

		നിർത്തുക;
	}

	തിരികെ StandardMode::NON_STANDARD;
}

/**
 * @return Get nav_state from a standard mode, or vehicle_status_s::NAVIGATION_STATE_MAX if not supported
 */
സ്ഥിതം വരിയിൽ uint8_t getNavStateFromStandardMode(StandardMode mode, uint8_t vehicle_type, തർക്കം is_vtol)
{
	തിരഞ്ഞെടുപ്പ് (mode) {
	അവസ്ഥ StandardMode::SAFE_RECOVERY: തിരികെ vehicle_status_s::NAVIGATION_STATE_AUTO_RTL;

	അവസ്ഥ StandardMode::MISSION: തിരികെ vehicle_status_s::NAVIGATION_STATE_AUTO_MISSION;

	അവസ്ഥ StandardMode::LAND: തിരികെ vehicle_status_s::NAVIGATION_STATE_AUTO_LAND;

	അവസ്ഥ StandardMode::TAKEOFF: തിരികെ vehicle_status_s::NAVIGATION_STATE_AUTO_TAKEOFF;

	അവസ്ഥ StandardMode::ALTITUDE_HOLD:
		എങ്കിൽ (is_vtol || vehicle_type == vehicle_status_s::VEHICLE_TYPE_ROTARY_WING
		    || vehicle_type == vehicle_status_s::VEHICLE_TYPE_FIXED_WING) {
			തിരികെ vehicle_status_s::NAVIGATION_STATE_ALTCTL;
		}

		നിർത്തുക;

	അവസ്ഥ StandardMode::POSITION_HOLD:
		എങ്കിൽ (!is_vtol && vehicle_type == vehicle_status_s::VEHICLE_TYPE_ROTARY_WING) {
			തിരികെ vehicle_status_s::NAVIGATION_STATE_POSCTL;
		}

		നിർത്തുക;

	അവസ്ഥ StandardMode::CRUISE:
		എങ്കിൽ (!is_vtol && vehicle_type == vehicle_status_s::VEHICLE_TYPE_FIXED_WING) {
			തിരികെ vehicle_status_s::NAVIGATION_STATE_POSCTL;
		}

		നിർത്തുക;

	അവസ്ഥ StandardMode::ORBIT:
		എങ്കിൽ (vehicle_type == vehicle_status_s::VEHICLE_TYPE_ROTARY_WING) {
			തിരികെ vehicle_status_s::NAVIGATION_STATE_ORBIT;
		}

		എങ്കിൽ (vehicle_type == vehicle_status_s::VEHICLE_TYPE_FIXED_WING) {
			തിരികെ vehicle_status_s::NAVIGATION_STATE_AUTO_LOITER;
		}

		നിർത്തുക;

	സ്ഥിരസ്ഥിതി: നിർത്തുക;
	}

	തിരികെ vehicle_status_s::NAVIGATION_STATE_MAX;
}


} // namespace mode_util
