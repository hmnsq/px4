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

#অন্তর্ভাব "DifferentialPressure.hpp"

#অন্তর্ভাব "Utilities.hpp"

#অন্তর্ভাব <lib/drivers/device/Device.hpp>
#অন্তর্ভাব <lib/parameters/param.h>

#অন্তর্ভাব <float.h>

নামাকাশ calibration
{

DifferentialPressure::DifferentialPressure()
{
	Reset();
}

DifferentialPressure::DifferentialPressure(uint32_t device_id)
{
	set_device_id(device_id);
}

শূন্য DifferentialPressure::set_device_id(uint32_t device_id)
{
	ধ্রুবক যৌক্তিক external = device::device_is_external(device_id);

	যদি (_device_id != device_id || _external != external) {

		_device_id = device_id;
		_external = external;

		Reset();

		ParametersUpdate();
	}
}

যৌক্তিক DifferentialPressure::set_offset(ধ্রুবক দশমিক &offset)
{
	যদি (fabsf(_offset - offset) > 0.01f) {
		যদি (PX4_ISFINITE(offset)) {
			_offset = offset;
			calibration_গুনতি++;
			ফেরত সত্য;
		}
	}

	ফেরত মিথ্যা;
}

যৌক্তিক DifferentialPressure::set_calibration_সূচক(পূর্ণসংখ্যা calibration_সূচক)
{
	যদি ((calibration_সূচক >= 0) && (calibration_সূচক < MAX_SENSOR_গুনতি)) {
		calibration_সূচক_2 = calibration_সূচক;
		ফেরত সত্য;
	}

	ফেরত মিথ্যা;
}

শূন্য DifferentialPressure::ParametersUpdate()
{
	যদি (_device_id == 0) {
		ফেরত;
	}

	calibration_সূচক_2 = Find_Current_Calibration_সূচক(SensorString(), _device_id);

	যদি (calibration_সূচক_2 == -1) {
		// no saved calibration available
		Reset();

	} অন্যথা {
		ParametersLoad();
	}
}

যৌক্তিক DifferentialPressure::ParametersLoad()
{
	যদি (calibration_সূচক_2 >= 0 && calibration_সূচক_2 < MAX_SENSOR_গুনতি) {
		// CAL_DPRESx_OFF
		set_offset(GetCalibrationParamFloat(SensorString(), "OFF", calibration_সূচক_2));

		ফেরত সত্য;
	}

	ফেরত মিথ্যা;
}

যৌক্তিক DifferentialPressure::AdoptLegacyOffset()
{
	যদি ((_device_id == 0) || calibrated()) {
		ফেরত মিথ্যা;
	}

	// slot 0 must still be free, otherwise another sensor already owns the legacy offset
	যদি (GetCalibrationParamInt32(SensorString(), "ID", k_Legacy_সূচক) != 0) {
		ফেরত মিথ্যা;
	}

	দশমিক legacy_offset = 0.f;

	যদি (param_get(param_find(kLegacyOffsetParam), &legacy_offset) != PX4_OK) {
		ফেরত মিথ্যা;
	}

	যদি (!PX4_ISFINITE(legacy_offset) || (fabsf(legacy_offset) < FLT_EPSILON)) {
		ফেরত মিথ্যা;
	}

	_offset = legacy_offset;
	calibration_সূচক_2 = k_Legacy_সূচক;

	PX4_INFO("%s %" PRIu32 " adopted %s %.3f Pa", SensorString(), _device_id, kLegacyOffsetParam, (দ্বিগুণ)_offset);

	ফেরত সত্য;
}

শূন্য DifferentialPressure::UpdateLegacyOffset() ধ্রুবক
{
	// keep SENS_DPRES_OFF in step with slot 0 so that ground stations reading it can still tell
	// whether airspeed has been calibrated
	যদি (calibration_সূচক_2 != k_Legacy_সূচক) {
		ফেরত;
	}

	ধ্রুবক param_t handle = param_find(kLegacyOffsetParam);

	যদি (handle == PARAM_INVALID) {
		ফেরত;
	}

	দশমিক offset = _offset;
	param_set_no_notification(handle, &offset);
}

শূন্য DifferentialPressure::Reset()
{
	_offset = 0;

	calibration_সূচক_2 = -1;

	calibration_গুনতি = 0;
}

যৌক্তিক DifferentialPressure::ParametersSave(পূর্ণসংখ্যা desired_calibration_সূচক, যৌক্তিক force)
{
	যদি (force && desired_calibration_সূচক >= 0 && desired_calibration_সূচক < MAX_SENSOR_গুনতি) {
		calibration_সূচক_2 = desired_calibration_সূচক;

	} অন্যথা যদি (!force || (calibration_সূচক_2 < 0)
		   || (desired_calibration_সূচক != -1 && desired_calibration_সূচক != calibration_সূচক_2)) {

		// ensure we have a valid calibration slot (matching existing or first available slot)
		int8_t calibration_সূচক_পূর্ব = calibration_সূচক_2;
		calibration_সূচক_2 = Find_Available_Calibration_সূচক(SensorString(), _device_id, desired_calibration_সূচক);

		যদি (calibration_সূচক_পূর্ব >= 0 && (calibration_সূচক_পূর্ব != calibration_সূচক_2)) {
			PX4_WARN("%s %" PRIu32 " calibration index changed %" PRIi8 " -> %" PRIi8, SensorString(), _device_id,
				 calibration_সূচক_পূর্ব, calibration_সূচক_2);
		}
	}

	যদি (calibration_সূচক_2 >= 0 && calibration_সূচক_2 < MAX_SENSOR_গুনতি) {
		// save calibration
		যৌক্তিক success = সত্য;
		success &= SetCalibrationParam(SensorString(), "ID", calibration_সূচক_2, _device_id);
		success &= SetCalibrationParam(SensorString(), "OFF", calibration_সূচক_2, _offset);

		UpdateLegacyOffset();

		ফেরত success;
	}

	// index 3 is possible: FindAvailableCalibrationIndex() searches four slots, we define three
	PX4_ERR("%s %" PRIu32 " no free calibration slot (got %" PRIi8 ")", SensorString(), _device_id, calibration_সূচক_2);

	ফেরত মিথ্যা;
}

শূন্য DifferentialPressure::PrintStatus()
{
	PX4_INFO_RAW("%s %" PRIu32 " offset: %05.3f Pa, %s\n",
		     SensorString(), device_id(), (দ্বিগুণ)_offset,
		     external() ? "Ext" : "Internal");
}

} // namespace calibration
