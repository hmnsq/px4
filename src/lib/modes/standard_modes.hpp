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

#ضمن <uORB/topics/vehicle_status.h>

#ضمن <stdint.h>

نطاق mode_util
{

// This matches the definition from MAVLink MAV_STANDARD_MODE
تعداد מחלקה StandardMode : uint8_t {
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
ساكن مضمن StandardMode getStandardModeFromNavState(uint8_t nav_state, uint8_t vehicle_type, منطقي is_vtol)
{
	بدل (nav_state) {
	حالة vehicle_status_s::NAVIGATION_STATE_AUTO_RTL: החזר StandardMode::SAFE_RECOVERY;

	حالة vehicle_status_s::NAVIGATION_STATE_AUTO_MISSION: החזר StandardMode::MISSION;

	حالة vehicle_status_s::NAVIGATION_STATE_AUTO_LAND: החזר StandardMode::LAND;

	حالة vehicle_status_s::NAVIGATION_STATE_AUTO_TAKEOFF: החזר StandardMode::TAKEOFF;

	حالة vehicle_status_s::NAVIGATION_STATE_ALTCTL:
		אם (is_vtol || vehicle_type == vehicle_status_s::VEHICLE_TYPE_ROTARY_WING
		    || vehicle_type == vehicle_status_s::VEHICLE_TYPE_FIXED_WING) {
			החזר StandardMode::ALTITUDE_HOLD;
		}

		اكسر;

	حالة vehicle_status_s::NAVIGATION_STATE_POSCTL:
		אם (!is_vtol && vehicle_type == vehicle_status_s::VEHICLE_TYPE_ROTARY_WING) {
			החזר StandardMode::POSITION_HOLD;
		}

		אם (!is_vtol && vehicle_type == vehicle_status_s::VEHICLE_TYPE_FIXED_WING) {
			החזר StandardMode::CRUISE;
		}

		اكسر;

	حالة vehicle_status_s::NAVIGATION_STATE_ORBIT:
		אם (is_vtol || vehicle_type == vehicle_status_s::VEHICLE_TYPE_ROTARY_WING
		    || vehicle_type == vehicle_status_s::VEHICLE_TYPE_FIXED_WING) {
			החזר StandardMode::ORBIT;
		}

		اكسر;
	}

	החזר StandardMode::NON_STANDARD;
}

/**
 * @return Get nav_state from a standard mode, or vehicle_status_s::NAVIGATION_STATE_MAX if not supported
 */
ساكن مضمن uint8_t getNavStateFromStandardMode(StandardMode mode, uint8_t vehicle_type, منطقي is_vtol)
{
	بدل (mode) {
	حالة StandardMode::SAFE_RECOVERY: החזר vehicle_status_s::NAVIGATION_STATE_AUTO_RTL;

	حالة StandardMode::MISSION: החזר vehicle_status_s::NAVIGATION_STATE_AUTO_MISSION;

	حالة StandardMode::LAND: החזר vehicle_status_s::NAVIGATION_STATE_AUTO_LAND;

	حالة StandardMode::TAKEOFF: החזר vehicle_status_s::NAVIGATION_STATE_AUTO_TAKEOFF;

	حالة StandardMode::ALTITUDE_HOLD:
		אם (is_vtol || vehicle_type == vehicle_status_s::VEHICLE_TYPE_ROTARY_WING
		    || vehicle_type == vehicle_status_s::VEHICLE_TYPE_FIXED_WING) {
			החזר vehicle_status_s::NAVIGATION_STATE_ALTCTL;
		}

		اكسر;

	حالة StandardMode::POSITION_HOLD:
		אם (!is_vtol && vehicle_type == vehicle_status_s::VEHICLE_TYPE_ROTARY_WING) {
			החזר vehicle_status_s::NAVIGATION_STATE_POSCTL;
		}

		اكسر;

	حالة StandardMode::CRUISE:
		אם (!is_vtol && vehicle_type == vehicle_status_s::VEHICLE_TYPE_FIXED_WING) {
			החזר vehicle_status_s::NAVIGATION_STATE_POSCTL;
		}

		اكسر;

	حالة StandardMode::ORBIT:
		אם (vehicle_type == vehicle_status_s::VEHICLE_TYPE_ROTARY_WING) {
			החזר vehicle_status_s::NAVIGATION_STATE_ORBIT;
		}

		אם (vehicle_type == vehicle_status_s::VEHICLE_TYPE_FIXED_WING) {
			החזר vehicle_status_s::NAVIGATION_STATE_AUTO_LOITER;
		}

		اكسر;

	افتراضي: اكسر;
	}

	החזר vehicle_status_s::NAVIGATION_STATE_MAX;
}


} // namespace mode_util
