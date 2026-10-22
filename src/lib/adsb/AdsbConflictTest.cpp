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
#ಅನ್ತರ್ಭಾವ "AdsbConflict.h"

#ಅನ್ತರ್ಭಾವ <parameters/param.h>

#ಅನ್ತರ್ಭಾವ <lib/geo/geo.h>
#ಅನ್ತರ್ಭಾವ <lib/mathlib/mathlib.h>
#ಅನ್ತರ್ಭಾವ <limits>

ಸಂರಚನಾ RelativeTrafficScenario {
	ಭಿನ್ನ distance_m;
	ಭಿನ್ನ bearing_rad;
	ಭಿನ್ನ altitude_offset_m;
	ಭಿನ್ನ heading_rad;
	ಭಿನ್ನ hor_speed_m_s;
	ಭಿನ್ನ ver_speed_up_m_s;
};

ವರ್ಗ AdsbConflictTest : ಸಾರ್ವಜನಿಕ ::testing::Test
{
ರಕ್ಷಿತ:
	ಧ್ರುವ matrix::Vector2d uav_lat_lon_{32.617013, -96.490564};
	ಧ್ರುವ ಭಿನ್ನ uav_alt_{1000.f};
	ಧ್ರುವ matrix::Vector3f stationary_uav_vel_ned_{0.f, 0.f, 0.f};

	ಶೂನ್ಯ SetUp() override
	{
		param_control_autosave(ಅಸತ್ಯ);
		param_reset_all();
#ಯದಿ defined(CONFIG_NAVIGATOR_ADSB_F3442) && CONFIG_NAVIGATOR_ADSB_F3442
		set_f3442_params(20.f, 10.f, 100.f, 50.f, 30, 40);
#ಅನ್ಯಥಾ
		set_crosstrack_params(500.f, 500.f, 60);
#endif // CONFIG_NAVIGATOR_ADSB_F3442
	}

#ಯದಿ defined(CONFIG_NAVIGATOR_ADSB_F3442) && CONFIG_NAVIGATOR_ADSB_F3442
	ಶೂನ್ಯ set_f3442_params(ಭಿನ್ನ nmac_radius, ಭಿನ್ನ nmac_height, ಭಿನ್ನ wc_radius,
			      ಭಿನ್ನ wc_height, ಪೂರ್ಣಾಙ್ಕ nmac_latency_s, ಪೂರ್ಣಾಙ್ಕ wc_latency_s)
	{
		param_set(param_handle(px4::params::DAA_LVL_CRIT_RAD), &nmac_radius);
		param_set(param_handle(px4::params::DAA_LVL_CRIT_HGT), &nmac_height);
		param_set(param_handle(px4::params::DAA_LVL_ಮೇಲಿನ_RAD), &wc_radius);
		param_set(param_handle(px4::params::DAA_LVL_ಮೇಲಿನ_HGT), &wc_height);
		param_set(param_handle(px4::params::DAA_LVL_MED_TIME), &nmac_latency_s);
		param_set(param_handle(px4::params::DAA_LVL_ಕೆಳಗಿನ_TIME), &wc_latency_s);
	}
#endif // CONFIG_NAVIGATOR_ADSB_F3442

	ಶೂನ್ಯ set_crosstrack_params(ಭಿನ್ನ horizontal_separation_m,
				   ಭಿನ್ನ vertical_separation_m, ಪೂರ್ಣಾಙ್ಕ collision_time_s)
	{
		param_set(param_handle(px4::params::NAV_TRAFF_A_HOR), &horizontal_separation_m);
		param_set(param_handle(px4::params::NAV_TRAFF_A_VER), &vertical_separation_m);
		param_set(param_handle(px4::params::NAV_TRAFF_COLL_T), &collision_time_s);
	}

	// Build a traffic report from a relative encounter against the fixed ownship state.
	transponder_report_s create_relative_report(ಧ್ರುವ RelativeTrafficScenario &scenario)
	{
		transponder_report_s report{};
		report.timestamp = hrt_absolute_time();
		waypoint_from_heading_and_distance(uav_lat_lon_(0), uav_lat_lon_(1),
						   scenario.bearing_rad, scenario.distance_m, &report.lat, &report.lon);
		report.altitude = uav_alt_ + scenario.altitude_offset_m;
		report.heading = scenario.heading_rad;
		report.hor_velocity = scenario.hor_speed_m_s;
		report.ver_velocity = scenario.ver_speed_up_m_s;
		ಪ್ರತ್ಯಾಗಮ report;
	}

	daa_ಒಳಹರಿವು_s create_daa_ಒಳಹರಿವು(ಧ್ರುವ matrix::Vector2d &uav_lat_lon, ಧ್ರುವ ಭಿನ್ನ uav_alt,
				     ಧ್ರುವ matrix::Vector3f &uav_vel_ned, ಧ್ರುವ transponder_report_s &report) ಧ್ರುವ
	{
		daa_ಒಳಹರಿವು_s daa_ಒಳಹರಿವು{};
		daa_ಒಳಹರಿವು.uav_lat_lon = uav_lat_lon;
		daa_ಒಳಹರಿವು.uav_alt = uav_alt;
		daa_ಒಳಹರಿವು.uav_vel_ned = uav_vel_ned;
		daa_ಒಳಹರಿವು.transponder_report = report;
		ಪ್ರತ್ಯಾಗಮ daa_ಒಳಹರಿವು;
	}
};

TEST_F(AdsbConflictTest, RejectsInvalidAircraftState)
{
	AdsbConflict adsb_conflict;
	ಧ್ರುವ RelativeTrafficScenario valid_traffic{200.f, math::radians(90.f), 0.f, 0.f, 30.f, 0.f};

	detect_and_avoid_s daa_output{};
	transponder_report_s report = create_relative_report(valid_traffic);
	ಧ್ರುವ ಭಿನ್ನ nan = std::numeric_limits<ಭಿನ್ನ>::quiet_NaN();
	ಧ್ರುವ ದ್ವಿಗುಣ nan_d = std::numeric_limits<ದ್ವಿಗುಣ>::quiet_NaN();
	ಧ್ರುವ ಭಿನ್ನ inf = std::numeric_limits<ಭಿನ್ನ>::infinity();

	// non-finite ownship coordinate
	EXPECT_FALSE(adsb_conflict.calculate_daa_output(create_daa_ಒಳಹರಿವು(matrix::Vector2d(nan_d, uav_lat_lon_(1)), uav_alt_,
			stationary_uav_vel_ned_, report), daa_output));
	EXPECT_FALSE(adsb_conflict.calculate_daa_output(create_daa_ಒಳಹರಿವು(matrix::Vector2d(uav_lat_lon_(0), nan_d), uav_alt_,
			stationary_uav_vel_ned_, report), daa_output));
	EXPECT_FALSE(adsb_conflict.calculate_daa_output(create_daa_ಒಳಹರಿವು(matrix::Vector2d(90.1, uav_lat_lon_(1)), uav_alt_,
			stationary_uav_vel_ned_, report), daa_output));
	EXPECT_FALSE(adsb_conflict.calculate_daa_output(create_daa_ಒಳಹರಿವು(matrix::Vector2d(uav_lat_lon_(0), -180.1), uav_alt_,
			stationary_uav_vel_ned_, report), daa_output));

	report.lat = nan_d;
	EXPECT_FALSE(adsb_conflict.calculate_daa_output(create_daa_ಒಳಹರಿವು(uav_lat_lon_, uav_alt_,
			stationary_uav_vel_ned_, report), daa_output));
	report = create_relative_report(valid_traffic);
	report.lon = 180.1;
	EXPECT_FALSE(adsb_conflict.calculate_daa_output(create_daa_ಒಳಹರಿವು(uav_lat_lon_, uav_alt_,
			stationary_uav_vel_ned_, report), daa_output));

	report = create_relative_report(valid_traffic);
	EXPECT_FALSE(adsb_conflict.calculate_daa_output(create_daa_ಒಳಹರಿವು(uav_lat_lon_, nan,
			stationary_uav_vel_ned_, report), daa_output));
	report.altitude = inf;
	EXPECT_FALSE(adsb_conflict.calculate_daa_output(create_daa_ಒಳಹರಿವು(uav_lat_lon_, uav_alt_,
			stationary_uav_vel_ned_, report), daa_output));

	report = create_relative_report(valid_traffic);
	report.hor_velocity = inf;
	EXPECT_FALSE(adsb_conflict.calculate_daa_output(create_daa_ಒಳಹರಿವು(uav_lat_lon_, uav_alt_,
			stationary_uav_vel_ned_, report), daa_output));
	report.hor_velocity = -1.f;
	EXPECT_FALSE(adsb_conflict.calculate_daa_output(create_daa_ಒಳಹರಿವು(uav_lat_lon_, uav_alt_,
			stationary_uav_vel_ned_, report), daa_output));

	report = create_relative_report(valid_traffic);
	EXPECT_FALSE(adsb_conflict.calculate_daa_output(create_daa_ಒಳಹರಿವು(uav_lat_lon_, uav_alt_,
			matrix::Vector3f(0.f, nan, 0.f), report), daa_output));
}

#ಯದಿ !defined(CONFIG_NAVIGATOR_ADSB_F3442) || !CONFIG_NAVIGATOR_ADSB_F3442
TEST_F(AdsbConflictTest, CrosstrackRejectsNonFiniteTrafficHeading)
{
	AdsbConflict adsb_conflict;
	ಧ್ರುವ RelativeTrafficScenario approaching_traffic{200.f, math::radians(90.f), 0.f, math::radians(270.f), 30.f, 0.f};

	detect_and_avoid_s daa_output{};
	transponder_report_s report = create_relative_report(approaching_traffic);
	ಧ್ರುವ ಭಿನ್ನ nan = std::numeric_limits<ಭಿನ್ನ>::quiet_NaN();

	report.heading = nan;
	EXPECT_FALSE(adsb_conflict.calculate_daa_output(create_daa_ಒಳಹರಿವು(uav_lat_lon_, uav_alt_,
			stationary_uav_vel_ned_, report), daa_output));
}
#endif // !CONFIG_NAVIGATOR_ADSB_F3442

// try_updating_params() validates against the built standard and rejects bad values.
TEST_F(AdsbConflictTest, UsesBuiltStandardForParamValidation)
{
	AdsbConflict adsb_conflict;
	constexpr ಭಿನ್ನ invalid_bound{-1.f};

	EXPECT_TRUE(adsb_conflict.try_updating_params());

#ಯದಿ defined(CONFIG_NAVIGATOR_ADSB_F3442) && CONFIG_NAVIGATOR_ADSB_F3442
	set_f3442_params(invalid_bound, 10.f, 100.f, 50.f, 30, 40);
#ಅನ್ಯಥಾ
	set_crosstrack_params(invalid_bound, 500.f, 60);
#endif // CONFIG_NAVIGATOR_ADSB_F3442

	EXPECT_FALSE(adsb_conflict.try_updating_params());
}

// The wrapper forwards the built standard's result (level + distances).
TEST_F(AdsbConflictTest, DelegatesToBuiltStandardAndForwardsOutput)
{
	AdsbConflict adsb_conflict;
	ಧ್ರುವ RelativeTrafficScenario approaching_traffic{19.f, math::radians(90.f), 9.f, math::radians(270.f), 5.f, 0.f};
	ಧ್ರುವ transponder_report_s report = create_relative_report(approaching_traffic);

	detect_and_avoid_s daa_output{};

	ASSERT_TRUE(adsb_conflict.try_updating_params());
	ASSERT_TRUE(adsb_conflict.calculate_daa_output(create_daa_ಒಳಹರಿವು(uav_lat_lon_, uav_alt_,
			stationary_uav_vel_ned_, report), daa_output));

#ಯದಿ defined(CONFIG_NAVIGATOR_ADSB_F3442) && CONFIG_NAVIGATOR_ADSB_F3442
	EXPECT_EQ(daa_output.conflict_level, detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL);
	EXPECT_NEAR(daa_output.aircraft_dist_hor, approaching_traffic.distance_m, 0.1f);
	EXPECT_NEAR(daa_output.aircraft_dist_vert, approaching_traffic.altitude_offset_m, 0.1f);
#ಅನ್ಯಥಾ
	EXPECT_EQ(daa_output.conflict_level, detect_and_avoid_s::DAA_CONFLICT_LVL_ಮೇಲಿನ);
	EXPECT_NEAR(daa_output.aircraft_dist_vert, approaching_traffic.altitude_offset_m, 0.1f);
	EXPECT_LT(fabsf(daa_output.aircraft_dist_hor), 500.f);
#endif // CONFIG_NAVIGATOR_ADSB_F3442
}
