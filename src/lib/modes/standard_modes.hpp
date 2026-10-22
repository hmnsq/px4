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

#அந்தர்பாவ <uORB/topics/vehicle_status.h>

#அந்தர்பாவ <stdint.h>

நாமாகாஶ mode_util
{

// This matches the definition from MAVLink MAV_STANDARD_MODE
எண்ணிக்கை வகுப்பு StandardMode : uint8_t {
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
நிலையான உள்வரி StandardMode getStandardModeFromNavState(uint8_t nav_state, uint8_t vehicle_type, தருக்க is_vtol)
{
	தேர்வு (nav_state) {
	நிலை vehicle_status_s::NAVIGATION_STATE_AUTO_RTL: திரும்பு StandardMode::SAFE_RECOVERY;

	நிலை vehicle_status_s::NAVIGATION_STATE_AUTO_MISSION: திரும்பு StandardMode::MISSION;

	நிலை vehicle_status_s::NAVIGATION_STATE_AUTO_LAND: திரும்பு StandardMode::LAND;

	நிலை vehicle_status_s::NAVIGATION_STATE_AUTO_TAKEOFF: திரும்பு StandardMode::TAKEOFF;

	நிலை vehicle_status_s::NAVIGATION_STATE_ALTCTL:
		எனில் (is_vtol || vehicle_type == vehicle_status_s::VEHICLE_TYPE_ROTARY_WING
		    || vehicle_type == vehicle_status_s::VEHICLE_TYPE_FIXED_WING) {
			திரும்பு StandardMode::ALTITUDE_HOLD;
		}

		நிறுத்து;

	நிலை vehicle_status_s::NAVIGATION_STATE_POSCTL:
		எனில் (!is_vtol && vehicle_type == vehicle_status_s::VEHICLE_TYPE_ROTARY_WING) {
			திரும்பு StandardMode::POSITION_HOLD;
		}

		எனில் (!is_vtol && vehicle_type == vehicle_status_s::VEHICLE_TYPE_FIXED_WING) {
			திரும்பு StandardMode::CRUISE;
		}

		நிறுத்து;

	நிலை vehicle_status_s::NAVIGATION_STATE_ORBIT:
		எனில் (is_vtol || vehicle_type == vehicle_status_s::VEHICLE_TYPE_ROTARY_WING
		    || vehicle_type == vehicle_status_s::VEHICLE_TYPE_FIXED_WING) {
			திரும்பு StandardMode::ORBIT;
		}

		நிறுத்து;
	}

	திரும்பு StandardMode::NON_STANDARD;
}

/**
 * @return Get nav_state from a standard mode, or vehicle_status_s::NAVIGATION_STATE_MAX if not supported
 */
நிலையான உள்வரி uint8_t getNavStateFromStandardMode(StandardMode mode, uint8_t vehicle_type, தருக்க is_vtol)
{
	தேர்வு (mode) {
	நிலை StandardMode::SAFE_RECOVERY: திரும்பு vehicle_status_s::NAVIGATION_STATE_AUTO_RTL;

	நிலை StandardMode::MISSION: திரும்பு vehicle_status_s::NAVIGATION_STATE_AUTO_MISSION;

	நிலை StandardMode::LAND: திரும்பு vehicle_status_s::NAVIGATION_STATE_AUTO_LAND;

	நிலை StandardMode::TAKEOFF: திரும்பு vehicle_status_s::NAVIGATION_STATE_AUTO_TAKEOFF;

	நிலை StandardMode::ALTITUDE_HOLD:
		எனில் (is_vtol || vehicle_type == vehicle_status_s::VEHICLE_TYPE_ROTARY_WING
		    || vehicle_type == vehicle_status_s::VEHICLE_TYPE_FIXED_WING) {
			திரும்பு vehicle_status_s::NAVIGATION_STATE_ALTCTL;
		}

		நிறுத்து;

	நிலை StandardMode::POSITION_HOLD:
		எனில் (!is_vtol && vehicle_type == vehicle_status_s::VEHICLE_TYPE_ROTARY_WING) {
			திரும்பு vehicle_status_s::NAVIGATION_STATE_POSCTL;
		}

		நிறுத்து;

	நிலை StandardMode::CRUISE:
		எனில் (!is_vtol && vehicle_type == vehicle_status_s::VEHICLE_TYPE_FIXED_WING) {
			திரும்பு vehicle_status_s::NAVIGATION_STATE_POSCTL;
		}

		நிறுத்து;

	நிலை StandardMode::ORBIT:
		எனில் (vehicle_type == vehicle_status_s::VEHICLE_TYPE_ROTARY_WING) {
			திரும்பு vehicle_status_s::NAVIGATION_STATE_ORBIT;
		}

		எனில் (vehicle_type == vehicle_status_s::VEHICLE_TYPE_FIXED_WING) {
			திரும்பு vehicle_status_s::NAVIGATION_STATE_AUTO_LOITER;
		}

		நிறுத்து;

	இயல்பு: நிறுத்து;
	}

	திரும்பு vehicle_status_s::NAVIGATION_STATE_MAX;
}


} // namespace mode_util
