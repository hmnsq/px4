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
	स्थिर constexpr पूर्णांक MAX_SENSOR_मोजणी = 3; ///< also the airspeed selector's sensor limit

	स्थिर constexpr स्थिरांक अक्षर *SensorString() { परत "DPRES"; }

	DifferentialPressure();
	explicit DifferentialPressure(uint32_t device_id);

	~DifferentialPressure() = पूर्वनिर्धारित;

	शून्य PrintStatus();

	तार्किक set_calibration_index(पूर्णांक calibration_index);
	शून्य set_device_id(uint32_t device_id);
	तार्किक set_offset(स्थिरांक दशांश &offset);

	तार्किक calibrated() स्थिरांक { परत (_device_id != 0) && (_calibration_index >= 0); }
	uint8_t calibration_मोजणी_2() स्थिरांक { परत calibration_मोजणी; }
	int8_t calibration_index() स्थिरांक { परत _calibration_index; }
	uint32_t device_id() स्थिरांक { परत _device_id; }
	तार्किक external() स्थिरांक { परत _external; }
	स्थिरांक दशांश &offset() स्थिरांक { परत _offset; }

	/// remove the zero offset from a raw differential pressure reading [Pa]
	ओळीत दशांश Correct(स्थिरांक दशांश &data) स्थिरांक { परत data - _offset; }

	ओळीत दशांश Uncorrect(स्थिरांक दशांश &corrected_data) स्थिरांक { परत corrected_data + _offset; }

	तार्किक ParametersLoad();
	तार्किक ParametersSave(पूर्णांक desired_calibration_index = -1, तार्किक force = असत्य);
	शून्य ParametersUpdate();

	/**
	 * Take over the offset left in SENS_DPRES_OFF, using slot 0.
	 * Does nothing if slot 0 is already claimed.
	 *
	 * @return true if the offset was adopted (the caller should save)
	 */
	तार्किक AdoptLegacyOffset();

	/** mirror slot 0's offset into SENS_DPRES_OFF for ground stations that still read it */
	शून्य UpdateLegacyOffset() स्थिरांक;

	शून्य Reset();

खाजगी:
	स्थिर constexpr पूर्णांक kLegacyIndex{0};
	स्थिर constexpr स्थिरांक अक्षर *kLegacyOffsetParam{"SENS_DPRES_OFF"};

	दशांश _offset{0};

	int8_t _calibration_index{-1};
	uint32_t _device_id{0};
	uint8_t calibration_मोजणी{0};

	तार्किक _external{असत्य};
};
} // namespace calibration
