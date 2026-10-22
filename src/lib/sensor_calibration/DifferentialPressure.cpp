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

#അന്തര്ഭാവ "DifferentialPressure.hpp"

#അന്തര്ഭാവ "Utilities.hpp"

#അന്തര്ഭാവ <lib/drivers/device/Device.hpp>
#അന്തര്ഭാവ <lib/parameters/param.h>

#അന്തര്ഭാവ <float.h>

നാമാകാശ calibration
{

DifferentialPressure::DifferentialPressure()
{
	Reset();
}

DifferentialPressure::DifferentialPressure(uint32_t device_id)
{
	set_device_id(device_id);
}

ശൂന്യം DifferentialPressure::set_device_id(uint32_t device_id)
{
	സ്ഥിരം തർക്കം external = device::device_is_external(device_id);

	എങ്കിൽ (_device_id != device_id || _external != external) {

		_device_id = device_id;
		_external = external;

		Reset();

		ParametersUpdate();
	}
}

തർക്കം DifferentialPressure::set_offset(സ്ഥിരം ദശാംശം &offset)
{
	എങ്കിൽ (fabsf(_offset - offset) > 0.01f) {
		എങ്കിൽ (PX4_ISFINITE(offset)) {
			_offset = offset;
			calibration_എണ്ണം++;
			തിരികെ സത്യം;
		}
	}

	തിരികെ അസത്യം;
}

തർക്കം DifferentialPressure::set_calibration_index(പൂർണ്ണസംഖ്യ calibration_index)
{
	എങ്കിൽ ((calibration_index >= 0) && (calibration_index < MAX_SENSOR_എണ്ണം)) {
		_calibration_index = calibration_index;
		തിരികെ സത്യം;
	}

	തിരികെ അസത്യം;
}

ശൂന്യം DifferentialPressure::ParametersUpdate()
{
	എങ്കിൽ (_device_id == 0) {
		തിരികെ;
	}

	_calibration_index = FindCurrentCalibrationIndex(SensorString(), _device_id);

	എങ്കിൽ (_calibration_index == -1) {
		// no saved calibration available
		Reset();

	} അല്ലെങ്കിൽ {
		ParametersLoad();
	}
}

തർക്കം DifferentialPressure::ParametersLoad()
{
	എങ്കിൽ (_calibration_index >= 0 && _calibration_index < MAX_SENSOR_എണ്ണം) {
		// CAL_DPRESx_OFF
		set_offset(GetCalibrationParamFloat(SensorString(), "OFF", _calibration_index));

		തിരികെ സത്യം;
	}

	തിരികെ അസത്യം;
}

തർക്കം DifferentialPressure::AdoptLegacyOffset()
{
	എങ്കിൽ ((_device_id == 0) || calibrated()) {
		തിരികെ അസത്യം;
	}

	// slot 0 must still be free, otherwise another sensor already owns the legacy offset
	എങ്കിൽ (GetCalibrationParamInt32(SensorString(), "ID", kLegacyIndex) != 0) {
		തിരികെ അസത്യം;
	}

	ദശാംശം legacy_offset = 0.f;

	എങ്കിൽ (param_get(param_find(kLegacyOffsetParam), &legacy_offset) != PX4_OK) {
		തിരികെ അസത്യം;
	}

	എങ്കിൽ (!PX4_ISFINITE(legacy_offset) || (fabsf(legacy_offset) < FLT_EPSILON)) {
		തിരികെ അസത്യം;
	}

	_offset = legacy_offset;
	_calibration_index = kLegacyIndex;

	PX4_INFO("%s %" PRIu32 " adopted %s %.3f Pa", SensorString(), _device_id, kLegacyOffsetParam, (ഇരട്ട)_offset);

	തിരികെ സത്യം;
}

ശൂന്യം DifferentialPressure::UpdateLegacyOffset() സ്ഥിരം
{
	// keep SENS_DPRES_OFF in step with slot 0 so that ground stations reading it can still tell
	// whether airspeed has been calibrated
	എങ്കിൽ (_calibration_index != kLegacyIndex) {
		തിരികെ;
	}

	സ്ഥിരം param_t handle = param_find(kLegacyOffsetParam);

	എങ്കിൽ (handle == PARAM_INVALID) {
		തിരികെ;
	}

	ദശാംശം offset = _offset;
	param_set_no_notification(handle, &offset);
}

ശൂന്യം DifferentialPressure::Reset()
{
	_offset = 0;

	_calibration_index = -1;

	calibration_എണ്ണം = 0;
}

തർക്കം DifferentialPressure::ParametersSave(പൂർണ്ണസംഖ്യ desired_calibration_index, തർക്കം force)
{
	എങ്കിൽ (force && desired_calibration_index >= 0 && desired_calibration_index < MAX_SENSOR_എണ്ണം) {
		_calibration_index = desired_calibration_index;

	} അല്ലെങ്കിൽ എങ്കിൽ (!force || (_calibration_index < 0)
		   || (desired_calibration_index != -1 && desired_calibration_index != _calibration_index)) {

		// ensure we have a valid calibration slot (matching existing or first available slot)
		int8_t calibration_index_prev = _calibration_index;
		_calibration_index = FindAvailableCalibrationIndex(SensorString(), _device_id, desired_calibration_index);

		എങ്കിൽ (calibration_index_prev >= 0 && (calibration_index_prev != _calibration_index)) {
			PX4_WARN("%s %" PRIu32 " calibration index changed %" PRIi8 " -> %" PRIi8, SensorString(), _device_id,
				 calibration_index_prev, _calibration_index);
		}
	}

	എങ്കിൽ (_calibration_index >= 0 && _calibration_index < MAX_SENSOR_എണ്ണം) {
		// save calibration
		തർക്കം success = സത്യം;
		success &= SetCalibrationParam(SensorString(), "ID", _calibration_index, _device_id);
		success &= SetCalibrationParam(SensorString(), "OFF", _calibration_index, _offset);

		UpdateLegacyOffset();

		തിരികെ success;
	}

	// index 3 is possible: FindAvailableCalibrationIndex() searches four slots, we define three
	PX4_ERR("%s %" PRIu32 " no free calibration slot (got %" PRIi8 ")", SensorString(), _device_id, _calibration_index);

	തിരികെ അസത്യം;
}

ശൂന്യം DifferentialPressure::PrintStatus()
{
	PX4_INFO_RAW("%s %" PRIu32 " offset: %05.3f Pa, %s\n",
		     SensorString(), device_id(), (ഇരട്ട)_offset,
		     external() ? "Ext" : "Internal");
}

} // namespace calibration
