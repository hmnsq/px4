/****************************************************************************
 *
 * Copyright (c) 2025 PX4 Development Team. All rights reserved.
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

#અન્તર્ભાવ "AirspeedDirectionController.hpp"
#અન્તર્ભાવ <matrix/math.hpp>
#અન્તર્ભાવ <lib/mathlib/mathlib.h>

using matrix::Vector2f;
AirspeedDirectionController::AirspeedDirectionController()
{
	// Constructor
}

દશાંશ AirspeedDirectionController::controlHeading(અચલ દશાંશ heading_sp, અચલ દશાંશ heading,
		અચલ દશાંશ airspeed) અચલ
{

	અચલ Vector2f airspeed_vector = Vector2f{cosf(heading), sinf(heading)} * airspeed;
	અચલ Vector2f airspeed_sp_vector_unit = Vector2f{cosf(heading_sp), sinf(heading_sp)};

	અચલ દશાંશ dot_air_vel_err = airspeed_vector.dot(airspeed_sp_vector_unit);
	અચલ દશાંશ cross_air_vel_err = airspeed_vector.cross(airspeed_sp_vector_unit);

	જો (dot_air_vel_err < 0.0f) {
		// hold max lateral acceleration command above 90 deg heading error
		પાછા p_gain_ * ((cross_air_vel_err < 0.0f) ? -airspeed : airspeed);

	} નહીંતર {
		// airspeed/airspeed_ref is used to scale any incremented airspeed reference back to the current airspeed
		// for acceleration commands in a "feedback" sense (i.e. at the current vehicle airspeed)
		// todo use airspeed_ref or adapt comment
		પાછા p_gain_ * cross_air_vel_err;
	}
}
