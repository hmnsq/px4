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

#شامل "ObstacleMath.hpp"
#شامل <mathlib/math/Limits.hpp>

using فضاینام matrix;

فضاینام ObstacleMath
{

تهی project_distance_on_horizontal_plane(اعشاری &distance, ثابت اعشاری yaw, ثابت matrix::Quatf &q_world_vehicle)
{
	ثابت Quatf q_vehicle_sensor(Quatf(cosf(yaw / 2.f), 0.f, 0.f, sinf(yaw / 2.f)));
	ثابت Quatf q_world_sensor = q_world_vehicle * q_vehicle_sensor;
	ثابت Vector3f forward(1.f, 0.f, 0.f);
	ثابت Vector3f sensor_direction_in_world = q_world_sensor.rotateVector(forward);

	اعشاری horizontal_projection_scale = sensor_direction_in_world.xy().norm();
	horizontal_projection_scale = math::constrain(horizontal_projection_scale, FLT_EPSILON, 1.0f);
	distance *= horizontal_projection_scale;
}

صحیح get_bin_at_angle(اعشاری bin_width, اعشاری angle)
{
	صحیح bin_at_angle = (صحیح)round(matrix::wrap(angle, 0.f, 360.f) / bin_width);
	بازگشت wrap_bin(bin_at_angle, 360 / bin_width);
}

اعشاری get_lower_bound_angle(صحیح bin, اعشاری bin_width, اعشاری angle_offset)
{
	bin = wrap_bin(bin, 360 / bin_width);
	بازگشت wrap_360(bin * bin_width + angle_offset - bin_width / 2.f);
}

صحیح get_offset_bin_index(صحیح bin, اعشاری bin_width, اعشاری angle_offset)
{
	صحیح offset = get_bin_at_angle(bin_width, angle_offset);
	بازگشت wrap_bin(bin - offset, 360 / bin_width);
}

اعشاری sensor_orientation_to_yaw_offset(ثابت SensorOrientation orientation, ثابت اعشاری q[4])
{
	اعشاری offset = 0.0f;

	انتخاب (orientation) {
	حالت SensorOrientation::ROTATION_YAW_0:
		offset = 0.0f;
		بشکن;

	حالت SensorOrientation::ROTATION_YAW_45:
		offset = M_PI_F / 4.0f;
		بشکن;

	حالت SensorOrientation::ROTATION_YAW_90:
		offset = M_PI_F / 2.0f;
		بشکن;

	حالت SensorOrientation::ROTATION_YAW_135:
		offset = 3.0f * M_PI_F / 4.0f;
		بشکن;

	حالت SensorOrientation::ROTATION_YAW_180:
		offset = M_PI_F;
		بشکن;

	حالت SensorOrientation::ROTATION_YAW_225:
		offset = -3.0f * M_PI_F / 4.0f;
		بشکن;

	حالت SensorOrientation::ROTATION_YAW_270:
		offset = -M_PI_F / 2.0f;
		بشکن;

	حالت SensorOrientation::ROTATION_YAW_315:
		offset = -M_PI_F / 4.0f;
		بشکن;

	حالت SensorOrientation::ROTATION_CUSTOM:
		اگر (q != nullptr) {
			offset = Eulerf(Quatf(q)).psi();
		}

		بشکن;
	}

	بازگشت offset;
}

صحیح wrap_bin(صحیح bin, صحیح bin_شمار)
{
	بازگشت (bin + bin_شمار) % bin_شمار;
}

اعشاری wrap_360(ثابت اعشاری angle)
{
	بازگشت matrix::wrap(angle, 0.0f, 360.0f);
}

} // ObstacleMath
