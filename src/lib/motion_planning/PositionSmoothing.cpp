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

#ಅನ್ತರ್ಭಾವ "PositionSmoothing.hpp"
#ಅನ್ತರ್ಭಾವ "TrajectoryConstraints.hpp"
#ಅನ್ತರ್ಭಾವ <mathlib/mathlib.h>
#ಅನ್ತರ್ಭಾವ <matrix/matrix/math.hpp>

ಶೂನ್ಯ PositionSmoothing::_generateSetpoints(
	ಸ್ಥಿರಾಂಕ Vector3f &position,
	ಸ್ಥಿರಾಂಕ Vector3f(&waypoints)[3],
	ತಾರ್ಕಿಕ is_single_waypoint,
	ಸ್ಥಿರಾಂಕ Vector3f &feedforward_velocity,
	ದಶಮಾಂಶ delta_ಸಮಯ,
	ತಾರ್ಕಿಕ force_zero_velocity_setpoint,
	PositionSmoothingSetpoints &ಹೊರಹೋಗು_setpoints)
{
	Vector3f velocity_setpoint{0.f, 0.f, 0.f};

	ಆದರೆ (!force_zero_velocity_setpoint) {
		velocity_setpoint = _generateVelocitySetpoint(position, waypoints, is_single_waypoint, feedforward_velocity);
	}

	ಹೊರಹೋಗು_setpoints.unsmoothed_velocity = velocity_setpoint;

	_generateTrajectory(
		position,
		velocity_setpoint,
		delta_ಸಮಯ,
		ಹೊರಹೋಗು_setpoints
	);
}


ತಾರ್ಕಿಕ PositionSmoothing::_isTurning(ಸ್ಥಿರಾಂಕ Vector3f &ಗುರಿ) ಸ್ಥಿರಾಂಕ
{
	ಸ್ಥಿರಾಂಕ Vector2f vel_traj(_trajectory[0].getCurrentVelocity(),
				_trajectory[1].getCurrentVelocity());
	ಸ್ಥಿರಾಂಕ Vector2f pos_traj(_trajectory[0].getCurrentPosition(),
				_trajectory[1].getCurrentPosition());
	ಸ್ಥಿರಾಂಕ Vector2f ಗುರಿ_xy(ಗುರಿ);
	ಸ್ಥಿರಾಂಕ Vector2f u_vel_traj = vel_traj.unit_or_zero();
	ಸ್ಥಿರಾಂಕ Vector2f pos_ಗಮ್ಯ_ಗುರಿ = Vector2f(ಗುರಿ_xy - pos_traj);
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ cos_align = u_vel_traj * pos_ಗಮ್ಯ_ಗುರಿ.unit_or_zero();

	// The vehicle is turning if the angle between the velocity vector
	// and the direction to the target is greater than 10 degrees, the
	// velocity is large enough and the drone isn't in the acceptance
	// radius of the last WP.
	ಹಿಂತಿರುಗಿ (vel_traj.longerThan(0.2f)
		&& cos_align < 0.98f
		&& pos_ಗಮ್ಯ_ಗುರಿ.longerThan(ಗುರಿ_acceptance_radius));
}

ದಶಮಾಂಶ PositionSmoothing::_getMaxXYSpeed(ಸ್ಥಿರಾಂಕ Vector3f(&waypoints)[3]) ಸ್ಥಿರಾಂಕ
{
	Vector3f pos_traj(_trajectory[0].getCurrentPosition(),
			  _trajectory[1].getCurrentPosition(),
			  _trajectory[2].getCurrentPosition());

	math::trajectory::VehicleDynamicLimits config;
	config.z_accept_rad = _vertical_acceptance_radius;
	config.xy_accept_rad = ಗುರಿ_acceptance_radius;
	config.max_acc_xy = _trajectory[0].getMaxAccel();
	config.max_jerk = _trajectory[0].getMaxJerk();
	config.max_ವೇಗ_xy = cruise_ವೇಗ;
	config.max_acc_xy_radius_scale = _horizontal_trajectory_gain;

	// constrain velocity to go to the position setpoint first if the position setpoint has been modified by an external source
	// (eg. Obstacle Avoidance)

	Vector3f pos_ಗಮ್ಯ_waypoints[3] = {pos_traj, waypoints[1], waypoints[2]};

	ಹಿಂತಿರುಗಿ math::trajectory::computeXYSpeedFromWaypoints<3>(pos_ಗಮ್ಯ_waypoints, config);
}

ದಶಮಾಂಶ PositionSmoothing::_getMaxZSpeed(ಸ್ಥಿರಾಂಕ Vector3f(&waypoints)[3]) ಸ್ಥಿರಾಂಕ
{
	ಸ್ಥಿರಾಂಕ Vector3f &ಆರಂಭ_position = {_trajectory[0].getCurrentPosition(),
					  _trajectory[1].getCurrentPosition(),
					  _trajectory[2].getCurrentPosition()
					 };
	ಸ್ಥಿರಾಂಕ Vector3f &ಗುರಿ = waypoints[1];
	ಸ್ಥಿರಾಂಕ Vector3f &ಮುಂದಿನ_ಗುರಿ = waypoints[2];

	ಸ್ಥಿರಾಂಕ Vector2f ಆರಂಭ_position_xy_z = {ಆರಂಭ_position.xy().norm(), ಆರಂಭ_position(2)};
	ಸ್ಥಿರಾಂಕ Vector2f ಗುರಿ_xy_z = {ಗುರಿ.xy().norm(), ಗುರಿ(2)};
	ಸ್ಥಿರಾಂಕ Vector2f ಮುಂದಿನ_ಗುರಿ_xy_z = {ಮುಂದಿನ_ಗುರಿ.xy().norm(), ಮುಂದಿನ_ಗುರಿ(2)};

	ದಶಮಾಂಶ ಆಗಮನ_z_ವೇಗ = 0.0f;
	ಸ್ಥಿರಾಂಕ ತಾರ್ಕಿಕ ಗುರಿ_ಮುಂದಿನ_different = fabsf(ಗುರಿ(2) - ಮುಂದಿನ_ಗುರಿ(2)) > 0.001f;

	ಆದರೆ (ಗುರಿ_ಮುಂದಿನ_different) {
		// rounding can push the dot product of two unit vectors past 1, which acosf turns into NaN
		ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ cos_alpha = math::constrain(Vector2f((ಗುರಿ_xy_z - ಆರಂಭ_position_xy_z)).unit_or_zero().dot(
				Vector2f((ಗುರಿ_xy_z - ಮುಂದಿನ_ಗುರಿ_xy_z)).unit_or_zero()), -1.f, 1.f);
		ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ alpha = acosf(cos_alpha);

		ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ ಸುರಕ್ಷಿತ_alpha = math::constrain(alpha, 0.f, M_PI_F - FLT_EPSILON);
		ದಶಮಾಂಶ accel_tmp = _trajectory[2].getMaxAccel();
		ದಶಮಾಂಶ max_ವೇಗ_ಒಳ_turn = math::trajectory::compute_Max_ವೇಗ_ಒಳ_Waypoint(ಸುರಕ್ಷಿತ_alpha, accel_tmp,
					  _vertical_acceptance_radius);
		ಆಗಮನ_z_ವೇಗ = math::min(max_ವೇಗ_ಒಳ_turn, _trajectory[2].getMaxVel());
	}

	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ ದೂರ_ಆರಂಭ_ಗುರಿ = fabs(ಗುರಿ(2) - ಆರಂಭ_position(2));
	ದಶಮಾಂಶ max_ವೇಗ = math::min(_trajectory[2].getMaxVel(), math::trajectory::compute_Max_ವೇಗ_From_ದೂರ(
					    _trajectory[2].getMaxJerk(), _trajectory[2].getMaxAccel(),
					    ದೂರ_ಆರಂಭ_ಗುರಿ, ಆಗಮನ_z_ವೇಗ));

	ಹಿಂತಿರುಗಿ max_ವೇಗ;
}

ಸ್ಥಿರಾಂಕ Vector3f PositionSmoothing::get_ದಾಟುವಿಕೆ_ಬಿಂದು(ಸ್ಥಿರಾಂಕ Vector3f &position, ಸ್ಥಿರಾಂಕ Vector3f(&waypoints)[3]) ಸ್ಥಿರಾಂಕ
{
	ಸ್ಥಿರಾಂಕ ಸ್ವಯಂಚಾಲಿತ &ಗುರಿ = waypoints[1];

	ಆದರೆ (!_isTurning(ಗುರಿ)) {
		ಹಿಂತಿರುಗಿ ಗುರಿ;
	}

	// Get the crossing point using L1-style guidance
	ಹಿಂತಿರುಗಿ get_L1_ಬಿಂದು(position, waypoints);
}

ಸ್ಥಿರಾಂಕ Vector3f PositionSmoothing::get_L1_ಬಿಂದು(ಸ್ಥಿರಾಂಕ Vector3f &position, ಸ್ಥಿರಾಂಕ Vector3f(&waypoints)[3]) ಸ್ಥಿರಾಂಕ
{
	ಸ್ಥಿರಾಂಕ Vector3f pos_traj(_trajectory[0].getCurrentPosition(), _trajectory[1].getCurrentPosition(),
				_trajectory[2].getCurrentPosition());
	ಸ್ಥಿರಾಂಕ Vector3f prev_ಗಮ್ಯ_ಗುರಿ = waypoints[1] - waypoints[0];
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ leg_ಉದ್ದಳತೆ = prev_ಗಮ್ಯ_ಗುರಿ.ಉದ್ದಳತೆ();
	ಸ್ಥಿರಾಂಕ Vector3f u_prev_ಗಮ್ಯ_ಗುರಿ = (leg_ಉದ್ದಳತೆ > FLT_EPSILON) ? prev_ಗಮ್ಯ_ಗುರಿ / leg_ಉದ್ದಳತೆ : Vector3f();
	ಸ್ಥಿರಾಂಕ Vector3f prev_ಗಮ್ಯ_pos(pos_traj - waypoints[0]);
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ along_track = prev_ಗಮ್ಯ_pos * u_prev_ಗಮ್ಯ_ಗುರಿ;

	// If the trajectory has already passed the target, aim straight at it instead of
	// projecting the look-ahead point further along the (extended) leg. Otherwise an
	// overshoot of a not-yet-accepted waypoint marches the setpoint away from it forever.
	ಆದರೆ (along_track >= leg_ಉದ್ದಳತೆ) {
		ಹಿಂತಿರುಗಿ waypoints[1];
	}

	ಸ್ಥಿರಾಂಕ Vector3f closest_pt = waypoints[0] + u_prev_ಗಮ್ಯ_ಗುರಿ * along_track;

	// Compute along-track error using L1 distance and cross-track error
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ crosstrack_error = (closest_pt - pos_traj).ಉದ್ದಳತೆ();

	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ l1 = math::max(ಗುರಿ_acceptance_radius, 5.f);
	ದಶಮಾಂಶ alongtrack_error = 0.f;

	// Protect against sqrt of a negative number
	ಆದರೆ (l1 > crosstrack_error) {
		alongtrack_error = sqrtf(l1 * l1 - crosstrack_error * crosstrack_error);
	}

	// Position of the point on the line where L1 intersect the line between the two waypoints
	ಹಿಂತಿರುಗಿ closest_pt + alongtrack_error * u_prev_ಗಮ್ಯ_ಗುರಿ;
}

ಸ್ಥಿರಾಂಕ Vector3f PositionSmoothing::_generateVelocitySetpoint(ಸ್ಥಿರಾಂಕ Vector3f &position, ಸ್ಥಿರಾಂಕ Vector3f(&waypoints)[3],
		ತಾರ್ಕಿಕ is_single_waypoint,
		ಸ್ಥಿರಾಂಕ Vector3f &feedforward_velocity_setpoint)
{
	// Interface: A valid position setpoint generates a velocity target using conservative motion constraints.
	// If a velocity is specified, that is used as a feedforward to track the position setpoint
	// (ie. it assumes the position setpoint is moving at the specified velocity)
	// If the position setpoints are set to NAN, the values in the velocity setpoints are used as velocity targets: nothing to do here.
	ಸ್ಥಿರಾಂಕ Vector3f &ಗುರಿ = waypoints[1];
	ಸ್ಥಿರಾಂಕ ತಾರ್ಕಿಕ xy_ಗುರಿ_valid = Vector2f(ಗುರಿ).isAllFinite();
	ಸ್ಥಿರಾಂಕ ತಾರ್ಕಿಕ z_ಗುರಿ_valid = PX4_ISFINITE(ಗುರಿ(2));

	Vector3f velocity_setpoint = feedforward_velocity_setpoint;

	ಆದರೆ (xy_ಗುರಿ_valid && z_ಗುರಿ_valid) {
		// Use 3D position setpoint to generate a 3D velocity setpoint
		Vector3f pos_traj(_trajectory[0].getCurrentPosition(),
				  _trajectory[1].getCurrentPosition(),
				  _trajectory[2].getCurrentPosition());
		ಸ್ಥಿರಾಂಕ Vector3f ದಾಟುವಿಕೆ_ಬಿಂದು = is_single_waypoint ? ಗುರಿ : get_ದಾಟುವಿಕೆ_ಬಿಂದು(position, waypoints);
		ಸ್ಥಿರಾಂಕ Vector3f u_pos_traj_ಗಮ್ಯ_dest{(ದಾಟುವಿಕೆ_ಬಿಂದು - pos_traj).unit_or_zero()};

		ದಶಮಾಂಶ xy_ವೇಗ = _getMaxXYSpeed(waypoints);
		ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ z_ವೇಗ = _getMaxZSpeed(waypoints);

		ಆದರೆ (!is_single_waypoint && _isTurning(ಗುರಿ)) {
			// Limit speed during a turn
			xy_ವೇಗ = math::min(max_ವೇಗ_ಹಿಂದಿನ, xy_ವೇಗ);

		} ಇಲ್ಲದಿದ್ದರೆ {
			max_ವೇಗ_ಹಿಂದಿನ = xy_ವೇಗ;
		}

		// Scale one 3D direction to fit both limits, so the slower axis limits the other
		// one and a leg that changes altitude is flown as a straight line
		Vector3f vel_sp_constrained = u_pos_traj_ಗಮ್ಯ_dest * sqrtf(xy_ವೇಗ * xy_ವೇಗ + z_ವೇಗ * z_ವೇಗ);
		math::trajectory::clamp_ಗಮ್ಯ_XYNorm(vel_sp_constrained, xy_ವೇಗ, 0.5f);
		math::trajectory::clamp_ಗಮ್ಯ_ZNorm(vel_sp_constrained, z_ವೇಗ, 0.5f);

		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < 3; i++) {
			// If available, use the existing velocity as a feedforward, otherwise replace it
			ಆದರೆ (PX4_ISFINITE(velocity_setpoint(i))) {
				velocity_setpoint(i) += vel_sp_constrained(i);

			} ಇಲ್ಲದಿದ್ದರೆ {
				velocity_setpoint(i) = vel_sp_constrained(i);
			}
		}
	}

	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (xy_ಗುರಿ_valid) {
		// Use 2D position setpoint to generate a 2D velocity setpoint

		// Get various path specific vectors
		Vector2f pos_traj(_trajectory[0].getCurrentPosition(), _trajectory[1].getCurrentPosition());
		Vector2f ದಾಟುವಿಕೆ_ಬಿಂದು = is_single_waypoint ? Vector2f(ಗುರಿ) : Vector2f(get_ದಾಟುವಿಕೆ_ಬಿಂದು(position, waypoints));
		Vector2f pos_traj_ಗಮ್ಯ_dest_xy = ದಾಟುವಿಕೆ_ಬಿಂದು - pos_traj;
		Vector2f u_pos_traj_ಗಮ್ಯ_dest_xy(pos_traj_ಗಮ್ಯ_dest_xy.unit_or_zero());

		ದಶಮಾಂಶ xy_ವೇಗ = _getMaxXYSpeed(waypoints);

		ಆದರೆ (_isTurning(ಗುರಿ)) {
			// Lock speed during turn
			xy_ವೇಗ = math::min(max_ವೇಗ_ಹಿಂದಿನ, xy_ವೇಗ);

		} ಇಲ್ಲದಿದ್ದರೆ {
			max_ವೇಗ_ಹಿಂದಿನ = xy_ವೇಗ;
		}

		Vector2f vel_sp_constrained_xy = u_pos_traj_ಗಮ್ಯ_dest_xy * xy_ವೇಗ;

		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < 2; i++) {
			// If available, use the existing velocity as a feedforward, otherwise replace it
			ಆದರೆ (PX4_ISFINITE(velocity_setpoint(i))) {
				velocity_setpoint(i) += vel_sp_constrained_xy(i);

			} ಇಲ್ಲದಿದ್ದರೆ {
				velocity_setpoint(i) = vel_sp_constrained_xy(i);
			}
		}
	}

	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (z_ಗುರಿ_valid) {
		// Use Z position setpoint to generate a Z velocity setpoint

		ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ z_dir = ಮಾತೃಕೆ::sign(ಗುರಿ(2) - _trajectory[2].getCurrentPosition());
		ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ vel_sp_z = z_dir * _getMaxZSpeed(waypoints);

		// If available, use the existing velocity as a feedforward, otherwise replace it
		ಆದರೆ (PX4_ISFINITE(velocity_setpoint(2))) {
			velocity_setpoint(2) += vel_sp_z;

		} ಇಲ್ಲದಿದ್ದರೆ {
			velocity_setpoint(2) = vel_sp_z;
		}
	}

	ಹಿಂತಿರುಗಿ velocity_setpoint;
}


ಶೂನ್ಯ PositionSmoothing::_generateTrajectory(
	ಸ್ಥಿರಾಂಕ Vector3f &position,
	ಸ್ಥಿರಾಂಕ Vector3f &velocity_setpoint,
	ದಶಮಾಂಶ delta_ಸಮಯ,
	PositionSmoothingSetpoints &ಹೊರಹೋಗು_setpoints)
{
	ಆದರೆ (!velocity_setpoint.isAllFinite()) {
		ಹಿಂತಿರುಗಿ;
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
	Vector2f drone_ಗಮ್ಯ_trajectory_xy(position_trajectory_xy - position_xy);
	ದಶಮಾಂಶ position_error_xy = drone_ಗಮ್ಯ_trajectory_xy.ಉದ್ದಳತೆ();

	ದಶಮಾಂಶ ಸಮಯ_stretch_xy = 1.f;

	// Only stretch time if there's no division by zero and the drone isn't ahead of the position setpoint
	ಆದರೆ ((_max_allowed_horizontal_error > FLT_EPSILON)
	    && drone_ಗಮ್ಯ_trajectory_xy.dot(vel_traj_xy) >= 0) {
		ಸಮಯ_stretch_xy = 1.f - math::constrain(position_error_xy / _max_allowed_horizontal_error, 0.f, 1.f);
	}

	ಆದರೆ (drone_ಗಮ್ಯ_trajectory_xy.dot(vel_traj_xy) < 0.f) {
		ಸಮಯ_stretch_xy = 1.f;
	}

	// Same idea for the vertical axis: only stretch when the virtual trajectory is leading the
	// drone AND moving further away from it. Reversing trajectories (virtual coming back toward
	// the drone) are left alone so we can recover without artificially slowing down.
	ದಶಮಾಂಶ ಸಮಯ_stretch_z = 1.f;
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ drone_ಗಮ್ಯ_traj_z = _trajectory[2].getCurrentPosition() - position(2);
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ vel_traj_z = _trajectory[2].getCurrentVelocity();

	ಆದರೆ ((_max_allowed_vertical_error > FLT_EPSILON)
	    && drone_ಗಮ್ಯ_traj_z * vel_traj_z >= 0.f) {
		ಸಮಯ_stretch_z = 1.f - math::constrain(fabsf(drone_ಗಮ್ಯ_traj_z) / _max_allowed_vertical_error, 0.f, 1.f);
	}

	// Axis 0,1 (XY) get the horizontal stretch; axis 2 (Z) gets the vertical stretch.
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ axis_stretch[3] = { ಸಮಯ_stretch_xy, ಸಮಯ_stretch_xy, ಸಮಯ_stretch_z };

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < 3; ++i) {
		_trajectory[i].updateTraj(delta_ಸಮಯ, axis_stretch[i]);
		ಹೊರಹೋಗು_setpoints.jerk(i) = _trajectory[i].getCurrentJerk();
		ಹೊರಹೋಗು_setpoints.acceleration(i) = _trajectory[i].getCurrentAcceleration();
		ಹೊರಹೋಗು_setpoints.velocity(i) = _trajectory[i].getCurrentVelocity();
		ಹೊರಹೋಗು_setpoints.position(i) = _trajectory[i].getCurrentPosition();
	}

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < 3; ++i) {
		_trajectory[i].updateDurations(velocity_setpoint(i));
	}

	VelocitySmoothing::ಸಮಯ_Synchronization(_trajectory, 3);
}
