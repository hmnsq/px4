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

// This matches the definition from MAVLink MAV_STANDARD_MODE
ਗਿਣਤੀ ਵਰਗ Standard_ਬਹੁਲਕ : uint8_t {
	NON_STANDARD = 0,
	POSITION_HOLD = 1,
	ORBIT = 2,
	CRUISE = 3,
	ALTITUDE_HOLD = 4,
	ਸੁਰੱਖਿਅਤ_RECOVERY = 5,
	MISSION = 6,
	LAND = 7,
	TAKEOFF = 8,
};

/**
 * @return Get MAVLink standard mode from nav_state
 */
ਸਥਿਰ ਸਤਰਅੰਦਰ Standard_ਬਹੁਲਕ get_Standard_ਬਹੁਲਕ_From_Nav_State(uint8_t nav_state, uint8_t vehicle_type, ਤਾਰਕਿਕ is_vtol)
{
	ਚੋਣ (nav_state) {
	ਹਾਲਤ vehicle_ਸਥਿਤੀ_s::NAVIGATION_STATE_AUTO_RTL: ਵਾਪਸ Standard_ਬਹੁਲਕ::ਸੁਰੱਖਿਅਤ_RECOVERY;

	ਹਾਲਤ vehicle_ਸਥਿਤੀ_s::NAVIGATION_STATE_AUTO_MISSION: ਵਾਪਸ Standard_ਬਹੁਲਕ::MISSION;

	ਹਾਲਤ vehicle_ਸਥਿਤੀ_s::NAVIGATION_STATE_AUTO_LAND: ਵਾਪਸ Standard_ਬਹੁਲਕ::LAND;

	ਹਾਲਤ vehicle_ਸਥਿਤੀ_s::NAVIGATION_STATE_AUTO_TAKEOFF: ਵਾਪਸ Standard_ਬਹੁਲਕ::TAKEOFF;

	ਹਾਲਤ vehicle_ਸਥਿਤੀ_s::NAVIGATION_STATE_ALTCTL:
		ਜੇ (is_vtol || vehicle_type == vehicle_ਸਥਿਤੀ_s::VEHICLE_TYPE_ROTARY_WING
		    || vehicle_type == vehicle_ਸਥਿਤੀ_s::VEHICLE_TYPE_FIXED_WING) {
			ਵਾਪਸ Standard_ਬਹੁਲਕ::ALTITUDE_HOLD;
		}

		ਰੋਕੋ;

	ਹਾਲਤ vehicle_ਸਥਿਤੀ_s::NAVIGATION_STATE_POSCTL:
		ਜੇ (!is_vtol && vehicle_type == vehicle_ਸਥਿਤੀ_s::VEHICLE_TYPE_ROTARY_WING) {
			ਵਾਪਸ Standard_ਬਹੁਲਕ::POSITION_HOLD;
		}

		ਜੇ (!is_vtol && vehicle_type == vehicle_ਸਥਿਤੀ_s::VEHICLE_TYPE_FIXED_WING) {
			ਵਾਪਸ Standard_ਬਹੁਲਕ::CRUISE;
		}

		ਰੋਕੋ;

	ਹਾਲਤ vehicle_ਸਥਿਤੀ_s::NAVIGATION_STATE_ORBIT:
		ਜੇ (is_vtol || vehicle_type == vehicle_ਸਥਿਤੀ_s::VEHICLE_TYPE_ROTARY_WING
		    || vehicle_type == vehicle_ਸਥਿਤੀ_s::VEHICLE_TYPE_FIXED_WING) {
			ਵਾਪਸ Standard_ਬਹੁਲਕ::ORBIT;
		}

		ਰੋਕੋ;
	}

	ਵਾਪਸ Standard_ਬਹੁਲਕ::NON_STANDARD;
}

/**
 * @return Get nav_state from a standard mode, or vehicle_status_s::NAVIGATION_STATE_MAX if not supported
 */
ਸਥਿਰ ਸਤਰਅੰਦਰ uint8_t get_Nav_State_From_Standard_ਬਹੁਲਕ(Standard_ਬਹੁਲਕ ਬਹੁਲਕ, uint8_t vehicle_type, ਤਾਰਕਿਕ is_vtol)
{
	ਚੋਣ (ਬਹੁਲਕ) {
	ਹਾਲਤ Standard_ਬਹੁਲਕ::ਸੁਰੱਖਿਅਤ_RECOVERY: ਵਾਪਸ vehicle_ਸਥਿਤੀ_s::NAVIGATION_STATE_AUTO_RTL;

	ਹਾਲਤ Standard_ਬਹੁਲਕ::MISSION: ਵਾਪਸ vehicle_ਸਥਿਤੀ_s::NAVIGATION_STATE_AUTO_MISSION;

	ਹਾਲਤ Standard_ਬਹੁਲਕ::LAND: ਵਾਪਸ vehicle_ਸਥਿਤੀ_s::NAVIGATION_STATE_AUTO_LAND;

	ਹਾਲਤ Standard_ਬਹੁਲਕ::TAKEOFF: ਵਾਪਸ vehicle_ਸਥਿਤੀ_s::NAVIGATION_STATE_AUTO_TAKEOFF;

	ਹਾਲਤ Standard_ਬਹੁਲਕ::ALTITUDE_HOLD:
		ਜੇ (is_vtol || vehicle_type == vehicle_ਸਥਿਤੀ_s::VEHICLE_TYPE_ROTARY_WING
		    || vehicle_type == vehicle_ਸਥਿਤੀ_s::VEHICLE_TYPE_FIXED_WING) {
			ਵਾਪਸ vehicle_ਸਥਿਤੀ_s::NAVIGATION_STATE_ALTCTL;
		}

		ਰੋਕੋ;

	ਹਾਲਤ Standard_ਬਹੁਲਕ::POSITION_HOLD:
		ਜੇ (!is_vtol && vehicle_type == vehicle_ਸਥਿਤੀ_s::VEHICLE_TYPE_ROTARY_WING) {
			ਵਾਪਸ vehicle_ਸਥਿਤੀ_s::NAVIGATION_STATE_POSCTL;
		}

		ਰੋਕੋ;

	ਹਾਲਤ Standard_ਬਹੁਲਕ::CRUISE:
		ਜੇ (!is_vtol && vehicle_type == vehicle_ਸਥਿਤੀ_s::VEHICLE_TYPE_FIXED_WING) {
			ਵਾਪਸ vehicle_ਸਥਿਤੀ_s::NAVIGATION_STATE_POSCTL;
		}

		ਰੋਕੋ;

	ਹਾਲਤ Standard_ਬਹੁਲਕ::ORBIT:
		ਜੇ (vehicle_type == vehicle_ਸਥਿਤੀ_s::VEHICLE_TYPE_ROTARY_WING) {
			ਵਾਪਸ vehicle_ਸਥਿਤੀ_s::NAVIGATION_STATE_ORBIT;
		}

		ਜੇ (vehicle_type == vehicle_ਸਥਿਤੀ_s::VEHICLE_TYPE_FIXED_WING) {
			ਵਾਪਸ vehicle_ਸਥਿਤੀ_s::NAVIGATION_STATE_AUTO_LOITER;
		}

		ਰੋਕੋ;

	ਮੂਲ: ਰੋਕੋ;
	}

	ਵਾਪਸ vehicle_ਸਥਿਤੀ_s::NAVIGATION_STATE_MAX;
}


} // namespace mode_util
