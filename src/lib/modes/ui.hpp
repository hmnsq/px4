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

#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ <uORB/topics/vehicle_status.h>

#ਅਨ੍ਤਰ੍ਭਾਵ <stdint.h>

ਨਾਮਾਕਾਸ਼ ਬਹੁਲਕ_util
{

/**
 * @return Bitmask with all valid modes
 */
ਸਥਿਰ ਸਤਰਅੰਦਰ uint32_t get_Valid_Nav_ਹਾਲਤਾਂ(uint32_t ਬਹੁਲਕ_not_supported = 0)
{
	ਵਾਪਸ ((1u << vehicle_ਸਥਿਤੀ_s::NAVIGATION_STATE_MANUAL) |
		(1u << vehicle_ਸਥਿਤੀ_s::NAVIGATION_STATE_ALTCTL) |
		(1u << vehicle_ਸਥਿਤੀ_s::NAVIGATION_STATE_ALTITUDE_CRUISE) |
		(1u << vehicle_ਸਥਿਤੀ_s::NAVIGATION_STATE_MANUAL_PARKING) |
		(1u << vehicle_ਸਥਿਤੀ_s::NAVIGATION_STATE_POSCTL) |
		(1u << vehicle_ਸਥਿਤੀ_s::NAVIGATION_STATE_AUTO_MISSION) |
		(1u << vehicle_ਸਥਿਤੀ_s::NAVIGATION_STATE_AUTO_LOITER) |
		(1u << vehicle_ਸਥਿਤੀ_s::NAVIGATION_STATE_GUIDED_ਕੋਰਸ) |
		(1u << vehicle_ਸਥਿਤੀ_s::NAVIGATION_STATE_AUTO_RTL) |
		(1u << vehicle_ਸਥਿਤੀ_s::NAVIGATION_STATE_POSITION_SLOW) |
		(1u << vehicle_ਸਥਿਤੀ_s::NAVIGATION_STATE_ACRO) |
		(1u << vehicle_ਸਥਿਤੀ_s::NAVIGATION_STATE_TERMINATION) |
		(1u << vehicle_ਸਥਿਤੀ_s::NAVIGATION_STATE_OFFBOARD) |
		(1u << vehicle_ਸਥਿਤੀ_s::NAVIGATION_STATE_STAB) |
		(1u << vehicle_ਸਥਿਤੀ_s::NAVIGATION_STATE_AUTO_TAKEOFF) |
		(1u << vehicle_ਸਥਿਤੀ_s::NAVIGATION_STATE_AUTO_LAND) |
		(1u << vehicle_ਸਥਿਤੀ_s::NAVIGATION_STATE_DESCEND) |
		(1u << vehicle_ਸਥਿਤੀ_s::NAVIGATION_STATE_AUTO_FOLLOW_ਨਿਸ਼ਾਨਾ) |
		(1u << vehicle_ਸਥਿਤੀ_s::NAVIGATION_STATE_AUTO_PRECLAND) |
		(1u << vehicle_ਸਥਿਤੀ_s::NAVIGATION_STATE_ORBIT) |
		(1u << vehicle_ਸਥਿਤੀ_s::NAVIGATION_STATE_AUTO_VTOL_TAKEOFF)) & ~ਬਹੁਲਕ_not_supported;

	static_assert(vehicle_ਸਥਿਤੀ_s::NAVIGATION_STATE_MAX  == 31, "update valid nav states");
}

ਸਤਰਅੰਦਰ ਅਚਲ ਅੱਖਰ *ਅਚਲ nav_state_ਨਾਂ[vehicle_ਸਥਿਤੀ_s::NAVIGATION_STATE_MAX] = {
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
ਸਥਿਰ ਸਤਰਅੰਦਰ ਅਚਲ ਅੱਖਰ *nav_state_name(uint8_t nav_state, uint8_t vehicle_type)
{
	ਜੇ (nav_state >= vehicle_ਸਥਿਤੀ_s::NAVIGATION_STATE_MAX) {
		ਵਾਪਸ "(unknown)";
	}

	ਜੇ (nav_state == vehicle_ਸਥਿਤੀ_s::NAVIGATION_STATE_POSCTL
	    && vehicle_type == vehicle_ਸਥਿਤੀ_s::VEHICLE_TYPE_FIXED_WING) {
		ਵਾਪਸ "Cruise";
	}

	ਵਾਪਸ nav_state_ਨਾਂ[nav_state];
}

/**
 * @return returns true for advanced modes
 */
ਸਥਿਰ ਸਤਰਅੰਦਰ ਤਾਰਕਿਕ isAdvanced(uint8_t nav_state)
{
	ਚੋਣ (nav_state) {
	ਹਾਲਤ vehicle_ਸਥਿਤੀ_s::NAVIGATION_STATE_ALTCTL: ਵਾਪਸ ਝੂਠ;

	ਹਾਲਤ vehicle_ਸਥਿਤੀ_s::NAVIGATION_STATE_POSCTL: ਵਾਪਸ ਝੂਠ;

	ਹਾਲਤ vehicle_ਸਥਿਤੀ_s::NAVIGATION_STATE_EXTERNAL1: ਵਾਪਸ ਝੂਠ;

	ਹਾਲਤ vehicle_ਸਥਿਤੀ_s::NAVIGATION_STATE_EXTERNAL2: ਵਾਪਸ ਝੂਠ;

	ਹਾਲਤ vehicle_ਸਥਿਤੀ_s::NAVIGATION_STATE_EXTERNAL3: ਵਾਪਸ ਝੂਠ;

	ਹਾਲਤ vehicle_ਸਥਿਤੀ_s::NAVIGATION_STATE_EXTERNAL4: ਵਾਪਸ ਝੂਠ;

	ਹਾਲਤ vehicle_ਸਥਿਤੀ_s::NAVIGATION_STATE_EXTERNAL5: ਵਾਪਸ ਝੂਠ;

	ਹਾਲਤ vehicle_ਸਥਿਤੀ_s::NAVIGATION_STATE_EXTERNAL6: ਵਾਪਸ ਝੂਠ;

	ਹਾਲਤ vehicle_ਸਥਿਤੀ_s::NAVIGATION_STATE_EXTERNAL7: ਵਾਪਸ ਝੂਠ;

	ਹਾਲਤ vehicle_ਸਥਿਤੀ_s::NAVIGATION_STATE_EXTERNAL8: ਵਾਪਸ ਝੂਠ;

	}

	ਵਾਪਸ ਸੱਚ;
}

} // namespace mode_util
