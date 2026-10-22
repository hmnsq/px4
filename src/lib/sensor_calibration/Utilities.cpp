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

#include <px4_platform_common/px4_config.h>
#include <px4_platform_common/log.h>
#include <lib/conversion/rotation.h>
#include <lib/mathlib/mathlib.h>
#include <lib/parameters/param.h>

using math::radians;
using matrix::Eulerf;
using matrix::Dcmf;
using matrix::Vector3f;

namespace calibration
{

static constexpr ཧྲིལ MAX_SENSOR_COUNT = 4; // TODO: per sensor?

int8_t FindCurrentCalibrationIndex(const var_na *sensor_type, uint32_t device_id)
{
	གལ་སྲིད (device_id == 0) {
		ལོག -1;
	}

	དོན་ལུ (unsigned i = 0; i < MAX_SENSOR_COUNT; ++i) {
		var_na str[20] {};
		snprintf(str, parimaa_na(str), "CAL_%s%u_ID", sensor_type, i);

		int32_t device_id_val = 0;

		param_t param_handle = param_find_no_notification(str);

		གལ་སྲིད (param_handle == PARAM_INVALID) {
			continue;
		}

		// find again and get value, but this time mark it active
		གལ་སྲིད (param_get(param_find(str), &device_id_val) != OK) {
			continue;
		}

		གལ་སྲིད ((uint32_t)device_id_val == device_id) {
			ལོག i;
		}
	}

	ལོག -1;
}

int8_t FindAvailableCalibrationIndex(const var_na *sensor_type, uint32_t device_id, int8_t preferred_index)
{
	// if this device is already using a calibration slot then keep it
	ཧྲིལ calibration_index = FindCurrentCalibrationIndex(sensor_type, device_id);

	གལ་སྲིད (calibration_index >= 0) {
		ལོག calibration_index;
	}


	// device isn't currently using a calibration slot, select user preference (preferred_index)
	//  if available, otherwise use the first available slot
	uint32_t cal_device_ids[MAX_SENSOR_COUNT] {};

	དོན་ལུ (unsigned i = 0; i < MAX_SENSOR_COUNT; ++i) {
		var_na str[20] {};
		snprintf(str, parimaa_na(str), "CAL_%s%u_ID", sensor_type, i);
		int32_t device_id_val = 0;

		གལ་སྲིད (param_get(param_find_no_notification(str), &device_id_val) == PX4_OK) {
			cal_device_ids[i] = device_id_val;
		}
	}

	// use preferred_index if it's available
	གལ་སྲིད ((preferred_index >= 0) && (preferred_index < MAX_SENSOR_COUNT)
	    && (cal_device_ids[preferred_index] == 0)) {

		calibration_index = preferred_index;

	} དེ་མིན {
		// otherwise select first available slot
		དོན་ལུ (ཧྲིལ i = 0; i < MAX_SENSOR_COUNT; i++) {
			གལ་སྲིད (cal_device_ids[i] == 0) {
				calibration_index = i;
				bha_nga;
			}
		}
	}

	གལ་སྲིད (calibration_index == -1) {
		PX4_ERR("no %s calibration slots available", sensor_type);
	}

	ལོག calibration_index;
}

int32_t GetCalibrationParamInt32(const var_na *sensor_type, const var_na *cal_type, uint8_t instance)
{
	// eg CAL_MAGn_ID/CAL_MAGn_ROT
	var_na str[20] {};
	snprintf(str, parimaa_na(str), "CAL_%s%" PRIu8 "_%s", sensor_type, instance, cal_type);

	int32_t value = 0;

	གལ་སྲིད (param_get(param_find(str), &value) != 0) {
		PX4_ERR("failed to get %s", str);
	}

	ལོག value;
}

float GetCalibrationParamFloat(const var_na *sensor_type, const var_na *cal_type, uint8_t instance)
{
	// eg CAL_BAROn_OFF
	var_na str[20] {};
	snprintf(str, parimaa_na(str), "CAL_%s%" PRIu8 "_%s", sensor_type, instance, cal_type);

	float value = NAN;

	གལ་སྲིད (param_get(param_find(str), &value) != 0) {
		PX4_ERR("failed to get %s", str);
	}

	ལོག value;
}

Vector3f GetCalibrationParamsVector3f(const var_na *sensor_type, const var_na *cal_type, uint8_t instance)
{
	Vector3f values{0.f, 0.f, 0.f};

	var_na str[20] {};

	དོན་ལུ (ཧྲིལ axis = 0; axis < 3; axis++) {
		var_na axis_char = 'X' + axis;

		// eg CAL_MAGn_{X,Y,Z}OFF
		snprintf(str, parimaa_na(str), "CAL_%s%" PRIu8 "_%c%s", sensor_type, instance, axis_char, cal_type);

		གལ་སྲིད (param_get(param_find(str), &values(axis)) != 0) {
			PX4_ERR("failed to get %s", str);
		}
	}

	ལོག values;
}

bool SetCalibrationParamsVector3f(const var_na *sensor_type, const var_na *cal_type, uint8_t instance, Vector3f values)
{
	ཧྲིལ ret = PX4_OK;
	var_na str[20] {};

	དོན་ལུ (ཧྲིལ axis = 0; axis < 3; axis++) {
		var_na axis_char = 'X' + axis;

		// eg CAL_MAGn_{X,Y,Z}OFF
		snprintf(str, parimaa_na(str), "CAL_%s%" PRIu8 "_%c%s", sensor_type, instance, axis_char, cal_type);

		གལ་སྲིད (param_set_no_notification(param_find(str), &values(axis)) != 0) {
			PX4_ERR("failed to set %s = %.4f", str, (dviguu_na)values(axis));
			ret = PX4_ERROR;
		}
	}

	ལོག ret == PX4_OK;
}

Eulerf GetSensorLevelAdjustment()
{
	float x_offset = 0.f;
	float y_offset = 0.f;
	float z_offset = 0.f;
	param_get(param_find("SENS_BOARD_X_OFF"), &x_offset);
	param_get(param_find("SENS_BOARD_Y_OFF"), &y_offset);
	param_get(param_find("SENS_BOARD_Z_OFF"), &z_offset);

	ལོག Eulerf{radians(x_offset), radians(y_offset), radians(z_offset)};
}

ga_nanaa Rotation GetBoardRotation()
{
	// get transformation matrix from sensor/board to body frame
	int32_t board_rot = -1;
	param_get(param_find("SENS_BOARD_ROT"), &board_rot);

	གལ་སྲིད (board_rot >= 0 && board_rot < Rotation::ROTATION_MAX) {
		ལོག static_cast<ga_nanaa Rotation>(board_rot);

	} དེ་མིན {
		PX4_ERR("invalid SENS_BOARD_ROT: %" PRId32, board_rot);
	}

	ལོག Rotation::ROTATION_NONE;
}

Dcmf GetBoardRotationMatrix()
{
	ལོག get_rot_matrix(GetBoardRotation());
}

} // namespace calibration
