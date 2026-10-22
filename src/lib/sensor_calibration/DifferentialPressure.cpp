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
	سيٽ_device_id(device_id);
}

فراغ DifferentialPressure::سيٽ_device_id(uint32_t device_id)
{
	ثابت منطقي external = device::device_is_external(device_id);

	إذا (_device_id != device_id || _external != external) {

		_device_id = device_id;
		_external = external;

		Reset();

		ParametersUpdate();
	}
}

منطقي DifferentialPressure::سيٽ_وٿي(ثابت عائم &وٿي)
{
	إذا (fabsf(وٿي_2 - وٿي) > 0.01f) {
		إذا (PX4_ISFINITE(وٿي)) {
			وٿي_2 = وٿي;
			calibration_ڳڻپ++;
			أرجع سچ;
		}
	}

	أرجع خطأ;
}

منطقي DifferentialPressure::سيٽ_calibration_اشاريو(صحيح calibration_اشاريو)
{
	إذا ((calibration_اشاريو >= 0) && (calibration_اشاريو < MAX_SENSOR_ڳڻپ)) {
		calibration_اشاريو_2 = calibration_اشاريو;
		أرجع سچ;
	}

	أرجع خطأ;
}

فراغ DifferentialPressure::ParametersUpdate()
{
	إذا (_device_id == 0) {
		أرجع;
	}

	calibration_اشاريو_2 = ڳوليو_Current_Calibration_اشاريو(SensorString(), _device_id);

	إذا (calibration_اشاريو_2 == -1) {
		// no saved calibration available
		Reset();

	} وإلا {
		Parameters_ڀريو();
	}
}

منطقي DifferentialPressure::Parameters_ڀريو()
{
	إذا (calibration_اشاريو_2 >= 0 && calibration_اشاريو_2 < MAX_SENSOR_ڳڻپ) {
		// CAL_DPRESx_OFF
		سيٽ_وٿي(GetCalibrationParamFloat(SensorString(), "OFF", calibration_اشاريو_2));

		أرجع سچ;
	}

	أرجع خطأ;
}

منطقي DifferentialPressure::Adopt_Legacy_وٿي()
{
	إذا ((_device_id == 0) || calibrated()) {
		أرجع خطأ;
	}

	// slot 0 must still be free, otherwise another sensor already owns the legacy offset
	إذا (GetCalibrationParamInt32(SensorString(), "ID", k_Legacy_اشاريو) != 0) {
		أرجع خطأ;
	}

	عائم legacy_وٿي = 0.f;

	إذا (param_get(param_ڳوليو(k_Legacy_وٿي_Param), &legacy_وٿي) != PX4_OK) {
		أرجع خطأ;
	}

	إذا (!PX4_ISFINITE(legacy_وٿي) || (fabsf(legacy_وٿي) < FLT_EPSILON)) {
		أرجع خطأ;
	}

	وٿي_2 = legacy_وٿي;
	calibration_اشاريو_2 = k_Legacy_اشاريو;

	PX4_INFO("%s %" PRIu32 " adopted %s %.3f Pa", SensorString(), _device_id, k_Legacy_وٿي_Param, (مزدوج)وٿي_2);

	أرجع سچ;
}

فراغ DifferentialPressure::Update_Legacy_وٿي() ثابت
{
	// keep SENS_DPRES_OFF in step with slot 0 so that ground stations reading it can still tell
	// whether airspeed has been calibrated
	إذا (calibration_اشاريو_2 != k_Legacy_اشاريو) {
		أرجع;
	}

	ثابت param_t handle = param_ڳوليو(k_Legacy_وٿي_Param);

	إذا (handle == PARAM_INVALID) {
		أرجع;
	}

	عائم وٿي = وٿي_2;
	param_سيٽ_no_notification(handle, &وٿي);
}

فراغ DifferentialPressure::Reset()
{
	وٿي_2 = 0;

	calibration_اشاريو_2 = -1;

	calibration_ڳڻپ = 0;
}

منطقي DifferentialPressure::ParametersSave(صحيح desired_calibration_اشاريو, منطقي force)
{
	إذا (force && desired_calibration_اشاريو >= 0 && desired_calibration_اشاريو < MAX_SENSOR_ڳڻپ) {
		calibration_اشاريو_2 = desired_calibration_اشاريو;

	} وإلا إذا (!force || (calibration_اشاريو_2 < 0)
		   || (desired_calibration_اشاريو != -1 && desired_calibration_اشاريو != calibration_اشاريو_2)) {

		// ensure we have a valid calibration slot (matching existing or first available slot)
		int8_t calibration_اشاريو_prev = calibration_اشاريو_2;
		calibration_اشاريو_2 = ڳوليو_موجود_Calibration_اشاريو(SensorString(), _device_id, desired_calibration_اشاريو);

		إذا (calibration_اشاريو_prev >= 0 && (calibration_اشاريو_prev != calibration_اشاريو_2)) {
			PX4_WARN("%s %" PRIu32 " calibration index changed %" PRIi8 " -> %" PRIi8, SensorString(), _device_id,
				 calibration_اشاريو_prev, calibration_اشاريو_2);
		}
	}

	إذا (calibration_اشاريو_2 >= 0 && calibration_اشاريو_2 < MAX_SENSOR_ڳڻپ) {
		// save calibration
		منطقي success = سچ;
		success &= سيٽ_Calibration_Param(SensorString(), "ID", calibration_اشاريو_2, _device_id);
		success &= سيٽ_Calibration_Param(SensorString(), "OFF", calibration_اشاريو_2, وٿي_2);

		Update_Legacy_وٿي();

		أرجع success;
	}

	// index 3 is possible: FindAvailableCalibrationIndex() searches four slots, we define three
	PX4_ERR("%s %" PRIu32 " no free calibration slot (got %" PRIi8 ")", SensorString(), _device_id, calibration_اشاريو_2);

	أرجع خطأ;
}

فراغ DifferentialPressure::Print_حالت()
{
	PX4_INFO_RAW("%s %" PRIu32 " offset: %05.3f Pa, %s\n",
		     SensorString(), device_id(), (مزدوج)وٿي_2,
		     external() ? "Ext" : "Internal");
}

} // namespace calibration
