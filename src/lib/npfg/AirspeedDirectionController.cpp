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

#అన్తర్భావ "AirspeedDirectionController.hpp"
#అన్తర్భావ <matrix/math.hpp>
#అన్తర్భావ <lib/mathlib/mathlib.h>

ఉపయోగించి మాతృక::Vector2f;
AirspeedDirectionController::AirspeedDirectionController()
{
	// Constructor
}

దశాంశం AirspeedDirectionController::controlHeading(స్థిరరాశి దశాంశం heading_sp, స్థిరరాశి దశాంశం heading,
		స్థిరరాశి దశాంశం airspeed) స్థిరరాశి
{

	స్థిరరాశి Vector2f airspeed_సదిశ = Vector2f{cosf(heading), sinf(heading)} * airspeed;
	స్థిరరాశి Vector2f airspeed_sp_సదిశ_unit = Vector2f{cosf(heading_sp), sinf(heading_sp)};

	స్థిరరాశి దశాంశం dot_air_vel_err = airspeed_సదిశ.dot(airspeed_sp_సదిశ_unit);
	స్థిరరాశి దశాంశం cross_air_vel_err = airspeed_సదిశ.cross(airspeed_sp_సదిశ_unit);

	ఒకవేళ (dot_air_vel_err < 0.0f) {
		// hold max lateral acceleration command above 90 deg heading error
		తిరిగి p_gain_ * ((cross_air_vel_err < 0.0f) ? -airspeed : airspeed);

	} లేకపోతే {
		// airspeed/airspeed_ref is used to scale any incremented airspeed reference back to the current airspeed
		// for acceleration commands in a "feedback" sense (i.e. at the current vehicle airspeed)
		// todo use airspeed_ref or adapt comment
		తిరిగి p_gain_ * cross_air_vel_err;
	}
}
