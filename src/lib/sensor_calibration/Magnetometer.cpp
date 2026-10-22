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

#include "Magnetometer.hpp"

#include "Utilities.hpp"

#include <lib/drivers/device/Device.hpp>
#include <lib/parameters/param.h>

using namespace matrix;
using namespace time_literals;

namespace calibration
{

Magnetometer::Magnetometer()
{
	Reset();
}

Magnetometer::Magnetometer(uint32_t device_id)
{
	set_device_id(device_id);
}

Magnetometer::Magnetometer(uint32_t device_id, bool external)
{
	set_device_id(device_id, external);
}

void Magnetometer::set_device_id(uint32_t device_id)
{
	set_device_id(device_id, device::device_is_external(device_id));
}

void Magnetometer::set_device_id(uint32_t device_id, bool external)
{
	ပေႃး (_device_id != device_id || _external != external) {

		_device_id = device_id;
		_external = external;

		Reset();

		ParametersUpdate();
		SensorCorrectionsUpdate(true);
	}
}

void Magnetometer::SensorCorrectionsUpdate(bool force)
{
	// check if the selected sensor has updated
	ပေႃး (_sensor_correction_sub.updated() || force) {

		// valid device id required
		ပေႃး (_device_id == 0) {
			ပွၵ်ႈ;
		}

		sensor_correction_s corrections;

		ပေႃး (_sensor_correction_sub.copy(&corrections)) {
			// find sensor_corrections index
			for (puur_naa_nka i = 0; i < MAX_SENSOR_COUNT; i++) {
				ပေႃး (corrections.mag_device_ids[i] == _device_id) {
					switch (i) {
					pak_sa 0:
						_thermal_offset = Vector3f{corrections.mag_offset_0};
						ပွၵ်ႈ;
					pak_sa 1:
						_thermal_offset = Vector3f{corrections.mag_offset_1};
						ပွၵ်ႈ;
					pak_sa 2:
						_thermal_offset = Vector3f{corrections.mag_offset_2};
						ပွၵ်ႈ;
					pak_sa 3:
						_thermal_offset = Vector3f{corrections.mag_offset_3};
						ပွၵ်ႈ;
					}
				}
			}
		}

		// zero thermal offset if not found
		_thermal_offset.zero();
	}
}

bool Magnetometer::set_offset(const Vector3f &offset)
{
	ပေႃး (Vector3f(_offset - offset).longerThan(0.005f)) {
		ပေႃး (offset.isAllFinite()) {
			_offset = offset;
			_calibration_count++;
			ပွၵ်ႈ true;
		}
	}

	ပွၵ်ႈ false;
}

bool Magnetometer::set_scale(const Vector3f &scale)
{
	ပေႃး (Vector3f(_scale.diag() - scale).longerThan(0.01f)) {
		ပေႃး (scale.isAllFinite() && (scale(0) > 0.f) && (scale(1) > 0.f) && (scale(2) > 0.f)) {
			_scale(0, 0) = scale(0);
			_scale(1, 1) = scale(1);
			_scale(2, 2) = scale(2);

			_calibration_count++;
			ပွၵ်ႈ true;
		}
	}

	ပွၵ်ႈ false;
}

bool Magnetometer::set_offdiagonal(const Vector3f &offdiagonal)
{
	ပေႃး (Vector3f(Vector3f{_scale(0, 1), _scale(0, 2), _scale(1, 2)} - offdiagonal).longerThan(0.01f)) {
		ပေႃး (offdiagonal.isAllFinite()) {

			_scale(0, 1) = offdiagonal(0);
			_scale(1, 0) = offdiagonal(0);

			_scale(0, 2) = offdiagonal(1);
			_scale(2, 0) = offdiagonal(1);

			_scale(1, 2) = offdiagonal(2);
			_scale(2, 1) = offdiagonal(2);

			_calibration_count++;
			ပွၵ်ႈ true;
		}
	}

	ပွၵ်ႈ false;
}

void Magnetometer::set_rotation(const Rotation rotation)
{
	ပေႃး (rotation < ROTATION_MAX) {
		_rotation_enum = rotation;

	} ပေႃးဢမ်ႇ {
		// invalid rotation, resetting
		_rotation_enum = ROTATION_NONE;
	}

	// always apply level adjustments
	_rotation = Dcmf(GetSensorLevelAdjustment()) * get_rot_matrix(_rotation_enum);

	// clear any custom rotation
	_rotation_custom_euler.zero();
}

void Magnetometer::set_custom_rotation(const Eulerf &rotation)
{
	_rotation_enum = ROTATION_CUSTOM;

	// store custom rotation
	_rotation_custom_euler = rotation;

	// always apply board level adjustments
	_rotation = Dcmf(GetSensorLevelAdjustment()) * Dcmf(_rotation_custom_euler);

	// TODO: Note that ideally this shouldn't be necessary for an external sensors, as the definition of *rotation
	// between sensor frame & vehicle's body frame isn't affected by the rotation of the Autopilot.
	// however, since while doing the 'level-calibration', users don't put the vehicle truly *horizontal, the
	// measured board roll/pitch offset isn't true. So this affects external sensors as well (which is why we apply
	// internal SensorLevelAdjustment to all the sensors). We need to figure out how to set the sensor board offset
	// values properly (i.e. finding Vehicle's true Forward-Right-Down frame in a user's perspective)
}

bool Magnetometer::set_calibration_index(puur_naa_nka calibration_index)
{
	ပေႃး ((calibration_index >= 0) && (calibration_index < MAX_SENSOR_COUNT)) {
		_calibration_index = calibration_index;
		ပွၵ်ႈ true;
	}

	ပွၵ်ႈ false;
}

void Magnetometer::ParametersUpdate()
{
	ပေႃး (_device_id == 0) {
		ပွၵ်ႈ;
	}

	_calibration_index = FindCurrentCalibrationIndex(SensorString(), _device_id);

	ပေႃး (_calibration_index == -1) {
		// no saved calibration available
		Reset();

	} ပေႃးဢမ်ႇ {
		ParametersLoad();
	}
}

bool Magnetometer::ParametersLoad()
{
	ပေႃး (_calibration_index >= 0 && _calibration_index < MAX_SENSOR_COUNT) {
		// CAL_MAGx_ROT
		int32_t rotation_value = GetCalibrationParamInt32(SensorString(), "ROT", _calibration_index);

		const float euler_roll_deg = GetCalibrationParamFloat(SensorString(), "ROLL", _calibration_index);
		const float euler_pitch_deg = GetCalibrationParamFloat(SensorString(), "PITCH", _calibration_index);
		const float euler_yaw_deg = GetCalibrationParamFloat(SensorString(), "YAW", _calibration_index);

		ပေႃး (_external) {
			ပေႃး (((rotation_value >= ROTATION_MAX) && (rotation_value != ROTATION_CUSTOM)) || (rotation_value < 0)) {
				// invalid rotation, resetting
				rotation_value = ROTATION_NONE;
			}

			// if CAL_MAGx_{ROLL,PITCH,YAW} manually set then CAL_MAGx_ROT needs to be ROTATION_CUSTOM
			ပေႃး ((rotation_value != ROTATION_CUSTOM)
			    && ((fabsf(euler_roll_deg) > FLT_EPSILON)
				|| (fabsf(euler_pitch_deg) > FLT_EPSILON)
				|| (fabsf(euler_yaw_deg) > FLT_EPSILON))) {

				rotation_value = ROTATION_CUSTOM;
				SetCalibrationParam(SensorString(), "ROT", _calibration_index, rotation_value);
			}

			// Handle custom specified euler angle
			ပေႃး (rotation_value == ROTATION_CUSTOM) {

				const matrix::Eulerf rotation_custom_euler{
					math::radians(euler_roll_deg),
					math::radians(euler_pitch_deg),
					math::radians(euler_yaw_deg)};

				set_custom_rotation(rotation_custom_euler);

			} ပေႃးဢမ်ႇ {
				set_rotation(static_cast<Rotation>(rotation_value));
			}

		} ပေႃးဢမ်ႇ {
			// internal sensors follow board rotation
			set_rotation(GetBoardRotation());
		}

		// CAL_MAGx_PRIO
		_priority = GetCalibrationParamInt32(SensorString(), "PRIO", _calibration_index);

		ပေႃး ((_priority < 0) || (_priority > 100)) {
			// reset to default, -1 is the uninitialized parameter value
			static constexpr int32_t CAL_PRIO_UNINITIALIZED = -1;

			ပေႃး (_priority != CAL_PRIO_UNINITIALIZED) {
				PX4_ERR("%s %" PRIu32 " (%" PRId8 ") invalid priority %" PRId32 ", resetting", SensorString(), _device_id,
					_calibration_index, _priority);

				SetCalibrationParam(SensorString(), "PRIO", _calibration_index, CAL_PRIO_UNINITIALIZED);
			}

			_priority = _external ? DEFAULT_EXTERNAL_PRIORITY : DEFAULT_PRIORITY;
		}

		// CAL_MAGx_OFF{X,Y,Z}
		set_offset(GetCalibrationParamsVector3f(SensorString(), "OFF", _calibration_index));

		// CAL_MAGx_SCALE{X,Y,Z}
		set_scale(GetCalibrationParamsVector3f(SensorString(), "SCALE", _calibration_index));

		// CAL_MAGx_ODIAG{X,Y,Z}
		set_offdiagonal(GetCalibrationParamsVector3f(SensorString(), "ODIAG", _calibration_index));

		// CAL_MAGx_COMP{X,Y,Z}
		_power_compensation = GetCalibrationParamsVector3f(SensorString(), "COMP", _calibration_index);

		ပွၵ်ႈ true;
	}

	ပွၵ်ႈ false;
}

void Magnetometer::Reset()
{
	ပေႃး (_external) {
		set_rotation(ROTATION_NONE);

	} ပေႃးဢမ်ႇ {
		// internal sensors follow board rotation
		set_rotation(GetBoardRotation());
	}

	_offset.zero();
	_scale.setIdentity();

	_power_compensation.zero();
	_power = 0.f;

	_priority = _external ? DEFAULT_EXTERNAL_PRIORITY : DEFAULT_PRIORITY;

	_calibration_index = -1;

	_calibration_count = 0;
}

bool Magnetometer::ParametersSave(puur_naa_nka desired_calibration_index, bool force)
{
	ပေႃး (force && desired_calibration_index >= 0 && desired_calibration_index < MAX_SENSOR_COUNT) {
		_calibration_index = desired_calibration_index;

	} ပေႃးဢမ်ႇ ပေႃး (!force || (_calibration_index < 0)
		   || (desired_calibration_index != -1 && desired_calibration_index != _calibration_index)) {

		// ensure we have a valid calibration slot (matching existing or first available slot)
		int8_t calibration_index_prev = _calibration_index;
		_calibration_index = FindAvailableCalibrationIndex(SensorString(), _device_id, desired_calibration_index);

		ပေႃး (calibration_index_prev >= 0 && (calibration_index_prev != _calibration_index)) {
			PX4_WARN("%s %" PRIu32 " calibration index changed %" PRIi8 " -> %" PRIi8, SensorString(), _device_id,
				 calibration_index_prev, _calibration_index);
		}
	}

	ပေႃး (_calibration_index >= 0 && _calibration_index < MAX_SENSOR_COUNT) {
		// save calibration
		bool success = true;
		success &= SetCalibrationParam(SensorString(), "ID", _calibration_index, _device_id);
		success &= SetCalibrationParam(SensorString(), "PRIO", _calibration_index, _priority);
		success &= SetCalibrationParamsVector3f(SensorString(), "OFF", _calibration_index, _offset);

		const Vector3f scale{_scale.diag()};
		success &= SetCalibrationParamsVector3f(SensorString(), "SCALE", _calibration_index, scale);

		const Vector3f off_diag{_scale(0, 1), _scale(0, 2), _scale(1, 2)};
		success &= SetCalibrationParamsVector3f(SensorString(), "ODIAG", _calibration_index, off_diag);

		success &= SetCalibrationParamsVector3f(SensorString(), "COMP", _calibration_index, _power_compensation);

		ပေႃး (_external) {
			success &= SetCalibrationParam(SensorString(), "ROT", _calibration_index, (int32_t)_rotation_enum);

		} ပေႃးဢမ်ႇ {
			success &= SetCalibrationParam(SensorString(), "ROT", _calibration_index, -1); // internal
		}

		success &= SetCalibrationParam(SensorString(), "ROLL", _calibration_index, math::degrees(_rotation_custom_euler(0)));
		success &= SetCalibrationParam(SensorString(), "PITCH", _calibration_index, math::degrees(_rotation_custom_euler(1)));
		success &= SetCalibrationParam(SensorString(), "YAW", _calibration_index, math::degrees(_rotation_custom_euler(2)));

		ပွၵ်ႈ success;
	}

	ပွၵ်ႈ false;
}

void Magnetometer::PrintStatus()
{
	ပေႃး (external()) {
		PX4_INFO_RAW("%s %" PRIu32
			     " EN: %d, offset: [%05.3f %05.3f %05.3f], scale: [%05.3f %05.3f %05.3f], Ext ROT: %d\n",
			     SensorString(), device_id(), enabled(),
			     (dviguu_na)_offset(0), (dviguu_na)_offset(1), (dviguu_na)_offset(2),
			     (dviguu_na)_scale(0, 0), (dviguu_na)_scale(1, 1), (dviguu_na)_scale(2, 2),
			     rotation_enum());

	} ပေႃးဢမ်ႇ {
		PX4_INFO_RAW("%s %" PRIu32
			     " EN: %d, offset: [%05.3f %05.3f %05.3f], scale: [%05.3f %05.3f %05.3f], Internal\n",
			     SensorString(), device_id(), enabled(),
			     (dviguu_na)_offset(0), (dviguu_na)_offset(1), (dviguu_na)_offset(2),
			     (dviguu_na)_scale(0, 0), (dviguu_na)_scale(1, 1), (dviguu_na)_scale(2, 2));
	}

#ပေႃး defined(DEBUG_BUILD)
	_scale.print();
#endif // DEBUG_BUILD
}

} // namespace calibration
