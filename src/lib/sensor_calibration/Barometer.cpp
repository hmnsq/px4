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

#અન્તર્ભાવ "Barometer.hpp"

#અન્તર્ભાવ "Utilities.hpp"

#અન્તર્ભાવ <lib/drivers/device/Device.hpp>
#અન્તર્ભાવ <lib/parameters/param.h>

using નામાકાશ matrix;
using નામાકાશ time_literals;

નામાકાશ calibration
{

Barometer::Barometer()
{
	Reset();
}

Barometer::Barometer(uint32_t device_id)
{
	set_device_id(device_id);
}

Barometer::Barometer(uint32_t device_id, તાર્કિક external)
{
	set_device_id(device_id, external);
}

શૂન્ય Barometer::set_device_id(uint32_t device_id)
{
	set_device_id(device_id, device::device_is_external(device_id));
}

શૂન્ય Barometer::set_device_id(uint32_t device_id, તાર્કિક external)
{
	જો (_device_id != device_id || _external != external) {

		_device_id = device_id;
		_external = external;

		Reset();

		ParametersUpdate();
		SensorCorrectionsUpdate(સાચું);
	}
}

શૂન્ય Barometer::SensorCorrectionsUpdate(તાર્કિક force)
{
	// check if the selected sensor has updated
	જો (_sensor_correction_sub.updated() || force) {

		// valid device id required
		જો (_device_id == 0) {
			પાછા;
		}

		sensor_correction_s corrections;

		જો (_sensor_correction_sub.copy(&corrections)) {
			// find sensor_corrections index
			માટે (પૂર્ણાંક i = 0; i < MAX_SENSOR_ગણતરી; i++) {
				જો (corrections.baro_device_ids[i] == _device_id) {
					પસંદગી (i) {
					સ્થિતિ 0:
						_thermal_offset = corrections.baro_offset_0;
						પાછા;

					સ્થિતિ 1:
						_thermal_offset = corrections.baro_offset_1;
						પાછા;

					સ્થિતિ 2:
						_thermal_offset = corrections.baro_offset_2;
						પાછા;

					સ્થિતિ 3:
						_thermal_offset = corrections.baro_offset_3;
						પાછા;
					}
				}
			}
		}

		// zero thermal offset if not found
		_thermal_offset = 0;
	}
}

તાર્કિક Barometer::set_offset(અચલ દશાંશ &offset)
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

તાર્કિક Barometer::set_calibration_index(પૂર્ણાંક calibration_index)
{
	જો ((calibration_index >= 0) && (calibration_index < MAX_SENSOR_ગણતરી)) {
		_calibration_index = calibration_index;
		પાછા સાચું;
	}

	પાછા ખોટું;
}

શૂન્ય Barometer::ParametersUpdate()
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

તાર્કિક Barometer::ParametersLoad()
{
	જો (_calibration_index >= 0 && _calibration_index < MAX_SENSOR_ગણતરી) {
		// CAL_BAROx_PRIO
		_priority = GetCalibrationParamInt32(SensorString(), "PRIO", _calibration_index);

		જો ((_priority < 0) || (_priority > 100)) {
			// reset to default, -1 is the uninitialized parameter value
			સ્થિર constexpr int32_t CAL_PRIO_UNINITIALIZED = -1;

			જો (_priority != CAL_PRIO_UNINITIALIZED) {
				PX4_ERR("%s %" PRIu32 " (%" PRId8 ") invalid priority %" PRId32 ", resetting", SensorString(), _device_id,
					_calibration_index, _priority);

				SetCalibrationParam(SensorString(), "PRIO", _calibration_index, CAL_PRIO_UNINITIALIZED);
			}

			_priority = _external ? DEFAULT_EXTERNAL_PRIORITY : DEFAULT_PRIORITY;
		}

		// CAL_BAROx_OFF
		set_offset(GetCalibrationParamFloat(SensorString(), "OFF", _calibration_index));

		પાછા સાચું;
	}

	પાછા ખોટું;
}

શૂન્ય Barometer::Reset()
{
	_offset = 0;

	_thermal_offset = 0;

	_priority = _external ? DEFAULT_EXTERNAL_PRIORITY : DEFAULT_PRIORITY;

	_calibration_index = -1;

	calibration_ગણતરી = 0;
}

તાર્કિક Barometer::ParametersSave(પૂર્ણાંક desired_calibration_index, તાર્કિક force)
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
		success &= SetCalibrationParam(SensorString(), "PRIO", _calibration_index, _priority);
		success &= SetCalibrationParam(SensorString(), "OFF", _calibration_index, _offset);

		પાછા success;
	}

	પાછા ખોટું;
}

શૂન્ય Barometer::PrintStatus()
{
	જો (external()) {
		PX4_INFO_RAW("%s %" PRIu32
			     " EN: %d, offset: %05.3f, Ext\n",
			     SensorString(), device_id(), enabled(),
			     (દ્વિગુણ)_offset);

	} નહીંતર {
		PX4_INFO_RAW("%s %" PRIu32 " EN: %d, offset: %05.3f, Internal\n",
			     SensorString(), device_id(), enabled(),
			     (દ્વિગુણ)_offset);
	}

	જો (fabsf(_thermal_offset) > 0.f) {
		PX4_INFO_RAW("%s %" PRIu32 " temperature offset: %.4f\n", SensorString(), _device_id, (દ્વિગુણ)_thermal_offset);
	}
}

} // namespace calibration
