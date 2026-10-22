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

استفاده نطاق ماتریس;

// to run: make tests TESTFILTER=CollisionPrevention
hrt_abstime mocked_زمان = 0;
ثابت uint bin_اندازه = CollisionPrevention::BIN_اندازه;
ثابت uint bin_شمار = CollisionPrevention::BIN_شمار;

صنف CollisionPreventionTest : عام ::testing::Test
{
عام:
	فراغ مجموعه_به_بالا() override
	{
		param_control_autosave(خطأ);
		param_reset_all();
	}
};

صنف TestCollisionPrevention : عام CollisionPrevention
{
عام:
	TestCollisionPrevention() : CollisionPrevention(اشاره_تهی) {}
	فراغ params_تغییریافته() {CollisionPrevention::updateParamsImpl();}
	obstacle_فاصله_s &getObstacleMap() {أرجع obstacle_map_body_قاب;}
	فراغ test_جمع_کن_فاصله_Sensor_داده(فاصله_sensor_s &فاصله_sensor, ثابت Quatf &attitude)
	{
		جمع_کن_فاصله_Sensor_داده(فاصله_sensor, attitude);
	}
	فراغ test_جمع_کن_Obstacle_Sensor_داده(ثابت obstacle_فاصله_s &obstacle, ثابت عائم vehicle_yaw)
	{
		جمع_کن_Obstacle_Sensor_داده(obstacle, vehicle_yaw);
	}
	فراغ test_adaptSetpointDirection(Vector2f &setpoint_dir, صحيح &setpoint_شاخص_2,
					 عائم vehicle_yaw_angle_rad)
	{
		_adaptSetpointDirection(setpoint_dir, setpoint_شاخص_2, vehicle_yaw_angle_rad);
	}
	منطقي test_enter_داده(صحيح map_شاخص, عائم sensor_range, عائم sensor_reading)
	{
		أرجع enter_داده(map_شاخص, sensor_range, sensor_reading);
	}
};

صنف TestTimingCollisionPrevention : عام TestCollisionPrevention
{
عام:
	TestTimingCollisionPrevention() : TestCollisionPrevention() {}
محمي:
	hrt_abstime get_زمان() override
	{
		أرجع mocked_زمان;
	}

	hrt_abstime get_Elapsed_زمان(ثابت hrt_abstime *ptr) override
	{
		أرجع mocked_زمان - *ptr;
	}
};

TEST_F(CollisionPreventionTest, instantiation) { CollisionPrevention cp(اشاره_تهی); }

TEST_F(CollisionPreventionTest, behaviorOff)
{
	// GIVEN: a simple setup condition
	TestCollisionPrevention cp;

	// THEN: the collision prevention should be turned off by default
	EXPECT_FALSE(cp.is_فعال());
}

TEST_F(CollisionPreventionTest, no_Sensor_داده)
{
	// GIVEN: a simple setup condition
	TestCollisionPrevention cp;
	Vector2f original_setpoint(10, 0);
	Vector2f curr_vel(2, 0);

	// AND: a parameter handle
	param_t param = param_handle(px4::params::CP_DIST);

	// WHEN: we set the parameter check then apply the setpoint modification
	عائم value = 10; // try to keep 10m away from obstacles
	param_مجموعه(param, &value);
	cp.params_تغییریافته();

	Vector2f modified_setpoint = original_setpoint;
	cp.modifySetpoint(modified_setpoint, curr_vel);

	// THEN: collision prevention should be enabled and limit the speed to zero
	EXPECT_TRUE(cp.is_فعال());
	EXPECT_FLOAT_EQ(0.f, modified_setpoint.norm());
}

TEST_F(CollisionPreventionTest, test_Behavior_On_With_Obstacle_پیام)
{
	// GIVEN: a simple setup condition
	TestCollisionPrevention cp;
	Vector2f original_setpoint1(10, 0);
	Vector2f original_setpoint2(-10, 0);
	Vector2f curr_vel(2, 0);
	vehicle_attitude_s attitude;
	attitude.timestamp = hrt_absolute_زمان();
	attitude.q[0] = 1.0f;
	attitude.q[1] = 0.0f;
	attitude.q[2] = 0.0f;
	attitude.q[3] = 0.0f;

	// AND: a parameter handle
	param_t param1 = param_handle(px4::params::CP_DIST);
	عائم value1 = 10; // try to keep 10m distance
	param_مجموعه(param1, &value1);
	param_t param2 = param_handle(px4::params::CP_GUIDE_ANG);
	عائم value2 = 0; // dont guide sideways
	param_مجموعه(param2, &value2);
	cp.params_تغییریافته();

	// AND: an obstacle message
	obstacle_فاصله_s پیام_3;
	memset(&پیام_3, 0xDEAD, حجم(پیام_3));
	پیام_3.قاب = پیام_3.MAV_قاب_GLOBAL; //north aligned
	پیام_3.min_فاصله = 100;
	پیام_3.max_فاصله = 10000;
	پیام_3.angle_جابه_جایی = 0;
	پیام_3.timestamp = hrt_absolute_زمان();
	صحيح distances_array_اندازه = حجم(پیام_3.distances) / حجم(پیام_3.distances[0]);
	پیام_3.increment = 360 / distances_array_اندازه;

	لكل (صحيح i = 0; i < distances_array_اندازه; i++) {
		إذا (i < 10) {
			پیام_3.distances[i] = 101;

		} وإلا {
			پیام_3.distances[i] = 10001;
		}
	}

	// WHEN: we publish the message and set the parameter and then run the setpoint modification
	orb_advert_t obstacle_فاصله_pub = orb_advertise(ORB_ID(obstacle_فاصله), &پیام_3);
	orb_advert_t vehicle_attitude_pub = orb_advertise(ORB_ID(vehicle_attitude), &attitude);
	orb_publish(ORB_ID(obstacle_فاصله), obstacle_فاصله_pub, &پیام_3);
	orb_publish(ORB_ID(vehicle_attitude), vehicle_attitude_pub, &attitude);
	Vector2f modified_setpoint1 = original_setpoint1;
	Vector2f modified_setpoint2 = original_setpoint2;
	cp.modifySetpoint(modified_setpoint1, curr_vel);
	cp.modifySetpoint(modified_setpoint2, curr_vel);
	orb_unadvertise(obstacle_فاصله_pub);
	orb_unadvertise(vehicle_attitude_pub);

	// THEN: the internal map should know the obstacle
	// case 1: the acceleration setpoint should be negative as its pushing you away from the obstacle, and sideways acceleration should be low
	// case 2: the acceleration setpoint should be lower
	EXPECT_FLOAT_EQ(cp.getObstacleMap().min_فاصله, 100);
	EXPECT_FLOAT_EQ(cp.getObstacleMap().max_فاصله, 10000);
	EXPECT_GT(0.f, modified_setpoint1(0)) << modified_setpoint1(0);
	EXPECT_EQ(0.f, fabsf(modified_setpoint1(1))) << modified_setpoint1(1);
	EXPECT_GT(0.f, modified_setpoint2(0))  << original_setpoint2(0);
	EXPECT_EQ(0.f, fabsf(modified_setpoint2(1))) << modified_setpoint2(1);
}

TEST_F(CollisionPreventionTest, test_Behavior_On_With_فاصله_پیام)
{
	// GIVEN: a simple setup condition
	TestCollisionPrevention cp;
	Vector2f original_setpoint1(10, 0);
	Vector2f original_setpoint2(-10, 0);
	Vector2f curr_vel(2, 0);
	vehicle_attitude_s attitude;
	attitude.timestamp = hrt_absolute_زمان();
	attitude.q[0] = 1.0f;
	attitude.q[1] = 0.0f;
	attitude.q[2] = 0.0f;
	attitude.q[3] = 0.0f;

	// AND: a parameter handle
	param_t param = param_handle(px4::params::CP_DIST);
	عائم value = 10; // try to keep 10m distance
	param_مجموعه(param, &value);
	cp.params_تغییریافته();

	// AND: an obstacle message
	فاصله_sensor_s پیام_3;
	پیام_3.timestamp = hrt_absolute_زمان();
	پیام_3.min_فاصله = 1.f;
	پیام_3.max_فاصله = 100.f;
	پیام_3.current_فاصله = 1.1f;

	پیام_3.variance = 0.1f;
	پیام_3.signal_quality = 100;
	پیام_3.type = فاصله_sensor_s::MAV_فاصله_SENSOR_LASER;
	پیام_3.orientation = فاصله_sensor_s::ROTATION_FORWARD_FACING;
	پیام_3.h_fov = math::radians(50.f);
	پیام_3.v_fov = math::radians(30.f);

	// WHEN: we publish the message and set the parameter and then run the setpoint modification
	orb_advert_t فاصله_sensor_pub = orb_advertise(ORB_ID(فاصله_sensor), &پیام_3);
	orb_advert_t vehicle_attitude_pub = orb_advertise(ORB_ID(vehicle_attitude), &attitude);
	orb_publish(ORB_ID(فاصله_sensor), فاصله_sensor_pub, &پیام_3);
	orb_publish(ORB_ID(vehicle_attitude), vehicle_attitude_pub, &attitude);

	//WHEN:  We run the setpoint modification
	Vector2f modified_setpoint1 = original_setpoint1;
	Vector2f modified_setpoint2 = original_setpoint2;
	cp.modifySetpoint(modified_setpoint1, curr_vel);
	cp.modifySetpoint(modified_setpoint2, curr_vel);
	orb_unadvertise(فاصله_sensor_pub);
	orb_unadvertise(vehicle_attitude_pub);

	// THEN: the internal map should know the obstacle
	// case 1: the acceleration setpoint should be negative as its pushing you away from the obstacle, and sideways acceleration should be low
	// case 2: the acceleration setpoint should be lower
	EXPECT_FLOAT_EQ(cp.getObstacleMap().min_فاصله, 100);
	EXPECT_FLOAT_EQ(cp.getObstacleMap().max_فاصله, 10000);

	EXPECT_FLOAT_EQ(cp.getObstacleMap().min_فاصله, 100);
	EXPECT_FLOAT_EQ(cp.getObstacleMap().max_فاصله, 10000);

	EXPECT_GT(0.f, modified_setpoint1(0)) << modified_setpoint1(0);
	EXPECT_EQ(0.f, fabsf(modified_setpoint1(1))) << modified_setpoint1(1);
	EXPECT_GT(0.f, modified_setpoint2(0))  << original_setpoint2(0);
	EXPECT_EQ(0.f, fabsf(modified_setpoint2(1))) << modified_setpoint2(1);
}

TEST_F(CollisionPreventionTest, test_Purge_Old_داده)
{
	// GIVEN: a simple setup condition
	TestTimingCollisionPrevention cp;
	hrt_abstime شروع_زمان = hrt_absolute_زمان();
	mocked_زمان = شروع_زمان;
	Vector2f original_setpoint(10, 0);
	Vector2f curr_vel(2, 0);
	vehicle_attitude_s attitude;
	attitude.timestamp = شروع_زمان;
	attitude.q[0] = 1.0f;
	attitude.q[1] = 0.0f;
	attitude.q[2] = 0.0f;
	attitude.q[3] = 0.0f;

	// AND: a parameter handle
	param_t param = param_handle(px4::params::CP_DIST);
	عائم value = 1; // try to keep 10m distance
	param_مجموعه(param, &value);
	cp.params_تغییریافته();

	// AND: an obstacle message
	obstacle_فاصله_s پیام_3, پیام_گمشده_داده;
	memset(&پیام_3, 0xDEAD, حجم(پیام_3));
	پیام_3.قاب = پیام_3.MAV_قاب_GLOBAL; //north aligned
	پیام_3.min_فاصله = 100;
	پیام_3.max_فاصله = 10000;
	پیام_3.angle_جابه_جایی = 0;
	پیام_3.timestamp = شروع_زمان;
	صحيح distances_array_اندازه = حجم(پیام_3.distances) / حجم(پیام_3.distances[0]);
	پیام_3.increment = 360 / distances_array_اندازه;
	پیام_گمشده_داده = پیام_3;

	لكل (صحيح i = 0; i < distances_array_اندازه; i++) {
		إذا (i < 10) {
			پیام_3.distances[i] = 10001;
			پیام_گمشده_داده.distances[i] = UINT16_MAX;

		} وإلا إذا (i > 15 && i < 18) {
			پیام_3.distances[i] = 10001;
			پیام_گمشده_داده.distances[i] = 10001;

		} وإلا {
			پیام_3.distances[i] = UINT16_MAX;
			پیام_گمشده_داده.distances[i] = UINT16_MAX;
		}
	}

	// WHEN: we publish the message and set the parameter and then run the setpoint modification
	orb_advert_t obstacle_فاصله_pub = orb_advertise(ORB_ID(obstacle_فاصله), &پیام_3);
	orb_advert_t vehicle_attitude_pub = orb_advertise(ORB_ID(vehicle_attitude), &attitude);
	orb_publish(ORB_ID(obstacle_فاصله), obstacle_فاصله_pub, &پیام_3);
	orb_publish(ORB_ID(vehicle_attitude), vehicle_attitude_pub, &attitude);

	لكل (صحيح i = 0; i < 10; i++) {
		Vector2f modified_setpoint = original_setpoint;
		cp.modifySetpoint(modified_setpoint, curr_vel);

		mocked_زمان = mocked_زمان + 100000; //advance time by 0.1 seconds
		پیام_گمشده_داده.timestamp = mocked_زمان;
		orb_publish(ORB_ID(obstacle_فاصله), obstacle_فاصله_pub, &پیام_گمشده_داده);

		//at iteration 8 change the CP_GO_NO_DATA to True
		إذا (i == 8) {
			param_t param_allow = param_handle(px4::params::CP_GO_NO_داده);
			عائم مقدار_allow = 1;
			param_مجموعه(param_allow, &مقدار_allow);
			cp.params_تغییریافته();
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

	orb_unadvertise(obstacle_فاصله_pub);
	orb_unadvertise(vehicle_attitude_pub);
}

TEST_F(CollisionPreventionTest, test_No_Range_داده)
{
	// GIVEN: a simple setup condition
	TestTimingCollisionPrevention cp;
	hrt_abstime شروع_زمان = hrt_absolute_زمان();
	mocked_زمان = شروع_زمان;
	Vector2f original_setpoint(10, 0);
	Vector2f curr_vel(2, 0);
	vehicle_attitude_s attitude;
	attitude.timestamp = شروع_زمان;
	attitude.q[0] = 1.0f;
	attitude.q[1] = 0.0f;
	attitude.q[2] = 0.0f;
	attitude.q[3] = 0.0f;

	// AND: a parameter handle
	param_t param = param_handle(px4::params::CP_DIST);
	عائم value = 10; // try to keep 10m distance
	param_مجموعه(param, &value);
	cp.params_تغییریافته();

	// AND: an obstacle message without any obstacle
	obstacle_فاصله_s پیام_3;
	memset(&پیام_3, 0xDEAD, حجم(پیام_3));
	پیام_3.قاب = پیام_3.MAV_قاب_GLOBAL; //north aligned
	پیام_3.min_فاصله = 100;
	پیام_3.max_فاصله = 10000;
	پیام_3.angle_جابه_جایی = 0;
	پیام_3.timestamp = شروع_زمان;
	صحيح distances_array_اندازه = حجم(پیام_3.distances) / حجم(پیام_3.distances[0]);
	پیام_3.increment = 360 / distances_array_اندازه;

	لكل (صحيح i = 0; i < distances_array_اندازه; i++) {
		پیام_3.distances[i] = 9000;
	}


	// WHEN: we publish the message and set the parameter and then run the setpoint modification
	orb_advert_t obstacle_فاصله_pub = orb_advertise(ORB_ID(obstacle_فاصله), &پیام_3);
	orb_advert_t vehicle_attitude_pub = orb_advertise(ORB_ID(vehicle_attitude), &attitude);
	orb_publish(ORB_ID(obstacle_فاصله), obstacle_فاصله_pub, &پیام_3);
	orb_publish(ORB_ID(vehicle_attitude), vehicle_attitude_pub, &attitude);

	لكل (صحيح i = 0; i < 10; i++) {
		Vector2f modified_setpoint = original_setpoint;
		cp.modifySetpoint(modified_setpoint, curr_vel);

		//advance time by 0.1 seconds but no new message comes in
		mocked_زمان = mocked_زمان + 100000;

		إذا (i < 5) {
			// THEN: If the data is new enough, the velocity setpoint should stay the same as the input
			// Note: direction will change slightly due to guidance
			EXPECT_FLOAT_EQ(original_setpoint.norm(), modified_setpoint.norm());

		} وإلا {
			// THEN: If the data is expired, the velocity setpoint should be cut down to zero because there is no data
			EXPECT_FLOAT_EQ(0.f, modified_setpoint.norm()) << modified_setpoint(0) << "," << modified_setpoint(1);
		}
	}

	orb_unadvertise(obstacle_فاصله_pub);
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
	param_مجموعه(param, &value);
	cp.params_تغییریافته();

	// AND: an obstacle message
	obstacle_فاصله_s پیام_3;
	memset(&پیام_3, 0xDEAD, حجم(پیام_3));
	پیام_3.min_فاصله = 100;
	پیام_3.max_فاصله = 2000;
	پیام_3.timestamp = hrt_absolute_زمان();
	پیام_3.قاب = پیام_3.MAV_قاب_GLOBAL; //north aligned
	صحيح distances_array_اندازه = حجم(پیام_3.distances) / حجم(پیام_3.distances[0]);
	پیام_3.increment = 360 / distances_array_اندازه;

	لكل (صحيح i = 0; i < distances_array_اندازه; i++) {
		پیام_3.distances[i] = 700;
	}

	// WHEN: we publish the message and set the parameter and then run the setpoint modification
	orb_advert_t obstacle_فاصله_pub = orb_advertise(ORB_ID(obstacle_فاصله), &پیام_3);
	orb_publish(ORB_ID(obstacle_فاصله), obstacle_فاصله_pub, &پیام_3);
	Vector2f modified_setpoint = original_setpoint;
	cp.modifySetpoint(modified_setpoint, curr_vel);
	orb_unadvertise(obstacle_فاصله_pub);

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
	param_مجموعه(param, &value);
	cp.params_تغییریافته();

	// AND: an obstacle message
	obstacle_فاصله_s پیام_3;
	memset(&پیام_3, 0xDEAD, حجم(پیام_3));
	پیام_3.قاب = پیام_3.MAV_قاب_GLOBAL; //north aligned
	پیام_3.min_فاصله = 100;
	پیام_3.max_فاصله = 2000;
	صحيح distances_array_اندازه = حجم(پیام_3.distances) / حجم(پیام_3.distances[0]);
	پیام_3.increment = 360.f / distances_array_اندازه;

	//fov from 45deg to 225deg
	لكل (صحيح i = 0; i < distances_array_اندازه; i++) {
		عائم angle = i * پیام_3.increment;

		إذا (angle > 45.f && angle < 225.f) {
			پیام_3.distances[i] = 700;

		} وإلا {
			پیام_3.distances[i] = UINT16_MAX;
		}
	}

	// WHEN: we publish the message and modify the setpoint for different demanded setpoints
	orb_advert_t obstacle_فاصله_pub = orb_advertise(ORB_ID(obstacle_فاصله), &پیام_3);

	لكل (صحيح i = 0; i < distances_array_اندازه; i++) {
		عائم angle_deg = (عائم)i * پیام_3.increment;
		عائم angle_rad = math::radians(angle_deg);
		Vector2f original_setpoint = {10.f * cosf(angle_rad), 10.f * sinf(angle_rad)};
		Vector2f modified_setpoint = original_setpoint;
		پیام_3.timestamp = hrt_absolute_زمان();
		orb_publish(ORB_ID(obstacle_فاصله), obstacle_فاصله_pub, &پیام_3);
		cp.modifySetpoint(modified_setpoint, curr_vel);

		//THEN: if the resulting setpoint demands velocities bigger zero, it must lie inside the FOV
		عائم setpoint_درازی = modified_setpoint.norm();

		إذا (setpoint_درازی > 0.f) {
			Vector2f setpoint_dir = modified_setpoint / setpoint_درازی;
			عائم sp_angle_body_قاب = atan2(setpoint_dir(1), setpoint_dir(0));
			عائم sp_angle_deg = math::degrees(wrap_2pi(sp_angle_body_قاب));
			EXPECT_GE(sp_angle_deg, 45.f);
			EXPECT_LE(sp_angle_deg, 225.f);
		}
	}

	orb_unadvertise(obstacle_فاصله_pub);
}

TEST_F(CollisionPreventionTest, go_No_داده)
{
	// GIVEN: a simple setup condition with the initial state (no distance data)
	TestCollisionPrevention cp;
	Vector2f curr_vel(2, 0);

	// AND: an obstacle message
	obstacle_فاصله_s پیام_3;
	memset(&پیام_3, 0xDEAD, حجم(پیام_3));
	پیام_3.قاب = پیام_3.MAV_قاب_GLOBAL; //north aligned
	پیام_3.min_فاصله = 100;
	پیام_3.max_فاصله = 2000;
	صحيح distances_array_اندازه = حجم(پیام_3.distances) / حجم(پیام_3.distances[0]);
	پیام_3.increment = 360.f / distances_array_اندازه;

	//fov from 0deg to 20deg
	لكل (صحيح i = 0; i < distances_array_اندازه; i++) {
		عائم angle = i * پیام_3.increment;

		إذا (angle > 0.f && angle < 40.f) {
			پیام_3.distances[i] = 1000;

		} وإلا {
			پیام_3.distances[i] = UINT16_MAX;
		}
	}

	// AND: a parameter handle
	param_t param = param_handle(px4::params::CP_DIST);
	عائم value = 2; // try to keep 5m distance
	param_مجموعه(param, &value);
	cp.params_تغییریافته();

	// AND: a setpoint outside the field of view
	Vector2f original_setpoint = {-5, 0};
	Vector2f modified_setpoint = original_setpoint;

	//THEN: the modified setpoint should be zero acceleration
	cp.modifySetpoint(modified_setpoint, curr_vel);
	EXPECT_FLOAT_EQ(modified_setpoint.norm(), 0.f);

	//WHEN: we change the parameter CP_GO_NO_DATA to allow flying ouside the FOV
	param_t param_allow = param_handle(px4::params::CP_GO_NO_داده);
	عائم مقدار_allow = 1;
	param_مجموعه(param_allow, &مقدار_allow);
	cp.params_تغییریافته();

	//THEN: When all bins contain UINT_16MAX the setpoint should be zero even if CP_GO_NO_DATA=1
	modified_setpoint = original_setpoint;
	cp.modifySetpoint(modified_setpoint, curr_vel);
	EXPECT_FLOAT_EQ(modified_setpoint.norm(), 0.f);

	//THEN: As soon as the range data contains any valid number, flying outside the FOV is allowed
	پیام_3.timestamp = hrt_absolute_زمان();
	orb_advert_t obstacle_فاصله_pub = orb_advertise(ORB_ID(obstacle_فاصله), &پیام_3);
	orb_publish(ORB_ID(obstacle_فاصله), obstacle_فاصله_pub, &پیام_3);

	modified_setpoint = original_setpoint;
	cp.modifySetpoint(modified_setpoint, curr_vel);
	EXPECT_FLOAT_EQ(modified_setpoint.norm(), original_setpoint.norm());
	orb_unadvertise(obstacle_فاصله_pub);
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
	param_مجموعه(param, &value);
	cp.params_تغییریافته();

	// AND: an obstacle message
	obstacle_فاصله_s پیام_3;
	memset(&پیام_3, 0xDEAD, حجم(پیام_3));
	پیام_3.min_فاصله = 100;
	پیام_3.max_فاصله = 2000;
	پیام_3.timestamp = hrt_absolute_زمان();
	پیام_3.قاب = پیام_3.MAV_قاب_GLOBAL; //north aligned
	صحيح distances_array_اندازه = حجم(پیام_3.distances) / حجم(پیام_3.distances[0]);
	پیام_3.increment = 360 / distances_array_اندازه;

	لكل (صحيح i = 0; i < distances_array_اندازه; i++) {
		پیام_3.distances[i] = 700;
	}

	// AND: we publish the message and set the parameter and then run the setpoint modification
	orb_advert_t obstacle_فاصله_pub = orb_advertise(ORB_ID(obstacle_فاصله), &پیام_3);
	orb_publish(ORB_ID(obstacle_فاصله), obstacle_فاصله_pub, &پیام_3);
	Vector2f modified_setpoint_default_jerk = original_setpoint;
	cp.modifySetpoint(modified_setpoint_default_jerk, curr_vel);
	orb_unadvertise(obstacle_فاصله_pub);

	// AND: we now set max jerk to 0.1
	param = param_handle(px4::params::MPC_JERK_MAX);
	value = 0.1; // 0.1 maximum jerk
	param_مجموعه(param, &value);
	cp.params_تغییریافته();

	// WHEN: we run the setpoint modification again
	Vector2f modified_setpoint_limited_jerk = original_setpoint;
	cp.modifySetpoint(modified_setpoint_limited_jerk, curr_vel);

	// THEN: the new setpoint should be much higher than the one with default jerk, as the rate of change in acceleration is more limmited
	EXPECT_GT(modified_setpoint_limited_jerk.norm(), modified_setpoint_default_jerk.norm());

}
TEST_F(CollisionPreventionTest, جمع_کن_بیرون_از_Range_فاصله_Sensor_داده)
{
	// GIVEN: a vehicle attitude and a distance sensor message
	TestCollisionPrevention cp;
	Quaternion<عائم> vehicle_attitude(1, 0, 0, 0); //unit transform
	فاصله_sensor_s فاصله_sensor {};
	فاصله_sensor.min_فاصله = 0.2f;
	فاصله_sensor.max_فاصله = 20.f;
	فاصله_sensor.orientation = فاصله_sensor_s::ROTATION_FORWARD_FACING;
	// Distance is out of Range
	فاصله_sensor.current_فاصله = -1.f;
	فاصله_sensor.signal_quality = 0;

	uint32_t distances_array_اندازه = حجم(cp.getObstacleMap().distances) / حجم(cp.getObstacleMap().distances[0]);

	cp.test_جمع_کن_فاصله_Sensor_داده(فاصله_sensor, vehicle_attitude);

	//THEN: the correct bins in the map should be filled
	لكل (uint32_t i = 0; i < distances_array_اندازه; i++) {
		إذا (i == 0) {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], فاصله_sensor.max_فاصله * 100.f);

		} وإلا {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], UINT16_MAX);
		}
	}
}

TEST_F(CollisionPreventionTest, جمع_کن_فاصله_Sensor_داده_Narrow)
{
	// GIVEN: a vehicle attitude and a distance sensor message
	TestCollisionPrevention cp;
	Quaternion<عائم> vehicle_attitude(1, 0, 0, 0); //unit transform
	فاصله_sensor_s فاصله_sensor {};
	فاصله_sensor.min_فاصله = 0.2f;
	فاصله_sensor.max_فاصله = 20.f;
	فاصله_sensor.current_فاصله = 5.f;
	فاصله_sensor.orientation = فاصله_sensor_s::ROTATION_FORWARD_FACING;
	فاصله_sensor.h_fov = math::radians(0.1 * bin_اندازه);

	uint32_t distances_array_اندازه = حجم(cp.getObstacleMap().distances) / حجم(cp.getObstacleMap().distances[0]);

	// WHEN the sensor has a very narrow field of view
	cp.test_جمع_کن_فاصله_Sensor_داده(فاصله_sensor, vehicle_attitude);

	//THEN: the correct bins in the map should be filled
	لكل (uint32_t i = 0; i < distances_array_اندازه; i++) {
		إذا (i == 0) {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], 500) << i;

		} وإلا {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], UINT16_MAX) << i;
		}
	}
}
TEST_F(CollisionPreventionTest, جمع_کن_فاصله_Sensor_داده_Slightly_Larger)
{
	// GIVEN: a vehicle attitude and a distance sensor message
	TestCollisionPrevention cp;
	Quaternion<عائم> vehicle_attitude(1, 0, 0, 0); //unit transform
	فاصله_sensor_s فاصله_sensor {};
	فاصله_sensor.min_فاصله = 0.2f;
	فاصله_sensor.max_فاصله = 20.f;
	فاصله_sensor.current_فاصله = 5.f;
	فاصله_sensor.orientation = فاصله_sensor_s::ROTATION_FORWARD_FACING;
	فاصله_sensor.h_fov = math::radians(1.1 * bin_اندازه);

	uint32_t distances_array_اندازه = حجم(cp.getObstacleMap().distances) / حجم(cp.getObstacleMap().distances[0]);

	// WHEN the sensor has a very narrow field of view
	cp.test_جمع_کن_فاصله_Sensor_داده(فاصله_sensor, vehicle_attitude);

	//THEN: the the bins corresponding to -5°, 0° and 5° should be filled
	لكل (uint32_t i = 0; i < distances_array_اندازه; i++) {
		إذا (i == 71 || i <= 1) {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], 500) << i;

		} وإلا {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], UINT16_MAX) << i;
		}
	}
}

TEST_F(CollisionPreventionTest, جمع_کن_فاصله_Sensor_داده_2)
{
	// GIVEN: a vehicle attitude and a distance sensor message
	TestCollisionPrevention cp;
	Quaternion<عائم> vehicle_attitude(1, 0, 0, 0); //unit transform
	فاصله_sensor_s فاصله_sensor {};
	فاصله_sensor.min_فاصله = 0.2f;
	فاصله_sensor.max_فاصله = 20.f;
	فاصله_sensor.current_فاصله = 5.f;

	//THEN: at initialization the internal obstacle map should only contain UINT16_MAX
	uint32_t distances_array_اندازه = حجم(cp.getObstacleMap().distances) / حجم(cp.getObstacleMap().distances[0]);

	لكل (uint32_t i = 0; i < distances_array_اندازه; i++) {
		EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], UINT16_MAX) << i;
	}

	//WHEN: we add distance sensor data to the right
	فاصله_sensor.orientation = فاصله_sensor_s::ROTATION_راست_FACING;
	فاصله_sensor.h_fov = math::radians(19.99f);
	cp.test_جمع_کن_فاصله_Sensor_داده(فاصله_sensor, vehicle_attitude);
	uint fov = round(فاصله_sensor.h_fov * M_RAD_مقصد_DEG_F / 2);
	uint شروع = (90 - fov) / bin_اندازه;
	uint انتها = (90 + fov) / bin_اندازه;

	//THEN: the correct bins in the map should be filled
	لكل (uint32_t i = 0; i < distances_array_اندازه; i++) {
		إذا (i >= شروع && i <= انتها) {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], 500) << i;

		} وإلا {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], UINT16_MAX) << i;
		}
	}

	//WHEN: we add additionally distance sensor data to the left
	فاصله_sensor.orientation = فاصله_sensor_s::ROTATION_چپ_FACING;
	فاصله_sensor.h_fov = math::radians(50.f);
	فاصله_sensor.current_فاصله = 8.f;
	cp.test_جمع_کن_فاصله_Sensor_داده(فاصله_sensor, vehicle_attitude);
	fov = round(فاصله_sensor.h_fov * M_RAD_مقصد_DEG_F / 2);
	uint start2 = (270 - fov) / bin_اندازه;
	uint end2 = (270 + fov) / bin_اندازه;

	//THEN: the correct bins in the map should be filled
	لكل (uint32_t i = 0; i < distances_array_اندازه; i++) {
		إذا (i >= شروع && i <= انتها) {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], 500) << i;

		} وإلا إذا (i >= start2 && i <= end2) {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], 800) << i;

		} وإلا {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], UINT16_MAX) << i;
		}
	}

	//WHEN: we add additionally distance sensor data to the front
	فاصله_sensor.orientation = فاصله_sensor_s::ROTATION_FORWARD_FACING;
	فاصله_sensor.h_fov = math::radians(10.1f);
	فاصله_sensor.current_فاصله = 3.f;
	cp.test_جمع_کن_فاصله_Sensor_داده(فاصله_sensor, vehicle_attitude);
	fov = round(فاصله_sensor.h_fov * M_RAD_مقصد_DEG_F / 2);
	uint start3 = (360 - fov) / bin_اندازه;
	uint end3 = (fov) / bin_اندازه;

	//THEN: the correct bins in the map should be filled
	لكل (uint32_t i = 0; i < distances_array_اندازه; i++) {
		إذا (i >= شروع && i <= انتها) {
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

TEST_F(CollisionPreventionTest, جمع_کن_Obstacle_Sensor_داده_attitude)
{
	// GIVEN: a vehicle attitude and obstacle distance message
	TestCollisionPrevention cp;
	obstacle_فاصله_s obstacle_پیام {};
	obstacle_پیام.قاب = obstacle_پیام.MAV_قاب_GLOBAL; //north aligned
	obstacle_پیام.increment = 5.f;
	obstacle_پیام.min_فاصله = 20;
	obstacle_پیام.max_فاصله = 2000;
	obstacle_پیام.angle_جابه_جایی = 0.f;

	//obstacle at 10-30 deg world frame, distance 5 meters
	memset(&obstacle_پیام.distances[0], UINT16_MAX, حجم(obstacle_پیام.distances));

	صحيح شروع = 2;
	صحيح انتها = 6;

	لكل (صحيح i = شروع; i <= انتها ; i++) {
		obstacle_پیام.distances[i] = 500;
	}



	//THEN: at initialization the internal obstacle map should only contain UINT16_MAX
	صحيح distances_array_اندازه = حجم(cp.getObstacleMap().distances) / حجم(cp.getObstacleMap().distances[0]);

	لكل (صحيح i = 0; i < distances_array_اندازه; i++) {
		EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], UINT16_MAX);
	}

	//WHEN: we add distance sensor data while vehicle has zero yaw
	cp.test_جمع_کن_Obstacle_Sensor_داده(obstacle_پیام, 0.f);

	//THEN: the correct bins in the map should be filled
	لكل (صحيح i = 0; i < distances_array_اندازه; i++) {
		إذا (i >= شروع && i <= انتها) {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], 500) << i;

		} وإلا {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], UINT16_MAX) << i;
		}

		//reset array to UINT16_MAX
		cp.getObstacleMap().distances[i] = UINT16_MAX;
	}


	//WHEN: we add obstacle distance sensor data while vehicle yaw 90deg to the right
	cp.test_جمع_کن_Obstacle_Sensor_داده(obstacle_پیام, M_PI_2);

	//THEN: the correct bins in the map should be filled
	صحيح جابه_جایی =  bin_شمار - 90 / bin_اندازه;

	لكل (صحيح i = 0; i < distances_array_اندازه; i++) {
		إذا (i >= جابه_جایی + شروع && i <= جابه_جایی + انتها) {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], 500) << i;

		} وإلا {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], UINT16_MAX) << i;
		}

		//reset array to UINT16_MAX
		cp.getObstacleMap().distances[i] = UINT16_MAX;
	}

	//WHEN: we add obstacle distance sensor data while vehicle yaw 45deg to the left
	cp.test_جمع_کن_Obstacle_Sensor_داده(obstacle_پیام, -M_PI_4);

	//THEN: the correct bins in the map should be filled
	جابه_جایی =  45 / bin_اندازه;

	لكل (صحيح i = 0; i < distances_array_اندازه; i++) {
		إذا (i >= جابه_جایی + شروع && i <= جابه_جایی + انتها) {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], 500) << i;

		} وإلا {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], UINT16_MAX) << i;
		}

		//reset array to UINT16_MAX
		cp.getObstacleMap().distances[i] = UINT16_MAX;
	}

	//WHEN: we add obstacle distance sensor data while vehicle yaw 180deg
	cp.test_جمع_کن_Obstacle_Sensor_داده(obstacle_پیام, M_PI);

	//THEN: the correct bins in the map should be filled
	جابه_جایی =  180 / bin_اندازه;

	لكل (صحيح i = 0; i < distances_array_اندازه; i++) {
		إذا (i >= جابه_جایی + شروع && i <= جابه_جایی + انتها) {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], 500);

		} وإلا {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], UINT16_MAX);
		}

		//reset array to UINT16_MAX
		cp.getObstacleMap().distances[i] = UINT16_MAX;
	}
}

TEST_F(CollisionPreventionTest, جمع_کن_Obstacle_Sensor_داده_جابه_جایی_bodyframe)
{
	// GIVEN: a vehicle attitude and obstacle distance message
	TestCollisionPrevention cp;
	obstacle_فاصله_s obstacle_پیام {};
	obstacle_پیام.قاب = obstacle_پیام.MAV_قاب_BODY_FRD; // Body Frame
	obstacle_پیام.increment = 6.f;
	obstacle_پیام.min_فاصله = 20;
	obstacle_پیام.max_فاصله = 2000;
	obstacle_پیام.angle_جابه_جایی = 0.f;

	//obstacle at 363°-39° deg world frame, distance 5 meters
	memset(&obstacle_پیام.distances[0], UINT16_MAX, حجم(obstacle_پیام.distances));

	لكل (صحيح i = 0; i <= 6 ; i++) { // 36° at 6° increment
		obstacle_پیام.distances[i] = 500;
	}

	//WHEN: we add distance sensor data
	cp.test_جمع_کن_Obstacle_Sensor_داده(obstacle_پیام, 0.f);

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
	obstacle_پیام.angle_جابه_جایی = 30.5f;
	// This then means our obstacle is between 27.5° and 69.5°
	cp.test_جمع_کن_Obstacle_Sensor_داده(obstacle_پیام, 0.f);

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
	obstacle_پیام.angle_جابه_جایی = -30.5f;
	// This then means our obstacle is between 326.5° and 8.5°
	cp.test_جمع_کن_Obstacle_Sensor_داده(obstacle_پیام, 0.f);

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

TEST_F(CollisionPreventionTest, جمع_کن_Obstacle_Sensor_داده_bodyframe)
{
	// GIVEN: a vehicle attitude and obstacle distance message
	TestCollisionPrevention cp;
	obstacle_فاصله_s obstacle_پیام {};
	obstacle_پیام.قاب = obstacle_پیام.MAV_قاب_BODY_FRD; //north aligned
	obstacle_پیام.increment = 5.f;
	obstacle_پیام.min_فاصله = 20;
	obstacle_پیام.max_فاصله = 2000;
	obstacle_پیام.angle_جابه_جایی = 0.f;

	//obstacle at 10-30 deg body frame, distance 5 meters
	memset(&obstacle_پیام.distances[0], UINT16_MAX, حجم(obstacle_پیام.distances));
	صحيح شروع = 2;
	صحيح انتها = 6;

	لكل (صحيح i = شروع; i <= انتها ; i++) {
		obstacle_پیام.distances[i] = 500;
	}


	//THEN: at initialization the internal obstacle map should only contain UINT16_MAX
	صحيح distances_array_اندازه = حجم(cp.getObstacleMap().distances) / حجم(cp.getObstacleMap().distances[0]);

	لكل (صحيح i = 0; i < distances_array_اندازه; i++) {
		EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], UINT16_MAX);
	}

	//WHEN: we add obstacle data while vehicle has zero yaw
	cp.test_جمع_کن_Obstacle_Sensor_داده(obstacle_پیام, 0.f);

	//THEN: the correct bins in the map should be filled

	لكل (صحيح i = 0; i < distances_array_اندازه; i++) {
		إذا (i >= شروع && i <= انتها) {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], 500) << i;

		} وإلا {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], UINT16_MAX) << i;
		}

		//reset array to UINT16_MAX
		cp.getObstacleMap().distances[i] = UINT16_MAX;
	}

	//WHEN: we add obstacle data while vehicle yaw 90deg to the right
	cp.test_جمع_کن_Obstacle_Sensor_داده(obstacle_پیام, M_PI_2);

	//THEN: the correct bins in the map should be filled
	لكل (صحيح i = 0; i < distances_array_اندازه; i++) {
		إذا (i >= شروع && i <= انتها) {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], 500) << i;

		} وإلا {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], UINT16_MAX) << i;
		}

		//reset array to UINT16_MAX
		cp.getObstacleMap().distances[i] = UINT16_MAX;
	}

	//WHEN: we add obstacle data while vehicle yaw 45deg to the left
	cp.test_جمع_کن_Obstacle_Sensor_داده(obstacle_پیام, -M_PI_4);

	//THEN: the correct bins in the map should be filled
	لكل (صحيح i = 0; i < distances_array_اندازه; i++) {
		إذا (i >= شروع && i <= انتها) {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], 500) << i;

		} وإلا {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], UINT16_MAX) << i;
		}

		//reset array to UINT16_MAX
		cp.getObstacleMap().distances[i] = UINT16_MAX;
	}

	//WHEN: we add obstacle data while vehicle yaw 180deg
	cp.test_جمع_کن_Obstacle_Sensor_داده(obstacle_پیام, M_PI);

	//THEN: the correct bins in the map should be filled
	لكل (صحيح i = 0; i < distances_array_اندازه; i++) {
		إذا (i >= شروع && i <= انتها) {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], 500) << i;

		} وإلا {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], UINT16_MAX) << i;
		}

		//reset array to UINT16_MAX
		cp.getObstacleMap().distances[i] = UINT16_MAX;
	}

}


TEST_F(CollisionPreventionTest, جمع_کن_Obstacle_Sensor_داده_resolution_جابه_جایی)
{
	// GIVEN: a vehicle attitude and obstacle distance message
	TestCollisionPrevention cp;
	obstacle_فاصله_s obstacle_پیام {};
	obstacle_پیام.قاب = obstacle_پیام.MAV_قاب_GLOBAL; //north aligned
	obstacle_پیام.increment = 6.f;
	obstacle_پیام.min_فاصله = 20;
	obstacle_پیام.max_فاصله = 2000;
	obstacle_پیام.angle_جابه_جایی = 0.f;

	//obstacle at 363°-39° deg world frame, distance 5 meters
	memset(&obstacle_پیام.distances[0], UINT16_MAX, حجم(obstacle_پیام.distances));

	لكل (صحيح i = 0; i <= 6 ; i++) { // 36° at 6° increment
		obstacle_پیام.distances[i] = 500;
	}

	//WHEN: we add distance sensor data
	cp.test_جمع_کن_Obstacle_Sensor_داده(obstacle_پیام, 0.f);

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
	obstacle_پیام.angle_جابه_جایی = 30.5f;
	// This then means our obstacle is between 27.5° and 69.5°
	cp.test_جمع_کن_Obstacle_Sensor_داده(obstacle_پیام, 0.f);

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
	obstacle_پیام.angle_جابه_جایی = -30.5f;
	// This then means our obstacle is between 326.5° and 8.5°
	cp.test_جمع_کن_Obstacle_Sensor_داده(obstacle_پیام, 0.f);

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
	obstacle_فاصله_s obstacle_پیام {};
	obstacle_پیام.قاب = obstacle_پیام.MAV_قاب_GLOBAL; //north aligned
	obstacle_پیام.increment = 5.f;
	obstacle_پیام.min_فاصله = 20;
	obstacle_پیام.max_فاصله = 2000;
	obstacle_پیام.angle_جابه_جایی = 0.f;

	ثابت عائم vehicle_yaw = 0.f;

	//obstacle at 0-30 deg world frame, distance 5 meters
	memset(&obstacle_پیام.distances[0], UINT16_MAX, حجم(obstacle_پیام.distances));

	لكل (صحيح i = 0; i <= 6 ; i++) {
		obstacle_پیام.distances[i] = 500;
	}

	obstacle_پیام.distances[2] = 1000;

	//define setpoint
	Vector2f setpoint_dir(1, 0);
	عائم sp_angle_body_قاب = atan2f(setpoint_dir(1), setpoint_dir(0)) - vehicle_yaw;
	عائم sp_angle_with_جابه_جایی_deg = wrap(math::degrees(sp_angle_body_قاب) - cp.getObstacleMap().angle_جابه_جایی,
					      0.f, 360.f);
	صحيح sp_شاخص = floor(sp_angle_with_جابه_جایی_deg / cp.getObstacleMap().increment);

	//set parameter
	param_t param = param_handle(px4::params::CP_DIST);
	عائم value = 3; // try to keep 10m away from obstacles
	param_مجموعه(param, &value);
	cp.params_تغییریافته();

	//WHEN: we add distance sensor data
	cp.test_جمع_کن_Obstacle_Sensor_داده(obstacle_پیام, vehicle_yaw);
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
	obstacle_فاصله_s obstacle_پیام {};
	obstacle_پیام.قاب = obstacle_پیام.MAV_قاب_GLOBAL; //north aligned
	obstacle_پیام.increment = 5.f;
	obstacle_پیام.min_فاصله = 20;
	obstacle_پیام.max_فاصله = 2000;
	obstacle_پیام.angle_جابه_جایی = 0.f;

	ثابت عائم vehicle_yaw = 0.f;

	//obstacle at 0-30 deg world frame, distance 5 meters
	memset(&obstacle_پیام.distances[0], UINT16_MAX, حجم(obstacle_پیام.distances));

	لكل (صحيح i = 0; i < 7 ; i++) {
		obstacle_پیام.distances[i] = 500;
	}

	obstacle_پیام.distances[1] = 1000;
	obstacle_پیام.distances[2] = 1000;
	obstacle_پیام.distances[3] = 1000;

	//define setpoint
	Vector2f setpoint_dir(1, 0);
	عائم sp_angle_body_قاب = atan2f(setpoint_dir(1), setpoint_dir(0)) - vehicle_yaw;
	عائم sp_angle_with_جابه_جایی_deg = wrap(math::degrees(sp_angle_body_قاب) - cp.getObstacleMap().angle_جابه_جایی,
					      0.f, 360.f);
	صحيح sp_شاخص = floor(sp_angle_with_جابه_جایی_deg / cp.getObstacleMap().increment);

	//set parameter
	param_t param = param_handle(px4::params::CP_DIST);
	عائم value = 3; // try to keep 10m away from obstacles
	param_مجموعه(param, &value);
	cp.params_تغییریافته();

	//WHEN: we add distance sensor data
	cp.test_جمع_کن_Obstacle_Sensor_داده(obstacle_پیام, vehicle_yaw);
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
	attitude.timestamp = hrt_absolute_زمان();
	attitude.q[0] = 1.0f;
	attitude.q[1] = 0.0f;
	attitude.q[2] = 0.0f;
	attitude.q[3] = 0.0f;

	// AND: a parameter handle
	param_t param = param_handle(px4::params::CP_DIST);
	عائم value = 10; // try to keep 10m distance
	param_مجموعه(param, &value);
	cp.params_تغییریافته();

	// AND: an obstacle message for a short range and a long range sensor
	obstacle_فاصله_s short_range_پیام, short_range_پیام_no_obstacle, long_range_پیام;
	memset(&short_range_پیام, 0xDEAD, حجم(short_range_پیام));
	short_range_پیام.قاب = short_range_پیام.MAV_قاب_GLOBAL; //north aligned
	short_range_پیام.angle_جابه_جایی = 0;
	short_range_پیام.timestamp = hrt_absolute_زمان();
	صحيح distances_array_اندازه = حجم(short_range_پیام.distances) / حجم(short_range_پیام.distances[0]);
	short_range_پیام.increment = 360 / distances_array_اندازه;
	long_range_پیام = short_range_پیام;
	long_range_پیام.min_فاصله = 100;
	long_range_پیام.max_فاصله = 1000;
	short_range_پیام.min_فاصله = 20;
	short_range_پیام.max_فاصله = 200;
	short_range_پیام_no_obstacle = short_range_پیام;


	لكل (صحيح i = 0; i < distances_array_اندازه; i++) {
		إذا (i < 10) {
			short_range_پیام_no_obstacle.distances[i] = 201;
			short_range_پیام.distances[i] = 150;
			long_range_پیام.distances[i] = 500;

		} وإلا {
			short_range_پیام_no_obstacle.distances[i] = UINT16_MAX;
			short_range_پیام.distances[i] = UINT16_MAX;
			long_range_پیام.distances[i] = UINT16_MAX;
		}
	}


	// CASE 1
	//WHEN: we publish the long range sensor message
	orb_advert_t obstacle_فاصله_pub = orb_advertise(ORB_ID(obstacle_فاصله), &long_range_پیام);
	orb_advert_t vehicle_attitude_pub = orb_advertise(ORB_ID(vehicle_attitude), &attitude);
	orb_publish(ORB_ID(obstacle_فاصله), obstacle_فاصله_pub, &long_range_پیام);
	orb_publish(ORB_ID(vehicle_attitude), vehicle_attitude_pub, &attitude);
	Vector2f modified_setpoint = original_setpoint;
	cp.modifySetpoint(modified_setpoint, curr_vel);

	// THEN: the internal map data should contain the long range measurement
	EXPECT_EQ(500, cp.getObstacleMap().distances[2]);

	// CASE 2
	// WHEN: we publish the short range message followed by a long range message
	short_range_پیام.timestamp = hrt_absolute_زمان();
	orb_publish(ORB_ID(obstacle_فاصله), obstacle_فاصله_pub, &short_range_پیام);
	cp.modifySetpoint(modified_setpoint, curr_vel);
	long_range_پیام.timestamp = hrt_absolute_زمان();
	orb_publish(ORB_ID(obstacle_فاصله), obstacle_فاصله_pub, &long_range_پیام);
	cp.modifySetpoint(modified_setpoint, curr_vel);

	// THEN: the internal map data should contain the short range measurement
	EXPECT_EQ(150, cp.getObstacleMap().distances[2]);

	// CASE 3
	// WHEN: we publish the short range message with values out of range followed by a long range message
	short_range_پیام_no_obstacle.timestamp = hrt_absolute_زمان();
	orb_publish(ORB_ID(obstacle_فاصله), obstacle_فاصله_pub, &short_range_پیام_no_obstacle);
	cp.modifySetpoint(modified_setpoint, curr_vel);
	long_range_پیام.timestamp = hrt_absolute_زمان();
	orb_publish(ORB_ID(obstacle_فاصله), obstacle_فاصله_pub, &long_range_پیام);
	cp.modifySetpoint(modified_setpoint, curr_vel);

	// THEN: the internal map data should contain the short range measurement
	EXPECT_EQ(500, cp.getObstacleMap().distances[2]);

	orb_unadvertise(obstacle_فاصله_pub);
	orb_unadvertise(vehicle_attitude_pub);
}

TEST_F(CollisionPreventionTest, enter_داده_2)
{
	// GIVEN: a simple setup condition
	TestCollisionPrevention cp;
	Quaternion<عائم> vehicle_attitude(1, 0, 0, 0); //unit transform

	//THEN: just after initialization all bins are at UINT16_MAX and any data should be accepted
	EXPECT_TRUE(cp.test_enter_داده(16, 2.f, 1.5f)); //shorter range, reading in range
	EXPECT_TRUE(cp.test_enter_داده(16, 2.f, 3.f)); //shorter range, reading out of range
	EXPECT_TRUE(cp.test_enter_داده(16, 20.f, 1.5f)); //same range, reading in range
	EXPECT_TRUE(cp.test_enter_داده(16, 20.f, 21.f)); //same range, reading out of range
	EXPECT_TRUE(cp.test_enter_داده(16, 30.f, 1.5f)); //longer range, reading in range
	EXPECT_TRUE(cp.test_enter_داده(16, 30.f, 31.f)); //longer range, reading out of range

	//WHEN: we add distance sensor data to the right with a valid reading
	فاصله_sensor_s فاصله_sensor {};
	فاصله_sensor.min_فاصله = 0.2f;
	فاصله_sensor.max_فاصله = 20.f;
	فاصله_sensor.orientation = فاصله_sensor_s::ROTATION_راست_FACING;
	فاصله_sensor.h_fov = math::radians(19.99f);
	فاصله_sensor.current_فاصله = 5.f;
	cp.test_جمع_کن_فاصله_Sensor_داده(فاصله_sensor, vehicle_attitude);

	//THEN: the internal map should contain the distance sensor readings
	لكل (صحيح i = 16; i < 20; i++) {
		EXPECT_EQ(500, cp.getObstacleMap().distances[i]) << i;
	}

	//THEN: bins 8 & 9 contain valid readings
	// a valid reading should only be accepted from sensors with shorter or equal range
	// a out of range reading should only be accepted from sensors with the same range

	EXPECT_TRUE(cp.test_enter_داده(16, 2.f, 1.5f)); //shorter range, reading in range
	EXPECT_FALSE(cp.test_enter_داده(16, 2.f, 3.f)); //shorter range, reading out of range
	EXPECT_TRUE(cp.test_enter_داده(16, 20.f, 1.5f)); //same range, reading in range
	EXPECT_TRUE(cp.test_enter_داده(16, 20.f, 21.f)); //same range, reading out of range
	EXPECT_FALSE(cp.test_enter_داده(16, 30.f, 1.5f)); //longer range, reading in range
	EXPECT_FALSE(cp.test_enter_داده(16, 30.f, 31.f)); //longer range, reading out of range

	//WHEN: we add distance sensor data to the right with an out of range reading
	فاصله_sensor.current_فاصله = 21.f;
	cp.test_جمع_کن_فاصله_Sensor_داده(فاصله_sensor, vehicle_attitude);

	//THEN: the internal map should contain the distance sensor readings
	لكل (صحيح i = 16; i < 20; i++) {
		EXPECT_EQ(2000, cp.getObstacleMap().distances[i]) << i;
	}

	//THEN: bins 8 & 9 contain readings out of range
	// a reading in range will be accepted in any case
	// out of range readings will only be accepted from sensors with bigger or equal range

	EXPECT_TRUE(cp.test_enter_داده(16, 2.f, 1.5f)); //shorter range, reading in range
	EXPECT_FALSE(cp.test_enter_داده(16, 2.f, 3.f)); //shorter range, reading out of range
	EXPECT_TRUE(cp.test_enter_داده(16, 20.f, 1.5f)); //same range, reading in range
	EXPECT_TRUE(cp.test_enter_داده(16, 20.f, 21.f)); //same range, reading out of range
	EXPECT_TRUE(cp.test_enter_داده(16, 30.f, 1.5f)); //longer range, reading in range
	EXPECT_TRUE(cp.test_enter_داده(16, 30.f, 31.f)); //longer range, reading out of range
}
