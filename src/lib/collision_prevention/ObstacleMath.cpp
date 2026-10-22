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

उपयुज्य नामाकाश matrix;

नामाकाश ObstacleMath
{

शून्य project_distance_on_horizontal_plane(भिन्न &distance, ध्रुव भिन्न yaw, ध्रुव matrix::Quatf &q_world_vehicle)
{
	ध्रुव Quatf q_vehicle_sensor(Quatf(cosf(yaw / 2.f), 0.f, 0.f, sinf(yaw / 2.f)));
	ध्रुव Quatf q_world_sensor = q_world_vehicle * q_vehicle_sensor;
	ध्रुव Vector3f forward(1.f, 0.f, 0.f);
	ध्रुव Vector3f sensor_direction_अन्तर्_world = q_world_sensor.rotate_सदिशम्(forward);

	भिन्न horizontal_projection_scale = sensor_direction_अन्तर्_world.xy().norm();
	horizontal_projection_scale = math::constrain(horizontal_projection_scale, FLT_EPSILON, 1.0f);
	distance *= horizontal_projection_scale;
}

पूर्णाङ्क get_bin_at_angle(भिन्न bin_विस्तारः, भिन्न angle)
{
	पूर्णाङ्क bin_at_angle = (पूर्णाङ्क)round(matrix::wrap(angle, 0.f, 360.f) / bin_विस्तारः);
	प्रत्यागम wrap_bin(bin_at_angle, 360 / bin_विस्तारः);
}

भिन्न get_अधस्तनम्_bound_angle(पूर्णाङ्क bin, भिन्न bin_विस्तारः, भिन्न angle_विस्थापनम्)
{
	bin = wrap_bin(bin, 360 / bin_विस्तारः);
	प्रत्यागम wrap_360(bin * bin_विस्तारः + angle_विस्थापनम् - bin_विस्तारः / 2.f);
}

पूर्णाङ्क get_विस्थापनम्_bin_सूचकांक(पूर्णाङ्क bin, भिन्न bin_विस्तारः, भिन्न angle_विस्थापनम्)
{
	पूर्णाङ्क विस्थापनम् = get_bin_at_angle(bin_विस्तारः, angle_विस्थापनम्);
	प्रत्यागम wrap_bin(bin - विस्थापनम्, 360 / bin_विस्तारः);
}

भिन्न sensor_orientation_गन्तव्यम्_yaw_विस्थापनम्(ध्रुव SensorOrientation orientation, ध्रुव भिन्न q[4])
{
	भिन्न विस्थापनम् = 0.0f;

	विकल्प (orientation) {
	पक्ष SensorOrientation::ROTATION_YAW_0:
		विस्थापनम् = 0.0f;
		भङ्ग;

	पक्ष SensorOrientation::ROTATION_YAW_45:
		विस्थापनम् = M_PI_F / 4.0f;
		भङ्ग;

	पक्ष SensorOrientation::ROTATION_YAW_90:
		विस्थापनम् = M_PI_F / 2.0f;
		भङ्ग;

	पक्ष SensorOrientation::ROTATION_YAW_135:
		विस्थापनम् = 3.0f * M_PI_F / 4.0f;
		भङ्ग;

	पक्ष SensorOrientation::ROTATION_YAW_180:
		विस्थापनम् = M_PI_F;
		भङ्ग;

	पक्ष SensorOrientation::ROTATION_YAW_225:
		विस्थापनम् = -3.0f * M_PI_F / 4.0f;
		भङ्ग;

	पक्ष SensorOrientation::ROTATION_YAW_270:
		विस्थापनम् = -M_PI_F / 2.0f;
		भङ्ग;

	पक्ष SensorOrientation::ROTATION_YAW_315:
		विस्थापनम् = -M_PI_F / 4.0f;
		भङ्ग;

	पक्ष SensorOrientation::ROTATION_CUSTOM:
		यदि (q != शून्यसूचकः) {
			विस्थापनम् = Eulerf(Quatf(q)).psi();
		}

		भङ्ग;
	}

	प्रत्यागम विस्थापनम्;
}

पूर्णाङ्क wrap_bin(पूर्णाङ्क bin, पूर्णाङ्क bin_गणनम्)
{
	प्रत्यागम (bin + bin_गणनम्) % bin_गणनम्;
}

भिन्न wrap_360(ध्रुव भिन्न angle)
{
	प्रत्यागम matrix::wrap(angle, 0.0f, 360.0f);
}

} // ObstacleMath
