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

#அந்தர்பாவ "ObstacleMath.hpp"
#அந்தர்பாவ <mathlib/math/Limits.hpp>

using நாமாகாஶ matrix;

நாமாகாஶ ObstacleMath
{

வெறுமை project_distance_on_horizontal_plane(தசம &distance, மாறிலி தசம yaw, மாறிலி matrix::Quatf &q_world_vehicle)
{
	மாறிலி Quatf q_vehicle_sensor(Quatf(cosf(yaw / 2.f), 0.f, 0.f, sinf(yaw / 2.f)));
	மாறிலி Quatf q_world_sensor = q_world_vehicle * q_vehicle_sensor;
	மாறிலி Vector3f forward(1.f, 0.f, 0.f);
	மாறிலி Vector3f sensor_direction_in_world = q_world_sensor.rotateVector(forward);

	தசம horizontal_projection_scale = sensor_direction_in_world.xy().norm();
	horizontal_projection_scale = math::constrain(horizontal_projection_scale, FLT_EPSILON, 1.0f);
	distance *= horizontal_projection_scale;
}

முழு get_bin_at_angle(தசம bin_width, தசம angle)
{
	முழு bin_at_angle = (முழு)round(matrix::wrap(angle, 0.f, 360.f) / bin_width);
	திரும்பு wrap_bin(bin_at_angle, 360 / bin_width);
}

தசம get_lower_bound_angle(முழு bin, தசம bin_width, தசம angle_offset)
{
	bin = wrap_bin(bin, 360 / bin_width);
	திரும்பு wrap_360(bin * bin_width + angle_offset - bin_width / 2.f);
}

முழு get_offset_bin_index(முழு bin, தசம bin_width, தசம angle_offset)
{
	முழு offset = get_bin_at_angle(bin_width, angle_offset);
	திரும்பு wrap_bin(bin - offset, 360 / bin_width);
}

தசம sensor_orientation_to_yaw_offset(மாறிலி SensorOrientation orientation, மாறிலி தசம q[4])
{
	தசம offset = 0.0f;

	தேர்வு (orientation) {
	நிலை SensorOrientation::ROTATION_YAW_0:
		offset = 0.0f;
		நிறுத்து;

	நிலை SensorOrientation::ROTATION_YAW_45:
		offset = M_PI_F / 4.0f;
		நிறுத்து;

	நிலை SensorOrientation::ROTATION_YAW_90:
		offset = M_PI_F / 2.0f;
		நிறுத்து;

	நிலை SensorOrientation::ROTATION_YAW_135:
		offset = 3.0f * M_PI_F / 4.0f;
		நிறுத்து;

	நிலை SensorOrientation::ROTATION_YAW_180:
		offset = M_PI_F;
		நிறுத்து;

	நிலை SensorOrientation::ROTATION_YAW_225:
		offset = -3.0f * M_PI_F / 4.0f;
		நிறுத்து;

	நிலை SensorOrientation::ROTATION_YAW_270:
		offset = -M_PI_F / 2.0f;
		நிறுத்து;

	நிலை SensorOrientation::ROTATION_YAW_315:
		offset = -M_PI_F / 4.0f;
		நிறுத்து;

	நிலை SensorOrientation::ROTATION_CUSTOM:
		எனில் (q != nullptr) {
			offset = Eulerf(Quatf(q)).psi();
		}

		நிறுத்து;
	}

	திரும்பு offset;
}

முழு wrap_bin(முழு bin, முழு bin_மொத்தம்)
{
	திரும்பு (bin + bin_மொத்தம்) % bin_மொத்தம்;
}

தசம wrap_360(மாறிலி தசம angle)
{
	திரும்பு matrix::wrap(angle, 0.0f, 360.0f);
}

} // ObstacleMath
