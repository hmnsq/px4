/****************************************************************************
 *
 * Copyright (c) 2025 PX4 Development Team. All rights reserved.
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

#ಅನ್ತರ್ಭಾವ "CourseToAirspeedRefMapper.hpp"
ಬಳಸಿ ಮಾತೃಕೆ::Vector2f;

ದಶಮಾಂಶ
ಕೋರ್ಸು_ಗಮ್ಯ_Airspeed_Ref_Mapper::map_ಕೋರ್ಸು_Setpoint_ಗಮ್ಯ_Heading_Setpoint(ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ bearing_setpoint, ಸ್ಥಿರಾಂಕ Vector2f &wind_vel,
		ದಶಮಾಂಶ airspeed_sp) ಸ್ಥಿರಾಂಕ
{
	ಸ್ಥಿರಾಂಕ Vector2f bearing_ಸದಿಶ = Vector2f{cosf(bearing_setpoint), sinf(bearing_setpoint)};
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ wind_cross_bearing = wind_vel.cross(bearing_ಸದಿಶ);
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ wind_dot_bearing = wind_vel.dot(bearing_ಸದಿಶ);

	Vector2f air_vel_ref;

	ಆದರೆ (bearingIsFeasible(wind_cross_bearing, wind_dot_bearing, airspeed_sp, wind_vel.norm())) {

		ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ airsp_dot_bearing = projectAirspOnBearing(airspeed_sp, wind_cross_bearing);
		air_vel_ref = solveWindTriangle(wind_cross_bearing, airsp_dot_bearing, bearing_ಸದಿಶ);

	} ಇಲ್ಲದಿದ್ದರೆ {
		air_vel_ref = infeasibleAirVelRef(wind_vel, bearing_ಸದಿಶ, wind_vel.norm(), airspeed_sp);
	}

	ಹಿಂತಿರುಗಿ atan2f(air_vel_ref(1), air_vel_ref(0));
}

ದಶಮಾಂಶ
ಕೋರ್ಸು_ಗಮ್ಯ_Airspeed_Ref_Mapper::getMinAirspeedForCurrentBearing(ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ bearing_setpoint, ಸ್ಥಿರಾಂಕ Vector2f &wind_vel,
		ದಶಮಾಂಶ airspeed_max, ದಶಮಾಂಶ min_ground_ವೇಗ) ಸ್ಥಿರಾಂಕ
{
	ಸ್ಥಿರಾಂಕ Vector2f bearing_ಸದಿಶ = Vector2f{cosf(bearing_setpoint), sinf(bearing_setpoint)};
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ wind_cross_bearing = wind_vel.cross(bearing_ಸದಿಶ);
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ wind_dot_bearing = wind_vel.dot(bearing_ಸದಿಶ);

	ಸ್ಥಿರಾಂಕ ತಾರ್ಕಿಕ wind_along_bearing_is_ಕೆಳಗೆ_min_ground_ವೇಗ = min_ground_ವೇಗ > wind_dot_bearing;

	ದಶಮಾಂಶ airspeed_min = 0.f; // return 0 if no min airspeed is necessary

	ಆದರೆ (wind_along_bearing_is_ಕೆಳಗೆ_min_ground_ವೇಗ) {
		// airspeed required to achieve minimum ground speed along bearing vector (5.18)
		airspeed_min = sqrtf((min_ground_ವೇಗ - wind_dot_bearing) * (min_ground_ವೇಗ - wind_dot_bearing) +
				     wind_cross_bearing * wind_cross_bearing);

	}

	ಹಿಂತಿರುಗಿ math::min(airspeed_min, airspeed_max);
}

ದಶಮಾಂಶ ಕೋರ್ಸು_ಗಮ್ಯ_Airspeed_Ref_Mapper::projectAirspOnBearing(ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ airspeed_true, ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ wind_cross_bearing) ಸ್ಥಿರಾಂಕ
{
	// NOTE: wind_cross_bearing must be less than airspeed to use this function
	// it is assumed that bearing feasibility is checked and found feasible (e.g. bearingIsFeasible() = true) prior to entering this method
	// otherwise the return will be erroneous

	// 3.5.8
	ಹಿಂತಿರುಗಿ sqrtf(math::max(airspeed_true * airspeed_true - wind_cross_bearing * wind_cross_bearing, 0.0f));
}

ಪೂರ್ಣಾಂಕ ಕೋರ್ಸು_ಗಮ್ಯ_Airspeed_Ref_Mapper::bearingIsFeasible(ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ wind_cross_bearing, ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ wind_dot_bearing,
		ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ airspeed, ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ wind_ವೇಗ) ಸ್ಥಿರಾಂಕ
{
	ಹಿಂತಿರುಗಿ (fabsf(wind_cross_bearing) < airspeed) && ((wind_dot_bearing > 0.0f) || (wind_ವೇಗ < airspeed));
}

ಮಾತೃಕೆ::Vector2f
ಕೋರ್ಸು_ಗಮ್ಯ_Airspeed_Ref_Mapper::solveWindTriangle(ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ wind_cross_bearing, ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ airsp_dot_bearing,
		ಸ್ಥಿರಾಂಕ Vector2f &bearing_vec) ಸ್ಥಿರಾಂಕ
{
	// essentially a 2D rotation with the speeds (magnitudes) baked in
	ಹಿಂತಿರುಗಿ Vector2f{airsp_dot_bearing * bearing_vec(0) - wind_cross_bearing * bearing_vec(1),
			wind_cross_bearing * bearing_vec(0) + airsp_dot_bearing * bearing_vec(1)};
}

ಮಾತೃಕೆ::Vector2f ಕೋರ್ಸು_ಗಮ್ಯ_Airspeed_Ref_Mapper::infeasibleAirVelRef(ಸ್ಥಿರಾಂಕ Vector2f &wind_vel, ಸ್ಥಿರಾಂಕ Vector2f &bearing_vec,
		ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ wind_ವೇಗ, ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ airspeed) ಸ್ಥಿರಾಂಕ
{
	// NOTE: wind speed must be greater than airspeed, and airspeed must be greater than zero to use this function
	// it is assumed that bearing feasibility is checked and found infeasible (e.g. bearingIsFeasible() = false) prior to entering this method
	// otherwise the normalization of the air velocity vector could have a division by zero
	Vector2f air_vel_ref = sqrtf(math::max(wind_ವೇಗ * wind_ವೇಗ - airspeed * airspeed, 0.0f)) * bearing_vec - wind_vel;
	ಹಿಂತಿರುಗಿ air_vel_ref.normalized() * airspeed;
}
