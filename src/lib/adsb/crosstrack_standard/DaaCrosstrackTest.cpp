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

#ضمن <gtest/gtest.h>

#ضمن "DaaCrosstrackTestData.h"
#ضمن "DaaCrosstrack.h"

#ضمن <parameters/param.h>

#ضمن <lib/geo/geo.h>
#ضمن <lib/mathlib/mathlib.h>
#ضمن <limits>
#ضمن <uORB/topics/detect_and_avoid.h>

نطاق
{
ثابت matrix::Vector2d kUavLatLon{32.617013, -96.490564};
constexpr عائم kUavAlt{1000.f};

فراغ set_crosstrack_params(عائم horizontal_separation_m, عائم vertical_separation_m, صحيح collision_time_s)
{
	param_set(param_handle(px4::params::NAV_TRAFF_A_HOR), &horizontal_separation_m);
	param_set(param_handle(px4::params::NAV_TRAFF_A_VER), &vertical_separation_m);
	param_set(param_handle(px4::params::NAV_TRAFF_COLL_T), &collision_time_s);
}

matrix::Vector3f velocity_ned_from_track(عائم heading_rad, عائم horizontal_speed_m_s, عائم vertical_speed_up_m_s)
{
	ܦܢܐ matrix::Vector3f(cosf(heading_rad) * horizontal_speed_m_s, sinf(heading_rad) * horizontal_speed_m_s, -vertical_speed_up_m_s);
}

aircraft_state_s create_aircraft_state(ثابت matrix::Vector2d &reference_lat_lon, عائم reference_altitude,
				       عائم distance_m, عائم bearing_rad, عائم altitude_offset_m, عائم heading_rad,
				       ثابت matrix::Vector3f &velocity_ned)
{
	مزدوج lat{0.0};
	مزدوج lon{0.0};
	waypoint_from_heading_and_distance(reference_lat_lon(0), reference_lat_lon(1), bearing_rad, distance_m, &lat, &lon);

	ܦܢܐ aircraft_state_s{
		matrix::Vector2d(lat, lon),
		reference_altitude + altitude_offset_m,
		velocity_ned,
		heading_rad
	};
}

aircraft_state_s create_aircraft_state(ثابت traffic_data_s &traffic)
{
	ܦܢܐ aircraft_state_s{
		matrix::Vector2d(traffic.lat_traffic, traffic.lon_traffic),
		traffic.alt_traffic,
		velocity_ned_from_track(traffic.heading_traffic, traffic.vxy_traffic, traffic.vz_traffic),
		traffic.heading_traffic
	};
}
} // namespace

صنف DaaCrosstrackTest : عام ::testing::Test
{
محمي:
	DaaCrosstrack daa;

	فراغ SetUp() override
	{
		param_control_autosave(ܕܓܠܐ);
		param_reset_all();
		set_crosstrack_params(500.f, 500.f, 60);
	}
};

TEST_F(DaaCrosstrackTest, RejectsInvalidParams)
{
	ASSERT_TRUE(daa.try_setting_params());

	ثابت عائم nan = std::numeric_limits<عائم>::quiet_NaN();
	ثابت عائم inf = std::numeric_limits<عائم>::infinity();

	لكل (ثابت عائم bad : {nan, inf, 0.f}) {
		set_crosstrack_params(bad, 500.f, 60);
		EXPECT_FALSE(daa.try_setting_params()) << "horizontal separation = " << bad;
	}

	لكل (ثابت عائم bad : {nan, inf, 0.f}) {
		set_crosstrack_params(500.f, bad, 60);
		EXPECT_FALSE(daa.try_setting_params()) << "vertical separation = " << bad;
	}

	لكل (ثابت صحيح bad : {0, -1}) {
		set_crosstrack_params(500.f, 500.f, bad);
		EXPECT_FALSE(daa.try_setting_params()) << "collision time = " << bad;
	}

	// valid again
	set_crosstrack_params(500.f, 500.f, 60);
	EXPECT_TRUE(daa.try_setting_params());
}

// A NaN traffic heading (no predicted path line) returns no conflict with cleared stats.
TEST_F(DaaCrosstrackTest, InvalidTrafficHeadingReturnsNoConflict)
{
	ASSERT_TRUE(daa.try_setting_params());

	ثابت aircraft_state_s uav_state{
		kUavLatLon,
		kUavAlt,
		matrix::Vector3f(0.f, 0.f, 0.f),
		0.f
	};
	aircraft_state_s traffic_state = create_aircraft_state(kUavLatLon, kUavAlt, 200.f, math::radians(90.f), 0.f,
					 std::numeric_limits<عائم>::quiet_NaN(), matrix::Vector3f(0.f, -30.f, 0.f));

	daa_stats_s daa_stats{};
	EXPECT_EQ(daa.calculate_daa_stats(uav_state, traffic_state, daa_stats), detect_and_avoid_s::DAA_CONFLICT_LVL_NONE);

	EXPECT_FLOAT_EQ(daa_stats.aircraft_dist, 0.f);
	EXPECT_FLOAT_EQ(daa_stats.aircraft_dist_hor, 0.f);
	EXPECT_FLOAT_EQ(daa_stats.aircraft_dist_vert, 0.f);
	EXPECT_FLOAT_EQ(daa_stats.expected_min_dist_time_sec, 0.f);
}

// A head-on approaching encounter is flagged HIGH with bounded crosstrack, vertical and timing stats.
TEST_F(DaaCrosstrackTest, DetectsApproachingConflictAndReportsExpectedStats)
{
	ASSERT_TRUE(daa.try_setting_params());

	ثابت aircraft_state_s uav_state{
		kUavLatLon,
		kUavAlt,
		matrix::Vector3f(0.f, 0.f, 0.f),
		0.f
	};
	ثابت aircraft_state_s traffic_state = create_aircraft_state(kUavLatLon, kUavAlt, 200.f, math::radians(90.f), 0.f,
					       math::radians(270.f), matrix::Vector3f(0.f, -30.f, 0.f));

	daa_stats_s daa_stats{};
	EXPECT_EQ(daa.calculate_daa_stats(uav_state, traffic_state, daa_stats), detect_and_avoid_s::DAA_CONFLICT_LVL_HIGH);

	EXPECT_NEAR(daa_stats.aircraft_dist, 200.f, 0.1f);
	EXPECT_LT(fabsf(daa_stats.aircraft_dist_hor), 500.f);
	EXPECT_NEAR(daa_stats.aircraft_dist_vert, 0.f, 0.1f);
	EXPECT_NEAR(daa_stats.expected_min_dist_time_sec, 200.f / 30.f, 0.1f);
}

// Regression: the current solver reproduces the legacy conflict decisions for every frozen sample
// (dataset from adsb/test_adsb_helper.ipynb).
TEST_F(DaaCrosstrackTest, MatchesLegacyNotebookDataset)
{
	ASSERT_TRUE(daa.try_setting_params());

	ثابت aircraft_state_s uav_state{
		kUavLatLon,
		kUavAlt,
		matrix::Vector3f(0.f, 0.f, 0.f),
		0.f
	};

	لكل (uint32_t i = 0; i < حجم(kTrafficDataset) / حجم(kTrafficDataset[0]); ++i) {
		ثابت traffic_data_s &traffic = kTrafficDataset[i];
		SCOPED_TRACE(testing::Message() << "kTrafficDataset[" << i << "]");

		daa_stats_s daa_stats{};
		ثابت uint8_t conflict_level = daa.calculate_daa_stats(uav_state, create_aircraft_state(traffic), daa_stats);
		ثابت uint8_t expected_conflict_level = traffic.in_conflict ? detect_and_avoid_s::DAA_CONFLICT_LVL_HIGH :
							detect_and_avoid_s::DAA_CONFLICT_LVL_NONE;

		EXPECT_EQ(conflict_level, expected_conflict_level);
	}
}
