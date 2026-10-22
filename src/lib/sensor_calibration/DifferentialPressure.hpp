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

#অন্তর্ভাৱ <px4_platform_common/px4_config.h>
#অন্তর্ভাৱ <px4_platform_common/defines.h>
#অন্তর্ভাৱ <px4_platform_common/log.h>

#অন্তর্ভাৱ <inttypes.h>
#অন্তর্ভাৱ <math.h>

নামাকাশ calibration
{
শ্ৰেণী DifferentialPressure
{
সার্ৱজনিক:
	স্থির constexpr পূর্ণাঙ্ক MAX_SENSOR_গুনতি = 3; ///< also the airspeed selector's sensor limit

	স্থির constexpr ধ্রুৱ ৱর্ণ *SensorString() { ঘূৰাই "DPRES"; }

	DifferentialPressure();
	explicit DifferentialPressure(uint32_t device_id);

	~DifferentialPressure() = স্ৱভাৱ;

	শূন্য PrintStatus();

	তার্কিক set_calibration_index(পূর্ণাঙ্ক calibration_index);
	শূন্য set_device_id(uint32_t device_id);
	তার্কিক set_offset(ধ্রুৱ ভিন্ন &offset);

	তার্কিক calibrated() ধ্রুৱ { ঘূৰাই (_device_id != 0) && (_calibration_index >= 0); }
	uint8_t calibration_গুনতি_2() ধ্রুৱ { ঘূৰাই calibration_গুনতি; }
	int8_t calibration_index() ধ্রুৱ { ঘূৰাই _calibration_index; }
	uint32_t device_id() ধ্রুৱ { ঘূৰাই _device_id; }
	তার্কিক external() ধ্রুৱ { ঘূৰাই _external; }
	ধ্রুৱ ভিন্ন &offset() ধ্রুৱ { ঘূৰাই _offset; }

	/// remove the zero offset from a raw differential pressure reading [Pa]
	অন্তঃস্থ ভিন্ন Correct(ধ্রুৱ ভিন্ন &data) ধ্রুৱ { ঘূৰাই data - _offset; }

	অন্তঃস্থ ভিন্ন Uncorrect(ধ্রুৱ ভিন্ন &corrected_data) ধ্রুৱ { ঘূৰাই corrected_data + _offset; }

	তার্কিক ParametersLoad();
	তার্কিক ParametersSave(পূর্ণাঙ্ক desired_calibration_index = -1, তার্কিক force = মিছা);
	শূন্য ParametersUpdate();

	/**
	 * Take over the offset left in SENS_DPRES_OFF, using slot 0.
	 * Does nothing if slot 0 is already claimed.
	 *
	 * @return true if the offset was adopted (the caller should save)
	 */
	তার্কিক AdoptLegacyOffset();

	/** mirror slot 0's offset into SENS_DPRES_OFF for ground stations that still read it */
	শূন্য UpdateLegacyOffset() ধ্রুৱ;

	শূন্য Reset();

গুপ্ত:
	স্থির constexpr পূর্ণাঙ্ক kLegacyIndex{0};
	স্থির constexpr ধ্রুৱ ৱর্ণ *kLegacyOffsetParam{"SENS_DPRES_OFF"};

	ভিন্ন _offset{0};

	int8_t _calibration_index{-1};
	uint32_t _device_id{0};
	uint8_t calibration_গুনতি{0};

	তার্কিক _external{মিছা};
};
} // namespace calibration
