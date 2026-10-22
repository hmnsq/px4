/****************************************************************************
 *
 *   Copyright (c) 2025 PX4 Development Team. All rights reserved.
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

#ضمن "RoverControl.hpp"
استفاده نطاق ماتریس;
نطاق RoverControl
{
عائم throttleControl(Slew_نرخ<عائم> &motor_setpoint, ثابت عائم throttle_setpoint,
		      ثابت عائم current_motor_setpoint, ثابت عائم max_accel, ثابت عائم max_decel, ثابت عائم max_thr_spd,
		      ثابت عائم dt)
{
	// Sanitize inputs
	إذا (!PX4_ISFINITE(throttle_setpoint) || !PX4_ISFINITE(current_motor_setpoint) || !PX4_ISFINITE(dt)) {
		أرجع NAN;
	}

	ثابت منطقي accelerating = fabsf(throttle_setpoint) > fabsf(current_motor_setpoint);

	إذا (accelerating && max_accel > FLT_EPSILON && max_thr_spd > FLT_EPSILON) { // Acceleration slew rate
		motor_setpoint.مجموعه_Slew_نرخ(max_accel / max_thr_spd);
		motor_setpoint.update(throttle_setpoint, dt);

		// Reinitialize slew rate if current value is closer to setpoint than post slew rate value
		إذا (fabsf(motor_setpoint.getState() - current_motor_setpoint) > fabsf(throttle_setpoint -
				current_motor_setpoint)) {
			motor_setpoint.مجموعه_Forced_مقدار(throttle_setpoint);
		}

	} وإلا إذا (!accelerating && max_decel > FLT_EPSILON && max_thr_spd > FLT_EPSILON) { // Deceleration slew rate
		motor_setpoint.مجموعه_Slew_نرخ(max_decel / max_thr_spd);
		motor_setpoint.update(throttle_setpoint, dt);

		// Reinitialize slew rate if current value is closer to setpoint than post slew rate value
		إذا (fabsf(motor_setpoint.getState() - current_motor_setpoint) > fabsf(throttle_setpoint -
				current_motor_setpoint)) {
			motor_setpoint.مجموعه_Forced_مقدار(throttle_setpoint);
		}

	} وإلا { // Fallthrough if slew rate parameters are not configured
		motor_setpoint.مجموعه_Forced_مقدار(throttle_setpoint);
	}

	أرجع motor_setpoint.getState();
}

عائم attitudeControl(Slew_نرخ_Yaw<عائم> &adjusted_yaw_setpoint, PID &pid_yaw,
		      ثابت عائم yaw_slew_نرخ, عائم vehicle_yaw, عائم yaw_setpoint, ثابت عائم dt)
{
	// Sanitize inputs
	إذا (!PX4_ISFINITE(yaw_setpoint) || !PX4_ISFINITE(vehicle_yaw) || !PX4_ISFINITE(dt)) {
		أرجع NAN;
	}

	yaw_setpoint = wrap_pi(yaw_setpoint);
	vehicle_yaw = wrap_pi(vehicle_yaw);

	إذا (yaw_slew_نرخ > FLT_EPSILON) { // Apply slew rate if configured
		adjusted_yaw_setpoint.مجموعه_Slew_نرخ(yaw_slew_نرخ);
		adjusted_yaw_setpoint.update(yaw_setpoint, dt);

		// Reinitialize slew rate if current value is closer to setpoint than post slew rate value
		إذا (fabsf(wrap_pi(adjusted_yaw_setpoint.getState() - vehicle_yaw)) > fabsf(wrap_pi(yaw_setpoint - vehicle_yaw))) {
			adjusted_yaw_setpoint.مجموعه_Forced_مقدار(yaw_setpoint);
		}

	} وإلا {
		adjusted_yaw_setpoint.مجموعه_Forced_مقدار(yaw_setpoint);
	}

	// Feedback control
	pid_yaw.مجموعه_Setpoint(
		wrap_pi(adjusted_yaw_setpoint.getState() - vehicle_yaw));  // Error as setpoint to take care of wrapping
	أرجع pid_yaw.update(0.f, dt);

}

عائم سرعت_Control(Slew_نرخ<عائم> &سرعت_with_نرخ_حد, PID &pid_سرعت, ثابت عائم سرعت_setpoint,
		   ثابت عائم vehicle_سرعت, ثابت عائم max_accel, ثابت عائم max_decel, ثابت عائم max_thr_سرعت, ثابت عائم dt)
{
	// Sanitize inputs
	إذا (!PX4_ISFINITE(سرعت_setpoint) || !PX4_ISFINITE(vehicle_سرعت) || !PX4_ISFINITE(dt)) {
		أرجع NAN;
	}

	// Apply acceleration and deceleration limit
	إذا (fabsf(سرعت_setpoint) >= fabsf(vehicle_سرعت) && max_accel > FLT_EPSILON) {
		سرعت_with_نرخ_حد.مجموعه_Slew_نرخ(max_accel);
		سرعت_with_نرخ_حد.update(سرعت_setpoint, dt);

		// Reinitialize slew rate if current value is closer to setpoint than post slew rate value
		إذا (fabsf(سرعت_with_نرخ_حد.getState() - vehicle_سرعت) > fabsf(
			    سرعت_setpoint - vehicle_سرعت)) {
			سرعت_with_نرخ_حد.مجموعه_Forced_مقدار(سرعت_setpoint);
		}

	} وإلا إذا (fabsf(سرعت_setpoint) < fabsf(vehicle_سرعت) && max_decel > FLT_EPSILON) {
		سرعت_with_نرخ_حد.مجموعه_Slew_نرخ(max_decel);
		سرعت_with_نرخ_حد.update(سرعت_setpoint, dt);

		// Reinitialize slew rate if current value is closer to setpoint than post slew rate value
		إذا (fabsf(سرعت_with_نرخ_حد.getState() - vehicle_سرعت) > fabsf(
			    سرعت_setpoint - vehicle_سرعت)) {
			سرعت_with_نرخ_حد.مجموعه_Forced_مقدار(سرعت_setpoint);
		}

	} وإلا { // Fallthrough if slew rate is not configured
		سرعت_with_نرخ_حد.مجموعه_Forced_مقدار(سرعت_setpoint);
	}

	// Calculate normalized forward speed setpoint
	عائم forward_سرعت_normalized{0.f};

	// Feedforward
	إذا (max_thr_سرعت > FLT_EPSILON) {
		forward_سرعت_normalized = math::interpolate<عائم>(سرعت_with_نرخ_حد.getState(),
					   -max_thr_سرعت, max_thr_سرعت,
					   -1.f, 1.f);
	}

	// Feedback control
	إذا (fabsf(سرعت_with_نرخ_حد.getState()) > FLT_EPSILON) {
		pid_سرعت.مجموعه_Setpoint(سرعت_with_نرخ_حد.getState());
		forward_سرعت_normalized += pid_سرعت.update(vehicle_سرعت, dt);

	} وإلا {
		pid_سرعت.resetIntegral();
	}


	أرجع math::constrain(forward_سرعت_normalized, -1.f, 1.f);
}

عائم نرخ_Control_2(Slew_نرخ<عائم> &adjusted_yaw_نرخ_setpoint, PID &pid_yaw_نرخ, ثابت عائم yaw_نرخ_setpoint,
		  ثابت عائم vehicle_yaw_نرخ, ثابت عائم max_thr_سرعت, ثابت عائم yaw_نرخ_corr, ثابت عائم max_yaw_accel,
		  ثابت عائم max_yaw_decel,
		  ثابت عائم wheel_track, ثابت عائم dt)
{
	// Apply acceleration and deceleration limit
	إذا (fabsf(yaw_نرخ_setpoint) >= fabsf(vehicle_yaw_نرخ) && max_yaw_accel > FLT_EPSILON) {
		adjusted_yaw_نرخ_setpoint.مجموعه_Slew_نرخ(max_yaw_accel);
		adjusted_yaw_نرخ_setpoint.update(yaw_نرخ_setpoint, dt);

		// Reinitialize slew rate if current value is closer to setpoint than post slew rate value
		إذا (fabsf(adjusted_yaw_نرخ_setpoint.getState() - vehicle_yaw_نرخ) > fabsf(
			    yaw_نرخ_setpoint - vehicle_yaw_نرخ)) {
			adjusted_yaw_نرخ_setpoint.مجموعه_Forced_مقدار(yaw_نرخ_setpoint);
		}


	} وإلا إذا (fabsf(yaw_نرخ_setpoint) < fabsf(vehicle_yaw_نرخ) && max_yaw_decel > FLT_EPSILON) {
		adjusted_yaw_نرخ_setpoint.مجموعه_Slew_نرخ(max_yaw_decel);
		adjusted_yaw_نرخ_setpoint.update(yaw_نرخ_setpoint, dt);

		// Reinitialize slew rate if current value is closer to setpoint than post slew rate value
		إذا (fabsf(adjusted_yaw_نرخ_setpoint.getState() - vehicle_yaw_نرخ) > fabsf(
			    yaw_نرخ_setpoint - vehicle_yaw_نرخ)) {
			adjusted_yaw_نرخ_setpoint.مجموعه_Forced_مقدار(yaw_نرخ_setpoint);
		}


	} وإلا { // Fallthrough if slew rate is not configured
		adjusted_yaw_نرخ_setpoint.مجموعه_Forced_مقدار(yaw_نرخ_setpoint);
	}

	// Transform yaw rate into speed difference
	عائم سرعت_diff_normalized{0.f};

	إذا (wheel_track > FLT_EPSILON && max_thr_سرعت > FLT_EPSILON) { // Feedforward
		ثابت عائم سرعت_diff = (adjusted_yaw_نرخ_setpoint.getState() * wheel_track /
					  2.f) * yaw_نرخ_corr;
		سرعت_diff_normalized = math::interpolate<عائم>(سرعت_diff, -max_thr_سرعت,
					max_thr_سرعت, -1.f, 1.f);
	}

	// Feedback control
	إذا (fabsf(adjusted_yaw_نرخ_setpoint.getState()) > FLT_EPSILON) {
		pid_yaw_نرخ.مجموعه_Setpoint(adjusted_yaw_نرخ_setpoint.getState());
		سرعت_diff_normalized += pid_yaw_نرخ.update(vehicle_yaw_نرخ, dt);

	} وإلا {
		pid_yaw_نرخ.resetIntegral();
	}


	أرجع math::constrain(سرعت_diff_normalized, -1.f, 1.f);
}

فراغ global_مقصد_Local_Setpoint_Triplet(Vector2f &curr_wp_ned, Vector2f &prev_wp_ned, Vector2f &بعدی_wp_ned,
				  position_setpoint_triplet_s position_setpoint_triplet, Vector2f &curr_pos_ned, MapProjection &global_ned_proj_ref)
{
	إذا (position_setpoint_triplet.current.valid && PX4_ISFINITE(position_setpoint_triplet.current.lat)
	    && PX4_ISFINITE(position_setpoint_triplet.current.lon)) {
		curr_wp_ned = global_ned_proj_ref.project(position_setpoint_triplet.current.lat, position_setpoint_triplet.current.lon);

	} وإلا {
		curr_wp_ned = curr_pos_ned.isAllFinite() ? curr_pos_ned : Vector2f(NAN, NAN); // Fallback if current waypoint is invalid
	}

	إذا (position_setpoint_triplet.قبلی.valid && PX4_ISFINITE(position_setpoint_triplet.قبلی.lat)
	    && PX4_ISFINITE(position_setpoint_triplet.قبلی.lon)) {
		prev_wp_ned = global_ned_proj_ref.project(position_setpoint_triplet.قبلی.lat,
				position_setpoint_triplet.قبلی.lon);

	} وإلا {
		prev_wp_ned = curr_pos_ned.isAllFinite() ? curr_pos_ned : Vector2f(NAN,
				NAN); // Fallback if previous waypoint is invalid
	}

	إذا (position_setpoint_triplet.بعدی.valid && PX4_ISFINITE(position_setpoint_triplet.بعدی.lat)
	    && PX4_ISFINITE(position_setpoint_triplet.بعدی.lon)) {
		بعدی_wp_ned = global_ned_proj_ref.project(position_setpoint_triplet.بعدی.lat, position_setpoint_triplet.بعدی.lon);

	} وإلا {
		بعدی_wp_ned = Vector2f(NAN, NAN);
	}
}

عائم calcWaypointTransitionAngle(Vector2f &prev_wp_ned, Vector2f &curr_wp_ned, Vector2f &بعدی_wp_ned)
{
	// Sanitize inputs
	إذا (!prev_wp_ned.isAllFinite() || !curr_wp_ned.isAllFinite() || !بعدی_wp_ned.isAllFinite()) {
		أرجع NAN;
	}

	ثابت Vector2f curr_مقصد_بعدی_wp_ned = بعدی_wp_ned - curr_wp_ned;
	ثابت Vector2f curr_مقصد_prev_wp_ned = prev_wp_ned - curr_wp_ned;

	// Waypoint overlap
	إذا (curr_مقصد_بعدی_wp_ned.norm() < FLT_EPSILON || curr_مقصد_prev_wp_ned.norm() < FLT_EPSILON) {
		أرجع NAN;
	}

	عائم cosin = curr_مقصد_prev_wp_ned.unit_or_zero() * curr_مقصد_بعدی_wp_ned.unit_or_zero();
	cosin = math::constrain<عائم>(cosin, -1.f, 1.f); // Protect against float precision problem
	أرجع acosf(cosin);
}

} // RoverControl
