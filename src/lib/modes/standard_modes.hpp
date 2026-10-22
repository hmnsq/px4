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

// This matches the definition from MAVLink MAV_STANDARD_MODE
గణన తరగతి Standard_బహుళకం : uint8_t {
	NON_STANDARD = 0,
	POSITION_HOLD = 1,
	ORBIT = 2,
	CRUISE = 3,
	ALTITUDE_HOLD = 4,
	సురక్షిత_RECOVERY = 5,
	MISSION = 6,
	LAND = 7,
	TAKEOFF = 8,
};

/**
 * @return Get MAVLink standard mode from nav_state
 */
స్థిర అంతఃవరుస Standard_బహుళకం get_Standard_బహుళకం_From_Nav_State(uint8_t nav_state, uint8_t vehicle_type, తార్కిక is_vtol)
{
	ఎంపిక (nav_state) {
	స్థితి vehicle_స్థితి_s::NAVIGATION_STATE_AUTO_RTL: తిరిగి Standard_బహుళకం::సురక్షిత_RECOVERY;

	స్థితి vehicle_స్థితి_s::NAVIGATION_STATE_AUTO_MISSION: తిరిగి Standard_బహుళకం::MISSION;

	స్థితి vehicle_స్థితి_s::NAVIGATION_STATE_AUTO_LAND: తిరిగి Standard_బహుళకం::LAND;

	స్థితి vehicle_స్థితి_s::NAVIGATION_STATE_AUTO_TAKEOFF: తిరిగి Standard_బహుళకం::TAKEOFF;

	స్థితి vehicle_స్థితి_s::NAVIGATION_STATE_ALTCTL:
		ఒకవేళ (is_vtol || vehicle_type == vehicle_స్థితి_s::VEHICLE_TYPE_ROTARY_WING
		    || vehicle_type == vehicle_స్థితి_s::VEHICLE_TYPE_FIXED_WING) {
			తిరిగి Standard_బహుళకం::ALTITUDE_HOLD;
		}

		ఆపు;

	స్థితి vehicle_స్థితి_s::NAVIGATION_STATE_POSCTL:
		ఒకవేళ (!is_vtol && vehicle_type == vehicle_స్థితి_s::VEHICLE_TYPE_ROTARY_WING) {
			తిరిగి Standard_బహుళకం::POSITION_HOLD;
		}

		ఒకవేళ (!is_vtol && vehicle_type == vehicle_స్థితి_s::VEHICLE_TYPE_FIXED_WING) {
			తిరిగి Standard_బహుళకం::CRUISE;
		}

		ఆపు;

	స్థితి vehicle_స్థితి_s::NAVIGATION_STATE_ORBIT:
		ఒకవేళ (is_vtol || vehicle_type == vehicle_స్థితి_s::VEHICLE_TYPE_ROTARY_WING
		    || vehicle_type == vehicle_స్థితి_s::VEHICLE_TYPE_FIXED_WING) {
			తిరిగి Standard_బహుళకం::ORBIT;
		}

		ఆపు;
	}

	తిరిగి Standard_బహుళకం::NON_STANDARD;
}

/**
 * @return Get nav_state from a standard mode, or vehicle_status_s::NAVIGATION_STATE_MAX if not supported
 */
స్థిర అంతఃవరుస uint8_t get_Nav_State_From_Standard_బహుళకం(Standard_బహుళకం బహుళకం, uint8_t vehicle_type, తార్కిక is_vtol)
{
	ఎంపిక (బహుళకం) {
	స్థితి Standard_బహుళకం::సురక్షిత_RECOVERY: తిరిగి vehicle_స్థితి_s::NAVIGATION_STATE_AUTO_RTL;

	స్థితి Standard_బహుళకం::MISSION: తిరిగి vehicle_స్థితి_s::NAVIGATION_STATE_AUTO_MISSION;

	స్థితి Standard_బహుళకం::LAND: తిరిగి vehicle_స్థితి_s::NAVIGATION_STATE_AUTO_LAND;

	స్థితి Standard_బహుళకం::TAKEOFF: తిరిగి vehicle_స్థితి_s::NAVIGATION_STATE_AUTO_TAKEOFF;

	స్థితి Standard_బహుళకం::ALTITUDE_HOLD:
		ఒకవేళ (is_vtol || vehicle_type == vehicle_స్థితి_s::VEHICLE_TYPE_ROTARY_WING
		    || vehicle_type == vehicle_స్థితి_s::VEHICLE_TYPE_FIXED_WING) {
			తిరిగి vehicle_స్థితి_s::NAVIGATION_STATE_ALTCTL;
		}

		ఆపు;

	స్థితి Standard_బహుళకం::POSITION_HOLD:
		ఒకవేళ (!is_vtol && vehicle_type == vehicle_స్థితి_s::VEHICLE_TYPE_ROTARY_WING) {
			తిరిగి vehicle_స్థితి_s::NAVIGATION_STATE_POSCTL;
		}

		ఆపు;

	స్థితి Standard_బహుళకం::CRUISE:
		ఒకవేళ (!is_vtol && vehicle_type == vehicle_స్థితి_s::VEHICLE_TYPE_FIXED_WING) {
			తిరిగి vehicle_స్థితి_s::NAVIGATION_STATE_POSCTL;
		}

		ఆపు;

	స్థితి Standard_బహుళకం::ORBIT:
		ఒకవేళ (vehicle_type == vehicle_స్థితి_s::VEHICLE_TYPE_ROTARY_WING) {
			తిరిగి vehicle_స్థితి_s::NAVIGATION_STATE_ORBIT;
		}

		ఒకవేళ (vehicle_type == vehicle_స్థితి_s::VEHICLE_TYPE_FIXED_WING) {
			తిరిగి vehicle_స్థితి_s::NAVIGATION_STATE_AUTO_LOITER;
		}

		ఆపు;

	అప్రమేయ: ఆపు;
	}

	తిరిగి vehicle_స్థితి_s::NAVIGATION_STATE_MAX;
}


} // namespace mode_util
