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

#ಅನ್ತರ್ಭಾವ <px4_platform_common/px4_config.h>
#ಅನ್ತರ್ಭಾವ <px4_platform_common/defines.h>
#ಅನ್ತರ್ಭಾವ <px4_platform_common/log.h>

#ಅನ್ತರ್ಭಾವ <inttypes.h>
#ಅನ್ತರ್ಭಾವ <math.h>

ನಾಮಾಕಾಶ calibration
{
ವರ್ಗ DifferentialPressure
{
ಸಾರ್ವಜನಿಕ:
	ಸ್ಥಿರ constexpr ಪೂರ್ಣಾಙ್ಕ MAX_SENSOR_ಎಣಿಕೆ = 3; ///< also the airspeed selector's sensor limit

	ಸ್ಥಿರ constexpr ಧ್ರುವ ವರ್ಣ *SensorString() { ಪ್ರತ್ಯಾಗಮ "DPRES"; }

	DifferentialPressure();
	explicit DifferentialPressure(uint32_t device_id);

	~DifferentialPressure() = ಸ್ವಭಾವ;

	ಶೂನ್ಯ PrintStatus();

	ತಾರ್ಕಿಕ set_calibration_index(ಪೂರ್ಣಾಙ್ಕ calibration_index);
	ಶೂನ್ಯ set_device_id(uint32_t device_id);
	ತಾರ್ಕಿಕ set_offset(ಧ್ರುವ ಭಿನ್ನ &offset);

	ತಾರ್ಕಿಕ calibrated() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ (_device_id != 0) && (_calibration_index >= 0); }
	uint8_t calibration_ಎಣಿಕೆ_2() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ calibration_ಎಣಿಕೆ; }
	int8_t calibration_index() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ _calibration_index; }
	uint32_t device_id() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ _device_id; }
	ತಾರ್ಕಿಕ external() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ _external; }
	ಧ್ರುವ ಭಿನ್ನ &offset() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ _offset; }

	/// remove the zero offset from a raw differential pressure reading [Pa]
	ಅನ್ತಃಸ್ಥ ಭಿನ್ನ Correct(ಧ್ರುವ ಭಿನ್ನ &data) ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ data - _offset; }

	ಅನ್ತಃಸ್ಥ ಭಿನ್ನ Uncorrect(ಧ್ರುವ ಭಿನ್ನ &corrected_data) ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ corrected_data + _offset; }

	ತಾರ್ಕಿಕ ParametersLoad();
	ತಾರ್ಕಿಕ ParametersSave(ಪೂರ್ಣಾಙ್ಕ desired_calibration_index = -1, ತಾರ್ಕಿಕ force = ಅಸತ್ಯ);
	ಶೂನ್ಯ ParametersUpdate();

	/**
	 * Take over the offset left in SENS_DPRES_OFF, using slot 0.
	 * Does nothing if slot 0 is already claimed.
	 *
	 * @return true if the offset was adopted (the caller should save)
	 */
	ತಾರ್ಕಿಕ AdoptLegacyOffset();

	/** mirror slot 0's offset into SENS_DPRES_OFF for ground stations that still read it */
	ಶೂನ್ಯ UpdateLegacyOffset() ಧ್ರುವ;

	ಶೂನ್ಯ Reset();

ಗುಪ್ತ:
	ಸ್ಥಿರ constexpr ಪೂರ್ಣಾಙ್ಕ kLegacyIndex{0};
	ಸ್ಥಿರ constexpr ಧ್ರುವ ವರ್ಣ *kLegacyOffsetParam{"SENS_DPRES_OFF"};

	ಭಿನ್ನ _offset{0};

	int8_t _calibration_index{-1};
	uint32_t _device_id{0};
	uint8_t calibration_ಎಣಿಕೆ{0};

	ತಾರ್ಕಿಕ _external{ಅಸತ್ಯ};
};
} // namespace calibration
