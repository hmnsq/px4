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

#निर्देशः once

#अन्तर्भाव <px4_platform_common/px4_config.h>
#अन्तर्भाव <px4_platform_common/defines.h>
#अन्तर्भाव <px4_platform_common/log.h>

#अन्तर्भाव <inttypes.h>
#अन्तर्भाव <math.h>

नामाकाश calibration
{
वर्ग DifferentialPressure
{
सार्वजनिक:
	स्थिर constexpr पूर्णाङ्क MAX_SENSOR_गणनम् = 3; ///< also the airspeed selector's sensor limit

	स्थिर constexpr ध्रुव वर्ण *SensorString() { प्रत्यागम "DPRES"; }

	DifferentialPressure();
	स्पष्ट DifferentialPressure(uint32_t device_id);

	~DifferentialPressure() = स्वभाव;

	शून्य Print_स्थितिः();

	तार्किक समुच्चयः_calibration_सूचकांक(पूर्णाङ्क calibration_सूचकांक);
	शून्य समुच्चयः_device_id(uint32_t device_id);
	तार्किक समुच्चयः_विस्थापनम्(ध्रुव भिन्न &विस्थापनम्);

	तार्किक calibrated() ध्रुव { प्रत्यागम (_device_id != 0) && (calibration_सूचकांक_2 >= 0); }
	uint8_t calibration_गणनम्_2() ध्रुव { प्रत्यागम calibration_गणनम्; }
	int8_t calibration_सूचकांक() ध्रुव { प्रत्यागम calibration_सूचकांक_2; }
	uint32_t device_id() ध्रुव { प्रत्यागम _device_id; }
	तार्किक external() ध्रुव { प्रत्यागम _external; }
	ध्रुव भिन्न &विस्थापनम्() ध्रुव { प्रत्यागम विस्थापनम्_2; }

	/// remove the zero offset from a raw differential pressure reading [Pa]
	अन्तःस्थ भिन्न Correct(ध्रुव भिन्न &दत्त) ध्रुव { प्रत्यागम दत्त - विस्थापनम्_2; }

	अन्तःस्थ भिन्न Uncorrect(ध्रुव भिन्न &corrected_दत्त) ध्रुव { प्रत्यागम corrected_दत्त + विस्थापनम्_2; }

	तार्किक Parameters_आरोपय();
	तार्किक ParametersSave(पूर्णाङ्क desired_calibration_सूचकांक = -1, तार्किक force = असत्य);
	शून्य ParametersUpdate();

	/**
	 * Take over the offset left in SENS_DPRES_OFF, using slot 0.
	 * Does nothing if slot 0 is already claimed.
	 *
	 * @return true if the offset was adopted (the caller should save)
	 */
	तार्किक Adopt_Legacy_विस्थापनम्();

	/** mirror slot 0's offset into SENS_DPRES_OFF for ground stations that still read it */
	शून्य Update_Legacy_विस्थापनम्() ध्रुव;

	शून्य Reset();

गुप्त:
	स्थिर constexpr पूर्णाङ्क k_Legacy_सूचकांक{0};
	स्थिर constexpr ध्रुव वर्ण *k_Legacy_विस्थापनम्_Param{"SENS_DPRES_OFF"};

	भिन्न विस्थापनम्_2{0};

	int8_t calibration_सूचकांक_2{-1};
	uint32_t _device_id{0};
	uint8_t calibration_गणनम्{0};

	तार्किक _external{असत्य};
};
} // namespace calibration
