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
	مجموعه_device_id(device_id);
}

فراغ DifferentialPressure::مجموعه_device_id(uint32_t device_id)
{
	ثابت منطقي external = device::device_is_external(device_id);

	إذا (_device_id != device_id || _external != external) {

		_device_id = device_id;
		_external = external;

		Reset();

		ParametersUpdate();
	}
}

منطقي DifferentialPressure::مجموعه_جابه_جایی(ثابت عائم &جابه_جایی)
{
	إذا (fabsf(جابه_جایی_2 - جابه_جایی) > 0.01f) {
		إذا (PX4_ISFINITE(جابه_جایی)) {
			جابه_جایی_2 = جابه_جایی;
			calibration_شمار++;
			أرجع درست;
		}
	}

	أرجع خطأ;
}

منطقي DifferentialPressure::مجموعه_calibration_شاخص(صحيح calibration_شاخص)
{
	إذا ((calibration_شاخص >= 0) && (calibration_شاخص < MAX_SENSOR_شمار)) {
		calibration_شاخص_2 = calibration_شاخص;
		أرجع درست;
	}

	أرجع خطأ;
}

فراغ DifferentialPressure::ParametersUpdate()
{
	إذا (_device_id == 0) {
		أرجع;
	}

	calibration_شاخص_2 = پیدا_کن_Current_Calibration_شاخص(SensorString(), _device_id);

	إذا (calibration_شاخص_2 == -1) {
		// no saved calibration available
		Reset();

	} وإلا {
		Parameters_بارگیری();
	}
}

منطقي DifferentialPressure::Parameters_بارگیری()
{
	إذا (calibration_شاخص_2 >= 0 && calibration_شاخص_2 < MAX_SENSOR_شمار) {
		// CAL_DPRESx_OFF
		مجموعه_جابه_جایی(GetCalibrationParamFloat(SensorString(), "OFF", calibration_شاخص_2));

		أرجع درست;
	}

	أرجع خطأ;
}

منطقي DifferentialPressure::Adopt_Legacy_جابه_جایی()
{
	إذا ((_device_id == 0) || calibrated()) {
		أرجع خطأ;
	}

	// slot 0 must still be free, otherwise another sensor already owns the legacy offset
	إذا (GetCalibrationParamInt32(SensorString(), "ID", k_Legacy_شاخص) != 0) {
		أرجع خطأ;
	}

	عائم legacy_جابه_جایی = 0.f;

	إذا (param_get(param_پیدا_کن(k_Legacy_جابه_جایی_Param), &legacy_جابه_جایی) != PX4_OK) {
		أرجع خطأ;
	}

	إذا (!PX4_ISFINITE(legacy_جابه_جایی) || (fabsf(legacy_جابه_جایی) < FLT_EPSILON)) {
		أرجع خطأ;
	}

	جابه_جایی_2 = legacy_جابه_جایی;
	calibration_شاخص_2 = k_Legacy_شاخص;

	PX4_INFO("%s %" PRIu32 " adopted %s %.3f Pa", SensorString(), _device_id, k_Legacy_جابه_جایی_Param, (مزدوج)جابه_جایی_2);

	أرجع درست;
}

فراغ DifferentialPressure::Update_Legacy_جابه_جایی() ثابت
{
	// keep SENS_DPRES_OFF in step with slot 0 so that ground stations reading it can still tell
	// whether airspeed has been calibrated
	إذا (calibration_شاخص_2 != k_Legacy_شاخص) {
		أرجع;
	}

	ثابت param_t handle = param_پیدا_کن(k_Legacy_جابه_جایی_Param);

	إذا (handle == PARAM_INVALID) {
		أرجع;
	}

	عائم جابه_جایی = جابه_جایی_2;
	param_مجموعه_no_notification(handle, &جابه_جایی);
}

فراغ DifferentialPressure::Reset()
{
	جابه_جایی_2 = 0;

	calibration_شاخص_2 = -1;

	calibration_شمار = 0;
}

منطقي DifferentialPressure::ParametersSave(صحيح desired_calibration_شاخص, منطقي force)
{
	إذا (force && desired_calibration_شاخص >= 0 && desired_calibration_شاخص < MAX_SENSOR_شمار) {
		calibration_شاخص_2 = desired_calibration_شاخص;

	} وإلا إذا (!force || (calibration_شاخص_2 < 0)
		   || (desired_calibration_شاخص != -1 && desired_calibration_شاخص != calibration_شاخص_2)) {

		// ensure we have a valid calibration slot (matching existing or first available slot)
		int8_t calibration_شاخص_prev = calibration_شاخص_2;
		calibration_شاخص_2 = پیدا_کن_موجود_Calibration_شاخص(SensorString(), _device_id, desired_calibration_شاخص);

		إذا (calibration_شاخص_prev >= 0 && (calibration_شاخص_prev != calibration_شاخص_2)) {
			PX4_WARN("%s %" PRIu32 " calibration index changed %" PRIi8 " -> %" PRIi8, SensorString(), _device_id,
				 calibration_شاخص_prev, calibration_شاخص_2);
		}
	}

	إذا (calibration_شاخص_2 >= 0 && calibration_شاخص_2 < MAX_SENSOR_شمار) {
		// save calibration
		منطقي success = درست;
		success &= مجموعه_Calibration_Param(SensorString(), "ID", calibration_شاخص_2, _device_id);
		success &= مجموعه_Calibration_Param(SensorString(), "OFF", calibration_شاخص_2, جابه_جایی_2);

		Update_Legacy_جابه_جایی();

		أرجع success;
	}

	// index 3 is possible: FindAvailableCalibrationIndex() searches four slots, we define three
	PX4_ERR("%s %" PRIu32 " no free calibration slot (got %" PRIi8 ")", SensorString(), _device_id, calibration_شاخص_2);

	أرجع خطأ;
}

فراغ DifferentialPressure::Print_وضعیت()
{
	PX4_INFO_RAW("%s %" PRIu32 " offset: %05.3f Pa, %s\n",
		     SensorString(), device_id(), (مزدوج)جابه_جایی_2,
		     external() ? "Ext" : "Internal");
}

} // namespace calibration
