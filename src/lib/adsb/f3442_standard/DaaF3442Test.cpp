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

#ਅਨ੍ਤਰ੍ਭਾਵ <gtest/gtest.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <parameters/param.h>

#ਅਨ੍ਤਰ੍ਭਾਵ <array>
#ਅਨ੍ਤਰ੍ਭਾਵ <lib/geo/geo.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <lib/mathlib/mathlib.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <limits>
#ਅਨ੍ਤਰ੍ਭਾਵ <uORB/topics/detect_and_avoid.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "DaaF3442.h"

ਵਰਗ DaaF3442Test : ਜਨਤਕ ::testing::Test
{
ਸੁਰੱਖਿਅਤ:
	DaaF3442 daa;

	ਖਾਲੀ ਸੈੱਟ_ਉੱਪਰ() override
	{
		param_control_autosave(ਝੂਠ);
		param_reset_all();
	}

	ਖਾਲੀ ਸੈੱਟ_f3442_params(ਦਸ਼ਮਲਵ nmac_radius, ਦਸ਼ਮਲਵ nmac_ਉਚਾਈ, ਦਸ਼ਮਲਵ wc_radius, ਦਸ਼ਮਲਵ wc_ਉਚਾਈ,
			      ਪੂਰਨ nmac_latency_s, ਪੂਰਨ wc_latency_s)
	{
		param_ਸੈੱਟ(param_handle(px4::params::DAA_LVL_CRIT_RAD), &nmac_radius);
		param_ਸੈੱਟ(param_handle(px4::params::DAA_LVL_CRIT_HGT), &nmac_ਉਚਾਈ);
		param_ਸੈੱਟ(param_handle(px4::params::DAA_LVL_ਉੱਚਾ_RAD), &wc_radius);
		param_ਸੈੱਟ(param_handle(px4::params::DAA_LVL_ਉੱਚਾ_HGT), &wc_ਉਚਾਈ);
		param_ਸੈੱਟ(param_handle(px4::params::DAA_LVL_MED_ਸਮਾਂ), &nmac_latency_s);
		param_ਸੈੱਟ(param_handle(px4::params::DAA_LVL_ਨੀਵਾਂ_ਸਮਾਂ), &wc_latency_s);
	}
};

TEST_F(DaaF3442Test, RejectsInvalidParams)
{
	ਬਣਤਰ ParamCase {
		ਦਸ਼ਮਲਵ nmac_radius;
		ਦਸ਼ਮਲਵ nmac_ਉਚਾਈ;
		ਦਸ਼ਮਲਵ wc_radius;
		ਦਸ਼ਮਲਵ wc_ਉਚਾਈ;
		ਪੂਰਨ nmac_latency_s;
		ਪੂਰਨ wc_latency_s;
		ਤਾਰਕਿਕ expected_ਨਤੀਜਾ;
	};

	constexpr ਦਸ਼ਮਲਵ valid_nmac_radius_m = 10.f;
	constexpr ਦਸ਼ਮਲਵ valid_nmac_ਉਚਾਈ_m = 10.f;
	constexpr ਦਸ਼ਮਲਵ valid_wc_radius_m = 20.f;
	constexpr ਦਸ਼ਮਲਵ valid_wc_ਉਚਾਈ_m = 20.f;
	constexpr ਪੂਰਨ valid_nmac_latency_s = 5;
	constexpr ਪੂਰਨ valid_wc_latency_s = 5;
	ਅਚਲ ਦਸ਼ਮਲਵ nan = std::numeric_limits<ਦਸ਼ਮਲਵ>::quiet_NaN();
	ਅਚਲ ਦਸ਼ਮਲਵ inf = std::numeric_limits<ਦਸ਼ਮਲਵ>::infinity();

	ਅਚਲ std::array<ParamCase, 13> cases{{
			{valid_nmac_radius_m, valid_nmac_ਉਚਾਈ_m, valid_wc_radius_m, valid_wc_ਉਚਾਈ_m, valid_nmac_latency_s, valid_wc_latency_s, ਸੱਚ},
			{0.f, valid_nmac_ਉਚਾਈ_m, valid_wc_radius_m, valid_wc_ਉਚਾਈ_m, valid_nmac_latency_s, valid_wc_latency_s, ਝੂਠ},
			{-valid_nmac_radius_m, valid_nmac_ਉਚਾਈ_m, valid_wc_radius_m, valid_wc_ਉਚਾਈ_m, valid_nmac_latency_s, valid_wc_latency_s, ਝੂਠ},
			{valid_nmac_radius_m, -valid_nmac_ਉਚਾਈ_m, valid_wc_radius_m, valid_wc_ਉਚਾਈ_m, valid_nmac_latency_s, valid_wc_latency_s, ਝੂਠ},
			{valid_nmac_radius_m, valid_nmac_ਉਚਾਈ_m, -valid_wc_radius_m, valid_wc_ਉਚਾਈ_m, valid_nmac_latency_s, valid_wc_latency_s, ਝੂਠ},
			{valid_nmac_radius_m, valid_nmac_ਉਚਾਈ_m, valid_wc_radius_m, -valid_wc_ਉਚਾਈ_m, valid_nmac_latency_s, valid_wc_latency_s, ਝੂਠ},
			{nan, valid_nmac_ਉਚਾਈ_m, valid_wc_radius_m, valid_wc_ਉਚਾਈ_m, valid_nmac_latency_s, valid_wc_latency_s, ਝੂਠ},
			{valid_nmac_radius_m, inf, valid_wc_radius_m, valid_wc_ਉਚਾਈ_m, valid_nmac_latency_s, valid_wc_latency_s, ਝੂਠ},
			{valid_nmac_radius_m, valid_nmac_ਉਚਾਈ_m, nan, valid_wc_ਉਚਾਈ_m, valid_nmac_latency_s, valid_wc_latency_s, ਝੂਠ},
			{valid_nmac_radius_m, valid_nmac_ਉਚਾਈ_m, valid_wc_radius_m, inf, valid_nmac_latency_s, valid_wc_latency_s, ਝੂਠ},
			{valid_nmac_radius_m, valid_nmac_ਉਚਾਈ_m, valid_wc_radius_m, valid_wc_ਉਚਾਈ_m, -valid_nmac_latency_s, valid_wc_latency_s, ਝੂਠ},
			{valid_nmac_radius_m, valid_nmac_ਉਚਾਈ_m, valid_wc_radius_m, valid_wc_ਉਚਾਈ_m, valid_nmac_latency_s, -valid_wc_latency_s, ਝੂਠ},
			{valid_nmac_radius_m, valid_nmac_ਉਚਾਈ_m, valid_wc_radius_m, valid_wc_ਉਚਾਈ_m, valid_nmac_latency_s + 1, valid_wc_latency_s, ਝੂਠ},
		}};

	ਲਈ (size_t i = 0; i < cases.ਮਾਪ(); ++i) {
		ਅਚਲ ParamCase &test_case = cases[i];
		SCOPED_TRACE(i);

		ਸੈੱਟ_f3442_params(test_case.nmac_radius, test_case.nmac_ਉਚਾਈ, test_case.wc_radius, test_case.wc_ਉਚਾਈ,
				 test_case.nmac_latency_s, test_case.wc_latency_s);
		EXPECT_EQ(daa.try_setting_params(), test_case.expected_ਨਤੀਜਾ);
	}
}

// Bounds must nest: an outer zone no larger than the inner one is rejected.
TEST_F(DaaF3442Test, Rejects_Non_ਦਰਜਾਵਾਰ_Bounds)
{
	ਬਣਤਰ HierarchyCase {
		ਦਸ਼ਮਲਵ nmac_radius;
		ਦਸ਼ਮਲਵ nmac_ਉਚਾਈ;
		ਦਸ਼ਮਲਵ wc_radius;
		ਦਸ਼ਮਲਵ wc_ਉਚਾਈ;
	};

	constexpr ਪੂਰਨ nmac_latency_s = 30;
	constexpr ਪੂਰਨ wc_latency_s = 30;
	ਅਚਲ std::array<HierarchyCase, 3> cases{{
			{20.f, 10.f, 19.f, 10.f},
			{20.f, 10.f, 20.f, 9.f},
			{200.f, 100.f, 100.f, 50.f},
		}};

	ਲਈ (size_t i = 0; i < cases.ਮਾਪ(); ++i) {
		ਅਚਲ HierarchyCase &test_case = cases[i];
		SCOPED_TRACE(i);

		ਸੈੱਟ_f3442_params(test_case.nmac_radius, test_case.nmac_ਉਚਾਈ, test_case.wc_radius, test_case.wc_ਉਚਾਈ,
				 nmac_latency_s, wc_latency_s);

		EXPECT_FALSE(daa.try_setting_params());
	}
}

// is_in_bounds() is inclusive and fails on negative or non-finite inputs.
TEST_F(DaaF3442Test, Is_ਵਿੱਚ_Bounds_Rejects_Invalid_Inputs)
{
	ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector2f bounds(10.f, 10.f);
	ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector2f within_bounds(5.f, 5.f);
	ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector2f on_boundary(10.f, 10.f);
	ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector2f outside_both_bounds(15.f, 15.f);
	ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector2f outside_horizontal_bounds(15.f, 4.f);
	ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector2f outside_vertical_bounds(4.f, 15.f);
	ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector2f negative_bounds(-10.f, 10.f);
	ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector2f negative_ਦੂਰੀ(-5.f, 5.f);
	ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector2f non_finite_ਦੂਰੀ(5.f, std::numeric_limits<ਦਸ਼ਮਲਵ>::quiet_NaN());

	EXPECT_TRUE(daa.is_ਵਿੱਚ_bounds(within_bounds, bounds));
	EXPECT_TRUE(daa.is_ਵਿੱਚ_bounds(on_boundary, bounds));
	EXPECT_FALSE(daa.is_ਵਿੱਚ_bounds(outside_both_bounds, bounds));
	EXPECT_FALSE(daa.is_ਵਿੱਚ_bounds(outside_horizontal_bounds, bounds));
	EXPECT_FALSE(daa.is_ਵਿੱਚ_bounds(outside_vertical_bounds, bounds));
	EXPECT_FALSE(daa.is_ਵਿੱਚ_bounds(within_bounds, negative_bounds));
	EXPECT_FALSE(daa.is_ਵਿੱਚ_bounds(negative_ਦੂਰੀ, bounds));
	EXPECT_FALSE(daa.is_ਵਿੱਚ_bounds(non_finite_ਦੂਰੀ, bounds));
}

// Volume augmentation uses |velocity| and |latency|, so signs never shrink it.
TEST_F(DaaF3442Test, Calculate_Aircraft_Conflict_Volume_ਵਰਤਦਾ_Absolute_Inputs)
{
	ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector2f ਅਧਾਰ_bounds(10.f, 20.f);
	ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector2f velocity(-2.f, 4.f);
	ਅਚਲ ਦਸ਼ਮਲਵ latency_s = -5.f;
	ਅਚਲ ਦਸ਼ਮਲਵ expected_horizontal_bounds_m = ਅਧਾਰ_bounds(0) + fabsf(velocity(0)) * fabsf(latency_s);
	ਅਚਲ ਦਸ਼ਮਲਵ expected_vertical_bounds_m = ਅਧਾਰ_bounds(1) + fabsf(velocity(1)) * fabsf(latency_s);

	ਮੈਟ੍ਰਿਕਸ::Vector2f augmented_bounds{};
	daa.calculate_aircraft_conflict_volume(ਅਧਾਰ_bounds, velocity, latency_s, augmented_bounds);

	EXPECT_FLOAT_EQ(augmented_bounds(0), expected_horizontal_bounds_m);
	EXPECT_FLOAT_EQ(augmented_bounds(1), expected_vertical_bounds_m);
}

// Augmented boundary sums both aircraft's volumes (motion from each side).
TEST_F(DaaF3442Test, CalculateAugmentedBoundariesAddsBothAircraftVolumes)
{
	ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector2f ਅਧਾਰ_bounds(10.f, 20.f);
	ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector2f uav_velocity(2.f, 4.f);
	ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector2f traffic_velocity(8.f, 10.f);
	ਅਚਲ ਦਸ਼ਮਲਵ latency_s = 3.f;
	ਅਚਲ ਦਸ਼ਮਲਵ expected_horizontal_bounds_m = 2.f * ਅਧਾਰ_bounds(0)
			+ (fabsf(uav_velocity(0)) + fabsf(traffic_velocity(0))) * latency_s;
	ਅਚਲ ਦਸ਼ਮਲਵ expected_vertical_bounds_m = 2.f * ਅਧਾਰ_bounds(1)
			+ (fabsf(uav_velocity(1)) + fabsf(traffic_velocity(1))) * latency_s;

	ਮੈਟ੍ਰਿਕਸ::Vector2f augmented_bounds{};
	daa.calculate_augmented_boundaries(ਅਧਾਰ_bounds, uav_velocity, traffic_velocity, latency_s, augmented_bounds);

	EXPECT_FLOAT_EQ(augmented_bounds(0), expected_horizontal_bounds_m);
	EXPECT_FLOAT_EQ(augmented_bounds(1), expected_vertical_bounds_m);
}

// calculate_conflict_level() returns the strictest breached zone; a single-axis miss is NONE.
TEST_F(DaaF3442Test, Calculate_Conflict_Level_Respects_ਤਰਜੀਹ_ਕ੍ਰਮ)
{
	constexpr ਦਸ਼ਮਲਵ nmac_radius_m = 20.f;
	constexpr ਦਸ਼ਮਲਵ nmac_ਉਚਾਈ_m = 10.f;
	constexpr ਦਸ਼ਮਲਵ wc_radius_m = 100.f;
	constexpr ਦਸ਼ਮਲਵ wc_ਉਚਾਈ_m = 50.f;
	constexpr ਪੂਰਨ nmac_latency_s = 30;
	constexpr ਪੂਰਨ wc_latency_s = 40;
	ਸੈੱਟ_f3442_params(nmac_radius_m, nmac_ਉਚਾਈ_m, wc_radius_m, wc_ਉਚਾਈ_m, nmac_latency_s, wc_latency_s);
	ASSERT_TRUE(daa.try_setting_params());

	ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector2f uav_velocity(10.f, 2.f);
	ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector2f traffic_velocity(5.f, 4.f);
	ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector2f nmac_bounds(2.f * nmac_radius_m, 2.f * nmac_ਉਚਾਈ_m);
	ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector2f wc_bounds(2.f * wc_radius_m, 2.f * wc_ਉਚਾਈ_m);
	ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector2f aug_nmac_bounds(
		2.f * nmac_radius_m + (fabsf(uav_velocity(0)) + fabsf(traffic_velocity(0))) * nmac_latency_s,
		2.f * nmac_ਉਚਾਈ_m + (fabsf(uav_velocity(1)) + fabsf(traffic_velocity(1))) * nmac_latency_s);
	ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector2f aug_wc_bounds(
		2.f * wc_radius_m + (fabsf(uav_velocity(0)) + fabsf(traffic_velocity(0))) * wc_latency_s,
		2.f * wc_ਉਚਾਈ_m + (fabsf(uav_velocity(1)) + fabsf(traffic_velocity(1))) * wc_latency_s);
	constexpr ਦਸ਼ਮਲਵ inside_margin_m = 1.f;
	ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector2f critical_ਦੂਰੀ(nmac_bounds(0) - inside_margin_m, nmac_bounds(1) - inside_margin_m);
	ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector2f ਉੱਚਾ_ਦੂਰੀ(wc_bounds(0) - inside_margin_m, wc_bounds(1) - inside_margin_m);
	ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector2f medium_ਦੂਰੀ(aug_nmac_bounds(0) - inside_margin_m, aug_nmac_bounds(1) - inside_margin_m);
	ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector2f ਨੀਵਾਂ_ਦੂਰੀ(aug_wc_bounds(0) - inside_margin_m, aug_wc_bounds(1) - inside_margin_m);
	ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector2f outside_ਨੀਵਾਂ_ਦੂਰੀ(aug_wc_bounds(0) + inside_margin_m, aug_wc_bounds(1) + inside_margin_m);
	ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector2f outside_ਨੀਵਾਂ_vertical_only(ਨੀਵਾਂ_ਦੂਰੀ(0), aug_wc_bounds(1) + inside_margin_m);
	ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector2f outside_ਨੀਵਾਂ_horizontal_only(aug_wc_bounds(0) + inside_margin_m, ਨੀਵਾਂ_ਦੂਰੀ(1));

	// same velocity pair against progressively weaker breaches
	EXPECT_EQ(daa.calculate_conflict_level(critical_ਦੂਰੀ, uav_velocity, traffic_velocity),
		  detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL);
	EXPECT_EQ(daa.calculate_conflict_level(ਉੱਚਾ_ਦੂਰੀ, uav_velocity, traffic_velocity),
		  detect_and_avoid_s::DAA_CONFLICT_LVL_ਉੱਚਾ);
	EXPECT_EQ(daa.calculate_conflict_level(medium_ਦੂਰੀ, uav_velocity, traffic_velocity),
		  detect_and_avoid_s::DAA_CONFLICT_LVL_MEDIUM);
	EXPECT_EQ(daa.calculate_conflict_level(ਨੀਵਾਂ_ਦੂਰੀ, uav_velocity, traffic_velocity),
		  detect_and_avoid_s::DAA_CONFLICT_LVL_ਨੀਵਾਂ);
	EXPECT_EQ(daa.calculate_conflict_level(outside_ਨੀਵਾਂ_ਦੂਰੀ, uav_velocity, traffic_velocity),
		  detect_and_avoid_s::DAA_CONFLICT_LVL_NONE);
	EXPECT_EQ(daa.calculate_conflict_level(outside_ਨੀਵਾਂ_vertical_only, uav_velocity, traffic_velocity),
		  detect_and_avoid_s::DAA_CONFLICT_LVL_NONE);
	EXPECT_EQ(daa.calculate_conflict_level(outside_ਨੀਵਾਂ_horizontal_only, uav_velocity, traffic_velocity),
		  detect_and_avoid_s::DAA_CONFLICT_LVL_NONE);
}

// calculate_daa_stats() reports measured geometry and severity for a near-collision and a clear pass.
TEST_F(DaaF3442Test, CalculateDaaStatsReportsExpectedGeometryAndLevel)
{
	constexpr ਦਸ਼ਮਲਵ nmac_radius_m = 20.f;
	constexpr ਦਸ਼ਮਲਵ nmac_ਉਚਾਈ_m = 10.f;
	constexpr ਦਸ਼ਮਲਵ wc_radius_m = 100.f;
	constexpr ਦਸ਼ਮਲਵ wc_ਉਚਾਈ_m = 50.f;
	constexpr ਪੂਰਨ nmac_latency_s = 30;
	constexpr ਪੂਰਨ wc_latency_s = 40;
	ਸੈੱਟ_f3442_params(nmac_radius_m, nmac_ਉਚਾਈ_m, wc_radius_m, wc_ਉਚਾਈ_m, nmac_latency_s, wc_latency_s);
	ASSERT_TRUE(daa.try_setting_params());

	ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector2d uav_lat_lon(46.52342, 6.524234);
	constexpr ਦਸ਼ਮਲਵ uav_altitude_m = 400.f;
	ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector3f uav_velocity_ned_m_s(10.f, 0.f, -2.f);
	constexpr ਦਸ਼ਮਲਵ uav_heading_rad = 0.f;
	ਅਚਲ aircraft_state_s uav_state{
		uav_lat_lon,
		uav_altitude_m,
		uav_velocity_ned_m_s,
		uav_heading_rad
	};

	aircraft_state_s traffic_state{};
	ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector3f traffic_velocity_ned_m_s(-5.f, 0.f, -4.f);
	ਅਚਲ ਦਸ਼ਮਲਵ traffic_heading_rad = math::radians(180.f);
	ਅਚਲ ਦਸ਼ਮਲਵ east_bearing_rad = math::radians(90.f);
	constexpr ਦਸ਼ਮਲਵ expectation_tolerance = 0.1f;
	ਦੁੱਗਣਾ traffic_lat{0.0};
	ਦੁੱਗਣਾ traffic_lon{0.0};
	traffic_state.velocity_ned = traffic_velocity_ned_m_s;
	traffic_state.heading = traffic_heading_rad;

	ਸਵੈਚਾਲਿਤ ਰੱਖੋ_traffic_due_east = [&](ਦਸ਼ਮਲਵ horizontal_ਦੂਰੀ_m, ਦਸ਼ਮਲਵ traffic_altitude_m) {
		waypoint_from_heading_and_ਦੂਰੀ(uav_state.lat_lon(0), uav_state.lat_lon(1), east_bearing_rad, horizontal_ਦੂਰੀ_m,
						   &traffic_lat, &traffic_lon);
		traffic_state.lat_lon = ਮੈਟ੍ਰਿਕਸ::Vector2d(traffic_lat, traffic_lon);
		traffic_state.altitude = traffic_altitude_m;
	};

	ਅਚਲ ਦਸ਼ਮਲਵ uav_ਰਫਤਾਰ_m_s = uav_velocity_ned_m_s.norm();
	ਅਚਲ ਦਸ਼ਮਲਵ traffic_ਰਫਤਾਰ_m_s = traffic_velocity_ned_m_s.norm();
	ਅਚਲ ਦਸ਼ਮਲਵ expected_relative_ਰਫਤਾਰ_m_s = uav_ਰਫਤਾਰ_m_s + traffic_ਰਫਤਾਰ_m_s;
	constexpr ਦਸ਼ਮਲਵ near_horizontal_ਦੂਰੀ_m = 39.f;
	constexpr ਦਸ਼ਮਲਵ near_vertical_separation_m = 9.f;
	ਅਚਲ ਦਸ਼ਮਲਵ near_traffic_altitude_m = uav_altitude_m + near_vertical_separation_m;
	ਅਚਲ ਦਸ਼ਮਲਵ near_aircraft_ਦੂਰੀ_m = hypotf(near_horizontal_ਦੂਰੀ_m, near_vertical_separation_m);
	ਰੱਖੋ_traffic_due_east(near_horizontal_ਦੂਰੀ_m, near_traffic_altitude_m);

	// near-conflict geometry
	daa_stats_s daa_stats{};
	EXPECT_EQ(daa.calculate_daa_stats(uav_state, traffic_state, daa_stats), detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL);
	EXPECT_NEAR(daa_stats.aircraft_dist, near_aircraft_ਦੂਰੀ_m, expectation_tolerance);
	EXPECT_NEAR(daa_stats.aircraft_dist_hor, near_horizontal_ਦੂਰੀ_m, expectation_tolerance);
	EXPECT_NEAR(daa_stats.aircraft_dist_vert, near_vertical_separation_m, expectation_tolerance);
	EXPECT_NEAR(daa_stats.expected_min_dist_ਸਮਾਂ_sec, near_aircraft_ਦੂਰੀ_m / expected_relative_ਰਫਤਾਰ_m_s,
		    expectation_tolerance);

	// traffic well outside every protection volume
	constexpr ਦਸ਼ਮਲਵ far_horizontal_ਦੂਰੀ_m = 900.f;
	constexpr ਦਸ਼ਮਲਵ far_vertical_separation_m = 400.f;
	ਅਚਲ ਦਸ਼ਮਲਵ far_traffic_altitude_m = uav_altitude_m + far_vertical_separation_m;
	ਅਚਲ ਦਸ਼ਮਲਵ far_aircraft_ਦੂਰੀ_m = hypotf(far_horizontal_ਦੂਰੀ_m, far_vertical_separation_m);
	ਰੱਖੋ_traffic_due_east(far_horizontal_ਦੂਰੀ_m, far_traffic_altitude_m);

	EXPECT_EQ(daa.calculate_daa_stats(uav_state, traffic_state, daa_stats), detect_and_avoid_s::DAA_CONFLICT_LVL_NONE);
	EXPECT_NEAR(daa_stats.aircraft_dist, far_aircraft_ਦੂਰੀ_m, expectation_tolerance);
	EXPECT_NEAR(daa_stats.aircraft_dist_hor, far_horizontal_ਦੂਰੀ_m, expectation_tolerance);
	EXPECT_NEAR(daa_stats.aircraft_dist_vert, far_vertical_separation_m, expectation_tolerance);
	EXPECT_NEAR(daa_stats.expected_min_dist_ਸਮਾਂ_sec, far_aircraft_ਦੂਰੀ_m / expected_relative_ਰਫਤਾਰ_m_s,
		    expectation_tolerance);
}
