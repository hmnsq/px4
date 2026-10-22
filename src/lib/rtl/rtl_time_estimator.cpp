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

rtl_time_estimate_s RtlTimeEstimator::getEstimate() स्थिरांक
{
	rtl_time_estimate_s time_estimate{};

	जर (_is_valid && PX4_ISFINITE(_time_estimate)) {
		time_estimate.valid = सत्य;
		time_estimate.time_estimate = _time_estimate;
		// Use actual time estimate to compute the safer time estimate with additional scale factor and a margin
		time_estimate.safe_time_estimate = _param_rtl_time_factor.get() * _time_estimate + _param_rtl_time_margin.get();

	} अन्यथा {
		time_estimate.valid = असत्य;
	}

	time_estimate.timestamp = hrt_absolute_time();
	परत time_estimate;
}

शून्य RtlTimeEstimator::update()
{
	_wind_sub.update();

	जर (_parameter_update_sub.updated()) {
		parameter_update_s param_update;
		_parameter_update_sub.copy(&param_update);

		// If any parameter updated, call updateParams() to check if
		// this class attributes need updating (and do so).
		updateParams();
	}
}

शून्य RtlTimeEstimator::addVertDistance(दशांश alt)
{
	जर (PX4_ISFINITE(alt)) {
		_is_valid = सत्य;

		_time_estimate += calcVertTimeEstimate(alt);
	}
}

शून्य RtlTimeEstimator::addDistance(दशांश hor_dist, स्थिरांक matrix::Vector2f &direction, दशांश vert_dist)
{
	जर (PX4_ISFINITE(hor_dist) && PX4_ISFINITE(vert_dist)) {
		_is_valid = सत्य;

		दशांश hor_time_estimate{0.f};

		जर (hor_dist > भग्न_अंतर && PX4_ISFINITE(hor_dist)) {
			hor_time_estimate = hor_dist / getCruiseGroundSpeed(direction.normalized());
		}

		दशांश ver_time_estimate{calcVertTimeEstimate(vert_dist)};

		_time_estimate += math::max(hor_time_estimate, ver_time_estimate);

	}
}

शून्य RtlTimeEstimator::addWait(दशांश time_s)
{
	जर (PX4_ISFINITE(time_s)) {
		_is_valid = सत्य;

		जर (time_s > भग्न_अंतर) {
			_time_estimate += time_s;
		}
	}
}

दशांश RtlTimeEstimator::getCruiseGroundSpeed(स्थिरांक matrix::Vector2f &direction_norm)
{
	दशांश cruise_speed = getCruiseSpeed();

	जर (_vehicle_type == vehicle_status_s::VEHICLE_TYPE_FIXED_WING) {
		matrix::Vector2f wind = get_wind();

		स्थिरांक दशांश wind_along_dir = wind.dot(direction_norm);
		स्थिरांक दशांश wind_across_dir = matrix::Vector2f(wind - direction_norm * wind_along_dir).norm();

		// Note: use fminf so that we don't _rely_ on tailwind towards direction to make Return more efficient
		स्थिरांक दशांश ground_speed = sqrtf(cruise_speed * cruise_speed - wind_across_dir * wind_across_dir) + fminf(
						   0.f, wind_along_dir);

		// Assume that minimum ground speed is always satisfied. If
		//   windspeed < cas2tas(FW_AIRSPD_MAX) - FW_GND_SPD_MIN
		// the assumption always holds. Otherwise it breaks down when
		// flying upwind, and the return time estimate is optimistic.

		दशांश fw_gnd_spd_min = 5.0f;

		जर (_param_fw_gnd_spd_min != PARAM_INVALID) {
			param_get(_param_fw_gnd_spd_min, &fw_gnd_spd_min);
		}

		cruise_speed = fmaxf(ground_speed, fw_gnd_spd_min);
	}

	परत cruise_speed;
}

दशांश RtlTimeEstimator::calcVertTimeEstimate(दशांश alt)
{
	दशांश vertical_rate{0.1f};
	दशांश time_estimate{0.f};

	जर (alt > भग्न_अंतर) {
		vertical_rate = getClimbRate();

	} अन्यथा {
		vertical_rate = getDescendRate();
	}

	दशांश abs_alt = fabsf(alt);

	जर (abs_alt > भग्न_अंतर) {
		time_estimate = abs_alt / vertical_rate;
	}

	परत time_estimate;
}

दशांश RtlTimeEstimator::getCruiseSpeed()
{
	दशांश ret = 1e6f;

	जर (_vehicle_type == vehicle_status_s::VEHICLE_TYPE_ROTARY_WING) {
		जर (_param_mpc_xy_cruise == PARAM_INVALID || param_get(_param_mpc_xy_cruise, &ret) != PX4_OK) {
			ret = 1e6f;
		}

	} अन्यथा जर (_vehicle_type == vehicle_status_s::VEHICLE_TYPE_FIXED_WING) {
		जर (_param_fw_airspeed_trim == PARAM_INVALID || param_get(_param_fw_airspeed_trim, &ret) != PX4_OK) {
			ret = 1e6f;
		}

	} अन्यथा जर (_vehicle_type == vehicle_status_s::VEHICLE_TYPE_ROVER) {
		जर (_param_rover_cruise_speed == PARAM_INVALID || param_get(_param_rover_cruise_speed, &ret) != PX4_OK) {
			ret = 1e6f;
		}
	}

	परत ret;
}

matrix::Vector2f RtlTimeEstimator::get_wind()
{
	_wind_sub.update();
	matrix::Vector2f wind{};

	जर (hrt_absolute_time() - _wind_sub.get().timestamp < 1_s) {
		wind(0) = _wind_sub.get().windspeed_north;
		wind(1) = _wind_sub.get().windspeed_east;
	}

	परत wind;
}

दशांश RtlTimeEstimator::getClimbRate()
{
	दशांश ret = 1e6f;

	जर (_vehicle_type == vehicle_status_s::VEHICLE_TYPE_ROTARY_WING) {
		जर (_param_mpc_z_v_auto_up == PARAM_INVALID || param_get(_param_mpc_z_v_auto_up, &ret) != PX4_OK) {
			ret = 1e6f;
		}

	} अन्यथा जर (_vehicle_type == vehicle_status_s::VEHICLE_TYPE_FIXED_WING) {

		जर (_param_fw_climb_rate == PARAM_INVALID || param_get(_param_fw_climb_rate, &ret) != PX4_OK) {
			ret = 1e6f;
		}
	}

	परत ret;
}

दशांश RtlTimeEstimator::getDescendRate()
{
	दशांश ret = 1e6f;

	जर (_vehicle_type == vehicle_status_s::VEHICLE_TYPE_ROTARY_WING) {
		जर (_param_mpc_z_v_auto_dn == PARAM_INVALID || param_get(_param_mpc_z_v_auto_dn, &ret) != PX4_OK) {
			ret = 1e6f;
		}

	} अन्यथा जर (_vehicle_type == vehicle_status_s::VEHICLE_TYPE_FIXED_WING) {
		जर (_param_fw_sink_rate == PARAM_INVALID || param_get(_param_fw_sink_rate, &ret) != PX4_OK) {
			ret = 1e6f;
		}
	}

	परत ret;
}
