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

ನಾಮಾಕಾಶ math
{

ನಾಮಾಕಾಶ trajectory
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
ಅನ್ತಃಸ್ಥ ಭಿನ್ನ computeMaxSpeedFromDistance(ಧ್ರುವ ಭಿನ್ನ jerk, ಧ್ರುವ ಭಿನ್ನ accel, ಧ್ರುವ ಭಿನ್ನ braking_distance,
		ಧ್ರುವ ಭಿನ್ನ final_speed)
{
	auto sqr = [](ಭಿನ್ನ f) {ಪ್ರತ್ಯಾಗಮ f * f;};
	ಭಿನ್ನ b =  4.0f * sqr(accel) / jerk;
	ಭಿನ್ನ c = - 2.0f * accel * braking_distance - sqr(final_speed);
	ಭಿನ್ನ max_speed = 0.5f * (-b + sqrtf(sqr(b) - 4.0f * c));

	// don't slow down more than the end speed, even if the conservative accel ramp time requests it
	ಪ್ರತ್ಯಾಗಮ fmaxf(max_speed, final_speed);
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
ಅನ್ತಃಸ್ಥ ಭಿನ್ನ computeMaxSpeedInWaypoint(ಧ್ರುವ ಭಿನ್ನ alpha, ಧ್ರುವ ಭಿನ್ನ accel, ಧ್ರುವ ಭಿನ್ನ d)
{
	ಭಿನ್ನ tan_alpha = tanf(alpha / 2.0f);
	ಭಿನ್ನ max_speed_in_turn = sqrtf(accel * d * tan_alpha);

	ಪ್ರತ್ಯಾಗಮ max_speed_in_turn;
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
ಅನ್ತಃಸ್ಥ ಭಿನ್ನ computeBrakingDistanceFromVelocity(ಧ್ರುವ ಭಿನ್ನ velocity, ಧ್ರುವ ಭಿನ್ನ jerk, ಧ್ರುವ ಭಿನ್ನ accel,
		ಧ್ರುವ ಭಿನ್ನ accel_delay_max)
{
	ಪ್ರತ್ಯಾಗಮ velocity * (velocity / (2.0f * accel) + accel_delay_max / jerk);
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
ಅನ್ತಃಸ್ಥ ಭಿನ್ನ getMaxDistanceToCircle(ಧ್ರುವ matrix::Vector2f &pos, ಧ್ರುವ matrix::Vector2f &circle_pos, ಭಿನ್ನ radius,
				    ಧ್ರುವ matrix::Vector2f &direction)
{
	matrix::Vector2f center_to_pos = pos - circle_pos;
	ಧ್ರುವ ಭಿನ್ನ b = 2.f * center_to_pos.dot(direction.unit_or_zero());
	ಧ್ರುವ ಭಿನ್ನ c = center_to_pos.norm_squared() - radius * radius;
	ಧ್ರುವ ಭಿನ್ನ delta = b * b - 4.f * c;

	ಭಿನ್ನ distance_to_circle;

	ಯದಿ (delta >= 0.f && direction.longerThan(0.f)) {
		distance_to_circle = fmaxf((-b + sqrtf(delta)) / 2.f, 0.f);

	} ಅನ್ಯಥಾ {
		// Never intersecting the circle
		distance_to_circle = NAN;
	}

	ಪ್ರತ್ಯಾಗಮ distance_to_circle;
}

} /* namespace traj */
} /* namespace math */
