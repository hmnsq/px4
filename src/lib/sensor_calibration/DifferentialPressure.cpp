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

#அந்தர்பாவ "DifferentialPressure.hpp"

#அந்தர்பாவ "Utilities.hpp"

#அந்தர்பாவ <lib/drivers/device/Device.hpp>
#அந்தர்பாவ <lib/parameters/param.h>

#அந்தர்பாவ <float.h>

நாமாகாஶ calibration
{

DifferentialPressure::DifferentialPressure()
{
	Reset();
}

DifferentialPressure::DifferentialPressure(uint32_t device_id)
{
	set_device_id(device_id);
}

வெறுமை DifferentialPressure::set_device_id(uint32_t device_id)
{
	மாறிலி தருக்க external = device::device_is_external(device_id);

	எனில் (_device_id != device_id || _external != external) {

		_device_id = device_id;
		_external = external;

		Reset();

		ParametersUpdate();
	}
}

தருக்க DifferentialPressure::set_offset(மாறிலி தசம &offset)
{
	எனில் (fabsf(_offset - offset) > 0.01f) {
		எனில் (PX4_ISFINITE(offset)) {
			_offset = offset;
			calibration_மொத்தம்++;
			திரும்பு உண்மை;
		}
	}

	திரும்பு பொய்;
}

தருக்க DifferentialPressure::set_calibration_index(முழு calibration_index)
{
	எனில் ((calibration_index >= 0) && (calibration_index < MAX_SENSOR_மொத்தம்)) {
		_calibration_index = calibration_index;
		திரும்பு உண்மை;
	}

	திரும்பு பொய்;
}

வெறுமை DifferentialPressure::ParametersUpdate()
{
	எனில் (_device_id == 0) {
		திரும்பு;
	}

	_calibration_index = FindCurrentCalibrationIndex(SensorString(), _device_id);

	எனில் (_calibration_index == -1) {
		// no saved calibration available
		Reset();

	} இல்லையெனில் {
		ParametersLoad();
	}
}

தருக்க DifferentialPressure::ParametersLoad()
{
	எனில் (_calibration_index >= 0 && _calibration_index < MAX_SENSOR_மொத்தம்) {
		// CAL_DPRESx_OFF
		set_offset(GetCalibrationParamFloat(SensorString(), "OFF", _calibration_index));

		திரும்பு உண்மை;
	}

	திரும்பு பொய்;
}

தருக்க DifferentialPressure::AdoptLegacyOffset()
{
	எனில் ((_device_id == 0) || calibrated()) {
		திரும்பு பொய்;
	}

	// slot 0 must still be free, otherwise another sensor already owns the legacy offset
	எனில் (GetCalibrationParamInt32(SensorString(), "ID", kLegacyIndex) != 0) {
		திரும்பு பொய்;
	}

	தசம legacy_offset = 0.f;

	எனில் (param_get(param_find(kLegacyOffsetParam), &legacy_offset) != PX4_OK) {
		திரும்பு பொய்;
	}

	எனில் (!PX4_ISFINITE(legacy_offset) || (fabsf(legacy_offset) < FLT_EPSILON)) {
		திரும்பு பொய்;
	}

	_offset = legacy_offset;
	_calibration_index = kLegacyIndex;

	PX4_INFO("%s %" PRIu32 " adopted %s %.3f Pa", SensorString(), _device_id, kLegacyOffsetParam, (இரட்டை)_offset);

	திரும்பு உண்மை;
}

வெறுமை DifferentialPressure::UpdateLegacyOffset() மாறிலி
{
	// keep SENS_DPRES_OFF in step with slot 0 so that ground stations reading it can still tell
	// whether airspeed has been calibrated
	எனில் (_calibration_index != kLegacyIndex) {
		திரும்பு;
	}

	மாறிலி param_t handle = param_find(kLegacyOffsetParam);

	எனில் (handle == PARAM_INVALID) {
		திரும்பு;
	}

	தசம offset = _offset;
	param_set_no_notification(handle, &offset);
}

வெறுமை DifferentialPressure::Reset()
{
	_offset = 0;

	_calibration_index = -1;

	calibration_மொத்தம் = 0;
}

தருக்க DifferentialPressure::ParametersSave(முழு desired_calibration_index, தருக்க force)
{
	எனில் (force && desired_calibration_index >= 0 && desired_calibration_index < MAX_SENSOR_மொத்தம்) {
		_calibration_index = desired_calibration_index;

	} இல்லையெனில் எனில் (!force || (_calibration_index < 0)
		   || (desired_calibration_index != -1 && desired_calibration_index != _calibration_index)) {

		// ensure we have a valid calibration slot (matching existing or first available slot)
		int8_t calibration_index_prev = _calibration_index;
		_calibration_index = FindAvailableCalibrationIndex(SensorString(), _device_id, desired_calibration_index);

		எனில் (calibration_index_prev >= 0 && (calibration_index_prev != _calibration_index)) {
			PX4_WARN("%s %" PRIu32 " calibration index changed %" PRIi8 " -> %" PRIi8, SensorString(), _device_id,
				 calibration_index_prev, _calibration_index);
		}
	}

	எனில் (_calibration_index >= 0 && _calibration_index < MAX_SENSOR_மொத்தம்) {
		// save calibration
		தருக்க success = உண்மை;
		success &= SetCalibrationParam(SensorString(), "ID", _calibration_index, _device_id);
		success &= SetCalibrationParam(SensorString(), "OFF", _calibration_index, _offset);

		UpdateLegacyOffset();

		திரும்பு success;
	}

	// index 3 is possible: FindAvailableCalibrationIndex() searches four slots, we define three
	PX4_ERR("%s %" PRIu32 " no free calibration slot (got %" PRIi8 ")", SensorString(), _device_id, _calibration_index);

	திரும்பு பொய்;
}

வெறுமை DifferentialPressure::PrintStatus()
{
	PX4_INFO_RAW("%s %" PRIu32 " offset: %05.3f Pa, %s\n",
		     SensorString(), device_id(), (இரட்டை)_offset,
		     external() ? "Ext" : "Internal");
}

} // namespace calibration
