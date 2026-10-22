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

Rtl_ویلا_Estimator::Rtl_ویلا_Estimator() : ModuleParams(خالی_سوچک)
{
	param_mpc_z_v_auto_اتے = param_لبھو("MPC_Z_V_AUTO_UP");
	_param_mpc_z_v_auto_dn = param_لبھو("MPC_Z_V_AUTO_DN");
	param_fw_climb_شرح = param_لبھو("FW_T_CLMB_R_SP");
	param_fw_sink_شرح = param_لبھو("FW_T_SINK_R_SP");
	_param_fw_airspeed_trim = param_لبھو("FW_AIRSPD_TRIM");
	_param_fw_gnd_spd_min = param_لبھو("FW_GND_SPD_MIN");
	_param_mpc_xy_cruise = param_لبھو("MPC_XY_CRUISE");
	param_rover_cruise_رفتار = param_لبھو("RO_SPEED_LIM");
};

rtl_ویلا_estimate_s Rtl_ویلا_Estimator::getEstimate() ثابت
{
	rtl_ویلا_estimate_s ویلا_estimate{};

	إذا (_is_valid && PX4_ISFINITE(ویلا_estimate_2)) {
		ویلا_estimate.valid = سچ;
		ویلا_estimate.ویلا_estimate = ویلا_estimate_2;
		// Use actual time estimate to compute the safer time estimate with additional scale factor and a margin
		ویلا_estimate.محفوظ_ویلا_estimate = param_rtl_ویلا_ضارب.get() * ویلا_estimate_2 + param_rtl_ویلا_margin.get();

	} وإلا {
		ویلا_estimate.valid = خطأ;
	}

	ویلا_estimate.timestamp = hrt_absolute_ویلا();
	أرجع ویلا_estimate;
}

فراغ Rtl_ویلا_Estimator::update()
{
	_wind_sub.update();

	إذا (_parameter_update_sub.updated()) {
		parameter_update_s param_update;
		_parameter_update_sub.copy(&param_update);

		// If any parameter updated, call updateParams() to check if
		// this class attributes need updating (and do so).
		updateParams();
	}
}

فراغ Rtl_ویلا_Estimator::جوڑو_Vert_فاصلہ(عائم alt)
{
	إذا (PX4_ISFINITE(alt)) {
		_is_valid = سچ;

		ویلا_estimate_2 += calc_Vert_ویلا_Estimate(alt);
	}
}

فراغ Rtl_ویلا_Estimator::جوڑو_فاصلہ(عائم hor_dist, ثابت میٹرکس::Vector2f &direction, عائم vert_dist)
{
	إذا (PX4_ISFINITE(hor_dist) && PX4_ISFINITE(vert_dist)) {
		_is_valid = سچ;

		عائم hor_ویلا_estimate{0.f};

		إذا (hor_dist > FLT_EPSILON && PX4_ISFINITE(hor_dist)) {
			hor_ویلا_estimate = hor_dist / get_Cruise_Ground_رفتار(direction.normalized());
		}

		عائم ver_ویلا_estimate{calc_Vert_ویلا_Estimate(vert_dist)};

		ویلا_estimate_2 += math::max(hor_ویلا_estimate, ver_ویلا_estimate);

	}
}

فراغ Rtl_ویلا_Estimator::جوڑو_Wait(عائم ویلا_s)
{
	إذا (PX4_ISFINITE(ویلا_s)) {
		_is_valid = سچ;

		إذا (ویلا_s > FLT_EPSILON) {
			ویلا_estimate_2 += ویلا_s;
		}
	}
}

عائم Rtl_ویلا_Estimator::get_Cruise_Ground_رفتار(ثابت میٹرکس::Vector2f &direction_norm)
{
	عائم cruise_رفتار_2 = get_Cruise_رفتار();

	إذا (_vehicle_type == vehicle_حالت_s::VEHICLE_TYPE_FIXED_WING) {
		میٹرکس::Vector2f wind = get_wind();

		ثابت عائم wind_along_dir = wind.dot(direction_norm);
		ثابت عائم wind_across_dir = میٹرکس::Vector2f(wind - direction_norm * wind_along_dir).norm();

		// Note: use fminf so that we don't _rely_ on tailwind towards direction to make Return more efficient
		ثابت عائم ground_رفتار = sqrtf(cruise_رفتار_2 * cruise_رفتار_2 - wind_across_dir * wind_across_dir) + fminf(
						   0.f, wind_along_dir);

		// Assume that minimum ground speed is always satisfied. If
		//   windspeed < cas2tas(FW_AIRSPD_MAX) - FW_GND_SPD_MIN
		// the assumption always holds. Otherwise it breaks down when
		// flying upwind, and the return time estimate is optimistic.

		عائم fw_gnd_spd_min = 5.0f;

		إذا (_param_fw_gnd_spd_min != PARAM_INVALID) {
			param_get(_param_fw_gnd_spd_min, &fw_gnd_spd_min);
		}

		cruise_رفتار_2 = fmaxf(ground_رفتار, fw_gnd_spd_min);
	}

	أرجع cruise_رفتار_2;
}

عائم Rtl_ویلا_Estimator::calc_Vert_ویلا_Estimate(عائم alt)
{
	عائم vertical_شرح{0.1f};
	عائم ویلا_estimate{0.f};

	إذا (alt > FLT_EPSILON) {
		vertical_شرح = get_Climb_شرح();

	} وإلا {
		vertical_شرح = get_Descend_شرح();
	}

	عائم abs_alt = fabsf(alt);

	إذا (abs_alt > FLT_EPSILON) {
		ویلا_estimate = abs_alt / vertical_شرح;
	}

	أرجع ویلا_estimate;
}

عائم Rtl_ویلا_Estimator::get_Cruise_رفتار()
{
	عائم ret = 1e6f;

	إذا (_vehicle_type == vehicle_حالت_s::VEHICLE_TYPE_ROTARY_WING) {
		إذا (_param_mpc_xy_cruise == PARAM_INVALID || param_get(_param_mpc_xy_cruise, &ret) != PX4_OK) {
			ret = 1e6f;
		}

	} وإلا إذا (_vehicle_type == vehicle_حالت_s::VEHICLE_TYPE_FIXED_WING) {
		إذا (_param_fw_airspeed_trim == PARAM_INVALID || param_get(_param_fw_airspeed_trim, &ret) != PX4_OK) {
			ret = 1e6f;
		}

	} وإلا إذا (_vehicle_type == vehicle_حالت_s::VEHICLE_TYPE_ROVER) {
		إذا (param_rover_cruise_رفتار == PARAM_INVALID || param_get(param_rover_cruise_رفتار, &ret) != PX4_OK) {
			ret = 1e6f;
		}
	}

	أرجع ret;
}

میٹرکس::Vector2f Rtl_ویلا_Estimator::get_wind()
{
	_wind_sub.update();
	میٹرکس::Vector2f wind{};

	إذا (hrt_absolute_ویلا() - _wind_sub.get().timestamp < 1_s) {
		wind(0) = _wind_sub.get().windspeed_north;
		wind(1) = _wind_sub.get().windspeed_east;
	}

	أرجع wind;
}

عائم Rtl_ویلا_Estimator::get_Climb_شرح()
{
	عائم ret = 1e6f;

	إذا (_vehicle_type == vehicle_حالت_s::VEHICLE_TYPE_ROTARY_WING) {
		إذا (param_mpc_z_v_auto_اتے == PARAM_INVALID || param_get(param_mpc_z_v_auto_اتے, &ret) != PX4_OK) {
			ret = 1e6f;
		}

	} وإلا إذا (_vehicle_type == vehicle_حالت_s::VEHICLE_TYPE_FIXED_WING) {

		إذا (param_fw_climb_شرح == PARAM_INVALID || param_get(param_fw_climb_شرح, &ret) != PX4_OK) {
			ret = 1e6f;
		}
	}

	أرجع ret;
}

عائم Rtl_ویلا_Estimator::get_Descend_شرح()
{
	عائم ret = 1e6f;

	إذا (_vehicle_type == vehicle_حالت_s::VEHICLE_TYPE_ROTARY_WING) {
		إذا (_param_mpc_z_v_auto_dn == PARAM_INVALID || param_get(_param_mpc_z_v_auto_dn, &ret) != PX4_OK) {
			ret = 1e6f;
		}

	} وإلا إذا (_vehicle_type == vehicle_حالت_s::VEHICLE_TYPE_FIXED_WING) {
		إذا (param_fw_sink_شرح == PARAM_INVALID || param_get(param_fw_sink_شرح, &ret) != PX4_OK) {
			ret = 1e6f;
		}
	}

	أرجع ret;
}
