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
DirectionalGuidance::guideToPath(स्थिरांक Vector2f &curr_pos_local, स्थिरांक Vector2f &ground_vel, स्थिरांक Vector2f &wind_vel,
				 स्थिरांक Vector2f &unit_path_tangent, स्थिरांक Vector2f &position_on_path,
				 स्थिरांक दशांश path_curvature)
{
	स्थिरांक दशांश ground_speed = ground_vel.norm();

	स्थिरांक Vector2f air_vel = ground_vel - wind_vel;
	स्थिरांक दशांश airspeed = air_vel.norm();

	स्थिरांक दशांश wind_speed = wind_vel.norm();

	स्थिरांक Vector2f path_pos_to_vehicle{curr_pos_local - position_on_path};
	signed_track_error_ = unit_path_tangent.cross(path_pos_to_vehicle);

	// on-track wind triangle projections
	स्थिरांक दशांश wind_cross_upt = wind_vel.cross(unit_path_tangent);
	स्थिरांक दशांश wind_dot_upt = wind_vel.dot(unit_path_tangent);

	// calculate the bearing feasibility on the track at the current closest point
	feas_on_track_ = bearingFeasibility(wind_cross_upt, wind_dot_upt, airspeed, wind_speed);

	स्थिरांक दशांश track_error = fabsf(signed_track_error_);

	// update control parameters considering upper and lower stability bounds (if enabled)
	// must be called before trackErrorBound() as it updates time_const_
	adapted_period_ = adaptPeriod(ground_speed, airspeed, wind_speed, track_error,
				      path_curvature, wind_vel, unit_path_tangent, feas_on_track_);
	स्थिरांक दशांश time_const = timeConst(adapted_period_, damping_);

	// track error bound is dynamic depending on ground speed
	track_error_bound_ = trackErrorBound(ground_speed, time_const);
	स्थिरांक दशांश normalized_track_error = normalizedTrackError(track_error, track_error_bound_);

	// look ahead angle based solely on track proximity
	स्थिरांक दशांश look_ahead_ang = lookAheadAngle(normalized_track_error);

	track_proximity_ = trackProximity(look_ahead_ang);

	bearing_vec_ = bearingVec(unit_path_tangent, look_ahead_ang, signed_track_error_);

	// wind triangle projections
	स्थिरांक दशांश wind_cross_bearing = wind_vel.cross(bearing_vec_);
	स्थिरांक दशांश wind_dot_bearing = wind_vel.dot(bearing_vec_);

	// continuous representation of the bearing feasibility
	feas_ = bearingFeasibility(wind_cross_bearing, wind_dot_bearing, airspeed, wind_speed);

	// we consider feasibility of both the current bearing as well as that on the track at the current closest point
	स्थिरांक दशांश feas_combined = feas_ * feas_on_track_;
	// lateral acceleration needed to stay on curved track (assuming no heading error)
	lateral_accel_ff_ = lateralAccelFF(unit_path_tangent, ground_vel, wind_dot_upt,
					   wind_cross_upt, airspeed, wind_speed, signed_track_error_, path_curvature) * feas_combined * track_proximity_;
	course_sp_ = atan2f(bearing_vec_(1), bearing_vec_(0));

	परत DirectionalGuidanceOutput{.course_setpoint = course_sp_,
					 .lateral_acceleration_feedforward = lateral_accel_ff_};
}

दशांश DirectionalGuidance::adaptPeriod(स्थिरांक दशांश ground_speed, स्थिरांक दशांश airspeed, स्थिरांक दशांश wind_speed,
				       स्थिरांक दशांश track_error, स्थिरांक दशांश path_curvature, स्थिरांक Vector2f &wind_vel,
				       स्थिरांक Vector2f &unit_path_tangent, स्थिरांक दशांश feas_on_track) स्थिरांक
{
	दशांश period = period_;
	स्थिरांक दशांश air_turn_rate = fabsf(path_curvature * airspeed);
	स्थिरांक दशांश wind_factor = windFactor(airspeed, wind_speed);

	जर (en_period_lb_ && roll_time_const_ > NPFG_EPSILON) {
		// lower bound for period not considering path curvature
		स्थिरांक दशांश period_lb_zero_curvature = periodLowerBound(0.0f, wind_factor, feas_on_track) * period_safety_factor_;

		// lower bound for period *considering path curvature
		दशांश period_lb = periodLowerBound(air_turn_rate, wind_factor, feas_on_track) * period_safety_factor_;

		// calculate the time constant and track error bound considering the zero
		// curvature, lower-bounded period and subsequently recalculate the normalized
		// track error
		स्थिरांक दशांश time_const = timeConst(period_lb_zero_curvature, damping_);
		स्थिरांक दशांश track_error_bound = trackErrorBound(ground_speed, time_const);
		स्थिरांक दशांश normalized_track_error = normalizedTrackError(track_error, track_error_bound);

		// calculate nominal track proximity with lower bounded time constant
		// (only a numerical solution can find corresponding track proximity
		// and adapted gains simultaneously)
		स्थिरांक दशांश look_ahead_ang = lookAheadAngle(normalized_track_error);
		स्थिरांक दशांश track_proximity = trackProximity(look_ahead_ang);

		// linearly ramp in curvature dependent lower bound with track proximity
		period_lb = period_lb * track_proximity + (1.0f - track_proximity) * period_lb_zero_curvature;

		// lower bounded period
		period = math::max(period_lb, period);

		// only allow upper bounding ONLY if lower bounding is enabled (is otherwise
		// dangerous to allow period decrements without stability checks).
		// NOTE: if the roll time constant is not accurately known, lower-bound
		// checks may be too optimistic and reducing the period can still destabilize
		// the system! enable this feature at your own risk.
		जर (en_period_ub_) {

			स्थिरांक दशांश period_ub = periodUpperBound(air_turn_rate, wind_factor, feas_on_track);

			जर (en_period_ub_ && PX4_ISFINITE(period_ub) && period > period_ub) {
				// NOTE: if the roll time constant is not accurately known, reducing
				// the period here can destabilize the system!
				// enable this feature at your own risk!

				// upper bound the period (for track keeping stability), prefer lower bound if violated
				स्थिरांक दशांश period_adapted = math::max(period_lb, period_ub);

				// transition from the nominal period to the adapted period as we get
				// closer to the track
				period = period_adapted * track_proximity + (1.0f - track_proximity) * period;
			}
		}
	}

	परत period;
}

दशांश DirectionalGuidance::normalizedTrackError(स्थिरांक दशांश track_error, स्थिरांक दशांश track_error_bound) स्थिरांक
{
	परत math::constrain(track_error / track_error_bound, 0.0f, 1.0f);
}

दशांश DirectionalGuidance::windFactor(स्थिरांक दशांश airspeed, स्थिरांक दशांश wind_speed) स्थिरांक
{
	// See [TODO: include citation] for definition/elaboration of this approximation.
	जर (wind_speed > airspeed || airspeed < NPFG_EPSILON) {
		परत 2.0f;

	} अन्यथा {
		परत 2.0f * (1.0f - sqrtf(1.0f - math::min(1.0f, wind_speed / airspeed)));
	}
}

दशांश DirectionalGuidance::periodUpperBound(स्थिरांक दशांश air_turn_rate, स्थिरांक दशांश wind_factor,
		स्थिरांक दशांश feas_on_track) स्थिरांक
{
	जर (air_turn_rate * wind_factor > NPFG_EPSILON) {
		// multiply air turn rate by feasibility on track to zero out when we anyway
		// should not consider the curvature
		परत 4.0f * M_PI_F * damping_ / (air_turn_rate * wind_factor * feas_on_track);
	}

	परत अनन्त;
}

दशांश DirectionalGuidance::periodLowerBound(स्थिरांक दशांश air_turn_rate, स्थिरांक दशांश wind_factor,
		स्थिरांक दशांश feas_on_track) स्थिरांक
{
	// this method considers a "conservative" lower period bound, i.e. a constant
	// worst case bound for any wind ratio, airspeed, and path curvature

	// the lower bound for zero curvature and no wind OR damping ratio < 0.5
	स्थिरांक दशांश period_lb = M_PI_F * roll_time_const_ / damping_;

	जर (air_turn_rate * wind_factor < NPFG_EPSILON || damping_ < 0.5f) {
		परत period_lb;

	} अन्यथा {
		// the lower bound for tracking a curved path in wind with damping ratio > 0.5
		स्थिरांक दशांश period_windy_curved_damped = 4.0f * M_PI_F * roll_time_const_ * damping_;

		// blend the two together as the bearing on track becomes less feasible
		परत period_windy_curved_damped * feas_on_track + (1.0f - feas_on_track) * period_lb;
	}
}

दशांश DirectionalGuidance::trackProximity(स्थिरांक दशांश look_ahead_ang) स्थिरांक
{
	स्थिरांक दशांश sin_look_ahead_ang = sinf(look_ahead_ang);
	परत sin_look_ahead_ang * sin_look_ahead_ang;
}

दशांश DirectionalGuidance::trackErrorBound(स्थिरांक दशांश ground_speed, स्थिरांक दशांश time_const) स्थिरांक
{
	जर (ground_speed > 1.0f) {
		परत ground_speed * time_const;

	} अन्यथा {
		// limit bound to some minimum ground speed to avoid singularities in track
		// error normalization. the following equation assumes ground speed minimum = 1.0
		परत 0.5f * time_const * (ground_speed * ground_speed + 1.0f);
	}
}

दशांश DirectionalGuidance::timeConst(स्थिरांक दशांश period, स्थिरांक दशांश damping) स्थिरांक
{
	परत period * damping;
}

दशांश DirectionalGuidance::lookAheadAngle(स्थिरांक दशांश normalized_track_error) स्थिरांक
{
	परत M_PI_2_F * (normalized_track_error - 1.0f) * (normalized_track_error - 1.0f);
}


matrix::Vector2f DirectionalGuidance::bearingVec(स्थिरांक Vector2f &unit_path_tangent, स्थिरांक दशांश look_ahead_ang,
		स्थिरांक दशांश signed_track_error) स्थिरांक
{
	स्थिरांक दशांश cos_look_ahead_ang = cosf(look_ahead_ang);
	स्थिरांक दशांश sin_look_ahead_ang = sinf(look_ahead_ang);

	Vector2f unit_path_normal(-unit_path_tangent(1), unit_path_tangent(0)); // right handed 90 deg (clockwise) turn
	Vector2f unit_track_error = -((signed_track_error < 0.0f) ? -1.0f : 1.0f) * unit_path_normal;

	परत cos_look_ahead_ang * unit_track_error + sin_look_ahead_ang * unit_path_tangent;
}

दशांश
DirectionalGuidance::bearingFeasibility(दशांश wind_cross_bearing, स्थिरांक दशांश wind_dot_bearing, स्थिरांक दशांश airspeed,
					स्थिरांक दशांश wind_speed) स्थिरांक
{
	जर (wind_dot_bearing < 0.0f) {
		wind_cross_bearing = wind_speed;

	} अन्यथा {
		wind_cross_bearing = fabsf(wind_cross_bearing);
	}

	दशांश sin_arg = sinf(M_PI_F * 0.5f * math::constrain((airspeed - wind_cross_bearing) / AIRSPEED_BUFFER, 0.0f, 1.0f));
	परत sin_arg * sin_arg;
}

दशांश DirectionalGuidance::lateralAccelFF(स्थिरांक Vector2f &unit_path_tangent, स्थिरांक Vector2f &ground_vel,
		स्थिरांक दशांश wind_dot_upt, स्थिरांक दशांश wind_cross_upt, स्थिरांक दशांश airspeed,
		स्थिरांक दशांश wind_speed, स्थिरांक दशांश signed_track_error,
		स्थिरांक दशांश path_curvature) स्थिरांक
{
	// NOTE: all calculations within this function take place at the closet point
	// on the path, as if the aircraft were already tracking the given path at
	// this point with zero angular error. this allows us to evaluate curvature
	// induced requirements for lateral acceleration incrementation and ramp them
	// in with the track proximity and further consider the bearing feasibility
	// in excess wind conditions (this is done external to this method).

	// path frame curvature is the instantaneous curvature at our current distance
	// from the actual path (considering e.g. concentric circles emanating outward/inward)
	स्थिरांक दशांश path_frame_curvature = path_curvature / math::max(1.0f - path_curvature * signed_track_error,
					   fabsf(path_curvature) * MIN_RADIUS);

	// limit tangent ground speed to along track (forward moving) direction
	स्थिरांक दशांश tangent_ground_speed = math::max(ground_vel.dot(unit_path_tangent), 0.0f);

	स्थिरांक दशांश path_frame_rate = path_frame_curvature * tangent_ground_speed;

	// speed ratio = projection of ground vel on track / projection of air velocity
	// on track
	स्थिरांक दशांश speed_ratio = (1.0f + wind_dot_upt / math::max(projectAirspOnBearing(airspeed, wind_cross_upt),
				   NPFG_EPSILON));

	// note the use of airspeed * speed_ratio as oppose to ground_speed^2 here --
	// the prior considers that we command lateral acceleration in the air mass
	// relative frame while the latter does not
	परत airspeed * speed_ratio * path_frame_rate;
}

दशांश DirectionalGuidance::projectAirspOnBearing(स्थिरांक दशांश airspeed, स्थिरांक दशांश wind_cross_bearing) स्थिरांक
{
	// NOTE: wind_cross_bearing must be less than airspeed to use this function
	// it is assumed that bearing feasibility is checked and found feasible (e.g. bearingIsFeasible() = true) prior to entering this method
	// otherwise the return will be erroneous
	परत sqrtf(math::max(airspeed * airspeed - wind_cross_bearing * wind_cross_bearing, 0.0f));
}

दशांश DirectionalGuidance::switchDistance(दशांश wp_radius) स्थिरांक
{
	परत math::min(wp_radius, track_error_bound_ * switch_distance_multiplier_);
}
