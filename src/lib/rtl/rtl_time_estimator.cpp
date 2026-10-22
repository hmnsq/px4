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

#അന്തര്ഭാവ "rtl_time_estimator.h"

#അന്തര്ഭാവ <float.h>
#അന്തര്ഭാവ <math.h>

#അന്തര്ഭാവ <px4_platform_common/defines.h>
#അന്തര്ഭാവ <drivers/drv_hrt.h>
#അന്തര്ഭാവ <uORB/topics/vehicle_status.h>

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

rtl_time_estimate_s RtlTimeEstimator::getEstimate() സ്ഥിരം
{
	rtl_time_estimate_s time_estimate{};

	എങ്കിൽ (_is_valid && PX4_ISFINITE(_time_estimate)) {
		time_estimate.valid = സത്യം;
		time_estimate.time_estimate = _time_estimate;
		// Use actual time estimate to compute the safer time estimate with additional scale factor and a margin
		time_estimate.safe_time_estimate = _param_rtl_time_factor.get() * _time_estimate + _param_rtl_time_margin.get();

	} അല്ലെങ്കിൽ {
		time_estimate.valid = അസത്യം;
	}

	time_estimate.timestamp = hrt_absolute_time();
	തിരികെ time_estimate;
}

ശൂന്യം RtlTimeEstimator::update()
{
	_wind_sub.update();

	എങ്കിൽ (_parameter_update_sub.updated()) {
		parameter_update_s param_update;
		_parameter_update_sub.copy(&param_update);

		// If any parameter updated, call updateParams() to check if
		// this class attributes need updating (and do so).
		updateParams();
	}
}

ശൂന്യം RtlTimeEstimator::addVertDistance(ദശാംശം alt)
{
	എങ്കിൽ (PX4_ISFINITE(alt)) {
		_is_valid = സത്യം;

		_time_estimate += calcVertTimeEstimate(alt);
	}
}

ശൂന്യം RtlTimeEstimator::addDistance(ദശാംശം hor_dist, സ്ഥിരം matrix::Vector2f &direction, ദശാംശം vert_dist)
{
	എങ്കിൽ (PX4_ISFINITE(hor_dist) && PX4_ISFINITE(vert_dist)) {
		_is_valid = സത്യം;

		ദശാംശം hor_time_estimate{0.f};

		എങ്കിൽ (hor_dist > FLT_EPSILON && PX4_ISFINITE(hor_dist)) {
			hor_time_estimate = hor_dist / getCruiseGroundSpeed(direction.normalized());
		}

		ദശാംശം ver_time_estimate{calcVertTimeEstimate(vert_dist)};

		_time_estimate += math::max(hor_time_estimate, ver_time_estimate);

	}
}

ശൂന്യം RtlTimeEstimator::addWait(ദശാംശം time_s)
{
	എങ്കിൽ (PX4_ISFINITE(time_s)) {
		_is_valid = സത്യം;

		എങ്കിൽ (time_s > FLT_EPSILON) {
			_time_estimate += time_s;
		}
	}
}

ദശാംശം RtlTimeEstimator::getCruiseGroundSpeed(സ്ഥിരം matrix::Vector2f &direction_norm)
{
	ദശാംശം cruise_speed = getCruiseSpeed();

	എങ്കിൽ (_vehicle_type == vehicle_status_s::VEHICLE_TYPE_FIXED_WING) {
		matrix::Vector2f wind = get_wind();

		സ്ഥിരം ദശാംശം wind_along_dir = wind.dot(direction_norm);
		സ്ഥിരം ദശാംശം wind_across_dir = matrix::Vector2f(wind - direction_norm * wind_along_dir).norm();

		// Note: use fminf so that we don't _rely_ on tailwind towards direction to make Return more efficient
		സ്ഥിരം ദശാംശം ground_speed = sqrtf(cruise_speed * cruise_speed - wind_across_dir * wind_across_dir) + fminf(
						   0.f, wind_along_dir);

		// Assume that minimum ground speed is always satisfied. If
		//   windspeed < cas2tas(FW_AIRSPD_MAX) - FW_GND_SPD_MIN
		// the assumption always holds. Otherwise it breaks down when
		// flying upwind, and the return time estimate is optimistic.

		ദശാംശം fw_gnd_spd_min = 5.0f;

		എങ്കിൽ (_param_fw_gnd_spd_min != PARAM_INVALID) {
			param_get(_param_fw_gnd_spd_min, &fw_gnd_spd_min);
		}

		cruise_speed = fmaxf(ground_speed, fw_gnd_spd_min);
	}

	തിരികെ cruise_speed;
}

ദശാംശം RtlTimeEstimator::calcVertTimeEstimate(ദശാംശം alt)
{
	ദശാംശം vertical_rate{0.1f};
	ദശാംശം time_estimate{0.f};

	എങ്കിൽ (alt > FLT_EPSILON) {
		vertical_rate = getClimbRate();

	} അല്ലെങ്കിൽ {
		vertical_rate = getDescendRate();
	}

	ദശാംശം abs_alt = fabsf(alt);

	എങ്കിൽ (abs_alt > FLT_EPSILON) {
		time_estimate = abs_alt / vertical_rate;
	}

	തിരികെ time_estimate;
}

ദശാംശം RtlTimeEstimator::getCruiseSpeed()
{
	ദശാംശം ret = 1e6f;

	എങ്കിൽ (_vehicle_type == vehicle_status_s::VEHICLE_TYPE_ROTARY_WING) {
		എങ്കിൽ (_param_mpc_xy_cruise == PARAM_INVALID || param_get(_param_mpc_xy_cruise, &ret) != PX4_OK) {
			ret = 1e6f;
		}

	} അല്ലെങ്കിൽ എങ്കിൽ (_vehicle_type == vehicle_status_s::VEHICLE_TYPE_FIXED_WING) {
		എങ്കിൽ (_param_fw_airspeed_trim == PARAM_INVALID || param_get(_param_fw_airspeed_trim, &ret) != PX4_OK) {
			ret = 1e6f;
		}

	} അല്ലെങ്കിൽ എങ്കിൽ (_vehicle_type == vehicle_status_s::VEHICLE_TYPE_ROVER) {
		എങ്കിൽ (_param_rover_cruise_speed == PARAM_INVALID || param_get(_param_rover_cruise_speed, &ret) != PX4_OK) {
			ret = 1e6f;
		}
	}

	തിരികെ ret;
}

matrix::Vector2f RtlTimeEstimator::get_wind()
{
	_wind_sub.update();
	matrix::Vector2f wind{};

	എങ്കിൽ (hrt_absolute_time() - _wind_sub.get().timestamp < 1_s) {
		wind(0) = _wind_sub.get().windspeed_north;
		wind(1) = _wind_sub.get().windspeed_east;
	}

	തിരികെ wind;
}

ദശാംശം RtlTimeEstimator::getClimbRate()
{
	ദശാംശം ret = 1e6f;

	എങ്കിൽ (_vehicle_type == vehicle_status_s::VEHICLE_TYPE_ROTARY_WING) {
		എങ്കിൽ (_param_mpc_z_v_auto_up == PARAM_INVALID || param_get(_param_mpc_z_v_auto_up, &ret) != PX4_OK) {
			ret = 1e6f;
		}

	} അല്ലെങ്കിൽ എങ്കിൽ (_vehicle_type == vehicle_status_s::VEHICLE_TYPE_FIXED_WING) {

		എങ്കിൽ (_param_fw_climb_rate == PARAM_INVALID || param_get(_param_fw_climb_rate, &ret) != PX4_OK) {
			ret = 1e6f;
		}
	}

	തിരികെ ret;
}

ദശാംശം RtlTimeEstimator::getDescendRate()
{
	ദശാംശം ret = 1e6f;

	എങ്കിൽ (_vehicle_type == vehicle_status_s::VEHICLE_TYPE_ROTARY_WING) {
		എങ്കിൽ (_param_mpc_z_v_auto_dn == PARAM_INVALID || param_get(_param_mpc_z_v_auto_dn, &ret) != PX4_OK) {
			ret = 1e6f;
		}

	} അല്ലെങ്കിൽ എങ്കിൽ (_vehicle_type == vehicle_status_s::VEHICLE_TYPE_FIXED_WING) {
		എങ്കിൽ (_param_fw_sink_rate == PARAM_INVALID || param_get(_param_fw_sink_rate, &ret) != PX4_OK) {
			ret = 1e6f;
		}
	}

	തിരികെ ret;
}
