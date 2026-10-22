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

#ضمن "ObstacleMath.hpp"
#ضمن <mathlib/math/Limits.hpp>

استفاده نطاق ماتریس;

نطاق ObstacleMath
{

فراغ project_فاصله_on_horizontal_plane(عائم &فاصله, ثابت عائم yaw, ثابت ماتریس::Quatf &q_world_vehicle)
{
	ثابت Quatf q_vehicle_sensor(Quatf(cosf(yaw / 2.f), 0.f, 0.f, sinf(yaw / 2.f)));
	ثابت Quatf q_world_sensor = q_world_vehicle * q_vehicle_sensor;
	ثابت Vector3f forward(1.f, 0.f, 0.f);
	ثابت Vector3f sensor_direction_درون_world = q_world_sensor.rotate_وکتور(forward);

	عائم horizontal_projection_scale = sensor_direction_درون_world.xy().norm();
	horizontal_projection_scale = math::constrain(horizontal_projection_scale, FLT_EPSILON, 1.0f);
	فاصله *= horizontal_projection_scale;
}

صحيح get_bin_at_angle(عائم bin_عرض, عائم angle)
{
	صحيح bin_at_angle = (صحيح)round(ماتریس::wrap(angle, 0.f, 360.f) / bin_عرض);
	أرجع wrap_bin(bin_at_angle, 360 / bin_عرض);
}

عائم get_پایینی_bound_angle(صحيح bin, عائم bin_عرض, عائم angle_جابه_جایی)
{
	bin = wrap_bin(bin, 360 / bin_عرض);
	أرجع wrap_360(bin * bin_عرض + angle_جابه_جایی - bin_عرض / 2.f);
}

صحيح get_جابه_جایی_bin_شاخص(صحيح bin, عائم bin_عرض, عائم angle_جابه_جایی)
{
	صحيح جابه_جایی = get_bin_at_angle(bin_عرض, angle_جابه_جایی);
	أرجع wrap_bin(bin - جابه_جایی, 360 / bin_عرض);
}

عائم sensor_orientation_مقصد_yaw_جابه_جایی(ثابت SensorOrientation orientation, ثابت عائم q[4])
{
	عائم جابه_جایی = 0.0f;

	بدل (orientation) {
	حالة SensorOrientation::ROTATION_YAW_0:
		جابه_جایی = 0.0f;
		اكسر;

	حالة SensorOrientation::ROTATION_YAW_45:
		جابه_جایی = M_PI_F / 4.0f;
		اكسر;

	حالة SensorOrientation::ROTATION_YAW_90:
		جابه_جایی = M_PI_F / 2.0f;
		اكسر;

	حالة SensorOrientation::ROTATION_YAW_135:
		جابه_جایی = 3.0f * M_PI_F / 4.0f;
		اكسر;

	حالة SensorOrientation::ROTATION_YAW_180:
		جابه_جایی = M_PI_F;
		اكسر;

	حالة SensorOrientation::ROTATION_YAW_225:
		جابه_جایی = -3.0f * M_PI_F / 4.0f;
		اكسر;

	حالة SensorOrientation::ROTATION_YAW_270:
		جابه_جایی = -M_PI_F / 2.0f;
		اكسر;

	حالة SensorOrientation::ROTATION_YAW_315:
		جابه_جایی = -M_PI_F / 4.0f;
		اكسر;

	حالة SensorOrientation::ROTATION_CUSTOM:
		إذا (q != اشاره_تهی) {
			جابه_جایی = Eulerf(Quatf(q)).psi();
		}

		اكسر;
	}

	أرجع جابه_جایی;
}

صحيح wrap_bin(صحيح bin, صحيح bin_شمار)
{
	أرجع (bin + bin_شمار) % bin_شمار;
}

عائم wrap_360(ثابت عائم angle)
{
	أرجع ماتریس::wrap(angle, 0.0f, 360.0f);
}

} // ObstacleMath
