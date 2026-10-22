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

/**
 * @return Bitmask with all valid modes
 */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ uint32_t get_Valid_Nav_ಸ್ಥಿತಿಗಳು(uint32_t ಬಹುಲಕ_not_supported = 0)
{
	ಹಿಂತಿರುಗಿ ((1u << vehicle_ಸ್ಥಿತಿ_s::NAVIGATION_STATE_MANUAL) |
		(1u << vehicle_ಸ್ಥಿತಿ_s::NAVIGATION_STATE_ALTCTL) |
		(1u << vehicle_ಸ್ಥಿತಿ_s::NAVIGATION_STATE_ALTITUDE_CRUISE) |
		(1u << vehicle_ಸ್ಥಿತಿ_s::NAVIGATION_STATE_MANUAL_PARKING) |
		(1u << vehicle_ಸ್ಥಿತಿ_s::NAVIGATION_STATE_POSCTL) |
		(1u << vehicle_ಸ್ಥಿತಿ_s::NAVIGATION_STATE_AUTO_MISSION) |
		(1u << vehicle_ಸ್ಥಿತಿ_s::NAVIGATION_STATE_AUTO_LOITER) |
		(1u << vehicle_ಸ್ಥಿತಿ_s::NAVIGATION_STATE_GUIDED_ಕೋರ್ಸು) |
		(1u << vehicle_ಸ್ಥಿತಿ_s::NAVIGATION_STATE_AUTO_RTL) |
		(1u << vehicle_ಸ್ಥಿತಿ_s::NAVIGATION_STATE_POSITION_SLOW) |
		(1u << vehicle_ಸ್ಥಿತಿ_s::NAVIGATION_STATE_ACRO) |
		(1u << vehicle_ಸ್ಥಿತಿ_s::NAVIGATION_STATE_TERMINATION) |
		(1u << vehicle_ಸ್ಥಿತಿ_s::NAVIGATION_STATE_OFFBOARD) |
		(1u << vehicle_ಸ್ಥಿತಿ_s::NAVIGATION_STATE_STAB) |
		(1u << vehicle_ಸ್ಥಿತಿ_s::NAVIGATION_STATE_AUTO_TAKEOFF) |
		(1u << vehicle_ಸ್ಥಿತಿ_s::NAVIGATION_STATE_AUTO_LAND) |
		(1u << vehicle_ಸ್ಥಿತಿ_s::NAVIGATION_STATE_DESCEND) |
		(1u << vehicle_ಸ್ಥಿತಿ_s::NAVIGATION_STATE_AUTO_FOLLOW_ಗುರಿ) |
		(1u << vehicle_ಸ್ಥಿತಿ_s::NAVIGATION_STATE_AUTO_PRECLAND) |
		(1u << vehicle_ಸ್ಥಿತಿ_s::NAVIGATION_STATE_ORBIT) |
		(1u << vehicle_ಸ್ಥಿತಿ_s::NAVIGATION_STATE_AUTO_VTOL_TAKEOFF)) & ~ಬಹುಲಕ_not_supported;

	static_assert(vehicle_ಸ್ಥಿತಿ_s::NAVIGATION_STATE_MAX  == 31, "update valid nav states");
}

ಸಾಲೊಳಗೆ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸ್ಥಿರಾಂಕ nav_state_ಹೆಸರುಗಳು[vehicle_ಸ್ಥಿತಿ_s::NAVIGATION_STATE_MAX] = {
	"Manual",
	"Altitude",
	"Position",
	"Mission",
	"Hold",
	"Return",
	"Position Slow",
	"Guided Course",
	"Altitude Cruise",
	"Manual Parking",
	"Acro",
	"11: UNUSED",
	"Descend",
	"Termination",
	"Offboard",
	"Stabilized",
	"16: UNUSED2",
	"Takeoff",
	"Land",
	"Follow Target",
	"Precision Landing",
	"Orbit",
	"VTOL Takeoff",
	"External 1",
	"External 2",
	"External 3",
	"External 4",
	"External 5",
	"External 6",
	"External 7",
	"External 8",
};

/**
 * @return Human-readable name for a nav_state, taking the vehicle type into account.
 *
 * The position control mode is shown as "Cruise" for fixed-wing vehicles (including VTOLs in
 * forward flight, which report VEHICLE_TYPE_FIXED_WING) and as "Position" for all other types.
 */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *nav_state_name(uint8_t nav_state, uint8_t vehicle_type)
{
	ಆದರೆ (nav_state >= vehicle_ಸ್ಥಿತಿ_s::NAVIGATION_STATE_MAX) {
		ಹಿಂತಿರುಗಿ "(unknown)";
	}

	ಆದರೆ (nav_state == vehicle_ಸ್ಥಿತಿ_s::NAVIGATION_STATE_POSCTL
	    && vehicle_type == vehicle_ಸ್ಥಿತಿ_s::VEHICLE_TYPE_FIXED_WING) {
		ಹಿಂತಿರುಗಿ "Cruise";
	}

	ಹಿಂತಿರುಗಿ nav_state_ಹೆಸರುಗಳು[nav_state];
}

/**
 * @return returns true for advanced modes
 */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ತಾರ್ಕಿಕ isAdvanced(uint8_t nav_state)
{
	ಆಯ್ಕೆ (nav_state) {
	ಸ್ಥಿತಿ vehicle_ಸ್ಥಿತಿ_s::NAVIGATION_STATE_ALTCTL: ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;

	ಸ್ಥಿತಿ vehicle_ಸ್ಥಿತಿ_s::NAVIGATION_STATE_POSCTL: ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;

	ಸ್ಥಿತಿ vehicle_ಸ್ಥಿತಿ_s::NAVIGATION_STATE_EXTERNAL1: ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;

	ಸ್ಥಿತಿ vehicle_ಸ್ಥಿತಿ_s::NAVIGATION_STATE_EXTERNAL2: ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;

	ಸ್ಥಿತಿ vehicle_ಸ್ಥಿತಿ_s::NAVIGATION_STATE_EXTERNAL3: ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;

	ಸ್ಥಿತಿ vehicle_ಸ್ಥಿತಿ_s::NAVIGATION_STATE_EXTERNAL4: ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;

	ಸ್ಥಿತಿ vehicle_ಸ್ಥಿತಿ_s::NAVIGATION_STATE_EXTERNAL5: ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;

	ಸ್ಥಿತಿ vehicle_ಸ್ಥಿತಿ_s::NAVIGATION_STATE_EXTERNAL6: ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;

	ಸ್ಥಿತಿ vehicle_ಸ್ಥಿತಿ_s::NAVIGATION_STATE_EXTERNAL7: ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;

	ಸ್ಥಿತಿ vehicle_ಸ್ಥಿತಿ_s::NAVIGATION_STATE_EXTERNAL8: ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;

	}

	ಹಿಂತಿರುಗಿ ನಿಜ;
}

} // namespace mode_util
