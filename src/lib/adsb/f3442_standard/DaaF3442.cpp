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

#ضمن "DaaF3442.h"

#ضمن <float.h>

#ضمن <lib/geo/geo.h>
#ضمن <lib/mathlib/mathlib.h>
#ضمن <px4_platform_common/defines.h>
#ضمن <px4_platform_common/log.h>
#ضمن <uORB/topics/detect_and_avoid.h>

DaaF3442::DaaF3442() :
	ModuleParams(خالی_سوچک)
{
}

منطقي DaaF3442::try_setting_params()
{
	updateParams();

	ثابت میٹرکس::Vector2f nmac_bounds{_param_daa_lvl_critical_rad.get(), _param_daa_lvl_critical_hgt.get()};
	ثابت میٹرکس::Vector2f wc_bounds{param_daa_lvl_بلند_rad.get(), param_daa_lvl_بلند_hgt.get()};
	ثابت int32_t nmac_latency = param_daa_lvl_medium_ویلا.get();
	ثابت int32_t wc_latency = param_daa_lvl_کم_ویلا.get();

	ثابت منطقي nmac_ok = nmac_bounds.isAllFinite() && nmac_bounds.min() > 0.f;
	ثابت منطقي wc_ok = wc_bounds.isAllFinite() && wc_bounds.min() > 0.f;
	ثابت منطقي ordering_ok = nmac_ok && wc_ok && (wc_bounds - nmac_bounds).min() >= 0.f;
	ثابت منطقي latencies_ok = nmac_latency >= 0 && wc_latency >= nmac_latency;

	إذا (!(nmac_ok && wc_ok && ordering_ok && latencies_ok)) {
		PX4_ERR("DAA: invalid F3442 parameters");
		أرجع خطأ;
	}

	_nmac_bounds_m = nmac_bounds;
	_wc_bounds_m = wc_bounds;
	_aug_nmac_latency_s = static_cast<عائم>(nmac_latency);
	_aug_wc_latency_s = static_cast<عائم>(wc_latency);

	أرجع سچ;
}

منطقي DaaF3442::is_وچ_bounds(ثابت میٹرکس::Vector2f &فاصلہ, ثابت میٹرکس::Vector2f &bounds)
{
	إذا (!bounds.isAllFinite() || bounds.min() < 0.f) {
		PX4_DEBUG("F34 bounds nan/neg");
		أرجع خطأ;
	}

	إذا (!فاصلہ.isAllFinite() || فاصلہ.min() < 0.f) {
		PX4_DEBUG("F34 dist nan/neg");
		أرجع خطأ;
	}

	أرجع (فاصلہ - bounds).max() <= 0.f;
}

فراغ DaaF3442::calculate_aircraft_conflict_volume(ثابت میٹرکس::Vector2f &ادھار_bounds,
		ثابت میٹرکس::Vector2f &vel,
		ثابت عائم latency, میٹرکس::Vector2f &augmented_bounds)
{
	// Symmetric (abs) to cover sudden course changes.
	augmented_bounds = ادھار_bounds + vel.مطلق_قدر() * fabsf(latency);
}

فراغ DaaF3442::calculate_augmented_boundaries(ثابت میٹرکس::Vector2f &ادھار_bounds,
		ثابت میٹرکس::Vector2f &uav_vel_hor_vert,
		ثابت میٹرکس::Vector2f &traffic_vel, ثابت عائم latency, میٹرکس::Vector2f &augmented_bounds)
{
	میٹرکس::Vector2f uav_bounds{};
	calculate_aircraft_conflict_volume(ادھار_bounds, uav_vel_hor_vert, latency, uav_bounds);

	میٹرکس::Vector2f traffic_bounds{};
	calculate_aircraft_conflict_volume(ادھار_bounds, traffic_vel, latency, traffic_bounds);

	augmented_bounds = uav_bounds + traffic_bounds;
}

uint8_t DaaF3442::calculate_conflict_level(ثابت میٹرکس::Vector2f &فاصلہ,
		ثابت میٹرکس::Vector2f &uav_vel_hor_vert, ثابت میٹرکس::Vector2f &traffic_vel) ثابت
{
	// Severity is imposed by evaluation order; HIGH and MEDIUM may overlap without containment.

	// Multiply by two because both aircraft have their respective NMAC bounds
	إذا (is_وچ_bounds(فاصلہ, 2.f * _nmac_bounds_m)) {
		PX4_DEBUG("F3442: NMAC breach");
		أرجع detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL;
	}

	// Multiply by two because both aircraft have their respective WC bounds
	إذا (is_وچ_bounds(فاصلہ, 2.f * _wc_bounds_m)) {
		PX4_DEBUG("F3442: WC breach");
		أرجع detect_and_avoid_s::DAA_CONFLICT_LVL_بلند;
	}

	میٹرکس::Vector2f aug_nmac_bounds{};
	calculate_augmented_boundaries(_nmac_bounds_m, uav_vel_hor_vert, traffic_vel, _aug_nmac_latency_s, aug_nmac_bounds);

	إذا (is_وچ_bounds(فاصلہ, aug_nmac_bounds)) {
		PX4_DEBUG("F3442: AUG_NMAC breach");
		أرجع detect_and_avoid_s::DAA_CONFLICT_LVL_MEDIUM;
	}

	میٹرکس::Vector2f aug_wc_bounds{};
	calculate_augmented_boundaries(_wc_bounds_m, uav_vel_hor_vert, traffic_vel, _aug_wc_latency_s, aug_wc_bounds);

	إذا (is_وچ_bounds(فاصلہ, aug_wc_bounds)) {
		PX4_DEBUG("F3442: AUG_WC breach");
		أرجع detect_and_avoid_s::DAA_CONFLICT_LVL_کم;
	}

	أرجع detect_and_avoid_s::DAA_CONFLICT_LVL_NONE;
}

uint8_t DaaF3442::calculate_daa_stats(ثابت aircraft_state_s &uav_state, ثابت aircraft_state_s &traffic_state,
				      daa_stats_s &daa_stats) ثابت
{
	عائم horizontal_dist{0.f};
	عائم vertical_dist{0.f};
	get_فاصلہ_منزل_نقطہ_global_wgs84(uav_state.lat_lon(0), uav_state.lat_lon(1), uav_state.altitude,
					   traffic_state.lat_lon(0), traffic_state.lat_lon(1), traffic_state.altitude, &horizontal_dist, &vertical_dist);

	ثابت عائم aircraft_dist = hypotf(horizontal_dist, vertical_dist);
	daa_stats.aircraft_dist = aircraft_dist;
	daa_stats.aircraft_dist_hor = horizontal_dist;
	daa_stats.aircraft_dist_vert = fabsf(vertical_dist);
	ثابت عائم relative_uav_traffic_رفتار = calculate_relative_uav_traffic_رفتار(uav_state, traffic_state);
	daa_stats.expected_min_dist_ویلا_sec = relative_uav_traffic_رفتار > FLT_EPSILON ?
					       aircraft_dist / relative_uav_traffic_رفتار : 0.f;

	ثابت میٹرکس::Vector2f فاصلہ(fabsf(horizontal_dist), fabsf(vertical_dist));
	ثابت میٹرکس::Vector2f uav_vel_hor_vert = calculate_horizontal_vertical_رفتار_magnitudes(uav_state);
	ثابت میٹرکس::Vector2f traffic_vel_hor_vert = calculate_horizontal_vertical_رفتار_magnitudes(traffic_state);

	أرجع calculate_conflict_level(فاصلہ, uav_vel_hor_vert, traffic_vel_hor_vert);
}
