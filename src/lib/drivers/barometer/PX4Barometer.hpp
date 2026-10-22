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

#ಅನ್ತರ್ಭಾವ <math.h>

#ಅನ್ತರ್ಭಾವ <drivers/drv_hrt.h>
#ಅನ್ತರ್ಭಾವ <uORB/PublicationMulti.hpp>
#ಅನ್ತರ್ಭಾವ <uORB/topics/sensor_baro.h>

#ಅನ್ತರ್ಭಾವ <lib/failure_injection/FailureInjection.hpp>

ವರ್ಗ PX4Barometer
{
ಸಾರ್ವಜನಿಕ:
	PX4Barometer(uint32_t device_id);
	PX4Barometer(uint32_t device_id, ತಾರ್ಕಿಕ external);

	ಶೂನ್ಯ set_device_id(uint32_t device_id);
	ಶೂನ್ಯ set_external(ತಾರ್ಕಿಕ external);
	ಶೂನ್ಯ set_error_ಎಣಿಕೆ(uint32_t error_ಎಣಿಕೆ) { _report.error_ಎಣಿಕೆ = error_ಎಣಿಕೆ; }
	ಶೂನ್ಯ set_temperature(ಭಿನ್ನ temperature) { _report.temperature = temperature; }

	ಶೂನ್ಯ update(ಧ್ರುವ hrt_abstime &timestamp_sample, ಭಿನ್ನ pressure);

	ಪೂರ್ಣಾಙ್ಕ get_instance() { ಪ್ರತ್ಯಾಗಮ _sensor_pub.get_instance(); };
	uint32_t get_device_id() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ _report.device_id; }

ಗುಪ್ತ:
	uORB::PublicationMulti<sensor_baro_s> _sensor_pub{ORB_ID(sensor_baro)};

	sensor_baro_s	_report{};

	ತಾರ್ಕಿಕ		_external_forced{ಅಸತ್ಯ}; // classification set by the driver, do not re-derive from the device id

	failure_injection::Config _failure_config;
	failure_injection::Stuck<sensor_baro_s> _stuck;
};
