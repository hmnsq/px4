/****************************************************************************
 *
 *   Copyright (C) 2019 PX4 Development Team. All rights reserved.
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

#ضمن <gtest/gtest.h>
#ضمن "CollisionPrevention.hpp"

کارول نطاق ماتریکس;

// to run: make tests TESTFILTER=CollisionPrevention
hrt_abstime mocked_وخت = 0;
ثابت uint bin_اندازه = CollisionPrevention::BIN_اندازه;
ثابت uint bin_شمېر = CollisionPrevention::BIN_شمېر;

صنف CollisionPreventionTest : عام ::testing::Test
{
عام:
	فراغ سېټ_پورته() override
	{
		param_control_autosave(خطأ);
		param_reset_all();
	}
};

صنف TestCollisionPrevention : عام CollisionPrevention
{
عام:
	TestCollisionPrevention() : CollisionPrevention(تش_اشاره) {}
	فراغ params_بدل_شوی() {CollisionPrevention::updateParamsImpl();}
	obstacle_واټن_s &getObstacleMap() {أرجع obstacle_map_body_چوکاټ;}
	فراغ test_جمع_کړه_واټن_Sensor_معلومات(واټن_sensor_s &واټن_sensor, ثابت Quatf &attitude)
	{
		جمع_کړه_واټن_Sensor_معلومات(واټن_sensor, attitude);
	}
	فراغ test_جمع_کړه_Obstacle_Sensor_معلومات(ثابت obstacle_واټن_s &obstacle, ثابت عائم vehicle_yaw)
	{
		جمع_کړه_Obstacle_Sensor_معلومات(obstacle, vehicle_yaw);
	}
	فراغ test_adaptSetpointDirection(Vector2f &setpoint_dir, صحيح &setpoint_شاخص_2,
					 عائم vehicle_yaw_angle_rad)
	{
		_adaptSetpointDirection(setpoint_dir, setpoint_شاخص_2, vehicle_yaw_angle_rad);
	}
	منطقي test_enter_معلومات(صحيح map_شاخص, عائم sensor_range, عائم sensor_reading)
	{
		أرجع enter_معلومات(map_شاخص, sensor_range, sensor_reading);
	}
};

صنف TestTimingCollisionPrevention : عام TestCollisionPrevention
{
عام:
	TestTimingCollisionPrevention() : TestCollisionPrevention() {}
محمي:
	hrt_abstime get_وخت() override
	{
		أرجع mocked_وخت;
	}

	hrt_abstime get_Elapsed_وخت(ثابت hrt_abstime *ptr) override
	{
		أرجع mocked_وخت - *ptr;
	}
};

TEST_F(CollisionPreventionTest, instantiation) { CollisionPrevention cp(تش_اشاره); }

TEST_F(CollisionPreventionTest, behaviorOff)
{
	// GIVEN: a simple setup condition
	TestCollisionPrevention cp;

	// THEN: the collision prevention should be turned off by default
	EXPECT_FALSE(cp.is_فعال());
}

TEST_F(CollisionPreventionTest, no_Sensor_معلومات)
{
	// GIVEN: a simple setup condition
	TestCollisionPrevention cp;
	Vector2f original_setpoint(10, 0);
	Vector2f curr_vel(2, 0);

	// AND: a parameter handle
	param_t param = param_handle(px4::params::CP_DIST);

	// WHEN: we set the parameter check then apply the setpoint modification
	عائم value = 10; // try to keep 10m away from obstacles
	param_سېټ(param, &value);
	cp.params_بدل_شوی();

	Vector2f modified_setpoint = original_setpoint;
	cp.modifySetpoint(modified_setpoint, curr_vel);

	// THEN: collision prevention should be enabled and limit the speed to zero
	EXPECT_TRUE(cp.is_فعال());
	EXPECT_FLOAT_EQ(0.f, modified_setpoint.norm());
}

TEST_F(CollisionPreventionTest, test_Behavior_On_With_Obstacle_پیغام)
{
	// GIVEN: a simple setup condition
	TestCollisionPrevention cp;
	Vector2f original_setpoint1(10, 0);
	Vector2f original_setpoint2(-10, 0);
	Vector2f curr_vel(2, 0);
	vehicle_attitude_s attitude;
	attitude.timestamp = hrt_absolute_وخت();
	attitude.q[0] = 1.0f;
	attitude.q[1] = 0.0f;
	attitude.q[2] = 0.0f;
	attitude.q[3] = 0.0f;

	// AND: a parameter handle
	param_t param1 = param_handle(px4::params::CP_DIST);
	عائم value1 = 10; // try to keep 10m distance
	param_سېټ(param1, &value1);
	param_t param2 = param_handle(px4::params::CP_GUIDE_ANG);
	عائم value2 = 0; // dont guide sideways
	param_سېټ(param2, &value2);
	cp.params_بدل_شوی();

	// AND: an obstacle message
	obstacle_واټن_s پیغام_3;
	memset(&پیغام_3, 0xDEAD, حجم(پیغام_3));
	پیغام_3.چوکاټ = پیغام_3.MAV_چوکاټ_GLOBAL; //north aligned
	پیغام_3.min_واټن = 100;
	پیغام_3.max_واټن = 10000;
	پیغام_3.angle_دننه_واټن = 0;
	پیغام_3.timestamp = hrt_absolute_وخت();
	صحيح distances_array_اندازه = حجم(پیغام_3.distances) / حجم(پیغام_3.distances[0]);
	پیغام_3.increment = 360 / distances_array_اندازه;

	لكل (صحيح i = 0; i < distances_array_اندازه; i++) {
		إذا (i < 10) {
			پیغام_3.distances[i] = 101;

		} وإلا {
			پیغام_3.distances[i] = 10001;
		}
	}

	// WHEN: we publish the message and set the parameter and then run the setpoint modification
	orb_advert_t obstacle_واټن_pub = orb_advertise(ORB_ID(obstacle_واټن), &پیغام_3);
	orb_advert_t vehicle_attitude_pub = orb_advertise(ORB_ID(vehicle_attitude), &attitude);
	orb_publish(ORB_ID(obstacle_واټن), obstacle_واټن_pub, &پیغام_3);
	orb_publish(ORB_ID(vehicle_attitude), vehicle_attitude_pub, &attitude);
	Vector2f modified_setpoint1 = original_setpoint1;
	Vector2f modified_setpoint2 = original_setpoint2;
	cp.modifySetpoint(modified_setpoint1, curr_vel);
	cp.modifySetpoint(modified_setpoint2, curr_vel);
	orb_unadvertise(obstacle_واټن_pub);
	orb_unadvertise(vehicle_attitude_pub);

	// THEN: the internal map should know the obstacle
	// case 1: the acceleration setpoint should be negative as its pushing you away from the obstacle, and sideways acceleration should be low
	// case 2: the acceleration setpoint should be lower
	EXPECT_FLOAT_EQ(cp.getObstacleMap().min_واټن, 100);
	EXPECT_FLOAT_EQ(cp.getObstacleMap().max_واټن, 10000);
	EXPECT_GT(0.f, modified_setpoint1(0)) << modified_setpoint1(0);
	EXPECT_EQ(0.f, fabsf(modified_setpoint1(1))) << modified_setpoint1(1);
	EXPECT_GT(0.f, modified_setpoint2(0))  << original_setpoint2(0);
	EXPECT_EQ(0.f, fabsf(modified_setpoint2(1))) << modified_setpoint2(1);
}

TEST_F(CollisionPreventionTest, test_Behavior_On_With_واټن_پیغام)
{
	// GIVEN: a simple setup condition
	TestCollisionPrevention cp;
	Vector2f original_setpoint1(10, 0);
	Vector2f original_setpoint2(-10, 0);
	Vector2f curr_vel(2, 0);
	vehicle_attitude_s attitude;
	attitude.timestamp = hrt_absolute_وخت();
	attitude.q[0] = 1.0f;
	attitude.q[1] = 0.0f;
	attitude.q[2] = 0.0f;
	attitude.q[3] = 0.0f;

	// AND: a parameter handle
	param_t param = param_handle(px4::params::CP_DIST);
	عائم value = 10; // try to keep 10m distance
	param_سېټ(param, &value);
	cp.params_بدل_شوی();

	// AND: an obstacle message
	واټن_sensor_s پیغام_3;
	پیغام_3.timestamp = hrt_absolute_وخت();
	پیغام_3.min_واټن = 1.f;
	پیغام_3.max_واټن = 100.f;
	پیغام_3.current_واټن = 1.1f;

	پیغام_3.variance = 0.1f;
	پیغام_3.signal_quality = 100;
	پیغام_3.type = واټن_sensor_s::MAV_واټن_SENSOR_LASER;
	پیغام_3.orientation = واټن_sensor_s::ROTATION_FORWARD_FACING;
	پیغام_3.h_fov = math::radians(50.f);
	پیغام_3.v_fov = math::radians(30.f);

	// WHEN: we publish the message and set the parameter and then run the setpoint modification
	orb_advert_t واټن_sensor_pub = orb_advertise(ORB_ID(واټن_sensor), &پیغام_3);
	orb_advert_t vehicle_attitude_pub = orb_advertise(ORB_ID(vehicle_attitude), &attitude);
	orb_publish(ORB_ID(واټن_sensor), واټن_sensor_pub, &پیغام_3);
	orb_publish(ORB_ID(vehicle_attitude), vehicle_attitude_pub, &attitude);

	//WHEN:  We run the setpoint modification
	Vector2f modified_setpoint1 = original_setpoint1;
	Vector2f modified_setpoint2 = original_setpoint2;
	cp.modifySetpoint(modified_setpoint1, curr_vel);
	cp.modifySetpoint(modified_setpoint2, curr_vel);
	orb_unadvertise(واټن_sensor_pub);
	orb_unadvertise(vehicle_attitude_pub);

	// THEN: the internal map should know the obstacle
	// case 1: the acceleration setpoint should be negative as its pushing you away from the obstacle, and sideways acceleration should be low
	// case 2: the acceleration setpoint should be lower
	EXPECT_FLOAT_EQ(cp.getObstacleMap().min_واټن, 100);
	EXPECT_FLOAT_EQ(cp.getObstacleMap().max_واټن, 10000);

	EXPECT_FLOAT_EQ(cp.getObstacleMap().min_واټن, 100);
	EXPECT_FLOAT_EQ(cp.getObstacleMap().max_واټن, 10000);

	EXPECT_GT(0.f, modified_setpoint1(0)) << modified_setpoint1(0);
	EXPECT_EQ(0.f, fabsf(modified_setpoint1(1))) << modified_setpoint1(1);
	EXPECT_GT(0.f, modified_setpoint2(0))  << original_setpoint2(0);
	EXPECT_EQ(0.f, fabsf(modified_setpoint2(1))) << modified_setpoint2(1);
}

TEST_F(CollisionPreventionTest, test_Purge_Old_معلومات)
{
	// GIVEN: a simple setup condition
	TestTimingCollisionPrevention cp;
	hrt_abstime پیل_وخت = hrt_absolute_وخت();
	mocked_وخت = پیل_وخت;
	Vector2f original_setpoint(10, 0);
	Vector2f curr_vel(2, 0);
	vehicle_attitude_s attitude;
	attitude.timestamp = پیل_وخت;
	attitude.q[0] = 1.0f;
	attitude.q[1] = 0.0f;
	attitude.q[2] = 0.0f;
	attitude.q[3] = 0.0f;

	// AND: a parameter handle
	param_t param = param_handle(px4::params::CP_DIST);
	عائم value = 1; // try to keep 10m distance
	param_سېټ(param, &value);
	cp.params_بدل_شوی();

	// AND: an obstacle message
	obstacle_واټن_s پیغام_3, پیغام_ورک_معلومات;
	memset(&پیغام_3, 0xDEAD, حجم(پیغام_3));
	پیغام_3.چوکاټ = پیغام_3.MAV_چوکاټ_GLOBAL; //north aligned
	پیغام_3.min_واټن = 100;
	پیغام_3.max_واټن = 10000;
	پیغام_3.angle_دننه_واټن = 0;
	پیغام_3.timestamp = پیل_وخت;
	صحيح distances_array_اندازه = حجم(پیغام_3.distances) / حجم(پیغام_3.distances[0]);
	پیغام_3.increment = 360 / distances_array_اندازه;
	پیغام_ورک_معلومات = پیغام_3;

	لكل (صحيح i = 0; i < distances_array_اندازه; i++) {
		إذا (i < 10) {
			پیغام_3.distances[i] = 10001;
			پیغام_ورک_معلومات.distances[i] = UINT16_MAX;

		} وإلا إذا (i > 15 && i < 18) {
			پیغام_3.distances[i] = 10001;
			پیغام_ورک_معلومات.distances[i] = 10001;

		} وإلا {
			پیغام_3.distances[i] = UINT16_MAX;
			پیغام_ورک_معلومات.distances[i] = UINT16_MAX;
		}
	}

	// WHEN: we publish the message and set the parameter and then run the setpoint modification
	orb_advert_t obstacle_واټن_pub = orb_advertise(ORB_ID(obstacle_واټن), &پیغام_3);
	orb_advert_t vehicle_attitude_pub = orb_advertise(ORB_ID(vehicle_attitude), &attitude);
	orb_publish(ORB_ID(obstacle_واټن), obstacle_واټن_pub, &پیغام_3);
	orb_publish(ORB_ID(vehicle_attitude), vehicle_attitude_pub, &attitude);

	لكل (صحيح i = 0; i < 10; i++) {
		Vector2f modified_setpoint = original_setpoint;
		cp.modifySetpoint(modified_setpoint, curr_vel);

		mocked_وخت = mocked_وخت + 100000; //advance time by 0.1 seconds
		پیغام_ورک_معلومات.timestamp = mocked_وخت;
		orb_publish(ORB_ID(obstacle_واټن), obstacle_واټن_pub, &پیغام_ورک_معلومات);

		//at iteration 8 change the CP_GO_NO_DATA to True
		إذا (i == 8) {
			param_t param_allow = param_handle(px4::params::CP_GO_NO_معلومات);
			عائم ارزښت_allow = 1;
			param_سېټ(param_allow, &ارزښت_allow);
			cp.params_بدل_شوی();
		}

		إذا (i < 6) {
			// THEN: If the data is new enough, the velocity setpoint should stay the same as the input
			// Note: direction will change slightly due to guidance
			EXPECT_FLOAT_EQ(original_setpoint.norm(), modified_setpoint.norm());

		} وإلا {
			// THEN: If the data is expired, the velocity setpoint should be cut down to zero because there is no data
			//(even if CP_GO_NO_DATA is set to true, because we once had data in those bins and now lost the sensor)
			EXPECT_FLOAT_EQ(0.f, modified_setpoint.norm()) << modified_setpoint(0) << "," << modified_setpoint(1);
		}
	}

	orb_unadvertise(obstacle_واټن_pub);
	orb_unadvertise(vehicle_attitude_pub);
}

TEST_F(CollisionPreventionTest, test_No_Range_معلومات)
{
	// GIVEN: a simple setup condition
	TestTimingCollisionPrevention cp;
	hrt_abstime پیل_وخت = hrt_absolute_وخت();
	mocked_وخت = پیل_وخت;
	Vector2f original_setpoint(10, 0);
	Vector2f curr_vel(2, 0);
	vehicle_attitude_s attitude;
	attitude.timestamp = پیل_وخت;
	attitude.q[0] = 1.0f;
	attitude.q[1] = 0.0f;
	attitude.q[2] = 0.0f;
	attitude.q[3] = 0.0f;

	// AND: a parameter handle
	param_t param = param_handle(px4::params::CP_DIST);
	عائم value = 10; // try to keep 10m distance
	param_سېټ(param, &value);
	cp.params_بدل_شوی();

	// AND: an obstacle message without any obstacle
	obstacle_واټن_s پیغام_3;
	memset(&پیغام_3, 0xDEAD, حجم(پیغام_3));
	پیغام_3.چوکاټ = پیغام_3.MAV_چوکاټ_GLOBAL; //north aligned
	پیغام_3.min_واټن = 100;
	پیغام_3.max_واټن = 10000;
	پیغام_3.angle_دننه_واټن = 0;
	پیغام_3.timestamp = پیل_وخت;
	صحيح distances_array_اندازه = حجم(پیغام_3.distances) / حجم(پیغام_3.distances[0]);
	پیغام_3.increment = 360 / distances_array_اندازه;

	لكل (صحيح i = 0; i < distances_array_اندازه; i++) {
		پیغام_3.distances[i] = 9000;
	}


	// WHEN: we publish the message and set the parameter and then run the setpoint modification
	orb_advert_t obstacle_واټن_pub = orb_advertise(ORB_ID(obstacle_واټن), &پیغام_3);
	orb_advert_t vehicle_attitude_pub = orb_advertise(ORB_ID(vehicle_attitude), &attitude);
	orb_publish(ORB_ID(obstacle_واټن), obstacle_واټن_pub, &پیغام_3);
	orb_publish(ORB_ID(vehicle_attitude), vehicle_attitude_pub, &attitude);

	لكل (صحيح i = 0; i < 10; i++) {
		Vector2f modified_setpoint = original_setpoint;
		cp.modifySetpoint(modified_setpoint, curr_vel);

		//advance time by 0.1 seconds but no new message comes in
		mocked_وخت = mocked_وخت + 100000;

		إذا (i < 5) {
			// THEN: If the data is new enough, the velocity setpoint should stay the same as the input
			// Note: direction will change slightly due to guidance
			EXPECT_FLOAT_EQ(original_setpoint.norm(), modified_setpoint.norm());

		} وإلا {
			// THEN: If the data is expired, the velocity setpoint should be cut down to zero because there is no data
			EXPECT_FLOAT_EQ(0.f, modified_setpoint.norm()) << modified_setpoint(0) << "," << modified_setpoint(1);
		}
	}

	orb_unadvertise(obstacle_واټن_pub);
	orb_unadvertise(vehicle_attitude_pub);
}

TEST_F(CollisionPreventionTest, noBias)
{
	// GIVEN: a simple setup condition
	TestCollisionPrevention cp;
	Vector2f original_setpoint(10, 0);
	Vector2f curr_vel(2, 0);

	// AND: a parameter handle
	param_t param = param_handle(px4::params::CP_DIST);
	عائم value = 2; // try to keep 2m distance
	param_سېټ(param, &value);
	cp.params_بدل_شوی();

	// AND: an obstacle message
	obstacle_واټن_s پیغام_3;
	memset(&پیغام_3, 0xDEAD, حجم(پیغام_3));
	پیغام_3.min_واټن = 100;
	پیغام_3.max_واټن = 2000;
	پیغام_3.timestamp = hrt_absolute_وخت();
	پیغام_3.چوکاټ = پیغام_3.MAV_چوکاټ_GLOBAL; //north aligned
	صحيح distances_array_اندازه = حجم(پیغام_3.distances) / حجم(پیغام_3.distances[0]);
	پیغام_3.increment = 360 / distances_array_اندازه;

	لكل (صحيح i = 0; i < distances_array_اندازه; i++) {
		پیغام_3.distances[i] = 700;
	}

	// WHEN: we publish the message and set the parameter and then run the setpoint modification
	orb_advert_t obstacle_واټن_pub = orb_advertise(ORB_ID(obstacle_واټن), &پیغام_3);
	orb_publish(ORB_ID(obstacle_واټن), obstacle_واټن_pub, &پیغام_3);
	Vector2f modified_setpoint = original_setpoint;
	cp.modifySetpoint(modified_setpoint, curr_vel);
	orb_unadvertise(obstacle_واټن_pub);

	// THEN: setpoint should go into the same direction as the stick input
	EXPECT_FLOAT_EQ(original_setpoint.normalized()(0), modified_setpoint.normalized()(0));
	EXPECT_FLOAT_EQ(original_setpoint.normalized()(1), modified_setpoint.normalized()(1));
}

TEST_F(CollisionPreventionTest, outsideFOV)
{
	// GIVEN: a simple setup condition
	TestCollisionPrevention cp;
	Vector2f curr_vel(2, 0);

	// AND: a parameter handle
	param_t param = param_handle(px4::params::CP_DIST);
	عائم value = 5; // try to keep 5m distance
	param_سېټ(param, &value);
	cp.params_بدل_شوی();

	// AND: an obstacle message
	obstacle_واټن_s پیغام_3;
	memset(&پیغام_3, 0xDEAD, حجم(پیغام_3));
	پیغام_3.چوکاټ = پیغام_3.MAV_چوکاټ_GLOBAL; //north aligned
	پیغام_3.min_واټن = 100;
	پیغام_3.max_واټن = 2000;
	صحيح distances_array_اندازه = حجم(پیغام_3.distances) / حجم(پیغام_3.distances[0]);
	پیغام_3.increment = 360.f / distances_array_اندازه;

	//fov from 45deg to 225deg
	لكل (صحيح i = 0; i < distances_array_اندازه; i++) {
		عائم angle = i * پیغام_3.increment;

		إذا (angle > 45.f && angle < 225.f) {
			پیغام_3.distances[i] = 700;

		} وإلا {
			پیغام_3.distances[i] = UINT16_MAX;
		}
	}

	// WHEN: we publish the message and modify the setpoint for different demanded setpoints
	orb_advert_t obstacle_واټن_pub = orb_advertise(ORB_ID(obstacle_واټن), &پیغام_3);

	لكل (صحيح i = 0; i < distances_array_اندازه; i++) {
		عائم angle_deg = (عائم)i * پیغام_3.increment;
		عائم angle_rad = math::radians(angle_deg);
		Vector2f original_setpoint = {10.f * cosf(angle_rad), 10.f * sinf(angle_rad)};
		Vector2f modified_setpoint = original_setpoint;
		پیغام_3.timestamp = hrt_absolute_وخت();
		orb_publish(ORB_ID(obstacle_واټن), obstacle_واټن_pub, &پیغام_3);
		cp.modifySetpoint(modified_setpoint, curr_vel);

		//THEN: if the resulting setpoint demands velocities bigger zero, it must lie inside the FOV
		عائم setpoint_اوږدتیا = modified_setpoint.norm();

		إذا (setpoint_اوږدتیا > 0.f) {
			Vector2f setpoint_dir = modified_setpoint / setpoint_اوږدتیا;
			عائم sp_angle_body_چوکاټ = atan2(setpoint_dir(1), setpoint_dir(0));
			عائم sp_angle_deg = math::degrees(wrap_2pi(sp_angle_body_چوکاټ));
			EXPECT_GE(sp_angle_deg, 45.f);
			EXPECT_LE(sp_angle_deg, 225.f);
		}
	}

	orb_unadvertise(obstacle_واټن_pub);
}

TEST_F(CollisionPreventionTest, go_No_معلومات)
{
	// GIVEN: a simple setup condition with the initial state (no distance data)
	TestCollisionPrevention cp;
	Vector2f curr_vel(2, 0);

	// AND: an obstacle message
	obstacle_واټن_s پیغام_3;
	memset(&پیغام_3, 0xDEAD, حجم(پیغام_3));
	پیغام_3.چوکاټ = پیغام_3.MAV_چوکاټ_GLOBAL; //north aligned
	پیغام_3.min_واټن = 100;
	پیغام_3.max_واټن = 2000;
	صحيح distances_array_اندازه = حجم(پیغام_3.distances) / حجم(پیغام_3.distances[0]);
	پیغام_3.increment = 360.f / distances_array_اندازه;

	//fov from 0deg to 20deg
	لكل (صحيح i = 0; i < distances_array_اندازه; i++) {
		عائم angle = i * پیغام_3.increment;

		إذا (angle > 0.f && angle < 40.f) {
			پیغام_3.distances[i] = 1000;

		} وإلا {
			پیغام_3.distances[i] = UINT16_MAX;
		}
	}

	// AND: a parameter handle
	param_t param = param_handle(px4::params::CP_DIST);
	عائم value = 2; // try to keep 5m distance
	param_سېټ(param, &value);
	cp.params_بدل_شوی();

	// AND: a setpoint outside the field of view
	Vector2f original_setpoint = {-5, 0};
	Vector2f modified_setpoint = original_setpoint;

	//THEN: the modified setpoint should be zero acceleration
	cp.modifySetpoint(modified_setpoint, curr_vel);
	EXPECT_FLOAT_EQ(modified_setpoint.norm(), 0.f);

	//WHEN: we change the parameter CP_GO_NO_DATA to allow flying ouside the FOV
	param_t param_allow = param_handle(px4::params::CP_GO_NO_معلومات);
	عائم ارزښت_allow = 1;
	param_سېټ(param_allow, &ارزښت_allow);
	cp.params_بدل_شوی();

	//THEN: When all bins contain UINT_16MAX the setpoint should be zero even if CP_GO_NO_DATA=1
	modified_setpoint = original_setpoint;
	cp.modifySetpoint(modified_setpoint, curr_vel);
	EXPECT_FLOAT_EQ(modified_setpoint.norm(), 0.f);

	//THEN: As soon as the range data contains any valid number, flying outside the FOV is allowed
	پیغام_3.timestamp = hrt_absolute_وخت();
	orb_advert_t obstacle_واټن_pub = orb_advertise(ORB_ID(obstacle_واټن), &پیغام_3);
	orb_publish(ORB_ID(obstacle_واټن), obstacle_واټن_pub, &پیغام_3);

	modified_setpoint = original_setpoint;
	cp.modifySetpoint(modified_setpoint, curr_vel);
	EXPECT_FLOAT_EQ(modified_setpoint.norm(), original_setpoint.norm());
	orb_unadvertise(obstacle_واټن_pub);
}

TEST_F(CollisionPreventionTest, jerk_حد)
{
	// GIVEN: a simple setup condition
	TestCollisionPrevention cp;
	Vector2f original_setpoint(10, 0);
	Vector2f curr_vel(2, 0);

	// AND: distance set to 5m
	param_t param = param_handle(px4::params::CP_DIST);
	عائم value = 5; // try to keep 5m distance
	param_سېټ(param, &value);
	cp.params_بدل_شوی();

	// AND: an obstacle message
	obstacle_واټن_s پیغام_3;
	memset(&پیغام_3, 0xDEAD, حجم(پیغام_3));
	پیغام_3.min_واټن = 100;
	پیغام_3.max_واټن = 2000;
	پیغام_3.timestamp = hrt_absolute_وخت();
	پیغام_3.چوکاټ = پیغام_3.MAV_چوکاټ_GLOBAL; //north aligned
	صحيح distances_array_اندازه = حجم(پیغام_3.distances) / حجم(پیغام_3.distances[0]);
	پیغام_3.increment = 360 / distances_array_اندازه;

	لكل (صحيح i = 0; i < distances_array_اندازه; i++) {
		پیغام_3.distances[i] = 700;
	}

	// AND: we publish the message and set the parameter and then run the setpoint modification
	orb_advert_t obstacle_واټن_pub = orb_advertise(ORB_ID(obstacle_واټن), &پیغام_3);
	orb_publish(ORB_ID(obstacle_واټن), obstacle_واټن_pub, &پیغام_3);
	Vector2f modified_setpoint_default_jerk = original_setpoint;
	cp.modifySetpoint(modified_setpoint_default_jerk, curr_vel);
	orb_unadvertise(obstacle_واټن_pub);

	// AND: we now set max jerk to 0.1
	param = param_handle(px4::params::MPC_JERK_MAX);
	value = 0.1; // 0.1 maximum jerk
	param_سېټ(param, &value);
	cp.params_بدل_شوی();

	// WHEN: we run the setpoint modification again
	Vector2f modified_setpoint_limited_jerk = original_setpoint;
	cp.modifySetpoint(modified_setpoint_limited_jerk, curr_vel);

	// THEN: the new setpoint should be much higher than the one with default jerk, as the rate of change in acceleration is more limmited
	EXPECT_GT(modified_setpoint_limited_jerk.norm(), modified_setpoint_default_jerk.norm());

}
TEST_F(CollisionPreventionTest, جمع_کړه_بهر_د_Range_واټن_Sensor_معلومات)
{
	// GIVEN: a vehicle attitude and a distance sensor message
	TestCollisionPrevention cp;
	Quaternion<عائم> vehicle_attitude(1, 0, 0, 0); //unit transform
	واټن_sensor_s واټن_sensor {};
	واټن_sensor.min_واټن = 0.2f;
	واټن_sensor.max_واټن = 20.f;
	واټن_sensor.orientation = واټن_sensor_s::ROTATION_FORWARD_FACING;
	// Distance is out of Range
	واټن_sensor.current_واټن = -1.f;
	واټن_sensor.signal_quality = 0;

	uint32_t distances_array_اندازه = حجم(cp.getObstacleMap().distances) / حجم(cp.getObstacleMap().distances[0]);

	cp.test_جمع_کړه_واټن_Sensor_معلومات(واټن_sensor, vehicle_attitude);

	//THEN: the correct bins in the map should be filled
	لكل (uint32_t i = 0; i < distances_array_اندازه; i++) {
		إذا (i == 0) {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], واټن_sensor.max_واټن * 100.f);

		} وإلا {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], UINT16_MAX);
		}
	}
}

TEST_F(CollisionPreventionTest, جمع_کړه_واټن_Sensor_معلومات_Narrow)
{
	// GIVEN: a vehicle attitude and a distance sensor message
	TestCollisionPrevention cp;
	Quaternion<عائم> vehicle_attitude(1, 0, 0, 0); //unit transform
	واټن_sensor_s واټن_sensor {};
	واټن_sensor.min_واټن = 0.2f;
	واټن_sensor.max_واټن = 20.f;
	واټن_sensor.current_واټن = 5.f;
	واټن_sensor.orientation = واټن_sensor_s::ROTATION_FORWARD_FACING;
	واټن_sensor.h_fov = math::radians(0.1 * bin_اندازه);

	uint32_t distances_array_اندازه = حجم(cp.getObstacleMap().distances) / حجم(cp.getObstacleMap().distances[0]);

	// WHEN the sensor has a very narrow field of view
	cp.test_جمع_کړه_واټن_Sensor_معلومات(واټن_sensor, vehicle_attitude);

	//THEN: the correct bins in the map should be filled
	لكل (uint32_t i = 0; i < distances_array_اندازه; i++) {
		إذا (i == 0) {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], 500) << i;

		} وإلا {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], UINT16_MAX) << i;
		}
	}
}
TEST_F(CollisionPreventionTest, جمع_کړه_واټن_Sensor_معلومات_Slightly_Larger)
{
	// GIVEN: a vehicle attitude and a distance sensor message
	TestCollisionPrevention cp;
	Quaternion<عائم> vehicle_attitude(1, 0, 0, 0); //unit transform
	واټن_sensor_s واټن_sensor {};
	واټن_sensor.min_واټن = 0.2f;
	واټن_sensor.max_واټن = 20.f;
	واټن_sensor.current_واټن = 5.f;
	واټن_sensor.orientation = واټن_sensor_s::ROTATION_FORWARD_FACING;
	واټن_sensor.h_fov = math::radians(1.1 * bin_اندازه);

	uint32_t distances_array_اندازه = حجم(cp.getObstacleMap().distances) / حجم(cp.getObstacleMap().distances[0]);

	// WHEN the sensor has a very narrow field of view
	cp.test_جمع_کړه_واټن_Sensor_معلومات(واټن_sensor, vehicle_attitude);

	//THEN: the the bins corresponding to -5°, 0° and 5° should be filled
	لكل (uint32_t i = 0; i < distances_array_اندازه; i++) {
		إذا (i == 71 || i <= 1) {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], 500) << i;

		} وإلا {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], UINT16_MAX) << i;
		}
	}
}

TEST_F(CollisionPreventionTest, جمع_کړه_واټن_Sensor_معلومات_2)
{
	// GIVEN: a vehicle attitude and a distance sensor message
	TestCollisionPrevention cp;
	Quaternion<عائم> vehicle_attitude(1, 0, 0, 0); //unit transform
	واټن_sensor_s واټن_sensor {};
	واټن_sensor.min_واټن = 0.2f;
	واټن_sensor.max_واټن = 20.f;
	واټن_sensor.current_واټن = 5.f;

	//THEN: at initialization the internal obstacle map should only contain UINT16_MAX
	uint32_t distances_array_اندازه = حجم(cp.getObstacleMap().distances) / حجم(cp.getObstacleMap().distances[0]);

	لكل (uint32_t i = 0; i < distances_array_اندازه; i++) {
		EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], UINT16_MAX) << i;
	}

	//WHEN: we add distance sensor data to the right
	واټن_sensor.orientation = واټن_sensor_s::ROTATION_ښی_FACING;
	واټن_sensor.h_fov = math::radians(19.99f);
	cp.test_جمع_کړه_واټن_Sensor_معلومات(واټن_sensor, vehicle_attitude);
	uint fov = round(واټن_sensor.h_fov * M_RAD_منزل_DEG_F / 2);
	uint پیل = (90 - fov) / bin_اندازه;
	uint پای_ټکی = (90 + fov) / bin_اندازه;

	//THEN: the correct bins in the map should be filled
	لكل (uint32_t i = 0; i < distances_array_اندازه; i++) {
		إذا (i >= پیل && i <= پای_ټکی) {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], 500) << i;

		} وإلا {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], UINT16_MAX) << i;
		}
	}

	//WHEN: we add additionally distance sensor data to the left
	واټن_sensor.orientation = واټن_sensor_s::ROTATION_کیڼ_FACING;
	واټن_sensor.h_fov = math::radians(50.f);
	واټن_sensor.current_واټن = 8.f;
	cp.test_جمع_کړه_واټن_Sensor_معلومات(واټن_sensor, vehicle_attitude);
	fov = round(واټن_sensor.h_fov * M_RAD_منزل_DEG_F / 2);
	uint start2 = (270 - fov) / bin_اندازه;
	uint end2 = (270 + fov) / bin_اندازه;

	//THEN: the correct bins in the map should be filled
	لكل (uint32_t i = 0; i < distances_array_اندازه; i++) {
		إذا (i >= پیل && i <= پای_ټکی) {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], 500) << i;

		} وإلا إذا (i >= start2 && i <= end2) {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], 800) << i;

		} وإلا {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], UINT16_MAX) << i;
		}
	}

	//WHEN: we add additionally distance sensor data to the front
	واټن_sensor.orientation = واټن_sensor_s::ROTATION_FORWARD_FACING;
	واټن_sensor.h_fov = math::radians(10.1f);
	واټن_sensor.current_واټن = 3.f;
	cp.test_جمع_کړه_واټن_Sensor_معلومات(واټن_sensor, vehicle_attitude);
	fov = round(واټن_sensor.h_fov * M_RAD_منزل_DEG_F / 2);
	uint start3 = (360 - fov) / bin_اندازه;
	uint end3 = (fov) / bin_اندازه;

	//THEN: the correct bins in the map should be filled
	لكل (uint32_t i = 0; i < distances_array_اندازه; i++) {
		إذا (i >= پیل && i <= پای_ټکی) {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], 500)  << i;

		} وإلا إذا (i >= start2 && i <= end2) {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], 800) << i;

		} وإلا إذا (i >= start3 || i <= end3) {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], 300) << i;

		} وإلا {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], UINT16_MAX) << i;
		}
	}


}

TEST_F(CollisionPreventionTest, جمع_کړه_Obstacle_Sensor_معلومات_attitude)
{
	// GIVEN: a vehicle attitude and obstacle distance message
	TestCollisionPrevention cp;
	obstacle_واټن_s obstacle_پیغام {};
	obstacle_پیغام.چوکاټ = obstacle_پیغام.MAV_چوکاټ_GLOBAL; //north aligned
	obstacle_پیغام.increment = 5.f;
	obstacle_پیغام.min_واټن = 20;
	obstacle_پیغام.max_واټن = 2000;
	obstacle_پیغام.angle_دننه_واټن = 0.f;

	//obstacle at 10-30 deg world frame, distance 5 meters
	memset(&obstacle_پیغام.distances[0], UINT16_MAX, حجم(obstacle_پیغام.distances));

	صحيح پیل = 2;
	صحيح پای_ټکی = 6;

	لكل (صحيح i = پیل; i <= پای_ټکی ; i++) {
		obstacle_پیغام.distances[i] = 500;
	}



	//THEN: at initialization the internal obstacle map should only contain UINT16_MAX
	صحيح distances_array_اندازه = حجم(cp.getObstacleMap().distances) / حجم(cp.getObstacleMap().distances[0]);

	لكل (صحيح i = 0; i < distances_array_اندازه; i++) {
		EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], UINT16_MAX);
	}

	//WHEN: we add distance sensor data while vehicle has zero yaw
	cp.test_جمع_کړه_Obstacle_Sensor_معلومات(obstacle_پیغام, 0.f);

	//THEN: the correct bins in the map should be filled
	لكل (صحيح i = 0; i < distances_array_اندازه; i++) {
		إذا (i >= پیل && i <= پای_ټکی) {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], 500) << i;

		} وإلا {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], UINT16_MAX) << i;
		}

		//reset array to UINT16_MAX
		cp.getObstacleMap().distances[i] = UINT16_MAX;
	}


	//WHEN: we add obstacle distance sensor data while vehicle yaw 90deg to the right
	cp.test_جمع_کړه_Obstacle_Sensor_معلومات(obstacle_پیغام, M_PI_2);

	//THEN: the correct bins in the map should be filled
	صحيح دننه_واټن =  bin_شمېر - 90 / bin_اندازه;

	لكل (صحيح i = 0; i < distances_array_اندازه; i++) {
		إذا (i >= دننه_واټن + پیل && i <= دننه_واټن + پای_ټکی) {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], 500) << i;

		} وإلا {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], UINT16_MAX) << i;
		}

		//reset array to UINT16_MAX
		cp.getObstacleMap().distances[i] = UINT16_MAX;
	}

	//WHEN: we add obstacle distance sensor data while vehicle yaw 45deg to the left
	cp.test_جمع_کړه_Obstacle_Sensor_معلومات(obstacle_پیغام, -M_PI_4);

	//THEN: the correct bins in the map should be filled
	دننه_واټن =  45 / bin_اندازه;

	لكل (صحيح i = 0; i < distances_array_اندازه; i++) {
		إذا (i >= دننه_واټن + پیل && i <= دننه_واټن + پای_ټکی) {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], 500) << i;

		} وإلا {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], UINT16_MAX) << i;
		}

		//reset array to UINT16_MAX
		cp.getObstacleMap().distances[i] = UINT16_MAX;
	}

	//WHEN: we add obstacle distance sensor data while vehicle yaw 180deg
	cp.test_جمع_کړه_Obstacle_Sensor_معلومات(obstacle_پیغام, M_PI);

	//THEN: the correct bins in the map should be filled
	دننه_واټن =  180 / bin_اندازه;

	لكل (صحيح i = 0; i < distances_array_اندازه; i++) {
		إذا (i >= دننه_واټن + پیل && i <= دننه_واټن + پای_ټکی) {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], 500);

		} وإلا {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], UINT16_MAX);
		}

		//reset array to UINT16_MAX
		cp.getObstacleMap().distances[i] = UINT16_MAX;
	}
}

TEST_F(CollisionPreventionTest, جمع_کړه_Obstacle_Sensor_معلومات_دننه_واټن_bodyframe)
{
	// GIVEN: a vehicle attitude and obstacle distance message
	TestCollisionPrevention cp;
	obstacle_واټن_s obstacle_پیغام {};
	obstacle_پیغام.چوکاټ = obstacle_پیغام.MAV_چوکاټ_BODY_FRD; // Body Frame
	obstacle_پیغام.increment = 6.f;
	obstacle_پیغام.min_واټن = 20;
	obstacle_پیغام.max_واټن = 2000;
	obstacle_پیغام.angle_دننه_واټن = 0.f;

	//obstacle at 363°-39° deg world frame, distance 5 meters
	memset(&obstacle_پیغام.distances[0], UINT16_MAX, حجم(obstacle_پیغام.distances));

	لكل (صحيح i = 0; i <= 6 ; i++) { // 36° at 6° increment
		obstacle_پیغام.distances[i] = 500;
	}

	//WHEN: we add distance sensor data
	cp.test_جمع_کړه_Obstacle_Sensor_معلومات(obstacle_پیغام, 0.f);

	//THEN: the the bins from 0 to 40 in map should be filled, which correspond to the angles from -2.5° to 42.5°
	صحيح distances_array_اندازه = حجم(cp.getObstacleMap().distances) / حجم(cp.getObstacleMap().distances[0]);

	لكل (صحيح i = 0; i < distances_array_اندازه; i++) {
		إذا (i >= 0 && i <= 8) {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], 500) << i;

		} وإلا {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], UINT16_MAX) << i;
		}

		//reset array to UINT16_MAX
		cp.getObstacleMap().distances[i] = UINT16_MAX;
	}

	//WHEN: we add the same obstacle distance sensor data with an angle offset of 30.5°
	obstacle_پیغام.angle_دننه_واټن = 30.5f;
	// This then means our obstacle is between 27.5° and 69.5°
	cp.test_جمع_کړه_Obstacle_Sensor_معلومات(obstacle_پیغام, 0.f);

	//THEN: the bins from 30° to 70° in map should be filled, which correspond to the angles from 27.5° to 72.5°
	لكل (صحيح i = 0; i < distances_array_اندازه; i++) {
		إذا (i >= 6 && i <= 14) {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], 500) << i;

		} وإلا {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], UINT16_MAX) << i;
		}

		//reset array to UINT16_MAX
		cp.getObstacleMap().distances[i] = UINT16_MAX;
	}

	//WHEN: we increase the offset to -30.5°
	obstacle_پیغام.angle_دننه_واټن = -30.5f;
	// This then means our obstacle is between 326.5° and 8.5°
	cp.test_جمع_کړه_Obstacle_Sensor_معلومات(obstacle_پیغام, 0.f);

	//THEN: the bins from 325° to 10° in map should be filled, which correspond to the angles from 322.5° to 12.5°

	لكل (صحيح i = 0; i < distances_array_اندازه; i++) {
		إذا (i >= 65 || i <= 2) {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], 500) << i;

		} وإلا {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], UINT16_MAX) << i;
		}

		//reset array to UINT16_MAX
		cp.getObstacleMap().distances[i] = UINT16_MAX;
	}
}

TEST_F(CollisionPreventionTest, جمع_کړه_Obstacle_Sensor_معلومات_bodyframe)
{
	// GIVEN: a vehicle attitude and obstacle distance message
	TestCollisionPrevention cp;
	obstacle_واټن_s obstacle_پیغام {};
	obstacle_پیغام.چوکاټ = obstacle_پیغام.MAV_چوکاټ_BODY_FRD; //north aligned
	obstacle_پیغام.increment = 5.f;
	obstacle_پیغام.min_واټن = 20;
	obstacle_پیغام.max_واټن = 2000;
	obstacle_پیغام.angle_دننه_واټن = 0.f;

	//obstacle at 10-30 deg body frame, distance 5 meters
	memset(&obstacle_پیغام.distances[0], UINT16_MAX, حجم(obstacle_پیغام.distances));
	صحيح پیل = 2;
	صحيح پای_ټکی = 6;

	لكل (صحيح i = پیل; i <= پای_ټکی ; i++) {
		obstacle_پیغام.distances[i] = 500;
	}


	//THEN: at initialization the internal obstacle map should only contain UINT16_MAX
	صحيح distances_array_اندازه = حجم(cp.getObstacleMap().distances) / حجم(cp.getObstacleMap().distances[0]);

	لكل (صحيح i = 0; i < distances_array_اندازه; i++) {
		EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], UINT16_MAX);
	}

	//WHEN: we add obstacle data while vehicle has zero yaw
	cp.test_جمع_کړه_Obstacle_Sensor_معلومات(obstacle_پیغام, 0.f);

	//THEN: the correct bins in the map should be filled

	لكل (صحيح i = 0; i < distances_array_اندازه; i++) {
		إذا (i >= پیل && i <= پای_ټکی) {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], 500) << i;

		} وإلا {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], UINT16_MAX) << i;
		}

		//reset array to UINT16_MAX
		cp.getObstacleMap().distances[i] = UINT16_MAX;
	}

	//WHEN: we add obstacle data while vehicle yaw 90deg to the right
	cp.test_جمع_کړه_Obstacle_Sensor_معلومات(obstacle_پیغام, M_PI_2);

	//THEN: the correct bins in the map should be filled
	لكل (صحيح i = 0; i < distances_array_اندازه; i++) {
		إذا (i >= پیل && i <= پای_ټکی) {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], 500) << i;

		} وإلا {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], UINT16_MAX) << i;
		}

		//reset array to UINT16_MAX
		cp.getObstacleMap().distances[i] = UINT16_MAX;
	}

	//WHEN: we add obstacle data while vehicle yaw 45deg to the left
	cp.test_جمع_کړه_Obstacle_Sensor_معلومات(obstacle_پیغام, -M_PI_4);

	//THEN: the correct bins in the map should be filled
	لكل (صحيح i = 0; i < distances_array_اندازه; i++) {
		إذا (i >= پیل && i <= پای_ټکی) {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], 500) << i;

		} وإلا {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], UINT16_MAX) << i;
		}

		//reset array to UINT16_MAX
		cp.getObstacleMap().distances[i] = UINT16_MAX;
	}

	//WHEN: we add obstacle data while vehicle yaw 180deg
	cp.test_جمع_کړه_Obstacle_Sensor_معلومات(obstacle_پیغام, M_PI);

	//THEN: the correct bins in the map should be filled
	لكل (صحيح i = 0; i < distances_array_اندازه; i++) {
		إذا (i >= پیل && i <= پای_ټکی) {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], 500) << i;

		} وإلا {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], UINT16_MAX) << i;
		}

		//reset array to UINT16_MAX
		cp.getObstacleMap().distances[i] = UINT16_MAX;
	}

}


TEST_F(CollisionPreventionTest, جمع_کړه_Obstacle_Sensor_معلومات_resolution_دننه_واټن)
{
	// GIVEN: a vehicle attitude and obstacle distance message
	TestCollisionPrevention cp;
	obstacle_واټن_s obstacle_پیغام {};
	obstacle_پیغام.چوکاټ = obstacle_پیغام.MAV_چوکاټ_GLOBAL; //north aligned
	obstacle_پیغام.increment = 6.f;
	obstacle_پیغام.min_واټن = 20;
	obstacle_پیغام.max_واټن = 2000;
	obstacle_پیغام.angle_دننه_واټن = 0.f;

	//obstacle at 363°-39° deg world frame, distance 5 meters
	memset(&obstacle_پیغام.distances[0], UINT16_MAX, حجم(obstacle_پیغام.distances));

	لكل (صحيح i = 0; i <= 6 ; i++) { // 36° at 6° increment
		obstacle_پیغام.distances[i] = 500;
	}

	//WHEN: we add distance sensor data
	cp.test_جمع_کړه_Obstacle_Sensor_معلومات(obstacle_پیغام, 0.f);

	//THEN: the the bins from 0 to 40 in map should be filled, which correspond to the angles from -2.5° to 42.5°
	صحيح distances_array_اندازه = حجم(cp.getObstacleMap().distances) / حجم(cp.getObstacleMap().distances[0]);

	لكل (صحيح i = 0; i < distances_array_اندازه; i++) {
		إذا (i >= 0 && i <= 8) {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], 500) << i;

		} وإلا {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], UINT16_MAX) << i;
		}

		//reset array to UINT16_MAX
		cp.getObstacleMap().distances[i] = UINT16_MAX;
	}

	//WHEN: we add the same obstacle distance sensor data with an angle offset of 30.5°
	obstacle_پیغام.angle_دننه_واټن = 30.5f;
	// This then means our obstacle is between 27.5° and 69.5°
	cp.test_جمع_کړه_Obstacle_Sensor_معلومات(obstacle_پیغام, 0.f);

	//THEN: the bins from 30° to 70° in map should be filled, which correspond to the angles from 27.5° to 72.5°
	لكل (صحيح i = 0; i < distances_array_اندازه; i++) {
		إذا (i >= 6 && i <= 14) {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], 500) << i;

		} وإلا {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], UINT16_MAX) << i;
		}

		//reset array to UINT16_MAX
		cp.getObstacleMap().distances[i] = UINT16_MAX;
	}

	//WHEN: we increase the offset to -30.5°
	obstacle_پیغام.angle_دننه_واټن = -30.5f;
	// This then means our obstacle is between 326.5° and 8.5°
	cp.test_جمع_کړه_Obstacle_Sensor_معلومات(obstacle_پیغام, 0.f);

	//THEN: the bins from 325° to 10° in map should be filled, which correspond to the angles from 322.5° to 12.5°

	لكل (صحيح i = 0; i < distances_array_اندازه; i++) {
		إذا (i >= 65 || i <= 2) {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], 500) << i;

		} وإلا {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], UINT16_MAX) << i;
		}

		//reset array to UINT16_MAX
		cp.getObstacleMap().distances[i] = UINT16_MAX;
	}
}

TEST_F(CollisionPreventionTest, adaptSetpointDirection_distinct_minimum)
{
	// GIVEN: a vehicle attitude and obstacle distance message
	TestCollisionPrevention cp;
	obstacle_واټن_s obstacle_پیغام {};
	obstacle_پیغام.چوکاټ = obstacle_پیغام.MAV_چوکاټ_GLOBAL; //north aligned
	obstacle_پیغام.increment = 5.f;
	obstacle_پیغام.min_واټن = 20;
	obstacle_پیغام.max_واټن = 2000;
	obstacle_پیغام.angle_دننه_واټن = 0.f;

	ثابت عائم vehicle_yaw = 0.f;

	//obstacle at 0-30 deg world frame, distance 5 meters
	memset(&obstacle_پیغام.distances[0], UINT16_MAX, حجم(obstacle_پیغام.distances));

	لكل (صحيح i = 0; i <= 6 ; i++) {
		obstacle_پیغام.distances[i] = 500;
	}

	obstacle_پیغام.distances[2] = 1000;

	//define setpoint
	Vector2f setpoint_dir(1, 0);
	عائم sp_angle_body_چوکاټ = atan2f(setpoint_dir(1), setpoint_dir(0)) - vehicle_yaw;
	عائم sp_angle_with_دننه_واټن_deg = wrap(math::degrees(sp_angle_body_چوکاټ) - cp.getObstacleMap().angle_دننه_واټن,
					      0.f, 360.f);
	صحيح sp_شاخص = floor(sp_angle_with_دننه_واټن_deg / cp.getObstacleMap().increment);

	//set parameter
	param_t param = param_handle(px4::params::CP_DIST);
	عائم value = 3; // try to keep 10m away from obstacles
	param_سېټ(param, &value);
	cp.params_بدل_شوی();

	//WHEN: we add distance sensor data
	cp.test_جمع_کړه_Obstacle_Sensor_معلومات(obstacle_پیغام, vehicle_yaw);
	cp.test_adaptSetpointDirection(setpoint_dir, sp_شاخص, vehicle_yaw);

	//THEN: the setpoint direction should be modified correctly
	EXPECT_EQ(sp_شاخص, 2);
	EXPECT_FLOAT_EQ(setpoint_dir(0), 0.98480773f);
	EXPECT_FLOAT_EQ(setpoint_dir(1), 0.17364818f);
}

TEST_F(CollisionPreventionTest, adaptSetpointDirection_flat_minimum)
{
	// GIVEN: a vehicle attitude and obstacle distance message
	TestCollisionPrevention cp;
	obstacle_واټن_s obstacle_پیغام {};
	obstacle_پیغام.چوکاټ = obstacle_پیغام.MAV_چوکاټ_GLOBAL; //north aligned
	obstacle_پیغام.increment = 5.f;
	obstacle_پیغام.min_واټن = 20;
	obstacle_پیغام.max_واټن = 2000;
	obstacle_پیغام.angle_دننه_واټن = 0.f;

	ثابت عائم vehicle_yaw = 0.f;

	//obstacle at 0-30 deg world frame, distance 5 meters
	memset(&obstacle_پیغام.distances[0], UINT16_MAX, حجم(obstacle_پیغام.distances));

	لكل (صحيح i = 0; i < 7 ; i++) {
		obstacle_پیغام.distances[i] = 500;
	}

	obstacle_پیغام.distances[1] = 1000;
	obstacle_پیغام.distances[2] = 1000;
	obstacle_پیغام.distances[3] = 1000;

	//define setpoint
	Vector2f setpoint_dir(1, 0);
	عائم sp_angle_body_چوکاټ = atan2f(setpoint_dir(1), setpoint_dir(0)) - vehicle_yaw;
	عائم sp_angle_with_دننه_واټن_deg = wrap(math::degrees(sp_angle_body_چوکاټ) - cp.getObstacleMap().angle_دننه_واټن,
					      0.f, 360.f);
	صحيح sp_شاخص = floor(sp_angle_with_دننه_واټن_deg / cp.getObstacleMap().increment);

	//set parameter
	param_t param = param_handle(px4::params::CP_DIST);
	عائم value = 3; // try to keep 10m away from obstacles
	param_سېټ(param, &value);
	cp.params_بدل_شوی();

	//WHEN: we add distance sensor data
	cp.test_جمع_کړه_Obstacle_Sensor_معلومات(obstacle_پیغام, vehicle_yaw);
	cp.test_adaptSetpointDirection(setpoint_dir, sp_شاخص, vehicle_yaw);

	//THEN: the setpoint direction should be modified correctly
	EXPECT_EQ(sp_شاخص, 2);
	EXPECT_FLOAT_EQ(setpoint_dir(0), 0.98480773f);
	EXPECT_FLOAT_EQ(setpoint_dir(1), 0.17364818f);
}

TEST_F(CollisionPreventionTest, overlappingSensors)
{
	// GIVEN: a simple setup condition
	TestCollisionPrevention cp;
	Vector2f original_setpoint(10, 0);
	Vector2f curr_vel(2, 0);
	vehicle_attitude_s attitude;
	attitude.timestamp = hrt_absolute_وخت();
	attitude.q[0] = 1.0f;
	attitude.q[1] = 0.0f;
	attitude.q[2] = 0.0f;
	attitude.q[3] = 0.0f;

	// AND: a parameter handle
	param_t param = param_handle(px4::params::CP_DIST);
	عائم value = 10; // try to keep 10m distance
	param_سېټ(param, &value);
	cp.params_بدل_شوی();

	// AND: an obstacle message for a short range and a long range sensor
	obstacle_واټن_s short_range_پیغام, short_range_پیغام_no_obstacle, long_range_پیغام;
	memset(&short_range_پیغام, 0xDEAD, حجم(short_range_پیغام));
	short_range_پیغام.چوکاټ = short_range_پیغام.MAV_چوکاټ_GLOBAL; //north aligned
	short_range_پیغام.angle_دننه_واټن = 0;
	short_range_پیغام.timestamp = hrt_absolute_وخت();
	صحيح distances_array_اندازه = حجم(short_range_پیغام.distances) / حجم(short_range_پیغام.distances[0]);
	short_range_پیغام.increment = 360 / distances_array_اندازه;
	long_range_پیغام = short_range_پیغام;
	long_range_پیغام.min_واټن = 100;
	long_range_پیغام.max_واټن = 1000;
	short_range_پیغام.min_واټن = 20;
	short_range_پیغام.max_واټن = 200;
	short_range_پیغام_no_obstacle = short_range_پیغام;


	لكل (صحيح i = 0; i < distances_array_اندازه; i++) {
		إذا (i < 10) {
			short_range_پیغام_no_obstacle.distances[i] = 201;
			short_range_پیغام.distances[i] = 150;
			long_range_پیغام.distances[i] = 500;

		} وإلا {
			short_range_پیغام_no_obstacle.distances[i] = UINT16_MAX;
			short_range_پیغام.distances[i] = UINT16_MAX;
			long_range_پیغام.distances[i] = UINT16_MAX;
		}
	}


	// CASE 1
	//WHEN: we publish the long range sensor message
	orb_advert_t obstacle_واټن_pub = orb_advertise(ORB_ID(obstacle_واټن), &long_range_پیغام);
	orb_advert_t vehicle_attitude_pub = orb_advertise(ORB_ID(vehicle_attitude), &attitude);
	orb_publish(ORB_ID(obstacle_واټن), obstacle_واټن_pub, &long_range_پیغام);
	orb_publish(ORB_ID(vehicle_attitude), vehicle_attitude_pub, &attitude);
	Vector2f modified_setpoint = original_setpoint;
	cp.modifySetpoint(modified_setpoint, curr_vel);

	// THEN: the internal map data should contain the long range measurement
	EXPECT_EQ(500, cp.getObstacleMap().distances[2]);

	// CASE 2
	// WHEN: we publish the short range message followed by a long range message
	short_range_پیغام.timestamp = hrt_absolute_وخت();
	orb_publish(ORB_ID(obstacle_واټن), obstacle_واټن_pub, &short_range_پیغام);
	cp.modifySetpoint(modified_setpoint, curr_vel);
	long_range_پیغام.timestamp = hrt_absolute_وخت();
	orb_publish(ORB_ID(obstacle_واټن), obstacle_واټن_pub, &long_range_پیغام);
	cp.modifySetpoint(modified_setpoint, curr_vel);

	// THEN: the internal map data should contain the short range measurement
	EXPECT_EQ(150, cp.getObstacleMap().distances[2]);

	// CASE 3
	// WHEN: we publish the short range message with values out of range followed by a long range message
	short_range_پیغام_no_obstacle.timestamp = hrt_absolute_وخت();
	orb_publish(ORB_ID(obstacle_واټن), obstacle_واټن_pub, &short_range_پیغام_no_obstacle);
	cp.modifySetpoint(modified_setpoint, curr_vel);
	long_range_پیغام.timestamp = hrt_absolute_وخت();
	orb_publish(ORB_ID(obstacle_واټن), obstacle_واټن_pub, &long_range_پیغام);
	cp.modifySetpoint(modified_setpoint, curr_vel);

	// THEN: the internal map data should contain the short range measurement
	EXPECT_EQ(500, cp.getObstacleMap().distances[2]);

	orb_unadvertise(obstacle_واټن_pub);
	orb_unadvertise(vehicle_attitude_pub);
}

TEST_F(CollisionPreventionTest, enter_معلومات_2)
{
	// GIVEN: a simple setup condition
	TestCollisionPrevention cp;
	Quaternion<عائم> vehicle_attitude(1, 0, 0, 0); //unit transform

	//THEN: just after initialization all bins are at UINT16_MAX and any data should be accepted
	EXPECT_TRUE(cp.test_enter_معلومات(16, 2.f, 1.5f)); //shorter range, reading in range
	EXPECT_TRUE(cp.test_enter_معلومات(16, 2.f, 3.f)); //shorter range, reading out of range
	EXPECT_TRUE(cp.test_enter_معلومات(16, 20.f, 1.5f)); //same range, reading in range
	EXPECT_TRUE(cp.test_enter_معلومات(16, 20.f, 21.f)); //same range, reading out of range
	EXPECT_TRUE(cp.test_enter_معلومات(16, 30.f, 1.5f)); //longer range, reading in range
	EXPECT_TRUE(cp.test_enter_معلومات(16, 30.f, 31.f)); //longer range, reading out of range

	//WHEN: we add distance sensor data to the right with a valid reading
	واټن_sensor_s واټن_sensor {};
	واټن_sensor.min_واټن = 0.2f;
	واټن_sensor.max_واټن = 20.f;
	واټن_sensor.orientation = واټن_sensor_s::ROTATION_ښی_FACING;
	واټن_sensor.h_fov = math::radians(19.99f);
	واټن_sensor.current_واټن = 5.f;
	cp.test_جمع_کړه_واټن_Sensor_معلومات(واټن_sensor, vehicle_attitude);

	//THEN: the internal map should contain the distance sensor readings
	لكل (صحيح i = 16; i < 20; i++) {
		EXPECT_EQ(500, cp.getObstacleMap().distances[i]) << i;
	}

	//THEN: bins 8 & 9 contain valid readings
	// a valid reading should only be accepted from sensors with shorter or equal range
	// a out of range reading should only be accepted from sensors with the same range

	EXPECT_TRUE(cp.test_enter_معلومات(16, 2.f, 1.5f)); //shorter range, reading in range
	EXPECT_FALSE(cp.test_enter_معلومات(16, 2.f, 3.f)); //shorter range, reading out of range
	EXPECT_TRUE(cp.test_enter_معلومات(16, 20.f, 1.5f)); //same range, reading in range
	EXPECT_TRUE(cp.test_enter_معلومات(16, 20.f, 21.f)); //same range, reading out of range
	EXPECT_FALSE(cp.test_enter_معلومات(16, 30.f, 1.5f)); //longer range, reading in range
	EXPECT_FALSE(cp.test_enter_معلومات(16, 30.f, 31.f)); //longer range, reading out of range

	//WHEN: we add distance sensor data to the right with an out of range reading
	واټن_sensor.current_واټن = 21.f;
	cp.test_جمع_کړه_واټن_Sensor_معلومات(واټن_sensor, vehicle_attitude);

	//THEN: the internal map should contain the distance sensor readings
	لكل (صحيح i = 16; i < 20; i++) {
		EXPECT_EQ(2000, cp.getObstacleMap().distances[i]) << i;
	}

	//THEN: bins 8 & 9 contain readings out of range
	// a reading in range will be accepted in any case
	// out of range readings will only be accepted from sensors with bigger or equal range

	EXPECT_TRUE(cp.test_enter_معلومات(16, 2.f, 1.5f)); //shorter range, reading in range
	EXPECT_FALSE(cp.test_enter_معلومات(16, 2.f, 3.f)); //shorter range, reading out of range
	EXPECT_TRUE(cp.test_enter_معلومات(16, 20.f, 1.5f)); //same range, reading in range
	EXPECT_TRUE(cp.test_enter_معلومات(16, 20.f, 21.f)); //same range, reading out of range
	EXPECT_TRUE(cp.test_enter_معلومات(16, 30.f, 1.5f)); //longer range, reading in range
	EXPECT_TRUE(cp.test_enter_معلومات(16, 30.f, 31.f)); //longer range, reading out of range
}
