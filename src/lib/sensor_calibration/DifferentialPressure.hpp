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

#அந்தர்பாவ <px4_platform_common/px4_config.h>
#அந்தர்பாவ <px4_platform_common/defines.h>
#அந்தர்பாவ <px4_platform_common/log.h>

#அந்தர்பாவ <inttypes.h>
#அந்தர்பாவ <math.h>

நாமாகாஶ calibration
{
வகுப்பு DifferentialPressure
{
பொது:
	நிலையான constexpr முழு MAX_SENSOR_மொத்தம் = 3; ///< also the airspeed selector's sensor limit

	நிலையான constexpr மாறிலி எழுத்து *SensorString() { திரும்பு "DPRES"; }

	DifferentialPressure();
	explicit DifferentialPressure(uint32_t device_id);

	~DifferentialPressure() = இயல்பு;

	வெறுமை PrintStatus();

	தருக்க set_calibration_index(முழு calibration_index);
	வெறுமை set_device_id(uint32_t device_id);
	தருக்க set_offset(மாறிலி தசம &offset);

	தருக்க calibrated() மாறிலி { திரும்பு (_device_id != 0) && (_calibration_index >= 0); }
	uint8_t calibration_மொத்தம்_2() மாறிலி { திரும்பு calibration_மொத்தம்; }
	int8_t calibration_index() மாறிலி { திரும்பு _calibration_index; }
	uint32_t device_id() மாறிலி { திரும்பு _device_id; }
	தருக்க external() மாறிலி { திரும்பு _external; }
	மாறிலி தசம &offset() மாறிலி { திரும்பு _offset; }

	/// remove the zero offset from a raw differential pressure reading [Pa]
	உள்வரி தசம Correct(மாறிலி தசம &data) மாறிலி { திரும்பு data - _offset; }

	உள்வரி தசம Uncorrect(மாறிலி தசம &corrected_data) மாறிலி { திரும்பு corrected_data + _offset; }

	தருக்க ParametersLoad();
	தருக்க ParametersSave(முழு desired_calibration_index = -1, தருக்க force = பொய்);
	வெறுமை ParametersUpdate();

	/**
	 * Take over the offset left in SENS_DPRES_OFF, using slot 0.
	 * Does nothing if slot 0 is already claimed.
	 *
	 * @return true if the offset was adopted (the caller should save)
	 */
	தருக்க AdoptLegacyOffset();

	/** mirror slot 0's offset into SENS_DPRES_OFF for ground stations that still read it */
	வெறுமை UpdateLegacyOffset() மாறிலி;

	வெறுமை Reset();

தனிப்பட்ட:
	நிலையான constexpr முழு kLegacyIndex{0};
	நிலையான constexpr மாறிலி எழுத்து *kLegacyOffsetParam{"SENS_DPRES_OFF"};

	தசம _offset{0};

	int8_t _calibration_index{-1};
	uint32_t _device_id{0};
	uint8_t calibration_மொத்தம்{0};

	தருக்க _external{பொய்};
};
} // namespace calibration
