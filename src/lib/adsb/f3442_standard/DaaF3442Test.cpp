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

#అన్తర్భావ <gtest/gtest.h>
#అన్తర్భావ <parameters/param.h>

#అన్తర్భావ <array>
#అన్తర్భావ <lib/geo/geo.h>
#అన్తర్భావ <lib/mathlib/mathlib.h>
#అన్తర్భావ <limits>
#అన్తర్భావ <uORB/topics/detect_and_avoid.h>

#అన్తర్భావ "DaaF3442.h"

తరగతి DaaF3442Test : బహిరంగ ::testing::Test
{
రక్షిత:
	DaaF3442 daa;

	శూన్య సమితి_పైకి() override
	{
		param_control_autosave(అబద్ధం);
		param_reset_all();
	}

	శూన్య సమితి_f3442_params(దశాంశం nmac_radius, దశాంశం nmac_ఎత్తు, దశాంశం wc_radius, దశాంశం wc_ఎత్తు,
			      పూర్ణాంకం nmac_latency_s, పూర్ణాంకం wc_latency_s)
	{
		param_సమితి(param_handle(px4::params::DAA_LVL_CRIT_RAD), &nmac_radius);
		param_సమితి(param_handle(px4::params::DAA_LVL_CRIT_HGT), &nmac_ఎత్తు);
		param_సమితి(param_handle(px4::params::DAA_LVL_ఎగువ_RAD), &wc_radius);
		param_సమితి(param_handle(px4::params::DAA_LVL_ఎగువ_HGT), &wc_ఎత్తు);
		param_సమితి(param_handle(px4::params::DAA_LVL_MED_సమయం), &nmac_latency_s);
		param_సమితి(param_handle(px4::params::DAA_LVL_దిగువ_సమయం), &wc_latency_s);
	}
};

TEST_F(DaaF3442Test, RejectsInvalidParams)
{
	నిర్మాణం ParamCase {
		దశాంశం nmac_radius;
		దశాంశం nmac_ఎత్తు;
		దశాంశం wc_radius;
		దశాంశం wc_ఎత్తు;
		పూర్ణాంకం nmac_latency_s;
		పూర్ణాంకం wc_latency_s;
		తార్కిక expected_ఫలితం;
	};

	constexpr దశాంశం valid_nmac_radius_m = 10.f;
	constexpr దశాంశం valid_nmac_ఎత్తు_m = 10.f;
	constexpr దశాంశం valid_wc_radius_m = 20.f;
	constexpr దశాంశం valid_wc_ఎత్తు_m = 20.f;
	constexpr పూర్ణాంకం valid_nmac_latency_s = 5;
	constexpr పూర్ణాంకం valid_wc_latency_s = 5;
	స్థిరరాశి దశాంశం nan = std::numeric_limits<దశాంశం>::quiet_NaN();
	స్థిరరాశి దశాంశం inf = std::numeric_limits<దశాంశం>::infinity();

	స్థిరరాశి std::array<ParamCase, 13> cases{{
			{valid_nmac_radius_m, valid_nmac_ఎత్తు_m, valid_wc_radius_m, valid_wc_ఎత్తు_m, valid_nmac_latency_s, valid_wc_latency_s, నిజం},
			{0.f, valid_nmac_ఎత్తు_m, valid_wc_radius_m, valid_wc_ఎత్తు_m, valid_nmac_latency_s, valid_wc_latency_s, అబద్ధం},
			{-valid_nmac_radius_m, valid_nmac_ఎత్తు_m, valid_wc_radius_m, valid_wc_ఎత్తు_m, valid_nmac_latency_s, valid_wc_latency_s, అబద్ధం},
			{valid_nmac_radius_m, -valid_nmac_ఎత్తు_m, valid_wc_radius_m, valid_wc_ఎత్తు_m, valid_nmac_latency_s, valid_wc_latency_s, అబద్ధం},
			{valid_nmac_radius_m, valid_nmac_ఎత్తు_m, -valid_wc_radius_m, valid_wc_ఎత్తు_m, valid_nmac_latency_s, valid_wc_latency_s, అబద్ధం},
			{valid_nmac_radius_m, valid_nmac_ఎత్తు_m, valid_wc_radius_m, -valid_wc_ఎత్తు_m, valid_nmac_latency_s, valid_wc_latency_s, అబద్ధం},
			{nan, valid_nmac_ఎత్తు_m, valid_wc_radius_m, valid_wc_ఎత్తు_m, valid_nmac_latency_s, valid_wc_latency_s, అబద్ధం},
			{valid_nmac_radius_m, inf, valid_wc_radius_m, valid_wc_ఎత్తు_m, valid_nmac_latency_s, valid_wc_latency_s, అబద్ధం},
			{valid_nmac_radius_m, valid_nmac_ఎత్తు_m, nan, valid_wc_ఎత్తు_m, valid_nmac_latency_s, valid_wc_latency_s, అబద్ధం},
			{valid_nmac_radius_m, valid_nmac_ఎత్తు_m, valid_wc_radius_m, inf, valid_nmac_latency_s, valid_wc_latency_s, అబద్ధం},
			{valid_nmac_radius_m, valid_nmac_ఎత్తు_m, valid_wc_radius_m, valid_wc_ఎత్తు_m, -valid_nmac_latency_s, valid_wc_latency_s, అబద్ధం},
			{valid_nmac_radius_m, valid_nmac_ఎత్తు_m, valid_wc_radius_m, valid_wc_ఎత్తు_m, valid_nmac_latency_s, -valid_wc_latency_s, అబద్ధం},
			{valid_nmac_radius_m, valid_nmac_ఎత్తు_m, valid_wc_radius_m, valid_wc_ఎత్తు_m, valid_nmac_latency_s + 1, valid_wc_latency_s, అబద్ధం},
		}};

	కొరకు (size_t i = 0; i < cases.కొలత(); ++i) {
		స్థిరరాశి ParamCase &test_case = cases[i];
		SCOPED_TRACE(i);

		సమితి_f3442_params(test_case.nmac_radius, test_case.nmac_ఎత్తు, test_case.wc_radius, test_case.wc_ఎత్తు,
				 test_case.nmac_latency_s, test_case.wc_latency_s);
		EXPECT_EQ(daa.try_setting_params(), test_case.expected_ఫలితం);
	}
}

// Bounds must nest: an outer zone no larger than the inner one is rejected.
TEST_F(DaaF3442Test, Rejects_Non_క్రమానుగత_Bounds)
{
	నిర్మాణం HierarchyCase {
		దశాంశం nmac_radius;
		దశాంశం nmac_ఎత్తు;
		దశాంశం wc_radius;
		దశాంశం wc_ఎత్తు;
	};

	constexpr పూర్ణాంకం nmac_latency_s = 30;
	constexpr పూర్ణాంకం wc_latency_s = 30;
	స్థిరరాశి std::array<HierarchyCase, 3> cases{{
			{20.f, 10.f, 19.f, 10.f},
			{20.f, 10.f, 20.f, 9.f},
			{200.f, 100.f, 100.f, 50.f},
		}};

	కొరకు (size_t i = 0; i < cases.కొలత(); ++i) {
		స్థిరరాశి HierarchyCase &test_case = cases[i];
		SCOPED_TRACE(i);

		సమితి_f3442_params(test_case.nmac_radius, test_case.nmac_ఎత్తు, test_case.wc_radius, test_case.wc_ఎత్తు,
				 nmac_latency_s, wc_latency_s);

		EXPECT_FALSE(daa.try_setting_params());
	}
}

// is_in_bounds() is inclusive and fails on negative or non-finite inputs.
TEST_F(DaaF3442Test, Is_లోపల_Bounds_Rejects_Invalid_Inputs)
{
	స్థిరరాశి మాతృక::Vector2f bounds(10.f, 10.f);
	స్థిరరాశి మాతృక::Vector2f within_bounds(5.f, 5.f);
	స్థిరరాశి మాతృక::Vector2f on_boundary(10.f, 10.f);
	స్థిరరాశి మాతృక::Vector2f outside_both_bounds(15.f, 15.f);
	స్థిరరాశి మాతృక::Vector2f outside_horizontal_bounds(15.f, 4.f);
	స్థిరరాశి మాతృక::Vector2f outside_vertical_bounds(4.f, 15.f);
	స్థిరరాశి మాతృక::Vector2f negative_bounds(-10.f, 10.f);
	స్థిరరాశి మాతృక::Vector2f negative_దూరం(-5.f, 5.f);
	స్థిరరాశి మాతృక::Vector2f non_finite_దూరం(5.f, std::numeric_limits<దశాంశం>::quiet_NaN());

	EXPECT_TRUE(daa.is_లోపల_bounds(within_bounds, bounds));
	EXPECT_TRUE(daa.is_లోపల_bounds(on_boundary, bounds));
	EXPECT_FALSE(daa.is_లోపల_bounds(outside_both_bounds, bounds));
	EXPECT_FALSE(daa.is_లోపల_bounds(outside_horizontal_bounds, bounds));
	EXPECT_FALSE(daa.is_లోపల_bounds(outside_vertical_bounds, bounds));
	EXPECT_FALSE(daa.is_లోపల_bounds(within_bounds, negative_bounds));
	EXPECT_FALSE(daa.is_లోపల_bounds(negative_దూరం, bounds));
	EXPECT_FALSE(daa.is_లోపల_bounds(non_finite_దూరం, bounds));
}

// Volume augmentation uses |velocity| and |latency|, so signs never shrink it.
TEST_F(DaaF3442Test, Calculate_Aircraft_Conflict_Volume_ఉపయోగిస్తుంది_Absolute_Inputs)
{
	స్థిరరాశి మాతృక::Vector2f పునాది_bounds(10.f, 20.f);
	స్థిరరాశి మాతృక::Vector2f velocity(-2.f, 4.f);
	స్థిరరాశి దశాంశం latency_s = -5.f;
	స్థిరరాశి దశాంశం expected_horizontal_bounds_m = పునాది_bounds(0) + fabsf(velocity(0)) * fabsf(latency_s);
	స్థిరరాశి దశాంశం expected_vertical_bounds_m = పునాది_bounds(1) + fabsf(velocity(1)) * fabsf(latency_s);

	మాతృక::Vector2f augmented_bounds{};
	daa.calculate_aircraft_conflict_volume(పునాది_bounds, velocity, latency_s, augmented_bounds);

	EXPECT_FLOAT_EQ(augmented_bounds(0), expected_horizontal_bounds_m);
	EXPECT_FLOAT_EQ(augmented_bounds(1), expected_vertical_bounds_m);
}

// Augmented boundary sums both aircraft's volumes (motion from each side).
TEST_F(DaaF3442Test, CalculateAugmentedBoundariesAddsBothAircraftVolumes)
{
	స్థిరరాశి మాతృక::Vector2f పునాది_bounds(10.f, 20.f);
	స్థిరరాశి మాతృక::Vector2f uav_velocity(2.f, 4.f);
	స్థిరరాశి మాతృక::Vector2f traffic_velocity(8.f, 10.f);
	స్థిరరాశి దశాంశం latency_s = 3.f;
	స్థిరరాశి దశాంశం expected_horizontal_bounds_m = 2.f * పునాది_bounds(0)
			+ (fabsf(uav_velocity(0)) + fabsf(traffic_velocity(0))) * latency_s;
	స్థిరరాశి దశాంశం expected_vertical_bounds_m = 2.f * పునాది_bounds(1)
			+ (fabsf(uav_velocity(1)) + fabsf(traffic_velocity(1))) * latency_s;

	మాతృక::Vector2f augmented_bounds{};
	daa.calculate_augmented_boundaries(పునాది_bounds, uav_velocity, traffic_velocity, latency_s, augmented_bounds);

	EXPECT_FLOAT_EQ(augmented_bounds(0), expected_horizontal_bounds_m);
	EXPECT_FLOAT_EQ(augmented_bounds(1), expected_vertical_bounds_m);
}

// calculate_conflict_level() returns the strictest breached zone; a single-axis miss is NONE.
TEST_F(DaaF3442Test, Calculate_Conflict_Level_Respects_ప్రాధాన్యత_క్రమం)
{
	constexpr దశాంశం nmac_radius_m = 20.f;
	constexpr దశాంశం nmac_ఎత్తు_m = 10.f;
	constexpr దశాంశం wc_radius_m = 100.f;
	constexpr దశాంశం wc_ఎత్తు_m = 50.f;
	constexpr పూర్ణాంకం nmac_latency_s = 30;
	constexpr పూర్ణాంకం wc_latency_s = 40;
	సమితి_f3442_params(nmac_radius_m, nmac_ఎత్తు_m, wc_radius_m, wc_ఎత్తు_m, nmac_latency_s, wc_latency_s);
	ASSERT_TRUE(daa.try_setting_params());

	స్థిరరాశి మాతృక::Vector2f uav_velocity(10.f, 2.f);
	స్థిరరాశి మాతృక::Vector2f traffic_velocity(5.f, 4.f);
	స్థిరరాశి మాతృక::Vector2f nmac_bounds(2.f * nmac_radius_m, 2.f * nmac_ఎత్తు_m);
	స్థిరరాశి మాతృక::Vector2f wc_bounds(2.f * wc_radius_m, 2.f * wc_ఎత్తు_m);
	స్థిరరాశి మాతృక::Vector2f aug_nmac_bounds(
		2.f * nmac_radius_m + (fabsf(uav_velocity(0)) + fabsf(traffic_velocity(0))) * nmac_latency_s,
		2.f * nmac_ఎత్తు_m + (fabsf(uav_velocity(1)) + fabsf(traffic_velocity(1))) * nmac_latency_s);
	స్థిరరాశి మాతృక::Vector2f aug_wc_bounds(
		2.f * wc_radius_m + (fabsf(uav_velocity(0)) + fabsf(traffic_velocity(0))) * wc_latency_s,
		2.f * wc_ఎత్తు_m + (fabsf(uav_velocity(1)) + fabsf(traffic_velocity(1))) * wc_latency_s);
	constexpr దశాంశం inside_margin_m = 1.f;
	స్థిరరాశి మాతృక::Vector2f critical_దూరం(nmac_bounds(0) - inside_margin_m, nmac_bounds(1) - inside_margin_m);
	స్థిరరాశి మాతృక::Vector2f ఎగువ_దూరం(wc_bounds(0) - inside_margin_m, wc_bounds(1) - inside_margin_m);
	స్థిరరాశి మాతృక::Vector2f medium_దూరం(aug_nmac_bounds(0) - inside_margin_m, aug_nmac_bounds(1) - inside_margin_m);
	స్థిరరాశి మాతృక::Vector2f దిగువ_దూరం(aug_wc_bounds(0) - inside_margin_m, aug_wc_bounds(1) - inside_margin_m);
	స్థిరరాశి మాతృక::Vector2f outside_దిగువ_దూరం(aug_wc_bounds(0) + inside_margin_m, aug_wc_bounds(1) + inside_margin_m);
	స్థిరరాశి మాతృక::Vector2f outside_దిగువ_vertical_only(దిగువ_దూరం(0), aug_wc_bounds(1) + inside_margin_m);
	స్థిరరాశి మాతృక::Vector2f outside_దిగువ_horizontal_only(aug_wc_bounds(0) + inside_margin_m, దిగువ_దూరం(1));

	// same velocity pair against progressively weaker breaches
	EXPECT_EQ(daa.calculate_conflict_level(critical_దూరం, uav_velocity, traffic_velocity),
		  detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL);
	EXPECT_EQ(daa.calculate_conflict_level(ఎగువ_దూరం, uav_velocity, traffic_velocity),
		  detect_and_avoid_s::DAA_CONFLICT_LVL_ఎగువ);
	EXPECT_EQ(daa.calculate_conflict_level(medium_దూరం, uav_velocity, traffic_velocity),
		  detect_and_avoid_s::DAA_CONFLICT_LVL_MEDIUM);
	EXPECT_EQ(daa.calculate_conflict_level(దిగువ_దూరం, uav_velocity, traffic_velocity),
		  detect_and_avoid_s::DAA_CONFLICT_LVL_దిగువ);
	EXPECT_EQ(daa.calculate_conflict_level(outside_దిగువ_దూరం, uav_velocity, traffic_velocity),
		  detect_and_avoid_s::DAA_CONFLICT_LVL_NONE);
	EXPECT_EQ(daa.calculate_conflict_level(outside_దిగువ_vertical_only, uav_velocity, traffic_velocity),
		  detect_and_avoid_s::DAA_CONFLICT_LVL_NONE);
	EXPECT_EQ(daa.calculate_conflict_level(outside_దిగువ_horizontal_only, uav_velocity, traffic_velocity),
		  detect_and_avoid_s::DAA_CONFLICT_LVL_NONE);
}

// calculate_daa_stats() reports measured geometry and severity for a near-collision and a clear pass.
TEST_F(DaaF3442Test, CalculateDaaStatsReportsExpectedGeometryAndLevel)
{
	constexpr దశాంశం nmac_radius_m = 20.f;
	constexpr దశాంశం nmac_ఎత్తు_m = 10.f;
	constexpr దశాంశం wc_radius_m = 100.f;
	constexpr దశాంశం wc_ఎత్తు_m = 50.f;
	constexpr పూర్ణాంకం nmac_latency_s = 30;
	constexpr పూర్ణాంకం wc_latency_s = 40;
	సమితి_f3442_params(nmac_radius_m, nmac_ఎత్తు_m, wc_radius_m, wc_ఎత్తు_m, nmac_latency_s, wc_latency_s);
	ASSERT_TRUE(daa.try_setting_params());

	స్థిరరాశి మాతృక::Vector2d uav_lat_lon(46.52342, 6.524234);
	constexpr దశాంశం uav_altitude_m = 400.f;
	స్థిరరాశి మాతృక::Vector3f uav_velocity_ned_m_s(10.f, 0.f, -2.f);
	constexpr దశాంశం uav_heading_rad = 0.f;
	స్థిరరాశి aircraft_state_s uav_state{
		uav_lat_lon,
		uav_altitude_m,
		uav_velocity_ned_m_s,
		uav_heading_rad
	};

	aircraft_state_s traffic_state{};
	స్థిరరాశి మాతృక::Vector3f traffic_velocity_ned_m_s(-5.f, 0.f, -4.f);
	స్థిరరాశి దశాంశం traffic_heading_rad = math::radians(180.f);
	స్థిరరాశి దశాంశం east_bearing_rad = math::radians(90.f);
	constexpr దశాంశం expectation_tolerance = 0.1f;
	ద్విగుణ traffic_lat{0.0};
	ద్విగుణ traffic_lon{0.0};
	traffic_state.velocity_ned = traffic_velocity_ned_m_s;
	traffic_state.heading = traffic_heading_rad;

	స్వయంచాలక ఉంచు_traffic_due_east = [&](దశాంశం horizontal_దూరం_m, దశాంశం traffic_altitude_m) {
		waypoint_from_heading_and_దూరం(uav_state.lat_lon(0), uav_state.lat_lon(1), east_bearing_rad, horizontal_దూరం_m,
						   &traffic_lat, &traffic_lon);
		traffic_state.lat_lon = మాతృక::Vector2d(traffic_lat, traffic_lon);
		traffic_state.altitude = traffic_altitude_m;
	};

	స్థిరరాశి దశాంశం uav_వేగం_m_s = uav_velocity_ned_m_s.norm();
	స్థిరరాశి దశాంశం traffic_వేగం_m_s = traffic_velocity_ned_m_s.norm();
	స్థిరరాశి దశాంశం expected_relative_వేగం_m_s = uav_వేగం_m_s + traffic_వేగం_m_s;
	constexpr దశాంశం near_horizontal_దూరం_m = 39.f;
	constexpr దశాంశం near_vertical_separation_m = 9.f;
	స్థిరరాశి దశాంశం near_traffic_altitude_m = uav_altitude_m + near_vertical_separation_m;
	స్థిరరాశి దశాంశం near_aircraft_దూరం_m = hypotf(near_horizontal_దూరం_m, near_vertical_separation_m);
	ఉంచు_traffic_due_east(near_horizontal_దూరం_m, near_traffic_altitude_m);

	// near-conflict geometry
	daa_stats_s daa_stats{};
	EXPECT_EQ(daa.calculate_daa_stats(uav_state, traffic_state, daa_stats), detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL);
	EXPECT_NEAR(daa_stats.aircraft_dist, near_aircraft_దూరం_m, expectation_tolerance);
	EXPECT_NEAR(daa_stats.aircraft_dist_hor, near_horizontal_దూరం_m, expectation_tolerance);
	EXPECT_NEAR(daa_stats.aircraft_dist_vert, near_vertical_separation_m, expectation_tolerance);
	EXPECT_NEAR(daa_stats.expected_min_dist_సమయం_sec, near_aircraft_దూరం_m / expected_relative_వేగం_m_s,
		    expectation_tolerance);

	// traffic well outside every protection volume
	constexpr దశాంశం far_horizontal_దూరం_m = 900.f;
	constexpr దశాంశం far_vertical_separation_m = 400.f;
	స్థిరరాశి దశాంశం far_traffic_altitude_m = uav_altitude_m + far_vertical_separation_m;
	స్థిరరాశి దశాంశం far_aircraft_దూరం_m = hypotf(far_horizontal_దూరం_m, far_vertical_separation_m);
	ఉంచు_traffic_due_east(far_horizontal_దూరం_m, far_traffic_altitude_m);

	EXPECT_EQ(daa.calculate_daa_stats(uav_state, traffic_state, daa_stats), detect_and_avoid_s::DAA_CONFLICT_LVL_NONE);
	EXPECT_NEAR(daa_stats.aircraft_dist, far_aircraft_దూరం_m, expectation_tolerance);
	EXPECT_NEAR(daa_stats.aircraft_dist_hor, far_horizontal_దూరం_m, expectation_tolerance);
	EXPECT_NEAR(daa_stats.aircraft_dist_vert, far_vertical_separation_m, expectation_tolerance);
	EXPECT_NEAR(daa_stats.expected_min_dist_సమయం_sec, far_aircraft_దూరం_m / expected_relative_వేగం_m_s,
		    expectation_tolerance);
}
