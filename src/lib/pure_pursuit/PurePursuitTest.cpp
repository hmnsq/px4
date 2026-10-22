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

#ਅਨ੍ਤਰ੍ਭਾਵ <gtest/gtest.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <lib/pure_pursuit/PurePursuit.hpp>
#ਅਨ੍ਤਰ੍ਭਾਵ <uORB/topics/pure_pursuit_status.h>

ਵਰਤੋਂ ਨਾਮਾਕਾਸ਼ ਮੈਟ੍ਰਿਕਸ;

TEST(ਸ਼ੁੱਧ_Pursuit_Test, Invalid_ਰਫਤਾਰ)
{
	//      V   C
	//         /
	//  	  /
	//	 /
	//	P
	ਸ਼ੁੱਧ_pursuit_ਸਥਿਤੀ_s ਸ਼ੁੱਧ_pursuit{};
	ਅਚਲ Vector2f curr_wp_ned(10.f, 10.f);
	ਅਚਲ Vector2f prev_wp_ned(0.f, 0.f);
	ਅਚਲ Vector2f curr_pos_ned(10.f, 0.f);
	// NaN speed
	ਅਚਲ ਦਸ਼ਮਲਵ ਨਿਸ਼ਾਨਾ_bearing1 = ਸ਼ੁੱਧ_Pursuit::calc_ਨਿਸ਼ਾਨਾ_Bearing(ਸ਼ੁੱਧ_pursuit, 1.f, 10.f, 1.f, curr_wp_ned, prev_wp_ned,
				      curr_pos_ned, NAN);
	EXPECT_FALSE(PX4_ISFINITE(ਨਿਸ਼ਾਨਾ_bearing1));
}

TEST(ਸ਼ੁੱਧ_Pursuit_Test, InvalidWaypoints)
{
	//	V   C
	//         /
	//  	  /
	//	 /
	//	P
	ਸ਼ੁੱਧ_pursuit_ਸਥਿਤੀ_s ਸ਼ੁੱਧ_pursuit{};
	ਅਚਲ Vector2f curr_wp_ned(10.f, 10.f);
	ਅਚਲ Vector2f prev_wp_ned(0.f, 0.f);
	ਅਚਲ Vector2f curr_pos_ned(10.f, 0.f);
	ਅਚਲ ਦਸ਼ਮਲਵ lookahead_ਦੂਰੀ{5.f};
	// Prev WP is NAN
	ਅਚਲ ਦਸ਼ਮਲਵ ਨਿਸ਼ਾਨਾ_bearing1 = ਸ਼ੁੱਧ_Pursuit::calc_ਨਿਸ਼ਾਨਾ_Bearing(ਸ਼ੁੱਧ_pursuit, 1.f, 10.f, 1.f, curr_wp_ned, Vector2f(NAN,
				      NAN), curr_pos_ned,
				      lookahead_ਦੂਰੀ);
	// Curr WP is NAN
	ਅਚਲ ਦਸ਼ਮਲਵ ਨਿਸ਼ਾਨਾ_bearing2 = ਸ਼ੁੱਧ_Pursuit::calc_ਨਿਸ਼ਾਨਾ_Bearing(ਸ਼ੁੱਧ_pursuit, 1.f, 10.f, 1.f, Vector2f(NAN, NAN),
				      prev_wp_ned, curr_pos_ned,
				      lookahead_ਦੂਰੀ);

	// Curr Pos is NAN
	ਅਚਲ ਦਸ਼ਮਲਵ ਨਿਸ਼ਾਨਾ_bearing3 = ਸ਼ੁੱਧ_Pursuit::calc_ਨਿਸ਼ਾਨਾ_Bearing(ਸ਼ੁੱਧ_pursuit, 1.f, 10.f, 1.f, curr_wp_ned, prev_wp_ned,
				      Vector2f(NAN, NAN), lookahead_ਦੂਰੀ);
	EXPECT_FALSE(PX4_ISFINITE(ਨਿਸ਼ਾਨਾ_bearing1));
	EXPECT_FALSE(PX4_ISFINITE(ਨਿਸ਼ਾਨਾ_bearing2));
	EXPECT_FALSE(PX4_ISFINITE(ਨਿਸ਼ਾਨਾ_bearing3));
}

TEST(ਸ਼ੁੱਧ_Pursuit_Test, ਬਾਹਰਲਾ_ਦਾ_Lookahead)
{
	ਸ਼ੁੱਧ_pursuit_ਸਥਿਤੀ_s ਸ਼ੁੱਧ_pursuit{};
	ਅਚਲ ਦਸ਼ਮਲਵ lookahead_ਦੂਰੀ{5.f};
	//	V   C
	//         /
	//  	  /
	//	 /
	//	P
	ਅਚਲ ਦਸ਼ਮਲਵ ਨਿਸ਼ਾਨਾ_bearing1 = ਸ਼ੁੱਧ_Pursuit::calc_ਨਿਸ਼ਾਨਾ_Bearing(ਸ਼ੁੱਧ_pursuit, 1.f, 10.f, 1.f, Vector2f(10.f, 10.f),
				      Vector2f(0.f, 0.f), Vector2f(10.f, 0.f), lookahead_ਦੂਰੀ);
	//	    V
	//
	//	P ----- C
	ਅਚਲ ਦਸ਼ਮਲਵ ਨਿਸ਼ਾਨਾ_bearing2 = ਸ਼ੁੱਧ_Pursuit::calc_ਨਿਸ਼ਾਨਾ_Bearing(ਸ਼ੁੱਧ_pursuit, 1.f, 10.f, 1.f, Vector2f(0.f, 20.f),
				      Vector2f(0.f, 0.f), Vector2f(10.f, 10.f), lookahead_ਦੂਰੀ);
	// V
	//
	//     P ------ C
	ਅਚਲ ਦਸ਼ਮਲਵ ਨਿਸ਼ਾਨਾ_bearing3 = ਸ਼ੁੱਧ_Pursuit::calc_ਨਿਸ਼ਾਨਾ_Bearing(ਸ਼ੁੱਧ_pursuit, 1.f, 10.f, 1.f, Vector2f(0.f, 20.f),
				      Vector2f(0.f, 10.f), Vector2f(10.f, 0.f), lookahead_ਦੂਰੀ);
	//		V
	//
	// P ------ C
	ਅਚਲ ਦਸ਼ਮਲਵ ਨਿਸ਼ਾਨਾ_bearing4 = ਸ਼ੁੱਧ_Pursuit::calc_ਨਿਸ਼ਾਨਾ_Bearing(ਸ਼ੁੱਧ_pursuit, 1.f, 10.f, 1.f, Vector2f(0.f, 10.f),
				      Vector2f(0.f, 0.f), Vector2f(10.f, 20.f), lookahead_ਦੂਰੀ);

	EXPECT_NEAR(ਨਿਸ਼ਾਨਾ_bearing1, M_PI_2_F + M_PI_4_F, FLT_EPSILON); // Fallback: Bearing to closest point on path
	EXPECT_NEAR(fabsf(ਨਿਸ਼ਾਨਾ_bearing2), M_PI_F, 1e-6f); 	// Fallback: Bearing to closest point on path, +-pi is the same bearing
	EXPECT_NEAR(ਨਿਸ਼ਾਨਾ_bearing3, M_PI_F - atan2f(10, 10), FLT_EPSILON); // Fallback: Bearing to previous waypoint
	EXPECT_NEAR(ਨਿਸ਼ਾਨਾ_bearing4, -M_PI_F + atan2f(10, 10), FLT_EPSILON); // Fallback: Bearing to current waypoint
}

TEST(ਸ਼ੁੱਧ_Pursuit_Test, WaypointOverlap)
{
	ਸ਼ੁੱਧ_pursuit_ਸਥਿਤੀ_s ਸ਼ੁੱਧ_pursuit{};
	ਅਚਲ ਦਸ਼ਮਲਵ lookahead_ਦੂਰੀ{5.f};
	//	    C/P
	//
	//
	//
	//	V
	ਅਚਲ ਦਸ਼ਮਲਵ ਨਿਸ਼ਾਨਾ_bearing1 = ਸ਼ੁੱਧ_Pursuit::calc_ਨਿਸ਼ਾਨਾ_Bearing(ਸ਼ੁੱਧ_pursuit, 1.f, 10.f, 1.f, Vector2f(10.f, 10.f),
				      Vector2f(10.f, 10.f), Vector2f(0.f, 0.f), lookahead_ਦੂਰੀ);
	//	    V
	//
	//
	//
	//	C/P
	ਅਚਲ ਦਸ਼ਮਲਵ ਨਿਸ਼ਾਨਾ_bearing2 = ਸ਼ੁੱਧ_Pursuit::calc_ਨਿਸ਼ਾਨਾ_Bearing(ਸ਼ੁੱਧ_pursuit, 1.f, 10.f, 1.f, Vector2f(0.f, 0.f),
				      Vector2f(0.f, 0.f), Vector2f(10.f, 10.f), lookahead_ਦੂਰੀ);
	EXPECT_NEAR(ਨਿਸ਼ਾਨਾ_bearing1, M_PI_4_F, FLT_EPSILON); 		   // Fallback: Bearing to closest point on path
	EXPECT_NEAR(ਨਿਸ਼ਾਨਾ_bearing2, -(M_PI_4_F + M_PI_2_F), FLT_EPSILON); // Fallback: Bearing to closest point on path
}

TEST(ਸ਼ੁੱਧ_Pursuit_Test, CurrAndPrevSameNorthCoordinate)
{
	ਸ਼ੁੱਧ_pursuit_ਸਥਿਤੀ_s ਸ਼ੁੱਧ_pursuit{};
	ਅਚਲ ਦਸ਼ਮਲਵ lookahead_ਦੂਰੀ{5.f};
	//	P -- V -- C
	ਅਚਲ ਦਸ਼ਮਲਵ ਨਿਸ਼ਾਨਾ_bearing1 = ਸ਼ੁੱਧ_Pursuit::calc_ਨਿਸ਼ਾਨਾ_Bearing(ਸ਼ੁੱਧ_pursuit, 1.f, 10.f, 1.f, Vector2f(0.f, 20.f),
				      Vector2f(0.f, 0.f), Vector2f(0.f, 10.f), lookahead_ਦੂਰੀ);

	//	     V
	//	P ------ C
	ਅਚਲ ਦਸ਼ਮਲਵ ਨਿਸ਼ਾਨਾ_bearing2 = ਸ਼ੁੱਧ_Pursuit::calc_ਨਿਸ਼ਾਨਾ_Bearing(ਸ਼ੁੱਧ_pursuit, 1.f, 10.f, 1.f, Vector2f(0.f, 20.f),
				      Vector2f(0.f, 0.f), Vector2f(5.f / sqrtf(2.f), 10.f), lookahead_ਦੂਰੀ);
	//	     V
	//	C ------ P
	ਅਚਲ ਦਸ਼ਮਲਵ ਨਿਸ਼ਾਨਾ_bearing3 = ਸ਼ੁੱਧ_Pursuit::calc_ਨਿਸ਼ਾਨਾ_Bearing(ਸ਼ੁੱਧ_pursuit, 1.f, 10.f, 1.f, Vector2f(0.f, 0.f),
				      Vector2f(0.f, 20.f), Vector2f(5.f / sqrtf(2.f), 10.f), lookahead_ਦੂਰੀ);
	//	     V
	//
	//	P ------ C
	ਅਚਲ ਦਸ਼ਮਲਵ ਨਿਸ਼ਾਨਾ_bearing4 = ਸ਼ੁੱਧ_Pursuit::calc_ਨਿਸ਼ਾਨਾ_Bearing(ਸ਼ੁੱਧ_pursuit, 1.f, 10.f, 1.f, Vector2f(0.f, 20.f),
				      Vector2f(0.f, 0.f), Vector2f(10.f, 10.f), lookahead_ਦੂਰੀ);

	EXPECT_NEAR(ਨਿਸ਼ਾਨਾ_bearing1, M_PI_2_F, FLT_EPSILON);
	EXPECT_NEAR(ਨਿਸ਼ਾਨਾ_bearing2, M_PI_2_F + M_PI_4_F, FLT_EPSILON);
	EXPECT_NEAR(ਨਿਸ਼ਾਨਾ_bearing3, -(M_PI_2_F + M_PI_4_F), FLT_EPSILON);
	EXPECT_NEAR(fabsf(ਨਿਸ਼ਾਨਾ_bearing4), M_PI_F, 1e-6f); // Fallback: Bearing to closest point on path, +-pi is the same bearing
}

TEST(ਸ਼ੁੱਧ_Pursuit_Test, VehiclePastCurrentWaypoint)
{
	ਸ਼ੁੱਧ_pursuit_ਸਥਿਤੀ_s ਸ਼ੁੱਧ_pursuit{};
	ਅਚਲ ਦਸ਼ਮਲਵ lookahead_ਦੂਰੀ{5.f};
	// The vehicle has passed the current waypoint by more than the lookahead and sits on the extended path.
	// The lookahead intersection would lie even further along the extension; the waypoint has to be targeted instead.
	//	P ------ C ------ V
	ਅਚਲ ਦਸ਼ਮਲਵ ਨਿਸ਼ਾਨਾ_bearing1 = ਸ਼ੁੱਧ_Pursuit::calc_ਨਿਸ਼ਾਨਾ_Bearing(ਸ਼ੁੱਧ_pursuit, 1.f, 10.f, 1.f, Vector2f(0.f, 10.f),
				      Vector2f(0.f, 0.f), Vector2f(0.f, 20.f), lookahead_ਦੂਰੀ);
	// Off the extended path, but within the lookahead corridor
	//		   V
	//	P ------ C
	ਅਚਲ ਦਸ਼ਮਲਵ ਨਿਸ਼ਾਨਾ_bearing2 = ਸ਼ੁੱਧ_Pursuit::calc_ਨਿਸ਼ਾਨਾ_Bearing(ਸ਼ੁੱਧ_pursuit, 1.f, 10.f, 1.f, Vector2f(0.f, 10.f),
				      Vector2f(0.f, 0.f), Vector2f(2.f, 20.f), lookahead_ਦੂਰੀ);
	// Still ahead of the current waypoint: regular pure pursuit is unaffected
	//	P -- V -- C
	ਅਚਲ ਦਸ਼ਮਲਵ ਨਿਸ਼ਾਨਾ_bearing3 = ਸ਼ੁੱਧ_Pursuit::calc_ਨਿਸ਼ਾਨਾ_Bearing(ਸ਼ੁੱਧ_pursuit, 1.f, 10.f, 1.f, Vector2f(0.f, 10.f),
				      Vector2f(0.f, 0.f), Vector2f(0.f, 2.f), lookahead_ਦੂਰੀ);

	EXPECT_NEAR(ਨਿਸ਼ਾਨਾ_bearing1, -M_PI_2_F, FLT_EPSILON);			 // Back to the current waypoint
	EXPECT_NEAR(ਨਿਸ਼ਾਨਾ_bearing2, atan2f(-10.f, -2.f), FLT_EPSILON); // Back to the current waypoint
	EXPECT_NEAR(ਨਿਸ਼ਾਨਾ_bearing3, M_PI_2_F, FLT_EPSILON);			 // Along the path
}

TEST(ਸ਼ੁੱਧ_Pursuit_Test, CrosstrackError)
{
	ਸ਼ੁੱਧ_pursuit_ਸਥਿਤੀ_s ਸ਼ੁੱਧ_pursuit{};
	ਅਚਲ ਦਸ਼ਮਲਵ lookahead_ਦੂਰੀ{5.f};
	//	    V
	//
	//	P ----- C
	ਸ਼ੁੱਧ_Pursuit::calc_ਨਿਸ਼ਾਨਾ_Bearing(ਸ਼ੁੱਧ_pursuit, 1.f, 10.f, 1.f, Vector2f(0.f, 20.f),
				       Vector2f(0.f, 0.f), Vector2f(10.f, 10.f), lookahead_ਦੂਰੀ);
	ਅਚਲ ਦਸ਼ਮਲਵ crosstrack1 = ਸ਼ੁੱਧ_pursuit.crosstrack_error;

	//	    V
	//	P ----- C
	ਸ਼ੁੱਧ_Pursuit::calc_ਨਿਸ਼ਾਨਾ_Bearing(ਸ਼ੁੱਧ_pursuit, 1.f, 10.f, 1.f, Vector2f(0.f, 20.f),
				       Vector2f(0.f, 0.f), Vector2f(5.f, 10.f), lookahead_ਦੂਰੀ);
	ਅਚਲ ਦਸ਼ਮਲਵ crosstrack2 = ਸ਼ੁੱਧ_pursuit.crosstrack_error;

	//	P ----- C
	//
	//	    V
	ਸ਼ੁੱਧ_Pursuit::calc_ਨਿਸ਼ਾਨਾ_Bearing(ਸ਼ੁੱਧ_pursuit, 1.f, 10.f, 1.f, Vector2f(0.f, 20.f),
				       Vector2f(0.f, 0.f), Vector2f(-10.f, 10.f), lookahead_ਦੂਰੀ);
	ਅਚਲ ਦਸ਼ਮਲਵ crosstrack3 = ਸ਼ੁੱਧ_pursuit.crosstrack_error;

	//	P ----- C
	//	    V
	ਸ਼ੁੱਧ_Pursuit::calc_ਨਿਸ਼ਾਨਾ_Bearing(ਸ਼ੁੱਧ_pursuit, 1.f, 10.f, 1.f, Vector2f(0.f, 20.f),
				       Vector2f(0.f, 0.f), Vector2f(-5.f, 10.f), lookahead_ਦੂਰੀ);
	ਅਚਲ ਦਸ਼ਮਲਵ crosstrack4 = ਸ਼ੁੱਧ_pursuit.crosstrack_error;

	EXPECT_NEAR(crosstrack1, -10.f, FLT_EPSILON);
	EXPECT_NEAR(crosstrack2, -5.f, FLT_EPSILON);
	EXPECT_NEAR(crosstrack3, 10.f, FLT_EPSILON);
	EXPECT_NEAR(crosstrack4, 5.f, FLT_EPSILON);

}
