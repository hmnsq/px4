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

/*
 * @file DirectionalGuidance.cpp
 */
#అన్తర్భావ "DirectionalGuidance.hpp"
ఉపయోగించి మాతృక::Vector2d;
ఉపయోగించి మాతృక::Vector2f;

DirectionalGuidance::DirectionalGuidance()
{

}

DirectionalGuidanceOutput
DirectionalGuidance::guide_గమ్యం_మార్గం(స్థిరరాశి Vector2f &curr_pos_local, స్థిరరాశి Vector2f &ground_vel, స్థిరరాశి Vector2f &wind_vel,
				 స్థిరరాశి Vector2f &unit_మార్గం_tangent, స్థిరరాశి Vector2f &position_on_మార్గం,
				 స్థిరరాశి దశాంశం మార్గం_curvature)
{
	స్థిరరాశి దశాంశం ground_వేగం = ground_vel.norm();

	స్థిరరాశి Vector2f air_vel = ground_vel - wind_vel;
	స్థిరరాశి దశాంశం airspeed = air_vel.norm();

	స్థిరరాశి దశాంశం wind_వేగం = wind_vel.norm();

	స్థిరరాశి Vector2f మార్గం_pos_గమ్యం_vehicle{curr_pos_local - position_on_మార్గం};
	signed_track_error_ = unit_మార్గం_tangent.cross(మార్గం_pos_గమ్యం_vehicle);

	// on-track wind triangle projections
	స్థిరరాశి దశాంశం wind_cross_upt = wind_vel.cross(unit_మార్గం_tangent);
	స్థిరరాశి దశాంశం wind_dot_upt = wind_vel.dot(unit_మార్గం_tangent);

	// calculate the bearing feasibility on the track at the current closest point
	feas_on_track_ = bearingFeasibility(wind_cross_upt, wind_dot_upt, airspeed, wind_వేగం);

	స్థిరరాశి దశాంశం track_error = fabsf(signed_track_error_);

	// update control parameters considering upper and lower stability bounds (if enabled)
	// must be called before trackErrorBound() as it updates time_const_
	adapted_period_ = adaptPeriod(ground_వేగం, airspeed, wind_వేగం, track_error,
				      మార్గం_curvature, wind_vel, unit_మార్గం_tangent, feas_on_track_);
	స్థిరరాశి దశాంశం సమయం_const = సమయం_Const(adapted_period_, damping_);

	// track error bound is dynamic depending on ground speed
	track_error_bound_ = trackErrorBound(ground_వేగం, సమయం_const);
	స్థిరరాశి దశాంశం normalized_track_error = normalizedTrackError(track_error, track_error_bound_);

	// look ahead angle based solely on track proximity
	స్థిరరాశి దశాంశం look_ahead_ang = lookAheadAngle(normalized_track_error);

	track_proximity_ = trackProximity(look_ahead_ang);

	bearing_vec_ = bearingVec(unit_మార్గం_tangent, look_ahead_ang, signed_track_error_);

	// wind triangle projections
	స్థిరరాశి దశాంశం wind_cross_bearing = wind_vel.cross(bearing_vec_);
	స్థిరరాశి దశాంశం wind_dot_bearing = wind_vel.dot(bearing_vec_);

	// continuous representation of the bearing feasibility
	feas_ = bearingFeasibility(wind_cross_bearing, wind_dot_bearing, airspeed, wind_వేగం);

	// we consider feasibility of both the current bearing as well as that on the track at the current closest point
	స్థిరరాశి దశాంశం feas_combined = feas_ * feas_on_track_;
	// lateral acceleration needed to stay on curved track (assuming no heading error)
	lateral_accel_ff_ = lateralAccelFF(unit_మార్గం_tangent, ground_vel, wind_dot_upt,
					   wind_cross_upt, airspeed, wind_వేగం, signed_track_error_, మార్గం_curvature) * feas_combined * track_proximity_;
	కోర్సు_sp = atan2f(bearing_vec_(1), bearing_vec_(0));

	తిరిగి DirectionalGuidanceOutput{.కోర్సు_setpoint = కోర్సు_sp,
					 .lateral_acceleration_feedforward = lateral_accel_ff_};
}

దశాంశం DirectionalGuidance::adaptPeriod(స్థిరరాశి దశాంశం ground_వేగం, స్థిరరాశి దశాంశం airspeed, స్థిరరాశి దశాంశం wind_వేగం,
				       స్థిరరాశి దశాంశం track_error, స్థిరరాశి దశాంశం మార్గం_curvature, స్థిరరాశి Vector2f &wind_vel,
				       స్థిరరాశి Vector2f &unit_మార్గం_tangent, స్థిరరాశి దశాంశం feas_on_track) స్థిరరాశి
{
	దశాంశం period = period_;
	స్థిరరాశి దశాంశం air_turn_రేటు = fabsf(మార్గం_curvature * airspeed);
	స్థిరరాశి దశాంశం wind_గుణకం = wind_గుణకం_2(airspeed, wind_వేగం);

	ఒకవేళ (en_period_lb_ && roll_సమయం_const > NPFG_EPSILON) {
		// lower bound for period not considering path curvature
		స్థిరరాశి దశాంశం period_lb_zero_curvature = period_క్రింది_Bound(0.0f, wind_గుణకం, feas_on_track) * period_safety_గుణకం;

		// lower bound for period *considering path curvature
		దశాంశం period_lb = period_క్రింది_Bound(air_turn_రేటు, wind_గుణకం, feas_on_track) * period_safety_గుణకం;

		// calculate the time constant and track error bound considering the zero
		// curvature, lower-bounded period and subsequently recalculate the normalized
		// track error
		స్థిరరాశి దశాంశం సమయం_const = సమయం_Const(period_lb_zero_curvature, damping_);
		స్థిరరాశి దశాంశం track_error_bound = trackErrorBound(ground_వేగం, సమయం_const);
		స్థిరరాశి దశాంశం normalized_track_error = normalizedTrackError(track_error, track_error_bound);

		// calculate nominal track proximity with lower bounded time constant
		// (only a numerical solution can find corresponding track proximity
		// and adapted gains simultaneously)
		స్థిరరాశి దశాంశం look_ahead_ang = lookAheadAngle(normalized_track_error);
		స్థిరరాశి దశాంశం track_proximity = trackProximity(look_ahead_ang);

		// linearly ramp in curvature dependent lower bound with track proximity
		period_lb = period_lb * track_proximity + (1.0f - track_proximity) * period_lb_zero_curvature;

		// lower bounded period
		period = math::max(period_lb, period);

		// only allow upper bounding ONLY if lower bounding is enabled (is otherwise
		// dangerous to allow period decrements without stability checks).
		// NOTE: if the roll time constant is not accurately known, lower-bound
		// checks may be too optimistic and reducing the period can still destabilize
		// the system! enable this feature at your own risk.
		ఒకవేళ (en_period_ub_) {

			స్థిరరాశి దశాంశం period_ub = period_పై_Bound(air_turn_రేటు, wind_గుణకం, feas_on_track);

			ఒకవేళ (en_period_ub_ && PX4_ISFINITE(period_ub) && period > period_ub) {
				// NOTE: if the roll time constant is not accurately known, reducing
				// the period here can destabilize the system!
				// enable this feature at your own risk!

				// upper bound the period (for track keeping stability), prefer lower bound if violated
				స్థిరరాశి దశాంశం period_adapted = math::max(period_lb, period_ub);

				// transition from the nominal period to the adapted period as we get
				// closer to the track
				period = period_adapted * track_proximity + (1.0f - track_proximity) * period;
			}
		}
	}

	తిరిగి period;
}

దశాంశం DirectionalGuidance::normalizedTrackError(స్థిరరాశి దశాంశం track_error, స్థిరరాశి దశాంశం track_error_bound) స్థిరరాశి
{
	తిరిగి math::constrain(track_error / track_error_bound, 0.0f, 1.0f);
}

దశాంశం DirectionalGuidance::wind_గుణకం_2(స్థిరరాశి దశాంశం airspeed, స్థిరరాశి దశాంశం wind_వేగం) స్థిరరాశి
{
	// See [TODO: include citation] for definition/elaboration of this approximation.
	ఒకవేళ (wind_వేగం > airspeed || airspeed < NPFG_EPSILON) {
		తిరిగి 2.0f;

	} లేకపోతే {
		తిరిగి 2.0f * (1.0f - sqrtf(1.0f - math::min(1.0f, wind_వేగం / airspeed)));
	}
}

దశాంశం DirectionalGuidance::period_పై_Bound(స్థిరరాశి దశాంశం air_turn_రేటు, స్థిరరాశి దశాంశం wind_గుణకం,
		స్థిరరాశి దశాంశం feas_on_track) స్థిరరాశి
{
	ఒకవేళ (air_turn_రేటు * wind_గుణకం > NPFG_EPSILON) {
		// multiply air turn rate by feasibility on track to zero out when we anyway
		// should not consider the curvature
		తిరిగి 4.0f * M_PI_F * damping_ / (air_turn_రేటు * wind_గుణకం * feas_on_track);
	}

	తిరిగి INFINITY;
}

దశాంశం DirectionalGuidance::period_క్రింది_Bound(స్థిరరాశి దశాంశం air_turn_రేటు, స్థిరరాశి దశాంశం wind_గుణకం,
		స్థిరరాశి దశాంశం feas_on_track) స్థిరరాశి
{
	// this method considers a "conservative" lower period bound, i.e. a constant
	// worst case bound for any wind ratio, airspeed, and path curvature

	// the lower bound for zero curvature and no wind OR damping ratio < 0.5
	స్థిరరాశి దశాంశం period_lb = M_PI_F * roll_సమయం_const / damping_;

	ఒకవేళ (air_turn_రేటు * wind_గుణకం < NPFG_EPSILON || damping_ < 0.5f) {
		తిరిగి period_lb;

	} లేకపోతే {
		// the lower bound for tracking a curved path in wind with damping ratio > 0.5
		స్థిరరాశి దశాంశం period_windy_curved_damped = 4.0f * M_PI_F * roll_సమయం_const * damping_;

		// blend the two together as the bearing on track becomes less feasible
		తిరిగి period_windy_curved_damped * feas_on_track + (1.0f - feas_on_track) * period_lb;
	}
}

దశాంశం DirectionalGuidance::trackProximity(స్థిరరాశి దశాంశం look_ahead_ang) స్థిరరాశి
{
	స్థిరరాశి దశాంశం sin_look_ahead_ang = sinf(look_ahead_ang);
	తిరిగి sin_look_ahead_ang * sin_look_ahead_ang;
}

దశాంశం DirectionalGuidance::trackErrorBound(స్థిరరాశి దశాంశం ground_వేగం, స్థిరరాశి దశాంశం సమయం_const) స్థిరరాశి
{
	ఒకవేళ (ground_వేగం > 1.0f) {
		తిరిగి ground_వేగం * సమయం_const;

	} లేకపోతే {
		// limit bound to some minimum ground speed to avoid singularities in track
		// error normalization. the following equation assumes ground speed minimum = 1.0
		తిరిగి 0.5f * సమయం_const * (ground_వేగం * ground_వేగం + 1.0f);
	}
}

దశాంశం DirectionalGuidance::సమయం_Const(స్థిరరాశి దశాంశం period, స్థిరరాశి దశాంశం damping) స్థిరరాశి
{
	తిరిగి period * damping;
}

దశాంశం DirectionalGuidance::lookAheadAngle(స్థిరరాశి దశాంశం normalized_track_error) స్థిరరాశి
{
	తిరిగి M_PI_2_F * (normalized_track_error - 1.0f) * (normalized_track_error - 1.0f);
}


మాతృక::Vector2f DirectionalGuidance::bearingVec(స్థిరరాశి Vector2f &unit_మార్గం_tangent, స్థిరరాశి దశాంశం look_ahead_ang,
		స్థిరరాశి దశాంశం signed_track_error) స్థిరరాశి
{
	స్థిరరాశి దశాంశం cos_look_ahead_ang = cosf(look_ahead_ang);
	స్థిరరాశి దశాంశం sin_look_ahead_ang = sinf(look_ahead_ang);

	Vector2f unit_మార్గం_సాధారణ(-unit_మార్గం_tangent(1), unit_మార్గం_tangent(0)); // right handed 90 deg (clockwise) turn
	Vector2f unit_track_error = -((signed_track_error < 0.0f) ? -1.0f : 1.0f) * unit_మార్గం_సాధారణ;

	తిరిగి cos_look_ahead_ang * unit_track_error + sin_look_ahead_ang * unit_మార్గం_tangent;
}

దశాంశం
DirectionalGuidance::bearingFeasibility(దశాంశం wind_cross_bearing, స్థిరరాశి దశాంశం wind_dot_bearing, స్థిరరాశి దశాంశం airspeed,
					స్థిరరాశి దశాంశం wind_వేగం) స్థిరరాశి
{
	ఒకవేళ (wind_dot_bearing < 0.0f) {
		wind_cross_bearing = wind_వేగం;

	} లేకపోతే {
		wind_cross_bearing = fabsf(wind_cross_bearing);
	}

	దశాంశం sin_arg = sinf(M_PI_F * 0.5f * math::constrain((airspeed - wind_cross_bearing) / AIRSPEED_బఫరు, 0.0f, 1.0f));
	తిరిగి sin_arg * sin_arg;
}

దశాంశం DirectionalGuidance::lateralAccelFF(స్థిరరాశి Vector2f &unit_మార్గం_tangent, స్థిరరాశి Vector2f &ground_vel,
		స్థిరరాశి దశాంశం wind_dot_upt, స్థిరరాశి దశాంశం wind_cross_upt, స్థిరరాశి దశాంశం airspeed,
		స్థిరరాశి దశాంశం wind_వేగం, స్థిరరాశి దశాంశం signed_track_error,
		స్థిరరాశి దశాంశం మార్గం_curvature) స్థిరరాశి
{
	// NOTE: all calculations within this function take place at the closet point
	// on the path, as if the aircraft were already tracking the given path at
	// this point with zero angular error. this allows us to evaluate curvature
	// induced requirements for lateral acceleration incrementation and ramp them
	// in with the track proximity and further consider the bearing feasibility
	// in excess wind conditions (this is done external to this method).

	// path frame curvature is the instantaneous curvature at our current distance
	// from the actual path (considering e.g. concentric circles emanating outward/inward)
	స్థిరరాశి దశాంశం మార్గం_చట్రం_curvature = మార్గం_curvature / math::max(1.0f - మార్గం_curvature * signed_track_error,
					   fabsf(మార్గం_curvature) * MIN_RADIUS);

	// limit tangent ground speed to along track (forward moving) direction
	స్థిరరాశి దశాంశం tangent_ground_వేగం = math::max(ground_vel.dot(unit_మార్గం_tangent), 0.0f);

	స్థిరరాశి దశాంశం మార్గం_చట్రం_రేటు = మార్గం_చట్రం_curvature * tangent_ground_వేగం;

	// speed ratio = projection of ground vel on track / projection of air velocity
	// on track
	స్థిరరాశి దశాంశం వేగం_నిష్పత్తి = (1.0f + wind_dot_upt / math::max(projectAirspOnBearing(airspeed, wind_cross_upt),
				   NPFG_EPSILON));

	// note the use of airspeed * speed_ratio as oppose to ground_speed^2 here --
	// the prior considers that we command lateral acceleration in the air mass
	// relative frame while the latter does not
	తిరిగి airspeed * వేగం_నిష్పత్తి * మార్గం_చట్రం_రేటు;
}

దశాంశం DirectionalGuidance::projectAirspOnBearing(స్థిరరాశి దశాంశం airspeed, స్థిరరాశి దశాంశం wind_cross_bearing) స్థిరరాశి
{
	// NOTE: wind_cross_bearing must be less than airspeed to use this function
	// it is assumed that bearing feasibility is checked and found feasible (e.g. bearingIsFeasible() = true) prior to entering this method
	// otherwise the return will be erroneous
	తిరిగి sqrtf(math::max(airspeed * airspeed - wind_cross_bearing * wind_cross_bearing, 0.0f));
}

దశాంశం DirectionalGuidance::switch_దూరం(దశాంశం wp_radius) స్థిరరాశి
{
	తిరిగి math::min(wp_radius, track_error_bound_ * switch_దూరం_multiplier);
}
