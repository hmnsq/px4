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

#ਅਨ੍ਤਰ੍ਭਾਵ "ObstacleMath.hpp"
#ਅਨ੍ਤਰ੍ਭਾਵ <mathlib/math/Limits.hpp>

ਵਰਤੋਂ ਨਾਮਾਕਾਸ਼ ਮੈਟ੍ਰਿਕਸ;

ਨਾਮਾਕਾਸ਼ ObstacleMath
{

ਖਾਲੀ project_ਦੂਰੀ_on_horizontal_plane(ਦਸ਼ਮਲਵ &ਦੂਰੀ, ਅਚਲ ਦਸ਼ਮਲਵ yaw, ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Quatf &q_world_vehicle)
{
	ਅਚਲ Quatf q_vehicle_sensor(Quatf(cosf(yaw / 2.f), 0.f, 0.f, sinf(yaw / 2.f)));
	ਅਚਲ Quatf q_world_sensor = q_world_vehicle * q_vehicle_sensor;
	ਅਚਲ Vector3f forward(1.f, 0.f, 0.f);
	ਅਚਲ Vector3f sensor_direction_ਵਿੱਚ_world = q_world_sensor.rotate_ਸਦਿਸ਼(forward);

	ਦਸ਼ਮਲਵ horizontal_projection_scale = sensor_direction_ਵਿੱਚ_world.xy().norm();
	horizontal_projection_scale = math::constrain(horizontal_projection_scale, FLT_EPSILON, 1.0f);
	ਦੂਰੀ *= horizontal_projection_scale;
}

ਪੂਰਨ get_bin_at_angle(ਦਸ਼ਮਲਵ bin_ਚੌੜਾਈ, ਦਸ਼ਮਲਵ angle)
{
	ਪੂਰਨ bin_at_angle = (ਪੂਰਨ)round(ਮੈਟ੍ਰਿਕਸ::wrap(angle, 0.f, 360.f) / bin_ਚੌੜਾਈ);
	ਵਾਪਸ wrap_bin(bin_at_angle, 360 / bin_ਚੌੜਾਈ);
}

ਦਸ਼ਮਲਵ get_ਹੇਠਲਾ_bound_angle(ਪੂਰਨ bin, ਦਸ਼ਮਲਵ bin_ਚੌੜਾਈ, ਦਸ਼ਮਲਵ angle_ਵਿੱਥ)
{
	bin = wrap_bin(bin, 360 / bin_ਚੌੜਾਈ);
	ਵਾਪਸ wrap_360(bin * bin_ਚੌੜਾਈ + angle_ਵਿੱਥ - bin_ਚੌੜਾਈ / 2.f);
}

ਪੂਰਨ get_ਵਿੱਥ_bin_ਸੂਚਕਾਂਕ(ਪੂਰਨ bin, ਦਸ਼ਮਲਵ bin_ਚੌੜਾਈ, ਦਸ਼ਮਲਵ angle_ਵਿੱਥ)
{
	ਪੂਰਨ ਵਿੱਥ = get_bin_at_angle(bin_ਚੌੜਾਈ, angle_ਵਿੱਥ);
	ਵਾਪਸ wrap_bin(bin - ਵਿੱਥ, 360 / bin_ਚੌੜਾਈ);
}

ਦਸ਼ਮਲਵ sensor_orientation_ਮੰਜ਼ਿਲ_yaw_ਵਿੱਥ(ਅਚਲ SensorOrientation orientation, ਅਚਲ ਦਸ਼ਮਲਵ q[4])
{
	ਦਸ਼ਮਲਵ ਵਿੱਥ = 0.0f;

	ਚੋਣ (orientation) {
	ਹਾਲਤ SensorOrientation::ROTATION_YAW_0:
		ਵਿੱਥ = 0.0f;
		ਰੋਕੋ;

	ਹਾਲਤ SensorOrientation::ROTATION_YAW_45:
		ਵਿੱਥ = M_PI_F / 4.0f;
		ਰੋਕੋ;

	ਹਾਲਤ SensorOrientation::ROTATION_YAW_90:
		ਵਿੱਥ = M_PI_F / 2.0f;
		ਰੋਕੋ;

	ਹਾਲਤ SensorOrientation::ROTATION_YAW_135:
		ਵਿੱਥ = 3.0f * M_PI_F / 4.0f;
		ਰੋਕੋ;

	ਹਾਲਤ SensorOrientation::ROTATION_YAW_180:
		ਵਿੱਥ = M_PI_F;
		ਰੋਕੋ;

	ਹਾਲਤ SensorOrientation::ROTATION_YAW_225:
		ਵਿੱਥ = -3.0f * M_PI_F / 4.0f;
		ਰੋਕੋ;

	ਹਾਲਤ SensorOrientation::ROTATION_YAW_270:
		ਵਿੱਥ = -M_PI_F / 2.0f;
		ਰੋਕੋ;

	ਹਾਲਤ SensorOrientation::ROTATION_YAW_315:
		ਵਿੱਥ = -M_PI_F / 4.0f;
		ਰੋਕੋ;

	ਹਾਲਤ SensorOrientation::ROTATION_CUSTOM:
		ਜੇ (q != ਖਾਲੀ_ਸੂਚਕ) {
			ਵਿੱਥ = Eulerf(Quatf(q)).psi();
		}

		ਰੋਕੋ;
	}

	ਵਾਪਸ ਵਿੱਥ;
}

ਪੂਰਨ wrap_bin(ਪੂਰਨ bin, ਪੂਰਨ bin_ਗਿਣਤੀ)
{
	ਵਾਪਸ (bin + bin_ਗਿਣਤੀ) % bin_ਗਿਣਤੀ;
}

ਦਸ਼ਮਲਵ wrap_360(ਅਚਲ ਦਸ਼ਮਲਵ angle)
{
	ਵਾਪਸ ਮੈਟ੍ਰਿਕਸ::wrap(angle, 0.0f, 360.0f);
}

} // ObstacleMath
