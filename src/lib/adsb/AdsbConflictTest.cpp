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
#అన్తర్భావ "AdsbConflict.h"

#అన్తర్భావ <parameters/param.h>

#అన్తర్భావ <lib/geo/geo.h>
#అన్తర్భావ <lib/mathlib/mathlib.h>
#అన్తర్భావ <limits>

నిర్మాణం RelativeTrafficScenario {
	దశాంశం దూరం_m;
	దశాంశం bearing_rad;
	దశాంశం altitude_స్థానభ్రంశం_m;
	దశాంశం heading_rad;
	దశాంశం hor_వేగం_m_s;
	దశాంశం ver_వేగం_పైకి_m_s;
};

తరగతి AdsbConflictTest : బహిరంగ ::testing::Test
{
రక్షిత:
	స్థిరరాశి మాతృక::Vector2d uav_lat_lon_{32.617013, -96.490564};
	స్థిరరాశి దశాంశం uav_alt_{1000.f};
	స్థిరరాశి మాతృక::Vector3f stationary_uav_vel_ned_{0.f, 0.f, 0.f};

	శూన్య సమితి_పైకి() override
	{
		param_control_autosave(అబద్ధం);
		param_reset_all();
#ఒకవేళ defined(CONFIG_NAVIGATOR_ADSB_F3442) && CONFIG_NAVIGATOR_ADSB_F3442
		సమితి_f3442_params(20.f, 10.f, 100.f, 50.f, 30, 40);
#లేకపోతే
		సమితి_crosstrack_params(500.f, 500.f, 60);
#అయితే_ముగింపు // CONFIG_NAVIGATOR_ADSB_F3442
	}

#ఒకవేళ defined(CONFIG_NAVIGATOR_ADSB_F3442) && CONFIG_NAVIGATOR_ADSB_F3442
	శూన్య సమితి_f3442_params(దశాంశం nmac_radius, దశాంశం nmac_ఎత్తు, దశాంశం wc_radius,
			      దశాంశం wc_ఎత్తు, పూర్ణాంకం nmac_latency_s, పూర్ణాంకం wc_latency_s)
	{
		param_సమితి(param_handle(px4::params::DAA_LVL_CRIT_RAD), &nmac_radius);
		param_సమితి(param_handle(px4::params::DAA_LVL_CRIT_HGT), &nmac_ఎత్తు);
		param_సమితి(param_handle(px4::params::DAA_LVL_ఎగువ_RAD), &wc_radius);
		param_సమితి(param_handle(px4::params::DAA_LVL_ఎగువ_HGT), &wc_ఎత్తు);
		param_సమితి(param_handle(px4::params::DAA_LVL_MED_సమయం), &nmac_latency_s);
		param_సమితి(param_handle(px4::params::DAA_LVL_దిగువ_సమయం), &wc_latency_s);
	}
#అయితే_ముగింపు // CONFIG_NAVIGATOR_ADSB_F3442

	శూన్య సమితి_crosstrack_params(దశాంశం horizontal_separation_m,
				   దశాంశం vertical_separation_m, పూర్ణాంకం collision_సమయం_s)
	{
		param_సమితి(param_handle(px4::params::NAV_TRAFF_A_HOR), &horizontal_separation_m);
		param_సమితి(param_handle(px4::params::NAV_TRAFF_A_VER), &vertical_separation_m);
		param_సమితి(param_handle(px4::params::NAV_TRAFF_COLL_T), &collision_సమయం_s);
	}

	// Build a traffic report from a relative encounter against the fixed ownship state.
	transponder_report_s create_relative_report(స్థిరరాశి RelativeTrafficScenario &scenario)
	{
		transponder_report_s report{};
		report.timestamp = hrt_absolute_సమయం();
		waypoint_from_heading_and_దూరం(uav_lat_lon_(0), uav_lat_lon_(1),
						   scenario.bearing_rad, scenario.దూరం_m, &report.lat, &report.lon);
		report.altitude = uav_alt_ + scenario.altitude_స్థానభ్రంశం_m;
		report.heading = scenario.heading_rad;
		report.hor_velocity = scenario.hor_వేగం_m_s;
		report.ver_velocity = scenario.ver_వేగం_పైకి_m_s;
		తిరిగి report;
	}

	daa_ఆదానం_s create_daa_ఆదానం(స్థిరరాశి మాతృక::Vector2d &uav_lat_lon, స్థిరరాశి దశాంశం uav_alt,
				     స్థిరరాశి మాతృక::Vector3f &uav_vel_ned, స్థిరరాశి transponder_report_s &report) స్థిరరాశి
	{
		daa_ఆదానం_s daa_ఆదానం{};
		daa_ఆదానం.uav_lat_lon = uav_lat_lon;
		daa_ఆదానం.uav_alt = uav_alt;
		daa_ఆదానం.uav_vel_ned = uav_vel_ned;
		daa_ఆదానం.transponder_report = report;
		తిరిగి daa_ఆదానం;
	}
};

TEST_F(AdsbConflictTest, RejectsInvalidAircraftState)
{
	AdsbConflict adsb_conflict;
	స్థిరరాశి RelativeTrafficScenario valid_traffic{200.f, math::radians(90.f), 0.f, 0.f, 30.f, 0.f};

	detect_and_avoid_s daa_output{};
	transponder_report_s report = create_relative_report(valid_traffic);
	స్థిరరాశి దశాంశం nan = std::numeric_limits<దశాంశం>::quiet_NaN();
	స్థిరరాశి ద్విగుణ nan_d = std::numeric_limits<ద్విగుణ>::quiet_NaN();
	స్థిరరాశి దశాంశం inf = std::numeric_limits<దశాంశం>::infinity();

	// non-finite ownship coordinate
	EXPECT_FALSE(adsb_conflict.calculate_daa_output(create_daa_ఆదానం(మాతృక::Vector2d(nan_d, uav_lat_lon_(1)), uav_alt_,
			stationary_uav_vel_ned_, report), daa_output));
	EXPECT_FALSE(adsb_conflict.calculate_daa_output(create_daa_ఆదానం(మాతృక::Vector2d(uav_lat_lon_(0), nan_d), uav_alt_,
			stationary_uav_vel_ned_, report), daa_output));
	EXPECT_FALSE(adsb_conflict.calculate_daa_output(create_daa_ఆదానం(మాతృక::Vector2d(90.1, uav_lat_lon_(1)), uav_alt_,
			stationary_uav_vel_ned_, report), daa_output));
	EXPECT_FALSE(adsb_conflict.calculate_daa_output(create_daa_ఆదానం(మాతృక::Vector2d(uav_lat_lon_(0), -180.1), uav_alt_,
			stationary_uav_vel_ned_, report), daa_output));

	report.lat = nan_d;
	EXPECT_FALSE(adsb_conflict.calculate_daa_output(create_daa_ఆదానం(uav_lat_lon_, uav_alt_,
			stationary_uav_vel_ned_, report), daa_output));
	report = create_relative_report(valid_traffic);
	report.lon = 180.1;
	EXPECT_FALSE(adsb_conflict.calculate_daa_output(create_daa_ఆదానం(uav_lat_lon_, uav_alt_,
			stationary_uav_vel_ned_, report), daa_output));

	report = create_relative_report(valid_traffic);
	EXPECT_FALSE(adsb_conflict.calculate_daa_output(create_daa_ఆదానం(uav_lat_lon_, nan,
			stationary_uav_vel_ned_, report), daa_output));
	report.altitude = inf;
	EXPECT_FALSE(adsb_conflict.calculate_daa_output(create_daa_ఆదానం(uav_lat_lon_, uav_alt_,
			stationary_uav_vel_ned_, report), daa_output));

	report = create_relative_report(valid_traffic);
	report.hor_velocity = inf;
	EXPECT_FALSE(adsb_conflict.calculate_daa_output(create_daa_ఆదానం(uav_lat_lon_, uav_alt_,
			stationary_uav_vel_ned_, report), daa_output));
	report.hor_velocity = -1.f;
	EXPECT_FALSE(adsb_conflict.calculate_daa_output(create_daa_ఆదానం(uav_lat_lon_, uav_alt_,
			stationary_uav_vel_ned_, report), daa_output));

	report = create_relative_report(valid_traffic);
	EXPECT_FALSE(adsb_conflict.calculate_daa_output(create_daa_ఆదానం(uav_lat_lon_, uav_alt_,
			మాతృక::Vector3f(0.f, nan, 0.f), report), daa_output));
}

#ఒకవేళ !defined(CONFIG_NAVIGATOR_ADSB_F3442) || !CONFIG_NAVIGATOR_ADSB_F3442
TEST_F(AdsbConflictTest, CrosstrackRejectsNonFiniteTrafficHeading)
{
	AdsbConflict adsb_conflict;
	స్థిరరాశి RelativeTrafficScenario approaching_traffic{200.f, math::radians(90.f), 0.f, math::radians(270.f), 30.f, 0.f};

	detect_and_avoid_s daa_output{};
	transponder_report_s report = create_relative_report(approaching_traffic);
	స్థిరరాశి దశాంశం nan = std::numeric_limits<దశాంశం>::quiet_NaN();

	report.heading = nan;
	EXPECT_FALSE(adsb_conflict.calculate_daa_output(create_daa_ఆదానం(uav_lat_lon_, uav_alt_,
			stationary_uav_vel_ned_, report), daa_output));
}
#అయితే_ముగింపు // !CONFIG_NAVIGATOR_ADSB_F3442

// try_updating_params() validates against the built standard and rejects bad values.
TEST_F(AdsbConflictTest, ఉపయోగిస్తుంది_Built_Standard_For_Param_Validation)
{
	AdsbConflict adsb_conflict;
	constexpr దశాంశం invalid_bound{-1.f};

	EXPECT_TRUE(adsb_conflict.try_updating_params());

#ఒకవేళ defined(CONFIG_NAVIGATOR_ADSB_F3442) && CONFIG_NAVIGATOR_ADSB_F3442
	సమితి_f3442_params(invalid_bound, 10.f, 100.f, 50.f, 30, 40);
#లేకపోతే
	సమితి_crosstrack_params(invalid_bound, 500.f, 60);
#అయితే_ముగింపు // CONFIG_NAVIGATOR_ADSB_F3442

	EXPECT_FALSE(adsb_conflict.try_updating_params());
}

// The wrapper forwards the built standard's result (level + distances).
TEST_F(AdsbConflictTest, Delegates_గమ్యం_Built_Standard_And_Forwards_Output)
{
	AdsbConflict adsb_conflict;
	స్థిరరాశి RelativeTrafficScenario approaching_traffic{19.f, math::radians(90.f), 9.f, math::radians(270.f), 5.f, 0.f};
	స్థిరరాశి transponder_report_s report = create_relative_report(approaching_traffic);

	detect_and_avoid_s daa_output{};

	ASSERT_TRUE(adsb_conflict.try_updating_params());
	ASSERT_TRUE(adsb_conflict.calculate_daa_output(create_daa_ఆదానం(uav_lat_lon_, uav_alt_,
			stationary_uav_vel_ned_, report), daa_output));

#ఒకవేళ defined(CONFIG_NAVIGATOR_ADSB_F3442) && CONFIG_NAVIGATOR_ADSB_F3442
	EXPECT_EQ(daa_output.conflict_level, detect_and_avoid_s::DAA_CONFLICT_LVL_CRITICAL);
	EXPECT_NEAR(daa_output.aircraft_dist_hor, approaching_traffic.దూరం_m, 0.1f);
	EXPECT_NEAR(daa_output.aircraft_dist_vert, approaching_traffic.altitude_స్థానభ్రంశం_m, 0.1f);
#లేకపోతే
	EXPECT_EQ(daa_output.conflict_level, detect_and_avoid_s::DAA_CONFLICT_LVL_ఎగువ);
	EXPECT_NEAR(daa_output.aircraft_dist_vert, approaching_traffic.altitude_స్థానభ్రంశం_m, 0.1f);
	EXPECT_LT(fabsf(daa_output.aircraft_dist_hor), 500.f);
#అయితే_ముగింపు // CONFIG_NAVIGATOR_ADSB_F3442
}
