/****************************************************************************
 *
 *   Copyright (c) 2018 PX4 Development Team. All rights reserved.
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


#अन्तर्भाव "PX4Magnetometer.hpp"

#अन्तर्भाव <lib/drivers/device/Device.hpp>

PX4Magnetometer::PX4Magnetometer(uint32_t device_id, गणना Rotation rotation) :
	PX4Magnetometer(device_id, rotation, device::device_is_external(device_id))
{
	_external_forced = मिच्छ;
}

PX4Magnetometer::PX4Magnetometer(uint32_t device_id, गणना Rotation rotation, तार्किक external) :
	_device_id{device_id},
	_rotation{rotation},
	_is_external{external},
	_external_forced{सच्च}
{
}

शून्य PX4Magnetometer::set_device_id(uint32_t device_id)
{
	_device_id = device_id;

	जइ (!_external_forced) {
		_is_external = device::device_is_external(device_id);
	}
}

शून्य PX4Magnetometer::set_external(तार्किक external)
{
	_is_external = external;
	_external_forced = सच्च;
}

PX4Magnetometer::~PX4Magnetometer()
{
	_sensor_pub.unadvertise();
}

शून्य PX4Magnetometer::set_device_type(uint8_t devtype)
{
	// current DeviceStructure
	सङ्घ device::Device::DeviceId device_id;
	device_id.devid = _device_id;

	// update to new device type
	device_id.devid_s.devtype = devtype;

	// copy back
	_device_id = device_id.devid;
}

शून्य PX4Magnetometer::update(ध्रुव hrt_abstime &timestamp_sample, भिन्न x, भिन्न y, भिन्न z)
{
	sensor_mag_s report;
	report.timestamp_sample = timestamp_sample;
	report.device_id = _device_id;
	report.is_external = _is_external;
	report.temperature = _temperature;
	report.range = _range;
	report.error_गणनम् = error_गणनम्_2;

	// Apply rotation (before scaling)
	rotate_3f(_rotation, x, y, z);

	report.x = x * _scale;
	report.y = y * _scale;
	report.z = z * _scale;

	report.timestamp = hrt_absolute_time();

	_failure_config.update();

	जइ (!failure_injection::process(_failure_config, failure_injection_s::FAILURE_UNIT_SENSOR_MAG,
					_sensor_pub.get_instance(), report, _stuck)) {
		प्रत्यागम;
	}

	_sensor_pub.publish(report);
}
