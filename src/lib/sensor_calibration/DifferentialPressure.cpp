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

#అన్తర్భావ "DifferentialPressure.hpp"

#అన్తర్భావ "Utilities.hpp"

#అన్తర్భావ <lib/drivers/device/Device.hpp>
#అన్తర్భావ <lib/parameters/param.h>

#అన్తర్భావ <float.h>

నామాకాశ calibration
{

DifferentialPressure::DifferentialPressure()
{
	Reset();
}

DifferentialPressure::DifferentialPressure(uint32_t device_id)
{
	సమితి_device_id(device_id);
}

శూన్య DifferentialPressure::సమితి_device_id(uint32_t device_id)
{
	స్థిరరాశి తార్కిక external = device::device_is_external(device_id);

	ఒకవేళ (_device_id != device_id || _external != external) {

		_device_id = device_id;
		_external = external;

		Reset();

		ParametersUpdate();
	}
}

తార్కిక DifferentialPressure::సమితి_స్థానభ్రంశం(స్థిరరాశి దశాంశం &స్థానభ్రంశం)
{
	ఒకవేళ (fabsf(స్థానభ్రంశం_2 - స్థానభ్రంశం) > 0.01f) {
		ఒకవేళ (PX4_ISFINITE(స్థానభ్రంశం)) {
			స్థానభ్రంశం_2 = స్థానభ్రంశం;
			calibration_లెక్క++;
			తిరిగి నిజం;
		}
	}

	తిరిగి అబద్ధం;
}

తార్కిక DifferentialPressure::సమితి_calibration_సూచీ(పూర్ణాంకం calibration_సూచీ)
{
	ఒకవేళ ((calibration_సూచీ >= 0) && (calibration_సూచీ < MAX_SENSOR_లెక్క)) {
		calibration_సూచీ_2 = calibration_సూచీ;
		తిరిగి నిజం;
	}

	తిరిగి అబద్ధం;
}

శూన్య DifferentialPressure::ParametersUpdate()
{
	ఒకవేళ (_device_id == 0) {
		తిరిగి;
	}

	calibration_సూచీ_2 = కనుగొను_Current_Calibration_సూచీ(SensorString(), _device_id);

	ఒకవేళ (calibration_సూచీ_2 == -1) {
		// no saved calibration available
		Reset();

	} లేకపోతే {
		Parameters_ఎక్కించు();
	}
}

తార్కిక DifferentialPressure::Parameters_ఎక్కించు()
{
	ఒకవేళ (calibration_సూచీ_2 >= 0 && calibration_సూచీ_2 < MAX_SENSOR_లెక్క) {
		// CAL_DPRESx_OFF
		సమితి_స్థానభ్రంశం(GetCalibrationParamFloat(SensorString(), "OFF", calibration_సూచీ_2));

		తిరిగి నిజం;
	}

	తిరిగి అబద్ధం;
}

తార్కిక DifferentialPressure::Adopt_Legacy_స్థానభ్రంశం()
{
	ఒకవేళ ((_device_id == 0) || calibrated()) {
		తిరిగి అబద్ధం;
	}

	// slot 0 must still be free, otherwise another sensor already owns the legacy offset
	ఒకవేళ (GetCalibrationParamInt32(SensorString(), "ID", k_Legacy_సూచీ) != 0) {
		తిరిగి అబద్ధం;
	}

	దశాంశం legacy_స్థానభ్రంశం = 0.f;

	ఒకవేళ (param_get(param_కనుగొను(k_Legacy_స్థానభ్రంశం_Param), &legacy_స్థానభ్రంశం) != PX4_OK) {
		తిరిగి అబద్ధం;
	}

	ఒకవేళ (!PX4_ISFINITE(legacy_స్థానభ్రంశం) || (fabsf(legacy_స్థానభ్రంశం) < FLT_EPSILON)) {
		తిరిగి అబద్ధం;
	}

	స్థానభ్రంశం_2 = legacy_స్థానభ్రంశం;
	calibration_సూచీ_2 = k_Legacy_సూచీ;

	PX4_INFO("%s %" PRIu32 " adopted %s %.3f Pa", SensorString(), _device_id, k_Legacy_స్థానభ్రంశం_Param, (ద్విగుణ)స్థానభ్రంశం_2);

	తిరిగి నిజం;
}

శూన్య DifferentialPressure::Update_Legacy_స్థానభ్రంశం() స్థిరరాశి
{
	// keep SENS_DPRES_OFF in step with slot 0 so that ground stations reading it can still tell
	// whether airspeed has been calibrated
	ఒకవేళ (calibration_సూచీ_2 != k_Legacy_సూచీ) {
		తిరిగి;
	}

	స్థిరరాశి param_t handle = param_కనుగొను(k_Legacy_స్థానభ్రంశం_Param);

	ఒకవేళ (handle == PARAM_INVALID) {
		తిరిగి;
	}

	దశాంశం స్థానభ్రంశం = స్థానభ్రంశం_2;
	param_సమితి_no_notification(handle, &స్థానభ్రంశం);
}

శూన్య DifferentialPressure::Reset()
{
	స్థానభ్రంశం_2 = 0;

	calibration_సూచీ_2 = -1;

	calibration_లెక్క = 0;
}

తార్కిక DifferentialPressure::ParametersSave(పూర్ణాంకం desired_calibration_సూచీ, తార్కిక force)
{
	ఒకవేళ (force && desired_calibration_సూచీ >= 0 && desired_calibration_సూచీ < MAX_SENSOR_లెక్క) {
		calibration_సూచీ_2 = desired_calibration_సూచీ;

	} లేకపోతే ఒకవేళ (!force || (calibration_సూచీ_2 < 0)
		   || (desired_calibration_సూచీ != -1 && desired_calibration_సూచీ != calibration_సూచీ_2)) {

		// ensure we have a valid calibration slot (matching existing or first available slot)
		int8_t calibration_సూచీ_prev = calibration_సూచీ_2;
		calibration_సూచీ_2 = కనుగొను_అందుబాటు_Calibration_సూచీ(SensorString(), _device_id, desired_calibration_సూచీ);

		ఒకవేళ (calibration_సూచీ_prev >= 0 && (calibration_సూచీ_prev != calibration_సూచీ_2)) {
			PX4_WARN("%s %" PRIu32 " calibration index changed %" PRIi8 " -> %" PRIi8, SensorString(), _device_id,
				 calibration_సూచీ_prev, calibration_సూచీ_2);
		}
	}

	ఒకవేళ (calibration_సూచీ_2 >= 0 && calibration_సూచీ_2 < MAX_SENSOR_లెక్క) {
		// save calibration
		తార్కిక success = నిజం;
		success &= సమితి_Calibration_Param(SensorString(), "ID", calibration_సూచీ_2, _device_id);
		success &= సమితి_Calibration_Param(SensorString(), "OFF", calibration_సూచీ_2, స్థానభ్రంశం_2);

		Update_Legacy_స్థానభ్రంశం();

		తిరిగి success;
	}

	// index 3 is possible: FindAvailableCalibrationIndex() searches four slots, we define three
	PX4_ERR("%s %" PRIu32 " no free calibration slot (got %" PRIi8 ")", SensorString(), _device_id, calibration_సూచీ_2);

	తిరిగి అబద్ధం;
}

శూన్య DifferentialPressure::Print_స్థితి()
{
	PX4_INFO_RAW("%s %" PRIu32 " offset: %05.3f Pa, %s\n",
		     SensorString(), device_id(), (ద్విగుణ)స్థానభ్రంశం_2,
		     external() ? "Ext" : "Internal");
}

} // namespace calibration
