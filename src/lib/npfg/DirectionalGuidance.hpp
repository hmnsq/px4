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
 * @file DirectionalGuidance.hpp
 *
 * Original Author:  Thomas Stastny <tstastny@ethz.ch>
 * Refactored to better suite new control API: Roman Bapst <roman@auterion.com>
 *
 * * Notes:
 * - The wind estimate should be dynamic enough to capture ~1-2 second length gusts,
 *   Otherwise the performance will suffer.
 *
 * Acknowledgements and References:
 *
 * The logic is mostly based on [1] and Paper III of [2].
 * TODO: Concise, up to date documentation and stability analysis for the following
 *       implementation.
 *
 * [1] T. Stastny and R. Siegwart. "On Flying Backwards: Preventing Run-away of
 *     Small, Low-speed, Fixed-wing UAVs in Strong Winds". IEEE International Conference
 *     on Intelligent Robots and Systems (IROS). 2019.
 *     https://arxiv.org/pdf/1908.01381.pdf
 * [2] T. Stastny. "Low-Altitude Control and Local Re-Planning Strategies for Small
 *     Fixed-Wing UAVs". Doctoral Thesis, ETH Zürich. 2020.
 *     https://tstastny.github.io/pdf/tstastny_phd_thesis_wcover.pdf
 */

#ifndef PX4_DIRECTIONALGUIDANCE_HPP
#define PX4_DIRECTIONALGUIDANCE_HPP
#அந்தர்பாவ <matrix/math.hpp>
#அந்தர்பாவ <lib/mathlib/mathlib.h>

கட்டமைப்பு DirectionalGuidanceOutput {
	தசம course_setpoint{NAN};
	தசம lateral_acceleration_feedforward{NAN};
};

வகுப்பு DirectionalGuidance
{
பொது:

	DirectionalGuidance();

	DirectionalGuidanceOutput guideToPath(மாறிலி matrix::Vector2f &curr_pos_local, மாறிலி matrix::Vector2f &ground_vel,
					      மாறிலி matrix::Vector2f &wind_vel,
					      மாறிலி matrix::Vector2f &unit_path_tangent, மாறிலி matrix::Vector2f &position_on_path,
					      மாறிலி தசம path_curvature);
	/*
	 * Set the nominal controller period [s].
	 */
	வெறுமை setPeriod(தசம period) { period_ = math::max(period, NPFG_EPSILON); }

	/*
	 * Set the nominal controller damping ratio.
	 */
	வெறுமை setDamping(தசம damping) { damping_ = math::constrain(damping, NPFG_EPSILON, 1.0f); }

	/*
	 * Enable automatic lower bounding of the user set controller period.
	 */
	வெறுமை enablePeriodLB(மாறிலி தருக்க en) { en_period_lb_ = en; }

	/*
	 * Enable automatic adaptation of the user set controller period for track keeping
	 * performance.
	 */
	வெறுமை enablePeriodUB(மாறிலி தருக்க en) { en_period_ub_ = en; }


	/*
	 * Set the autopilot roll response time constant [s].
	 */
	வெறுமை setRollTimeConst(தசம tc) { roll_time_const_ = tc; }

	/*
	* Set the switch distance multiplier.
	*/
	வெறுமை setSwitchDistanceMultiplier(தசம mult) { switch_distance_multiplier_ = math::max(mult, 0.1f); }

	/*
	 * Set the period safety factor.
	 */
	வெறுமை setPeriodSafetyFactor(தசம sf) { period_safety_factor_ = math::max(sf, 1.0f); }
	/*
	 * [Copied directly from ECL_L1_Pos_Controller]
	 *
	 * Get the switch distance
	 *
	 * This is the distance at which the system will switch to the next waypoint.
	 * This depends on the period and damping
	 *
	 * @param[in] wp_radius The switching radius the waypoint has set.
	 */
	தசம switchDistance(தசம wp_radius) மாறிலி;

	தசம getCourseSetpoint() மாறிலி { திரும்பு course_sp_; }
	தசம getLateralAccelerationSetpoint() மாறிலி { திரும்பு lateral_accel_ff_; }
	தசம getBearingFeasibility() மாறிலி { திரும்பு feas_; }
	தசம getBearingFeasibilityOnTrack() மாறிலி { திரும்பு feas_on_track_; }
	தசம getSignedTrackError() மாறிலி { திரும்பு signed_track_error_; }
	தசம getTrackErrorBound() மாறிலி { திரும்பு track_error_bound_; }
	தசம getAdaptedPeriod() மாறிலி { திரும்பு adapted_period_; }

தனிப்பட்ட:
	நிலையான constexpr தசம NPFG_EPSILON = 1.0e-6; // small number *bigger than machine epsilon
	நிலையான constexpr தசம MIN_RADIUS = 0.5f; // minimum effective radius (avoid singularities) [m]
	நிலையான constexpr தசம AIRSPEED_BUFFER = 1.5f; // airspeed buffer [m/s] (must be > 0)

	தசம period_{10.0f}; // nominal (desired) period -- user defined [s]
	தசம damping_{0.7071f}; // nominal (desired) damping ratio -- user defined
	தசம adapted_period_{10.0f}; // auto-adapted period (if stability bounds enabled) [s]

	/*
	 * user defined guidance settings
	 */

	// guidance options
	தருக்க en_period_lb_{உண்மை}; // enables automatic lower bound constraints on controller period
	தருக்க en_period_ub_{உண்மை}; // enables automatic upper bound constraints on controller period (remains disabled if lower bound is disabled)

	// guidance settings
	தசம roll_time_const_{0.0f}; // autopilot roll response time constant [s]

	// guidance parameters
	தசம switch_distance_multiplier_{0.32f}; // a value multiplied by the track error boundary resulting in a lower switch distance
	// ^as the bearing angle changes quadratically (instead of linearly as in L1), the time constant (automatically calculated for on track stability) proportional track error boundary typically over estimates the required switching distance
	தசம period_safety_factor_{1.5f}; // multiplied by the minimum period for conservative lower bound

	/*
	* internal guidance states
	*/

	//bearing feasibility
	தசம feas_{1.0f}; // continous representation of bearing feasibility in [0,1] (0=infeasible, 1=feasible)
	தசம feas_on_track_{1.0f}; // continuous bearing feasibility "on track"

	// track proximity
	தசம track_error_bound_{212.13f}; // the current ground speed dependent track error bound [m]
	தசம track_proximity_{0.0f}; // value in [0,1] indicating proximity to track, 0 = at track error boundary or beyond, 1 = on track
	தசம signed_track_error_{0.0f}; // signed track error [m]
	matrix::Vector2f bearing_vec_{matrix::Vector2f{1.0f, 0.0f}}; // bearing unit vector
	தசம course_sp_{0.f}; // course setpoint [rad]
	தசம lateral_accel_ff_{0.f}; // lateral acceleration feedforward [m/s^2]

	/*
	 * Cacluates a continuous representation of the bearing feasibility from [0,1].
	 * 0 = infeasible, 1 = fully feasible, partial feasibility in between.
	 *
	 * @param[in] wind_cross_bearing 2D cross product of wind velocity and bearing vector [m/s]
	 * @param[in] wind_dot_bearing 2D dot product of wind velocity and bearing vector [m/s]
	 * @param[in] airspeed Vehicle true airspeed [m/s]
	 * @param[in] wind_speed Wind speed [m/s]
	 * @return bearing feasibility
	 */
	தசம bearingFeasibility(தசம wind_cross_bearing, மாறிலி தசம wind_dot_bearing, மாறிலி தசம airspeed,
				 மாறிலி தசம wind_speed) மாறிலி;
	/*
	 * Adapts the controller period considering user defined inputs, current flight
	 * condition, path properties, and stability bounds.
	 *
	 * @param[in] ground_speed Vehicle ground speed [m/s]
	 * @param[in] airspeed Vehicle true airspeed [m/s]
	 * @param[in] wind_speed Wind speed [m/s]
	 * @param[in] track_error Track error (magnitude) [m]
	 * @param[in] path_curvature Path curvature at closest point on track [m^-1]
	 * @param[in] wind_vel Wind velocity vector in inertial frame [m/s]
	 * @param[in] unit_path_tangent Unit vector tangent to path at closest point
	 *            in direction of path
	 * @param[in] feas_on_track Bearing feasibility on track at the closest point
	 * @return Adapted period [s]
	 */
	தசம adaptPeriod(மாறிலி தசம ground_speed, மாறிலி தசம airspeed, மாறிலி தசம wind_speed,
			  மாறிலி தசம track_error, மாறிலி தசம path_curvature, மாறிலி matrix::Vector2f &wind_vel,
			  மாறிலி matrix::Vector2f &unit_path_tangent, மாறிலி தசம feas_on_track) மாறிலி;
	/*
	 * Returns normalized (unitless) and constrained track error [0,1].
	 *
	 * @param[in] track_error Track error (magnitude) [m]
	 * @param[in] track_error_bound Track error boundary [m]
	 * @return Normalized track error
	 */
	தசம normalizedTrackError(மாறிலி தசம track_error, மாறிலி தசம track_error_bound) மாறிலி;

	/*
	 * Cacluates an approximation of the wind factor (see [TODO: include citation]).
	 *
	 * @param[in] airspeed Vehicle true airspeed [m/s]
	 * @param[in] wind_speed Wind speed [m/s]
	 * @return Non-dimensional wind factor approximation
	 */
	தசம windFactor(மாறிலி தசம airspeed, மாறிலி தசம wind_speed) மாறிலி;

	/*
	 * Calculates a theoretical upper bound on the user defined period to maintain
	 * track keeping stability.
	 *
	 * @param[in] air_turn_rate The turn rate required to track the current path
	 *            curvature at the current true airspeed, in a no-wind condition [rad/s]
	 * @param[in] wind_factor Non-dimensional wind factor (see [TODO: include citation])
	 * @return Period upper bound [s]
	 */
	தசம periodUpperBound(மாறிலி தசம air_turn_rate, மாறிலி தசம wind_factor, மாறிலி தசம feas_on_track) மாறிலி;

	/*
	 * Calculates a theoretical lower bound on the user defined period to avoid
	 * limit cycle oscillations considering an acceleration actuation delay (e.g.
	 * roll response delay). Note this lower bound defines *marginal stability,
	 * and a safety factor should be applied in addition to the returned value.
	 *
	 * @param[in] air_turn_rate The turn rate required to track the current path
	 *            curvature at the current true airspeed, in a no-wind condition [rad/s]
	 * @param[in] wind_factor Non-dimensional wind factor (see [TODO: include citation])
	 * @return Period lower bound [s]
	 */
	தசம periodLowerBound(மாறிலி தசம air_turn_rate, மாறிலி தசம wind_factor, மாறிலி தசம feas_on_track) மாறிலி;

	/*
	 * Computes a continous non-dimensional track proximity [0,1] - 0 when the
	 * vehicle is at the track error boundary, and 1 when on track.
	 *
	 * @param[in] look_ahead_ang The angle at which the bearing vector deviates
	 *            from the unit track error vector [rad]
	 * @return Track proximity
	 */
	தசம trackProximity(மாறிலி தசம look_ahead_ang) மாறிலி;

	/*
	 * Calculates a ground speed modulated track error bound under which the
	 * look ahead angle is quadratically transitioned from alignment with the
	 * track error vector to that of the path tangent vector.
	 *
	 * @param[in] ground_speed Vehicle ground speed [m/s]
	 * @param[in] time_const Controller time constant [s]
	 * @return Track error boundary [m]
	 */
	தசம trackErrorBound(மாறிலி தசம ground_speed, மாறிலி தசம time_const) மாறிலி;

	/*
	 * Calculates the required controller time constant to achieve the desired
	 * system period and damping ratio. NOTE: actual period and damping will vary
	 * when following paths with curvature in wind.
	 *
	 * @param[in] period Desired system period [s]
	 * @param[in] damping Desired system damping ratio
	 * @return Time constant [s]
	 */
	தசம timeConst(மாறிலி தசம period, மாறிலி தசம damping) மாறிலி;

	/*
	 * Cacluates the look ahead angle as a quadratic function of the normalized
	 * track error.
	 *
	 * @param[in] normalized_track_error Normalized track error (track error / track error boundary)
	 * @return Look ahead angle [rad]
	 */
	தசம lookAheadAngle(மாறிலி தசம normalized_track_error) மாறிலி;
	/*
	 * Calculates the bearing vector and track proximity transitioning variable
	 * from the look-ahead angle mapping.
	 *
	 * @param[in] unit_path_tangent Unit vector tangent to path at closest point
	 *            in direction of path
	 * @param[in] look_ahead_ang The bearing vector lies at this angle from
	 *            the path normal vector [rad]
	 * @param[in] signed_track_error Signed error to track at closest point (sign
	 *            determined by path normal direction) [m]
	 * @return Unit bearing vector
	 */
	matrix::Vector2f bearingVec(மாறிலி matrix::Vector2f &unit_path_tangent, மாறிலி தசம look_ahead_ang,
				    மாறிலி தசம signed_track_error) மாறிலி;

	/*
	 * Projection of the air velocity vector onto the bearing line considering
	 * a connected wind triangle.
	 *
	 * @param[in] airspeed Vehicle true airspeed [m/s]
	 * @param[in] wind_cross_bearing 2D cross product of wind velocity and bearing vector [m/s]
	 * @return Projection of air velocity vector on bearing vector [m/s]
	 */
	தசம projectAirspOnBearing(மாறிலி தசம airspeed, மாறிலி தசம wind_cross_bearing) மாறிலி;
	/*
	 * Calculates an additional feed-forward lateral acceleration demand considering
	 * the path curvature.
	 *
	 * @param[in] unit_path_tangent Unit vector tangent to path at closest point
	 *            in direction of path
	 * @param[in] ground_vel Vehicle ground velocity vector [m/s]
	 * @param[in] wind_vel Wind velocity vector [m/s]
	 * @param[in] airspeed Vehicle true airspeed [m/s]
	 * @param[in] wind_speed Wind speed [m/s]
	 * @param[in] signed_track_error Signed error to track at closest point (sign
	 *             determined by path normal direction) [m]
	 * @param[in] path_curvature Path curvature at closest point on track [m^-1]
	 * @return Feed-forward lateral acceleration command [m/s^2]
	 */
	தசம lateralAccelFF(மாறிலி matrix::Vector2f &unit_path_tangent, மாறிலி matrix::Vector2f &ground_vel,
			     மாறிலி தசம wind_dot_upt, மாறிலி தசம wind_cross_upt, மாறிலி தசம airspeed,
			     மாறிலி தசம wind_speed, மாறிலி தசம signed_track_error, மாறிலி தசம path_curvature) மாறிலி;

};

#endif //PX4_DIRECTIONALGUIDANCE_HPP
