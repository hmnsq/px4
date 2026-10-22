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

#अन्तर्भाव <math.h>

#अन्तर्भाव <drivers/drv_hrt.h>
#अन्तर्भाव <uORB/PublicationMulti.hpp>
#अन्तर्भाव <uORB/topics/sensor_baro.h>

#अन्तर्भाव <lib/failure_injection/FailureInjection.hpp>

वर्ग PX4Barometer
{
सार्वजनिक:
	PX4Barometer(uint32_t device_id);
	PX4Barometer(uint32_t device_id, तार्किक external);

	शून्य set_device_id(uint32_t device_id);
	शून्य set_external(तार्किक external);
	शून्य set_error_गिनती(uint32_t error_गिनती) { _report.error_गिनती = error_गिनती; }
	शून्य set_temperature(भिन्न temperature) { _report.temperature = temperature; }

	शून्य update(ध्रुव hrt_abstime &timestamp_sample, भिन्न pressure);

	पूर्णाङ्क get_instance() { प्रत्यागम _sensor_pub.get_instance(); };
	uint32_t get_device_id() ध्रुव { प्रत्यागम _report.device_id; }

गुप्त:
	uORB::PublicationMulti<sensor_baro_s> _sensor_pub{ORB_ID(sensor_baro)};

	sensor_baro_s	_report{};

	तार्किक		_external_forced{असत्य}; // classification set by the driver, do not re-derive from the device id

	failure_injection::Config _failure_config;
	failure_injection::Stuck<sensor_baro_s> _stuck;
};
