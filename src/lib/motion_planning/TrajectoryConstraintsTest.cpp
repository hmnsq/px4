#ਅਨ੍ਤਰ੍ਭਾਵ <gtest/gtest.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "TrajectoryConstraints.hpp"

ਵਰਤੋਂ ਨਾਮਾਕਾਸ਼ ਮੈਟ੍ਰਿਕਸ;
ਵਰਤੋਂ ਨਾਮਾਕਾਸ਼ math::trajectory;

ਵਰਗ TrajectoryConstraintsTest : ਜਨਤਕ ::testing::Test
{
ਜਨਤਕ:
	VehicleDynamicLimits config;

	Vector3f vehicle_location;
	Vector3f ਨਿਸ਼ਾਨਾ;
	Vector3f ਅਗਲਾ_ਨਿਸ਼ਾਨਾ;

	ਦਸ਼ਮਲਵ final_ਰਫਤਾਰ = 0;

	ਖਾਲੀ ਸੈੱਟ_ਉੱਪਰ() override
	{
		config.z_accept_rad = 1.f;
		config.xy_accept_rad = 0.99f;

		config.max_acc_xy = 3.f;
		config.max_jerk = 10.f;

		config.max_ਰਫਤਾਰ_xy = 10.f;

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
		ਨਿਸ਼ਾਨਾ = Vector3f(20, 10, 5);
		ਅਗਲਾ_ਨਿਸ਼ਾਨਾ = Vector3f(20, 20, 5);
	}
};

TEST_F(TrajectoryConstraintsTest, testStraight)
{
	// GIVEN: 3 waypoints in straight line
	ਅਗਲਾ_ਨਿਸ਼ਾਨਾ = ਨਿਸ਼ਾਨਾ + 2.f * (ਨਿਸ਼ਾਨਾ - vehicle_location);
	ਨਿਸ਼ਾਨਾ = vehicle_location + 0.5f * (ਅਗਲਾ_ਨਿਸ਼ਾਨਾ - vehicle_location);

	// WHEN: we get the speed for straight line travel
	Vector3f waypoints[3] = {vehicle_location, ਨਿਸ਼ਾਨਾ, ਅਗਲਾ_ਨਿਸ਼ਾਨਾ};
	ਦਸ਼ਮਲਵ through_ਰਫਤਾਰ = computeXYSpeedFromWaypoints<3>(waypoints, config);

	// THEN: it should be the same as speed directly to the end point
	Vector3f ਸਿੱਧਾ_points[2] = {vehicle_location, ਅਗਲਾ_ਨਿਸ਼ਾਨਾ};
	ਦਸ਼ਮਲਵ ਸਿੱਧਾ_ਰਫਤਾਰ = computeXYSpeedFromWaypoints<2>(ਸਿੱਧਾ_points, config);

	EXPECT_FLOAT_EQ(through_ਰਫਤਾਰ, ਸਿੱਧਾ_ਰਫਤਾਰ);
}

TEST_F(TrajectoryConstraintsTest, testStraightNaN)
{
	// GIVEN: 3 waypoints in straight line
	ਅਗਲਾ_ਨਿਸ਼ਾਨਾ = ਨਿਸ਼ਾਨਾ + 2.f * (ਨਿਸ਼ਾਨਾ - vehicle_location);
	ਨਿਸ਼ਾਨਾ = vehicle_location + 0.5f * (ਅਗਲਾ_ਨਿਸ਼ਾਨਾ - vehicle_location);
	ਅਗਲਾ_ਨਿਸ਼ਾਨਾ(0) = NAN;
	ਅਗਲਾ_ਨਿਸ਼ਾਨਾ(1) = NAN;

	// WHEN: we get the speed for points which are NaN afterwards
	Vector3f waypoints[3] = {vehicle_location, ਨਿਸ਼ਾਨਾ, ਅਗਲਾ_ਨਿਸ਼ਾਨਾ};
	ਦਸ਼ਮਲਵ through_ਰਫਤਾਰ = computeXYSpeedFromWaypoints<3>(waypoints, config);

	// THEN: it should be the same as speed to the closer point
	Vector3f ਸਿੱਧਾ_points[2] = {vehicle_location, ਨਿਸ਼ਾਨਾ};
	ਦਸ਼ਮਲਵ ਸਿੱਧਾ_ਰਫਤਾਰ = computeXYSpeedFromWaypoints<2>(ਸਿੱਧਾ_points, config);

	EXPECT_FLOAT_EQ(through_ਰਫਤਾਰ, ਸਿੱਧਾ_ਰਫਤਾਰ);
}

TEST_F(TrajectoryConstraintsTest, test_Straight_ਨੀਵਾਂ_Jerk_Close)
{
	// GIVEN: 3 waypoints in straight line
	ਅਗਲਾ_ਨਿਸ਼ਾਨਾ = ਨਿਸ਼ਾਨਾ + 2.f * (ਨਿਸ਼ਾਨਾ - vehicle_location);
	ਨਿਸ਼ਾਨਾ = vehicle_location + 0.05f * (ਅਗਲਾ_ਨਿਸ਼ਾਨਾ - vehicle_location);
	config.max_jerk = 8.f;

	// WHEN: we get the speed for straight line travel
	Vector3f waypoints[3] = {vehicle_location, ਨਿਸ਼ਾਨਾ, ਅਗਲਾ_ਨਿਸ਼ਾਨਾ};
	ਦਸ਼ਮਲਵ through_ਰਫਤਾਰ = computeXYSpeedFromWaypoints<3>(waypoints, config);

	// THEN: it should be the same as speed directly to the end point
	Vector3f ਸਿੱਧਾ_points[2] = {vehicle_location, ਅਗਲਾ_ਨਿਸ਼ਾਨਾ};
	ਦਸ਼ਮਲਵ ਸਿੱਧਾ_ਰਫਤਾਰ = computeXYSpeedFromWaypoints<2>(ਸਿੱਧਾ_points, config);

	EXPECT_FLOAT_EQ(through_ਰਫਤਾਰ, ਸਿੱਧਾ_ਰਫਤਾਰ);
}

TEST_F(TrajectoryConstraintsTest, testStraightMidClose)
{
	// GIVEN: 3 waypoints in straight line
	ਅਗਲਾ_ਨਿਸ਼ਾਨਾ = ਨਿਸ਼ਾਨਾ + 2.f * (ਨਿਸ਼ਾਨਾ - vehicle_location);
	ਨਿਸ਼ਾਨਾ = vehicle_location + 0.05f * (ਅਗਲਾ_ਨਿਸ਼ਾਨਾ - vehicle_location);

	// WHEN: we get the speed for straight line travel
	Vector3f waypoints[3] = {vehicle_location, ਨਿਸ਼ਾਨਾ, ਅਗਲਾ_ਨਿਸ਼ਾਨਾ};
	ਦਸ਼ਮਲਵ through_ਰਫਤਾਰ = computeXYSpeedFromWaypoints<3>(waypoints, config);

	// THEN: it should be the same as speed directly to the end point
	Vector3f ਸਿੱਧਾ_points[2] = {vehicle_location, ਅਗਲਾ_ਨਿਸ਼ਾਨਾ};
	ਦਸ਼ਮਲਵ ਸਿੱਧਾ_ਰਫਤਾਰ = computeXYSpeedFromWaypoints<2>(ਸਿੱਧਾ_points, config);

	EXPECT_FLOAT_EQ(through_ਰਫਤਾਰ, ਸਿੱਧਾ_ਰਫਤਾਰ);
}

TEST_F(TrajectoryConstraintsTest, testStraightMidFar)
{
	// GIVEN: 3 waypoints in straight line
	ਅਗਲਾ_ਨਿਸ਼ਾਨਾ = ਨਿਸ਼ਾਨਾ + 2.f * (ਨਿਸ਼ਾਨਾ - vehicle_location);
	ਨਿਸ਼ਾਨਾ = vehicle_location + 0.95f * (ਅਗਲਾ_ਨਿਸ਼ਾਨਾ - vehicle_location);

	// WHEN: we get the speed for straight line travel
	Vector3f waypoints[3] = {vehicle_location, ਨਿਸ਼ਾਨਾ, ਅਗਲਾ_ਨਿਸ਼ਾਨਾ};
	ਦਸ਼ਮਲਵ through_ਰਫਤਾਰ = computeXYSpeedFromWaypoints<3>(waypoints, config);

	// THEN: it should be the same as speed directly to the end point
	Vector3f ਸਿੱਧਾ_points[2] = {vehicle_location, ਅਗਲਾ_ਨਿਸ਼ਾਨਾ};
	ਦਸ਼ਮਲਵ ਸਿੱਧਾ_ਰਫਤਾਰ = computeXYSpeedFromWaypoints<2>(ਸਿੱਧਾ_points, config);

	EXPECT_FLOAT_EQ(through_ਰਫਤਾਰ, ਸਿੱਧਾ_ਰਫਤਾਰ);
}


TEST_F(TrajectoryConstraintsTest, test90Angle)
{
	// GIVEN: 3 waypoints in 90 degree angle
	EXPECT_FLOAT_EQ(0.f, (vehicle_location - ਨਿਸ਼ਾਨਾ).dot(ਨਿਸ਼ਾਨਾ - ਅਗਲਾ_ਨਿਸ਼ਾਨਾ));

	// WHEN: we get the speed for travel around the path
	Vector3f waypoints[3] = {vehicle_location, ਨਿਸ਼ਾਨਾ, ਅਗਲਾ_ਨਿਸ਼ਾਨਾ};
	ਦਸ਼ਮਲਵ through_ਰਫਤਾਰ = computeXYSpeedFromWaypoints<3>(waypoints, config);

	// THEN: it should be slightly faster than stopping at the intermediate point
	Vector3f stop_points[2] = {vehicle_location, ਨਿਸ਼ਾਨਾ};
	ਦਸ਼ਮਲਵ stop_ਰਫਤਾਰ = computeXYSpeedFromWaypoints<2>(stop_points, config);

	EXPECT_GT(through_ਰਫਤਾਰ, stop_ਰਫਤਾਰ); //faster
	EXPECT_LT(through_ਰਫਤਾਰ, stop_ਰਫਤਾਰ * 1.03f); // but less than 3% faster
}

TEST_F(TrajectoryConstraintsTest, test45Angle)
{
	// GIVEN: 3 waypoints in 45 degree angle
	ਅਗਲਾ_ਨਿਸ਼ਾਨਾ = Vector3f(25, 15, 5);

	// WHEN: we get the speed for travel around the path
	Vector3f waypoints[3] = {vehicle_location, ਨਿਸ਼ਾਨਾ, ਅਗਲਾ_ਨਿਸ਼ਾਨਾ};
	ਦਸ਼ਮਲਵ through_ਰਫਤਾਰ = computeXYSpeedFromWaypoints<3>(waypoints, config);

	// THEN: it should be slightly faster than stopping at the intermediate point
	Vector3f stop_points[2] = {vehicle_location, ਨਿਸ਼ਾਨਾ};
	ਦਸ਼ਮਲਵ stop_ਰਫਤਾਰ = computeXYSpeedFromWaypoints<2>(stop_points, config);

	EXPECT_GT(through_ਰਫਤਾਰ, stop_ਰਫਤਾਰ * 1.03f); // more than 3% faster
	EXPECT_LT(through_ਰਫਤਾਰ, stop_ਰਫਤਾਰ * 1.06f); // but less than 6% faster
}

TEST_F(TrajectoryConstraintsTest, test10Angle)
{
	// GIVEN: 3 waypoints in 10 degree angle
	ਅਗਲਾ_ਨਿਸ਼ਾਨਾ = Vector3f(30, 11.7, 5);

	// WHEN: we get the speed for travel around the path
	Vector3f waypoints[3] = {vehicle_location, ਨਿਸ਼ਾਨਾ, ਅਗਲਾ_ਨਿਸ਼ਾਨਾ};
	ਦਸ਼ਮਲਵ through_ਰਫਤਾਰ = computeXYSpeedFromWaypoints<3>(waypoints, config);

	// THEN: it should be slightly faster than stopping at the intermediate point
	Vector3f stop_points[2] = {vehicle_location, ਨਿਸ਼ਾਨਾ};
	ਦਸ਼ਮਲਵ stop_ਰਫਤਾਰ = computeXYSpeedFromWaypoints<2>(stop_points, config);

	EXPECT_GT(through_ਰਫਤਾਰ, stop_ਰਫਤਾਰ * 1.25f); // more than 25% faster
	EXPECT_LT(through_ਰਫਤਾਰ, stop_ਰਫਤਾਰ * 1.3f); // but less than 30% faster
}

TEST_F(TrajectoryConstraintsTest, test10_Angle_Far_ਅਗਲਾ)
{
	// GIVEN: 3 waypoints in 10 degree angle, but next waypoint is far
	ਅਗਲਾ_ਨਿਸ਼ਾਨਾ = 2.f * (Vector3f(30, 11.7, 5) - ਨਿਸ਼ਾਨਾ) + ਨਿਸ਼ਾਨਾ;

	// WHEN: we get the speed for travel around the path
	Vector3f far_waypoints[3] = {vehicle_location, ਨਿਸ਼ਾਨਾ, ਅਗਲਾ_ਨਿਸ਼ਾਨਾ};
	ਦਸ਼ਮਲਵ far_ਰਫਤਾਰ = computeXYSpeedFromWaypoints<3>(far_waypoints, config);

	// THEN: it should be the same speed as a closer next waypoint at the same angle, since the bottleneck is the turn
	ਅਗਲਾ_ਨਿਸ਼ਾਨਾ = Vector3f(30, 11.7, 5);
	Vector3f close_waypoints[3] = {vehicle_location, ਨਿਸ਼ਾਨਾ, ਅਗਲਾ_ਨਿਸ਼ਾਨਾ};
	ਦਸ਼ਮਲਵ close_ਰਫਤਾਰ = computeXYSpeedFromWaypoints<3>(close_waypoints, config);

	EXPECT_FLOAT_EQ(far_ਰਫਤਾਰ, close_ਰਫਤਾਰ);
}

TEST_F(TrajectoryConstraintsTest, test10_Angle_Close_ਅਗਲਾ)
{
	// GIVEN: 3 waypoints in right angle, but next waypoint is far
	ਅਗਲਾ_ਨਿਸ਼ਾਨਾ = .2f * (Vector3f(30, 11.7, 5) - ਨਿਸ਼ਾਨਾ) + ਨਿਸ਼ਾਨਾ;

	// WHEN: we get the speed for travel around the path
	Vector3f close_waypoints[3] = {vehicle_location, ਨਿਸ਼ਾਨਾ, ਅਗਲਾ_ਨਿਸ਼ਾਨਾ};
	ਦਸ਼ਮਲਵ close_ਰਫਤਾਰ = computeXYSpeedFromWaypoints<3>(close_waypoints, config);

	// THEN: it should be slower than a further next waypoint at the same angle, since the bottleneck is the distance
	ਅਗਲਾ_ਨਿਸ਼ਾਨਾ = Vector3f(30, 11.7, 5);
	Vector3f ਸਾਧਾਰਨ_waypoints[3] = {vehicle_location, ਨਿਸ਼ਾਨਾ, ਅਗਲਾ_ਨਿਸ਼ਾਨਾ};
	ਦਸ਼ਮਲਵ ਸਾਧਾਰਨ_ਰਫਤਾਰ = computeXYSpeedFromWaypoints<3>(ਸਾਧਾਰਨ_waypoints, config);

	EXPECT_LT(close_ਰਫਤਾਰ, ਸਾਧਾਰਨ_ਰਫਤਾਰ);
}

TEST(TrajectoryConstraintsClamp, clamp_ਮੰਜ਼ਿਲ_XYNorm_No_Effect_Large)
{
	// GIVEN: a short vector
	Vector3f vec(1, 2, 3);

	// WHEN: we clamp it on XY with a long cutoff
	clamp_ਮੰਜ਼ਿਲ_XYNorm(vec, 1000.f);

	// THEN: it shouldn't change
	EXPECT_EQ(vec, Vector3f(1, 2, 3));
}

TEST(TrajectoryConstraintsClamp, clamp_ਮੰਜ਼ਿਲ_ZNorm_No_Effect)
{
	// GIVEN: a short vector
	Vector3f vec(1, 2, 3);

	// WHEN: we clamp it on XY with a long cutoff
	clamp_ਮੰਜ਼ਿਲ_ZNorm(vec, 1000.f);

	// THEN: it shouldn't change
	EXPECT_EQ(vec, Vector3f(1, 2, 3));
}

TEST(TrajectoryConstraintsClamp, clamp_ਮੰਜ਼ਿਲ_XYNorm_No_Effect_Exact)
{
	// GIVEN: a vector
	Vector3f vec(3, 4, 1);

	// WHEN: we clamp it on XY with exact cutoff
	clamp_ਮੰਜ਼ਿਲ_XYNorm(vec, 5.f);

	// THEN: it shouldn't change
	EXPECT_EQ(vec, Vector3f(3, 4, 1));
}

TEST(TrajectoryConstraintsClamp, clamp_ਮੰਜ਼ਿਲ_ZNorm_No_Effect_Exact)
{
	// GIVEN: a vector
	Vector3f vec(3, 4, -1);

	// WHEN: we clamp it on Z with exact cutoff
	clamp_ਮੰਜ਼ਿਲ_ZNorm(vec, 1.f);

	// THEN: it shouldn't change
	EXPECT_EQ(vec, Vector3f(3, 4, -1));
}

TEST(TrajectoryConstraintsClamp, clamp_ਮੰਜ਼ਿਲ_XYNorm_Half)
{
	// GIVEN: a vector
	Vector3f vec(3, 4, 1);

	// WHEN: we clamp it on XY with half hypot length
	clamp_ਮੰਜ਼ਿਲ_XYNorm(vec, 2.5f);

	// THEN: it should be half length
	EXPECT_TRUE(vec == Vector3f(1.5f, 2.f, 0.5f));
}

TEST(TrajectoryConstraintsClamp, clamp_ਮੰਜ਼ਿਲ_ZNorm_Half)
{
	// GIVEN: a vector
	Vector3f vec(3, 4, 10);

	// WHEN: we clamp it on Z with half length
	clamp_ਮੰਜ਼ਿਲ_ZNorm(vec, 5.f);

	// THEN: it should be half length
	EXPECT_TRUE(vec == Vector3f(1.5f, 2.f, 5.f));
}

TEST(TrajectoryConstraintsClamp, clamp_ਮੰਜ਼ਿਲ_XYNorm_Zero)
{
	// GIVEN: a vector
	Vector3f vec(3, 4, 1);

	// WHEN: we clamp it on XY with half hypot length
	clamp_ਮੰਜ਼ਿਲ_XYNorm(vec, 0.f);

	// THEN: it should be 0
	EXPECT_TRUE(vec == Vector3f(0.f, 0.f, 0.f));
}

TEST(TrajectoryConstraintsClamp, clamp_ਮੰਜ਼ਿਲ_ZNorm_Zero)
{
	// GIVEN: a vector
	Vector3f vec(3, 4, 1);

	// WHEN: we clamp it on Z with half hypot length
	clamp_ਮੰਜ਼ਿਲ_ZNorm(vec, 0.f);

	// THEN: it should be 0
	EXPECT_TRUE(vec == Vector3f(0.f, 0.f, 0.f));
}

TEST(TrajectoryConstraintsClamp, clamp_ਮੰਜ਼ਿਲ_XYNorm_Vec_Zero)
{
	// GIVEN: a vector
	Vector3f vec(0, 0, 0);

	// WHEN: we clamp it on XY
	clamp_ਮੰਜ਼ਿਲ_XYNorm(vec, 1.f);

	// THEN: it should be 0 still
	EXPECT_TRUE(vec == Vector3f(0.f, 0.f, 0.f));
}

TEST(TrajectoryConstraintsClamp, clamp_ਮੰਜ਼ਿਲ_ZNorm_Vec_Zero)
{
	// GIVEN: a vector
	Vector3f vec(0, 0, 0);

	// WHEN: we clamp it on Z
	clamp_ਮੰਜ਼ਿਲ_ZNorm(vec, 1.f);

	// THEN: it should be 0 still
	EXPECT_TRUE(vec == Vector3f(0.f, 0.f, 0.f));
}

TEST(TrajectoryConstraintsClamp, clamp_ਮੰਜ਼ਿਲ_XYNorm_Vec_Zero_ਮੰਜ਼ਿਲ_Zero)
{
	// GIVEN: a vector
	Vector3f vec(0, 0, 0);

	// WHEN: we clamp it on XY
	clamp_ਮੰਜ਼ਿਲ_XYNorm(vec, 0.f);

	// THEN: it should be 0 still
	EXPECT_TRUE(vec == Vector3f(0.f, 0.f, 0.f));
}

TEST(TrajectoryConstraintsClamp, clamp_ਮੰਜ਼ਿਲ_ZNorm_Vec_Zero_ਮੰਜ਼ਿਲ_Zero)
{
	// GIVEN: a vector
	Vector3f vec(0, 0, 0);

	// WHEN: we clamp it on XY
	clamp_ਮੰਜ਼ਿਲ_ZNorm(vec, 0.f);

	// THEN: it should be 0 still
	EXPECT_TRUE(vec == Vector3f(0.f, 0.f, 0.f));
}
