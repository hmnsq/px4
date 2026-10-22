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

#ضمن <px4_platform_common/px4_config.h>
#ضمن <px4_platform_common/defines.h>
#ضمن <px4_platform_common/log.h>

#ضمن <inttypes.h>
#ضمن <math.h>

نطاق calibration
{
صنف DifferentialPressure
{
عام:
	ساكن constexpr صحيح MAX_SENSOR_تعداد = 3; ///< also the airspeed selector's sensor limit

	ساكن constexpr ثابت حرف *SensorString() { إرجاع "DPRES"; }

	DifferentialPressure();
	explicit DifferentialPressure(uint32_t device_id);

	~DifferentialPressure() = افتراضي;

	فراغ PrintStatus();

	منطقي set_calibration_index(صحيح calibration_index);
	فراغ set_device_id(uint32_t device_id);
	منطقي set_offset(ثابت عائم &offset);

	منطقي calibrated() ثابت { إرجاع (_device_id != 0) && (_calibration_index >= 0); }
	uint8_t calibration_تعداد_2() ثابت { إرجاع calibration_تعداد; }
	int8_t calibration_index() ثابت { إرجاع _calibration_index; }
	uint32_t device_id() ثابت { إرجاع _device_id; }
	منطقي external() ثابت { إرجاع _external; }
	ثابت عائم &offset() ثابت { إرجاع _offset; }

	/// remove the zero offset from a raw differential pressure reading [Pa]
	مضمن عائم Correct(ثابت عائم &data) ثابت { إرجاع data - _offset; }

	مضمن عائم Uncorrect(ثابت عائم &corrected_data) ثابت { إرجاع corrected_data + _offset; }

	منطقي ParametersLoad();
	منطقي ParametersSave(صحيح desired_calibration_index = -1, منطقي force = خطأ);
	فراغ ParametersUpdate();

	/**
	 * Take over the offset left in SENS_DPRES_OFF, using slot 0.
	 * Does nothing if slot 0 is already claimed.
	 *
	 * @return true if the offset was adopted (the caller should save)
	 */
	منطقي AdoptLegacyOffset();

	/** mirror slot 0's offset into SENS_DPRES_OFF for ground stations that still read it */
	فراغ UpdateLegacyOffset() ثابت;

	فراغ Reset();

خاص:
	ساكن constexpr صحيح kLegacyIndex{0};
	ساكن constexpr ثابت حرف *kLegacyOffsetParam{"SENS_DPRES_OFF"};

	عائم _offset{0};

	int8_t _calibration_index{-1};
	uint32_t _device_id{0};
	uint8_t calibration_تعداد{0};

	منطقي _external{خطأ};
};
} // namespace calibration
