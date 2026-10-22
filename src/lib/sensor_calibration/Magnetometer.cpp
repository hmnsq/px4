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

#ضمن "Magnetometer.hpp"

#ضمن "Utilities.hpp"

#ضمن <lib/drivers/device/Device.hpp>
#ضمن <lib/parameters/param.h>

استعمال نطاق ميٽرڪس;
استعمال نطاق وقت_literals;

نطاق calibration
{

Magnetometer::Magnetometer()
{
	Reset();
}

Magnetometer::Magnetometer(uint32_t device_id)
{
	سيٽ_device_id(device_id);
}

Magnetometer::Magnetometer(uint32_t device_id, منطقي external)
{
	سيٽ_device_id(device_id, external);
}

فراغ Magnetometer::سيٽ_device_id(uint32_t device_id)
{
	سيٽ_device_id(device_id, device::device_is_external(device_id));
}

فراغ Magnetometer::سيٽ_device_id(uint32_t device_id, منطقي external)
{
	إذا (_device_id != device_id || _external != external) {

		_device_id = device_id;
		_external = external;

		Reset();

		ParametersUpdate();
		SensorCorrectionsUpdate(سچ);
	}
}

فراغ Magnetometer::SensorCorrectionsUpdate(منطقي force)
{
	// check if the selected sensor has updated
	إذا (_sensor_correction_sub.updated() || force) {

		// valid device id required
		إذا (_device_id == 0) {
			أرجع;
		}

		sensor_correction_s corrections;

		إذا (_sensor_correction_sub.copy(&corrections)) {
			// find sensor_corrections index
			لكل (صحيح i = 0; i < MAX_SENSOR_ڳڻپ; i++) {
				إذا (corrections.mag_device_ids[i] == _device_id) {
					بدل (i) {
					حالة 0:
						thermal_وٿي = Vector3f{corrections.mag_وٿي_0};
						أرجع;
					حالة 1:
						thermal_وٿي = Vector3f{corrections.mag_وٿي_1};
						أرجع;
					حالة 2:
						thermal_وٿي = Vector3f{corrections.mag_وٿي_2};
						أرجع;
					حالة 3:
						thermal_وٿي = Vector3f{corrections.mag_وٿي_3};
						أرجع;
					}
				}
			}
		}

		// zero thermal offset if not found
		thermal_وٿي.zero();
	}
}

منطقي Magnetometer::سيٽ_وٿي(ثابت Vector3f &وٿي)
{
	إذا (Vector3f(وٿي_2 - وٿي).longerThan(0.005f)) {
		إذا (وٿي.isAllFinite()) {
			وٿي_2 = وٿي;
			calibration_ڳڻپ++;
			أرجع سچ;
		}
	}

	أرجع خطأ;
}

منطقي Magnetometer::سيٽ_scale(ثابت Vector3f &scale)
{
	إذا (Vector3f(_scale.diag() - scale).longerThan(0.01f)) {
		إذا (scale.isAllFinite() && (scale(0) > 0.f) && (scale(1) > 0.f) && (scale(2) > 0.f)) {
			_scale(0, 0) = scale(0);
			_scale(1, 1) = scale(1);
			_scale(2, 2) = scale(2);

			calibration_ڳڻپ++;
			أرجع سچ;
		}
	}

	أرجع خطأ;
}

منطقي Magnetometer::سيٽ_offdiagonal(ثابت Vector3f &offdiagonal)
{
	إذا (Vector3f(Vector3f{_scale(0, 1), _scale(0, 2), _scale(1, 2)} - offdiagonal).longerThan(0.01f)) {
		إذا (offdiagonal.isAllFinite()) {

			_scale(0, 1) = offdiagonal(0);
			_scale(1, 0) = offdiagonal(0);

			_scale(0, 2) = offdiagonal(1);
			_scale(2, 0) = offdiagonal(1);

			_scale(1, 2) = offdiagonal(2);
			_scale(2, 1) = offdiagonal(2);

			calibration_ڳڻپ++;
			أرجع سچ;
		}
	}

	أرجع خطأ;
}

فراغ Magnetometer::سيٽ_rotation(ثابت Rotation rotation)
{
	إذا (rotation < ROTATION_MAX) {
		_rotation_enum = rotation;

	} وإلا {
		// invalid rotation, resetting
		_rotation_enum = ROTATION_NONE;
	}

	// always apply level adjustments
	_rotation = Dcmf(GetSensorLevelAdjustment()) * get_rot_ميٽرڪس(_rotation_enum);

	// clear any custom rotation
	_rotation_custom_euler.zero();
}

فراغ Magnetometer::سيٽ_custom_rotation(ثابت Eulerf &rotation)
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

منطقي Magnetometer::سيٽ_calibration_اشاريو(صحيح calibration_اشاريو)
{
	إذا ((calibration_اشاريو >= 0) && (calibration_اشاريو < MAX_SENSOR_ڳڻپ)) {
		calibration_اشاريو_2 = calibration_اشاريو;
		أرجع سچ;
	}

	أرجع خطأ;
}

فراغ Magnetometer::ParametersUpdate()
{
	إذا (_device_id == 0) {
		أرجع;
	}

	calibration_اشاريو_2 = ڳوليو_Current_Calibration_اشاريو(SensorString(), _device_id);

	إذا (calibration_اشاريو_2 == -1) {
		// no saved calibration available
		Reset();

	} وإلا {
		Parameters_ڀريو();
	}
}

منطقي Magnetometer::Parameters_ڀريو()
{
	إذا (calibration_اشاريو_2 >= 0 && calibration_اشاريو_2 < MAX_SENSOR_ڳڻپ) {
		// CAL_MAGx_ROT
		int32_t rotation_قدر = GetCalibrationParamInt32(SensorString(), "ROT", calibration_اشاريو_2);

		ثابت عائم euler_roll_deg = GetCalibrationParamFloat(SensorString(), "ROLL", calibration_اشاريو_2);
		ثابت عائم euler_pitch_deg = GetCalibrationParamFloat(SensorString(), "PITCH", calibration_اشاريو_2);
		ثابت عائم euler_yaw_deg = GetCalibrationParamFloat(SensorString(), "YAW", calibration_اشاريو_2);

		إذا (_external) {
			إذا (((rotation_قدر >= ROTATION_MAX) && (rotation_قدر != ROTATION_CUSTOM)) || (rotation_قدر < 0)) {
				// invalid rotation, resetting
				rotation_قدر = ROTATION_NONE;
			}

			// if CAL_MAGx_{ROLL,PITCH,YAW} manually set then CAL_MAGx_ROT needs to be ROTATION_CUSTOM
			إذا ((rotation_قدر != ROTATION_CUSTOM)
			    && ((fabsf(euler_roll_deg) > FLT_EPSILON)
				|| (fabsf(euler_pitch_deg) > FLT_EPSILON)
				|| (fabsf(euler_yaw_deg) > FLT_EPSILON))) {

				rotation_قدر = ROTATION_CUSTOM;
				سيٽ_Calibration_Param(SensorString(), "ROT", calibration_اشاريو_2, rotation_قدر);
			}

			// Handle custom specified euler angle
			إذا (rotation_قدر == ROTATION_CUSTOM) {

				ثابت ميٽرڪس::Eulerf rotation_custom_euler{
					math::radians(euler_roll_deg),
					math::radians(euler_pitch_deg),
					math::radians(euler_yaw_deg)};

				سيٽ_custom_rotation(rotation_custom_euler);

			} وإلا {
				سيٽ_rotation(static_cast<Rotation>(rotation_قدر));
			}

		} وإلا {
			// internal sensors follow board rotation
			سيٽ_rotation(GetBoardRotation());
		}

		// CAL_MAGx_PRIO
		ترجيح_2 = GetCalibrationParamInt32(SensorString(), "PRIO", calibration_اشاريو_2);

		إذا ((ترجيح_2 < 0) || (ترجيح_2 > 100)) {
			// reset to default, -1 is the uninitialized parameter value
			ساكن constexpr int32_t CAL_PRIO_UNINITIALIZED = -1;

			إذا (ترجيح_2 != CAL_PRIO_UNINITIALIZED) {
				PX4_ERR("%s %" PRIu32 " (%" PRId8 ") invalid priority %" PRId32 ", resetting", SensorString(), _device_id,
					calibration_اشاريو_2, ترجيح_2);

				سيٽ_Calibration_Param(SensorString(), "PRIO", calibration_اشاريو_2, CAL_PRIO_UNINITIALIZED);
			}

			ترجيح_2 = _external ? DEFAULT_EXTERNAL_ترجيح : DEFAULT_ترجيح;
		}

		// CAL_MAGx_OFF{X,Y,Z}
		سيٽ_وٿي(GetCalibrationParamsVector3f(SensorString(), "OFF", calibration_اشاريو_2));

		// CAL_MAGx_SCALE{X,Y,Z}
		سيٽ_scale(GetCalibrationParamsVector3f(SensorString(), "SCALE", calibration_اشاريو_2));

		// CAL_MAGx_ODIAG{X,Y,Z}
		سيٽ_offdiagonal(GetCalibrationParamsVector3f(SensorString(), "ODIAG", calibration_اشاريو_2));

		// CAL_MAGx_COMP{X,Y,Z}
		_power_compensation = GetCalibrationParamsVector3f(SensorString(), "COMP", calibration_اشاريو_2);

		أرجع سچ;
	}

	أرجع خطأ;
}

فراغ Magnetometer::Reset()
{
	إذا (_external) {
		سيٽ_rotation(ROTATION_NONE);

	} وإلا {
		// internal sensors follow board rotation
		سيٽ_rotation(GetBoardRotation());
	}

	وٿي_2.zero();
	_scale.سيٽ_سڃاڻپ_عنصر();

	_power_compensation.zero();
	_power = 0.f;

	ترجيح_2 = _external ? DEFAULT_EXTERNAL_ترجيح : DEFAULT_ترجيح;

	calibration_اشاريو_2 = -1;

	calibration_ڳڻپ = 0;
}

منطقي Magnetometer::ParametersSave(صحيح desired_calibration_اشاريو, منطقي force)
{
	إذا (force && desired_calibration_اشاريو >= 0 && desired_calibration_اشاريو < MAX_SENSOR_ڳڻپ) {
		calibration_اشاريو_2 = desired_calibration_اشاريو;

	} وإلا إذا (!force || (calibration_اشاريو_2 < 0)
		   || (desired_calibration_اشاريو != -1 && desired_calibration_اشاريو != calibration_اشاريو_2)) {

		// ensure we have a valid calibration slot (matching existing or first available slot)
		int8_t calibration_اشاريو_prev = calibration_اشاريو_2;
		calibration_اشاريو_2 = ڳوليو_موجود_Calibration_اشاريو(SensorString(), _device_id, desired_calibration_اشاريو);

		إذا (calibration_اشاريو_prev >= 0 && (calibration_اشاريو_prev != calibration_اشاريو_2)) {
			PX4_WARN("%s %" PRIu32 " calibration index changed %" PRIi8 " -> %" PRIi8, SensorString(), _device_id,
				 calibration_اشاريو_prev, calibration_اشاريو_2);
		}
	}

	إذا (calibration_اشاريو_2 >= 0 && calibration_اشاريو_2 < MAX_SENSOR_ڳڻپ) {
		// save calibration
		منطقي success = سچ;
		success &= سيٽ_Calibration_Param(SensorString(), "ID", calibration_اشاريو_2, _device_id);
		success &= سيٽ_Calibration_Param(SensorString(), "PRIO", calibration_اشاريو_2, ترجيح_2);
		success &= سيٽ_Calibration_Params_Vector3f(SensorString(), "OFF", calibration_اشاريو_2, وٿي_2);

		ثابت Vector3f scale{_scale.diag()};
		success &= سيٽ_Calibration_Params_Vector3f(SensorString(), "SCALE", calibration_اشاريو_2, scale);

		ثابت Vector3f off_diag{_scale(0, 1), _scale(0, 2), _scale(1, 2)};
		success &= سيٽ_Calibration_Params_Vector3f(SensorString(), "ODIAG", calibration_اشاريو_2, off_diag);

		success &= سيٽ_Calibration_Params_Vector3f(SensorString(), "COMP", calibration_اشاريو_2, _power_compensation);

		إذا (_external) {
			success &= سيٽ_Calibration_Param(SensorString(), "ROT", calibration_اشاريو_2, (int32_t)_rotation_enum);

		} وإلا {
			success &= سيٽ_Calibration_Param(SensorString(), "ROT", calibration_اشاريو_2, -1); //اندروني
		}

		success &= سيٽ_Calibration_Param(SensorString(), "ROLL", calibration_اشاريو_2, math::degrees(_rotation_custom_euler(0)));
		success &= سيٽ_Calibration_Param(SensorString(), "PITCH", calibration_اشاريو_2, math::degrees(_rotation_custom_euler(1)));
		success &= سيٽ_Calibration_Param(SensorString(), "YAW", calibration_اشاريو_2, math::degrees(_rotation_custom_euler(2)));

		أرجع success;
	}

	أرجع خطأ;
}

فراغ Magnetometer::Print_حالت()
{
	إذا (external()) {
		PX4_INFO_RAW("%s %" PRIu32
			     " EN: %d, offset: [%05.3f %05.3f %05.3f], scale: [%05.3f %05.3f %05.3f], Ext ROT: %d\n",
			     SensorString(), device_id(), enabled(),
			     (مزدوج)وٿي_2(0), (مزدوج)وٿي_2(1), (مزدوج)وٿي_2(2),
			     (مزدوج)_scale(0, 0), (مزدوج)_scale(1, 1), (مزدوج)_scale(2, 2),
			     rotation_enum());

	} وإلا {
		PX4_INFO_RAW("%s %" PRIu32
			     " EN: %d, offset: [%05.3f %05.3f %05.3f], scale: [%05.3f %05.3f %05.3f], Internal\n",
			     SensorString(), device_id(), enabled(),
			     (مزدوج)وٿي_2(0), (مزدوج)وٿي_2(1), (مزدوج)وٿي_2(2),
			     (مزدوج)_scale(0, 0), (مزدوج)_scale(1, 1), (مزدوج)_scale(2, 2));
	}

#إذا defined(DEBUG_BUILD)
	_scale.print();
#جيڪڏهن_پورو // DEBUG_BUILD
}

} // namespace calibration
