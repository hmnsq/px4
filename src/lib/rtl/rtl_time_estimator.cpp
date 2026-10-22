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

#ਅਨ੍ਤਰ੍ਭਾਵ "rtl_time_estimator.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <float.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <math.h>

#ਅਨ੍ਤਰ੍ਭਾਵ <px4_platform_common/defines.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <drivers/drv_hrt.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <uORB/topics/vehicle_status.h>

Rtl_ਸਮਾਂ_Estimator::Rtl_ਸਮਾਂ_Estimator() : ModuleParams(ਖਾਲੀ_ਸੂਚਕ)
{
	param_mpc_z_v_auto_ਉੱਪਰ = param_ਲੱਭੋ("MPC_Z_V_AUTO_UP");
	_param_mpc_z_v_auto_dn = param_ਲੱਭੋ("MPC_Z_V_AUTO_DN");
	param_fw_climb_ਦਰ = param_ਲੱਭੋ("FW_T_CLMB_R_SP");
	param_fw_sink_ਦਰ = param_ਲੱਭੋ("FW_T_SINK_R_SP");
	_param_fw_airspeed_trim = param_ਲੱਭੋ("FW_AIRSPD_TRIM");
	_param_fw_gnd_spd_min = param_ਲੱਭੋ("FW_GND_SPD_MIN");
	_param_mpc_xy_cruise = param_ਲੱਭੋ("MPC_XY_CRUISE");
	param_rover_cruise_ਰਫਤਾਰ = param_ਲੱਭੋ("RO_SPEED_LIM");
};

rtl_ਸਮਾਂ_estimate_s Rtl_ਸਮਾਂ_Estimator::getEstimate() ਅਚਲ
{
	rtl_ਸਮਾਂ_estimate_s ਸਮਾਂ_estimate{};

	ਜੇ (_is_valid && PX4_ISFINITE(ਸਮਾਂ_estimate_2)) {
		ਸਮਾਂ_estimate.valid = ਸੱਚ;
		ਸਮਾਂ_estimate.ਸਮਾਂ_estimate = ਸਮਾਂ_estimate_2;
		// Use actual time estimate to compute the safer time estimate with additional scale factor and a margin
		ਸਮਾਂ_estimate.ਸੁਰੱਖਿਅਤ_ਸਮਾਂ_estimate = param_rtl_ਸਮਾਂ_ਗੁਣਕ.get() * ਸਮਾਂ_estimate_2 + param_rtl_ਸਮਾਂ_margin.get();

	} ਨਹੀਂਤਾਂ {
		ਸਮਾਂ_estimate.valid = ਝੂਠ;
	}

	ਸਮਾਂ_estimate.timestamp = hrt_absolute_ਸਮਾਂ();
	ਵਾਪਸ ਸਮਾਂ_estimate;
}

ਖਾਲੀ Rtl_ਸਮਾਂ_Estimator::update()
{
	_wind_sub.update();

	ਜੇ (_parameter_update_sub.updated()) {
		parameter_update_s param_update;
		_parameter_update_sub.copy(&param_update);

		// If any parameter updated, call updateParams() to check if
		// this class attributes need updating (and do so).
		updateParams();
	}
}

ਖਾਲੀ Rtl_ਸਮਾਂ_Estimator::ਜੋੜੋ_Vert_ਦੂਰੀ(ਦਸ਼ਮਲਵ alt)
{
	ਜੇ (PX4_ISFINITE(alt)) {
		_is_valid = ਸੱਚ;

		ਸਮਾਂ_estimate_2 += calc_Vert_ਸਮਾਂ_Estimate(alt);
	}
}

ਖਾਲੀ Rtl_ਸਮਾਂ_Estimator::ਜੋੜੋ_ਦੂਰੀ(ਦਸ਼ਮਲਵ hor_dist, ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector2f &direction, ਦਸ਼ਮਲਵ vert_dist)
{
	ਜੇ (PX4_ISFINITE(hor_dist) && PX4_ISFINITE(vert_dist)) {
		_is_valid = ਸੱਚ;

		ਦਸ਼ਮਲਵ hor_ਸਮਾਂ_estimate{0.f};

		ਜੇ (hor_dist > FLT_EPSILON && PX4_ISFINITE(hor_dist)) {
			hor_ਸਮਾਂ_estimate = hor_dist / get_Cruise_Ground_ਰਫਤਾਰ(direction.normalized());
		}

		ਦਸ਼ਮਲਵ ver_ਸਮਾਂ_estimate{calc_Vert_ਸਮਾਂ_Estimate(vert_dist)};

		ਸਮਾਂ_estimate_2 += math::max(hor_ਸਮਾਂ_estimate, ver_ਸਮਾਂ_estimate);

	}
}

ਖਾਲੀ Rtl_ਸਮਾਂ_Estimator::ਜੋੜੋ_Wait(ਦਸ਼ਮਲਵ ਸਮਾਂ_s)
{
	ਜੇ (PX4_ISFINITE(ਸਮਾਂ_s)) {
		_is_valid = ਸੱਚ;

		ਜੇ (ਸਮਾਂ_s > FLT_EPSILON) {
			ਸਮਾਂ_estimate_2 += ਸਮਾਂ_s;
		}
	}
}

ਦਸ਼ਮਲਵ Rtl_ਸਮਾਂ_Estimator::get_Cruise_Ground_ਰਫਤਾਰ(ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector2f &direction_norm)
{
	ਦਸ਼ਮਲਵ cruise_ਰਫਤਾਰ_2 = get_Cruise_ਰਫਤਾਰ();

	ਜੇ (_vehicle_type == vehicle_ਸਥਿਤੀ_s::VEHICLE_TYPE_FIXED_WING) {
		ਮੈਟ੍ਰਿਕਸ::Vector2f wind = get_wind();

		ਅਚਲ ਦਸ਼ਮਲਵ wind_along_dir = wind.dot(direction_norm);
		ਅਚਲ ਦਸ਼ਮਲਵ wind_across_dir = ਮੈਟ੍ਰਿਕਸ::Vector2f(wind - direction_norm * wind_along_dir).norm();

		// Note: use fminf so that we don't _rely_ on tailwind towards direction to make Return more efficient
		ਅਚਲ ਦਸ਼ਮਲਵ ground_ਰਫਤਾਰ = sqrtf(cruise_ਰਫਤਾਰ_2 * cruise_ਰਫਤਾਰ_2 - wind_across_dir * wind_across_dir) + fminf(
						   0.f, wind_along_dir);

		// Assume that minimum ground speed is always satisfied. If
		//   windspeed < cas2tas(FW_AIRSPD_MAX) - FW_GND_SPD_MIN
		// the assumption always holds. Otherwise it breaks down when
		// flying upwind, and the return time estimate is optimistic.

		ਦਸ਼ਮਲਵ fw_gnd_spd_min = 5.0f;

		ਜੇ (_param_fw_gnd_spd_min != PARAM_INVALID) {
			param_get(_param_fw_gnd_spd_min, &fw_gnd_spd_min);
		}

		cruise_ਰਫਤਾਰ_2 = fmaxf(ground_ਰਫਤਾਰ, fw_gnd_spd_min);
	}

	ਵਾਪਸ cruise_ਰਫਤਾਰ_2;
}

ਦਸ਼ਮਲਵ Rtl_ਸਮਾਂ_Estimator::calc_Vert_ਸਮਾਂ_Estimate(ਦਸ਼ਮਲਵ alt)
{
	ਦਸ਼ਮਲਵ vertical_ਦਰ{0.1f};
	ਦਸ਼ਮਲਵ ਸਮਾਂ_estimate{0.f};

	ਜੇ (alt > FLT_EPSILON) {
		vertical_ਦਰ = get_Climb_ਦਰ();

	} ਨਹੀਂਤਾਂ {
		vertical_ਦਰ = get_Descend_ਦਰ();
	}

	ਦਸ਼ਮਲਵ abs_alt = fabsf(alt);

	ਜੇ (abs_alt > FLT_EPSILON) {
		ਸਮਾਂ_estimate = abs_alt / vertical_ਦਰ;
	}

	ਵਾਪਸ ਸਮਾਂ_estimate;
}

ਦਸ਼ਮਲਵ Rtl_ਸਮਾਂ_Estimator::get_Cruise_ਰਫਤਾਰ()
{
	ਦਸ਼ਮਲਵ ret = 1e6f;

	ਜੇ (_vehicle_type == vehicle_ਸਥਿਤੀ_s::VEHICLE_TYPE_ROTARY_WING) {
		ਜੇ (_param_mpc_xy_cruise == PARAM_INVALID || param_get(_param_mpc_xy_cruise, &ret) != PX4_OK) {
			ret = 1e6f;
		}

	} ਨਹੀਂਤਾਂ ਜੇ (_vehicle_type == vehicle_ਸਥਿਤੀ_s::VEHICLE_TYPE_FIXED_WING) {
		ਜੇ (_param_fw_airspeed_trim == PARAM_INVALID || param_get(_param_fw_airspeed_trim, &ret) != PX4_OK) {
			ret = 1e6f;
		}

	} ਨਹੀਂਤਾਂ ਜੇ (_vehicle_type == vehicle_ਸਥਿਤੀ_s::VEHICLE_TYPE_ROVER) {
		ਜੇ (param_rover_cruise_ਰਫਤਾਰ == PARAM_INVALID || param_get(param_rover_cruise_ਰਫਤਾਰ, &ret) != PX4_OK) {
			ret = 1e6f;
		}
	}

	ਵਾਪਸ ret;
}

ਮੈਟ੍ਰਿਕਸ::Vector2f Rtl_ਸਮਾਂ_Estimator::get_wind()
{
	_wind_sub.update();
	ਮੈਟ੍ਰਿਕਸ::Vector2f wind{};

	ਜੇ (hrt_absolute_ਸਮਾਂ() - _wind_sub.get().timestamp < 1_s) {
		wind(0) = _wind_sub.get().windspeed_north;
		wind(1) = _wind_sub.get().windspeed_east;
	}

	ਵਾਪਸ wind;
}

ਦਸ਼ਮਲਵ Rtl_ਸਮਾਂ_Estimator::get_Climb_ਦਰ()
{
	ਦਸ਼ਮਲਵ ret = 1e6f;

	ਜੇ (_vehicle_type == vehicle_ਸਥਿਤੀ_s::VEHICLE_TYPE_ROTARY_WING) {
		ਜੇ (param_mpc_z_v_auto_ਉੱਪਰ == PARAM_INVALID || param_get(param_mpc_z_v_auto_ਉੱਪਰ, &ret) != PX4_OK) {
			ret = 1e6f;
		}

	} ਨਹੀਂਤਾਂ ਜੇ (_vehicle_type == vehicle_ਸਥਿਤੀ_s::VEHICLE_TYPE_FIXED_WING) {

		ਜੇ (param_fw_climb_ਦਰ == PARAM_INVALID || param_get(param_fw_climb_ਦਰ, &ret) != PX4_OK) {
			ret = 1e6f;
		}
	}

	ਵਾਪਸ ret;
}

ਦਸ਼ਮਲਵ Rtl_ਸਮਾਂ_Estimator::get_Descend_ਦਰ()
{
	ਦਸ਼ਮਲਵ ret = 1e6f;

	ਜੇ (_vehicle_type == vehicle_ਸਥਿਤੀ_s::VEHICLE_TYPE_ROTARY_WING) {
		ਜੇ (_param_mpc_z_v_auto_dn == PARAM_INVALID || param_get(_param_mpc_z_v_auto_dn, &ret) != PX4_OK) {
			ret = 1e6f;
		}

	} ਨਹੀਂਤਾਂ ਜੇ (_vehicle_type == vehicle_ਸਥਿਤੀ_s::VEHICLE_TYPE_FIXED_WING) {
		ਜੇ (param_fw_sink_ਦਰ == PARAM_INVALID || param_get(param_fw_sink_ਦਰ, &ret) != PX4_OK) {
			ret = 1e6f;
		}
	}

	ਵਾਪਸ ret;
}
