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

#அந்தர்பாவ "CourseToAirspeedRefMapper.hpp"
using matrix::Vector2f;

தசம
CourseToAirspeedRefMapper::mapCourseSetpointToHeadingSetpoint(மாறிலி தசம bearing_setpoint, மாறிலி Vector2f &wind_vel,
		தசம airspeed_sp) மாறிலி
{
	மாறிலி Vector2f bearing_vector = Vector2f{cosf(bearing_setpoint), sinf(bearing_setpoint)};
	மாறிலி தசம wind_cross_bearing = wind_vel.cross(bearing_vector);
	மாறிலி தசம wind_dot_bearing = wind_vel.dot(bearing_vector);

	Vector2f air_vel_ref;

	எனில் (bearingIsFeasible(wind_cross_bearing, wind_dot_bearing, airspeed_sp, wind_vel.norm())) {

		மாறிலி தசம airsp_dot_bearing = projectAirspOnBearing(airspeed_sp, wind_cross_bearing);
		air_vel_ref = solveWindTriangle(wind_cross_bearing, airsp_dot_bearing, bearing_vector);

	} இல்லையெனில் {
		air_vel_ref = infeasibleAirVelRef(wind_vel, bearing_vector, wind_vel.norm(), airspeed_sp);
	}

	திரும்பு atan2f(air_vel_ref(1), air_vel_ref(0));
}

தசம
CourseToAirspeedRefMapper::getMinAirspeedForCurrentBearing(மாறிலி தசம bearing_setpoint, மாறிலி Vector2f &wind_vel,
		தசம airspeed_max, தசம min_ground_speed) மாறிலி
{
	மாறிலி Vector2f bearing_vector = Vector2f{cosf(bearing_setpoint), sinf(bearing_setpoint)};
	மாறிலி தசம wind_cross_bearing = wind_vel.cross(bearing_vector);
	மாறிலி தசம wind_dot_bearing = wind_vel.dot(bearing_vector);

	மாறிலி தருக்க wind_along_bearing_is_below_min_ground_speed = min_ground_speed > wind_dot_bearing;

	தசம airspeed_min = 0.f; // return 0 if no min airspeed is necessary

	எனில் (wind_along_bearing_is_below_min_ground_speed) {
		// airspeed required to achieve minimum ground speed along bearing vector (5.18)
		airspeed_min = sqrtf((min_ground_speed - wind_dot_bearing) * (min_ground_speed - wind_dot_bearing) +
				     wind_cross_bearing * wind_cross_bearing);

	}

	திரும்பு math::min(airspeed_min, airspeed_max);
}

தசம CourseToAirspeedRefMapper::projectAirspOnBearing(மாறிலி தசம airspeed_true, மாறிலி தசம wind_cross_bearing) மாறிலி
{
	// NOTE: wind_cross_bearing must be less than airspeed to use this function
	// it is assumed that bearing feasibility is checked and found feasible (e.g. bearingIsFeasible() = true) prior to entering this method
	// otherwise the return will be erroneous

	// 3.5.8
	திரும்பு sqrtf(math::max(airspeed_true * airspeed_true - wind_cross_bearing * wind_cross_bearing, 0.0f));
}

முழு CourseToAirspeedRefMapper::bearingIsFeasible(மாறிலி தசம wind_cross_bearing, மாறிலி தசம wind_dot_bearing,
		மாறிலி தசம airspeed, மாறிலி தசம wind_speed) மாறிலி
{
	திரும்பு (fabsf(wind_cross_bearing) < airspeed) && ((wind_dot_bearing > 0.0f) || (wind_speed < airspeed));
}

matrix::Vector2f
CourseToAirspeedRefMapper::solveWindTriangle(மாறிலி தசம wind_cross_bearing, மாறிலி தசம airsp_dot_bearing,
		மாறிலி Vector2f &bearing_vec) மாறிலி
{
	// essentially a 2D rotation with the speeds (magnitudes) baked in
	திரும்பு Vector2f{airsp_dot_bearing * bearing_vec(0) - wind_cross_bearing * bearing_vec(1),
			wind_cross_bearing * bearing_vec(0) + airsp_dot_bearing * bearing_vec(1)};
}

matrix::Vector2f CourseToAirspeedRefMapper::infeasibleAirVelRef(மாறிலி Vector2f &wind_vel, மாறிலி Vector2f &bearing_vec,
		மாறிலி தசம wind_speed, மாறிலி தசம airspeed) மாறிலி
{
	// NOTE: wind speed must be greater than airspeed, and airspeed must be greater than zero to use this function
	// it is assumed that bearing feasibility is checked and found infeasible (e.g. bearingIsFeasible() = false) prior to entering this method
	// otherwise the normalization of the air velocity vector could have a division by zero
	Vector2f air_vel_ref = sqrtf(math::max(wind_speed * wind_speed - airspeed * airspeed, 0.0f)) * bearing_vec - wind_vel;
	திரும்பு air_vel_ref.normalized() * airspeed;
}
