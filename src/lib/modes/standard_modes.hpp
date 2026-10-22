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

#لارښوونه once

#ضمن <uORB/topics/vehicle_status.h>

#ضمن <stdint.h>

نطاق ډېرپېښ_util
{

// This matches the definition from MAVLink MAV_STANDARD_MODE
تعداد صنف Standard_ډېرپېښ : uint8_t {
	NON_STANDARD = 0,
	POSITION_HOLD = 1,
	ORBIT = 2,
	CRUISE = 3,
	ALTITUDE_HOLD = 4,
	خوندي_RECOVERY = 5,
	MISSION = 6,
	LAND = 7,
	TAKEOFF = 8,
};

/**
 * @return Get MAVLink standard mode from nav_state
 */
ساكن مضمن Standard_ډېرپېښ get_Standard_ډېرپېښ_From_Nav_State(uint8_t nav_state, uint8_t vehicle_type, منطقي is_vtol)
{
	بدل (nav_state) {
	حالة vehicle_حالت_s::NAVIGATION_STATE_AUTO_RTL: أرجع Standard_ډېرپېښ::خوندي_RECOVERY;

	حالة vehicle_حالت_s::NAVIGATION_STATE_AUTO_MISSION: أرجع Standard_ډېرپېښ::MISSION;

	حالة vehicle_حالت_s::NAVIGATION_STATE_AUTO_LAND: أرجع Standard_ډېرپېښ::LAND;

	حالة vehicle_حالت_s::NAVIGATION_STATE_AUTO_TAKEOFF: أرجع Standard_ډېرپېښ::TAKEOFF;

	حالة vehicle_حالت_s::NAVIGATION_STATE_ALTCTL:
		إذا (is_vtol || vehicle_type == vehicle_حالت_s::VEHICLE_TYPE_ROTARY_WING
		    || vehicle_type == vehicle_حالت_s::VEHICLE_TYPE_FIXED_WING) {
			أرجع Standard_ډېرپېښ::ALTITUDE_HOLD;
		}

		اكسر;

	حالة vehicle_حالت_s::NAVIGATION_STATE_POSCTL:
		إذا (!is_vtol && vehicle_type == vehicle_حالت_s::VEHICLE_TYPE_ROTARY_WING) {
			أرجع Standard_ډېرپېښ::POSITION_HOLD;
		}

		إذا (!is_vtol && vehicle_type == vehicle_حالت_s::VEHICLE_TYPE_FIXED_WING) {
			أرجع Standard_ډېرپېښ::CRUISE;
		}

		اكسر;

	حالة vehicle_حالت_s::NAVIGATION_STATE_ORBIT:
		إذا (is_vtol || vehicle_type == vehicle_حالت_s::VEHICLE_TYPE_ROTARY_WING
		    || vehicle_type == vehicle_حالت_s::VEHICLE_TYPE_FIXED_WING) {
			أرجع Standard_ډېرپېښ::ORBIT;
		}

		اكسر;
	}

	أرجع Standard_ډېرپېښ::NON_STANDARD;
}

/**
 * @return Get nav_state from a standard mode, or vehicle_status_s::NAVIGATION_STATE_MAX if not supported
 */
ساكن مضمن uint8_t get_Nav_State_From_Standard_ډېرپېښ(Standard_ډېرپېښ ډېرپېښ, uint8_t vehicle_type, منطقي is_vtol)
{
	بدل (ډېرپېښ) {
	حالة Standard_ډېرپېښ::خوندي_RECOVERY: أرجع vehicle_حالت_s::NAVIGATION_STATE_AUTO_RTL;

	حالة Standard_ډېرپېښ::MISSION: أرجع vehicle_حالت_s::NAVIGATION_STATE_AUTO_MISSION;

	حالة Standard_ډېرپېښ::LAND: أرجع vehicle_حالت_s::NAVIGATION_STATE_AUTO_LAND;

	حالة Standard_ډېرپېښ::TAKEOFF: أرجع vehicle_حالت_s::NAVIGATION_STATE_AUTO_TAKEOFF;

	حالة Standard_ډېرپېښ::ALTITUDE_HOLD:
		إذا (is_vtol || vehicle_type == vehicle_حالت_s::VEHICLE_TYPE_ROTARY_WING
		    || vehicle_type == vehicle_حالت_s::VEHICLE_TYPE_FIXED_WING) {
			أرجع vehicle_حالت_s::NAVIGATION_STATE_ALTCTL;
		}

		اكسر;

	حالة Standard_ډېرپېښ::POSITION_HOLD:
		إذا (!is_vtol && vehicle_type == vehicle_حالت_s::VEHICLE_TYPE_ROTARY_WING) {
			أرجع vehicle_حالت_s::NAVIGATION_STATE_POSCTL;
		}

		اكسر;

	حالة Standard_ډېرپېښ::CRUISE:
		إذا (!is_vtol && vehicle_type == vehicle_حالت_s::VEHICLE_TYPE_FIXED_WING) {
			أرجع vehicle_حالت_s::NAVIGATION_STATE_POSCTL;
		}

		اكسر;

	حالة Standard_ډېرپېښ::ORBIT:
		إذا (vehicle_type == vehicle_حالت_s::VEHICLE_TYPE_ROTARY_WING) {
			أرجع vehicle_حالت_s::NAVIGATION_STATE_ORBIT;
		}

		إذا (vehicle_type == vehicle_حالت_s::VEHICLE_TYPE_FIXED_WING) {
			أرجع vehicle_حالت_s::NAVIGATION_STATE_AUTO_LOITER;
		}

		اكسر;

	افتراضي: اكسر;
	}

	أرجع vehicle_حالت_s::NAVIGATION_STATE_MAX;
}


} // namespace mode_util
