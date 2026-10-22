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

#ضمن <gtest/gtest.h>
#ضمن <parameters/param.h>

#ضمن <array>
#ضمن <lib/geo/geo.h>
#ضمن <lib/mathlib/mathlib.h>
#ضمن <limits>
#ضمن <uORB/topics/detect_and_avoid.h>

#ضمن "DaaF3442.h"

صنف DaaF3442Test : عام ::testing::Test
{
محمي:
	DaaF3442 daa;

	فراغ سيٽ_مٿي() override
	{
		param_control_autosave(خطأ);
		param_reset_all();
	}

	فراغ سيٽ_f3442_params(عائم nmac_radius, عائم nmac_اوچائي, عائم wc_radius, عائم wc_اوچائي,
			      صحيح nmac_latency_s, صحيح wc_latency_s)
	{
		param_سيٽ(param_handle(px4::params::DAA_LVL_CRIT_RAD), &nmac_radius);
		param_سيٽ(param_handle(px4::params::DAA_LVL_CRIT_HGT), &nmac_اوچائي);
		param_سيٽ(param_handle(px4::params::DAA_LVL_مٿاهون_RAD), &wc_radius);
		param_سيٽ(param_handle(px4::params::DAA_LVL_مٿاهون_HGT), &wc_اوچائي);
		param_سيٽ(param_handle(px4::params::DAA_LVL_MED_وقت), &nmac_latency_s);
		param_سيٽ(param_handle(px4::params::DAA_LVL_هيٺيون_وقت), &wc_latency_s);
	}
};

TEST_F(DaaF3442Test, RejectsInvalidParams)
{
	بنية ParamCase {
		عائم nmac_radius;
		عائم nmac_اوچائي;
		عائم wc_radius;
		عائم wc_اوچائي;
		صحيح nmac_latency_s;
		صحيح wc_latency_s;
		منطقي expected_نتيجو;
	};

	constexpr عائم valid_nmac_radius_m = 10.f;
	constexpr عائم valid_nmac_اوچائي_m = 10.f;
	constexpr عائم valid_wc_radius_m = 20.f;
	constexpr عائم valid_wc_اوچائي_m = 20.f;
	constexpr صحيح valid_nmac_latency_s = 5;
	constexpr صحيح valid_wc_latency_s = 5;
	ثابت عائم nan = std::numeric_limits<عائم>::quiet_NaN();
	ثابت عائم inf = std::numeric_limits<عائم>::infinity();

	ثابت std::array<ParamCase, 13> cases{{
			{valid_nmac_radius_m, valid_nmac_اوچائي_m, valid_wc_radius_m, valid_wc_اوچائي_m, valid_nmac_latency_s, valid_wc_latency_s, سچ},
			{0.f, valid_nmac_اوچائي_m, valid_wc_radius_m, valid_wc_اوچائي_m, valid_nmac_latency_s, valid_wc_latency_s, خطأ},
			{-valid_nmac_radius_m, valid_nmac_اوچائي_m, valid_wc_radius_m, valid_wc_اوچائي_m, valid_nmac_latency_s, valid_wc_latency_s, خطأ},
			{valid_nmac_radius_m, -valid_nmac_اوچائي_m, valid_wc_radius_m, valid_wc_اوچائي_m, valid_nmac_latency_s, valid_wc_latency_s, خطأ},
			{valid_nmac_radius_m, valid_nmac_اوچائي_m, -valid_wc_radius_m, valid_wc_اوچائي_m, valid_nmac_latency_s, valid_wc_latency_s, خطأ},
			{valid_nmac_radius_m, valid_nmac_اوچائي_m, valid_wc_radius_m, -valid_wc_اوچائي_m, valid_nmac_latency_s, valid_wc_latency_s, خطأ},
			{nan, valid_nmac_اوچائي_m, valid_wc_radius_m, valid_wc_اوچائي_m, valid_nmac_latency_s, valid_wc_latency_s, خطأ},
			{valid_nmac_radius_m, inf, valid_wc_radius_m, valid_wc_اوچائي_m, valid_nmac_latency_s, valid_wc_latency_s, خطأ},
			{valid_nmac_radius_m, valid_nmac_اوچائي_m, nan, valid_wc_اوچائي_m, valid_nmac_latency_s, valid_wc_latency_s, خطأ},
			{valid_nmac_radius_m, valid_nmac_اوچائي_m, valid_wc_radius_m, inf, valid_nmac_latency_s, valid_wc_latency_s, خطأ},
			{valid_nmac_radius_m, valid_nmac_اوچائي_m, valid_wc_radius_m, valid_wc_اوچائي_m, -valid_nmac_latency_s, valid_wc_latency_s, خطأ},
			{valid_nmac_radius_m, valid_nmac_اوچائي_m, valid_wc_radius_m, valid_wc_اوچائي_m, valid_nmac_latency_s, -valid_wc_latency_s, خطأ},
			{valid_nmac_radius_m, valid_nmac_اوچائي_m, valid_wc_radius_m, valid_wc_اوچائي_m, valid_nmac_latency_s + 1, valid_wc_latency_s, خطأ},
		}};

	لكل (size_t i = 0; i < cases.سائيز(); ++i) {
		ثابت ParamCase &test_case = cases[i];
		SCOPED_TRACE(i);

		سيٽ_f3442_params(test_case.nmac_radius, test_case.nmac_اوچائي, test_case.wc_radius, test_case.wc_اوچائي,
				 test_case.nmac_latency_s, test_case.wc_latency_s);
		EXPECT_EQ(daa.try_setting_params(), test_case.expected_نتيجو);
	}
}

// Bounds must nest: an outer zone no larger than the inner one is rejected.
TEST_F(DaaF3442Test, Rejects_Non_درجيوار_Bounds)
{
	بنية HierarchyCase {
		عائم nmac_radius;
		عائم nmac_اوچائي;
		عائم wc_radius;
		عائم wc_اوچائي;
	};

	constexpr صحيح nmac_latency_s = 30;
	constexpr صحيح wc_latency_s = 30;
	ثابت std::array<HierarchyCase, 3> cases{{
			{20.f, 10.f, 19.f, 10.f},
			{20.f, 10.f, 20.f, 9.f},
			{200.f, 100.f, 100.f, 50.f},
		}};

	لكل (size_t i = 0; i < cases.سائيز(); ++i) {
		ثابت HierarchyCase &test_case = cases[i];
		SCOPED_TRACE(i);

		سيٽ_f3442_params(test_case.nmac_radius, test_case.nmac_اوچائي, test_case.wc_radius, test_case.wc_اوچائي,
				 nmac_latency_s, wc_latency_s);

		EXPECT_FALSE(daa.try_setting_params());
	}
}

// is_in_bounds() is inclusive and fails on negative or non-finite inputs.
TEST_F(DaaF3442Test, Is_اندريون_Bounds_Rejects_Invalid_Inputs)
{
	ثابت ميٽرڪس::Vector2f bounds(10.f, 10.f);
	ثابت ميٽرڪس::Vector2f within_bounds(5.f, 5.f);
	ثابت ميٽرڪس::Vector2f on_boundary(10.f, 10.f);
	ثابت ميٽرڪس::Vector2f outside_both_bounds(15.f, 15.f);
	ثابت ميٽرڪس::Vector2f outside_horizontal_bounds(15.f, 4.f);
	ثابت ميٽرڪس::Vector2f outside_vertical_bounds(4.f, 15.f);
	ثابت ميٽرڪس::Vector2f negative_bounds(-10.f, 10.f);
	ثابت ميٽرڪس::Vector2f negative_مفاصلو(-5.f, 5.f);
	ثابت ميٽرڪس::Vector2f non_finite_مفاصلو(5.f, std::numeric_limits<عائم>::quiet_NaN());

	EXPECT_TRUE(daa.is_اندريون_bounds(within_bounds, bounds));
	EXPECT_TRUE(daa.is_اندريون_bounds(on_boundary, bounds));
	EXPECT_FALSE(daa.is_اندريون_bounds(outside_both_bounds, bounds));
	EXPECT_FALSE(daa.is_اندريون_bounds(outside_horizontal_bounds, bounds));
	EXPECT_FALSE(daa.is_اندريون_bounds(outside_vertical_bounds, bounds));
	EXPECT_FALSE(daa.is_اندريون_bounds(within_bounds, negative_bounds));
	EXPECT_FALSE(daa.is_اندريون_bounds(negative_مفاصلو, bounds));
	EXPECT_FALSE(daa.is_اندريون_bounds(non_finite_مفاصلو, bounds));
}

// Volume augmentation uses |velocity| and |latency|, so signs never shrink it.
TEST_F(DaaF3442Test, Calculate_Aircraft_Conflict_Volume_استعمال_ڪري_Absolute_Inputs)
{
	ثابت ميٽرڪس::Vector2f بنياد_bounds(10.f, 20.f);
	ثابت ميٽرڪس::Vector2f velocity(-2.f, 4.f);
	ثابت عائم latency_s = -5.f;
	ثابت عائم expected_horizontal_bounds_m = بنياد_bounds(0) + fabsf(velocity(0)) * fabsf(latency_s);
	ثابت عائم expected_vertical_bounds_m = بنياد_bounds(1) + fabsf(velocity(1)) * fabsf(latency_s);

	ميٽرڪس::Vector2f augmented_bounds{};
	daa.calculate_aircraft_conflict_volume(بنياد_bounds, velocity, latency_s, augmented_bounds);

	EXPECT_FLOAT_EQ(augmented_bounds(0), expected_horizontal_bounds_m);
	EXPECT_FLOAT_EQ(augmented_bounds(1), expected_vertical_bounds_m);
}

// Augmented boundary sums both aircraft's volumes (motion from each side).
TEST_F(DaaF3442Test, CalculateAugmentedBoundariesAddsBothAircraftVolumes)
{
	ثابت ميٽرڪس::Vector2f بنياد_bounds(10.f, 20.f);
	ثابت ميٽرڪس::Vector2f uav_velocity(2.f, 4.f);
	ثابت ميٽرڪس::Vector2f traffic_velocity(8.f, 10.f);
	ثابت عائم latency_s = 3.f;
	ثابت عائم expected_horizontal_bounds_m = 2.f * بنياد_bounds(0)
			+ (fabsf(uav_velocity(0)) + fabsf(traffic_velocity(0))) * latency_s;
	ثابت عائم expected_vertical_bounds_m = 2.f * بنياد_bounds(1)
			+ (fabsf(uav_velocity(1)) + fabsf(traffic_velocity(1))) * latency_s;

	ميٽرڪس::Vector2f augmented_bounds{};
	daa.calculate_augmented_boundaries(بنياد_bounds, uav_velocity, traffic_velocity, latency_s, augmented_bounds);

	EXPECT_FLOAT_EQ(augmented_bounds(0), expected_horizontal_bounds_m);
	EXPECT_FLOAT_EQ(augmented_bounds(1), expected_vertical_bounds_m);
}

// calculate_conflict_level() returns the strictest breached zone; a single-axis miss is NONE.
TEST_F(DaaF3442Test, Calculate_Conflict_Level_Respects_ترجيح_ترتيب)
{
	constexpr عائم nmac_radius_m = 20.f;
	constexpr عائم nmac_اوچائي_m = 10.f;
	constexpr عائم wc_radius_m = 100.f;
	constexpr عائم wc_اوچائي_m = 50.f;
	constexpr صحيح nmac_latency_s = 30;
	constexpr صحيح wc_latency_s = 40;
	سيٽ_f3442_params(nmac_radius_m, nmac_اوچائي_m, wc_radius_m, wc_اوچائي_m, nmac_latency_s, wc_latency_s);
	ASSERT_TRUE(daa.try_setting_params());

	ثابت ميٽرڪس::Vector2f uav_velocity(10.f, 2.f);
	ثابت ميٽرڪس::Vector2f traffic_velocity(5.f, 4.f);
	ثابت ميٽرڪس::Vector2f nmac_bounds(2.f * nmac_radius_m, 2.f * nmac_اوچائي_m);
	ثابت ميٽرڪس::Vector2f wc_bounds(2.f * wc_radius_m, 2.f * wc_اوچائي_m);
	ثابت ميٽرڪس::Vector2f aug_nmac_bounds(
		2.f * nmac_radius_m + (fabsf(uav_velocity(0)) + fabsf(traffic_velocity(0))) * nmac_latency_s,
		2.f * nmac_اوچائي_m + (fabsf(uav_velocity(1)) + fabsf(traffic_velocity(1))) * nmac_latency_s);
	ثابت ميٽرڪس::Vector2f aug_wc_bounds(
		2.f * wc_radius_m + (fabsf(uav_velocity(0)) + fabsf(traffic_velocity(0))) * wc_latency_s,
		2.f * wc_اوچائي_m + (fabsf(uav_velocity(1)) + fabsf(traffic_velocity(1))) * wc_latency_s);
	constexpr عائم inside_margin_m = 1.f;
	ثابت ميٽرڪس::Vector2f critical_مفاصلو(nmac_bounds(0) - inside_margin_m, nmac_bounds(1) - inside_margin_m);
	ثابت ميٽرڪس::Vector2f مٿاهون_مفاصلو(wc_bounds(0) - inside_margin_m, wc_bounds(1) - inside_margin_m);
	ثابت ميٽرڪس::Vector2f medium_مفاصلو(aug_nmac_bounds(0) - inside_margin_m, aug_nmac_bounds(1) - inside_margin_m);
	ثابت ميٽرڪس::Vector2f هيٺيون_مفاصلو(aug_wc_bounds(0) - inside_margin_m, aug_wc_bounds(1) - inside_margin_m);
	ثابت ميٽرڪس::Vector2f outside_هيٺيون_مفاصلو(aug_wc_bounds(0) + inside_margin_m, aug_wc_bounds(1) + inside_margin_m);
	ثابت ميٽرڪس::Vector2f outside_هيٺيون_vertical_only(هيٺيون_مفاصلو(0), aug_wc_bounds(1) + inside_margin_m);
	ثابت ميٽرڪس::Vector2f outside_هيٺيون_horizontal_only(aug_wc_bounds(0) + inside_margin_m, هيٺيون_مفاصلو(1));

	// same velocity pair against progressively weaker breaches
	EXPECT_EQ(daa.calculate_conflict_level(critical_مفاصلو, uav_velocity, traffic_velocity),
		  detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL);
	EXPECT_EQ(daa.calculate_conflict_level(مٿاهون_مفاصلو, uav_velocity, traffic_velocity),
		  detect_and_avoid_s::DAA_CONFLICT_LVL_مٿاهون);
	EXPECT_EQ(daa.calculate_conflict_level(medium_مفاصلو, uav_velocity, traffic_velocity),
		  detect_and_avoid_s::DAA_CONFLICT_LVL_MEDIUM);
	EXPECT_EQ(daa.calculate_conflict_level(هيٺيون_مفاصلو, uav_velocity, traffic_velocity),
		  detect_and_avoid_s::DAA_CONFLICT_LVL_هيٺيون);
	EXPECT_EQ(daa.calculate_conflict_level(outside_هيٺيون_مفاصلو, uav_velocity, traffic_velocity),
		  detect_and_avoid_s::DAA_CONFLICT_LVL_NONE);
	EXPECT_EQ(daa.calculate_conflict_level(outside_هيٺيون_vertical_only, uav_velocity, traffic_velocity),
		  detect_and_avoid_s::DAA_CONFLICT_LVL_NONE);
	EXPECT_EQ(daa.calculate_conflict_level(outside_هيٺيون_horizontal_only, uav_velocity, traffic_velocity),
		  detect_and_avoid_s::DAA_CONFLICT_LVL_NONE);
}

// calculate_daa_stats() reports measured geometry and severity for a near-collision and a clear pass.
TEST_F(DaaF3442Test, CalculateDaaStatsReportsExpectedGeometryAndLevel)
{
	constexpr عائم nmac_radius_m = 20.f;
	constexpr عائم nmac_اوچائي_m = 10.f;
	constexpr عائم wc_radius_m = 100.f;
	constexpr عائم wc_اوچائي_m = 50.f;
	constexpr صحيح nmac_latency_s = 30;
	constexpr صحيح wc_latency_s = 40;
	سيٽ_f3442_params(nmac_radius_m, nmac_اوچائي_m, wc_radius_m, wc_اوچائي_m, nmac_latency_s, wc_latency_s);
	ASSERT_TRUE(daa.try_setting_params());

	ثابت ميٽرڪس::Vector2d uav_lat_lon(46.52342, 6.524234);
	constexpr عائم uav_altitude_m = 400.f;
	ثابت ميٽرڪس::Vector3f uav_velocity_ned_m_s(10.f, 0.f, -2.f);
	constexpr عائم uav_heading_rad = 0.f;
	ثابت aircraft_state_s uav_state{
		uav_lat_lon,
		uav_altitude_m,
		uav_velocity_ned_m_s,
		uav_heading_rad
	};

	aircraft_state_s traffic_state{};
	ثابت ميٽرڪس::Vector3f traffic_velocity_ned_m_s(-5.f, 0.f, -4.f);
	ثابت عائم traffic_heading_rad = math::radians(180.f);
	ثابت عائم east_bearing_rad = math::radians(90.f);
	constexpr عائم expectation_tolerance = 0.1f;
	مزدوج traffic_lat{0.0};
	مزدوج traffic_lon{0.0};
	traffic_state.velocity_ned = traffic_velocity_ned_m_s;
	traffic_state.heading = traffic_heading_rad;

	خودڪار رکو_traffic_due_east = [&](عائم horizontal_مفاصلو_m, عائم traffic_altitude_m) {
		waypoint_from_heading_and_مفاصلو(uav_state.lat_lon(0), uav_state.lat_lon(1), east_bearing_rad, horizontal_مفاصلو_m,
						   &traffic_lat, &traffic_lon);
		traffic_state.lat_lon = ميٽرڪس::Vector2d(traffic_lat, traffic_lon);
		traffic_state.altitude = traffic_altitude_m;
	};

	ثابت عائم uav_رفتار_m_s = uav_velocity_ned_m_s.norm();
	ثابت عائم traffic_رفتار_m_s = traffic_velocity_ned_m_s.norm();
	ثابت عائم expected_relative_رفتار_m_s = uav_رفتار_m_s + traffic_رفتار_m_s;
	constexpr عائم near_horizontal_مفاصلو_m = 39.f;
	constexpr عائم near_vertical_separation_m = 9.f;
	ثابت عائم near_traffic_altitude_m = uav_altitude_m + near_vertical_separation_m;
	ثابت عائم near_aircraft_مفاصلو_m = hypotf(near_horizontal_مفاصلو_m, near_vertical_separation_m);
	رکو_traffic_due_east(near_horizontal_مفاصلو_m, near_traffic_altitude_m);

	// near-conflict geometry
	daa_stats_s daa_stats{};
	EXPECT_EQ(daa.calculate_daa_stats(uav_state, traffic_state, daa_stats), detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL);
	EXPECT_NEAR(daa_stats.aircraft_dist, near_aircraft_مفاصلو_m, expectation_tolerance);
	EXPECT_NEAR(daa_stats.aircraft_dist_hor, near_horizontal_مفاصلو_m, expectation_tolerance);
	EXPECT_NEAR(daa_stats.aircraft_dist_vert, near_vertical_separation_m, expectation_tolerance);
	EXPECT_NEAR(daa_stats.expected_min_dist_وقت_sec, near_aircraft_مفاصلو_m / expected_relative_رفتار_m_s,
		    expectation_tolerance);

	// traffic well outside every protection volume
	constexpr عائم far_horizontal_مفاصلو_m = 900.f;
	constexpr عائم far_vertical_separation_m = 400.f;
	ثابت عائم far_traffic_altitude_m = uav_altitude_m + far_vertical_separation_m;
	ثابت عائم far_aircraft_مفاصلو_m = hypotf(far_horizontal_مفاصلو_m, far_vertical_separation_m);
	رکو_traffic_due_east(far_horizontal_مفاصلو_m, far_traffic_altitude_m);

	EXPECT_EQ(daa.calculate_daa_stats(uav_state, traffic_state, daa_stats), detect_and_avoid_s::DAA_CONFLICT_LVL_NONE);
	EXPECT_NEAR(daa_stats.aircraft_dist, far_aircraft_مفاصلو_m, expectation_tolerance);
	EXPECT_NEAR(daa_stats.aircraft_dist_hor, far_horizontal_مفاصلو_m, expectation_tolerance);
	EXPECT_NEAR(daa_stats.aircraft_dist_vert, far_vertical_separation_m, expectation_tolerance);
	EXPECT_NEAR(daa_stats.expected_min_dist_وقت_sec, far_aircraft_مفاصلو_m / expected_relative_رفتار_m_s,
		    expectation_tolerance);
}
