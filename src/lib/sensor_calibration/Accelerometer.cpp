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

#अन्तर्भाव "Accelerometer.hpp"

#अन्तर्भाव "Utilities.hpp"

#अन्तर्भाव <lib/drivers/device/Device.hpp>
#अन्तर्भाव <lib/parameters/param.h>

using नामाकाश matrix;
using नामाकाश time_literals;

नामाकाश calibration
{

Accelerometer::Accelerometer()
{
	Reset();
}

Accelerometer::Accelerometer(uint32_t device_id)
{
	set_device_id(device_id);
}

Accelerometer::Accelerometer(uint32_t device_id, तार्किक external)
{
	set_device_id(device_id, external);
}

शून्य Accelerometer::set_device_id(uint32_t device_id)
{
	set_device_id(device_id, device::device_is_external(device_id));
}

शून्य Accelerometer::set_device_id(uint32_t device_id, तार्किक external)
{
	जइ (_device_id != device_id || _external != external) {

		_device_id = device_id;
		_external = external;

		Reset();

		ParametersUpdate();
		SensorCorrectionsUpdate(सच्च);
	}
}

शून्य Accelerometer::SensorCorrectionsUpdate(तार्किक force)
{
	// check if the selected sensor has updated
	जइ (_sensor_correction_sub.updated() || force) {

		// valid device id required
		जइ (_device_id == 0) {
			प्रत्यागम;
		}

		sensor_correction_s corrections;

		जइ (_sensor_correction_sub.copy(&corrections)) {
			// find sensor_corrections index
			प्रति (पूर्णाङ्क i = 0; i < MAX_SENSOR_गणनम्; i++) {
				जइ (corrections.accel_device_ids[i] == _device_id) {
					विकल्प (i) {
					पक्ष 0:
						_thermal_offset = Vector3f{corrections.accel_offset_0};
						प्रत्यागम;
					पक्ष 1:
						_thermal_offset = Vector3f{corrections.accel_offset_1};
						प्रत्यागम;
					पक्ष 2:
						_thermal_offset = Vector3f{corrections.accel_offset_2};
						प्रत्यागम;
					पक्ष 3:
						_thermal_offset = Vector3f{corrections.accel_offset_3};
						प्रत्यागम;
					}
				}
			}
		}

		// zero thermal offset if not found
		_thermal_offset.zero();
	}
}

तार्किक Accelerometer::set_offset(ध्रुव Vector3f &offset)
{
	जइ (Vector3f(_offset - offset).longerThan(0.01f)) {
		जइ (offset.isAllFinite()) {
			_offset = offset;
			calibration_गणनम्++;
			प्रत्यागम सच्च;
		}
	}

	प्रत्यागम मिच्छ;
}

तार्किक Accelerometer::set_scale(ध्रुव Vector3f &scale)
{
	जइ (Vector3f(_scale - scale).longerThan(0.01f)) {
		जइ (scale.isAllFinite() && (scale(0) > 0.f) && (scale(1) > 0.f) && (scale(2) > 0.f)) {
			_scale = scale;
			calibration_गणनम्++;
			प्रत्यागम सच्च;
		}
	}

	प्रत्यागम मिच्छ;
}

शून्य Accelerometer::set_rotation(Rotation rotation)
{
	_rotation_enum = rotation;

	// always apply board level adjustments
	_rotation = Dcmf(GetSensorLevelAdjustment()) * get_rot_matrix(rotation);
}

तार्किक Accelerometer::set_calibration_index(पूर्णाङ्क calibration_index)
{
	जइ ((calibration_index >= 0) && (calibration_index < MAX_SENSOR_गणनम्)) {
		_calibration_index = calibration_index;
		प्रत्यागम सच्च;
	}

	प्रत्यागम मिच्छ;
}

शून्य Accelerometer::ParametersUpdate()
{
	जइ (_device_id == 0) {
		प्रत्यागम;
	}

	_calibration_index = FindCurrentCalibrationIndex(SensorString(), _device_id);

	जइ (_calibration_index == -1) {
		// no saved calibration available
		Reset();

	} अण्णहा {
		ParametersLoad();
	}
}

तार्किक Accelerometer::ParametersLoad()
{
	जइ (_calibration_index >= 0 && _calibration_index < MAX_SENSOR_गणनम्) {
		// CAL_ACCx_ROT
		int32_t rotation_मानम् = GetCalibrationParamInt32(SensorString(), "ROT", _calibration_index);

		जइ (_external) {
			जइ ((rotation_मानम् >= ROTATION_MAX) || (rotation_मानम् < 0)) {
				// invalid rotation, resetting
				rotation_मानम् = ROTATION_NONE;
			}

			set_rotation(static_cast<Rotation>(rotation_मानम्));

		} अण्णहा {
			// internal sensors follow board rotation
			set_rotation(GetBoardRotation());
		}

		// CAL_ACCx_PRIO
		_priority = GetCalibrationParamInt32(SensorString(), "PRIO", _calibration_index);

		जइ ((_priority < 0) || (_priority > 100)) {
			// reset to default, -1 is the uninitialized parameter value
			स्थिर constexpr int32_t CAL_PRIO_UNINITIALIZED = -1;

			जइ (_priority != CAL_PRIO_UNINITIALIZED) {
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

		प्रत्यागम सच्च;
	}

	प्रत्यागम मिच्छ;
}

शून्य Accelerometer::Reset()
{
	जइ (_external) {
		set_rotation(ROTATION_NONE);

	} अण्णहा {
		// internal sensors follow board rotation
		set_rotation(GetBoardRotation());
	}

	_offset.zero();
	_scale = Vector3f{1.f, 1.f, 1.f};

	_thermal_offset.zero();

	_priority = _external ? DEFAULT_EXTERNAL_PRIORITY : DEFAULT_PRIORITY;

	_calibration_index = -1;

	calibration_गणनम् = 0;
}

तार्किक Accelerometer::ParametersSave(पूर्णाङ्क desired_calibration_index, तार्किक force)
{
	जइ (force && desired_calibration_index >= 0 && desired_calibration_index < MAX_SENSOR_गणनम्) {
		_calibration_index = desired_calibration_index;

	} अण्णहा जइ (!force || (_calibration_index < 0)
		   || (desired_calibration_index != -1 && desired_calibration_index != _calibration_index)) {

		// ensure we have a valid calibration slot (matching existing or first available slot)
		int8_t calibration_index_prev = _calibration_index;
		_calibration_index = FindAvailableCalibrationIndex(SensorString(), _device_id, desired_calibration_index);

		जइ (calibration_index_prev >= 0 && (calibration_index_prev != _calibration_index)) {
			PX4_WARN("%s %" PRIu32 " calibration index changed %" PRIi8 " -> %" PRIi8, SensorString(), _device_id,
				 calibration_index_prev, _calibration_index);
		}
	}

	जइ (_calibration_index >= 0 && _calibration_index < MAX_SENSOR_गणनम्) {
		// save calibration
		तार्किक success = सच्च;
		success &= SetCalibrationParam(SensorString(), "ID", _calibration_index, _device_id);
		success &= SetCalibrationParam(SensorString(), "PRIO", _calibration_index, _priority);
		success &= SetCalibrationParamsVector3f(SensorString(), "OFF", _calibration_index, _offset);
		success &= SetCalibrationParamsVector3f(SensorString(), "SCALE", _calibration_index, _scale);

		जइ (_external) {
			success &= SetCalibrationParam(SensorString(), "ROT", _calibration_index, (int32_t)_rotation_enum);

		} अण्णहा {
			success &= SetCalibrationParam(SensorString(), "ROT", _calibration_index, -1); // internal
		}

		प्रत्यागम success;
	}

	प्रत्यागम मिच्छ;
}

शून्य Accelerometer::PrintStatus()
{
	जइ (external()) {
		PX4_INFO_RAW("%s %" PRIu32
			     " EN: %d, offset: [%05.3f %05.3f %05.3f], scale: [%05.3f %05.3f %05.3f], Ext ROT: %d\n",
			     SensorString(), device_id(), enabled(),
			     (द्विगुण)_offset(0), (द्विगुण)_offset(1), (द्विगुण)_offset(2),
			     (द्विगुण)_scale(0), (द्विगुण)_scale(1), (द्विगुण)_scale(2),
			     rotation_enum());

	} अण्णहा {
		PX4_INFO_RAW("%s %" PRIu32
			     " EN: %d, offset: [%05.3f %05.3f %05.3f], scale: [%05.3f %05.3f %05.3f], Internal\n",
			     SensorString(), device_id(), enabled(),
			     (द्विगुण)_offset(0), (द्विगुण)_offset(1), (द्विगुण)_offset(2),
			     (द्विगुण)_scale(0), (द्विगुण)_scale(1), (द्विगुण)_scale(2));
	}

	जइ (_thermal_offset.norm() > 0.f) {
		PX4_INFO_RAW("%s %" PRIu32 " temperature offset: [%.4f %.4f %.4f]\n", SensorString(), _device_id,
			     (द्विगुण)_thermal_offset(0), (द्विगुण)_thermal_offset(1), (द्विगुण)_thermal_offset(2));
	}
}

} // namespace calibration
