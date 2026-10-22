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

#ಅನ್ತರ್ಭಾವ <gtest/gtest.h>

#ಅನ್ತರ್ಭಾವ "DaaCrosstrackTestData.h"
#ಅನ್ತರ್ಭಾವ "DaaCrosstrack.h"

#ಅನ್ತರ್ಭಾವ <parameters/param.h>

#ಅನ್ತರ್ಭಾವ <lib/geo/geo.h>
#ಅನ್ತರ್ಭಾವ <lib/mathlib/mathlib.h>
#ಅನ್ತರ್ಭಾವ <limits>
#ಅನ್ತರ್ಭಾವ <uORB/topics/detect_and_avoid.h>

ನಾಮಾಕಾಶ
{
ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::Vector2d kUavLatLon{32.617013, -96.490564};
constexpr ದಶಮಾಂಶ kUavAlt{1000.f};

ಶೂನ್ಯ ಗಣ_crosstrack_params(ದಶಮಾಂಶ horizontal_separation_m, ದಶಮಾಂಶ vertical_separation_m, ಪೂರ್ಣಾಂಕ collision_ಸಮಯ_s)
{
	param_ಗಣ(param_handle(px4::params::NAV_TRAFF_A_HOR), &horizontal_separation_m);
	param_ಗಣ(param_handle(px4::params::NAV_TRAFF_A_VER), &vertical_separation_m);
	param_ಗಣ(param_handle(px4::params::NAV_TRAFF_COLL_T), &collision_ಸಮಯ_s);
}

ಮಾತೃಕೆ::Vector3f velocity_ned_from_track(ದಶಮಾಂಶ heading_rad, ದಶಮಾಂಶ horizontal_ವೇಗ_m_s, ದಶಮಾಂಶ vertical_ವೇಗ_ಮೇಲೆ_m_s)
{
	ಹಿಂತಿರುಗಿ ಮಾತೃಕೆ::Vector3f(cosf(heading_rad) * horizontal_ವೇಗ_m_s, sinf(heading_rad) * horizontal_ವೇಗ_m_s, -vertical_ವೇಗ_ಮೇಲೆ_m_s);
}

aircraft_state_s create_aircraft_state(ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::Vector2d &reference_lat_lon, ದಶಮಾಂಶ reference_altitude,
				       ದಶಮಾಂಶ ದೂರ_m, ದಶಮಾಂಶ bearing_rad, ದಶಮಾಂಶ altitude_ಸ್ಥಾನಾಂತರ_m, ದಶಮಾಂಶ heading_rad,
				       ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::Vector3f &velocity_ned)
{
	ದ್ವಿಗುಣ lat{0.0};
	ದ್ವಿಗುಣ lon{0.0};
	waypoint_from_heading_and_ದೂರ(reference_lat_lon(0), reference_lat_lon(1), bearing_rad, ದೂರ_m, &lat, &lon);

	ಹಿಂತಿರುಗಿ aircraft_state_s{
		ಮಾತೃಕೆ::Vector2d(lat, lon),
		reference_altitude + altitude_ಸ್ಥಾನಾಂತರ_m,
		velocity_ned,
		heading_rad
	};
}

aircraft_state_s create_aircraft_state(ಸ್ಥಿರಾಂಕ traffic_ದತ್ತಾಂಶ_s &traffic)
{
	ಹಿಂತಿರುಗಿ aircraft_state_s{
		ಮಾತೃಕೆ::Vector2d(traffic.lat_traffic, traffic.lon_traffic),
		traffic.alt_traffic,
		velocity_ned_from_track(traffic.heading_traffic, traffic.vxy_traffic, traffic.vz_traffic),
		traffic.heading_traffic
	};
}
} // namespace

ವರ್ಗ DaaCrosstrackTest : ಸಾರ್ವಜನಿಕ ::testing::Test
{
ಸಂರಕ್ಷಿತ:
	DaaCrosstrack daa;

	ಶೂನ್ಯ ಗಣ_ಮೇಲೆ() override
	{
		param_control_autosave(ಸುಳ್ಳು);
		param_reset_all();
		ಗಣ_crosstrack_params(500.f, 500.f, 60);
	}
};

TEST_F(DaaCrosstrackTest, RejectsInvalidParams)
{
	ASSERT_TRUE(daa.try_setting_params());

	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ nan = std::numeric_limits<ದಶಮಾಂಶ>::quiet_NaN();
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ inf = std::numeric_limits<ದಶಮಾಂಶ>::infinity();

	ಗಾಗಿ (ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ bad : {nan, inf, 0.f}) {
		ಗಣ_crosstrack_params(bad, 500.f, 60);
		EXPECT_FALSE(daa.try_setting_params()) << "horizontal separation = " << bad;
	}

	ಗಾಗಿ (ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ bad : {nan, inf, 0.f}) {
		ಗಣ_crosstrack_params(500.f, bad, 60);
		EXPECT_FALSE(daa.try_setting_params()) << "vertical separation = " << bad;
	}

	ಗಾಗಿ (ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ bad : {0, -1}) {
		ಗಣ_crosstrack_params(500.f, 500.f, bad);
		EXPECT_FALSE(daa.try_setting_params()) << "collision time = " << bad;
	}

	// valid again
	ಗಣ_crosstrack_params(500.f, 500.f, 60);
	EXPECT_TRUE(daa.try_setting_params());
}

// A NaN traffic heading (no predicted path line) returns no conflict with cleared stats.
TEST_F(DaaCrosstrackTest, InvalidTrafficHeadingReturnsNoConflict)
{
	ASSERT_TRUE(daa.try_setting_params());

	ಸ್ಥಿರಾಂಕ aircraft_state_s uav_state{
		kUavLatLon,
		kUavAlt,
		ಮಾತೃಕೆ::Vector3f(0.f, 0.f, 0.f),
		0.f
	};
	aircraft_state_s traffic_state = create_aircraft_state(kUavLatLon, kUavAlt, 200.f, math::radians(90.f), 0.f,
					 std::numeric_limits<ದಶಮಾಂಶ>::quiet_NaN(), ಮಾತೃಕೆ::Vector3f(0.f, -30.f, 0.f));

	daa_stats_s daa_stats{};
	EXPECT_EQ(daa.calculate_daa_stats(uav_state, traffic_state, daa_stats), detect_and_avoid_s::DAA_CONFLICT_LVL_NONE);

	EXPECT_FLOAT_EQ(daa_stats.aircraft_dist, 0.f);
	EXPECT_FLOAT_EQ(daa_stats.aircraft_dist_hor, 0.f);
	EXPECT_FLOAT_EQ(daa_stats.aircraft_dist_vert, 0.f);
	EXPECT_FLOAT_EQ(daa_stats.expected_min_dist_ಸಮಯ_sec, 0.f);
}

// A head-on approaching encounter is flagged HIGH with bounded crosstrack, vertical and timing stats.
TEST_F(DaaCrosstrackTest, DetectsApproachingConflictAndReportsExpectedStats)
{
	ASSERT_TRUE(daa.try_setting_params());

	ಸ್ಥಿರಾಂಕ aircraft_state_s uav_state{
		kUavLatLon,
		kUavAlt,
		ಮಾತೃಕೆ::Vector3f(0.f, 0.f, 0.f),
		0.f
	};
	ಸ್ಥಿರಾಂಕ aircraft_state_s traffic_state = create_aircraft_state(kUavLatLon, kUavAlt, 200.f, math::radians(90.f), 0.f,
					       math::radians(270.f), ಮಾತೃಕೆ::Vector3f(0.f, -30.f, 0.f));

	daa_stats_s daa_stats{};
	EXPECT_EQ(daa.calculate_daa_stats(uav_state, traffic_state, daa_stats), detect_and_avoid_s::DAA_CONFLICT_LVL_ಮೇಲಿನ);

	EXPECT_NEAR(daa_stats.aircraft_dist, 200.f, 0.1f);
	EXPECT_LT(fabsf(daa_stats.aircraft_dist_hor), 500.f);
	EXPECT_NEAR(daa_stats.aircraft_dist_vert, 0.f, 0.1f);
	EXPECT_NEAR(daa_stats.expected_min_dist_ಸಮಯ_sec, 200.f / 30.f, 0.1f);
}

// Regression: the current solver reproduces the legacy conflict decisions for every frozen sample
// (dataset from adsb/test_adsb_helper.ipynb).
TEST_F(DaaCrosstrackTest, MatchesLegacyNotebookDataset)
{
	ASSERT_TRUE(daa.try_setting_params());

	ಸ್ಥಿರಾಂಕ aircraft_state_s uav_state{
		kUavLatLon,
		kUavAlt,
		ಮಾತೃಕೆ::Vector3f(0.f, 0.f, 0.f),
		0.f
	};

	ಗಾಗಿ (uint32_t i = 0; i < ಗಾತ್ರ(kTrafficDataset) / ಗಾತ್ರ(kTrafficDataset[0]); ++i) {
		ಸ್ಥಿರಾಂಕ traffic_ದತ್ತಾಂಶ_s &traffic = kTrafficDataset[i];
		SCOPED_TRACE(testing::ಸಂದೇಶ() << "kTrafficDataset[" << i << "]");

		daa_stats_s daa_stats{};
		ಸ್ಥಿರಾಂಕ uint8_t conflict_level = daa.calculate_daa_stats(uav_state, create_aircraft_state(traffic), daa_stats);
		ಸ್ಥಿರಾಂಕ uint8_t expected_conflict_level = traffic.ಒಳ_conflict ? detect_and_avoid_s::DAA_CONFLICT_LVL_ಮೇಲಿನ :
							detect_and_avoid_s::DAA_CONFLICT_LVL_NONE;

		EXPECT_EQ(conflict_level, expected_conflict_level);
	}
}
