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
#ಅನ್ತರ್ಭಾವ "DirectionalGuidance.hpp"
ಬಳಸಿ ಮಾತೃಕೆ::Vector2d;
ಬಳಸಿ ಮಾತೃಕೆ::Vector2f;

DirectionalGuidance::DirectionalGuidance()
{

}

DirectionalGuidanceOutput
DirectionalGuidance::guide_ಗಮ್ಯ_ಮಾರ್ಗ(ಸ್ಥಿರಾಂಕ Vector2f &curr_pos_local, ಸ್ಥಿರಾಂಕ Vector2f &ground_vel, ಸ್ಥಿರಾಂಕ Vector2f &wind_vel,
				 ಸ್ಥಿರಾಂಕ Vector2f &unit_ಮಾರ್ಗ_tangent, ಸ್ಥಿರಾಂಕ Vector2f &position_on_ಮಾರ್ಗ,
				 ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ ಮಾರ್ಗ_curvature)
{
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ ground_ವೇಗ = ground_vel.norm();

	ಸ್ಥಿರಾಂಕ Vector2f air_vel = ground_vel - wind_vel;
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ airspeed = air_vel.norm();

	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ wind_ವೇಗ = wind_vel.norm();

	ಸ್ಥಿರಾಂಕ Vector2f ಮಾರ್ಗ_pos_ಗಮ್ಯ_vehicle{curr_pos_local - position_on_ಮಾರ್ಗ};
	signed_track_error_ = unit_ಮಾರ್ಗ_tangent.cross(ಮಾರ್ಗ_pos_ಗಮ್ಯ_vehicle);

	// on-track wind triangle projections
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ wind_cross_upt = wind_vel.cross(unit_ಮಾರ್ಗ_tangent);
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ wind_dot_upt = wind_vel.dot(unit_ಮಾರ್ಗ_tangent);

	// calculate the bearing feasibility on the track at the current closest point
	feas_on_track_ = bearingFeasibility(wind_cross_upt, wind_dot_upt, airspeed, wind_ವೇಗ);

	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ track_error = fabsf(signed_track_error_);

	// update control parameters considering upper and lower stability bounds (if enabled)
	// must be called before trackErrorBound() as it updates time_const_
	adapted_period_ = adaptPeriod(ground_ವೇಗ, airspeed, wind_ವೇಗ, track_error,
				      ಮಾರ್ಗ_curvature, wind_vel, unit_ಮಾರ್ಗ_tangent, feas_on_track_);
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ ಸಮಯ_const = ಸಮಯ_Const(adapted_period_, damping_);

	// track error bound is dynamic depending on ground speed
	track_error_bound_ = trackErrorBound(ground_ವೇಗ, ಸಮಯ_const);
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ normalized_track_error = normalizedTrackError(track_error, track_error_bound_);

	// look ahead angle based solely on track proximity
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ look_ahead_ang = lookAheadAngle(normalized_track_error);

	track_proximity_ = trackProximity(look_ahead_ang);

	bearing_vec_ = bearingVec(unit_ಮಾರ್ಗ_tangent, look_ahead_ang, signed_track_error_);

	// wind triangle projections
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ wind_cross_bearing = wind_vel.cross(bearing_vec_);
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ wind_dot_bearing = wind_vel.dot(bearing_vec_);

	// continuous representation of the bearing feasibility
	feas_ = bearingFeasibility(wind_cross_bearing, wind_dot_bearing, airspeed, wind_ವೇಗ);

	// we consider feasibility of both the current bearing as well as that on the track at the current closest point
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ feas_combined = feas_ * feas_on_track_;
	// lateral acceleration needed to stay on curved track (assuming no heading error)
	lateral_accel_ff_ = lateralAccelFF(unit_ಮಾರ್ಗ_tangent, ground_vel, wind_dot_upt,
					   wind_cross_upt, airspeed, wind_ವೇಗ, signed_track_error_, ಮಾರ್ಗ_curvature) * feas_combined * track_proximity_;
	ಕೋರ್ಸು_sp = atan2f(bearing_vec_(1), bearing_vec_(0));

	ಹಿಂತಿರುಗಿ DirectionalGuidanceOutput{.ಕೋರ್ಸು_setpoint = ಕೋರ್ಸು_sp,
					 .lateral_acceleration_feedforward = lateral_accel_ff_};
}

ದಶಮಾಂಶ DirectionalGuidance::adaptPeriod(ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ ground_ವೇಗ, ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ airspeed, ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ wind_ವೇಗ,
				       ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ track_error, ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ ಮಾರ್ಗ_curvature, ಸ್ಥಿರಾಂಕ Vector2f &wind_vel,
				       ಸ್ಥಿರಾಂಕ Vector2f &unit_ಮಾರ್ಗ_tangent, ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ feas_on_track) ಸ್ಥಿರಾಂಕ
{
	ದಶಮಾಂಶ period = period_;
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ air_turn_ದರ = fabsf(ಮಾರ್ಗ_curvature * airspeed);
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ wind_ಗುಣಕ = wind_ಗುಣಕ_2(airspeed, wind_ವೇಗ);

	ಆದರೆ (en_period_lb_ && roll_ಸಮಯ_const > NPFG_EPSILON) {
		// lower bound for period not considering path curvature
		ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ period_lb_zero_curvature = period_ಅಧೋ_Bound(0.0f, wind_ಗುಣಕ, feas_on_track) * period_safety_ಗುಣಕ;

		// lower bound for period *considering path curvature
		ದಶಮಾಂಶ period_lb = period_ಅಧೋ_Bound(air_turn_ದರ, wind_ಗುಣಕ, feas_on_track) * period_safety_ಗುಣಕ;

		// calculate the time constant and track error bound considering the zero
		// curvature, lower-bounded period and subsequently recalculate the normalized
		// track error
		ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ ಸಮಯ_const = ಸಮಯ_Const(period_lb_zero_curvature, damping_);
		ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ track_error_bound = trackErrorBound(ground_ವೇಗ, ಸಮಯ_const);
		ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ normalized_track_error = normalizedTrackError(track_error, track_error_bound);

		// calculate nominal track proximity with lower bounded time constant
		// (only a numerical solution can find corresponding track proximity
		// and adapted gains simultaneously)
		ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ look_ahead_ang = lookAheadAngle(normalized_track_error);
		ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ track_proximity = trackProximity(look_ahead_ang);

		// linearly ramp in curvature dependent lower bound with track proximity
		period_lb = period_lb * track_proximity + (1.0f - track_proximity) * period_lb_zero_curvature;

		// lower bounded period
		period = math::max(period_lb, period);

		// only allow upper bounding ONLY if lower bounding is enabled (is otherwise
		// dangerous to allow period decrements without stability checks).
		// NOTE: if the roll time constant is not accurately known, lower-bound
		// checks may be too optimistic and reducing the period can still destabilize
		// the system! enable this feature at your own risk.
		ಆದರೆ (en_period_ub_) {

			ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ period_ub = period_ಊರ್ಧ್ವ_Bound(air_turn_ದರ, wind_ಗುಣಕ, feas_on_track);

			ಆದರೆ (en_period_ub_ && PX4_ISFINITE(period_ub) && period > period_ub) {
				// NOTE: if the roll time constant is not accurately known, reducing
				// the period here can destabilize the system!
				// enable this feature at your own risk!

				// upper bound the period (for track keeping stability), prefer lower bound if violated
				ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ period_adapted = math::max(period_lb, period_ub);

				// transition from the nominal period to the adapted period as we get
				// closer to the track
				period = period_adapted * track_proximity + (1.0f - track_proximity) * period;
			}
		}
	}

	ಹಿಂತಿರುಗಿ period;
}

ದಶಮಾಂಶ DirectionalGuidance::normalizedTrackError(ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ track_error, ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ track_error_bound) ಸ್ಥಿರಾಂಕ
{
	ಹಿಂತಿರುಗಿ math::constrain(track_error / track_error_bound, 0.0f, 1.0f);
}

ದಶಮಾಂಶ DirectionalGuidance::wind_ಗುಣಕ_2(ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ airspeed, ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ wind_ವೇಗ) ಸ್ಥಿರಾಂಕ
{
	// See [TODO: include citation] for definition/elaboration of this approximation.
	ಆದರೆ (wind_ವೇಗ > airspeed || airspeed < NPFG_EPSILON) {
		ಹಿಂತಿರುಗಿ 2.0f;

	} ಇಲ್ಲದಿದ್ದರೆ {
		ಹಿಂತಿರುಗಿ 2.0f * (1.0f - sqrtf(1.0f - math::min(1.0f, wind_ವೇಗ / airspeed)));
	}
}

ದಶಮಾಂಶ DirectionalGuidance::period_ಊರ್ಧ್ವ_Bound(ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ air_turn_ದರ, ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ wind_ಗುಣಕ,
		ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ feas_on_track) ಸ್ಥಿರಾಂಕ
{
	ಆದರೆ (air_turn_ದರ * wind_ಗುಣಕ > NPFG_EPSILON) {
		// multiply air turn rate by feasibility on track to zero out when we anyway
		// should not consider the curvature
		ಹಿಂತಿರುಗಿ 4.0f * M_PI_F * damping_ / (air_turn_ದರ * wind_ಗುಣಕ * feas_on_track);
	}

	ಹಿಂತಿರುಗಿ INFINITY;
}

ದಶಮಾಂಶ DirectionalGuidance::period_ಅಧೋ_Bound(ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ air_turn_ದರ, ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ wind_ಗುಣಕ,
		ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ feas_on_track) ಸ್ಥಿರಾಂಕ
{
	// this method considers a "conservative" lower period bound, i.e. a constant
	// worst case bound for any wind ratio, airspeed, and path curvature

	// the lower bound for zero curvature and no wind OR damping ratio < 0.5
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ period_lb = M_PI_F * roll_ಸಮಯ_const / damping_;

	ಆದರೆ (air_turn_ದರ * wind_ಗುಣಕ < NPFG_EPSILON || damping_ < 0.5f) {
		ಹಿಂತಿರುಗಿ period_lb;

	} ಇಲ್ಲದಿದ್ದರೆ {
		// the lower bound for tracking a curved path in wind with damping ratio > 0.5
		ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ period_windy_curved_damped = 4.0f * M_PI_F * roll_ಸಮಯ_const * damping_;

		// blend the two together as the bearing on track becomes less feasible
		ಹಿಂತಿರುಗಿ period_windy_curved_damped * feas_on_track + (1.0f - feas_on_track) * period_lb;
	}
}

ದಶಮಾಂಶ DirectionalGuidance::trackProximity(ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ look_ahead_ang) ಸ್ಥಿರಾಂಕ
{
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ sin_look_ahead_ang = sinf(look_ahead_ang);
	ಹಿಂತಿರುಗಿ sin_look_ahead_ang * sin_look_ahead_ang;
}

ದಶಮಾಂಶ DirectionalGuidance::trackErrorBound(ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ ground_ವೇಗ, ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ ಸಮಯ_const) ಸ್ಥಿರಾಂಕ
{
	ಆದರೆ (ground_ವೇಗ > 1.0f) {
		ಹಿಂತಿರುಗಿ ground_ವೇಗ * ಸಮಯ_const;

	} ಇಲ್ಲದಿದ್ದರೆ {
		// limit bound to some minimum ground speed to avoid singularities in track
		// error normalization. the following equation assumes ground speed minimum = 1.0
		ಹಿಂತಿರುಗಿ 0.5f * ಸಮಯ_const * (ground_ವೇಗ * ground_ವೇಗ + 1.0f);
	}
}

ದಶಮಾಂಶ DirectionalGuidance::ಸಮಯ_Const(ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ period, ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ damping) ಸ್ಥಿರಾಂಕ
{
	ಹಿಂತಿರುಗಿ period * damping;
}

ದಶಮಾಂಶ DirectionalGuidance::lookAheadAngle(ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ normalized_track_error) ಸ್ಥಿರಾಂಕ
{
	ಹಿಂತಿರುಗಿ M_PI_2_F * (normalized_track_error - 1.0f) * (normalized_track_error - 1.0f);
}


ಮಾತೃಕೆ::Vector2f DirectionalGuidance::bearingVec(ಸ್ಥಿರಾಂಕ Vector2f &unit_ಮಾರ್ಗ_tangent, ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ look_ahead_ang,
		ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ signed_track_error) ಸ್ಥಿರಾಂಕ
{
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ cos_look_ahead_ang = cosf(look_ahead_ang);
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ sin_look_ahead_ang = sinf(look_ahead_ang);

	Vector2f unit_ಮಾರ್ಗ_ಸಾಮಾನ್ಯ(-unit_ಮಾರ್ಗ_tangent(1), unit_ಮಾರ್ಗ_tangent(0)); // right handed 90 deg (clockwise) turn
	Vector2f unit_track_error = -((signed_track_error < 0.0f) ? -1.0f : 1.0f) * unit_ಮಾರ್ಗ_ಸಾಮಾನ್ಯ;

	ಹಿಂತಿರುಗಿ cos_look_ahead_ang * unit_track_error + sin_look_ahead_ang * unit_ಮಾರ್ಗ_tangent;
}

ದಶಮಾಂಶ
DirectionalGuidance::bearingFeasibility(ದಶಮಾಂಶ wind_cross_bearing, ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ wind_dot_bearing, ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ airspeed,
					ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ wind_ವೇಗ) ಸ್ಥಿರಾಂಕ
{
	ಆದರೆ (wind_dot_bearing < 0.0f) {
		wind_cross_bearing = wind_ವೇಗ;

	} ಇಲ್ಲದಿದ್ದರೆ {
		wind_cross_bearing = fabsf(wind_cross_bearing);
	}

	ದಶಮಾಂಶ sin_arg = sinf(M_PI_F * 0.5f * math::constrain((airspeed - wind_cross_bearing) / AIRSPEED_ಬಫರ್, 0.0f, 1.0f));
	ಹಿಂತಿರುಗಿ sin_arg * sin_arg;
}

ದಶಮಾಂಶ DirectionalGuidance::lateralAccelFF(ಸ್ಥಿರಾಂಕ Vector2f &unit_ಮಾರ್ಗ_tangent, ಸ್ಥಿರಾಂಕ Vector2f &ground_vel,
		ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ wind_dot_upt, ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ wind_cross_upt, ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ airspeed,
		ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ wind_ವೇಗ, ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ signed_track_error,
		ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ ಮಾರ್ಗ_curvature) ಸ್ಥಿರಾಂಕ
{
	// NOTE: all calculations within this function take place at the closet point
	// on the path, as if the aircraft were already tracking the given path at
	// this point with zero angular error. this allows us to evaluate curvature
	// induced requirements for lateral acceleration incrementation and ramp them
	// in with the track proximity and further consider the bearing feasibility
	// in excess wind conditions (this is done external to this method).

	// path frame curvature is the instantaneous curvature at our current distance
	// from the actual path (considering e.g. concentric circles emanating outward/inward)
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ ಮಾರ್ಗ_ಚೌಕಟ್ಟು_curvature = ಮಾರ್ಗ_curvature / math::max(1.0f - ಮಾರ್ಗ_curvature * signed_track_error,
					   fabsf(ಮಾರ್ಗ_curvature) * MIN_RADIUS);

	// limit tangent ground speed to along track (forward moving) direction
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ tangent_ground_ವೇಗ = math::max(ground_vel.dot(unit_ಮಾರ್ಗ_tangent), 0.0f);

	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ ಮಾರ್ಗ_ಚೌಕಟ್ಟು_ದರ = ಮಾರ್ಗ_ಚೌಕಟ್ಟು_curvature * tangent_ground_ವೇಗ;

	// speed ratio = projection of ground vel on track / projection of air velocity
	// on track
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ ವೇಗ_ಅನುಪಾತ = (1.0f + wind_dot_upt / math::max(projectAirspOnBearing(airspeed, wind_cross_upt),
				   NPFG_EPSILON));

	// note the use of airspeed * speed_ratio as oppose to ground_speed^2 here --
	// the prior considers that we command lateral acceleration in the air mass
	// relative frame while the latter does not
	ಹಿಂತಿರುಗಿ airspeed * ವೇಗ_ಅನುಪಾತ * ಮಾರ್ಗ_ಚೌಕಟ್ಟು_ದರ;
}

ದಶಮಾಂಶ DirectionalGuidance::projectAirspOnBearing(ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ airspeed, ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ wind_cross_bearing) ಸ್ಥಿರಾಂಕ
{
	// NOTE: wind_cross_bearing must be less than airspeed to use this function
	// it is assumed that bearing feasibility is checked and found feasible (e.g. bearingIsFeasible() = true) prior to entering this method
	// otherwise the return will be erroneous
	ಹಿಂತಿರುಗಿ sqrtf(math::max(airspeed * airspeed - wind_cross_bearing * wind_cross_bearing, 0.0f));
}

ದಶಮಾಂಶ DirectionalGuidance::switch_ದೂರ(ದಶಮಾಂಶ wp_radius) ಸ್ಥಿರಾಂಕ
{
	ಹಿಂತಿರುಗಿ math::min(wp_radius, track_error_bound_ * switch_ದೂರ_multiplier);
}
