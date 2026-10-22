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

#అన్తర్భావ "CourseToAirspeedRefMapper.hpp"
ఉపయోగించి మాతృక::Vector2f;

దశాంశం
కోర్సు_గమ్యం_Airspeed_Ref_Mapper::map_కోర్సు_Setpoint_గమ్యం_Heading_Setpoint(స్థిరరాశి దశాంశం bearing_setpoint, స్థిరరాశి Vector2f &wind_vel,
		దశాంశం airspeed_sp) స్థిరరాశి
{
	స్థిరరాశి Vector2f bearing_సదిశ = Vector2f{cosf(bearing_setpoint), sinf(bearing_setpoint)};
	స్థిరరాశి దశాంశం wind_cross_bearing = wind_vel.cross(bearing_సదిశ);
	స్థిరరాశి దశాంశం wind_dot_bearing = wind_vel.dot(bearing_సదిశ);

	Vector2f air_vel_ref;

	ఒకవేళ (bearingIsFeasible(wind_cross_bearing, wind_dot_bearing, airspeed_sp, wind_vel.norm())) {

		స్థిరరాశి దశాంశం airsp_dot_bearing = projectAirspOnBearing(airspeed_sp, wind_cross_bearing);
		air_vel_ref = solveWindTriangle(wind_cross_bearing, airsp_dot_bearing, bearing_సదిశ);

	} లేకపోతే {
		air_vel_ref = infeasibleAirVelRef(wind_vel, bearing_సదిశ, wind_vel.norm(), airspeed_sp);
	}

	తిరిగి atan2f(air_vel_ref(1), air_vel_ref(0));
}

దశాంశం
కోర్సు_గమ్యం_Airspeed_Ref_Mapper::getMinAirspeedForCurrentBearing(స్థిరరాశి దశాంశం bearing_setpoint, స్థిరరాశి Vector2f &wind_vel,
		దశాంశం airspeed_max, దశాంశం min_ground_వేగం) స్థిరరాశి
{
	స్థిరరాశి Vector2f bearing_సదిశ = Vector2f{cosf(bearing_setpoint), sinf(bearing_setpoint)};
	స్థిరరాశి దశాంశం wind_cross_bearing = wind_vel.cross(bearing_సదిశ);
	స్థిరరాశి దశాంశం wind_dot_bearing = wind_vel.dot(bearing_సదిశ);

	స్థిరరాశి తార్కిక wind_along_bearing_is_క్రింద_min_ground_వేగం = min_ground_వేగం > wind_dot_bearing;

	దశాంశం airspeed_min = 0.f; // return 0 if no min airspeed is necessary

	ఒకవేళ (wind_along_bearing_is_క్రింద_min_ground_వేగం) {
		// airspeed required to achieve minimum ground speed along bearing vector (5.18)
		airspeed_min = sqrtf((min_ground_వేగం - wind_dot_bearing) * (min_ground_వేగం - wind_dot_bearing) +
				     wind_cross_bearing * wind_cross_bearing);

	}

	తిరిగి math::min(airspeed_min, airspeed_max);
}

దశాంశం కోర్సు_గమ్యం_Airspeed_Ref_Mapper::projectAirspOnBearing(స్థిరరాశి దశాంశం airspeed_true, స్థిరరాశి దశాంశం wind_cross_bearing) స్థిరరాశి
{
	// NOTE: wind_cross_bearing must be less than airspeed to use this function
	// it is assumed that bearing feasibility is checked and found feasible (e.g. bearingIsFeasible() = true) prior to entering this method
	// otherwise the return will be erroneous

	// 3.5.8
	తిరిగి sqrtf(math::max(airspeed_true * airspeed_true - wind_cross_bearing * wind_cross_bearing, 0.0f));
}

పూర్ణాంకం కోర్సు_గమ్యం_Airspeed_Ref_Mapper::bearingIsFeasible(స్థిరరాశి దశాంశం wind_cross_bearing, స్థిరరాశి దశాంశం wind_dot_bearing,
		స్థిరరాశి దశాంశం airspeed, స్థిరరాశి దశాంశం wind_వేగం) స్థిరరాశి
{
	తిరిగి (fabsf(wind_cross_bearing) < airspeed) && ((wind_dot_bearing > 0.0f) || (wind_వేగం < airspeed));
}

మాతృక::Vector2f
కోర్సు_గమ్యం_Airspeed_Ref_Mapper::solveWindTriangle(స్థిరరాశి దశాంశం wind_cross_bearing, స్థిరరాశి దశాంశం airsp_dot_bearing,
		స్థిరరాశి Vector2f &bearing_vec) స్థిరరాశి
{
	// essentially a 2D rotation with the speeds (magnitudes) baked in
	తిరిగి Vector2f{airsp_dot_bearing * bearing_vec(0) - wind_cross_bearing * bearing_vec(1),
			wind_cross_bearing * bearing_vec(0) + airsp_dot_bearing * bearing_vec(1)};
}

మాతృక::Vector2f కోర్సు_గమ్యం_Airspeed_Ref_Mapper::infeasibleAirVelRef(స్థిరరాశి Vector2f &wind_vel, స్థిరరాశి Vector2f &bearing_vec,
		స్థిరరాశి దశాంశం wind_వేగం, స్థిరరాశి దశాంశం airspeed) స్థిరరాశి
{
	// NOTE: wind speed must be greater than airspeed, and airspeed must be greater than zero to use this function
	// it is assumed that bearing feasibility is checked and found infeasible (e.g. bearingIsFeasible() = false) prior to entering this method
	// otherwise the normalization of the air velocity vector could have a division by zero
	Vector2f air_vel_ref = sqrtf(math::max(wind_వేగం * wind_వేగం - airspeed * airspeed, 0.0f)) * bearing_vec - wind_vel;
	తిరిగి air_vel_ref.normalized() * airspeed;
}
