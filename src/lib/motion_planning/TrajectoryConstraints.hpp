/****************************************************************************
 *
 *   Copyright (c) 2019 PX4 Development Team. All rights reserved.
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

#رهنمود once

#ضمن <px4_defines.h>

#ضمن <matrix/math.hpp>
#ضمن <mathlib/mathlib.h>

نطاق math
{
نطاق trajectory
{
استفاده ماتریس::Vector3f;
استفاده ماتریس::Vector2f;

بنية VehicleDynamicLimits {
	عائم z_accept_rad;
	عائم xy_accept_rad;

	عائم max_acc_xy;
	عائم max_jerk;

	عائم max_سرعت_xy;

	// TODO: remove this
	عائم max_acc_xy_radius_scale;
};

/*
 * Compute the maximum allowed speed at the waypoint assuming that we want to
 * connect the two lines (current-target and target-next)
 * with a tangent circle with constant speed and desired centripetal acceleration: a_centripetal = speed^2 / radius
 * The circle should in theory start and end at the intersection of the lines and the waypoint's acceptance radius.
 * This is not exactly true in reality since Navigator switches the waypoint so we have to take in account that
 * the real acceptance radius is smaller.
 *
 */
مضمن عائم compute_شروع_XYSpeed_From_Waypoints(ثابت Vector3f &شروع_position, ثابت Vector3f &هدف,
		ثابت Vector3f &بعدی_هدف, عائم exit_سرعت, ثابت VehicleDynamicLimits &config)
{
	ثابت عائم فاصله_هدف_بعدی = (هدف - بعدی_هدف).xy().norm();

	ثابت منطقي هدف_بعدی_different = فاصله_هدف_بعدی  > 0.001f;
	ثابت منطقي waypoint_overlap = فاصله_هدف_بعدی < config.xy_accept_rad;

	عائم سرعت_at_هدف = 0.0f;

	إذا (هدف_بعدی_different &&
	    !waypoint_overlap
	   ) {
		// rounding can push the dot product of two unit vectors past 1, which acosf turns into NaN
		ثابت عائم cos_alpha = constrain(Vector2f((هدف - شروع_position).xy()).unit_or_zero().dot(
				Vector2f((هدف - بعدی_هدف).xy()).unit_or_zero()), -1.f, 1.f);
		ثابت عائم alpha = acosf(cos_alpha);
		ثابت عائم امن_alpha = constrain(alpha, 0.f, M_PI_F - FLT_EPSILON);
		عائم accel_tmp = config.max_acc_xy_radius_scale * config.max_acc_xy;
		عائم max_سرعت_درون_turn = compute_Max_سرعت_درون_Waypoint(امن_alpha, accel_tmp, config.xy_accept_rad);
		سرعت_at_هدف = min(max_سرعت_درون_turn, exit_سرعت, config.max_سرعت_xy);
	}

	عائم شروع_مقصد_هدف = (شروع_position - هدف).xy().norm();
	عائم max_سرعت = compute_Max_سرعت_From_فاصله(config.max_jerk, config.max_acc_xy, شروع_مقصد_هدف, سرعت_at_هدف);

	أرجع min(config.max_سرعت_xy, max_سرعت);
}

/*
 * This function computes the maximum speed XY that can be travelled, given a set of waypoints and vehicle dynamics
 *
 * The first waypoint should be the starting location, and the later waypoints the desired points to be followed.
 *
 * @param waypoints the list of waypoints to be followed, the first of which should be the starting location
 * @param config the vehicle dynamic limits
 *
 * @return the maximum speed at waypoint[0] which allows it to follow the trajectory while respecting the dynamic limits
 */
قالب <صحيح N>
عائم computeXYSpeedFromWaypoints(ثابت Vector3f waypoints[N], ثابت VehicleDynamicLimits &config)
{
	static_assert(N >= 2, "Need at least 2 points to compute speed");

	عائم max_سرعت = 0.f;

	// go backwards through the waypoints
	لكل (صحيح i = (N - 2); i >= 0; i--) {
		max_سرعت = compute_شروع_XYSpeed_From_Waypoints(waypoints[i],
				waypoints[i + 1],
				waypoints[min(i + 2, N - 1)],
				max_سرعت, config);
	}

	أرجع max_سرعت;
}

/*
 * Constrain the 3D vector given a maximum XY norm
 * If the XY norm of the 3D vector is larger than the maximum norm, the whole vector
 * is scaled down to respect the constraint.
 * If the maximum norm is small (defined by the "accuracy" parameter),
 * only the XY components are scaled down to avoid affecting
 * Z in case of numerical issues
 */
مضمن فراغ clamp_مقصد_XYNorm(Vector3f &هدف, عائم max_xy_norm, عائم accuracy = FLT_EPSILON)
{
	ثابت عائم xynorm = هدف.xy().norm();
	ثابت عائم scale_ضریب = (xynorm > FLT_EPSILON)
				   ? max_xy_norm / xynorm
				   : 1.f;

	إذا (scale_ضریب < 1.f) {
		إذا (max_xy_norm < accuracy && xynorm < accuracy) {
			هدف.xy() = Vector2f(هدف) * scale_ضریب;

		} وإلا {
			هدف *= scale_ضریب;
		}
	}
}

/*
 * Constrain the 3D vector given a maximum Z norm
 * If the Z component of the 3D vector is larger than the maximum norm, the whole vector
 * is scaled down to respect the constraint.
 * If the maximum norm is small (defined by the "accuracy parameter),
 * only the Z component is scaled down to avoid affecting
 * XY in case of numerical issues
 */
مضمن فراغ clamp_مقصد_ZNorm(Vector3f &هدف, عائم max_z_norm, عائم accuracy = FLT_EPSILON)
{
	ثابت عائم znorm = fabs(هدف(2));
	ثابت عائم scale_ضریب = (znorm > FLT_EPSILON)
				   ? max_z_norm / znorm
				   : 1.f;

	إذا (scale_ضریب < 1.f) {
		إذا (max_z_norm < accuracy && znorm < accuracy) {
			هدف(2) *= scale_ضریب;

		} وإلا {
			هدف *= scale_ضریب;
		}
	}
}

}
}
