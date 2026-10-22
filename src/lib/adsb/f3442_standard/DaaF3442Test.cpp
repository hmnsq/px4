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

#شامل <gtest/gtest.h>
#شامل <parameters/param.h>

#شامل <array>
#شامل <lib/geo/geo.h>
#شامل <lib/mathlib/mathlib.h>
#شامل <limits>
#شامل <uORB/topics/detect_and_avoid.h>

#شامل "DaaF3442.h"

جماعت DaaF3442Test : عوامی ::testing::Test
{
محفوظ:
	DaaF3442 daa;

	خالی سیٹ_اوپر() override
	{
		param_control_autosave(جھوٹ);
		param_reset_all();
	}

	خالی سیٹ_f3442_params(اعشاری nmac_radius, اعشاری nmac_اونچائی, اعشاری wc_radius, اعشاری wc_اونچائی,
			      صحیح nmac_latency_s, صحیح wc_latency_s)
	{
		param_سیٹ(param_handle(px4::params::DAA_LVL_CRIT_RAD), &nmac_radius);
		param_سیٹ(param_handle(px4::params::DAA_LVL_CRIT_HGT), &nmac_اونچائی);
		param_سیٹ(param_handle(px4::params::DAA_LVL_بلند_RAD), &wc_radius);
		param_سیٹ(param_handle(px4::params::DAA_LVL_بلند_HGT), &wc_اونچائی);
		param_سیٹ(param_handle(px4::params::DAA_LVL_MED_وقت), &nmac_latency_s);
		param_سیٹ(param_handle(px4::params::DAA_LVL_کم_وقت), &wc_latency_s);
	}
};

TEST_F(DaaF3442Test, RejectsInvalidParams)
{
	ساخت ParamCase {
		اعشاری nmac_radius;
		اعشاری nmac_اونچائی;
		اعشاری wc_radius;
		اعشاری wc_اونچائی;
		صحیح nmac_latency_s;
		صحیح wc_latency_s;
		منطقی expected_نتیجہ;
	};

	constexpr اعشاری valid_nmac_radius_m = 10.f;
	constexpr اعشاری valid_nmac_اونچائی_m = 10.f;
	constexpr اعشاری valid_wc_radius_m = 20.f;
	constexpr اعشاری valid_wc_اونچائی_m = 20.f;
	constexpr صحیح valid_nmac_latency_s = 5;
	constexpr صحیح valid_wc_latency_s = 5;
	ثابت اعشاری nan = std::numeric_limits<اعشاری>::quiet_NaN();
	ثابت اعشاری inf = std::numeric_limits<اعشاری>::infinity();

	ثابت std::array<ParamCase, 13> cases{{
			{valid_nmac_radius_m, valid_nmac_اونچائی_m, valid_wc_radius_m, valid_wc_اونچائی_m, valid_nmac_latency_s, valid_wc_latency_s, سچ},
			{0.f, valid_nmac_اونچائی_m, valid_wc_radius_m, valid_wc_اونچائی_m, valid_nmac_latency_s, valid_wc_latency_s, جھوٹ},
			{-valid_nmac_radius_m, valid_nmac_اونچائی_m, valid_wc_radius_m, valid_wc_اونچائی_m, valid_nmac_latency_s, valid_wc_latency_s, جھوٹ},
			{valid_nmac_radius_m, -valid_nmac_اونچائی_m, valid_wc_radius_m, valid_wc_اونچائی_m, valid_nmac_latency_s, valid_wc_latency_s, جھوٹ},
			{valid_nmac_radius_m, valid_nmac_اونچائی_m, -valid_wc_radius_m, valid_wc_اونچائی_m, valid_nmac_latency_s, valid_wc_latency_s, جھوٹ},
			{valid_nmac_radius_m, valid_nmac_اونچائی_m, valid_wc_radius_m, -valid_wc_اونچائی_m, valid_nmac_latency_s, valid_wc_latency_s, جھوٹ},
			{nan, valid_nmac_اونچائی_m, valid_wc_radius_m, valid_wc_اونچائی_m, valid_nmac_latency_s, valid_wc_latency_s, جھوٹ},
			{valid_nmac_radius_m, inf, valid_wc_radius_m, valid_wc_اونچائی_m, valid_nmac_latency_s, valid_wc_latency_s, جھوٹ},
			{valid_nmac_radius_m, valid_nmac_اونچائی_m, nan, valid_wc_اونچائی_m, valid_nmac_latency_s, valid_wc_latency_s, جھوٹ},
			{valid_nmac_radius_m, valid_nmac_اونچائی_m, valid_wc_radius_m, inf, valid_nmac_latency_s, valid_wc_latency_s, جھوٹ},
			{valid_nmac_radius_m, valid_nmac_اونچائی_m, valid_wc_radius_m, valid_wc_اونچائی_m, -valid_nmac_latency_s, valid_wc_latency_s, جھوٹ},
			{valid_nmac_radius_m, valid_nmac_اونچائی_m, valid_wc_radius_m, valid_wc_اونچائی_m, valid_nmac_latency_s, -valid_wc_latency_s, جھوٹ},
			{valid_nmac_radius_m, valid_nmac_اونچائی_m, valid_wc_radius_m, valid_wc_اونچائی_m, valid_nmac_latency_s + 1, valid_wc_latency_s, جھوٹ},
		}};

	برائے (size_t i = 0; i < cases.سائز(); ++i) {
		ثابت ParamCase &test_case = cases[i];
		SCOPED_TRACE(i);

		سیٹ_f3442_params(test_case.nmac_radius, test_case.nmac_اونچائی, test_case.wc_radius, test_case.wc_اونچائی,
				 test_case.nmac_latency_s, test_case.wc_latency_s);
		EXPECT_EQ(daa.try_setting_params(), test_case.expected_نتیجہ);
	}
}

// Bounds must nest: an outer zone no larger than the inner one is rejected.
TEST_F(DaaF3442Test, Rejects_Non_درجہ_وار_Bounds)
{
	ساخت HierarchyCase {
		اعشاری nmac_radius;
		اعشاری nmac_اونچائی;
		اعشاری wc_radius;
		اعشاری wc_اونچائی;
	};

	constexpr صحیح nmac_latency_s = 30;
	constexpr صحیح wc_latency_s = 30;
	ثابت std::array<HierarchyCase, 3> cases{{
			{20.f, 10.f, 19.f, 10.f},
			{20.f, 10.f, 20.f, 9.f},
			{200.f, 100.f, 100.f, 50.f},
		}};

	برائے (size_t i = 0; i < cases.سائز(); ++i) {
		ثابت HierarchyCase &test_case = cases[i];
		SCOPED_TRACE(i);

		سیٹ_f3442_params(test_case.nmac_radius, test_case.nmac_اونچائی, test_case.wc_radius, test_case.wc_اونچائی,
				 nmac_latency_s, wc_latency_s);

		EXPECT_FALSE(daa.try_setting_params());
	}
}

// is_in_bounds() is inclusive and fails on negative or non-finite inputs.
TEST_F(DaaF3442Test, Is_اندر_Bounds_Rejects_Invalid_Inputs)
{
	ثابت میٹرکس::Vector2f bounds(10.f, 10.f);
	ثابت میٹرکس::Vector2f within_bounds(5.f, 5.f);
	ثابت میٹرکس::Vector2f on_boundary(10.f, 10.f);
	ثابت میٹرکس::Vector2f outside_both_bounds(15.f, 15.f);
	ثابت میٹرکس::Vector2f outside_horizontal_bounds(15.f, 4.f);
	ثابت میٹرکس::Vector2f outside_vertical_bounds(4.f, 15.f);
	ثابت میٹرکس::Vector2f negative_bounds(-10.f, 10.f);
	ثابت میٹرکس::Vector2f negative_فاصلہ(-5.f, 5.f);
	ثابت میٹرکس::Vector2f non_finite_فاصلہ(5.f, std::numeric_limits<اعشاری>::quiet_NaN());

	EXPECT_TRUE(daa.is_اندر_bounds(within_bounds, bounds));
	EXPECT_TRUE(daa.is_اندر_bounds(on_boundary, bounds));
	EXPECT_FALSE(daa.is_اندر_bounds(outside_both_bounds, bounds));
	EXPECT_FALSE(daa.is_اندر_bounds(outside_horizontal_bounds, bounds));
	EXPECT_FALSE(daa.is_اندر_bounds(outside_vertical_bounds, bounds));
	EXPECT_FALSE(daa.is_اندر_bounds(within_bounds, negative_bounds));
	EXPECT_FALSE(daa.is_اندر_bounds(negative_فاصلہ, bounds));
	EXPECT_FALSE(daa.is_اندر_bounds(non_finite_فاصلہ, bounds));
}

// Volume augmentation uses |velocity| and |latency|, so signs never shrink it.
TEST_F(DaaF3442Test, Calculate_Aircraft_Conflict_Volume_استعمال_کرتا_Absolute_Inputs)
{
	ثابت میٹرکس::Vector2f بنیاد_bounds(10.f, 20.f);
	ثابت میٹرکس::Vector2f velocity(-2.f, 4.f);
	ثابت اعشاری latency_s = -5.f;
	ثابت اعشاری expected_horizontal_bounds_m = بنیاد_bounds(0) + fabsf(velocity(0)) * fabsf(latency_s);
	ثابت اعشاری expected_vertical_bounds_m = بنیاد_bounds(1) + fabsf(velocity(1)) * fabsf(latency_s);

	میٹرکس::Vector2f augmented_bounds{};
	daa.calculate_aircraft_conflict_volume(بنیاد_bounds, velocity, latency_s, augmented_bounds);

	EXPECT_FLOAT_EQ(augmented_bounds(0), expected_horizontal_bounds_m);
	EXPECT_FLOAT_EQ(augmented_bounds(1), expected_vertical_bounds_m);
}

// Augmented boundary sums both aircraft's volumes (motion from each side).
TEST_F(DaaF3442Test, CalculateAugmentedBoundariesAddsBothAircraftVolumes)
{
	ثابت میٹرکس::Vector2f بنیاد_bounds(10.f, 20.f);
	ثابت میٹرکس::Vector2f uav_velocity(2.f, 4.f);
	ثابت میٹرکس::Vector2f traffic_velocity(8.f, 10.f);
	ثابت اعشاری latency_s = 3.f;
	ثابت اعشاری expected_horizontal_bounds_m = 2.f * بنیاد_bounds(0)
			+ (fabsf(uav_velocity(0)) + fabsf(traffic_velocity(0))) * latency_s;
	ثابت اعشاری expected_vertical_bounds_m = 2.f * بنیاد_bounds(1)
			+ (fabsf(uav_velocity(1)) + fabsf(traffic_velocity(1))) * latency_s;

	میٹرکس::Vector2f augmented_bounds{};
	daa.calculate_augmented_boundaries(بنیاد_bounds, uav_velocity, traffic_velocity, latency_s, augmented_bounds);

	EXPECT_FLOAT_EQ(augmented_bounds(0), expected_horizontal_bounds_m);
	EXPECT_FLOAT_EQ(augmented_bounds(1), expected_vertical_bounds_m);
}

// calculate_conflict_level() returns the strictest breached zone; a single-axis miss is NONE.
TEST_F(DaaF3442Test, Calculate_Conflict_Level_Respects_ترجیح_ترتیب)
{
	constexpr اعشاری nmac_radius_m = 20.f;
	constexpr اعشاری nmac_اونچائی_m = 10.f;
	constexpr اعشاری wc_radius_m = 100.f;
	constexpr اعشاری wc_اونچائی_m = 50.f;
	constexpr صحیح nmac_latency_s = 30;
	constexpr صحیح wc_latency_s = 40;
	سیٹ_f3442_params(nmac_radius_m, nmac_اونچائی_m, wc_radius_m, wc_اونچائی_m, nmac_latency_s, wc_latency_s);
	ASSERT_TRUE(daa.try_setting_params());

	ثابت میٹرکس::Vector2f uav_velocity(10.f, 2.f);
	ثابت میٹرکس::Vector2f traffic_velocity(5.f, 4.f);
	ثابت میٹرکس::Vector2f nmac_bounds(2.f * nmac_radius_m, 2.f * nmac_اونچائی_m);
	ثابت میٹرکس::Vector2f wc_bounds(2.f * wc_radius_m, 2.f * wc_اونچائی_m);
	ثابت میٹرکس::Vector2f aug_nmac_bounds(
		2.f * nmac_radius_m + (fabsf(uav_velocity(0)) + fabsf(traffic_velocity(0))) * nmac_latency_s,
		2.f * nmac_اونچائی_m + (fabsf(uav_velocity(1)) + fabsf(traffic_velocity(1))) * nmac_latency_s);
	ثابت میٹرکس::Vector2f aug_wc_bounds(
		2.f * wc_radius_m + (fabsf(uav_velocity(0)) + fabsf(traffic_velocity(0))) * wc_latency_s,
		2.f * wc_اونچائی_m + (fabsf(uav_velocity(1)) + fabsf(traffic_velocity(1))) * wc_latency_s);
	constexpr اعشاری inside_margin_m = 1.f;
	ثابت میٹرکس::Vector2f critical_فاصلہ(nmac_bounds(0) - inside_margin_m, nmac_bounds(1) - inside_margin_m);
	ثابت میٹرکس::Vector2f بلند_فاصلہ(wc_bounds(0) - inside_margin_m, wc_bounds(1) - inside_margin_m);
	ثابت میٹرکس::Vector2f medium_فاصلہ(aug_nmac_bounds(0) - inside_margin_m, aug_nmac_bounds(1) - inside_margin_m);
	ثابت میٹرکس::Vector2f کم_فاصلہ(aug_wc_bounds(0) - inside_margin_m, aug_wc_bounds(1) - inside_margin_m);
	ثابت میٹرکس::Vector2f outside_کم_فاصلہ(aug_wc_bounds(0) + inside_margin_m, aug_wc_bounds(1) + inside_margin_m);
	ثابت میٹرکس::Vector2f outside_کم_vertical_only(کم_فاصلہ(0), aug_wc_bounds(1) + inside_margin_m);
	ثابت میٹرکس::Vector2f outside_کم_horizontal_only(aug_wc_bounds(0) + inside_margin_m, کم_فاصلہ(1));

	// same velocity pair against progressively weaker breaches
	EXPECT_EQ(daa.calculate_conflict_level(critical_فاصلہ, uav_velocity, traffic_velocity),
		  detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL);
	EXPECT_EQ(daa.calculate_conflict_level(بلند_فاصلہ, uav_velocity, traffic_velocity),
		  detect_and_avoid_s::DAA_CONFLICT_LVL_بلند);
	EXPECT_EQ(daa.calculate_conflict_level(medium_فاصلہ, uav_velocity, traffic_velocity),
		  detect_and_avoid_s::DAA_CONFLICT_LVL_MEDIUM);
	EXPECT_EQ(daa.calculate_conflict_level(کم_فاصلہ, uav_velocity, traffic_velocity),
		  detect_and_avoid_s::DAA_CONFLICT_LVL_کم);
	EXPECT_EQ(daa.calculate_conflict_level(outside_کم_فاصلہ, uav_velocity, traffic_velocity),
		  detect_and_avoid_s::DAA_CONFLICT_LVL_NONE);
	EXPECT_EQ(daa.calculate_conflict_level(outside_کم_vertical_only, uav_velocity, traffic_velocity),
		  detect_and_avoid_s::DAA_CONFLICT_LVL_NONE);
	EXPECT_EQ(daa.calculate_conflict_level(outside_کم_horizontal_only, uav_velocity, traffic_velocity),
		  detect_and_avoid_s::DAA_CONFLICT_LVL_NONE);
}

// calculate_daa_stats() reports measured geometry and severity for a near-collision and a clear pass.
TEST_F(DaaF3442Test, CalculateDaaStatsReportsExpectedGeometryAndLevel)
{
	constexpr اعشاری nmac_radius_m = 20.f;
	constexpr اعشاری nmac_اونچائی_m = 10.f;
	constexpr اعشاری wc_radius_m = 100.f;
	constexpr اعشاری wc_اونچائی_m = 50.f;
	constexpr صحیح nmac_latency_s = 30;
	constexpr صحیح wc_latency_s = 40;
	سیٹ_f3442_params(nmac_radius_m, nmac_اونچائی_m, wc_radius_m, wc_اونچائی_m, nmac_latency_s, wc_latency_s);
	ASSERT_TRUE(daa.try_setting_params());

	ثابت میٹرکس::Vector2d uav_lat_lon(46.52342, 6.524234);
	constexpr اعشاری uav_altitude_m = 400.f;
	ثابت میٹرکس::Vector3f uav_velocity_ned_m_s(10.f, 0.f, -2.f);
	constexpr اعشاری uav_heading_rad = 0.f;
	ثابت aircraft_state_s uav_state{
		uav_lat_lon,
		uav_altitude_m,
		uav_velocity_ned_m_s,
		uav_heading_rad
	};

	aircraft_state_s traffic_state{};
	ثابت میٹرکس::Vector3f traffic_velocity_ned_m_s(-5.f, 0.f, -4.f);
	ثابت اعشاری traffic_heading_rad = math::radians(180.f);
	ثابت اعشاری east_bearing_rad = math::radians(90.f);
	constexpr اعشاری expectation_tolerance = 0.1f;
	دوہرا traffic_lat{0.0};
	دوہرا traffic_lon{0.0};
	traffic_state.velocity_ned = traffic_velocity_ned_m_s;
	traffic_state.heading = traffic_heading_rad;

	خودکار رکھو_traffic_due_east = [&](اعشاری horizontal_فاصلہ_m, اعشاری traffic_altitude_m) {
		waypoint_from_heading_and_فاصلہ(uav_state.lat_lon(0), uav_state.lat_lon(1), east_bearing_rad, horizontal_فاصلہ_m,
						   &traffic_lat, &traffic_lon);
		traffic_state.lat_lon = میٹرکس::Vector2d(traffic_lat, traffic_lon);
		traffic_state.altitude = traffic_altitude_m;
	};

	ثابت اعشاری uav_رفتار_m_s = uav_velocity_ned_m_s.norm();
	ثابت اعشاری traffic_رفتار_m_s = traffic_velocity_ned_m_s.norm();
	ثابت اعشاری expected_relative_رفتار_m_s = uav_رفتار_m_s + traffic_رفتار_m_s;
	constexpr اعشاری near_horizontal_فاصلہ_m = 39.f;
	constexpr اعشاری near_vertical_separation_m = 9.f;
	ثابت اعشاری near_traffic_altitude_m = uav_altitude_m + near_vertical_separation_m;
	ثابت اعشاری near_aircraft_فاصلہ_m = hypotf(near_horizontal_فاصلہ_m, near_vertical_separation_m);
	رکھو_traffic_due_east(near_horizontal_فاصلہ_m, near_traffic_altitude_m);

	// near-conflict geometry
	daa_stats_s daa_stats{};
	EXPECT_EQ(daa.calculate_daa_stats(uav_state, traffic_state, daa_stats), detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL);
	EXPECT_NEAR(daa_stats.aircraft_dist, near_aircraft_فاصلہ_m, expectation_tolerance);
	EXPECT_NEAR(daa_stats.aircraft_dist_hor, near_horizontal_فاصلہ_m, expectation_tolerance);
	EXPECT_NEAR(daa_stats.aircraft_dist_vert, near_vertical_separation_m, expectation_tolerance);
	EXPECT_NEAR(daa_stats.expected_min_dist_وقت_sec, near_aircraft_فاصلہ_m / expected_relative_رفتار_m_s,
		    expectation_tolerance);

	// traffic well outside every protection volume
	constexpr اعشاری far_horizontal_فاصلہ_m = 900.f;
	constexpr اعشاری far_vertical_separation_m = 400.f;
	ثابت اعشاری far_traffic_altitude_m = uav_altitude_m + far_vertical_separation_m;
	ثابت اعشاری far_aircraft_فاصلہ_m = hypotf(far_horizontal_فاصلہ_m, far_vertical_separation_m);
	رکھو_traffic_due_east(far_horizontal_فاصلہ_m, far_traffic_altitude_m);

	EXPECT_EQ(daa.calculate_daa_stats(uav_state, traffic_state, daa_stats), detect_and_avoid_s::DAA_CONFLICT_LVL_NONE);
	EXPECT_NEAR(daa_stats.aircraft_dist, far_aircraft_فاصلہ_m, expectation_tolerance);
	EXPECT_NEAR(daa_stats.aircraft_dist_hor, far_horizontal_فاصلہ_m, expectation_tolerance);
	EXPECT_NEAR(daa_stats.aircraft_dist_vert, far_vertical_separation_m, expectation_tolerance);
	EXPECT_NEAR(daa_stats.expected_min_dist_وقت_sec, far_aircraft_فاصلہ_m / expected_relative_رفتار_m_s,
		    expectation_tolerance);
}
