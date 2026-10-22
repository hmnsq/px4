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

استفاده نطاق ماتریس;
استفاده نطاق زمان_literals;

نطاق calibration
{

Magnetometer::Magnetometer()
{
	Reset();
}

Magnetometer::Magnetometer(uint32_t device_id)
{
	مجموعه_device_id(device_id);
}

Magnetometer::Magnetometer(uint32_t device_id, منطقي external)
{
	مجموعه_device_id(device_id, external);
}

فراغ Magnetometer::مجموعه_device_id(uint32_t device_id)
{
	مجموعه_device_id(device_id, device::device_is_external(device_id));
}

فراغ Magnetometer::مجموعه_device_id(uint32_t device_id, منطقي external)
{
	إذا (_device_id != device_id || _external != external) {

		_device_id = device_id;
		_external = external;

		Reset();

		ParametersUpdate();
		SensorCorrectionsUpdate(درست);
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
			لكل (صحيح i = 0; i < MAX_SENSOR_شمار; i++) {
				إذا (corrections.mag_device_ids[i] == _device_id) {
					بدل (i) {
					حالة 0:
						thermal_جابه_جایی = Vector3f{corrections.mag_جابه_جایی_0};
						أرجع;
					حالة 1:
						thermal_جابه_جایی = Vector3f{corrections.mag_جابه_جایی_1};
						أرجع;
					حالة 2:
						thermal_جابه_جایی = Vector3f{corrections.mag_جابه_جایی_2};
						أرجع;
					حالة 3:
						thermal_جابه_جایی = Vector3f{corrections.mag_جابه_جایی_3};
						أرجع;
					}
				}
			}
		}

		// zero thermal offset if not found
		thermal_جابه_جایی.zero();
	}
}

منطقي Magnetometer::مجموعه_جابه_جایی(ثابت Vector3f &جابه_جایی)
{
	إذا (Vector3f(جابه_جایی_2 - جابه_جایی).longerThan(0.005f)) {
		إذا (جابه_جایی.isAllFinite()) {
			جابه_جایی_2 = جابه_جایی;
			calibration_شمار++;
			أرجع درست;
		}
	}

	أرجع خطأ;
}

منطقي Magnetometer::مجموعه_scale(ثابت Vector3f &scale)
{
	إذا (Vector3f(_scale.diag() - scale).longerThan(0.01f)) {
		إذا (scale.isAllFinite() && (scale(0) > 0.f) && (scale(1) > 0.f) && (scale(2) > 0.f)) {
			_scale(0, 0) = scale(0);
			_scale(1, 1) = scale(1);
			_scale(2, 2) = scale(2);

			calibration_شمار++;
			أرجع درست;
		}
	}

	أرجع خطأ;
}

منطقي Magnetometer::مجموعه_offdiagonal(ثابت Vector3f &offdiagonal)
{
	إذا (Vector3f(Vector3f{_scale(0, 1), _scale(0, 2), _scale(1, 2)} - offdiagonal).longerThan(0.01f)) {
		إذا (offdiagonal.isAllFinite()) {

			_scale(0, 1) = offdiagonal(0);
			_scale(1, 0) = offdiagonal(0);

			_scale(0, 2) = offdiagonal(1);
			_scale(2, 0) = offdiagonal(1);

			_scale(1, 2) = offdiagonal(2);
			_scale(2, 1) = offdiagonal(2);

			calibration_شمار++;
			أرجع درست;
		}
	}

	أرجع خطأ;
}

فراغ Magnetometer::مجموعه_rotation(ثابت Rotation rotation)
{
	إذا (rotation < ROTATION_MAX) {
		_rotation_enum = rotation;

	} وإلا {
		// invalid rotation, resetting
		_rotation_enum = ROTATION_NONE;
	}

	// always apply level adjustments
	_rotation = Dcmf(GetSensorLevelAdjustment()) * get_rot_ماتریس(_rotation_enum);

	// clear any custom rotation
	_rotation_custom_euler.zero();
}

فراغ Magnetometer::مجموعه_custom_rotation(ثابت Eulerf &rotation)
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

منطقي Magnetometer::مجموعه_calibration_شاخص(صحيح calibration_شاخص)
{
	إذا ((calibration_شاخص >= 0) && (calibration_شاخص < MAX_SENSOR_شمار)) {
		calibration_شاخص_2 = calibration_شاخص;
		أرجع درست;
	}

	أرجع خطأ;
}

فراغ Magnetometer::ParametersUpdate()
{
	إذا (_device_id == 0) {
		أرجع;
	}

	calibration_شاخص_2 = پیدا_کن_Current_Calibration_شاخص(SensorString(), _device_id);

	إذا (calibration_شاخص_2 == -1) {
		// no saved calibration available
		Reset();

	} وإلا {
		Parameters_بارگیری();
	}
}

منطقي Magnetometer::Parameters_بارگیری()
{
	إذا (calibration_شاخص_2 >= 0 && calibration_شاخص_2 < MAX_SENSOR_شمار) {
		// CAL_MAGx_ROT
		int32_t rotation_مقدار = GetCalibrationParamInt32(SensorString(), "ROT", calibration_شاخص_2);

		ثابت عائم euler_roll_deg = GetCalibrationParamFloat(SensorString(), "ROLL", calibration_شاخص_2);
		ثابت عائم euler_pitch_deg = GetCalibrationParamFloat(SensorString(), "PITCH", calibration_شاخص_2);
		ثابت عائم euler_yaw_deg = GetCalibrationParamFloat(SensorString(), "YAW", calibration_شاخص_2);

		إذا (_external) {
			إذا (((rotation_مقدار >= ROTATION_MAX) && (rotation_مقدار != ROTATION_CUSTOM)) || (rotation_مقدار < 0)) {
				// invalid rotation, resetting
				rotation_مقدار = ROTATION_NONE;
			}

			// if CAL_MAGx_{ROLL,PITCH,YAW} manually set then CAL_MAGx_ROT needs to be ROTATION_CUSTOM
			إذا ((rotation_مقدار != ROTATION_CUSTOM)
			    && ((fabsf(euler_roll_deg) > FLT_EPSILON)
				|| (fabsf(euler_pitch_deg) > FLT_EPSILON)
				|| (fabsf(euler_yaw_deg) > FLT_EPSILON))) {

				rotation_مقدار = ROTATION_CUSTOM;
				مجموعه_Calibration_Param(SensorString(), "ROT", calibration_شاخص_2, rotation_مقدار);
			}

			// Handle custom specified euler angle
			إذا (rotation_مقدار == ROTATION_CUSTOM) {

				ثابت ماتریس::Eulerf rotation_custom_euler{
					math::radians(euler_roll_deg),
					math::radians(euler_pitch_deg),
					math::radians(euler_yaw_deg)};

				مجموعه_custom_rotation(rotation_custom_euler);

			} وإلا {
				مجموعه_rotation(static_cast<Rotation>(rotation_مقدار));
			}

		} وإلا {
			// internal sensors follow board rotation
			مجموعه_rotation(GetBoardRotation());
		}

		// CAL_MAGx_PRIO
		اولویت_2 = GetCalibrationParamInt32(SensorString(), "PRIO", calibration_شاخص_2);

		إذا ((اولویت_2 < 0) || (اولویت_2 > 100)) {
			// reset to default, -1 is the uninitialized parameter value
			ساكن constexpr int32_t CAL_PRIO_UNINITIALIZED = -1;

			إذا (اولویت_2 != CAL_PRIO_UNINITIALIZED) {
				PX4_ERR("%s %" PRIu32 " (%" PRId8 ") invalid priority %" PRId32 ", resetting", SensorString(), _device_id,
					calibration_شاخص_2, اولویت_2);

				مجموعه_Calibration_Param(SensorString(), "PRIO", calibration_شاخص_2, CAL_PRIO_UNINITIALIZED);
			}

			اولویت_2 = _external ? DEFAULT_EXTERNAL_اولویت : DEFAULT_اولویت;
		}

		// CAL_MAGx_OFF{X,Y,Z}
		مجموعه_جابه_جایی(GetCalibrationParamsVector3f(SensorString(), "OFF", calibration_شاخص_2));

		// CAL_MAGx_SCALE{X,Y,Z}
		مجموعه_scale(GetCalibrationParamsVector3f(SensorString(), "SCALE", calibration_شاخص_2));

		// CAL_MAGx_ODIAG{X,Y,Z}
		مجموعه_offdiagonal(GetCalibrationParamsVector3f(SensorString(), "ODIAG", calibration_شاخص_2));

		// CAL_MAGx_COMP{X,Y,Z}
		_power_compensation = GetCalibrationParamsVector3f(SensorString(), "COMP", calibration_شاخص_2);

		أرجع درست;
	}

	أرجع خطأ;
}

فراغ Magnetometer::Reset()
{
	إذا (_external) {
		مجموعه_rotation(ROTATION_NONE);

	} وإلا {
		// internal sensors follow board rotation
		مجموعه_rotation(GetBoardRotation());
	}

	جابه_جایی_2.zero();
	_scale.مجموعه_عنصر_خنثی();

	_power_compensation.zero();
	_power = 0.f;

	اولویت_2 = _external ? DEFAULT_EXTERNAL_اولویت : DEFAULT_اولویت;

	calibration_شاخص_2 = -1;

	calibration_شمار = 0;
}

منطقي Magnetometer::ParametersSave(صحيح desired_calibration_شاخص, منطقي force)
{
	إذا (force && desired_calibration_شاخص >= 0 && desired_calibration_شاخص < MAX_SENSOR_شمار) {
		calibration_شاخص_2 = desired_calibration_شاخص;

	} وإلا إذا (!force || (calibration_شاخص_2 < 0)
		   || (desired_calibration_شاخص != -1 && desired_calibration_شاخص != calibration_شاخص_2)) {

		// ensure we have a valid calibration slot (matching existing or first available slot)
		int8_t calibration_شاخص_prev = calibration_شاخص_2;
		calibration_شاخص_2 = پیدا_کن_موجود_Calibration_شاخص(SensorString(), _device_id, desired_calibration_شاخص);

		إذا (calibration_شاخص_prev >= 0 && (calibration_شاخص_prev != calibration_شاخص_2)) {
			PX4_WARN("%s %" PRIu32 " calibration index changed %" PRIi8 " -> %" PRIi8, SensorString(), _device_id,
				 calibration_شاخص_prev, calibration_شاخص_2);
		}
	}

	إذا (calibration_شاخص_2 >= 0 && calibration_شاخص_2 < MAX_SENSOR_شمار) {
		// save calibration
		منطقي success = درست;
		success &= مجموعه_Calibration_Param(SensorString(), "ID", calibration_شاخص_2, _device_id);
		success &= مجموعه_Calibration_Param(SensorString(), "PRIO", calibration_شاخص_2, اولویت_2);
		success &= مجموعه_Calibration_Params_Vector3f(SensorString(), "OFF", calibration_شاخص_2, جابه_جایی_2);

		ثابت Vector3f scale{_scale.diag()};
		success &= مجموعه_Calibration_Params_Vector3f(SensorString(), "SCALE", calibration_شاخص_2, scale);

		ثابت Vector3f off_diag{_scale(0, 1), _scale(0, 2), _scale(1, 2)};
		success &= مجموعه_Calibration_Params_Vector3f(SensorString(), "ODIAG", calibration_شاخص_2, off_diag);

		success &= مجموعه_Calibration_Params_Vector3f(SensorString(), "COMP", calibration_شاخص_2, _power_compensation);

		إذا (_external) {
			success &= مجموعه_Calibration_Param(SensorString(), "ROT", calibration_شاخص_2, (int32_t)_rotation_enum);

		} وإلا {
			success &= مجموعه_Calibration_Param(SensorString(), "ROT", calibration_شاخص_2, -1); //داخلی
		}

		success &= مجموعه_Calibration_Param(SensorString(), "ROLL", calibration_شاخص_2, math::degrees(_rotation_custom_euler(0)));
		success &= مجموعه_Calibration_Param(SensorString(), "PITCH", calibration_شاخص_2, math::degrees(_rotation_custom_euler(1)));
		success &= مجموعه_Calibration_Param(SensorString(), "YAW", calibration_شاخص_2, math::degrees(_rotation_custom_euler(2)));

		أرجع success;
	}

	أرجع خطأ;
}

فراغ Magnetometer::Print_وضعیت()
{
	إذا (external()) {
		PX4_INFO_RAW("%s %" PRIu32
			     " EN: %d, offset: [%05.3f %05.3f %05.3f], scale: [%05.3f %05.3f %05.3f], Ext ROT: %d\n",
			     SensorString(), device_id(), enabled(),
			     (مزدوج)جابه_جایی_2(0), (مزدوج)جابه_جایی_2(1), (مزدوج)جابه_جایی_2(2),
			     (مزدوج)_scale(0, 0), (مزدوج)_scale(1, 1), (مزدوج)_scale(2, 2),
			     rotation_enum());

	} وإلا {
		PX4_INFO_RAW("%s %" PRIu32
			     " EN: %d, offset: [%05.3f %05.3f %05.3f], scale: [%05.3f %05.3f %05.3f], Internal\n",
			     SensorString(), device_id(), enabled(),
			     (مزدوج)جابه_جایی_2(0), (مزدوج)جابه_جایی_2(1), (مزدوج)جابه_جایی_2(2),
			     (مزدوج)_scale(0, 0), (مزدوج)_scale(1, 1), (مزدوج)_scale(2, 2));
	}

#إذا defined(DEBUG_BUILD)
	_scale.print();
#پایان_اگر // DEBUG_BUILD
}

} // namespace calibration
