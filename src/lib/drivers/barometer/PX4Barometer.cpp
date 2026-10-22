/****************************************************************************
 *
 *   Copyright (c) 2026 PX4 Development Team. All rights reserved.
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


#অন্তর্ভাব "PX4Barometer.hpp"

#অন্তর্ভাব <lib/drivers/device/Device.hpp>

PX4Barometer::PX4Barometer(uint32_t device_id)
{
	set_device_id(device_id);
	_report.temperature = 15; // if no temperature is set, report the sea level standard temperature of 15°C. As used in VehicleAirData
}

PX4Barometer::PX4Barometer(uint32_t device_id, তার্কিক external)
{
	set_device_id(device_id);
	set_external(external);
	_report.temperature = 15; // if no temperature is set, report the sea level standard temperature of 15°C. As used in VehicleAirData
}

শূন্য PX4Barometer::set_device_id(uint32_t device_id)
{
	_report.device_id = device_id;

	যদি (!_external_forced) {
		_report.is_external = device::device_is_external(device_id);
	}
}

শূন্য PX4Barometer::set_external(তার্কিক external)
{
	_report.is_external = external;
	_external_forced = সত্য;
}

শূন্য PX4Barometer::update(ধ্রুব hrt_abstime &timestamp_sample, ভিন্ন pressure)
{
	যদি (!PX4_ISFINITE(pressure)) {
		প্রত্যাগম;
	}

	_report.timestamp_sample = timestamp_sample;
	_report.pressure = pressure;
	_report.timestamp = hrt_absolute_time();

	_failure_config.update();

	যদি (!failure_injection::process(_failure_config, failure_injection_s::FAILURE_UNIT_SENSOR_BARO,
					_sensor_pub.get_instance(), _report, _stuck)) {
		প্রত্যাগম;
	}

	_sensor_pub.publish(_report);
}
