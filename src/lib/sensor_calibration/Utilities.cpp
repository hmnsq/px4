/****************************************************************************
 *
 *   Copyright (c) 2020-2021 PX4 Development Team. All rights reserved.
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

#شامل <px4_platform_common/px4_config.h>
#شامل <px4_platform_common/log.h>
#شامل <lib/conversion/rotation.h>
#شامل <lib/mathlib/mathlib.h>
#شامل <lib/parameters/param.h>

using math::radians;
using matrix::Eulerf;
using matrix::Dcmf;
using matrix::Vector3f;

فضاینام calibration
{

ایستا constexpr صحیح MAX_SENSOR_شمار = 4; // TODO: per sensor?

int8_t FindCurrentCalibrationIndex(ثابت نویسه *sensor_type, uint32_t device_id)
{
	اگر (device_id == 0) {
		بازگشت -1;
	}

	برای (unsigned i = 0; i < MAX_SENSOR_شمار; ++i) {
		نویسه str[20] {};
		snprintf(str, اندازه(str), "CAL_%s%u_ID", sensor_type, i);

		int32_t device_id_val = 0;

		param_t param_handle = param_find_no_notification(str);

		اگر (param_handle == PARAM_INVALID) {
			ادامه;
		}

		// find again and get value, but this time mark it active
		اگر (param_get(param_find(str), &device_id_val) != OK) {
			ادامه;
		}

		اگر ((uint32_t)device_id_val == device_id) {
			بازگشت i;
		}
	}

	بازگشت -1;
}

int8_t FindAvailableCalibrationIndex(ثابت نویسه *sensor_type, uint32_t device_id, int8_t preferred_index)
{
	// if this device is already using a calibration slot then keep it
	صحیح calibration_index = FindCurrentCalibrationIndex(sensor_type, device_id);

	اگر (calibration_index >= 0) {
		بازگشت calibration_index;
	}


	// device isn't currently using a calibration slot, select user preference (preferred_index)
	//  if available, otherwise use the first available slot
	uint32_t cal_device_ids[MAX_SENSOR_شمار] {};

	برای (unsigned i = 0; i < MAX_SENSOR_شمار; ++i) {
		نویسه str[20] {};
		snprintf(str, اندازه(str), "CAL_%s%u_ID", sensor_type, i);
		int32_t device_id_val = 0;

		اگر (param_get(param_find_no_notification(str), &device_id_val) == PX4_OK) {
			cal_device_ids[i] = device_id_val;
		}
	}

	// use preferred_index if it's available
	اگر ((preferred_index >= 0) && (preferred_index < MAX_SENSOR_شمار)
	    && (cal_device_ids[preferred_index] == 0)) {

		calibration_index = preferred_index;

	} وگرنه {
		// otherwise select first available slot
		برای (صحیح i = 0; i < MAX_SENSOR_شمار; i++) {
			اگر (cal_device_ids[i] == 0) {
				calibration_index = i;
				بشکن;
			}
		}
	}

	اگر (calibration_index == -1) {
		PX4_ERR("no %s calibration slots available", sensor_type);
	}

	بازگشت calibration_index;
}

int32_t GetCalibrationParamInt32(ثابت نویسه *sensor_type, ثابت نویسه *cal_type, uint8_t instance)
{
	// eg CAL_MAGn_ID/CAL_MAGn_ROT
	نویسه str[20] {};
	snprintf(str, اندازه(str), "CAL_%s%" PRIu8 "_%s", sensor_type, instance, cal_type);

	int32_t value = 0;

	اگر (param_get(param_find(str), &value) != 0) {
		PX4_ERR("failed to get %s", str);
	}

	بازگشت value;
}

اعشاری GetCalibrationParamFloat(ثابت نویسه *sensor_type, ثابت نویسه *cal_type, uint8_t instance)
{
	// eg CAL_BAROn_OFF
	نویسه str[20] {};
	snprintf(str, اندازه(str), "CAL_%s%" PRIu8 "_%s", sensor_type, instance, cal_type);

	اعشاری value = NAN;

	اگر (param_get(param_find(str), &value) != 0) {
		PX4_ERR("failed to get %s", str);
	}

	بازگشت value;
}

Vector3f GetCalibrationParamsVector3f(ثابت نویسه *sensor_type, ثابت نویسه *cal_type, uint8_t instance)
{
	Vector3f مقادیر_2{0.f, 0.f, 0.f};

	نویسه str[20] {};

	برای (صحیح axis = 0; axis < 3; axis++) {
		نویسه axis_char = 'X' + axis;

		// eg CAL_MAGn_{X,Y,Z}OFF
		snprintf(str, اندازه(str), "CAL_%s%" PRIu8 "_%c%s", sensor_type, instance, axis_char, cal_type);

		اگر (param_get(param_find(str), &مقادیر_2(axis)) != 0) {
			PX4_ERR("failed to get %s", str);
		}
	}

	بازگشت مقادیر_2;
}

منطقی SetCalibrationParamsVector3f(ثابت نویسه *sensor_type, ثابت نویسه *cal_type, uint8_t instance, Vector3f مقادیر_2)
{
	صحیح ret = PX4_OK;
	نویسه str[20] {};

	برای (صحیح axis = 0; axis < 3; axis++) {
		نویسه axis_char = 'X' + axis;

		// eg CAL_MAGn_{X,Y,Z}OFF
		snprintf(str, اندازه(str), "CAL_%s%" PRIu8 "_%c%s", sensor_type, instance, axis_char, cal_type);

		اگر (param_set_no_notification(param_find(str), &مقادیر_2(axis)) != 0) {
			PX4_ERR("failed to set %s = %.4f", str, (دوگانه)مقادیر_2(axis));
			ret = PX4_ERROR;
		}
	}

	بازگشت ret == PX4_OK;
}

Eulerf GetSensorLevelAdjustment()
{
	اعشاری x_offset = 0.f;
	اعشاری y_offset = 0.f;
	اعشاری z_offset = 0.f;
	param_get(param_find("SENS_BOARD_X_OFF"), &x_offset);
	param_get(param_find("SENS_BOARD_Y_OFF"), &y_offset);
	param_get(param_find("SENS_BOARD_Z_OFF"), &z_offset);

	بازگشت Eulerf{radians(x_offset), radians(y_offset), radians(z_offset)};
}

شمارش Rotation GetBoardRotation()
{
	// get transformation matrix from sensor/board to body frame
	int32_t board_rot = -1;
	param_get(param_find("SENS_BOARD_ROT"), &board_rot);

	اگر (board_rot >= 0 && board_rot < Rotation::ROTATION_MAX) {
		بازگشت static_cast<شمارش Rotation>(board_rot);

	} وگرنه {
		PX4_ERR("invalid SENS_BOARD_ROT: %" PRId32, board_rot);
	}

	بازگشت Rotation::ROTATION_NONE;
}

Dcmf GetBoardRotationMatrix()
{
	بازگشت get_rot_matrix(GetBoardRotation());
}

} // namespace calibration
