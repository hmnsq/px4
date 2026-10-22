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

#సూచన once

#అన్తర్భావ <uORB/topics/vehicle_status.h>

#అన్తర్భావ <stdint.h>

నామాకాశ బహుళకం_util
{

/**
 * @return Bitmask with all valid modes
 */
స్థిర అంతఃవరుస uint32_t get_Valid_Nav_స్థితులు(uint32_t బహుళకం_not_supported = 0)
{
	తిరిగి ((1u << vehicle_స్థితి_s::NAVIGATION_STATE_MANUAL) |
		(1u << vehicle_స్థితి_s::NAVIGATION_STATE_ALTCTL) |
		(1u << vehicle_స్థితి_s::NAVIGATION_STATE_ALTITUDE_CRUISE) |
		(1u << vehicle_స్థితి_s::NAVIGATION_STATE_MANUAL_PARKING) |
		(1u << vehicle_స్థితి_s::NAVIGATION_STATE_POSCTL) |
		(1u << vehicle_స్థితి_s::NAVIGATION_STATE_AUTO_MISSION) |
		(1u << vehicle_స్థితి_s::NAVIGATION_STATE_AUTO_LOITER) |
		(1u << vehicle_స్థితి_s::NAVIGATION_STATE_GUIDED_కోర్సు) |
		(1u << vehicle_స్థితి_s::NAVIGATION_STATE_AUTO_RTL) |
		(1u << vehicle_స్థితి_s::NAVIGATION_STATE_POSITION_SLOW) |
		(1u << vehicle_స్థితి_s::NAVIGATION_STATE_ACRO) |
		(1u << vehicle_స్థితి_s::NAVIGATION_STATE_TERMINATION) |
		(1u << vehicle_స్థితి_s::NAVIGATION_STATE_OFFBOARD) |
		(1u << vehicle_స్థితి_s::NAVIGATION_STATE_STAB) |
		(1u << vehicle_స్థితి_s::NAVIGATION_STATE_AUTO_TAKEOFF) |
		(1u << vehicle_స్థితి_s::NAVIGATION_STATE_AUTO_LAND) |
		(1u << vehicle_స్థితి_s::NAVIGATION_STATE_DESCEND) |
		(1u << vehicle_స్థితి_s::NAVIGATION_STATE_AUTO_FOLLOW_లక్ష్యం) |
		(1u << vehicle_స్థితి_s::NAVIGATION_STATE_AUTO_PRECLAND) |
		(1u << vehicle_స్థితి_s::NAVIGATION_STATE_ORBIT) |
		(1u << vehicle_స్థితి_s::NAVIGATION_STATE_AUTO_VTOL_TAKEOFF)) & ~బహుళకం_not_supported;

	static_assert(vehicle_స్థితి_s::NAVIGATION_STATE_MAX  == 31, "update valid nav states");
}

అంతఃవరుస స్థిరరాశి అక్షరం *స్థిరరాశి nav_state_పేర్లు[vehicle_స్థితి_s::NAVIGATION_STATE_MAX] = {
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
స్థిర అంతఃవరుస స్థిరరాశి అక్షరం *nav_state_name(uint8_t nav_state, uint8_t vehicle_type)
{
	ఒకవేళ (nav_state >= vehicle_స్థితి_s::NAVIGATION_STATE_MAX) {
		తిరిగి "(unknown)";
	}

	ఒకవేళ (nav_state == vehicle_స్థితి_s::NAVIGATION_STATE_POSCTL
	    && vehicle_type == vehicle_స్థితి_s::VEHICLE_TYPE_FIXED_WING) {
		తిరిగి "Cruise";
	}

	తిరిగి nav_state_పేర్లు[nav_state];
}

/**
 * @return returns true for advanced modes
 */
స్థిర అంతఃవరుస తార్కిక isAdvanced(uint8_t nav_state)
{
	ఎంపిక (nav_state) {
	స్థితి vehicle_స్థితి_s::NAVIGATION_STATE_ALTCTL: తిరిగి అబద్ధం;

	స్థితి vehicle_స్థితి_s::NAVIGATION_STATE_POSCTL: తిరిగి అబద్ధం;

	స్థితి vehicle_స్థితి_s::NAVIGATION_STATE_EXTERNAL1: తిరిగి అబద్ధం;

	స్థితి vehicle_స్థితి_s::NAVIGATION_STATE_EXTERNAL2: తిరిగి అబద్ధం;

	స్థితి vehicle_స్థితి_s::NAVIGATION_STATE_EXTERNAL3: తిరిగి అబద్ధం;

	స్థితి vehicle_స్థితి_s::NAVIGATION_STATE_EXTERNAL4: తిరిగి అబద్ధం;

	స్థితి vehicle_స్థితి_s::NAVIGATION_STATE_EXTERNAL5: తిరిగి అబద్ధం;

	స్థితి vehicle_స్థితి_s::NAVIGATION_STATE_EXTERNAL6: తిరిగి అబద్ధం;

	స్థితి vehicle_స్థితి_s::NAVIGATION_STATE_EXTERNAL7: తిరిగి అబద్ధం;

	స్థితి vehicle_స్థితి_s::NAVIGATION_STATE_EXTERNAL8: తిరిగి అబద్ధం;

	}

	తిరిగి నిజం;
}

} // namespace mode_util
