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

#अन्तर्भाव "DifferentialPressure.hpp"

#अन्तर्भाव "Utilities.hpp"

#अन्तर्भाव <lib/drivers/device/Device.hpp>
#अन्तर्भाव <lib/parameters/param.h>

#अन्तर्भाव <float.h>

नामाकाश calibration
{

DifferentialPressure::DifferentialPressure()
{
	Reset();
}

DifferentialPressure::DifferentialPressure(uint32_t device_id)
{
	set_device_id(device_id);
}

शून्य DifferentialPressure::set_device_id(uint32_t device_id)
{
	ध्रुव तार्किक external = device::device_is_external(device_id);

	यदि (_device_id != device_id || _external != external) {

		_device_id = device_id;
		_external = external;

		Reset();

		ParametersUpdate();
	}
}

तार्किक DifferentialPressure::set_offset(ध्रुव भिन्न &offset)
{
	यदि (fabsf(_offset - offset) > 0.01f) {
		यदि (PX4_ISFINITE(offset)) {
			_offset = offset;
			calibration_गिनती++;
			प्रत्यागम सत्य;
		}
	}

	प्रत्यागम असत्य;
}

तार्किक DifferentialPressure::set_calibration_index(पूर्णाङ्क calibration_index)
{
	यदि ((calibration_index >= 0) && (calibration_index < MAX_SENSOR_गिनती)) {
		_calibration_index = calibration_index;
		प्रत्यागम सत्य;
	}

	प्रत्यागम असत्य;
}

शून्य DifferentialPressure::ParametersUpdate()
{
	यदि (_device_id == 0) {
		प्रत्यागम;
	}

	_calibration_index = FindCurrentCalibrationIndex(SensorString(), _device_id);

	यदि (_calibration_index == -1) {
		// no saved calibration available
		Reset();

	} अन्यथा {
		ParametersLoad();
	}
}

तार्किक DifferentialPressure::ParametersLoad()
{
	यदि (_calibration_index >= 0 && _calibration_index < MAX_SENSOR_गिनती) {
		// CAL_DPRESx_OFF
		set_offset(GetCalibrationParamFloat(SensorString(), "OFF", _calibration_index));

		प्रत्यागम सत्य;
	}

	प्रत्यागम असत्य;
}

तार्किक DifferentialPressure::AdoptLegacyOffset()
{
	यदि ((_device_id == 0) || calibrated()) {
		प्रत्यागम असत्य;
	}

	// slot 0 must still be free, otherwise another sensor already owns the legacy offset
	यदि (GetCalibrationParamInt32(SensorString(), "ID", kLegacyIndex) != 0) {
		प्रत्यागम असत्य;
	}

	भिन्न legacy_offset = 0.f;

	यदि (param_get(param_find(kLegacyOffsetParam), &legacy_offset) != PX4_OK) {
		प्रत्यागम असत्य;
	}

	यदि (!PX4_ISFINITE(legacy_offset) || (fabsf(legacy_offset) < FLT_EPSILON)) {
		प्रत्यागम असत्य;
	}

	_offset = legacy_offset;
	_calibration_index = kLegacyIndex;

	PX4_INFO("%s %" PRIu32 " adopted %s %.3f Pa", SensorString(), _device_id, kLegacyOffsetParam, (द्विगुण)_offset);

	प्रत्यागम सत्य;
}

शून्य DifferentialPressure::UpdateLegacyOffset() ध्रुव
{
	// keep SENS_DPRES_OFF in step with slot 0 so that ground stations reading it can still tell
	// whether airspeed has been calibrated
	यदि (_calibration_index != kLegacyIndex) {
		प्रत्यागम;
	}

	ध्रुव param_t handle = param_find(kLegacyOffsetParam);

	यदि (handle == PARAM_INVALID) {
		प्रत्यागम;
	}

	भिन्न offset = _offset;
	param_set_no_notification(handle, &offset);
}

शून्य DifferentialPressure::Reset()
{
	_offset = 0;

	_calibration_index = -1;

	calibration_गिनती = 0;
}

तार्किक DifferentialPressure::ParametersSave(पूर्णाङ्क desired_calibration_index, तार्किक force)
{
	यदि (force && desired_calibration_index >= 0 && desired_calibration_index < MAX_SENSOR_गिनती) {
		_calibration_index = desired_calibration_index;

	} अन्यथा यदि (!force || (_calibration_index < 0)
		   || (desired_calibration_index != -1 && desired_calibration_index != _calibration_index)) {

		// ensure we have a valid calibration slot (matching existing or first available slot)
		int8_t calibration_index_prev = _calibration_index;
		_calibration_index = FindAvailableCalibrationIndex(SensorString(), _device_id, desired_calibration_index);

		यदि (calibration_index_prev >= 0 && (calibration_index_prev != _calibration_index)) {
			PX4_WARN("%s %" PRIu32 " calibration index changed %" PRIi8 " -> %" PRIi8, SensorString(), _device_id,
				 calibration_index_prev, _calibration_index);
		}
	}

	यदि (_calibration_index >= 0 && _calibration_index < MAX_SENSOR_गिनती) {
		// save calibration
		तार्किक success = सत्य;
		success &= SetCalibrationParam(SensorString(), "ID", _calibration_index, _device_id);
		success &= SetCalibrationParam(SensorString(), "OFF", _calibration_index, _offset);

		UpdateLegacyOffset();

		प्रत्यागम success;
	}

	// index 3 is possible: FindAvailableCalibrationIndex() searches four slots, we define three
	PX4_ERR("%s %" PRIu32 " no free calibration slot (got %" PRIi8 ")", SensorString(), _device_id, _calibration_index);

	प्रत्यागम असत्य;
}

शून्य DifferentialPressure::PrintStatus()
{
	PX4_INFO_RAW("%s %" PRIu32 " offset: %05.3f Pa, %s\n",
		     SensorString(), device_id(), (द्विगुण)_offset,
		     external() ? "Ext" : "Internal");
}

} // namespace calibration
