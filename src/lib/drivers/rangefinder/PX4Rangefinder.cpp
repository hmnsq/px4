/****************************************************************************
 *
 *   Copyright (c) 2019 PX4 Development Team. All rights reserved.
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

#অন্তর্ভাব "PX4Rangefinder.hpp"

#অন্তর্ভাব <lib/drivers/device/Device.hpp>

using নামাকাশ time_literals;

PX4Rangefinder::PX4Rangefinder(ধ্রুবক uint32_t device_id, ধ্রুবক uint8_t device_orientation)
{
	set_device_id(device_id);
	set_orientation(device_orientation);
	set_rangefinder_type(distance_sensor_s::MAV_DISTANCE_SENSOR_LASER);
	set_mode(distance_sensor_s::MODE_UNKNOWN);
}

PX4Rangefinder::~PX4Rangefinder()
{
	_distance_sensor_pub.unadvertise();
}

শূন্য PX4Rangefinder::set_device_type(uint8_t device_type)
{
	// current DeviceStructure
	সংঘ device::Device::DeviceId device_id;
	device_id.devid = _distance_sensor_pub.get().device_id;

	// update to new device type
	device_id.devid_s.devtype = device_type;

	// copy back to report
	_distance_sensor_pub.get().device_id = device_id.devid;
}

শূন্য PX4Rangefinder::set_orientation(ধ্রুবক uint8_t device_orientation)
{
	_distance_sensor_pub.get().orientation = device_orientation;
}

শূন্য PX4Rangefinder::update(ধ্রুবক hrt_abstime &timestamp_sample, ধ্রুবক দশমিক distance, ধ্রুবক int8_t quality, ধ্রুবক দশমিক *q, uint8_t q_len)
{
	distance_sensor_s &report = _distance_sensor_pub.get();
	report.timestamp = timestamp_sample;
	report.current_distance = distance;
	report.signal_quality = quality;

	// if quality is unavailable (-1) set to 0 if distance is outside bounds
	যদি (quality < 0) {
		যদি ((distance < report.min_distance) || (distance > report.max_distance)) {
			report.signal_quality = 0;
		}
	}

	// Update the quaternion in the sample update
	যদি (q != nullptr) {
		memcpy(report.q, q, আকার(দশমিক) * q_len);
	}

	_failure_config.update();

	যদি (!failure_injection::process(_failure_config, failure_injection_s::FAILURE_UNIT_SENSOR_DISTANCE_SENSOR,
					_distance_sensor_pub.get_instance(), report, _stuck)) {
		ফেরত;
	}

	_distance_sensor_pub.update();
}
