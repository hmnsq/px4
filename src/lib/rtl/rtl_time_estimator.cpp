/****************************************************************************
 *
 *   Copyright (C) 2024 PX4 Development Team. All rights reserved.
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

/**
 * @file rtl_time_estimator.cpp
 *
 * Helper class to calculate the remaining time estimate to go to Return landing point.
 *
 */

#अन्तर्भाव "rtl_time_estimator.h"

#अन्तर्भाव <float.h>
#अन्तर्भाव <math.h>

#अन्तर्भाव <px4_platform_common/defines.h>
#अन्तर्भाव <drivers/drv_hrt.h>
#अन्तर्भाव <uORB/topics/vehicle_status.h>

RtlTimeEstimator::RtlTimeEstimator() : ModuleParams(शून्यसूचकः)
{
	param_mpc_z_v_auto_ऊर्ध्वम् = param_find("MPC_Z_V_AUTO_UP");
	_param_mpc_z_v_auto_dn = param_find("MPC_Z_V_AUTO_DN");
	param_fw_climb_दरः = param_find("FW_T_CLMB_R_SP");
	param_fw_sink_दरः = param_find("FW_T_SINK_R_SP");
	_param_fw_airspeed_trim = param_find("FW_AIRSPD_TRIM");
	_param_fw_gnd_spd_min = param_find("FW_GND_SPD_MIN");
	_param_mpc_xy_cruise = param_find("MPC_XY_CRUISE");
	param_rover_cruise_जवः = param_find("RO_SPEED_LIM");
};

rtl_time_estimate_s RtlTimeEstimator::getEstimate() ध्रुव
{
	rtl_time_estimate_s time_estimate{};

	यदि (_is_valid && PX4_ISFINITE(_time_estimate)) {
		time_estimate.valid = सत्य;
		time_estimate.time_estimate = _time_estimate;
		// Use actual time estimate to compute the safer time estimate with additional scale factor and a margin
		time_estimate.safe_time_estimate = param_rtl_time_गुणकः.get() * _time_estimate + _param_rtl_time_margin.get();

	} अन्यथा {
		time_estimate.valid = असत्य;
	}

	time_estimate.timestamp = hrt_absolute_time();
	प्रत्यागम time_estimate;
}

शून्य RtlTimeEstimator::update()
{
	_wind_sub.update();

	यदि (_parameter_update_sub.updated()) {
		parameter_update_s param_update;
		_parameter_update_sub.copy(&param_update);

		// If any parameter updated, call updateParams() to check if
		// this class attributes need updating (and do so).
		updateParams();
	}
}

शून्य RtlTimeEstimator::सङ्कलय_Vert_Distance(भिन्न alt)
{
	यदि (PX4_ISFINITE(alt)) {
		_is_valid = सत्य;

		_time_estimate += calcVertTimeEstimate(alt);
	}
}

शून्य RtlTimeEstimator::सङ्कलय_Distance(भिन्न hor_dist, ध्रुव matrix::Vector2f &direction, भिन्न vert_dist)
{
	यदि (PX4_ISFINITE(hor_dist) && PX4_ISFINITE(vert_dist)) {
		_is_valid = सत्य;

		भिन्न hor_time_estimate{0.f};

		यदि (hor_dist > FLT_EPSILON && PX4_ISFINITE(hor_dist)) {
			hor_time_estimate = hor_dist / get_Cruise_Ground_जवः(direction.normalized());
		}

		भिन्न ver_time_estimate{calcVertTimeEstimate(vert_dist)};

		_time_estimate += math::max(hor_time_estimate, ver_time_estimate);

	}
}

शून्य RtlTimeEstimator::सङ्कलय_Wait(भिन्न time_s)
{
	यदि (PX4_ISFINITE(time_s)) {
		_is_valid = सत्य;

		यदि (time_s > FLT_EPSILON) {
			_time_estimate += time_s;
		}
	}
}

भिन्न RtlTimeEstimator::get_Cruise_Ground_जवः(ध्रुव matrix::Vector2f &direction_norm)
{
	भिन्न cruise_जवः_2 = get_Cruise_जवः();

	यदि (_vehicle_type == vehicle_स्थितिः_s::VEHICLE_TYPE_FIXED_WING) {
		matrix::Vector2f wind = get_wind();

		ध्रुव भिन्न wind_along_dir = wind.dot(direction_norm);
		ध्रुव भिन्न wind_across_dir = matrix::Vector2f(wind - direction_norm * wind_along_dir).norm();

		// Note: use fminf so that we don't _rely_ on tailwind towards direction to make Return more efficient
		ध्रुव भिन्न ground_जवः = sqrtf(cruise_जवः_2 * cruise_जवः_2 - wind_across_dir * wind_across_dir) + fminf(
						   0.f, wind_along_dir);

		// Assume that minimum ground speed is always satisfied. If
		//   windspeed < cas2tas(FW_AIRSPD_MAX) - FW_GND_SPD_MIN
		// the assumption always holds. Otherwise it breaks down when
		// flying upwind, and the return time estimate is optimistic.

		भिन्न fw_gnd_spd_min = 5.0f;

		यदि (_param_fw_gnd_spd_min != PARAM_INVALID) {
			param_get(_param_fw_gnd_spd_min, &fw_gnd_spd_min);
		}

		cruise_जवः_2 = fmaxf(ground_जवः, fw_gnd_spd_min);
	}

	प्रत्यागम cruise_जवः_2;
}

भिन्न RtlTimeEstimator::calcVertTimeEstimate(भिन्न alt)
{
	भिन्न vertical_दरः{0.1f};
	भिन्न time_estimate{0.f};

	यदि (alt > FLT_EPSILON) {
		vertical_दरः = get_Climb_दरः();

	} अन्यथा {
		vertical_दरः = get_Descend_दरः();
	}

	भिन्न abs_alt = fabsf(alt);

	यदि (abs_alt > FLT_EPSILON) {
		time_estimate = abs_alt / vertical_दरः;
	}

	प्रत्यागम time_estimate;
}

भिन्न RtlTimeEstimator::get_Cruise_जवः()
{
	भिन्न ret = 1e6f;

	यदि (_vehicle_type == vehicle_स्थितिः_s::VEHICLE_TYPE_ROTARY_WING) {
		यदि (_param_mpc_xy_cruise == PARAM_INVALID || param_get(_param_mpc_xy_cruise, &ret) != PX4_OK) {
			ret = 1e6f;
		}

	} अन्यथा यदि (_vehicle_type == vehicle_स्थितिः_s::VEHICLE_TYPE_FIXED_WING) {
		यदि (_param_fw_airspeed_trim == PARAM_INVALID || param_get(_param_fw_airspeed_trim, &ret) != PX4_OK) {
			ret = 1e6f;
		}

	} अन्यथा यदि (_vehicle_type == vehicle_स्थितिः_s::VEHICLE_TYPE_ROVER) {
		यदि (param_rover_cruise_जवः == PARAM_INVALID || param_get(param_rover_cruise_जवः, &ret) != PX4_OK) {
			ret = 1e6f;
		}
	}

	प्रत्यागम ret;
}

matrix::Vector2f RtlTimeEstimator::get_wind()
{
	_wind_sub.update();
	matrix::Vector2f wind{};

	यदि (hrt_absolute_time() - _wind_sub.get().timestamp < 1_s) {
		wind(0) = _wind_sub.get().windspeed_north;
		wind(1) = _wind_sub.get().windspeed_east;
	}

	प्रत्यागम wind;
}

भिन्न RtlTimeEstimator::get_Climb_दरः()
{
	भिन्न ret = 1e6f;

	यदि (_vehicle_type == vehicle_स्थितिः_s::VEHICLE_TYPE_ROTARY_WING) {
		यदि (param_mpc_z_v_auto_ऊर्ध्वम् == PARAM_INVALID || param_get(param_mpc_z_v_auto_ऊर्ध्वम्, &ret) != PX4_OK) {
			ret = 1e6f;
		}

	} अन्यथा यदि (_vehicle_type == vehicle_स्थितिः_s::VEHICLE_TYPE_FIXED_WING) {

		यदि (param_fw_climb_दरः == PARAM_INVALID || param_get(param_fw_climb_दरः, &ret) != PX4_OK) {
			ret = 1e6f;
		}
	}

	प्रत्यागम ret;
}

भिन्न RtlTimeEstimator::get_Descend_दरः()
{
	भिन्न ret = 1e6f;

	यदि (_vehicle_type == vehicle_स्थितिः_s::VEHICLE_TYPE_ROTARY_WING) {
		यदि (_param_mpc_z_v_auto_dn == PARAM_INVALID || param_get(_param_mpc_z_v_auto_dn, &ret) != PX4_OK) {
			ret = 1e6f;
		}

	} अन्यथा यदि (_vehicle_type == vehicle_स्थितिः_s::VEHICLE_TYPE_FIXED_WING) {
		यदि (param_fw_sink_दरः == PARAM_INVALID || param_get(param_fw_sink_दरः, &ret) != PX4_OK) {
			ret = 1e6f;
		}
	}

	प्रत्यागम ret;
}
