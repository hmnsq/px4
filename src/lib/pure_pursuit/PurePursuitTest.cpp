/****************************************************************************
 *
 *   Copyright (C) 2024 PX4 Development Team. All rights reserved.
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
 * Test code for the Pure Pursuit algorithm
 * Run this test only using "make tests TESTFILTER=PurePursuit"
 *
 * Graphic interpretation:
 * 	Legend:
 * 		C: Current waypoint
 * 		P: Previous waypoint
 * 		V: Vehicle
 * 		|: Line segment
 * 	Orientation:
 * 							C
 * 		  				       /
 * 						    __/__
 * 						  /  /    \
 * 						 /  /      \
 * 						|  /  V     |
 * 						 \/        /
 * 						 /\ _____ /
 * 	         	    N (0 rad)		/
 * 			        ^	       P
 * 				|
 * 				| D
 *  	   (-1.5708 rad) <----- ⨂ -----> E (1.5708 rad)
 * 				|
 * 				|
 * 				⌄
 * 			(+- 3.14159 rad)
 *
 * NOTE:
 * 	The tuning parameters for the pure pursuit algorithm are set to the following for all tests:
 * 	   PP_LOOKAHD_GAIN = 1.f
 * 	   PP_LOOKAHD_MAX = 10.f
 * 	   PP_LOOKAHD_MIN = 1.f
 * 	This way passing the vehicle_speed in calcDesiredHeading function is equivalent to passing
 * 	the lookahead distance.
 *
******************************************************************/

#అన్తర్భావ <gtest/gtest.h>
#అన్తర్భావ <lib/pure_pursuit/PurePursuit.hpp>
#అన్తర్భావ <uORB/topics/pure_pursuit_status.h>

ఉపయోగించి నామాకాశ మాతృక;

TEST(శుద్ధ_Pursuit_Test, Invalid_వేగం)
{
	//      V   C
	//         /
	//  	  /
	//	 /
	//	P
	శుద్ధ_pursuit_స్థితి_s శుద్ధ_pursuit{};
	స్థిరరాశి Vector2f curr_wp_ned(10.f, 10.f);
	స్థిరరాశి Vector2f prev_wp_ned(0.f, 0.f);
	స్థిరరాశి Vector2f curr_pos_ned(10.f, 0.f);
	// NaN speed
	స్థిరరాశి దశాంశం లక్ష్యం_bearing1 = శుద్ధ_Pursuit::calc_లక్ష్యం_Bearing(శుద్ధ_pursuit, 1.f, 10.f, 1.f, curr_wp_ned, prev_wp_ned,
				      curr_pos_ned, NAN);
	EXPECT_FALSE(PX4_ISFINITE(లక్ష్యం_bearing1));
}

TEST(శుద్ధ_Pursuit_Test, InvalidWaypoints)
{
	//	V   C
	//         /
	//  	  /
	//	 /
	//	P
	శుద్ధ_pursuit_స్థితి_s శుద్ధ_pursuit{};
	స్థిరరాశి Vector2f curr_wp_ned(10.f, 10.f);
	స్థిరరాశి Vector2f prev_wp_ned(0.f, 0.f);
	స్థిరరాశి Vector2f curr_pos_ned(10.f, 0.f);
	స్థిరరాశి దశాంశం lookahead_దూరం{5.f};
	// Prev WP is NAN
	స్థిరరాశి దశాంశం లక్ష్యం_bearing1 = శుద్ధ_Pursuit::calc_లక్ష్యం_Bearing(శుద్ధ_pursuit, 1.f, 10.f, 1.f, curr_wp_ned, Vector2f(NAN,
				      NAN), curr_pos_ned,
				      lookahead_దూరం);
	// Curr WP is NAN
	స్థిరరాశి దశాంశం లక్ష్యం_bearing2 = శుద్ధ_Pursuit::calc_లక్ష్యం_Bearing(శుద్ధ_pursuit, 1.f, 10.f, 1.f, Vector2f(NAN, NAN),
				      prev_wp_ned, curr_pos_ned,
				      lookahead_దూరం);

	// Curr Pos is NAN
	స్థిరరాశి దశాంశం లక్ష్యం_bearing3 = శుద్ధ_Pursuit::calc_లక్ష్యం_Bearing(శుద్ధ_pursuit, 1.f, 10.f, 1.f, curr_wp_ned, prev_wp_ned,
				      Vector2f(NAN, NAN), lookahead_దూరం);
	EXPECT_FALSE(PX4_ISFINITE(లక్ష్యం_bearing1));
	EXPECT_FALSE(PX4_ISFINITE(లక్ష్యం_bearing2));
	EXPECT_FALSE(PX4_ISFINITE(లక్ష్యం_bearing3));
}

TEST(శుద్ధ_Pursuit_Test, బహిర్గతం_యొక్క_Lookahead)
{
	శుద్ధ_pursuit_స్థితి_s శుద్ధ_pursuit{};
	స్థిరరాశి దశాంశం lookahead_దూరం{5.f};
	//	V   C
	//         /
	//  	  /
	//	 /
	//	P
	స్థిరరాశి దశాంశం లక్ష్యం_bearing1 = శుద్ధ_Pursuit::calc_లక్ష్యం_Bearing(శుద్ధ_pursuit, 1.f, 10.f, 1.f, Vector2f(10.f, 10.f),
				      Vector2f(0.f, 0.f), Vector2f(10.f, 0.f), lookahead_దూరం);
	//	    V
	//
	//	P ----- C
	స్థిరరాశి దశాంశం లక్ష్యం_bearing2 = శుద్ధ_Pursuit::calc_లక్ష్యం_Bearing(శుద్ధ_pursuit, 1.f, 10.f, 1.f, Vector2f(0.f, 20.f),
				      Vector2f(0.f, 0.f), Vector2f(10.f, 10.f), lookahead_దూరం);
	// V
	//
	//     P ------ C
	స్థిరరాశి దశాంశం లక్ష్యం_bearing3 = శుద్ధ_Pursuit::calc_లక్ష్యం_Bearing(శుద్ధ_pursuit, 1.f, 10.f, 1.f, Vector2f(0.f, 20.f),
				      Vector2f(0.f, 10.f), Vector2f(10.f, 0.f), lookahead_దూరం);
	//		V
	//
	// P ------ C
	స్థిరరాశి దశాంశం లక్ష్యం_bearing4 = శుద్ధ_Pursuit::calc_లక్ష్యం_Bearing(శుద్ధ_pursuit, 1.f, 10.f, 1.f, Vector2f(0.f, 10.f),
				      Vector2f(0.f, 0.f), Vector2f(10.f, 20.f), lookahead_దూరం);

	EXPECT_NEAR(లక్ష్యం_bearing1, M_PI_2_F + M_PI_4_F, FLT_EPSILON); // Fallback: Bearing to closest point on path
	EXPECT_NEAR(fabsf(లక్ష్యం_bearing2), M_PI_F, 1e-6f); 	// Fallback: Bearing to closest point on path, +-pi is the same bearing
	EXPECT_NEAR(లక్ష్యం_bearing3, M_PI_F - atan2f(10, 10), FLT_EPSILON); // Fallback: Bearing to previous waypoint
	EXPECT_NEAR(లక్ష్యం_bearing4, -M_PI_F + atan2f(10, 10), FLT_EPSILON); // Fallback: Bearing to current waypoint
}

TEST(శుద్ధ_Pursuit_Test, WaypointOverlap)
{
	శుద్ధ_pursuit_స్థితి_s శుద్ధ_pursuit{};
	స్థిరరాశి దశాంశం lookahead_దూరం{5.f};
	//	    C/P
	//
	//
	//
	//	V
	స్థిరరాశి దశాంశం లక్ష్యం_bearing1 = శుద్ధ_Pursuit::calc_లక్ష్యం_Bearing(శుద్ధ_pursuit, 1.f, 10.f, 1.f, Vector2f(10.f, 10.f),
				      Vector2f(10.f, 10.f), Vector2f(0.f, 0.f), lookahead_దూరం);
	//	    V
	//
	//
	//
	//	C/P
	స్థిరరాశి దశాంశం లక్ష్యం_bearing2 = శుద్ధ_Pursuit::calc_లక్ష్యం_Bearing(శుద్ధ_pursuit, 1.f, 10.f, 1.f, Vector2f(0.f, 0.f),
				      Vector2f(0.f, 0.f), Vector2f(10.f, 10.f), lookahead_దూరం);
	EXPECT_NEAR(లక్ష్యం_bearing1, M_PI_4_F, FLT_EPSILON); 		   // Fallback: Bearing to closest point on path
	EXPECT_NEAR(లక్ష్యం_bearing2, -(M_PI_4_F + M_PI_2_F), FLT_EPSILON); // Fallback: Bearing to closest point on path
}

TEST(శుద్ధ_Pursuit_Test, CurrAndPrevSameNorthCoordinate)
{
	శుద్ధ_pursuit_స్థితి_s శుద్ధ_pursuit{};
	స్థిరరాశి దశాంశం lookahead_దూరం{5.f};
	//	P -- V -- C
	స్థిరరాశి దశాంశం లక్ష్యం_bearing1 = శుద్ధ_Pursuit::calc_లక్ష్యం_Bearing(శుద్ధ_pursuit, 1.f, 10.f, 1.f, Vector2f(0.f, 20.f),
				      Vector2f(0.f, 0.f), Vector2f(0.f, 10.f), lookahead_దూరం);

	//	     V
	//	P ------ C
	స్థిరరాశి దశాంశం లక్ష్యం_bearing2 = శుద్ధ_Pursuit::calc_లక్ష్యం_Bearing(శుద్ధ_pursuit, 1.f, 10.f, 1.f, Vector2f(0.f, 20.f),
				      Vector2f(0.f, 0.f), Vector2f(5.f / sqrtf(2.f), 10.f), lookahead_దూరం);
	//	     V
	//	C ------ P
	స్థిరరాశి దశాంశం లక్ష్యం_bearing3 = శుద్ధ_Pursuit::calc_లక్ష్యం_Bearing(శుద్ధ_pursuit, 1.f, 10.f, 1.f, Vector2f(0.f, 0.f),
				      Vector2f(0.f, 20.f), Vector2f(5.f / sqrtf(2.f), 10.f), lookahead_దూరం);
	//	     V
	//
	//	P ------ C
	స్థిరరాశి దశాంశం లక్ష్యం_bearing4 = శుద్ధ_Pursuit::calc_లక్ష్యం_Bearing(శుద్ధ_pursuit, 1.f, 10.f, 1.f, Vector2f(0.f, 20.f),
				      Vector2f(0.f, 0.f), Vector2f(10.f, 10.f), lookahead_దూరం);

	EXPECT_NEAR(లక్ష్యం_bearing1, M_PI_2_F, FLT_EPSILON);
	EXPECT_NEAR(లక్ష్యం_bearing2, M_PI_2_F + M_PI_4_F, FLT_EPSILON);
	EXPECT_NEAR(లక్ష్యం_bearing3, -(M_PI_2_F + M_PI_4_F), FLT_EPSILON);
	EXPECT_NEAR(fabsf(లక్ష్యం_bearing4), M_PI_F, 1e-6f); // Fallback: Bearing to closest point on path, +-pi is the same bearing
}

TEST(శుద్ధ_Pursuit_Test, VehiclePastCurrentWaypoint)
{
	శుద్ధ_pursuit_స్థితి_s శుద్ధ_pursuit{};
	స్థిరరాశి దశాంశం lookahead_దూరం{5.f};
	// The vehicle has passed the current waypoint by more than the lookahead and sits on the extended path.
	// The lookahead intersection would lie even further along the extension; the waypoint has to be targeted instead.
	//	P ------ C ------ V
	స్థిరరాశి దశాంశం లక్ష్యం_bearing1 = శుద్ధ_Pursuit::calc_లక్ష్యం_Bearing(శుద్ధ_pursuit, 1.f, 10.f, 1.f, Vector2f(0.f, 10.f),
				      Vector2f(0.f, 0.f), Vector2f(0.f, 20.f), lookahead_దూరం);
	// Off the extended path, but within the lookahead corridor
	//		   V
	//	P ------ C
	స్థిరరాశి దశాంశం లక్ష్యం_bearing2 = శుద్ధ_Pursuit::calc_లక్ష్యం_Bearing(శుద్ధ_pursuit, 1.f, 10.f, 1.f, Vector2f(0.f, 10.f),
				      Vector2f(0.f, 0.f), Vector2f(2.f, 20.f), lookahead_దూరం);
	// Still ahead of the current waypoint: regular pure pursuit is unaffected
	//	P -- V -- C
	స్థిరరాశి దశాంశం లక్ష్యం_bearing3 = శుద్ధ_Pursuit::calc_లక్ష్యం_Bearing(శుద్ధ_pursuit, 1.f, 10.f, 1.f, Vector2f(0.f, 10.f),
				      Vector2f(0.f, 0.f), Vector2f(0.f, 2.f), lookahead_దూరం);

	EXPECT_NEAR(లక్ష్యం_bearing1, -M_PI_2_F, FLT_EPSILON);			 // Back to the current waypoint
	EXPECT_NEAR(లక్ష్యం_bearing2, atan2f(-10.f, -2.f), FLT_EPSILON); // Back to the current waypoint
	EXPECT_NEAR(లక్ష్యం_bearing3, M_PI_2_F, FLT_EPSILON);			 // Along the path
}

TEST(శుద్ధ_Pursuit_Test, CrosstrackError)
{
	శుద్ధ_pursuit_స్థితి_s శుద్ధ_pursuit{};
	స్థిరరాశి దశాంశం lookahead_దూరం{5.f};
	//	    V
	//
	//	P ----- C
	శుద్ధ_Pursuit::calc_లక్ష్యం_Bearing(శుద్ధ_pursuit, 1.f, 10.f, 1.f, Vector2f(0.f, 20.f),
				       Vector2f(0.f, 0.f), Vector2f(10.f, 10.f), lookahead_దూరం);
	స్థిరరాశి దశాంశం crosstrack1 = శుద్ధ_pursuit.crosstrack_error;

	//	    V
	//	P ----- C
	శుద్ధ_Pursuit::calc_లక్ష్యం_Bearing(శుద్ధ_pursuit, 1.f, 10.f, 1.f, Vector2f(0.f, 20.f),
				       Vector2f(0.f, 0.f), Vector2f(5.f, 10.f), lookahead_దూరం);
	స్థిరరాశి దశాంశం crosstrack2 = శుద్ధ_pursuit.crosstrack_error;

	//	P ----- C
	//
	//	    V
	శుద్ధ_Pursuit::calc_లక్ష్యం_Bearing(శుద్ధ_pursuit, 1.f, 10.f, 1.f, Vector2f(0.f, 20.f),
				       Vector2f(0.f, 0.f), Vector2f(-10.f, 10.f), lookahead_దూరం);
	స్థిరరాశి దశాంశం crosstrack3 = శుద్ధ_pursuit.crosstrack_error;

	//	P ----- C
	//	    V
	శుద్ధ_Pursuit::calc_లక్ష్యం_Bearing(శుద్ధ_pursuit, 1.f, 10.f, 1.f, Vector2f(0.f, 20.f),
				       Vector2f(0.f, 0.f), Vector2f(-5.f, 10.f), lookahead_దూరం);
	స్థిరరాశి దశాంశం crosstrack4 = శుద్ధ_pursuit.crosstrack_error;

	EXPECT_NEAR(crosstrack1, -10.f, FLT_EPSILON);
	EXPECT_NEAR(crosstrack2, -5.f, FLT_EPSILON);
	EXPECT_NEAR(crosstrack3, 10.f, FLT_EPSILON);
	EXPECT_NEAR(crosstrack4, 5.f, FLT_EPSILON);

}
