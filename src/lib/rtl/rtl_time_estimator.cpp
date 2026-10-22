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

#అన్తర్భావ "rtl_time_estimator.h"

#అన్తర్భావ <float.h>
#అన్తర్భావ <math.h>

#అన్తర్భావ <px4_platform_common/defines.h>
#అన్తర్భావ <drivers/drv_hrt.h>
#అన్తర్భావ <uORB/topics/vehicle_status.h>

Rtl_సమయం_Estimator::Rtl_సమయం_Estimator() : ModuleParams(శూన్యసూచిక)
{
	param_mpc_z_v_auto_పైకి = param_కనుగొను("MPC_Z_V_AUTO_UP");
	_param_mpc_z_v_auto_dn = param_కనుగొను("MPC_Z_V_AUTO_DN");
	param_fw_climb_రేటు = param_కనుగొను("FW_T_CLMB_R_SP");
	param_fw_sink_రేటు = param_కనుగొను("FW_T_SINK_R_SP");
	_param_fw_airspeed_trim = param_కనుగొను("FW_AIRSPD_TRIM");
	_param_fw_gnd_spd_min = param_కనుగొను("FW_GND_SPD_MIN");
	_param_mpc_xy_cruise = param_కనుగొను("MPC_XY_CRUISE");
	param_rover_cruise_వేగం = param_కనుగొను("RO_SPEED_LIM");
};

rtl_సమయం_estimate_s Rtl_సమయం_Estimator::getEstimate() స్థిరరాశి
{
	rtl_సమయం_estimate_s సమయం_estimate{};

	ఒకవేళ (_is_valid && PX4_ISFINITE(సమయం_estimate_2)) {
		సమయం_estimate.valid = నిజం;
		సమయం_estimate.సమయం_estimate = సమయం_estimate_2;
		// Use actual time estimate to compute the safer time estimate with additional scale factor and a margin
		సమయం_estimate.సురక్షిత_సమయం_estimate = param_rtl_సమయం_గుణకం.get() * సమయం_estimate_2 + param_rtl_సమయం_margin.get();

	} లేకపోతే {
		సమయం_estimate.valid = అబద్ధం;
	}

	సమయం_estimate.timestamp = hrt_absolute_సమయం();
	తిరిగి సమయం_estimate;
}

శూన్య Rtl_సమయం_Estimator::update()
{
	_wind_sub.update();

	ఒకవేళ (_parameter_update_sub.updated()) {
		parameter_update_s param_update;
		_parameter_update_sub.copy(&param_update);

		// If any parameter updated, call updateParams() to check if
		// this class attributes need updating (and do so).
		updateParams();
	}
}

శూన్య Rtl_సమయం_Estimator::కూడు_Vert_దూరం(దశాంశం alt)
{
	ఒకవేళ (PX4_ISFINITE(alt)) {
		_is_valid = నిజం;

		సమయం_estimate_2 += calc_Vert_సమయం_Estimate(alt);
	}
}

శూన్య Rtl_సమయం_Estimator::కూడు_దూరం(దశాంశం hor_dist, స్థిరరాశి మాతృక::Vector2f &direction, దశాంశం vert_dist)
{
	ఒకవేళ (PX4_ISFINITE(hor_dist) && PX4_ISFINITE(vert_dist)) {
		_is_valid = నిజం;

		దశాంశం hor_సమయం_estimate{0.f};

		ఒకవేళ (hor_dist > FLT_EPSILON && PX4_ISFINITE(hor_dist)) {
			hor_సమయం_estimate = hor_dist / get_Cruise_Ground_వేగం(direction.normalized());
		}

		దశాంశం ver_సమయం_estimate{calc_Vert_సమయం_Estimate(vert_dist)};

		సమయం_estimate_2 += math::max(hor_సమయం_estimate, ver_సమయం_estimate);

	}
}

శూన్య Rtl_సమయం_Estimator::కూడు_Wait(దశాంశం సమయం_s)
{
	ఒకవేళ (PX4_ISFINITE(సమయం_s)) {
		_is_valid = నిజం;

		ఒకవేళ (సమయం_s > FLT_EPSILON) {
			సమయం_estimate_2 += సమయం_s;
		}
	}
}

దశాంశం Rtl_సమయం_Estimator::get_Cruise_Ground_వేగం(స్థిరరాశి మాతృక::Vector2f &direction_norm)
{
	దశాంశం cruise_వేగం_2 = get_Cruise_వేగం();

	ఒకవేళ (_vehicle_type == vehicle_స్థితి_s::VEHICLE_TYPE_FIXED_WING) {
		మాతృక::Vector2f wind = get_wind();

		స్థిరరాశి దశాంశం wind_along_dir = wind.dot(direction_norm);
		స్థిరరాశి దశాంశం wind_across_dir = మాతృక::Vector2f(wind - direction_norm * wind_along_dir).norm();

		// Note: use fminf so that we don't _rely_ on tailwind towards direction to make Return more efficient
		స్థిరరాశి దశాంశం ground_వేగం = sqrtf(cruise_వేగం_2 * cruise_వేగం_2 - wind_across_dir * wind_across_dir) + fminf(
						   0.f, wind_along_dir);

		// Assume that minimum ground speed is always satisfied. If
		//   windspeed < cas2tas(FW_AIRSPD_MAX) - FW_GND_SPD_MIN
		// the assumption always holds. Otherwise it breaks down when
		// flying upwind, and the return time estimate is optimistic.

		దశాంశం fw_gnd_spd_min = 5.0f;

		ఒకవేళ (_param_fw_gnd_spd_min != PARAM_INVALID) {
			param_get(_param_fw_gnd_spd_min, &fw_gnd_spd_min);
		}

		cruise_వేగం_2 = fmaxf(ground_వేగం, fw_gnd_spd_min);
	}

	తిరిగి cruise_వేగం_2;
}

దశాంశం Rtl_సమయం_Estimator::calc_Vert_సమయం_Estimate(దశాంశం alt)
{
	దశాంశం vertical_రేటు{0.1f};
	దశాంశం సమయం_estimate{0.f};

	ఒకవేళ (alt > FLT_EPSILON) {
		vertical_రేటు = get_Climb_రేటు();

	} లేకపోతే {
		vertical_రేటు = get_Descend_రేటు();
	}

	దశాంశం abs_alt = fabsf(alt);

	ఒకవేళ (abs_alt > FLT_EPSILON) {
		సమయం_estimate = abs_alt / vertical_రేటు;
	}

	తిరిగి సమయం_estimate;
}

దశాంశం Rtl_సమయం_Estimator::get_Cruise_వేగం()
{
	దశాంశం ret = 1e6f;

	ఒకవేళ (_vehicle_type == vehicle_స్థితి_s::VEHICLE_TYPE_ROTARY_WING) {
		ఒకవేళ (_param_mpc_xy_cruise == PARAM_INVALID || param_get(_param_mpc_xy_cruise, &ret) != PX4_OK) {
			ret = 1e6f;
		}

	} లేకపోతే ఒకవేళ (_vehicle_type == vehicle_స్థితి_s::VEHICLE_TYPE_FIXED_WING) {
		ఒకవేళ (_param_fw_airspeed_trim == PARAM_INVALID || param_get(_param_fw_airspeed_trim, &ret) != PX4_OK) {
			ret = 1e6f;
		}

	} లేకపోతే ఒకవేళ (_vehicle_type == vehicle_స్థితి_s::VEHICLE_TYPE_ROVER) {
		ఒకవేళ (param_rover_cruise_వేగం == PARAM_INVALID || param_get(param_rover_cruise_వేగం, &ret) != PX4_OK) {
			ret = 1e6f;
		}
	}

	తిరిగి ret;
}

మాతృక::Vector2f Rtl_సమయం_Estimator::get_wind()
{
	_wind_sub.update();
	మాతృక::Vector2f wind{};

	ఒకవేళ (hrt_absolute_సమయం() - _wind_sub.get().timestamp < 1_s) {
		wind(0) = _wind_sub.get().windspeed_north;
		wind(1) = _wind_sub.get().windspeed_east;
	}

	తిరిగి wind;
}

దశాంశం Rtl_సమయం_Estimator::get_Climb_రేటు()
{
	దశాంశం ret = 1e6f;

	ఒకవేళ (_vehicle_type == vehicle_స్థితి_s::VEHICLE_TYPE_ROTARY_WING) {
		ఒకవేళ (param_mpc_z_v_auto_పైకి == PARAM_INVALID || param_get(param_mpc_z_v_auto_పైకి, &ret) != PX4_OK) {
			ret = 1e6f;
		}

	} లేకపోతే ఒకవేళ (_vehicle_type == vehicle_స్థితి_s::VEHICLE_TYPE_FIXED_WING) {

		ఒకవేళ (param_fw_climb_రేటు == PARAM_INVALID || param_get(param_fw_climb_రేటు, &ret) != PX4_OK) {
			ret = 1e6f;
		}
	}

	తిరిగి ret;
}

దశాంశం Rtl_సమయం_Estimator::get_Descend_రేటు()
{
	దశాంశం ret = 1e6f;

	ఒకవేళ (_vehicle_type == vehicle_స్థితి_s::VEHICLE_TYPE_ROTARY_WING) {
		ఒకవేళ (_param_mpc_z_v_auto_dn == PARAM_INVALID || param_get(_param_mpc_z_v_auto_dn, &ret) != PX4_OK) {
			ret = 1e6f;
		}

	} లేకపోతే ఒకవేళ (_vehicle_type == vehicle_స్థితి_s::VEHICLE_TYPE_FIXED_WING) {
		ఒకవేళ (param_fw_sink_రేటు == PARAM_INVALID || param_get(param_fw_sink_రేటు, &ret) != PX4_OK) {
			ret = 1e6f;
		}
	}

	తిరిగి ret;
}
