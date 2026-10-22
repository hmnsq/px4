
/****************************************************************************
 *
 *   Copyright (C) 2025 PX4 Development Team. All rights reserved.
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

/******************************************************************
 * Test code for the NPFG algorithm
 * Run this test only using "make tests TESTFILTER=Npfg"
 *
 *
 * NOTE:
 *
 *
******************************************************************/

#ਅਨ੍ਤਰ੍ਭਾਵ <gtest/gtest.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <lib/npfg/CourseToAirspeedRefMapper.hpp>
#ਅਨ੍ਤਰ੍ਭਾਵ <lib/npfg/AirspeedDirectionController.hpp>

ਵਰਤੋਂ ਨਾਮਾਕਾਸ਼ ਮੈਟ੍ਰਿਕਸ;

TEST(NpfgTest, NoWind)
{
	ਕੋਰਸ_ਮੰਜ਼ਿਲ_Airspeed_Ref_Mapper ਕੋਰਸ_ਮੰਜ਼ਿਲ_airspeed;

	// GIVEN
	ਅਚਲ Vector2f wind_vel(0.f, 0.f);
	ਦਸ਼ਮਲਵ bearing = 0.f;
	ਦਸ਼ਮਲਵ airspeed_max = 20.f;
	ਦਸ਼ਮਲਵ min_ground_ਰਫਤਾਰ = 5.0f;
	ਦਸ਼ਮਲਵ airspeed_setpoint = 15.f;

	// WHEN: we update bearing and airspeed magnitude augmentation
	ਦਸ਼ਮਲਵ min_airspeed_for_bearing = ਕੋਰਸ_ਮੰਜ਼ਿਲ_airspeed.getMinAirspeedForCurrentBearing(bearing, wind_vel,
					 airspeed_max, min_ground_ਰਫਤਾਰ);

	ਦਸ਼ਮਲਵ airspeed_setpoint_adapted = math::constrain(airspeed_setpoint, min_airspeed_for_bearing, airspeed_max);

	ਦਸ਼ਮਲਵ heading_setpoint = ਕੋਰਸ_ਮੰਜ਼ਿਲ_airspeed.map_ਕੋਰਸ_Setpoint_ਮੰਜ਼ਿਲ_Heading_Setpoint(bearing, wind_vel,
				 airspeed_setpoint_adapted);

	// THEN: expect heading due North with a min airspeed equal to min_ground_speed
	EXPECT_NEAR(heading_setpoint, 0.f, 0.01f);
	EXPECT_NEAR(min_airspeed_for_bearing, min_ground_ਰਫਤਾਰ, FLT_EPSILON);

	// GIVEN: bearing due East
	bearing = M_PI_2_F;
	airspeed_max = 20.f;
	min_ground_ਰਫਤਾਰ = 5.0f;

	// WHEN: we update bearing and airspeed magnitude augmentation
	min_airspeed_for_bearing = ਕੋਰਸ_ਮੰਜ਼ਿਲ_airspeed.getMinAirspeedForCurrentBearing(bearing, wind_vel,
				   airspeed_max, min_ground_ਰਫਤਾਰ);

	airspeed_setpoint_adapted = math::constrain(airspeed_setpoint, min_airspeed_for_bearing, airspeed_max);

	heading_setpoint = ਮੈਟ੍ਰਿਕਸ::wrap_pi(ਕੋਰਸ_ਮੰਜ਼ਿਲ_airspeed.map_ਕੋਰਸ_Setpoint_ਮੰਜ਼ਿਲ_Heading_Setpoint(bearing, wind_vel,
					   airspeed_setpoint_adapted));


	// THEN: expect heading due East with a min airspeed equal to min_ground_speed
	EXPECT_NEAR(heading_setpoint, M_PI_2_F,  0.01f);
	EXPECT_NEAR(min_airspeed_for_bearing, min_ground_ਰਫਤਾਰ, FLT_EPSILON);
}

TEST(NpfgTest, LightCrossWind)
{
	ਕੋਰਸ_ਮੰਜ਼ਿਲ_Airspeed_Ref_Mapper ਕੋਰਸ_ਮੰਜ਼ਿਲ_airspeed;

	// GIVEN
	ਅਚਲ Vector2f wind_vel(0.f, 6.f);
	ਦਸ਼ਮਲਵ bearing = 0.f;
	ਦਸ਼ਮਲਵ airspeed_max = 20.f;
	ਦਸ਼ਮਲਵ min_ground_ਰਫਤਾਰ = 5.0f;
	ਦਸ਼ਮਲਵ airspeed_setpoint = 15.f;

	// WHEN: we update bearing and airspeed magnitude augmentation
	ਦਸ਼ਮਲਵ min_airspeed_for_bearing = ਕੋਰਸ_ਮੰਜ਼ਿਲ_airspeed.getMinAirspeedForCurrentBearing(bearing, wind_vel,
					 airspeed_max, min_ground_ਰਫਤਾਰ);

	ਦਸ਼ਮਲਵ airspeed_setpoint_adapted = math::constrain(airspeed_setpoint, min_airspeed_for_bearing, airspeed_max);

	ਦਸ਼ਮਲਵ heading_setpoint = ਮੈਟ੍ਰਿਕਸ::wrap_pi(ਕੋਰਸ_ਮੰਜ਼ਿਲ_airspeed.map_ਕੋਰਸ_Setpoint_ਮੰਜ਼ਿਲ_Heading_Setpoint(bearing, wind_vel,
				 airspeed_setpoint_adapted));

	// THEN: expect heading -0.4115168 with a min airspeed of to 7.8 (sqrt(25+36))
	EXPECT_NEAR(heading_setpoint, -0.4115168,  0.01f);
	EXPECT_NEAR(min_airspeed_for_bearing, 7.8f, 0.1f);

	// GIVEN: bearing due South
	bearing = M_PI_F;
	airspeed_max = 20.f;
	min_ground_ਰਫਤਾਰ = 5.0f;

	// WHEN: we update bearing and airspeed magnitude augmentation
	min_airspeed_for_bearing = ਕੋਰਸ_ਮੰਜ਼ਿਲ_airspeed.getMinAirspeedForCurrentBearing(bearing, wind_vel,
				   airspeed_max, min_ground_ਰਫਤਾਰ);

	airspeed_setpoint_adapted = math::constrain(airspeed_setpoint, min_airspeed_for_bearing, airspeed_max);

	heading_setpoint = ਮੈਟ੍ਰਿਕਸ::wrap_pi(ਕੋਰਸ_ਮੰਜ਼ਿਲ_airspeed.map_ਕੋਰਸ_Setpoint_ਮੰਜ਼ਿਲ_Heading_Setpoint(bearing, wind_vel,
					   airspeed_setpoint_adapted));

	// THEN: expect heading of -2.73 and a min airspeed of 7.8 (sqrt(25+36))
	EXPECT_NEAR(heading_setpoint, -2.73f,  0.01f);
	EXPECT_NEAR(min_airspeed_for_bearing, 7.8f, 0.1f);
}

TEST(NpfgTest, Strong_ਸਿਰ_Wind)
{
	ਕੋਰਸ_ਮੰਜ਼ਿਲ_Airspeed_Ref_Mapper ਕੋਰਸ_ਮੰਜ਼ਿਲ_airspeed;

	// GIVEN: bearing due North and wind from the North
	ਅਚਲ Vector2f wind_vel(-16.f, 0.f);
	ਦਸ਼ਮਲਵ bearing = 0.f;
	ਦਸ਼ਮਲਵ airspeed_max = 25.f;
	ਦਸ਼ਮਲਵ min_ground_ਰਫਤਾਰ = 5.0f;
	ਦਸ਼ਮਲਵ airspeed_setpoint = 15.f;

	// WHEN: we update bearing and airspeed magnitude augmentation
	ਦਸ਼ਮਲਵ min_airspeed_for_bearing = ਕੋਰਸ_ਮੰਜ਼ਿਲ_airspeed.getMinAirspeedForCurrentBearing(bearing, wind_vel,
					 airspeed_max, min_ground_ਰਫਤਾਰ);

	ਦਸ਼ਮਲਵ airspeed_setpoint_adapted = math::constrain(airspeed_setpoint, min_airspeed_for_bearing, airspeed_max);

	ਦਸ਼ਮਲਵ heading_setpoint = ਮੈਟ੍ਰਿਕਸ::wrap_pi(ਕੋਰਸ_ਮੰਜ਼ਿਲ_airspeed.map_ਕੋਰਸ_Setpoint_ਮੰਜ਼ਿਲ_Heading_Setpoint(bearing, wind_vel,
				 airspeed_setpoint_adapted));

	// THEN: expect heading due North with a min airspeed equal to 16+min_ground_speed
	EXPECT_NEAR(heading_setpoint, 0.f, 0.01f);
	EXPECT_NEAR(min_airspeed_for_bearing, 16 + min_ground_ਰਫਤਾਰ, 0.1f);
}

TEST(NpfgTest, Strong_ਪੂਛ_Wind)
{
	ਕੋਰਸ_ਮੰਜ਼ਿਲ_Airspeed_Ref_Mapper ਕੋਰਸ_ਮੰਜ਼ਿਲ_airspeed;

	// GIVEN: bearing due East and wind from the West
	ਅਚਲ Vector2f wind_vel(0.f, 16.f);
	ਦਸ਼ਮਲਵ bearing = M_PI_2_F;
	ਦਸ਼ਮਲਵ airspeed_max = 25.f;
	ਦਸ਼ਮਲਵ min_ground_ਰਫਤਾਰ = 5.0f;
	ਦਸ਼ਮਲਵ airspeed_setpoint = 15.f;

	// WHEN: we update bearing and airspeed magnitude augmentation
	ਦਸ਼ਮਲਵ min_airspeed_for_bearing = ਕੋਰਸ_ਮੰਜ਼ਿਲ_airspeed.getMinAirspeedForCurrentBearing(bearing, wind_vel,
					 airspeed_max, min_ground_ਰਫਤਾਰ);

	ਦਸ਼ਮਲਵ airspeed_setpoint_adapted = math::constrain(airspeed_setpoint, min_airspeed_for_bearing, airspeed_max);

	ਦਸ਼ਮਲਵ heading_setpoint = ਮੈਟ੍ਰਿਕਸ::wrap_pi(ਕੋਰਸ_ਮੰਜ਼ਿਲ_airspeed.map_ਕੋਰਸ_Setpoint_ਮੰਜ਼ਿਲ_Heading_Setpoint(bearing, wind_vel,
				 airspeed_setpoint_adapted));

	// THEN: expect heading due East with a min airspeed at 0
	EXPECT_NEAR(heading_setpoint, M_PI_2_F, 0.01f);
	EXPECT_NEAR(min_airspeed_for_bearing, 0.f, 0.1f);
}

TEST(NpfgTest, Excess_ਸਿਰ_Wind)
{
	// TEST DESCRIPTION: infeasible bearing, with |wind| = |airspeed|. Align with wind

	ਕੋਰਸ_ਮੰਜ਼ਿਲ_Airspeed_Ref_Mapper ਕੋਰਸ_ਮੰਜ਼ਿਲ_airspeed;

	// GIVEN: bearing due North and wind from the North
	ਅਚਲ Vector2f wind_vel(-25.f, 0.f);
	ਦਸ਼ਮਲਵ bearing = 0.f;
	ਦਸ਼ਮਲਵ airspeed_max = 25.f;
	ਦਸ਼ਮਲਵ min_ground_ਰਫਤਾਰ = 5.0f;
	ਦਸ਼ਮਲਵ airspeed_setpoint = 15.f;

	// WHEN: we update bearing and airspeed magnitude augmentation
	ਦਸ਼ਮਲਵ min_airspeed_for_bearing = ਕੋਰਸ_ਮੰਜ਼ਿਲ_airspeed.getMinAirspeedForCurrentBearing(bearing, wind_vel,
					 airspeed_max, min_ground_ਰਫਤਾਰ);

	ਦਸ਼ਮਲਵ airspeed_setpoint_adapted = math::constrain(airspeed_setpoint, min_airspeed_for_bearing, airspeed_max);

	ਦਸ਼ਮਲਵ heading_setpoint = ਮੈਟ੍ਰਿਕਸ::wrap_pi(ਕੋਰਸ_ਮੰਜ਼ਿਲ_airspeed.map_ਕੋਰਸ_Setpoint_ਮੰਜ਼ਿਲ_Heading_Setpoint(bearing, wind_vel,
				 airspeed_setpoint_adapted));

	// THEN: expect heading sp due North with a min airspeed equal to airspeed_max
	EXPECT_NEAR(heading_setpoint, 0.f, 0.01f);
	EXPECT_NEAR(min_airspeed_for_bearing, airspeed_max, 0.1f);

	// WHEN: we increase the maximum airspeed
	airspeed_max = 35.f;

	min_airspeed_for_bearing = ਕੋਰਸ_ਮੰਜ਼ਿਲ_airspeed.getMinAirspeedForCurrentBearing(bearing, wind_vel,
				   airspeed_max, min_ground_ਰਫਤਾਰ);

	// THEN: expect the minimum airspeed to be high enough to maintain minimum groundspeed
	EXPECT_NEAR(min_airspeed_for_bearing, 30.f, 0.1f);

	// TEST DESCRIPTION: infeasible bearing, with |wind| = |airspeed|. Align with wind

	// GIVEN: bearing east
	bearing = M_PI_F / 2.f;
	airspeed_max = 25.f;

	// WHEN: we update bearing and airspeed magnitude augmentation
	min_airspeed_for_bearing = ਕੋਰਸ_ਮੰਜ਼ਿਲ_airspeed.getMinAirspeedForCurrentBearing(bearing, wind_vel,
				   airspeed_max, min_ground_ਰਫਤਾਰ);

	airspeed_setpoint_adapted = math::constrain(airspeed_setpoint, min_airspeed_for_bearing, airspeed_max);

	heading_setpoint = ਮੈਟ੍ਰਿਕਸ::wrap_pi(ਕੋਰਸ_ਮੰਜ਼ਿਲ_airspeed.map_ਕੋਰਸ_Setpoint_ਮੰਜ਼ਿਲ_Heading_Setpoint(bearing, wind_vel,
					   airspeed_setpoint_adapted));

	// THEN: expect heading sp due North with a min airspeed equal to airspeed_max
	EXPECT_NEAR(heading_setpoint, 0.f, 0.01f);
	EXPECT_NEAR(min_airspeed_for_bearing, airspeed_max, 0.1f);

	// TEST DESCRIPTION: infeasible bearing, with |wind| > |airspeed|. Aircraft should have a heading between the target bearing
	// and wind direction to minimize drift while still attempting to reach the bearing.

	// GIVEN: bearing NE
	bearing = M_PI_F / 4.f;
	airspeed_max = 20.f;

	// WHEN: we update bearing and airspeed magnitude augmentation
	min_airspeed_for_bearing = ਕੋਰਸ_ਮੰਜ਼ਿਲ_airspeed.getMinAirspeedForCurrentBearing(bearing, wind_vel,
				   airspeed_max, min_ground_ਰਫਤਾਰ);

	airspeed_setpoint_adapted = math::constrain(airspeed_setpoint, min_airspeed_for_bearing, airspeed_max);

	heading_setpoint = ਮੈਟ੍ਰਿਕਸ::wrap_pi(ਕੋਰਸ_ਮੰਜ਼ਿਲ_airspeed.map_ਕੋਰਸ_Setpoint_ਮੰਜ਼ਿਲ_Heading_Setpoint(bearing, wind_vel,
					   airspeed_setpoint_adapted));

	// THEN: expect heading setpoint to be between the target bearing and the cross wind
	// & the minimum airspeed to be = maximum airspeed
	EXPECT_TRUE((heading_setpoint > -M_PI_F / 2.f) && (heading_setpoint < bearing));
	EXPECT_NEAR(min_airspeed_for_bearing, airspeed_max, 0.1f);
}

TEST(NpfgTest, Excess_ਪੂਛ_Wind)
{
	ਕੋਰਸ_ਮੰਜ਼ਿਲ_Airspeed_Ref_Mapper ਕੋਰਸ_ਮੰਜ਼ਿਲ_airspeed;

	// GIVEN: bearing due East and wind from the West
	ਅਚਲ Vector2f wind_vel(0.f, 25.f);
	ਦਸ਼ਮਲਵ bearing = M_PI_2_F;
	ਦਸ਼ਮਲਵ airspeed_max = 25.f;
	ਦਸ਼ਮਲਵ min_ground_ਰਫਤਾਰ = 5.0f;
	ਦਸ਼ਮਲਵ airspeed_setpoint = 15.f;

	// WHEN: we update bearing and airspeed magnitude augmentation
	ਅਚਲ ਦਸ਼ਮਲਵ min_airspeed_for_bearing = ਕੋਰਸ_ਮੰਜ਼ਿਲ_airspeed.getMinAirspeedForCurrentBearing(bearing, wind_vel,
					       airspeed_max, min_ground_ਰਫਤਾਰ);

	ਦਸ਼ਮਲਵ airspeed_setpoint_adapted = math::constrain(airspeed_setpoint, min_airspeed_for_bearing, airspeed_max);

	ਦਸ਼ਮਲਵ heading_setpoint = ਮੈਟ੍ਰਿਕਸ::wrap_pi(ਕੋਰਸ_ਮੰਜ਼ਿਲ_airspeed.map_ਕੋਰਸ_Setpoint_ਮੰਜ਼ਿਲ_Heading_Setpoint(bearing, wind_vel,
				 airspeed_setpoint_adapted));

	// THEN: expect heading due East with a min airspeed equal to 0
	EXPECT_NEAR(heading_setpoint, M_PI_2_F, 0.01f);
	EXPECT_NEAR(min_airspeed_for_bearing, 0.f, 0.1f);
}

TEST(NpfgTest, ExcessCrossWind)
{
	// TEST DESCRIPTION: infeasible bearing, with |wind| > |airspeed|. Aircraft should have a heading between the target bearing
	// and wind direction to minimize drift while still attempting to reach the bearing.

	ਕੋਰਸ_ਮੰਜ਼ਿਲ_Airspeed_Ref_Mapper ਕੋਰਸ_ਮੰਜ਼ਿਲ_airspeed;

	// GIVEN: bearing due North, strong wind due East
	ਅਚਲ Vector2f wind_vel(0, 30.f);
	ਦਸ਼ਮਲਵ bearing = 0.f;
	ਦਸ਼ਮਲਵ airspeed_max = 25.f;
	ਦਸ਼ਮਲਵ min_ground_ਰਫਤਾਰ = 5.f;
	ਦਸ਼ਮਲਵ airspeed_setpoint = 15.f;

	// WHEN: we update bearing and airspeed magnitude augmentation
	ਦਸ਼ਮਲਵ min_airspeed_for_bearing = ਕੋਰਸ_ਮੰਜ਼ਿਲ_airspeed.getMinAirspeedForCurrentBearing(bearing, wind_vel,
					 airspeed_max, min_ground_ਰਫਤਾਰ);

	ਦਸ਼ਮਲਵ airspeed_setpoint_adapted = math::constrain(airspeed_setpoint, min_airspeed_for_bearing, airspeed_max);

	ਦਸ਼ਮਲਵ heading_setpoint = ਮੈਟ੍ਰਿਕਸ::wrap_pi(ਕੋਰਸ_ਮੰਜ਼ਿਲ_airspeed.map_ਕੋਰਸ_Setpoint_ਮੰਜ਼ਿਲ_Heading_Setpoint(bearing, wind_vel,
				 airspeed_setpoint_adapted));

	// THEN: expect heading setpoint to be between the target bearing and the cross wind
	// & the minimum airspeed to be = maximum airspeed
	EXPECT_TRUE((heading_setpoint > -M_PI_F / 2.f) && (heading_setpoint < bearing));
	EXPECT_NEAR(min_airspeed_for_bearing, airspeed_max, 0.1f);

	// TEST DESCRIPTION: infeasible bearing, with |wind| = |airspeed|. Align with wind.

	airspeed_max = 30.f;

	min_airspeed_for_bearing = ਕੋਰਸ_ਮੰਜ਼ਿਲ_airspeed.getMinAirspeedForCurrentBearing(bearing, wind_vel,
				   airspeed_max, min_ground_ਰਫਤਾਰ);

	airspeed_setpoint_adapted = math::constrain(airspeed_setpoint, min_airspeed_for_bearing, airspeed_max);

	heading_setpoint = ਮੈਟ੍ਰਿਕਸ::wrap_pi(ਕੋਰਸ_ਮੰਜ਼ਿਲ_airspeed.map_ਕੋਰਸ_Setpoint_ਮੰਜ਼ਿਲ_Heading_Setpoint(bearing, wind_vel,
					   airspeed_setpoint_adapted));

	EXPECT_NEAR(heading_setpoint, -M_PI_F / 2.f, 0.01f);
	EXPECT_NEAR(min_airspeed_for_bearing, airspeed_max, 0.1f);
}

TEST(NpfgTest, HeadingControl)
{
	AirspeedDirectionController _airspeed_reference_controller;
	ਅਚਲ ਦਸ਼ਮਲਵ p_gain = 0.8885f;

	// GIVEN: that we are already aligned with out heading setpoint
	ਦਸ਼ਮਲਵ heading_sp = 0.f;
	ਦਸ਼ਮਲਵ heading = 0.f;
	ਦਸ਼ਮਲਵ airspeed = 15.f;

	// WHEN: we compute the lateral acceleration setpoint
	ਦਸ਼ਮਲਵ lateral_acceleration_setpoint = _airspeed_reference_controller.controlHeading(heading_sp, heading, airspeed);

	// THEN: we expect 0 lateral acceleration
	EXPECT_NEAR(lateral_acceleration_setpoint, 0.f, 0.01f);

	// GIVEN: current heading 45 deg NW
	heading = - M_PI_F / 4.f;

	// WHEN: we compute the lateral acceleration setpoint
	lateral_acceleration_setpoint = _airspeed_reference_controller.controlHeading(heading_sp, heading, airspeed);

	// THEN: we expect a positive lateral acceleration input.  = Airspeed vector
	// required to correct the difference between the setpoint and the current heading,
	// scaled by p_gain.
	EXPECT_NEAR(lateral_acceleration_setpoint, airspeed * sinf(heading_sp - heading) * p_gain, 0.01f);

	// GIVEN: current heading 180 (South)
	heading = M_PI_F;

	// WHEN: we compute the lateral acceleration setpoint
	lateral_acceleration_setpoint = _airspeed_reference_controller.controlHeading(heading_sp, heading, airspeed);

	// THEN: we we expect maxmimum lateral acceleration setpoint
	EXPECT_NEAR(lateral_acceleration_setpoint, airspeed * p_gain, 0.01f);
}
