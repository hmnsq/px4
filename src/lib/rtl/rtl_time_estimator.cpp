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

Rtl_ಸಮಯ_Estimator::Rtl_ಸಮಯ_Estimator() : ModuleParams(ಶೂನ್ಯಸೂಚಕ)
{
	param_mpc_z_v_auto_ಮೇಲೆ = param_ಕಂಡುಹಿಡಿ("MPC_Z_V_AUTO_UP");
	_param_mpc_z_v_auto_dn = param_ಕಂಡುಹಿಡಿ("MPC_Z_V_AUTO_DN");
	param_fw_climb_ದರ = param_ಕಂಡುಹಿಡಿ("FW_T_CLMB_R_SP");
	param_fw_sink_ದರ = param_ಕಂಡುಹಿಡಿ("FW_T_SINK_R_SP");
	_param_fw_airspeed_trim = param_ಕಂಡುಹಿಡಿ("FW_AIRSPD_TRIM");
	_param_fw_gnd_spd_min = param_ಕಂಡುಹಿಡಿ("FW_GND_SPD_MIN");
	_param_mpc_xy_cruise = param_ಕಂಡುಹಿಡಿ("MPC_XY_CRUISE");
	param_rover_cruise_ವೇಗ = param_ಕಂಡುಹಿಡಿ("RO_SPEED_LIM");
};

rtl_ಸಮಯ_estimate_s Rtl_ಸಮಯ_Estimator::getEstimate() ಸ್ಥಿರಾಂಕ
{
	rtl_ಸಮಯ_estimate_s ಸಮಯ_estimate{};

	ಆದರೆ (_is_valid && PX4_ISFINITE(ಸಮಯ_estimate_2)) {
		ಸಮಯ_estimate.valid = ನಿಜ;
		ಸಮಯ_estimate.ಸಮಯ_estimate = ಸಮಯ_estimate_2;
		// Use actual time estimate to compute the safer time estimate with additional scale factor and a margin
		ಸಮಯ_estimate.ಸುರಕ್ಷಿತ_ಸಮಯ_estimate = param_rtl_ಸಮಯ_ಗುಣಕ.get() * ಸಮಯ_estimate_2 + param_rtl_ಸಮಯ_margin.get();

	} ಇಲ್ಲದಿದ್ದರೆ {
		ಸಮಯ_estimate.valid = ಸುಳ್ಳು;
	}

	ಸಮಯ_estimate.timestamp = hrt_absolute_ಸಮಯ();
	ಹಿಂತಿರುಗಿ ಸಮಯ_estimate;
}

ಶೂನ್ಯ Rtl_ಸಮಯ_Estimator::update()
{
	_wind_sub.update();

	ಆದರೆ (_parameter_update_sub.updated()) {
		parameter_update_s param_update;
		_parameter_update_sub.copy(&param_update);

		// If any parameter updated, call updateParams() to check if
		// this class attributes need updating (and do so).
		updateParams();
	}
}

ಶೂನ್ಯ Rtl_ಸಮಯ_Estimator::ಕೂಡಿಸು_Vert_ದೂರ(ದಶಮಾಂಶ alt)
{
	ಆದರೆ (PX4_ISFINITE(alt)) {
		_is_valid = ನಿಜ;

		ಸಮಯ_estimate_2 += calc_Vert_ಸಮಯ_Estimate(alt);
	}
}

ಶೂನ್ಯ Rtl_ಸಮಯ_Estimator::ಕೂಡಿಸು_ದೂರ(ದಶಮಾಂಶ hor_dist, ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::Vector2f &direction, ದಶಮಾಂಶ vert_dist)
{
	ಆದರೆ (PX4_ISFINITE(hor_dist) && PX4_ISFINITE(vert_dist)) {
		_is_valid = ನಿಜ;

		ದಶಮಾಂಶ hor_ಸಮಯ_estimate{0.f};

		ಆದರೆ (hor_dist > FLT_EPSILON && PX4_ISFINITE(hor_dist)) {
			hor_ಸಮಯ_estimate = hor_dist / get_Cruise_Ground_ವೇಗ(direction.normalized());
		}

		ದಶಮಾಂಶ ver_ಸಮಯ_estimate{calc_Vert_ಸಮಯ_Estimate(vert_dist)};

		ಸಮಯ_estimate_2 += math::max(hor_ಸಮಯ_estimate, ver_ಸಮಯ_estimate);

	}
}

ಶೂನ್ಯ Rtl_ಸಮಯ_Estimator::ಕೂಡಿಸು_Wait(ದಶಮಾಂಶ ಸಮಯ_s)
{
	ಆದರೆ (PX4_ISFINITE(ಸಮಯ_s)) {
		_is_valid = ನಿಜ;

		ಆದರೆ (ಸಮಯ_s > FLT_EPSILON) {
			ಸಮಯ_estimate_2 += ಸಮಯ_s;
		}
	}
}

ದಶಮಾಂಶ Rtl_ಸಮಯ_Estimator::get_Cruise_Ground_ವೇಗ(ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::Vector2f &direction_norm)
{
	ದಶಮಾಂಶ cruise_ವೇಗ_2 = get_Cruise_ವೇಗ();

	ಆದರೆ (_vehicle_type == vehicle_ಸ್ಥಿತಿ_s::VEHICLE_TYPE_FIXED_WING) {
		ಮಾತೃಕೆ::Vector2f wind = get_wind();

		ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ wind_along_dir = wind.dot(direction_norm);
		ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ wind_across_dir = ಮಾತೃಕೆ::Vector2f(wind - direction_norm * wind_along_dir).norm();

		// Note: use fminf so that we don't _rely_ on tailwind towards direction to make Return more efficient
		ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ ground_ವೇಗ = sqrtf(cruise_ವೇಗ_2 * cruise_ವೇಗ_2 - wind_across_dir * wind_across_dir) + fminf(
						   0.f, wind_along_dir);

		// Assume that minimum ground speed is always satisfied. If
		//   windspeed < cas2tas(FW_AIRSPD_MAX) - FW_GND_SPD_MIN
		// the assumption always holds. Otherwise it breaks down when
		// flying upwind, and the return time estimate is optimistic.

		ದಶಮಾಂಶ fw_gnd_spd_min = 5.0f;

		ಆದರೆ (_param_fw_gnd_spd_min != PARAM_INVALID) {
			param_get(_param_fw_gnd_spd_min, &fw_gnd_spd_min);
		}

		cruise_ವೇಗ_2 = fmaxf(ground_ವೇಗ, fw_gnd_spd_min);
	}

	ಹಿಂತಿರುಗಿ cruise_ವೇಗ_2;
}

ದಶಮಾಂಶ Rtl_ಸಮಯ_Estimator::calc_Vert_ಸಮಯ_Estimate(ದಶಮಾಂಶ alt)
{
	ದಶಮಾಂಶ vertical_ದರ{0.1f};
	ದಶಮಾಂಶ ಸಮಯ_estimate{0.f};

	ಆದರೆ (alt > FLT_EPSILON) {
		vertical_ದರ = get_Climb_ದರ();

	} ಇಲ್ಲದಿದ್ದರೆ {
		vertical_ದರ = get_Descend_ದರ();
	}

	ದಶಮಾಂಶ abs_alt = fabsf(alt);

	ಆದರೆ (abs_alt > FLT_EPSILON) {
		ಸಮಯ_estimate = abs_alt / vertical_ದರ;
	}

	ಹಿಂತಿರುಗಿ ಸಮಯ_estimate;
}

ದಶಮಾಂಶ Rtl_ಸಮಯ_Estimator::get_Cruise_ವೇಗ()
{
	ದಶಮಾಂಶ ret = 1e6f;

	ಆದರೆ (_vehicle_type == vehicle_ಸ್ಥಿತಿ_s::VEHICLE_TYPE_ROTARY_WING) {
		ಆದರೆ (_param_mpc_xy_cruise == PARAM_INVALID || param_get(_param_mpc_xy_cruise, &ret) != PX4_OK) {
			ret = 1e6f;
		}

	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (_vehicle_type == vehicle_ಸ್ಥಿತಿ_s::VEHICLE_TYPE_FIXED_WING) {
		ಆದರೆ (_param_fw_airspeed_trim == PARAM_INVALID || param_get(_param_fw_airspeed_trim, &ret) != PX4_OK) {
			ret = 1e6f;
		}

	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (_vehicle_type == vehicle_ಸ್ಥಿತಿ_s::VEHICLE_TYPE_ROVER) {
		ಆದರೆ (param_rover_cruise_ವೇಗ == PARAM_INVALID || param_get(param_rover_cruise_ವೇಗ, &ret) != PX4_OK) {
			ret = 1e6f;
		}
	}

	ಹಿಂತಿರುಗಿ ret;
}

ಮಾತೃಕೆ::Vector2f Rtl_ಸಮಯ_Estimator::get_wind()
{
	_wind_sub.update();
	ಮಾತೃಕೆ::Vector2f wind{};

	ಆದರೆ (hrt_absolute_ಸಮಯ() - _wind_sub.get().timestamp < 1_s) {
		wind(0) = _wind_sub.get().windspeed_north;
		wind(1) = _wind_sub.get().windspeed_east;
	}

	ಹಿಂತಿರುಗಿ wind;
}

ದಶಮಾಂಶ Rtl_ಸಮಯ_Estimator::get_Climb_ದರ()
{
	ದಶಮಾಂಶ ret = 1e6f;

	ಆದರೆ (_vehicle_type == vehicle_ಸ್ಥಿತಿ_s::VEHICLE_TYPE_ROTARY_WING) {
		ಆದರೆ (param_mpc_z_v_auto_ಮೇಲೆ == PARAM_INVALID || param_get(param_mpc_z_v_auto_ಮೇಲೆ, &ret) != PX4_OK) {
			ret = 1e6f;
		}

	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (_vehicle_type == vehicle_ಸ್ಥಿತಿ_s::VEHICLE_TYPE_FIXED_WING) {

		ಆದರೆ (param_fw_climb_ದರ == PARAM_INVALID || param_get(param_fw_climb_ದರ, &ret) != PX4_OK) {
			ret = 1e6f;
		}
	}

	ಹಿಂತಿರುಗಿ ret;
}

ದಶಮಾಂಶ Rtl_ಸಮಯ_Estimator::get_Descend_ದರ()
{
	ದಶಮಾಂಶ ret = 1e6f;

	ಆದರೆ (_vehicle_type == vehicle_ಸ್ಥಿತಿ_s::VEHICLE_TYPE_ROTARY_WING) {
		ಆದರೆ (_param_mpc_z_v_auto_dn == PARAM_INVALID || param_get(_param_mpc_z_v_auto_dn, &ret) != PX4_OK) {
			ret = 1e6f;
		}

	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (_vehicle_type == vehicle_ಸ್ಥಿತಿ_s::VEHICLE_TYPE_FIXED_WING) {
		ಆದರೆ (param_fw_sink_ದರ == PARAM_INVALID || param_get(param_fw_sink_ದರ, &ret) != PX4_OK) {
			ret = 1e6f;
		}
	}

	ಹಿಂತಿರುಗಿ ret;
}
