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
	عائم delta_وخت,
	منطقي force_zero_velocity_setpoint,
	PositionSmoothingSetpoints &بهر_setpoints)
{
	Vector3f velocity_setpoint{0.f, 0.f, 0.f};

	إذا (!force_zero_velocity_setpoint) {
		velocity_setpoint = _generateVelocitySetpoint(position, waypoints, is_single_waypoint, feedforward_velocity);
	}

	بهر_setpoints.unsmoothed_velocity = velocity_setpoint;

	_generateTrajectory(
		position,
		velocity_setpoint,
		delta_وخت,
		بهر_setpoints
	);
}


منطقي PositionSmoothing::_isTurning(ثابت Vector3f &موخه) ثابت
{
	ثابت Vector2f vel_traj(_trajectory[0].getCurrentVelocity(),
				_trajectory[1].getCurrentVelocity());
	ثابت Vector2f pos_traj(_trajectory[0].getCurrentPosition(),
				_trajectory[1].getCurrentPosition());
	ثابت Vector2f موخه_xy(موخه);
	ثابت Vector2f u_vel_traj = vel_traj.unit_or_zero();
	ثابت Vector2f pos_منزل_موخه = Vector2f(موخه_xy - pos_traj);
	ثابت عائم cos_align = u_vel_traj * pos_منزل_موخه.unit_or_zero();

	// The vehicle is turning if the angle between the velocity vector
	// and the direction to the target is greater than 10 degrees, the
	// velocity is large enough and the drone isn't in the acceptance
	// radius of the last WP.
	أرجع (vel_traj.longerThan(0.2f)
		&& cos_align < 0.98f
		&& pos_منزل_موخه.longerThan(موخه_acceptance_radius));
}

عائم PositionSmoothing::_getMaxXYSpeed(ثابت Vector3f(&waypoints)[3]) ثابت
{
	Vector3f pos_traj(_trajectory[0].getCurrentPosition(),
			  _trajectory[1].getCurrentPosition(),
			  _trajectory[2].getCurrentPosition());

	math::trajectory::VehicleDynamicLimits config;
	config.z_accept_rad = _vertical_acceptance_radius;
	config.xy_accept_rad = موخه_acceptance_radius;
	config.max_acc_xy = _trajectory[0].getMaxAccel();
	config.max_jerk = _trajectory[0].getMaxJerk();
	config.max_چټکتیا_xy = cruise_چټکتیا;
	config.max_acc_xy_radius_scale = _horizontal_trajectory_gain;

	// constrain velocity to go to the position setpoint first if the position setpoint has been modified by an external source
	// (eg. Obstacle Avoidance)

	Vector3f pos_منزل_waypoints[3] = {pos_traj, waypoints[1], waypoints[2]};

	أرجع math::trajectory::computeXYSpeedFromWaypoints<3>(pos_منزل_waypoints, config);
}

عائم PositionSmoothing::_getMaxZSpeed(ثابت Vector3f(&waypoints)[3]) ثابت
{
	ثابت Vector3f &پیل_position = {_trajectory[0].getCurrentPosition(),
					  _trajectory[1].getCurrentPosition(),
					  _trajectory[2].getCurrentPosition()
					 };
	ثابت Vector3f &موخه = waypoints[1];
	ثابت Vector3f &بل_موخه = waypoints[2];

	ثابت Vector2f پیل_position_xy_z = {پیل_position.xy().norm(), پیل_position(2)};
	ثابت Vector2f موخه_xy_z = {موخه.xy().norm(), موخه(2)};
	ثابت Vector2f بل_موخه_xy_z = {بل_موخه.xy().norm(), بل_موخه(2)};

	عائم رارسېدنه_z_چټکتیا = 0.0f;
	ثابت منطقي موخه_بل_different = fabsf(موخه(2) - بل_موخه(2)) > 0.001f;

	إذا (موخه_بل_different) {
		// rounding can push the dot product of two unit vectors past 1, which acosf turns into NaN
		ثابت عائم cos_alpha = math::constrain(Vector2f((موخه_xy_z - پیل_position_xy_z)).unit_or_zero().dot(
				Vector2f((موخه_xy_z - بل_موخه_xy_z)).unit_or_zero()), -1.f, 1.f);
		ثابت عائم alpha = acosf(cos_alpha);

		ثابت عائم خوندي_alpha = math::constrain(alpha, 0.f, M_PI_F - FLT_EPSILON);
		عائم accel_tmp = _trajectory[2].getMaxAccel();
		عائم max_چټکتیا_دننه_turn = math::trajectory::compute_Max_چټکتیا_دننه_Waypoint(خوندي_alpha, accel_tmp,
					  _vertical_acceptance_radius);
		رارسېدنه_z_چټکتیا = math::min(max_چټکتیا_دننه_turn, _trajectory[2].getMaxVel());
	}

	ثابت عائم واټن_پیل_موخه = fabs(موخه(2) - پیل_position(2));
	عائم max_چټکتیا = math::min(_trajectory[2].getMaxVel(), math::trajectory::compute_Max_چټکتیا_From_واټن(
					    _trajectory[2].getMaxJerk(), _trajectory[2].getMaxAccel(),
					    واټن_پیل_موخه, رارسېدنه_z_چټکتیا));

	أرجع max_چټکتیا;
}

ثابت Vector3f PositionSmoothing::get_تېرېدنه_ټکی(ثابت Vector3f &position, ثابت Vector3f(&waypoints)[3]) ثابت
{
	ثابت خپلکاری &موخه = waypoints[1];

	إذا (!_isTurning(موخه)) {
		أرجع موخه;
	}

	// Get the crossing point using L1-style guidance
	أرجع get_L1_ټکی(position, waypoints);
}

ثابت Vector3f PositionSmoothing::get_L1_ټکی(ثابت Vector3f &position, ثابت Vector3f(&waypoints)[3]) ثابت
{
	ثابت Vector3f pos_traj(_trajectory[0].getCurrentPosition(), _trajectory[1].getCurrentPosition(),
				_trajectory[2].getCurrentPosition());
	ثابت Vector3f prev_منزل_موخه = waypoints[1] - waypoints[0];
	ثابت عائم leg_اوږدتیا = prev_منزل_موخه.اوږدتیا();
	ثابت Vector3f u_prev_منزل_موخه = (leg_اوږدتیا > FLT_EPSILON) ? prev_منزل_موخه / leg_اوږدتیا : Vector3f();
	ثابت Vector3f prev_منزل_pos(pos_traj - waypoints[0]);
	ثابت عائم along_track = prev_منزل_pos * u_prev_منزل_موخه;

	// If the trajectory has already passed the target, aim straight at it instead of
	// projecting the look-ahead point further along the (extended) leg. Otherwise an
	// overshoot of a not-yet-accepted waypoint marches the setpoint away from it forever.
	إذا (along_track >= leg_اوږدتیا) {
		أرجع waypoints[1];
	}

	ثابت Vector3f closest_pt = waypoints[0] + u_prev_منزل_موخه * along_track;

	// Compute along-track error using L1 distance and cross-track error
	ثابت عائم crosstrack_error = (closest_pt - pos_traj).اوږدتیا();

	ثابت عائم l1 = math::max(موخه_acceptance_radius, 5.f);
	عائم alongtrack_error = 0.f;

	// Protect against sqrt of a negative number
	إذا (l1 > crosstrack_error) {
		alongtrack_error = sqrtf(l1 * l1 - crosstrack_error * crosstrack_error);
	}

	// Position of the point on the line where L1 intersect the line between the two waypoints
	أرجع closest_pt + alongtrack_error * u_prev_منزل_موخه;
}

ثابت Vector3f PositionSmoothing::_generateVelocitySetpoint(ثابت Vector3f &position, ثابت Vector3f(&waypoints)[3],
		منطقي is_single_waypoint,
		ثابت Vector3f &feedforward_velocity_setpoint)
{
	// Interface: A valid position setpoint generates a velocity target using conservative motion constraints.
	// If a velocity is specified, that is used as a feedforward to track the position setpoint
	// (ie. it assumes the position setpoint is moving at the specified velocity)
	// If the position setpoints are set to NAN, the values in the velocity setpoints are used as velocity targets: nothing to do here.
	ثابت Vector3f &موخه = waypoints[1];
	ثابت منطقي xy_موخه_valid = Vector2f(موخه).isAllFinite();
	ثابت منطقي z_موخه_valid = PX4_ISFINITE(موخه(2));

	Vector3f velocity_setpoint = feedforward_velocity_setpoint;

	إذا (xy_موخه_valid && z_موخه_valid) {
		// Use 3D position setpoint to generate a 3D velocity setpoint
		Vector3f pos_traj(_trajectory[0].getCurrentPosition(),
				  _trajectory[1].getCurrentPosition(),
				  _trajectory[2].getCurrentPosition());
		ثابت Vector3f تېرېدنه_ټکی = is_single_waypoint ? موخه : get_تېرېدنه_ټکی(position, waypoints);
		ثابت Vector3f u_pos_traj_منزل_dest{(تېرېدنه_ټکی - pos_traj).unit_or_zero()};

		عائم xy_چټکتیا = _getMaxXYSpeed(waypoints);
		ثابت عائم z_چټکتیا = _getMaxZSpeed(waypoints);

		إذا (!is_single_waypoint && _isTurning(موخه)) {
			// Limit speed during a turn
			xy_چټکتیا = math::min(max_چټکتیا_پخوانی, xy_چټکتیا);

		} وإلا {
			max_چټکتیا_پخوانی = xy_چټکتیا;
		}

		// Scale one 3D direction to fit both limits, so the slower axis limits the other
		// one and a leg that changes altitude is flown as a straight line
		Vector3f vel_sp_constrained = u_pos_traj_منزل_dest * sqrtf(xy_چټکتیا * xy_چټکتیا + z_چټکتیا * z_چټکتیا);
		math::trajectory::clamp_منزل_XYNorm(vel_sp_constrained, xy_چټکتیا, 0.5f);
		math::trajectory::clamp_منزل_ZNorm(vel_sp_constrained, z_چټکتیا, 0.5f);

		لكل (صحيح i = 0; i < 3; i++) {
			// If available, use the existing velocity as a feedforward, otherwise replace it
			إذا (PX4_ISFINITE(velocity_setpoint(i))) {
				velocity_setpoint(i) += vel_sp_constrained(i);

			} وإلا {
				velocity_setpoint(i) = vel_sp_constrained(i);
			}
		}
	}

	وإلا إذا (xy_موخه_valid) {
		// Use 2D position setpoint to generate a 2D velocity setpoint

		// Get various path specific vectors
		Vector2f pos_traj(_trajectory[0].getCurrentPosition(), _trajectory[1].getCurrentPosition());
		Vector2f تېرېدنه_ټکی = is_single_waypoint ? Vector2f(موخه) : Vector2f(get_تېرېدنه_ټکی(position, waypoints));
		Vector2f pos_traj_منزل_dest_xy = تېرېدنه_ټکی - pos_traj;
		Vector2f u_pos_traj_منزل_dest_xy(pos_traj_منزل_dest_xy.unit_or_zero());

		عائم xy_چټکتیا = _getMaxXYSpeed(waypoints);

		إذا (_isTurning(موخه)) {
			// Lock speed during turn
			xy_چټکتیا = math::min(max_چټکتیا_پخوانی, xy_چټکتیا);

		} وإلا {
			max_چټکتیا_پخوانی = xy_چټکتیا;
		}

		Vector2f vel_sp_constrained_xy = u_pos_traj_منزل_dest_xy * xy_چټکتیا;

		لكل (صحيح i = 0; i < 2; i++) {
			// If available, use the existing velocity as a feedforward, otherwise replace it
			إذا (PX4_ISFINITE(velocity_setpoint(i))) {
				velocity_setpoint(i) += vel_sp_constrained_xy(i);

			} وإلا {
				velocity_setpoint(i) = vel_sp_constrained_xy(i);
			}
		}
	}

	وإلا إذا (z_موخه_valid) {
		// Use Z position setpoint to generate a Z velocity setpoint

		ثابت عائم z_dir = ماتریکس::sign(موخه(2) - _trajectory[2].getCurrentPosition());
		ثابت عائم vel_sp_z = z_dir * _getMaxZSpeed(waypoints);

		// If available, use the existing velocity as a feedforward, otherwise replace it
		إذا (PX4_ISFINITE(velocity_setpoint(2))) {
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
	عائم delta_وخت,
	PositionSmoothingSetpoints &بهر_setpoints)
{
	إذا (!velocity_setpoint.isAllFinite()) {
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
	Vector2f drone_منزل_trajectory_xy(position_trajectory_xy - position_xy);
	عائم position_error_xy = drone_منزل_trajectory_xy.اوږدتیا();

	عائم وخت_stretch_xy = 1.f;

	// Only stretch time if there's no division by zero and the drone isn't ahead of the position setpoint
	إذا ((_max_allowed_horizontal_error > FLT_EPSILON)
	    && drone_منزل_trajectory_xy.dot(vel_traj_xy) >= 0) {
		وخت_stretch_xy = 1.f - math::constrain(position_error_xy / _max_allowed_horizontal_error, 0.f, 1.f);
	}

	إذا (drone_منزل_trajectory_xy.dot(vel_traj_xy) < 0.f) {
		وخت_stretch_xy = 1.f;
	}

	// Same idea for the vertical axis: only stretch when the virtual trajectory is leading the
	// drone AND moving further away from it. Reversing trajectories (virtual coming back toward
	// the drone) are left alone so we can recover without artificially slowing down.
	عائم وخت_stretch_z = 1.f;
	ثابت عائم drone_منزل_traj_z = _trajectory[2].getCurrentPosition() - position(2);
	ثابت عائم vel_traj_z = _trajectory[2].getCurrentVelocity();

	إذا ((_max_allowed_vertical_error > FLT_EPSILON)
	    && drone_منزل_traj_z * vel_traj_z >= 0.f) {
		وخت_stretch_z = 1.f - math::constrain(fabsf(drone_منزل_traj_z) / _max_allowed_vertical_error, 0.f, 1.f);
	}

	// Axis 0,1 (XY) get the horizontal stretch; axis 2 (Z) gets the vertical stretch.
	ثابت عائم axis_stretch[3] = { وخت_stretch_xy, وخت_stretch_xy, وخت_stretch_z };

	لكل (صحيح i = 0; i < 3; ++i) {
		_trajectory[i].updateTraj(delta_وخت, axis_stretch[i]);
		بهر_setpoints.jerk(i) = _trajectory[i].getCurrentJerk();
		بهر_setpoints.acceleration(i) = _trajectory[i].getCurrentAcceleration();
		بهر_setpoints.velocity(i) = _trajectory[i].getCurrentVelocity();
		بهر_setpoints.position(i) = _trajectory[i].getCurrentPosition();
	}

	لكل (صحيح i = 0; i < 3; ++i) {
		_trajectory[i].updateDurations(velocity_setpoint(i));
	}

	VelocitySmoothing::وخت_Synchronization(_trajectory, 3);
}
