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

#ಅನ್ತರ್ಭಾವ "ObstacleMath.hpp"
#ಅನ್ತರ್ಭಾವ <mathlib/math/Limits.hpp>

using ನಾಮಾಕಾಶ matrix;

ನಾಮಾಕಾಶ ObstacleMath
{

ಶೂನ್ಯ project_distance_on_horizontal_plane(ಭಿನ್ನ &distance, ಧ್ರುವ ಭಿನ್ನ yaw, ಧ್ರುವ matrix::Quatf &q_world_vehicle)
{
	ಧ್ರುವ Quatf q_vehicle_sensor(Quatf(cosf(yaw / 2.f), 0.f, 0.f, sinf(yaw / 2.f)));
	ಧ್ರುವ Quatf q_world_sensor = q_world_vehicle * q_vehicle_sensor;
	ಧ್ರುವ Vector3f forward(1.f, 0.f, 0.f);
	ಧ್ರುವ Vector3f sensor_direction_in_world = q_world_sensor.rotateVector(forward);

	ಭಿನ್ನ horizontal_projection_scale = sensor_direction_in_world.xy().norm();
	horizontal_projection_scale = math::constrain(horizontal_projection_scale, FLT_EPSILON, 1.0f);
	distance *= horizontal_projection_scale;
}

ಪೂರ್ಣಾಙ್ಕ get_bin_at_angle(ಭಿನ್ನ bin_width, ಭಿನ್ನ angle)
{
	ಪೂರ್ಣಾಙ್ಕ bin_at_angle = (ಪೂರ್ಣಾಙ್ಕ)round(matrix::wrap(angle, 0.f, 360.f) / bin_width);
	ಪ್ರತ್ಯಾಗಮ wrap_bin(bin_at_angle, 360 / bin_width);
}

ಭಿನ್ನ get_lower_bound_angle(ಪೂರ್ಣಾಙ್ಕ bin, ಭಿನ್ನ bin_width, ಭಿನ್ನ angle_offset)
{
	bin = wrap_bin(bin, 360 / bin_width);
	ಪ್ರತ್ಯಾಗಮ wrap_360(bin * bin_width + angle_offset - bin_width / 2.f);
}

ಪೂರ್ಣಾಙ್ಕ get_offset_bin_index(ಪೂರ್ಣಾಙ್ಕ bin, ಭಿನ್ನ bin_width, ಭಿನ್ನ angle_offset)
{
	ಪೂರ್ಣಾಙ್ಕ offset = get_bin_at_angle(bin_width, angle_offset);
	ಪ್ರತ್ಯಾಗಮ wrap_bin(bin - offset, 360 / bin_width);
}

ಭಿನ್ನ sensor_orientation_to_yaw_offset(ಧ್ರುವ SensorOrientation orientation, ಧ್ರುವ ಭಿನ್ನ q[4])
{
	ಭಿನ್ನ offset = 0.0f;

	ವಿಕಲ್ಪ (orientation) {
	ಪಕ್ಷ SensorOrientation::ROTATION_YAW_0:
		offset = 0.0f;
		ಭಙ್ಗ;

	ಪಕ್ಷ SensorOrientation::ROTATION_YAW_45:
		offset = M_PI_F / 4.0f;
		ಭಙ್ಗ;

	ಪಕ್ಷ SensorOrientation::ROTATION_YAW_90:
		offset = M_PI_F / 2.0f;
		ಭಙ್ಗ;

	ಪಕ್ಷ SensorOrientation::ROTATION_YAW_135:
		offset = 3.0f * M_PI_F / 4.0f;
		ಭಙ್ಗ;

	ಪಕ್ಷ SensorOrientation::ROTATION_YAW_180:
		offset = M_PI_F;
		ಭಙ್ಗ;

	ಪಕ್ಷ SensorOrientation::ROTATION_YAW_225:
		offset = -3.0f * M_PI_F / 4.0f;
		ಭಙ್ಗ;

	ಪಕ್ಷ SensorOrientation::ROTATION_YAW_270:
		offset = -M_PI_F / 2.0f;
		ಭಙ್ಗ;

	ಪಕ್ಷ SensorOrientation::ROTATION_YAW_315:
		offset = -M_PI_F / 4.0f;
		ಭಙ್ಗ;

	ಪಕ್ಷ SensorOrientation::ROTATION_CUSTOM:
		ಯದಿ (q != nullptr) {
			offset = Eulerf(Quatf(q)).psi();
		}

		ಭಙ್ಗ;
	}

	ಪ್ರತ್ಯಾಗಮ offset;
}

ಪೂರ್ಣಾಙ್ಕ wrap_bin(ಪೂರ್ಣಾಙ್ಕ bin, ಪೂರ್ಣಾಙ್ಕ bin_ಎಣಿಕೆ)
{
	ಪ್ರತ್ಯಾಗಮ (bin + bin_ಎಣಿಕೆ) % bin_ಎಣಿಕೆ;
}

ಭಿನ್ನ wrap_360(ಧ್ರುವ ಭಿನ್ನ angle)
{
	ಪ್ರತ್ಯಾಗಮ matrix::wrap(angle, 0.0f, 360.0f);
}

} // ObstacleMath
