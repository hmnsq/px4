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

/**
 * @file TrajMath.hpp
 *
 * collection of functions used for trajectory generation
 */

#pragma once

નામાકાશ math
{

નામાકાશ trajectory
{

/* Compute the maximum possible speed on the track given the desired speed,
 * remaining distance, the maximum acceleration and the maximum jerk.
 * We assume a constant acceleration profile with a delay of 2*accel/jerk
 * (time to reach the desired acceleration from opposite max acceleration)
 * Equation to solve: vel_final^2 = vel_initial^2 - 2*accel*(x - vel_initial*2*accel/jerk)
 *
 * @param jerk maximum jerk
 * @param accel maximum acceleration
 * @param braking_distance distance to the desired point
 * @param final_speed the still-remaining speed of the vehicle when it reaches the braking_distance
 *
 * @return maximum speed
 */
અંતઃરેખ દશાંશ computeMaxSpeedFromDistance(અચલ દશાંશ jerk, અચલ દશાંશ accel, અચલ દશાંશ braking_distance,
		અચલ દશાંશ final_speed)
{
	auto sqr = [](દશાંશ f) {પાછા f * f;};
	દશાંશ b =  4.0f * sqr(accel) / jerk;
	દશાંશ c = - 2.0f * accel * braking_distance - sqr(final_speed);
	દશાંશ max_speed = 0.5f * (-b + sqrtf(sqr(b) - 4.0f * c));

	// don't slow down more than the end speed, even if the conservative accel ramp time requests it
	પાછા fmaxf(max_speed, final_speed);
}

/* Compute the maximum tangential speed in a circle defined by two line segments of length "d"
 * forming a V shape, opened by an angle "alpha". The circle is tangent to the end of the
 * two segments as shown below:
 *      \\
 *      | \ d
 *      /  \
 *  __='___a\
 *      d
 *  @param alpha angle between the two line segments
 *  @param accel maximum lateral acceleration
 *  @param d length of the two line segments
 *
 *  @return maximum tangential speed
 */
અંતઃરેખ દશાંશ computeMaxSpeedInWaypoint(અચલ દશાંશ alpha, અચલ દશાંશ accel, અચલ દશાંશ d)
{
	દશાંશ tan_alpha = tanf(alpha / 2.0f);
	દશાંશ max_speed_in_turn = sqrtf(accel * d * tan_alpha);

	પાછા max_speed_in_turn;
}

/* Compute the braking distance given a maximum acceleration, maximum jerk and a maximum delay acceleration.
 * We assume a constant acceleration profile with a delay of accel_delay_max/jerk
 * (time to reach the desired acceleration from opposite max acceleration)
 * Equation to solve: vel_final^2 = vel_initial^2 - 2*accel*(x - vel_initial*2*accel/jerk)
 *
 * @param velocity initial velocity
 * @param jerk maximum jerk
 * @param accel maximum target acceleration during the braking maneuver
 * @param accel_delay_max the acceleration defining the delay described above
 *
 * @return braking distance
 */
અંતઃરેખ દશાંશ computeBrakingDistanceFromVelocity(અચલ દશાંશ velocity, અચલ દશાંશ jerk, અચલ દશાંશ accel,
		અચલ દશાંશ accel_delay_max)
{
	પાછા velocity * (velocity / (2.0f * accel) + accel_delay_max / jerk);
}

/* Compute the maximum distance between a point and a circle given a direction vector pointing from the point
 * towards the circle. The point can be inside or outside the circle.
 *                  _
 *               ,=' '=,               __
 *    P-->------/-------A   Distance = PA
 *       Dir   |    x    |
 *              \       /
 *               "=,_,="
 * Equation to solve: ||(point - circle_pos) + direction_unit * distance_to_circle|| = radius
 *
 * @param pos position of the point
 * @param circle_pos position of the center of the circle
 * @param radius radius of the circle
 * @param direction vector pointing from the point towards the circle
 *
 * @return longest distance between the point to the circle in the direction indicated by the vector or NAN if the
 * vector does not point towards the circle
 */
અંતઃરેખ દશાંશ getMaxDistanceToCircle(અચલ matrix::Vector2f &pos, અચલ matrix::Vector2f &circle_pos, દશાંશ radius,
				    અચલ matrix::Vector2f &direction)
{
	matrix::Vector2f center_to_pos = pos - circle_pos;
	અચલ દશાંશ b = 2.f * center_to_pos.dot(direction.unit_or_zero());
	અચલ દશાંશ c = center_to_pos.norm_squared() - radius * radius;
	અચલ દશાંશ delta = b * b - 4.f * c;

	દશાંશ distance_to_circle;

	જો (delta >= 0.f && direction.longerThan(0.f)) {
		distance_to_circle = fmaxf((-b + sqrtf(delta)) / 2.f, 0.f);

	} નહીંતર {
		// Never intersecting the circle
		distance_to_circle = NAN;
	}

	પાછા distance_to_circle;
}

} /* namespace traj */
} /* namespace math */
