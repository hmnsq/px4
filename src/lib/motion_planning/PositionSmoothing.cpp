/****************************************************************************
 *
 *   Copyright (c) 2021 PX4 Development Team. All rights reserved.
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

#ਅਨ੍ਤਰ੍ਭਾਵ "PositionSmoothing.hpp"
#ਅਨ੍ਤਰ੍ਭਾਵ "TrajectoryConstraints.hpp"
#ਅਨ੍ਤਰ੍ਭਾਵ <mathlib/mathlib.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <matrix/matrix/math.hpp>

ਖਾਲੀ PositionSmoothing::_generateSetpoints(
	ਅਚਲ Vector3f &position,
	ਅਚਲ Vector3f(&waypoints)[3],
	ਤਾਰਕਿਕ is_single_waypoint,
	ਅਚਲ Vector3f &feedforward_velocity,
	ਦਸ਼ਮਲਵ delta_ਸਮਾਂ,
	ਤਾਰਕਿਕ force_zero_velocity_setpoint,
	PositionSmoothingSetpoints &ਬਾਹਰਲਾ_setpoints)
{
	Vector3f velocity_setpoint{0.f, 0.f, 0.f};

	ਜੇ (!force_zero_velocity_setpoint) {
		velocity_setpoint = _generateVelocitySetpoint(position, waypoints, is_single_waypoint, feedforward_velocity);
	}

	ਬਾਹਰਲਾ_setpoints.unsmoothed_velocity = velocity_setpoint;

	_generateTrajectory(
		position,
		velocity_setpoint,
		delta_ਸਮਾਂ,
		ਬਾਹਰਲਾ_setpoints
	);
}


ਤਾਰਕਿਕ PositionSmoothing::_isTurning(ਅਚਲ Vector3f &ਨਿਸ਼ਾਨਾ) ਅਚਲ
{
	ਅਚਲ Vector2f vel_traj(_trajectory[0].getCurrentVelocity(),
				_trajectory[1].getCurrentVelocity());
	ਅਚਲ Vector2f pos_traj(_trajectory[0].getCurrentPosition(),
				_trajectory[1].getCurrentPosition());
	ਅਚਲ Vector2f ਨਿਸ਼ਾਨਾ_xy(ਨਿਸ਼ਾਨਾ);
	ਅਚਲ Vector2f u_vel_traj = vel_traj.unit_or_zero();
	ਅਚਲ Vector2f pos_ਮੰਜ਼ਿਲ_ਨਿਸ਼ਾਨਾ = Vector2f(ਨਿਸ਼ਾਨਾ_xy - pos_traj);
	ਅਚਲ ਦਸ਼ਮਲਵ cos_align = u_vel_traj * pos_ਮੰਜ਼ਿਲ_ਨਿਸ਼ਾਨਾ.unit_or_zero();

	// The vehicle is turning if the angle between the velocity vector
	// and the direction to the target is greater than 10 degrees, the
	// velocity is large enough and the drone isn't in the acceptance
	// radius of the last WP.
	ਵਾਪਸ (vel_traj.longerThan(0.2f)
		&& cos_align < 0.98f
		&& pos_ਮੰਜ਼ਿਲ_ਨਿਸ਼ਾਨਾ.longerThan(ਨਿਸ਼ਾਨਾ_acceptance_radius));
}

ਦਸ਼ਮਲਵ PositionSmoothing::_getMaxXYSpeed(ਅਚਲ Vector3f(&waypoints)[3]) ਅਚਲ
{
	Vector3f pos_traj(_trajectory[0].getCurrentPosition(),
			  _trajectory[1].getCurrentPosition(),
			  _trajectory[2].getCurrentPosition());

	math::trajectory::VehicleDynamicLimits config;
	config.z_accept_rad = _vertical_acceptance_radius;
	config.xy_accept_rad = ਨਿਸ਼ਾਨਾ_acceptance_radius;
	config.max_acc_xy = _trajectory[0].getMaxAccel();
	config.max_jerk = _trajectory[0].getMaxJerk();
	config.max_ਰਫਤਾਰ_xy = cruise_ਰਫਤਾਰ;
	config.max_acc_xy_radius_scale = _horizontal_trajectory_gain;

	// constrain velocity to go to the position setpoint first if the position setpoint has been modified by an external source
	// (eg. Obstacle Avoidance)

	Vector3f pos_ਮੰਜ਼ਿਲ_waypoints[3] = {pos_traj, waypoints[1], waypoints[2]};

	ਵਾਪਸ math::trajectory::computeXYSpeedFromWaypoints<3>(pos_ਮੰਜ਼ਿਲ_waypoints, config);
}

ਦਸ਼ਮਲਵ PositionSmoothing::_getMaxZSpeed(ਅਚਲ Vector3f(&waypoints)[3]) ਅਚਲ
{
	ਅਚਲ Vector3f &ਸ਼ੁਰੂ_position = {_trajectory[0].getCurrentPosition(),
					  _trajectory[1].getCurrentPosition(),
					  _trajectory[2].getCurrentPosition()
					 };
	ਅਚਲ Vector3f &ਨਿਸ਼ਾਨਾ = waypoints[1];
	ਅਚਲ Vector3f &ਅਗਲਾ_ਨਿਸ਼ਾਨਾ = waypoints[2];

	ਅਚਲ Vector2f ਸ਼ੁਰੂ_position_xy_z = {ਸ਼ੁਰੂ_position.xy().norm(), ਸ਼ੁਰੂ_position(2)};
	ਅਚਲ Vector2f ਨਿਸ਼ਾਨਾ_xy_z = {ਨਿਸ਼ਾਨਾ.xy().norm(), ਨਿਸ਼ਾਨਾ(2)};
	ਅਚਲ Vector2f ਅਗਲਾ_ਨਿਸ਼ਾਨਾ_xy_z = {ਅਗਲਾ_ਨਿਸ਼ਾਨਾ.xy().norm(), ਅਗਲਾ_ਨਿਸ਼ਾਨਾ(2)};

	ਦਸ਼ਮਲਵ ਆਮਦ_z_ਰਫਤਾਰ = 0.0f;
	ਅਚਲ ਤਾਰਕਿਕ ਨਿਸ਼ਾਨਾ_ਅਗਲਾ_different = fabsf(ਨਿਸ਼ਾਨਾ(2) - ਅਗਲਾ_ਨਿਸ਼ਾਨਾ(2)) > 0.001f;

	ਜੇ (ਨਿਸ਼ਾਨਾ_ਅਗਲਾ_different) {
		// rounding can push the dot product of two unit vectors past 1, which acosf turns into NaN
		ਅਚਲ ਦਸ਼ਮਲਵ cos_alpha = math::constrain(Vector2f((ਨਿਸ਼ਾਨਾ_xy_z - ਸ਼ੁਰੂ_position_xy_z)).unit_or_zero().dot(
				Vector2f((ਨਿਸ਼ਾਨਾ_xy_z - ਅਗਲਾ_ਨਿਸ਼ਾਨਾ_xy_z)).unit_or_zero()), -1.f, 1.f);
		ਅਚਲ ਦਸ਼ਮਲਵ alpha = acosf(cos_alpha);

		ਅਚਲ ਦਸ਼ਮਲਵ ਸੁਰੱਖਿਅਤ_alpha = math::constrain(alpha, 0.f, M_PI_F - FLT_EPSILON);
		ਦਸ਼ਮਲਵ accel_tmp = _trajectory[2].getMaxAccel();
		ਦਸ਼ਮਲਵ max_ਰਫਤਾਰ_ਵਿੱਚ_turn = math::trajectory::compute_Max_ਰਫਤਾਰ_ਵਿੱਚ_Waypoint(ਸੁਰੱਖਿਅਤ_alpha, accel_tmp,
					  _vertical_acceptance_radius);
		ਆਮਦ_z_ਰਫਤਾਰ = math::min(max_ਰਫਤਾਰ_ਵਿੱਚ_turn, _trajectory[2].getMaxVel());
	}

	ਅਚਲ ਦਸ਼ਮਲਵ ਦੂਰੀ_ਸ਼ੁਰੂ_ਨਿਸ਼ਾਨਾ = fabs(ਨਿਸ਼ਾਨਾ(2) - ਸ਼ੁਰੂ_position(2));
	ਦਸ਼ਮਲਵ max_ਰਫਤਾਰ = math::min(_trajectory[2].getMaxVel(), math::trajectory::compute_Max_ਰਫਤਾਰ_From_ਦੂਰੀ(
					    _trajectory[2].getMaxJerk(), _trajectory[2].getMaxAccel(),
					    ਦੂਰੀ_ਸ਼ੁਰੂ_ਨਿਸ਼ਾਨਾ, ਆਮਦ_z_ਰਫਤਾਰ));

	ਵਾਪਸ max_ਰਫਤਾਰ;
}

ਅਚਲ Vector3f PositionSmoothing::get_ਪਾਰ_ਬਿੰਦੂ(ਅਚਲ Vector3f &position, ਅਚਲ Vector3f(&waypoints)[3]) ਅਚਲ
{
	ਅਚਲ ਸਵੈਚਾਲਿਤ &ਨਿਸ਼ਾਨਾ = waypoints[1];

	ਜੇ (!_isTurning(ਨਿਸ਼ਾਨਾ)) {
		ਵਾਪਸ ਨਿਸ਼ਾਨਾ;
	}

	// Get the crossing point using L1-style guidance
	ਵਾਪਸ get_L1_ਬਿੰਦੂ(position, waypoints);
}

ਅਚਲ Vector3f PositionSmoothing::get_L1_ਬਿੰਦੂ(ਅਚਲ Vector3f &position, ਅਚਲ Vector3f(&waypoints)[3]) ਅਚਲ
{
	ਅਚਲ Vector3f pos_traj(_trajectory[0].getCurrentPosition(), _trajectory[1].getCurrentPosition(),
				_trajectory[2].getCurrentPosition());
	ਅਚਲ Vector3f prev_ਮੰਜ਼ਿਲ_ਨਿਸ਼ਾਨਾ = waypoints[1] - waypoints[0];
	ਅਚਲ ਦਸ਼ਮਲਵ leg_ਲੰਮਾਈ = prev_ਮੰਜ਼ਿਲ_ਨਿਸ਼ਾਨਾ.ਲੰਮਾਈ();
	ਅਚਲ Vector3f u_prev_ਮੰਜ਼ਿਲ_ਨਿਸ਼ਾਨਾ = (leg_ਲੰਮਾਈ > FLT_EPSILON) ? prev_ਮੰਜ਼ਿਲ_ਨਿਸ਼ਾਨਾ / leg_ਲੰਮਾਈ : Vector3f();
	ਅਚਲ Vector3f prev_ਮੰਜ਼ਿਲ_pos(pos_traj - waypoints[0]);
	ਅਚਲ ਦਸ਼ਮਲਵ along_track = prev_ਮੰਜ਼ਿਲ_pos * u_prev_ਮੰਜ਼ਿਲ_ਨਿਸ਼ਾਨਾ;

	// If the trajectory has already passed the target, aim straight at it instead of
	// projecting the look-ahead point further along the (extended) leg. Otherwise an
	// overshoot of a not-yet-accepted waypoint marches the setpoint away from it forever.
	ਜੇ (along_track >= leg_ਲੰਮਾਈ) {
		ਵਾਪਸ waypoints[1];
	}

	ਅਚਲ Vector3f closest_pt = waypoints[0] + u_prev_ਮੰਜ਼ਿਲ_ਨਿਸ਼ਾਨਾ * along_track;

	// Compute along-track error using L1 distance and cross-track error
	ਅਚਲ ਦਸ਼ਮਲਵ crosstrack_error = (closest_pt - pos_traj).ਲੰਮਾਈ();

	ਅਚਲ ਦਸ਼ਮਲਵ l1 = math::max(ਨਿਸ਼ਾਨਾ_acceptance_radius, 5.f);
	ਦਸ਼ਮਲਵ alongtrack_error = 0.f;

	// Protect against sqrt of a negative number
	ਜੇ (l1 > crosstrack_error) {
		alongtrack_error = sqrtf(l1 * l1 - crosstrack_error * crosstrack_error);
	}

	// Position of the point on the line where L1 intersect the line between the two waypoints
	ਵਾਪਸ closest_pt + alongtrack_error * u_prev_ਮੰਜ਼ਿਲ_ਨਿਸ਼ਾਨਾ;
}

ਅਚਲ Vector3f PositionSmoothing::_generateVelocitySetpoint(ਅਚਲ Vector3f &position, ਅਚਲ Vector3f(&waypoints)[3],
		ਤਾਰਕਿਕ is_single_waypoint,
		ਅਚਲ Vector3f &feedforward_velocity_setpoint)
{
	// Interface: A valid position setpoint generates a velocity target using conservative motion constraints.
	// If a velocity is specified, that is used as a feedforward to track the position setpoint
	// (ie. it assumes the position setpoint is moving at the specified velocity)
	// If the position setpoints are set to NAN, the values in the velocity setpoints are used as velocity targets: nothing to do here.
	ਅਚਲ Vector3f &ਨਿਸ਼ਾਨਾ = waypoints[1];
	ਅਚਲ ਤਾਰਕਿਕ xy_ਨਿਸ਼ਾਨਾ_valid = Vector2f(ਨਿਸ਼ਾਨਾ).isAllFinite();
	ਅਚਲ ਤਾਰਕਿਕ z_ਨਿਸ਼ਾਨਾ_valid = PX4_ISFINITE(ਨਿਸ਼ਾਨਾ(2));

	Vector3f velocity_setpoint = feedforward_velocity_setpoint;

	ਜੇ (xy_ਨਿਸ਼ਾਨਾ_valid && z_ਨਿਸ਼ਾਨਾ_valid) {
		// Use 3D position setpoint to generate a 3D velocity setpoint
		Vector3f pos_traj(_trajectory[0].getCurrentPosition(),
				  _trajectory[1].getCurrentPosition(),
				  _trajectory[2].getCurrentPosition());
		ਅਚਲ Vector3f ਪਾਰ_ਬਿੰਦੂ = is_single_waypoint ? ਨਿਸ਼ਾਨਾ : get_ਪਾਰ_ਬਿੰਦੂ(position, waypoints);
		ਅਚਲ Vector3f u_pos_traj_ਮੰਜ਼ਿਲ_dest{(ਪਾਰ_ਬਿੰਦੂ - pos_traj).unit_or_zero()};

		ਦਸ਼ਮਲਵ xy_ਰਫਤਾਰ = _getMaxXYSpeed(waypoints);
		ਅਚਲ ਦਸ਼ਮਲਵ z_ਰਫਤਾਰ = _getMaxZSpeed(waypoints);

		ਜੇ (!is_single_waypoint && _isTurning(ਨਿਸ਼ਾਨਾ)) {
			// Limit speed during a turn
			xy_ਰਫਤਾਰ = math::min(max_ਰਫਤਾਰ_ਪਿਛਲਾ, xy_ਰਫਤਾਰ);

		} ਨਹੀਂਤਾਂ {
			max_ਰਫਤਾਰ_ਪਿਛਲਾ = xy_ਰਫਤਾਰ;
		}

		// Scale one 3D direction to fit both limits, so the slower axis limits the other
		// one and a leg that changes altitude is flown as a straight line
		Vector3f vel_sp_constrained = u_pos_traj_ਮੰਜ਼ਿਲ_dest * sqrtf(xy_ਰਫਤਾਰ * xy_ਰਫਤਾਰ + z_ਰਫਤਾਰ * z_ਰਫਤਾਰ);
		math::trajectory::clamp_ਮੰਜ਼ਿਲ_XYNorm(vel_sp_constrained, xy_ਰਫਤਾਰ, 0.5f);
		math::trajectory::clamp_ਮੰਜ਼ਿਲ_ZNorm(vel_sp_constrained, z_ਰਫਤਾਰ, 0.5f);

		ਲਈ (ਪੂਰਨ i = 0; i < 3; i++) {
			// If available, use the existing velocity as a feedforward, otherwise replace it
			ਜੇ (PX4_ISFINITE(velocity_setpoint(i))) {
				velocity_setpoint(i) += vel_sp_constrained(i);

			} ਨਹੀਂਤਾਂ {
				velocity_setpoint(i) = vel_sp_constrained(i);
			}
		}
	}

	ਨਹੀਂਤਾਂ ਜੇ (xy_ਨਿਸ਼ਾਨਾ_valid) {
		// Use 2D position setpoint to generate a 2D velocity setpoint

		// Get various path specific vectors
		Vector2f pos_traj(_trajectory[0].getCurrentPosition(), _trajectory[1].getCurrentPosition());
		Vector2f ਪਾਰ_ਬਿੰਦੂ = is_single_waypoint ? Vector2f(ਨਿਸ਼ਾਨਾ) : Vector2f(get_ਪਾਰ_ਬਿੰਦੂ(position, waypoints));
		Vector2f pos_traj_ਮੰਜ਼ਿਲ_dest_xy = ਪਾਰ_ਬਿੰਦੂ - pos_traj;
		Vector2f u_pos_traj_ਮੰਜ਼ਿਲ_dest_xy(pos_traj_ਮੰਜ਼ਿਲ_dest_xy.unit_or_zero());

		ਦਸ਼ਮਲਵ xy_ਰਫਤਾਰ = _getMaxXYSpeed(waypoints);

		ਜੇ (_isTurning(ਨਿਸ਼ਾਨਾ)) {
			// Lock speed during turn
			xy_ਰਫਤਾਰ = math::min(max_ਰਫਤਾਰ_ਪਿਛਲਾ, xy_ਰਫਤਾਰ);

		} ਨਹੀਂਤਾਂ {
			max_ਰਫਤਾਰ_ਪਿਛਲਾ = xy_ਰਫਤਾਰ;
		}

		Vector2f vel_sp_constrained_xy = u_pos_traj_ਮੰਜ਼ਿਲ_dest_xy * xy_ਰਫਤਾਰ;

		ਲਈ (ਪੂਰਨ i = 0; i < 2; i++) {
			// If available, use the existing velocity as a feedforward, otherwise replace it
			ਜੇ (PX4_ISFINITE(velocity_setpoint(i))) {
				velocity_setpoint(i) += vel_sp_constrained_xy(i);

			} ਨਹੀਂਤਾਂ {
				velocity_setpoint(i) = vel_sp_constrained_xy(i);
			}
		}
	}

	ਨਹੀਂਤਾਂ ਜੇ (z_ਨਿਸ਼ਾਨਾ_valid) {
		// Use Z position setpoint to generate a Z velocity setpoint

		ਅਚਲ ਦਸ਼ਮਲਵ z_dir = ਮੈਟ੍ਰਿਕਸ::sign(ਨਿਸ਼ਾਨਾ(2) - _trajectory[2].getCurrentPosition());
		ਅਚਲ ਦਸ਼ਮਲਵ vel_sp_z = z_dir * _getMaxZSpeed(waypoints);

		// If available, use the existing velocity as a feedforward, otherwise replace it
		ਜੇ (PX4_ISFINITE(velocity_setpoint(2))) {
			velocity_setpoint(2) += vel_sp_z;

		} ਨਹੀਂਤਾਂ {
			velocity_setpoint(2) = vel_sp_z;
		}
	}

	ਵਾਪਸ velocity_setpoint;
}


ਖਾਲੀ PositionSmoothing::_generateTrajectory(
	ਅਚਲ Vector3f &position,
	ਅਚਲ Vector3f &velocity_setpoint,
	ਦਸ਼ਮਲਵ delta_ਸਮਾਂ,
	PositionSmoothingSetpoints &ਬਾਹਰਲਾ_setpoints)
{
	ਜੇ (!velocity_setpoint.isAllFinite()) {
		ਵਾਪਸ;
	}

	/* Slow down the trajectory by decreasing the integration time based on the position error.
	 * This is only performed when the drone is behind the trajectory.
	 *
	 * Horizontal and vertical lag are evaluated independently and stretched per-axis. We
	 * deliberately do NOT fold Z lag into the XY stretch (and vice versa): a noisy altitude
	 * reference shouldn't cap forward flight speed during a straight mission leg, and stalled
	 * horizontal progress shouldn't freeze the climb profile. The Z check exists to keep the
	 * commanded altitude from walking away from the drone when the alt setpoint is jittering.
	 */
	Vector2f position_trajectory_xy(_trajectory[0].getCurrentPosition(), _trajectory[1].getCurrentPosition());
	Vector2f position_xy(position);
	Vector2f vel_traj_xy(_trajectory[0].getCurrentVelocity(), _trajectory[1].getCurrentVelocity());
	Vector2f drone_ਮੰਜ਼ਿਲ_trajectory_xy(position_trajectory_xy - position_xy);
	ਦਸ਼ਮਲਵ position_error_xy = drone_ਮੰਜ਼ਿਲ_trajectory_xy.ਲੰਮਾਈ();

	ਦਸ਼ਮਲਵ ਸਮਾਂ_stretch_xy = 1.f;

	// Only stretch time if there's no division by zero and the drone isn't ahead of the position setpoint
	ਜੇ ((_max_allowed_horizontal_error > FLT_EPSILON)
	    && drone_ਮੰਜ਼ਿਲ_trajectory_xy.dot(vel_traj_xy) >= 0) {
		ਸਮਾਂ_stretch_xy = 1.f - math::constrain(position_error_xy / _max_allowed_horizontal_error, 0.f, 1.f);
	}

	ਜੇ (drone_ਮੰਜ਼ਿਲ_trajectory_xy.dot(vel_traj_xy) < 0.f) {
		ਸਮਾਂ_stretch_xy = 1.f;
	}

	// Same idea for the vertical axis: only stretch when the virtual trajectory is leading the
	// drone AND moving further away from it. Reversing trajectories (virtual coming back toward
	// the drone) are left alone so we can recover without artificially slowing down.
	ਦਸ਼ਮਲਵ ਸਮਾਂ_stretch_z = 1.f;
	ਅਚਲ ਦਸ਼ਮਲਵ drone_ਮੰਜ਼ਿਲ_traj_z = _trajectory[2].getCurrentPosition() - position(2);
	ਅਚਲ ਦਸ਼ਮਲਵ vel_traj_z = _trajectory[2].getCurrentVelocity();

	ਜੇ ((_max_allowed_vertical_error > FLT_EPSILON)
	    && drone_ਮੰਜ਼ਿਲ_traj_z * vel_traj_z >= 0.f) {
		ਸਮਾਂ_stretch_z = 1.f - math::constrain(fabsf(drone_ਮੰਜ਼ਿਲ_traj_z) / _max_allowed_vertical_error, 0.f, 1.f);
	}

	// Axis 0,1 (XY) get the horizontal stretch; axis 2 (Z) gets the vertical stretch.
	ਅਚਲ ਦਸ਼ਮਲਵ axis_stretch[3] = { ਸਮਾਂ_stretch_xy, ਸਮਾਂ_stretch_xy, ਸਮਾਂ_stretch_z };

	ਲਈ (ਪੂਰਨ i = 0; i < 3; ++i) {
		_trajectory[i].updateTraj(delta_ਸਮਾਂ, axis_stretch[i]);
		ਬਾਹਰਲਾ_setpoints.jerk(i) = _trajectory[i].getCurrentJerk();
		ਬਾਹਰਲਾ_setpoints.acceleration(i) = _trajectory[i].getCurrentAcceleration();
		ਬਾਹਰਲਾ_setpoints.velocity(i) = _trajectory[i].getCurrentVelocity();
		ਬਾਹਰਲਾ_setpoints.position(i) = _trajectory[i].getCurrentPosition();
	}

	ਲਈ (ਪੂਰਨ i = 0; i < 3; ++i) {
		_trajectory[i].updateDurations(velocity_setpoint(i));
	}

	VelocitySmoothing::ਸਮਾਂ_Synchronization(_trajectory, 3);
}
