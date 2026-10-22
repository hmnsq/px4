/****************************************************************************
 *
 *   Copyright (c) 2018-2022 PX4 Development Team. All rights reserved.
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


#شامل "PX4Gyroscope.hpp"

#شامل <lib/drivers/device/Device.hpp>
#شامل <lib/parameters/param.h>

استعمال نامخلا وقت_literals;

ساکن constexpr int32_t مجموع(ثابت int16_t samples[], uint8_t len)
{
	int32_t مجموع = 0;

	برائے (صحیح n = 0; n < len; n++) {
		مجموع += samples[n];
	}

	واپس مجموع;
}

ساکن constexpr uint8_t clipping(ثابت int16_t samples[], uint8_t len, int16_t clip_حد_بندی)
{
	غیرعلامتی clip_گنتی = 0;

	برائے (صحیح n = 0; n < len; n++) {
		// symmetric about zero: rotation negates samples and |INT16_MIN| = INT16_MAX + 1
		اگر ((samples[n] <= -clip_حد_بندی) || (samples[n] >= clip_حد_بندی)) {
			clip_گنتی++;
		}
	}

	واپس clip_گنتی;
}

PX4Gyroscope::PX4Gyroscope(uint32_t device_id, شمار Rotation rotation) :
	PX4Gyroscope(device_id, rotation, device::device_is_external(device_id))
{
	_external_forced = جھوٹ;
}

PX4Gyroscope::PX4Gyroscope(uint32_t device_id, شمار Rotation rotation, منطقی external) :
	_device_id{device_id},
	_rotation{rotation},
	_is_external{external},
	_external_forced{سچ}
{
	// advertise immediately to keep instance numbering in sync
	_sensor_pub.advertise();

	param_get(param_ڈھونڈو("IMU_GYRO_RATEMAX"), &imu_gyro_شرح_max);
}

خالی PX4Gyroscope::سیٹ_device_id(uint32_t device_id)
{
	_device_id = device_id;

	اگر (!_external_forced) {
		_is_external = device::device_is_external(device_id);
	}
}

خالی PX4Gyroscope::سیٹ_external(منطقی external)
{
	_is_external = external;
	_external_forced = سچ;
}

PX4Gyroscope::~PX4Gyroscope()
{
	_sensor_pub.unadvertise();
	_sensor_fifo_pub.unadvertise();
}

خالی PX4Gyroscope::سیٹ_device_type(uint8_t devtype)
{
	// current DeviceStructure
	اتحاد device::Device::DeviceId device_id;
	device_id.devid = _device_id;

	// update to new device type
	device_id.devid_s.devtype = devtype;

	// copy back
	_device_id = device_id.devid;
}

خالی PX4Gyroscope::سیٹ_scale(اعشاری scale)
{
	اگر (fabsf(scale - _scale) > FLT_EPSILON) {
		// rescale last sample on scale change
		اعشاری rescale = _scale / scale;

		برائے (خودکار &s : آخری_sample) {
			s = roundf(s * rescale);
		}

		_scale = scale;

		Update_Clip_حد_بندی();
	}
}

خالی PX4Gyroscope::update(ثابت hrt_abstime &timestamp_sample, اعشاری x, اعشاری y, اعشاری z)
{
	// Apply rotation (before scaling)
	rotate_3f(_rotation, x, y, z);

	sensor_gyro_s report;

	report.timestamp_sample = timestamp_sample;
	report.device_id = _device_id;
	report.is_external = _is_external;
	report.temperature = _temperature;
	report.error_گنتی = error_گنتی_2;
	report.x = x * _scale;
	report.y = y * _scale;
	report.z = z * _scale;
	report.clip_شمار[0] = (fabsf(x) >= clip_حد_بندی_2);
	report.clip_شمار[1] = (fabsf(y) >= clip_حد_بندی_2);
	report.clip_شمار[2] = (fabsf(z) >= clip_حد_بندی_2);
	report.samples = 1;
	report.timestamp = hrt_absolute_وقت();

	_failure_config.update();

	اگر (!failure_injection::process(_failure_config, failure_injection_s::FAILURE_UNIT_SENSOR_GYRO,
					_sensor_pub.get_instance(), report, _stuck)) {
		واپس;
	}

	_sensor_pub.publish(report);
}

خالی PX4Gyroscope::updateFIFO(sensor_gyro_fifo_s &sample)
{
	// rotate all raw samples and publish fifo
	برائے (صحیح n = 0; n < sample.samples; n++) {
		rotate_3i(_rotation, sample.x[n], sample.y[n], sample.z[n]);
	}

	sample.device_id = _device_id;
	sample.is_external = _is_external;
	sample.scale = _scale;
	sample.timestamp = hrt_absolute_وقت();

	_failure_config.update();

	اگر (!failure_injection::process(_failure_config, failure_injection_s::FAILURE_UNIT_SENSOR_GYRO,
					_sensor_pub.get_instance(), sample, _stuck_fifo)) {
		واپس;
	}

	_sensor_fifo_pub.publish(sample);

	ثابت uint8_t N = sample.samples;

	// publish
	sensor_gyro_s report;
	report.timestamp_sample = sample.timestamp_sample;
	report.device_id = _device_id;
	report.is_external = _is_external;
	report.temperature = _temperature;
	report.error_گنتی = error_گنتی_2;

	// trapezoidal integration (equally spaced)
	ثابت اعشاری scale = _scale / (اعشاری)N;
	report.x = (0.5f * (آخری_sample[0] + sample.x[N - 1]) + مجموع(sample.x, N - 1)) * scale;
	report.y = (0.5f * (آخری_sample[1] + sample.y[N - 1]) + مجموع(sample.y, N - 1)) * scale;
	report.z = (0.5f * (آخری_sample[2] + sample.z[N - 1]) + مجموع(sample.z, N - 1)) * scale;

	آخری_sample[0] = sample.x[N - 1];
	آخری_sample[1] = sample.y[N - 1];
	آخری_sample[2] = sample.z[N - 1];

	// The declared range is not the int16 rail on every part (ST high-g and dps scales leave
	// headroom in the word), so clip against range/scale like update() does. The 0.999 margin
	// in _clip_limit also covers sensors that reuse the lowest bit for a sync flag.
	ثابت int16_t clip_حد_بندی = static_cast<int16_t>(math::min(clip_حد_بندی_2, (اعشاری)(INT16_MAX - 1)));
	report.clip_شمار[0] = clipping(sample.x, N, clip_حد_بندی);
	report.clip_شمار[1] = clipping(sample.y, N, clip_حد_بندی);
	report.clip_شمار[2] = clipping(sample.z, N, clip_حد_بندی);
	report.samples = N;
	report.timestamp = hrt_absolute_وقت();

	اگر (!failure_injection::process(_failure_config, failure_injection_s::FAILURE_UNIT_SENSOR_GYRO,
					_sensor_pub.get_instance(), report, _stuck)) {
		واپس;
	}

	_sensor_pub.publish(report);
}

خالی PX4Gyroscope::Update_Clip_حد_بندی()
{
	// 99.9% of potential max
	clip_حد_بندی_2 = fabsf(_range / _scale * 0.999f);
}
