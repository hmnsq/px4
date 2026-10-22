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

#ಅನ್ತರ್ಭಾವ <gtest/gtest.h>
#ಅನ್ತರ್ಭಾವ <parameters/param.h>

#ಅನ್ತರ್ಭಾವ <array>
#ಅನ್ತರ್ಭಾವ <lib/geo/geo.h>
#ಅನ್ತರ್ಭಾವ <lib/mathlib/mathlib.h>
#ಅನ್ತರ್ಭಾವ <limits>
#ಅನ್ತರ್ಭಾವ <uORB/topics/detect_and_avoid.h>

#ಅನ್ತರ್ಭಾವ "DaaF3442.h"

ವರ್ಗ DaaF3442Test : ಸಾರ್ವಜನಿಕ ::testing::Test
{
ಸಂರಕ್ಷಿತ:
	DaaF3442 daa;

	ಶೂನ್ಯ ಗಣ_ಮೇಲೆ() override
	{
		param_control_autosave(ಸುಳ್ಳು);
		param_reset_all();
	}

	ಶೂನ್ಯ ಗಣ_f3442_params(ದಶಮಾಂಶ nmac_radius, ದಶಮಾಂಶ nmac_ಎತ್ತರ, ದಶಮಾಂಶ wc_radius, ದಶಮಾಂಶ wc_ಎತ್ತರ,
			      ಪೂರ್ಣಾಂಕ nmac_latency_s, ಪೂರ್ಣಾಂಕ wc_latency_s)
	{
		param_ಗಣ(param_handle(px4::params::DAA_LVL_CRIT_RAD), &nmac_radius);
		param_ಗಣ(param_handle(px4::params::DAA_LVL_CRIT_HGT), &nmac_ಎತ್ತರ);
		param_ಗಣ(param_handle(px4::params::DAA_LVL_ಮೇಲಿನ_RAD), &wc_radius);
		param_ಗಣ(param_handle(px4::params::DAA_LVL_ಮೇಲಿನ_HGT), &wc_ಎತ್ತರ);
		param_ಗಣ(param_handle(px4::params::DAA_LVL_MED_ಸಮಯ), &nmac_latency_s);
		param_ಗಣ(param_handle(px4::params::DAA_LVL_ಕೆಳಗಿನ_ಸಮಯ), &wc_latency_s);
	}
};

TEST_F(DaaF3442Test, RejectsInvalidParams)
{
	ರಚನೆ ParamCase {
		ದಶಮಾಂಶ nmac_radius;
		ದಶಮಾಂಶ nmac_ಎತ್ತರ;
		ದಶಮಾಂಶ wc_radius;
		ದಶಮಾಂಶ wc_ಎತ್ತರ;
		ಪೂರ್ಣಾಂಕ nmac_latency_s;
		ಪೂರ್ಣಾಂಕ wc_latency_s;
		ತಾರ್ಕಿಕ expected_ಫಲಿತಾಂಶ;
	};

	constexpr ದಶಮಾಂಶ valid_nmac_radius_m = 10.f;
	constexpr ದಶಮಾಂಶ valid_nmac_ಎತ್ತರ_m = 10.f;
	constexpr ದಶಮಾಂಶ valid_wc_radius_m = 20.f;
	constexpr ದಶಮಾಂಶ valid_wc_ಎತ್ತರ_m = 20.f;
	constexpr ಪೂರ್ಣಾಂಕ valid_nmac_latency_s = 5;
	constexpr ಪೂರ್ಣಾಂಕ valid_wc_latency_s = 5;
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ nan = std::numeric_limits<ದಶಮಾಂಶ>::quiet_NaN();
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ inf = std::numeric_limits<ದಶಮಾಂಶ>::infinity();

	ಸ್ಥಿರಾಂಕ std::array<ParamCase, 13> cases{{
			{valid_nmac_radius_m, valid_nmac_ಎತ್ತರ_m, valid_wc_radius_m, valid_wc_ಎತ್ತರ_m, valid_nmac_latency_s, valid_wc_latency_s, ನಿಜ},
			{0.f, valid_nmac_ಎತ್ತರ_m, valid_wc_radius_m, valid_wc_ಎತ್ತರ_m, valid_nmac_latency_s, valid_wc_latency_s, ಸುಳ್ಳು},
			{-valid_nmac_radius_m, valid_nmac_ಎತ್ತರ_m, valid_wc_radius_m, valid_wc_ಎತ್ತರ_m, valid_nmac_latency_s, valid_wc_latency_s, ಸುಳ್ಳು},
			{valid_nmac_radius_m, -valid_nmac_ಎತ್ತರ_m, valid_wc_radius_m, valid_wc_ಎತ್ತರ_m, valid_nmac_latency_s, valid_wc_latency_s, ಸುಳ್ಳು},
			{valid_nmac_radius_m, valid_nmac_ಎತ್ತರ_m, -valid_wc_radius_m, valid_wc_ಎತ್ತರ_m, valid_nmac_latency_s, valid_wc_latency_s, ಸುಳ್ಳು},
			{valid_nmac_radius_m, valid_nmac_ಎತ್ತರ_m, valid_wc_radius_m, -valid_wc_ಎತ್ತರ_m, valid_nmac_latency_s, valid_wc_latency_s, ಸುಳ್ಳು},
			{nan, valid_nmac_ಎತ್ತರ_m, valid_wc_radius_m, valid_wc_ಎತ್ತರ_m, valid_nmac_latency_s, valid_wc_latency_s, ಸುಳ್ಳು},
			{valid_nmac_radius_m, inf, valid_wc_radius_m, valid_wc_ಎತ್ತರ_m, valid_nmac_latency_s, valid_wc_latency_s, ಸುಳ್ಳು},
			{valid_nmac_radius_m, valid_nmac_ಎತ್ತರ_m, nan, valid_wc_ಎತ್ತರ_m, valid_nmac_latency_s, valid_wc_latency_s, ಸುಳ್ಳು},
			{valid_nmac_radius_m, valid_nmac_ಎತ್ತರ_m, valid_wc_radius_m, inf, valid_nmac_latency_s, valid_wc_latency_s, ಸುಳ್ಳು},
			{valid_nmac_radius_m, valid_nmac_ಎತ್ತರ_m, valid_wc_radius_m, valid_wc_ಎತ್ತರ_m, -valid_nmac_latency_s, valid_wc_latency_s, ಸುಳ್ಳು},
			{valid_nmac_radius_m, valid_nmac_ಎತ್ತರ_m, valid_wc_radius_m, valid_wc_ಎತ್ತರ_m, valid_nmac_latency_s, -valid_wc_latency_s, ಸುಳ್ಳು},
			{valid_nmac_radius_m, valid_nmac_ಎತ್ತರ_m, valid_wc_radius_m, valid_wc_ಎತ್ತರ_m, valid_nmac_latency_s + 1, valid_wc_latency_s, ಸುಳ್ಳು},
		}};

	ಗಾಗಿ (size_t i = 0; i < cases.ಗಾತ್ರ_2(); ++i) {
		ಸ್ಥಿರಾಂಕ ParamCase &test_case = cases[i];
		SCOPED_TRACE(i);

		ಗಣ_f3442_params(test_case.nmac_radius, test_case.nmac_ಎತ್ತರ, test_case.wc_radius, test_case.wc_ಎತ್ತರ,
				 test_case.nmac_latency_s, test_case.wc_latency_s);
		EXPECT_EQ(daa.try_setting_params(), test_case.expected_ಫಲಿತಾಂಶ);
	}
}

// Bounds must nest: an outer zone no larger than the inner one is rejected.
TEST_F(DaaF3442Test, Rejects_Non_ಶ್ರೇಣೀಕೃತ_Bounds)
{
	ರಚನೆ HierarchyCase {
		ದಶಮಾಂಶ nmac_radius;
		ದಶಮಾಂಶ nmac_ಎತ್ತರ;
		ದಶಮಾಂಶ wc_radius;
		ದಶಮಾಂಶ wc_ಎತ್ತರ;
	};

	constexpr ಪೂರ್ಣಾಂಕ nmac_latency_s = 30;
	constexpr ಪೂರ್ಣಾಂಕ wc_latency_s = 30;
	ಸ್ಥಿರಾಂಕ std::array<HierarchyCase, 3> cases{{
			{20.f, 10.f, 19.f, 10.f},
			{20.f, 10.f, 20.f, 9.f},
			{200.f, 100.f, 100.f, 50.f},
		}};

	ಗಾಗಿ (size_t i = 0; i < cases.ಗಾತ್ರ_2(); ++i) {
		ಸ್ಥಿರಾಂಕ HierarchyCase &test_case = cases[i];
		SCOPED_TRACE(i);

		ಗಣ_f3442_params(test_case.nmac_radius, test_case.nmac_ಎತ್ತರ, test_case.wc_radius, test_case.wc_ಎತ್ತರ,
				 nmac_latency_s, wc_latency_s);

		EXPECT_FALSE(daa.try_setting_params());
	}
}

// is_in_bounds() is inclusive and fails on negative or non-finite inputs.
TEST_F(DaaF3442Test, Is_ಒಳ_Bounds_Rejects_Invalid_Inputs)
{
	ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::Vector2f bounds(10.f, 10.f);
	ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::Vector2f within_bounds(5.f, 5.f);
	ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::Vector2f on_boundary(10.f, 10.f);
	ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::Vector2f outside_both_bounds(15.f, 15.f);
	ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::Vector2f outside_horizontal_bounds(15.f, 4.f);
	ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::Vector2f outside_vertical_bounds(4.f, 15.f);
	ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::Vector2f negative_bounds(-10.f, 10.f);
	ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::Vector2f negative_ದೂರ(-5.f, 5.f);
	ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::Vector2f non_finite_ದೂರ(5.f, std::numeric_limits<ದಶಮಾಂಶ>::quiet_NaN());

	EXPECT_TRUE(daa.is_ಒಳ_bounds(within_bounds, bounds));
	EXPECT_TRUE(daa.is_ಒಳ_bounds(on_boundary, bounds));
	EXPECT_FALSE(daa.is_ಒಳ_bounds(outside_both_bounds, bounds));
	EXPECT_FALSE(daa.is_ಒಳ_bounds(outside_horizontal_bounds, bounds));
	EXPECT_FALSE(daa.is_ಒಳ_bounds(outside_vertical_bounds, bounds));
	EXPECT_FALSE(daa.is_ಒಳ_bounds(within_bounds, negative_bounds));
	EXPECT_FALSE(daa.is_ಒಳ_bounds(negative_ದೂರ, bounds));
	EXPECT_FALSE(daa.is_ಒಳ_bounds(non_finite_ದೂರ, bounds));
}

// Volume augmentation uses |velocity| and |latency|, so signs never shrink it.
TEST_F(DaaF3442Test, Calculate_Aircraft_Conflict_Volume_ಬಳಸುತ್ತದೆ_Absolute_Inputs)
{
	ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::Vector2f ತಳ_bounds(10.f, 20.f);
	ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::Vector2f velocity(-2.f, 4.f);
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ latency_s = -5.f;
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ expected_horizontal_bounds_m = ತಳ_bounds(0) + fabsf(velocity(0)) * fabsf(latency_s);
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ expected_vertical_bounds_m = ತಳ_bounds(1) + fabsf(velocity(1)) * fabsf(latency_s);

	ಮಾತೃಕೆ::Vector2f augmented_bounds{};
	daa.calculate_aircraft_conflict_volume(ತಳ_bounds, velocity, latency_s, augmented_bounds);

	EXPECT_FLOAT_EQ(augmented_bounds(0), expected_horizontal_bounds_m);
	EXPECT_FLOAT_EQ(augmented_bounds(1), expected_vertical_bounds_m);
}

// Augmented boundary sums both aircraft's volumes (motion from each side).
TEST_F(DaaF3442Test, CalculateAugmentedBoundariesAddsBothAircraftVolumes)
{
	ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::Vector2f ತಳ_bounds(10.f, 20.f);
	ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::Vector2f uav_velocity(2.f, 4.f);
	ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::Vector2f traffic_velocity(8.f, 10.f);
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ latency_s = 3.f;
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ expected_horizontal_bounds_m = 2.f * ತಳ_bounds(0)
			+ (fabsf(uav_velocity(0)) + fabsf(traffic_velocity(0))) * latency_s;
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ expected_vertical_bounds_m = 2.f * ತಳ_bounds(1)
			+ (fabsf(uav_velocity(1)) + fabsf(traffic_velocity(1))) * latency_s;

	ಮಾತೃಕೆ::Vector2f augmented_bounds{};
	daa.calculate_augmented_boundaries(ತಳ_bounds, uav_velocity, traffic_velocity, latency_s, augmented_bounds);

	EXPECT_FLOAT_EQ(augmented_bounds(0), expected_horizontal_bounds_m);
	EXPECT_FLOAT_EQ(augmented_bounds(1), expected_vertical_bounds_m);
}

// calculate_conflict_level() returns the strictest breached zone; a single-axis miss is NONE.
TEST_F(DaaF3442Test, Calculate_Conflict_Level_Respects_ಆದ್ಯತೆ_ಕ್ರಮ)
{
	constexpr ದಶಮಾಂಶ nmac_radius_m = 20.f;
	constexpr ದಶಮಾಂಶ nmac_ಎತ್ತರ_m = 10.f;
	constexpr ದಶಮಾಂಶ wc_radius_m = 100.f;
	constexpr ದಶಮಾಂಶ wc_ಎತ್ತರ_m = 50.f;
	constexpr ಪೂರ್ಣಾಂಕ nmac_latency_s = 30;
	constexpr ಪೂರ್ಣಾಂಕ wc_latency_s = 40;
	ಗಣ_f3442_params(nmac_radius_m, nmac_ಎತ್ತರ_m, wc_radius_m, wc_ಎತ್ತರ_m, nmac_latency_s, wc_latency_s);
	ASSERT_TRUE(daa.try_setting_params());

	ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::Vector2f uav_velocity(10.f, 2.f);
	ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::Vector2f traffic_velocity(5.f, 4.f);
	ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::Vector2f nmac_bounds(2.f * nmac_radius_m, 2.f * nmac_ಎತ್ತರ_m);
	ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::Vector2f wc_bounds(2.f * wc_radius_m, 2.f * wc_ಎತ್ತರ_m);
	ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::Vector2f aug_nmac_bounds(
		2.f * nmac_radius_m + (fabsf(uav_velocity(0)) + fabsf(traffic_velocity(0))) * nmac_latency_s,
		2.f * nmac_ಎತ್ತರ_m + (fabsf(uav_velocity(1)) + fabsf(traffic_velocity(1))) * nmac_latency_s);
	ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::Vector2f aug_wc_bounds(
		2.f * wc_radius_m + (fabsf(uav_velocity(0)) + fabsf(traffic_velocity(0))) * wc_latency_s,
		2.f * wc_ಎತ್ತರ_m + (fabsf(uav_velocity(1)) + fabsf(traffic_velocity(1))) * wc_latency_s);
	constexpr ದಶಮಾಂಶ inside_margin_m = 1.f;
	ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::Vector2f critical_ದೂರ(nmac_bounds(0) - inside_margin_m, nmac_bounds(1) - inside_margin_m);
	ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::Vector2f ಮೇಲಿನ_ದೂರ(wc_bounds(0) - inside_margin_m, wc_bounds(1) - inside_margin_m);
	ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::Vector2f medium_ದೂರ(aug_nmac_bounds(0) - inside_margin_m, aug_nmac_bounds(1) - inside_margin_m);
	ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::Vector2f ಕೆಳಗಿನ_ದೂರ(aug_wc_bounds(0) - inside_margin_m, aug_wc_bounds(1) - inside_margin_m);
	ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::Vector2f outside_ಕೆಳಗಿನ_ದೂರ(aug_wc_bounds(0) + inside_margin_m, aug_wc_bounds(1) + inside_margin_m);
	ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::Vector2f outside_ಕೆಳಗಿನ_vertical_only(ಕೆಳಗಿನ_ದೂರ(0), aug_wc_bounds(1) + inside_margin_m);
	ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::Vector2f outside_ಕೆಳಗಿನ_horizontal_only(aug_wc_bounds(0) + inside_margin_m, ಕೆಳಗಿನ_ದೂರ(1));

	// same velocity pair against progressively weaker breaches
	EXPECT_EQ(daa.calculate_conflict_level(critical_ದೂರ, uav_velocity, traffic_velocity),
		  detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL);
	EXPECT_EQ(daa.calculate_conflict_level(ಮೇಲಿನ_ದೂರ, uav_velocity, traffic_velocity),
		  detect_and_avoid_s::DAA_CONFLICT_LVL_ಮೇಲಿನ);
	EXPECT_EQ(daa.calculate_conflict_level(medium_ದೂರ, uav_velocity, traffic_velocity),
		  detect_and_avoid_s::DAA_CONFLICT_LVL_MEDIUM);
	EXPECT_EQ(daa.calculate_conflict_level(ಕೆಳಗಿನ_ದೂರ, uav_velocity, traffic_velocity),
		  detect_and_avoid_s::DAA_CONFLICT_LVL_ಕೆಳಗಿನ);
	EXPECT_EQ(daa.calculate_conflict_level(outside_ಕೆಳಗಿನ_ದೂರ, uav_velocity, traffic_velocity),
		  detect_and_avoid_s::DAA_CONFLICT_LVL_NONE);
	EXPECT_EQ(daa.calculate_conflict_level(outside_ಕೆಳಗಿನ_vertical_only, uav_velocity, traffic_velocity),
		  detect_and_avoid_s::DAA_CONFLICT_LVL_NONE);
	EXPECT_EQ(daa.calculate_conflict_level(outside_ಕೆಳಗಿನ_horizontal_only, uav_velocity, traffic_velocity),
		  detect_and_avoid_s::DAA_CONFLICT_LVL_NONE);
}

// calculate_daa_stats() reports measured geometry and severity for a near-collision and a clear pass.
TEST_F(DaaF3442Test, CalculateDaaStatsReportsExpectedGeometryAndLevel)
{
	constexpr ದಶಮಾಂಶ nmac_radius_m = 20.f;
	constexpr ದಶಮಾಂಶ nmac_ಎತ್ತರ_m = 10.f;
	constexpr ದಶಮಾಂಶ wc_radius_m = 100.f;
	constexpr ದಶಮಾಂಶ wc_ಎತ್ತರ_m = 50.f;
	constexpr ಪೂರ್ಣಾಂಕ nmac_latency_s = 30;
	constexpr ಪೂರ್ಣಾಂಕ wc_latency_s = 40;
	ಗಣ_f3442_params(nmac_radius_m, nmac_ಎತ್ತರ_m, wc_radius_m, wc_ಎತ್ತರ_m, nmac_latency_s, wc_latency_s);
	ASSERT_TRUE(daa.try_setting_params());

	ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::Vector2d uav_lat_lon(46.52342, 6.524234);
	constexpr ದಶಮಾಂಶ uav_altitude_m = 400.f;
	ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::Vector3f uav_velocity_ned_m_s(10.f, 0.f, -2.f);
	constexpr ದಶಮಾಂಶ uav_heading_rad = 0.f;
	ಸ್ಥಿರಾಂಕ aircraft_state_s uav_state{
		uav_lat_lon,
		uav_altitude_m,
		uav_velocity_ned_m_s,
		uav_heading_rad
	};

	aircraft_state_s traffic_state{};
	ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::Vector3f traffic_velocity_ned_m_s(-5.f, 0.f, -4.f);
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ traffic_heading_rad = math::radians(180.f);
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ east_bearing_rad = math::radians(90.f);
	constexpr ದಶಮಾಂಶ expectation_tolerance = 0.1f;
	ದ್ವಿಗುಣ traffic_lat{0.0};
	ದ್ವಿಗುಣ traffic_lon{0.0};
	traffic_state.velocity_ned = traffic_velocity_ned_m_s;
	traffic_state.heading = traffic_heading_rad;

	ಸ್ವಯಂಚಾಲಿತ ಇರಿಸು_traffic_due_east = [&](ದಶಮಾಂಶ horizontal_ದೂರ_m, ದಶಮಾಂಶ traffic_altitude_m) {
		waypoint_from_heading_and_ದೂರ(uav_state.lat_lon(0), uav_state.lat_lon(1), east_bearing_rad, horizontal_ದೂರ_m,
						   &traffic_lat, &traffic_lon);
		traffic_state.lat_lon = ಮಾತೃಕೆ::Vector2d(traffic_lat, traffic_lon);
		traffic_state.altitude = traffic_altitude_m;
	};

	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ uav_ವೇಗ_m_s = uav_velocity_ned_m_s.norm();
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ traffic_ವೇಗ_m_s = traffic_velocity_ned_m_s.norm();
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ expected_relative_ವೇಗ_m_s = uav_ವೇಗ_m_s + traffic_ವೇಗ_m_s;
	constexpr ದಶಮಾಂಶ near_horizontal_ದೂರ_m = 39.f;
	constexpr ದಶಮಾಂಶ near_vertical_separation_m = 9.f;
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ near_traffic_altitude_m = uav_altitude_m + near_vertical_separation_m;
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ near_aircraft_ದೂರ_m = hypotf(near_horizontal_ದೂರ_m, near_vertical_separation_m);
	ಇರಿಸು_traffic_due_east(near_horizontal_ದೂರ_m, near_traffic_altitude_m);

	// near-conflict geometry
	daa_stats_s daa_stats{};
	EXPECT_EQ(daa.calculate_daa_stats(uav_state, traffic_state, daa_stats), detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL);
	EXPECT_NEAR(daa_stats.aircraft_dist, near_aircraft_ದೂರ_m, expectation_tolerance);
	EXPECT_NEAR(daa_stats.aircraft_dist_hor, near_horizontal_ದೂರ_m, expectation_tolerance);
	EXPECT_NEAR(daa_stats.aircraft_dist_vert, near_vertical_separation_m, expectation_tolerance);
	EXPECT_NEAR(daa_stats.expected_min_dist_ಸಮಯ_sec, near_aircraft_ದೂರ_m / expected_relative_ವೇಗ_m_s,
		    expectation_tolerance);

	// traffic well outside every protection volume
	constexpr ದಶಮಾಂಶ far_horizontal_ದೂರ_m = 900.f;
	constexpr ದಶಮಾಂಶ far_vertical_separation_m = 400.f;
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ far_traffic_altitude_m = uav_altitude_m + far_vertical_separation_m;
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ far_aircraft_ದೂರ_m = hypotf(far_horizontal_ದೂರ_m, far_vertical_separation_m);
	ಇರಿಸು_traffic_due_east(far_horizontal_ದೂರ_m, far_traffic_altitude_m);

	EXPECT_EQ(daa.calculate_daa_stats(uav_state, traffic_state, daa_stats), detect_and_avoid_s::DAA_CONFLICT_LVL_NONE);
	EXPECT_NEAR(daa_stats.aircraft_dist, far_aircraft_ದೂರ_m, expectation_tolerance);
	EXPECT_NEAR(daa_stats.aircraft_dist_hor, far_horizontal_ದೂರ_m, expectation_tolerance);
	EXPECT_NEAR(daa_stats.aircraft_dist_vert, far_vertical_separation_m, expectation_tolerance);
	EXPECT_NEAR(daa_stats.expected_min_dist_ಸಮಯ_sec, far_aircraft_ದೂರ_m / expected_relative_ವೇಗ_m_s,
		    expectation_tolerance);
}
