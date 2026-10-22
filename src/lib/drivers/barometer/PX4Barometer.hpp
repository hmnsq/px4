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

#ଅନ୍ତର୍ଭାଵ <math.h>

#ଅନ୍ତର୍ଭାଵ <drivers/drv_hrt.h>
#ଅନ୍ତର୍ଭାଵ <uORB/PublicationMulti.hpp>
#ଅନ୍ତର୍ଭାଵ <uORB/topics/sensor_baro.h>

#ଅନ୍ତର୍ଭାଵ <lib/failure_injection/FailureInjection.hpp>

ଶ୍ରେଣୀ PX4Barometer
{
ସର୍ବସାଧାରଣ:
	PX4Barometer(uint32_t device_id);
	PX4Barometer(uint32_t device_id, ତାର୍କିକ external);

	ଶୂନ୍ୟ set_device_id(uint32_t device_id);
	ଶୂନ୍ୟ set_external(ତାର୍କିକ external);
	ଶୂନ୍ୟ set_error_ଗଣନା(uint32_t error_ଗଣନା) { _report.error_ଗଣନା = error_ଗଣନା; }
	ଶୂନ୍ୟ set_temperature(ଦଶମିକ temperature) { _report.temperature = temperature; }

	ଶୂନ୍ୟ update(ଅଚଳ hrt_abstime &timestamp_sample, ଦଶମିକ pressure);

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_instance() { ଫେରନ୍ତୁ _sensor_pub.get_instance(); };
	uint32_t get_device_id() ଅଚଳ { ଫେରନ୍ତୁ _report.device_id; }

ବ୍ୟକ୍ତିଗତ:
	uORB::PublicationMulti<sensor_baro_s> _sensor_pub{ORB_ID(sensor_baro)};

	sensor_baro_s	_report{};

	ତାର୍କିକ		_external_forced{ମିଥ୍ୟା}; // classification set by the driver, do not re-derive from the device id

	failure_injection::Config _failure_config;
	failure_injection::Stuck<sensor_baro_s> _stuck;
};
