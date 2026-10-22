/****************************************************************************
 *
 *   Copyright (c) 2018-2022 PX4 Development Team. All rights reserved.
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

#شامل <drivers/drv_hrt.h>
#شامل <lib/conversion/rotation.h>
#شامل <uORB/PublicationMulti.hpp>
#شامل <uORB/topics/sensor_gyro.h>
#شامل <uORB/topics/sensor_gyro_fifo.h>

#شامل <lib/failure_injection/FailureInjection.hpp>

رده PX4Gyroscope
{
عمومی:
	PX4Gyroscope(uint32_t device_id, شمارش Rotation rotation = ROTATION_NONE);
	PX4Gyroscope(uint32_t device_id, شمارش Rotation rotation, منطقی external);
	~PX4Gyroscope();


	تهی set_device_id(uint32_t device_id);
	تهی set_device_type(uint8_t devtype);
	تهی set_external(منطقی external);
	تهی set_error_شمار(uint32_t error_شمار) { error_شمار_2 = error_شمار; }
	تهی set_range(اعشاری range) { _range = range; UpdateClipLimit(); }
	تهی set_scale(اعشاری scale);
	تهی set_temperature(اعشاری temperature) { _temperature = temperature; }

	تهی update(ثابت hrt_abstime &timestamp_sample, اعشاری x, اعشاری y, اعشاری z);

	تهی updateFIFO(sensor_gyro_fifo_s &sample);

	uint32_t get_device_id() ثابت { بازگشت _device_id; }
	صحیح get_instance() { بازگشت _sensor_pub.get_instance(); };
	int32_t get_max_rate_hz() ثابت { بازگشت math::constrain(_imu_gyro_rate_max, static_cast<int32_t>(100), static_cast<int32_t>(4000)); }

خصوصی:
	تهی UpdateClipLimit();

	uORB::PublicationMulti<sensor_gyro_s> _sensor_pub{ORB_ID(sensor_gyro)};
	uORB::PublicationMulti<sensor_gyro_fifo_s>  _sensor_fifo_pub{ORB_ID(sensor_gyro_fifo)};

	uint32_t		_device_id{0};
	ثابت شمارش Rotation	_rotation;

	منطقی			_is_external{نادرست};
	منطقی			_external_forced{نادرست}; // classification set by the driver, do not re-derive from the device id

	int32_t			_imu_gyro_rate_max{0};

	اعشاری			_range{math::radians(2000.f)};
	اعشاری			_scale{1.f};
	اعشاری			_temperature{NAN};

	اعشاری			_clip_limit{_range / _scale};

	uint32_t		error_شمار_2{0};

	int16_t			_last_sample[3] {};

	failure_injection::Config _failure_config;
	failure_injection::Stuck<sensor_gyro_s> _stuck;
	failure_injection::Stuck<sensor_gyro_fifo_s> _stuck_fifo;
};
