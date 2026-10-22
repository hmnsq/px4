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

#ଅନ୍ତର୍ଭାଵ "Magnetometer.hpp"

#ଅନ୍ତର୍ଭାଵ "Utilities.hpp"

#ଅନ୍ତର୍ଭାଵ <lib/drivers/device/Device.hpp>
#ଅନ୍ତର୍ଭାଵ <lib/parameters/param.h>

using ନାମାକାଶ matrix;
using ନାମାକାଶ time_literals;

ନାମାକାଶ calibration
{

Magnetometer::Magnetometer()
{
	Reset();
}

Magnetometer::Magnetometer(uint32_t device_id)
{
	set_device_id(device_id);
}

Magnetometer::Magnetometer(uint32_t device_id, ତାର୍କିକ external)
{
	set_device_id(device_id, external);
}

ଶୂନ୍ୟ Magnetometer::set_device_id(uint32_t device_id)
{
	set_device_id(device_id, device::device_is_external(device_id));
}

ଶୂନ୍ୟ Magnetometer::set_device_id(uint32_t device_id, ତାର୍କିକ external)
{
	ଯଦି (_device_id != device_id || _external != external) {

		_device_id = device_id;
		_external = external;

		Reset();

		ParametersUpdate();
		SensorCorrectionsUpdate(ସତ୍ୟ);
	}
}

ଶୂନ୍ୟ Magnetometer::SensorCorrectionsUpdate(ତାର୍କିକ force)
{
	// check if the selected sensor has updated
	ଯଦି (_sensor_correction_sub.updated() || force) {

		// valid device id required
		ଯଦି (_device_id == 0) {
			ଫେରନ୍ତୁ;
		}

		sensor_correction_s corrections;

		ଯଦି (_sensor_correction_sub.copy(&corrections)) {
			// find sensor_corrections index
			ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < MAX_SENSOR_ଗଣନା; i++) {
				ଯଦି (corrections.mag_device_ids[i] == _device_id) {
					ବାଛ (i) {
					ସ୍ଥିତି 0:
						_thermal_offset = Vector3f{corrections.mag_offset_0};
						ଫେରନ୍ତୁ;
					ସ୍ଥିତି 1:
						_thermal_offset = Vector3f{corrections.mag_offset_1};
						ଫେରନ୍ତୁ;
					ସ୍ଥିତି 2:
						_thermal_offset = Vector3f{corrections.mag_offset_2};
						ଫେରନ୍ତୁ;
					ସ୍ଥିତି 3:
						_thermal_offset = Vector3f{corrections.mag_offset_3};
						ଫେରନ୍ତୁ;
					}
				}
			}
		}

		// zero thermal offset if not found
		_thermal_offset.zero();
	}
}

ତାର୍କିକ Magnetometer::set_offset(ଅଚଳ Vector3f &offset)
{
	ଯଦି (Vector3f(_offset - offset).longerThan(0.005f)) {
		ଯଦି (offset.isAllFinite()) {
			_offset = offset;
			calibration_ଗଣନା++;
			ଫେରନ୍ତୁ ସତ୍ୟ;
		}
	}

	ଫେରନ୍ତୁ ମିଥ୍ୟା;
}

ତାର୍କିକ Magnetometer::set_scale(ଅଚଳ Vector3f &scale)
{
	ଯଦି (Vector3f(_scale.diag() - scale).longerThan(0.01f)) {
		ଯଦି (scale.isAllFinite() && (scale(0) > 0.f) && (scale(1) > 0.f) && (scale(2) > 0.f)) {
			_scale(0, 0) = scale(0);
			_scale(1, 1) = scale(1);
			_scale(2, 2) = scale(2);

			calibration_ଗଣନା++;
			ଫେରନ୍ତୁ ସତ୍ୟ;
		}
	}

	ଫେରନ୍ତୁ ମିଥ୍ୟା;
}

ତାର୍କିକ Magnetometer::set_offdiagonal(ଅଚଳ Vector3f &offdiagonal)
{
	ଯଦି (Vector3f(Vector3f{_scale(0, 1), _scale(0, 2), _scale(1, 2)} - offdiagonal).longerThan(0.01f)) {
		ଯଦି (offdiagonal.isAllFinite()) {

			_scale(0, 1) = offdiagonal(0);
			_scale(1, 0) = offdiagonal(0);

			_scale(0, 2) = offdiagonal(1);
			_scale(2, 0) = offdiagonal(1);

			_scale(1, 2) = offdiagonal(2);
			_scale(2, 1) = offdiagonal(2);

			calibration_ଗଣନା++;
			ଫେରନ୍ତୁ ସତ୍ୟ;
		}
	}

	ଫେରନ୍ତୁ ମିଥ୍ୟା;
}

ଶୂନ୍ୟ Magnetometer::set_rotation(ଅଚଳ Rotation rotation)
{
	ଯଦି (rotation < ROTATION_MAX) {
		_rotation_enum = rotation;

	} ଅନ୍ୟଥା {
		// invalid rotation, resetting
		_rotation_enum = ROTATION_NONE;
	}

	// always apply level adjustments
	_rotation = Dcmf(GetSensorLevelAdjustment()) * get_rot_matrix(_rotation_enum);

	// clear any custom rotation
	_rotation_custom_euler.zero();
}

ଶୂନ୍ୟ Magnetometer::set_custom_rotation(ଅଚଳ Eulerf &rotation)
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

ତାର୍କିକ Magnetometer::set_calibration_index(ପୂର୍ଣ୍ଣସଂଖ୍ୟା calibration_index)
{
	ଯଦି ((calibration_index >= 0) && (calibration_index < MAX_SENSOR_ଗଣନା)) {
		_calibration_index = calibration_index;
		ଫେରନ୍ତୁ ସତ୍ୟ;
	}

	ଫେରନ୍ତୁ ମିଥ୍ୟା;
}

ଶୂନ୍ୟ Magnetometer::ParametersUpdate()
{
	ଯଦି (_device_id == 0) {
		ଫେରନ୍ତୁ;
	}

	_calibration_index = FindCurrentCalibrationIndex(SensorString(), _device_id);

	ଯଦି (_calibration_index == -1) {
		// no saved calibration available
		Reset();

	} ଅନ୍ୟଥା {
		ParametersLoad();
	}
}

ତାର୍କିକ Magnetometer::ParametersLoad()
{
	ଯଦି (_calibration_index >= 0 && _calibration_index < MAX_SENSOR_ଗଣନା) {
		// CAL_MAGx_ROT
		int32_t rotation_ମୂଲ୍ୟ = GetCalibrationParamInt32(SensorString(), "ROT", _calibration_index);

		ଅଚଳ ଦଶମିକ euler_roll_deg = GetCalibrationParamFloat(SensorString(), "ROLL", _calibration_index);
		ଅଚଳ ଦଶମିକ euler_pitch_deg = GetCalibrationParamFloat(SensorString(), "PITCH", _calibration_index);
		ଅଚଳ ଦଶମିକ euler_yaw_deg = GetCalibrationParamFloat(SensorString(), "YAW", _calibration_index);

		ଯଦି (_external) {
			ଯଦି (((rotation_ମୂଲ୍ୟ >= ROTATION_MAX) && (rotation_ମୂଲ୍ୟ != ROTATION_CUSTOM)) || (rotation_ମୂଲ୍ୟ < 0)) {
				// invalid rotation, resetting
				rotation_ମୂଲ୍ୟ = ROTATION_NONE;
			}

			// if CAL_MAGx_{ROLL,PITCH,YAW} manually set then CAL_MAGx_ROT needs to be ROTATION_CUSTOM
			ଯଦି ((rotation_ମୂଲ୍ୟ != ROTATION_CUSTOM)
			    && ((fabsf(euler_roll_deg) > FLT_EPSILON)
				|| (fabsf(euler_pitch_deg) > FLT_EPSILON)
				|| (fabsf(euler_yaw_deg) > FLT_EPSILON))) {

				rotation_ମୂଲ୍ୟ = ROTATION_CUSTOM;
				SetCalibrationParam(SensorString(), "ROT", _calibration_index, rotation_ମୂଲ୍ୟ);
			}

			// Handle custom specified euler angle
			ଯଦି (rotation_ମୂଲ୍ୟ == ROTATION_CUSTOM) {

				ଅଚଳ matrix::Eulerf rotation_custom_euler{
					math::radians(euler_roll_deg),
					math::radians(euler_pitch_deg),
					math::radians(euler_yaw_deg)};

				set_custom_rotation(rotation_custom_euler);

			} ଅନ୍ୟଥା {
				set_rotation(static_cast<Rotation>(rotation_ମୂଲ୍ୟ));
			}

		} ଅନ୍ୟଥା {
			// internal sensors follow board rotation
			set_rotation(GetBoardRotation());
		}

		// CAL_MAGx_PRIO
		_priority = GetCalibrationParamInt32(SensorString(), "PRIO", _calibration_index);

		ଯଦି ((_priority < 0) || (_priority > 100)) {
			// reset to default, -1 is the uninitialized parameter value
			ସ୍ଥିର constexpr int32_t CAL_PRIO_UNINITIALIZED = -1;

			ଯଦି (_priority != CAL_PRIO_UNINITIALIZED) {
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

		ଫେରନ୍ତୁ ସତ୍ୟ;
	}

	ଫେରନ୍ତୁ ମିଥ୍ୟା;
}

ଶୂନ୍ୟ Magnetometer::Reset()
{
	ଯଦି (_external) {
		set_rotation(ROTATION_NONE);

	} ଅନ୍ୟଥା {
		// internal sensors follow board rotation
		set_rotation(GetBoardRotation());
	}

	_offset.zero();
	_scale.setIdentity();

	_power_compensation.zero();
	_power = 0.f;

	_priority = _external ? DEFAULT_EXTERNAL_PRIORITY : DEFAULT_PRIORITY;

	_calibration_index = -1;

	calibration_ଗଣନା = 0;
}

ତାର୍କିକ Magnetometer::ParametersSave(ପୂର୍ଣ୍ଣସଂଖ୍ୟା desired_calibration_index, ତାର୍କିକ force)
{
	ଯଦି (force && desired_calibration_index >= 0 && desired_calibration_index < MAX_SENSOR_ଗଣନା) {
		_calibration_index = desired_calibration_index;

	} ଅନ୍ୟଥା ଯଦି (!force || (_calibration_index < 0)
		   || (desired_calibration_index != -1 && desired_calibration_index != _calibration_index)) {

		// ensure we have a valid calibration slot (matching existing or first available slot)
		int8_t calibration_index_prev = _calibration_index;
		_calibration_index = FindAvailableCalibrationIndex(SensorString(), _device_id, desired_calibration_index);

		ଯଦି (calibration_index_prev >= 0 && (calibration_index_prev != _calibration_index)) {
			PX4_WARN("%s %" PRIu32 " calibration index changed %" PRIi8 " -> %" PRIi8, SensorString(), _device_id,
				 calibration_index_prev, _calibration_index);
		}
	}

	ଯଦି (_calibration_index >= 0 && _calibration_index < MAX_SENSOR_ଗଣନା) {
		// save calibration
		ତାର୍କିକ success = ସତ୍ୟ;
		success &= SetCalibrationParam(SensorString(), "ID", _calibration_index, _device_id);
		success &= SetCalibrationParam(SensorString(), "PRIO", _calibration_index, _priority);
		success &= SetCalibrationParamsVector3f(SensorString(), "OFF", _calibration_index, _offset);

		ଅଚଳ Vector3f scale{_scale.diag()};
		success &= SetCalibrationParamsVector3f(SensorString(), "SCALE", _calibration_index, scale);

		ଅଚଳ Vector3f off_diag{_scale(0, 1), _scale(0, 2), _scale(1, 2)};
		success &= SetCalibrationParamsVector3f(SensorString(), "ODIAG", _calibration_index, off_diag);

		success &= SetCalibrationParamsVector3f(SensorString(), "COMP", _calibration_index, _power_compensation);

		ଯଦି (_external) {
			success &= SetCalibrationParam(SensorString(), "ROT", _calibration_index, (int32_t)_rotation_enum);

		} ଅନ୍ୟଥା {
			success &= SetCalibrationParam(SensorString(), "ROT", _calibration_index, -1); // internal
		}

		success &= SetCalibrationParam(SensorString(), "ROLL", _calibration_index, math::degrees(_rotation_custom_euler(0)));
		success &= SetCalibrationParam(SensorString(), "PITCH", _calibration_index, math::degrees(_rotation_custom_euler(1)));
		success &= SetCalibrationParam(SensorString(), "YAW", _calibration_index, math::degrees(_rotation_custom_euler(2)));

		ଫେରନ୍ତୁ success;
	}

	ଫେରନ୍ତୁ ମିଥ୍ୟା;
}

ଶୂନ୍ୟ Magnetometer::PrintStatus()
{
	ଯଦି (external()) {
		PX4_INFO_RAW("%s %" PRIu32
			     " EN: %d, offset: [%05.3f %05.3f %05.3f], scale: [%05.3f %05.3f %05.3f], Ext ROT: %d\n",
			     SensorString(), device_id(), enabled(),
			     (ଦ୍ୱିଗୁଣ)_offset(0), (ଦ୍ୱିଗୁଣ)_offset(1), (ଦ୍ୱିଗୁଣ)_offset(2),
			     (ଦ୍ୱିଗୁଣ)_scale(0, 0), (ଦ୍ୱିଗୁଣ)_scale(1, 1), (ଦ୍ୱିଗୁଣ)_scale(2, 2),
			     rotation_enum());

	} ଅନ୍ୟଥା {
		PX4_INFO_RAW("%s %" PRIu32
			     " EN: %d, offset: [%05.3f %05.3f %05.3f], scale: [%05.3f %05.3f %05.3f], Internal\n",
			     SensorString(), device_id(), enabled(),
			     (ଦ୍ୱିଗୁଣ)_offset(0), (ଦ୍ୱିଗୁଣ)_offset(1), (ଦ୍ୱିଗୁଣ)_offset(2),
			     (ଦ୍ୱିଗୁଣ)_scale(0, 0), (ଦ୍ୱିଗୁଣ)_scale(1, 1), (ଦ୍ୱିଗୁଣ)_scale(2, 2));
	}

#ଯଦି defined(DEBUG_BUILD)
	_scale.print();
#endif // DEBUG_BUILD
}

} // namespace calibration
