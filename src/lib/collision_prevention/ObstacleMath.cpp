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

#అన్తర్భావ "ObstacleMath.hpp"
#అన్తర్భావ <mathlib/math/Limits.hpp>

ఉపయోగించి నామాకాశ మాతృక;

నామాకాశ ObstacleMath
{

శూన్య project_దూరం_on_horizontal_plane(దశాంశం &దూరం, స్థిరరాశి దశాంశం yaw, స్థిరరాశి మాతృక::Quatf &q_world_vehicle)
{
	స్థిరరాశి Quatf q_vehicle_sensor(Quatf(cosf(yaw / 2.f), 0.f, 0.f, sinf(yaw / 2.f)));
	స్థిరరాశి Quatf q_world_sensor = q_world_vehicle * q_vehicle_sensor;
	స్థిరరాశి Vector3f forward(1.f, 0.f, 0.f);
	స్థిరరాశి Vector3f sensor_direction_లోపల_world = q_world_sensor.rotate_సదిశ(forward);

	దశాంశం horizontal_projection_scale = sensor_direction_లోపల_world.xy().norm();
	horizontal_projection_scale = math::constrain(horizontal_projection_scale, FLT_EPSILON, 1.0f);
	దూరం *= horizontal_projection_scale;
}

పూర్ణాంకం get_bin_at_angle(దశాంశం bin_వెడల్పు, దశాంశం angle)
{
	పూర్ణాంకం bin_at_angle = (పూర్ణాంకం)round(మాతృక::wrap(angle, 0.f, 360.f) / bin_వెడల్పు);
	తిరిగి wrap_bin(bin_at_angle, 360 / bin_వెడల్పు);
}

దశాంశం get_క్రింది_bound_angle(పూర్ణాంకం bin, దశాంశం bin_వెడల్పు, దశాంశం angle_స్థానభ్రంశం)
{
	bin = wrap_bin(bin, 360 / bin_వెడల్పు);
	తిరిగి wrap_360(bin * bin_వెడల్పు + angle_స్థానభ్రంశం - bin_వెడల్పు / 2.f);
}

పూర్ణాంకం get_స్థానభ్రంశం_bin_సూచీ(పూర్ణాంకం bin, దశాంశం bin_వెడల్పు, దశాంశం angle_స్థానభ్రంశం)
{
	పూర్ణాంకం స్థానభ్రంశం = get_bin_at_angle(bin_వెడల్పు, angle_స్థానభ్రంశం);
	తిరిగి wrap_bin(bin - స్థానభ్రంశం, 360 / bin_వెడల్పు);
}

దశాంశం sensor_orientation_గమ్యం_yaw_స్థానభ్రంశం(స్థిరరాశి SensorOrientation orientation, స్థిరరాశి దశాంశం q[4])
{
	దశాంశం స్థానభ్రంశం = 0.0f;

	ఎంపిక (orientation) {
	స్థితి SensorOrientation::ROTATION_YAW_0:
		స్థానభ్రంశం = 0.0f;
		ఆపు;

	స్థితి SensorOrientation::ROTATION_YAW_45:
		స్థానభ్రంశం = M_PI_F / 4.0f;
		ఆపు;

	స్థితి SensorOrientation::ROTATION_YAW_90:
		స్థానభ్రంశం = M_PI_F / 2.0f;
		ఆపు;

	స్థితి SensorOrientation::ROTATION_YAW_135:
		స్థానభ్రంశం = 3.0f * M_PI_F / 4.0f;
		ఆపు;

	స్థితి SensorOrientation::ROTATION_YAW_180:
		స్థానభ్రంశం = M_PI_F;
		ఆపు;

	స్థితి SensorOrientation::ROTATION_YAW_225:
		స్థానభ్రంశం = -3.0f * M_PI_F / 4.0f;
		ఆపు;

	స్థితి SensorOrientation::ROTATION_YAW_270:
		స్థానభ్రంశం = -M_PI_F / 2.0f;
		ఆపు;

	స్థితి SensorOrientation::ROTATION_YAW_315:
		స్థానభ్రంశం = -M_PI_F / 4.0f;
		ఆపు;

	స్థితి SensorOrientation::ROTATION_CUSTOM:
		ఒకవేళ (q != శూన్యసూచిక) {
			స్థానభ్రంశం = Eulerf(Quatf(q)).psi();
		}

		ఆపు;
	}

	తిరిగి స్థానభ్రంశం;
}

పూర్ణాంకం wrap_bin(పూర్ణాంకం bin, పూర్ణాంకం bin_లెక్క)
{
	తిరిగి (bin + bin_లెక్క) % bin_లెక్క;
}

దశాంశం wrap_360(స్థిరరాశి దశాంశం angle)
{
	తిరిగి మాతృక::wrap(angle, 0.0f, 360.0f);
}

} // ObstacleMath
