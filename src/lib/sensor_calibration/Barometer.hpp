/****************************************************************************
 *
 *   Copyright (c) 2022 PX4 Development Team. All rights reserved.
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

#شامل <px4_platform_common/px4_config.h>
#شامل <px4_platform_common/log.h>
#شامل <uORB/Subscription.hpp>
#شامل <uORB/topics/sensor_correction.h>

فضاینام calibration
{
رده Barometer
{
عمومی:
	ایستا constexpr صحیح MAX_SENSOR_شمار = 4;

	ایستا constexpr uint8_t DEFAULT_PRIORITY = 50;
	ایستا constexpr uint8_t DEFAULT_EXTERNAL_PRIORITY = 75;

	ایستا constexpr ثابت نویسه *SensorString() { بازگشت "BARO"; }

	Barometer();
	explicit Barometer(uint32_t device_id);
	Barometer(uint32_t device_id, منطقی external);

	~Barometer() = پیشفرض;

	تهی PrintStatus();

	منطقی set_calibration_index(صحیح calibration_index);
	تهی set_device_id(uint32_t device_id); ///< classification falls back to the device id's bus
	تهی set_device_id(uint32_t device_id, منطقی external);
	منطقی set_offset(ثابت اعشاری &offset);

	منطقی calibrated() ثابت { بازگشت (_device_id != 0) && (_calibration_index >= 0); }
	uint8_t calibration_شمار_2() ثابت { بازگشت calibration_شمار; }
	int8_t calibration_index() ثابت { بازگشت _calibration_index; }
	uint32_t device_id() ثابت { بازگشت _device_id; }
	منطقی enabled() ثابت { بازگشت (_priority > 0); }
	منطقی external() ثابت { بازگشت _external; }
	ثابت اعشاری &offset() ثابت { بازگشت _offset; }
	ثابت int32_t &priority() ثابت { بازگشت _priority; }
	ثابت اعشاری &thermal_offset() ثابت { بازگشت _thermal_offset; }

	// apply offsets
	درونخطی اعشاری Correct(ثابت اعشاری &data) ثابت
	{
		بازگشت data - _thermal_offset - _offset;
	}

	درونخطی اعشاری Uncorrect(ثابت اعشاری &corrected_data) ثابت
	{
		بازگشت corrected_data + _thermal_offset + _offset;
	}

	// Compute sensor offset from bias (board frame)
	اعشاری BiasCorrectedSensorOffset(ثابت اعشاری &bias) ثابت
	{
		بازگشت bias + _thermal_offset + _offset;
	}

	منطقی ParametersLoad();
	منطقی ParametersSave(صحیح desired_calibration_index = -1, منطقی force = نادرست);
	تهی ParametersUpdate();

	تهی Reset();

	تهی SensorCorrectionsUpdate(منطقی force = نادرست);

خصوصی:
	uORB::Subscription _sensor_correction_sub{ORB_ID(sensor_correction)};

	اعشاری _offset{0};
	اعشاری _thermal_offset{0};

	int8_t _calibration_index{-1};
	uint32_t _device_id{0};
	int32_t _priority{-1};

	منطقی _external{نادرست};

	uint8_t calibration_شمار{0};
};
} // namespace calibration
