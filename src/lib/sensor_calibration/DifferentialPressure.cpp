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

#شامل "DifferentialPressure.hpp"

#شامل "Utilities.hpp"

#شامل <lib/drivers/device/Device.hpp>
#شامل <lib/parameters/param.h>

#شامل <float.h>

نامخلا calibration
{

DifferentialPressure::DifferentialPressure()
{
	Reset();
}

DifferentialPressure::DifferentialPressure(uint32_t device_id)
{
	سیٹ_device_id(device_id);
}

خالی DifferentialPressure::سیٹ_device_id(uint32_t device_id)
{
	ثابت منطقی external = device::device_is_external(device_id);

	اگر (_device_id != device_id || _external != external) {

		_device_id = device_id;
		_external = external;

		Reset();

		ParametersUpdate();
	}
}

منطقی DifferentialPressure::سیٹ_انحراف(ثابت اعشاری &انحراف)
{
	اگر (fabsf(انحراف_2 - انحراف) > 0.01f) {
		اگر (PX4_ISFINITE(انحراف)) {
			انحراف_2 = انحراف;
			calibration_گنتی++;
			واپس سچ;
		}
	}

	واپس جھوٹ;
}

منطقی DifferentialPressure::سیٹ_calibration_اشاریہ(صحیح calibration_اشاریہ)
{
	اگر ((calibration_اشاریہ >= 0) && (calibration_اشاریہ < MAX_SENSOR_گنتی)) {
		calibration_اشاریہ_2 = calibration_اشاریہ;
		واپس سچ;
	}

	واپس جھوٹ;
}

خالی DifferentialPressure::ParametersUpdate()
{
	اگر (_device_id == 0) {
		واپس;
	}

	calibration_اشاریہ_2 = ڈھونڈو_Current_Calibration_اشاریہ(SensorString(), _device_id);

	اگر (calibration_اشاریہ_2 == -1) {
		// no saved calibration available
		Reset();

	} ورنہ {
		Parameters_بھرو();
	}
}

منطقی DifferentialPressure::Parameters_بھرو()
{
	اگر (calibration_اشاریہ_2 >= 0 && calibration_اشاریہ_2 < MAX_SENSOR_گنتی) {
		// CAL_DPRESx_OFF
		سیٹ_انحراف(GetCalibrationParamFloat(SensorString(), "OFF", calibration_اشاریہ_2));

		واپس سچ;
	}

	واپس جھوٹ;
}

منطقی DifferentialPressure::Adopt_Legacy_انحراف()
{
	اگر ((_device_id == 0) || calibrated()) {
		واپس جھوٹ;
	}

	// slot 0 must still be free, otherwise another sensor already owns the legacy offset
	اگر (GetCalibrationParamInt32(SensorString(), "ID", k_Legacy_اشاریہ) != 0) {
		واپس جھوٹ;
	}

	اعشاری legacy_انحراف = 0.f;

	اگر (param_get(param_ڈھونڈو(k_Legacy_انحراف_Param), &legacy_انحراف) != PX4_OK) {
		واپس جھوٹ;
	}

	اگر (!PX4_ISFINITE(legacy_انحراف) || (fabsf(legacy_انحراف) < FLT_EPSILON)) {
		واپس جھوٹ;
	}

	انحراف_2 = legacy_انحراف;
	calibration_اشاریہ_2 = k_Legacy_اشاریہ;

	PX4_INFO("%s %" PRIu32 " adopted %s %.3f Pa", SensorString(), _device_id, k_Legacy_انحراف_Param, (دوہرا)انحراف_2);

	واپس سچ;
}

خالی DifferentialPressure::Update_Legacy_انحراف() ثابت
{
	// keep SENS_DPRES_OFF in step with slot 0 so that ground stations reading it can still tell
	// whether airspeed has been calibrated
	اگر (calibration_اشاریہ_2 != k_Legacy_اشاریہ) {
		واپس;
	}

	ثابت param_t handle = param_ڈھونڈو(k_Legacy_انحراف_Param);

	اگر (handle == PARAM_INVALID) {
		واپس;
	}

	اعشاری انحراف = انحراف_2;
	param_سیٹ_no_notification(handle, &انحراف);
}

خالی DifferentialPressure::Reset()
{
	انحراف_2 = 0;

	calibration_اشاریہ_2 = -1;

	calibration_گنتی = 0;
}

منطقی DifferentialPressure::ParametersSave(صحیح desired_calibration_اشاریہ, منطقی force)
{
	اگر (force && desired_calibration_اشاریہ >= 0 && desired_calibration_اشاریہ < MAX_SENSOR_گنتی) {
		calibration_اشاریہ_2 = desired_calibration_اشاریہ;

	} ورنہ اگر (!force || (calibration_اشاریہ_2 < 0)
		   || (desired_calibration_اشاریہ != -1 && desired_calibration_اشاریہ != calibration_اشاریہ_2)) {

		// ensure we have a valid calibration slot (matching existing or first available slot)
		int8_t calibration_اشاریہ_پچھلا = calibration_اشاریہ_2;
		calibration_اشاریہ_2 = ڈھونڈو_دستیاب_Calibration_اشاریہ(SensorString(), _device_id, desired_calibration_اشاریہ);

		اگر (calibration_اشاریہ_پچھلا >= 0 && (calibration_اشاریہ_پچھلا != calibration_اشاریہ_2)) {
			PX4_WARN("%s %" PRIu32 " calibration index changed %" PRIi8 " -> %" PRIi8, SensorString(), _device_id,
				 calibration_اشاریہ_پچھلا, calibration_اشاریہ_2);
		}
	}

	اگر (calibration_اشاریہ_2 >= 0 && calibration_اشاریہ_2 < MAX_SENSOR_گنتی) {
		// save calibration
		منطقی success = سچ;
		success &= سیٹ_Calibration_Param(SensorString(), "ID", calibration_اشاریہ_2, _device_id);
		success &= سیٹ_Calibration_Param(SensorString(), "OFF", calibration_اشاریہ_2, انحراف_2);

		Update_Legacy_انحراف();

		واپس success;
	}

	// index 3 is possible: FindAvailableCalibrationIndex() searches four slots, we define three
	PX4_ERR("%s %" PRIu32 " no free calibration slot (got %" PRIi8 ")", SensorString(), _device_id, calibration_اشاریہ_2);

	واپس جھوٹ;
}

خالی DifferentialPressure::Print_حالت()
{
	PX4_INFO_RAW("%s %" PRIu32 " offset: %05.3f Pa, %s\n",
		     SensorString(), device_id(), (دوہرا)انحراف_2,
		     external() ? "Ext" : "Internal");
}

} // namespace calibration
