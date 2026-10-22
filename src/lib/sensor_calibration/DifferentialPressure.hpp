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

#సూచన once

#అన్తర్భావ <px4_platform_common/px4_config.h>
#అన్తర్భావ <px4_platform_common/defines.h>
#అన్తర్భావ <px4_platform_common/log.h>

#అన్తర్భావ <inttypes.h>
#అన్తర్భావ <math.h>

నామాకాశ calibration
{
తరగతి DifferentialPressure
{
బహిరంగ:
	స్థిర constexpr పూర్ణాంకం MAX_SENSOR_లెక్క = 3; ///< also the airspeed selector's sensor limit

	స్థిర constexpr స్థిరరాశి అక్షరం *SensorString() { తిరిగి "DPRES"; }

	DifferentialPressure();
	స్పష్టమైన DifferentialPressure(uint32_t device_id);

	~DifferentialPressure() = అప్రమేయ;

	శూన్య Print_స్థితి();

	తార్కిక సమితి_calibration_సూచీ(పూర్ణాంకం calibration_సూచీ);
	శూన్య సమితి_device_id(uint32_t device_id);
	తార్కిక సమితి_స్థానభ్రంశం(స్థిరరాశి దశాంశం &స్థానభ్రంశం);

	తార్కిక calibrated() స్థిరరాశి { తిరిగి (_device_id != 0) && (calibration_సూచీ_2 >= 0); }
	uint8_t calibration_లెక్క_2() స్థిరరాశి { తిరిగి calibration_లెక్క; }
	int8_t calibration_సూచీ() స్థిరరాశి { తిరిగి calibration_సూచీ_2; }
	uint32_t device_id() స్థిరరాశి { తిరిగి _device_id; }
	తార్కిక external() స్థిరరాశి { తిరిగి _external; }
	స్థిరరాశి దశాంశం &స్థానభ్రంశం() స్థిరరాశి { తిరిగి స్థానభ్రంశం_2; }

	/// remove the zero offset from a raw differential pressure reading [Pa]
	అంతఃవరుస దశాంశం Correct(స్థిరరాశి దశాంశం &దత్తాంశం) స్థిరరాశి { తిరిగి దత్తాంశం - స్థానభ్రంశం_2; }

	అంతఃవరుస దశాంశం Uncorrect(స్థిరరాశి దశాంశం &corrected_దత్తాంశం) స్థిరరాశి { తిరిగి corrected_దత్తాంశం + స్థానభ్రంశం_2; }

	తార్కిక Parameters_ఎక్కించు();
	తార్కిక ParametersSave(పూర్ణాంకం desired_calibration_సూచీ = -1, తార్కిక force = అబద్ధం);
	శూన్య ParametersUpdate();

	/**
	 * Take over the offset left in SENS_DPRES_OFF, using slot 0.
	 * Does nothing if slot 0 is already claimed.
	 *
	 * @return true if the offset was adopted (the caller should save)
	 */
	తార్కిక Adopt_Legacy_స్థానభ్రంశం();

	/** mirror slot 0's offset into SENS_DPRES_OFF for ground stations that still read it */
	శూన్య Update_Legacy_స్థానభ్రంశం() స్థిరరాశి;

	శూన్య Reset();

అంతరంగ:
	స్థిర constexpr పూర్ణాంకం k_Legacy_సూచీ{0};
	స్థిర constexpr స్థిరరాశి అక్షరం *k_Legacy_స్థానభ్రంశం_Param{"SENS_DPRES_OFF"};

	దశాంశం స్థానభ్రంశం_2{0};

	int8_t calibration_సూచీ_2{-1};
	uint32_t _device_id{0};
	uint8_t calibration_లెక్క{0};

	తార్కిక _external{అబద్ధం};
};
} // namespace calibration
