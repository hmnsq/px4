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
#ضمن "DirectionalGuidance.hpp"
استعمال ميٽرڪس::Vector2d;
استعمال ميٽرڪس::Vector2f;

DirectionalGuidance::DirectionalGuidance()
{

}

DirectionalGuidanceOutput
DirectionalGuidance::guide_منزل_رستو(ثابت Vector2f &curr_pos_local, ثابت Vector2f &ground_vel, ثابت Vector2f &wind_vel,
				 ثابت Vector2f &unit_رستو_tangent, ثابت Vector2f &position_on_رستو,
				 ثابت عائم رستو_curvature)
{
	ثابت عائم ground_رفتار = ground_vel.norm();

	ثابت Vector2f air_vel = ground_vel - wind_vel;
	ثابت عائم airspeed = air_vel.norm();

	ثابت عائم wind_رفتار = wind_vel.norm();

	ثابت Vector2f رستو_pos_منزل_vehicle{curr_pos_local - position_on_رستو};
	signed_track_error_ = unit_رستو_tangent.cross(رستو_pos_منزل_vehicle);

	// on-track wind triangle projections
	ثابت عائم wind_cross_upt = wind_vel.cross(unit_رستو_tangent);
	ثابت عائم wind_dot_upt = wind_vel.dot(unit_رستو_tangent);

	// calculate the bearing feasibility on the track at the current closest point
	feas_on_track_ = bearingFeasibility(wind_cross_upt, wind_dot_upt, airspeed, wind_رفتار);

	ثابت عائم track_error = fabsf(signed_track_error_);

	// update control parameters considering upper and lower stability bounds (if enabled)
	// must be called before trackErrorBound() as it updates time_const_
	adapted_period_ = adaptPeriod(ground_رفتار, airspeed, wind_رفتار, track_error,
				      رستو_curvature, wind_vel, unit_رستو_tangent, feas_on_track_);
	ثابت عائم وقت_const = وقت_Const(adapted_period_, damping_);

	// track error bound is dynamic depending on ground speed
	track_error_bound_ = trackErrorBound(ground_رفتار, وقت_const);
	ثابت عائم normalized_track_error = normalizedTrackError(track_error, track_error_bound_);

	// look ahead angle based solely on track proximity
	ثابت عائم look_ahead_ang = lookAheadAngle(normalized_track_error);

	track_proximity_ = trackProximity(look_ahead_ang);

	bearing_vec_ = bearingVec(unit_رستو_tangent, look_ahead_ang, signed_track_error_);

	// wind triangle projections
	ثابت عائم wind_cross_bearing = wind_vel.cross(bearing_vec_);
	ثابت عائم wind_dot_bearing = wind_vel.dot(bearing_vec_);

	// continuous representation of the bearing feasibility
	feas_ = bearingFeasibility(wind_cross_bearing, wind_dot_bearing, airspeed, wind_رفتار);

	// we consider feasibility of both the current bearing as well as that on the track at the current closest point
	ثابت عائم feas_combined = feas_ * feas_on_track_;
	// lateral acceleration needed to stay on curved track (assuming no heading error)
	lateral_accel_ff_ = lateralAccelFF(unit_رستو_tangent, ground_vel, wind_dot_upt,
					   wind_cross_upt, airspeed, wind_رفتار, signed_track_error_, رستو_curvature) * feas_combined * track_proximity_;
	ڪورس_sp = atan2f(bearing_vec_(1), bearing_vec_(0));

	أرجع DirectionalGuidanceOutput{.ڪورس_setpoint = ڪورس_sp,
					 .lateral_acceleration_feedforward = lateral_accel_ff_};
}

عائم DirectionalGuidance::adaptPeriod(ثابت عائم ground_رفتار, ثابت عائم airspeed, ثابت عائم wind_رفتار,
				       ثابت عائم track_error, ثابت عائم رستو_curvature, ثابت Vector2f &wind_vel,
				       ثابت Vector2f &unit_رستو_tangent, ثابت عائم feas_on_track) ثابت
{
	عائم period = period_;
	ثابت عائم air_turn_شرح = fabsf(رستو_curvature * airspeed);
	ثابت عائم wind_ضرب_ڪندڙ = wind_ضرب_ڪندڙ_2(airspeed, wind_رفتار);

	إذا (en_period_lb_ && roll_وقت_const > NPFG_EPSILON) {
		// lower bound for period not considering path curvature
		ثابت عائم period_lb_zero_curvature = period_هيٺاهون_Bound(0.0f, wind_ضرب_ڪندڙ, feas_on_track) * period_safety_ضرب_ڪندڙ;

		// lower bound for period *considering path curvature
		عائم period_lb = period_هيٺاهون_Bound(air_turn_شرح, wind_ضرب_ڪندڙ, feas_on_track) * period_safety_ضرب_ڪندڙ;

		// calculate the time constant and track error bound considering the zero
		// curvature, lower-bounded period and subsequently recalculate the normalized
		// track error
		ثابت عائم وقت_const = وقت_Const(period_lb_zero_curvature, damping_);
		ثابت عائم track_error_bound = trackErrorBound(ground_رفتار, وقت_const);
		ثابت عائم normalized_track_error = normalizedTrackError(track_error, track_error_bound);

		// calculate nominal track proximity with lower bounded time constant
		// (only a numerical solution can find corresponding track proximity
		// and adapted gains simultaneously)
		ثابت عائم look_ahead_ang = lookAheadAngle(normalized_track_error);
		ثابت عائم track_proximity = trackProximity(look_ahead_ang);

		// linearly ramp in curvature dependent lower bound with track proximity
		period_lb = period_lb * track_proximity + (1.0f - track_proximity) * period_lb_zero_curvature;

		// lower bounded period
		period = math::max(period_lb, period);

		// only allow upper bounding ONLY if lower bounding is enabled (is otherwise
		// dangerous to allow period decrements without stability checks).
		// NOTE: if the roll time constant is not accurately known, lower-bound
		// checks may be too optimistic and reducing the period can still destabilize
		// the system! enable this feature at your own risk.
		إذا (en_period_ub_) {

			ثابت عائم period_ub = period_مٿانهون_Bound(air_turn_شرح, wind_ضرب_ڪندڙ, feas_on_track);

			إذا (en_period_ub_ && PX4_ISFINITE(period_ub) && period > period_ub) {
				// NOTE: if the roll time constant is not accurately known, reducing
				// the period here can destabilize the system!
				// enable this feature at your own risk!

				// upper bound the period (for track keeping stability), prefer lower bound if violated
				ثابت عائم period_adapted = math::max(period_lb, period_ub);

				// transition from the nominal period to the adapted period as we get
				// closer to the track
				period = period_adapted * track_proximity + (1.0f - track_proximity) * period;
			}
		}
	}

	أرجع period;
}

عائم DirectionalGuidance::normalizedTrackError(ثابت عائم track_error, ثابت عائم track_error_bound) ثابت
{
	أرجع math::constrain(track_error / track_error_bound, 0.0f, 1.0f);
}

عائم DirectionalGuidance::wind_ضرب_ڪندڙ_2(ثابت عائم airspeed, ثابت عائم wind_رفتار) ثابت
{
	// See [TODO: include citation] for definition/elaboration of this approximation.
	إذا (wind_رفتار > airspeed || airspeed < NPFG_EPSILON) {
		أرجع 2.0f;

	} وإلا {
		أرجع 2.0f * (1.0f - sqrtf(1.0f - math::min(1.0f, wind_رفتار / airspeed)));
	}
}

عائم DirectionalGuidance::period_مٿانهون_Bound(ثابت عائم air_turn_شرح, ثابت عائم wind_ضرب_ڪندڙ,
		ثابت عائم feas_on_track) ثابت
{
	إذا (air_turn_شرح * wind_ضرب_ڪندڙ > NPFG_EPSILON) {
		// multiply air turn rate by feasibility on track to zero out when we anyway
		// should not consider the curvature
		أرجع 4.0f * M_PI_F * damping_ / (air_turn_شرح * wind_ضرب_ڪندڙ * feas_on_track);
	}

	أرجع INFINITY;
}

عائم DirectionalGuidance::period_هيٺاهون_Bound(ثابت عائم air_turn_شرح, ثابت عائم wind_ضرب_ڪندڙ,
		ثابت عائم feas_on_track) ثابت
{
	// this method considers a "conservative" lower period bound, i.e. a constant
	// worst case bound for any wind ratio, airspeed, and path curvature

	// the lower bound for zero curvature and no wind OR damping ratio < 0.5
	ثابت عائم period_lb = M_PI_F * roll_وقت_const / damping_;

	إذا (air_turn_شرح * wind_ضرب_ڪندڙ < NPFG_EPSILON || damping_ < 0.5f) {
		أرجع period_lb;

	} وإلا {
		// the lower bound for tracking a curved path in wind with damping ratio > 0.5
		ثابت عائم period_windy_curved_damped = 4.0f * M_PI_F * roll_وقت_const * damping_;

		// blend the two together as the bearing on track becomes less feasible
		أرجع period_windy_curved_damped * feas_on_track + (1.0f - feas_on_track) * period_lb;
	}
}

عائم DirectionalGuidance::trackProximity(ثابت عائم look_ahead_ang) ثابت
{
	ثابت عائم sin_look_ahead_ang = sinf(look_ahead_ang);
	أرجع sin_look_ahead_ang * sin_look_ahead_ang;
}

عائم DirectionalGuidance::trackErrorBound(ثابت عائم ground_رفتار, ثابت عائم وقت_const) ثابت
{
	إذا (ground_رفتار > 1.0f) {
		أرجع ground_رفتار * وقت_const;

	} وإلا {
		// limit bound to some minimum ground speed to avoid singularities in track
		// error normalization. the following equation assumes ground speed minimum = 1.0
		أرجع 0.5f * وقت_const * (ground_رفتار * ground_رفتار + 1.0f);
	}
}

عائم DirectionalGuidance::وقت_Const(ثابت عائم period, ثابت عائم damping) ثابت
{
	أرجع period * damping;
}

عائم DirectionalGuidance::lookAheadAngle(ثابت عائم normalized_track_error) ثابت
{
	أرجع M_PI_2_F * (normalized_track_error - 1.0f) * (normalized_track_error - 1.0f);
}


ميٽرڪس::Vector2f DirectionalGuidance::bearingVec(ثابت Vector2f &unit_رستو_tangent, ثابت عائم look_ahead_ang,
		ثابت عائم signed_track_error) ثابت
{
	ثابت عائم cos_look_ahead_ang = cosf(look_ahead_ang);
	ثابت عائم sin_look_ahead_ang = sinf(look_ahead_ang);

	Vector2f unit_رستو_عام(-unit_رستو_tangent(1), unit_رستو_tangent(0)); // right handed 90 deg (clockwise) turn
	Vector2f unit_track_error = -((signed_track_error < 0.0f) ? -1.0f : 1.0f) * unit_رستو_عام;

	أرجع cos_look_ahead_ang * unit_track_error + sin_look_ahead_ang * unit_رستو_tangent;
}

عائم
DirectionalGuidance::bearingFeasibility(عائم wind_cross_bearing, ثابت عائم wind_dot_bearing, ثابت عائم airspeed,
					ثابت عائم wind_رفتار) ثابت
{
	إذا (wind_dot_bearing < 0.0f) {
		wind_cross_bearing = wind_رفتار;

	} وإلا {
		wind_cross_bearing = fabsf(wind_cross_bearing);
	}

	عائم sin_arg = sinf(M_PI_F * 0.5f * math::constrain((airspeed - wind_cross_bearing) / AIRSPEED_بفر, 0.0f, 1.0f));
	أرجع sin_arg * sin_arg;
}

عائم DirectionalGuidance::lateralAccelFF(ثابت Vector2f &unit_رستو_tangent, ثابت Vector2f &ground_vel,
		ثابت عائم wind_dot_upt, ثابت عائم wind_cross_upt, ثابت عائم airspeed,
		ثابت عائم wind_رفتار, ثابت عائم signed_track_error,
		ثابت عائم رستو_curvature) ثابت
{
	// NOTE: all calculations within this function take place at the closet point
	// on the path, as if the aircraft were already tracking the given path at
	// this point with zero angular error. this allows us to evaluate curvature
	// induced requirements for lateral acceleration incrementation and ramp them
	// in with the track proximity and further consider the bearing feasibility
	// in excess wind conditions (this is done external to this method).

	// path frame curvature is the instantaneous curvature at our current distance
	// from the actual path (considering e.g. concentric circles emanating outward/inward)
	ثابت عائم رستو_چوکٺو_curvature = رستو_curvature / math::max(1.0f - رستو_curvature * signed_track_error,
					   fabsf(رستو_curvature) * MIN_RADIUS);

	// limit tangent ground speed to along track (forward moving) direction
	ثابت عائم tangent_ground_رفتار = math::max(ground_vel.dot(unit_رستو_tangent), 0.0f);

	ثابت عائم رستو_چوکٺو_شرح = رستو_چوکٺو_curvature * tangent_ground_رفتار;

	// speed ratio = projection of ground vel on track / projection of air velocity
	// on track
	ثابت عائم رفتار_تناسب = (1.0f + wind_dot_upt / math::max(projectAirspOnBearing(airspeed, wind_cross_upt),
				   NPFG_EPSILON));

	// note the use of airspeed * speed_ratio as oppose to ground_speed^2 here --
	// the prior considers that we command lateral acceleration in the air mass
	// relative frame while the latter does not
	أرجع airspeed * رفتار_تناسب * رستو_چوکٺو_شرح;
}

عائم DirectionalGuidance::projectAirspOnBearing(ثابت عائم airspeed, ثابت عائم wind_cross_bearing) ثابت
{
	// NOTE: wind_cross_bearing must be less than airspeed to use this function
	// it is assumed that bearing feasibility is checked and found feasible (e.g. bearingIsFeasible() = true) prior to entering this method
	// otherwise the return will be erroneous
	أرجع sqrtf(math::max(airspeed * airspeed - wind_cross_bearing * wind_cross_bearing, 0.0f));
}

عائم DirectionalGuidance::switch_مفاصلو(عائم wp_radius) ثابت
{
	أرجع math::min(wp_radius, track_error_bound_ * switch_مفاصلو_multiplier);
}
