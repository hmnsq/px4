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

#అన్తర్భావ "PositionSmoothing.hpp"
#అన్తర్భావ "TrajectoryConstraints.hpp"
#అన్తర్భావ <mathlib/mathlib.h>
#అన్తర్భావ <matrix/matrix/math.hpp>

శూన్య PositionSmoothing::_generateSetpoints(
	స్థిరరాశి Vector3f &position,
	స్థిరరాశి Vector3f(&waypoints)[3],
	తార్కిక is_single_waypoint,
	స్థిరరాశి Vector3f &feedforward_velocity,
	దశాంశం delta_సమయం,
	తార్కిక force_zero_velocity_setpoint,
	PositionSmoothingSetpoints &బహిర్గతం_setpoints)
{
	Vector3f velocity_setpoint{0.f, 0.f, 0.f};

	ఒకవేళ (!force_zero_velocity_setpoint) {
		velocity_setpoint = _generateVelocitySetpoint(position, waypoints, is_single_waypoint, feedforward_velocity);
	}

	బహిర్గతం_setpoints.unsmoothed_velocity = velocity_setpoint;

	_generateTrajectory(
		position,
		velocity_setpoint,
		delta_సమయం,
		బహిర్గతం_setpoints
	);
}


తార్కిక PositionSmoothing::_isTurning(స్థిరరాశి Vector3f &లక్ష్యం) స్థిరరాశి
{
	స్థిరరాశి Vector2f vel_traj(_trajectory[0].getCurrentVelocity(),
				_trajectory[1].getCurrentVelocity());
	స్థిరరాశి Vector2f pos_traj(_trajectory[0].getCurrentPosition(),
				_trajectory[1].getCurrentPosition());
	స్థిరరాశి Vector2f లక్ష్యం_xy(లక్ష్యం);
	స్థిరరాశి Vector2f u_vel_traj = vel_traj.unit_or_zero();
	స్థిరరాశి Vector2f pos_గమ్యం_లక్ష్యం = Vector2f(లక్ష్యం_xy - pos_traj);
	స్థిరరాశి దశాంశం cos_align = u_vel_traj * pos_గమ్యం_లక్ష్యం.unit_or_zero();

	// The vehicle is turning if the angle between the velocity vector
	// and the direction to the target is greater than 10 degrees, the
	// velocity is large enough and the drone isn't in the acceptance
	// radius of the last WP.
	తిరిగి (vel_traj.longerThan(0.2f)
		&& cos_align < 0.98f
		&& pos_గమ్యం_లక్ష్యం.longerThan(లక్ష్యం_acceptance_radius));
}

దశాంశం PositionSmoothing::_getMaxXYSpeed(స్థిరరాశి Vector3f(&waypoints)[3]) స్థిరరాశి
{
	Vector3f pos_traj(_trajectory[0].getCurrentPosition(),
			  _trajectory[1].getCurrentPosition(),
			  _trajectory[2].getCurrentPosition());

	math::trajectory::VehicleDynamicLimits config;
	config.z_accept_rad = _vertical_acceptance_radius;
	config.xy_accept_rad = లక్ష్యం_acceptance_radius;
	config.max_acc_xy = _trajectory[0].getMaxAccel();
	config.max_jerk = _trajectory[0].getMaxJerk();
	config.max_వేగం_xy = cruise_వేగం;
	config.max_acc_xy_radius_scale = _horizontal_trajectory_gain;

	// constrain velocity to go to the position setpoint first if the position setpoint has been modified by an external source
	// (eg. Obstacle Avoidance)

	Vector3f pos_గమ్యం_waypoints[3] = {pos_traj, waypoints[1], waypoints[2]};

	తిరిగి math::trajectory::computeXYSpeedFromWaypoints<3>(pos_గమ్యం_waypoints, config);
}

దశాంశం PositionSmoothing::_getMaxZSpeed(స్థిరరాశి Vector3f(&waypoints)[3]) స్థిరరాశి
{
	స్థిరరాశి Vector3f &ప్రారంభం_position = {_trajectory[0].getCurrentPosition(),
					  _trajectory[1].getCurrentPosition(),
					  _trajectory[2].getCurrentPosition()
					 };
	స్థిరరాశి Vector3f &లక్ష్యం = waypoints[1];
	స్థిరరాశి Vector3f &తదుపరి_లక్ష్యం = waypoints[2];

	స్థిరరాశి Vector2f ప్రారంభం_position_xy_z = {ప్రారంభం_position.xy().norm(), ప్రారంభం_position(2)};
	స్థిరరాశి Vector2f లక్ష్యం_xy_z = {లక్ష్యం.xy().norm(), లక్ష్యం(2)};
	స్థిరరాశి Vector2f తదుపరి_లక్ష్యం_xy_z = {తదుపరి_లక్ష్యం.xy().norm(), తదుపరి_లక్ష్యం(2)};

	దశాంశం రాక_z_వేగం = 0.0f;
	స్థిరరాశి తార్కిక లక్ష్యం_తదుపరి_different = fabsf(లక్ష్యం(2) - తదుపరి_లక్ష్యం(2)) > 0.001f;

	ఒకవేళ (లక్ష్యం_తదుపరి_different) {
		// rounding can push the dot product of two unit vectors past 1, which acosf turns into NaN
		స్థిరరాశి దశాంశం cos_alpha = math::constrain(Vector2f((లక్ష్యం_xy_z - ప్రారంభం_position_xy_z)).unit_or_zero().dot(
				Vector2f((లక్ష్యం_xy_z - తదుపరి_లక్ష్యం_xy_z)).unit_or_zero()), -1.f, 1.f);
		స్థిరరాశి దశాంశం alpha = acosf(cos_alpha);

		స్థిరరాశి దశాంశం సురక్షిత_alpha = math::constrain(alpha, 0.f, M_PI_F - FLT_EPSILON);
		దశాంశం accel_tmp = _trajectory[2].getMaxAccel();
		దశాంశం max_వేగం_లోపల_turn = math::trajectory::compute_Max_వేగం_లోపల_Waypoint(సురక్షిత_alpha, accel_tmp,
					  _vertical_acceptance_radius);
		రాక_z_వేగం = math::min(max_వేగం_లోపల_turn, _trajectory[2].getMaxVel());
	}

	స్థిరరాశి దశాంశం దూరం_ప్రారంభం_లక్ష్యం = fabs(లక్ష్యం(2) - ప్రారంభం_position(2));
	దశాంశం max_వేగం = math::min(_trajectory[2].getMaxVel(), math::trajectory::compute_Max_వేగం_From_దూరం(
					    _trajectory[2].getMaxJerk(), _trajectory[2].getMaxAccel(),
					    దూరం_ప్రారంభం_లక్ష్యం, రాక_z_వేగం));

	తిరిగి max_వేగం;
}

స్థిరరాశి Vector3f PositionSmoothing::get_దాటడం_బిందువు(స్థిరరాశి Vector3f &position, స్థిరరాశి Vector3f(&waypoints)[3]) స్థిరరాశి
{
	స్థిరరాశి స్వయంచాలక &లక్ష్యం = waypoints[1];

	ఒకవేళ (!_isTurning(లక్ష్యం)) {
		తిరిగి లక్ష్యం;
	}

	// Get the crossing point using L1-style guidance
	తిరిగి get_L1_బిందువు(position, waypoints);
}

స్థిరరాశి Vector3f PositionSmoothing::get_L1_బిందువు(స్థిరరాశి Vector3f &position, స్థిరరాశి Vector3f(&waypoints)[3]) స్థిరరాశి
{
	స్థిరరాశి Vector3f pos_traj(_trajectory[0].getCurrentPosition(), _trajectory[1].getCurrentPosition(),
				_trajectory[2].getCurrentPosition());
	స్థిరరాశి Vector3f prev_గమ్యం_లక్ష్యం = waypoints[1] - waypoints[0];
	స్థిరరాశి దశాంశం leg_నిడివి = prev_గమ్యం_లక్ష్యం.నిడివి();
	స్థిరరాశి Vector3f u_prev_గమ్యం_లక్ష్యం = (leg_నిడివి > FLT_EPSILON) ? prev_గమ్యం_లక్ష్యం / leg_నిడివి : Vector3f();
	స్థిరరాశి Vector3f prev_గమ్యం_pos(pos_traj - waypoints[0]);
	స్థిరరాశి దశాంశం along_track = prev_గమ్యం_pos * u_prev_గమ్యం_లక్ష్యం;

	// If the trajectory has already passed the target, aim straight at it instead of
	// projecting the look-ahead point further along the (extended) leg. Otherwise an
	// overshoot of a not-yet-accepted waypoint marches the setpoint away from it forever.
	ఒకవేళ (along_track >= leg_నిడివి) {
		తిరిగి waypoints[1];
	}

	స్థిరరాశి Vector3f closest_pt = waypoints[0] + u_prev_గమ్యం_లక్ష్యం * along_track;

	// Compute along-track error using L1 distance and cross-track error
	స్థిరరాశి దశాంశం crosstrack_error = (closest_pt - pos_traj).నిడివి();

	స్థిరరాశి దశాంశం l1 = math::max(లక్ష్యం_acceptance_radius, 5.f);
	దశాంశం alongtrack_error = 0.f;

	// Protect against sqrt of a negative number
	ఒకవేళ (l1 > crosstrack_error) {
		alongtrack_error = sqrtf(l1 * l1 - crosstrack_error * crosstrack_error);
	}

	// Position of the point on the line where L1 intersect the line between the two waypoints
	తిరిగి closest_pt + alongtrack_error * u_prev_గమ్యం_లక్ష్యం;
}

స్థిరరాశి Vector3f PositionSmoothing::_generateVelocitySetpoint(స్థిరరాశి Vector3f &position, స్థిరరాశి Vector3f(&waypoints)[3],
		తార్కిక is_single_waypoint,
		స్థిరరాశి Vector3f &feedforward_velocity_setpoint)
{
	// Interface: A valid position setpoint generates a velocity target using conservative motion constraints.
	// If a velocity is specified, that is used as a feedforward to track the position setpoint
	// (ie. it assumes the position setpoint is moving at the specified velocity)
	// If the position setpoints are set to NAN, the values in the velocity setpoints are used as velocity targets: nothing to do here.
	స్థిరరాశి Vector3f &లక్ష్యం = waypoints[1];
	స్థిరరాశి తార్కిక xy_లక్ష్యం_valid = Vector2f(లక్ష్యం).isAllFinite();
	స్థిరరాశి తార్కిక z_లక్ష్యం_valid = PX4_ISFINITE(లక్ష్యం(2));

	Vector3f velocity_setpoint = feedforward_velocity_setpoint;

	ఒకవేళ (xy_లక్ష్యం_valid && z_లక్ష్యం_valid) {
		// Use 3D position setpoint to generate a 3D velocity setpoint
		Vector3f pos_traj(_trajectory[0].getCurrentPosition(),
				  _trajectory[1].getCurrentPosition(),
				  _trajectory[2].getCurrentPosition());
		స్థిరరాశి Vector3f దాటడం_బిందువు = is_single_waypoint ? లక్ష్యం : get_దాటడం_బిందువు(position, waypoints);
		స్థిరరాశి Vector3f u_pos_traj_గమ్యం_dest{(దాటడం_బిందువు - pos_traj).unit_or_zero()};

		దశాంశం xy_వేగం = _getMaxXYSpeed(waypoints);
		స్థిరరాశి దశాంశం z_వేగం = _getMaxZSpeed(waypoints);

		ఒకవేళ (!is_single_waypoint && _isTurning(లక్ష్యం)) {
			// Limit speed during a turn
			xy_వేగం = math::min(max_వేగం_మునుపటి, xy_వేగం);

		} లేకపోతే {
			max_వేగం_మునుపటి = xy_వేగం;
		}

		// Scale one 3D direction to fit both limits, so the slower axis limits the other
		// one and a leg that changes altitude is flown as a straight line
		Vector3f vel_sp_constrained = u_pos_traj_గమ్యం_dest * sqrtf(xy_వేగం * xy_వేగం + z_వేగం * z_వేగం);
		math::trajectory::clamp_గమ్యం_XYNorm(vel_sp_constrained, xy_వేగం, 0.5f);
		math::trajectory::clamp_గమ్యం_ZNorm(vel_sp_constrained, z_వేగం, 0.5f);

		కొరకు (పూర్ణాంకం i = 0; i < 3; i++) {
			// If available, use the existing velocity as a feedforward, otherwise replace it
			ఒకవేళ (PX4_ISFINITE(velocity_setpoint(i))) {
				velocity_setpoint(i) += vel_sp_constrained(i);

			} లేకపోతే {
				velocity_setpoint(i) = vel_sp_constrained(i);
			}
		}
	}

	లేకపోతే ఒకవేళ (xy_లక్ష్యం_valid) {
		// Use 2D position setpoint to generate a 2D velocity setpoint

		// Get various path specific vectors
		Vector2f pos_traj(_trajectory[0].getCurrentPosition(), _trajectory[1].getCurrentPosition());
		Vector2f దాటడం_బిందువు = is_single_waypoint ? Vector2f(లక్ష్యం) : Vector2f(get_దాటడం_బిందువు(position, waypoints));
		Vector2f pos_traj_గమ్యం_dest_xy = దాటడం_బిందువు - pos_traj;
		Vector2f u_pos_traj_గమ్యం_dest_xy(pos_traj_గమ్యం_dest_xy.unit_or_zero());

		దశాంశం xy_వేగం = _getMaxXYSpeed(waypoints);

		ఒకవేళ (_isTurning(లక్ష్యం)) {
			// Lock speed during turn
			xy_వేగం = math::min(max_వేగం_మునుపటి, xy_వేగం);

		} లేకపోతే {
			max_వేగం_మునుపటి = xy_వేగం;
		}

		Vector2f vel_sp_constrained_xy = u_pos_traj_గమ్యం_dest_xy * xy_వేగం;

		కొరకు (పూర్ణాంకం i = 0; i < 2; i++) {
			// If available, use the existing velocity as a feedforward, otherwise replace it
			ఒకవేళ (PX4_ISFINITE(velocity_setpoint(i))) {
				velocity_setpoint(i) += vel_sp_constrained_xy(i);

			} లేకపోతే {
				velocity_setpoint(i) = vel_sp_constrained_xy(i);
			}
		}
	}

	లేకపోతే ఒకవేళ (z_లక్ష్యం_valid) {
		// Use Z position setpoint to generate a Z velocity setpoint

		స్థిరరాశి దశాంశం z_dir = మాతృక::sign(లక్ష్యం(2) - _trajectory[2].getCurrentPosition());
		స్థిరరాశి దశాంశం vel_sp_z = z_dir * _getMaxZSpeed(waypoints);

		// If available, use the existing velocity as a feedforward, otherwise replace it
		ఒకవేళ (PX4_ISFINITE(velocity_setpoint(2))) {
			velocity_setpoint(2) += vel_sp_z;

		} లేకపోతే {
			velocity_setpoint(2) = vel_sp_z;
		}
	}

	తిరిగి velocity_setpoint;
}


శూన్య PositionSmoothing::_generateTrajectory(
	స్థిరరాశి Vector3f &position,
	స్థిరరాశి Vector3f &velocity_setpoint,
	దశాంశం delta_సమయం,
	PositionSmoothingSetpoints &బహిర్గతం_setpoints)
{
	ఒకవేళ (!velocity_setpoint.isAllFinite()) {
		తిరిగి;
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
	Vector2f drone_గమ్యం_trajectory_xy(position_trajectory_xy - position_xy);
	దశాంశం position_error_xy = drone_గమ్యం_trajectory_xy.నిడివి();

	దశాంశం సమయం_stretch_xy = 1.f;

	// Only stretch time if there's no division by zero and the drone isn't ahead of the position setpoint
	ఒకవేళ ((_max_allowed_horizontal_error > FLT_EPSILON)
	    && drone_గమ్యం_trajectory_xy.dot(vel_traj_xy) >= 0) {
		సమయం_stretch_xy = 1.f - math::constrain(position_error_xy / _max_allowed_horizontal_error, 0.f, 1.f);
	}

	ఒకవేళ (drone_గమ్యం_trajectory_xy.dot(vel_traj_xy) < 0.f) {
		సమయం_stretch_xy = 1.f;
	}

	// Same idea for the vertical axis: only stretch when the virtual trajectory is leading the
	// drone AND moving further away from it. Reversing trajectories (virtual coming back toward
	// the drone) are left alone so we can recover without artificially slowing down.
	దశాంశం సమయం_stretch_z = 1.f;
	స్థిరరాశి దశాంశం drone_గమ్యం_traj_z = _trajectory[2].getCurrentPosition() - position(2);
	స్థిరరాశి దశాంశం vel_traj_z = _trajectory[2].getCurrentVelocity();

	ఒకవేళ ((_max_allowed_vertical_error > FLT_EPSILON)
	    && drone_గమ్యం_traj_z * vel_traj_z >= 0.f) {
		సమయం_stretch_z = 1.f - math::constrain(fabsf(drone_గమ్యం_traj_z) / _max_allowed_vertical_error, 0.f, 1.f);
	}

	// Axis 0,1 (XY) get the horizontal stretch; axis 2 (Z) gets the vertical stretch.
	స్థిరరాశి దశాంశం axis_stretch[3] = { సమయం_stretch_xy, సమయం_stretch_xy, సమయం_stretch_z };

	కొరకు (పూర్ణాంకం i = 0; i < 3; ++i) {
		_trajectory[i].updateTraj(delta_సమయం, axis_stretch[i]);
		బహిర్గతం_setpoints.jerk(i) = _trajectory[i].getCurrentJerk();
		బహిర్గతం_setpoints.acceleration(i) = _trajectory[i].getCurrentAcceleration();
		బహిర్గతం_setpoints.velocity(i) = _trajectory[i].getCurrentVelocity();
		బహిర్గతం_setpoints.position(i) = _trajectory[i].getCurrentPosition();
	}

	కొరకు (పూర్ణాంకం i = 0; i < 3; ++i) {
		_trajectory[i].updateDurations(velocity_setpoint(i));
	}

	VelocitySmoothing::సమయం_Synchronization(_trajectory, 3);
}
