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

#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ <math.h>

#ਅਨ੍ਤਰ੍ਭਾਵ <drivers/drv_hrt.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <uORB/PublicationMulti.hpp>
#ਅਨ੍ਤਰ੍ਭਾਵ <uORB/topics/sensor_baro.h>

#ਅਨ੍ਤਰ੍ਭਾਵ <lib/failure_injection/FailureInjection.hpp>

ਵਰਗ PX4Barometer
{
ਜਨਤਕ:
	PX4Barometer(uint32_t device_id);
	PX4Barometer(uint32_t device_id, ਤਾਰਕਿਕ external);

	ਖਾਲੀ ਸੈੱਟ_device_id(uint32_t device_id);
	ਖਾਲੀ ਸੈੱਟ_external(ਤਾਰਕਿਕ external);
	ਖਾਲੀ ਸੈੱਟ_error_ਗਿਣਤੀ(uint32_t error_ਗਿਣਤੀ) { _report.error_ਗਿਣਤੀ = error_ਗਿਣਤੀ; }
	ਖਾਲੀ ਸੈੱਟ_temperature(ਦਸ਼ਮਲਵ temperature) { _report.temperature = temperature; }

	ਖਾਲੀ update(ਅਚਲ hrt_abstime &timestamp_sample, ਦਸ਼ਮਲਵ pressure);

	ਪੂਰਨ get_instance() { ਵਾਪਸ _sensor_pub.get_instance(); };
	uint32_t get_device_id() ਅਚਲ { ਵਾਪਸ _report.device_id; }

ਨਿਜੀ:
	uORB::PublicationMulti<sensor_baro_s> _sensor_pub{ORB_ID(sensor_baro)};

	sensor_baro_s	_report{};

	ਤਾਰਕਿਕ		_external_forced{ਝੂਠ}; // classification set by the driver, do not re-derive from the device id

	failure_injection::Config _failure_config;
	failure_injection::Stuck<sensor_baro_s> _stuck;
};
