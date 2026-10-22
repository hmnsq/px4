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
#অন্তর্ভাব "DirectionalGuidance.hpp"
using matrix::Vector2d;
using matrix::Vector2f;

DirectionalGuidance::DirectionalGuidance()
{

}

DirectionalGuidanceOutput
DirectionalGuidance::guideToPath(ধ্রুবক Vector2f &curr_pos_local, ধ্রুবক Vector2f &ground_vel, ধ্রুবক Vector2f &wind_vel,
				 ধ্রুবক Vector2f &unit_path_tangent, ধ্রুবক Vector2f &position_on_path,
				 ধ্রুবক দশমিক path_curvature)
{
	ধ্রুবক দশমিক ground_speed = ground_vel.norm();

	ধ্রুবক Vector2f air_vel = ground_vel - wind_vel;
	ধ্রুবক দশমিক airspeed = air_vel.norm();

	ধ্রুবক দশমিক wind_speed = wind_vel.norm();

	ধ্রুবক Vector2f path_pos_to_vehicle{curr_pos_local - position_on_path};
	signed_track_error_ = unit_path_tangent.cross(path_pos_to_vehicle);

	// on-track wind triangle projections
	ধ্রুবক দশমিক wind_cross_upt = wind_vel.cross(unit_path_tangent);
	ধ্রুবক দশমিক wind_dot_upt = wind_vel.dot(unit_path_tangent);

	// calculate the bearing feasibility on the track at the current closest point
	feas_on_track_ = bearingFeasibility(wind_cross_upt, wind_dot_upt, airspeed, wind_speed);

	ধ্রুবক দশমিক track_error = fabsf(signed_track_error_);

	// update control parameters considering upper and lower stability bounds (if enabled)
	// must be called before trackErrorBound() as it updates time_const_
	adapted_period_ = adaptPeriod(ground_speed, airspeed, wind_speed, track_error,
				      path_curvature, wind_vel, unit_path_tangent, feas_on_track_);
	ধ্রুবক দশমিক time_const = timeConst(adapted_period_, damping_);

	// track error bound is dynamic depending on ground speed
	track_error_bound_ = trackErrorBound(ground_speed, time_const);
	ধ্রুবক দশমিক normalized_track_error = normalizedTrackError(track_error, track_error_bound_);

	// look ahead angle based solely on track proximity
	ধ্রুবক দশমিক look_ahead_ang = lookAheadAngle(normalized_track_error);

	track_proximity_ = trackProximity(look_ahead_ang);

	bearing_vec_ = bearingVec(unit_path_tangent, look_ahead_ang, signed_track_error_);

	// wind triangle projections
	ধ্রুবক দশমিক wind_cross_bearing = wind_vel.cross(bearing_vec_);
	ধ্রুবক দশমিক wind_dot_bearing = wind_vel.dot(bearing_vec_);

	// continuous representation of the bearing feasibility
	feas_ = bearingFeasibility(wind_cross_bearing, wind_dot_bearing, airspeed, wind_speed);

	// we consider feasibility of both the current bearing as well as that on the track at the current closest point
	ধ্রুবক দশমিক feas_combined = feas_ * feas_on_track_;
	// lateral acceleration needed to stay on curved track (assuming no heading error)
	lateral_accel_ff_ = lateralAccelFF(unit_path_tangent, ground_vel, wind_dot_upt,
					   wind_cross_upt, airspeed, wind_speed, signed_track_error_, path_curvature) * feas_combined * track_proximity_;
	course_sp_ = atan2f(bearing_vec_(1), bearing_vec_(0));

	ফেরত DirectionalGuidanceOutput{.course_setpoint = course_sp_,
					 .lateral_acceleration_feedforward = lateral_accel_ff_};
}

দশমিক DirectionalGuidance::adaptPeriod(ধ্রুবক দশমিক ground_speed, ধ্রুবক দশমিক airspeed, ধ্রুবক দশমিক wind_speed,
				       ধ্রুবক দশমিক track_error, ধ্রুবক দশমিক path_curvature, ধ্রুবক Vector2f &wind_vel,
				       ধ্রুবক Vector2f &unit_path_tangent, ধ্রুবক দশমিক feas_on_track) ধ্রুবক
{
	দশমিক period = period_;
	ধ্রুবক দশমিক air_turn_rate = fabsf(path_curvature * airspeed);
	ধ্রুবক দশমিক wind_factor = windFactor(airspeed, wind_speed);

	যদি (en_period_lb_ && roll_time_const_ > NPFG_EPSILON) {
		// lower bound for period not considering path curvature
		ধ্রুবক দশমিক period_lb_zero_curvature = periodLowerBound(0.0f, wind_factor, feas_on_track) * period_safety_factor_;

		// lower bound for period *considering path curvature
		দশমিক period_lb = periodLowerBound(air_turn_rate, wind_factor, feas_on_track) * period_safety_factor_;

		// calculate the time constant and track error bound considering the zero
		// curvature, lower-bounded period and subsequently recalculate the normalized
		// track error
		ধ্রুবক দশমিক time_const = timeConst(period_lb_zero_curvature, damping_);
		ধ্রুবক দশমিক track_error_bound = trackErrorBound(ground_speed, time_const);
		ধ্রুবক দশমিক normalized_track_error = normalizedTrackError(track_error, track_error_bound);

		// calculate nominal track proximity with lower bounded time constant
		// (only a numerical solution can find corresponding track proximity
		// and adapted gains simultaneously)
		ধ্রুবক দশমিক look_ahead_ang = lookAheadAngle(normalized_track_error);
		ধ্রুবক দশমিক track_proximity = trackProximity(look_ahead_ang);

		// linearly ramp in curvature dependent lower bound with track proximity
		period_lb = period_lb * track_proximity + (1.0f - track_proximity) * period_lb_zero_curvature;

		// lower bounded period
		period = math::max(period_lb, period);

		// only allow upper bounding ONLY if lower bounding is enabled (is otherwise
		// dangerous to allow period decrements without stability checks).
		// NOTE: if the roll time constant is not accurately known, lower-bound
		// checks may be too optimistic and reducing the period can still destabilize
		// the system! enable this feature at your own risk.
		যদি (en_period_ub_) {

			ধ্রুবক দশমিক period_ub = periodUpperBound(air_turn_rate, wind_factor, feas_on_track);

			যদি (en_period_ub_ && PX4_ISFINITE(period_ub) && period > period_ub) {
				// NOTE: if the roll time constant is not accurately known, reducing
				// the period here can destabilize the system!
				// enable this feature at your own risk!

				// upper bound the period (for track keeping stability), prefer lower bound if violated
				ধ্রুবক দশমিক period_adapted = math::max(period_lb, period_ub);

				// transition from the nominal period to the adapted period as we get
				// closer to the track
				period = period_adapted * track_proximity + (1.0f - track_proximity) * period;
			}
		}
	}

	ফেরত period;
}

দশমিক DirectionalGuidance::normalizedTrackError(ধ্রুবক দশমিক track_error, ধ্রুবক দশমিক track_error_bound) ধ্রুবক
{
	ফেরত math::constrain(track_error / track_error_bound, 0.0f, 1.0f);
}

দশমিক DirectionalGuidance::windFactor(ধ্রুবক দশমিক airspeed, ধ্রুবক দশমিক wind_speed) ধ্রুবক
{
	// See [TODO: include citation] for definition/elaboration of this approximation.
	যদি (wind_speed > airspeed || airspeed < NPFG_EPSILON) {
		ফেরত 2.0f;

	} অন্যথা {
		ফেরত 2.0f * (1.0f - sqrtf(1.0f - math::min(1.0f, wind_speed / airspeed)));
	}
}

দশমিক DirectionalGuidance::periodUpperBound(ধ্রুবক দশমিক air_turn_rate, ধ্রুবক দশমিক wind_factor,
		ধ্রুবক দশমিক feas_on_track) ধ্রুবক
{
	যদি (air_turn_rate * wind_factor > NPFG_EPSILON) {
		// multiply air turn rate by feasibility on track to zero out when we anyway
		// should not consider the curvature
		ফেরত 4.0f * M_PI_F * damping_ / (air_turn_rate * wind_factor * feas_on_track);
	}

	ফেরত INFINITY;
}

দশমিক DirectionalGuidance::periodLowerBound(ধ্রুবক দশমিক air_turn_rate, ধ্রুবক দশমিক wind_factor,
		ধ্রুবক দশমিক feas_on_track) ধ্রুবক
{
	// this method considers a "conservative" lower period bound, i.e. a constant
	// worst case bound for any wind ratio, airspeed, and path curvature

	// the lower bound for zero curvature and no wind OR damping ratio < 0.5
	ধ্রুবক দশমিক period_lb = M_PI_F * roll_time_const_ / damping_;

	যদি (air_turn_rate * wind_factor < NPFG_EPSILON || damping_ < 0.5f) {
		ফেরত period_lb;

	} অন্যথা {
		// the lower bound for tracking a curved path in wind with damping ratio > 0.5
		ধ্রুবক দশমিক period_windy_curved_damped = 4.0f * M_PI_F * roll_time_const_ * damping_;

		// blend the two together as the bearing on track becomes less feasible
		ফেরত period_windy_curved_damped * feas_on_track + (1.0f - feas_on_track) * period_lb;
	}
}

দশমিক DirectionalGuidance::trackProximity(ধ্রুবক দশমিক look_ahead_ang) ধ্রুবক
{
	ধ্রুবক দশমিক sin_look_ahead_ang = sinf(look_ahead_ang);
	ফেরত sin_look_ahead_ang * sin_look_ahead_ang;
}

দশমিক DirectionalGuidance::trackErrorBound(ধ্রুবক দশমিক ground_speed, ধ্রুবক দশমিক time_const) ধ্রুবক
{
	যদি (ground_speed > 1.0f) {
		ফেরত ground_speed * time_const;

	} অন্যথা {
		// limit bound to some minimum ground speed to avoid singularities in track
		// error normalization. the following equation assumes ground speed minimum = 1.0
		ফেরত 0.5f * time_const * (ground_speed * ground_speed + 1.0f);
	}
}

দশমিক DirectionalGuidance::timeConst(ধ্রুবক দশমিক period, ধ্রুবক দশমিক damping) ধ্রুবক
{
	ফেরত period * damping;
}

দশমিক DirectionalGuidance::lookAheadAngle(ধ্রুবক দশমিক normalized_track_error) ধ্রুবক
{
	ফেরত M_PI_2_F * (normalized_track_error - 1.0f) * (normalized_track_error - 1.0f);
}


matrix::Vector2f DirectionalGuidance::bearingVec(ধ্রুবক Vector2f &unit_path_tangent, ধ্রুবক দশমিক look_ahead_ang,
		ধ্রুবক দশমিক signed_track_error) ধ্রুবক
{
	ধ্রুবক দশমিক cos_look_ahead_ang = cosf(look_ahead_ang);
	ধ্রুবক দশমিক sin_look_ahead_ang = sinf(look_ahead_ang);

	Vector2f unit_path_normal(-unit_path_tangent(1), unit_path_tangent(0)); // right handed 90 deg (clockwise) turn
	Vector2f unit_track_error = -((signed_track_error < 0.0f) ? -1.0f : 1.0f) * unit_path_normal;

	ফেরত cos_look_ahead_ang * unit_track_error + sin_look_ahead_ang * unit_path_tangent;
}

দশমিক
DirectionalGuidance::bearingFeasibility(দশমিক wind_cross_bearing, ধ্রুবক দশমিক wind_dot_bearing, ধ্রুবক দশমিক airspeed,
					ধ্রুবক দশমিক wind_speed) ধ্রুবক
{
	যদি (wind_dot_bearing < 0.0f) {
		wind_cross_bearing = wind_speed;

	} অন্যথা {
		wind_cross_bearing = fabsf(wind_cross_bearing);
	}

	দশমিক sin_arg = sinf(M_PI_F * 0.5f * math::constrain((airspeed - wind_cross_bearing) / AIRSPEED_বাফার, 0.0f, 1.0f));
	ফেরত sin_arg * sin_arg;
}

দশমিক DirectionalGuidance::lateralAccelFF(ধ্রুবক Vector2f &unit_path_tangent, ধ্রুবক Vector2f &ground_vel,
		ধ্রুবক দশমিক wind_dot_upt, ধ্রুবক দশমিক wind_cross_upt, ধ্রুবক দশমিক airspeed,
		ধ্রুবক দশমিক wind_speed, ধ্রুবক দশমিক signed_track_error,
		ধ্রুবক দশমিক path_curvature) ধ্রুবক
{
	// NOTE: all calculations within this function take place at the closet point
	// on the path, as if the aircraft were already tracking the given path at
	// this point with zero angular error. this allows us to evaluate curvature
	// induced requirements for lateral acceleration incrementation and ramp them
	// in with the track proximity and further consider the bearing feasibility
	// in excess wind conditions (this is done external to this method).

	// path frame curvature is the instantaneous curvature at our current distance
	// from the actual path (considering e.g. concentric circles emanating outward/inward)
	ধ্রুবক দশমিক path_frame_curvature = path_curvature / math::max(1.0f - path_curvature * signed_track_error,
					   fabsf(path_curvature) * MIN_RADIUS);

	// limit tangent ground speed to along track (forward moving) direction
	ধ্রুবক দশমিক tangent_ground_speed = math::max(ground_vel.dot(unit_path_tangent), 0.0f);

	ধ্রুবক দশমিক path_frame_rate = path_frame_curvature * tangent_ground_speed;

	// speed ratio = projection of ground vel on track / projection of air velocity
	// on track
	ধ্রুবক দশমিক speed_ratio = (1.0f + wind_dot_upt / math::max(projectAirspOnBearing(airspeed, wind_cross_upt),
				   NPFG_EPSILON));

	// note the use of airspeed * speed_ratio as oppose to ground_speed^2 here --
	// the prior considers that we command lateral acceleration in the air mass
	// relative frame while the latter does not
	ফেরত airspeed * speed_ratio * path_frame_rate;
}

দশমিক DirectionalGuidance::projectAirspOnBearing(ধ্রুবক দশমিক airspeed, ধ্রুবক দশমিক wind_cross_bearing) ধ্রুবক
{
	// NOTE: wind_cross_bearing must be less than airspeed to use this function
	// it is assumed that bearing feasibility is checked and found feasible (e.g. bearingIsFeasible() = true) prior to entering this method
	// otherwise the return will be erroneous
	ফেরত sqrtf(math::max(airspeed * airspeed - wind_cross_bearing * wind_cross_bearing, 0.0f));
}

দশমিক DirectionalGuidance::switchDistance(দশমিক wp_radius) ধ্রুবক
{
	ফেরত math::min(wp_radius, track_error_bound_ * switch_distance_multiplier_);
}
