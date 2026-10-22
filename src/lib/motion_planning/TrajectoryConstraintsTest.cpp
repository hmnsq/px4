#ضمن <gtest/gtest.h>

#ضمن "TrajectoryConstraints.hpp"

کارول نطاق ماتریکس;
کارول نطاق math::trajectory;

صنف TrajectoryConstraintsTest : عام ::testing::Test
{
عام:
	VehicleDynamicLimits config;

	Vector3f vehicle_location;
	Vector3f موخه;
	Vector3f بل_موخه;

	عائم final_چټکتیا = 0;

	فراغ سېټ_پورته() override
	{
		config.z_accept_rad = 1.f;
		config.xy_accept_rad = 0.99f;

		config.max_acc_xy = 3.f;
		config.max_jerk = 10.f;

		config.max_چټکتیا_xy = 10.f;

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
		موخه = Vector3f(20, 10, 5);
		بل_موخه = Vector3f(20, 20, 5);
	}
};

TEST_F(TrajectoryConstraintsTest, testStraight)
{
	// GIVEN: 3 waypoints in straight line
	بل_موخه = موخه + 2.f * (موخه - vehicle_location);
	موخه = vehicle_location + 0.5f * (بل_موخه - vehicle_location);

	// WHEN: we get the speed for straight line travel
	Vector3f waypoints[3] = {vehicle_location, موخه, بل_موخه};
	عائم through_چټکتیا = computeXYSpeedFromWaypoints<3>(waypoints, config);

	// THEN: it should be the same as speed directly to the end point
	Vector3f مستقیم_points[2] = {vehicle_location, بل_موخه};
	عائم مستقیم_چټکتیا = computeXYSpeedFromWaypoints<2>(مستقیم_points, config);

	EXPECT_FLOAT_EQ(through_چټکتیا, مستقیم_چټکتیا);
}

TEST_F(TrajectoryConstraintsTest, testStraightNaN)
{
	// GIVEN: 3 waypoints in straight line
	بل_موخه = موخه + 2.f * (موخه - vehicle_location);
	موخه = vehicle_location + 0.5f * (بل_موخه - vehicle_location);
	بل_موخه(0) = NAN;
	بل_موخه(1) = NAN;

	// WHEN: we get the speed for points which are NaN afterwards
	Vector3f waypoints[3] = {vehicle_location, موخه, بل_موخه};
	عائم through_چټکتیا = computeXYSpeedFromWaypoints<3>(waypoints, config);

	// THEN: it should be the same as speed to the closer point
	Vector3f مستقیم_points[2] = {vehicle_location, موخه};
	عائم مستقیم_چټکتیا = computeXYSpeedFromWaypoints<2>(مستقیم_points, config);

	EXPECT_FLOAT_EQ(through_چټکتیا, مستقیم_چټکتیا);
}

TEST_F(TrajectoryConstraintsTest, test_Straight_ټیټ_Jerk_Close)
{
	// GIVEN: 3 waypoints in straight line
	بل_موخه = موخه + 2.f * (موخه - vehicle_location);
	موخه = vehicle_location + 0.05f * (بل_موخه - vehicle_location);
	config.max_jerk = 8.f;

	// WHEN: we get the speed for straight line travel
	Vector3f waypoints[3] = {vehicle_location, موخه, بل_موخه};
	عائم through_چټکتیا = computeXYSpeedFromWaypoints<3>(waypoints, config);

	// THEN: it should be the same as speed directly to the end point
	Vector3f مستقیم_points[2] = {vehicle_location, بل_موخه};
	عائم مستقیم_چټکتیا = computeXYSpeedFromWaypoints<2>(مستقیم_points, config);

	EXPECT_FLOAT_EQ(through_چټکتیا, مستقیم_چټکتیا);
}

TEST_F(TrajectoryConstraintsTest, testStraightMidClose)
{
	// GIVEN: 3 waypoints in straight line
	بل_موخه = موخه + 2.f * (موخه - vehicle_location);
	موخه = vehicle_location + 0.05f * (بل_موخه - vehicle_location);

	// WHEN: we get the speed for straight line travel
	Vector3f waypoints[3] = {vehicle_location, موخه, بل_موخه};
	عائم through_چټکتیا = computeXYSpeedFromWaypoints<3>(waypoints, config);

	// THEN: it should be the same as speed directly to the end point
	Vector3f مستقیم_points[2] = {vehicle_location, بل_موخه};
	عائم مستقیم_چټکتیا = computeXYSpeedFromWaypoints<2>(مستقیم_points, config);

	EXPECT_FLOAT_EQ(through_چټکتیا, مستقیم_چټکتیا);
}

TEST_F(TrajectoryConstraintsTest, testStraightMidFar)
{
	// GIVEN: 3 waypoints in straight line
	بل_موخه = موخه + 2.f * (موخه - vehicle_location);
	موخه = vehicle_location + 0.95f * (بل_موخه - vehicle_location);

	// WHEN: we get the speed for straight line travel
	Vector3f waypoints[3] = {vehicle_location, موخه, بل_موخه};
	عائم through_چټکتیا = computeXYSpeedFromWaypoints<3>(waypoints, config);

	// THEN: it should be the same as speed directly to the end point
	Vector3f مستقیم_points[2] = {vehicle_location, بل_موخه};
	عائم مستقیم_چټکتیا = computeXYSpeedFromWaypoints<2>(مستقیم_points, config);

	EXPECT_FLOAT_EQ(through_چټکتیا, مستقیم_چټکتیا);
}


TEST_F(TrajectoryConstraintsTest, test90Angle)
{
	// GIVEN: 3 waypoints in 90 degree angle
	EXPECT_FLOAT_EQ(0.f, (vehicle_location - موخه).dot(موخه - بل_موخه));

	// WHEN: we get the speed for travel around the path
	Vector3f waypoints[3] = {vehicle_location, موخه, بل_موخه};
	عائم through_چټکتیا = computeXYSpeedFromWaypoints<3>(waypoints, config);

	// THEN: it should be slightly faster than stopping at the intermediate point
	Vector3f stop_points[2] = {vehicle_location, موخه};
	عائم stop_چټکتیا = computeXYSpeedFromWaypoints<2>(stop_points, config);

	EXPECT_GT(through_چټکتیا, stop_چټکتیا); //faster
	EXPECT_LT(through_چټکتیا, stop_چټکتیا * 1.03f); // but less than 3% faster
}

TEST_F(TrajectoryConstraintsTest, test45Angle)
{
	// GIVEN: 3 waypoints in 45 degree angle
	بل_موخه = Vector3f(25, 15, 5);

	// WHEN: we get the speed for travel around the path
	Vector3f waypoints[3] = {vehicle_location, موخه, بل_موخه};
	عائم through_چټکتیا = computeXYSpeedFromWaypoints<3>(waypoints, config);

	// THEN: it should be slightly faster than stopping at the intermediate point
	Vector3f stop_points[2] = {vehicle_location, موخه};
	عائم stop_چټکتیا = computeXYSpeedFromWaypoints<2>(stop_points, config);

	EXPECT_GT(through_چټکتیا, stop_چټکتیا * 1.03f); // more than 3% faster
	EXPECT_LT(through_چټکتیا, stop_چټکتیا * 1.06f); // but less than 6% faster
}

TEST_F(TrajectoryConstraintsTest, test10Angle)
{
	// GIVEN: 3 waypoints in 10 degree angle
	بل_موخه = Vector3f(30, 11.7, 5);

	// WHEN: we get the speed for travel around the path
	Vector3f waypoints[3] = {vehicle_location, موخه, بل_موخه};
	عائم through_چټکتیا = computeXYSpeedFromWaypoints<3>(waypoints, config);

	// THEN: it should be slightly faster than stopping at the intermediate point
	Vector3f stop_points[2] = {vehicle_location, موخه};
	عائم stop_چټکتیا = computeXYSpeedFromWaypoints<2>(stop_points, config);

	EXPECT_GT(through_چټکتیا, stop_چټکتیا * 1.25f); // more than 25% faster
	EXPECT_LT(through_چټکتیا, stop_چټکتیا * 1.3f); // but less than 30% faster
}

TEST_F(TrajectoryConstraintsTest, test10_Angle_Far_بل)
{
	// GIVEN: 3 waypoints in 10 degree angle, but next waypoint is far
	بل_موخه = 2.f * (Vector3f(30, 11.7, 5) - موخه) + موخه;

	// WHEN: we get the speed for travel around the path
	Vector3f far_waypoints[3] = {vehicle_location, موخه, بل_موخه};
	عائم far_چټکتیا = computeXYSpeedFromWaypoints<3>(far_waypoints, config);

	// THEN: it should be the same speed as a closer next waypoint at the same angle, since the bottleneck is the turn
	بل_موخه = Vector3f(30, 11.7, 5);
	Vector3f close_waypoints[3] = {vehicle_location, موخه, بل_موخه};
	عائم close_چټکتیا = computeXYSpeedFromWaypoints<3>(close_waypoints, config);

	EXPECT_FLOAT_EQ(far_چټکتیا, close_چټکتیا);
}

TEST_F(TrajectoryConstraintsTest, test10_Angle_Close_بل)
{
	// GIVEN: 3 waypoints in right angle, but next waypoint is far
	بل_موخه = .2f * (Vector3f(30, 11.7, 5) - موخه) + موخه;

	// WHEN: we get the speed for travel around the path
	Vector3f close_waypoints[3] = {vehicle_location, موخه, بل_موخه};
	عائم close_چټکتیا = computeXYSpeedFromWaypoints<3>(close_waypoints, config);

	// THEN: it should be slower than a further next waypoint at the same angle, since the bottleneck is the distance
	بل_موخه = Vector3f(30, 11.7, 5);
	Vector3f نورمال_waypoints[3] = {vehicle_location, موخه, بل_موخه};
	عائم نورمال_چټکتیا = computeXYSpeedFromWaypoints<3>(نورمال_waypoints, config);

	EXPECT_LT(close_چټکتیا, نورمال_چټکتیا);
}

TEST(TrajectoryConstraintsClamp, clamp_منزل_XYNorm_No_Effect_Large)
{
	// GIVEN: a short vector
	Vector3f vec(1, 2, 3);

	// WHEN: we clamp it on XY with a long cutoff
	clamp_منزل_XYNorm(vec, 1000.f);

	// THEN: it shouldn't change
	EXPECT_EQ(vec, Vector3f(1, 2, 3));
}

TEST(TrajectoryConstraintsClamp, clamp_منزل_ZNorm_No_Effect)
{
	// GIVEN: a short vector
	Vector3f vec(1, 2, 3);

	// WHEN: we clamp it on XY with a long cutoff
	clamp_منزل_ZNorm(vec, 1000.f);

	// THEN: it shouldn't change
	EXPECT_EQ(vec, Vector3f(1, 2, 3));
}

TEST(TrajectoryConstraintsClamp, clamp_منزل_XYNorm_No_Effect_Exact)
{
	// GIVEN: a vector
	Vector3f vec(3, 4, 1);

	// WHEN: we clamp it on XY with exact cutoff
	clamp_منزل_XYNorm(vec, 5.f);

	// THEN: it shouldn't change
	EXPECT_EQ(vec, Vector3f(3, 4, 1));
}

TEST(TrajectoryConstraintsClamp, clamp_منزل_ZNorm_No_Effect_Exact)
{
	// GIVEN: a vector
	Vector3f vec(3, 4, -1);

	// WHEN: we clamp it on Z with exact cutoff
	clamp_منزل_ZNorm(vec, 1.f);

	// THEN: it shouldn't change
	EXPECT_EQ(vec, Vector3f(3, 4, -1));
}

TEST(TrajectoryConstraintsClamp, clamp_منزل_XYNorm_Half)
{
	// GIVEN: a vector
	Vector3f vec(3, 4, 1);

	// WHEN: we clamp it on XY with half hypot length
	clamp_منزل_XYNorm(vec, 2.5f);

	// THEN: it should be half length
	EXPECT_TRUE(vec == Vector3f(1.5f, 2.f, 0.5f));
}

TEST(TrajectoryConstraintsClamp, clamp_منزل_ZNorm_Half)
{
	// GIVEN: a vector
	Vector3f vec(3, 4, 10);

	// WHEN: we clamp it on Z with half length
	clamp_منزل_ZNorm(vec, 5.f);

	// THEN: it should be half length
	EXPECT_TRUE(vec == Vector3f(1.5f, 2.f, 5.f));
}

TEST(TrajectoryConstraintsClamp, clamp_منزل_XYNorm_Zero)
{
	// GIVEN: a vector
	Vector3f vec(3, 4, 1);

	// WHEN: we clamp it on XY with half hypot length
	clamp_منزل_XYNorm(vec, 0.f);

	// THEN: it should be 0
	EXPECT_TRUE(vec == Vector3f(0.f, 0.f, 0.f));
}

TEST(TrajectoryConstraintsClamp, clamp_منزل_ZNorm_Zero)
{
	// GIVEN: a vector
	Vector3f vec(3, 4, 1);

	// WHEN: we clamp it on Z with half hypot length
	clamp_منزل_ZNorm(vec, 0.f);

	// THEN: it should be 0
	EXPECT_TRUE(vec == Vector3f(0.f, 0.f, 0.f));
}

TEST(TrajectoryConstraintsClamp, clamp_منزل_XYNorm_Vec_Zero)
{
	// GIVEN: a vector
	Vector3f vec(0, 0, 0);

	// WHEN: we clamp it on XY
	clamp_منزل_XYNorm(vec, 1.f);

	// THEN: it should be 0 still
	EXPECT_TRUE(vec == Vector3f(0.f, 0.f, 0.f));
}

TEST(TrajectoryConstraintsClamp, clamp_منزل_ZNorm_Vec_Zero)
{
	// GIVEN: a vector
	Vector3f vec(0, 0, 0);

	// WHEN: we clamp it on Z
	clamp_منزل_ZNorm(vec, 1.f);

	// THEN: it should be 0 still
	EXPECT_TRUE(vec == Vector3f(0.f, 0.f, 0.f));
}

TEST(TrajectoryConstraintsClamp, clamp_منزل_XYNorm_Vec_Zero_منزل_Zero)
{
	// GIVEN: a vector
	Vector3f vec(0, 0, 0);

	// WHEN: we clamp it on XY
	clamp_منزل_XYNorm(vec, 0.f);

	// THEN: it should be 0 still
	EXPECT_TRUE(vec == Vector3f(0.f, 0.f, 0.f));
}

TEST(TrajectoryConstraintsClamp, clamp_منزل_ZNorm_Vec_Zero_منزل_Zero)
{
	// GIVEN: a vector
	Vector3f vec(0, 0, 0);

	// WHEN: we clamp it on XY
	clamp_منزل_ZNorm(vec, 0.f);

	// THEN: it should be 0 still
	EXPECT_TRUE(vec == Vector3f(0.f, 0.f, 0.f));
}
