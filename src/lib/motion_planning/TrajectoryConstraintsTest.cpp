#ضمن <gtest/gtest.h>

#ضمن "TrajectoryConstraints.hpp"

استفاده نطاق ماتریس;
استفاده نطاق math::trajectory;

صنف TrajectoryConstraintsTest : عام ::testing::Test
{
عام:
	VehicleDynamicLimits config;

	Vector3f vehicle_location;
	Vector3f هدف;
	Vector3f بعدی_هدف;

	عائم final_سرعت = 0;

	فراغ مجموعه_به_بالا() override
	{
		config.z_accept_rad = 1.f;
		config.xy_accept_rad = 0.99f;

		config.max_acc_xy = 3.f;
		config.max_jerk = 10.f;

		config.max_سرعت_xy = 10.f;

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
		هدف = Vector3f(20, 10, 5);
		بعدی_هدف = Vector3f(20, 20, 5);
	}
};

TEST_F(TrajectoryConstraintsTest, testStraight)
{
	// GIVEN: 3 waypoints in straight line
	بعدی_هدف = هدف + 2.f * (هدف - vehicle_location);
	هدف = vehicle_location + 0.5f * (بعدی_هدف - vehicle_location);

	// WHEN: we get the speed for straight line travel
	Vector3f waypoints[3] = {vehicle_location, هدف, بعدی_هدف};
	عائم through_سرعت = computeXYSpeedFromWaypoints<3>(waypoints, config);

	// THEN: it should be the same as speed directly to the end point
	Vector3f مستقیم_points[2] = {vehicle_location, بعدی_هدف};
	عائم مستقیم_سرعت = computeXYSpeedFromWaypoints<2>(مستقیم_points, config);

	EXPECT_FLOAT_EQ(through_سرعت, مستقیم_سرعت);
}

TEST_F(TrajectoryConstraintsTest, testStraightNaN)
{
	// GIVEN: 3 waypoints in straight line
	بعدی_هدف = هدف + 2.f * (هدف - vehicle_location);
	هدف = vehicle_location + 0.5f * (بعدی_هدف - vehicle_location);
	بعدی_هدف(0) = NAN;
	بعدی_هدف(1) = NAN;

	// WHEN: we get the speed for points which are NaN afterwards
	Vector3f waypoints[3] = {vehicle_location, هدف, بعدی_هدف};
	عائم through_سرعت = computeXYSpeedFromWaypoints<3>(waypoints, config);

	// THEN: it should be the same as speed to the closer point
	Vector3f مستقیم_points[2] = {vehicle_location, هدف};
	عائم مستقیم_سرعت = computeXYSpeedFromWaypoints<2>(مستقیم_points, config);

	EXPECT_FLOAT_EQ(through_سرعت, مستقیم_سرعت);
}

TEST_F(TrajectoryConstraintsTest, test_Straight_کم_Jerk_Close)
{
	// GIVEN: 3 waypoints in straight line
	بعدی_هدف = هدف + 2.f * (هدف - vehicle_location);
	هدف = vehicle_location + 0.05f * (بعدی_هدف - vehicle_location);
	config.max_jerk = 8.f;

	// WHEN: we get the speed for straight line travel
	Vector3f waypoints[3] = {vehicle_location, هدف, بعدی_هدف};
	عائم through_سرعت = computeXYSpeedFromWaypoints<3>(waypoints, config);

	// THEN: it should be the same as speed directly to the end point
	Vector3f مستقیم_points[2] = {vehicle_location, بعدی_هدف};
	عائم مستقیم_سرعت = computeXYSpeedFromWaypoints<2>(مستقیم_points, config);

	EXPECT_FLOAT_EQ(through_سرعت, مستقیم_سرعت);
}

TEST_F(TrajectoryConstraintsTest, testStraightMidClose)
{
	// GIVEN: 3 waypoints in straight line
	بعدی_هدف = هدف + 2.f * (هدف - vehicle_location);
	هدف = vehicle_location + 0.05f * (بعدی_هدف - vehicle_location);

	// WHEN: we get the speed for straight line travel
	Vector3f waypoints[3] = {vehicle_location, هدف, بعدی_هدف};
	عائم through_سرعت = computeXYSpeedFromWaypoints<3>(waypoints, config);

	// THEN: it should be the same as speed directly to the end point
	Vector3f مستقیم_points[2] = {vehicle_location, بعدی_هدف};
	عائم مستقیم_سرعت = computeXYSpeedFromWaypoints<2>(مستقیم_points, config);

	EXPECT_FLOAT_EQ(through_سرعت, مستقیم_سرعت);
}

TEST_F(TrajectoryConstraintsTest, testStraightMidFar)
{
	// GIVEN: 3 waypoints in straight line
	بعدی_هدف = هدف + 2.f * (هدف - vehicle_location);
	هدف = vehicle_location + 0.95f * (بعدی_هدف - vehicle_location);

	// WHEN: we get the speed for straight line travel
	Vector3f waypoints[3] = {vehicle_location, هدف, بعدی_هدف};
	عائم through_سرعت = computeXYSpeedFromWaypoints<3>(waypoints, config);

	// THEN: it should be the same as speed directly to the end point
	Vector3f مستقیم_points[2] = {vehicle_location, بعدی_هدف};
	عائم مستقیم_سرعت = computeXYSpeedFromWaypoints<2>(مستقیم_points, config);

	EXPECT_FLOAT_EQ(through_سرعت, مستقیم_سرعت);
}


TEST_F(TrajectoryConstraintsTest, test90Angle)
{
	// GIVEN: 3 waypoints in 90 degree angle
	EXPECT_FLOAT_EQ(0.f, (vehicle_location - هدف).dot(هدف - بعدی_هدف));

	// WHEN: we get the speed for travel around the path
	Vector3f waypoints[3] = {vehicle_location, هدف, بعدی_هدف};
	عائم through_سرعت = computeXYSpeedFromWaypoints<3>(waypoints, config);

	// THEN: it should be slightly faster than stopping at the intermediate point
	Vector3f stop_points[2] = {vehicle_location, هدف};
	عائم stop_سرعت = computeXYSpeedFromWaypoints<2>(stop_points, config);

	EXPECT_GT(through_سرعت, stop_سرعت); //faster
	EXPECT_LT(through_سرعت, stop_سرعت * 1.03f); // but less than 3% faster
}

TEST_F(TrajectoryConstraintsTest, test45Angle)
{
	// GIVEN: 3 waypoints in 45 degree angle
	بعدی_هدف = Vector3f(25, 15, 5);

	// WHEN: we get the speed for travel around the path
	Vector3f waypoints[3] = {vehicle_location, هدف, بعدی_هدف};
	عائم through_سرعت = computeXYSpeedFromWaypoints<3>(waypoints, config);

	// THEN: it should be slightly faster than stopping at the intermediate point
	Vector3f stop_points[2] = {vehicle_location, هدف};
	عائم stop_سرعت = computeXYSpeedFromWaypoints<2>(stop_points, config);

	EXPECT_GT(through_سرعت, stop_سرعت * 1.03f); // more than 3% faster
	EXPECT_LT(through_سرعت, stop_سرعت * 1.06f); // but less than 6% faster
}

TEST_F(TrajectoryConstraintsTest, test10Angle)
{
	// GIVEN: 3 waypoints in 10 degree angle
	بعدی_هدف = Vector3f(30, 11.7, 5);

	// WHEN: we get the speed for travel around the path
	Vector3f waypoints[3] = {vehicle_location, هدف, بعدی_هدف};
	عائم through_سرعت = computeXYSpeedFromWaypoints<3>(waypoints, config);

	// THEN: it should be slightly faster than stopping at the intermediate point
	Vector3f stop_points[2] = {vehicle_location, هدف};
	عائم stop_سرعت = computeXYSpeedFromWaypoints<2>(stop_points, config);

	EXPECT_GT(through_سرعت, stop_سرعت * 1.25f); // more than 25% faster
	EXPECT_LT(through_سرعت, stop_سرعت * 1.3f); // but less than 30% faster
}

TEST_F(TrajectoryConstraintsTest, test10_Angle_Far_بعدی)
{
	// GIVEN: 3 waypoints in 10 degree angle, but next waypoint is far
	بعدی_هدف = 2.f * (Vector3f(30, 11.7, 5) - هدف) + هدف;

	// WHEN: we get the speed for travel around the path
	Vector3f far_waypoints[3] = {vehicle_location, هدف, بعدی_هدف};
	عائم far_سرعت = computeXYSpeedFromWaypoints<3>(far_waypoints, config);

	// THEN: it should be the same speed as a closer next waypoint at the same angle, since the bottleneck is the turn
	بعدی_هدف = Vector3f(30, 11.7, 5);
	Vector3f close_waypoints[3] = {vehicle_location, هدف, بعدی_هدف};
	عائم close_سرعت = computeXYSpeedFromWaypoints<3>(close_waypoints, config);

	EXPECT_FLOAT_EQ(far_سرعت, close_سرعت);
}

TEST_F(TrajectoryConstraintsTest, test10_Angle_Close_بعدی)
{
	// GIVEN: 3 waypoints in right angle, but next waypoint is far
	بعدی_هدف = .2f * (Vector3f(30, 11.7, 5) - هدف) + هدف;

	// WHEN: we get the speed for travel around the path
	Vector3f close_waypoints[3] = {vehicle_location, هدف, بعدی_هدف};
	عائم close_سرعت = computeXYSpeedFromWaypoints<3>(close_waypoints, config);

	// THEN: it should be slower than a further next waypoint at the same angle, since the bottleneck is the distance
	بعدی_هدف = Vector3f(30, 11.7, 5);
	Vector3f نرمال_waypoints[3] = {vehicle_location, هدف, بعدی_هدف};
	عائم نرمال_سرعت = computeXYSpeedFromWaypoints<3>(نرمال_waypoints, config);

	EXPECT_LT(close_سرعت, نرمال_سرعت);
}

TEST(TrajectoryConstraintsClamp, clamp_مقصد_XYNorm_No_Effect_Large)
{
	// GIVEN: a short vector
	Vector3f vec(1, 2, 3);

	// WHEN: we clamp it on XY with a long cutoff
	clamp_مقصد_XYNorm(vec, 1000.f);

	// THEN: it shouldn't change
	EXPECT_EQ(vec, Vector3f(1, 2, 3));
}

TEST(TrajectoryConstraintsClamp, clamp_مقصد_ZNorm_No_Effect)
{
	// GIVEN: a short vector
	Vector3f vec(1, 2, 3);

	// WHEN: we clamp it on XY with a long cutoff
	clamp_مقصد_ZNorm(vec, 1000.f);

	// THEN: it shouldn't change
	EXPECT_EQ(vec, Vector3f(1, 2, 3));
}

TEST(TrajectoryConstraintsClamp, clamp_مقصد_XYNorm_No_Effect_Exact)
{
	// GIVEN: a vector
	Vector3f vec(3, 4, 1);

	// WHEN: we clamp it on XY with exact cutoff
	clamp_مقصد_XYNorm(vec, 5.f);

	// THEN: it shouldn't change
	EXPECT_EQ(vec, Vector3f(3, 4, 1));
}

TEST(TrajectoryConstraintsClamp, clamp_مقصد_ZNorm_No_Effect_Exact)
{
	// GIVEN: a vector
	Vector3f vec(3, 4, -1);

	// WHEN: we clamp it on Z with exact cutoff
	clamp_مقصد_ZNorm(vec, 1.f);

	// THEN: it shouldn't change
	EXPECT_EQ(vec, Vector3f(3, 4, -1));
}

TEST(TrajectoryConstraintsClamp, clamp_مقصد_XYNorm_Half)
{
	// GIVEN: a vector
	Vector3f vec(3, 4, 1);

	// WHEN: we clamp it on XY with half hypot length
	clamp_مقصد_XYNorm(vec, 2.5f);

	// THEN: it should be half length
	EXPECT_TRUE(vec == Vector3f(1.5f, 2.f, 0.5f));
}

TEST(TrajectoryConstraintsClamp, clamp_مقصد_ZNorm_Half)
{
	// GIVEN: a vector
	Vector3f vec(3, 4, 10);

	// WHEN: we clamp it on Z with half length
	clamp_مقصد_ZNorm(vec, 5.f);

	// THEN: it should be half length
	EXPECT_TRUE(vec == Vector3f(1.5f, 2.f, 5.f));
}

TEST(TrajectoryConstraintsClamp, clamp_مقصد_XYNorm_Zero)
{
	// GIVEN: a vector
	Vector3f vec(3, 4, 1);

	// WHEN: we clamp it on XY with half hypot length
	clamp_مقصد_XYNorm(vec, 0.f);

	// THEN: it should be 0
	EXPECT_TRUE(vec == Vector3f(0.f, 0.f, 0.f));
}

TEST(TrajectoryConstraintsClamp, clamp_مقصد_ZNorm_Zero)
{
	// GIVEN: a vector
	Vector3f vec(3, 4, 1);

	// WHEN: we clamp it on Z with half hypot length
	clamp_مقصد_ZNorm(vec, 0.f);

	// THEN: it should be 0
	EXPECT_TRUE(vec == Vector3f(0.f, 0.f, 0.f));
}

TEST(TrajectoryConstraintsClamp, clamp_مقصد_XYNorm_Vec_Zero)
{
	// GIVEN: a vector
	Vector3f vec(0, 0, 0);

	// WHEN: we clamp it on XY
	clamp_مقصد_XYNorm(vec, 1.f);

	// THEN: it should be 0 still
	EXPECT_TRUE(vec == Vector3f(0.f, 0.f, 0.f));
}

TEST(TrajectoryConstraintsClamp, clamp_مقصد_ZNorm_Vec_Zero)
{
	// GIVEN: a vector
	Vector3f vec(0, 0, 0);

	// WHEN: we clamp it on Z
	clamp_مقصد_ZNorm(vec, 1.f);

	// THEN: it should be 0 still
	EXPECT_TRUE(vec == Vector3f(0.f, 0.f, 0.f));
}

TEST(TrajectoryConstraintsClamp, clamp_مقصد_XYNorm_Vec_Zero_مقصد_Zero)
{
	// GIVEN: a vector
	Vector3f vec(0, 0, 0);

	// WHEN: we clamp it on XY
	clamp_مقصد_XYNorm(vec, 0.f);

	// THEN: it should be 0 still
	EXPECT_TRUE(vec == Vector3f(0.f, 0.f, 0.f));
}

TEST(TrajectoryConstraintsClamp, clamp_مقصد_ZNorm_Vec_Zero_مقصد_Zero)
{
	// GIVEN: a vector
	Vector3f vec(0, 0, 0);

	// WHEN: we clamp it on XY
	clamp_مقصد_ZNorm(vec, 0.f);

	// THEN: it should be 0 still
	EXPECT_TRUE(vec == Vector3f(0.f, 0.f, 0.f));
}
