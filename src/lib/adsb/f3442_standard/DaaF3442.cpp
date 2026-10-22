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
 * @file AdsbF3442.cpp
 *
 * Helper class to do detect and avoid based on the ASTM F3442 standard
 *
 * @author Jonas Perolini <jonspero@me.com>
 */

#অন্তর্ভাব "DaaF3442.h"

#অন্তর্ভাব <float.h>

#অন্তর্ভাব <lib/geo/geo.h>
#অন্তর্ভাব <lib/mathlib/mathlib.h>
#অন্তর্ভাব <px4_platform_common/defines.h>
#অন্তর্ভাব <px4_platform_common/log.h>
#অন্তর্ভাব <uORB/topics/detect_and_avoid.h>

DaaF3442::DaaF3442() :
	ModuleParams(nullptr)
{
}

যৌক্তিক DaaF3442::try_setting_params()
{
	updateParams();

	ধ্রুবক matrix::Vector2f nmac_bounds{_param_daa_lvl_critical_rad.get(), _param_daa_lvl_critical_hgt.get()};
	ধ্রুবক matrix::Vector2f wc_bounds{param_daa_lvl_উপরের_rad.get(), param_daa_lvl_উপরের_hgt.get()};
	ধ্রুবক int32_t nmac_latency = _param_daa_lvl_medium_time.get();
	ধ্রুবক int32_t wc_latency = param_daa_lvl_নিচের_time.get();

	ধ্রুবক যৌক্তিক nmac_ok = nmac_bounds.isAllFinite() && nmac_bounds.min() > 0.f;
	ধ্রুবক যৌক্তিক wc_ok = wc_bounds.isAllFinite() && wc_bounds.min() > 0.f;
	ধ্রুবক যৌক্তিক ordering_ok = nmac_ok && wc_ok && (wc_bounds - nmac_bounds).min() >= 0.f;
	ধ্রুবক যৌক্তিক latencies_ok = nmac_latency >= 0 && wc_latency >= nmac_latency;

	যদি (!(nmac_ok && wc_ok && ordering_ok && latencies_ok)) {
		PX4_ERR("DAA: invalid F3442 parameters");
		ফেরত মিথ্যা;
	}

	_nmac_bounds_m = nmac_bounds;
	_wc_bounds_m = wc_bounds;
	_aug_nmac_latency_s = static_cast<দশমিক>(nmac_latency);
	_aug_wc_latency_s = static_cast<দশমিক>(wc_latency);

	ফেরত সত্য;
}

যৌক্তিক DaaF3442::is_in_bounds(ধ্রুবক matrix::Vector2f &distance, ধ্রুবক matrix::Vector2f &bounds)
{
	যদি (!bounds.isAllFinite() || bounds.min() < 0.f) {
		PX4_DEBUG("F34 bounds nan/neg");
		ফেরত মিথ্যা;
	}

	যদি (!distance.isAllFinite() || distance.min() < 0.f) {
		PX4_DEBUG("F34 dist nan/neg");
		ফেরত মিথ্যা;
	}

	ফেরত (distance - bounds).max() <= 0.f;
}

শূন্য DaaF3442::calculate_aircraft_conflict_volume(ধ্রুবক matrix::Vector2f &base_bounds,
		ধ্রুবক matrix::Vector2f &vel,
		ধ্রুবক দশমিক latency, matrix::Vector2f &augmented_bounds)
{
	// Symmetric (abs) to cover sudden course changes.
	augmented_bounds = base_bounds + vel.abs() * fabsf(latency);
}

শূন্য DaaF3442::calculate_augmented_boundaries(ধ্রুবক matrix::Vector2f &base_bounds,
		ধ্রুবক matrix::Vector2f &uav_vel_hor_vert,
		ধ্রুবক matrix::Vector2f &traffic_vel, ধ্রুবক দশমিক latency, matrix::Vector2f &augmented_bounds)
{
	matrix::Vector2f uav_bounds{};
	calculate_aircraft_conflict_volume(base_bounds, uav_vel_hor_vert, latency, uav_bounds);

	matrix::Vector2f traffic_bounds{};
	calculate_aircraft_conflict_volume(base_bounds, traffic_vel, latency, traffic_bounds);

	augmented_bounds = uav_bounds + traffic_bounds;
}

uint8_t DaaF3442::calculate_conflict_level(ধ্রুবক matrix::Vector2f &distance,
		ধ্রুবক matrix::Vector2f &uav_vel_hor_vert, ধ্রুবক matrix::Vector2f &traffic_vel) ধ্রুবক
{
	// Severity is imposed by evaluation order; HIGH and MEDIUM may overlap without containment.

	// Multiply by two because both aircraft have their respective NMAC bounds
	যদি (is_in_bounds(distance, 2.f * _nmac_bounds_m)) {
		PX4_DEBUG("F3442: NMAC breach");
		ফেরত detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL;
	}

	// Multiply by two because both aircraft have their respective WC bounds
	যদি (is_in_bounds(distance, 2.f * _wc_bounds_m)) {
		PX4_DEBUG("F3442: WC breach");
		ফেরত detect_and_avoid_s::DAA_CONFLICT_LVL_উপরের;
	}

	matrix::Vector2f aug_nmac_bounds{};
	calculate_augmented_boundaries(_nmac_bounds_m, uav_vel_hor_vert, traffic_vel, _aug_nmac_latency_s, aug_nmac_bounds);

	যদি (is_in_bounds(distance, aug_nmac_bounds)) {
		PX4_DEBUG("F3442: AUG_NMAC breach");
		ফেরত detect_and_avoid_s::DAA_CONFLICT_LVL_MEDIUM;
	}

	matrix::Vector2f aug_wc_bounds{};
	calculate_augmented_boundaries(_wc_bounds_m, uav_vel_hor_vert, traffic_vel, _aug_wc_latency_s, aug_wc_bounds);

	যদি (is_in_bounds(distance, aug_wc_bounds)) {
		PX4_DEBUG("F3442: AUG_WC breach");
		ফেরত detect_and_avoid_s::DAA_CONFLICT_LVL_নিচের;
	}

	ফেরত detect_and_avoid_s::DAA_CONFLICT_LVL_NONE;
}

uint8_t DaaF3442::calculate_daa_stats(ধ্রুবক aircraft_state_s &uav_state, ধ্রুবক aircraft_state_s &traffic_state,
				      daa_stats_s &daa_stats) ধ্রুবক
{
	দশমিক horizontal_dist{0.f};
	দশমিক vertical_dist{0.f};
	get_distance_to_point_global_wgs84(uav_state.lat_lon(0), uav_state.lat_lon(1), uav_state.altitude,
					   traffic_state.lat_lon(0), traffic_state.lat_lon(1), traffic_state.altitude, &horizontal_dist, &vertical_dist);

	ধ্রুবক দশমিক aircraft_dist = hypotf(horizontal_dist, vertical_dist);
	daa_stats.aircraft_dist = aircraft_dist;
	daa_stats.aircraft_dist_hor = horizontal_dist;
	daa_stats.aircraft_dist_vert = fabsf(vertical_dist);
	ধ্রুবক দশমিক relative_uav_traffic_speed = calculate_relative_uav_traffic_speed(uav_state, traffic_state);
	daa_stats.expected_min_dist_time_sec = relative_uav_traffic_speed > FLT_EPSILON ?
					       aircraft_dist / relative_uav_traffic_speed : 0.f;

	ধ্রুবক matrix::Vector2f distance(fabsf(horizontal_dist), fabsf(vertical_dist));
	ধ্রুবক matrix::Vector2f uav_vel_hor_vert = calculate_horizontal_vertical_speed_magnitudes(uav_state);
	ধ্রুবক matrix::Vector2f traffic_vel_hor_vert = calculate_horizontal_vertical_speed_magnitudes(traffic_state);

	ফেরত calculate_conflict_level(distance, uav_vel_hor_vert, traffic_vel_hor_vert);
}
