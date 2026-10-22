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

#شامل "Magnetometer.hpp"

#شامل "Utilities.hpp"

#شامل <lib/drivers/device/Device.hpp>
#شامل <lib/parameters/param.h>

using فضاینام matrix;
using فضاینام time_literals;

فضاینام calibration
{

Magnetometer::Magnetometer()
{
	Reset();
}

Magnetometer::Magnetometer(uint32_t device_id)
{
	set_device_id(device_id);
}

Magnetometer::Magnetometer(uint32_t device_id, منطقی external)
{
	set_device_id(device_id, external);
}

تهی Magnetometer::set_device_id(uint32_t device_id)
{
	set_device_id(device_id, device::device_is_external(device_id));
}

تهی Magnetometer::set_device_id(uint32_t device_id, منطقی external)
{
	اگر (_device_id != device_id || _external != external) {

		_device_id = device_id;
		_external = external;

		Reset();

		ParametersUpdate();
		SensorCorrectionsUpdate(درست);
	}
}

تهی Magnetometer::SensorCorrectionsUpdate(منطقی force)
{
	// check if the selected sensor has updated
	اگر (_sensor_correction_sub.updated() || force) {

		// valid device id required
		اگر (_device_id == 0) {
			بازگشت;
		}

		sensor_correction_s corrections;

		اگر (_sensor_correction_sub.copy(&corrections)) {
			// find sensor_corrections index
			برای (صحیح i = 0; i < MAX_SENSOR_شمار; i++) {
				اگر (corrections.mag_device_ids[i] == _device_id) {
					انتخاب (i) {
					حالت 0:
						_thermal_offset = Vector3f{corrections.mag_offset_0};
						بازگشت;
					حالت 1:
						_thermal_offset = Vector3f{corrections.mag_offset_1};
						بازگشت;
					حالت 2:
						_thermal_offset = Vector3f{corrections.mag_offset_2};
						بازگشت;
					حالت 3:
						_thermal_offset = Vector3f{corrections.mag_offset_3};
						بازگشت;
					}
				}
			}
		}

		// zero thermal offset if not found
		_thermal_offset.zero();
	}
}

منطقی Magnetometer::set_offset(ثابت Vector3f &offset)
{
	اگر (Vector3f(_offset - offset).longerThan(0.005f)) {
		اگر (offset.isAllFinite()) {
			_offset = offset;
			calibration_شمار++;
			بازگشت درست;
		}
	}

	بازگشت نادرست;
}

منطقی Magnetometer::set_scale(ثابت Vector3f &scale)
{
	اگر (Vector3f(_scale.diag() - scale).longerThan(0.01f)) {
		اگر (scale.isAllFinite() && (scale(0) > 0.f) && (scale(1) > 0.f) && (scale(2) > 0.f)) {
			_scale(0, 0) = scale(0);
			_scale(1, 1) = scale(1);
			_scale(2, 2) = scale(2);

			calibration_شمار++;
			بازگشت درست;
		}
	}

	بازگشت نادرست;
}

منطقی Magnetometer::set_offdiagonal(ثابت Vector3f &offdiagonal)
{
	اگر (Vector3f(Vector3f{_scale(0, 1), _scale(0, 2), _scale(1, 2)} - offdiagonal).longerThan(0.01f)) {
		اگر (offdiagonal.isAllFinite()) {

			_scale(0, 1) = offdiagonal(0);
			_scale(1, 0) = offdiagonal(0);

			_scale(0, 2) = offdiagonal(1);
			_scale(2, 0) = offdiagonal(1);

			_scale(1, 2) = offdiagonal(2);
			_scale(2, 1) = offdiagonal(2);

			calibration_شمار++;
			بازگشت درست;
		}
	}

	بازگشت نادرست;
}

تهی Magnetometer::set_rotation(ثابت Rotation rotation)
{
	اگر (rotation < ROTATION_MAX) {
		_rotation_enum = rotation;

	} وگرنه {
		// invalid rotation, resetting
		_rotation_enum = ROTATION_NONE;
	}

	// always apply level adjustments
	_rotation = Dcmf(GetSensorLevelAdjustment()) * get_rot_matrix(_rotation_enum);

	// clear any custom rotation
	_rotation_custom_euler.zero();
}

تهی Magnetometer::set_custom_rotation(ثابت Eulerf &rotation)
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

منطقی Magnetometer::set_calibration_index(صحیح calibration_index)
{
	اگر ((calibration_index >= 0) && (calibration_index < MAX_SENSOR_شمار)) {
		_calibration_index = calibration_index;
		بازگشت درست;
	}

	بازگشت نادرست;
}

تهی Magnetometer::ParametersUpdate()
{
	اگر (_device_id == 0) {
		بازگشت;
	}

	_calibration_index = FindCurrentCalibrationIndex(SensorString(), _device_id);

	اگر (_calibration_index == -1) {
		// no saved calibration available
		Reset();

	} وگرنه {
		ParametersLoad();
	}
}

منطقی Magnetometer::ParametersLoad()
{
	اگر (_calibration_index >= 0 && _calibration_index < MAX_SENSOR_شمار) {
		// CAL_MAGx_ROT
		int32_t rotation_مقدار = GetCalibrationParamInt32(SensorString(), "ROT", _calibration_index);

		ثابت اعشاری euler_roll_deg = GetCalibrationParamFloat(SensorString(), "ROLL", _calibration_index);
		ثابت اعشاری euler_pitch_deg = GetCalibrationParamFloat(SensorString(), "PITCH", _calibration_index);
		ثابت اعشاری euler_yaw_deg = GetCalibrationParamFloat(SensorString(), "YAW", _calibration_index);

		اگر (_external) {
			اگر (((rotation_مقدار >= ROTATION_MAX) && (rotation_مقدار != ROTATION_CUSTOM)) || (rotation_مقدار < 0)) {
				// invalid rotation, resetting
				rotation_مقدار = ROTATION_NONE;
			}

			// if CAL_MAGx_{ROLL,PITCH,YAW} manually set then CAL_MAGx_ROT needs to be ROTATION_CUSTOM
			اگر ((rotation_مقدار != ROTATION_CUSTOM)
			    && ((fabsf(euler_roll_deg) > FLT_EPSILON)
				|| (fabsf(euler_pitch_deg) > FLT_EPSILON)
				|| (fabsf(euler_yaw_deg) > FLT_EPSILON))) {

				rotation_مقدار = ROTATION_CUSTOM;
				SetCalibrationParam(SensorString(), "ROT", _calibration_index, rotation_مقدار);
			}

			// Handle custom specified euler angle
			اگر (rotation_مقدار == ROTATION_CUSTOM) {

				ثابت matrix::Eulerf rotation_custom_euler{
					math::radians(euler_roll_deg),
					math::radians(euler_pitch_deg),
					math::radians(euler_yaw_deg)};

				set_custom_rotation(rotation_custom_euler);

			} وگرنه {
				set_rotation(static_cast<Rotation>(rotation_مقدار));
			}

		} وگرنه {
			// internal sensors follow board rotation
			set_rotation(GetBoardRotation());
		}

		// CAL_MAGx_PRIO
		_priority = GetCalibrationParamInt32(SensorString(), "PRIO", _calibration_index);

		اگر ((_priority < 0) || (_priority > 100)) {
			// reset to default, -1 is the uninitialized parameter value
			ایستا constexpr int32_t CAL_PRIO_UNINITIALIZED = -1;

			اگر (_priority != CAL_PRIO_UNINITIALIZED) {
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

		بازگشت درست;
	}

	بازگشت نادرست;
}

تهی Magnetometer::Reset()
{
	اگر (_external) {
		set_rotation(ROTATION_NONE);

	} وگرنه {
		// internal sensors follow board rotation
		set_rotation(GetBoardRotation());
	}

	_offset.zero();
	_scale.setIdentity();

	_power_compensation.zero();
	_power = 0.f;

	_priority = _external ? DEFAULT_EXTERNAL_PRIORITY : DEFAULT_PRIORITY;

	_calibration_index = -1;

	calibration_شمار = 0;
}

منطقی Magnetometer::ParametersSave(صحیح desired_calibration_index, منطقی force)
{
	اگر (force && desired_calibration_index >= 0 && desired_calibration_index < MAX_SENSOR_شمار) {
		_calibration_index = desired_calibration_index;

	} وگرنه اگر (!force || (_calibration_index < 0)
		   || (desired_calibration_index != -1 && desired_calibration_index != _calibration_index)) {

		// ensure we have a valid calibration slot (matching existing or first available slot)
		int8_t calibration_index_prev = _calibration_index;
		_calibration_index = FindAvailableCalibrationIndex(SensorString(), _device_id, desired_calibration_index);

		اگر (calibration_index_prev >= 0 && (calibration_index_prev != _calibration_index)) {
			PX4_WARN("%s %" PRIu32 " calibration index changed %" PRIi8 " -> %" PRIi8, SensorString(), _device_id,
				 calibration_index_prev, _calibration_index);
		}
	}

	اگر (_calibration_index >= 0 && _calibration_index < MAX_SENSOR_شمار) {
		// save calibration
		منطقی success = درست;
		success &= SetCalibrationParam(SensorString(), "ID", _calibration_index, _device_id);
		success &= SetCalibrationParam(SensorString(), "PRIO", _calibration_index, _priority);
		success &= SetCalibrationParamsVector3f(SensorString(), "OFF", _calibration_index, _offset);

		ثابت Vector3f scale{_scale.diag()};
		success &= SetCalibrationParamsVector3f(SensorString(), "SCALE", _calibration_index, scale);

		ثابت Vector3f off_diag{_scale(0, 1), _scale(0, 2), _scale(1, 2)};
		success &= SetCalibrationParamsVector3f(SensorString(), "ODIAG", _calibration_index, off_diag);

		success &= SetCalibrationParamsVector3f(SensorString(), "COMP", _calibration_index, _power_compensation);

		اگر (_external) {
			success &= SetCalibrationParam(SensorString(), "ROT", _calibration_index, (int32_t)_rotation_enum);

		} وگرنه {
			success &= SetCalibrationParam(SensorString(), "ROT", _calibration_index, -1); // internal
		}

		success &= SetCalibrationParam(SensorString(), "ROLL", _calibration_index, math::degrees(_rotation_custom_euler(0)));
		success &= SetCalibrationParam(SensorString(), "PITCH", _calibration_index, math::degrees(_rotation_custom_euler(1)));
		success &= SetCalibrationParam(SensorString(), "YAW", _calibration_index, math::degrees(_rotation_custom_euler(2)));

		بازگشت success;
	}

	بازگشت نادرست;
}

تهی Magnetometer::PrintStatus()
{
	اگر (external()) {
		PX4_INFO_RAW("%s %" PRIu32
			     " EN: %d, offset: [%05.3f %05.3f %05.3f], scale: [%05.3f %05.3f %05.3f], Ext ROT: %d\n",
			     SensorString(), device_id(), enabled(),
			     (دوگانه)_offset(0), (دوگانه)_offset(1), (دوگانه)_offset(2),
			     (دوگانه)_scale(0, 0), (دوگانه)_scale(1, 1), (دوگانه)_scale(2, 2),
			     rotation_enum());

	} وگرنه {
		PX4_INFO_RAW("%s %" PRIu32
			     " EN: %d, offset: [%05.3f %05.3f %05.3f], scale: [%05.3f %05.3f %05.3f], Internal\n",
			     SensorString(), device_id(), enabled(),
			     (دوگانه)_offset(0), (دوگانه)_offset(1), (دوگانه)_offset(2),
			     (دوگانه)_scale(0, 0), (دوگانه)_scale(1, 1), (دوگانه)_scale(2, 2));
	}

#اگر defined(DEBUG_BUILD)
	_scale.print();
#endif // DEBUG_BUILD
}

} // namespace calibration
