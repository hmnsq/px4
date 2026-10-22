/****************************************************************************
 *
 *   Copyright (c) 2026 PX4 Development Team. All rights reserved.
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
 * @file Crosstrack.cpp
 *
 * Helper class to do detect and avoid based on crosstrack distance
 *
 */

#ضمن "DaaCrosstrack.h"

#ضمن <float.h>

#ضمن <lib/geo/geo.h>
#ضمن <px4_platform_common/defines.h>
#ضمن <px4_platform_common/log.h>
#ضمن <uORB/topics/detect_and_avoid.h>

DaaCrosstrack::DaaCrosstrack() :
	ModuleParams(اشاره_تهی)
{
}

منطقي DaaCrosstrack::try_setting_params()
{
	updateParams();

	ثابت عائم crosstrack_sep = _param_nav_traff_a_hor.get();
	ثابت عائم vertical_sep = _param_nav_traff_a_ver.get();
	ثابت int32_t collision_زمان = _param_nav_traff_coll_t.get();

	ثابت منطقي crosstrack_ok = PX4_ISFINITE(crosstrack_sep) && crosstrack_sep > 0.f;
	ثابت منطقي vertical_ok = PX4_ISFINITE(vertical_sep) && vertical_sep > 0.f;
	ثابت منطقي collision_زمان_ok = collision_زمان > 0;

	إذا (!(crosstrack_ok && vertical_ok && collision_زمان_ok)) {
		PX4_ERR("DAA: invalid crosstrack parameters");
		أرجع خطأ;
	}

	_crosstrack_separation_m = crosstrack_sep;
	_vertical_separation_m = vertical_sep;
	collision_زمان_آستانه_s = collision_زمان;
	أرجع درست;
}

uint8_t DaaCrosstrack::calculate_daa_stats(ثابت aircraft_state_s &uav_state, ثابت aircraft_state_s &traffic_state,
		daa_stats_s &daa_stats) ثابت
{
	إذا (!PX4_ISFINITE(traffic_state.heading)) {
		PX4_DEBUG("CRT lib: invalid traffic heading");
		daa_stats = {};
		أرجع detect_and_avoid_s::DAA_CONFLICT_LVL_NONE;
	}

	عائم d_hor{0.f};
	عائم d_vert{0.f};
	get_فاصله_مقصد_نقطه_global_wgs84(uav_state.lat_lon(0), uav_state.lat_lon(1), uav_state.altitude,
					   traffic_state.lat_lon(0), traffic_state.lat_lon(1), traffic_state.altitude, &d_hor, &d_vert);

	ثابت عائم relative_uav_traffic_سرعت = calculate_relative_uav_traffic_سرعت(uav_state, traffic_state);

	// Predict until the vehicle would have passed this system at its current speed
	ثابت عائم prediction_فاصله = d_hor + k_Traffic_مقصد_Uav_فاصله_Extension;

	مزدوج انتها_lat{0.0};
	مزدوج انتها_lon{0.0};
	waypoint_from_heading_and_فاصله(traffic_state.lat_lon(0), traffic_state.lat_lon(1),
					   traffic_state.heading, prediction_فاصله, &انتها_lat, &انتها_lon);

	crosstrack_error_s crosstrack_error{};
	ثابت منطقي سطر_فاصله_valid = !get_فاصله_مقصد_سطر(crosstrack_error, uav_state.lat_lon(0),
					 uav_state.lat_lon(1), traffic_state.lat_lon(0),
					 traffic_state.lat_lon(1), انتها_lat,
					 انتها_lon);

	ثابت منطقي cs_فاصله_conflict_آستانه = سطر_فاصله_valid
			&& (!crosstrack_error.past_انتها)
			&& (fabsf(crosstrack_error.فاصله) < _crosstrack_separation_m);

	ثابت عائم vertical_separation = fabsf(uav_state.altitude - traffic_state.altitude);
	ثابت منطقي vertical_separation_conflict = vertical_separation < _vertical_separation_m;

	منطقي collision_زمان_check = خطأ;

	ثابت عائم d_xyz = hypotf(d_hor, d_vert);
	عائم زمان_مقصد_collision = 0.f;

	إذا (relative_uav_traffic_سرعت > FLT_EPSILON) {
		زمان_مقصد_collision = d_xyz / relative_uav_traffic_سرعت;
		collision_زمان_check = زمان_مقصد_collision < collision_زمان_آستانه_s;
	}

	daa_stats.aircraft_dist = d_xyz;
	daa_stats.aircraft_dist_hor = سطر_فاصله_valid ? crosstrack_error.فاصله : d_hor;
	daa_stats.aircraft_dist_vert = vertical_separation;
	daa_stats.expected_min_dist_زمان_sec = زمان_مقصد_collision;

	ثابت منطقي conflict_detected = cs_فاصله_conflict_آستانه && vertical_separation_conflict && collision_زمان_check;

	إذا (!conflict_detected) {
		أرجع detect_and_avoid_s::DAA_CONFLICT_LVL_NONE;
	}

	// Crosstrack mode has one conflict level, mapped to HIGH for compatibility.
	أرجع detect_and_avoid_s::DAA_CONFLICT_LVL_بالا;
}
