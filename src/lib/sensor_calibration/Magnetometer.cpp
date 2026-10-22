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

उपयुज्य नामाकाश matrix;
उपयुज्य नामाकाश time_literals;

नामाकाश calibration
{

Magnetometer::Magnetometer()
{
	Reset();
}

Magnetometer::Magnetometer(uint32_t device_id)
{
	समुच्चयः_device_id(device_id);
}

Magnetometer::Magnetometer(uint32_t device_id, तार्किक external)
{
	समुच्चयः_device_id(device_id, external);
}

शून्य Magnetometer::समुच्चयः_device_id(uint32_t device_id)
{
	समुच्चयः_device_id(device_id, device::device_is_external(device_id));
}

शून्य Magnetometer::समुच्चयः_device_id(uint32_t device_id, तार्किक external)
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
			प्रति (पूर्णाङ्क i = 0; i < MAX_SENSOR_गणनम्; i++) {
				यदि (corrections.mag_device_ids[i] == _device_id) {
					विकल्प (i) {
					पक्ष 0:
						thermal_विस्थापनम् = Vector3f{corrections.mag_विस्थापनम्_0};
						प्रत्यागम;
					पक्ष 1:
						thermal_विस्थापनम् = Vector3f{corrections.mag_विस्थापनम्_1};
						प्रत्यागम;
					पक्ष 2:
						thermal_विस्थापनम् = Vector3f{corrections.mag_विस्थापनम्_2};
						प्रत्यागम;
					पक्ष 3:
						thermal_विस्थापनम् = Vector3f{corrections.mag_विस्थापनम्_3};
						प्रत्यागम;
					}
				}
			}
		}

		// zero thermal offset if not found
		thermal_विस्थापनम्.zero();
	}
}

तार्किक Magnetometer::समुच्चयः_विस्थापनम्(ध्रुव Vector3f &विस्थापनम्)
{
	यदि (Vector3f(विस्थापनम्_2 - विस्थापनम्).longerThan(0.005f)) {
		यदि (विस्थापनम्.isAllFinite()) {
			विस्थापनम्_2 = विस्थापनम्;
			calibration_गणनम्++;
			प्रत्यागम सत्य;
		}
	}

	प्रत्यागम असत्य;
}

तार्किक Magnetometer::समुच्चयः_scale(ध्रुव Vector3f &scale)
{
	यदि (Vector3f(_scale.diag() - scale).longerThan(0.01f)) {
		यदि (scale.isAllFinite() && (scale(0) > 0.f) && (scale(1) > 0.f) && (scale(2) > 0.f)) {
			_scale(0, 0) = scale(0);
			_scale(1, 1) = scale(1);
			_scale(2, 2) = scale(2);

			calibration_गणनम्++;
			प्रत्यागम सत्य;
		}
	}

	प्रत्यागम असत्य;
}

तार्किक Magnetometer::समुच्चयः_offdiagonal(ध्रुव Vector3f &offdiagonal)
{
	यदि (Vector3f(Vector3f{_scale(0, 1), _scale(0, 2), _scale(1, 2)} - offdiagonal).longerThan(0.01f)) {
		यदि (offdiagonal.isAllFinite()) {

			_scale(0, 1) = offdiagonal(0);
			_scale(1, 0) = offdiagonal(0);

			_scale(0, 2) = offdiagonal(1);
			_scale(2, 0) = offdiagonal(1);

			_scale(1, 2) = offdiagonal(2);
			_scale(2, 1) = offdiagonal(2);

			calibration_गणनम्++;
			प्रत्यागम सत्य;
		}
	}

	प्रत्यागम असत्य;
}

शून्य Magnetometer::समुच्चयः_rotation(ध्रुव Rotation rotation)
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

शून्य Magnetometer::समुच्चयः_custom_rotation(ध्रुव Eulerf &rotation)
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

तार्किक Magnetometer::समुच्चयः_calibration_सूचकांक(पूर्णाङ्क calibration_सूचकांक)
{
	यदि ((calibration_सूचकांक >= 0) && (calibration_सूचकांक < MAX_SENSOR_गणनम्)) {
		calibration_सूचकांक_2 = calibration_सूचकांक;
		प्रत्यागम सत्य;
	}

	प्रत्यागम असत्य;
}

शून्य Magnetometer::ParametersUpdate()
{
	यदि (_device_id == 0) {
		प्रत्यागम;
	}

	calibration_सूचकांक_2 = Find_Current_Calibration_सूचकांक(SensorString(), _device_id);

	यदि (calibration_सूचकांक_2 == -1) {
		// no saved calibration available
		Reset();

	} अन्यथा {
		Parameters_आरोपय();
	}
}

तार्किक Magnetometer::Parameters_आरोपय()
{
	यदि (calibration_सूचकांक_2 >= 0 && calibration_सूचकांक_2 < MAX_SENSOR_गणनम्) {
		// CAL_MAGx_ROT
		int32_t rotation_मानम् = GetCalibrationParamInt32(SensorString(), "ROT", calibration_सूचकांक_2);

		ध्रुव भिन्न euler_roll_deg = GetCalibrationParamFloat(SensorString(), "ROLL", calibration_सूचकांक_2);
		ध्रुव भिन्न euler_pitch_deg = GetCalibrationParamFloat(SensorString(), "PITCH", calibration_सूचकांक_2);
		ध्रुव भिन्न euler_yaw_deg = GetCalibrationParamFloat(SensorString(), "YAW", calibration_सूचकांक_2);

		यदि (_external) {
			यदि (((rotation_मानम् >= ROTATION_MAX) && (rotation_मानम् != ROTATION_CUSTOM)) || (rotation_मानम् < 0)) {
				// invalid rotation, resetting
				rotation_मानम् = ROTATION_NONE;
			}

			// if CAL_MAGx_{ROLL,PITCH,YAW} manually set then CAL_MAGx_ROT needs to be ROTATION_CUSTOM
			यदि ((rotation_मानम् != ROTATION_CUSTOM)
			    && ((fabsf(euler_roll_deg) > FLT_EPSILON)
				|| (fabsf(euler_pitch_deg) > FLT_EPSILON)
				|| (fabsf(euler_yaw_deg) > FLT_EPSILON))) {

				rotation_मानम् = ROTATION_CUSTOM;
				समुच्चयः_Calibration_Param(SensorString(), "ROT", calibration_सूचकांक_2, rotation_मानम्);
			}

			// Handle custom specified euler angle
			यदि (rotation_मानम् == ROTATION_CUSTOM) {

				ध्रुव matrix::Eulerf rotation_custom_euler{
					math::radians(euler_roll_deg),
					math::radians(euler_pitch_deg),
					math::radians(euler_yaw_deg)};

				समुच्चयः_custom_rotation(rotation_custom_euler);

			} अन्यथा {
				समुच्चयः_rotation(static_cast<Rotation>(rotation_मानम्));
			}

		} अन्यथा {
			// internal sensors follow board rotation
			समुच्चयः_rotation(GetBoardRotation());
		}

		// CAL_MAGx_PRIO
		प्राथमिकता_2 = GetCalibrationParamInt32(SensorString(), "PRIO", calibration_सूचकांक_2);

		यदि ((प्राथमिकता_2 < 0) || (प्राथमिकता_2 > 100)) {
			// reset to default, -1 is the uninitialized parameter value
			स्थिर constexpr int32_t CAL_PRIO_UNINITIALIZED = -1;

			यदि (प्राथमिकता_2 != CAL_PRIO_UNINITIALIZED) {
				PX4_ERR("%s %" PRIu32 " (%" PRId8 ") invalid priority %" PRId32 ", resetting", SensorString(), _device_id,
					calibration_सूचकांक_2, प्राथमिकता_2);

				समुच्चयः_Calibration_Param(SensorString(), "PRIO", calibration_सूचकांक_2, CAL_PRIO_UNINITIALIZED);
			}

			प्राथमिकता_2 = _external ? DEFAULT_EXTERNAL_प्राथमिकता : DEFAULT_प्राथमिकता;
		}

		// CAL_MAGx_OFF{X,Y,Z}
		समुच्चयः_विस्थापनम्(GetCalibrationParamsVector3f(SensorString(), "OFF", calibration_सूचकांक_2));

		// CAL_MAGx_SCALE{X,Y,Z}
		समुच्चयः_scale(GetCalibrationParamsVector3f(SensorString(), "SCALE", calibration_सूचकांक_2));

		// CAL_MAGx_ODIAG{X,Y,Z}
		समुच्चयः_offdiagonal(GetCalibrationParamsVector3f(SensorString(), "ODIAG", calibration_सूचकांक_2));

		// CAL_MAGx_COMP{X,Y,Z}
		_power_compensation = GetCalibrationParamsVector3f(SensorString(), "COMP", calibration_सूचकांक_2);

		प्रत्यागम सत्य;
	}

	प्रत्यागम असत्य;
}

शून्य Magnetometer::Reset()
{
	यदि (_external) {
		समुच्चयः_rotation(ROTATION_NONE);

	} अन्यथा {
		// internal sensors follow board rotation
		समुच्चयः_rotation(GetBoardRotation());
	}

	विस्थापनम्_2.zero();
	_scale.समुच्चयः_तटस्थः();

	_power_compensation.zero();
	_power = 0.f;

	प्राथमिकता_2 = _external ? DEFAULT_EXTERNAL_प्राथमिकता : DEFAULT_प्राथमिकता;

	calibration_सूचकांक_2 = -1;

	calibration_गणनम् = 0;
}

तार्किक Magnetometer::ParametersSave(पूर्णाङ्क desired_calibration_सूचकांक, तार्किक force)
{
	यदि (force && desired_calibration_सूचकांक >= 0 && desired_calibration_सूचकांक < MAX_SENSOR_गणनम्) {
		calibration_सूचकांक_2 = desired_calibration_सूचकांक;

	} अन्यथा यदि (!force || (calibration_सूचकांक_2 < 0)
		   || (desired_calibration_सूचकांक != -1 && desired_calibration_सूचकांक != calibration_सूचकांक_2)) {

		// ensure we have a valid calibration slot (matching existing or first available slot)
		int8_t calibration_सूचकांक_पूर्व = calibration_सूचकांक_2;
		calibration_सूचकांक_2 = Find_उपलब्धम्_Calibration_सूचकांक(SensorString(), _device_id, desired_calibration_सूचकांक);

		यदि (calibration_सूचकांक_पूर्व >= 0 && (calibration_सूचकांक_पूर्व != calibration_सूचकांक_2)) {
			PX4_WARN("%s %" PRIu32 " calibration index changed %" PRIi8 " -> %" PRIi8, SensorString(), _device_id,
				 calibration_सूचकांक_पूर्व, calibration_सूचकांक_2);
		}
	}

	यदि (calibration_सूचकांक_2 >= 0 && calibration_सूचकांक_2 < MAX_SENSOR_गणनम्) {
		// save calibration
		तार्किक success = सत्य;
		success &= समुच्चयः_Calibration_Param(SensorString(), "ID", calibration_सूचकांक_2, _device_id);
		success &= समुच्चयः_Calibration_Param(SensorString(), "PRIO", calibration_सूचकांक_2, प्राथमिकता_2);
		success &= समुच्चयः_Calibration_Params_Vector3f(SensorString(), "OFF", calibration_सूचकांक_2, विस्थापनम्_2);

		ध्रुव Vector3f scale{_scale.diag()};
		success &= समुच्चयः_Calibration_Params_Vector3f(SensorString(), "SCALE", calibration_सूचकांक_2, scale);

		ध्रुव Vector3f off_diag{_scale(0, 1), _scale(0, 2), _scale(1, 2)};
		success &= समुच्चयः_Calibration_Params_Vector3f(SensorString(), "ODIAG", calibration_सूचकांक_2, off_diag);

		success &= समुच्चयः_Calibration_Params_Vector3f(SensorString(), "COMP", calibration_सूचकांक_2, _power_compensation);

		यदि (_external) {
			success &= समुच्चयः_Calibration_Param(SensorString(), "ROT", calibration_सूचकांक_2, (int32_t)_rotation_enum);

		} अन्यथा {
			success &= समुच्चयः_Calibration_Param(SensorString(), "ROT", calibration_सूचकांक_2, -1); //आन्तरिकम्
		}

		success &= समुच्चयः_Calibration_Param(SensorString(), "ROLL", calibration_सूचकांक_2, math::degrees(_rotation_custom_euler(0)));
		success &= समुच्चयः_Calibration_Param(SensorString(), "PITCH", calibration_सूचकांक_2, math::degrees(_rotation_custom_euler(1)));
		success &= समुच्चयः_Calibration_Param(SensorString(), "YAW", calibration_सूचकांक_2, math::degrees(_rotation_custom_euler(2)));

		प्रत्यागम success;
	}

	प्रत्यागम असत्य;
}

शून्य Magnetometer::Print_स्थितिः()
{
	यदि (external()) {
		PX4_INFO_RAW("%s %" PRIu32
			     " EN: %d, offset: [%05.3f %05.3f %05.3f], scale: [%05.3f %05.3f %05.3f], Ext ROT: %d\n",
			     SensorString(), device_id(), enabled(),
			     (द्विगुण)विस्थापनम्_2(0), (द्विगुण)विस्थापनम्_2(1), (द्विगुण)विस्थापनम्_2(2),
			     (द्विगुण)_scale(0, 0), (द्विगुण)_scale(1, 1), (द्विगुण)_scale(2, 2),
			     rotation_enum());

	} अन्यथा {
		PX4_INFO_RAW("%s %" PRIu32
			     " EN: %d, offset: [%05.3f %05.3f %05.3f], scale: [%05.3f %05.3f %05.3f], Internal\n",
			     SensorString(), device_id(), enabled(),
			     (द्विगुण)विस्थापनम्_2(0), (द्विगुण)विस्थापनम्_2(1), (द्विगुण)विस्थापनम्_2(2),
			     (द्विगुण)_scale(0, 0), (द्विगुण)_scale(1, 1), (द्विगुण)_scale(2, 2));
	}

#यदि defined(DEBUG_BUILD)
	_scale.print();
#यदि_अन्तः // DEBUG_BUILD
}

} // namespace calibration
