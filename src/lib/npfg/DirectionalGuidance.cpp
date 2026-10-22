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
#ਅਨ੍ਤਰ੍ਭਾਵ "DirectionalGuidance.hpp"
ਵਰਤੋਂ ਮੈਟ੍ਰਿਕਸ::Vector2d;
ਵਰਤੋਂ ਮੈਟ੍ਰਿਕਸ::Vector2f;

DirectionalGuidance::DirectionalGuidance()
{

}

DirectionalGuidanceOutput
DirectionalGuidance::guide_ਮੰਜ਼ਿਲ_ਰਸਤਾ(ਅਚਲ Vector2f &curr_pos_local, ਅਚਲ Vector2f &ground_vel, ਅਚਲ Vector2f &wind_vel,
				 ਅਚਲ Vector2f &unit_ਰਸਤਾ_tangent, ਅਚਲ Vector2f &position_on_ਰਸਤਾ,
				 ਅਚਲ ਦਸ਼ਮਲਵ ਰਸਤਾ_curvature)
{
	ਅਚਲ ਦਸ਼ਮਲਵ ground_ਰਫਤਾਰ = ground_vel.norm();

	ਅਚਲ Vector2f air_vel = ground_vel - wind_vel;
	ਅਚਲ ਦਸ਼ਮਲਵ airspeed = air_vel.norm();

	ਅਚਲ ਦਸ਼ਮਲਵ wind_ਰਫਤਾਰ = wind_vel.norm();

	ਅਚਲ Vector2f ਰਸਤਾ_pos_ਮੰਜ਼ਿਲ_vehicle{curr_pos_local - position_on_ਰਸਤਾ};
	signed_track_error_ = unit_ਰਸਤਾ_tangent.cross(ਰਸਤਾ_pos_ਮੰਜ਼ਿਲ_vehicle);

	// on-track wind triangle projections
	ਅਚਲ ਦਸ਼ਮਲਵ wind_cross_upt = wind_vel.cross(unit_ਰਸਤਾ_tangent);
	ਅਚਲ ਦਸ਼ਮਲਵ wind_dot_upt = wind_vel.dot(unit_ਰਸਤਾ_tangent);

	// calculate the bearing feasibility on the track at the current closest point
	feas_on_track_ = bearingFeasibility(wind_cross_upt, wind_dot_upt, airspeed, wind_ਰਫਤਾਰ);

	ਅਚਲ ਦਸ਼ਮਲਵ track_error = fabsf(signed_track_error_);

	// update control parameters considering upper and lower stability bounds (if enabled)
	// must be called before trackErrorBound() as it updates time_const_
	adapted_period_ = adaptPeriod(ground_ਰਫਤਾਰ, airspeed, wind_ਰਫਤਾਰ, track_error,
				      ਰਸਤਾ_curvature, wind_vel, unit_ਰਸਤਾ_tangent, feas_on_track_);
	ਅਚਲ ਦਸ਼ਮਲਵ ਸਮਾਂ_const = ਸਮਾਂ_Const(adapted_period_, damping_);

	// track error bound is dynamic depending on ground speed
	track_error_bound_ = trackErrorBound(ground_ਰਫਤਾਰ, ਸਮਾਂ_const);
	ਅਚਲ ਦਸ਼ਮਲਵ normalized_track_error = normalizedTrackError(track_error, track_error_bound_);

	// look ahead angle based solely on track proximity
	ਅਚਲ ਦਸ਼ਮਲਵ look_ahead_ang = lookAheadAngle(normalized_track_error);

	track_proximity_ = trackProximity(look_ahead_ang);

	bearing_vec_ = bearingVec(unit_ਰਸਤਾ_tangent, look_ahead_ang, signed_track_error_);

	// wind triangle projections
	ਅਚਲ ਦਸ਼ਮਲਵ wind_cross_bearing = wind_vel.cross(bearing_vec_);
	ਅਚਲ ਦਸ਼ਮਲਵ wind_dot_bearing = wind_vel.dot(bearing_vec_);

	// continuous representation of the bearing feasibility
	feas_ = bearingFeasibility(wind_cross_bearing, wind_dot_bearing, airspeed, wind_ਰਫਤਾਰ);

	// we consider feasibility of both the current bearing as well as that on the track at the current closest point
	ਅਚਲ ਦਸ਼ਮਲਵ feas_combined = feas_ * feas_on_track_;
	// lateral acceleration needed to stay on curved track (assuming no heading error)
	lateral_accel_ff_ = lateralAccelFF(unit_ਰਸਤਾ_tangent, ground_vel, wind_dot_upt,
					   wind_cross_upt, airspeed, wind_ਰਫਤਾਰ, signed_track_error_, ਰਸਤਾ_curvature) * feas_combined * track_proximity_;
	ਕੋਰਸ_sp = atan2f(bearing_vec_(1), bearing_vec_(0));

	ਵਾਪਸ DirectionalGuidanceOutput{.ਕੋਰਸ_setpoint = ਕੋਰਸ_sp,
					 .lateral_acceleration_feedforward = lateral_accel_ff_};
}

ਦਸ਼ਮਲਵ DirectionalGuidance::adaptPeriod(ਅਚਲ ਦਸ਼ਮਲਵ ground_ਰਫਤਾਰ, ਅਚਲ ਦਸ਼ਮਲਵ airspeed, ਅਚਲ ਦਸ਼ਮਲਵ wind_ਰਫਤਾਰ,
				       ਅਚਲ ਦਸ਼ਮਲਵ track_error, ਅਚਲ ਦਸ਼ਮਲਵ ਰਸਤਾ_curvature, ਅਚਲ Vector2f &wind_vel,
				       ਅਚਲ Vector2f &unit_ਰਸਤਾ_tangent, ਅਚਲ ਦਸ਼ਮਲਵ feas_on_track) ਅਚਲ
{
	ਦਸ਼ਮਲਵ period = period_;
	ਅਚਲ ਦਸ਼ਮਲਵ air_turn_ਦਰ = fabsf(ਰਸਤਾ_curvature * airspeed);
	ਅਚਲ ਦਸ਼ਮਲਵ wind_ਗੁਣਕ = wind_ਗੁਣਕ_2(airspeed, wind_ਰਫਤਾਰ);

	ਜੇ (en_period_lb_ && roll_ਸਮਾਂ_const > NPFG_EPSILON) {
		// lower bound for period not considering path curvature
		ਅਚਲ ਦਸ਼ਮਲਵ period_lb_zero_curvature = period_ਹੇਠਲਾ_Bound(0.0f, wind_ਗੁਣਕ, feas_on_track) * period_safety_ਗੁਣਕ;

		// lower bound for period *considering path curvature
		ਦਸ਼ਮਲਵ period_lb = period_ਹੇਠਲਾ_Bound(air_turn_ਦਰ, wind_ਗੁਣਕ, feas_on_track) * period_safety_ਗੁਣਕ;

		// calculate the time constant and track error bound considering the zero
		// curvature, lower-bounded period and subsequently recalculate the normalized
		// track error
		ਅਚਲ ਦਸ਼ਮਲਵ ਸਮਾਂ_const = ਸਮਾਂ_Const(period_lb_zero_curvature, damping_);
		ਅਚਲ ਦਸ਼ਮਲਵ track_error_bound = trackErrorBound(ground_ਰਫਤਾਰ, ਸਮਾਂ_const);
		ਅਚਲ ਦਸ਼ਮਲਵ normalized_track_error = normalizedTrackError(track_error, track_error_bound);

		// calculate nominal track proximity with lower bounded time constant
		// (only a numerical solution can find corresponding track proximity
		// and adapted gains simultaneously)
		ਅਚਲ ਦਸ਼ਮਲਵ look_ahead_ang = lookAheadAngle(normalized_track_error);
		ਅਚਲ ਦਸ਼ਮਲਵ track_proximity = trackProximity(look_ahead_ang);

		// linearly ramp in curvature dependent lower bound with track proximity
		period_lb = period_lb * track_proximity + (1.0f - track_proximity) * period_lb_zero_curvature;

		// lower bounded period
		period = math::max(period_lb, period);

		// only allow upper bounding ONLY if lower bounding is enabled (is otherwise
		// dangerous to allow period decrements without stability checks).
		// NOTE: if the roll time constant is not accurately known, lower-bound
		// checks may be too optimistic and reducing the period can still destabilize
		// the system! enable this feature at your own risk.
		ਜੇ (en_period_ub_) {

			ਅਚਲ ਦਸ਼ਮਲਵ period_ub = period_ਉੱਤਲਾ_Bound(air_turn_ਦਰ, wind_ਗੁਣਕ, feas_on_track);

			ਜੇ (en_period_ub_ && PX4_ISFINITE(period_ub) && period > period_ub) {
				// NOTE: if the roll time constant is not accurately known, reducing
				// the period here can destabilize the system!
				// enable this feature at your own risk!

				// upper bound the period (for track keeping stability), prefer lower bound if violated
				ਅਚਲ ਦਸ਼ਮਲਵ period_adapted = math::max(period_lb, period_ub);

				// transition from the nominal period to the adapted period as we get
				// closer to the track
				period = period_adapted * track_proximity + (1.0f - track_proximity) * period;
			}
		}
	}

	ਵਾਪਸ period;
}

ਦਸ਼ਮਲਵ DirectionalGuidance::normalizedTrackError(ਅਚਲ ਦਸ਼ਮਲਵ track_error, ਅਚਲ ਦਸ਼ਮਲਵ track_error_bound) ਅਚਲ
{
	ਵਾਪਸ math::constrain(track_error / track_error_bound, 0.0f, 1.0f);
}

ਦਸ਼ਮਲਵ DirectionalGuidance::wind_ਗੁਣਕ_2(ਅਚਲ ਦਸ਼ਮਲਵ airspeed, ਅਚਲ ਦਸ਼ਮਲਵ wind_ਰਫਤਾਰ) ਅਚਲ
{
	// See [TODO: include citation] for definition/elaboration of this approximation.
	ਜੇ (wind_ਰਫਤਾਰ > airspeed || airspeed < NPFG_EPSILON) {
		ਵਾਪਸ 2.0f;

	} ਨਹੀਂਤਾਂ {
		ਵਾਪਸ 2.0f * (1.0f - sqrtf(1.0f - math::min(1.0f, wind_ਰਫਤਾਰ / airspeed)));
	}
}

ਦਸ਼ਮਲਵ DirectionalGuidance::period_ਉੱਤਲਾ_Bound(ਅਚਲ ਦਸ਼ਮਲਵ air_turn_ਦਰ, ਅਚਲ ਦਸ਼ਮਲਵ wind_ਗੁਣਕ,
		ਅਚਲ ਦਸ਼ਮਲਵ feas_on_track) ਅਚਲ
{
	ਜੇ (air_turn_ਦਰ * wind_ਗੁਣਕ > NPFG_EPSILON) {
		// multiply air turn rate by feasibility on track to zero out when we anyway
		// should not consider the curvature
		ਵਾਪਸ 4.0f * M_PI_F * damping_ / (air_turn_ਦਰ * wind_ਗੁਣਕ * feas_on_track);
	}

	ਵਾਪਸ INFINITY;
}

ਦਸ਼ਮਲਵ DirectionalGuidance::period_ਹੇਠਲਾ_Bound(ਅਚਲ ਦਸ਼ਮਲਵ air_turn_ਦਰ, ਅਚਲ ਦਸ਼ਮਲਵ wind_ਗੁਣਕ,
		ਅਚਲ ਦਸ਼ਮਲਵ feas_on_track) ਅਚਲ
{
	// this method considers a "conservative" lower period bound, i.e. a constant
	// worst case bound for any wind ratio, airspeed, and path curvature

	// the lower bound for zero curvature and no wind OR damping ratio < 0.5
	ਅਚਲ ਦਸ਼ਮਲਵ period_lb = M_PI_F * roll_ਸਮਾਂ_const / damping_;

	ਜੇ (air_turn_ਦਰ * wind_ਗੁਣਕ < NPFG_EPSILON || damping_ < 0.5f) {
		ਵਾਪਸ period_lb;

	} ਨਹੀਂਤਾਂ {
		// the lower bound for tracking a curved path in wind with damping ratio > 0.5
		ਅਚਲ ਦਸ਼ਮਲਵ period_windy_curved_damped = 4.0f * M_PI_F * roll_ਸਮਾਂ_const * damping_;

		// blend the two together as the bearing on track becomes less feasible
		ਵਾਪਸ period_windy_curved_damped * feas_on_track + (1.0f - feas_on_track) * period_lb;
	}
}

ਦਸ਼ਮਲਵ DirectionalGuidance::trackProximity(ਅਚਲ ਦਸ਼ਮਲਵ look_ahead_ang) ਅਚਲ
{
	ਅਚਲ ਦਸ਼ਮਲਵ sin_look_ahead_ang = sinf(look_ahead_ang);
	ਵਾਪਸ sin_look_ahead_ang * sin_look_ahead_ang;
}

ਦਸ਼ਮਲਵ DirectionalGuidance::trackErrorBound(ਅਚਲ ਦਸ਼ਮਲਵ ground_ਰਫਤਾਰ, ਅਚਲ ਦਸ਼ਮਲਵ ਸਮਾਂ_const) ਅਚਲ
{
	ਜੇ (ground_ਰਫਤਾਰ > 1.0f) {
		ਵਾਪਸ ground_ਰਫਤਾਰ * ਸਮਾਂ_const;

	} ਨਹੀਂਤਾਂ {
		// limit bound to some minimum ground speed to avoid singularities in track
		// error normalization. the following equation assumes ground speed minimum = 1.0
		ਵਾਪਸ 0.5f * ਸਮਾਂ_const * (ground_ਰਫਤਾਰ * ground_ਰਫਤਾਰ + 1.0f);
	}
}

ਦਸ਼ਮਲਵ DirectionalGuidance::ਸਮਾਂ_Const(ਅਚਲ ਦਸ਼ਮਲਵ period, ਅਚਲ ਦਸ਼ਮਲਵ damping) ਅਚਲ
{
	ਵਾਪਸ period * damping;
}

ਦਸ਼ਮਲਵ DirectionalGuidance::lookAheadAngle(ਅਚਲ ਦਸ਼ਮਲਵ normalized_track_error) ਅਚਲ
{
	ਵਾਪਸ M_PI_2_F * (normalized_track_error - 1.0f) * (normalized_track_error - 1.0f);
}


ਮੈਟ੍ਰਿਕਸ::Vector2f DirectionalGuidance::bearingVec(ਅਚਲ Vector2f &unit_ਰਸਤਾ_tangent, ਅਚਲ ਦਸ਼ਮਲਵ look_ahead_ang,
		ਅਚਲ ਦਸ਼ਮਲਵ signed_track_error) ਅਚਲ
{
	ਅਚਲ ਦਸ਼ਮਲਵ cos_look_ahead_ang = cosf(look_ahead_ang);
	ਅਚਲ ਦਸ਼ਮਲਵ sin_look_ahead_ang = sinf(look_ahead_ang);

	Vector2f unit_ਰਸਤਾ_ਸਾਧਾਰਨ(-unit_ਰਸਤਾ_tangent(1), unit_ਰਸਤਾ_tangent(0)); // right handed 90 deg (clockwise) turn
	Vector2f unit_track_error = -((signed_track_error < 0.0f) ? -1.0f : 1.0f) * unit_ਰਸਤਾ_ਸਾਧਾਰਨ;

	ਵਾਪਸ cos_look_ahead_ang * unit_track_error + sin_look_ahead_ang * unit_ਰਸਤਾ_tangent;
}

ਦਸ਼ਮਲਵ
DirectionalGuidance::bearingFeasibility(ਦਸ਼ਮਲਵ wind_cross_bearing, ਅਚਲ ਦਸ਼ਮਲਵ wind_dot_bearing, ਅਚਲ ਦਸ਼ਮਲਵ airspeed,
					ਅਚਲ ਦਸ਼ਮਲਵ wind_ਰਫਤਾਰ) ਅਚਲ
{
	ਜੇ (wind_dot_bearing < 0.0f) {
		wind_cross_bearing = wind_ਰਫਤਾਰ;

	} ਨਹੀਂਤਾਂ {
		wind_cross_bearing = fabsf(wind_cross_bearing);
	}

	ਦਸ਼ਮਲਵ sin_arg = sinf(M_PI_F * 0.5f * math::constrain((airspeed - wind_cross_bearing) / AIRSPEED_ਬਫਰ, 0.0f, 1.0f));
	ਵਾਪਸ sin_arg * sin_arg;
}

ਦਸ਼ਮਲਵ DirectionalGuidance::lateralAccelFF(ਅਚਲ Vector2f &unit_ਰਸਤਾ_tangent, ਅਚਲ Vector2f &ground_vel,
		ਅਚਲ ਦਸ਼ਮਲਵ wind_dot_upt, ਅਚਲ ਦਸ਼ਮਲਵ wind_cross_upt, ਅਚਲ ਦਸ਼ਮਲਵ airspeed,
		ਅਚਲ ਦਸ਼ਮਲਵ wind_ਰਫਤਾਰ, ਅਚਲ ਦਸ਼ਮਲਵ signed_track_error,
		ਅਚਲ ਦਸ਼ਮਲਵ ਰਸਤਾ_curvature) ਅਚਲ
{
	// NOTE: all calculations within this function take place at the closet point
	// on the path, as if the aircraft were already tracking the given path at
	// this point with zero angular error. this allows us to evaluate curvature
	// induced requirements for lateral acceleration incrementation and ramp them
	// in with the track proximity and further consider the bearing feasibility
	// in excess wind conditions (this is done external to this method).

	// path frame curvature is the instantaneous curvature at our current distance
	// from the actual path (considering e.g. concentric circles emanating outward/inward)
	ਅਚਲ ਦਸ਼ਮਲਵ ਰਸਤਾ_ਚੌਖਟਾ_curvature = ਰਸਤਾ_curvature / math::max(1.0f - ਰਸਤਾ_curvature * signed_track_error,
					   fabsf(ਰਸਤਾ_curvature) * MIN_RADIUS);

	// limit tangent ground speed to along track (forward moving) direction
	ਅਚਲ ਦਸ਼ਮਲਵ tangent_ground_ਰਫਤਾਰ = math::max(ground_vel.dot(unit_ਰਸਤਾ_tangent), 0.0f);

	ਅਚਲ ਦਸ਼ਮਲਵ ਰਸਤਾ_ਚੌਖਟਾ_ਦਰ = ਰਸਤਾ_ਚੌਖਟਾ_curvature * tangent_ground_ਰਫਤਾਰ;

	// speed ratio = projection of ground vel on track / projection of air velocity
	// on track
	ਅਚਲ ਦਸ਼ਮਲਵ ਰਫਤਾਰ_ਅਨੁਪਾਤ = (1.0f + wind_dot_upt / math::max(projectAirspOnBearing(airspeed, wind_cross_upt),
				   NPFG_EPSILON));

	// note the use of airspeed * speed_ratio as oppose to ground_speed^2 here --
	// the prior considers that we command lateral acceleration in the air mass
	// relative frame while the latter does not
	ਵਾਪਸ airspeed * ਰਫਤਾਰ_ਅਨੁਪਾਤ * ਰਸਤਾ_ਚੌਖਟਾ_ਦਰ;
}

ਦਸ਼ਮਲਵ DirectionalGuidance::projectAirspOnBearing(ਅਚਲ ਦਸ਼ਮਲਵ airspeed, ਅਚਲ ਦਸ਼ਮਲਵ wind_cross_bearing) ਅਚਲ
{
	// NOTE: wind_cross_bearing must be less than airspeed to use this function
	// it is assumed that bearing feasibility is checked and found feasible (e.g. bearingIsFeasible() = true) prior to entering this method
	// otherwise the return will be erroneous
	ਵਾਪਸ sqrtf(math::max(airspeed * airspeed - wind_cross_bearing * wind_cross_bearing, 0.0f));
}

ਦਸ਼ਮਲਵ DirectionalGuidance::switch_ਦੂਰੀ(ਦਸ਼ਮਲਵ wp_radius) ਅਚਲ
{
	ਵਾਪਸ math::min(wp_radius, track_error_bound_ * switch_ਦੂਰੀ_multiplier);
}
