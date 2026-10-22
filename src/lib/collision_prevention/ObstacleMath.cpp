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

#অন্তর্ভাব "ObstacleMath.hpp"
#অন্তর্ভাব <mathlib/math/Limits.hpp>

using নামাকাশ matrix;

নামাকাশ ObstacleMath
{

শূন্য project_distance_on_horizontal_plane(ভিন্ন &distance, ধ্রুব ভিন্ন yaw, ধ্রুব matrix::Quatf &q_world_vehicle)
{
	ধ্রুব Quatf q_vehicle_sensor(Quatf(cosf(yaw / 2.f), 0.f, 0.f, sinf(yaw / 2.f)));
	ধ্রুব Quatf q_world_sensor = q_world_vehicle * q_vehicle_sensor;
	ধ্রুব Vector3f forward(1.f, 0.f, 0.f);
	ধ্রুব Vector3f sensor_direction_in_world = q_world_sensor.rotateVector(forward);

	ভিন্ন horizontal_projection_scale = sensor_direction_in_world.xy().norm();
	horizontal_projection_scale = math::constrain(horizontal_projection_scale, FLT_EPSILON, 1.0f);
	distance *= horizontal_projection_scale;
}

পূর্ণাঙ্ক get_bin_at_angle(ভিন্ন bin_width, ভিন্ন angle)
{
	পূর্ণাঙ্ক bin_at_angle = (পূর্ণাঙ্ক)round(matrix::wrap(angle, 0.f, 360.f) / bin_width);
	প্রত্যাগম wrap_bin(bin_at_angle, 360 / bin_width);
}

ভিন্ন get_lower_bound_angle(পূর্ণাঙ্ক bin, ভিন্ন bin_width, ভিন্ন angle_offset)
{
	bin = wrap_bin(bin, 360 / bin_width);
	প্রত্যাগম wrap_360(bin * bin_width + angle_offset - bin_width / 2.f);
}

পূর্ণাঙ্ক get_offset_bin_index(পূর্ণাঙ্ক bin, ভিন্ন bin_width, ভিন্ন angle_offset)
{
	পূর্ণাঙ্ক offset = get_bin_at_angle(bin_width, angle_offset);
	প্রত্যাগম wrap_bin(bin - offset, 360 / bin_width);
}

ভিন্ন sensor_orientation_to_yaw_offset(ধ্রুব SensorOrientation orientation, ধ্রুব ভিন্ন q[4])
{
	ভিন্ন offset = 0.0f;

	বিকল্প (orientation) {
	পক্ষ SensorOrientation::ROTATION_YAW_0:
		offset = 0.0f;
		ভঙ্গ;

	পক্ষ SensorOrientation::ROTATION_YAW_45:
		offset = M_PI_F / 4.0f;
		ভঙ্গ;

	পক্ষ SensorOrientation::ROTATION_YAW_90:
		offset = M_PI_F / 2.0f;
		ভঙ্গ;

	পক্ষ SensorOrientation::ROTATION_YAW_135:
		offset = 3.0f * M_PI_F / 4.0f;
		ভঙ্গ;

	পক্ষ SensorOrientation::ROTATION_YAW_180:
		offset = M_PI_F;
		ভঙ্গ;

	পক্ষ SensorOrientation::ROTATION_YAW_225:
		offset = -3.0f * M_PI_F / 4.0f;
		ভঙ্গ;

	পক্ষ SensorOrientation::ROTATION_YAW_270:
		offset = -M_PI_F / 2.0f;
		ভঙ্গ;

	পক্ষ SensorOrientation::ROTATION_YAW_315:
		offset = -M_PI_F / 4.0f;
		ভঙ্গ;

	পক্ষ SensorOrientation::ROTATION_CUSTOM:
		যদি (q != nullptr) {
			offset = Eulerf(Quatf(q)).psi();
		}

		ভঙ্গ;
	}

	প্রত্যাগম offset;
}

পূর্ণাঙ্ক wrap_bin(পূর্ণাঙ্ক bin, পূর্ণাঙ্ক bin_গুনতি)
{
	প্রত্যাগম (bin + bin_গুনতি) % bin_গুনতি;
}

ভিন্ন wrap_360(ধ্রুব ভিন্ন angle)
{
	প্রত্যাগম matrix::wrap(angle, 0.0f, 360.0f);
}

} // ObstacleMath
