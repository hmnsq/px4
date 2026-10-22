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

#ಸೂಚನೆ once

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
	ಸ್ಥಿರ constexpr ಪೂರ್ಣಾಂಕ MAX_SENSOR_ಎಣಿಕೆ = 3; ///< also the airspeed selector's sensor limit

	ಸ್ಥಿರ constexpr ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *SensorString() { ಹಿಂತಿರುಗಿ "DPRES"; }

	DifferentialPressure();
	ಸ್ಪಷ್ಟ DifferentialPressure(uint32_t device_id);

	~DifferentialPressure() = ಪೂರ್ವನಿಯೋಜಿತ;

	ಶೂನ್ಯ Print_ಸ್ಥಿತಿ();

	ತಾರ್ಕಿಕ ಗಣ_calibration_ಸೂಚ್ಯಂಕ(ಪೂರ್ಣಾಂಕ calibration_ಸೂಚ್ಯಂಕ);
	ಶೂನ್ಯ ಗಣ_device_id(uint32_t device_id);
	ತಾರ್ಕಿಕ ಗಣ_ಸ್ಥಾನಾಂತರ(ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ &ಸ್ಥಾನಾಂತರ);

	ತಾರ್ಕಿಕ calibrated() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ (_device_id != 0) && (calibration_ಸೂಚ್ಯಂಕ_2 >= 0); }
	uint8_t calibration_ಎಣಿಕೆ_2() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ calibration_ಎಣಿಕೆ; }
	int8_t calibration_ಸೂಚ್ಯಂಕ() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ calibration_ಸೂಚ್ಯಂಕ_2; }
	uint32_t device_id() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ _device_id; }
	ತಾರ್ಕಿಕ external() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ _external; }
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ &ಸ್ಥಾನಾಂತರ() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ಸ್ಥಾನಾಂತರ_2; }

	/// remove the zero offset from a raw differential pressure reading [Pa]
	ಸಾಲೊಳಗೆ ದಶಮಾಂಶ Correct(ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ &ದತ್ತಾಂಶ) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ದತ್ತಾಂಶ - ಸ್ಥಾನಾಂತರ_2; }

	ಸಾಲೊಳಗೆ ದಶಮಾಂಶ Uncorrect(ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ &corrected_ದತ್ತಾಂಶ) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ corrected_ದತ್ತಾಂಶ + ಸ್ಥಾನಾಂತರ_2; }

	ತಾರ್ಕಿಕ Parameters_ತುಂಬು();
	ತಾರ್ಕಿಕ ParametersSave(ಪೂರ್ಣಾಂಕ desired_calibration_ಸೂಚ್ಯಂಕ = -1, ತಾರ್ಕಿಕ force = ಸುಳ್ಳು);
	ಶೂನ್ಯ ParametersUpdate();

	/**
	 * Take over the offset left in SENS_DPRES_OFF, using slot 0.
	 * Does nothing if slot 0 is already claimed.
	 *
	 * @return true if the offset was adopted (the caller should save)
	 */
	ತಾರ್ಕಿಕ Adopt_Legacy_ಸ್ಥಾನಾಂತರ();

	/** mirror slot 0's offset into SENS_DPRES_OFF for ground stations that still read it */
	ಶೂನ್ಯ Update_Legacy_ಸ್ಥಾನಾಂತರ() ಸ್ಥಿರಾಂಕ;

	ಶೂನ್ಯ Reset();

ಖಾಸಗಿ:
	ಸ್ಥಿರ constexpr ಪೂರ್ಣಾಂಕ k_Legacy_ಸೂಚ್ಯಂಕ{0};
	ಸ್ಥಿರ constexpr ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *k_Legacy_ಸ್ಥಾನಾಂತರ_Param{"SENS_DPRES_OFF"};

	ದಶಮಾಂಶ ಸ್ಥಾನಾಂತರ_2{0};

	int8_t calibration_ಸೂಚ್ಯಂಕ_2{-1};
	uint32_t _device_id{0};
	uint8_t calibration_ಎಣಿಕೆ{0};

	ತಾರ್ಕಿಕ _external{ಸುಳ್ಳು};
};
} // namespace calibration
