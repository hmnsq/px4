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

#ਅਨ੍ਤਰ੍ਭਾਵ "Magnetometer.hpp"

#ਅਨ੍ਤਰ੍ਭਾਵ "Utilities.hpp"

#ਅਨ੍ਤਰ੍ਭਾਵ <lib/drivers/device/Device.hpp>
#ਅਨ੍ਤਰ੍ਭਾਵ <lib/parameters/param.h>

ਵਰਤੋਂ ਨਾਮਾਕਾਸ਼ ਮੈਟ੍ਰਿਕਸ;
ਵਰਤੋਂ ਨਾਮਾਕਾਸ਼ ਸਮਾਂ_literals;

ਨਾਮਾਕਾਸ਼ calibration
{

Magnetometer::Magnetometer()
{
	Reset();
}

Magnetometer::Magnetometer(uint32_t device_id)
{
	ਸੈੱਟ_device_id(device_id);
}

Magnetometer::Magnetometer(uint32_t device_id, ਤਾਰਕਿਕ external)
{
	ਸੈੱਟ_device_id(device_id, external);
}

ਖਾਲੀ Magnetometer::ਸੈੱਟ_device_id(uint32_t device_id)
{
	ਸੈੱਟ_device_id(device_id, device::device_is_external(device_id));
}

ਖਾਲੀ Magnetometer::ਸੈੱਟ_device_id(uint32_t device_id, ਤਾਰਕਿਕ external)
{
	ਜੇ (_device_id != device_id || _external != external) {

		_device_id = device_id;
		_external = external;

		Reset();

		ParametersUpdate();
		SensorCorrectionsUpdate(ਸੱਚ);
	}
}

ਖਾਲੀ Magnetometer::SensorCorrectionsUpdate(ਤਾਰਕਿਕ force)
{
	// check if the selected sensor has updated
	ਜੇ (_sensor_correction_sub.updated() || force) {

		// valid device id required
		ਜੇ (_device_id == 0) {
			ਵਾਪਸ;
		}

		sensor_correction_s corrections;

		ਜੇ (_sensor_correction_sub.copy(&corrections)) {
			// find sensor_corrections index
			ਲਈ (ਪੂਰਨ i = 0; i < MAX_SENSOR_ਗਿਣਤੀ; i++) {
				ਜੇ (corrections.mag_device_ids[i] == _device_id) {
					ਚੋਣ (i) {
					ਹਾਲਤ 0:
						thermal_ਵਿੱਥ = Vector3f{corrections.mag_ਵਿੱਥ_0};
						ਵਾਪਸ;
					ਹਾਲਤ 1:
						thermal_ਵਿੱਥ = Vector3f{corrections.mag_ਵਿੱਥ_1};
						ਵਾਪਸ;
					ਹਾਲਤ 2:
						thermal_ਵਿੱਥ = Vector3f{corrections.mag_ਵਿੱਥ_2};
						ਵਾਪਸ;
					ਹਾਲਤ 3:
						thermal_ਵਿੱਥ = Vector3f{corrections.mag_ਵਿੱਥ_3};
						ਵਾਪਸ;
					}
				}
			}
		}

		// zero thermal offset if not found
		thermal_ਵਿੱਥ.zero();
	}
}

ਤਾਰਕਿਕ Magnetometer::ਸੈੱਟ_ਵਿੱਥ(ਅਚਲ Vector3f &ਵਿੱਥ)
{
	ਜੇ (Vector3f(ਵਿੱਥ_2 - ਵਿੱਥ).longerThan(0.005f)) {
		ਜੇ (ਵਿੱਥ.isAllFinite()) {
			ਵਿੱਥ_2 = ਵਿੱਥ;
			calibration_ਗਿਣਤੀ++;
			ਵਾਪਸ ਸੱਚ;
		}
	}

	ਵਾਪਸ ਝੂਠ;
}

ਤਾਰਕਿਕ Magnetometer::ਸੈੱਟ_scale(ਅਚਲ Vector3f &scale)
{
	ਜੇ (Vector3f(_scale.diag() - scale).longerThan(0.01f)) {
		ਜੇ (scale.isAllFinite() && (scale(0) > 0.f) && (scale(1) > 0.f) && (scale(2) > 0.f)) {
			_scale(0, 0) = scale(0);
			_scale(1, 1) = scale(1);
			_scale(2, 2) = scale(2);

			calibration_ਗਿਣਤੀ++;
			ਵਾਪਸ ਸੱਚ;
		}
	}

	ਵਾਪਸ ਝੂਠ;
}

ਤਾਰਕਿਕ Magnetometer::ਸੈੱਟ_offdiagonal(ਅਚਲ Vector3f &offdiagonal)
{
	ਜੇ (Vector3f(Vector3f{_scale(0, 1), _scale(0, 2), _scale(1, 2)} - offdiagonal).longerThan(0.01f)) {
		ਜੇ (offdiagonal.isAllFinite()) {

			_scale(0, 1) = offdiagonal(0);
			_scale(1, 0) = offdiagonal(0);

			_scale(0, 2) = offdiagonal(1);
			_scale(2, 0) = offdiagonal(1);

			_scale(1, 2) = offdiagonal(2);
			_scale(2, 1) = offdiagonal(2);

			calibration_ਗਿਣਤੀ++;
			ਵਾਪਸ ਸੱਚ;
		}
	}

	ਵਾਪਸ ਝੂਠ;
}

ਖਾਲੀ Magnetometer::ਸੈੱਟ_rotation(ਅਚਲ Rotation rotation)
{
	ਜੇ (rotation < ROTATION_MAX) {
		_rotation_enum = rotation;

	} ਨਹੀਂਤਾਂ {
		// invalid rotation, resetting
		_rotation_enum = ROTATION_NONE;
	}

	// always apply level adjustments
	_rotation = Dcmf(GetSensorLevelAdjustment()) * get_rot_ਮੈਟ੍ਰਿਕਸ(_rotation_enum);

	// clear any custom rotation
	_rotation_custom_euler.zero();
}

ਖਾਲੀ Magnetometer::ਸੈੱਟ_custom_rotation(ਅਚਲ Eulerf &rotation)
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

ਤਾਰਕਿਕ Magnetometer::ਸੈੱਟ_calibration_ਸੂਚਕਾਂਕ(ਪੂਰਨ calibration_ਸੂਚਕਾਂਕ)
{
	ਜੇ ((calibration_ਸੂਚਕਾਂਕ >= 0) && (calibration_ਸੂਚਕਾਂਕ < MAX_SENSOR_ਗਿਣਤੀ)) {
		calibration_ਸੂਚਕਾਂਕ_2 = calibration_ਸੂਚਕਾਂਕ;
		ਵਾਪਸ ਸੱਚ;
	}

	ਵਾਪਸ ਝੂਠ;
}

ਖਾਲੀ Magnetometer::ParametersUpdate()
{
	ਜੇ (_device_id == 0) {
		ਵਾਪਸ;
	}

	calibration_ਸੂਚਕਾਂਕ_2 = ਲੱਭੋ_Current_Calibration_ਸੂਚਕਾਂਕ(SensorString(), _device_id);

	ਜੇ (calibration_ਸੂਚਕਾਂਕ_2 == -1) {
		// no saved calibration available
		Reset();

	} ਨਹੀਂਤਾਂ {
		Parameters_ਲੱਦੋ();
	}
}

ਤਾਰਕਿਕ Magnetometer::Parameters_ਲੱਦੋ()
{
	ਜੇ (calibration_ਸੂਚਕਾਂਕ_2 >= 0 && calibration_ਸੂਚਕਾਂਕ_2 < MAX_SENSOR_ਗਿਣਤੀ) {
		// CAL_MAGx_ROT
		int32_t rotation_ਮੁੱਲ = GetCalibrationParamInt32(SensorString(), "ROT", calibration_ਸੂਚਕਾਂਕ_2);

		ਅਚਲ ਦਸ਼ਮਲਵ euler_roll_deg = GetCalibrationParamFloat(SensorString(), "ROLL", calibration_ਸੂਚਕਾਂਕ_2);
		ਅਚਲ ਦਸ਼ਮਲਵ euler_pitch_deg = GetCalibrationParamFloat(SensorString(), "PITCH", calibration_ਸੂਚਕਾਂਕ_2);
		ਅਚਲ ਦਸ਼ਮਲਵ euler_yaw_deg = GetCalibrationParamFloat(SensorString(), "YAW", calibration_ਸੂਚਕਾਂਕ_2);

		ਜੇ (_external) {
			ਜੇ (((rotation_ਮੁੱਲ >= ROTATION_MAX) && (rotation_ਮੁੱਲ != ROTATION_CUSTOM)) || (rotation_ਮੁੱਲ < 0)) {
				// invalid rotation, resetting
				rotation_ਮੁੱਲ = ROTATION_NONE;
			}

			// if CAL_MAGx_{ROLL,PITCH,YAW} manually set then CAL_MAGx_ROT needs to be ROTATION_CUSTOM
			ਜੇ ((rotation_ਮੁੱਲ != ROTATION_CUSTOM)
			    && ((fabsf(euler_roll_deg) > FLT_EPSILON)
				|| (fabsf(euler_pitch_deg) > FLT_EPSILON)
				|| (fabsf(euler_yaw_deg) > FLT_EPSILON))) {

				rotation_ਮੁੱਲ = ROTATION_CUSTOM;
				ਸੈੱਟ_Calibration_Param(SensorString(), "ROT", calibration_ਸੂਚਕਾਂਕ_2, rotation_ਮੁੱਲ);
			}

			// Handle custom specified euler angle
			ਜੇ (rotation_ਮੁੱਲ == ROTATION_CUSTOM) {

				ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Eulerf rotation_custom_euler{
					math::radians(euler_roll_deg),
					math::radians(euler_pitch_deg),
					math::radians(euler_yaw_deg)};

				ਸੈੱਟ_custom_rotation(rotation_custom_euler);

			} ਨਹੀਂਤਾਂ {
				ਸੈੱਟ_rotation(static_cast<Rotation>(rotation_ਮੁੱਲ));
			}

		} ਨਹੀਂਤਾਂ {
			// internal sensors follow board rotation
			ਸੈੱਟ_rotation(GetBoardRotation());
		}

		// CAL_MAGx_PRIO
		ਤਰਜੀਹ_2 = GetCalibrationParamInt32(SensorString(), "PRIO", calibration_ਸੂਚਕਾਂਕ_2);

		ਜੇ ((ਤਰਜੀਹ_2 < 0) || (ਤਰਜੀਹ_2 > 100)) {
			// reset to default, -1 is the uninitialized parameter value
			ਸਥਿਰ constexpr int32_t CAL_PRIO_UNINITIALIZED = -1;

			ਜੇ (ਤਰਜੀਹ_2 != CAL_PRIO_UNINITIALIZED) {
				PX4_ERR("%s %" PRIu32 " (%" PRId8 ") invalid priority %" PRId32 ", resetting", SensorString(), _device_id,
					calibration_ਸੂਚਕਾਂਕ_2, ਤਰਜੀਹ_2);

				ਸੈੱਟ_Calibration_Param(SensorString(), "PRIO", calibration_ਸੂਚਕਾਂਕ_2, CAL_PRIO_UNINITIALIZED);
			}

			ਤਰਜੀਹ_2 = _external ? DEFAULT_EXTERNAL_ਤਰਜੀਹ : DEFAULT_ਤਰਜੀਹ;
		}

		// CAL_MAGx_OFF{X,Y,Z}
		ਸੈੱਟ_ਵਿੱਥ(GetCalibrationParamsVector3f(SensorString(), "OFF", calibration_ਸੂਚਕਾਂਕ_2));

		// CAL_MAGx_SCALE{X,Y,Z}
		ਸੈੱਟ_scale(GetCalibrationParamsVector3f(SensorString(), "SCALE", calibration_ਸੂਚਕਾਂਕ_2));

		// CAL_MAGx_ODIAG{X,Y,Z}
		ਸੈੱਟ_offdiagonal(GetCalibrationParamsVector3f(SensorString(), "ODIAG", calibration_ਸੂਚਕਾਂਕ_2));

		// CAL_MAGx_COMP{X,Y,Z}
		_power_compensation = GetCalibrationParamsVector3f(SensorString(), "COMP", calibration_ਸੂਚਕਾਂਕ_2);

		ਵਾਪਸ ਸੱਚ;
	}

	ਵਾਪਸ ਝੂਠ;
}

ਖਾਲੀ Magnetometer::Reset()
{
	ਜੇ (_external) {
		ਸੈੱਟ_rotation(ROTATION_NONE);

	} ਨਹੀਂਤਾਂ {
		// internal sensors follow board rotation
		ਸੈੱਟ_rotation(GetBoardRotation());
	}

	ਵਿੱਥ_2.zero();
	_scale.ਸੈੱਟ_ਪਛਾਣ_ਤੱਤ();

	_power_compensation.zero();
	_power = 0.f;

	ਤਰਜੀਹ_2 = _external ? DEFAULT_EXTERNAL_ਤਰਜੀਹ : DEFAULT_ਤਰਜੀਹ;

	calibration_ਸੂਚਕਾਂਕ_2 = -1;

	calibration_ਗਿਣਤੀ = 0;
}

ਤਾਰਕਿਕ Magnetometer::ParametersSave(ਪੂਰਨ desired_calibration_ਸੂਚਕਾਂਕ, ਤਾਰਕਿਕ force)
{
	ਜੇ (force && desired_calibration_ਸੂਚਕਾਂਕ >= 0 && desired_calibration_ਸੂਚਕਾਂਕ < MAX_SENSOR_ਗਿਣਤੀ) {
		calibration_ਸੂਚਕਾਂਕ_2 = desired_calibration_ਸੂਚਕਾਂਕ;

	} ਨਹੀਂਤਾਂ ਜੇ (!force || (calibration_ਸੂਚਕਾਂਕ_2 < 0)
		   || (desired_calibration_ਸੂਚਕਾਂਕ != -1 && desired_calibration_ਸੂਚਕਾਂਕ != calibration_ਸੂਚਕਾਂਕ_2)) {

		// ensure we have a valid calibration slot (matching existing or first available slot)
		int8_t calibration_ਸੂਚਕਾਂਕ_prev = calibration_ਸੂਚਕਾਂਕ_2;
		calibration_ਸੂਚਕਾਂਕ_2 = ਲੱਭੋ_ਉਪਲਬਧ_Calibration_ਸੂਚਕਾਂਕ(SensorString(), _device_id, desired_calibration_ਸੂਚਕਾਂਕ);

		ਜੇ (calibration_ਸੂਚਕਾਂਕ_prev >= 0 && (calibration_ਸੂਚਕਾਂਕ_prev != calibration_ਸੂਚਕਾਂਕ_2)) {
			PX4_WARN("%s %" PRIu32 " calibration index changed %" PRIi8 " -> %" PRIi8, SensorString(), _device_id,
				 calibration_ਸੂਚਕਾਂਕ_prev, calibration_ਸੂਚਕਾਂਕ_2);
		}
	}

	ਜੇ (calibration_ਸੂਚਕਾਂਕ_2 >= 0 && calibration_ਸੂਚਕਾਂਕ_2 < MAX_SENSOR_ਗਿਣਤੀ) {
		// save calibration
		ਤਾਰਕਿਕ success = ਸੱਚ;
		success &= ਸੈੱਟ_Calibration_Param(SensorString(), "ID", calibration_ਸੂਚਕਾਂਕ_2, _device_id);
		success &= ਸੈੱਟ_Calibration_Param(SensorString(), "PRIO", calibration_ਸੂਚਕਾਂਕ_2, ਤਰਜੀਹ_2);
		success &= ਸੈੱਟ_Calibration_Params_Vector3f(SensorString(), "OFF", calibration_ਸੂਚਕਾਂਕ_2, ਵਿੱਥ_2);

		ਅਚਲ Vector3f scale{_scale.diag()};
		success &= ਸੈੱਟ_Calibration_Params_Vector3f(SensorString(), "SCALE", calibration_ਸੂਚਕਾਂਕ_2, scale);

		ਅਚਲ Vector3f off_diag{_scale(0, 1), _scale(0, 2), _scale(1, 2)};
		success &= ਸੈੱਟ_Calibration_Params_Vector3f(SensorString(), "ODIAG", calibration_ਸੂਚਕਾਂਕ_2, off_diag);

		success &= ਸੈੱਟ_Calibration_Params_Vector3f(SensorString(), "COMP", calibration_ਸੂਚਕਾਂਕ_2, _power_compensation);

		ਜੇ (_external) {
			success &= ਸੈੱਟ_Calibration_Param(SensorString(), "ROT", calibration_ਸੂਚਕਾਂਕ_2, (int32_t)_rotation_enum);

		} ਨਹੀਂਤਾਂ {
			success &= ਸੈੱਟ_Calibration_Param(SensorString(), "ROT", calibration_ਸੂਚਕਾਂਕ_2, -1); //ਅੰਦਰੂਨੀ
		}

		success &= ਸੈੱਟ_Calibration_Param(SensorString(), "ROLL", calibration_ਸੂਚਕਾਂਕ_2, math::degrees(_rotation_custom_euler(0)));
		success &= ਸੈੱਟ_Calibration_Param(SensorString(), "PITCH", calibration_ਸੂਚਕਾਂਕ_2, math::degrees(_rotation_custom_euler(1)));
		success &= ਸੈੱਟ_Calibration_Param(SensorString(), "YAW", calibration_ਸੂਚਕਾਂਕ_2, math::degrees(_rotation_custom_euler(2)));

		ਵਾਪਸ success;
	}

	ਵਾਪਸ ਝੂਠ;
}

ਖਾਲੀ Magnetometer::Print_ਸਥਿਤੀ()
{
	ਜੇ (external()) {
		PX4_INFO_RAW("%s %" PRIu32
			     " EN: %d, offset: [%05.3f %05.3f %05.3f], scale: [%05.3f %05.3f %05.3f], Ext ROT: %d\n",
			     SensorString(), device_id(), enabled(),
			     (ਦੁੱਗਣਾ)ਵਿੱਥ_2(0), (ਦੁੱਗਣਾ)ਵਿੱਥ_2(1), (ਦੁੱਗਣਾ)ਵਿੱਥ_2(2),
			     (ਦੁੱਗਣਾ)_scale(0, 0), (ਦੁੱਗਣਾ)_scale(1, 1), (ਦੁੱਗਣਾ)_scale(2, 2),
			     rotation_enum());

	} ਨਹੀਂਤਾਂ {
		PX4_INFO_RAW("%s %" PRIu32
			     " EN: %d, offset: [%05.3f %05.3f %05.3f], scale: [%05.3f %05.3f %05.3f], Internal\n",
			     SensorString(), device_id(), enabled(),
			     (ਦੁੱਗਣਾ)ਵਿੱਥ_2(0), (ਦੁੱਗਣਾ)ਵਿੱਥ_2(1), (ਦੁੱਗਣਾ)ਵਿੱਥ_2(2),
			     (ਦੁੱਗਣਾ)_scale(0, 0), (ਦੁੱਗਣਾ)_scale(1, 1), (ਦੁੱਗਣਾ)_scale(2, 2));
	}

#ਜੇ defined(DEBUG_BUILD)
	_scale.print();
#ਜੇ_ਅੰਤ // DEBUG_BUILD
}

} // namespace calibration
