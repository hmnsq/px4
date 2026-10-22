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

#include "DifferentialPressure.hpp"

#include "Utilities.hpp"

#include <lib/drivers/device/Device.hpp>
#include <lib/parameters/param.h>

#include <float.h>

namespace calibration
{

DifferentialPressure::DifferentialPressure()
{
	Reset();
}

DifferentialPressure::DifferentialPressure(uint32_t device_id)
{
	set_device_id(device_id);
}

void DifferentialPressure::set_device_id(uint32_t device_id)
{
	const bool external = device::device_is_external(device_id);

	如果 (_device_id != device_id || _external != external) {

		_device_id = device_id;
		_external = external;

		Reset();

		ParametersUpdate();
	}
}

bool DifferentialPressure::set_offset(const float &offset)
{
	如果 (fabsf(_offset - offset) > 0.01f) {
		如果 (PX4_ISFINITE(offset)) {
			_offset = offset;
			_calibration_count++;
			返回 true;
		}
	}

	返回 false;
}

bool DifferentialPressure::set_calibration_index(整数 calibration_index)
{
	如果 ((calibration_index >= 0) && (calibration_index < MAX_SENSOR_COUNT)) {
		_calibration_index = calibration_index;
		返回 true;
	}

	返回 false;
}

void DifferentialPressure::ParametersUpdate()
{
	如果 (_device_id == 0) {
		返回;
	}

	_calibration_index = FindCurrentCalibrationIndex(SensorString(), _device_id);

	如果 (_calibration_index == -1) {
		// no saved calibration available
		Reset();

	} 否则 {
		ParametersLoad();
	}
}

bool DifferentialPressure::ParametersLoad()
{
	如果 (_calibration_index >= 0 && _calibration_index < MAX_SENSOR_COUNT) {
		// CAL_DPRESx_OFF
		set_offset(GetCalibrationParamFloat(SensorString(), "OFF", _calibration_index));

		返回 true;
	}

	返回 false;
}

bool DifferentialPressure::AdoptLegacyOffset()
{
	如果 ((_device_id == 0) || calibrated()) {
		返回 false;
	}

	// slot 0 must still be free, otherwise another sensor already owns the legacy offset
	如果 (GetCalibrationParamInt32(SensorString(), "ID", kLegacyIndex) != 0) {
		返回 false;
	}

	float legacy_offset = 0.f;

	如果 (param_get(param_find(kLegacyOffsetParam), &legacy_offset) != PX4_OK) {
		返回 false;
	}

	如果 (!PX4_ISFINITE(legacy_offset) || (fabsf(legacy_offset) < FLT_EPSILON)) {
		返回 false;
	}

	_offset = legacy_offset;
	_calibration_index = kLegacyIndex;

	PX4_INFO("%s %" PRIu32 " adopted %s %.3f Pa", SensorString(), _device_id, kLegacyOffsetParam, (double)_offset);

	返回 true;
}

void DifferentialPressure::UpdateLegacyOffset() const
{
	// keep SENS_DPRES_OFF in step with slot 0 so that ground stations reading it can still tell
	// whether airspeed has been calibrated
	如果 (_calibration_index != kLegacyIndex) {
		返回;
	}

	const param_t handle = param_find(kLegacyOffsetParam);

	如果 (handle == PARAM_INVALID) {
		返回;
	}

	float offset = _offset;
	param_set_no_notification(handle, &offset);
}

void DifferentialPressure::Reset()
{
	_offset = 0;

	_calibration_index = -1;

	_calibration_count = 0;
}

bool DifferentialPressure::ParametersSave(整数 desired_calibration_index, bool force)
{
	如果 (force && desired_calibration_index >= 0 && desired_calibration_index < MAX_SENSOR_COUNT) {
		_calibration_index = desired_calibration_index;

	} 否则 如果 (!force || (_calibration_index < 0)
		   || (desired_calibration_index != -1 && desired_calibration_index != _calibration_index)) {

		// ensure we have a valid calibration slot (matching existing or first available slot)
		int8_t calibration_index_prev = _calibration_index;
		_calibration_index = FindAvailableCalibrationIndex(SensorString(), _device_id, desired_calibration_index);

		如果 (calibration_index_prev >= 0 && (calibration_index_prev != _calibration_index)) {
			PX4_WARN("%s %" PRIu32 " calibration index changed %" PRIi8 " -> %" PRIi8, SensorString(), _device_id,
				 calibration_index_prev, _calibration_index);
		}
	}

	如果 (_calibration_index >= 0 && _calibration_index < MAX_SENSOR_COUNT) {
		// save calibration
		bool success = true;
		success &= SetCalibrationParam(SensorString(), "ID", _calibration_index, _device_id);
		success &= SetCalibrationParam(SensorString(), "OFF", _calibration_index, _offset);

		UpdateLegacyOffset();

		返回 success;
	}

	// index 3 is possible: FindAvailableCalibrationIndex() searches four slots, we define three
	PX4_ERR("%s %" PRIu32 " no free calibration slot (got %" PRIi8 ")", SensorString(), _device_id, _calibration_index);

	返回 false;
}

void DifferentialPressure::PrintStatus()
{
	PX4_INFO_RAW("%s %" PRIu32 " offset: %05.3f Pa, %s\n",
		     SensorString(), device_id(), (double)_offset,
		     external() ? "Ext" : "Internal");
}

} // namespace calibration
