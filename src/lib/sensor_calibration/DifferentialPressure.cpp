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

#ضمن "DifferentialPressure.hpp"

#ضمن "Utilities.hpp"

#ضمن <lib/drivers/device/Device.hpp>
#ضمن <lib/parameters/param.h>

#ضمن <float.h>

نطاق calibration
{

DifferentialPressure::DifferentialPressure()
{
	Reset();
}

DifferentialPressure::DifferentialPressure(uint32_t device_id)
{
	سیٹ_device_id(device_id);
}

فراغ DifferentialPressure::سیٹ_device_id(uint32_t device_id)
{
	ثابت منطقي external = device::device_is_external(device_id);

	إذا (_device_id != device_id || _external != external) {

		_device_id = device_id;
		_external = external;

		Reset();

		ParametersUpdate();
	}
}

منطقي DifferentialPressure::سیٹ_وتھ(ثابت عائم &وتھ)
{
	إذا (fabsf(وتھ_2 - وتھ) > 0.01f) {
		إذا (PX4_ISFINITE(وتھ)) {
			وتھ_2 = وتھ;
			calibration_گنتی++;
			أرجع سچ;
		}
	}

	أرجع خطأ;
}

منطقي DifferentialPressure::سیٹ_calibration_اشاریہ(صحيح calibration_اشاریہ)
{
	إذا ((calibration_اشاریہ >= 0) && (calibration_اشاریہ < MAX_SENSOR_گنتی)) {
		calibration_اشاریہ_2 = calibration_اشاریہ;
		أرجع سچ;
	}

	أرجع خطأ;
}

فراغ DifferentialPressure::ParametersUpdate()
{
	إذا (_device_id == 0) {
		أرجع;
	}

	calibration_اشاریہ_2 = لبھو_Current_Calibration_اشاریہ(SensorString(), _device_id);

	إذا (calibration_اشاریہ_2 == -1) {
		// no saved calibration available
		Reset();

	} وإلا {
		Parameters_لدو();
	}
}

منطقي DifferentialPressure::Parameters_لدو()
{
	إذا (calibration_اشاریہ_2 >= 0 && calibration_اشاریہ_2 < MAX_SENSOR_گنتی) {
		// CAL_DPRESx_OFF
		سیٹ_وتھ(GetCalibrationParamFloat(SensorString(), "OFF", calibration_اشاریہ_2));

		أرجع سچ;
	}

	أرجع خطأ;
}

منطقي DifferentialPressure::Adopt_Legacy_وتھ()
{
	إذا ((_device_id == 0) || calibrated()) {
		أرجع خطأ;
	}

	// slot 0 must still be free, otherwise another sensor already owns the legacy offset
	إذا (GetCalibrationParamInt32(SensorString(), "ID", k_Legacy_اشاریہ) != 0) {
		أرجع خطأ;
	}

	عائم legacy_وتھ = 0.f;

	إذا (param_get(param_لبھو(k_Legacy_وتھ_Param), &legacy_وتھ) != PX4_OK) {
		أرجع خطأ;
	}

	إذا (!PX4_ISFINITE(legacy_وتھ) || (fabsf(legacy_وتھ) < FLT_EPSILON)) {
		أرجع خطأ;
	}

	وتھ_2 = legacy_وتھ;
	calibration_اشاریہ_2 = k_Legacy_اشاریہ;

	PX4_INFO("%s %" PRIu32 " adopted %s %.3f Pa", SensorString(), _device_id, k_Legacy_وتھ_Param, (مزدوج)وتھ_2);

	أرجع سچ;
}

فراغ DifferentialPressure::Update_Legacy_وتھ() ثابت
{
	// keep SENS_DPRES_OFF in step with slot 0 so that ground stations reading it can still tell
	// whether airspeed has been calibrated
	إذا (calibration_اشاریہ_2 != k_Legacy_اشاریہ) {
		أرجع;
	}

	ثابت param_t handle = param_لبھو(k_Legacy_وتھ_Param);

	إذا (handle == PARAM_INVALID) {
		أرجع;
	}

	عائم وتھ = وتھ_2;
	param_سیٹ_no_notification(handle, &وتھ);
}

فراغ DifferentialPressure::Reset()
{
	وتھ_2 = 0;

	calibration_اشاریہ_2 = -1;

	calibration_گنتی = 0;
}

منطقي DifferentialPressure::ParametersSave(صحيح desired_calibration_اشاریہ, منطقي force)
{
	إذا (force && desired_calibration_اشاریہ >= 0 && desired_calibration_اشاریہ < MAX_SENSOR_گنتی) {
		calibration_اشاریہ_2 = desired_calibration_اشاریہ;

	} وإلا إذا (!force || (calibration_اشاریہ_2 < 0)
		   || (desired_calibration_اشاریہ != -1 && desired_calibration_اشاریہ != calibration_اشاریہ_2)) {

		// ensure we have a valid calibration slot (matching existing or first available slot)
		int8_t calibration_اشاریہ_prev = calibration_اشاریہ_2;
		calibration_اشاریہ_2 = لبھو_دستیاب_Calibration_اشاریہ(SensorString(), _device_id, desired_calibration_اشاریہ);

		إذا (calibration_اشاریہ_prev >= 0 && (calibration_اشاریہ_prev != calibration_اشاریہ_2)) {
			PX4_WARN("%s %" PRIu32 " calibration index changed %" PRIi8 " -> %" PRIi8, SensorString(), _device_id,
				 calibration_اشاریہ_prev, calibration_اشاریہ_2);
		}
	}

	إذا (calibration_اشاریہ_2 >= 0 && calibration_اشاریہ_2 < MAX_SENSOR_گنتی) {
		// save calibration
		منطقي success = سچ;
		success &= سیٹ_Calibration_Param(SensorString(), "ID", calibration_اشاریہ_2, _device_id);
		success &= سیٹ_Calibration_Param(SensorString(), "OFF", calibration_اشاریہ_2, وتھ_2);

		Update_Legacy_وتھ();

		أرجع success;
	}

	// index 3 is possible: FindAvailableCalibrationIndex() searches four slots, we define three
	PX4_ERR("%s %" PRIu32 " no free calibration slot (got %" PRIi8 ")", SensorString(), _device_id, calibration_اشاریہ_2);

	أرجع خطأ;
}

فراغ DifferentialPressure::Print_حالت()
{
	PX4_INFO_RAW("%s %" PRIu32 " offset: %05.3f Pa, %s\n",
		     SensorString(), device_id(), (مزدوج)وتھ_2,
		     external() ? "Ext" : "Internal");
}

} // namespace calibration
