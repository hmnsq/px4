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

#ہدایت once

#شامل <uORB/topics/vehicle_status.h>

#شامل <stdint.h>

نامخلا منوال_util
{

// This matches the definition from MAVLink MAV_STANDARD_MODE
شمار جماعت Standard_منوال : uint8_t {
	NON_STANDARD = 0,
	POSITION_HOLD = 1,
	ORBIT = 2,
	CRUISE = 3,
	ALTITUDE_HOLD = 4,
	محفوظ_RECOVERY = 5,
	MISSION = 6,
	LAND = 7,
	TAKEOFF = 8,
};

/**
 * @return Get MAVLink standard mode from nav_state
 */
ساکن اندرونی Standard_منوال get_Standard_منوال_From_Nav_State(uint8_t nav_state, uint8_t vehicle_type, منطقی is_vtol)
{
	انتخاب (nav_state) {
	حالت vehicle_حالت_s::NAVIGATION_STATE_AUTO_RTL: واپس Standard_منوال::محفوظ_RECOVERY;

	حالت vehicle_حالت_s::NAVIGATION_STATE_AUTO_MISSION: واپس Standard_منوال::MISSION;

	حالت vehicle_حالت_s::NAVIGATION_STATE_AUTO_LAND: واپس Standard_منوال::LAND;

	حالت vehicle_حالت_s::NAVIGATION_STATE_AUTO_TAKEOFF: واپس Standard_منوال::TAKEOFF;

	حالت vehicle_حالت_s::NAVIGATION_STATE_ALTCTL:
		اگر (is_vtol || vehicle_type == vehicle_حالت_s::VEHICLE_TYPE_ROTARY_WING
		    || vehicle_type == vehicle_حالت_s::VEHICLE_TYPE_FIXED_WING) {
			واپس Standard_منوال::ALTITUDE_HOLD;
		}

		توڑو;

	حالت vehicle_حالت_s::NAVIGATION_STATE_POSCTL:
		اگر (!is_vtol && vehicle_type == vehicle_حالت_s::VEHICLE_TYPE_ROTARY_WING) {
			واپس Standard_منوال::POSITION_HOLD;
		}

		اگر (!is_vtol && vehicle_type == vehicle_حالت_s::VEHICLE_TYPE_FIXED_WING) {
			واپس Standard_منوال::CRUISE;
		}

		توڑو;

	حالت vehicle_حالت_s::NAVIGATION_STATE_ORBIT:
		اگر (is_vtol || vehicle_type == vehicle_حالت_s::VEHICLE_TYPE_ROTARY_WING
		    || vehicle_type == vehicle_حالت_s::VEHICLE_TYPE_FIXED_WING) {
			واپس Standard_منوال::ORBIT;
		}

		توڑو;
	}

	واپس Standard_منوال::NON_STANDARD;
}

/**
 * @return Get nav_state from a standard mode, or vehicle_status_s::NAVIGATION_STATE_MAX if not supported
 */
ساکن اندرونی uint8_t get_Nav_State_From_Standard_منوال(Standard_منوال منوال, uint8_t vehicle_type, منطقی is_vtol)
{
	انتخاب (منوال) {
	حالت Standard_منوال::محفوظ_RECOVERY: واپس vehicle_حالت_s::NAVIGATION_STATE_AUTO_RTL;

	حالت Standard_منوال::MISSION: واپس vehicle_حالت_s::NAVIGATION_STATE_AUTO_MISSION;

	حالت Standard_منوال::LAND: واپس vehicle_حالت_s::NAVIGATION_STATE_AUTO_LAND;

	حالت Standard_منوال::TAKEOFF: واپس vehicle_حالت_s::NAVIGATION_STATE_AUTO_TAKEOFF;

	حالت Standard_منوال::ALTITUDE_HOLD:
		اگر (is_vtol || vehicle_type == vehicle_حالت_s::VEHICLE_TYPE_ROTARY_WING
		    || vehicle_type == vehicle_حالت_s::VEHICLE_TYPE_FIXED_WING) {
			واپس vehicle_حالت_s::NAVIGATION_STATE_ALTCTL;
		}

		توڑو;

	حالت Standard_منوال::POSITION_HOLD:
		اگر (!is_vtol && vehicle_type == vehicle_حالت_s::VEHICLE_TYPE_ROTARY_WING) {
			واپس vehicle_حالت_s::NAVIGATION_STATE_POSCTL;
		}

		توڑو;

	حالت Standard_منوال::CRUISE:
		اگر (!is_vtol && vehicle_type == vehicle_حالت_s::VEHICLE_TYPE_FIXED_WING) {
			واپس vehicle_حالت_s::NAVIGATION_STATE_POSCTL;
		}

		توڑو;

	حالت Standard_منوال::ORBIT:
		اگر (vehicle_type == vehicle_حالت_s::VEHICLE_TYPE_ROTARY_WING) {
			واپس vehicle_حالت_s::NAVIGATION_STATE_ORBIT;
		}

		اگر (vehicle_type == vehicle_حالت_s::VEHICLE_TYPE_FIXED_WING) {
			واپس vehicle_حالت_s::NAVIGATION_STATE_AUTO_LOITER;
		}

		توڑو;

	طےشدہ: توڑو;
	}

	واپس vehicle_حالت_s::NAVIGATION_STATE_MAX;
}


} // namespace mode_util
