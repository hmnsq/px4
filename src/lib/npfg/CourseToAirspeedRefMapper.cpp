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

#অন্তর্ভাব "CourseToAirspeedRefMapper.hpp"
using matrix::Vector2f;

ভিন্ন
CourseToAirspeedRefMapper::mapCourseSetpointToHeadingSetpoint(ধ্রুব ভিন্ন bearing_setpoint, ধ্রুব Vector2f &wind_vel,
		ভিন্ন airspeed_sp) ধ্রুব
{
	ধ্রুব Vector2f bearing_vector = Vector2f{cosf(bearing_setpoint), sinf(bearing_setpoint)};
	ধ্রুব ভিন্ন wind_cross_bearing = wind_vel.cross(bearing_vector);
	ধ্রুব ভিন্ন wind_dot_bearing = wind_vel.dot(bearing_vector);

	Vector2f air_vel_ref;

	যদি (bearingIsFeasible(wind_cross_bearing, wind_dot_bearing, airspeed_sp, wind_vel.norm())) {

		ধ্রুব ভিন্ন airsp_dot_bearing = projectAirspOnBearing(airspeed_sp, wind_cross_bearing);
		air_vel_ref = solveWindTriangle(wind_cross_bearing, airsp_dot_bearing, bearing_vector);

	} অন্যথা {
		air_vel_ref = infeasibleAirVelRef(wind_vel, bearing_vector, wind_vel.norm(), airspeed_sp);
	}

	প্রত্যাগম atan2f(air_vel_ref(1), air_vel_ref(0));
}

ভিন্ন
CourseToAirspeedRefMapper::getMinAirspeedForCurrentBearing(ধ্রুব ভিন্ন bearing_setpoint, ধ্রুব Vector2f &wind_vel,
		ভিন্ন airspeed_max, ভিন্ন min_ground_speed) ধ্রুব
{
	ধ্রুব Vector2f bearing_vector = Vector2f{cosf(bearing_setpoint), sinf(bearing_setpoint)};
	ধ্রুব ভিন্ন wind_cross_bearing = wind_vel.cross(bearing_vector);
	ধ্রুব ভিন্ন wind_dot_bearing = wind_vel.dot(bearing_vector);

	ধ্রুব তার্কিক wind_along_bearing_is_below_min_ground_speed = min_ground_speed > wind_dot_bearing;

	ভিন্ন airspeed_min = 0.f; // return 0 if no min airspeed is necessary

	যদি (wind_along_bearing_is_below_min_ground_speed) {
		// airspeed required to achieve minimum ground speed along bearing vector (5.18)
		airspeed_min = sqrtf((min_ground_speed - wind_dot_bearing) * (min_ground_speed - wind_dot_bearing) +
				     wind_cross_bearing * wind_cross_bearing);

	}

	প্রত্যাগম math::min(airspeed_min, airspeed_max);
}

ভিন্ন CourseToAirspeedRefMapper::projectAirspOnBearing(ধ্রুব ভিন্ন airspeed_true, ধ্রুব ভিন্ন wind_cross_bearing) ধ্রুব
{
	// NOTE: wind_cross_bearing must be less than airspeed to use this function
	// it is assumed that bearing feasibility is checked and found feasible (e.g. bearingIsFeasible() = true) prior to entering this method
	// otherwise the return will be erroneous

	// 3.5.8
	প্রত্যাগম sqrtf(math::max(airspeed_true * airspeed_true - wind_cross_bearing * wind_cross_bearing, 0.0f));
}

পূর্ণাঙ্ক CourseToAirspeedRefMapper::bearingIsFeasible(ধ্রুব ভিন্ন wind_cross_bearing, ধ্রুব ভিন্ন wind_dot_bearing,
		ধ্রুব ভিন্ন airspeed, ধ্রুব ভিন্ন wind_speed) ধ্রুব
{
	প্রত্যাগম (fabsf(wind_cross_bearing) < airspeed) && ((wind_dot_bearing > 0.0f) || (wind_speed < airspeed));
}

matrix::Vector2f
CourseToAirspeedRefMapper::solveWindTriangle(ধ্রুব ভিন্ন wind_cross_bearing, ধ্রুব ভিন্ন airsp_dot_bearing,
		ধ্রুব Vector2f &bearing_vec) ধ্রুব
{
	// essentially a 2D rotation with the speeds (magnitudes) baked in
	প্রত্যাগম Vector2f{airsp_dot_bearing * bearing_vec(0) - wind_cross_bearing * bearing_vec(1),
			wind_cross_bearing * bearing_vec(0) + airsp_dot_bearing * bearing_vec(1)};
}

matrix::Vector2f CourseToAirspeedRefMapper::infeasibleAirVelRef(ধ্রুব Vector2f &wind_vel, ধ্রুব Vector2f &bearing_vec,
		ধ্রুব ভিন্ন wind_speed, ধ্রুব ভিন্ন airspeed) ধ্রুব
{
	// NOTE: wind speed must be greater than airspeed, and airspeed must be greater than zero to use this function
	// it is assumed that bearing feasibility is checked and found infeasible (e.g. bearingIsFeasible() = false) prior to entering this method
	// otherwise the normalization of the air velocity vector could have a division by zero
	Vector2f air_vel_ref = sqrtf(math::max(wind_speed * wind_speed - airspeed * airspeed, 0.0f)) * bearing_vec - wind_vel;
	প্রত্যাগম air_vel_ref.normalized() * airspeed;
}
