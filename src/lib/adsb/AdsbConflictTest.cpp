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

#അന്തര്ഭാവ <gtest/gtest.h>
#അന്തര്ഭാവ "AdsbConflict.h"

#അന്തര്ഭാവ <parameters/param.h>

#അന്തര്ഭാവ <lib/geo/geo.h>
#അന്തര്ഭാവ <lib/mathlib/mathlib.h>
#അന്തര്ഭാവ <limits>

ഘടന RelativeTrafficScenario {
	ദശാംശം distance_m;
	ദശാംശം bearing_rad;
	ദശാംശം altitude_offset_m;
	ദശാംശം heading_rad;
	ദശാംശം hor_speed_m_s;
	ദശാംശം ver_speed_up_m_s;
};

വർഗ്ഗം AdsbConflictTest : പൊതു ::testing::Test
{
സംരക്ഷിത:
	സ്ഥിരം matrix::Vector2d uav_lat_lon_{32.617013, -96.490564};
	സ്ഥിരം ദശാംശം uav_alt_{1000.f};
	സ്ഥിരം matrix::Vector3f stationary_uav_vel_ned_{0.f, 0.f, 0.f};

	ശൂന്യം SetUp() override
	{
		param_control_autosave(അസത്യം);
		param_reset_all();
#എങ്കിൽ defined(CONFIG_NAVIGATOR_ADSB_F3442) && CONFIG_NAVIGATOR_ADSB_F3442
		set_f3442_params(20.f, 10.f, 100.f, 50.f, 30, 40);
#അല്ലെങ്കിൽ
		set_crosstrack_params(500.f, 500.f, 60);
#endif // CONFIG_NAVIGATOR_ADSB_F3442
	}

#എങ്കിൽ defined(CONFIG_NAVIGATOR_ADSB_F3442) && CONFIG_NAVIGATOR_ADSB_F3442
	ശൂന്യം set_f3442_params(ദശാംശം nmac_radius, ദശാംശം nmac_height, ദശാംശം wc_radius,
			      ദശാംശം wc_height, പൂർണ്ണസംഖ്യ nmac_latency_s, പൂർണ്ണസംഖ്യ wc_latency_s)
	{
		param_set(param_handle(px4::params::DAA_LVL_CRIT_RAD), &nmac_radius);
		param_set(param_handle(px4::params::DAA_LVL_CRIT_HGT), &nmac_height);
		param_set(param_handle(px4::params::DAA_LVL_മുകളിലെ_RAD), &wc_radius);
		param_set(param_handle(px4::params::DAA_LVL_മുകളിലെ_HGT), &wc_height);
		param_set(param_handle(px4::params::DAA_LVL_MED_TIME), &nmac_latency_s);
		param_set(param_handle(px4::params::DAA_LVL_താഴെയുള്ള_TIME), &wc_latency_s);
	}
#endif // CONFIG_NAVIGATOR_ADSB_F3442

	ശൂന്യം set_crosstrack_params(ദശാംശം horizontal_separation_m,
				   ദശാംശം vertical_separation_m, പൂർണ്ണസംഖ്യ collision_time_s)
	{
		param_set(param_handle(px4::params::NAV_TRAFF_A_HOR), &horizontal_separation_m);
		param_set(param_handle(px4::params::NAV_TRAFF_A_VER), &vertical_separation_m);
		param_set(param_handle(px4::params::NAV_TRAFF_COLL_T), &collision_time_s);
	}

	// Build a traffic report from a relative encounter against the fixed ownship state.
	transponder_report_s create_relative_report(സ്ഥിരം RelativeTrafficScenario &scenario)
	{
		transponder_report_s report{};
		report.timestamp = hrt_absolute_time();
		waypoint_from_heading_and_distance(uav_lat_lon_(0), uav_lat_lon_(1),
						   scenario.bearing_rad, scenario.distance_m, &report.lat, &report.lon);
		report.altitude = uav_alt_ + scenario.altitude_offset_m;
		report.heading = scenario.heading_rad;
		report.hor_velocity = scenario.hor_speed_m_s;
		report.ver_velocity = scenario.ver_speed_up_m_s;
		തിരികെ report;
	}

	daa_നിവേശം_s create_daa_നിവേശം(സ്ഥിരം matrix::Vector2d &uav_lat_lon, സ്ഥിരം ദശാംശം uav_alt,
				     സ്ഥിരം matrix::Vector3f &uav_vel_ned, സ്ഥിരം transponder_report_s &report) സ്ഥിരം
	{
		daa_നിവേശം_s daa_നിവേശം{};
		daa_നിവേശം.uav_lat_lon = uav_lat_lon;
		daa_നിവേശം.uav_alt = uav_alt;
		daa_നിവേശം.uav_vel_ned = uav_vel_ned;
		daa_നിവേശം.transponder_report = report;
		തിരികെ daa_നിവേശം;
	}
};

TEST_F(AdsbConflictTest, RejectsInvalidAircraftState)
{
	AdsbConflict adsb_conflict;
	സ്ഥിരം RelativeTrafficScenario valid_traffic{200.f, math::radians(90.f), 0.f, 0.f, 30.f, 0.f};

	detect_and_avoid_s daa_output{};
	transponder_report_s report = create_relative_report(valid_traffic);
	സ്ഥിരം ദശാംശം nan = std::numeric_limits<ദശാംശം>::quiet_NaN();
	സ്ഥിരം ഇരട്ട nan_d = std::numeric_limits<ഇരട്ട>::quiet_NaN();
	സ്ഥിരം ദശാംശം inf = std::numeric_limits<ദശാംശം>::infinity();

	// non-finite ownship coordinate
	EXPECT_FALSE(adsb_conflict.calculate_daa_output(create_daa_നിവേശം(matrix::Vector2d(nan_d, uav_lat_lon_(1)), uav_alt_,
			stationary_uav_vel_ned_, report), daa_output));
	EXPECT_FALSE(adsb_conflict.calculate_daa_output(create_daa_നിവേശം(matrix::Vector2d(uav_lat_lon_(0), nan_d), uav_alt_,
			stationary_uav_vel_ned_, report), daa_output));
	EXPECT_FALSE(adsb_conflict.calculate_daa_output(create_daa_നിവേശം(matrix::Vector2d(90.1, uav_lat_lon_(1)), uav_alt_,
			stationary_uav_vel_ned_, report), daa_output));
	EXPECT_FALSE(adsb_conflict.calculate_daa_output(create_daa_നിവേശം(matrix::Vector2d(uav_lat_lon_(0), -180.1), uav_alt_,
			stationary_uav_vel_ned_, report), daa_output));

	report.lat = nan_d;
	EXPECT_FALSE(adsb_conflict.calculate_daa_output(create_daa_നിവേശം(uav_lat_lon_, uav_alt_,
			stationary_uav_vel_ned_, report), daa_output));
	report = create_relative_report(valid_traffic);
	report.lon = 180.1;
	EXPECT_FALSE(adsb_conflict.calculate_daa_output(create_daa_നിവേശം(uav_lat_lon_, uav_alt_,
			stationary_uav_vel_ned_, report), daa_output));

	report = create_relative_report(valid_traffic);
	EXPECT_FALSE(adsb_conflict.calculate_daa_output(create_daa_നിവേശം(uav_lat_lon_, nan,
			stationary_uav_vel_ned_, report), daa_output));
	report.altitude = inf;
	EXPECT_FALSE(adsb_conflict.calculate_daa_output(create_daa_നിവേശം(uav_lat_lon_, uav_alt_,
			stationary_uav_vel_ned_, report), daa_output));

	report = create_relative_report(valid_traffic);
	report.hor_velocity = inf;
	EXPECT_FALSE(adsb_conflict.calculate_daa_output(create_daa_നിവേശം(uav_lat_lon_, uav_alt_,
			stationary_uav_vel_ned_, report), daa_output));
	report.hor_velocity = -1.f;
	EXPECT_FALSE(adsb_conflict.calculate_daa_output(create_daa_നിവേശം(uav_lat_lon_, uav_alt_,
			stationary_uav_vel_ned_, report), daa_output));

	report = create_relative_report(valid_traffic);
	EXPECT_FALSE(adsb_conflict.calculate_daa_output(create_daa_നിവേശം(uav_lat_lon_, uav_alt_,
			matrix::Vector3f(0.f, nan, 0.f), report), daa_output));
}

#എങ്കിൽ !defined(CONFIG_NAVIGATOR_ADSB_F3442) || !CONFIG_NAVIGATOR_ADSB_F3442
TEST_F(AdsbConflictTest, CrosstrackRejectsNonFiniteTrafficHeading)
{
	AdsbConflict adsb_conflict;
	സ്ഥിരം RelativeTrafficScenario approaching_traffic{200.f, math::radians(90.f), 0.f, math::radians(270.f), 30.f, 0.f};

	detect_and_avoid_s daa_output{};
	transponder_report_s report = create_relative_report(approaching_traffic);
	സ്ഥിരം ദശാംശം nan = std::numeric_limits<ദശാംശം>::quiet_NaN();

	report.heading = nan;
	EXPECT_FALSE(adsb_conflict.calculate_daa_output(create_daa_നിവേശം(uav_lat_lon_, uav_alt_,
			stationary_uav_vel_ned_, report), daa_output));
}
#endif // !CONFIG_NAVIGATOR_ADSB_F3442

// try_updating_params() validates against the built standard and rejects bad values.
TEST_F(AdsbConflictTest, UsesBuiltStandardForParamValidation)
{
	AdsbConflict adsb_conflict;
	constexpr ദശാംശം invalid_bound{-1.f};

	EXPECT_TRUE(adsb_conflict.try_updating_params());

#എങ്കിൽ defined(CONFIG_NAVIGATOR_ADSB_F3442) && CONFIG_NAVIGATOR_ADSB_F3442
	set_f3442_params(invalid_bound, 10.f, 100.f, 50.f, 30, 40);
#അല്ലെങ്കിൽ
	set_crosstrack_params(invalid_bound, 500.f, 60);
#endif // CONFIG_NAVIGATOR_ADSB_F3442

	EXPECT_FALSE(adsb_conflict.try_updating_params());
}

// The wrapper forwards the built standard's result (level + distances).
TEST_F(AdsbConflictTest, DelegatesToBuiltStandardAndForwardsOutput)
{
	AdsbConflict adsb_conflict;
	സ്ഥിരം RelativeTrafficScenario approaching_traffic{19.f, math::radians(90.f), 9.f, math::radians(270.f), 5.f, 0.f};
	സ്ഥിരം transponder_report_s report = create_relative_report(approaching_traffic);

	detect_and_avoid_s daa_output{};

	ASSERT_TRUE(adsb_conflict.try_updating_params());
	ASSERT_TRUE(adsb_conflict.calculate_daa_output(create_daa_നിവേശം(uav_lat_lon_, uav_alt_,
			stationary_uav_vel_ned_, report), daa_output));

#എങ്കിൽ defined(CONFIG_NAVIGATOR_ADSB_F3442) && CONFIG_NAVIGATOR_ADSB_F3442
	EXPECT_EQ(daa_output.conflict_level, detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL);
	EXPECT_NEAR(daa_output.aircraft_dist_hor, approaching_traffic.distance_m, 0.1f);
	EXPECT_NEAR(daa_output.aircraft_dist_vert, approaching_traffic.altitude_offset_m, 0.1f);
#അല്ലെങ്കിൽ
	EXPECT_EQ(daa_output.conflict_level, detect_and_avoid_s::DAA_CONFLICT_LVL_മുകളിലെ);
	EXPECT_NEAR(daa_output.aircraft_dist_vert, approaching_traffic.altitude_offset_m, 0.1f);
	EXPECT_LT(fabsf(daa_output.aircraft_dist_hor), 500.f);
#endif // CONFIG_NAVIGATOR_ADSB_F3442
}
