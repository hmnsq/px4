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

#ਅਨ੍ਤਰ੍ਭਾਵ <gtest/gtest.h>
#ਅਨ੍ਤਰ੍ਭਾਵ "AdsbConflict.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <parameters/param.h>

#ਅਨ੍ਤਰ੍ਭਾਵ <lib/geo/geo.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <lib/mathlib/mathlib.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <limits>

ਬਣਤਰ RelativeTrafficScenario {
	ਦਸ਼ਮਲਵ ਦੂਰੀ_m;
	ਦਸ਼ਮਲਵ bearing_rad;
	ਦਸ਼ਮਲਵ altitude_ਵਿੱਥ_m;
	ਦਸ਼ਮਲਵ heading_rad;
	ਦਸ਼ਮਲਵ hor_ਰਫਤਾਰ_m_s;
	ਦਸ਼ਮਲਵ ver_ਰਫਤਾਰ_ਉੱਪਰ_m_s;
};

ਵਰਗ AdsbConflictTest : ਜਨਤਕ ::testing::Test
{
ਸੁਰੱਖਿਅਤ:
	ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector2d uav_lat_lon_{32.617013, -96.490564};
	ਅਚਲ ਦਸ਼ਮਲਵ uav_alt_{1000.f};
	ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector3f stationary_uav_vel_ned_{0.f, 0.f, 0.f};

	ਖਾਲੀ ਸੈੱਟ_ਉੱਪਰ() override
	{
		param_control_autosave(ਝੂਠ);
		param_reset_all();
#ਜੇ defined(CONFIG_NAVIGATOR_ADSB_F3442) && CONFIG_NAVIGATOR_ADSB_F3442
		ਸੈੱਟ_f3442_params(20.f, 10.f, 100.f, 50.f, 30, 40);
#ਨਹੀਂਤਾਂ
		ਸੈੱਟ_crosstrack_params(500.f, 500.f, 60);
#ਜੇ_ਅੰਤ // CONFIG_NAVIGATOR_ADSB_F3442
	}

#ਜੇ defined(CONFIG_NAVIGATOR_ADSB_F3442) && CONFIG_NAVIGATOR_ADSB_F3442
	ਖਾਲੀ ਸੈੱਟ_f3442_params(ਦਸ਼ਮਲਵ nmac_radius, ਦਸ਼ਮਲਵ nmac_ਉਚਾਈ, ਦਸ਼ਮਲਵ wc_radius,
			      ਦਸ਼ਮਲਵ wc_ਉਚਾਈ, ਪੂਰਨ nmac_latency_s, ਪੂਰਨ wc_latency_s)
	{
		param_ਸੈੱਟ(param_handle(px4::params::DAA_LVL_CRIT_RAD), &nmac_radius);
		param_ਸੈੱਟ(param_handle(px4::params::DAA_LVL_CRIT_HGT), &nmac_ਉਚਾਈ);
		param_ਸੈੱਟ(param_handle(px4::params::DAA_LVL_ਉੱਚਾ_RAD), &wc_radius);
		param_ਸੈੱਟ(param_handle(px4::params::DAA_LVL_ਉੱਚਾ_HGT), &wc_ਉਚਾਈ);
		param_ਸੈੱਟ(param_handle(px4::params::DAA_LVL_MED_ਸਮਾਂ), &nmac_latency_s);
		param_ਸੈੱਟ(param_handle(px4::params::DAA_LVL_ਨੀਵਾਂ_ਸਮਾਂ), &wc_latency_s);
	}
#ਜੇ_ਅੰਤ // CONFIG_NAVIGATOR_ADSB_F3442

	ਖਾਲੀ ਸੈੱਟ_crosstrack_params(ਦਸ਼ਮਲਵ horizontal_separation_m,
				   ਦਸ਼ਮਲਵ vertical_separation_m, ਪੂਰਨ collision_ਸਮਾਂ_s)
	{
		param_ਸੈੱਟ(param_handle(px4::params::NAV_TRAFF_A_HOR), &horizontal_separation_m);
		param_ਸੈੱਟ(param_handle(px4::params::NAV_TRAFF_A_VER), &vertical_separation_m);
		param_ਸੈੱਟ(param_handle(px4::params::NAV_TRAFF_COLL_T), &collision_ਸਮਾਂ_s);
	}

	// Build a traffic report from a relative encounter against the fixed ownship state.
	transponder_report_s create_relative_report(ਅਚਲ RelativeTrafficScenario &scenario)
	{
		transponder_report_s report{};
		report.timestamp = hrt_absolute_ਸਮਾਂ();
		waypoint_from_heading_and_ਦੂਰੀ(uav_lat_lon_(0), uav_lat_lon_(1),
						   scenario.bearing_rad, scenario.ਦੂਰੀ_m, &report.lat, &report.lon);
		report.altitude = uav_alt_ + scenario.altitude_ਵਿੱਥ_m;
		report.heading = scenario.heading_rad;
		report.hor_velocity = scenario.hor_ਰਫਤਾਰ_m_s;
		report.ver_velocity = scenario.ver_ਰਫਤਾਰ_ਉੱਪਰ_m_s;
		ਵਾਪਸ report;
	}

	daa_ਨਿਵੇਸ਼_s create_daa_ਨਿਵੇਸ਼(ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector2d &uav_lat_lon, ਅਚਲ ਦਸ਼ਮਲਵ uav_alt,
				     ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector3f &uav_vel_ned, ਅਚਲ transponder_report_s &report) ਅਚਲ
	{
		daa_ਨਿਵੇਸ਼_s daa_ਨਿਵੇਸ਼{};
		daa_ਨਿਵੇਸ਼.uav_lat_lon = uav_lat_lon;
		daa_ਨਿਵੇਸ਼.uav_alt = uav_alt;
		daa_ਨਿਵੇਸ਼.uav_vel_ned = uav_vel_ned;
		daa_ਨਿਵੇਸ਼.transponder_report = report;
		ਵਾਪਸ daa_ਨਿਵੇਸ਼;
	}
};

TEST_F(AdsbConflictTest, RejectsInvalidAircraftState)
{
	AdsbConflict adsb_conflict;
	ਅਚਲ RelativeTrafficScenario valid_traffic{200.f, math::radians(90.f), 0.f, 0.f, 30.f, 0.f};

	detect_and_avoid_s daa_output{};
	transponder_report_s report = create_relative_report(valid_traffic);
	ਅਚਲ ਦਸ਼ਮਲਵ nan = std::numeric_limits<ਦਸ਼ਮਲਵ>::quiet_NaN();
	ਅਚਲ ਦੁੱਗਣਾ nan_d = std::numeric_limits<ਦੁੱਗਣਾ>::quiet_NaN();
	ਅਚਲ ਦਸ਼ਮਲਵ inf = std::numeric_limits<ਦਸ਼ਮਲਵ>::infinity();

	// non-finite ownship coordinate
	EXPECT_FALSE(adsb_conflict.calculate_daa_output(create_daa_ਨਿਵੇਸ਼(ਮੈਟ੍ਰਿਕਸ::Vector2d(nan_d, uav_lat_lon_(1)), uav_alt_,
			stationary_uav_vel_ned_, report), daa_output));
	EXPECT_FALSE(adsb_conflict.calculate_daa_output(create_daa_ਨਿਵੇਸ਼(ਮੈਟ੍ਰਿਕਸ::Vector2d(uav_lat_lon_(0), nan_d), uav_alt_,
			stationary_uav_vel_ned_, report), daa_output));
	EXPECT_FALSE(adsb_conflict.calculate_daa_output(create_daa_ਨਿਵੇਸ਼(ਮੈਟ੍ਰਿਕਸ::Vector2d(90.1, uav_lat_lon_(1)), uav_alt_,
			stationary_uav_vel_ned_, report), daa_output));
	EXPECT_FALSE(adsb_conflict.calculate_daa_output(create_daa_ਨਿਵੇਸ਼(ਮੈਟ੍ਰਿਕਸ::Vector2d(uav_lat_lon_(0), -180.1), uav_alt_,
			stationary_uav_vel_ned_, report), daa_output));

	report.lat = nan_d;
	EXPECT_FALSE(adsb_conflict.calculate_daa_output(create_daa_ਨਿਵੇਸ਼(uav_lat_lon_, uav_alt_,
			stationary_uav_vel_ned_, report), daa_output));
	report = create_relative_report(valid_traffic);
	report.lon = 180.1;
	EXPECT_FALSE(adsb_conflict.calculate_daa_output(create_daa_ਨਿਵੇਸ਼(uav_lat_lon_, uav_alt_,
			stationary_uav_vel_ned_, report), daa_output));

	report = create_relative_report(valid_traffic);
	EXPECT_FALSE(adsb_conflict.calculate_daa_output(create_daa_ਨਿਵੇਸ਼(uav_lat_lon_, nan,
			stationary_uav_vel_ned_, report), daa_output));
	report.altitude = inf;
	EXPECT_FALSE(adsb_conflict.calculate_daa_output(create_daa_ਨਿਵੇਸ਼(uav_lat_lon_, uav_alt_,
			stationary_uav_vel_ned_, report), daa_output));

	report = create_relative_report(valid_traffic);
	report.hor_velocity = inf;
	EXPECT_FALSE(adsb_conflict.calculate_daa_output(create_daa_ਨਿਵੇਸ਼(uav_lat_lon_, uav_alt_,
			stationary_uav_vel_ned_, report), daa_output));
	report.hor_velocity = -1.f;
	EXPECT_FALSE(adsb_conflict.calculate_daa_output(create_daa_ਨਿਵੇਸ਼(uav_lat_lon_, uav_alt_,
			stationary_uav_vel_ned_, report), daa_output));

	report = create_relative_report(valid_traffic);
	EXPECT_FALSE(adsb_conflict.calculate_daa_output(create_daa_ਨਿਵੇਸ਼(uav_lat_lon_, uav_alt_,
			ਮੈਟ੍ਰਿਕਸ::Vector3f(0.f, nan, 0.f), report), daa_output));
}

#ਜੇ !defined(CONFIG_NAVIGATOR_ADSB_F3442) || !CONFIG_NAVIGATOR_ADSB_F3442
TEST_F(AdsbConflictTest, CrosstrackRejectsNonFiniteTrafficHeading)
{
	AdsbConflict adsb_conflict;
	ਅਚਲ RelativeTrafficScenario approaching_traffic{200.f, math::radians(90.f), 0.f, math::radians(270.f), 30.f, 0.f};

	detect_and_avoid_s daa_output{};
	transponder_report_s report = create_relative_report(approaching_traffic);
	ਅਚਲ ਦਸ਼ਮਲਵ nan = std::numeric_limits<ਦਸ਼ਮਲਵ>::quiet_NaN();

	report.heading = nan;
	EXPECT_FALSE(adsb_conflict.calculate_daa_output(create_daa_ਨਿਵੇਸ਼(uav_lat_lon_, uav_alt_,
			stationary_uav_vel_ned_, report), daa_output));
}
#ਜੇ_ਅੰਤ // !CONFIG_NAVIGATOR_ADSB_F3442

// try_updating_params() validates against the built standard and rejects bad values.
TEST_F(AdsbConflictTest, ਵਰਤਦਾ_Built_Standard_For_Param_Validation)
{
	AdsbConflict adsb_conflict;
	constexpr ਦਸ਼ਮਲਵ invalid_bound{-1.f};

	EXPECT_TRUE(adsb_conflict.try_updating_params());

#ਜੇ defined(CONFIG_NAVIGATOR_ADSB_F3442) && CONFIG_NAVIGATOR_ADSB_F3442
	ਸੈੱਟ_f3442_params(invalid_bound, 10.f, 100.f, 50.f, 30, 40);
#ਨਹੀਂਤਾਂ
	ਸੈੱਟ_crosstrack_params(invalid_bound, 500.f, 60);
#ਜੇ_ਅੰਤ // CONFIG_NAVIGATOR_ADSB_F3442

	EXPECT_FALSE(adsb_conflict.try_updating_params());
}

// The wrapper forwards the built standard's result (level + distances).
TEST_F(AdsbConflictTest, Delegates_ਮੰਜ਼ਿਲ_Built_Standard_And_Forwards_Output)
{
	AdsbConflict adsb_conflict;
	ਅਚਲ RelativeTrafficScenario approaching_traffic{19.f, math::radians(90.f), 9.f, math::radians(270.f), 5.f, 0.f};
	ਅਚਲ transponder_report_s report = create_relative_report(approaching_traffic);

	detect_and_avoid_s daa_output{};

	ASSERT_TRUE(adsb_conflict.try_updating_params());
	ASSERT_TRUE(adsb_conflict.calculate_daa_output(create_daa_ਨਿਵੇਸ਼(uav_lat_lon_, uav_alt_,
			stationary_uav_vel_ned_, report), daa_output));

#ਜੇ defined(CONFIG_NAVIGATOR_ADSB_F3442) && CONFIG_NAVIGATOR_ADSB_F3442
	EXPECT_EQ(daa_output.conflict_level, detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL);
	EXPECT_NEAR(daa_output.aircraft_dist_hor, approaching_traffic.ਦੂਰੀ_m, 0.1f);
	EXPECT_NEAR(daa_output.aircraft_dist_vert, approaching_traffic.altitude_ਵਿੱਥ_m, 0.1f);
#ਨਹੀਂਤਾਂ
	EXPECT_EQ(daa_output.conflict_level, detect_and_avoid_s::DAA_CONFLICT_LVL_ਉੱਚਾ);
	EXPECT_NEAR(daa_output.aircraft_dist_vert, approaching_traffic.altitude_ਵਿੱਥ_m, 0.1f);
	EXPECT_LT(fabsf(daa_output.aircraft_dist_hor), 500.f);
#ਜੇ_ਅੰਤ // CONFIG_NAVIGATOR_ADSB_F3442
}
