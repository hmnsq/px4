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

#ضمن <px4_platform_common/px4_config.h>
#ضمن <px4_platform_common/log.h>
#ضمن <uORB/Subscription.hpp>
#ضمن <uORB/topics/sensor_correction.h>

نطاق calibration
{
صنف Barometer
{
عام:
	ساكن constexpr صحيح MAX_SENSOR_COUNT = 4;

	ساكن constexpr uint8_t DEFAULT_PRIORITY = 50;
	ساكن constexpr uint8_t DEFAULT_EXTERNAL_PRIORITY = 75;

	ساكن constexpr ثابت حرف *SensorString() { ܦܢܐ "BARO"; }

	Barometer();
	explicit Barometer(uint32_t device_id);
	Barometer(uint32_t device_id, منطقي external);

	~Barometer() = افتراضي;

	فراغ PrintStatus();

	منطقي set_calibration_index(صحيح calibration_index);
	فراغ set_device_id(uint32_t device_id); ///< classification falls back to the device id's bus
	فراغ set_device_id(uint32_t device_id, منطقي external);
	منطقي set_offset(ثابت عائم &offset);

	منطقي calibrated() ثابت { ܦܢܐ (_device_id != 0) && (_calibration_index >= 0); }
	uint8_t calibration_count() ثابت { ܦܢܐ _calibration_count; }
	int8_t calibration_index() ثابت { ܦܢܐ _calibration_index; }
	uint32_t device_id() ثابت { ܦܢܐ _device_id; }
	منطقي enabled() ثابت { ܦܢܐ (_priority > 0); }
	منطقي external() ثابت { ܦܢܐ _external; }
	ثابت عائم &offset() ثابت { ܦܢܐ _offset; }
	ثابت int32_t &priority() ثابت { ܦܢܐ _priority; }
	ثابت عائم &thermal_offset() ثابت { ܦܢܐ _thermal_offset; }

	// apply offsets
	مضمن عائم Correct(ثابت عائم &data) ثابت
	{
		ܦܢܐ data - _thermal_offset - _offset;
	}

	مضمن عائم Uncorrect(ثابت عائم &corrected_data) ثابت
	{
		ܦܢܐ corrected_data + _thermal_offset + _offset;
	}

	// Compute sensor offset from bias (board frame)
	عائم BiasCorrectedSensorOffset(ثابت عائم &bias) ثابت
	{
		ܦܢܐ bias + _thermal_offset + _offset;
	}

	منطقي ParametersLoad();
	منطقي ParametersSave(صحيح desired_calibration_index = -1, منطقي force = ܕܓܠܐ);
	فراغ ParametersUpdate();

	فراغ Reset();

	فراغ SensorCorrectionsUpdate(منطقي force = ܕܓܠܐ);

خاص:
	uORB::Subscription _sensor_correction_sub{ORB_ID(sensor_correction)};

	عائم _offset{0};
	عائم _thermal_offset{0};

	int8_t _calibration_index{-1};
	uint32_t _device_id{0};
	int32_t _priority{-1};

	منطقي _external{ܕܓܠܐ};

	uint8_t _calibration_count{0};
};
} // namespace calibration
