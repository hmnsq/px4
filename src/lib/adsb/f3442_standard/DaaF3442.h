/***************************************************************************
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
 * @file detect_and_avoid.h
 *
 * Helper class to do detect and avoid based on F3442 standard
 *
 * @author Jonas Perolini <jonspero@me.com>
 */

#pragma once

#অন্তর্ভাব "../DaaHelper.h"

#অন্তর্ভাব <px4_platform_common/module_params.h>

/**
 * @brief ASTM F3442 conflict evaluation.
 *
 * Evaluates NMAC (CRITICAL), Well-Clear (HIGH), and two augmented zones grown
 * by aircraft speed and a latency budget (MEDIUM and LOW). The first breached
 * zone in severity order defines the reported conflict level.
 */
class DaaF3442 : সর্বজনীন ModuleParams
{
সর্বজনীন:
	DaaF3442();

	// Distance, time-to-min-distance and the F3442 conflict level for one target.
	uint8_t calculate_daa_stats(ধ্রুবক aircraft_state_s &uav_state, ধ্রুবক aircraft_state_s &traffic_state,
				    daa_stats_s &daa_stats) ধ্রুবক;

	// True if every component of distance is within the symmetric bounds box.
	স্থির bool is_in_bounds(ধ্রুবক matrix::Vector2f &distance, ধ্রুবক matrix::Vector2f &bounds);

	/**
	 * @brief Expand a base bounds box by |vel| * |latency|.
	 *
	 * Result is symmetric so it covers a sudden course change regardless of @p vel sign.
	 */
	স্থির শূন্য calculate_aircraft_conflict_volume(ধ্রুবক matrix::Vector2f &base_bounds, ধ্রুবক matrix::Vector2f &vel,
			ধ্রুবক দশমিক latency, matrix::Vector2f &output_bounds);

	// Sum of the ownship and traffic augmented volumes for a base zone and latency.
	স্থির শূন্য calculate_augmented_boundaries(ধ্রুবক matrix::Vector2f &base_bounds, ধ্রুবক matrix::Vector2f &uav_vel_hor_vert,
			ধ্রুবক matrix::Vector2f &traffic_vel, ধ্রুবক দশমিক latency, matrix::Vector2f &augmented_bounds);

	// Evaluate the four zones in severity order and return the first breach.
	uint8_t calculate_conflict_level(ধ্রুবক matrix::Vector2f &distance, ধ্রুবক matrix::Vector2f &uav_vel_hor_vert,
					 ধ্রুবক matrix::Vector2f &traffic_vel) ধ্রুবক;

	/**
	 * @brief Refresh F3442 zone radii and latency from parameters.
	 *
	 * Rejects non-positive or non-finite bounds and settings that break the
	 * guaranteed NMAC-in-WC and augmented-MEDIUM-in-LOW containment.
	 */
	bool try_setting_params();

ব্যক্তিগত:
	// Per-aircraft half-zones (radius, height). They match the parameter defaults and the ASTM
	// F3442 zones once doubled in calculate_conflict_level(): NMAC = 2 x (153 m, 31 m),
	// Well-Clear = 2 x (610 m, 77 m). Overwritten by try_setting_params() on activation.
	matrix::Vector2f _nmac_bounds_m{77.f, 16.f};
	matrix::Vector2f _wc_bounds_m{305.f, 39.f};
	দশমিক _aug_nmac_latency_s{33.f};
	দশমিক _aug_wc_latency_s{33.f};

	DEFINE_PARAMETERS(
		(ParamFloat<px4::params::DAA_LVL_CRIT_RAD>) _param_daa_lvl_critical_rad,
		(ParamFloat<px4::params::DAA_LVL_CRIT_HGT>) _param_daa_lvl_critical_hgt,
		(ParamFloat<px4::params::DAA_LVL_উপরের_RAD>) param_daa_lvl_উপরের_rad,
		(ParamFloat<px4::params::DAA_LVL_উপরের_HGT>) param_daa_lvl_উপরের_hgt,
		(ParamInt<px4::params::DAA_LVL_MED_TIME>) _param_daa_lvl_medium_time,
		(ParamInt<px4::params::DAA_LVL_নিচের_TIME>) param_daa_lvl_নিচের_time
	)
};
