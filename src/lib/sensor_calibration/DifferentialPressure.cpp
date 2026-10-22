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

#અન્તર્ભાવ "DifferentialPressure.hpp"

#અન્તર્ભાવ "Utilities.hpp"

#અન્તર્ભાવ <lib/drivers/device/Device.hpp>
#અન્તર્ભાવ <lib/parameters/param.h>

#અન્તર્ભાવ <float.h>

નામાકાશ calibration
{

DifferentialPressure::DifferentialPressure()
{
	Reset();
}

DifferentialPressure::DifferentialPressure(uint32_t device_id)
{
	set_device_id(device_id);
}

શૂન્ય DifferentialPressure::set_device_id(uint32_t device_id)
{
	અચલ તાર્કિક external = device::device_is_external(device_id);

	જો (_device_id != device_id || _external != external) {

		_device_id = device_id;
		_external = external;

		Reset();

		ParametersUpdate();
	}
}

તાર્કિક DifferentialPressure::set_offset(અચલ દશાંશ &offset)
{
	જો (fabsf(_offset - offset) > 0.01f) {
		જો (PX4_ISFINITE(offset)) {
			_offset = offset;
			calibration_ગણતરી++;
			પાછા સાચું;
		}
	}

	પાછા ખોટું;
}

તાર્કિક DifferentialPressure::set_calibration_index(પૂર્ણાંક calibration_index)
{
	જો ((calibration_index >= 0) && (calibration_index < MAX_SENSOR_ગણતરી)) {
		_calibration_index = calibration_index;
		પાછા સાચું;
	}

	પાછા ખોટું;
}

શૂન્ય DifferentialPressure::ParametersUpdate()
{
	જો (_device_id == 0) {
		પાછા;
	}

	_calibration_index = FindCurrentCalibrationIndex(SensorString(), _device_id);

	જો (_calibration_index == -1) {
		// no saved calibration available
		Reset();

	} નહીંતર {
		ParametersLoad();
	}
}

તાર્કિક DifferentialPressure::ParametersLoad()
{
	જો (_calibration_index >= 0 && _calibration_index < MAX_SENSOR_ગણતરી) {
		// CAL_DPRESx_OFF
		set_offset(GetCalibrationParamFloat(SensorString(), "OFF", _calibration_index));

		પાછા સાચું;
	}

	પાછા ખોટું;
}

તાર્કિક DifferentialPressure::AdoptLegacyOffset()
{
	જો ((_device_id == 0) || calibrated()) {
		પાછા ખોટું;
	}

	// slot 0 must still be free, otherwise another sensor already owns the legacy offset
	જો (GetCalibrationParamInt32(SensorString(), "ID", kLegacyIndex) != 0) {
		પાછા ખોટું;
	}

	દશાંશ legacy_offset = 0.f;

	જો (param_get(param_find(kLegacyOffsetParam), &legacy_offset) != PX4_OK) {
		પાછા ખોટું;
	}

	જો (!PX4_ISFINITE(legacy_offset) || (fabsf(legacy_offset) < FLT_EPSILON)) {
		પાછા ખોટું;
	}

	_offset = legacy_offset;
	_calibration_index = kLegacyIndex;

	PX4_INFO("%s %" PRIu32 " adopted %s %.3f Pa", SensorString(), _device_id, kLegacyOffsetParam, (દ્વિગુણ)_offset);

	પાછા સાચું;
}

શૂન્ય DifferentialPressure::UpdateLegacyOffset() અચલ
{
	// keep SENS_DPRES_OFF in step with slot 0 so that ground stations reading it can still tell
	// whether airspeed has been calibrated
	જો (_calibration_index != kLegacyIndex) {
		પાછા;
	}

	અચલ param_t handle = param_find(kLegacyOffsetParam);

	જો (handle == PARAM_INVALID) {
		પાછા;
	}

	દશાંશ offset = _offset;
	param_set_no_notification(handle, &offset);
}

શૂન્ય DifferentialPressure::Reset()
{
	_offset = 0;

	_calibration_index = -1;

	calibration_ગણતરી = 0;
}

તાર્કિક DifferentialPressure::ParametersSave(પૂર્ણાંક desired_calibration_index, તાર્કિક force)
{
	જો (force && desired_calibration_index >= 0 && desired_calibration_index < MAX_SENSOR_ગણતરી) {
		_calibration_index = desired_calibration_index;

	} નહીંતર જો (!force || (_calibration_index < 0)
		   || (desired_calibration_index != -1 && desired_calibration_index != _calibration_index)) {

		// ensure we have a valid calibration slot (matching existing or first available slot)
		int8_t calibration_index_prev = _calibration_index;
		_calibration_index = FindAvailableCalibrationIndex(SensorString(), _device_id, desired_calibration_index);

		જો (calibration_index_prev >= 0 && (calibration_index_prev != _calibration_index)) {
			PX4_WARN("%s %" PRIu32 " calibration index changed %" PRIi8 " -> %" PRIi8, SensorString(), _device_id,
				 calibration_index_prev, _calibration_index);
		}
	}

	જો (_calibration_index >= 0 && _calibration_index < MAX_SENSOR_ગણતરી) {
		// save calibration
		તાર્કિક success = સાચું;
		success &= SetCalibrationParam(SensorString(), "ID", _calibration_index, _device_id);
		success &= SetCalibrationParam(SensorString(), "OFF", _calibration_index, _offset);

		UpdateLegacyOffset();

		પાછા success;
	}

	// index 3 is possible: FindAvailableCalibrationIndex() searches four slots, we define three
	PX4_ERR("%s %" PRIu32 " no free calibration slot (got %" PRIi8 ")", SensorString(), _device_id, _calibration_index);

	પાછા ખોટું;
}

શૂન્ય DifferentialPressure::PrintStatus()
{
	PX4_INFO_RAW("%s %" PRIu32 " offset: %05.3f Pa, %s\n",
		     SensorString(), device_id(), (દ્વિગુણ)_offset,
		     external() ? "Ext" : "Internal");
}

} // namespace calibration
