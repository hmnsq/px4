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

#అన్తర్భావ "DaaF3442.h"

#అన్తర్భావ <float.h>

#అన్తర్భావ <lib/geo/geo.h>
#అన్తర్భావ <lib/mathlib/mathlib.h>
#అన్తర్భావ <px4_platform_common/defines.h>
#అన్తర్భావ <px4_platform_common/log.h>
#అన్తర్భావ <uORB/topics/detect_and_avoid.h>

DaaF3442::DaaF3442() :
	ModuleParams(శూన్యసూచిక)
{
}

తార్కిక DaaF3442::try_setting_params()
{
	updateParams();

	స్థిరరాశి మాతృక::Vector2f nmac_bounds{_param_daa_lvl_critical_rad.get(), _param_daa_lvl_critical_hgt.get()};
	స్థిరరాశి మాతృక::Vector2f wc_bounds{param_daa_lvl_ఎగువ_rad.get(), param_daa_lvl_ఎగువ_hgt.get()};
	స్థిరరాశి int32_t nmac_latency = param_daa_lvl_medium_సమయం.get();
	స్థిరరాశి int32_t wc_latency = param_daa_lvl_దిగువ_సమయం.get();

	స్థిరరాశి తార్కిక nmac_ok = nmac_bounds.isAllFinite() && nmac_bounds.min() > 0.f;
	స్థిరరాశి తార్కిక wc_ok = wc_bounds.isAllFinite() && wc_bounds.min() > 0.f;
	స్థిరరాశి తార్కిక ordering_ok = nmac_ok && wc_ok && (wc_bounds - nmac_bounds).min() >= 0.f;
	స్థిరరాశి తార్కిక latencies_ok = nmac_latency >= 0 && wc_latency >= nmac_latency;

	ఒకవేళ (!(nmac_ok && wc_ok && ordering_ok && latencies_ok)) {
		PX4_ERR("DAA: invalid F3442 parameters");
		తిరిగి అబద్ధం;
	}

	_nmac_bounds_m = nmac_bounds;
	_wc_bounds_m = wc_bounds;
	_aug_nmac_latency_s = static_cast<దశాంశం>(nmac_latency);
	_aug_wc_latency_s = static_cast<దశాంశం>(wc_latency);

	తిరిగి నిజం;
}

తార్కిక DaaF3442::is_లోపల_bounds(స్థిరరాశి మాతృక::Vector2f &దూరం, స్థిరరాశి మాతృక::Vector2f &bounds)
{
	ఒకవేళ (!bounds.isAllFinite() || bounds.min() < 0.f) {
		PX4_DEBUG("F34 bounds nan/neg");
		తిరిగి అబద్ధం;
	}

	ఒకవేళ (!దూరం.isAllFinite() || దూరం.min() < 0.f) {
		PX4_DEBUG("F34 dist nan/neg");
		తిరిగి అబద్ధం;
	}

	తిరిగి (దూరం - bounds).max() <= 0.f;
}

శూన్య DaaF3442::calculate_aircraft_conflict_volume(స్థిరరాశి మాతృక::Vector2f &పునాది_bounds,
		స్థిరరాశి మాతృక::Vector2f &vel,
		స్థిరరాశి దశాంశం latency, మాతృక::Vector2f &augmented_bounds)
{
	// Symmetric (abs) to cover sudden course changes.
	augmented_bounds = పునాది_bounds + vel.నిరపేక్ష() * fabsf(latency);
}

శూన్య DaaF3442::calculate_augmented_boundaries(స్థిరరాశి మాతృక::Vector2f &పునాది_bounds,
		స్థిరరాశి మాతృక::Vector2f &uav_vel_hor_vert,
		స్థిరరాశి మాతృక::Vector2f &traffic_vel, స్థిరరాశి దశాంశం latency, మాతృక::Vector2f &augmented_bounds)
{
	మాతృక::Vector2f uav_bounds{};
	calculate_aircraft_conflict_volume(పునాది_bounds, uav_vel_hor_vert, latency, uav_bounds);

	మాతృక::Vector2f traffic_bounds{};
	calculate_aircraft_conflict_volume(పునాది_bounds, traffic_vel, latency, traffic_bounds);

	augmented_bounds = uav_bounds + traffic_bounds;
}

uint8_t DaaF3442::calculate_conflict_level(స్థిరరాశి మాతృక::Vector2f &దూరం,
		స్థిరరాశి మాతృక::Vector2f &uav_vel_hor_vert, స్థిరరాశి మాతృక::Vector2f &traffic_vel) స్థిరరాశి
{
	// Severity is imposed by evaluation order; HIGH and MEDIUM may overlap without containment.

	// Multiply by two because both aircraft have their respective NMAC bounds
	ఒకవేళ (is_లోపల_bounds(దూరం, 2.f * _nmac_bounds_m)) {
		PX4_DEBUG("F3442: NMAC breach");
		తిరిగి detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL;
	}

	// Multiply by two because both aircraft have their respective WC bounds
	ఒకవేళ (is_లోపల_bounds(దూరం, 2.f * _wc_bounds_m)) {
		PX4_DEBUG("F3442: WC breach");
		తిరిగి detect_and_avoid_s::DAA_CONFLICT_LVL_ఎగువ;
	}

	మాతృక::Vector2f aug_nmac_bounds{};
	calculate_augmented_boundaries(_nmac_bounds_m, uav_vel_hor_vert, traffic_vel, _aug_nmac_latency_s, aug_nmac_bounds);

	ఒకవేళ (is_లోపల_bounds(దూరం, aug_nmac_bounds)) {
		PX4_DEBUG("F3442: AUG_NMAC breach");
		తిరిగి detect_and_avoid_s::DAA_CONFLICT_LVL_MEDIUM;
	}

	మాతృక::Vector2f aug_wc_bounds{};
	calculate_augmented_boundaries(_wc_bounds_m, uav_vel_hor_vert, traffic_vel, _aug_wc_latency_s, aug_wc_bounds);

	ఒకవేళ (is_లోపల_bounds(దూరం, aug_wc_bounds)) {
		PX4_DEBUG("F3442: AUG_WC breach");
		తిరిగి detect_and_avoid_s::DAA_CONFLICT_LVL_దిగువ;
	}

	తిరిగి detect_and_avoid_s::DAA_CONFLICT_LVL_NONE;
}

uint8_t DaaF3442::calculate_daa_stats(స్థిరరాశి aircraft_state_s &uav_state, స్థిరరాశి aircraft_state_s &traffic_state,
				      daa_stats_s &daa_stats) స్థిరరాశి
{
	దశాంశం horizontal_dist{0.f};
	దశాంశం vertical_dist{0.f};
	get_దూరం_గమ్యం_బిందువు_global_wgs84(uav_state.lat_lon(0), uav_state.lat_lon(1), uav_state.altitude,
					   traffic_state.lat_lon(0), traffic_state.lat_lon(1), traffic_state.altitude, &horizontal_dist, &vertical_dist);

	స్థిరరాశి దశాంశం aircraft_dist = hypotf(horizontal_dist, vertical_dist);
	daa_stats.aircraft_dist = aircraft_dist;
	daa_stats.aircraft_dist_hor = horizontal_dist;
	daa_stats.aircraft_dist_vert = fabsf(vertical_dist);
	స్థిరరాశి దశాంశం relative_uav_traffic_వేగం = calculate_relative_uav_traffic_వేగం(uav_state, traffic_state);
	daa_stats.expected_min_dist_సమయం_sec = relative_uav_traffic_వేగం > FLT_EPSILON ?
					       aircraft_dist / relative_uav_traffic_వేగం : 0.f;

	స్థిరరాశి మాతృక::Vector2f దూరం(fabsf(horizontal_dist), fabsf(vertical_dist));
	స్థిరరాశి మాతృక::Vector2f uav_vel_hor_vert = calculate_horizontal_vertical_వేగం_magnitudes(uav_state);
	స్థిరరాశి మాతృక::Vector2f traffic_vel_hor_vert = calculate_horizontal_vertical_వేగం_magnitudes(traffic_state);

	తిరిగి calculate_conflict_level(దూరం, uav_vel_hor_vert, traffic_vel_hor_vert);
}
