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
#ضمن "AdsbConflict.h"

#ضمن <parameters/param.h>

#ضمن <lib/geo/geo.h>
#ضمن <lib/mathlib/mathlib.h>
#ضمن <limits>

بنية RelativeTrafficScenario {
	عائم فاصله_m;
	عائم bearing_rad;
	عائم altitude_جابه_جایی_m;
	عائم heading_rad;
	عائم hor_سرعت_m_s;
	عائم ver_سرعت_به_بالا_m_s;
};

صنف AdsbConflictTest : عام ::testing::Test
{
محمي:
	ثابت ماتریس::Vector2d uav_lat_lon_{32.617013, -96.490564};
	ثابت عائم uav_alt_{1000.f};
	ثابت ماتریس::Vector3f stationary_uav_vel_ned_{0.f, 0.f, 0.f};

	فراغ مجموعه_به_بالا() override
	{
		param_control_autosave(خطأ);
		param_reset_all();
#إذا defined(CONFIG_NAVIGATOR_ADSB_F3442) && CONFIG_NAVIGATOR_ADSB_F3442
		مجموعه_f3442_params(20.f, 10.f, 100.f, 50.f, 30, 40);
#وإلا
		مجموعه_crosstrack_params(500.f, 500.f, 60);
#پایان_اگر // CONFIG_NAVIGATOR_ADSB_F3442
	}

#إذا defined(CONFIG_NAVIGATOR_ADSB_F3442) && CONFIG_NAVIGATOR_ADSB_F3442
	فراغ مجموعه_f3442_params(عائم nmac_radius, عائم nmac_ارتفاع, عائم wc_radius,
			      عائم wc_ارتفاع, صحيح nmac_latency_s, صحيح wc_latency_s)
	{
		param_مجموعه(param_handle(px4::params::DAA_LVL_CRIT_RAD), &nmac_radius);
		param_مجموعه(param_handle(px4::params::DAA_LVL_CRIT_HGT), &nmac_ارتفاع);
		param_مجموعه(param_handle(px4::params::DAA_LVL_بالا_RAD), &wc_radius);
		param_مجموعه(param_handle(px4::params::DAA_LVL_بالا_HGT), &wc_ارتفاع);
		param_مجموعه(param_handle(px4::params::DAA_LVL_MED_زمان), &nmac_latency_s);
		param_مجموعه(param_handle(px4::params::DAA_LVL_کم_زمان), &wc_latency_s);
	}
#پایان_اگر // CONFIG_NAVIGATOR_ADSB_F3442

	فراغ مجموعه_crosstrack_params(عائم horizontal_separation_m,
				   عائم vertical_separation_m, صحيح collision_زمان_s)
	{
		param_مجموعه(param_handle(px4::params::NAV_TRAFF_A_HOR), &horizontal_separation_m);
		param_مجموعه(param_handle(px4::params::NAV_TRAFF_A_VER), &vertical_separation_m);
		param_مجموعه(param_handle(px4::params::NAV_TRAFF_COLL_T), &collision_زمان_s);
	}

	// Build a traffic report from a relative encounter against the fixed ownship state.
	transponder_report_s create_relative_report(ثابت RelativeTrafficScenario &scenario)
	{
		transponder_report_s report{};
		report.timestamp = hrt_absolute_زمان();
		waypoint_from_heading_and_فاصله(uav_lat_lon_(0), uav_lat_lon_(1),
						   scenario.bearing_rad, scenario.فاصله_m, &report.lat, &report.lon);
		report.altitude = uav_alt_ + scenario.altitude_جابه_جایی_m;
		report.heading = scenario.heading_rad;
		report.hor_velocity = scenario.hor_سرعت_m_s;
		report.ver_velocity = scenario.ver_سرعت_به_بالا_m_s;
		أرجع report;
	}

	daa_درون_داد_s create_daa_درون_داد(ثابت ماتریس::Vector2d &uav_lat_lon, ثابت عائم uav_alt,
				     ثابت ماتریس::Vector3f &uav_vel_ned, ثابت transponder_report_s &report) ثابت
	{
		daa_درون_داد_s daa_درون_داد{};
		daa_درون_داد.uav_lat_lon = uav_lat_lon;
		daa_درون_داد.uav_alt = uav_alt;
		daa_درون_داد.uav_vel_ned = uav_vel_ned;
		daa_درون_داد.transponder_report = report;
		أرجع daa_درون_داد;
	}
};

TEST_F(AdsbConflictTest, RejectsInvalidAircraftState)
{
	AdsbConflict adsb_conflict;
	ثابت RelativeTrafficScenario valid_traffic{200.f, math::radians(90.f), 0.f, 0.f, 30.f, 0.f};

	detect_and_avoid_s daa_output{};
	transponder_report_s report = create_relative_report(valid_traffic);
	ثابت عائم nan = std::numeric_limits<عائم>::quiet_NaN();
	ثابت مزدوج nan_d = std::numeric_limits<مزدوج>::quiet_NaN();
	ثابت عائم inf = std::numeric_limits<عائم>::infinity();

	// non-finite ownship coordinate
	EXPECT_FALSE(adsb_conflict.calculate_daa_output(create_daa_درون_داد(ماتریس::Vector2d(nan_d, uav_lat_lon_(1)), uav_alt_,
			stationary_uav_vel_ned_, report), daa_output));
	EXPECT_FALSE(adsb_conflict.calculate_daa_output(create_daa_درون_داد(ماتریس::Vector2d(uav_lat_lon_(0), nan_d), uav_alt_,
			stationary_uav_vel_ned_, report), daa_output));
	EXPECT_FALSE(adsb_conflict.calculate_daa_output(create_daa_درون_داد(ماتریس::Vector2d(90.1, uav_lat_lon_(1)), uav_alt_,
			stationary_uav_vel_ned_, report), daa_output));
	EXPECT_FALSE(adsb_conflict.calculate_daa_output(create_daa_درون_داد(ماتریس::Vector2d(uav_lat_lon_(0), -180.1), uav_alt_,
			stationary_uav_vel_ned_, report), daa_output));

	report.lat = nan_d;
	EXPECT_FALSE(adsb_conflict.calculate_daa_output(create_daa_درون_داد(uav_lat_lon_, uav_alt_,
			stationary_uav_vel_ned_, report), daa_output));
	report = create_relative_report(valid_traffic);
	report.lon = 180.1;
	EXPECT_FALSE(adsb_conflict.calculate_daa_output(create_daa_درون_داد(uav_lat_lon_, uav_alt_,
			stationary_uav_vel_ned_, report), daa_output));

	report = create_relative_report(valid_traffic);
	EXPECT_FALSE(adsb_conflict.calculate_daa_output(create_daa_درون_داد(uav_lat_lon_, nan,
			stationary_uav_vel_ned_, report), daa_output));
	report.altitude = inf;
	EXPECT_FALSE(adsb_conflict.calculate_daa_output(create_daa_درون_داد(uav_lat_lon_, uav_alt_,
			stationary_uav_vel_ned_, report), daa_output));

	report = create_relative_report(valid_traffic);
	report.hor_velocity = inf;
	EXPECT_FALSE(adsb_conflict.calculate_daa_output(create_daa_درون_داد(uav_lat_lon_, uav_alt_,
			stationary_uav_vel_ned_, report), daa_output));
	report.hor_velocity = -1.f;
	EXPECT_FALSE(adsb_conflict.calculate_daa_output(create_daa_درون_داد(uav_lat_lon_, uav_alt_,
			stationary_uav_vel_ned_, report), daa_output));

	report = create_relative_report(valid_traffic);
	EXPECT_FALSE(adsb_conflict.calculate_daa_output(create_daa_درون_داد(uav_lat_lon_, uav_alt_,
			ماتریس::Vector3f(0.f, nan, 0.f), report), daa_output));
}

#إذا !defined(CONFIG_NAVIGATOR_ADSB_F3442) || !CONFIG_NAVIGATOR_ADSB_F3442
TEST_F(AdsbConflictTest, CrosstrackRejectsNonFiniteTrafficHeading)
{
	AdsbConflict adsb_conflict;
	ثابت RelativeTrafficScenario approaching_traffic{200.f, math::radians(90.f), 0.f, math::radians(270.f), 30.f, 0.f};

	detect_and_avoid_s daa_output{};
	transponder_report_s report = create_relative_report(approaching_traffic);
	ثابت عائم nan = std::numeric_limits<عائم>::quiet_NaN();

	report.heading = nan;
	EXPECT_FALSE(adsb_conflict.calculate_daa_output(create_daa_درون_داد(uav_lat_lon_, uav_alt_,
			stationary_uav_vel_ned_, report), daa_output));
}
#پایان_اگر // !CONFIG_NAVIGATOR_ADSB_F3442

// try_updating_params() validates against the built standard and rejects bad values.
TEST_F(AdsbConflictTest, استفاده_میکند_Built_Standard_For_Param_Validation)
{
	AdsbConflict adsb_conflict;
	constexpr عائم invalid_bound{-1.f};

	EXPECT_TRUE(adsb_conflict.try_updating_params());

#إذا defined(CONFIG_NAVIGATOR_ADSB_F3442) && CONFIG_NAVIGATOR_ADSB_F3442
	مجموعه_f3442_params(invalid_bound, 10.f, 100.f, 50.f, 30, 40);
#وإلا
	مجموعه_crosstrack_params(invalid_bound, 500.f, 60);
#پایان_اگر // CONFIG_NAVIGATOR_ADSB_F3442

	EXPECT_FALSE(adsb_conflict.try_updating_params());
}

// The wrapper forwards the built standard's result (level + distances).
TEST_F(AdsbConflictTest, Delegates_مقصد_Built_Standard_And_Forwards_Output)
{
	AdsbConflict adsb_conflict;
	ثابت RelativeTrafficScenario approaching_traffic{19.f, math::radians(90.f), 9.f, math::radians(270.f), 5.f, 0.f};
	ثابت transponder_report_s report = create_relative_report(approaching_traffic);

	detect_and_avoid_s daa_output{};

	ASSERT_TRUE(adsb_conflict.try_updating_params());
	ASSERT_TRUE(adsb_conflict.calculate_daa_output(create_daa_درون_داد(uav_lat_lon_, uav_alt_,
			stationary_uav_vel_ned_, report), daa_output));

#إذا defined(CONFIG_NAVIGATOR_ADSB_F3442) && CONFIG_NAVIGATOR_ADSB_F3442
	EXPECT_EQ(daa_output.conflict_level, detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL);
	EXPECT_NEAR(daa_output.aircraft_dist_hor, approaching_traffic.فاصله_m, 0.1f);
	EXPECT_NEAR(daa_output.aircraft_dist_vert, approaching_traffic.altitude_جابه_جایی_m, 0.1f);
#وإلا
	EXPECT_EQ(daa_output.conflict_level, detect_and_avoid_s::DAA_CONFLICT_LVL_بالا);
	EXPECT_NEAR(daa_output.aircraft_dist_vert, approaching_traffic.altitude_جابه_جایی_m, 0.1f);
	EXPECT_LT(fabsf(daa_output.aircraft_dist_hor), 500.f);
#پایان_اگر // CONFIG_NAVIGATOR_ADSB_F3442
}
