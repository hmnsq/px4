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

#अन्तर्भाव <uORB/topics/vehicle_status.h>

#अन्तर्भाव <stdint.h>

नामाकाश mode_util
{

// This matches the definition from MAVLink MAV_STANDARD_MODE
गणना वर्ग StandardMode : uint8_t {
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
स्थिर ओळीत StandardMode getStandardModeFromNavState(uint8_t nav_state, uint8_t vehicle_type, तार्किक is_vtol)
{
	निवड (nav_state) {
	स्थिती vehicle_status_s::NAVIGATION_STATE_AUTO_RTL: परत StandardMode::SAFE_RECOVERY;

	स्थिती vehicle_status_s::NAVIGATION_STATE_AUTO_MISSION: परत StandardMode::MISSION;

	स्थिती vehicle_status_s::NAVIGATION_STATE_AUTO_LAND: परत StandardMode::LAND;

	स्थिती vehicle_status_s::NAVIGATION_STATE_AUTO_TAKEOFF: परत StandardMode::TAKEOFF;

	स्थिती vehicle_status_s::NAVIGATION_STATE_ALTCTL:
		जर (is_vtol || vehicle_type == vehicle_status_s::VEHICLE_TYPE_ROTARY_WING
		    || vehicle_type == vehicle_status_s::VEHICLE_TYPE_FIXED_WING) {
			परत StandardMode::ALTITUDE_HOLD;
		}

		थांबा;

	स्थिती vehicle_status_s::NAVIGATION_STATE_POSCTL:
		जर (!is_vtol && vehicle_type == vehicle_status_s::VEHICLE_TYPE_ROTARY_WING) {
			परत StandardMode::POSITION_HOLD;
		}

		जर (!is_vtol && vehicle_type == vehicle_status_s::VEHICLE_TYPE_FIXED_WING) {
			परत StandardMode::CRUISE;
		}

		थांबा;

	स्थिती vehicle_status_s::NAVIGATION_STATE_ORBIT:
		जर (is_vtol || vehicle_type == vehicle_status_s::VEHICLE_TYPE_ROTARY_WING
		    || vehicle_type == vehicle_status_s::VEHICLE_TYPE_FIXED_WING) {
			परत StandardMode::ORBIT;
		}

		थांबा;
	}

	परत StandardMode::NON_STANDARD;
}

/**
 * @return Get nav_state from a standard mode, or vehicle_status_s::NAVIGATION_STATE_MAX if not supported
 */
स्थिर ओळीत uint8_t getNavStateFromStandardMode(StandardMode mode, uint8_t vehicle_type, तार्किक is_vtol)
{
	निवड (mode) {
	स्थिती StandardMode::SAFE_RECOVERY: परत vehicle_status_s::NAVIGATION_STATE_AUTO_RTL;

	स्थिती StandardMode::MISSION: परत vehicle_status_s::NAVIGATION_STATE_AUTO_MISSION;

	स्थिती StandardMode::LAND: परत vehicle_status_s::NAVIGATION_STATE_AUTO_LAND;

	स्थिती StandardMode::TAKEOFF: परत vehicle_status_s::NAVIGATION_STATE_AUTO_TAKEOFF;

	स्थिती StandardMode::ALTITUDE_HOLD:
		जर (is_vtol || vehicle_type == vehicle_status_s::VEHICLE_TYPE_ROTARY_WING
		    || vehicle_type == vehicle_status_s::VEHICLE_TYPE_FIXED_WING) {
			परत vehicle_status_s::NAVIGATION_STATE_ALTCTL;
		}

		थांबा;

	स्थिती StandardMode::POSITION_HOLD:
		जर (!is_vtol && vehicle_type == vehicle_status_s::VEHICLE_TYPE_ROTARY_WING) {
			परत vehicle_status_s::NAVIGATION_STATE_POSCTL;
		}

		थांबा;

	स्थिती StandardMode::CRUISE:
		जर (!is_vtol && vehicle_type == vehicle_status_s::VEHICLE_TYPE_FIXED_WING) {
			परत vehicle_status_s::NAVIGATION_STATE_POSCTL;
		}

		थांबा;

	स्थिती StandardMode::ORBIT:
		जर (vehicle_type == vehicle_status_s::VEHICLE_TYPE_ROTARY_WING) {
			परत vehicle_status_s::NAVIGATION_STATE_ORBIT;
		}

		जर (vehicle_type == vehicle_status_s::VEHICLE_TYPE_FIXED_WING) {
			परत vehicle_status_s::NAVIGATION_STATE_AUTO_LOITER;
		}

		थांबा;

	पूर्वनिर्धारित: थांबा;
	}

	परत vehicle_status_s::NAVIGATION_STATE_MAX;
}


} // namespace mode_util
