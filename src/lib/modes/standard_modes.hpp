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

#निर्देशः once

#अन्तर्भाव <uORB/topics/vehicle_status.h>

#अन्तर्भाव <stdint.h>

नामाकाश बहुलकम्_util
{

// This matches the definition from MAVLink MAV_STANDARD_MODE
गणना वर्ग Standard_बहुलकम् : uint8_t {
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
स्थिर अन्तःस्थ Standard_बहुलकम् get_Standard_बहुलकम्_From_Nav_State(uint8_t nav_state, uint8_t vehicle_type, तार्किक is_vtol)
{
	विकल्प (nav_state) {
	पक्ष vehicle_स्थितिः_s::NAVIGATION_STATE_AUTO_RTL: प्रत्यागम Standard_बहुलकम्::SAFE_RECOVERY;

	पक्ष vehicle_स्थितिः_s::NAVIGATION_STATE_AUTO_MISSION: प्रत्यागम Standard_बहुलकम्::MISSION;

	पक्ष vehicle_स्थितिः_s::NAVIGATION_STATE_AUTO_LAND: प्रत्यागम Standard_बहुलकम्::LAND;

	पक्ष vehicle_स्थितिः_s::NAVIGATION_STATE_AUTO_TAKEOFF: प्रत्यागम Standard_बहुलकम्::TAKEOFF;

	पक्ष vehicle_स्थितिः_s::NAVIGATION_STATE_ALTCTL:
		यदि (is_vtol || vehicle_type == vehicle_स्थितिः_s::VEHICLE_TYPE_ROTARY_WING
		    || vehicle_type == vehicle_स्थितिः_s::VEHICLE_TYPE_FIXED_WING) {
			प्रत्यागम Standard_बहुलकम्::ALTITUDE_HOLD;
		}

		भङ्ग;

	पक्ष vehicle_स्थितिः_s::NAVIGATION_STATE_POSCTL:
		यदि (!is_vtol && vehicle_type == vehicle_स्थितिः_s::VEHICLE_TYPE_ROTARY_WING) {
			प्रत्यागम Standard_बहुलकम्::POSITION_HOLD;
		}

		यदि (!is_vtol && vehicle_type == vehicle_स्थितिः_s::VEHICLE_TYPE_FIXED_WING) {
			प्रत्यागम Standard_बहुलकम्::CRUISE;
		}

		भङ्ग;

	पक्ष vehicle_स्थितिः_s::NAVIGATION_STATE_ORBIT:
		यदि (is_vtol || vehicle_type == vehicle_स्थितिः_s::VEHICLE_TYPE_ROTARY_WING
		    || vehicle_type == vehicle_स्थितिः_s::VEHICLE_TYPE_FIXED_WING) {
			प्रत्यागम Standard_बहुलकम्::ORBIT;
		}

		भङ्ग;
	}

	प्रत्यागम Standard_बहुलकम्::NON_STANDARD;
}

/**
 * @return Get nav_state from a standard mode, or vehicle_status_s::NAVIGATION_STATE_MAX if not supported
 */
स्थिर अन्तःस्थ uint8_t get_Nav_State_From_Standard_बहुलकम्(Standard_बहुलकम् बहुलकम्, uint8_t vehicle_type, तार्किक is_vtol)
{
	विकल्प (बहुलकम्) {
	पक्ष Standard_बहुलकम्::SAFE_RECOVERY: प्रत्यागम vehicle_स्थितिः_s::NAVIGATION_STATE_AUTO_RTL;

	पक्ष Standard_बहुलकम्::MISSION: प्रत्यागम vehicle_स्थितिः_s::NAVIGATION_STATE_AUTO_MISSION;

	पक्ष Standard_बहुलकम्::LAND: प्रत्यागम vehicle_स्थितिः_s::NAVIGATION_STATE_AUTO_LAND;

	पक्ष Standard_बहुलकम्::TAKEOFF: प्रत्यागम vehicle_स्थितिः_s::NAVIGATION_STATE_AUTO_TAKEOFF;

	पक्ष Standard_बहुलकम्::ALTITUDE_HOLD:
		यदि (is_vtol || vehicle_type == vehicle_स्थितिः_s::VEHICLE_TYPE_ROTARY_WING
		    || vehicle_type == vehicle_स्थितिः_s::VEHICLE_TYPE_FIXED_WING) {
			प्रत्यागम vehicle_स्थितिः_s::NAVIGATION_STATE_ALTCTL;
		}

		भङ्ग;

	पक्ष Standard_बहुलकम्::POSITION_HOLD:
		यदि (!is_vtol && vehicle_type == vehicle_स्थितिः_s::VEHICLE_TYPE_ROTARY_WING) {
			प्रत्यागम vehicle_स्थितिः_s::NAVIGATION_STATE_POSCTL;
		}

		भङ्ग;

	पक्ष Standard_बहुलकम्::CRUISE:
		यदि (!is_vtol && vehicle_type == vehicle_स्थितिः_s::VEHICLE_TYPE_FIXED_WING) {
			प्रत्यागम vehicle_स्थितिः_s::NAVIGATION_STATE_POSCTL;
		}

		भङ्ग;

	पक्ष Standard_बहुलकम्::ORBIT:
		यदि (vehicle_type == vehicle_स्थितिः_s::VEHICLE_TYPE_ROTARY_WING) {
			प्रत्यागम vehicle_स्थितिः_s::NAVIGATION_STATE_ORBIT;
		}

		यदि (vehicle_type == vehicle_स्थितिः_s::VEHICLE_TYPE_FIXED_WING) {
			प्रत्यागम vehicle_स्थितिः_s::NAVIGATION_STATE_AUTO_LOITER;
		}

		भङ्ग;

	स्वभाव: भङ्ग;
	}

	प्रत्यागम vehicle_स्थितिः_s::NAVIGATION_STATE_MAX;
}


} // namespace mode_util
