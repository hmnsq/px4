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

#অন্তর্ভাব <uORB/topics/vehicle_status.h>

#অন্তর্ভাব <stdint.h>

নামাকাশ mode_util
{

// This matches the definition from MAVLink MAV_STANDARD_MODE
গণনা শ্রেণী StandardMode : uint8_t {
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
স্থির অন্তঃরেখ StandardMode getStandardModeFromNavState(uint8_t nav_state, uint8_t vehicle_type, যৌক্তিক is_vtol)
{
	নির্বাচন (nav_state) {
	ক্ষেত্র vehicle_status_s::NAVIGATION_STATE_AUTO_RTL: ফেরত StandardMode::SAFE_RECOVERY;

	ক্ষেত্র vehicle_status_s::NAVIGATION_STATE_AUTO_MISSION: ফেরত StandardMode::MISSION;

	ক্ষেত্র vehicle_status_s::NAVIGATION_STATE_AUTO_LAND: ফেরত StandardMode::LAND;

	ক্ষেত্র vehicle_status_s::NAVIGATION_STATE_AUTO_TAKEOFF: ফেরত StandardMode::TAKEOFF;

	ক্ষেত্র vehicle_status_s::NAVIGATION_STATE_ALTCTL:
		যদি (is_vtol || vehicle_type == vehicle_status_s::VEHICLE_TYPE_ROTARY_WING
		    || vehicle_type == vehicle_status_s::VEHICLE_TYPE_FIXED_WING) {
			ফেরত StandardMode::ALTITUDE_HOLD;
		}

		বিরতি;

	ক্ষেত্র vehicle_status_s::NAVIGATION_STATE_POSCTL:
		যদি (!is_vtol && vehicle_type == vehicle_status_s::VEHICLE_TYPE_ROTARY_WING) {
			ফেরত StandardMode::POSITION_HOLD;
		}

		যদি (!is_vtol && vehicle_type == vehicle_status_s::VEHICLE_TYPE_FIXED_WING) {
			ফেরত StandardMode::CRUISE;
		}

		বিরতি;

	ক্ষেত্র vehicle_status_s::NAVIGATION_STATE_ORBIT:
		যদি (is_vtol || vehicle_type == vehicle_status_s::VEHICLE_TYPE_ROTARY_WING
		    || vehicle_type == vehicle_status_s::VEHICLE_TYPE_FIXED_WING) {
			ফেরত StandardMode::ORBIT;
		}

		বিরতি;
	}

	ফেরত StandardMode::NON_STANDARD;
}

/**
 * @return Get nav_state from a standard mode, or vehicle_status_s::NAVIGATION_STATE_MAX if not supported
 */
স্থির অন্তঃরেখ uint8_t getNavStateFromStandardMode(StandardMode mode, uint8_t vehicle_type, যৌক্তিক is_vtol)
{
	নির্বাচন (mode) {
	ক্ষেত্র StandardMode::SAFE_RECOVERY: ফেরত vehicle_status_s::NAVIGATION_STATE_AUTO_RTL;

	ক্ষেত্র StandardMode::MISSION: ফেরত vehicle_status_s::NAVIGATION_STATE_AUTO_MISSION;

	ক্ষেত্র StandardMode::LAND: ফেরত vehicle_status_s::NAVIGATION_STATE_AUTO_LAND;

	ক্ষেত্র StandardMode::TAKEOFF: ফেরত vehicle_status_s::NAVIGATION_STATE_AUTO_TAKEOFF;

	ক্ষেত্র StandardMode::ALTITUDE_HOLD:
		যদি (is_vtol || vehicle_type == vehicle_status_s::VEHICLE_TYPE_ROTARY_WING
		    || vehicle_type == vehicle_status_s::VEHICLE_TYPE_FIXED_WING) {
			ফেরত vehicle_status_s::NAVIGATION_STATE_ALTCTL;
		}

		বিরতি;

	ক্ষেত্র StandardMode::POSITION_HOLD:
		যদি (!is_vtol && vehicle_type == vehicle_status_s::VEHICLE_TYPE_ROTARY_WING) {
			ফেরত vehicle_status_s::NAVIGATION_STATE_POSCTL;
		}

		বিরতি;

	ক্ষেত্র StandardMode::CRUISE:
		যদি (!is_vtol && vehicle_type == vehicle_status_s::VEHICLE_TYPE_FIXED_WING) {
			ফেরত vehicle_status_s::NAVIGATION_STATE_POSCTL;
		}

		বিরতি;

	ক্ষেত্র StandardMode::ORBIT:
		যদি (vehicle_type == vehicle_status_s::VEHICLE_TYPE_ROTARY_WING) {
			ফেরত vehicle_status_s::NAVIGATION_STATE_ORBIT;
		}

		যদি (vehicle_type == vehicle_status_s::VEHICLE_TYPE_FIXED_WING) {
			ফেরত vehicle_status_s::NAVIGATION_STATE_AUTO_LOITER;
		}

		বিরতি;

	পূর্বনির্ধারিত: বিরতি;
	}

	ফেরত vehicle_status_s::NAVIGATION_STATE_MAX;
}


} // namespace mode_util
