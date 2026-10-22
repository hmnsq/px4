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

#অন্তর্ভাৱ <gtest/gtest.h>
#অন্তর্ভাৱ <parameters/param.h>

#অন্তর্ভাৱ <array>
#অন্তর্ভাৱ <lib/geo/geo.h>
#অন্তর্ভাৱ <lib/mathlib/mathlib.h>
#অন্তর্ভাৱ <limits>
#অন্তর্ভাৱ <uORB/topics/detect_and_avoid.h>

#অন্তর্ভাৱ "DaaF3442.h"

শ্ৰেণী DaaF3442Test : সার্ৱজনিক ::testing::Test
{
রক্ষিত:
	DaaF3442 daa;

	শূন্য SetUp() override
	{
		param_control_autosave(মিছা);
		param_reset_all();
	}

	শূন্য set_f3442_params(ভিন্ন nmac_radius, ভিন্ন nmac_height, ভিন্ন wc_radius, ভিন্ন wc_height,
			      পূর্ণাঙ্ক nmac_latency_s, পূর্ণাঙ্ক wc_latency_s)
	{
		param_set(param_handle(px4::params::DAA_LVL_CRIT_RAD), &nmac_radius);
		param_set(param_handle(px4::params::DAA_LVL_CRIT_HGT), &nmac_height);
		param_set(param_handle(px4::params::DAA_LVL_উপৰেৰ_RAD), &wc_radius);
		param_set(param_handle(px4::params::DAA_LVL_উপৰেৰ_HGT), &wc_height);
		param_set(param_handle(px4::params::DAA_LVL_MED_TIME), &nmac_latency_s);
		param_set(param_handle(px4::params::DAA_LVL_নিচেৰ_TIME), &wc_latency_s);
	}
};

TEST_F(DaaF3442Test, RejectsInvalidParams)
{
	সংরচনা ParamCase {
		ভিন্ন nmac_radius;
		ভিন্ন nmac_height;
		ভিন্ন wc_radius;
		ভিন্ন wc_height;
		পূর্ণাঙ্ক nmac_latency_s;
		পূর্ণাঙ্ক wc_latency_s;
		তার্কিক expected_result;
	};

	constexpr ভিন্ন valid_nmac_radius_m = 10.f;
	constexpr ভিন্ন valid_nmac_height_m = 10.f;
	constexpr ভিন্ন valid_wc_radius_m = 20.f;
	constexpr ভিন্ন valid_wc_height_m = 20.f;
	constexpr পূর্ণাঙ্ক valid_nmac_latency_s = 5;
	constexpr পূর্ণাঙ্ক valid_wc_latency_s = 5;
	ধ্রুৱ ভিন্ন nan = std::numeric_limits<ভিন্ন>::quiet_NaN();
	ধ্রুৱ ভিন্ন inf = std::numeric_limits<ভিন্ন>::infinity();

	ধ্রুৱ std::array<ParamCase, 13> cases{{
			{valid_nmac_radius_m, valid_nmac_height_m, valid_wc_radius_m, valid_wc_height_m, valid_nmac_latency_s, valid_wc_latency_s, সঁচা},
			{0.f, valid_nmac_height_m, valid_wc_radius_m, valid_wc_height_m, valid_nmac_latency_s, valid_wc_latency_s, মিছা},
			{-valid_nmac_radius_m, valid_nmac_height_m, valid_wc_radius_m, valid_wc_height_m, valid_nmac_latency_s, valid_wc_latency_s, মিছা},
			{valid_nmac_radius_m, -valid_nmac_height_m, valid_wc_radius_m, valid_wc_height_m, valid_nmac_latency_s, valid_wc_latency_s, মিছা},
			{valid_nmac_radius_m, valid_nmac_height_m, -valid_wc_radius_m, valid_wc_height_m, valid_nmac_latency_s, valid_wc_latency_s, মিছা},
			{valid_nmac_radius_m, valid_nmac_height_m, valid_wc_radius_m, -valid_wc_height_m, valid_nmac_latency_s, valid_wc_latency_s, মিছা},
			{nan, valid_nmac_height_m, valid_wc_radius_m, valid_wc_height_m, valid_nmac_latency_s, valid_wc_latency_s, মিছা},
			{valid_nmac_radius_m, inf, valid_wc_radius_m, valid_wc_height_m, valid_nmac_latency_s, valid_wc_latency_s, মিছা},
			{valid_nmac_radius_m, valid_nmac_height_m, nan, valid_wc_height_m, valid_nmac_latency_s, valid_wc_latency_s, মিছা},
			{valid_nmac_radius_m, valid_nmac_height_m, valid_wc_radius_m, inf, valid_nmac_latency_s, valid_wc_latency_s, মিছা},
			{valid_nmac_radius_m, valid_nmac_height_m, valid_wc_radius_m, valid_wc_height_m, -valid_nmac_latency_s, valid_wc_latency_s, মিছা},
			{valid_nmac_radius_m, valid_nmac_height_m, valid_wc_radius_m, valid_wc_height_m, valid_nmac_latency_s, -valid_wc_latency_s, মিছা},
			{valid_nmac_radius_m, valid_nmac_height_m, valid_wc_radius_m, valid_wc_height_m, valid_nmac_latency_s + 1, valid_wc_latency_s, মিছা},
		}};

	বাবে (size_t i = 0; i < cases.size(); ++i) {
		ধ্রুৱ ParamCase &test_case = cases[i];
		SCOPED_TRACE(i);

		set_f3442_params(test_case.nmac_radius, test_case.nmac_height, test_case.wc_radius, test_case.wc_height,
				 test_case.nmac_latency_s, test_case.wc_latency_s);
		EXPECT_EQ(daa.try_setting_params(), test_case.expected_result);
	}
}

// Bounds must nest: an outer zone no larger than the inner one is rejected.
TEST_F(DaaF3442Test, RejectsNonHierarchicalBounds)
{
	সংরচনা HierarchyCase {
		ভিন্ন nmac_radius;
		ভিন্ন nmac_height;
		ভিন্ন wc_radius;
		ভিন্ন wc_height;
	};

	constexpr পূর্ণাঙ্ক nmac_latency_s = 30;
	constexpr পূর্ণাঙ্ক wc_latency_s = 30;
	ধ্রুৱ std::array<HierarchyCase, 3> cases{{
			{20.f, 10.f, 19.f, 10.f},
			{20.f, 10.f, 20.f, 9.f},
			{200.f, 100.f, 100.f, 50.f},
		}};

	বাবে (size_t i = 0; i < cases.size(); ++i) {
		ধ্রুৱ HierarchyCase &test_case = cases[i];
		SCOPED_TRACE(i);

		set_f3442_params(test_case.nmac_radius, test_case.nmac_height, test_case.wc_radius, test_case.wc_height,
				 nmac_latency_s, wc_latency_s);

		EXPECT_FALSE(daa.try_setting_params());
	}
}

// is_in_bounds() is inclusive and fails on negative or non-finite inputs.
TEST_F(DaaF3442Test, IsInBoundsRejectsInvalidInputs)
{
	ধ্রুৱ matrix::Vector2f bounds(10.f, 10.f);
	ধ্রুৱ matrix::Vector2f within_bounds(5.f, 5.f);
	ধ্রুৱ matrix::Vector2f on_boundary(10.f, 10.f);
	ধ্রুৱ matrix::Vector2f outside_both_bounds(15.f, 15.f);
	ধ্রুৱ matrix::Vector2f outside_horizontal_bounds(15.f, 4.f);
	ধ্রুৱ matrix::Vector2f outside_vertical_bounds(4.f, 15.f);
	ধ্রুৱ matrix::Vector2f negative_bounds(-10.f, 10.f);
	ধ্রুৱ matrix::Vector2f negative_distance(-5.f, 5.f);
	ধ্রুৱ matrix::Vector2f non_finite_distance(5.f, std::numeric_limits<ভিন্ন>::quiet_NaN());

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
	ধ্রুৱ matrix::Vector2f base_bounds(10.f, 20.f);
	ধ্রুৱ matrix::Vector2f velocity(-2.f, 4.f);
	ধ্রুৱ ভিন্ন latency_s = -5.f;
	ধ্রুৱ ভিন্ন expected_horizontal_bounds_m = base_bounds(0) + fabsf(velocity(0)) * fabsf(latency_s);
	ধ্রুৱ ভিন্ন expected_vertical_bounds_m = base_bounds(1) + fabsf(velocity(1)) * fabsf(latency_s);

	matrix::Vector2f augmented_bounds{};
	daa.calculate_aircraft_conflict_volume(base_bounds, velocity, latency_s, augmented_bounds);

	EXPECT_FLOAT_EQ(augmented_bounds(0), expected_horizontal_bounds_m);
	EXPECT_FLOAT_EQ(augmented_bounds(1), expected_vertical_bounds_m);
}

// Augmented boundary sums both aircraft's volumes (motion from each side).
TEST_F(DaaF3442Test, CalculateAugmentedBoundariesAddsBothAircraftVolumes)
{
	ধ্রুৱ matrix::Vector2f base_bounds(10.f, 20.f);
	ধ্রুৱ matrix::Vector2f uav_velocity(2.f, 4.f);
	ধ্রুৱ matrix::Vector2f traffic_velocity(8.f, 10.f);
	ধ্রুৱ ভিন্ন latency_s = 3.f;
	ধ্রুৱ ভিন্ন expected_horizontal_bounds_m = 2.f * base_bounds(0)
			+ (fabsf(uav_velocity(0)) + fabsf(traffic_velocity(0))) * latency_s;
	ধ্রুৱ ভিন্ন expected_vertical_bounds_m = 2.f * base_bounds(1)
			+ (fabsf(uav_velocity(1)) + fabsf(traffic_velocity(1))) * latency_s;

	matrix::Vector2f augmented_bounds{};
	daa.calculate_augmented_boundaries(base_bounds, uav_velocity, traffic_velocity, latency_s, augmented_bounds);

	EXPECT_FLOAT_EQ(augmented_bounds(0), expected_horizontal_bounds_m);
	EXPECT_FLOAT_EQ(augmented_bounds(1), expected_vertical_bounds_m);
}

// calculate_conflict_level() returns the strictest breached zone; a single-axis miss is NONE.
TEST_F(DaaF3442Test, CalculateConflictLevelRespectsPriorityOrder)
{
	constexpr ভিন্ন nmac_radius_m = 20.f;
	constexpr ভিন্ন nmac_height_m = 10.f;
	constexpr ভিন্ন wc_radius_m = 100.f;
	constexpr ভিন্ন wc_height_m = 50.f;
	constexpr পূর্ণাঙ্ক nmac_latency_s = 30;
	constexpr পূর্ণাঙ্ক wc_latency_s = 40;
	set_f3442_params(nmac_radius_m, nmac_height_m, wc_radius_m, wc_height_m, nmac_latency_s, wc_latency_s);
	ASSERT_TRUE(daa.try_setting_params());

	ধ্রুৱ matrix::Vector2f uav_velocity(10.f, 2.f);
	ধ্রুৱ matrix::Vector2f traffic_velocity(5.f, 4.f);
	ধ্রুৱ matrix::Vector2f nmac_bounds(2.f * nmac_radius_m, 2.f * nmac_height_m);
	ধ্রুৱ matrix::Vector2f wc_bounds(2.f * wc_radius_m, 2.f * wc_height_m);
	ধ্রুৱ matrix::Vector2f aug_nmac_bounds(
		2.f * nmac_radius_m + (fabsf(uav_velocity(0)) + fabsf(traffic_velocity(0))) * nmac_latency_s,
		2.f * nmac_height_m + (fabsf(uav_velocity(1)) + fabsf(traffic_velocity(1))) * nmac_latency_s);
	ধ্রুৱ matrix::Vector2f aug_wc_bounds(
		2.f * wc_radius_m + (fabsf(uav_velocity(0)) + fabsf(traffic_velocity(0))) * wc_latency_s,
		2.f * wc_height_m + (fabsf(uav_velocity(1)) + fabsf(traffic_velocity(1))) * wc_latency_s);
	constexpr ভিন্ন inside_margin_m = 1.f;
	ধ্রুৱ matrix::Vector2f critical_distance(nmac_bounds(0) - inside_margin_m, nmac_bounds(1) - inside_margin_m);
	ধ্রুৱ matrix::Vector2f উপৰেৰ_distance(wc_bounds(0) - inside_margin_m, wc_bounds(1) - inside_margin_m);
	ধ্রুৱ matrix::Vector2f medium_distance(aug_nmac_bounds(0) - inside_margin_m, aug_nmac_bounds(1) - inside_margin_m);
	ধ্রুৱ matrix::Vector2f নিচেৰ_distance(aug_wc_bounds(0) - inside_margin_m, aug_wc_bounds(1) - inside_margin_m);
	ধ্রুৱ matrix::Vector2f outside_নিচেৰ_distance(aug_wc_bounds(0) + inside_margin_m, aug_wc_bounds(1) + inside_margin_m);
	ধ্রুৱ matrix::Vector2f outside_নিচেৰ_vertical_only(নিচেৰ_distance(0), aug_wc_bounds(1) + inside_margin_m);
	ধ্রুৱ matrix::Vector2f outside_নিচেৰ_horizontal_only(aug_wc_bounds(0) + inside_margin_m, নিচেৰ_distance(1));

	// same velocity pair against progressively weaker breaches
	EXPECT_EQ(daa.calculate_conflict_level(critical_distance, uav_velocity, traffic_velocity),
		  detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL);
	EXPECT_EQ(daa.calculate_conflict_level(উপৰেৰ_distance, uav_velocity, traffic_velocity),
		  detect_and_avoid_s::DAA_CONFLICT_LVL_উপৰেৰ);
	EXPECT_EQ(daa.calculate_conflict_level(medium_distance, uav_velocity, traffic_velocity),
		  detect_and_avoid_s::DAA_CONFLICT_LVL_MEDIUM);
	EXPECT_EQ(daa.calculate_conflict_level(নিচেৰ_distance, uav_velocity, traffic_velocity),
		  detect_and_avoid_s::DAA_CONFLICT_LVL_নিচেৰ);
	EXPECT_EQ(daa.calculate_conflict_level(outside_নিচেৰ_distance, uav_velocity, traffic_velocity),
		  detect_and_avoid_s::DAA_CONFLICT_LVL_NONE);
	EXPECT_EQ(daa.calculate_conflict_level(outside_নিচেৰ_vertical_only, uav_velocity, traffic_velocity),
		  detect_and_avoid_s::DAA_CONFLICT_LVL_NONE);
	EXPECT_EQ(daa.calculate_conflict_level(outside_নিচেৰ_horizontal_only, uav_velocity, traffic_velocity),
		  detect_and_avoid_s::DAA_CONFLICT_LVL_NONE);
}

// calculate_daa_stats() reports measured geometry and severity for a near-collision and a clear pass.
TEST_F(DaaF3442Test, CalculateDaaStatsReportsExpectedGeometryAndLevel)
{
	constexpr ভিন্ন nmac_radius_m = 20.f;
	constexpr ভিন্ন nmac_height_m = 10.f;
	constexpr ভিন্ন wc_radius_m = 100.f;
	constexpr ভিন্ন wc_height_m = 50.f;
	constexpr পূর্ণাঙ্ক nmac_latency_s = 30;
	constexpr পূর্ণাঙ্ক wc_latency_s = 40;
	set_f3442_params(nmac_radius_m, nmac_height_m, wc_radius_m, wc_height_m, nmac_latency_s, wc_latency_s);
	ASSERT_TRUE(daa.try_setting_params());

	ধ্রুৱ matrix::Vector2d uav_lat_lon(46.52342, 6.524234);
	constexpr ভিন্ন uav_altitude_m = 400.f;
	ধ্রুৱ matrix::Vector3f uav_velocity_ned_m_s(10.f, 0.f, -2.f);
	constexpr ভিন্ন uav_heading_rad = 0.f;
	ধ্রুৱ aircraft_state_s uav_state{
		uav_lat_lon,
		uav_altitude_m,
		uav_velocity_ned_m_s,
		uav_heading_rad
	};

	aircraft_state_s traffic_state{};
	ধ্রুৱ matrix::Vector3f traffic_velocity_ned_m_s(-5.f, 0.f, -4.f);
	ধ্রুৱ ভিন্ন traffic_heading_rad = math::radians(180.f);
	ধ্রুৱ ভিন্ন east_bearing_rad = math::radians(90.f);
	constexpr ভিন্ন expectation_tolerance = 0.1f;
	দ্ৱিগুণ traffic_lat{0.0};
	দ্ৱিগুণ traffic_lon{0.0};
	traffic_state.velocity_ned = traffic_velocity_ned_m_s;
	traffic_state.heading = traffic_heading_rad;

	auto place_traffic_due_east = [&](ভিন্ন horizontal_distance_m, ভিন্ন traffic_altitude_m) {
		waypoint_from_heading_and_distance(uav_state.lat_lon(0), uav_state.lat_lon(1), east_bearing_rad, horizontal_distance_m,
						   &traffic_lat, &traffic_lon);
		traffic_state.lat_lon = matrix::Vector2d(traffic_lat, traffic_lon);
		traffic_state.altitude = traffic_altitude_m;
	};

	ধ্রুৱ ভিন্ন uav_speed_m_s = uav_velocity_ned_m_s.norm();
	ধ্রুৱ ভিন্ন traffic_speed_m_s = traffic_velocity_ned_m_s.norm();
	ধ্রুৱ ভিন্ন expected_relative_speed_m_s = uav_speed_m_s + traffic_speed_m_s;
	constexpr ভিন্ন near_horizontal_distance_m = 39.f;
	constexpr ভিন্ন near_vertical_separation_m = 9.f;
	ধ্রুৱ ভিন্ন near_traffic_altitude_m = uav_altitude_m + near_vertical_separation_m;
	ধ্রুৱ ভিন্ন near_aircraft_distance_m = hypotf(near_horizontal_distance_m, near_vertical_separation_m);
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
	constexpr ভিন্ন far_horizontal_distance_m = 900.f;
	constexpr ভিন্ন far_vertical_separation_m = 400.f;
	ধ্রুৱ ভিন্ন far_traffic_altitude_m = uav_altitude_m + far_vertical_separation_m;
	ধ্রুৱ ভিন্ন far_aircraft_distance_m = hypotf(far_horizontal_distance_m, far_vertical_separation_m);
	place_traffic_due_east(far_horizontal_distance_m, far_traffic_altitude_m);

	EXPECT_EQ(daa.calculate_daa_stats(uav_state, traffic_state, daa_stats), detect_and_avoid_s::DAA_CONFLICT_LVL_NONE);
	EXPECT_NEAR(daa_stats.aircraft_dist, far_aircraft_distance_m, expectation_tolerance);
	EXPECT_NEAR(daa_stats.aircraft_dist_hor, far_horizontal_distance_m, expectation_tolerance);
	EXPECT_NEAR(daa_stats.aircraft_dist_vert, far_vertical_separation_m, expectation_tolerance);
	EXPECT_NEAR(daa_stats.expected_min_dist_time_sec, far_aircraft_distance_m / expected_relative_speed_m_s,
		    expectation_tolerance);
}
