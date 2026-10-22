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
	ModuleParams(nullptr)
{
}

तार्किक DaaCrosstrack::try_setting_params()
{
	updateParams();

	स्थिरांक दशांश crosstrack_sep = _param_nav_traff_a_hor.get();
	स्थिरांक दशांश vertical_sep = _param_nav_traff_a_ver.get();
	स्थिरांक int32_t collision_time = _param_nav_traff_coll_t.get();

	स्थिरांक तार्किक crosstrack_ok = PX4_ISFINITE(crosstrack_sep) && crosstrack_sep > 0.f;
	स्थिरांक तार्किक vertical_ok = PX4_ISFINITE(vertical_sep) && vertical_sep > 0.f;
	स्थिरांक तार्किक collision_time_ok = collision_time > 0;

	जर (!(crosstrack_ok && vertical_ok && collision_time_ok)) {
		PX4_ERR("DAA: invalid crosstrack parameters");
		परत असत्य;
	}

	_crosstrack_separation_m = crosstrack_sep;
	_vertical_separation_m = vertical_sep;
	_collision_time_threshold_s = collision_time;
	परत सत्य;
}

uint8_t DaaCrosstrack::calculate_daa_stats(स्थिरांक aircraft_state_s &uav_state, स्थिरांक aircraft_state_s &traffic_state,
		daa_stats_s &daa_stats) स्थिरांक
{
	जर (!PX4_ISFINITE(traffic_state.heading)) {
		PX4_DEBUG("CRT lib: invalid traffic heading");
		daa_stats = {};
		परत detect_and_avoid_s::DAA_CONFLICT_LVL_NONE;
	}

	दशांश d_hor{0.f};
	दशांश d_vert{0.f};
	get_distance_to_point_global_wgs84(uav_state.lat_lon(0), uav_state.lat_lon(1), uav_state.altitude,
					   traffic_state.lat_lon(0), traffic_state.lat_lon(1), traffic_state.altitude, &d_hor, &d_vert);

	स्थिरांक दशांश relative_uav_traffic_speed = calculate_relative_uav_traffic_speed(uav_state, traffic_state);

	// Predict until the vehicle would have passed this system at its current speed
	स्थिरांक दशांश prediction_distance = d_hor + kTrafficToUavDistanceExtension;

	द्विगुण end_lat{0.0};
	द्विगुण end_lon{0.0};
	waypoint_from_heading_and_distance(traffic_state.lat_lon(0), traffic_state.lat_lon(1),
					   traffic_state.heading, prediction_distance, &end_lat, &end_lon);

	crosstrack_error_s crosstrack_error{};
	स्थिरांक तार्किक ओळ_distance_valid = !get_distance_to_ओळ(crosstrack_error, uav_state.lat_lon(0),
					 uav_state.lat_lon(1), traffic_state.lat_lon(0),
					 traffic_state.lat_lon(1), end_lat,
					 end_lon);

	स्थिरांक तार्किक cs_distance_conflict_threshold = ओळ_distance_valid
			&& (!crosstrack_error.past_end)
			&& (fabsf(crosstrack_error.distance) < _crosstrack_separation_m);

	स्थिरांक दशांश vertical_separation = fabsf(uav_state.altitude - traffic_state.altitude);
	स्थिरांक तार्किक vertical_separation_conflict = vertical_separation < _vertical_separation_m;

	तार्किक collision_time_check = असत्य;

	स्थिरांक दशांश d_xyz = hypotf(d_hor, d_vert);
	दशांश time_to_collision = 0.f;

	जर (relative_uav_traffic_speed > भग्न_अंतर) {
		time_to_collision = d_xyz / relative_uav_traffic_speed;
		collision_time_check = time_to_collision < _collision_time_threshold_s;
	}

	daa_stats.aircraft_dist = d_xyz;
	daa_stats.aircraft_dist_hor = ओळ_distance_valid ? crosstrack_error.distance : d_hor;
	daa_stats.aircraft_dist_vert = vertical_separation;
	daa_stats.expected_min_dist_time_sec = time_to_collision;

	स्थिरांक तार्किक conflict_detected = cs_distance_conflict_threshold && vertical_separation_conflict && collision_time_check;

	जर (!conflict_detected) {
		परत detect_and_avoid_s::DAA_CONFLICT_LVL_NONE;
	}

	// Crosstrack mode has one conflict level, mapped to HIGH for compatibility.
	परत detect_and_avoid_s::DAA_CONFLICT_LVL_वरचा;
}
