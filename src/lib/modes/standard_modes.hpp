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

#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ <uORB/topics/vehicle_status.h>

#ಅನ್ತರ್ಭಾವ <stdint.h>

ನಾಮಾಕಾಶ ಬಹುಲಕ_util
{

// This matches the definition from MAVLink MAV_STANDARD_MODE
ಎಣಿಕೆ ವರ್ಗ Standard_ಬಹುಲಕ : uint8_t {
	NON_STANDARD = 0,
	POSITION_HOLD = 1,
	ORBIT = 2,
	CRUISE = 3,
	ALTITUDE_HOLD = 4,
	ಸುರಕ್ಷಿತ_RECOVERY = 5,
	MISSION = 6,
	LAND = 7,
	TAKEOFF = 8,
};

/**
 * @return Get MAVLink standard mode from nav_state
 */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ Standard_ಬಹುಲಕ get_Standard_ಬಹುಲಕ_From_Nav_State(uint8_t nav_state, uint8_t vehicle_type, ತಾರ್ಕಿಕ is_vtol)
{
	ಆಯ್ಕೆ (nav_state) {
	ಸ್ಥಿತಿ vehicle_ಸ್ಥಿತಿ_s::NAVIGATION_STATE_AUTO_RTL: ಹಿಂತಿರುಗಿ Standard_ಬಹುಲಕ::ಸುರಕ್ಷಿತ_RECOVERY;

	ಸ್ಥಿತಿ vehicle_ಸ್ಥಿತಿ_s::NAVIGATION_STATE_AUTO_MISSION: ಹಿಂತಿರುಗಿ Standard_ಬಹುಲಕ::MISSION;

	ಸ್ಥಿತಿ vehicle_ಸ್ಥಿತಿ_s::NAVIGATION_STATE_AUTO_LAND: ಹಿಂತಿರುಗಿ Standard_ಬಹುಲಕ::LAND;

	ಸ್ಥಿತಿ vehicle_ಸ್ಥಿತಿ_s::NAVIGATION_STATE_AUTO_TAKEOFF: ಹಿಂತಿರುಗಿ Standard_ಬಹುಲಕ::TAKEOFF;

	ಸ್ಥಿತಿ vehicle_ಸ್ಥಿತಿ_s::NAVIGATION_STATE_ALTCTL:
		ಆದರೆ (is_vtol || vehicle_type == vehicle_ಸ್ಥಿತಿ_s::VEHICLE_TYPE_ROTARY_WING
		    || vehicle_type == vehicle_ಸ್ಥಿತಿ_s::VEHICLE_TYPE_FIXED_WING) {
			ಹಿಂತಿರುಗಿ Standard_ಬಹುಲಕ::ALTITUDE_HOLD;
		}

		ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ vehicle_ಸ್ಥಿತಿ_s::NAVIGATION_STATE_POSCTL:
		ಆದರೆ (!is_vtol && vehicle_type == vehicle_ಸ್ಥಿತಿ_s::VEHICLE_TYPE_ROTARY_WING) {
			ಹಿಂತಿರುಗಿ Standard_ಬಹುಲಕ::POSITION_HOLD;
		}

		ಆದರೆ (!is_vtol && vehicle_type == vehicle_ಸ್ಥಿತಿ_s::VEHICLE_TYPE_FIXED_WING) {
			ಹಿಂತಿರುಗಿ Standard_ಬಹುಲಕ::CRUISE;
		}

		ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ vehicle_ಸ್ಥಿತಿ_s::NAVIGATION_STATE_ORBIT:
		ಆದರೆ (is_vtol || vehicle_type == vehicle_ಸ್ಥಿತಿ_s::VEHICLE_TYPE_ROTARY_WING
		    || vehicle_type == vehicle_ಸ್ಥಿತಿ_s::VEHICLE_TYPE_FIXED_WING) {
			ಹಿಂತಿರುಗಿ Standard_ಬಹುಲಕ::ORBIT;
		}

		ನಿಲ್ಲಿಸು;
	}

	ಹಿಂತಿರುಗಿ Standard_ಬಹುಲಕ::NON_STANDARD;
}

/**
 * @return Get nav_state from a standard mode, or vehicle_status_s::NAVIGATION_STATE_MAX if not supported
 */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ uint8_t get_Nav_State_From_Standard_ಬಹುಲಕ(Standard_ಬಹುಲಕ ಬಹುಲಕ, uint8_t vehicle_type, ತಾರ್ಕಿಕ is_vtol)
{
	ಆಯ್ಕೆ (ಬಹುಲಕ) {
	ಸ್ಥಿತಿ Standard_ಬಹುಲಕ::ಸುರಕ್ಷಿತ_RECOVERY: ಹಿಂತಿರುಗಿ vehicle_ಸ್ಥಿತಿ_s::NAVIGATION_STATE_AUTO_RTL;

	ಸ್ಥಿತಿ Standard_ಬಹುಲಕ::MISSION: ಹಿಂತಿರುಗಿ vehicle_ಸ್ಥಿತಿ_s::NAVIGATION_STATE_AUTO_MISSION;

	ಸ್ಥಿತಿ Standard_ಬಹುಲಕ::LAND: ಹಿಂತಿರುಗಿ vehicle_ಸ್ಥಿತಿ_s::NAVIGATION_STATE_AUTO_LAND;

	ಸ್ಥಿತಿ Standard_ಬಹುಲಕ::TAKEOFF: ಹಿಂತಿರುಗಿ vehicle_ಸ್ಥಿತಿ_s::NAVIGATION_STATE_AUTO_TAKEOFF;

	ಸ್ಥಿತಿ Standard_ಬಹುಲಕ::ALTITUDE_HOLD:
		ಆದರೆ (is_vtol || vehicle_type == vehicle_ಸ್ಥಿತಿ_s::VEHICLE_TYPE_ROTARY_WING
		    || vehicle_type == vehicle_ಸ್ಥಿತಿ_s::VEHICLE_TYPE_FIXED_WING) {
			ಹಿಂತಿರುಗಿ vehicle_ಸ್ಥಿತಿ_s::NAVIGATION_STATE_ALTCTL;
		}

		ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ Standard_ಬಹುಲಕ::POSITION_HOLD:
		ಆದರೆ (!is_vtol && vehicle_type == vehicle_ಸ್ಥಿತಿ_s::VEHICLE_TYPE_ROTARY_WING) {
			ಹಿಂತಿರುಗಿ vehicle_ಸ್ಥಿತಿ_s::NAVIGATION_STATE_POSCTL;
		}

		ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ Standard_ಬಹುಲಕ::CRUISE:
		ಆದರೆ (!is_vtol && vehicle_type == vehicle_ಸ್ಥಿತಿ_s::VEHICLE_TYPE_FIXED_WING) {
			ಹಿಂತಿರುಗಿ vehicle_ಸ್ಥಿತಿ_s::NAVIGATION_STATE_POSCTL;
		}

		ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ Standard_ಬಹುಲಕ::ORBIT:
		ಆದರೆ (vehicle_type == vehicle_ಸ್ಥಿತಿ_s::VEHICLE_TYPE_ROTARY_WING) {
			ಹಿಂತಿರುಗಿ vehicle_ಸ್ಥಿತಿ_s::NAVIGATION_STATE_ORBIT;
		}

		ಆದರೆ (vehicle_type == vehicle_ಸ್ಥಿತಿ_s::VEHICLE_TYPE_FIXED_WING) {
			ಹಿಂತಿರುಗಿ vehicle_ಸ್ಥಿತಿ_s::NAVIGATION_STATE_AUTO_LOITER;
		}

		ನಿಲ್ಲಿಸು;

	ಪೂರ್ವನಿಯೋಜಿತ: ನಿಲ್ಲಿಸು;
	}

	ಹಿಂತಿರುಗಿ vehicle_ಸ್ಥಿತಿ_s::NAVIGATION_STATE_MAX;
}


} // namespace mode_util
