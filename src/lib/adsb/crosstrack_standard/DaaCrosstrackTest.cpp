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

#अन्तर्भाव <gtest/gtest.h>

#अन्तर्भाव "DaaCrosstrackTestData.h"
#अन्तर्भाव "DaaCrosstrack.h"

#अन्तर्भाव <parameters/param.h>

#अन्तर्भाव <lib/geo/geo.h>
#अन्तर्भाव <lib/mathlib/mathlib.h>
#अन्तर्भाव <limits>
#अन्तर्भाव <uORB/topics/detect_and_avoid.h>

नामाकाश
{
ध्रुव matrix::Vector2d kUavLatLon{32.617013, -96.490564};
constexpr भिन्न kUavAlt{1000.f};

शून्य set_crosstrack_params(भिन्न horizontal_separation_m, भिन्न vertical_separation_m, पूर्णाङ्क collision_time_s)
{
	param_set(param_handle(px4::params::NAV_TRAFF_A_HOR), &horizontal_separation_m);
	param_set(param_handle(px4::params::NAV_TRAFF_A_VER), &vertical_separation_m);
	param_set(param_handle(px4::params::NAV_TRAFF_COLL_T), &collision_time_s);
}

matrix::Vector3f velocity_ned_from_track(भिन्न heading_rad, भिन्न horizontal_speed_m_s, भिन्न vertical_speed_up_m_s)
{
	प्रत्यागम matrix::Vector3f(cosf(heading_rad) * horizontal_speed_m_s, sinf(heading_rad) * horizontal_speed_m_s, -vertical_speed_up_m_s);
}

aircraft_state_s create_aircraft_state(ध्रुव matrix::Vector2d &reference_lat_lon, भिन्न reference_altitude,
				       भिन्न distance_m, भिन्न bearing_rad, भिन्न altitude_offset_m, भिन्न heading_rad,
				       ध्रुव matrix::Vector3f &velocity_ned)
{
	द्विगुण lat{0.0};
	द्विगुण lon{0.0};
	waypoint_from_heading_and_distance(reference_lat_lon(0), reference_lat_lon(1), bearing_rad, distance_m, &lat, &lon);

	प्रत्यागम aircraft_state_s{
		matrix::Vector2d(lat, lon),
		reference_altitude + altitude_offset_m,
		velocity_ned,
		heading_rad
	};
}

aircraft_state_s create_aircraft_state(ध्रुव traffic_data_s &traffic)
{
	प्रत्यागम aircraft_state_s{
		matrix::Vector2d(traffic.lat_traffic, traffic.lon_traffic),
		traffic.alt_traffic,
		velocity_ned_from_track(traffic.heading_traffic, traffic.vxy_traffic, traffic.vz_traffic),
		traffic.heading_traffic
	};
}
} // namespace

वर्ग DaaCrosstrackTest : सार्वजनिक ::testing::Test
{
रक्षित:
	DaaCrosstrack daa;

	शून्य SetUp() override
	{
		param_control_autosave(मिच्छ);
		param_reset_all();
		set_crosstrack_params(500.f, 500.f, 60);
	}
};

TEST_F(DaaCrosstrackTest, RejectsInvalidParams)
{
	ASSERT_TRUE(daa.try_setting_params());

	ध्रुव भिन्न nan = std::numeric_limits<भिन्न>::quiet_NaN();
	ध्रुव भिन्न inf = std::numeric_limits<भिन्न>::infinity();

	प्रति (ध्रुव भिन्न bad : {nan, inf, 0.f}) {
		set_crosstrack_params(bad, 500.f, 60);
		EXPECT_FALSE(daa.try_setting_params()) << "horizontal separation = " << bad;
	}

	प्रति (ध्रुव भिन्न bad : {nan, inf, 0.f}) {
		set_crosstrack_params(500.f, bad, 60);
		EXPECT_FALSE(daa.try_setting_params()) << "vertical separation = " << bad;
	}

	प्रति (ध्रुव पूर्णाङ्क bad : {0, -1}) {
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

	ध्रुव aircraft_state_s uav_state{
		kUavLatLon,
		kUavAlt,
		matrix::Vector3f(0.f, 0.f, 0.f),
		0.f
	};
	aircraft_state_s traffic_state = create_aircraft_state(kUavLatLon, kUavAlt, 200.f, math::radians(90.f), 0.f,
					 std::numeric_limits<भिन्न>::quiet_NaN(), matrix::Vector3f(0.f, -30.f, 0.f));

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

	ध्रुव aircraft_state_s uav_state{
		kUavLatLon,
		kUavAlt,
		matrix::Vector3f(0.f, 0.f, 0.f),
		0.f
	};
	ध्रुव aircraft_state_s traffic_state = create_aircraft_state(kUavLatLon, kUavAlt, 200.f, math::radians(90.f), 0.f,
					       math::radians(270.f), matrix::Vector3f(0.f, -30.f, 0.f));

	daa_stats_s daa_stats{};
	EXPECT_EQ(daa.calculate_daa_stats(uav_state, traffic_state, daa_stats), detect_and_avoid_s::DAA_CONFLICT_LVL_उच्चः);

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

	ध्रुव aircraft_state_s uav_state{
		kUavLatLon,
		kUavAlt,
		matrix::Vector3f(0.f, 0.f, 0.f),
		0.f
	};

	प्रति (uint32_t i = 0; i < परिमाण(kTrafficDataset) / परिमाण(kTrafficDataset[0]); ++i) {
		ध्रुव traffic_data_s &traffic = kTrafficDataset[i];
		SCOPED_TRACE(testing::Message() << "kTrafficDataset[" << i << "]");

		daa_stats_s daa_stats{};
		ध्रुव uint8_t conflict_level = daa.calculate_daa_stats(uav_state, create_aircraft_state(traffic), daa_stats);
		ध्रुव uint8_t expected_conflict_level = traffic.in_conflict ? detect_and_avoid_s::DAA_CONFLICT_LVL_उच्चः :
							detect_and_avoid_s::DAA_CONFLICT_LVL_NONE;

		EXPECT_EQ(conflict_level, expected_conflict_level);
	}
}
