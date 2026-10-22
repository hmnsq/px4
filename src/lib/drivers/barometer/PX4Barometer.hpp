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

#pragma once

#ضمن <math.h>

#ضمن <drivers/drv_hrt.h>
#ضمن <uORB/PublicationMulti.hpp>
#ضمن <uORB/topics/sensor_baro.h>

#ضمن <lib/failure_injection/FailureInjection.hpp>

صنف PX4Barometer
{
عام:
	PX4Barometer(uint32_t device_id);
	PX4Barometer(uint32_t device_id, منطقي external);

	فراغ set_device_id(uint32_t device_id);
	فراغ set_external(منطقي external);
	فراغ set_error_تعداد(uint32_t error_تعداد) { _report.error_تعداد = error_تعداد; }
	فراغ set_temperature(عائم temperature) { _report.temperature = temperature; }

	فراغ update(ثابت hrt_abstime &timestamp_sample, عائم pressure);

	صحيح get_instance() { إرجاع _sensor_pub.get_instance(); };
	uint32_t get_device_id() ثابت { إرجاع _report.device_id; }

خاص:
	uORB::PublicationMulti<sensor_baro_s> _sensor_pub{ORB_ID(sensor_baro)};

	sensor_baro_s	_report{};

	منطقي		_external_forced{خطأ}; // classification set by the driver, do not re-derive from the device id

	failure_injection::Config _failure_config;
	failure_injection::Stuck<sensor_baro_s> _stuck;
};
