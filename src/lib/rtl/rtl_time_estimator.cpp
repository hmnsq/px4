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

#شامل "rtl_time_estimator.h"

#شامل <float.h>
#شامل <math.h>

#شامل <px4_platform_common/defines.h>
#شامل <drivers/drv_hrt.h>
#شامل <uORB/topics/vehicle_status.h>

Rtl_وقت_Estimator::Rtl_وقت_Estimator() : ModuleParams(خالی_اشاریہ)
{
	param_mpc_z_v_auto_اوپر = param_ڈھونڈو("MPC_Z_V_AUTO_UP");
	_param_mpc_z_v_auto_dn = param_ڈھونڈو("MPC_Z_V_AUTO_DN");
	param_fw_climb_شرح = param_ڈھونڈو("FW_T_CLMB_R_SP");
	param_fw_sink_شرح = param_ڈھونڈو("FW_T_SINK_R_SP");
	_param_fw_airspeed_trim = param_ڈھونڈو("FW_AIRSPD_TRIM");
	_param_fw_gnd_spd_min = param_ڈھونڈو("FW_GND_SPD_MIN");
	_param_mpc_xy_cruise = param_ڈھونڈو("MPC_XY_CRUISE");
	param_rover_cruise_رفتار = param_ڈھونڈو("RO_SPEED_LIM");
};

rtl_وقت_estimate_s Rtl_وقت_Estimator::getEstimate() ثابت
{
	rtl_وقت_estimate_s وقت_estimate{};

	اگر (_is_valid && PX4_ISFINITE(وقت_estimate_2)) {
		وقت_estimate.valid = سچ;
		وقت_estimate.وقت_estimate = وقت_estimate_2;
		// Use actual time estimate to compute the safer time estimate with additional scale factor and a margin
		وقت_estimate.محفوظ_وقت_estimate = param_rtl_وقت_ضربی_عدد.get() * وقت_estimate_2 + param_rtl_وقت_margin.get();

	} ورنہ {
		وقت_estimate.valid = جھوٹ;
	}

	وقت_estimate.timestamp = hrt_absolute_وقت();
	واپس وقت_estimate;
}

خالی Rtl_وقت_Estimator::update()
{
	_wind_sub.update();

	اگر (_parameter_update_sub.updated()) {
		parameter_update_s param_update;
		_parameter_update_sub.copy(&param_update);

		// If any parameter updated, call updateParams() to check if
		// this class attributes need updating (and do so).
		updateParams();
	}
}

خالی Rtl_وقت_Estimator::جوڑو_Vert_فاصلہ(اعشاری alt)
{
	اگر (PX4_ISFINITE(alt)) {
		_is_valid = سچ;

		وقت_estimate_2 += calc_Vert_وقت_Estimate(alt);
	}
}

خالی Rtl_وقت_Estimator::جوڑو_فاصلہ(اعشاری hor_dist, ثابت میٹرکس::Vector2f &direction, اعشاری vert_dist)
{
	اگر (PX4_ISFINITE(hor_dist) && PX4_ISFINITE(vert_dist)) {
		_is_valid = سچ;

		اعشاری hor_وقت_estimate{0.f};

		اگر (hor_dist > FLT_EPSILON && PX4_ISFINITE(hor_dist)) {
			hor_وقت_estimate = hor_dist / get_Cruise_Ground_رفتار(direction.normalized());
		}

		اعشاری ver_وقت_estimate{calc_Vert_وقت_Estimate(vert_dist)};

		وقت_estimate_2 += math::max(hor_وقت_estimate, ver_وقت_estimate);

	}
}

خالی Rtl_وقت_Estimator::جوڑو_Wait(اعشاری وقت_s)
{
	اگر (PX4_ISFINITE(وقت_s)) {
		_is_valid = سچ;

		اگر (وقت_s > FLT_EPSILON) {
			وقت_estimate_2 += وقت_s;
		}
	}
}

اعشاری Rtl_وقت_Estimator::get_Cruise_Ground_رفتار(ثابت میٹرکس::Vector2f &direction_norm)
{
	اعشاری cruise_رفتار_2 = get_Cruise_رفتار();

	اگر (_vehicle_type == vehicle_حالت_s::VEHICLE_TYPE_FIXED_WING) {
		میٹرکس::Vector2f wind = get_wind();

		ثابت اعشاری wind_along_dir = wind.dot(direction_norm);
		ثابت اعشاری wind_across_dir = میٹرکس::Vector2f(wind - direction_norm * wind_along_dir).norm();

		// Note: use fminf so that we don't _rely_ on tailwind towards direction to make Return more efficient
		ثابت اعشاری ground_رفتار = sqrtf(cruise_رفتار_2 * cruise_رفتار_2 - wind_across_dir * wind_across_dir) + fminf(
						   0.f, wind_along_dir);

		// Assume that minimum ground speed is always satisfied. If
		//   windspeed < cas2tas(FW_AIRSPD_MAX) - FW_GND_SPD_MIN
		// the assumption always holds. Otherwise it breaks down when
		// flying upwind, and the return time estimate is optimistic.

		اعشاری fw_gnd_spd_min = 5.0f;

		اگر (_param_fw_gnd_spd_min != PARAM_INVALID) {
			param_get(_param_fw_gnd_spd_min, &fw_gnd_spd_min);
		}

		cruise_رفتار_2 = fmaxf(ground_رفتار, fw_gnd_spd_min);
	}

	واپس cruise_رفتار_2;
}

اعشاری Rtl_وقت_Estimator::calc_Vert_وقت_Estimate(اعشاری alt)
{
	اعشاری vertical_شرح{0.1f};
	اعشاری وقت_estimate{0.f};

	اگر (alt > FLT_EPSILON) {
		vertical_شرح = get_Climb_شرح();

	} ورنہ {
		vertical_شرح = get_Descend_شرح();
	}

	اعشاری abs_alt = fabsf(alt);

	اگر (abs_alt > FLT_EPSILON) {
		وقت_estimate = abs_alt / vertical_شرح;
	}

	واپس وقت_estimate;
}

اعشاری Rtl_وقت_Estimator::get_Cruise_رفتار()
{
	اعشاری ret = 1e6f;

	اگر (_vehicle_type == vehicle_حالت_s::VEHICLE_TYPE_ROTARY_WING) {
		اگر (_param_mpc_xy_cruise == PARAM_INVALID || param_get(_param_mpc_xy_cruise, &ret) != PX4_OK) {
			ret = 1e6f;
		}

	} ورنہ اگر (_vehicle_type == vehicle_حالت_s::VEHICLE_TYPE_FIXED_WING) {
		اگر (_param_fw_airspeed_trim == PARAM_INVALID || param_get(_param_fw_airspeed_trim, &ret) != PX4_OK) {
			ret = 1e6f;
		}

	} ورنہ اگر (_vehicle_type == vehicle_حالت_s::VEHICLE_TYPE_ROVER) {
		اگر (param_rover_cruise_رفتار == PARAM_INVALID || param_get(param_rover_cruise_رفتار, &ret) != PX4_OK) {
			ret = 1e6f;
		}
	}

	واپس ret;
}

میٹرکس::Vector2f Rtl_وقت_Estimator::get_wind()
{
	_wind_sub.update();
	میٹرکس::Vector2f wind{};

	اگر (hrt_absolute_وقت() - _wind_sub.get().timestamp < 1_s) {
		wind(0) = _wind_sub.get().windspeed_north;
		wind(1) = _wind_sub.get().windspeed_east;
	}

	واپس wind;
}

اعشاری Rtl_وقت_Estimator::get_Climb_شرح()
{
	اعشاری ret = 1e6f;

	اگر (_vehicle_type == vehicle_حالت_s::VEHICLE_TYPE_ROTARY_WING) {
		اگر (param_mpc_z_v_auto_اوپر == PARAM_INVALID || param_get(param_mpc_z_v_auto_اوپر, &ret) != PX4_OK) {
			ret = 1e6f;
		}

	} ورنہ اگر (_vehicle_type == vehicle_حالت_s::VEHICLE_TYPE_FIXED_WING) {

		اگر (param_fw_climb_شرح == PARAM_INVALID || param_get(param_fw_climb_شرح, &ret) != PX4_OK) {
			ret = 1e6f;
		}
	}

	واپس ret;
}

اعشاری Rtl_وقت_Estimator::get_Descend_شرح()
{
	اعشاری ret = 1e6f;

	اگر (_vehicle_type == vehicle_حالت_s::VEHICLE_TYPE_ROTARY_WING) {
		اگر (_param_mpc_z_v_auto_dn == PARAM_INVALID || param_get(_param_mpc_z_v_auto_dn, &ret) != PX4_OK) {
			ret = 1e6f;
		}

	} ورنہ اگر (_vehicle_type == vehicle_حالت_s::VEHICLE_TYPE_FIXED_WING) {
		اگر (param_fw_sink_شرح == PARAM_INVALID || param_get(param_fw_sink_شرح, &ret) != PX4_OK) {
			ret = 1e6f;
		}
	}

	واپس ret;
}
