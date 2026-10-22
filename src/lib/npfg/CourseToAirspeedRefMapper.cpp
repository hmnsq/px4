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

#شامل "CourseToAirspeedRefMapper.hpp"
استعمال میٹرکس::Vector2f;

اعشاری
نصاب_مقصد_Airspeed_Ref_Mapper::map_نصاب_Setpoint_مقصد_Heading_Setpoint(ثابت اعشاری bearing_setpoint, ثابت Vector2f &wind_vel,
		اعشاری airspeed_sp) ثابت
{
	ثابت Vector2f bearing_سمتیہ = Vector2f{cosf(bearing_setpoint), sinf(bearing_setpoint)};
	ثابت اعشاری wind_cross_bearing = wind_vel.cross(bearing_سمتیہ);
	ثابت اعشاری wind_dot_bearing = wind_vel.dot(bearing_سمتیہ);

	Vector2f air_vel_ref;

	اگر (bearingIsFeasible(wind_cross_bearing, wind_dot_bearing, airspeed_sp, wind_vel.norm())) {

		ثابت اعشاری airsp_dot_bearing = projectAirspOnBearing(airspeed_sp, wind_cross_bearing);
		air_vel_ref = solveWindTriangle(wind_cross_bearing, airsp_dot_bearing, bearing_سمتیہ);

	} ورنہ {
		air_vel_ref = infeasibleAirVelRef(wind_vel, bearing_سمتیہ, wind_vel.norm(), airspeed_sp);
	}

	واپس atan2f(air_vel_ref(1), air_vel_ref(0));
}

اعشاری
نصاب_مقصد_Airspeed_Ref_Mapper::getMinAirspeedForCurrentBearing(ثابت اعشاری bearing_setpoint, ثابت Vector2f &wind_vel,
		اعشاری airspeed_max, اعشاری min_ground_رفتار) ثابت
{
	ثابت Vector2f bearing_سمتیہ = Vector2f{cosf(bearing_setpoint), sinf(bearing_setpoint)};
	ثابت اعشاری wind_cross_bearing = wind_vel.cross(bearing_سمتیہ);
	ثابت اعشاری wind_dot_bearing = wind_vel.dot(bearing_سمتیہ);

	ثابت منطقی wind_along_bearing_is_نیچے_min_ground_رفتار = min_ground_رفتار > wind_dot_bearing;

	اعشاری airspeed_min = 0.f; // return 0 if no min airspeed is necessary

	اگر (wind_along_bearing_is_نیچے_min_ground_رفتار) {
		// airspeed required to achieve minimum ground speed along bearing vector (5.18)
		airspeed_min = sqrtf((min_ground_رفتار - wind_dot_bearing) * (min_ground_رفتار - wind_dot_bearing) +
				     wind_cross_bearing * wind_cross_bearing);

	}

	واپس math::min(airspeed_min, airspeed_max);
}

اعشاری نصاب_مقصد_Airspeed_Ref_Mapper::projectAirspOnBearing(ثابت اعشاری airspeed_true, ثابت اعشاری wind_cross_bearing) ثابت
{
	// NOTE: wind_cross_bearing must be less than airspeed to use this function
	// it is assumed that bearing feasibility is checked and found feasible (e.g. bearingIsFeasible() = true) prior to entering this method
	// otherwise the return will be erroneous

	// 3.5.8
	واپس sqrtf(math::max(airspeed_true * airspeed_true - wind_cross_bearing * wind_cross_bearing, 0.0f));
}

صحیح نصاب_مقصد_Airspeed_Ref_Mapper::bearingIsFeasible(ثابت اعشاری wind_cross_bearing, ثابت اعشاری wind_dot_bearing,
		ثابت اعشاری airspeed, ثابت اعشاری wind_رفتار) ثابت
{
	واپس (fabsf(wind_cross_bearing) < airspeed) && ((wind_dot_bearing > 0.0f) || (wind_رفتار < airspeed));
}

میٹرکس::Vector2f
نصاب_مقصد_Airspeed_Ref_Mapper::solveWindTriangle(ثابت اعشاری wind_cross_bearing, ثابت اعشاری airsp_dot_bearing,
		ثابت Vector2f &bearing_vec) ثابت
{
	// essentially a 2D rotation with the speeds (magnitudes) baked in
	واپس Vector2f{airsp_dot_bearing * bearing_vec(0) - wind_cross_bearing * bearing_vec(1),
			wind_cross_bearing * bearing_vec(0) + airsp_dot_bearing * bearing_vec(1)};
}

میٹرکس::Vector2f نصاب_مقصد_Airspeed_Ref_Mapper::infeasibleAirVelRef(ثابت Vector2f &wind_vel, ثابت Vector2f &bearing_vec,
		ثابت اعشاری wind_رفتار, ثابت اعشاری airspeed) ثابت
{
	// NOTE: wind speed must be greater than airspeed, and airspeed must be greater than zero to use this function
	// it is assumed that bearing feasibility is checked and found infeasible (e.g. bearingIsFeasible() = false) prior to entering this method
	// otherwise the normalization of the air velocity vector could have a division by zero
	Vector2f air_vel_ref = sqrtf(math::max(wind_رفتار * wind_رفتار - airspeed * airspeed, 0.0f)) * bearing_vec - wind_vel;
	واپس air_vel_ref.normalized() * airspeed;
}
