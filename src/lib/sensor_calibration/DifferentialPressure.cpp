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
	ধ্রুব তার্কিক external = device::device_is_external(device_id);

	যদি (_device_id != device_id || _external != external) {

		_device_id = device_id;
		_external = external;

		Reset();

		ParametersUpdate();
	}
}

তার্কিক DifferentialPressure::set_offset(ধ্রুব ভিন্ন &offset)
{
	যদি (fabsf(_offset - offset) > 0.01f) {
		যদি (PX4_ISFINITE(offset)) {
			_offset = offset;
			calibration_গুনতি++;
			প্রত্যাগম সত্য;
		}
	}

	প্রত্যাগম অসত্য;
}

তার্কিক DifferentialPressure::set_calibration_index(পূর্ণাঙ্ক calibration_index)
{
	যদি ((calibration_index >= 0) && (calibration_index < MAX_SENSOR_গুনতি)) {
		_calibration_index = calibration_index;
		প্রত্যাগম সত্য;
	}

	প্রত্যাগম অসত্য;
}

শূন্য DifferentialPressure::ParametersUpdate()
{
	যদি (_device_id == 0) {
		প্রত্যাগম;
	}

	_calibration_index = FindCurrentCalibrationIndex(SensorString(), _device_id);

	যদি (_calibration_index == -1) {
		// no saved calibration available
		Reset();

	} অন্যথা {
		ParametersLoad();
	}
}

তার্কিক DifferentialPressure::ParametersLoad()
{
	যদি (_calibration_index >= 0 && _calibration_index < MAX_SENSOR_গুনতি) {
		// CAL_DPRESx_OFF
		set_offset(GetCalibrationParamFloat(SensorString(), "OFF", _calibration_index));

		প্রত্যাগম সত্য;
	}

	প্রত্যাগম অসত্য;
}

তার্কিক DifferentialPressure::AdoptLegacyOffset()
{
	যদি ((_device_id == 0) || calibrated()) {
		প্রত্যাগম অসত্য;
	}

	// slot 0 must still be free, otherwise another sensor already owns the legacy offset
	যদি (GetCalibrationParamInt32(SensorString(), "ID", kLegacyIndex) != 0) {
		প্রত্যাগম অসত্য;
	}

	ভিন্ন legacy_offset = 0.f;

	যদি (param_get(param_find(kLegacyOffsetParam), &legacy_offset) != PX4_OK) {
		প্রত্যাগম অসত্য;
	}

	যদি (!PX4_ISFINITE(legacy_offset) || (fabsf(legacy_offset) < FLT_EPSILON)) {
		প্রত্যাগম অসত্য;
	}

	_offset = legacy_offset;
	_calibration_index = kLegacyIndex;

	PX4_INFO("%s %" PRIu32 " adopted %s %.3f Pa", SensorString(), _device_id, kLegacyOffsetParam, (দ্বিগুণ)_offset);

	প্রত্যাগম সত্য;
}

শূন্য DifferentialPressure::UpdateLegacyOffset() ধ্রুব
{
	// keep SENS_DPRES_OFF in step with slot 0 so that ground stations reading it can still tell
	// whether airspeed has been calibrated
	যদি (_calibration_index != kLegacyIndex) {
		প্রত্যাগম;
	}

	ধ্রুব param_t handle = param_find(kLegacyOffsetParam);

	যদি (handle == PARAM_INVALID) {
		প্রত্যাগম;
	}

	ভিন্ন offset = _offset;
	param_set_no_notification(handle, &offset);
}

শূন্য DifferentialPressure::Reset()
{
	_offset = 0;

	_calibration_index = -1;

	calibration_গুনতি = 0;
}

তার্কিক DifferentialPressure::ParametersSave(পূর্ণাঙ্ক desired_calibration_index, তার্কিক force)
{
	যদি (force && desired_calibration_index >= 0 && desired_calibration_index < MAX_SENSOR_গুনতি) {
		_calibration_index = desired_calibration_index;

	} অন্যথা যদি (!force || (_calibration_index < 0)
		   || (desired_calibration_index != -1 && desired_calibration_index != _calibration_index)) {

		// ensure we have a valid calibration slot (matching existing or first available slot)
		int8_t calibration_index_prev = _calibration_index;
		_calibration_index = FindAvailableCalibrationIndex(SensorString(), _device_id, desired_calibration_index);

		যদি (calibration_index_prev >= 0 && (calibration_index_prev != _calibration_index)) {
			PX4_WARN("%s %" PRIu32 " calibration index changed %" PRIi8 " -> %" PRIi8, SensorString(), _device_id,
				 calibration_index_prev, _calibration_index);
		}
	}

	যদি (_calibration_index >= 0 && _calibration_index < MAX_SENSOR_গুনতি) {
		// save calibration
		তার্কিক success = সত্য;
		success &= SetCalibrationParam(SensorString(), "ID", _calibration_index, _device_id);
		success &= SetCalibrationParam(SensorString(), "OFF", _calibration_index, _offset);

		UpdateLegacyOffset();

		প্রত্যাগম success;
	}

	// index 3 is possible: FindAvailableCalibrationIndex() searches four slots, we define three
	PX4_ERR("%s %" PRIu32 " no free calibration slot (got %" PRIi8 ")", SensorString(), _device_id, _calibration_index);

	প্রত্যাগম অসত্য;
}

শূন্য DifferentialPressure::PrintStatus()
{
	PX4_INFO_RAW("%s %" PRIu32 " offset: %05.3f Pa, %s\n",
		     SensorString(), device_id(), (দ্বিগুণ)_offset,
		     external() ? "Ext" : "Internal");
}

} // namespace calibration
