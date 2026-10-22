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

#अन्तर्भाव <gtest/gtest.h>
#अन्तर्भाव <lib/pure_pursuit/PurePursuit.hpp>
#अन्तर्भाव <uORB/topics/pure_pursuit_status.h>

using नामाकाश matrix;

TEST(PurePursuitTest, InvalidSpeed)
{
	//      V   C
	//         /
	//  	  /
	//	 /
	//	P
	pure_pursuit_status_s pure_pursuit{};
	स्थिरांक Vector2f curr_wp_ned(10.f, 10.f);
	स्थिरांक Vector2f prev_wp_ned(0.f, 0.f);
	स्थिरांक Vector2f curr_pos_ned(10.f, 0.f);
	// NaN speed
	स्थिरांक दशांश target_bearing1 = PurePursuit::calcTargetBearing(pure_pursuit, 1.f, 10.f, 1.f, curr_wp_ned, prev_wp_ned,
				      curr_pos_ned, न_अंक);
	EXPECT_FALSE(PX4_ISFINITE(target_bearing1));
}

TEST(PurePursuitTest, InvalidWaypoints)
{
	//	V   C
	//         /
	//  	  /
	//	 /
	//	P
	pure_pursuit_status_s pure_pursuit{};
	स्थिरांक Vector2f curr_wp_ned(10.f, 10.f);
	स्थिरांक Vector2f prev_wp_ned(0.f, 0.f);
	स्थिरांक Vector2f curr_pos_ned(10.f, 0.f);
	स्थिरांक दशांश lookahead_distance{5.f};
	// Prev WP is NAN
	स्थिरांक दशांश target_bearing1 = PurePursuit::calcTargetBearing(pure_pursuit, 1.f, 10.f, 1.f, curr_wp_ned, Vector2f(न_अंक,
				      न_अंक), curr_pos_ned,
				      lookahead_distance);
	// Curr WP is NAN
	स्थिरांक दशांश target_bearing2 = PurePursuit::calcTargetBearing(pure_pursuit, 1.f, 10.f, 1.f, Vector2f(न_अंक, न_अंक),
				      prev_wp_ned, curr_pos_ned,
				      lookahead_distance);

	// Curr Pos is NAN
	स्थिरांक दशांश target_bearing3 = PurePursuit::calcTargetBearing(pure_pursuit, 1.f, 10.f, 1.f, curr_wp_ned, prev_wp_ned,
				      Vector2f(न_अंक, न_अंक), lookahead_distance);
	EXPECT_FALSE(PX4_ISFINITE(target_bearing1));
	EXPECT_FALSE(PX4_ISFINITE(target_bearing2));
	EXPECT_FALSE(PX4_ISFINITE(target_bearing3));
}

TEST(PurePursuitTest, OutOfLookahead)
{
	pure_pursuit_status_s pure_pursuit{};
	स्थिरांक दशांश lookahead_distance{5.f};
	//	V   C
	//         /
	//  	  /
	//	 /
	//	P
	स्थिरांक दशांश target_bearing1 = PurePursuit::calcTargetBearing(pure_pursuit, 1.f, 10.f, 1.f, Vector2f(10.f, 10.f),
				      Vector2f(0.f, 0.f), Vector2f(10.f, 0.f), lookahead_distance);
	//	    V
	//
	//	P ----- C
	स्थिरांक दशांश target_bearing2 = PurePursuit::calcTargetBearing(pure_pursuit, 1.f, 10.f, 1.f, Vector2f(0.f, 20.f),
				      Vector2f(0.f, 0.f), Vector2f(10.f, 10.f), lookahead_distance);
	// V
	//
	//     P ------ C
	स्थिरांक दशांश target_bearing3 = PurePursuit::calcTargetBearing(pure_pursuit, 1.f, 10.f, 1.f, Vector2f(0.f, 20.f),
				      Vector2f(0.f, 10.f), Vector2f(10.f, 0.f), lookahead_distance);
	//		V
	//
	// P ------ C
	स्थिरांक दशांश target_bearing4 = PurePursuit::calcTargetBearing(pure_pursuit, 1.f, 10.f, 1.f, Vector2f(0.f, 10.f),
				      Vector2f(0.f, 0.f), Vector2f(10.f, 20.f), lookahead_distance);

	EXPECT_NEAR(target_bearing1, M_PI_2_F + M_PI_4_F, भग्न_अंतर); // Fallback: Bearing to closest point on path
	EXPECT_NEAR(fabsf(target_bearing2), M_PI_F, 1e-6f); 	// Fallback: Bearing to closest point on path, +-pi is the same bearing
	EXPECT_NEAR(target_bearing3, M_PI_F - atan2f(10, 10), भग्न_अंतर); // Fallback: Bearing to previous waypoint
	EXPECT_NEAR(target_bearing4, -M_PI_F + atan2f(10, 10), भग्न_अंतर); // Fallback: Bearing to current waypoint
}

TEST(PurePursuitTest, WaypointOverlap)
{
	pure_pursuit_status_s pure_pursuit{};
	स्थिरांक दशांश lookahead_distance{5.f};
	//	    C/P
	//
	//
	//
	//	V
	स्थिरांक दशांश target_bearing1 = PurePursuit::calcTargetBearing(pure_pursuit, 1.f, 10.f, 1.f, Vector2f(10.f, 10.f),
				      Vector2f(10.f, 10.f), Vector2f(0.f, 0.f), lookahead_distance);
	//	    V
	//
	//
	//
	//	C/P
	स्थिरांक दशांश target_bearing2 = PurePursuit::calcTargetBearing(pure_pursuit, 1.f, 10.f, 1.f, Vector2f(0.f, 0.f),
				      Vector2f(0.f, 0.f), Vector2f(10.f, 10.f), lookahead_distance);
	EXPECT_NEAR(target_bearing1, M_PI_4_F, भग्न_अंतर); 		   // Fallback: Bearing to closest point on path
	EXPECT_NEAR(target_bearing2, -(M_PI_4_F + M_PI_2_F), भग्न_अंतर); // Fallback: Bearing to closest point on path
}

TEST(PurePursuitTest, CurrAndPrevSameNorthCoordinate)
{
	pure_pursuit_status_s pure_pursuit{};
	स्थिरांक दशांश lookahead_distance{5.f};
	//	P -- V -- C
	स्थिरांक दशांश target_bearing1 = PurePursuit::calcTargetBearing(pure_pursuit, 1.f, 10.f, 1.f, Vector2f(0.f, 20.f),
				      Vector2f(0.f, 0.f), Vector2f(0.f, 10.f), lookahead_distance);

	//	     V
	//	P ------ C
	स्थिरांक दशांश target_bearing2 = PurePursuit::calcTargetBearing(pure_pursuit, 1.f, 10.f, 1.f, Vector2f(0.f, 20.f),
				      Vector2f(0.f, 0.f), Vector2f(5.f / sqrtf(2.f), 10.f), lookahead_distance);
	//	     V
	//	C ------ P
	स्थिरांक दशांश target_bearing3 = PurePursuit::calcTargetBearing(pure_pursuit, 1.f, 10.f, 1.f, Vector2f(0.f, 0.f),
				      Vector2f(0.f, 20.f), Vector2f(5.f / sqrtf(2.f), 10.f), lookahead_distance);
	//	     V
	//
	//	P ------ C
	स्थिरांक दशांश target_bearing4 = PurePursuit::calcTargetBearing(pure_pursuit, 1.f, 10.f, 1.f, Vector2f(0.f, 20.f),
				      Vector2f(0.f, 0.f), Vector2f(10.f, 10.f), lookahead_distance);

	EXPECT_NEAR(target_bearing1, M_PI_2_F, भग्न_अंतर);
	EXPECT_NEAR(target_bearing2, M_PI_2_F + M_PI_4_F, भग्न_अंतर);
	EXPECT_NEAR(target_bearing3, -(M_PI_2_F + M_PI_4_F), भग्न_अंतर);
	EXPECT_NEAR(fabsf(target_bearing4), M_PI_F, 1e-6f); // Fallback: Bearing to closest point on path, +-pi is the same bearing
}

TEST(PurePursuitTest, VehiclePastCurrentWaypoint)
{
	pure_pursuit_status_s pure_pursuit{};
	स्थिरांक दशांश lookahead_distance{5.f};
	// The vehicle has passed the current waypoint by more than the lookahead and sits on the extended path.
	// The lookahead intersection would lie even further along the extension; the waypoint has to be targeted instead.
	//	P ------ C ------ V
	स्थिरांक दशांश target_bearing1 = PurePursuit::calcTargetBearing(pure_pursuit, 1.f, 10.f, 1.f, Vector2f(0.f, 10.f),
				      Vector2f(0.f, 0.f), Vector2f(0.f, 20.f), lookahead_distance);
	// Off the extended path, but within the lookahead corridor
	//		   V
	//	P ------ C
	स्थिरांक दशांश target_bearing2 = PurePursuit::calcTargetBearing(pure_pursuit, 1.f, 10.f, 1.f, Vector2f(0.f, 10.f),
				      Vector2f(0.f, 0.f), Vector2f(2.f, 20.f), lookahead_distance);
	// Still ahead of the current waypoint: regular pure pursuit is unaffected
	//	P -- V -- C
	स्थिरांक दशांश target_bearing3 = PurePursuit::calcTargetBearing(pure_pursuit, 1.f, 10.f, 1.f, Vector2f(0.f, 10.f),
				      Vector2f(0.f, 0.f), Vector2f(0.f, 2.f), lookahead_distance);

	EXPECT_NEAR(target_bearing1, -M_PI_2_F, भग्न_अंतर);			 // Back to the current waypoint
	EXPECT_NEAR(target_bearing2, atan2f(-10.f, -2.f), भग्न_अंतर); // Back to the current waypoint
	EXPECT_NEAR(target_bearing3, M_PI_2_F, भग्न_अंतर);			 // Along the path
}

TEST(PurePursuitTest, CrosstrackError)
{
	pure_pursuit_status_s pure_pursuit{};
	स्थिरांक दशांश lookahead_distance{5.f};
	//	    V
	//
	//	P ----- C
	PurePursuit::calcTargetBearing(pure_pursuit, 1.f, 10.f, 1.f, Vector2f(0.f, 20.f),
				       Vector2f(0.f, 0.f), Vector2f(10.f, 10.f), lookahead_distance);
	स्थिरांक दशांश crosstrack1 = pure_pursuit.crosstrack_error;

	//	    V
	//	P ----- C
	PurePursuit::calcTargetBearing(pure_pursuit, 1.f, 10.f, 1.f, Vector2f(0.f, 20.f),
				       Vector2f(0.f, 0.f), Vector2f(5.f, 10.f), lookahead_distance);
	स्थिरांक दशांश crosstrack2 = pure_pursuit.crosstrack_error;

	//	P ----- C
	//
	//	    V
	PurePursuit::calcTargetBearing(pure_pursuit, 1.f, 10.f, 1.f, Vector2f(0.f, 20.f),
				       Vector2f(0.f, 0.f), Vector2f(-10.f, 10.f), lookahead_distance);
	स्थिरांक दशांश crosstrack3 = pure_pursuit.crosstrack_error;

	//	P ----- C
	//	    V
	PurePursuit::calcTargetBearing(pure_pursuit, 1.f, 10.f, 1.f, Vector2f(0.f, 20.f),
				       Vector2f(0.f, 0.f), Vector2f(-5.f, 10.f), lookahead_distance);
	स्थिरांक दशांश crosstrack4 = pure_pursuit.crosstrack_error;

	EXPECT_NEAR(crosstrack1, -10.f, भग्न_अंतर);
	EXPECT_NEAR(crosstrack2, -5.f, भग्न_अंतर);
	EXPECT_NEAR(crosstrack3, 10.f, भग्न_अंतर);
	EXPECT_NEAR(crosstrack4, 5.f, भग्न_अंतर);

}
