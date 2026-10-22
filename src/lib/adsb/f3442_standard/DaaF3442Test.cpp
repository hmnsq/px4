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
 * @file DaaF3442Test.cpp
 *
 * Test file for the F3442 DAA standard
 *
 * @author Jonas Perolini <jonspero@me.com>
 */

#અન્તર્ભાવ <gtest/gtest.h>
#અન્તર્ભાવ <parameters/param.h>

#અન્તર્ભાવ <array>
#અન્તર્ભાવ <lib/geo/geo.h>
#અન્તર્ભાવ <lib/mathlib/mathlib.h>
#અન્તર્ભાવ <limits>
#અન્તર્ભાવ <uORB/topics/detect_and_avoid.h>

#અન્તર્ભાવ "DaaF3442.h"

વર્ગ DaaF3442Test : જાહેર ::testing::Test
{
સુરક્ષિત:
	DaaF3442 daa;

	શૂન્ય SetUp() override
	{
		param_control_autosave(ખોટું);
		param_reset_all();
	}

	શૂન્ય set_f3442_params(દશાંશ nmac_radius, દશાંશ nmac_height, દશાંશ wc_radius, દશાંશ wc_height,
			      પૂર્ણાંક nmac_latency_s, પૂર્ણાંક wc_latency_s)
	{
		param_set(param_handle(px4::params::DAA_LVL_CRIT_RAD), &nmac_radius);
		param_set(param_handle(px4::params::DAA_LVL_CRIT_HGT), &nmac_height);
		param_set(param_handle(px4::params::DAA_LVL_ઉપલું_RAD), &wc_radius);
		param_set(param_handle(px4::params::DAA_LVL_ઉપલું_HGT), &wc_height);
		param_set(param_handle(px4::params::DAA_LVL_MED_TIME), &nmac_latency_s);
		param_set(param_handle(px4::params::DAA_LVL_નીચલું_TIME), &wc_latency_s);
	}
};

TEST_F(DaaF3442Test, RejectsInvalidParams)
{
	સંરચના ParamCase {
		દશાંશ nmac_radius;
		દશાંશ nmac_height;
		દશાંશ wc_radius;
		દશાંશ wc_height;
		પૂર્ણાંક nmac_latency_s;
		પૂર્ણાંક wc_latency_s;
		તાર્કિક expected_result;
	};

	constexpr દશાંશ valid_nmac_radius_m = 10.f;
	constexpr દશાંશ valid_nmac_height_m = 10.f;
	constexpr દશાંશ valid_wc_radius_m = 20.f;
	constexpr દશાંશ valid_wc_height_m = 20.f;
	constexpr પૂર્ણાંક valid_nmac_latency_s = 5;
	constexpr પૂર્ણાંક valid_wc_latency_s = 5;
	અચલ દશાંશ nan = std::numeric_limits<દશાંશ>::quiet_NaN();
	અચલ દશાંશ inf = std::numeric_limits<દશાંશ>::infinity();

	અચલ std::array<ParamCase, 13> cases{{
			{valid_nmac_radius_m, valid_nmac_height_m, valid_wc_radius_m, valid_wc_height_m, valid_nmac_latency_s, valid_wc_latency_s, સાચું},
			{0.f, valid_nmac_height_m, valid_wc_radius_m, valid_wc_height_m, valid_nmac_latency_s, valid_wc_latency_s, ખોટું},
			{-valid_nmac_radius_m, valid_nmac_height_m, valid_wc_radius_m, valid_wc_height_m, valid_nmac_latency_s, valid_wc_latency_s, ખોટું},
			{valid_nmac_radius_m, -valid_nmac_height_m, valid_wc_radius_m, valid_wc_height_m, valid_nmac_latency_s, valid_wc_latency_s, ખોટું},
			{valid_nmac_radius_m, valid_nmac_height_m, -valid_wc_radius_m, valid_wc_height_m, valid_nmac_latency_s, valid_wc_latency_s, ખોટું},
			{valid_nmac_radius_m, valid_nmac_height_m, valid_wc_radius_m, -valid_wc_height_m, valid_nmac_latency_s, valid_wc_latency_s, ખોટું},
			{nan, valid_nmac_height_m, valid_wc_radius_m, valid_wc_height_m, valid_nmac_latency_s, valid_wc_latency_s, ખોટું},
			{valid_nmac_radius_m, inf, valid_wc_radius_m, valid_wc_height_m, valid_nmac_latency_s, valid_wc_latency_s, ખોટું},
			{valid_nmac_radius_m, valid_nmac_height_m, nan, valid_wc_height_m, valid_nmac_latency_s, valid_wc_latency_s, ખોટું},
			{valid_nmac_radius_m, valid_nmac_height_m, valid_wc_radius_m, inf, valid_nmac_latency_s, valid_wc_latency_s, ખોટું},
			{valid_nmac_radius_m, valid_nmac_height_m, valid_wc_radius_m, valid_wc_height_m, -valid_nmac_latency_s, valid_wc_latency_s, ખોટું},
			{valid_nmac_radius_m, valid_nmac_height_m, valid_wc_radius_m, valid_wc_height_m, valid_nmac_latency_s, -valid_wc_latency_s, ખોટું},
			{valid_nmac_radius_m, valid_nmac_height_m, valid_wc_radius_m, valid_wc_height_m, valid_nmac_latency_s + 1, valid_wc_latency_s, ખોટું},
		}};

	માટે (size_t i = 0; i < cases.size(); ++i) {
		અચલ ParamCase &test_case = cases[i];
		SCOPED_TRACE(i);

		set_f3442_params(test_case.nmac_radius, test_case.nmac_height, test_case.wc_radius, test_case.wc_height,
				 test_case.nmac_latency_s, test_case.wc_latency_s);
		EXPECT_EQ(daa.try_setting_params(), test_case.expected_result);
	}
}

// Bounds must nest: an outer zone no larger than the inner one is rejected.
TEST_F(DaaF3442Test, RejectsNonHierarchicalBounds)
{
	સંરચના HierarchyCase {
		દશાંશ nmac_radius;
		દશાંશ nmac_height;
		દશાંશ wc_radius;
		દશાંશ wc_height;
	};

	constexpr પૂર્ણાંક nmac_latency_s = 30;
	constexpr પૂર્ણાંક wc_latency_s = 30;
	અચલ std::array<HierarchyCase, 3> cases{{
			{20.f, 10.f, 19.f, 10.f},
			{20.f, 10.f, 20.f, 9.f},
			{200.f, 100.f, 100.f, 50.f},
		}};

	માટે (size_t i = 0; i < cases.size(); ++i) {
		અચલ HierarchyCase &test_case = cases[i];
		SCOPED_TRACE(i);

		set_f3442_params(test_case.nmac_radius, test_case.nmac_height, test_case.wc_radius, test_case.wc_height,
				 nmac_latency_s, wc_latency_s);

		EXPECT_FALSE(daa.try_setting_params());
	}
}

// is_in_bounds() is inclusive and fails on negative or non-finite inputs.
TEST_F(DaaF3442Test, IsInBoundsRejectsInvalidInputs)
{
	અચલ matrix::Vector2f bounds(10.f, 10.f);
	અચલ matrix::Vector2f within_bounds(5.f, 5.f);
	અચલ matrix::Vector2f on_boundary(10.f, 10.f);
	અચલ matrix::Vector2f outside_both_bounds(15.f, 15.f);
	અચલ matrix::Vector2f outside_horizontal_bounds(15.f, 4.f);
	અચલ matrix::Vector2f outside_vertical_bounds(4.f, 15.f);
	અચલ matrix::Vector2f negative_bounds(-10.f, 10.f);
	અચલ matrix::Vector2f negative_distance(-5.f, 5.f);
	અચલ matrix::Vector2f non_finite_distance(5.f, std::numeric_limits<દશાંશ>::quiet_NaN());

	EXPECT_TRUE(daa.is_in_bounds(within_bounds, bounds));
	EXPECT_TRUE(daa.is_in_bounds(on_boundary, bounds));
	EXPECT_FALSE(daa.is_in_bounds(outside_both_bounds, bounds));
	EXPECT_FALSE(daa.is_in_bounds(outside_horizontal_bounds, bounds));
	EXPECT_FALSE(daa.is_in_bounds(outside_vertical_bounds, bounds));
	EXPECT_FALSE(daa.is_in_bounds(within_bounds, negative_bounds));
	EXPECT_FALSE(daa.is_in_bounds(negative_distance, bounds));
	EXPECT_FALSE(daa.is_in_bounds(non_finite_distance, bounds));
}

// Volume augmentation uses |velocity| and |latency|, so signs never shrink it.
TEST_F(DaaF3442Test, CalculateAircraftConflictVolumeUsesAbsoluteInputs)
{
	અચલ matrix::Vector2f base_bounds(10.f, 20.f);
	અચલ matrix::Vector2f velocity(-2.f, 4.f);
	અચલ દશાંશ latency_s = -5.f;
	અચલ દશાંશ expected_horizontal_bounds_m = base_bounds(0) + fabsf(velocity(0)) * fabsf(latency_s);
	અચલ દશાંશ expected_vertical_bounds_m = base_bounds(1) + fabsf(velocity(1)) * fabsf(latency_s);

	matrix::Vector2f augmented_bounds{};
	daa.calculate_aircraft_conflict_volume(base_bounds, velocity, latency_s, augmented_bounds);

	EXPECT_FLOAT_EQ(augmented_bounds(0), expected_horizontal_bounds_m);
	EXPECT_FLOAT_EQ(augmented_bounds(1), expected_vertical_bounds_m);
}

// Augmented boundary sums both aircraft's volumes (motion from each side).
TEST_F(DaaF3442Test, CalculateAugmentedBoundariesAddsBothAircraftVolumes)
{
	અચલ matrix::Vector2f base_bounds(10.f, 20.f);
	અચલ matrix::Vector2f uav_velocity(2.f, 4.f);
	અચલ matrix::Vector2f traffic_velocity(8.f, 10.f);
	અચલ દશાંશ latency_s = 3.f;
	અચલ દશાંશ expected_horizontal_bounds_m = 2.f * base_bounds(0)
			+ (fabsf(uav_velocity(0)) + fabsf(traffic_velocity(0))) * latency_s;
	અચલ દશાંશ expected_vertical_bounds_m = 2.f * base_bounds(1)
			+ (fabsf(uav_velocity(1)) + fabsf(traffic_velocity(1))) * latency_s;

	matrix::Vector2f augmented_bounds{};
	daa.calculate_augmented_boundaries(base_bounds, uav_velocity, traffic_velocity, latency_s, augmented_bounds);

	EXPECT_FLOAT_EQ(augmented_bounds(0), expected_horizontal_bounds_m);
	EXPECT_FLOAT_EQ(augmented_bounds(1), expected_vertical_bounds_m);
}

// calculate_conflict_level() returns the strictest breached zone; a single-axis miss is NONE.
TEST_F(DaaF3442Test, CalculateConflictLevelRespectsPriorityOrder)
{
	constexpr દશાંશ nmac_radius_m = 20.f;
	constexpr દશાંશ nmac_height_m = 10.f;
	constexpr દશાંશ wc_radius_m = 100.f;
	constexpr દશાંશ wc_height_m = 50.f;
	constexpr પૂર્ણાંક nmac_latency_s = 30;
	constexpr પૂર્ણાંક wc_latency_s = 40;
	set_f3442_params(nmac_radius_m, nmac_height_m, wc_radius_m, wc_height_m, nmac_latency_s, wc_latency_s);
	ASSERT_TRUE(daa.try_setting_params());

	અચલ matrix::Vector2f uav_velocity(10.f, 2.f);
	અચલ matrix::Vector2f traffic_velocity(5.f, 4.f);
	અચલ matrix::Vector2f nmac_bounds(2.f * nmac_radius_m, 2.f * nmac_height_m);
	અચલ matrix::Vector2f wc_bounds(2.f * wc_radius_m, 2.f * wc_height_m);
	અચલ matrix::Vector2f aug_nmac_bounds(
		2.f * nmac_radius_m + (fabsf(uav_velocity(0)) + fabsf(traffic_velocity(0))) * nmac_latency_s,
		2.f * nmac_height_m + (fabsf(uav_velocity(1)) + fabsf(traffic_velocity(1))) * nmac_latency_s);
	અચલ matrix::Vector2f aug_wc_bounds(
		2.f * wc_radius_m + (fabsf(uav_velocity(0)) + fabsf(traffic_velocity(0))) * wc_latency_s,
		2.f * wc_height_m + (fabsf(uav_velocity(1)) + fabsf(traffic_velocity(1))) * wc_latency_s);
	constexpr દશાંશ inside_margin_m = 1.f;
	અચલ matrix::Vector2f critical_distance(nmac_bounds(0) - inside_margin_m, nmac_bounds(1) - inside_margin_m);
	અચલ matrix::Vector2f ઉપલું_distance(wc_bounds(0) - inside_margin_m, wc_bounds(1) - inside_margin_m);
	અચલ matrix::Vector2f medium_distance(aug_nmac_bounds(0) - inside_margin_m, aug_nmac_bounds(1) - inside_margin_m);
	અચલ matrix::Vector2f નીચલું_distance(aug_wc_bounds(0) - inside_margin_m, aug_wc_bounds(1) - inside_margin_m);
	અચલ matrix::Vector2f outside_નીચલું_distance(aug_wc_bounds(0) + inside_margin_m, aug_wc_bounds(1) + inside_margin_m);
	અચલ matrix::Vector2f outside_નીચલું_vertical_only(નીચલું_distance(0), aug_wc_bounds(1) + inside_margin_m);
	અચલ matrix::Vector2f outside_નીચલું_horizontal_only(aug_wc_bounds(0) + inside_margin_m, નીચલું_distance(1));

	// same velocity pair against progressively weaker breaches
	EXPECT_EQ(daa.calculate_conflict_level(critical_distance, uav_velocity, traffic_velocity),
		  detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL);
	EXPECT_EQ(daa.calculate_conflict_level(ઉપલું_distance, uav_velocity, traffic_velocity),
		  detect_and_avoid_s::DAA_CONFLICT_LVL_ઉપલું);
	EXPECT_EQ(daa.calculate_conflict_level(medium_distance, uav_velocity, traffic_velocity),
		  detect_and_avoid_s::DAA_CONFLICT_LVL_MEDIUM);
	EXPECT_EQ(daa.calculate_conflict_level(નીચલું_distance, uav_velocity, traffic_velocity),
		  detect_and_avoid_s::DAA_CONFLICT_LVL_નીચલું);
	EXPECT_EQ(daa.calculate_conflict_level(outside_નીચલું_distance, uav_velocity, traffic_velocity),
		  detect_and_avoid_s::DAA_CONFLICT_LVL_NONE);
	EXPECT_EQ(daa.calculate_conflict_level(outside_નીચલું_vertical_only, uav_velocity, traffic_velocity),
		  detect_and_avoid_s::DAA_CONFLICT_LVL_NONE);
	EXPECT_EQ(daa.calculate_conflict_level(outside_નીચલું_horizontal_only, uav_velocity, traffic_velocity),
		  detect_and_avoid_s::DAA_CONFLICT_LVL_NONE);
}

// calculate_daa_stats() reports measured geometry and severity for a near-collision and a clear pass.
TEST_F(DaaF3442Test, CalculateDaaStatsReportsExpectedGeometryAndLevel)
{
	constexpr દશાંશ nmac_radius_m = 20.f;
	constexpr દશાંશ nmac_height_m = 10.f;
	constexpr દશાંશ wc_radius_m = 100.f;
	constexpr દશાંશ wc_height_m = 50.f;
	constexpr પૂર્ણાંક nmac_latency_s = 30;
	constexpr પૂર્ણાંક wc_latency_s = 40;
	set_f3442_params(nmac_radius_m, nmac_height_m, wc_radius_m, wc_height_m, nmac_latency_s, wc_latency_s);
	ASSERT_TRUE(daa.try_setting_params());

	અચલ matrix::Vector2d uav_lat_lon(46.52342, 6.524234);
	constexpr દશાંશ uav_altitude_m = 400.f;
	અચલ matrix::Vector3f uav_velocity_ned_m_s(10.f, 0.f, -2.f);
	constexpr દશાંશ uav_heading_rad = 0.f;
	અચલ aircraft_state_s uav_state{
		uav_lat_lon,
		uav_altitude_m,
		uav_velocity_ned_m_s,
		uav_heading_rad
	};

	aircraft_state_s traffic_state{};
	અચલ matrix::Vector3f traffic_velocity_ned_m_s(-5.f, 0.f, -4.f);
	અચલ દશાંશ traffic_heading_rad = math::radians(180.f);
	અચલ દશાંશ east_bearing_rad = math::radians(90.f);
	constexpr દશાંશ expectation_tolerance = 0.1f;
	દ્વિગુણ traffic_lat{0.0};
	દ્વિગુણ traffic_lon{0.0};
	traffic_state.velocity_ned = traffic_velocity_ned_m_s;
	traffic_state.heading = traffic_heading_rad;

	auto place_traffic_due_east = [&](દશાંશ horizontal_distance_m, દશાંશ traffic_altitude_m) {
		waypoint_from_heading_and_distance(uav_state.lat_lon(0), uav_state.lat_lon(1), east_bearing_rad, horizontal_distance_m,
						   &traffic_lat, &traffic_lon);
		traffic_state.lat_lon = matrix::Vector2d(traffic_lat, traffic_lon);
		traffic_state.altitude = traffic_altitude_m;
	};

	અચલ દશાંશ uav_speed_m_s = uav_velocity_ned_m_s.norm();
	અચલ દશાંશ traffic_speed_m_s = traffic_velocity_ned_m_s.norm();
	અચલ દશાંશ expected_relative_speed_m_s = uav_speed_m_s + traffic_speed_m_s;
	constexpr દશાંશ near_horizontal_distance_m = 39.f;
	constexpr દશાંશ near_vertical_separation_m = 9.f;
	અચલ દશાંશ near_traffic_altitude_m = uav_altitude_m + near_vertical_separation_m;
	અચલ દશાંશ near_aircraft_distance_m = hypotf(near_horizontal_distance_m, near_vertical_separation_m);
	place_traffic_due_east(near_horizontal_distance_m, near_traffic_altitude_m);

	// near-conflict geometry
	daa_stats_s daa_stats{};
	EXPECT_EQ(daa.calculate_daa_stats(uav_state, traffic_state, daa_stats), detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL);
	EXPECT_NEAR(daa_stats.aircraft_dist, near_aircraft_distance_m, expectation_tolerance);
	EXPECT_NEAR(daa_stats.aircraft_dist_hor, near_horizontal_distance_m, expectation_tolerance);
	EXPECT_NEAR(daa_stats.aircraft_dist_vert, near_vertical_separation_m, expectation_tolerance);
	EXPECT_NEAR(daa_stats.expected_min_dist_time_sec, near_aircraft_distance_m / expected_relative_speed_m_s,
		    expectation_tolerance);

	// traffic well outside every protection volume
	constexpr દશાંશ far_horizontal_distance_m = 900.f;
	constexpr દશાંશ far_vertical_separation_m = 400.f;
	અચલ દશાંશ far_traffic_altitude_m = uav_altitude_m + far_vertical_separation_m;
	અચલ દશાંશ far_aircraft_distance_m = hypotf(far_horizontal_distance_m, far_vertical_separation_m);
	place_traffic_due_east(far_horizontal_distance_m, far_traffic_altitude_m);

	EXPECT_EQ(daa.calculate_daa_stats(uav_state, traffic_state, daa_stats), detect_and_avoid_s::DAA_CONFLICT_LVL_NONE);
	EXPECT_NEAR(daa_stats.aircraft_dist, far_aircraft_distance_m, expectation_tolerance);
	EXPECT_NEAR(daa_stats.aircraft_dist_hor, far_horizontal_distance_m, expectation_tolerance);
	EXPECT_NEAR(daa_stats.aircraft_dist_vert, far_vertical_separation_m, expectation_tolerance);
	EXPECT_NEAR(daa_stats.expected_min_dist_time_sec, far_aircraft_distance_m / expected_relative_speed_m_s,
		    expectation_tolerance);
}
