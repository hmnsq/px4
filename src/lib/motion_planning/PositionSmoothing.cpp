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

#ضمن "PositionSmoothing.hpp"
#ضمن "TrajectoryConstraints.hpp"
#ضمن <mathlib/mathlib.h>
#ضمن <matrix/matrix/math.hpp>

فراغ PositionSmoothing::_generateSetpoints(
	ثابت Vector3f &position,
	ثابت Vector3f(&waypoints)[3],
	منطقي is_single_waypoint,
	ثابت Vector3f &feedforward_velocity,
	عائم delta_time,
	منطقي force_zero_velocity_setpoint,
	PositionSmoothingSetpoints &out_setpoints)
{
	Vector3f velocity_setpoint{0.f, 0.f, 0.f};

	𐡄𐡍 (!force_zero_velocity_setpoint) {
		velocity_setpoint = _generateVelocitySetpoint(position, waypoints, is_single_waypoint, feedforward_velocity);
	}

	out_setpoints.unsmoothed_velocity = velocity_setpoint;

	_generateTrajectory(
		position,
		velocity_setpoint,
		delta_time,
		out_setpoints
	);
}


منطقي PositionSmoothing::_isTurning(ثابت Vector3f &target) ثابت
{
	ثابت Vector2f vel_traj(_trajectory[0].getCurrentVelocity(),
				_trajectory[1].getCurrentVelocity());
	ثابت Vector2f pos_traj(_trajectory[0].getCurrentPosition(),
				_trajectory[1].getCurrentPosition());
	ثابت Vector2f target_xy(target);
	ثابت Vector2f u_vel_traj = vel_traj.unit_or_zero();
	ثابت Vector2f pos_to_target = Vector2f(target_xy - pos_traj);
	ثابت عائم cos_align = u_vel_traj * pos_to_target.unit_or_zero();

	// The vehicle is turning if the angle between the velocity vector
	// and the direction to the target is greater than 10 degrees, the
	// velocity is large enough and the drone isn't in the acceptance
	// radius of the last WP.
	أرجع (vel_traj.longerThan(0.2f)
		&& cos_align < 0.98f
		&& pos_to_target.longerThan(_target_acceptance_radius));
}

عائم PositionSmoothing::_getMaxXYSpeed(ثابت Vector3f(&waypoints)[3]) ثابت
{
	Vector3f pos_traj(_trajectory[0].getCurrentPosition(),
			  _trajectory[1].getCurrentPosition(),
			  _trajectory[2].getCurrentPosition());

	math::trajectory::VehicleDynamicLimits config;
	config.z_accept_rad = _vertical_acceptance_radius;
	config.xy_accept_rad = _target_acceptance_radius;
	config.max_acc_xy = _trajectory[0].getMaxAccel();
	config.max_jerk = _trajectory[0].getMaxJerk();
	config.max_speed_xy = _cruise_speed;
	config.max_acc_xy_radius_scale = _horizontal_trajectory_gain;

	// constrain velocity to go to the position setpoint first if the position setpoint has been modified by an external source
	// (eg. Obstacle Avoidance)

	Vector3f pos_to_waypoints[3] = {pos_traj, waypoints[1], waypoints[2]};

	أرجع math::trajectory::computeXYSpeedFromWaypoints<3>(pos_to_waypoints, config);
}

عائم PositionSmoothing::_getMaxZSpeed(ثابت Vector3f(&waypoints)[3]) ثابت
{
	ثابت Vector3f &start_position = {_trajectory[0].getCurrentPosition(),
					  _trajectory[1].getCurrentPosition(),
					  _trajectory[2].getCurrentPosition()
					 };
	ثابت Vector3f &target = waypoints[1];
	ثابت Vector3f &next_target = waypoints[2];

	ثابت Vector2f start_position_xy_z = {start_position.xy().norm(), start_position(2)};
	ثابت Vector2f target_xy_z = {target.xy().norm(), target(2)};
	ثابت Vector2f next_target_xy_z = {next_target.xy().norm(), next_target(2)};

	عائم arrival_z_speed = 0.0f;
	ثابت منطقي target_next_different = fabsf(target(2) - next_target(2)) > 0.001f;

	𐡄𐡍 (target_next_different) {
		// rounding can push the dot product of two unit vectors past 1, which acosf turns into NaN
		ثابت عائم cos_alpha = math::constrain(Vector2f((target_xy_z - start_position_xy_z)).unit_or_zero().dot(
				Vector2f((target_xy_z - next_target_xy_z)).unit_or_zero()), -1.f, 1.f);
		ثابت عائم alpha = acosf(cos_alpha);

		ثابت عائم safe_alpha = math::constrain(alpha, 0.f, M_PI_F - FLT_EPSILON);
		عائم accel_tmp = _trajectory[2].getMaxAccel();
		عائم max_speed_in_turn = math::trajectory::computeMaxSpeedInWaypoint(safe_alpha, accel_tmp,
					  _vertical_acceptance_radius);
		arrival_z_speed = math::min(max_speed_in_turn, _trajectory[2].getMaxVel());
	}

	ثابت عائم distance_start_target = fabs(target(2) - start_position(2));
	عائم max_speed = math::min(_trajectory[2].getMaxVel(), math::trajectory::computeMaxSpeedFromDistance(
					    _trajectory[2].getMaxJerk(), _trajectory[2].getMaxAccel(),
					    distance_start_target, arrival_z_speed));

	أرجع max_speed;
}

ثابت Vector3f PositionSmoothing::_getCrossingPoint(ثابت Vector3f &position, ثابت Vector3f(&waypoints)[3]) ثابت
{
	ثابت auto &target = waypoints[1];

	𐡄𐡍 (!_isTurning(target)) {
		أرجع target;
	}

	// Get the crossing point using L1-style guidance
	أرجع _getL1Point(position, waypoints);
}

ثابت Vector3f PositionSmoothing::_getL1Point(ثابت Vector3f &position, ثابت Vector3f(&waypoints)[3]) ثابت
{
	ثابت Vector3f pos_traj(_trajectory[0].getCurrentPosition(), _trajectory[1].getCurrentPosition(),
				_trajectory[2].getCurrentPosition());
	ثابت Vector3f prev_to_target = waypoints[1] - waypoints[0];
	ثابت عائم leg_length = prev_to_target.length();
	ثابت Vector3f u_prev_to_target = (leg_length > FLT_EPSILON) ? prev_to_target / leg_length : Vector3f();
	ثابت Vector3f prev_to_pos(pos_traj - waypoints[0]);
	ثابت عائم along_track = prev_to_pos * u_prev_to_target;

	// If the trajectory has already passed the target, aim straight at it instead of
	// projecting the look-ahead point further along the (extended) leg. Otherwise an
	// overshoot of a not-yet-accepted waypoint marches the setpoint away from it forever.
	𐡄𐡍 (along_track >= leg_length) {
		أرجع waypoints[1];
	}

	ثابت Vector3f closest_pt = waypoints[0] + u_prev_to_target * along_track;

	// Compute along-track error using L1 distance and cross-track error
	ثابت عائم crosstrack_error = (closest_pt - pos_traj).length();

	ثابت عائم l1 = math::max(_target_acceptance_radius, 5.f);
	عائم alongtrack_error = 0.f;

	// Protect against sqrt of a negative number
	𐡄𐡍 (l1 > crosstrack_error) {
		alongtrack_error = sqrtf(l1 * l1 - crosstrack_error * crosstrack_error);
	}

	// Position of the point on the line where L1 intersect the line between the two waypoints
	أرجع closest_pt + alongtrack_error * u_prev_to_target;
}

ثابت Vector3f PositionSmoothing::_generateVelocitySetpoint(ثابت Vector3f &position, ثابت Vector3f(&waypoints)[3],
		منطقي is_single_waypoint,
		ثابت Vector3f &feedforward_velocity_setpoint)
{
	// Interface: A valid position setpoint generates a velocity target using conservative motion constraints.
	// If a velocity is specified, that is used as a feedforward to track the position setpoint
	// (ie. it assumes the position setpoint is moving at the specified velocity)
	// If the position setpoints are set to NAN, the values in the velocity setpoints are used as velocity targets: nothing to do here.
	ثابت Vector3f &target = waypoints[1];
	ثابت منطقي xy_target_valid = Vector2f(target).isAllFinite();
	ثابت منطقي z_target_valid = PX4_ISFINITE(target(2));

	Vector3f velocity_setpoint = feedforward_velocity_setpoint;

	𐡄𐡍 (xy_target_valid && z_target_valid) {
		// Use 3D position setpoint to generate a 3D velocity setpoint
		Vector3f pos_traj(_trajectory[0].getCurrentPosition(),
				  _trajectory[1].getCurrentPosition(),
				  _trajectory[2].getCurrentPosition());
		ثابت Vector3f crossing_point = is_single_waypoint ? target : _getCrossingPoint(position, waypoints);
		ثابت Vector3f u_pos_traj_to_dest{(crossing_point - pos_traj).unit_or_zero()};

		عائم xy_speed = _getMaxXYSpeed(waypoints);
		ثابت عائم z_speed = _getMaxZSpeed(waypoints);

		𐡄𐡍 (!is_single_waypoint && _isTurning(target)) {
			// Limit speed during a turn
			xy_speed = math::min(_max_speed_previous, xy_speed);

		} وإلا {
			_max_speed_previous = xy_speed;
		}

		// Scale one 3D direction to fit both limits, so the slower axis limits the other
		// one and a leg that changes altitude is flown as a straight line
		Vector3f vel_sp_constrained = u_pos_traj_to_dest * sqrtf(xy_speed * xy_speed + z_speed * z_speed);
		math::trajectory::clampToXYNorm(vel_sp_constrained, xy_speed, 0.5f);
		math::trajectory::clampToZNorm(vel_sp_constrained, z_speed, 0.5f);

		لكل (صحيح i = 0; i < 3; i++) {
			// If available, use the existing velocity as a feedforward, otherwise replace it
			𐡄𐡍 (PX4_ISFINITE(velocity_setpoint(i))) {
				velocity_setpoint(i) += vel_sp_constrained(i);

			} وإلا {
				velocity_setpoint(i) = vel_sp_constrained(i);
			}
		}
	}

	وإلا 𐡄𐡍 (xy_target_valid) {
		// Use 2D position setpoint to generate a 2D velocity setpoint

		// Get various path specific vectors
		Vector2f pos_traj(_trajectory[0].getCurrentPosition(), _trajectory[1].getCurrentPosition());
		Vector2f crossing_point = is_single_waypoint ? Vector2f(target) : Vector2f(_getCrossingPoint(position, waypoints));
		Vector2f pos_traj_to_dest_xy = crossing_point - pos_traj;
		Vector2f u_pos_traj_to_dest_xy(pos_traj_to_dest_xy.unit_or_zero());

		عائم xy_speed = _getMaxXYSpeed(waypoints);

		𐡄𐡍 (_isTurning(target)) {
			// Lock speed during turn
			xy_speed = math::min(_max_speed_previous, xy_speed);

		} وإلا {
			_max_speed_previous = xy_speed;
		}

		Vector2f vel_sp_constrained_xy = u_pos_traj_to_dest_xy * xy_speed;

		لكل (صحيح i = 0; i < 2; i++) {
			// If available, use the existing velocity as a feedforward, otherwise replace it
			𐡄𐡍 (PX4_ISFINITE(velocity_setpoint(i))) {
				velocity_setpoint(i) += vel_sp_constrained_xy(i);

			} وإلا {
				velocity_setpoint(i) = vel_sp_constrained_xy(i);
			}
		}
	}

	وإلا 𐡄𐡍 (z_target_valid) {
		// Use Z position setpoint to generate a Z velocity setpoint

		ثابت عائم z_dir = matrix::sign(target(2) - _trajectory[2].getCurrentPosition());
		ثابت عائم vel_sp_z = z_dir * _getMaxZSpeed(waypoints);

		// If available, use the existing velocity as a feedforward, otherwise replace it
		𐡄𐡍 (PX4_ISFINITE(velocity_setpoint(2))) {
			velocity_setpoint(2) += vel_sp_z;

		} وإلا {
			velocity_setpoint(2) = vel_sp_z;
		}
	}

	أرجع velocity_setpoint;
}


فراغ PositionSmoothing::_generateTrajectory(
	ثابت Vector3f &position,
	ثابت Vector3f &velocity_setpoint,
	عائم delta_time,
	PositionSmoothingSetpoints &out_setpoints)
{
	𐡄𐡍 (!velocity_setpoint.isAllFinite()) {
		أرجع;
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
	Vector2f drone_to_trajectory_xy(position_trajectory_xy - position_xy);
	عائم position_error_xy = drone_to_trajectory_xy.length();

	عائم time_stretch_xy = 1.f;

	// Only stretch time if there's no division by zero and the drone isn't ahead of the position setpoint
	𐡄𐡍 ((_max_allowed_horizontal_error > FLT_EPSILON)
	    && drone_to_trajectory_xy.dot(vel_traj_xy) >= 0) {
		time_stretch_xy = 1.f - math::constrain(position_error_xy / _max_allowed_horizontal_error, 0.f, 1.f);
	}

	𐡄𐡍 (drone_to_trajectory_xy.dot(vel_traj_xy) < 0.f) {
		time_stretch_xy = 1.f;
	}

	// Same idea for the vertical axis: only stretch when the virtual trajectory is leading the
	// drone AND moving further away from it. Reversing trajectories (virtual coming back toward
	// the drone) are left alone so we can recover without artificially slowing down.
	عائم time_stretch_z = 1.f;
	ثابت عائم drone_to_traj_z = _trajectory[2].getCurrentPosition() - position(2);
	ثابت عائم vel_traj_z = _trajectory[2].getCurrentVelocity();

	𐡄𐡍 ((_max_allowed_vertical_error > FLT_EPSILON)
	    && drone_to_traj_z * vel_traj_z >= 0.f) {
		time_stretch_z = 1.f - math::constrain(fabsf(drone_to_traj_z) / _max_allowed_vertical_error, 0.f, 1.f);
	}

	// Axis 0,1 (XY) get the horizontal stretch; axis 2 (Z) gets the vertical stretch.
	ثابت عائم axis_stretch[3] = { time_stretch_xy, time_stretch_xy, time_stretch_z };

	لكل (صحيح i = 0; i < 3; ++i) {
		_trajectory[i].updateTraj(delta_time, axis_stretch[i]);
		out_setpoints.jerk(i) = _trajectory[i].getCurrentJerk();
		out_setpoints.acceleration(i) = _trajectory[i].getCurrentAcceleration();
		out_setpoints.velocity(i) = _trajectory[i].getCurrentVelocity();
		out_setpoints.position(i) = _trajectory[i].getCurrentPosition();
	}

	لكل (صحيح i = 0; i < 3; ++i) {
		_trajectory[i].updateDurations(velocity_setpoint(i));
	}

	VelocitySmoothing::timeSynchronization(_trajectory, 3);
}
