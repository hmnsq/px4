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

#ضمن "rtl_time_estimator.h"

#ضمن <float.h>
#ضمن <math.h>

#ضمن <px4_platform_common/defines.h>
#ضمن <drivers/drv_hrt.h>
#ضمن <uORB/topics/vehicle_status.h>

RtlTimeEstimator::RtlTimeEstimator() : ModuleParams(nullptr)
{
	_param_mpc_z_v_auto_up = param_find("MPC_Z_V_AUTO_UP");
	_param_mpc_z_v_auto_dn = param_find("MPC_Z_V_AUTO_DN");
	_param_fw_climb_rate = param_find("FW_T_CLMB_R_SP");
	_param_fw_sink_rate = param_find("FW_T_SINK_R_SP");
	_param_fw_airspeed_trim = param_find("FW_AIRSPD_TRIM");
	_param_fw_gnd_spd_min = param_find("FW_GND_SPD_MIN");
	_param_mpc_xy_cruise = param_find("MPC_XY_CRUISE");
	_param_rover_cruise_speed = param_find("RO_SPEED_LIM");
};

rtl_time_estimate_s RtlTimeEstimator::getEstimate() ثابت
{
	rtl_time_estimate_s time_estimate{};

	אם (_is_valid && PX4_ISFINITE(_time_estimate)) {
		time_estimate.valid = אמת;
		time_estimate.time_estimate = _time_estimate;
		// Use actual time estimate to compute the safer time estimate with additional scale factor and a margin
		time_estimate.safe_time_estimate = _param_rtl_time_factor.get() * _time_estimate + _param_rtl_time_margin.get();

	} אחרת {
		time_estimate.valid = שקר;
	}

	time_estimate.timestamp = hrt_absolute_time();
	החזר time_estimate;
}

فراغ RtlTimeEstimator::update()
{
	_wind_sub.update();

	אם (_parameter_update_sub.updated()) {
		parameter_update_s param_update;
		_parameter_update_sub.copy(&param_update);

		// If any parameter updated, call updateParams() to check if
		// this class attributes need updating (and do so).
		updateParams();
	}
}

فراغ RtlTimeEstimator::addVertDistance(عائم alt)
{
	אם (PX4_ISFINITE(alt)) {
		_is_valid = אמת;

		_time_estimate += calcVertTimeEstimate(alt);
	}
}

فراغ RtlTimeEstimator::addDistance(عائم hor_dist, ثابت matrix::Vector2f &direction, عائم vert_dist)
{
	אם (PX4_ISFINITE(hor_dist) && PX4_ISFINITE(vert_dist)) {
		_is_valid = אמת;

		عائم hor_time_estimate{0.f};

		אם (hor_dist > FLT_EPSILON && PX4_ISFINITE(hor_dist)) {
			hor_time_estimate = hor_dist / getCruiseGroundSpeed(direction.normalized());
		}

		عائم ver_time_estimate{calcVertTimeEstimate(vert_dist)};

		_time_estimate += math::max(hor_time_estimate, ver_time_estimate);

	}
}

فراغ RtlTimeEstimator::addWait(عائم time_s)
{
	אם (PX4_ISFINITE(time_s)) {
		_is_valid = אמת;

		אם (time_s > FLT_EPSILON) {
			_time_estimate += time_s;
		}
	}
}

عائم RtlTimeEstimator::getCruiseGroundSpeed(ثابت matrix::Vector2f &direction_norm)
{
	عائم cruise_speed = getCruiseSpeed();

	אם (_vehicle_type == vehicle_status_s::VEHICLE_TYPE_FIXED_WING) {
		matrix::Vector2f wind = get_wind();

		ثابت عائم wind_along_dir = wind.dot(direction_norm);
		ثابت عائم wind_across_dir = matrix::Vector2f(wind - direction_norm * wind_along_dir).norm();

		// Note: use fminf so that we don't _rely_ on tailwind towards direction to make Return more efficient
		ثابت عائم ground_speed = sqrtf(cruise_speed * cruise_speed - wind_across_dir * wind_across_dir) + fminf(
						   0.f, wind_along_dir);

		// Assume that minimum ground speed is always satisfied. If
		//   windspeed < cas2tas(FW_AIRSPD_MAX) - FW_GND_SPD_MIN
		// the assumption always holds. Otherwise it breaks down when
		// flying upwind, and the return time estimate is optimistic.

		عائم fw_gnd_spd_min = 5.0f;

		אם (_param_fw_gnd_spd_min != PARAM_INVALID) {
			param_get(_param_fw_gnd_spd_min, &fw_gnd_spd_min);
		}

		cruise_speed = fmaxf(ground_speed, fw_gnd_spd_min);
	}

	החזר cruise_speed;
}

عائم RtlTimeEstimator::calcVertTimeEstimate(عائم alt)
{
	عائم vertical_rate{0.1f};
	عائم time_estimate{0.f};

	אם (alt > FLT_EPSILON) {
		vertical_rate = getClimbRate();

	} אחרת {
		vertical_rate = getDescendRate();
	}

	عائم abs_alt = fabsf(alt);

	אם (abs_alt > FLT_EPSILON) {
		time_estimate = abs_alt / vertical_rate;
	}

	החזר time_estimate;
}

عائم RtlTimeEstimator::getCruiseSpeed()
{
	عائم ret = 1e6f;

	אם (_vehicle_type == vehicle_status_s::VEHICLE_TYPE_ROTARY_WING) {
		אם (_param_mpc_xy_cruise == PARAM_INVALID || param_get(_param_mpc_xy_cruise, &ret) != PX4_OK) {
			ret = 1e6f;
		}

	} אחרת אם (_vehicle_type == vehicle_status_s::VEHICLE_TYPE_FIXED_WING) {
		אם (_param_fw_airspeed_trim == PARAM_INVALID || param_get(_param_fw_airspeed_trim, &ret) != PX4_OK) {
			ret = 1e6f;
		}

	} אחרת אם (_vehicle_type == vehicle_status_s::VEHICLE_TYPE_ROVER) {
		אם (_param_rover_cruise_speed == PARAM_INVALID || param_get(_param_rover_cruise_speed, &ret) != PX4_OK) {
			ret = 1e6f;
		}
	}

	החזר ret;
}

matrix::Vector2f RtlTimeEstimator::get_wind()
{
	_wind_sub.update();
	matrix::Vector2f wind{};

	אם (hrt_absolute_time() - _wind_sub.get().timestamp < 1_s) {
		wind(0) = _wind_sub.get().windspeed_north;
		wind(1) = _wind_sub.get().windspeed_east;
	}

	החזר wind;
}

عائم RtlTimeEstimator::getClimbRate()
{
	عائم ret = 1e6f;

	אם (_vehicle_type == vehicle_status_s::VEHICLE_TYPE_ROTARY_WING) {
		אם (_param_mpc_z_v_auto_up == PARAM_INVALID || param_get(_param_mpc_z_v_auto_up, &ret) != PX4_OK) {
			ret = 1e6f;
		}

	} אחרת אם (_vehicle_type == vehicle_status_s::VEHICLE_TYPE_FIXED_WING) {

		אם (_param_fw_climb_rate == PARAM_INVALID || param_get(_param_fw_climb_rate, &ret) != PX4_OK) {
			ret = 1e6f;
		}
	}

	החזר ret;
}

عائم RtlTimeEstimator::getDescendRate()
{
	عائم ret = 1e6f;

	אם (_vehicle_type == vehicle_status_s::VEHICLE_TYPE_ROTARY_WING) {
		אם (_param_mpc_z_v_auto_dn == PARAM_INVALID || param_get(_param_mpc_z_v_auto_dn, &ret) != PX4_OK) {
			ret = 1e6f;
		}

	} אחרת אם (_vehicle_type == vehicle_status_s::VEHICLE_TYPE_FIXED_WING) {
		אם (_param_fw_sink_rate == PARAM_INVALID || param_get(_param_fw_sink_rate, &ret) != PX4_OK) {
			ret = 1e6f;
		}
	}

	החזר ret;
}
