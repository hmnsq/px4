/****************************************************************************
 *
 *   Copyright (c) 2024 PX4 Development Team. All rights reserved.
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

#ضمن "PurePursuit.hpp"
استفاده نطاق ماتریس;
نطاق خالص_Pursuit
{
عائم calc_هدف_Bearing(خالص_pursuit_وضعیت_s &خالص_pursuit_وضعیت, ثابت عائم lookahead_gain,
			ثابت عائم lookahead_max, ثابت عائم lookahead_min, ثابت Vector2f &curr_wp_ned, ثابت Vector2f &prev_wp_ned,
			ثابت Vector2f &curr_pos_ned, ثابت عائم vehicle_سرعت)
{
	// Check input validity
	إذا (!curr_wp_ned.isAllFinite() || !curr_pos_ned.isAllFinite() || !PX4_ISFINITE(vehicle_سرعت)
	    || !prev_wp_ned.isAllFinite()) {
		أرجع NAN;
	}

	ثابت عائم lookahead_فاصله = math::constrain(lookahead_gain * fabsf(vehicle_سرعت), lookahead_min, lookahead_max);
	ثابت Vector2f curr_pos_مقصد_curr_wp = curr_wp_ned - curr_pos_ned;
	ثابت Vector2f prev_wp_مقصد_curr_wp = curr_wp_ned - prev_wp_ned;
	ثابت Vector2f prev_wp_مقصد_curr_pos = curr_pos_ned - prev_wp_ned;
	ثابت Vector2f prev_wp_مقصد_curr_wp_u = prev_wp_مقصد_curr_wp.unit_or_zero();
	ثابت عائم فاصله_along_مسیر = prev_wp_مقصد_curr_pos *
					  prev_wp_مقصد_curr_wp_u; // Signed distance of the current position along the path, from the previous waypoint
	ثابت Vector2f position_along_مسیر = فاصله_along_مسیر *
					     prev_wp_مقصد_curr_wp_u; // Projection of prev_wp_to_curr_pos onto prev_wp_to_curr_wp
	ثابت Vector2f curr_pos_مقصد_مسیر = position_along_مسیر -
					  prev_wp_مقصد_curr_pos; // Shortest vector from the current position to the path
	ثابت عائم crosstrack_error = sign(prev_wp_مقصد_curr_wp(1) * curr_pos_مقصد_مسیر(
			0) - prev_wp_مقصد_curr_wp(0) * curr_pos_مقصد_مسیر(1)) * curr_pos_مقصد_مسیر.norm();
	ثابت عائم bearing_مقصد_curr_waypoint = ماتریس::wrap_pi(atan2f(curr_pos_مقصد_curr_wp(1), curr_pos_مقصد_curr_wp(0)));
	عائم هدف_bearing{NAN};

	إذا (curr_pos_مقصد_curr_wp.norm() < lookahead_فاصله
	    || prev_wp_مقصد_curr_wp.norm() < FLT_EPSILON
	    || فاصله_along_مسیر > prev_wp_مقصد_curr_wp.norm()) {
		// Target current waypoint if closer to it than lookahead, if the waypoints overlap, or if the vehicle has passed
		// the current waypoint: the lookahead intersection would otherwise lie on the extension of the path beyond it
		// and lead the vehicle further away
		هدف_bearing = bearing_مقصد_curr_waypoint;

	} وإلا إذا (fabsf(crosstrack_error) >
		   lookahead_فاصله) { // Path segment is outside of lookahead (No intersection point)

		ثابت Vector2f prev_wp_مقصد_closest_نقطه_on_مسیر = curr_pos_مقصد_مسیر + prev_wp_مقصد_curr_pos;
		ثابت Vector2f curr_wp_مقصد_closest_نقطه_on_مسیر = curr_pos_مقصد_مسیر - curr_pos_مقصد_curr_wp;

		إذا (prev_wp_مقصد_closest_نقطه_on_مسیر * prev_wp_مقصد_curr_wp <
		    FLT_EPSILON) { // Target previous waypoint if closest point is on the the extended path segment "behind" previous waypoint
			هدف_bearing = ماتریس::wrap_pi(atan2f(-prev_wp_مقصد_curr_pos(1), -prev_wp_مقصد_curr_pos(0)));

		} وإلا إذا (curr_wp_مقصد_closest_نقطه_on_مسیر * prev_wp_مقصد_curr_wp >
			   FLT_EPSILON) { // Target current waypoint if closest point is on the extended path segment "ahead" of current waypoint
			هدف_bearing = bearing_مقصد_curr_waypoint;

		} وإلا { // Target closest point on path
			هدف_bearing = ماتریس::wrap_pi(atan2f(curr_pos_مقصد_مسیر(1), curr_pos_مقصد_مسیر(0)));
		}


	} وإلا { // Regular pure pursuit
		ثابت عائم سطر_extension = جذر(powf(lookahead_فاصله, 2.f) - powf(curr_pos_مقصد_مسیر.norm(),
						  2.f)); // Length of the vector from the endpoint of distance_on_line_segment to the intersection point
		ثابت Vector2f prev_wp_مقصد_intersection_نقطه = position_along_مسیر + سطر_extension *
				prev_wp_مقصد_curr_wp_u;
		ثابت Vector2f curr_pos_مقصد_intersection_نقطه = prev_wp_مقصد_intersection_نقطه - prev_wp_مقصد_curr_pos;
		هدف_bearing = ماتریس::wrap_pi(atan2f(curr_pos_مقصد_intersection_نقطه(1), curr_pos_مقصد_intersection_نقطه(0)));
	}


	خالص_pursuit_وضعیت.lookahead_فاصله = lookahead_فاصله;
	خالص_pursuit_وضعیت.هدف_bearing = هدف_bearing;
	خالص_pursuit_وضعیت.crosstrack_error = crosstrack_error;
	خالص_pursuit_وضعیت.فاصله_مقصد_waypoint = curr_pos_مقصد_curr_wp.norm();
	خالص_pursuit_وضعیت.bearing_مقصد_waypoint = bearing_مقصد_curr_waypoint;
	أرجع هدف_bearing;

}
} // Pure Pursuit
