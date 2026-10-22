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

#അന്തര്ഭാവ <px4_platform_common/px4_config.h>
#അന്തര്ഭാവ <px4_platform_common/defines.h>
#അന്തര്ഭാവ <px4_platform_common/log.h>

#അന്തര്ഭാവ <inttypes.h>
#അന്തര്ഭാവ <math.h>

നാമാകാശ calibration
{
വർഗ്ഗം DifferentialPressure
{
പൊതു:
	സ്ഥിതം constexpr പൂർണ്ണസംഖ്യ MAX_SENSOR_എണ്ണം = 3; ///< also the airspeed selector's sensor limit

	സ്ഥിതം constexpr സ്ഥിരം അക്ഷരം *SensorString() { തിരികെ "DPRES"; }

	DifferentialPressure();
	explicit DifferentialPressure(uint32_t device_id);

	~DifferentialPressure() = സ്ഥിരസ്ഥിതി;

	ശൂന്യം PrintStatus();

	തർക്കം set_calibration_index(പൂർണ്ണസംഖ്യ calibration_index);
	ശൂന്യം set_device_id(uint32_t device_id);
	തർക്കം set_offset(സ്ഥിരം ദശാംശം &offset);

	തർക്കം calibrated() സ്ഥിരം { തിരികെ (_device_id != 0) && (_calibration_index >= 0); }
	uint8_t calibration_എണ്ണം_2() സ്ഥിരം { തിരികെ calibration_എണ്ണം; }
	int8_t calibration_index() സ്ഥിരം { തിരികെ _calibration_index; }
	uint32_t device_id() സ്ഥിരം { തിരികെ _device_id; }
	തർക്കം external() സ്ഥിരം { തിരികെ _external; }
	സ്ഥിരം ദശാംശം &offset() സ്ഥിരം { തിരികെ _offset; }

	/// remove the zero offset from a raw differential pressure reading [Pa]
	വരിയിൽ ദശാംശം Correct(സ്ഥിരം ദശാംശം &data) സ്ഥിരം { തിരികെ data - _offset; }

	വരിയിൽ ദശാംശം Uncorrect(സ്ഥിരം ദശാംശം &corrected_data) സ്ഥിരം { തിരികെ corrected_data + _offset; }

	തർക്കം ParametersLoad();
	തർക്കം ParametersSave(പൂർണ്ണസംഖ്യ desired_calibration_index = -1, തർക്കം force = അസത്യം);
	ശൂന്യം ParametersUpdate();

	/**
	 * Take over the offset left in SENS_DPRES_OFF, using slot 0.
	 * Does nothing if slot 0 is already claimed.
	 *
	 * @return true if the offset was adopted (the caller should save)
	 */
	തർക്കം AdoptLegacyOffset();

	/** mirror slot 0's offset into SENS_DPRES_OFF for ground stations that still read it */
	ശൂന്യം UpdateLegacyOffset() സ്ഥിരം;

	ശൂന്യം Reset();

സ്വകാര്യ:
	സ്ഥിതം constexpr പൂർണ്ണസംഖ്യ kLegacyIndex{0};
	സ്ഥിതം constexpr സ്ഥിരം അക്ഷരം *kLegacyOffsetParam{"SENS_DPRES_OFF"};

	ദശാംശം _offset{0};

	int8_t _calibration_index{-1};
	uint32_t _device_id{0};
	uint8_t calibration_എണ്ണം{0};

	തർക്കം _external{അസത്യം};
};
} // namespace calibration
