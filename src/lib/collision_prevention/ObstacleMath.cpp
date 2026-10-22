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

#അന്തര്ഭാവ "ObstacleMath.hpp"
#അന്തര്ഭാവ <mathlib/math/Limits.hpp>

using നാമാകാശ matrix;

നാമാകാശ ObstacleMath
{

ശൂന്യം project_distance_on_horizontal_plane(ദശാംശം &distance, സ്ഥിരം ദശാംശം yaw, സ്ഥിരം matrix::Quatf &q_world_vehicle)
{
	സ്ഥിരം Quatf q_vehicle_sensor(Quatf(cosf(yaw / 2.f), 0.f, 0.f, sinf(yaw / 2.f)));
	സ്ഥിരം Quatf q_world_sensor = q_world_vehicle * q_vehicle_sensor;
	സ്ഥിരം Vector3f forward(1.f, 0.f, 0.f);
	സ്ഥിരം Vector3f sensor_direction_in_world = q_world_sensor.rotateVector(forward);

	ദശാംശം horizontal_projection_scale = sensor_direction_in_world.xy().norm();
	horizontal_projection_scale = math::constrain(horizontal_projection_scale, FLT_EPSILON, 1.0f);
	distance *= horizontal_projection_scale;
}

പൂർണ്ണസംഖ്യ get_bin_at_angle(ദശാംശം bin_width, ദശാംശം angle)
{
	പൂർണ്ണസംഖ്യ bin_at_angle = (പൂർണ്ണസംഖ്യ)round(matrix::wrap(angle, 0.f, 360.f) / bin_width);
	തിരികെ wrap_bin(bin_at_angle, 360 / bin_width);
}

ദശാംശം get_lower_bound_angle(പൂർണ്ണസംഖ്യ bin, ദശാംശം bin_width, ദശാംശം angle_offset)
{
	bin = wrap_bin(bin, 360 / bin_width);
	തിരികെ wrap_360(bin * bin_width + angle_offset - bin_width / 2.f);
}

പൂർണ്ണസംഖ്യ get_offset_bin_index(പൂർണ്ണസംഖ്യ bin, ദശാംശം bin_width, ദശാംശം angle_offset)
{
	പൂർണ്ണസംഖ്യ offset = get_bin_at_angle(bin_width, angle_offset);
	തിരികെ wrap_bin(bin - offset, 360 / bin_width);
}

ദശാംശം sensor_orientation_to_yaw_offset(സ്ഥിരം SensorOrientation orientation, സ്ഥിരം ദശാംശം q[4])
{
	ദശാംശം offset = 0.0f;

	തിരഞ്ഞെടുപ്പ് (orientation) {
	അവസ്ഥ SensorOrientation::ROTATION_YAW_0:
		offset = 0.0f;
		നിർത്തുക;

	അവസ്ഥ SensorOrientation::ROTATION_YAW_45:
		offset = M_PI_F / 4.0f;
		നിർത്തുക;

	അവസ്ഥ SensorOrientation::ROTATION_YAW_90:
		offset = M_PI_F / 2.0f;
		നിർത്തുക;

	അവസ്ഥ SensorOrientation::ROTATION_YAW_135:
		offset = 3.0f * M_PI_F / 4.0f;
		നിർത്തുക;

	അവസ്ഥ SensorOrientation::ROTATION_YAW_180:
		offset = M_PI_F;
		നിർത്തുക;

	അവസ്ഥ SensorOrientation::ROTATION_YAW_225:
		offset = -3.0f * M_PI_F / 4.0f;
		നിർത്തുക;

	അവസ്ഥ SensorOrientation::ROTATION_YAW_270:
		offset = -M_PI_F / 2.0f;
		നിർത്തുക;

	അവസ്ഥ SensorOrientation::ROTATION_YAW_315:
		offset = -M_PI_F / 4.0f;
		നിർത്തുക;

	അവസ്ഥ SensorOrientation::ROTATION_CUSTOM:
		എങ്കിൽ (q != nullptr) {
			offset = Eulerf(Quatf(q)).psi();
		}

		നിർത്തുക;
	}

	തിരികെ offset;
}

പൂർണ്ണസംഖ്യ wrap_bin(പൂർണ്ണസംഖ്യ bin, പൂർണ്ണസംഖ്യ bin_എണ്ണം)
{
	തിരികെ (bin + bin_എണ്ണം) % bin_എണ്ണം;
}

ദശാംശം wrap_360(സ്ഥിരം ദശാംശം angle)
{
	തിരികെ matrix::wrap(angle, 0.0f, 360.0f);
}

} // ObstacleMath
