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
#અન્તર્ભાવ <matrix/math.hpp>
#અન્તર્ભાવ <lib/mathlib/mathlib.h>

સંરચના DirectionalGuidanceOutput {
	દશાંશ course_setpoint{NAN};
	દશાંશ lateral_acceleration_feedforward{NAN};
};

વર્ગ DirectionalGuidance
{
જાહેર:

	DirectionalGuidance();

	DirectionalGuidanceOutput guideToPath(અચલ matrix::Vector2f &curr_pos_local, અચલ matrix::Vector2f &ground_vel,
					      અચલ matrix::Vector2f &wind_vel,
					      અચલ matrix::Vector2f &unit_path_tangent, અચલ matrix::Vector2f &position_on_path,
					      અચલ દશાંશ path_curvature);
	/*
	 * Set the nominal controller period [s].
	 */
	શૂન્ય setPeriod(દશાંશ period) { period_ = math::max(period, NPFG_EPSILON); }

	/*
	 * Set the nominal controller damping ratio.
	 */
	શૂન્ય setDamping(દશાંશ damping) { damping_ = math::constrain(damping, NPFG_EPSILON, 1.0f); }

	/*
	 * Enable automatic lower bounding of the user set controller period.
	 */
	શૂન્ય enablePeriodLB(અચલ તાર્કિક en) { en_period_lb_ = en; }

	/*
	 * Enable automatic adaptation of the user set controller period for track keeping
	 * performance.
	 */
	શૂન્ય enablePeriodUB(અચલ તાર્કિક en) { en_period_ub_ = en; }


	/*
	 * Set the autopilot roll response time constant [s].
	 */
	શૂન્ય setRollTimeConst(દશાંશ tc) { roll_time_const_ = tc; }

	/*
	* Set the switch distance multiplier.
	*/
	શૂન્ય setSwitchDistanceMultiplier(દશાંશ mult) { switch_distance_multiplier_ = math::max(mult, 0.1f); }

	/*
	 * Set the period safety factor.
	 */
	શૂન્ય setPeriodSafetyFactor(દશાંશ sf) { period_safety_factor_ = math::max(sf, 1.0f); }
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
	દશાંશ switchDistance(દશાંશ wp_radius) અચલ;

	દશાંશ getCourseSetpoint() અચલ { પાછા course_sp_; }
	દશાંશ getLateralAccelerationSetpoint() અચલ { પાછા lateral_accel_ff_; }
	દશાંશ getBearingFeasibility() અચલ { પાછા feas_; }
	દશાંશ getBearingFeasibilityOnTrack() અચલ { પાછા feas_on_track_; }
	દશાંશ getSignedTrackError() અચલ { પાછા signed_track_error_; }
	દશાંશ getTrackErrorBound() અચલ { પાછા track_error_bound_; }
	દશાંશ getAdaptedPeriod() અચલ { પાછા adapted_period_; }

ખાનગી:
	સ્થિર constexpr દશાંશ NPFG_EPSILON = 1.0e-6; // small number *bigger than machine epsilon
	સ્થિર constexpr દશાંશ MIN_RADIUS = 0.5f; // minimum effective radius (avoid singularities) [m]
	સ્થિર constexpr દશાંશ AIRSPEED_BUFFER = 1.5f; // airspeed buffer [m/s] (must be > 0)

	દશાંશ period_{10.0f}; // nominal (desired) period -- user defined [s]
	દશાંશ damping_{0.7071f}; // nominal (desired) damping ratio -- user defined
	દશાંશ adapted_period_{10.0f}; // auto-adapted period (if stability bounds enabled) [s]

	/*
	 * user defined guidance settings
	 */

	// guidance options
	તાર્કિક en_period_lb_{સાચું}; // enables automatic lower bound constraints on controller period
	તાર્કિક en_period_ub_{સાચું}; // enables automatic upper bound constraints on controller period (remains disabled if lower bound is disabled)

	// guidance settings
	દશાંશ roll_time_const_{0.0f}; // autopilot roll response time constant [s]

	// guidance parameters
	દશાંશ switch_distance_multiplier_{0.32f}; // a value multiplied by the track error boundary resulting in a lower switch distance
	// ^as the bearing angle changes quadratically (instead of linearly as in L1), the time constant (automatically calculated for on track stability) proportional track error boundary typically over estimates the required switching distance
	દશાંશ period_safety_factor_{1.5f}; // multiplied by the minimum period for conservative lower bound

	/*
	* internal guidance states
	*/

	//bearing feasibility
	દશાંશ feas_{1.0f}; // continous representation of bearing feasibility in [0,1] (0=infeasible, 1=feasible)
	દશાંશ feas_on_track_{1.0f}; // continuous bearing feasibility "on track"

	// track proximity
	દશાંશ track_error_bound_{212.13f}; // the current ground speed dependent track error bound [m]
	દશાંશ track_proximity_{0.0f}; // value in [0,1] indicating proximity to track, 0 = at track error boundary or beyond, 1 = on track
	દશાંશ signed_track_error_{0.0f}; // signed track error [m]
	matrix::Vector2f bearing_vec_{matrix::Vector2f{1.0f, 0.0f}}; // bearing unit vector
	દશાંશ course_sp_{0.f}; // course setpoint [rad]
	દશાંશ lateral_accel_ff_{0.f}; // lateral acceleration feedforward [m/s^2]

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
	દશાંશ bearingFeasibility(દશાંશ wind_cross_bearing, અચલ દશાંશ wind_dot_bearing, અચલ દશાંશ airspeed,
				 અચલ દશાંશ wind_speed) અચલ;
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
	દશાંશ adaptPeriod(અચલ દશાંશ ground_speed, અચલ દશાંશ airspeed, અચલ દશાંશ wind_speed,
			  અચલ દશાંશ track_error, અચલ દશાંશ path_curvature, અચલ matrix::Vector2f &wind_vel,
			  અચલ matrix::Vector2f &unit_path_tangent, અચલ દશાંશ feas_on_track) અચલ;
	/*
	 * Returns normalized (unitless) and constrained track error [0,1].
	 *
	 * @param[in] track_error Track error (magnitude) [m]
	 * @param[in] track_error_bound Track error boundary [m]
	 * @return Normalized track error
	 */
	દશાંશ normalizedTrackError(અચલ દશાંશ track_error, અચલ દશાંશ track_error_bound) અચલ;

	/*
	 * Cacluates an approximation of the wind factor (see [TODO: include citation]).
	 *
	 * @param[in] airspeed Vehicle true airspeed [m/s]
	 * @param[in] wind_speed Wind speed [m/s]
	 * @return Non-dimensional wind factor approximation
	 */
	દશાંશ windFactor(અચલ દશાંશ airspeed, અચલ દશાંશ wind_speed) અચલ;

	/*
	 * Calculates a theoretical upper bound on the user defined period to maintain
	 * track keeping stability.
	 *
	 * @param[in] air_turn_rate The turn rate required to track the current path
	 *            curvature at the current true airspeed, in a no-wind condition [rad/s]
	 * @param[in] wind_factor Non-dimensional wind factor (see [TODO: include citation])
	 * @return Period upper bound [s]
	 */
	દશાંશ periodUpperBound(અચલ દશાંશ air_turn_rate, અચલ દશાંશ wind_factor, અચલ દશાંશ feas_on_track) અચલ;

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
	દશાંશ periodLowerBound(અચલ દશાંશ air_turn_rate, અચલ દશાંશ wind_factor, અચલ દશાંશ feas_on_track) અચલ;

	/*
	 * Computes a continous non-dimensional track proximity [0,1] - 0 when the
	 * vehicle is at the track error boundary, and 1 when on track.
	 *
	 * @param[in] look_ahead_ang The angle at which the bearing vector deviates
	 *            from the unit track error vector [rad]
	 * @return Track proximity
	 */
	દશાંશ trackProximity(અચલ દશાંશ look_ahead_ang) અચલ;

	/*
	 * Calculates a ground speed modulated track error bound under which the
	 * look ahead angle is quadratically transitioned from alignment with the
	 * track error vector to that of the path tangent vector.
	 *
	 * @param[in] ground_speed Vehicle ground speed [m/s]
	 * @param[in] time_const Controller time constant [s]
	 * @return Track error boundary [m]
	 */
	દશાંશ trackErrorBound(અચલ દશાંશ ground_speed, અચલ દશાંશ time_const) અચલ;

	/*
	 * Calculates the required controller time constant to achieve the desired
	 * system period and damping ratio. NOTE: actual period and damping will vary
	 * when following paths with curvature in wind.
	 *
	 * @param[in] period Desired system period [s]
	 * @param[in] damping Desired system damping ratio
	 * @return Time constant [s]
	 */
	દશાંશ timeConst(અચલ દશાંશ period, અચલ દશાંશ damping) અચલ;

	/*
	 * Cacluates the look ahead angle as a quadratic function of the normalized
	 * track error.
	 *
	 * @param[in] normalized_track_error Normalized track error (track error / track error boundary)
	 * @return Look ahead angle [rad]
	 */
	દશાંશ lookAheadAngle(અચલ દશાંશ normalized_track_error) અચલ;
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
	matrix::Vector2f bearingVec(અચલ matrix::Vector2f &unit_path_tangent, અચલ દશાંશ look_ahead_ang,
				    અચલ દશાંશ signed_track_error) અચલ;

	/*
	 * Projection of the air velocity vector onto the bearing line considering
	 * a connected wind triangle.
	 *
	 * @param[in] airspeed Vehicle true airspeed [m/s]
	 * @param[in] wind_cross_bearing 2D cross product of wind velocity and bearing vector [m/s]
	 * @return Projection of air velocity vector on bearing vector [m/s]
	 */
	દશાંશ projectAirspOnBearing(અચલ દશાંશ airspeed, અચલ દશાંશ wind_cross_bearing) અચલ;
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
	દશાંશ lateralAccelFF(અચલ matrix::Vector2f &unit_path_tangent, અચલ matrix::Vector2f &ground_vel,
			     અચલ દશાંશ wind_dot_upt, અચલ દશાંશ wind_cross_upt, અચલ દશાંશ airspeed,
			     અચલ દશાંશ wind_speed, અચલ દશાંશ signed_track_error, અચલ દશાંશ path_curvature) અચલ;

};

#endif //PX4_DIRECTIONALGUIDANCE_HPP
