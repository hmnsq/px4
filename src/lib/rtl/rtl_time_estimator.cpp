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

#ಅನ್ತರ್ಭಾವ "rtl_time_estimator.h"

#ಅನ್ತರ್ಭಾವ <float.h>
#ಅನ್ತರ್ಭಾವ <math.h>

#ಅನ್ತರ್ಭಾವ <px4_platform_common/defines.h>
#ಅನ್ತರ್ಭಾವ <drivers/drv_hrt.h>
#ಅನ್ತರ್ಭಾವ <uORB/topics/vehicle_status.h>

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

rtl_time_estimate_s RtlTimeEstimator::getEstimate() ಧ್ರುವ
{
	rtl_time_estimate_s time_estimate{};

	ಯದಿ (_is_valid && PX4_ISFINITE(_time_estimate)) {
		time_estimate.valid = ಸತ್ಯ;
		time_estimate.time_estimate = _time_estimate;
		// Use actual time estimate to compute the safer time estimate with additional scale factor and a margin
		time_estimate.safe_time_estimate = _param_rtl_time_factor.get() * _time_estimate + _param_rtl_time_margin.get();

	} ಅನ್ಯಥಾ {
		time_estimate.valid = ಅಸತ್ಯ;
	}

	time_estimate.timestamp = hrt_absolute_time();
	ಪ್ರತ್ಯಾಗಮ time_estimate;
}

ಶೂನ್ಯ RtlTimeEstimator::update()
{
	_wind_sub.update();

	ಯದಿ (_parameter_update_sub.updated()) {
		parameter_update_s param_update;
		_parameter_update_sub.copy(&param_update);

		// If any parameter updated, call updateParams() to check if
		// this class attributes need updating (and do so).
		updateParams();
	}
}

ಶೂನ್ಯ RtlTimeEstimator::addVertDistance(ಭಿನ್ನ alt)
{
	ಯದಿ (PX4_ISFINITE(alt)) {
		_is_valid = ಸತ್ಯ;

		_time_estimate += calcVertTimeEstimate(alt);
	}
}

ಶೂನ್ಯ RtlTimeEstimator::addDistance(ಭಿನ್ನ hor_dist, ಧ್ರುವ matrix::Vector2f &direction, ಭಿನ್ನ vert_dist)
{
	ಯದಿ (PX4_ISFINITE(hor_dist) && PX4_ISFINITE(vert_dist)) {
		_is_valid = ಸತ್ಯ;

		ಭಿನ್ನ hor_time_estimate{0.f};

		ಯದಿ (hor_dist > FLT_EPSILON && PX4_ISFINITE(hor_dist)) {
			hor_time_estimate = hor_dist / getCruiseGroundSpeed(direction.normalized());
		}

		ಭಿನ್ನ ver_time_estimate{calcVertTimeEstimate(vert_dist)};

		_time_estimate += math::max(hor_time_estimate, ver_time_estimate);

	}
}

ಶೂನ್ಯ RtlTimeEstimator::addWait(ಭಿನ್ನ time_s)
{
	ಯದಿ (PX4_ISFINITE(time_s)) {
		_is_valid = ಸತ್ಯ;

		ಯದಿ (time_s > FLT_EPSILON) {
			_time_estimate += time_s;
		}
	}
}

ಭಿನ್ನ RtlTimeEstimator::getCruiseGroundSpeed(ಧ್ರುವ matrix::Vector2f &direction_norm)
{
	ಭಿನ್ನ cruise_speed = getCruiseSpeed();

	ಯದಿ (_vehicle_type == vehicle_status_s::VEHICLE_TYPE_FIXED_WING) {
		matrix::Vector2f wind = get_wind();

		ಧ್ರುವ ಭಿನ್ನ wind_along_dir = wind.dot(direction_norm);
		ಧ್ರುವ ಭಿನ್ನ wind_across_dir = matrix::Vector2f(wind - direction_norm * wind_along_dir).norm();

		// Note: use fminf so that we don't _rely_ on tailwind towards direction to make Return more efficient
		ಧ್ರುವ ಭಿನ್ನ ground_speed = sqrtf(cruise_speed * cruise_speed - wind_across_dir * wind_across_dir) + fminf(
						   0.f, wind_along_dir);

		// Assume that minimum ground speed is always satisfied. If
		//   windspeed < cas2tas(FW_AIRSPD_MAX) - FW_GND_SPD_MIN
		// the assumption always holds. Otherwise it breaks down when
		// flying upwind, and the return time estimate is optimistic.

		ಭಿನ್ನ fw_gnd_spd_min = 5.0f;

		ಯದಿ (_param_fw_gnd_spd_min != PARAM_INVALID) {
			param_get(_param_fw_gnd_spd_min, &fw_gnd_spd_min);
		}

		cruise_speed = fmaxf(ground_speed, fw_gnd_spd_min);
	}

	ಪ್ರತ್ಯಾಗಮ cruise_speed;
}

ಭಿನ್ನ RtlTimeEstimator::calcVertTimeEstimate(ಭಿನ್ನ alt)
{
	ಭಿನ್ನ vertical_rate{0.1f};
	ಭಿನ್ನ time_estimate{0.f};

	ಯದಿ (alt > FLT_EPSILON) {
		vertical_rate = getClimbRate();

	} ಅನ್ಯಥಾ {
		vertical_rate = getDescendRate();
	}

	ಭಿನ್ನ abs_alt = fabsf(alt);

	ಯದಿ (abs_alt > FLT_EPSILON) {
		time_estimate = abs_alt / vertical_rate;
	}

	ಪ್ರತ್ಯಾಗಮ time_estimate;
}

ಭಿನ್ನ RtlTimeEstimator::getCruiseSpeed()
{
	ಭಿನ್ನ ret = 1e6f;

	ಯದಿ (_vehicle_type == vehicle_status_s::VEHICLE_TYPE_ROTARY_WING) {
		ಯದಿ (_param_mpc_xy_cruise == PARAM_INVALID || param_get(_param_mpc_xy_cruise, &ret) != PX4_OK) {
			ret = 1e6f;
		}

	} ಅನ್ಯಥಾ ಯದಿ (_vehicle_type == vehicle_status_s::VEHICLE_TYPE_FIXED_WING) {
		ಯದಿ (_param_fw_airspeed_trim == PARAM_INVALID || param_get(_param_fw_airspeed_trim, &ret) != PX4_OK) {
			ret = 1e6f;
		}

	} ಅನ್ಯಥಾ ಯದಿ (_vehicle_type == vehicle_status_s::VEHICLE_TYPE_ROVER) {
		ಯದಿ (_param_rover_cruise_speed == PARAM_INVALID || param_get(_param_rover_cruise_speed, &ret) != PX4_OK) {
			ret = 1e6f;
		}
	}

	ಪ್ರತ್ಯಾಗಮ ret;
}

matrix::Vector2f RtlTimeEstimator::get_wind()
{
	_wind_sub.update();
	matrix::Vector2f wind{};

	ಯದಿ (hrt_absolute_time() - _wind_sub.get().timestamp < 1_s) {
		wind(0) = _wind_sub.get().windspeed_north;
		wind(1) = _wind_sub.get().windspeed_east;
	}

	ಪ್ರತ್ಯಾಗಮ wind;
}

ಭಿನ್ನ RtlTimeEstimator::getClimbRate()
{
	ಭಿನ್ನ ret = 1e6f;

	ಯದಿ (_vehicle_type == vehicle_status_s::VEHICLE_TYPE_ROTARY_WING) {
		ಯದಿ (_param_mpc_z_v_auto_up == PARAM_INVALID || param_get(_param_mpc_z_v_auto_up, &ret) != PX4_OK) {
			ret = 1e6f;
		}

	} ಅನ್ಯಥಾ ಯದಿ (_vehicle_type == vehicle_status_s::VEHICLE_TYPE_FIXED_WING) {

		ಯದಿ (_param_fw_climb_rate == PARAM_INVALID || param_get(_param_fw_climb_rate, &ret) != PX4_OK) {
			ret = 1e6f;
		}
	}

	ಪ್ರತ್ಯಾಗಮ ret;
}

ಭಿನ್ನ RtlTimeEstimator::getDescendRate()
{
	ಭಿನ್ನ ret = 1e6f;

	ಯದಿ (_vehicle_type == vehicle_status_s::VEHICLE_TYPE_ROTARY_WING) {
		ಯದಿ (_param_mpc_z_v_auto_dn == PARAM_INVALID || param_get(_param_mpc_z_v_auto_dn, &ret) != PX4_OK) {
			ret = 1e6f;
		}

	} ಅನ್ಯಥಾ ಯದಿ (_vehicle_type == vehicle_status_s::VEHICLE_TYPE_FIXED_WING) {
		ಯದಿ (_param_fw_sink_rate == PARAM_INVALID || param_get(_param_fw_sink_rate, &ret) != PX4_OK) {
			ret = 1e6f;
		}
	}

	ಪ್ರತ್ಯಾಗಮ ret;
}
