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

#ਅਨ੍ਤਰ੍ਭਾਵ "DaaCrosstrack.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <float.h>

#ਅਨ੍ਤਰ੍ਭਾਵ <lib/geo/geo.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <px4_platform_common/defines.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <px4_platform_common/log.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <uORB/topics/detect_and_avoid.h>

DaaCrosstrack::DaaCrosstrack() :
	ModuleParams(ਖਾਲੀ_ਸੂਚਕ)
{
}

ਤਾਰਕਿਕ DaaCrosstrack::try_setting_params()
{
	updateParams();

	ਅਚਲ ਦਸ਼ਮਲਵ crosstrack_sep = _param_nav_traff_a_hor.get();
	ਅਚਲ ਦਸ਼ਮਲਵ vertical_sep = _param_nav_traff_a_ver.get();
	ਅਚਲ int32_t collision_ਸਮਾਂ = _param_nav_traff_coll_t.get();

	ਅਚਲ ਤਾਰਕਿਕ crosstrack_ok = PX4_ISFINITE(crosstrack_sep) && crosstrack_sep > 0.f;
	ਅਚਲ ਤਾਰਕਿਕ vertical_ok = PX4_ISFINITE(vertical_sep) && vertical_sep > 0.f;
	ਅਚਲ ਤਾਰਕਿਕ collision_ਸਮਾਂ_ok = collision_ਸਮਾਂ > 0;

	ਜੇ (!(crosstrack_ok && vertical_ok && collision_ਸਮਾਂ_ok)) {
		PX4_ERR("DAA: invalid crosstrack parameters");
		ਵਾਪਸ ਝੂਠ;
	}

	_crosstrack_separation_m = crosstrack_sep;
	_vertical_separation_m = vertical_sep;
	collision_ਸਮਾਂ_ਹੱਦ_s = collision_ਸਮਾਂ;
	ਵਾਪਸ ਸੱਚ;
}

uint8_t DaaCrosstrack::calculate_daa_stats(ਅਚਲ aircraft_state_s &uav_state, ਅਚਲ aircraft_state_s &traffic_state,
		daa_stats_s &daa_stats) ਅਚਲ
{
	ਜੇ (!PX4_ISFINITE(traffic_state.heading)) {
		PX4_DEBUG("CRT lib: invalid traffic heading");
		daa_stats = {};
		ਵਾਪਸ detect_and_avoid_s::DAA_CONFLICT_LVL_NONE;
	}

	ਦਸ਼ਮਲਵ d_hor{0.f};
	ਦਸ਼ਮਲਵ d_vert{0.f};
	get_ਦੂਰੀ_ਮੰਜ਼ਿਲ_ਬਿੰਦੂ_global_wgs84(uav_state.lat_lon(0), uav_state.lat_lon(1), uav_state.altitude,
					   traffic_state.lat_lon(0), traffic_state.lat_lon(1), traffic_state.altitude, &d_hor, &d_vert);

	ਅਚਲ ਦਸ਼ਮਲਵ relative_uav_traffic_ਰਫਤਾਰ = calculate_relative_uav_traffic_ਰਫਤਾਰ(uav_state, traffic_state);

	// Predict until the vehicle would have passed this system at its current speed
	ਅਚਲ ਦਸ਼ਮਲਵ prediction_ਦੂਰੀ = d_hor + k_Traffic_ਮੰਜ਼ਿਲ_Uav_ਦੂਰੀ_Extension;

	ਦੁੱਗਣਾ ਸਿਰਾ_lat{0.0};
	ਦੁੱਗਣਾ ਸਿਰਾ_lon{0.0};
	waypoint_from_heading_and_ਦੂਰੀ(traffic_state.lat_lon(0), traffic_state.lat_lon(1),
					   traffic_state.heading, prediction_ਦੂਰੀ, &ਸਿਰਾ_lat, &ਸਿਰਾ_lon);

	crosstrack_error_s crosstrack_error{};
	ਅਚਲ ਤਾਰਕਿਕ ਸਤਰ_ਦੂਰੀ_valid = !get_ਦੂਰੀ_ਮੰਜ਼ਿਲ_ਸਤਰ(crosstrack_error, uav_state.lat_lon(0),
					 uav_state.lat_lon(1), traffic_state.lat_lon(0),
					 traffic_state.lat_lon(1), ਸਿਰਾ_lat,
					 ਸਿਰਾ_lon);

	ਅਚਲ ਤਾਰਕਿਕ cs_ਦੂਰੀ_conflict_ਹੱਦ = ਸਤਰ_ਦੂਰੀ_valid
			&& (!crosstrack_error.past_ਸਿਰਾ)
			&& (fabsf(crosstrack_error.ਦੂਰੀ) < _crosstrack_separation_m);

	ਅਚਲ ਦਸ਼ਮਲਵ vertical_separation = fabsf(uav_state.altitude - traffic_state.altitude);
	ਅਚਲ ਤਾਰਕਿਕ vertical_separation_conflict = vertical_separation < _vertical_separation_m;

	ਤਾਰਕਿਕ collision_ਸਮਾਂ_check = ਝੂਠ;

	ਅਚਲ ਦਸ਼ਮਲਵ d_xyz = hypotf(d_hor, d_vert);
	ਦਸ਼ਮਲਵ ਸਮਾਂ_ਮੰਜ਼ਿਲ_collision = 0.f;

	ਜੇ (relative_uav_traffic_ਰਫਤਾਰ > FLT_EPSILON) {
		ਸਮਾਂ_ਮੰਜ਼ਿਲ_collision = d_xyz / relative_uav_traffic_ਰਫਤਾਰ;
		collision_ਸਮਾਂ_check = ਸਮਾਂ_ਮੰਜ਼ਿਲ_collision < collision_ਸਮਾਂ_ਹੱਦ_s;
	}

	daa_stats.aircraft_dist = d_xyz;
	daa_stats.aircraft_dist_hor = ਸਤਰ_ਦੂਰੀ_valid ? crosstrack_error.ਦੂਰੀ : d_hor;
	daa_stats.aircraft_dist_vert = vertical_separation;
	daa_stats.expected_min_dist_ਸਮਾਂ_sec = ਸਮਾਂ_ਮੰਜ਼ਿਲ_collision;

	ਅਚਲ ਤਾਰਕਿਕ conflict_detected = cs_ਦੂਰੀ_conflict_ਹੱਦ && vertical_separation_conflict && collision_ਸਮਾਂ_check;

	ਜੇ (!conflict_detected) {
		ਵਾਪਸ detect_and_avoid_s::DAA_CONFLICT_LVL_NONE;
	}

	// Crosstrack mode has one conflict level, mapped to HIGH for compatibility.
	ਵਾਪਸ detect_and_avoid_s::DAA_CONFLICT_LVL_ਉੱਚਾ;
}
