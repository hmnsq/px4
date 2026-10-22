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

#شامل "Accelerometer.hpp"

#شامل "Utilities.hpp"

#شامل <lib/drivers/device/Device.hpp>
#شامل <lib/parameters/param.h>

using فضاینام matrix;
using فضاینام time_literals;

فضاینام calibration
{

Accelerometer::Accelerometer()
{
	Reset();
}

Accelerometer::Accelerometer(uint32_t device_id)
{
	set_device_id(device_id);
}

Accelerometer::Accelerometer(uint32_t device_id, منطقی external)
{
	set_device_id(device_id, external);
}

تهی Accelerometer::set_device_id(uint32_t device_id)
{
	set_device_id(device_id, device::device_is_external(device_id));
}

تهی Accelerometer::set_device_id(uint32_t device_id, منطقی external)
{
	اگر (_device_id != device_id || _external != external) {

		_device_id = device_id;
		_external = external;

		Reset();

		ParametersUpdate();
		SensorCorrectionsUpdate(درست);
	}
}

تهی Accelerometer::SensorCorrectionsUpdate(منطقی force)
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
				اگر (corrections.accel_device_ids[i] == _device_id) {
					انتخاب (i) {
					حالت 0:
						_thermal_offset = Vector3f{corrections.accel_offset_0};
						بازگشت;
					حالت 1:
						_thermal_offset = Vector3f{corrections.accel_offset_1};
						بازگشت;
					حالت 2:
						_thermal_offset = Vector3f{corrections.accel_offset_2};
						بازگشت;
					حالت 3:
						_thermal_offset = Vector3f{corrections.accel_offset_3};
						بازگشت;
					}
				}
			}
		}

		// zero thermal offset if not found
		_thermal_offset.zero();
	}
}

منطقی Accelerometer::set_offset(ثابت Vector3f &offset)
{
	اگر (Vector3f(_offset - offset).longerThan(0.01f)) {
		اگر (offset.isAllFinite()) {
			_offset = offset;
			calibration_شمار++;
			بازگشت درست;
		}
	}

	بازگشت نادرست;
}

منطقی Accelerometer::set_scale(ثابت Vector3f &scale)
{
	اگر (Vector3f(_scale - scale).longerThan(0.01f)) {
		اگر (scale.isAllFinite() && (scale(0) > 0.f) && (scale(1) > 0.f) && (scale(2) > 0.f)) {
			_scale = scale;
			calibration_شمار++;
			بازگشت درست;
		}
	}

	بازگشت نادرست;
}

تهی Accelerometer::set_rotation(Rotation rotation)
{
	_rotation_enum = rotation;

	// always apply board level adjustments
	_rotation = Dcmf(GetSensorLevelAdjustment()) * get_rot_matrix(rotation);
}

منطقی Accelerometer::set_calibration_index(صحیح calibration_index)
{
	اگر ((calibration_index >= 0) && (calibration_index < MAX_SENSOR_شمار)) {
		_calibration_index = calibration_index;
		بازگشت درست;
	}

	بازگشت نادرست;
}

تهی Accelerometer::ParametersUpdate()
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

منطقی Accelerometer::ParametersLoad()
{
	اگر (_calibration_index >= 0 && _calibration_index < MAX_SENSOR_شمار) {
		// CAL_ACCx_ROT
		int32_t rotation_مقدار = GetCalibrationParamInt32(SensorString(), "ROT", _calibration_index);

		اگر (_external) {
			اگر ((rotation_مقدار >= ROTATION_MAX) || (rotation_مقدار < 0)) {
				// invalid rotation, resetting
				rotation_مقدار = ROTATION_NONE;
			}

			set_rotation(static_cast<Rotation>(rotation_مقدار));

		} وگرنه {
			// internal sensors follow board rotation
			set_rotation(GetBoardRotation());
		}

		// CAL_ACCx_PRIO
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

		// CAL_ACCx_OFF{X,Y,Z}
		set_offset(GetCalibrationParamsVector3f(SensorString(), "OFF", _calibration_index));

		// CAL_ACCx_SCALE{X,Y,Z}
		set_scale(GetCalibrationParamsVector3f(SensorString(), "SCALE", _calibration_index));

		بازگشت درست;
	}

	بازگشت نادرست;
}

تهی Accelerometer::Reset()
{
	اگر (_external) {
		set_rotation(ROTATION_NONE);

	} وگرنه {
		// internal sensors follow board rotation
		set_rotation(GetBoardRotation());
	}

	_offset.zero();
	_scale = Vector3f{1.f, 1.f, 1.f};

	_thermal_offset.zero();

	_priority = _external ? DEFAULT_EXTERNAL_PRIORITY : DEFAULT_PRIORITY;

	_calibration_index = -1;

	calibration_شمار = 0;
}

منطقی Accelerometer::ParametersSave(صحیح desired_calibration_index, منطقی force)
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
		success &= SetCalibrationParamsVector3f(SensorString(), "SCALE", _calibration_index, _scale);

		اگر (_external) {
			success &= SetCalibrationParam(SensorString(), "ROT", _calibration_index, (int32_t)_rotation_enum);

		} وگرنه {
			success &= SetCalibrationParam(SensorString(), "ROT", _calibration_index, -1); // internal
		}

		بازگشت success;
	}

	بازگشت نادرست;
}

تهی Accelerometer::PrintStatus()
{
	اگر (external()) {
		PX4_INFO_RAW("%s %" PRIu32
			     " EN: %d, offset: [%05.3f %05.3f %05.3f], scale: [%05.3f %05.3f %05.3f], Ext ROT: %d\n",
			     SensorString(), device_id(), enabled(),
			     (دوگانه)_offset(0), (دوگانه)_offset(1), (دوگانه)_offset(2),
			     (دوگانه)_scale(0), (دوگانه)_scale(1), (دوگانه)_scale(2),
			     rotation_enum());

	} وگرنه {
		PX4_INFO_RAW("%s %" PRIu32
			     " EN: %d, offset: [%05.3f %05.3f %05.3f], scale: [%05.3f %05.3f %05.3f], Internal\n",
			     SensorString(), device_id(), enabled(),
			     (دوگانه)_offset(0), (دوگانه)_offset(1), (دوگانه)_offset(2),
			     (دوگانه)_scale(0), (دوگانه)_scale(1), (دوگانه)_scale(2));
	}

	اگر (_thermal_offset.norm() > 0.f) {
		PX4_INFO_RAW("%s %" PRIu32 " temperature offset: [%.4f %.4f %.4f]\n", SensorString(), _device_id,
			     (دوگانه)_thermal_offset(0), (دوگانه)_thermal_offset(1), (دوگانه)_thermal_offset(2));
	}
}

} // namespace calibration
