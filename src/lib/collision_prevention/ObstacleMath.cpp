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

#अन्तर्भाव "ObstacleMath.hpp"
#अन्तर्भाव <mathlib/math/Limits.hpp>

using नामाकाश matrix;

नामाकाश ObstacleMath
{

शून्य project_distance_on_horizontal_plane(दशांश &distance, स्थिरांक दशांश yaw, स्थिरांक matrix::Quatf &q_world_vehicle)
{
	स्थिरांक Quatf q_vehicle_sensor(Quatf(cosf(yaw / 2.f), 0.f, 0.f, sinf(yaw / 2.f)));
	स्थिरांक Quatf q_world_sensor = q_world_vehicle * q_vehicle_sensor;
	स्थिरांक Vector3f forward(1.f, 0.f, 0.f);
	स्थिरांक Vector3f sensor_direction_in_world = q_world_sensor.rotateVector(forward);

	दशांश horizontal_projection_scale = sensor_direction_in_world.xy().norm();
	horizontal_projection_scale = math::constrain(horizontal_projection_scale, भग्न_अंतर, 1.0f);
	distance *= horizontal_projection_scale;
}

पूर्णांक get_bin_at_angle(दशांश bin_width, दशांश angle)
{
	पूर्णांक bin_at_angle = (पूर्णांक)round(matrix::wrap(angle, 0.f, 360.f) / bin_width);
	परत wrap_bin(bin_at_angle, 360 / bin_width);
}

दशांश get_lower_bound_angle(पूर्णांक bin, दशांश bin_width, दशांश angle_offset)
{
	bin = wrap_bin(bin, 360 / bin_width);
	परत wrap_360(bin * bin_width + angle_offset - bin_width / 2.f);
}

पूर्णांक get_offset_bin_index(पूर्णांक bin, दशांश bin_width, दशांश angle_offset)
{
	पूर्णांक offset = get_bin_at_angle(bin_width, angle_offset);
	परत wrap_bin(bin - offset, 360 / bin_width);
}

दशांश sensor_orientation_to_yaw_offset(स्थिरांक SensorOrientation orientation, स्थिरांक दशांश q[4])
{
	दशांश offset = 0.0f;

	निवड (orientation) {
	स्थिती SensorOrientation::ROTATION_YAW_0:
		offset = 0.0f;
		थांबा;

	स्थिती SensorOrientation::ROTATION_YAW_45:
		offset = M_PI_F / 4.0f;
		थांबा;

	स्थिती SensorOrientation::ROTATION_YAW_90:
		offset = M_PI_F / 2.0f;
		थांबा;

	स्थिती SensorOrientation::ROTATION_YAW_135:
		offset = 3.0f * M_PI_F / 4.0f;
		थांबा;

	स्थिती SensorOrientation::ROTATION_YAW_180:
		offset = M_PI_F;
		थांबा;

	स्थिती SensorOrientation::ROTATION_YAW_225:
		offset = -3.0f * M_PI_F / 4.0f;
		थांबा;

	स्थिती SensorOrientation::ROTATION_YAW_270:
		offset = -M_PI_F / 2.0f;
		थांबा;

	स्थिती SensorOrientation::ROTATION_YAW_315:
		offset = -M_PI_F / 4.0f;
		थांबा;

	स्थिती SensorOrientation::ROTATION_CUSTOM:
		जर (q != nullptr) {
			offset = Eulerf(Quatf(q)).psi();
		}

		थांबा;
	}

	परत offset;
}

पूर्णांक wrap_bin(पूर्णांक bin, पूर्णांक bin_मोजणी)
{
	परत (bin + bin_मोजणी) % bin_मोजणी;
}

दशांश wrap_360(स्थिरांक दशांश angle)
{
	परत matrix::wrap(angle, 0.0f, 360.0f);
}

} // ObstacleMath
