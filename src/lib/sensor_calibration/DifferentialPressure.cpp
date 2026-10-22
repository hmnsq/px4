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
	set_device_id(device_id);
}

فراغ DifferentialPressure::set_device_id(uint32_t device_id)
{
	ثابت منطقي external = device::device_is_external(device_id);

	𐤀𐤌 (_device_id != device_id || _external != external) {

		_device_id = device_id;
		_external = external;

		Reset();

		ParametersUpdate();
	}
}

منطقي DifferentialPressure::set_offset(ثابت عائم &offset)
{
	𐤀𐤌 (fabsf(_offset - offset) > 0.01f) {
		𐤀𐤌 (PX4_ISFINITE(offset)) {
			_offset = offset;
			_calibration_count++;
			أرجع true;
		}
	}

	أرجع خطأ;
}

منطقي DifferentialPressure::set_calibration_index(صحيح calibration_index)
{
	𐤀𐤌 ((calibration_index >= 0) && (calibration_index < MAX_SENSOR_COUNT)) {
		_calibration_index = calibration_index;
		أرجع true;
	}

	أرجع خطأ;
}

فراغ DifferentialPressure::ParametersUpdate()
{
	𐤀𐤌 (_device_id == 0) {
		أرجع;
	}

	_calibration_index = FindCurrentCalibrationIndex(SensorString(), _device_id);

	𐤀𐤌 (_calibration_index == -1) {
		// no saved calibration available
		Reset();

	} وإلا {
		ParametersLoad();
	}
}

منطقي DifferentialPressure::ParametersLoad()
{
	𐤀𐤌 (_calibration_index >= 0 && _calibration_index < MAX_SENSOR_COUNT) {
		// CAL_DPRESx_OFF
		set_offset(GetCalibrationParamFloat(SensorString(), "OFF", _calibration_index));

		أرجع true;
	}

	أرجع خطأ;
}

منطقي DifferentialPressure::AdoptLegacyOffset()
{
	𐤀𐤌 ((_device_id == 0) || calibrated()) {
		أرجع خطأ;
	}

	// slot 0 must still be free, otherwise another sensor already owns the legacy offset
	𐤀𐤌 (GetCalibrationParamInt32(SensorString(), "ID", kLegacyIndex) != 0) {
		أرجع خطأ;
	}

	عائم legacy_offset = 0.f;

	𐤀𐤌 (param_get(param_find(kLegacyOffsetParam), &legacy_offset) != PX4_OK) {
		أرجع خطأ;
	}

	𐤀𐤌 (!PX4_ISFINITE(legacy_offset) || (fabsf(legacy_offset) < FLT_EPSILON)) {
		أرجع خطأ;
	}

	_offset = legacy_offset;
	_calibration_index = kLegacyIndex;

	PX4_INFO("%s %" PRIu32 " adopted %s %.3f Pa", SensorString(), _device_id, kLegacyOffsetParam, (مزدوج)_offset);

	أرجع true;
}

فراغ DifferentialPressure::UpdateLegacyOffset() ثابت
{
	// keep SENS_DPRES_OFF in step with slot 0 so that ground stations reading it can still tell
	// whether airspeed has been calibrated
	𐤀𐤌 (_calibration_index != kLegacyIndex) {
		أرجع;
	}

	ثابت param_t handle = param_find(kLegacyOffsetParam);

	𐤀𐤌 (handle == PARAM_INVALID) {
		أرجع;
	}

	عائم offset = _offset;
	param_set_no_notification(handle, &offset);
}

فراغ DifferentialPressure::Reset()
{
	_offset = 0;

	_calibration_index = -1;

	_calibration_count = 0;
}

منطقي DifferentialPressure::ParametersSave(صحيح desired_calibration_index, منطقي force)
{
	𐤀𐤌 (force && desired_calibration_index >= 0 && desired_calibration_index < MAX_SENSOR_COUNT) {
		_calibration_index = desired_calibration_index;

	} وإلا 𐤀𐤌 (!force || (_calibration_index < 0)
		   || (desired_calibration_index != -1 && desired_calibration_index != _calibration_index)) {

		// ensure we have a valid calibration slot (matching existing or first available slot)
		int8_t calibration_index_prev = _calibration_index;
		_calibration_index = FindAvailableCalibrationIndex(SensorString(), _device_id, desired_calibration_index);

		𐤀𐤌 (calibration_index_prev >= 0 && (calibration_index_prev != _calibration_index)) {
			PX4_WARN("%s %" PRIu32 " calibration index changed %" PRIi8 " -> %" PRIi8, SensorString(), _device_id,
				 calibration_index_prev, _calibration_index);
		}
	}

	𐤀𐤌 (_calibration_index >= 0 && _calibration_index < MAX_SENSOR_COUNT) {
		// save calibration
		منطقي success = true;
		success &= SetCalibrationParam(SensorString(), "ID", _calibration_index, _device_id);
		success &= SetCalibrationParam(SensorString(), "OFF", _calibration_index, _offset);

		UpdateLegacyOffset();

		أرجع success;
	}

	// index 3 is possible: FindAvailableCalibrationIndex() searches four slots, we define three
	PX4_ERR("%s %" PRIu32 " no free calibration slot (got %" PRIi8 ")", SensorString(), _device_id, _calibration_index);

	أرجع خطأ;
}

فراغ DifferentialPressure::PrintStatus()
{
	PX4_INFO_RAW("%s %" PRIu32 " offset: %05.3f Pa, %s\n",
		     SensorString(), device_id(), (مزدوج)_offset,
		     external() ? "Ext" : "Internal");
}

} // namespace calibration
