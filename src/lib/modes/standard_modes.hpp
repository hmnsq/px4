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

#ಅನ್ತರ್ಭಾವ <uORB/topics/vehicle_status.h>

#ಅನ್ತರ್ಭಾವ <stdint.h>

ನಾಮಾಕಾಶ mode_util
{

// This matches the definition from MAVLink MAV_STANDARD_MODE
ಗಣನಾ ವರ್ಗ StandardMode : uint8_t {
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
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ StandardMode getStandardModeFromNavState(uint8_t nav_state, uint8_t vehicle_type, ತಾರ್ಕಿಕ is_vtol)
{
	ವಿಕಲ್ಪ (nav_state) {
	ಪಕ್ಷ vehicle_status_s::NAVIGATION_STATE_AUTO_RTL: ಪ್ರತ್ಯಾಗಮ StandardMode::SAFE_RECOVERY;

	ಪಕ್ಷ vehicle_status_s::NAVIGATION_STATE_AUTO_MISSION: ಪ್ರತ್ಯಾಗಮ StandardMode::MISSION;

	ಪಕ್ಷ vehicle_status_s::NAVIGATION_STATE_AUTO_LAND: ಪ್ರತ್ಯಾಗಮ StandardMode::LAND;

	ಪಕ್ಷ vehicle_status_s::NAVIGATION_STATE_AUTO_TAKEOFF: ಪ್ರತ್ಯಾಗಮ StandardMode::TAKEOFF;

	ಪಕ್ಷ vehicle_status_s::NAVIGATION_STATE_ALTCTL:
		ಯದಿ (is_vtol || vehicle_type == vehicle_status_s::VEHICLE_TYPE_ROTARY_WING
		    || vehicle_type == vehicle_status_s::VEHICLE_TYPE_FIXED_WING) {
			ಪ್ರತ್ಯಾಗಮ StandardMode::ALTITUDE_HOLD;
		}

		ಭಙ್ಗ;

	ಪಕ್ಷ vehicle_status_s::NAVIGATION_STATE_POSCTL:
		ಯದಿ (!is_vtol && vehicle_type == vehicle_status_s::VEHICLE_TYPE_ROTARY_WING) {
			ಪ್ರತ್ಯಾಗಮ StandardMode::POSITION_HOLD;
		}

		ಯದಿ (!is_vtol && vehicle_type == vehicle_status_s::VEHICLE_TYPE_FIXED_WING) {
			ಪ್ರತ್ಯಾಗಮ StandardMode::CRUISE;
		}

		ಭಙ್ಗ;

	ಪಕ್ಷ vehicle_status_s::NAVIGATION_STATE_ORBIT:
		ಯದಿ (is_vtol || vehicle_type == vehicle_status_s::VEHICLE_TYPE_ROTARY_WING
		    || vehicle_type == vehicle_status_s::VEHICLE_TYPE_FIXED_WING) {
			ಪ್ರತ್ಯಾಗಮ StandardMode::ORBIT;
		}

		ಭಙ್ಗ;
	}

	ಪ್ರತ್ಯಾಗಮ StandardMode::NON_STANDARD;
}

/**
 * @return Get nav_state from a standard mode, or vehicle_status_s::NAVIGATION_STATE_MAX if not supported
 */
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ uint8_t getNavStateFromStandardMode(StandardMode mode, uint8_t vehicle_type, ತಾರ್ಕಿಕ is_vtol)
{
	ವಿಕಲ್ಪ (mode) {
	ಪಕ್ಷ StandardMode::SAFE_RECOVERY: ಪ್ರತ್ಯಾಗಮ vehicle_status_s::NAVIGATION_STATE_AUTO_RTL;

	ಪಕ್ಷ StandardMode::MISSION: ಪ್ರತ್ಯಾಗಮ vehicle_status_s::NAVIGATION_STATE_AUTO_MISSION;

	ಪಕ್ಷ StandardMode::LAND: ಪ್ರತ್ಯಾಗಮ vehicle_status_s::NAVIGATION_STATE_AUTO_LAND;

	ಪಕ್ಷ StandardMode::TAKEOFF: ಪ್ರತ್ಯಾಗಮ vehicle_status_s::NAVIGATION_STATE_AUTO_TAKEOFF;

	ಪಕ್ಷ StandardMode::ALTITUDE_HOLD:
		ಯದಿ (is_vtol || vehicle_type == vehicle_status_s::VEHICLE_TYPE_ROTARY_WING
		    || vehicle_type == vehicle_status_s::VEHICLE_TYPE_FIXED_WING) {
			ಪ್ರತ್ಯಾಗಮ vehicle_status_s::NAVIGATION_STATE_ALTCTL;
		}

		ಭಙ್ಗ;

	ಪಕ್ಷ StandardMode::POSITION_HOLD:
		ಯದಿ (!is_vtol && vehicle_type == vehicle_status_s::VEHICLE_TYPE_ROTARY_WING) {
			ಪ್ರತ್ಯಾಗಮ vehicle_status_s::NAVIGATION_STATE_POSCTL;
		}

		ಭಙ್ಗ;

	ಪಕ್ಷ StandardMode::CRUISE:
		ಯದಿ (!is_vtol && vehicle_type == vehicle_status_s::VEHICLE_TYPE_FIXED_WING) {
			ಪ್ರತ್ಯಾಗಮ vehicle_status_s::NAVIGATION_STATE_POSCTL;
		}

		ಭಙ್ಗ;

	ಪಕ್ಷ StandardMode::ORBIT:
		ಯದಿ (vehicle_type == vehicle_status_s::VEHICLE_TYPE_ROTARY_WING) {
			ಪ್ರತ್ಯಾಗಮ vehicle_status_s::NAVIGATION_STATE_ORBIT;
		}

		ಯದಿ (vehicle_type == vehicle_status_s::VEHICLE_TYPE_FIXED_WING) {
			ಪ್ರತ್ಯಾಗಮ vehicle_status_s::NAVIGATION_STATE_AUTO_LOITER;
		}

		ಭಙ್ಗ;

	ಸ್ವಭಾವ: ಭಙ್ಗ;
	}

	ಪ್ರತ್ಯಾಗಮ vehicle_status_s::NAVIGATION_STATE_MAX;
}


} // namespace mode_util
