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

ರಚನೆ RelativeTrafficScenario {
	ದಶಮಾಂಶ ದೂರ_m;
	ದಶಮಾಂಶ bearing_rad;
	ದಶಮಾಂಶ altitude_ಸ್ಥಾನಾಂತರ_m;
	ದಶಮಾಂಶ heading_rad;
	ದಶಮಾಂಶ hor_ವೇಗ_m_s;
	ದಶಮಾಂಶ ver_ವೇಗ_ಮೇಲೆ_m_s;
};

ವರ್ಗ AdsbConflictTest : ಸಾರ್ವಜನಿಕ ::testing::Test
{
ಸಂರಕ್ಷಿತ:
	ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::Vector2d uav_lat_lon_{32.617013, -96.490564};
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ uav_alt_{1000.f};
	ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::Vector3f stationary_uav_vel_ned_{0.f, 0.f, 0.f};

	ಶೂನ್ಯ ಗಣ_ಮೇಲೆ() override
	{
		param_control_autosave(ಸುಳ್ಳು);
		param_reset_all();
#ಆದರೆ defined(CONFIG_NAVIGATOR_ADSB_F3442) && CONFIG_NAVIGATOR_ADSB_F3442
		ಗಣ_f3442_params(20.f, 10.f, 100.f, 50.f, 30, 40);
#ಇಲ್ಲದಿದ್ದರೆ
		ಗಣ_crosstrack_params(500.f, 500.f, 60);
#ಆದರೆ_ಅಂತ್ಯ // CONFIG_NAVIGATOR_ADSB_F3442
	}

#ಆದರೆ defined(CONFIG_NAVIGATOR_ADSB_F3442) && CONFIG_NAVIGATOR_ADSB_F3442
	ಶೂನ್ಯ ಗಣ_f3442_params(ದಶಮಾಂಶ nmac_radius, ದಶಮಾಂಶ nmac_ಎತ್ತರ, ದಶಮಾಂಶ wc_radius,
			      ದಶಮಾಂಶ wc_ಎತ್ತರ, ಪೂರ್ಣಾಂಕ nmac_latency_s, ಪೂರ್ಣಾಂಕ wc_latency_s)
	{
		param_ಗಣ(param_handle(px4::params::DAA_LVL_CRIT_RAD), &nmac_radius);
		param_ಗಣ(param_handle(px4::params::DAA_LVL_CRIT_HGT), &nmac_ಎತ್ತರ);
		param_ಗಣ(param_handle(px4::params::DAA_LVL_ಮೇಲಿನ_RAD), &wc_radius);
		param_ಗಣ(param_handle(px4::params::DAA_LVL_ಮೇಲಿನ_HGT), &wc_ಎತ್ತರ);
		param_ಗಣ(param_handle(px4::params::DAA_LVL_MED_ಸಮಯ), &nmac_latency_s);
		param_ಗಣ(param_handle(px4::params::DAA_LVL_ಕೆಳಗಿನ_ಸಮಯ), &wc_latency_s);
	}
#ಆದರೆ_ಅಂತ್ಯ // CONFIG_NAVIGATOR_ADSB_F3442

	ಶೂನ್ಯ ಗಣ_crosstrack_params(ದಶಮಾಂಶ horizontal_separation_m,
				   ದಶಮಾಂಶ vertical_separation_m, ಪೂರ್ಣಾಂಕ collision_ಸಮಯ_s)
	{
		param_ಗಣ(param_handle(px4::params::NAV_TRAFF_A_HOR), &horizontal_separation_m);
		param_ಗಣ(param_handle(px4::params::NAV_TRAFF_A_VER), &vertical_separation_m);
		param_ಗಣ(param_handle(px4::params::NAV_TRAFF_COLL_T), &collision_ಸಮಯ_s);
	}

	// Build a traffic report from a relative encounter against the fixed ownship state.
	transponder_report_s create_relative_report(ಸ್ಥಿರಾಂಕ RelativeTrafficScenario &scenario)
	{
		transponder_report_s report{};
		report.timestamp = hrt_absolute_ಸಮಯ();
		waypoint_from_heading_and_ದೂರ(uav_lat_lon_(0), uav_lat_lon_(1),
						   scenario.bearing_rad, scenario.ದೂರ_m, &report.lat, &report.lon);
		report.altitude = uav_alt_ + scenario.altitude_ಸ್ಥಾನಾಂತರ_m;
		report.heading = scenario.heading_rad;
		report.hor_velocity = scenario.hor_ವೇಗ_m_s;
		report.ver_velocity = scenario.ver_ವೇಗ_ಮೇಲೆ_m_s;
		ಹಿಂತಿರುಗಿ report;
	}

	daa_ಒಳಹರಿವು_s create_daa_ಒಳಹರಿವು(ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::Vector2d &uav_lat_lon, ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ uav_alt,
				     ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::Vector3f &uav_vel_ned, ಸ್ಥಿರಾಂಕ transponder_report_s &report) ಸ್ಥಿರಾಂಕ
	{
		daa_ಒಳಹರಿವು_s daa_ಒಳಹರಿವು{};
		daa_ಒಳಹರಿವು.uav_lat_lon = uav_lat_lon;
		daa_ಒಳಹರಿವು.uav_alt = uav_alt;
		daa_ಒಳಹರಿವು.uav_vel_ned = uav_vel_ned;
		daa_ಒಳಹರಿವು.transponder_report = report;
		ಹಿಂತಿರುಗಿ daa_ಒಳಹರಿವು;
	}
};

TEST_F(AdsbConflictTest, RejectsInvalidAircraftState)
{
	AdsbConflict adsb_conflict;
	ಸ್ಥಿರಾಂಕ RelativeTrafficScenario valid_traffic{200.f, math::radians(90.f), 0.f, 0.f, 30.f, 0.f};

	detect_and_avoid_s daa_output{};
	transponder_report_s report = create_relative_report(valid_traffic);
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ nan = std::numeric_limits<ದಶಮಾಂಶ>::quiet_NaN();
	ಸ್ಥಿರಾಂಕ ದ್ವಿಗುಣ nan_d = std::numeric_limits<ದ್ವಿಗುಣ>::quiet_NaN();
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ inf = std::numeric_limits<ದಶಮಾಂಶ>::infinity();

	// non-finite ownship coordinate
	EXPECT_FALSE(adsb_conflict.calculate_daa_output(create_daa_ಒಳಹರಿವು(ಮಾತೃಕೆ::Vector2d(nan_d, uav_lat_lon_(1)), uav_alt_,
			stationary_uav_vel_ned_, report), daa_output));
	EXPECT_FALSE(adsb_conflict.calculate_daa_output(create_daa_ಒಳಹರಿವು(ಮಾತೃಕೆ::Vector2d(uav_lat_lon_(0), nan_d), uav_alt_,
			stationary_uav_vel_ned_, report), daa_output));
	EXPECT_FALSE(adsb_conflict.calculate_daa_output(create_daa_ಒಳಹರಿವು(ಮಾತೃಕೆ::Vector2d(90.1, uav_lat_lon_(1)), uav_alt_,
			stationary_uav_vel_ned_, report), daa_output));
	EXPECT_FALSE(adsb_conflict.calculate_daa_output(create_daa_ಒಳಹರಿವು(ಮಾತೃಕೆ::Vector2d(uav_lat_lon_(0), -180.1), uav_alt_,
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
			ಮಾತೃಕೆ::Vector3f(0.f, nan, 0.f), report), daa_output));
}

#ಆದರೆ !defined(CONFIG_NAVIGATOR_ADSB_F3442) || !CONFIG_NAVIGATOR_ADSB_F3442
TEST_F(AdsbConflictTest, CrosstrackRejectsNonFiniteTrafficHeading)
{
	AdsbConflict adsb_conflict;
	ಸ್ಥಿರಾಂಕ RelativeTrafficScenario approaching_traffic{200.f, math::radians(90.f), 0.f, math::radians(270.f), 30.f, 0.f};

	detect_and_avoid_s daa_output{};
	transponder_report_s report = create_relative_report(approaching_traffic);
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ nan = std::numeric_limits<ದಶಮಾಂಶ>::quiet_NaN();

	report.heading = nan;
	EXPECT_FALSE(adsb_conflict.calculate_daa_output(create_daa_ಒಳಹರಿವು(uav_lat_lon_, uav_alt_,
			stationary_uav_vel_ned_, report), daa_output));
}
#ಆದರೆ_ಅಂತ್ಯ // !CONFIG_NAVIGATOR_ADSB_F3442

// try_updating_params() validates against the built standard and rejects bad values.
TEST_F(AdsbConflictTest, ಬಳಸುತ್ತದೆ_Built_Standard_For_Param_Validation)
{
	AdsbConflict adsb_conflict;
	constexpr ದಶಮಾಂಶ invalid_bound{-1.f};

	EXPECT_TRUE(adsb_conflict.try_updating_params());

#ಆದರೆ defined(CONFIG_NAVIGATOR_ADSB_F3442) && CONFIG_NAVIGATOR_ADSB_F3442
	ಗಣ_f3442_params(invalid_bound, 10.f, 100.f, 50.f, 30, 40);
#ಇಲ್ಲದಿದ್ದರೆ
	ಗಣ_crosstrack_params(invalid_bound, 500.f, 60);
#ಆದರೆ_ಅಂತ್ಯ // CONFIG_NAVIGATOR_ADSB_F3442

	EXPECT_FALSE(adsb_conflict.try_updating_params());
}

// The wrapper forwards the built standard's result (level + distances).
TEST_F(AdsbConflictTest, Delegates_ಗಮ್ಯ_Built_Standard_And_Forwards_Output)
{
	AdsbConflict adsb_conflict;
	ಸ್ಥಿರಾಂಕ RelativeTrafficScenario approaching_traffic{19.f, math::radians(90.f), 9.f, math::radians(270.f), 5.f, 0.f};
	ಸ್ಥಿರಾಂಕ transponder_report_s report = create_relative_report(approaching_traffic);

	detect_and_avoid_s daa_output{};

	ASSERT_TRUE(adsb_conflict.try_updating_params());
	ASSERT_TRUE(adsb_conflict.calculate_daa_output(create_daa_ಒಳಹರಿವು(uav_lat_lon_, uav_alt_,
			stationary_uav_vel_ned_, report), daa_output));

#ಆದರೆ defined(CONFIG_NAVIGATOR_ADSB_F3442) && CONFIG_NAVIGATOR_ADSB_F3442
	EXPECT_EQ(daa_output.conflict_level, detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL);
	EXPECT_NEAR(daa_output.aircraft_dist_hor, approaching_traffic.ದೂರ_m, 0.1f);
	EXPECT_NEAR(daa_output.aircraft_dist_vert, approaching_traffic.altitude_ಸ್ಥಾನಾಂತರ_m, 0.1f);
#ಇಲ್ಲದಿದ್ದರೆ
	EXPECT_EQ(daa_output.conflict_level, detect_and_avoid_s::DAA_CONFLICT_LVL_ಮೇಲಿನ);
	EXPECT_NEAR(daa_output.aircraft_dist_vert, approaching_traffic.altitude_ಸ್ಥಾನಾಂತರ_m, 0.1f);
	EXPECT_LT(fabsf(daa_output.aircraft_dist_hor), 500.f);
#ಆದರೆ_ಅಂತ್ಯ // CONFIG_NAVIGATOR_ADSB_F3442
}
