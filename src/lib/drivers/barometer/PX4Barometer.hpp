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

#సూచన once

#అన్తర్భావ <math.h>

#అన్తర్భావ <drivers/drv_hrt.h>
#అన్తర్భావ <uORB/PublicationMulti.hpp>
#అన్తర్భావ <uORB/topics/sensor_baro.h>

#అన్తర్భావ <lib/failure_injection/FailureInjection.hpp>

తరగతి PX4Barometer
{
బహిరంగ:
	PX4Barometer(uint32_t device_id);
	PX4Barometer(uint32_t device_id, తార్కిక external);

	శూన్య సమితి_device_id(uint32_t device_id);
	శూన్య సమితి_external(తార్కిక external);
	శూన్య సమితి_error_లెక్క(uint32_t error_లెక్క) { _report.error_లెక్క = error_లెక్క; }
	శూన్య సమితి_temperature(దశాంశం temperature) { _report.temperature = temperature; }

	శూన్య update(స్థిరరాశి hrt_abstime &timestamp_sample, దశాంశం pressure);

	పూర్ణాంకం get_instance() { తిరిగి _sensor_pub.get_instance(); };
	uint32_t get_device_id() స్థిరరాశి { తిరిగి _report.device_id; }

అంతరంగ:
	uORB::PublicationMulti<sensor_baro_s> _sensor_pub{ORB_ID(sensor_baro)};

	sensor_baro_s	_report{};

	తార్కిక		_external_forced{అబద్ధం}; // classification set by the driver, do not re-derive from the device id

	failure_injection::Config _failure_config;
	failure_injection::Stuck<sensor_baro_s> _stuck;
};
