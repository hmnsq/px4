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
#अन्तर्भाव "DirectionalGuidance.hpp"
using matrix::Vector2d;
using matrix::Vector2f;

DirectionalGuidance::DirectionalGuidance()
{

}

DirectionalGuidanceOutput
DirectionalGuidance::guideToPath(ध्रुव Vector2f &curr_pos_local, ध्रुव Vector2f &ground_vel, ध्रुव Vector2f &wind_vel,
				 ध्रुव Vector2f &unit_path_tangent, ध्रुव Vector2f &position_on_path,
				 ध्रुव भिन्न path_curvature)
{
	ध्रुव भिन्न ground_speed = ground_vel.norm();

	ध्रुव Vector2f air_vel = ground_vel - wind_vel;
	ध्रुव भिन्न airspeed = air_vel.norm();

	ध्रुव भिन्न wind_speed = wind_vel.norm();

	ध्रुव Vector2f path_pos_to_vehicle{curr_pos_local - position_on_path};
	signed_track_error_ = unit_path_tangent.cross(path_pos_to_vehicle);

	// on-track wind triangle projections
	ध्रुव भिन्न wind_cross_upt = wind_vel.cross(unit_path_tangent);
	ध्रुव भिन्न wind_dot_upt = wind_vel.dot(unit_path_tangent);

	// calculate the bearing feasibility on the track at the current closest point
	feas_on_track_ = bearingFeasibility(wind_cross_upt, wind_dot_upt, airspeed, wind_speed);

	ध्रुव भिन्न track_error = fabsf(signed_track_error_);

	// update control parameters considering upper and lower stability bounds (if enabled)
	// must be called before trackErrorBound() as it updates time_const_
	adapted_period_ = adaptPeriod(ground_speed, airspeed, wind_speed, track_error,
				      path_curvature, wind_vel, unit_path_tangent, feas_on_track_);
	ध्रुव भिन्न time_const = timeConst(adapted_period_, damping_);

	// track error bound is dynamic depending on ground speed
	track_error_bound_ = trackErrorBound(ground_speed, time_const);
	ध्रुव भिन्न normalized_track_error = normalizedTrackError(track_error, track_error_bound_);

	// look ahead angle based solely on track proximity
	ध्रुव भिन्न look_ahead_ang = lookAheadAngle(normalized_track_error);

	track_proximity_ = trackProximity(look_ahead_ang);

	bearing_vec_ = bearingVec(unit_path_tangent, look_ahead_ang, signed_track_error_);

	// wind triangle projections
	ध्रुव भिन्न wind_cross_bearing = wind_vel.cross(bearing_vec_);
	ध्रुव भिन्न wind_dot_bearing = wind_vel.dot(bearing_vec_);

	// continuous representation of the bearing feasibility
	feas_ = bearingFeasibility(wind_cross_bearing, wind_dot_bearing, airspeed, wind_speed);

	// we consider feasibility of both the current bearing as well as that on the track at the current closest point
	ध्रुव भिन्न feas_combined = feas_ * feas_on_track_;
	// lateral acceleration needed to stay on curved track (assuming no heading error)
	lateral_accel_ff_ = lateralAccelFF(unit_path_tangent, ground_vel, wind_dot_upt,
					   wind_cross_upt, airspeed, wind_speed, signed_track_error_, path_curvature) * feas_combined * track_proximity_;
	course_sp_ = atan2f(bearing_vec_(1), bearing_vec_(0));

	प्रत्यागम DirectionalGuidanceOutput{.course_setpoint = course_sp_,
					 .lateral_acceleration_feedforward = lateral_accel_ff_};
}

भिन्न DirectionalGuidance::adaptPeriod(ध्रुव भिन्न ground_speed, ध्रुव भिन्न airspeed, ध्रुव भिन्न wind_speed,
				       ध्रुव भिन्न track_error, ध्रुव भिन्न path_curvature, ध्रुव Vector2f &wind_vel,
				       ध्रुव Vector2f &unit_path_tangent, ध्रुव भिन्न feas_on_track) ध्रुव
{
	भिन्न period = period_;
	ध्रुव भिन्न air_turn_rate = fabsf(path_curvature * airspeed);
	ध्रुव भिन्न wind_factor = windFactor(airspeed, wind_speed);

	यदि (en_period_lb_ && roll_time_const_ > NPFG_EPSILON) {
		// lower bound for period not considering path curvature
		ध्रुव भिन्न period_lb_zero_curvature = periodLowerBound(0.0f, wind_factor, feas_on_track) * period_safety_factor_;

		// lower bound for period *considering path curvature
		भिन्न period_lb = periodLowerBound(air_turn_rate, wind_factor, feas_on_track) * period_safety_factor_;

		// calculate the time constant and track error bound considering the zero
		// curvature, lower-bounded period and subsequently recalculate the normalized
		// track error
		ध्रुव भिन्न time_const = timeConst(period_lb_zero_curvature, damping_);
		ध्रुव भिन्न track_error_bound = trackErrorBound(ground_speed, time_const);
		ध्रुव भिन्न normalized_track_error = normalizedTrackError(track_error, track_error_bound);

		// calculate nominal track proximity with lower bounded time constant
		// (only a numerical solution can find corresponding track proximity
		// and adapted gains simultaneously)
		ध्रुव भिन्न look_ahead_ang = lookAheadAngle(normalized_track_error);
		ध्रुव भिन्न track_proximity = trackProximity(look_ahead_ang);

		// linearly ramp in curvature dependent lower bound with track proximity
		period_lb = period_lb * track_proximity + (1.0f - track_proximity) * period_lb_zero_curvature;

		// lower bounded period
		period = math::max(period_lb, period);

		// only allow upper bounding ONLY if lower bounding is enabled (is otherwise
		// dangerous to allow period decrements without stability checks).
		// NOTE: if the roll time constant is not accurately known, lower-bound
		// checks may be too optimistic and reducing the period can still destabilize
		// the system! enable this feature at your own risk.
		यदि (en_period_ub_) {

			ध्रुव भिन्न period_ub = periodUpperBound(air_turn_rate, wind_factor, feas_on_track);

			यदि (en_period_ub_ && PX4_ISFINITE(period_ub) && period > period_ub) {
				// NOTE: if the roll time constant is not accurately known, reducing
				// the period here can destabilize the system!
				// enable this feature at your own risk!

				// upper bound the period (for track keeping stability), prefer lower bound if violated
				ध्रुव भिन्न period_adapted = math::max(period_lb, period_ub);

				// transition from the nominal period to the adapted period as we get
				// closer to the track
				period = period_adapted * track_proximity + (1.0f - track_proximity) * period;
			}
		}
	}

	प्रत्यागम period;
}

भिन्न DirectionalGuidance::normalizedTrackError(ध्रुव भिन्न track_error, ध्रुव भिन्न track_error_bound) ध्रुव
{
	प्रत्यागम math::constrain(track_error / track_error_bound, 0.0f, 1.0f);
}

भिन्न DirectionalGuidance::windFactor(ध्रुव भिन्न airspeed, ध्रुव भिन्न wind_speed) ध्रुव
{
	// See [TODO: include citation] for definition/elaboration of this approximation.
	यदि (wind_speed > airspeed || airspeed < NPFG_EPSILON) {
		प्रत्यागम 2.0f;

	} अन्यथा {
		प्रत्यागम 2.0f * (1.0f - sqrtf(1.0f - math::min(1.0f, wind_speed / airspeed)));
	}
}

भिन्न DirectionalGuidance::periodUpperBound(ध्रुव भिन्न air_turn_rate, ध्रुव भिन्न wind_factor,
		ध्रुव भिन्न feas_on_track) ध्रुव
{
	यदि (air_turn_rate * wind_factor > NPFG_EPSILON) {
		// multiply air turn rate by feasibility on track to zero out when we anyway
		// should not consider the curvature
		प्रत्यागम 4.0f * M_PI_F * damping_ / (air_turn_rate * wind_factor * feas_on_track);
	}

	प्रत्यागम INFINITY;
}

भिन्न DirectionalGuidance::periodLowerBound(ध्रुव भिन्न air_turn_rate, ध्रुव भिन्न wind_factor,
		ध्रुव भिन्न feas_on_track) ध्रुव
{
	// this method considers a "conservative" lower period bound, i.e. a constant
	// worst case bound for any wind ratio, airspeed, and path curvature

	// the lower bound for zero curvature and no wind OR damping ratio < 0.5
	ध्रुव भिन्न period_lb = M_PI_F * roll_time_const_ / damping_;

	यदि (air_turn_rate * wind_factor < NPFG_EPSILON || damping_ < 0.5f) {
		प्रत्यागम period_lb;

	} अन्यथा {
		// the lower bound for tracking a curved path in wind with damping ratio > 0.5
		ध्रुव भिन्न period_windy_curved_damped = 4.0f * M_PI_F * roll_time_const_ * damping_;

		// blend the two together as the bearing on track becomes less feasible
		प्रत्यागम period_windy_curved_damped * feas_on_track + (1.0f - feas_on_track) * period_lb;
	}
}

भिन्न DirectionalGuidance::trackProximity(ध्रुव भिन्न look_ahead_ang) ध्रुव
{
	ध्रुव भिन्न sin_look_ahead_ang = sinf(look_ahead_ang);
	प्रत्यागम sin_look_ahead_ang * sin_look_ahead_ang;
}

भिन्न DirectionalGuidance::trackErrorBound(ध्रुव भिन्न ground_speed, ध्रुव भिन्न time_const) ध्रुव
{
	यदि (ground_speed > 1.0f) {
		प्रत्यागम ground_speed * time_const;

	} अन्यथा {
		// limit bound to some minimum ground speed to avoid singularities in track
		// error normalization. the following equation assumes ground speed minimum = 1.0
		प्रत्यागम 0.5f * time_const * (ground_speed * ground_speed + 1.0f);
	}
}

भिन्न DirectionalGuidance::timeConst(ध्रुव भिन्न period, ध्रुव भिन्न damping) ध्रुव
{
	प्रत्यागम period * damping;
}

भिन्न DirectionalGuidance::lookAheadAngle(ध्रुव भिन्न normalized_track_error) ध्रुव
{
	प्रत्यागम M_PI_2_F * (normalized_track_error - 1.0f) * (normalized_track_error - 1.0f);
}


matrix::Vector2f DirectionalGuidance::bearingVec(ध्रुव Vector2f &unit_path_tangent, ध्रुव भिन्न look_ahead_ang,
		ध्रुव भिन्न signed_track_error) ध्रुव
{
	ध्रुव भिन्न cos_look_ahead_ang = cosf(look_ahead_ang);
	ध्रुव भिन्न sin_look_ahead_ang = sinf(look_ahead_ang);

	Vector2f unit_path_normal(-unit_path_tangent(1), unit_path_tangent(0)); // right handed 90 deg (clockwise) turn
	Vector2f unit_track_error = -((signed_track_error < 0.0f) ? -1.0f : 1.0f) * unit_path_normal;

	प्रत्यागम cos_look_ahead_ang * unit_track_error + sin_look_ahead_ang * unit_path_tangent;
}

भिन्न
DirectionalGuidance::bearingFeasibility(भिन्न wind_cross_bearing, ध्रुव भिन्न wind_dot_bearing, ध्रुव भिन्न airspeed,
					ध्रुव भिन्न wind_speed) ध्रुव
{
	यदि (wind_dot_bearing < 0.0f) {
		wind_cross_bearing = wind_speed;

	} अन्यथा {
		wind_cross_bearing = fabsf(wind_cross_bearing);
	}

	भिन्न sin_arg = sinf(M_PI_F * 0.5f * math::constrain((airspeed - wind_cross_bearing) / AIRSPEED_BUFFER, 0.0f, 1.0f));
	प्रत्यागम sin_arg * sin_arg;
}

भिन्न DirectionalGuidance::lateralAccelFF(ध्रुव Vector2f &unit_path_tangent, ध्रुव Vector2f &ground_vel,
		ध्रुव भिन्न wind_dot_upt, ध्रुव भिन्न wind_cross_upt, ध्रुव भिन्न airspeed,
		ध्रुव भिन्न wind_speed, ध्रुव भिन्न signed_track_error,
		ध्रुव भिन्न path_curvature) ध्रुव
{
	// NOTE: all calculations within this function take place at the closet point
	// on the path, as if the aircraft were already tracking the given path at
	// this point with zero angular error. this allows us to evaluate curvature
	// induced requirements for lateral acceleration incrementation and ramp them
	// in with the track proximity and further consider the bearing feasibility
	// in excess wind conditions (this is done external to this method).

	// path frame curvature is the instantaneous curvature at our current distance
	// from the actual path (considering e.g. concentric circles emanating outward/inward)
	ध्रुव भिन्न path_frame_curvature = path_curvature / math::max(1.0f - path_curvature * signed_track_error,
					   fabsf(path_curvature) * MIN_RADIUS);

	// limit tangent ground speed to along track (forward moving) direction
	ध्रुव भिन्न tangent_ground_speed = math::max(ground_vel.dot(unit_path_tangent), 0.0f);

	ध्रुव भिन्न path_frame_rate = path_frame_curvature * tangent_ground_speed;

	// speed ratio = projection of ground vel on track / projection of air velocity
	// on track
	ध्रुव भिन्न speed_ratio = (1.0f + wind_dot_upt / math::max(projectAirspOnBearing(airspeed, wind_cross_upt),
				   NPFG_EPSILON));

	// note the use of airspeed * speed_ratio as oppose to ground_speed^2 here --
	// the prior considers that we command lateral acceleration in the air mass
	// relative frame while the latter does not
	प्रत्यागम airspeed * speed_ratio * path_frame_rate;
}

भिन्न DirectionalGuidance::projectAirspOnBearing(ध्रुव भिन्न airspeed, ध्रुव भिन्न wind_cross_bearing) ध्रुव
{
	// NOTE: wind_cross_bearing must be less than airspeed to use this function
	// it is assumed that bearing feasibility is checked and found feasible (e.g. bearingIsFeasible() = true) prior to entering this method
	// otherwise the return will be erroneous
	प्रत्यागम sqrtf(math::max(airspeed * airspeed - wind_cross_bearing * wind_cross_bearing, 0.0f));
}

भिन्न DirectionalGuidance::switchDistance(भिन्न wp_radius) ध्रुव
{
	प्रत्यागम math::min(wp_radius, track_error_bound_ * switch_distance_multiplier_);
}
