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

#ਅਨ੍ਤਰ੍ਭਾਵ "PX4Rangefinder.hpp"

#ਅਨ੍ਤਰ੍ਭਾਵ <lib/drivers/device/Device.hpp>

ਵਰਤੋਂ ਨਾਮਾਕਾਸ਼ ਸਮਾਂ_literals;

PX4Rangefinder::PX4Rangefinder(ਅਚਲ uint32_t device_id, ਅਚਲ uint8_t device_orientation)
{
	ਸੈੱਟ_device_id(device_id);
	ਸੈੱਟ_orientation(device_orientation);
	ਸੈੱਟ_rangefinder_type(ਦੂਰੀ_sensor_s::MAV_ਦੂਰੀ_SENSOR_LASER);
	ਸੈੱਟ_ਬਹੁਲਕ(ਦੂਰੀ_sensor_s::ਬਹੁਲਕ_UNKNOWN);
}

PX4Rangefinder::~PX4Rangefinder()
{
	ਦੂਰੀ_sensor_pub_2.unadvertise();
}

ਖਾਲੀ PX4Rangefinder::ਸੈੱਟ_device_type(uint8_t device_type)
{
	// current DeviceStructure
	ਸੰਘ device::Device::DeviceId device_id;
	device_id.devid = ਦੂਰੀ_sensor_pub_2.get().device_id;

	// update to new device type
	device_id.devid_s.devtype = device_type;

	// copy back to report
	ਦੂਰੀ_sensor_pub_2.get().device_id = device_id.devid;
}

ਖਾਲੀ PX4Rangefinder::ਸੈੱਟ_orientation(ਅਚਲ uint8_t device_orientation)
{
	ਦੂਰੀ_sensor_pub_2.get().orientation = device_orientation;
}

ਖਾਲੀ PX4Rangefinder::update(ਅਚਲ hrt_abstime &timestamp_sample, ਅਚਲ ਦਸ਼ਮਲਵ ਦੂਰੀ, ਅਚਲ int8_t quality, ਅਚਲ ਦਸ਼ਮਲਵ *q, uint8_t q_len)
{
	ਦੂਰੀ_sensor_s &report = ਦੂਰੀ_sensor_pub_2.get();
	report.timestamp = timestamp_sample;
	report.current_ਦੂਰੀ = ਦੂਰੀ;
	report.signal_quality = quality;

	// if quality is unavailable (-1) set to 0 if distance is outside bounds
	ਜੇ (quality < 0) {
		ਜੇ ((ਦੂਰੀ < report.min_ਦੂਰੀ) || (ਦੂਰੀ > report.max_ਦੂਰੀ)) {
			report.signal_quality = 0;
		}
	}

	// Update the quaternion in the sample update
	ਜੇ (q != ਖਾਲੀ_ਸੂਚਕ) {
		ਸਮ੍ਰਿਤੀ_ਨਕਲ(report.q, q, ਆਕਾਰ(ਦਸ਼ਮਲਵ) * q_len);
	}

	_failure_config.update();

	ਜੇ (!failure_injection::process(_failure_config, failure_injection_s::FAILURE_UNIT_SENSOR_ਦੂਰੀ_SENSOR,
					ਦੂਰੀ_sensor_pub_2.get_instance(), report, _stuck)) {
		ਵਾਪਸ;
	}

	ਦੂਰੀ_sensor_pub_2.update();
}
