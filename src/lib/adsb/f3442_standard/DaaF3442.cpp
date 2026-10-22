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

#ਅਨ੍ਤਰ੍ਭਾਵ "DaaF3442.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <float.h>

#ਅਨ੍ਤਰ੍ਭਾਵ <lib/geo/geo.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <lib/mathlib/mathlib.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <px4_platform_common/defines.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <px4_platform_common/log.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <uORB/topics/detect_and_avoid.h>

DaaF3442::DaaF3442() :
	ModuleParams(ਖਾਲੀ_ਸੂਚਕ)
{
}

ਤਾਰਕਿਕ DaaF3442::try_setting_params()
{
	updateParams();

	ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector2f nmac_bounds{_param_daa_lvl_critical_rad.get(), _param_daa_lvl_critical_hgt.get()};
	ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector2f wc_bounds{param_daa_lvl_ਉੱਚਾ_rad.get(), param_daa_lvl_ਉੱਚਾ_hgt.get()};
	ਅਚਲ int32_t nmac_latency = param_daa_lvl_medium_ਸਮਾਂ.get();
	ਅਚਲ int32_t wc_latency = param_daa_lvl_ਨੀਵਾਂ_ਸਮਾਂ.get();

	ਅਚਲ ਤਾਰਕਿਕ nmac_ok = nmac_bounds.isAllFinite() && nmac_bounds.min() > 0.f;
	ਅਚਲ ਤਾਰਕਿਕ wc_ok = wc_bounds.isAllFinite() && wc_bounds.min() > 0.f;
	ਅਚਲ ਤਾਰਕਿਕ ordering_ok = nmac_ok && wc_ok && (wc_bounds - nmac_bounds).min() >= 0.f;
	ਅਚਲ ਤਾਰਕਿਕ latencies_ok = nmac_latency >= 0 && wc_latency >= nmac_latency;

	ਜੇ (!(nmac_ok && wc_ok && ordering_ok && latencies_ok)) {
		PX4_ERR("DAA: invalid F3442 parameters");
		ਵਾਪਸ ਝੂਠ;
	}

	_nmac_bounds_m = nmac_bounds;
	_wc_bounds_m = wc_bounds;
	_aug_nmac_latency_s = static_cast<ਦਸ਼ਮਲਵ>(nmac_latency);
	_aug_wc_latency_s = static_cast<ਦਸ਼ਮਲਵ>(wc_latency);

	ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ DaaF3442::is_ਵਿੱਚ_bounds(ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector2f &ਦੂਰੀ, ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector2f &bounds)
{
	ਜੇ (!bounds.isAllFinite() || bounds.min() < 0.f) {
		PX4_DEBUG("F34 bounds nan/neg");
		ਵਾਪਸ ਝੂਠ;
	}

	ਜੇ (!ਦੂਰੀ.isAllFinite() || ਦੂਰੀ.min() < 0.f) {
		PX4_DEBUG("F34 dist nan/neg");
		ਵਾਪਸ ਝੂਠ;
	}

	ਵਾਪਸ (ਦੂਰੀ - bounds).max() <= 0.f;
}

ਖਾਲੀ DaaF3442::calculate_aircraft_conflict_volume(ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector2f &ਅਧਾਰ_bounds,
		ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector2f &vel,
		ਅਚਲ ਦਸ਼ਮਲਵ latency, ਮੈਟ੍ਰਿਕਸ::Vector2f &augmented_bounds)
{
	// Symmetric (abs) to cover sudden course changes.
	augmented_bounds = ਅਧਾਰ_bounds + vel.ਨਿਰਪੇਖ() * fabsf(latency);
}

ਖਾਲੀ DaaF3442::calculate_augmented_boundaries(ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector2f &ਅਧਾਰ_bounds,
		ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector2f &uav_vel_hor_vert,
		ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector2f &traffic_vel, ਅਚਲ ਦਸ਼ਮਲਵ latency, ਮੈਟ੍ਰਿਕਸ::Vector2f &augmented_bounds)
{
	ਮੈਟ੍ਰਿਕਸ::Vector2f uav_bounds{};
	calculate_aircraft_conflict_volume(ਅਧਾਰ_bounds, uav_vel_hor_vert, latency, uav_bounds);

	ਮੈਟ੍ਰਿਕਸ::Vector2f traffic_bounds{};
	calculate_aircraft_conflict_volume(ਅਧਾਰ_bounds, traffic_vel, latency, traffic_bounds);

	augmented_bounds = uav_bounds + traffic_bounds;
}

uint8_t DaaF3442::calculate_conflict_level(ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector2f &ਦੂਰੀ,
		ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector2f &uav_vel_hor_vert, ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector2f &traffic_vel) ਅਚਲ
{
	// Severity is imposed by evaluation order; HIGH and MEDIUM may overlap without containment.

	// Multiply by two because both aircraft have their respective NMAC bounds
	ਜੇ (is_ਵਿੱਚ_bounds(ਦੂਰੀ, 2.f * _nmac_bounds_m)) {
		PX4_DEBUG("F3442: NMAC breach");
		ਵਾਪਸ detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL;
	}

	// Multiply by two because both aircraft have their respective WC bounds
	ਜੇ (is_ਵਿੱਚ_bounds(ਦੂਰੀ, 2.f * _wc_bounds_m)) {
		PX4_DEBUG("F3442: WC breach");
		ਵਾਪਸ detect_and_avoid_s::DAA_CONFLICT_LVL_ਉੱਚਾ;
	}

	ਮੈਟ੍ਰਿਕਸ::Vector2f aug_nmac_bounds{};
	calculate_augmented_boundaries(_nmac_bounds_m, uav_vel_hor_vert, traffic_vel, _aug_nmac_latency_s, aug_nmac_bounds);

	ਜੇ (is_ਵਿੱਚ_bounds(ਦੂਰੀ, aug_nmac_bounds)) {
		PX4_DEBUG("F3442: AUG_NMAC breach");
		ਵਾਪਸ detect_and_avoid_s::DAA_CONFLICT_LVL_MEDIUM;
	}

	ਮੈਟ੍ਰਿਕਸ::Vector2f aug_wc_bounds{};
	calculate_augmented_boundaries(_wc_bounds_m, uav_vel_hor_vert, traffic_vel, _aug_wc_latency_s, aug_wc_bounds);

	ਜੇ (is_ਵਿੱਚ_bounds(ਦੂਰੀ, aug_wc_bounds)) {
		PX4_DEBUG("F3442: AUG_WC breach");
		ਵਾਪਸ detect_and_avoid_s::DAA_CONFLICT_LVL_ਨੀਵਾਂ;
	}

	ਵਾਪਸ detect_and_avoid_s::DAA_CONFLICT_LVL_NONE;
}

uint8_t DaaF3442::calculate_daa_stats(ਅਚਲ aircraft_state_s &uav_state, ਅਚਲ aircraft_state_s &traffic_state,
				      daa_stats_s &daa_stats) ਅਚਲ
{
	ਦਸ਼ਮਲਵ horizontal_dist{0.f};
	ਦਸ਼ਮਲਵ vertical_dist{0.f};
	get_ਦੂਰੀ_ਮੰਜ਼ਿਲ_ਬਿੰਦੂ_global_wgs84(uav_state.lat_lon(0), uav_state.lat_lon(1), uav_state.altitude,
					   traffic_state.lat_lon(0), traffic_state.lat_lon(1), traffic_state.altitude, &horizontal_dist, &vertical_dist);

	ਅਚਲ ਦਸ਼ਮਲਵ aircraft_dist = hypotf(horizontal_dist, vertical_dist);
	daa_stats.aircraft_dist = aircraft_dist;
	daa_stats.aircraft_dist_hor = horizontal_dist;
	daa_stats.aircraft_dist_vert = fabsf(vertical_dist);
	ਅਚਲ ਦਸ਼ਮਲਵ relative_uav_traffic_ਰਫਤਾਰ = calculate_relative_uav_traffic_ਰਫਤਾਰ(uav_state, traffic_state);
	daa_stats.expected_min_dist_ਸਮਾਂ_sec = relative_uav_traffic_ਰਫਤਾਰ > FLT_EPSILON ?
					       aircraft_dist / relative_uav_traffic_ਰਫਤਾਰ : 0.f;

	ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector2f ਦੂਰੀ(fabsf(horizontal_dist), fabsf(vertical_dist));
	ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector2f uav_vel_hor_vert = calculate_horizontal_vertical_ਰਫਤਾਰ_magnitudes(uav_state);
	ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector2f traffic_vel_hor_vert = calculate_horizontal_vertical_ਰਫਤਾਰ_magnitudes(traffic_state);

	ਵਾਪਸ calculate_conflict_level(ਦੂਰੀ, uav_vel_hor_vert, traffic_vel_hor_vert);
}
