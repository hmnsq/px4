/****************************************************************************
 *
 *   Copyright (c) 2022 PX4 Development Team. All rights reserved.
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

#അന്തര്ഭാവ "Barometer.hpp"

#അന്തര്ഭാവ "Utilities.hpp"

#അന്തര്ഭാവ <lib/drivers/device/Device.hpp>
#അന്തര്ഭാവ <lib/parameters/param.h>

using നാമാകാശ matrix;
using നാമാകാശ time_literals;

നാമാകാശ calibration
{

Barometer::Barometer()
{
	Reset();
}

Barometer::Barometer(uint32_t device_id)
{
	set_device_id(device_id);
}

Barometer::Barometer(uint32_t device_id, തർക്കം external)
{
	set_device_id(device_id, external);
}

ശൂന്യം Barometer::set_device_id(uint32_t device_id)
{
	set_device_id(device_id, device::device_is_external(device_id));
}

ശൂന്യം Barometer::set_device_id(uint32_t device_id, തർക്കം external)
{
	എങ്കിൽ (_device_id != device_id || _external != external) {

		_device_id = device_id;
		_external = external;

		Reset();

		ParametersUpdate();
		SensorCorrectionsUpdate(സത്യം);
	}
}

ശൂന്യം Barometer::SensorCorrectionsUpdate(തർക്കം force)
{
	// check if the selected sensor has updated
	എങ്കിൽ (_sensor_correction_sub.updated() || force) {

		// valid device id required
		എങ്കിൽ (_device_id == 0) {
			തിരികെ;
		}

		sensor_correction_s corrections;

		എങ്കിൽ (_sensor_correction_sub.copy(&corrections)) {
			// find sensor_corrections index
			വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < MAX_SENSOR_എണ്ണം; i++) {
				എങ്കിൽ (corrections.baro_device_ids[i] == _device_id) {
					തിരഞ്ഞെടുപ്പ് (i) {
					അവസ്ഥ 0:
						_thermal_offset = corrections.baro_offset_0;
						തിരികെ;

					അവസ്ഥ 1:
						_thermal_offset = corrections.baro_offset_1;
						തിരികെ;

					അവസ്ഥ 2:
						_thermal_offset = corrections.baro_offset_2;
						തിരികെ;

					അവസ്ഥ 3:
						_thermal_offset = corrections.baro_offset_3;
						തിരികെ;
					}
				}
			}
		}

		// zero thermal offset if not found
		_thermal_offset = 0;
	}
}

തർക്കം Barometer::set_offset(സ്ഥിരം ദശാംശം &offset)
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

തർക്കം Barometer::set_calibration_index(പൂർണ്ണസംഖ്യ calibration_index)
{
	എങ്കിൽ ((calibration_index >= 0) && (calibration_index < MAX_SENSOR_എണ്ണം)) {
		_calibration_index = calibration_index;
		തിരികെ സത്യം;
	}

	തിരികെ അസത്യം;
}

ശൂന്യം Barometer::ParametersUpdate()
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

തർക്കം Barometer::ParametersLoad()
{
	എങ്കിൽ (_calibration_index >= 0 && _calibration_index < MAX_SENSOR_എണ്ണം) {
		// CAL_BAROx_PRIO
		_priority = GetCalibrationParamInt32(SensorString(), "PRIO", _calibration_index);

		എങ്കിൽ ((_priority < 0) || (_priority > 100)) {
			// reset to default, -1 is the uninitialized parameter value
			സ്ഥിതം constexpr int32_t CAL_PRIO_UNINITIALIZED = -1;

			എങ്കിൽ (_priority != CAL_PRIO_UNINITIALIZED) {
				PX4_ERR("%s %" PRIu32 " (%" PRId8 ") invalid priority %" PRId32 ", resetting", SensorString(), _device_id,
					_calibration_index, _priority);

				SetCalibrationParam(SensorString(), "PRIO", _calibration_index, CAL_PRIO_UNINITIALIZED);
			}

			_priority = _external ? DEFAULT_EXTERNAL_PRIORITY : DEFAULT_PRIORITY;
		}

		// CAL_BAROx_OFF
		set_offset(GetCalibrationParamFloat(SensorString(), "OFF", _calibration_index));

		തിരികെ സത്യം;
	}

	തിരികെ അസത്യം;
}

ശൂന്യം Barometer::Reset()
{
	_offset = 0;

	_thermal_offset = 0;

	_priority = _external ? DEFAULT_EXTERNAL_PRIORITY : DEFAULT_PRIORITY;

	_calibration_index = -1;

	calibration_എണ്ണം = 0;
}

തർക്കം Barometer::ParametersSave(പൂർണ്ണസംഖ്യ desired_calibration_index, തർക്കം force)
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
		success &= SetCalibrationParam(SensorString(), "PRIO", _calibration_index, _priority);
		success &= SetCalibrationParam(SensorString(), "OFF", _calibration_index, _offset);

		തിരികെ success;
	}

	തിരികെ അസത്യം;
}

ശൂന്യം Barometer::PrintStatus()
{
	എങ്കിൽ (external()) {
		PX4_INFO_RAW("%s %" PRIu32
			     " EN: %d, offset: %05.3f, Ext\n",
			     SensorString(), device_id(), enabled(),
			     (ഇരട്ട)_offset);

	} അല്ലെങ്കിൽ {
		PX4_INFO_RAW("%s %" PRIu32 " EN: %d, offset: %05.3f, Internal\n",
			     SensorString(), device_id(), enabled(),
			     (ഇരട്ട)_offset);
	}

	എങ്കിൽ (fabsf(_thermal_offset) > 0.f) {
		PX4_INFO_RAW("%s %" PRIu32 " temperature offset: %.4f\n", SensorString(), _device_id, (ഇരട്ട)_thermal_offset);
	}
}

} // namespace calibration
