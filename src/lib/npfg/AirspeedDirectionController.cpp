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

#अन्तर्भाव "AirspeedDirectionController.hpp"
#अन्तर्भाव <matrix/math.hpp>
#अन्तर्भाव <lib/mathlib/mathlib.h>

उपयुज्य matrix::Vector2f;
AirspeedDirectionController::AirspeedDirectionController()
{
	// Constructor
}

भिन्न AirspeedDirectionController::controlHeading(ध्रुव भिन्न heading_sp, ध्रुव भिन्न heading,
		ध्रुव भिन्न airspeed) ध्रुव
{

	ध्रुव Vector2f airspeed_सदिशम् = Vector2f{cosf(heading), sinf(heading)} * airspeed;
	ध्रुव Vector2f airspeed_sp_सदिशम्_unit = Vector2f{cosf(heading_sp), sinf(heading_sp)};

	ध्रुव भिन्न dot_air_vel_err = airspeed_सदिशम्.dot(airspeed_sp_सदिशम्_unit);
	ध्रुव भिन्न cross_air_vel_err = airspeed_सदिशम्.cross(airspeed_sp_सदिशम्_unit);

	यदि (dot_air_vel_err < 0.0f) {
		// hold max lateral acceleration command above 90 deg heading error
		प्रत्यागम p_gain_ * ((cross_air_vel_err < 0.0f) ? -airspeed : airspeed);

	} अन्यथा {
		// airspeed/airspeed_ref is used to scale any incremented airspeed reference back to the current airspeed
		// for acceleration commands in a "feedback" sense (i.e. at the current vehicle airspeed)
		// todo use airspeed_ref or adapt comment
		प्रत्यागम p_gain_ * cross_air_vel_err;
	}
}
