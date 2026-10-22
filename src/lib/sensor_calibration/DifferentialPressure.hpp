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

#অন্তর্ভাব <px4_platform_common/px4_config.h>
#অন্তর্ভাব <px4_platform_common/defines.h>
#অন্তর্ভাব <px4_platform_common/log.h>

#অন্তর্ভাব <inttypes.h>
#অন্তর্ভাব <math.h>

নামাকাশ calibration
{
শ্রেণী DifferentialPressure
{
সর্বজনীন:
	স্থির constexpr পূর্ণসংখ্যা MAX_SENSOR_গুনতি = 3; ///< also the airspeed selector's sensor limit

	স্থির constexpr ধ্রুবক অক্ষর *SensorString() { ফেরত "DPRES"; }

	DifferentialPressure();
	explicit DifferentialPressure(uint32_t device_id);

	~DifferentialPressure() = পূর্বনির্ধারিত;

	শূন্য PrintStatus();

	যৌক্তিক set_calibration_সূচক(পূর্ণসংখ্যা calibration_সূচক);
	শূন্য set_device_id(uint32_t device_id);
	যৌক্তিক set_offset(ধ্রুবক দশমিক &offset);

	যৌক্তিক calibrated() ধ্রুবক { ফেরত (_device_id != 0) && (calibration_সূচক_2 >= 0); }
	uint8_t calibration_গুনতি_2() ধ্রুবক { ফেরত calibration_গুনতি; }
	int8_t calibration_সূচক() ধ্রুবক { ফেরত calibration_সূচক_2; }
	uint32_t device_id() ধ্রুবক { ফেরত _device_id; }
	যৌক্তিক external() ধ্রুবক { ফেরত _external; }
	ধ্রুবক দশমিক &offset() ধ্রুবক { ফেরত _offset; }

	/// remove the zero offset from a raw differential pressure reading [Pa]
	অন্তঃরেখ দশমিক Correct(ধ্রুবক দশমিক &তথ্য) ধ্রুবক { ফেরত তথ্য - _offset; }

	অন্তঃরেখ দশমিক Uncorrect(ধ্রুবক দশমিক &corrected_তথ্য) ধ্রুবক { ফেরত corrected_তথ্য + _offset; }

	যৌক্তিক ParametersLoad();
	যৌক্তিক ParametersSave(পূর্ণসংখ্যা desired_calibration_সূচক = -1, যৌক্তিক force = মিথ্যা);
	শূন্য ParametersUpdate();

	/**
	 * Take over the offset left in SENS_DPRES_OFF, using slot 0.
	 * Does nothing if slot 0 is already claimed.
	 *
	 * @return true if the offset was adopted (the caller should save)
	 */
	যৌক্তিক AdoptLegacyOffset();

	/** mirror slot 0's offset into SENS_DPRES_OFF for ground stations that still read it */
	শূন্য UpdateLegacyOffset() ধ্রুবক;

	শূন্য Reset();

ব্যক্তিগত:
	স্থির constexpr পূর্ণসংখ্যা k_Legacy_সূচক{0};
	স্থির constexpr ধ্রুবক অক্ষর *kLegacyOffsetParam{"SENS_DPRES_OFF"};

	দশমিক _offset{0};

	int8_t calibration_সূচক_2{-1};
	uint32_t _device_id{0};
	uint8_t calibration_গুনতি{0};

	যৌক্তিক _external{মিথ্যা};
};
} // namespace calibration
