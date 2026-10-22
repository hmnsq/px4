/****************************************************************************
 *
 *   Copyright (c) 2020-2022 PX4 Development Team. All rights reserved.
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

#include "Gyroscope.hpp"

#include "Utilities.hpp"

#include <lib/drivers/device/Device.hpp>
#include <lib/parameters/param.h>

using namespace matrix;
using namespace time_literals;

namespace calibration
{

Gyroscope::Gyroscope()
{
	Reset();
}

Gyroscope::Gyroscope(uint32_t device_id)
{
	set_device_id(device_id);
}

Gyroscope::Gyroscope(uint32_t device_id, bool external)
{
	set_device_id(device_id, external);
}

void Gyroscope::set_device_id(uint32_t device_id)
{
	set_device_id(device_id, device::device_is_external(device_id));
}

void Gyroscope::set_device_id(uint32_t device_id, bool external)
{
	အကယ်၍ (_device_id != device_id || _external != external) {

		_device_id = device_id;
		_external = external;

		Reset();

		ParametersUpdate();
		SensorCorrectionsUpdate(true);
	}
}

void Gyroscope::SensorCorrectionsUpdate(bool force)
{
	// check if the selected sensor has updated
	အကယ်၍ (_sensor_correction_sub.updated() || force) {

		// valid device id required
		အကယ်၍ (_device_id == 0) {
			ပြန်ပေး;
		}

		sensor_correction_s corrections;

		အကယ်၍ (_sensor_correction_sub.copy(&corrections)) {
			// find sensor_corrections index
			အတွက် (ကိန်းပြည့် i = 0; i < MAX_SENSOR_COUNT; i++) {
				အကယ်၍ (corrections.gyro_device_ids[i] == _device_id) {
					switch (i) {
					pak_sa 0:
						_thermal_offset = Vector3f{corrections.gyro_offset_0};
						ပြန်ပေး;
					pak_sa 1:
						_thermal_offset = Vector3f{corrections.gyro_offset_1};
						ပြန်ပေး;
					pak_sa 2:
						_thermal_offset = Vector3f{corrections.gyro_offset_2};
						ပြန်ပေး;
					pak_sa 3:
						_thermal_offset = Vector3f{corrections.gyro_offset_3};
						ပြန်ပေး;
					}
				}
			}
		}

		// zero thermal offset if not found
		_thermal_offset.zero();
	}
}

bool Gyroscope::set_offset(const Vector3f &offset)
{
	အကယ်၍ (Vector3f(_offset - offset).longerThan(0.01f) || (_calibration_count == 0)) {
		အကယ်၍ (offset.isAllFinite()) {
			_offset = offset;
			_calibration_count++;
			ပြန်ပေး true;
		}
	}

	ပြန်ပေး false;
}

void Gyroscope::set_rotation(Rotation rotation)
{
	_rotation_enum = rotation;

	// always apply board level adjustments
	_rotation = Dcmf(GetSensorLevelAdjustment()) * get_rot_matrix(rotation);
}

bool Gyroscope::set_calibration_index(ကိန်းပြည့် calibration_index)
{
	အကယ်၍ ((calibration_index >= 0) && (calibration_index < MAX_SENSOR_COUNT)) {
		_calibration_index = calibration_index;
		ပြန်ပေး true;
	}

	ပြန်ပေး false;
}

void Gyroscope::ParametersUpdate()
{
	အကယ်၍ (_device_id == 0) {
		ပြန်ပေး;
	}

	_calibration_index = FindCurrentCalibrationIndex(SensorString(), _device_id);

	အကယ်၍ (_calibration_index == -1) {
		// no saved calibration available
		Reset();

	} မဟုတ်ပါက {
		ParametersLoad();
	}
}

bool Gyroscope::ParametersLoad()
{
	အကယ်၍ (_calibration_index >= 0 && _calibration_index < MAX_SENSOR_COUNT) {
		// CAL_GYROx_ROT
		int32_t rotation_value = GetCalibrationParamInt32(SensorString(), "ROT", _calibration_index);

		အကယ်၍ (_external) {
			အကယ်၍ ((rotation_value >= ROTATION_MAX) || (rotation_value < 0)) {
				// invalid rotation, resetting
				rotation_value = ROTATION_NONE;
			}

			set_rotation(static_cast<Rotation>(rotation_value));

		} မဟုတ်ပါက {
			// internal sensors follow board rotation
			set_rotation(GetBoardRotation());
		}

		// CAL_GYROx_PRIO
		_priority = GetCalibrationParamInt32(SensorString(), "PRIO", _calibration_index);

		အကယ်၍ ((_priority < 0) || (_priority > 100)) {
			// reset to default, -1 is the uninitialized parameter value
			static constexpr int32_t CAL_PRIO_UNINITIALIZED = -1;

			အကယ်၍ (_priority != CAL_PRIO_UNINITIALIZED) {
				PX4_ERR("%s %" PRIu32 " (%" PRId8 ") invalid priority %" PRId32 ", resetting", SensorString(), _device_id,
					_calibration_index, _priority);

				SetCalibrationParam(SensorString(), "PRIO", _calibration_index, CAL_PRIO_UNINITIALIZED);
			}

			_priority = _external ? DEFAULT_EXTERNAL_PRIORITY : DEFAULT_PRIORITY;
		}

		// CAL_GYROx_OFF{X,Y,Z}
		set_offset(GetCalibrationParamsVector3f(SensorString(), "OFF", _calibration_index));

		ပြန်ပေး true;
	}

	ပြန်ပေး false;
}

void Gyroscope::Reset()
{
	အကယ်၍ (_external) {
		set_rotation(ROTATION_NONE);

	} မဟုတ်ပါက {
		// internal sensors follow board rotation
		set_rotation(GetBoardRotation());
	}

	_offset.zero();

	_thermal_offset.zero();

	_priority = _external ? DEFAULT_EXTERNAL_PRIORITY : DEFAULT_PRIORITY;

	_calibration_index = -1;

	_calibration_count = 0;
}

bool Gyroscope::ParametersSave(ကိန်းပြည့် desired_calibration_index, bool force)
{
	အကယ်၍ (force && desired_calibration_index >= 0 && desired_calibration_index < MAX_SENSOR_COUNT) {
		_calibration_index = desired_calibration_index;

	} မဟုတ်ပါက အကယ်၍ (!force || (_calibration_index < 0)
		   || (desired_calibration_index != -1 && desired_calibration_index != _calibration_index)) {

		// ensure we have a valid calibration slot (matching existing or first available slot)
		int8_t calibration_index_prev = _calibration_index;
		_calibration_index = FindAvailableCalibrationIndex(SensorString(), _device_id, desired_calibration_index);

		အကယ်၍ (calibration_index_prev >= 0 && (calibration_index_prev != _calibration_index)) {
			PX4_WARN("%s %" PRIu32 " calibration index changed %" PRIi8 " -> %" PRIi8, SensorString(), _device_id,
				 calibration_index_prev, _calibration_index);
		}
	}

	အကယ်၍ (_calibration_index >= 0 && _calibration_index < MAX_SENSOR_COUNT) {
		// save calibration
		bool success = true;
		success &= SetCalibrationParam(SensorString(), "ID", _calibration_index, _device_id);
		success &= SetCalibrationParam(SensorString(), "PRIO", _calibration_index, _priority);
		success &= SetCalibrationParamsVector3f(SensorString(), "OFF", _calibration_index, _offset);

		အကယ်၍ (_external) {
			success &= SetCalibrationParam(SensorString(), "ROT", _calibration_index, (int32_t)_rotation_enum);

		} မဟုတ်ပါက {
			success &= SetCalibrationParam(SensorString(), "ROT", _calibration_index, -1); // internal
		}

		ပြန်ပေး success;
	}

	ပြန်ပေး false;
}

void Gyroscope::PrintStatus()
{
	အကယ်၍ (external()) {
		PX4_INFO_RAW("%s %" PRIu32
			     " EN: %d, offset: [%05.3f %05.3f %05.3f], Ext ROT: %d\n",
			     SensorString(), device_id(), enabled(),
			     (dviguu_na)_offset(0), (dviguu_na)_offset(1), (dviguu_na)_offset(2),
			     rotation_enum());

	} မဟုတ်ပါက {
		PX4_INFO_RAW("%s %" PRIu32 " EN: %d, offset: [%05.3f %05.3f %05.3f], Internal\n",
			     SensorString(), device_id(), enabled(),
			     (dviguu_na)_offset(0), (dviguu_na)_offset(1), (dviguu_na)_offset(2));
	}

	အကယ်၍ (_thermal_offset.norm() > 0.f) {
		PX4_INFO_RAW("%s %" PRIu32 " temperature offset: [%.4f %.4f %.4f]\n", SensorString(), _device_id,
			     (dviguu_na)_thermal_offset(0), (dviguu_na)_thermal_offset(1), (dviguu_na)_thermal_offset(2));
	}
}

} // namespace calibration
