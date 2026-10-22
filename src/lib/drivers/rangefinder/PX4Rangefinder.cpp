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

#అన్తర్భావ "PX4Rangefinder.hpp"

#అన్తర్భావ <lib/drivers/device/Device.hpp>

ఉపయోగించి నామాకాశ సమయం_literals;

PX4Rangefinder::PX4Rangefinder(స్థిరరాశి uint32_t device_id, స్థిరరాశి uint8_t device_orientation)
{
	సమితి_device_id(device_id);
	సమితి_orientation(device_orientation);
	సమితి_rangefinder_type(దూరం_sensor_s::MAV_దూరం_SENSOR_LASER);
	సమితి_బహుళకం(దూరం_sensor_s::బహుళకం_UNKNOWN);
}

PX4Rangefinder::~PX4Rangefinder()
{
	దూరం_sensor_pub_2.unadvertise();
}

శూన్య PX4Rangefinder::సమితి_device_type(uint8_t device_type)
{
	// current DeviceStructure
	సంఘం device::Device::DeviceId device_id;
	device_id.devid = దూరం_sensor_pub_2.get().device_id;

	// update to new device type
	device_id.devid_s.devtype = device_type;

	// copy back to report
	దూరం_sensor_pub_2.get().device_id = device_id.devid;
}

శూన్య PX4Rangefinder::సమితి_orientation(స్థిరరాశి uint8_t device_orientation)
{
	దూరం_sensor_pub_2.get().orientation = device_orientation;
}

శూన్య PX4Rangefinder::update(స్థిరరాశి hrt_abstime &timestamp_sample, స్థిరరాశి దశాంశం దూరం, స్థిరరాశి int8_t quality, స్థిరరాశి దశాంశం *q, uint8_t q_len)
{
	దూరం_sensor_s &report = దూరం_sensor_pub_2.get();
	report.timestamp = timestamp_sample;
	report.current_దూరం = దూరం;
	report.signal_quality = quality;

	// if quality is unavailable (-1) set to 0 if distance is outside bounds
	ఒకవేళ (quality < 0) {
		ఒకవేళ ((దూరం < report.min_దూరం) || (దూరం > report.max_దూరం)) {
			report.signal_quality = 0;
		}
	}

	// Update the quaternion in the sample update
	ఒకవేళ (q != శూన్యసూచిక) {
		జ్ఞాపకం_నకలు(report.q, q, పరిమాణం(దశాంశం) * q_len);
	}

	_failure_config.update();

	ఒకవేళ (!failure_injection::process(_failure_config, failure_injection_s::FAILURE_UNIT_SENSOR_దూరం_SENSOR,
					దూరం_sensor_pub_2.get_instance(), report, _stuck)) {
		తిరిగి;
	}

	దూరం_sensor_pub_2.update();
}
