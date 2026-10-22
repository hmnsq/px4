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

#અન્તર્ભાવ <px4_platform_common/px4_config.h>
#અન્તર્ભાવ <px4_platform_common/defines.h>
#અન્તર્ભાવ <px4_platform_common/log.h>

#અન્તર્ભાવ <inttypes.h>
#અન્તર્ભાવ <math.h>

નામાકાશ calibration
{
વર્ગ DifferentialPressure
{
જાહેર:
	સ્થિર constexpr પૂર્ણાંક MAX_SENSOR_ગણતરી = 3; ///< also the airspeed selector's sensor limit

	સ્થિર constexpr અચલ અક્ષર *SensorString() { પાછા "DPRES"; }

	DifferentialPressure();
	explicit DifferentialPressure(uint32_t device_id);

	~DifferentialPressure() = મૂળભૂત;

	શૂન્ય PrintStatus();

	તાર્કિક set_calibration_index(પૂર્ણાંક calibration_index);
	શૂન્ય set_device_id(uint32_t device_id);
	તાર્કિક set_offset(અચલ દશાંશ &offset);

	તાર્કિક calibrated() અચલ { પાછા (_device_id != 0) && (_calibration_index >= 0); }
	uint8_t calibration_ગણતરી_2() અચલ { પાછા calibration_ગણતરી; }
	int8_t calibration_index() અચલ { પાછા _calibration_index; }
	uint32_t device_id() અચલ { પાછા _device_id; }
	તાર્કિક external() અચલ { પાછા _external; }
	અચલ દશાંશ &offset() અચલ { પાછા _offset; }

	/// remove the zero offset from a raw differential pressure reading [Pa]
	અંતઃરેખ દશાંશ Correct(અચલ દશાંશ &data) અચલ { પાછા data - _offset; }

	અંતઃરેખ દશાંશ Uncorrect(અચલ દશાંશ &corrected_data) અચલ { પાછા corrected_data + _offset; }

	તાર્કિક ParametersLoad();
	તાર્કિક ParametersSave(પૂર્ણાંક desired_calibration_index = -1, તાર્કિક force = ખોટું);
	શૂન્ય ParametersUpdate();

	/**
	 * Take over the offset left in SENS_DPRES_OFF, using slot 0.
	 * Does nothing if slot 0 is already claimed.
	 *
	 * @return true if the offset was adopted (the caller should save)
	 */
	તાર્કિક AdoptLegacyOffset();

	/** mirror slot 0's offset into SENS_DPRES_OFF for ground stations that still read it */
	શૂન્ય UpdateLegacyOffset() અચલ;

	શૂન્ય Reset();

ખાનગી:
	સ્થિર constexpr પૂર્ણાંક kLegacyIndex{0};
	સ્થિર constexpr અચલ અક્ષર *kLegacyOffsetParam{"SENS_DPRES_OFF"};

	દશાંશ _offset{0};

	int8_t _calibration_index{-1};
	uint32_t _device_id{0};
	uint8_t calibration_ગણતરી{0};

	તાર્કિક _external{ખોટું};
};
} // namespace calibration
