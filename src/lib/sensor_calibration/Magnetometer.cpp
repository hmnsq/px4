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

#अन्तर्भाव "Magnetometer.hpp"

#अन्तर्भाव "Utilities.hpp"

#अन्तर्भाव <lib/drivers/device/Device.hpp>
#अन्तर्भाव <lib/parameters/param.h>

using नामाकाश matrix;
using नामाकाश time_literals;

नामाकाश calibration
{

Magnetometer::Magnetometer()
{
	Reset();
}

Magnetometer::Magnetometer(uint32_t device_id)
{
	set_device_id(device_id);
}

Magnetometer::Magnetometer(uint32_t device_id, तार्किक external)
{
	set_device_id(device_id, external);
}

शून्य Magnetometer::set_device_id(uint32_t device_id)
{
	set_device_id(device_id, device::device_is_external(device_id));
}

शून्य Magnetometer::set_device_id(uint32_t device_id, तार्किक external)
{
	यदि (_device_id != device_id || _external != external) {

		_device_id = device_id;
		_external = external;

		Reset();

		ParametersUpdate();
		SensorCorrectionsUpdate(सत्य);
	}
}

शून्य Magnetometer::SensorCorrectionsUpdate(तार्किक force)
{
	// check if the selected sensor has updated
	यदि (_sensor_correction_sub.updated() || force) {

		// valid device id required
		यदि (_device_id == 0) {
			प्रत्यागम;
		}

		sensor_correction_s corrections;

		यदि (_sensor_correction_sub.copy(&corrections)) {
			// find sensor_corrections index
			प्रति (पूर्णाङ्क i = 0; i < MAX_SENSOR_गिनती; i++) {
				यदि (corrections.mag_device_ids[i] == _device_id) {
					विकल्प (i) {
					पक्ष 0:
						_thermal_offset = Vector3f{corrections.mag_offset_0};
						प्रत्यागम;
					पक्ष 1:
						_thermal_offset = Vector3f{corrections.mag_offset_1};
						प्रत्यागम;
					पक्ष 2:
						_thermal_offset = Vector3f{corrections.mag_offset_2};
						प्रत्यागम;
					पक्ष 3:
						_thermal_offset = Vector3f{corrections.mag_offset_3};
						प्रत्यागम;
					}
				}
			}
		}

		// zero thermal offset if not found
		_thermal_offset.zero();
	}
}

तार्किक Magnetometer::set_offset(ध्रुव Vector3f &offset)
{
	यदि (Vector3f(_offset - offset).longerThan(0.005f)) {
		यदि (offset.isAllFinite()) {
			_offset = offset;
			calibration_गिनती++;
			प्रत्यागम सत्य;
		}
	}

	प्रत्यागम असत्य;
}

तार्किक Magnetometer::set_scale(ध्रुव Vector3f &scale)
{
	यदि (Vector3f(_scale.diag() - scale).longerThan(0.01f)) {
		यदि (scale.isAllFinite() && (scale(0) > 0.f) && (scale(1) > 0.f) && (scale(2) > 0.f)) {
			_scale(0, 0) = scale(0);
			_scale(1, 1) = scale(1);
			_scale(2, 2) = scale(2);

			calibration_गिनती++;
			प्रत्यागम सत्य;
		}
	}

	प्रत्यागम असत्य;
}

तार्किक Magnetometer::set_offdiagonal(ध्रुव Vector3f &offdiagonal)
{
	यदि (Vector3f(Vector3f{_scale(0, 1), _scale(0, 2), _scale(1, 2)} - offdiagonal).longerThan(0.01f)) {
		यदि (offdiagonal.isAllFinite()) {

			_scale(0, 1) = offdiagonal(0);
			_scale(1, 0) = offdiagonal(0);

			_scale(0, 2) = offdiagonal(1);
			_scale(2, 0) = offdiagonal(1);

			_scale(1, 2) = offdiagonal(2);
			_scale(2, 1) = offdiagonal(2);

			calibration_गिनती++;
			प्रत्यागम सत्य;
		}
	}

	प्रत्यागम असत्य;
}

शून्य Magnetometer::set_rotation(ध्रुव Rotation rotation)
{
	यदि (rotation < ROTATION_MAX) {
		_rotation_enum = rotation;

	} अन्यथा {
		// invalid rotation, resetting
		_rotation_enum = ROTATION_NONE;
	}

	// always apply level adjustments
	_rotation = Dcmf(GetSensorLevelAdjustment()) * get_rot_matrix(_rotation_enum);

	// clear any custom rotation
	_rotation_custom_euler.zero();
}

शून्य Magnetometer::set_custom_rotation(ध्रुव Eulerf &rotation)
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

तार्किक Magnetometer::set_calibration_index(पूर्णाङ्क calibration_index)
{
	यदि ((calibration_index >= 0) && (calibration_index < MAX_SENSOR_गिनती)) {
		_calibration_index = calibration_index;
		प्रत्यागम सत्य;
	}

	प्रत्यागम असत्य;
}

शून्य Magnetometer::ParametersUpdate()
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

तार्किक Magnetometer::ParametersLoad()
{
	यदि (_calibration_index >= 0 && _calibration_index < MAX_SENSOR_गिनती) {
		// CAL_MAGx_ROT
		int32_t rotation_मूल्य = GetCalibrationParamInt32(SensorString(), "ROT", _calibration_index);

		ध्रुव भिन्न euler_roll_deg = GetCalibrationParamFloat(SensorString(), "ROLL", _calibration_index);
		ध्रुव भिन्न euler_pitch_deg = GetCalibrationParamFloat(SensorString(), "PITCH", _calibration_index);
		ध्रुव भिन्न euler_yaw_deg = GetCalibrationParamFloat(SensorString(), "YAW", _calibration_index);

		यदि (_external) {
			यदि (((rotation_मूल्य >= ROTATION_MAX) && (rotation_मूल्य != ROTATION_CUSTOM)) || (rotation_मूल्य < 0)) {
				// invalid rotation, resetting
				rotation_मूल्य = ROTATION_NONE;
			}

			// if CAL_MAGx_{ROLL,PITCH,YAW} manually set then CAL_MAGx_ROT needs to be ROTATION_CUSTOM
			यदि ((rotation_मूल्य != ROTATION_CUSTOM)
			    && ((fabsf(euler_roll_deg) > भग्न_अंतर)
				|| (fabsf(euler_pitch_deg) > भग्न_अंतर)
				|| (fabsf(euler_yaw_deg) > भग्न_अंतर))) {

				rotation_मूल्य = ROTATION_CUSTOM;
				SetCalibrationParam(SensorString(), "ROT", _calibration_index, rotation_मूल्य);
			}

			// Handle custom specified euler angle
			यदि (rotation_मूल्य == ROTATION_CUSTOM) {

				ध्रुव matrix::Eulerf rotation_custom_euler{
					math::radians(euler_roll_deg),
					math::radians(euler_pitch_deg),
					math::radians(euler_yaw_deg)};

				set_custom_rotation(rotation_custom_euler);

			} अन्यथा {
				set_rotation(static_cast<Rotation>(rotation_मूल्य));
			}

		} अन्यथा {
			// internal sensors follow board rotation
			set_rotation(GetBoardRotation());
		}

		// CAL_MAGx_PRIO
		_priority = GetCalibrationParamInt32(SensorString(), "PRIO", _calibration_index);

		यदि ((_priority < 0) || (_priority > 100)) {
			// reset to default, -1 is the uninitialized parameter value
			स्थिर constexpr int32_t CAL_PRIO_UNINITIALIZED = -1;

			यदि (_priority != CAL_PRIO_UNINITIALIZED) {
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

		प्रत्यागम सत्य;
	}

	प्रत्यागम असत्य;
}

शून्य Magnetometer::Reset()
{
	यदि (_external) {
		set_rotation(ROTATION_NONE);

	} अन्यथा {
		// internal sensors follow board rotation
		set_rotation(GetBoardRotation());
	}

	_offset.zero();
	_scale.setIdentity();

	_power_compensation.zero();
	_power = 0.f;

	_priority = _external ? DEFAULT_EXTERNAL_PRIORITY : DEFAULT_PRIORITY;

	_calibration_index = -1;

	calibration_गिनती = 0;
}

तार्किक Magnetometer::ParametersSave(पूर्णाङ्क desired_calibration_index, तार्किक force)
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
		success &= SetCalibrationParam(SensorString(), "PRIO", _calibration_index, _priority);
		success &= SetCalibrationParamsVector3f(SensorString(), "OFF", _calibration_index, _offset);

		ध्रुव Vector3f scale{_scale.diag()};
		success &= SetCalibrationParamsVector3f(SensorString(), "SCALE", _calibration_index, scale);

		ध्रुव Vector3f off_diag{_scale(0, 1), _scale(0, 2), _scale(1, 2)};
		success &= SetCalibrationParamsVector3f(SensorString(), "ODIAG", _calibration_index, off_diag);

		success &= SetCalibrationParamsVector3f(SensorString(), "COMP", _calibration_index, _power_compensation);

		यदि (_external) {
			success &= SetCalibrationParam(SensorString(), "ROT", _calibration_index, (int32_t)_rotation_enum);

		} अन्यथा {
			success &= SetCalibrationParam(SensorString(), "ROT", _calibration_index, -1); // internal
		}

		success &= SetCalibrationParam(SensorString(), "ROLL", _calibration_index, math::degrees(_rotation_custom_euler(0)));
		success &= SetCalibrationParam(SensorString(), "PITCH", _calibration_index, math::degrees(_rotation_custom_euler(1)));
		success &= SetCalibrationParam(SensorString(), "YAW", _calibration_index, math::degrees(_rotation_custom_euler(2)));

		प्रत्यागम success;
	}

	प्रत्यागम असत्य;
}

शून्य Magnetometer::PrintStatus()
{
	यदि (external()) {
		PX4_INFO_RAW("%s %" PRIu32
			     " EN: %d, offset: [%05.3f %05.3f %05.3f], scale: [%05.3f %05.3f %05.3f], Ext ROT: %d\n",
			     SensorString(), device_id(), enabled(),
			     (द्विगुण)_offset(0), (द्विगुण)_offset(1), (द्विगुण)_offset(2),
			     (द्विगुण)_scale(0, 0), (द्विगुण)_scale(1, 1), (द्विगुण)_scale(2, 2),
			     rotation_enum());

	} अन्यथा {
		PX4_INFO_RAW("%s %" PRIu32
			     " EN: %d, offset: [%05.3f %05.3f %05.3f], scale: [%05.3f %05.3f %05.3f], Internal\n",
			     SensorString(), device_id(), enabled(),
			     (द्विगुण)_offset(0), (द्विगुण)_offset(1), (द्विगुण)_offset(2),
			     (द्विगुण)_scale(0, 0), (द्विगुण)_scale(1, 1), (द्विगुण)_scale(2, 2));
	}

#यदि defined(DEBUG_BUILD)
	_scale.print();
#endif // DEBUG_BUILD
}

} // namespace calibration
