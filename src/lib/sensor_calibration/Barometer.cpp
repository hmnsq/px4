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

#include "Barometer.hpp"

#include "Utilities.hpp"

#include <lib/drivers/device/Device.hpp>
#include <lib/parameters/param.h>

using namespace matrix;
using namespace time_literals;

namespace calibration
{

Barometer::Barometer()
{
	Reset();
}

Barometer::Barometer(uint32_t device_id)
{
	set_device_id(device_id);
}

Barometer::Barometer(uint32_t device_id, bool external)
{
	set_device_id(device_id, external);
}

void Barometer::set_device_id(uint32_t device_id)
{
	set_device_id(device_id, device::device_is_external(device_id));
}

void Barometer::set_device_id(uint32_t device_id, bool external)
{
	만약 (_device_id != device_id || _external != external) {

		_device_id = device_id;
		_external = external;

		Reset();

		ParametersUpdate();
		SensorCorrectionsUpdate(true);
	}
}

void Barometer::SensorCorrectionsUpdate(bool force)
{
	// check if the selected sensor has updated
	만약 (_sensor_correction_sub.updated() || force) {

		// valid device id required
		만약 (_device_id == 0) {
			반환;
		}

		sensor_correction_s corrections;

		만약 (_sensor_correction_sub.copy(&corrections)) {
			// find sensor_corrections index
			위해 (정수 i = 0; i < MAX_SENSOR_COUNT; i++) {
				만약 (corrections.baro_device_ids[i] == _device_id) {
					switch (i) {
					case 0:
						_thermal_offset = corrections.baro_offset_0;
						반환;

					case 1:
						_thermal_offset = corrections.baro_offset_1;
						반환;

					case 2:
						_thermal_offset = corrections.baro_offset_2;
						반환;

					case 3:
						_thermal_offset = corrections.baro_offset_3;
						반환;
					}
				}
			}
		}

		// zero thermal offset if not found
		_thermal_offset = 0;
	}
}

bool Barometer::set_offset(const float &offset)
{
	만약 (fabsf(_offset - offset) > 0.01f) {
		만약 (PX4_ISFINITE(offset)) {
			_offset = offset;
			_calibration_count++;
			반환 true;
		}
	}

	반환 false;
}

bool Barometer::set_calibration_index(정수 calibration_index)
{
	만약 ((calibration_index >= 0) && (calibration_index < MAX_SENSOR_COUNT)) {
		_calibration_index = calibration_index;
		반환 true;
	}

	반환 false;
}

void Barometer::ParametersUpdate()
{
	만약 (_device_id == 0) {
		반환;
	}

	_calibration_index = FindCurrentCalibrationIndex(SensorString(), _device_id);

	만약 (_calibration_index == -1) {
		// no saved calibration available
		Reset();

	} 아니면 {
		ParametersLoad();
	}
}

bool Barometer::ParametersLoad()
{
	만약 (_calibration_index >= 0 && _calibration_index < MAX_SENSOR_COUNT) {
		// CAL_BAROx_PRIO
		_priority = GetCalibrationParamInt32(SensorString(), "PRIO", _calibration_index);

		만약 ((_priority < 0) || (_priority > 100)) {
			// reset to default, -1 is the uninitialized parameter value
			static constexpr int32_t CAL_PRIO_UNINITIALIZED = -1;

			만약 (_priority != CAL_PRIO_UNINITIALIZED) {
				PX4_ERR("%s %" PRIu32 " (%" PRId8 ") invalid priority %" PRId32 ", resetting", SensorString(), _device_id,
					_calibration_index, _priority);

				SetCalibrationParam(SensorString(), "PRIO", _calibration_index, CAL_PRIO_UNINITIALIZED);
			}

			_priority = _external ? DEFAULT_EXTERNAL_PRIORITY : DEFAULT_PRIORITY;
		}

		// CAL_BAROx_OFF
		set_offset(GetCalibrationParamFloat(SensorString(), "OFF", _calibration_index));

		반환 true;
	}

	반환 false;
}

void Barometer::Reset()
{
	_offset = 0;

	_thermal_offset = 0;

	_priority = _external ? DEFAULT_EXTERNAL_PRIORITY : DEFAULT_PRIORITY;

	_calibration_index = -1;

	_calibration_count = 0;
}

bool Barometer::ParametersSave(정수 desired_calibration_index, bool force)
{
	만약 (force && desired_calibration_index >= 0 && desired_calibration_index < MAX_SENSOR_COUNT) {
		_calibration_index = desired_calibration_index;

	} 아니면 만약 (!force || (_calibration_index < 0)
		   || (desired_calibration_index != -1 && desired_calibration_index != _calibration_index)) {

		// ensure we have a valid calibration slot (matching existing or first available slot)
		int8_t calibration_index_prev = _calibration_index;
		_calibration_index = FindAvailableCalibrationIndex(SensorString(), _device_id, desired_calibration_index);

		만약 (calibration_index_prev >= 0 && (calibration_index_prev != _calibration_index)) {
			PX4_WARN("%s %" PRIu32 " calibration index changed %" PRIi8 " -> %" PRIi8, SensorString(), _device_id,
				 calibration_index_prev, _calibration_index);
		}
	}

	만약 (_calibration_index >= 0 && _calibration_index < MAX_SENSOR_COUNT) {
		// save calibration
		bool success = true;
		success &= SetCalibrationParam(SensorString(), "ID", _calibration_index, _device_id);
		success &= SetCalibrationParam(SensorString(), "PRIO", _calibration_index, _priority);
		success &= SetCalibrationParam(SensorString(), "OFF", _calibration_index, _offset);

		반환 success;
	}

	반환 false;
}

void Barometer::PrintStatus()
{
	만약 (external()) {
		PX4_INFO_RAW("%s %" PRIu32
			     " EN: %d, offset: %05.3f, Ext\n",
			     SensorString(), device_id(), enabled(),
			     (double)_offset);

	} 아니면 {
		PX4_INFO_RAW("%s %" PRIu32 " EN: %d, offset: %05.3f, Internal\n",
			     SensorString(), device_id(), enabled(),
			     (double)_offset);
	}

	만약 (fabsf(_thermal_offset) > 0.f) {
		PX4_INFO_RAW("%s %" PRIu32 " temperature offset: %.4f\n", SensorString(), _device_id, (double)_thermal_offset);
	}
}

} // namespace calibration
