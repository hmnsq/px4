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
#شامل "DirectionalGuidance.hpp"
استعمال میٹرکس::Vector2d;
استعمال میٹرکس::Vector2f;

DirectionalGuidance::DirectionalGuidance()
{

}

DirectionalGuidanceOutput
DirectionalGuidance::guide_مقصد_راستہ(ثابت Vector2f &curr_pos_local, ثابت Vector2f &ground_vel, ثابت Vector2f &wind_vel,
				 ثابت Vector2f &unit_راستہ_tangent, ثابت Vector2f &position_on_راستہ,
				 ثابت اعشاری راستہ_curvature)
{
	ثابت اعشاری ground_رفتار = ground_vel.norm();

	ثابت Vector2f air_vel = ground_vel - wind_vel;
	ثابت اعشاری airspeed = air_vel.norm();

	ثابت اعشاری wind_رفتار = wind_vel.norm();

	ثابت Vector2f راستہ_pos_مقصد_vehicle{curr_pos_local - position_on_راستہ};
	signed_track_error_ = unit_راستہ_tangent.cross(راستہ_pos_مقصد_vehicle);

	// on-track wind triangle projections
	ثابت اعشاری wind_cross_upt = wind_vel.cross(unit_راستہ_tangent);
	ثابت اعشاری wind_dot_upt = wind_vel.dot(unit_راستہ_tangent);

	// calculate the bearing feasibility on the track at the current closest point
	feas_on_track_ = bearingFeasibility(wind_cross_upt, wind_dot_upt, airspeed, wind_رفتار);

	ثابت اعشاری track_error = fabsf(signed_track_error_);

	// update control parameters considering upper and lower stability bounds (if enabled)
	// must be called before trackErrorBound() as it updates time_const_
	adapted_period_ = adaptPeriod(ground_رفتار, airspeed, wind_رفتار, track_error,
				      راستہ_curvature, wind_vel, unit_راستہ_tangent, feas_on_track_);
	ثابت اعشاری وقت_const = وقت_Const(adapted_period_, damping_);

	// track error bound is dynamic depending on ground speed
	track_error_bound_ = trackErrorBound(ground_رفتار, وقت_const);
	ثابت اعشاری normalized_track_error = normalizedTrackError(track_error, track_error_bound_);

	// look ahead angle based solely on track proximity
	ثابت اعشاری look_ahead_ang = lookAheadAngle(normalized_track_error);

	track_proximity_ = trackProximity(look_ahead_ang);

	bearing_vec_ = bearingVec(unit_راستہ_tangent, look_ahead_ang, signed_track_error_);

	// wind triangle projections
	ثابت اعشاری wind_cross_bearing = wind_vel.cross(bearing_vec_);
	ثابت اعشاری wind_dot_bearing = wind_vel.dot(bearing_vec_);

	// continuous representation of the bearing feasibility
	feas_ = bearingFeasibility(wind_cross_bearing, wind_dot_bearing, airspeed, wind_رفتار);

	// we consider feasibility of both the current bearing as well as that on the track at the current closest point
	ثابت اعشاری feas_combined = feas_ * feas_on_track_;
	// lateral acceleration needed to stay on curved track (assuming no heading error)
	lateral_accel_ff_ = lateralAccelFF(unit_راستہ_tangent, ground_vel, wind_dot_upt,
					   wind_cross_upt, airspeed, wind_رفتار, signed_track_error_, راستہ_curvature) * feas_combined * track_proximity_;
	نصاب_sp = atan2f(bearing_vec_(1), bearing_vec_(0));

	واپس DirectionalGuidanceOutput{.نصاب_setpoint = نصاب_sp,
					 .lateral_acceleration_feedforward = lateral_accel_ff_};
}

اعشاری DirectionalGuidance::adaptPeriod(ثابت اعشاری ground_رفتار, ثابت اعشاری airspeed, ثابت اعشاری wind_رفتار,
				       ثابت اعشاری track_error, ثابت اعشاری راستہ_curvature, ثابت Vector2f &wind_vel,
				       ثابت Vector2f &unit_راستہ_tangent, ثابت اعشاری feas_on_track) ثابت
{
	اعشاری period = period_;
	ثابت اعشاری air_turn_شرح = fabsf(راستہ_curvature * airspeed);
	ثابت اعشاری wind_ضربی_عدد = wind_ضربی_عدد_2(airspeed, wind_رفتار);

	اگر (en_period_lb_ && roll_وقت_const > NPFG_EPSILON) {
		// lower bound for period not considering path curvature
		ثابت اعشاری period_lb_zero_curvature = period_زیریں_Bound(0.0f, wind_ضربی_عدد, feas_on_track) * period_safety_ضربی_عدد;

		// lower bound for period *considering path curvature
		اعشاری period_lb = period_زیریں_Bound(air_turn_شرح, wind_ضربی_عدد, feas_on_track) * period_safety_ضربی_عدد;

		// calculate the time constant and track error bound considering the zero
		// curvature, lower-bounded period and subsequently recalculate the normalized
		// track error
		ثابت اعشاری وقت_const = وقت_Const(period_lb_zero_curvature, damping_);
		ثابت اعشاری track_error_bound = trackErrorBound(ground_رفتار, وقت_const);
		ثابت اعشاری normalized_track_error = normalizedTrackError(track_error, track_error_bound);

		// calculate nominal track proximity with lower bounded time constant
		// (only a numerical solution can find corresponding track proximity
		// and adapted gains simultaneously)
		ثابت اعشاری look_ahead_ang = lookAheadAngle(normalized_track_error);
		ثابت اعشاری track_proximity = trackProximity(look_ahead_ang);

		// linearly ramp in curvature dependent lower bound with track proximity
		period_lb = period_lb * track_proximity + (1.0f - track_proximity) * period_lb_zero_curvature;

		// lower bounded period
		period = math::max(period_lb, period);

		// only allow upper bounding ONLY if lower bounding is enabled (is otherwise
		// dangerous to allow period decrements without stability checks).
		// NOTE: if the roll time constant is not accurately known, lower-bound
		// checks may be too optimistic and reducing the period can still destabilize
		// the system! enable this feature at your own risk.
		اگر (en_period_ub_) {

			ثابت اعشاری period_ub = period_بالائی_Bound(air_turn_شرح, wind_ضربی_عدد, feas_on_track);

			اگر (en_period_ub_ && PX4_ISFINITE(period_ub) && period > period_ub) {
				// NOTE: if the roll time constant is not accurately known, reducing
				// the period here can destabilize the system!
				// enable this feature at your own risk!

				// upper bound the period (for track keeping stability), prefer lower bound if violated
				ثابت اعشاری period_adapted = math::max(period_lb, period_ub);

				// transition from the nominal period to the adapted period as we get
				// closer to the track
				period = period_adapted * track_proximity + (1.0f - track_proximity) * period;
			}
		}
	}

	واپس period;
}

اعشاری DirectionalGuidance::normalizedTrackError(ثابت اعشاری track_error, ثابت اعشاری track_error_bound) ثابت
{
	واپس math::constrain(track_error / track_error_bound, 0.0f, 1.0f);
}

اعشاری DirectionalGuidance::wind_ضربی_عدد_2(ثابت اعشاری airspeed, ثابت اعشاری wind_رفتار) ثابت
{
	// See [TODO: include citation] for definition/elaboration of this approximation.
	اگر (wind_رفتار > airspeed || airspeed < NPFG_EPSILON) {
		واپس 2.0f;

	} ورنہ {
		واپس 2.0f * (1.0f - sqrtf(1.0f - math::min(1.0f, wind_رفتار / airspeed)));
	}
}

اعشاری DirectionalGuidance::period_بالائی_Bound(ثابت اعشاری air_turn_شرح, ثابت اعشاری wind_ضربی_عدد,
		ثابت اعشاری feas_on_track) ثابت
{
	اگر (air_turn_شرح * wind_ضربی_عدد > NPFG_EPSILON) {
		// multiply air turn rate by feasibility on track to zero out when we anyway
		// should not consider the curvature
		واپس 4.0f * M_PI_F * damping_ / (air_turn_شرح * wind_ضربی_عدد * feas_on_track);
	}

	واپس INFINITY;
}

اعشاری DirectionalGuidance::period_زیریں_Bound(ثابت اعشاری air_turn_شرح, ثابت اعشاری wind_ضربی_عدد,
		ثابت اعشاری feas_on_track) ثابت
{
	// this method considers a "conservative" lower period bound, i.e. a constant
	// worst case bound for any wind ratio, airspeed, and path curvature

	// the lower bound for zero curvature and no wind OR damping ratio < 0.5
	ثابت اعشاری period_lb = M_PI_F * roll_وقت_const / damping_;

	اگر (air_turn_شرح * wind_ضربی_عدد < NPFG_EPSILON || damping_ < 0.5f) {
		واپس period_lb;

	} ورنہ {
		// the lower bound for tracking a curved path in wind with damping ratio > 0.5
		ثابت اعشاری period_windy_curved_damped = 4.0f * M_PI_F * roll_وقت_const * damping_;

		// blend the two together as the bearing on track becomes less feasible
		واپس period_windy_curved_damped * feas_on_track + (1.0f - feas_on_track) * period_lb;
	}
}

اعشاری DirectionalGuidance::trackProximity(ثابت اعشاری look_ahead_ang) ثابت
{
	ثابت اعشاری sin_look_ahead_ang = sinf(look_ahead_ang);
	واپس sin_look_ahead_ang * sin_look_ahead_ang;
}

اعشاری DirectionalGuidance::trackErrorBound(ثابت اعشاری ground_رفتار, ثابت اعشاری وقت_const) ثابت
{
	اگر (ground_رفتار > 1.0f) {
		واپس ground_رفتار * وقت_const;

	} ورنہ {
		// limit bound to some minimum ground speed to avoid singularities in track
		// error normalization. the following equation assumes ground speed minimum = 1.0
		واپس 0.5f * وقت_const * (ground_رفتار * ground_رفتار + 1.0f);
	}
}

اعشاری DirectionalGuidance::وقت_Const(ثابت اعشاری period, ثابت اعشاری damping) ثابت
{
	واپس period * damping;
}

اعشاری DirectionalGuidance::lookAheadAngle(ثابت اعشاری normalized_track_error) ثابت
{
	واپس M_PI_2_F * (normalized_track_error - 1.0f) * (normalized_track_error - 1.0f);
}


میٹرکس::Vector2f DirectionalGuidance::bearingVec(ثابت Vector2f &unit_راستہ_tangent, ثابت اعشاری look_ahead_ang,
		ثابت اعشاری signed_track_error) ثابت
{
	ثابت اعشاری cos_look_ahead_ang = cosf(look_ahead_ang);
	ثابت اعشاری sin_look_ahead_ang = sinf(look_ahead_ang);

	Vector2f unit_راستہ_معمول(-unit_راستہ_tangent(1), unit_راستہ_tangent(0)); // right handed 90 deg (clockwise) turn
	Vector2f unit_track_error = -((signed_track_error < 0.0f) ? -1.0f : 1.0f) * unit_راستہ_معمول;

	واپس cos_look_ahead_ang * unit_track_error + sin_look_ahead_ang * unit_راستہ_tangent;
}

اعشاری
DirectionalGuidance::bearingFeasibility(اعشاری wind_cross_bearing, ثابت اعشاری wind_dot_bearing, ثابت اعشاری airspeed,
					ثابت اعشاری wind_رفتار) ثابت
{
	اگر (wind_dot_bearing < 0.0f) {
		wind_cross_bearing = wind_رفتار;

	} ورنہ {
		wind_cross_bearing = fabsf(wind_cross_bearing);
	}

	اعشاری sin_arg = sinf(M_PI_F * 0.5f * math::constrain((airspeed - wind_cross_bearing) / AIRSPEED_بفر, 0.0f, 1.0f));
	واپس sin_arg * sin_arg;
}

اعشاری DirectionalGuidance::lateralAccelFF(ثابت Vector2f &unit_راستہ_tangent, ثابت Vector2f &ground_vel,
		ثابت اعشاری wind_dot_upt, ثابت اعشاری wind_cross_upt, ثابت اعشاری airspeed,
		ثابت اعشاری wind_رفتار, ثابت اعشاری signed_track_error,
		ثابت اعشاری راستہ_curvature) ثابت
{
	// NOTE: all calculations within this function take place at the closet point
	// on the path, as if the aircraft were already tracking the given path at
	// this point with zero angular error. this allows us to evaluate curvature
	// induced requirements for lateral acceleration incrementation and ramp them
	// in with the track proximity and further consider the bearing feasibility
	// in excess wind conditions (this is done external to this method).

	// path frame curvature is the instantaneous curvature at our current distance
	// from the actual path (considering e.g. concentric circles emanating outward/inward)
	ثابت اعشاری راستہ_چوکھٹا_curvature = راستہ_curvature / math::max(1.0f - راستہ_curvature * signed_track_error,
					   fabsf(راستہ_curvature) * MIN_RADIUS);

	// limit tangent ground speed to along track (forward moving) direction
	ثابت اعشاری tangent_ground_رفتار = math::max(ground_vel.dot(unit_راستہ_tangent), 0.0f);

	ثابت اعشاری راستہ_چوکھٹا_شرح = راستہ_چوکھٹا_curvature * tangent_ground_رفتار;

	// speed ratio = projection of ground vel on track / projection of air velocity
	// on track
	ثابت اعشاری رفتار_تناسب = (1.0f + wind_dot_upt / math::max(projectAirspOnBearing(airspeed, wind_cross_upt),
				   NPFG_EPSILON));

	// note the use of airspeed * speed_ratio as oppose to ground_speed^2 here --
	// the prior considers that we command lateral acceleration in the air mass
	// relative frame while the latter does not
	واپس airspeed * رفتار_تناسب * راستہ_چوکھٹا_شرح;
}

اعشاری DirectionalGuidance::projectAirspOnBearing(ثابت اعشاری airspeed, ثابت اعشاری wind_cross_bearing) ثابت
{
	// NOTE: wind_cross_bearing must be less than airspeed to use this function
	// it is assumed that bearing feasibility is checked and found feasible (e.g. bearingIsFeasible() = true) prior to entering this method
	// otherwise the return will be erroneous
	واپس sqrtf(math::max(airspeed * airspeed - wind_cross_bearing * wind_cross_bearing, 0.0f));
}

اعشاری DirectionalGuidance::switch_فاصلہ(اعشاری wp_radius) ثابت
{
	واپس math::min(wp_radius, track_error_bound_ * switch_فاصلہ_multiplier);
}
