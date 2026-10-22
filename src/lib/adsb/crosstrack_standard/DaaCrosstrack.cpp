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

#अन्तर्भाव "DaaCrosstrack.h"

#अन्तर्भाव <float.h>

#अन्तर्भाव <lib/geo/geo.h>
#अन्तर्भाव <px4_platform_common/defines.h>
#अन्तर्भाव <px4_platform_common/log.h>
#अन्तर्भाव <uORB/topics/detect_and_avoid.h>

DaaCrosstrack::DaaCrosstrack() :
	ModuleParams(शून्यसूचकः)
{
}

तार्किक DaaCrosstrack::try_setting_params()
{
	updateParams();

	ध्रुव भिन्न crosstrack_sep = _param_nav_traff_a_hor.get();
	ध्रुव भिन्न vertical_sep = _param_nav_traff_a_ver.get();
	ध्रुव int32_t collision_time = _param_nav_traff_coll_t.get();

	ध्रुव तार्किक crosstrack_ok = PX4_ISFINITE(crosstrack_sep) && crosstrack_sep > 0.f;
	ध्रुव तार्किक vertical_ok = PX4_ISFINITE(vertical_sep) && vertical_sep > 0.f;
	ध्रुव तार्किक collision_time_ok = collision_time > 0;

	यदि (!(crosstrack_ok && vertical_ok && collision_time_ok)) {
		PX4_ERR("DAA: invalid crosstrack parameters");
		प्रत्यागम असत्य;
	}

	_crosstrack_separation_m = crosstrack_sep;
	_vertical_separation_m = vertical_sep;
	collision_time_देहली_s = collision_time;
	प्रत्यागम सत्य;
}

uint8_t DaaCrosstrack::calculate_daa_stats(ध्रुव aircraft_state_s &uav_state, ध्रुव aircraft_state_s &traffic_state,
		daa_stats_s &daa_stats) ध्रुव
{
	यदि (!PX4_ISFINITE(traffic_state.heading)) {
		PX4_DEBUG("CRT lib: invalid traffic heading");
		daa_stats = {};
		प्रत्यागम detect_and_avoid_s::DAA_CONFLICT_LVL_NONE;
	}

	भिन्न d_hor{0.f};
	भिन्न d_vert{0.f};
	get_distance_गन्तव्यम्_point_global_wgs84(uav_state.lat_lon(0), uav_state.lat_lon(1), uav_state.altitude,
					   traffic_state.lat_lon(0), traffic_state.lat_lon(1), traffic_state.altitude, &d_hor, &d_vert);

	ध्रुव भिन्न relative_uav_traffic_जवः = calculate_relative_uav_traffic_जवः(uav_state, traffic_state);

	// Predict until the vehicle would have passed this system at its current speed
	ध्रुव भिन्न prediction_distance = d_hor + k_Traffic_गन्तव्यम्_Uav_Distance_Extension;

	द्विगुण अन्तः_lat{0.0};
	द्विगुण अन्तः_lon{0.0};
	waypoint_from_heading_and_distance(traffic_state.lat_lon(0), traffic_state.lat_lon(1),
					   traffic_state.heading, prediction_distance, &अन्तः_lat, &अन्तः_lon);

	crosstrack_error_s crosstrack_error{};
	ध्रुव तार्किक पङ्क्तिः_distance_valid = !get_distance_गन्तव्यम्_पङ्क्तिः(crosstrack_error, uav_state.lat_lon(0),
					 uav_state.lat_lon(1), traffic_state.lat_lon(0),
					 traffic_state.lat_lon(1), अन्तः_lat,
					 अन्तः_lon);

	ध्रुव तार्किक cs_distance_conflict_देहली = पङ्क्तिः_distance_valid
			&& (!crosstrack_error.past_अन्तः)
			&& (fabsf(crosstrack_error.distance) < _crosstrack_separation_m);

	ध्रुव भिन्न vertical_separation = fabsf(uav_state.altitude - traffic_state.altitude);
	ध्रुव तार्किक vertical_separation_conflict = vertical_separation < _vertical_separation_m;

	तार्किक collision_time_check = असत्य;

	ध्रुव भिन्न d_xyz = hypotf(d_hor, d_vert);
	भिन्न time_गन्तव्यम्_collision = 0.f;

	यदि (relative_uav_traffic_जवः > FLT_EPSILON) {
		time_गन्तव्यम्_collision = d_xyz / relative_uav_traffic_जवः;
		collision_time_check = time_गन्तव्यम्_collision < collision_time_देहली_s;
	}

	daa_stats.aircraft_dist = d_xyz;
	daa_stats.aircraft_dist_hor = पङ्क्तिः_distance_valid ? crosstrack_error.distance : d_hor;
	daa_stats.aircraft_dist_vert = vertical_separation;
	daa_stats.expected_min_dist_time_sec = time_गन्तव्यम्_collision;

	ध्रुव तार्किक conflict_detected = cs_distance_conflict_देहली && vertical_separation_conflict && collision_time_check;

	यदि (!conflict_detected) {
		प्रत्यागम detect_and_avoid_s::DAA_CONFLICT_LVL_NONE;
	}

	// Crosstrack mode has one conflict level, mapped to HIGH for compatibility.
	प्रत्यागम detect_and_avoid_s::DAA_CONFLICT_LVL_उच्चः;
}
