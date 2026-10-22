#ಅನ್ತರ್ಭಾವ <gtest/gtest.h>

#ಅನ್ತರ್ಭಾವ "TrajectoryConstraints.hpp"

ಬಳಸಿ ನಾಮಾಕಾಶ ಮಾತೃಕೆ;
ಬಳಸಿ ನಾಮಾಕಾಶ math::trajectory;

ವರ್ಗ TrajectoryConstraintsTest : ಸಾರ್ವಜನಿಕ ::testing::Test
{
ಸಾರ್ವಜನಿಕ:
	VehicleDynamicLimits config;

	Vector3f vehicle_location;
	Vector3f ಗುರಿ;
	Vector3f ಮುಂದಿನ_ಗುರಿ;

	ದಶಮಾಂಶ final_ವೇಗ = 0;

	ಶೂನ್ಯ ಗಣ_ಮೇಲೆ() override
	{
		config.z_accept_rad = 1.f;
		config.xy_accept_rad = 0.99f;

		config.max_acc_xy = 3.f;
		config.max_jerk = 10.f;

		config.max_ವೇಗ_xy = 10.f;

		config.max_acc_xy_radius_scale = 0.8f;

		/*
		 *             (20,20)
		 *              Next target
		 *
		 *              ^
		 *              |
		 *
		 * (10,10)      (20,10)
		 * Vehicle  ->  Target
		 *
		 */
		vehicle_location = Vector3f(10, 10, 5);
		ಗುರಿ = Vector3f(20, 10, 5);
		ಮುಂದಿನ_ಗುರಿ = Vector3f(20, 20, 5);
	}
};

TEST_F(TrajectoryConstraintsTest, testStraight)
{
	// GIVEN: 3 waypoints in straight line
	ಮುಂದಿನ_ಗುರಿ = ಗುರಿ + 2.f * (ಗುರಿ - vehicle_location);
	ಗುರಿ = vehicle_location + 0.5f * (ಮುಂದಿನ_ಗುರಿ - vehicle_location);

	// WHEN: we get the speed for straight line travel
	Vector3f waypoints[3] = {vehicle_location, ಗುರಿ, ಮುಂದಿನ_ಗುರಿ};
	ದಶಮಾಂಶ through_ವೇಗ = computeXYSpeedFromWaypoints<3>(waypoints, config);

	// THEN: it should be the same as speed directly to the end point
	Vector3f ನೇರ_points[2] = {vehicle_location, ಮುಂದಿನ_ಗುರಿ};
	ದಶಮಾಂಶ ನೇರ_ವೇಗ = computeXYSpeedFromWaypoints<2>(ನೇರ_points, config);

	EXPECT_FLOAT_EQ(through_ವೇಗ, ನೇರ_ವೇಗ);
}

TEST_F(TrajectoryConstraintsTest, testStraightNaN)
{
	// GIVEN: 3 waypoints in straight line
	ಮುಂದಿನ_ಗುರಿ = ಗುರಿ + 2.f * (ಗುರಿ - vehicle_location);
	ಗುರಿ = vehicle_location + 0.5f * (ಮುಂದಿನ_ಗುರಿ - vehicle_location);
	ಮುಂದಿನ_ಗುರಿ(0) = NAN;
	ಮುಂದಿನ_ಗುರಿ(1) = NAN;

	// WHEN: we get the speed for points which are NaN afterwards
	Vector3f waypoints[3] = {vehicle_location, ಗುರಿ, ಮುಂದಿನ_ಗುರಿ};
	ದಶಮಾಂಶ through_ವೇಗ = computeXYSpeedFromWaypoints<3>(waypoints, config);

	// THEN: it should be the same as speed to the closer point
	Vector3f ನೇರ_points[2] = {vehicle_location, ಗುರಿ};
	ದಶಮಾಂಶ ನೇರ_ವೇಗ = computeXYSpeedFromWaypoints<2>(ನೇರ_points, config);

	EXPECT_FLOAT_EQ(through_ವೇಗ, ನೇರ_ವೇಗ);
}

TEST_F(TrajectoryConstraintsTest, test_Straight_ಕೆಳಗಿನ_Jerk_Close)
{
	// GIVEN: 3 waypoints in straight line
	ಮುಂದಿನ_ಗುರಿ = ಗುರಿ + 2.f * (ಗುರಿ - vehicle_location);
	ಗುರಿ = vehicle_location + 0.05f * (ಮುಂದಿನ_ಗುರಿ - vehicle_location);
	config.max_jerk = 8.f;

	// WHEN: we get the speed for straight line travel
	Vector3f waypoints[3] = {vehicle_location, ಗುರಿ, ಮುಂದಿನ_ಗುರಿ};
	ದಶಮಾಂಶ through_ವೇಗ = computeXYSpeedFromWaypoints<3>(waypoints, config);

	// THEN: it should be the same as speed directly to the end point
	Vector3f ನೇರ_points[2] = {vehicle_location, ಮುಂದಿನ_ಗುರಿ};
	ದಶಮಾಂಶ ನೇರ_ವೇಗ = computeXYSpeedFromWaypoints<2>(ನೇರ_points, config);

	EXPECT_FLOAT_EQ(through_ವೇಗ, ನೇರ_ವೇಗ);
}

TEST_F(TrajectoryConstraintsTest, testStraightMidClose)
{
	// GIVEN: 3 waypoints in straight line
	ಮುಂದಿನ_ಗುರಿ = ಗುರಿ + 2.f * (ಗುರಿ - vehicle_location);
	ಗುರಿ = vehicle_location + 0.05f * (ಮುಂದಿನ_ಗುರಿ - vehicle_location);

	// WHEN: we get the speed for straight line travel
	Vector3f waypoints[3] = {vehicle_location, ಗುರಿ, ಮುಂದಿನ_ಗುರಿ};
	ದಶಮಾಂಶ through_ವೇಗ = computeXYSpeedFromWaypoints<3>(waypoints, config);

	// THEN: it should be the same as speed directly to the end point
	Vector3f ನೇರ_points[2] = {vehicle_location, ಮುಂದಿನ_ಗುರಿ};
	ದಶಮಾಂಶ ನೇರ_ವೇಗ = computeXYSpeedFromWaypoints<2>(ನೇರ_points, config);

	EXPECT_FLOAT_EQ(through_ವೇಗ, ನೇರ_ವೇಗ);
}

TEST_F(TrajectoryConstraintsTest, testStraightMidFar)
{
	// GIVEN: 3 waypoints in straight line
	ಮುಂದಿನ_ಗುರಿ = ಗುರಿ + 2.f * (ಗುರಿ - vehicle_location);
	ಗುರಿ = vehicle_location + 0.95f * (ಮುಂದಿನ_ಗುರಿ - vehicle_location);

	// WHEN: we get the speed for straight line travel
	Vector3f waypoints[3] = {vehicle_location, ಗುರಿ, ಮುಂದಿನ_ಗುರಿ};
	ದಶಮಾಂಶ through_ವೇಗ = computeXYSpeedFromWaypoints<3>(waypoints, config);

	// THEN: it should be the same as speed directly to the end point
	Vector3f ನೇರ_points[2] = {vehicle_location, ಮುಂದಿನ_ಗುರಿ};
	ದಶಮಾಂಶ ನೇರ_ವೇಗ = computeXYSpeedFromWaypoints<2>(ನೇರ_points, config);

	EXPECT_FLOAT_EQ(through_ವೇಗ, ನೇರ_ವೇಗ);
}


TEST_F(TrajectoryConstraintsTest, test90Angle)
{
	// GIVEN: 3 waypoints in 90 degree angle
	EXPECT_FLOAT_EQ(0.f, (vehicle_location - ಗುರಿ).dot(ಗುರಿ - ಮುಂದಿನ_ಗುರಿ));

	// WHEN: we get the speed for travel around the path
	Vector3f waypoints[3] = {vehicle_location, ಗುರಿ, ಮುಂದಿನ_ಗುರಿ};
	ದಶಮಾಂಶ through_ವೇಗ = computeXYSpeedFromWaypoints<3>(waypoints, config);

	// THEN: it should be slightly faster than stopping at the intermediate point
	Vector3f stop_points[2] = {vehicle_location, ಗುರಿ};
	ದಶಮಾಂಶ stop_ವೇಗ = computeXYSpeedFromWaypoints<2>(stop_points, config);

	EXPECT_GT(through_ವೇಗ, stop_ವೇಗ); //faster
	EXPECT_LT(through_ವೇಗ, stop_ವೇಗ * 1.03f); // but less than 3% faster
}

TEST_F(TrajectoryConstraintsTest, test45Angle)
{
	// GIVEN: 3 waypoints in 45 degree angle
	ಮುಂದಿನ_ಗುರಿ = Vector3f(25, 15, 5);

	// WHEN: we get the speed for travel around the path
	Vector3f waypoints[3] = {vehicle_location, ಗುರಿ, ಮುಂದಿನ_ಗುರಿ};
	ದಶಮಾಂಶ through_ವೇಗ = computeXYSpeedFromWaypoints<3>(waypoints, config);

	// THEN: it should be slightly faster than stopping at the intermediate point
	Vector3f stop_points[2] = {vehicle_location, ಗುರಿ};
	ದಶಮಾಂಶ stop_ವೇಗ = computeXYSpeedFromWaypoints<2>(stop_points, config);

	EXPECT_GT(through_ವೇಗ, stop_ವೇಗ * 1.03f); // more than 3% faster
	EXPECT_LT(through_ವೇಗ, stop_ವೇಗ * 1.06f); // but less than 6% faster
}

TEST_F(TrajectoryConstraintsTest, test10Angle)
{
	// GIVEN: 3 waypoints in 10 degree angle
	ಮುಂದಿನ_ಗುರಿ = Vector3f(30, 11.7, 5);

	// WHEN: we get the speed for travel around the path
	Vector3f waypoints[3] = {vehicle_location, ಗುರಿ, ಮುಂದಿನ_ಗುರಿ};
	ದಶಮಾಂಶ through_ವೇಗ = computeXYSpeedFromWaypoints<3>(waypoints, config);

	// THEN: it should be slightly faster than stopping at the intermediate point
	Vector3f stop_points[2] = {vehicle_location, ಗುರಿ};
	ದಶಮಾಂಶ stop_ವೇಗ = computeXYSpeedFromWaypoints<2>(stop_points, config);

	EXPECT_GT(through_ವೇಗ, stop_ವೇಗ * 1.25f); // more than 25% faster
	EXPECT_LT(through_ವೇಗ, stop_ವೇಗ * 1.3f); // but less than 30% faster
}

TEST_F(TrajectoryConstraintsTest, test10_Angle_Far_ಮುಂದಿನ)
{
	// GIVEN: 3 waypoints in 10 degree angle, but next waypoint is far
	ಮುಂದಿನ_ಗುರಿ = 2.f * (Vector3f(30, 11.7, 5) - ಗುರಿ) + ಗುರಿ;

	// WHEN: we get the speed for travel around the path
	Vector3f far_waypoints[3] = {vehicle_location, ಗುರಿ, ಮುಂದಿನ_ಗುರಿ};
	ದಶಮಾಂಶ far_ವೇಗ = computeXYSpeedFromWaypoints<3>(far_waypoints, config);

	// THEN: it should be the same speed as a closer next waypoint at the same angle, since the bottleneck is the turn
	ಮುಂದಿನ_ಗುರಿ = Vector3f(30, 11.7, 5);
	Vector3f close_waypoints[3] = {vehicle_location, ಗುರಿ, ಮುಂದಿನ_ಗುರಿ};
	ದಶಮಾಂಶ close_ವೇಗ = computeXYSpeedFromWaypoints<3>(close_waypoints, config);

	EXPECT_FLOAT_EQ(far_ವೇಗ, close_ವೇಗ);
}

TEST_F(TrajectoryConstraintsTest, test10_Angle_Close_ಮುಂದಿನ)
{
	// GIVEN: 3 waypoints in right angle, but next waypoint is far
	ಮುಂದಿನ_ಗುರಿ = .2f * (Vector3f(30, 11.7, 5) - ಗುರಿ) + ಗುರಿ;

	// WHEN: we get the speed for travel around the path
	Vector3f close_waypoints[3] = {vehicle_location, ಗುರಿ, ಮುಂದಿನ_ಗುರಿ};
	ದಶಮಾಂಶ close_ವೇಗ = computeXYSpeedFromWaypoints<3>(close_waypoints, config);

	// THEN: it should be slower than a further next waypoint at the same angle, since the bottleneck is the distance
	ಮುಂದಿನ_ಗುರಿ = Vector3f(30, 11.7, 5);
	Vector3f ಸಾಮಾನ್ಯ_waypoints[3] = {vehicle_location, ಗುರಿ, ಮುಂದಿನ_ಗುರಿ};
	ದಶಮಾಂಶ ಸಾಮಾನ್ಯ_ವೇಗ = computeXYSpeedFromWaypoints<3>(ಸಾಮಾನ್ಯ_waypoints, config);

	EXPECT_LT(close_ವೇಗ, ಸಾಮಾನ್ಯ_ವೇಗ);
}

TEST(TrajectoryConstraintsClamp, clamp_ಗಮ್ಯ_XYNorm_No_Effect_Large)
{
	// GIVEN: a short vector
	Vector3f vec(1, 2, 3);

	// WHEN: we clamp it on XY with a long cutoff
	clamp_ಗಮ್ಯ_XYNorm(vec, 1000.f);

	// THEN: it shouldn't change
	EXPECT_EQ(vec, Vector3f(1, 2, 3));
}

TEST(TrajectoryConstraintsClamp, clamp_ಗಮ್ಯ_ZNorm_No_Effect)
{
	// GIVEN: a short vector
	Vector3f vec(1, 2, 3);

	// WHEN: we clamp it on XY with a long cutoff
	clamp_ಗಮ್ಯ_ZNorm(vec, 1000.f);

	// THEN: it shouldn't change
	EXPECT_EQ(vec, Vector3f(1, 2, 3));
}

TEST(TrajectoryConstraintsClamp, clamp_ಗಮ್ಯ_XYNorm_No_Effect_Exact)
{
	// GIVEN: a vector
	Vector3f vec(3, 4, 1);

	// WHEN: we clamp it on XY with exact cutoff
	clamp_ಗಮ್ಯ_XYNorm(vec, 5.f);

	// THEN: it shouldn't change
	EXPECT_EQ(vec, Vector3f(3, 4, 1));
}

TEST(TrajectoryConstraintsClamp, clamp_ಗಮ್ಯ_ZNorm_No_Effect_Exact)
{
	// GIVEN: a vector
	Vector3f vec(3, 4, -1);

	// WHEN: we clamp it on Z with exact cutoff
	clamp_ಗಮ್ಯ_ZNorm(vec, 1.f);

	// THEN: it shouldn't change
	EXPECT_EQ(vec, Vector3f(3, 4, -1));
}

TEST(TrajectoryConstraintsClamp, clamp_ಗಮ್ಯ_XYNorm_Half)
{
	// GIVEN: a vector
	Vector3f vec(3, 4, 1);

	// WHEN: we clamp it on XY with half hypot length
	clamp_ಗಮ್ಯ_XYNorm(vec, 2.5f);

	// THEN: it should be half length
	EXPECT_TRUE(vec == Vector3f(1.5f, 2.f, 0.5f));
}

TEST(TrajectoryConstraintsClamp, clamp_ಗಮ್ಯ_ZNorm_Half)
{
	// GIVEN: a vector
	Vector3f vec(3, 4, 10);

	// WHEN: we clamp it on Z with half length
	clamp_ಗಮ್ಯ_ZNorm(vec, 5.f);

	// THEN: it should be half length
	EXPECT_TRUE(vec == Vector3f(1.5f, 2.f, 5.f));
}

TEST(TrajectoryConstraintsClamp, clamp_ಗಮ್ಯ_XYNorm_Zero)
{
	// GIVEN: a vector
	Vector3f vec(3, 4, 1);

	// WHEN: we clamp it on XY with half hypot length
	clamp_ಗಮ್ಯ_XYNorm(vec, 0.f);

	// THEN: it should be 0
	EXPECT_TRUE(vec == Vector3f(0.f, 0.f, 0.f));
}

TEST(TrajectoryConstraintsClamp, clamp_ಗಮ್ಯ_ZNorm_Zero)
{
	// GIVEN: a vector
	Vector3f vec(3, 4, 1);

	// WHEN: we clamp it on Z with half hypot length
	clamp_ಗಮ್ಯ_ZNorm(vec, 0.f);

	// THEN: it should be 0
	EXPECT_TRUE(vec == Vector3f(0.f, 0.f, 0.f));
}

TEST(TrajectoryConstraintsClamp, clamp_ಗಮ್ಯ_XYNorm_Vec_Zero)
{
	// GIVEN: a vector
	Vector3f vec(0, 0, 0);

	// WHEN: we clamp it on XY
	clamp_ಗಮ್ಯ_XYNorm(vec, 1.f);

	// THEN: it should be 0 still
	EXPECT_TRUE(vec == Vector3f(0.f, 0.f, 0.f));
}

TEST(TrajectoryConstraintsClamp, clamp_ಗಮ್ಯ_ZNorm_Vec_Zero)
{
	// GIVEN: a vector
	Vector3f vec(0, 0, 0);

	// WHEN: we clamp it on Z
	clamp_ಗಮ್ಯ_ZNorm(vec, 1.f);

	// THEN: it should be 0 still
	EXPECT_TRUE(vec == Vector3f(0.f, 0.f, 0.f));
}

TEST(TrajectoryConstraintsClamp, clamp_ಗಮ್ಯ_XYNorm_Vec_Zero_ಗಮ್ಯ_Zero)
{
	// GIVEN: a vector
	Vector3f vec(0, 0, 0);

	// WHEN: we clamp it on XY
	clamp_ಗಮ್ಯ_XYNorm(vec, 0.f);

	// THEN: it should be 0 still
	EXPECT_TRUE(vec == Vector3f(0.f, 0.f, 0.f));
}

TEST(TrajectoryConstraintsClamp, clamp_ಗಮ್ಯ_ZNorm_Vec_Zero_ಗಮ್ಯ_Zero)
{
	// GIVEN: a vector
	Vector3f vec(0, 0, 0);

	// WHEN: we clamp it on XY
	clamp_ಗಮ್ಯ_ZNorm(vec, 0.f);

	// THEN: it should be 0 still
	EXPECT_TRUE(vec == Vector3f(0.f, 0.f, 0.f));
}
