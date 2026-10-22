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

#شامل <gtest/gtest.h>
#شامل <lib/pure_pursuit/PurePursuit.hpp>
#شامل <uORB/topics/pure_pursuit_status.h>

استعمال نامخلا میٹرکس;

TEST(خالص_Pursuit_Test, Invalid_رفتار)
{
	//      V   C
	//         /
	//  	  /
	//	 /
	//	P
	خالص_pursuit_حالت_s خالص_pursuit{};
	ثابت Vector2f curr_wp_ned(10.f, 10.f);
	ثابت Vector2f پچھلا_wp_ned(0.f, 0.f);
	ثابت Vector2f curr_pos_ned(10.f, 0.f);
	// NaN speed
	ثابت اعشاری ہدف_bearing1 = خالص_Pursuit::calc_ہدف_Bearing(خالص_pursuit, 1.f, 10.f, 1.f, curr_wp_ned, پچھلا_wp_ned,
				      curr_pos_ned, NAN);
	EXPECT_FALSE(PX4_ISFINITE(ہدف_bearing1));
}

TEST(خالص_Pursuit_Test, InvalidWaypoints)
{
	//	V   C
	//         /
	//  	  /
	//	 /
	//	P
	خالص_pursuit_حالت_s خالص_pursuit{};
	ثابت Vector2f curr_wp_ned(10.f, 10.f);
	ثابت Vector2f پچھلا_wp_ned(0.f, 0.f);
	ثابت Vector2f curr_pos_ned(10.f, 0.f);
	ثابت اعشاری lookahead_فاصلہ{5.f};
	// Prev WP is NAN
	ثابت اعشاری ہدف_bearing1 = خالص_Pursuit::calc_ہدف_Bearing(خالص_pursuit, 1.f, 10.f, 1.f, curr_wp_ned, Vector2f(NAN,
				      NAN), curr_pos_ned,
				      lookahead_فاصلہ);
	// Curr WP is NAN
	ثابت اعشاری ہدف_bearing2 = خالص_Pursuit::calc_ہدف_Bearing(خالص_pursuit, 1.f, 10.f, 1.f, Vector2f(NAN, NAN),
				      پچھلا_wp_ned, curr_pos_ned,
				      lookahead_فاصلہ);

	// Curr Pos is NAN
	ثابت اعشاری ہدف_bearing3 = خالص_Pursuit::calc_ہدف_Bearing(خالص_pursuit, 1.f, 10.f, 1.f, curr_wp_ned, پچھلا_wp_ned,
				      Vector2f(NAN, NAN), lookahead_فاصلہ);
	EXPECT_FALSE(PX4_ISFINITE(ہدف_bearing1));
	EXPECT_FALSE(PX4_ISFINITE(ہدف_bearing2));
	EXPECT_FALSE(PX4_ISFINITE(ہدف_bearing3));
}

TEST(خالص_Pursuit_Test, برآمد_کا_Lookahead)
{
	خالص_pursuit_حالت_s خالص_pursuit{};
	ثابت اعشاری lookahead_فاصلہ{5.f};
	//	V   C
	//         /
	//  	  /
	//	 /
	//	P
	ثابت اعشاری ہدف_bearing1 = خالص_Pursuit::calc_ہدف_Bearing(خالص_pursuit, 1.f, 10.f, 1.f, Vector2f(10.f, 10.f),
				      Vector2f(0.f, 0.f), Vector2f(10.f, 0.f), lookahead_فاصلہ);
	//	    V
	//
	//	P ----- C
	ثابت اعشاری ہدف_bearing2 = خالص_Pursuit::calc_ہدف_Bearing(خالص_pursuit, 1.f, 10.f, 1.f, Vector2f(0.f, 20.f),
				      Vector2f(0.f, 0.f), Vector2f(10.f, 10.f), lookahead_فاصلہ);
	// V
	//
	//     P ------ C
	ثابت اعشاری ہدف_bearing3 = خالص_Pursuit::calc_ہدف_Bearing(خالص_pursuit, 1.f, 10.f, 1.f, Vector2f(0.f, 20.f),
				      Vector2f(0.f, 10.f), Vector2f(10.f, 0.f), lookahead_فاصلہ);
	//		V
	//
	// P ------ C
	ثابت اعشاری ہدف_bearing4 = خالص_Pursuit::calc_ہدف_Bearing(خالص_pursuit, 1.f, 10.f, 1.f, Vector2f(0.f, 10.f),
				      Vector2f(0.f, 0.f), Vector2f(10.f, 20.f), lookahead_فاصلہ);

	EXPECT_NEAR(ہدف_bearing1, M_PI_2_F + M_PI_4_F, FLT_EPSILON); // Fallback: Bearing to closest point on path
	EXPECT_NEAR(fabsf(ہدف_bearing2), M_PI_F, 1e-6f); 	// Fallback: Bearing to closest point on path, +-pi is the same bearing
	EXPECT_NEAR(ہدف_bearing3, M_PI_F - atan2f(10, 10), FLT_EPSILON); // Fallback: Bearing to previous waypoint
	EXPECT_NEAR(ہدف_bearing4, -M_PI_F + atan2f(10, 10), FLT_EPSILON); // Fallback: Bearing to current waypoint
}

TEST(خالص_Pursuit_Test, WaypointOverlap)
{
	خالص_pursuit_حالت_s خالص_pursuit{};
	ثابت اعشاری lookahead_فاصلہ{5.f};
	//	    C/P
	//
	//
	//
	//	V
	ثابت اعشاری ہدف_bearing1 = خالص_Pursuit::calc_ہدف_Bearing(خالص_pursuit, 1.f, 10.f, 1.f, Vector2f(10.f, 10.f),
				      Vector2f(10.f, 10.f), Vector2f(0.f, 0.f), lookahead_فاصلہ);
	//	    V
	//
	//
	//
	//	C/P
	ثابت اعشاری ہدف_bearing2 = خالص_Pursuit::calc_ہدف_Bearing(خالص_pursuit, 1.f, 10.f, 1.f, Vector2f(0.f, 0.f),
				      Vector2f(0.f, 0.f), Vector2f(10.f, 10.f), lookahead_فاصلہ);
	EXPECT_NEAR(ہدف_bearing1, M_PI_4_F, FLT_EPSILON); 		   // Fallback: Bearing to closest point on path
	EXPECT_NEAR(ہدف_bearing2, -(M_PI_4_F + M_PI_2_F), FLT_EPSILON); // Fallback: Bearing to closest point on path
}

TEST(خالص_Pursuit_Test, Curr_And_پچھلا_Same_North_Coordinate)
{
	خالص_pursuit_حالت_s خالص_pursuit{};
	ثابت اعشاری lookahead_فاصلہ{5.f};
	//	P -- V -- C
	ثابت اعشاری ہدف_bearing1 = خالص_Pursuit::calc_ہدف_Bearing(خالص_pursuit, 1.f, 10.f, 1.f, Vector2f(0.f, 20.f),
				      Vector2f(0.f, 0.f), Vector2f(0.f, 10.f), lookahead_فاصلہ);

	//	     V
	//	P ------ C
	ثابت اعشاری ہدف_bearing2 = خالص_Pursuit::calc_ہدف_Bearing(خالص_pursuit, 1.f, 10.f, 1.f, Vector2f(0.f, 20.f),
				      Vector2f(0.f, 0.f), Vector2f(5.f / sqrtf(2.f), 10.f), lookahead_فاصلہ);
	//	     V
	//	C ------ P
	ثابت اعشاری ہدف_bearing3 = خالص_Pursuit::calc_ہدف_Bearing(خالص_pursuit, 1.f, 10.f, 1.f, Vector2f(0.f, 0.f),
				      Vector2f(0.f, 20.f), Vector2f(5.f / sqrtf(2.f), 10.f), lookahead_فاصلہ);
	//	     V
	//
	//	P ------ C
	ثابت اعشاری ہدف_bearing4 = خالص_Pursuit::calc_ہدف_Bearing(خالص_pursuit, 1.f, 10.f, 1.f, Vector2f(0.f, 20.f),
				      Vector2f(0.f, 0.f), Vector2f(10.f, 10.f), lookahead_فاصلہ);

	EXPECT_NEAR(ہدف_bearing1, M_PI_2_F, FLT_EPSILON);
	EXPECT_NEAR(ہدف_bearing2, M_PI_2_F + M_PI_4_F, FLT_EPSILON);
	EXPECT_NEAR(ہدف_bearing3, -(M_PI_2_F + M_PI_4_F), FLT_EPSILON);
	EXPECT_NEAR(fabsf(ہدف_bearing4), M_PI_F, 1e-6f); // Fallback: Bearing to closest point on path, +-pi is the same bearing
}

TEST(خالص_Pursuit_Test, VehiclePastCurrentWaypoint)
{
	خالص_pursuit_حالت_s خالص_pursuit{};
	ثابت اعشاری lookahead_فاصلہ{5.f};
	// The vehicle has passed the current waypoint by more than the lookahead and sits on the extended path.
	// The lookahead intersection would lie even further along the extension; the waypoint has to be targeted instead.
	//	P ------ C ------ V
	ثابت اعشاری ہدف_bearing1 = خالص_Pursuit::calc_ہدف_Bearing(خالص_pursuit, 1.f, 10.f, 1.f, Vector2f(0.f, 10.f),
				      Vector2f(0.f, 0.f), Vector2f(0.f, 20.f), lookahead_فاصلہ);
	// Off the extended path, but within the lookahead corridor
	//		   V
	//	P ------ C
	ثابت اعشاری ہدف_bearing2 = خالص_Pursuit::calc_ہدف_Bearing(خالص_pursuit, 1.f, 10.f, 1.f, Vector2f(0.f, 10.f),
				      Vector2f(0.f, 0.f), Vector2f(2.f, 20.f), lookahead_فاصلہ);
	// Still ahead of the current waypoint: regular pure pursuit is unaffected
	//	P -- V -- C
	ثابت اعشاری ہدف_bearing3 = خالص_Pursuit::calc_ہدف_Bearing(خالص_pursuit, 1.f, 10.f, 1.f, Vector2f(0.f, 10.f),
				      Vector2f(0.f, 0.f), Vector2f(0.f, 2.f), lookahead_فاصلہ);

	EXPECT_NEAR(ہدف_bearing1, -M_PI_2_F, FLT_EPSILON);			 // Back to the current waypoint
	EXPECT_NEAR(ہدف_bearing2, atan2f(-10.f, -2.f), FLT_EPSILON); // Back to the current waypoint
	EXPECT_NEAR(ہدف_bearing3, M_PI_2_F, FLT_EPSILON);			 // Along the path
}

TEST(خالص_Pursuit_Test, CrosstrackError)
{
	خالص_pursuit_حالت_s خالص_pursuit{};
	ثابت اعشاری lookahead_فاصلہ{5.f};
	//	    V
	//
	//	P ----- C
	خالص_Pursuit::calc_ہدف_Bearing(خالص_pursuit, 1.f, 10.f, 1.f, Vector2f(0.f, 20.f),
				       Vector2f(0.f, 0.f), Vector2f(10.f, 10.f), lookahead_فاصلہ);
	ثابت اعشاری crosstrack1 = خالص_pursuit.crosstrack_error;

	//	    V
	//	P ----- C
	خالص_Pursuit::calc_ہدف_Bearing(خالص_pursuit, 1.f, 10.f, 1.f, Vector2f(0.f, 20.f),
				       Vector2f(0.f, 0.f), Vector2f(5.f, 10.f), lookahead_فاصلہ);
	ثابت اعشاری crosstrack2 = خالص_pursuit.crosstrack_error;

	//	P ----- C
	//
	//	    V
	خالص_Pursuit::calc_ہدف_Bearing(خالص_pursuit, 1.f, 10.f, 1.f, Vector2f(0.f, 20.f),
				       Vector2f(0.f, 0.f), Vector2f(-10.f, 10.f), lookahead_فاصلہ);
	ثابت اعشاری crosstrack3 = خالص_pursuit.crosstrack_error;

	//	P ----- C
	//	    V
	خالص_Pursuit::calc_ہدف_Bearing(خالص_pursuit, 1.f, 10.f, 1.f, Vector2f(0.f, 20.f),
				       Vector2f(0.f, 0.f), Vector2f(-5.f, 10.f), lookahead_فاصلہ);
	ثابت اعشاری crosstrack4 = خالص_pursuit.crosstrack_error;

	EXPECT_NEAR(crosstrack1, -10.f, FLT_EPSILON);
	EXPECT_NEAR(crosstrack2, -5.f, FLT_EPSILON);
	EXPECT_NEAR(crosstrack3, 10.f, FLT_EPSILON);
	EXPECT_NEAR(crosstrack4, 5.f, FLT_EPSILON);

}
