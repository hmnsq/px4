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

#ضمن "PX4Rangefinder.hpp"

#ضمن <lib/drivers/device/Device.hpp>

استعمال نطاق وقت_literals;

PX4Rangefinder::PX4Rangefinder(ثابت uint32_t device_id, ثابت uint8_t device_orientation)
{
	سيٽ_device_id(device_id);
	سيٽ_orientation(device_orientation);
	سيٽ_rangefinder_type(مفاصلو_sensor_s::MAV_مفاصلو_SENSOR_LASER);
	سيٽ_گهڻائي(مفاصلو_sensor_s::گهڻائي_UNKNOWN);
}

PX4Rangefinder::~PX4Rangefinder()
{
	مفاصلو_sensor_pub_2.unadvertise();
}

فراغ PX4Rangefinder::سيٽ_device_type(uint8_t device_type)
{
	// current DeviceStructure
	اتحاد device::Device::DeviceId device_id;
	device_id.devid = مفاصلو_sensor_pub_2.get().device_id;

	// update to new device type
	device_id.devid_s.devtype = device_type;

	// copy back to report
	مفاصلو_sensor_pub_2.get().device_id = device_id.devid;
}

فراغ PX4Rangefinder::سيٽ_orientation(ثابت uint8_t device_orientation)
{
	مفاصلو_sensor_pub_2.get().orientation = device_orientation;
}

فراغ PX4Rangefinder::update(ثابت hrt_abstime &timestamp_sample, ثابت عائم مفاصلو, ثابت int8_t quality, ثابت عائم *q, uint8_t q_len)
{
	مفاصلو_sensor_s &report = مفاصلو_sensor_pub_2.get();
	report.timestamp = timestamp_sample;
	report.current_مفاصلو = مفاصلو;
	report.signal_quality = quality;

	// if quality is unavailable (-1) set to 0 if distance is outside bounds
	إذا (quality < 0) {
		إذا ((مفاصلو < report.min_مفاصلو) || (مفاصلو > report.max_مفاصلو)) {
			report.signal_quality = 0;
		}
	}

	// Update the quaternion in the sample update
	إذا (q != خالي_اشارو) {
		يادگيري_نقل(report.q, q, حجم(عائم) * q_len);
	}

	_failure_config.update();

	إذا (!failure_injection::process(_failure_config, failure_injection_s::FAILURE_UNIT_SENSOR_مفاصلو_SENSOR,
					مفاصلو_sensor_pub_2.get_instance(), report, _stuck)) {
		أرجع;
	}

	مفاصلو_sensor_pub_2.update();
}
