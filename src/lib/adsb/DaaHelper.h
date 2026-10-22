/****************************************************************************
 *
 *   Copyright (c) 2026 PX4 Development Team. All rights reserved.
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
 * @file DaaHelper.h
 * @brief Shared helpers and state structs for DAA standards
 * @author Jonas Perolini <jonspero@me.com>
 */

#pragma once

#ಅನ್ತರ್ಭಾವ <cstdint>

#ಅನ್ತರ್ಭಾವ <lib/mathlib/mathlib.h>
#ಅನ್ತರ್ಭಾವ <matrix/math.hpp>

ಸಂರಚನಾ aircraft_state_s {
	matrix::Vector2d lat_lon{};
	ಭಿನ್ನ altitude{0.f};
	matrix::Vector3f velocity_ned{};
	ಭಿನ್ನ heading{0.f};
};

ಸಂರಚನಾ daa_stats_s {
	ಭಿನ್ನ aircraft_dist{0.f};
	ಭಿನ್ನ aircraft_dist_hor{0.f};
	ಭಿನ್ನ aircraft_dist_vert{0.f};
	ಭಿನ್ನ expected_min_dist_time_sec{0.f};
};

ಅನ್ತಃಸ್ಥ matrix::Vector2f calculate_horizontal_vertical_speed_magnitudes(ಧ್ರುವ aircraft_state_s &aircraft_state)
{
	ಪ್ರತ್ಯಾಗಮ matrix::Vector2f(aircraft_state.velocity_ned.xy().norm(), fabsf(aircraft_state.velocity_ned(2)));
}

// Worst-case closing speed (UAV and traffic flying straight at each other).
ಅನ್ತಃಸ್ಥ ಭಿನ್ನ calculate_relative_uav_traffic_speed(ಧ್ರುವ aircraft_state_s &uav_state, ಧ್ರುವ aircraft_state_s &traffic_state)
{
	ಪ್ರತ್ಯಾಗಮ traffic_state.velocity_ned.norm() + uav_state.velocity_ned.norm();
}
