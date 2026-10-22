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

#અન્તર્ભાવ "Magnetometer.hpp"

#અન્તર્ભાવ "Utilities.hpp"

#અન્તર્ભાવ <lib/drivers/device/Device.hpp>
#અન્તર્ભાવ <lib/parameters/param.h>

using નામાકાશ matrix;
using નામાકાશ time_literals;

નામાકાશ calibration
{

Magnetometer::Magnetometer()
{
	Reset();
}

Magnetometer::Magnetometer(uint32_t device_id)
{
	set_device_id(device_id);
}

Magnetometer::Magnetometer(uint32_t device_id, તાર્કિક external)
{
	set_device_id(device_id, external);
}

શૂન્ય Magnetometer::set_device_id(uint32_t device_id)
{
	set_device_id(device_id, device::device_is_external(device_id));
}

શૂન્ય Magnetometer::set_device_id(uint32_t device_id, તાર્કિક external)
{
	જો (_device_id != device_id || _external != external) {

		_device_id = device_id;
		_external = external;

		Reset();

		ParametersUpdate();
		SensorCorrectionsUpdate(સાચું);
	}
}

શૂન્ય Magnetometer::SensorCorrectionsUpdate(તાર્કિક force)
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
				જો (corrections.mag_device_ids[i] == _device_id) {
					પસંદગી (i) {
					સ્થિતિ 0:
						_thermal_offset = Vector3f{corrections.mag_offset_0};
						પાછા;
					સ્થિતિ 1:
						_thermal_offset = Vector3f{corrections.mag_offset_1};
						પાછા;
					સ્થિતિ 2:
						_thermal_offset = Vector3f{corrections.mag_offset_2};
						પાછા;
					સ્થિતિ 3:
						_thermal_offset = Vector3f{corrections.mag_offset_3};
						પાછા;
					}
				}
			}
		}

		// zero thermal offset if not found
		_thermal_offset.zero();
	}
}

તાર્કિક Magnetometer::set_offset(અચલ Vector3f &offset)
{
	જો (Vector3f(_offset - offset).longerThan(0.005f)) {
		જો (offset.isAllFinite()) {
			_offset = offset;
			calibration_ગણતરી++;
			પાછા સાચું;
		}
	}

	પાછા ખોટું;
}

તાર્કિક Magnetometer::set_scale(અચલ Vector3f &scale)
{
	જો (Vector3f(_scale.diag() - scale).longerThan(0.01f)) {
		જો (scale.isAllFinite() && (scale(0) > 0.f) && (scale(1) > 0.f) && (scale(2) > 0.f)) {
			_scale(0, 0) = scale(0);
			_scale(1, 1) = scale(1);
			_scale(2, 2) = scale(2);

			calibration_ગણતરી++;
			પાછા સાચું;
		}
	}

	પાછા ખોટું;
}

તાર્કિક Magnetometer::set_offdiagonal(અચલ Vector3f &offdiagonal)
{
	જો (Vector3f(Vector3f{_scale(0, 1), _scale(0, 2), _scale(1, 2)} - offdiagonal).longerThan(0.01f)) {
		જો (offdiagonal.isAllFinite()) {

			_scale(0, 1) = offdiagonal(0);
			_scale(1, 0) = offdiagonal(0);

			_scale(0, 2) = offdiagonal(1);
			_scale(2, 0) = offdiagonal(1);

			_scale(1, 2) = offdiagonal(2);
			_scale(2, 1) = offdiagonal(2);

			calibration_ગણતરી++;
			પાછા સાચું;
		}
	}

	પાછા ખોટું;
}

શૂન્ય Magnetometer::set_rotation(અચલ Rotation rotation)
{
	જો (rotation < ROTATION_MAX) {
		_rotation_enum = rotation;

	} નહીંતર {
		// invalid rotation, resetting
		_rotation_enum = ROTATION_NONE;
	}

	// always apply level adjustments
	_rotation = Dcmf(GetSensorLevelAdjustment()) * get_rot_matrix(_rotation_enum);

	// clear any custom rotation
	_rotation_custom_euler.zero();
}

શૂન્ય Magnetometer::set_custom_rotation(અચલ Eulerf &rotation)
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

તાર્કિક Magnetometer::set_calibration_index(પૂર્ણાંક calibration_index)
{
	જો ((calibration_index >= 0) && (calibration_index < MAX_SENSOR_ગણતરી)) {
		_calibration_index = calibration_index;
		પાછા સાચું;
	}

	પાછા ખોટું;
}

શૂન્ય Magnetometer::ParametersUpdate()
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

તાર્કિક Magnetometer::ParametersLoad()
{
	જો (_calibration_index >= 0 && _calibration_index < MAX_SENSOR_ગણતરી) {
		// CAL_MAGx_ROT
		int32_t rotation_કિંમત = GetCalibrationParamInt32(SensorString(), "ROT", _calibration_index);

		અચલ દશાંશ euler_roll_deg = GetCalibrationParamFloat(SensorString(), "ROLL", _calibration_index);
		અચલ દશાંશ euler_pitch_deg = GetCalibrationParamFloat(SensorString(), "PITCH", _calibration_index);
		અચલ દશાંશ euler_yaw_deg = GetCalibrationParamFloat(SensorString(), "YAW", _calibration_index);

		જો (_external) {
			જો (((rotation_કિંમત >= ROTATION_MAX) && (rotation_કિંમત != ROTATION_CUSTOM)) || (rotation_કિંમત < 0)) {
				// invalid rotation, resetting
				rotation_કિંમત = ROTATION_NONE;
			}

			// if CAL_MAGx_{ROLL,PITCH,YAW} manually set then CAL_MAGx_ROT needs to be ROTATION_CUSTOM
			જો ((rotation_કિંમત != ROTATION_CUSTOM)
			    && ((fabsf(euler_roll_deg) > FLT_EPSILON)
				|| (fabsf(euler_pitch_deg) > FLT_EPSILON)
				|| (fabsf(euler_yaw_deg) > FLT_EPSILON))) {

				rotation_કિંમત = ROTATION_CUSTOM;
				SetCalibrationParam(SensorString(), "ROT", _calibration_index, rotation_કિંમત);
			}

			// Handle custom specified euler angle
			જો (rotation_કિંમત == ROTATION_CUSTOM) {

				અચલ matrix::Eulerf rotation_custom_euler{
					math::radians(euler_roll_deg),
					math::radians(euler_pitch_deg),
					math::radians(euler_yaw_deg)};

				set_custom_rotation(rotation_custom_euler);

			} નહીંતર {
				set_rotation(static_cast<Rotation>(rotation_કિંમત));
			}

		} નહીંતર {
			// internal sensors follow board rotation
			set_rotation(GetBoardRotation());
		}

		// CAL_MAGx_PRIO
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

		// CAL_MAGx_OFF{X,Y,Z}
		set_offset(GetCalibrationParamsVector3f(SensorString(), "OFF", _calibration_index));

		// CAL_MAGx_SCALE{X,Y,Z}
		set_scale(GetCalibrationParamsVector3f(SensorString(), "SCALE", _calibration_index));

		// CAL_MAGx_ODIAG{X,Y,Z}
		set_offdiagonal(GetCalibrationParamsVector3f(SensorString(), "ODIAG", _calibration_index));

		// CAL_MAGx_COMP{X,Y,Z}
		_power_compensation = GetCalibrationParamsVector3f(SensorString(), "COMP", _calibration_index);

		પાછા સાચું;
	}

	પાછા ખોટું;
}

શૂન્ય Magnetometer::Reset()
{
	જો (_external) {
		set_rotation(ROTATION_NONE);

	} નહીંતર {
		// internal sensors follow board rotation
		set_rotation(GetBoardRotation());
	}

	_offset.zero();
	_scale.setIdentity();

	_power_compensation.zero();
	_power = 0.f;

	_priority = _external ? DEFAULT_EXTERNAL_PRIORITY : DEFAULT_PRIORITY;

	_calibration_index = -1;

	calibration_ગણતરી = 0;
}

તાર્કિક Magnetometer::ParametersSave(પૂર્ણાંક desired_calibration_index, તાર્કિક force)
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
		success &= SetCalibrationParamsVector3f(SensorString(), "OFF", _calibration_index, _offset);

		અચલ Vector3f scale{_scale.diag()};
		success &= SetCalibrationParamsVector3f(SensorString(), "SCALE", _calibration_index, scale);

		અચલ Vector3f off_diag{_scale(0, 1), _scale(0, 2), _scale(1, 2)};
		success &= SetCalibrationParamsVector3f(SensorString(), "ODIAG", _calibration_index, off_diag);

		success &= SetCalibrationParamsVector3f(SensorString(), "COMP", _calibration_index, _power_compensation);

		જો (_external) {
			success &= SetCalibrationParam(SensorString(), "ROT", _calibration_index, (int32_t)_rotation_enum);

		} નહીંતર {
			success &= SetCalibrationParam(SensorString(), "ROT", _calibration_index, -1); // internal
		}

		success &= SetCalibrationParam(SensorString(), "ROLL", _calibration_index, math::degrees(_rotation_custom_euler(0)));
		success &= SetCalibrationParam(SensorString(), "PITCH", _calibration_index, math::degrees(_rotation_custom_euler(1)));
		success &= SetCalibrationParam(SensorString(), "YAW", _calibration_index, math::degrees(_rotation_custom_euler(2)));

		પાછા success;
	}

	પાછા ખોટું;
}

શૂન્ય Magnetometer::PrintStatus()
{
	જો (external()) {
		PX4_INFO_RAW("%s %" PRIu32
			     " EN: %d, offset: [%05.3f %05.3f %05.3f], scale: [%05.3f %05.3f %05.3f], Ext ROT: %d\n",
			     SensorString(), device_id(), enabled(),
			     (દ્વિગુણ)_offset(0), (દ્વિગુણ)_offset(1), (દ્વિગુણ)_offset(2),
			     (દ્વિગુણ)_scale(0, 0), (દ્વિગુણ)_scale(1, 1), (દ્વિગુણ)_scale(2, 2),
			     rotation_enum());

	} નહીંતર {
		PX4_INFO_RAW("%s %" PRIu32
			     " EN: %d, offset: [%05.3f %05.3f %05.3f], scale: [%05.3f %05.3f %05.3f], Internal\n",
			     SensorString(), device_id(), enabled(),
			     (દ્વિગુણ)_offset(0), (દ્વિગુણ)_offset(1), (દ્વિગુણ)_offset(2),
			     (દ્વિગુણ)_scale(0, 0), (દ્વિગુણ)_scale(1, 1), (દ્વિગુણ)_scale(2, 2));
	}

#જો defined(DEBUG_BUILD)
	_scale.print();
#endif // DEBUG_BUILD
}

} // namespace calibration
