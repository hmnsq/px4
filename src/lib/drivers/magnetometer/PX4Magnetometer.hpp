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

#pragma once

#ضمن <drivers/drv_hrt.h>
#ضمن <lib/conversion/rotation.h>
#ضمن <uORB/PublicationMulti.hpp>
#ضمن <uORB/topics/sensor_mag.h>

#ضمن <lib/failure_injection/FailureInjection.hpp>

ساكن constexpr عائم microTesla2Gauss(عائم micro_tesla) { أرجع micro_tesla * 0.01f; }

صنف PX4Magnetometer
{
عام:
	PX4Magnetometer(uint32_t device_id, تعداد Rotation rotation = ROTATION_NONE);
	PX4Magnetometer(uint32_t device_id, تعداد Rotation rotation, منطقي external);
	~PX4Magnetometer();

	فراغ set_device_id(uint32_t device_id);
	فراغ set_device_type(uint8_t devtype);
	فراغ set_external(منطقي external);
	فراغ set_error_گنتی(uint32_t error_گنتی) { error_گنتی_2 = error_گنتی; }
	فراغ set_range(عائم range) { _range = range; }
	فراغ set_scale(عائم scale) { _scale = scale; }
	فراغ set_temperature(عائم temperature) { _temperature = temperature; }

	فراغ update(ثابت hrt_abstime &timestamp_sample, عائم x, عائم y, عائم z);

	صحيح get_instance() { أرجع _sensor_pub.get_instance(); };
	uint32_t get_device_id() ثابت { أرجع _device_id; }

خاص:
	uORB::PublicationMulti<sensor_mag_s> _sensor_pub{ORB_ID(sensor_mag)};

	uint32_t		_device_id{0};
	ثابت تعداد Rotation	_rotation;

	منطقي			_is_external{خطأ};
	منطقي			_external_forced{خطأ}; // classification set by the driver, do not re-derive from the device id

	عائم			_range{0.f};
	عائم			_scale{1.f};
	عائم			_temperature{NAN};
	uint32_t		error_گنتی_2{0};

	failure_injection::Config _failure_config;
	failure_injection::Stuck<sensor_mag_s> _stuck;
};
