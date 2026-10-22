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

#ଅନ୍ତର୍ଭାଵ "DifferentialPressure.hpp"

#ଅନ୍ତର୍ଭାଵ "Utilities.hpp"

#ଅନ୍ତର୍ଭାଵ <lib/drivers/device/Device.hpp>
#ଅନ୍ତର୍ଭାଵ <lib/parameters/param.h>

#ଅନ୍ତର୍ଭାଵ <float.h>

ନାମାକାଶ calibration
{

DifferentialPressure::DifferentialPressure()
{
	Reset();
}

DifferentialPressure::DifferentialPressure(uint32_t device_id)
{
	set_device_id(device_id);
}

ଶୂନ୍ୟ DifferentialPressure::set_device_id(uint32_t device_id)
{
	ଅଚଳ ତାର୍କିକ external = device::device_is_external(device_id);

	ଯଦି (_device_id != device_id || _external != external) {

		_device_id = device_id;
		_external = external;

		Reset();

		ParametersUpdate();
	}
}

ତାର୍କିକ DifferentialPressure::set_offset(ଅଚଳ ଦଶମିକ &offset)
{
	ଯଦି (fabsf(_offset - offset) > 0.01f) {
		ଯଦି (PX4_ISFINITE(offset)) {
			_offset = offset;
			calibration_ଗଣନା++;
			ଫେରନ୍ତୁ ସତ୍ୟ;
		}
	}

	ଫେରନ୍ତୁ ମିଥ୍ୟା;
}

ତାର୍କିକ DifferentialPressure::set_calibration_index(ପୂର୍ଣ୍ଣସଂଖ୍ୟା calibration_index)
{
	ଯଦି ((calibration_index >= 0) && (calibration_index < MAX_SENSOR_ଗଣନା)) {
		_calibration_index = calibration_index;
		ଫେରନ୍ତୁ ସତ୍ୟ;
	}

	ଫେରନ୍ତୁ ମିଥ୍ୟା;
}

ଶୂନ୍ୟ DifferentialPressure::ParametersUpdate()
{
	ଯଦି (_device_id == 0) {
		ଫେରନ୍ତୁ;
	}

	_calibration_index = FindCurrentCalibrationIndex(SensorString(), _device_id);

	ଯଦି (_calibration_index == -1) {
		// no saved calibration available
		Reset();

	} ଅନ୍ୟଥା {
		ParametersLoad();
	}
}

ତାର୍କିକ DifferentialPressure::ParametersLoad()
{
	ଯଦି (_calibration_index >= 0 && _calibration_index < MAX_SENSOR_ଗଣନା) {
		// CAL_DPRESx_OFF
		set_offset(GetCalibrationParamFloat(SensorString(), "OFF", _calibration_index));

		ଫେରନ୍ତୁ ସତ୍ୟ;
	}

	ଫେରନ୍ତୁ ମିଥ୍ୟା;
}

ତାର୍କିକ DifferentialPressure::AdoptLegacyOffset()
{
	ଯଦି ((_device_id == 0) || calibrated()) {
		ଫେରନ୍ତୁ ମିଥ୍ୟା;
	}

	// slot 0 must still be free, otherwise another sensor already owns the legacy offset
	ଯଦି (GetCalibrationParamInt32(SensorString(), "ID", kLegacyIndex) != 0) {
		ଫେରନ୍ତୁ ମିଥ୍ୟା;
	}

	ଦଶମିକ legacy_offset = 0.f;

	ଯଦି (param_get(param_find(kLegacyOffsetParam), &legacy_offset) != PX4_OK) {
		ଫେରନ୍ତୁ ମିଥ୍ୟା;
	}

	ଯଦି (!PX4_ISFINITE(legacy_offset) || (fabsf(legacy_offset) < FLT_EPSILON)) {
		ଫେରନ୍ତୁ ମିଥ୍ୟା;
	}

	_offset = legacy_offset;
	_calibration_index = kLegacyIndex;

	PX4_INFO("%s %" PRIu32 " adopted %s %.3f Pa", SensorString(), _device_id, kLegacyOffsetParam, (ଦ୍ୱିଗୁଣ)_offset);

	ଫେରନ୍ତୁ ସତ୍ୟ;
}

ଶୂନ୍ୟ DifferentialPressure::UpdateLegacyOffset() ଅଚଳ
{
	// keep SENS_DPRES_OFF in step with slot 0 so that ground stations reading it can still tell
	// whether airspeed has been calibrated
	ଯଦି (_calibration_index != kLegacyIndex) {
		ଫେରନ୍ତୁ;
	}

	ଅଚଳ param_t handle = param_find(kLegacyOffsetParam);

	ଯଦି (handle == PARAM_INVALID) {
		ଫେରନ୍ତୁ;
	}

	ଦଶମିକ offset = _offset;
	param_set_no_notification(handle, &offset);
}

ଶୂନ୍ୟ DifferentialPressure::Reset()
{
	_offset = 0;

	_calibration_index = -1;

	calibration_ଗଣନା = 0;
}

ତାର୍କିକ DifferentialPressure::ParametersSave(ପୂର୍ଣ୍ଣସଂଖ୍ୟା desired_calibration_index, ତାର୍କିକ force)
{
	ଯଦି (force && desired_calibration_index >= 0 && desired_calibration_index < MAX_SENSOR_ଗଣନା) {
		_calibration_index = desired_calibration_index;

	} ଅନ୍ୟଥା ଯଦି (!force || (_calibration_index < 0)
		   || (desired_calibration_index != -1 && desired_calibration_index != _calibration_index)) {

		// ensure we have a valid calibration slot (matching existing or first available slot)
		int8_t calibration_index_prev = _calibration_index;
		_calibration_index = FindAvailableCalibrationIndex(SensorString(), _device_id, desired_calibration_index);

		ଯଦି (calibration_index_prev >= 0 && (calibration_index_prev != _calibration_index)) {
			PX4_WARN("%s %" PRIu32 " calibration index changed %" PRIi8 " -> %" PRIi8, SensorString(), _device_id,
				 calibration_index_prev, _calibration_index);
		}
	}

	ଯଦି (_calibration_index >= 0 && _calibration_index < MAX_SENSOR_ଗଣନା) {
		// save calibration
		ତାର୍କିକ success = ସତ୍ୟ;
		success &= SetCalibrationParam(SensorString(), "ID", _calibration_index, _device_id);
		success &= SetCalibrationParam(SensorString(), "OFF", _calibration_index, _offset);

		UpdateLegacyOffset();

		ଫେରନ୍ତୁ success;
	}

	// index 3 is possible: FindAvailableCalibrationIndex() searches four slots, we define three
	PX4_ERR("%s %" PRIu32 " no free calibration slot (got %" PRIi8 ")", SensorString(), _device_id, _calibration_index);

	ଫେରନ୍ତୁ ମିଥ୍ୟା;
}

ଶୂନ୍ୟ DifferentialPressure::PrintStatus()
{
	PX4_INFO_RAW("%s %" PRIu32 " offset: %05.3f Pa, %s\n",
		     SensorString(), device_id(), (ଦ୍ୱିଗୁଣ)_offset,
		     external() ? "Ext" : "Internal");
}

} // namespace calibration
