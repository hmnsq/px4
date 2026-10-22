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

فضاینام calibration
{

DifferentialPressure::DifferentialPressure()
{
	Reset();
}

DifferentialPressure::DifferentialPressure(uint32_t device_id)
{
	set_device_id(device_id);
}

تهی DifferentialPressure::set_device_id(uint32_t device_id)
{
	ثابت منطقی external = device::device_is_external(device_id);

	اگر (_device_id != device_id || _external != external) {

		_device_id = device_id;
		_external = external;

		Reset();

		ParametersUpdate();
	}
}

منطقی DifferentialPressure::set_offset(ثابت اعشاری &offset)
{
	اگر (fabsf(_offset - offset) > 0.01f) {
		اگر (PX4_ISFINITE(offset)) {
			_offset = offset;
			calibration_شمار++;
			بازگشت درست;
		}
	}

	بازگشت نادرست;
}

منطقی DifferentialPressure::set_calibration_index(صحیح calibration_index)
{
	اگر ((calibration_index >= 0) && (calibration_index < MAX_SENSOR_شمار)) {
		_calibration_index = calibration_index;
		بازگشت درست;
	}

	بازگشت نادرست;
}

تهی DifferentialPressure::ParametersUpdate()
{
	اگر (_device_id == 0) {
		بازگشت;
	}

	_calibration_index = FindCurrentCalibrationIndex(SensorString(), _device_id);

	اگر (_calibration_index == -1) {
		// no saved calibration available
		Reset();

	} وگرنه {
		ParametersLoad();
	}
}

منطقی DifferentialPressure::ParametersLoad()
{
	اگر (_calibration_index >= 0 && _calibration_index < MAX_SENSOR_شمار) {
		// CAL_DPRESx_OFF
		set_offset(GetCalibrationParamFloat(SensorString(), "OFF", _calibration_index));

		بازگشت درست;
	}

	بازگشت نادرست;
}

منطقی DifferentialPressure::AdoptLegacyOffset()
{
	اگر ((_device_id == 0) || calibrated()) {
		بازگشت نادرست;
	}

	// slot 0 must still be free, otherwise another sensor already owns the legacy offset
	اگر (GetCalibrationParamInt32(SensorString(), "ID", kLegacyIndex) != 0) {
		بازگشت نادرست;
	}

	اعشاری legacy_offset = 0.f;

	اگر (param_get(param_find(kLegacyOffsetParam), &legacy_offset) != PX4_OK) {
		بازگشت نادرست;
	}

	اگر (!PX4_ISFINITE(legacy_offset) || (fabsf(legacy_offset) < FLT_EPSILON)) {
		بازگشت نادرست;
	}

	_offset = legacy_offset;
	_calibration_index = kLegacyIndex;

	PX4_INFO("%s %" PRIu32 " adopted %s %.3f Pa", SensorString(), _device_id, kLegacyOffsetParam, (دوگانه)_offset);

	بازگشت درست;
}

تهی DifferentialPressure::UpdateLegacyOffset() ثابت
{
	// keep SENS_DPRES_OFF in step with slot 0 so that ground stations reading it can still tell
	// whether airspeed has been calibrated
	اگر (_calibration_index != kLegacyIndex) {
		بازگشت;
	}

	ثابت param_t handle = param_find(kLegacyOffsetParam);

	اگر (handle == PARAM_INVALID) {
		بازگشت;
	}

	اعشاری offset = _offset;
	param_set_no_notification(handle, &offset);
}

تهی DifferentialPressure::Reset()
{
	_offset = 0;

	_calibration_index = -1;

	calibration_شمار = 0;
}

منطقی DifferentialPressure::ParametersSave(صحیح desired_calibration_index, منطقی force)
{
	اگر (force && desired_calibration_index >= 0 && desired_calibration_index < MAX_SENSOR_شمار) {
		_calibration_index = desired_calibration_index;

	} وگرنه اگر (!force || (_calibration_index < 0)
		   || (desired_calibration_index != -1 && desired_calibration_index != _calibration_index)) {

		// ensure we have a valid calibration slot (matching existing or first available slot)
		int8_t calibration_index_prev = _calibration_index;
		_calibration_index = FindAvailableCalibrationIndex(SensorString(), _device_id, desired_calibration_index);

		اگر (calibration_index_prev >= 0 && (calibration_index_prev != _calibration_index)) {
			PX4_WARN("%s %" PRIu32 " calibration index changed %" PRIi8 " -> %" PRIi8, SensorString(), _device_id,
				 calibration_index_prev, _calibration_index);
		}
	}

	اگر (_calibration_index >= 0 && _calibration_index < MAX_SENSOR_شمار) {
		// save calibration
		منطقی success = درست;
		success &= SetCalibrationParam(SensorString(), "ID", _calibration_index, _device_id);
		success &= SetCalibrationParam(SensorString(), "OFF", _calibration_index, _offset);

		UpdateLegacyOffset();

		بازگشت success;
	}

	// index 3 is possible: FindAvailableCalibrationIndex() searches four slots, we define three
	PX4_ERR("%s %" PRIu32 " no free calibration slot (got %" PRIi8 ")", SensorString(), _device_id, _calibration_index);

	بازگشت نادرست;
}

تهی DifferentialPressure::PrintStatus()
{
	PX4_INFO_RAW("%s %" PRIu32 " offset: %05.3f Pa, %s\n",
		     SensorString(), device_id(), (دوگانه)_offset,
		     external() ? "Ext" : "Internal");
}

} // namespace calibration
