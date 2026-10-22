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

#ਅਨ੍ਤਰ੍ਭਾਵ "CourseToAirspeedRefMapper.hpp"
ਵਰਤੋਂ ਮੈਟ੍ਰਿਕਸ::Vector2f;

ਦਸ਼ਮਲਵ
ਕੋਰਸ_ਮੰਜ਼ਿਲ_Airspeed_Ref_Mapper::map_ਕੋਰਸ_Setpoint_ਮੰਜ਼ਿਲ_Heading_Setpoint(ਅਚਲ ਦਸ਼ਮਲਵ bearing_setpoint, ਅਚਲ Vector2f &wind_vel,
		ਦਸ਼ਮਲਵ airspeed_sp) ਅਚਲ
{
	ਅਚਲ Vector2f bearing_ਸਦਿਸ਼ = Vector2f{cosf(bearing_setpoint), sinf(bearing_setpoint)};
	ਅਚਲ ਦਸ਼ਮਲਵ wind_cross_bearing = wind_vel.cross(bearing_ਸਦਿਸ਼);
	ਅਚਲ ਦਸ਼ਮਲਵ wind_dot_bearing = wind_vel.dot(bearing_ਸਦਿਸ਼);

	Vector2f air_vel_ref;

	ਜੇ (bearingIsFeasible(wind_cross_bearing, wind_dot_bearing, airspeed_sp, wind_vel.norm())) {

		ਅਚਲ ਦਸ਼ਮਲਵ airsp_dot_bearing = projectAirspOnBearing(airspeed_sp, wind_cross_bearing);
		air_vel_ref = solveWindTriangle(wind_cross_bearing, airsp_dot_bearing, bearing_ਸਦਿਸ਼);

	} ਨਹੀਂਤਾਂ {
		air_vel_ref = infeasibleAirVelRef(wind_vel, bearing_ਸਦਿਸ਼, wind_vel.norm(), airspeed_sp);
	}

	ਵਾਪਸ atan2f(air_vel_ref(1), air_vel_ref(0));
}

ਦਸ਼ਮਲਵ
ਕੋਰਸ_ਮੰਜ਼ਿਲ_Airspeed_Ref_Mapper::getMinAirspeedForCurrentBearing(ਅਚਲ ਦਸ਼ਮਲਵ bearing_setpoint, ਅਚਲ Vector2f &wind_vel,
		ਦਸ਼ਮਲਵ airspeed_max, ਦਸ਼ਮਲਵ min_ground_ਰਫਤਾਰ) ਅਚਲ
{
	ਅਚਲ Vector2f bearing_ਸਦਿਸ਼ = Vector2f{cosf(bearing_setpoint), sinf(bearing_setpoint)};
	ਅਚਲ ਦਸ਼ਮਲਵ wind_cross_bearing = wind_vel.cross(bearing_ਸਦਿਸ਼);
	ਅਚਲ ਦਸ਼ਮਲਵ wind_dot_bearing = wind_vel.dot(bearing_ਸਦਿਸ਼);

	ਅਚਲ ਤਾਰਕਿਕ wind_along_bearing_is_ਹੇਠਾਂ_min_ground_ਰਫਤਾਰ = min_ground_ਰਫਤਾਰ > wind_dot_bearing;

	ਦਸ਼ਮਲਵ airspeed_min = 0.f; // return 0 if no min airspeed is necessary

	ਜੇ (wind_along_bearing_is_ਹੇਠਾਂ_min_ground_ਰਫਤਾਰ) {
		// airspeed required to achieve minimum ground speed along bearing vector (5.18)
		airspeed_min = sqrtf((min_ground_ਰਫਤਾਰ - wind_dot_bearing) * (min_ground_ਰਫਤਾਰ - wind_dot_bearing) +
				     wind_cross_bearing * wind_cross_bearing);

	}

	ਵਾਪਸ math::min(airspeed_min, airspeed_max);
}

ਦਸ਼ਮਲਵ ਕੋਰਸ_ਮੰਜ਼ਿਲ_Airspeed_Ref_Mapper::projectAirspOnBearing(ਅਚਲ ਦਸ਼ਮਲਵ airspeed_true, ਅਚਲ ਦਸ਼ਮਲਵ wind_cross_bearing) ਅਚਲ
{
	// NOTE: wind_cross_bearing must be less than airspeed to use this function
	// it is assumed that bearing feasibility is checked and found feasible (e.g. bearingIsFeasible() = true) prior to entering this method
	// otherwise the return will be erroneous

	// 3.5.8
	ਵਾਪਸ sqrtf(math::max(airspeed_true * airspeed_true - wind_cross_bearing * wind_cross_bearing, 0.0f));
}

ਪੂਰਨ ਕੋਰਸ_ਮੰਜ਼ਿਲ_Airspeed_Ref_Mapper::bearingIsFeasible(ਅਚਲ ਦਸ਼ਮਲਵ wind_cross_bearing, ਅਚਲ ਦਸ਼ਮਲਵ wind_dot_bearing,
		ਅਚਲ ਦਸ਼ਮਲਵ airspeed, ਅਚਲ ਦਸ਼ਮਲਵ wind_ਰਫਤਾਰ) ਅਚਲ
{
	ਵਾਪਸ (fabsf(wind_cross_bearing) < airspeed) && ((wind_dot_bearing > 0.0f) || (wind_ਰਫਤਾਰ < airspeed));
}

ਮੈਟ੍ਰਿਕਸ::Vector2f
ਕੋਰਸ_ਮੰਜ਼ਿਲ_Airspeed_Ref_Mapper::solveWindTriangle(ਅਚਲ ਦਸ਼ਮਲਵ wind_cross_bearing, ਅਚਲ ਦਸ਼ਮਲਵ airsp_dot_bearing,
		ਅਚਲ Vector2f &bearing_vec) ਅਚਲ
{
	// essentially a 2D rotation with the speeds (magnitudes) baked in
	ਵਾਪਸ Vector2f{airsp_dot_bearing * bearing_vec(0) - wind_cross_bearing * bearing_vec(1),
			wind_cross_bearing * bearing_vec(0) + airsp_dot_bearing * bearing_vec(1)};
}

ਮੈਟ੍ਰਿਕਸ::Vector2f ਕੋਰਸ_ਮੰਜ਼ਿਲ_Airspeed_Ref_Mapper::infeasibleAirVelRef(ਅਚਲ Vector2f &wind_vel, ਅਚਲ Vector2f &bearing_vec,
		ਅਚਲ ਦਸ਼ਮਲਵ wind_ਰਫਤਾਰ, ਅਚਲ ਦਸ਼ਮਲਵ airspeed) ਅਚਲ
{
	// NOTE: wind speed must be greater than airspeed, and airspeed must be greater than zero to use this function
	// it is assumed that bearing feasibility is checked and found infeasible (e.g. bearingIsFeasible() = false) prior to entering this method
	// otherwise the normalization of the air velocity vector could have a division by zero
	Vector2f air_vel_ref = sqrtf(math::max(wind_ਰਫਤਾਰ * wind_ਰਫਤਾਰ - airspeed * airspeed, 0.0f)) * bearing_vec - wind_vel;
	ਵਾਪਸ air_vel_ref.normalized() * airspeed;
}
