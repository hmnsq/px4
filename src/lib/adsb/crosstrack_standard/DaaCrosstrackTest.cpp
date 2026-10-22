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

#అన్తర్భావ <gtest/gtest.h>

#అన్తర్భావ "DaaCrosstrackTestData.h"
#అన్తర్భావ "DaaCrosstrack.h"

#అన్తర్భావ <parameters/param.h>

#అన్తర్భావ <lib/geo/geo.h>
#అన్తర్భావ <lib/mathlib/mathlib.h>
#అన్తర్భావ <limits>
#అన్తర్భావ <uORB/topics/detect_and_avoid.h>

నామాకాశ
{
స్థిరరాశి మాతృక::Vector2d kUavLatLon{32.617013, -96.490564};
constexpr దశాంశం kUavAlt{1000.f};

శూన్య సమితి_crosstrack_params(దశాంశం horizontal_separation_m, దశాంశం vertical_separation_m, పూర్ణాంకం collision_సమయం_s)
{
	param_సమితి(param_handle(px4::params::NAV_TRAFF_A_HOR), &horizontal_separation_m);
	param_సమితి(param_handle(px4::params::NAV_TRAFF_A_VER), &vertical_separation_m);
	param_సమితి(param_handle(px4::params::NAV_TRAFF_COLL_T), &collision_సమయం_s);
}

మాతృక::Vector3f velocity_ned_from_track(దశాంశం heading_rad, దశాంశం horizontal_వేగం_m_s, దశాంశం vertical_వేగం_పైకి_m_s)
{
	తిరిగి మాతృక::Vector3f(cosf(heading_rad) * horizontal_వేగం_m_s, sinf(heading_rad) * horizontal_వేగం_m_s, -vertical_వేగం_పైకి_m_s);
}

aircraft_state_s create_aircraft_state(స్థిరరాశి మాతృక::Vector2d &reference_lat_lon, దశాంశం reference_altitude,
				       దశాంశం దూరం_m, దశాంశం bearing_rad, దశాంశం altitude_స్థానభ్రంశం_m, దశాంశం heading_rad,
				       స్థిరరాశి మాతృక::Vector3f &velocity_ned)
{
	ద్విగుణ lat{0.0};
	ద్విగుణ lon{0.0};
	waypoint_from_heading_and_దూరం(reference_lat_lon(0), reference_lat_lon(1), bearing_rad, దూరం_m, &lat, &lon);

	తిరిగి aircraft_state_s{
		మాతృక::Vector2d(lat, lon),
		reference_altitude + altitude_స్థానభ్రంశం_m,
		velocity_ned,
		heading_rad
	};
}

aircraft_state_s create_aircraft_state(స్థిరరాశి traffic_దత్తాంశం_s &traffic)
{
	తిరిగి aircraft_state_s{
		మాతృక::Vector2d(traffic.lat_traffic, traffic.lon_traffic),
		traffic.alt_traffic,
		velocity_ned_from_track(traffic.heading_traffic, traffic.vxy_traffic, traffic.vz_traffic),
		traffic.heading_traffic
	};
}
} // namespace

తరగతి DaaCrosstrackTest : బహిరంగ ::testing::Test
{
రక్షిత:
	DaaCrosstrack daa;

	శూన్య సమితి_పైకి() override
	{
		param_control_autosave(అబద్ధం);
		param_reset_all();
		సమితి_crosstrack_params(500.f, 500.f, 60);
	}
};

TEST_F(DaaCrosstrackTest, RejectsInvalidParams)
{
	ASSERT_TRUE(daa.try_setting_params());

	స్థిరరాశి దశాంశం nan = std::numeric_limits<దశాంశం>::quiet_NaN();
	స్థిరరాశి దశాంశం inf = std::numeric_limits<దశాంశం>::infinity();

	కొరకు (స్థిరరాశి దశాంశం bad : {nan, inf, 0.f}) {
		సమితి_crosstrack_params(bad, 500.f, 60);
		EXPECT_FALSE(daa.try_setting_params()) << "horizontal separation = " << bad;
	}

	కొరకు (స్థిరరాశి దశాంశం bad : {nan, inf, 0.f}) {
		సమితి_crosstrack_params(500.f, bad, 60);
		EXPECT_FALSE(daa.try_setting_params()) << "vertical separation = " << bad;
	}

	కొరకు (స్థిరరాశి పూర్ణాంకం bad : {0, -1}) {
		సమితి_crosstrack_params(500.f, 500.f, bad);
		EXPECT_FALSE(daa.try_setting_params()) << "collision time = " << bad;
	}

	// valid again
	సమితి_crosstrack_params(500.f, 500.f, 60);
	EXPECT_TRUE(daa.try_setting_params());
}

// A NaN traffic heading (no predicted path line) returns no conflict with cleared stats.
TEST_F(DaaCrosstrackTest, InvalidTrafficHeadingReturnsNoConflict)
{
	ASSERT_TRUE(daa.try_setting_params());

	స్థిరరాశి aircraft_state_s uav_state{
		kUavLatLon,
		kUavAlt,
		మాతృక::Vector3f(0.f, 0.f, 0.f),
		0.f
	};
	aircraft_state_s traffic_state = create_aircraft_state(kUavLatLon, kUavAlt, 200.f, math::radians(90.f), 0.f,
					 std::numeric_limits<దశాంశం>::quiet_NaN(), మాతృక::Vector3f(0.f, -30.f, 0.f));

	daa_stats_s daa_stats{};
	EXPECT_EQ(daa.calculate_daa_stats(uav_state, traffic_state, daa_stats), detect_and_avoid_s::DAA_CONFLICT_LVL_NONE);

	EXPECT_FLOAT_EQ(daa_stats.aircraft_dist, 0.f);
	EXPECT_FLOAT_EQ(daa_stats.aircraft_dist_hor, 0.f);
	EXPECT_FLOAT_EQ(daa_stats.aircraft_dist_vert, 0.f);
	EXPECT_FLOAT_EQ(daa_stats.expected_min_dist_సమయం_sec, 0.f);
}

// A head-on approaching encounter is flagged HIGH with bounded crosstrack, vertical and timing stats.
TEST_F(DaaCrosstrackTest, DetectsApproachingConflictAndReportsExpectedStats)
{
	ASSERT_TRUE(daa.try_setting_params());

	స్థిరరాశి aircraft_state_s uav_state{
		kUavLatLon,
		kUavAlt,
		మాతృక::Vector3f(0.f, 0.f, 0.f),
		0.f
	};
	స్థిరరాశి aircraft_state_s traffic_state = create_aircraft_state(kUavLatLon, kUavAlt, 200.f, math::radians(90.f), 0.f,
					       math::radians(270.f), మాతృక::Vector3f(0.f, -30.f, 0.f));

	daa_stats_s daa_stats{};
	EXPECT_EQ(daa.calculate_daa_stats(uav_state, traffic_state, daa_stats), detect_and_avoid_s::DAA_CONFLICT_LVL_ఎగువ);

	EXPECT_NEAR(daa_stats.aircraft_dist, 200.f, 0.1f);
	EXPECT_LT(fabsf(daa_stats.aircraft_dist_hor), 500.f);
	EXPECT_NEAR(daa_stats.aircraft_dist_vert, 0.f, 0.1f);
	EXPECT_NEAR(daa_stats.expected_min_dist_సమయం_sec, 200.f / 30.f, 0.1f);
}

// Regression: the current solver reproduces the legacy conflict decisions for every frozen sample
// (dataset from adsb/test_adsb_helper.ipynb).
TEST_F(DaaCrosstrackTest, MatchesLegacyNotebookDataset)
{
	ASSERT_TRUE(daa.try_setting_params());

	స్థిరరాశి aircraft_state_s uav_state{
		kUavLatLon,
		kUavAlt,
		మాతృక::Vector3f(0.f, 0.f, 0.f),
		0.f
	};

	కొరకు (uint32_t i = 0; i < పరిమాణం(kTrafficDataset) / పరిమాణం(kTrafficDataset[0]); ++i) {
		స్థిరరాశి traffic_దత్తాంశం_s &traffic = kTrafficDataset[i];
		SCOPED_TRACE(testing::సందేశం() << "kTrafficDataset[" << i << "]");

		daa_stats_s daa_stats{};
		స్థిరరాశి uint8_t conflict_level = daa.calculate_daa_stats(uav_state, create_aircraft_state(traffic), daa_stats);
		స్థిరరాశి uint8_t expected_conflict_level = traffic.లోపల_conflict ? detect_and_avoid_s::DAA_CONFLICT_LVL_ఎగువ :
							detect_and_avoid_s::DAA_CONFLICT_LVL_NONE;

		EXPECT_EQ(conflict_level, expected_conflict_level);
	}
}
