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

#ਅਨ੍ਤਰ੍ਭਾਵ <gtest/gtest.h>
#ਅਨ੍ਤਰ੍ਭਾਵ "CollisionPrevention.hpp"

ਵਰਤੋਂ ਨਾਮਾਕਾਸ਼ ਮੈਟ੍ਰਿਕਸ;

// to run: make tests TESTFILTER=CollisionPrevention
hrt_abstime mocked_ਸਮਾਂ = 0;
ਅਚਲ uint bin_ਮਾਪ = CollisionPrevention::BIN_ਮਾਪ;
ਅਚਲ uint bin_ਗਿਣਤੀ = CollisionPrevention::BIN_ਗਿਣਤੀ;

ਵਰਗ CollisionPreventionTest : ਜਨਤਕ ::testing::Test
{
ਜਨਤਕ:
	ਖਾਲੀ ਸੈੱਟ_ਉੱਪਰ() override
	{
		param_control_autosave(ਝੂਠ);
		param_reset_all();
	}
};

ਵਰਗ TestCollisionPrevention : ਜਨਤਕ CollisionPrevention
{
ਜਨਤਕ:
	TestCollisionPrevention() : CollisionPrevention(ਖਾਲੀ_ਸੂਚਕ) {}
	ਖਾਲੀ params_ਬਦਲਿਆ() {CollisionPrevention::updateParamsImpl();}
	obstacle_ਦੂਰੀ_s &getObstacleMap() {ਵਾਪਸ obstacle_map_body_ਚੌਖਟਾ;}
	ਖਾਲੀ test_ਜੋੜੋ_ਦੂਰੀ_Sensor_ਅੰਕੜੇ(ਦੂਰੀ_sensor_s &ਦੂਰੀ_sensor, ਅਚਲ Quatf &attitude)
	{
		ਜੋੜੋ_ਦੂਰੀ_Sensor_ਅੰਕੜੇ(ਦੂਰੀ_sensor, attitude);
	}
	ਖਾਲੀ test_ਜੋੜੋ_Obstacle_Sensor_ਅੰਕੜੇ(ਅਚਲ obstacle_ਦੂਰੀ_s &obstacle, ਅਚਲ ਦਸ਼ਮਲਵ vehicle_yaw)
	{
		ਜੋੜੋ_Obstacle_Sensor_ਅੰਕੜੇ(obstacle, vehicle_yaw);
	}
	ਖਾਲੀ test_adaptSetpointDirection(Vector2f &setpoint_dir, ਪੂਰਨ &setpoint_ਸੂਚਕਾਂਕ_2,
					 ਦਸ਼ਮਲਵ vehicle_yaw_angle_rad)
	{
		_adaptSetpointDirection(setpoint_dir, setpoint_ਸੂਚਕਾਂਕ_2, vehicle_yaw_angle_rad);
	}
	ਤਾਰਕਿਕ test_enter_ਅੰਕੜੇ(ਪੂਰਨ map_ਸੂਚਕਾਂਕ, ਦਸ਼ਮਲਵ sensor_range, ਦਸ਼ਮਲਵ sensor_reading)
	{
		ਵਾਪਸ enter_ਅੰਕੜੇ(map_ਸੂਚਕਾਂਕ, sensor_range, sensor_reading);
	}
};

ਵਰਗ TestTimingCollisionPrevention : ਜਨਤਕ TestCollisionPrevention
{
ਜਨਤਕ:
	TestTimingCollisionPrevention() : TestCollisionPrevention() {}
ਸੁਰੱਖਿਅਤ:
	hrt_abstime get_ਸਮਾਂ() override
	{
		ਵਾਪਸ mocked_ਸਮਾਂ;
	}

	hrt_abstime get_Elapsed_ਸਮਾਂ(ਅਚਲ hrt_abstime *ptr) override
	{
		ਵਾਪਸ mocked_ਸਮਾਂ - *ptr;
	}
};

TEST_F(CollisionPreventionTest, instantiation) { CollisionPrevention cp(ਖਾਲੀ_ਸੂਚਕ); }

TEST_F(CollisionPreventionTest, behaviorOff)
{
	// GIVEN: a simple setup condition
	TestCollisionPrevention cp;

	// THEN: the collision prevention should be turned off by default
	EXPECT_FALSE(cp.is_ਸਰਗਰਮ());
}

TEST_F(CollisionPreventionTest, no_Sensor_ਅੰਕੜੇ)
{
	// GIVEN: a simple setup condition
	TestCollisionPrevention cp;
	Vector2f original_setpoint(10, 0);
	Vector2f curr_vel(2, 0);

	// AND: a parameter handle
	param_t param = param_handle(px4::params::CP_DIST);

	// WHEN: we set the parameter check then apply the setpoint modification
	ਦਸ਼ਮਲਵ value = 10; // try to keep 10m away from obstacles
	param_ਸੈੱਟ(param, &value);
	cp.params_ਬਦਲਿਆ();

	Vector2f modified_setpoint = original_setpoint;
	cp.modifySetpoint(modified_setpoint, curr_vel);

	// THEN: collision prevention should be enabled and limit the speed to zero
	EXPECT_TRUE(cp.is_ਸਰਗਰਮ());
	EXPECT_FLOAT_EQ(0.f, modified_setpoint.norm());
}

TEST_F(CollisionPreventionTest, test_Behavior_On_With_Obstacle_ਸੁਨੇਹਾ)
{
	// GIVEN: a simple setup condition
	TestCollisionPrevention cp;
	Vector2f original_setpoint1(10, 0);
	Vector2f original_setpoint2(-10, 0);
	Vector2f curr_vel(2, 0);
	vehicle_attitude_s attitude;
	attitude.timestamp = hrt_absolute_ਸਮਾਂ();
	attitude.q[0] = 1.0f;
	attitude.q[1] = 0.0f;
	attitude.q[2] = 0.0f;
	attitude.q[3] = 0.0f;

	// AND: a parameter handle
	param_t param1 = param_handle(px4::params::CP_DIST);
	ਦਸ਼ਮਲਵ value1 = 10; // try to keep 10m distance
	param_ਸੈੱਟ(param1, &value1);
	param_t param2 = param_handle(px4::params::CP_GUIDE_ANG);
	ਦਸ਼ਮਲਵ value2 = 0; // dont guide sideways
	param_ਸੈੱਟ(param2, &value2);
	cp.params_ਬਦਲਿਆ();

	// AND: an obstacle message
	obstacle_ਦੂਰੀ_s ਸੁਨੇਹਾ_3;
	memset(&ਸੁਨੇਹਾ_3, 0xDEAD, ਆਕਾਰ(ਸੁਨੇਹਾ_3));
	ਸੁਨੇਹਾ_3.ਚੌਖਟਾ = ਸੁਨੇਹਾ_3.MAV_ਚੌਖਟਾ_GLOBAL; //north aligned
	ਸੁਨੇਹਾ_3.min_ਦੂਰੀ = 100;
	ਸੁਨੇਹਾ_3.max_ਦੂਰੀ = 10000;
	ਸੁਨੇਹਾ_3.angle_ਵਿੱਥ = 0;
	ਸੁਨੇਹਾ_3.timestamp = hrt_absolute_ਸਮਾਂ();
	ਪੂਰਨ distances_array_ਮਾਪ = ਆਕਾਰ(ਸੁਨੇਹਾ_3.distances) / ਆਕਾਰ(ਸੁਨੇਹਾ_3.distances[0]);
	ਸੁਨੇਹਾ_3.increment = 360 / distances_array_ਮਾਪ;

	ਲਈ (ਪੂਰਨ i = 0; i < distances_array_ਮਾਪ; i++) {
		ਜੇ (i < 10) {
			ਸੁਨੇਹਾ_3.distances[i] = 101;

		} ਨਹੀਂਤਾਂ {
			ਸੁਨੇਹਾ_3.distances[i] = 10001;
		}
	}

	// WHEN: we publish the message and set the parameter and then run the setpoint modification
	orb_advert_t obstacle_ਦੂਰੀ_pub = orb_advertise(ORB_ID(obstacle_ਦੂਰੀ), &ਸੁਨੇਹਾ_3);
	orb_advert_t vehicle_attitude_pub = orb_advertise(ORB_ID(vehicle_attitude), &attitude);
	orb_publish(ORB_ID(obstacle_ਦੂਰੀ), obstacle_ਦੂਰੀ_pub, &ਸੁਨੇਹਾ_3);
	orb_publish(ORB_ID(vehicle_attitude), vehicle_attitude_pub, &attitude);
	Vector2f modified_setpoint1 = original_setpoint1;
	Vector2f modified_setpoint2 = original_setpoint2;
	cp.modifySetpoint(modified_setpoint1, curr_vel);
	cp.modifySetpoint(modified_setpoint2, curr_vel);
	orb_unadvertise(obstacle_ਦੂਰੀ_pub);
	orb_unadvertise(vehicle_attitude_pub);

	// THEN: the internal map should know the obstacle
	// case 1: the acceleration setpoint should be negative as its pushing you away from the obstacle, and sideways acceleration should be low
	// case 2: the acceleration setpoint should be lower
	EXPECT_FLOAT_EQ(cp.getObstacleMap().min_ਦੂਰੀ, 100);
	EXPECT_FLOAT_EQ(cp.getObstacleMap().max_ਦੂਰੀ, 10000);
	EXPECT_GT(0.f, modified_setpoint1(0)) << modified_setpoint1(0);
	EXPECT_EQ(0.f, fabsf(modified_setpoint1(1))) << modified_setpoint1(1);
	EXPECT_GT(0.f, modified_setpoint2(0))  << original_setpoint2(0);
	EXPECT_EQ(0.f, fabsf(modified_setpoint2(1))) << modified_setpoint2(1);
}

TEST_F(CollisionPreventionTest, test_Behavior_On_With_ਦੂਰੀ_ਸੁਨੇਹਾ)
{
	// GIVEN: a simple setup condition
	TestCollisionPrevention cp;
	Vector2f original_setpoint1(10, 0);
	Vector2f original_setpoint2(-10, 0);
	Vector2f curr_vel(2, 0);
	vehicle_attitude_s attitude;
	attitude.timestamp = hrt_absolute_ਸਮਾਂ();
	attitude.q[0] = 1.0f;
	attitude.q[1] = 0.0f;
	attitude.q[2] = 0.0f;
	attitude.q[3] = 0.0f;

	// AND: a parameter handle
	param_t param = param_handle(px4::params::CP_DIST);
	ਦਸ਼ਮਲਵ value = 10; // try to keep 10m distance
	param_ਸੈੱਟ(param, &value);
	cp.params_ਬਦਲਿਆ();

	// AND: an obstacle message
	ਦੂਰੀ_sensor_s ਸੁਨੇਹਾ_3;
	ਸੁਨੇਹਾ_3.timestamp = hrt_absolute_ਸਮਾਂ();
	ਸੁਨੇਹਾ_3.min_ਦੂਰੀ = 1.f;
	ਸੁਨੇਹਾ_3.max_ਦੂਰੀ = 100.f;
	ਸੁਨੇਹਾ_3.current_ਦੂਰੀ = 1.1f;

	ਸੁਨੇਹਾ_3.variance = 0.1f;
	ਸੁਨੇਹਾ_3.signal_quality = 100;
	ਸੁਨੇਹਾ_3.type = ਦੂਰੀ_sensor_s::MAV_ਦੂਰੀ_SENSOR_LASER;
	ਸੁਨੇਹਾ_3.orientation = ਦੂਰੀ_sensor_s::ROTATION_FORWARD_FACING;
	ਸੁਨੇਹਾ_3.h_fov = math::radians(50.f);
	ਸੁਨੇਹਾ_3.v_fov = math::radians(30.f);

	// WHEN: we publish the message and set the parameter and then run the setpoint modification
	orb_advert_t ਦੂਰੀ_sensor_pub = orb_advertise(ORB_ID(ਦੂਰੀ_sensor), &ਸੁਨੇਹਾ_3);
	orb_advert_t vehicle_attitude_pub = orb_advertise(ORB_ID(vehicle_attitude), &attitude);
	orb_publish(ORB_ID(ਦੂਰੀ_sensor), ਦੂਰੀ_sensor_pub, &ਸੁਨੇਹਾ_3);
	orb_publish(ORB_ID(vehicle_attitude), vehicle_attitude_pub, &attitude);

	//WHEN:  We run the setpoint modification
	Vector2f modified_setpoint1 = original_setpoint1;
	Vector2f modified_setpoint2 = original_setpoint2;
	cp.modifySetpoint(modified_setpoint1, curr_vel);
	cp.modifySetpoint(modified_setpoint2, curr_vel);
	orb_unadvertise(ਦੂਰੀ_sensor_pub);
	orb_unadvertise(vehicle_attitude_pub);

	// THEN: the internal map should know the obstacle
	// case 1: the acceleration setpoint should be negative as its pushing you away from the obstacle, and sideways acceleration should be low
	// case 2: the acceleration setpoint should be lower
	EXPECT_FLOAT_EQ(cp.getObstacleMap().min_ਦੂਰੀ, 100);
	EXPECT_FLOAT_EQ(cp.getObstacleMap().max_ਦੂਰੀ, 10000);

	EXPECT_FLOAT_EQ(cp.getObstacleMap().min_ਦੂਰੀ, 100);
	EXPECT_FLOAT_EQ(cp.getObstacleMap().max_ਦੂਰੀ, 10000);

	EXPECT_GT(0.f, modified_setpoint1(0)) << modified_setpoint1(0);
	EXPECT_EQ(0.f, fabsf(modified_setpoint1(1))) << modified_setpoint1(1);
	EXPECT_GT(0.f, modified_setpoint2(0))  << original_setpoint2(0);
	EXPECT_EQ(0.f, fabsf(modified_setpoint2(1))) << modified_setpoint2(1);
}

TEST_F(CollisionPreventionTest, test_Purge_Old_ਅੰਕੜੇ)
{
	// GIVEN: a simple setup condition
	TestTimingCollisionPrevention cp;
	hrt_abstime ਸ਼ੁਰੂ_ਸਮਾਂ = hrt_absolute_ਸਮਾਂ();
	mocked_ਸਮਾਂ = ਸ਼ੁਰੂ_ਸਮਾਂ;
	Vector2f original_setpoint(10, 0);
	Vector2f curr_vel(2, 0);
	vehicle_attitude_s attitude;
	attitude.timestamp = ਸ਼ੁਰੂ_ਸਮਾਂ;
	attitude.q[0] = 1.0f;
	attitude.q[1] = 0.0f;
	attitude.q[2] = 0.0f;
	attitude.q[3] = 0.0f;

	// AND: a parameter handle
	param_t param = param_handle(px4::params::CP_DIST);
	ਦਸ਼ਮਲਵ value = 1; // try to keep 10m distance
	param_ਸੈੱਟ(param, &value);
	cp.params_ਬਦਲਿਆ();

	// AND: an obstacle message
	obstacle_ਦੂਰੀ_s ਸੁਨੇਹਾ_3, ਸੁਨੇਹਾ_ਗੁਆਚਾ_ਅੰਕੜੇ;
	memset(&ਸੁਨੇਹਾ_3, 0xDEAD, ਆਕਾਰ(ਸੁਨੇਹਾ_3));
	ਸੁਨੇਹਾ_3.ਚੌਖਟਾ = ਸੁਨੇਹਾ_3.MAV_ਚੌਖਟਾ_GLOBAL; //north aligned
	ਸੁਨੇਹਾ_3.min_ਦੂਰੀ = 100;
	ਸੁਨੇਹਾ_3.max_ਦੂਰੀ = 10000;
	ਸੁਨੇਹਾ_3.angle_ਵਿੱਥ = 0;
	ਸੁਨੇਹਾ_3.timestamp = ਸ਼ੁਰੂ_ਸਮਾਂ;
	ਪੂਰਨ distances_array_ਮਾਪ = ਆਕਾਰ(ਸੁਨੇਹਾ_3.distances) / ਆਕਾਰ(ਸੁਨੇਹਾ_3.distances[0]);
	ਸੁਨੇਹਾ_3.increment = 360 / distances_array_ਮਾਪ;
	ਸੁਨੇਹਾ_ਗੁਆਚਾ_ਅੰਕੜੇ = ਸੁਨੇਹਾ_3;

	ਲਈ (ਪੂਰਨ i = 0; i < distances_array_ਮਾਪ; i++) {
		ਜੇ (i < 10) {
			ਸੁਨੇਹਾ_3.distances[i] = 10001;
			ਸੁਨੇਹਾ_ਗੁਆਚਾ_ਅੰਕੜੇ.distances[i] = UINT16_MAX;

		} ਨਹੀਂਤਾਂ ਜੇ (i > 15 && i < 18) {
			ਸੁਨੇਹਾ_3.distances[i] = 10001;
			ਸੁਨੇਹਾ_ਗੁਆਚਾ_ਅੰਕੜੇ.distances[i] = 10001;

		} ਨਹੀਂਤਾਂ {
			ਸੁਨੇਹਾ_3.distances[i] = UINT16_MAX;
			ਸੁਨੇਹਾ_ਗੁਆਚਾ_ਅੰਕੜੇ.distances[i] = UINT16_MAX;
		}
	}

	// WHEN: we publish the message and set the parameter and then run the setpoint modification
	orb_advert_t obstacle_ਦੂਰੀ_pub = orb_advertise(ORB_ID(obstacle_ਦੂਰੀ), &ਸੁਨੇਹਾ_3);
	orb_advert_t vehicle_attitude_pub = orb_advertise(ORB_ID(vehicle_attitude), &attitude);
	orb_publish(ORB_ID(obstacle_ਦੂਰੀ), obstacle_ਦੂਰੀ_pub, &ਸੁਨੇਹਾ_3);
	orb_publish(ORB_ID(vehicle_attitude), vehicle_attitude_pub, &attitude);

	ਲਈ (ਪੂਰਨ i = 0; i < 10; i++) {
		Vector2f modified_setpoint = original_setpoint;
		cp.modifySetpoint(modified_setpoint, curr_vel);

		mocked_ਸਮਾਂ = mocked_ਸਮਾਂ + 100000; //advance time by 0.1 seconds
		ਸੁਨੇਹਾ_ਗੁਆਚਾ_ਅੰਕੜੇ.timestamp = mocked_ਸਮਾਂ;
		orb_publish(ORB_ID(obstacle_ਦੂਰੀ), obstacle_ਦੂਰੀ_pub, &ਸੁਨੇਹਾ_ਗੁਆਚਾ_ਅੰਕੜੇ);

		//at iteration 8 change the CP_GO_NO_DATA to True
		ਜੇ (i == 8) {
			param_t param_allow = param_handle(px4::params::CP_GO_NO_ਅੰਕੜੇ);
			ਦਸ਼ਮਲਵ ਮੁੱਲ_allow = 1;
			param_ਸੈੱਟ(param_allow, &ਮੁੱਲ_allow);
			cp.params_ਬਦਲਿਆ();
		}

		ਜੇ (i < 6) {
			// THEN: If the data is new enough, the velocity setpoint should stay the same as the input
			// Note: direction will change slightly due to guidance
			EXPECT_FLOAT_EQ(original_setpoint.norm(), modified_setpoint.norm());

		} ਨਹੀਂਤਾਂ {
			// THEN: If the data is expired, the velocity setpoint should be cut down to zero because there is no data
			//(even if CP_GO_NO_DATA is set to true, because we once had data in those bins and now lost the sensor)
			EXPECT_FLOAT_EQ(0.f, modified_setpoint.norm()) << modified_setpoint(0) << "," << modified_setpoint(1);
		}
	}

	orb_unadvertise(obstacle_ਦੂਰੀ_pub);
	orb_unadvertise(vehicle_attitude_pub);
}

TEST_F(CollisionPreventionTest, test_No_Range_ਅੰਕੜੇ)
{
	// GIVEN: a simple setup condition
	TestTimingCollisionPrevention cp;
	hrt_abstime ਸ਼ੁਰੂ_ਸਮਾਂ = hrt_absolute_ਸਮਾਂ();
	mocked_ਸਮਾਂ = ਸ਼ੁਰੂ_ਸਮਾਂ;
	Vector2f original_setpoint(10, 0);
	Vector2f curr_vel(2, 0);
	vehicle_attitude_s attitude;
	attitude.timestamp = ਸ਼ੁਰੂ_ਸਮਾਂ;
	attitude.q[0] = 1.0f;
	attitude.q[1] = 0.0f;
	attitude.q[2] = 0.0f;
	attitude.q[3] = 0.0f;

	// AND: a parameter handle
	param_t param = param_handle(px4::params::CP_DIST);
	ਦਸ਼ਮਲਵ value = 10; // try to keep 10m distance
	param_ਸੈੱਟ(param, &value);
	cp.params_ਬਦਲਿਆ();

	// AND: an obstacle message without any obstacle
	obstacle_ਦੂਰੀ_s ਸੁਨੇਹਾ_3;
	memset(&ਸੁਨੇਹਾ_3, 0xDEAD, ਆਕਾਰ(ਸੁਨੇਹਾ_3));
	ਸੁਨੇਹਾ_3.ਚੌਖਟਾ = ਸੁਨੇਹਾ_3.MAV_ਚੌਖਟਾ_GLOBAL; //north aligned
	ਸੁਨੇਹਾ_3.min_ਦੂਰੀ = 100;
	ਸੁਨੇਹਾ_3.max_ਦੂਰੀ = 10000;
	ਸੁਨੇਹਾ_3.angle_ਵਿੱਥ = 0;
	ਸੁਨੇਹਾ_3.timestamp = ਸ਼ੁਰੂ_ਸਮਾਂ;
	ਪੂਰਨ distances_array_ਮਾਪ = ਆਕਾਰ(ਸੁਨੇਹਾ_3.distances) / ਆਕਾਰ(ਸੁਨੇਹਾ_3.distances[0]);
	ਸੁਨੇਹਾ_3.increment = 360 / distances_array_ਮਾਪ;

	ਲਈ (ਪੂਰਨ i = 0; i < distances_array_ਮਾਪ; i++) {
		ਸੁਨੇਹਾ_3.distances[i] = 9000;
	}


	// WHEN: we publish the message and set the parameter and then run the setpoint modification
	orb_advert_t obstacle_ਦੂਰੀ_pub = orb_advertise(ORB_ID(obstacle_ਦੂਰੀ), &ਸੁਨੇਹਾ_3);
	orb_advert_t vehicle_attitude_pub = orb_advertise(ORB_ID(vehicle_attitude), &attitude);
	orb_publish(ORB_ID(obstacle_ਦੂਰੀ), obstacle_ਦੂਰੀ_pub, &ਸੁਨੇਹਾ_3);
	orb_publish(ORB_ID(vehicle_attitude), vehicle_attitude_pub, &attitude);

	ਲਈ (ਪੂਰਨ i = 0; i < 10; i++) {
		Vector2f modified_setpoint = original_setpoint;
		cp.modifySetpoint(modified_setpoint, curr_vel);

		//advance time by 0.1 seconds but no new message comes in
		mocked_ਸਮਾਂ = mocked_ਸਮਾਂ + 100000;

		ਜੇ (i < 5) {
			// THEN: If the data is new enough, the velocity setpoint should stay the same as the input
			// Note: direction will change slightly due to guidance
			EXPECT_FLOAT_EQ(original_setpoint.norm(), modified_setpoint.norm());

		} ਨਹੀਂਤਾਂ {
			// THEN: If the data is expired, the velocity setpoint should be cut down to zero because there is no data
			EXPECT_FLOAT_EQ(0.f, modified_setpoint.norm()) << modified_setpoint(0) << "," << modified_setpoint(1);
		}
	}

	orb_unadvertise(obstacle_ਦੂਰੀ_pub);
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
	ਦਸ਼ਮਲਵ value = 2; // try to keep 2m distance
	param_ਸੈੱਟ(param, &value);
	cp.params_ਬਦਲਿਆ();

	// AND: an obstacle message
	obstacle_ਦੂਰੀ_s ਸੁਨੇਹਾ_3;
	memset(&ਸੁਨੇਹਾ_3, 0xDEAD, ਆਕਾਰ(ਸੁਨੇਹਾ_3));
	ਸੁਨੇਹਾ_3.min_ਦੂਰੀ = 100;
	ਸੁਨੇਹਾ_3.max_ਦੂਰੀ = 2000;
	ਸੁਨੇਹਾ_3.timestamp = hrt_absolute_ਸਮਾਂ();
	ਸੁਨੇਹਾ_3.ਚੌਖਟਾ = ਸੁਨੇਹਾ_3.MAV_ਚੌਖਟਾ_GLOBAL; //north aligned
	ਪੂਰਨ distances_array_ਮਾਪ = ਆਕਾਰ(ਸੁਨੇਹਾ_3.distances) / ਆਕਾਰ(ਸੁਨੇਹਾ_3.distances[0]);
	ਸੁਨੇਹਾ_3.increment = 360 / distances_array_ਮਾਪ;

	ਲਈ (ਪੂਰਨ i = 0; i < distances_array_ਮਾਪ; i++) {
		ਸੁਨੇਹਾ_3.distances[i] = 700;
	}

	// WHEN: we publish the message and set the parameter and then run the setpoint modification
	orb_advert_t obstacle_ਦੂਰੀ_pub = orb_advertise(ORB_ID(obstacle_ਦੂਰੀ), &ਸੁਨੇਹਾ_3);
	orb_publish(ORB_ID(obstacle_ਦੂਰੀ), obstacle_ਦੂਰੀ_pub, &ਸੁਨੇਹਾ_3);
	Vector2f modified_setpoint = original_setpoint;
	cp.modifySetpoint(modified_setpoint, curr_vel);
	orb_unadvertise(obstacle_ਦੂਰੀ_pub);

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
	ਦਸ਼ਮਲਵ value = 5; // try to keep 5m distance
	param_ਸੈੱਟ(param, &value);
	cp.params_ਬਦਲਿਆ();

	// AND: an obstacle message
	obstacle_ਦੂਰੀ_s ਸੁਨੇਹਾ_3;
	memset(&ਸੁਨੇਹਾ_3, 0xDEAD, ਆਕਾਰ(ਸੁਨੇਹਾ_3));
	ਸੁਨੇਹਾ_3.ਚੌਖਟਾ = ਸੁਨੇਹਾ_3.MAV_ਚੌਖਟਾ_GLOBAL; //north aligned
	ਸੁਨੇਹਾ_3.min_ਦੂਰੀ = 100;
	ਸੁਨੇਹਾ_3.max_ਦੂਰੀ = 2000;
	ਪੂਰਨ distances_array_ਮਾਪ = ਆਕਾਰ(ਸੁਨੇਹਾ_3.distances) / ਆਕਾਰ(ਸੁਨੇਹਾ_3.distances[0]);
	ਸੁਨੇਹਾ_3.increment = 360.f / distances_array_ਮਾਪ;

	//fov from 45deg to 225deg
	ਲਈ (ਪੂਰਨ i = 0; i < distances_array_ਮਾਪ; i++) {
		ਦਸ਼ਮਲਵ angle = i * ਸੁਨੇਹਾ_3.increment;

		ਜੇ (angle > 45.f && angle < 225.f) {
			ਸੁਨੇਹਾ_3.distances[i] = 700;

		} ਨਹੀਂਤਾਂ {
			ਸੁਨੇਹਾ_3.distances[i] = UINT16_MAX;
		}
	}

	// WHEN: we publish the message and modify the setpoint for different demanded setpoints
	orb_advert_t obstacle_ਦੂਰੀ_pub = orb_advertise(ORB_ID(obstacle_ਦੂਰੀ), &ਸੁਨੇਹਾ_3);

	ਲਈ (ਪੂਰਨ i = 0; i < distances_array_ਮਾਪ; i++) {
		ਦਸ਼ਮਲਵ angle_deg = (ਦਸ਼ਮਲਵ)i * ਸੁਨੇਹਾ_3.increment;
		ਦਸ਼ਮਲਵ angle_rad = math::radians(angle_deg);
		Vector2f original_setpoint = {10.f * cosf(angle_rad), 10.f * sinf(angle_rad)};
		Vector2f modified_setpoint = original_setpoint;
		ਸੁਨੇਹਾ_3.timestamp = hrt_absolute_ਸਮਾਂ();
		orb_publish(ORB_ID(obstacle_ਦੂਰੀ), obstacle_ਦੂਰੀ_pub, &ਸੁਨੇਹਾ_3);
		cp.modifySetpoint(modified_setpoint, curr_vel);

		//THEN: if the resulting setpoint demands velocities bigger zero, it must lie inside the FOV
		ਦਸ਼ਮਲਵ setpoint_ਲੰਮਾਈ = modified_setpoint.norm();

		ਜੇ (setpoint_ਲੰਮਾਈ > 0.f) {
			Vector2f setpoint_dir = modified_setpoint / setpoint_ਲੰਮਾਈ;
			ਦਸ਼ਮਲਵ sp_angle_body_ਚੌਖਟਾ = atan2(setpoint_dir(1), setpoint_dir(0));
			ਦਸ਼ਮਲਵ sp_angle_deg = math::degrees(wrap_2pi(sp_angle_body_ਚੌਖਟਾ));
			EXPECT_GE(sp_angle_deg, 45.f);
			EXPECT_LE(sp_angle_deg, 225.f);
		}
	}

	orb_unadvertise(obstacle_ਦੂਰੀ_pub);
}

TEST_F(CollisionPreventionTest, go_No_ਅੰਕੜੇ)
{
	// GIVEN: a simple setup condition with the initial state (no distance data)
	TestCollisionPrevention cp;
	Vector2f curr_vel(2, 0);

	// AND: an obstacle message
	obstacle_ਦੂਰੀ_s ਸੁਨੇਹਾ_3;
	memset(&ਸੁਨੇਹਾ_3, 0xDEAD, ਆਕਾਰ(ਸੁਨੇਹਾ_3));
	ਸੁਨੇਹਾ_3.ਚੌਖਟਾ = ਸੁਨੇਹਾ_3.MAV_ਚੌਖਟਾ_GLOBAL; //north aligned
	ਸੁਨੇਹਾ_3.min_ਦੂਰੀ = 100;
	ਸੁਨੇਹਾ_3.max_ਦੂਰੀ = 2000;
	ਪੂਰਨ distances_array_ਮਾਪ = ਆਕਾਰ(ਸੁਨੇਹਾ_3.distances) / ਆਕਾਰ(ਸੁਨੇਹਾ_3.distances[0]);
	ਸੁਨੇਹਾ_3.increment = 360.f / distances_array_ਮਾਪ;

	//fov from 0deg to 20deg
	ਲਈ (ਪੂਰਨ i = 0; i < distances_array_ਮਾਪ; i++) {
		ਦਸ਼ਮਲਵ angle = i * ਸੁਨੇਹਾ_3.increment;

		ਜੇ (angle > 0.f && angle < 40.f) {
			ਸੁਨੇਹਾ_3.distances[i] = 1000;

		} ਨਹੀਂਤਾਂ {
			ਸੁਨੇਹਾ_3.distances[i] = UINT16_MAX;
		}
	}

	// AND: a parameter handle
	param_t param = param_handle(px4::params::CP_DIST);
	ਦਸ਼ਮਲਵ value = 2; // try to keep 5m distance
	param_ਸੈੱਟ(param, &value);
	cp.params_ਬਦਲਿਆ();

	// AND: a setpoint outside the field of view
	Vector2f original_setpoint = {-5, 0};
	Vector2f modified_setpoint = original_setpoint;

	//THEN: the modified setpoint should be zero acceleration
	cp.modifySetpoint(modified_setpoint, curr_vel);
	EXPECT_FLOAT_EQ(modified_setpoint.norm(), 0.f);

	//WHEN: we change the parameter CP_GO_NO_DATA to allow flying ouside the FOV
	param_t param_allow = param_handle(px4::params::CP_GO_NO_ਅੰਕੜੇ);
	ਦਸ਼ਮਲਵ ਮੁੱਲ_allow = 1;
	param_ਸੈੱਟ(param_allow, &ਮੁੱਲ_allow);
	cp.params_ਬਦਲਿਆ();

	//THEN: When all bins contain UINT_16MAX the setpoint should be zero even if CP_GO_NO_DATA=1
	modified_setpoint = original_setpoint;
	cp.modifySetpoint(modified_setpoint, curr_vel);
	EXPECT_FLOAT_EQ(modified_setpoint.norm(), 0.f);

	//THEN: As soon as the range data contains any valid number, flying outside the FOV is allowed
	ਸੁਨੇਹਾ_3.timestamp = hrt_absolute_ਸਮਾਂ();
	orb_advert_t obstacle_ਦੂਰੀ_pub = orb_advertise(ORB_ID(obstacle_ਦੂਰੀ), &ਸੁਨੇਹਾ_3);
	orb_publish(ORB_ID(obstacle_ਦੂਰੀ), obstacle_ਦੂਰੀ_pub, &ਸੁਨੇਹਾ_3);

	modified_setpoint = original_setpoint;
	cp.modifySetpoint(modified_setpoint, curr_vel);
	EXPECT_FLOAT_EQ(modified_setpoint.norm(), original_setpoint.norm());
	orb_unadvertise(obstacle_ਦੂਰੀ_pub);
}

TEST_F(CollisionPreventionTest, jerk_ਹੱਦਬੰਦੀ)
{
	// GIVEN: a simple setup condition
	TestCollisionPrevention cp;
	Vector2f original_setpoint(10, 0);
	Vector2f curr_vel(2, 0);

	// AND: distance set to 5m
	param_t param = param_handle(px4::params::CP_DIST);
	ਦਸ਼ਮਲਵ value = 5; // try to keep 5m distance
	param_ਸੈੱਟ(param, &value);
	cp.params_ਬਦਲਿਆ();

	// AND: an obstacle message
	obstacle_ਦੂਰੀ_s ਸੁਨੇਹਾ_3;
	memset(&ਸੁਨੇਹਾ_3, 0xDEAD, ਆਕਾਰ(ਸੁਨੇਹਾ_3));
	ਸੁਨੇਹਾ_3.min_ਦੂਰੀ = 100;
	ਸੁਨੇਹਾ_3.max_ਦੂਰੀ = 2000;
	ਸੁਨੇਹਾ_3.timestamp = hrt_absolute_ਸਮਾਂ();
	ਸੁਨੇਹਾ_3.ਚੌਖਟਾ = ਸੁਨੇਹਾ_3.MAV_ਚੌਖਟਾ_GLOBAL; //north aligned
	ਪੂਰਨ distances_array_ਮਾਪ = ਆਕਾਰ(ਸੁਨੇਹਾ_3.distances) / ਆਕਾਰ(ਸੁਨੇਹਾ_3.distances[0]);
	ਸੁਨੇਹਾ_3.increment = 360 / distances_array_ਮਾਪ;

	ਲਈ (ਪੂਰਨ i = 0; i < distances_array_ਮਾਪ; i++) {
		ਸੁਨੇਹਾ_3.distances[i] = 700;
	}

	// AND: we publish the message and set the parameter and then run the setpoint modification
	orb_advert_t obstacle_ਦੂਰੀ_pub = orb_advertise(ORB_ID(obstacle_ਦੂਰੀ), &ਸੁਨੇਹਾ_3);
	orb_publish(ORB_ID(obstacle_ਦੂਰੀ), obstacle_ਦੂਰੀ_pub, &ਸੁਨੇਹਾ_3);
	Vector2f modified_setpoint_default_jerk = original_setpoint;
	cp.modifySetpoint(modified_setpoint_default_jerk, curr_vel);
	orb_unadvertise(obstacle_ਦੂਰੀ_pub);

	// AND: we now set max jerk to 0.1
	param = param_handle(px4::params::MPC_JERK_MAX);
	value = 0.1; // 0.1 maximum jerk
	param_ਸੈੱਟ(param, &value);
	cp.params_ਬਦਲਿਆ();

	// WHEN: we run the setpoint modification again
	Vector2f modified_setpoint_limited_jerk = original_setpoint;
	cp.modifySetpoint(modified_setpoint_limited_jerk, curr_vel);

	// THEN: the new setpoint should be much higher than the one with default jerk, as the rate of change in acceleration is more limmited
	EXPECT_GT(modified_setpoint_limited_jerk.norm(), modified_setpoint_default_jerk.norm());

}
TEST_F(CollisionPreventionTest, ਜੋੜੋ_ਬਾਹਰਲਾ_ਦਾ_Range_ਦੂਰੀ_Sensor_ਅੰਕੜੇ)
{
	// GIVEN: a vehicle attitude and a distance sensor message
	TestCollisionPrevention cp;
	Quaternion<ਦਸ਼ਮਲਵ> vehicle_attitude(1, 0, 0, 0); //unit transform
	ਦੂਰੀ_sensor_s ਦੂਰੀ_sensor {};
	ਦੂਰੀ_sensor.min_ਦੂਰੀ = 0.2f;
	ਦੂਰੀ_sensor.max_ਦੂਰੀ = 20.f;
	ਦੂਰੀ_sensor.orientation = ਦੂਰੀ_sensor_s::ROTATION_FORWARD_FACING;
	// Distance is out of Range
	ਦੂਰੀ_sensor.current_ਦੂਰੀ = -1.f;
	ਦੂਰੀ_sensor.signal_quality = 0;

	uint32_t distances_array_ਮਾਪ = ਆਕਾਰ(cp.getObstacleMap().distances) / ਆਕਾਰ(cp.getObstacleMap().distances[0]);

	cp.test_ਜੋੜੋ_ਦੂਰੀ_Sensor_ਅੰਕੜੇ(ਦੂਰੀ_sensor, vehicle_attitude);

	//THEN: the correct bins in the map should be filled
	ਲਈ (uint32_t i = 0; i < distances_array_ਮਾਪ; i++) {
		ਜੇ (i == 0) {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], ਦੂਰੀ_sensor.max_ਦੂਰੀ * 100.f);

		} ਨਹੀਂਤਾਂ {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], UINT16_MAX);
		}
	}
}

TEST_F(CollisionPreventionTest, ਜੋੜੋ_ਦੂਰੀ_Sensor_ਅੰਕੜੇ_Narrow)
{
	// GIVEN: a vehicle attitude and a distance sensor message
	TestCollisionPrevention cp;
	Quaternion<ਦਸ਼ਮਲਵ> vehicle_attitude(1, 0, 0, 0); //unit transform
	ਦੂਰੀ_sensor_s ਦੂਰੀ_sensor {};
	ਦੂਰੀ_sensor.min_ਦੂਰੀ = 0.2f;
	ਦੂਰੀ_sensor.max_ਦੂਰੀ = 20.f;
	ਦੂਰੀ_sensor.current_ਦੂਰੀ = 5.f;
	ਦੂਰੀ_sensor.orientation = ਦੂਰੀ_sensor_s::ROTATION_FORWARD_FACING;
	ਦੂਰੀ_sensor.h_fov = math::radians(0.1 * bin_ਮਾਪ);

	uint32_t distances_array_ਮਾਪ = ਆਕਾਰ(cp.getObstacleMap().distances) / ਆਕਾਰ(cp.getObstacleMap().distances[0]);

	// WHEN the sensor has a very narrow field of view
	cp.test_ਜੋੜੋ_ਦੂਰੀ_Sensor_ਅੰਕੜੇ(ਦੂਰੀ_sensor, vehicle_attitude);

	//THEN: the correct bins in the map should be filled
	ਲਈ (uint32_t i = 0; i < distances_array_ਮਾਪ; i++) {
		ਜੇ (i == 0) {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], 500) << i;

		} ਨਹੀਂਤਾਂ {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], UINT16_MAX) << i;
		}
	}
}
TEST_F(CollisionPreventionTest, ਜੋੜੋ_ਦੂਰੀ_Sensor_ਅੰਕੜੇ_Slightly_Larger)
{
	// GIVEN: a vehicle attitude and a distance sensor message
	TestCollisionPrevention cp;
	Quaternion<ਦਸ਼ਮਲਵ> vehicle_attitude(1, 0, 0, 0); //unit transform
	ਦੂਰੀ_sensor_s ਦੂਰੀ_sensor {};
	ਦੂਰੀ_sensor.min_ਦੂਰੀ = 0.2f;
	ਦੂਰੀ_sensor.max_ਦੂਰੀ = 20.f;
	ਦੂਰੀ_sensor.current_ਦੂਰੀ = 5.f;
	ਦੂਰੀ_sensor.orientation = ਦੂਰੀ_sensor_s::ROTATION_FORWARD_FACING;
	ਦੂਰੀ_sensor.h_fov = math::radians(1.1 * bin_ਮਾਪ);

	uint32_t distances_array_ਮਾਪ = ਆਕਾਰ(cp.getObstacleMap().distances) / ਆਕਾਰ(cp.getObstacleMap().distances[0]);

	// WHEN the sensor has a very narrow field of view
	cp.test_ਜੋੜੋ_ਦੂਰੀ_Sensor_ਅੰਕੜੇ(ਦੂਰੀ_sensor, vehicle_attitude);

	//THEN: the the bins corresponding to -5°, 0° and 5° should be filled
	ਲਈ (uint32_t i = 0; i < distances_array_ਮਾਪ; i++) {
		ਜੇ (i == 71 || i <= 1) {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], 500) << i;

		} ਨਹੀਂਤਾਂ {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], UINT16_MAX) << i;
		}
	}
}

TEST_F(CollisionPreventionTest, ਜੋੜੋ_ਦੂਰੀ_Sensor_ਅੰਕੜੇ_2)
{
	// GIVEN: a vehicle attitude and a distance sensor message
	TestCollisionPrevention cp;
	Quaternion<ਦਸ਼ਮਲਵ> vehicle_attitude(1, 0, 0, 0); //unit transform
	ਦੂਰੀ_sensor_s ਦੂਰੀ_sensor {};
	ਦੂਰੀ_sensor.min_ਦੂਰੀ = 0.2f;
	ਦੂਰੀ_sensor.max_ਦੂਰੀ = 20.f;
	ਦੂਰੀ_sensor.current_ਦੂਰੀ = 5.f;

	//THEN: at initialization the internal obstacle map should only contain UINT16_MAX
	uint32_t distances_array_ਮਾਪ = ਆਕਾਰ(cp.getObstacleMap().distances) / ਆਕਾਰ(cp.getObstacleMap().distances[0]);

	ਲਈ (uint32_t i = 0; i < distances_array_ਮਾਪ; i++) {
		EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], UINT16_MAX) << i;
	}

	//WHEN: we add distance sensor data to the right
	ਦੂਰੀ_sensor.orientation = ਦੂਰੀ_sensor_s::ROTATION_ਸੱਜਾ_FACING;
	ਦੂਰੀ_sensor.h_fov = math::radians(19.99f);
	cp.test_ਜੋੜੋ_ਦੂਰੀ_Sensor_ਅੰਕੜੇ(ਦੂਰੀ_sensor, vehicle_attitude);
	uint fov = round(ਦੂਰੀ_sensor.h_fov * M_RAD_ਮੰਜ਼ਿਲ_DEG_F / 2);
	uint ਸ਼ੁਰੂ = (90 - fov) / bin_ਮਾਪ;
	uint ਸਿਰਾ = (90 + fov) / bin_ਮਾਪ;

	//THEN: the correct bins in the map should be filled
	ਲਈ (uint32_t i = 0; i < distances_array_ਮਾਪ; i++) {
		ਜੇ (i >= ਸ਼ੁਰੂ && i <= ਸਿਰਾ) {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], 500) << i;

		} ਨਹੀਂਤਾਂ {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], UINT16_MAX) << i;
		}
	}

	//WHEN: we add additionally distance sensor data to the left
	ਦੂਰੀ_sensor.orientation = ਦੂਰੀ_sensor_s::ROTATION_ਖੱਬਾ_FACING;
	ਦੂਰੀ_sensor.h_fov = math::radians(50.f);
	ਦੂਰੀ_sensor.current_ਦੂਰੀ = 8.f;
	cp.test_ਜੋੜੋ_ਦੂਰੀ_Sensor_ਅੰਕੜੇ(ਦੂਰੀ_sensor, vehicle_attitude);
	fov = round(ਦੂਰੀ_sensor.h_fov * M_RAD_ਮੰਜ਼ਿਲ_DEG_F / 2);
	uint start2 = (270 - fov) / bin_ਮਾਪ;
	uint end2 = (270 + fov) / bin_ਮਾਪ;

	//THEN: the correct bins in the map should be filled
	ਲਈ (uint32_t i = 0; i < distances_array_ਮਾਪ; i++) {
		ਜੇ (i >= ਸ਼ੁਰੂ && i <= ਸਿਰਾ) {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], 500) << i;

		} ਨਹੀਂਤਾਂ ਜੇ (i >= start2 && i <= end2) {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], 800) << i;

		} ਨਹੀਂਤਾਂ {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], UINT16_MAX) << i;
		}
	}

	//WHEN: we add additionally distance sensor data to the front
	ਦੂਰੀ_sensor.orientation = ਦੂਰੀ_sensor_s::ROTATION_FORWARD_FACING;
	ਦੂਰੀ_sensor.h_fov = math::radians(10.1f);
	ਦੂਰੀ_sensor.current_ਦੂਰੀ = 3.f;
	cp.test_ਜੋੜੋ_ਦੂਰੀ_Sensor_ਅੰਕੜੇ(ਦੂਰੀ_sensor, vehicle_attitude);
	fov = round(ਦੂਰੀ_sensor.h_fov * M_RAD_ਮੰਜ਼ਿਲ_DEG_F / 2);
	uint start3 = (360 - fov) / bin_ਮਾਪ;
	uint end3 = (fov) / bin_ਮਾਪ;

	//THEN: the correct bins in the map should be filled
	ਲਈ (uint32_t i = 0; i < distances_array_ਮਾਪ; i++) {
		ਜੇ (i >= ਸ਼ੁਰੂ && i <= ਸਿਰਾ) {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], 500)  << i;

		} ਨਹੀਂਤਾਂ ਜੇ (i >= start2 && i <= end2) {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], 800) << i;

		} ਨਹੀਂਤਾਂ ਜੇ (i >= start3 || i <= end3) {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], 300) << i;

		} ਨਹੀਂਤਾਂ {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], UINT16_MAX) << i;
		}
	}


}

TEST_F(CollisionPreventionTest, ਜੋੜੋ_Obstacle_Sensor_ਅੰਕੜੇ_attitude)
{
	// GIVEN: a vehicle attitude and obstacle distance message
	TestCollisionPrevention cp;
	obstacle_ਦੂਰੀ_s obstacle_ਸੁਨੇਹਾ {};
	obstacle_ਸੁਨੇਹਾ.ਚੌਖਟਾ = obstacle_ਸੁਨੇਹਾ.MAV_ਚੌਖਟਾ_GLOBAL; //north aligned
	obstacle_ਸੁਨੇਹਾ.increment = 5.f;
	obstacle_ਸੁਨੇਹਾ.min_ਦੂਰੀ = 20;
	obstacle_ਸੁਨੇਹਾ.max_ਦੂਰੀ = 2000;
	obstacle_ਸੁਨੇਹਾ.angle_ਵਿੱਥ = 0.f;

	//obstacle at 10-30 deg world frame, distance 5 meters
	memset(&obstacle_ਸੁਨੇਹਾ.distances[0], UINT16_MAX, ਆਕਾਰ(obstacle_ਸੁਨੇਹਾ.distances));

	ਪੂਰਨ ਸ਼ੁਰੂ = 2;
	ਪੂਰਨ ਸਿਰਾ = 6;

	ਲਈ (ਪੂਰਨ i = ਸ਼ੁਰੂ; i <= ਸਿਰਾ ; i++) {
		obstacle_ਸੁਨੇਹਾ.distances[i] = 500;
	}



	//THEN: at initialization the internal obstacle map should only contain UINT16_MAX
	ਪੂਰਨ distances_array_ਮਾਪ = ਆਕਾਰ(cp.getObstacleMap().distances) / ਆਕਾਰ(cp.getObstacleMap().distances[0]);

	ਲਈ (ਪੂਰਨ i = 0; i < distances_array_ਮਾਪ; i++) {
		EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], UINT16_MAX);
	}

	//WHEN: we add distance sensor data while vehicle has zero yaw
	cp.test_ਜੋੜੋ_Obstacle_Sensor_ਅੰਕੜੇ(obstacle_ਸੁਨੇਹਾ, 0.f);

	//THEN: the correct bins in the map should be filled
	ਲਈ (ਪੂਰਨ i = 0; i < distances_array_ਮਾਪ; i++) {
		ਜੇ (i >= ਸ਼ੁਰੂ && i <= ਸਿਰਾ) {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], 500) << i;

		} ਨਹੀਂਤਾਂ {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], UINT16_MAX) << i;
		}

		//reset array to UINT16_MAX
		cp.getObstacleMap().distances[i] = UINT16_MAX;
	}


	//WHEN: we add obstacle distance sensor data while vehicle yaw 90deg to the right
	cp.test_ਜੋੜੋ_Obstacle_Sensor_ਅੰਕੜੇ(obstacle_ਸੁਨੇਹਾ, M_PI_2);

	//THEN: the correct bins in the map should be filled
	ਪੂਰਨ ਵਿੱਥ =  bin_ਗਿਣਤੀ - 90 / bin_ਮਾਪ;

	ਲਈ (ਪੂਰਨ i = 0; i < distances_array_ਮਾਪ; i++) {
		ਜੇ (i >= ਵਿੱਥ + ਸ਼ੁਰੂ && i <= ਵਿੱਥ + ਸਿਰਾ) {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], 500) << i;

		} ਨਹੀਂਤਾਂ {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], UINT16_MAX) << i;
		}

		//reset array to UINT16_MAX
		cp.getObstacleMap().distances[i] = UINT16_MAX;
	}

	//WHEN: we add obstacle distance sensor data while vehicle yaw 45deg to the left
	cp.test_ਜੋੜੋ_Obstacle_Sensor_ਅੰਕੜੇ(obstacle_ਸੁਨੇਹਾ, -M_PI_4);

	//THEN: the correct bins in the map should be filled
	ਵਿੱਥ =  45 / bin_ਮਾਪ;

	ਲਈ (ਪੂਰਨ i = 0; i < distances_array_ਮਾਪ; i++) {
		ਜੇ (i >= ਵਿੱਥ + ਸ਼ੁਰੂ && i <= ਵਿੱਥ + ਸਿਰਾ) {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], 500) << i;

		} ਨਹੀਂਤਾਂ {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], UINT16_MAX) << i;
		}

		//reset array to UINT16_MAX
		cp.getObstacleMap().distances[i] = UINT16_MAX;
	}

	//WHEN: we add obstacle distance sensor data while vehicle yaw 180deg
	cp.test_ਜੋੜੋ_Obstacle_Sensor_ਅੰਕੜੇ(obstacle_ਸੁਨੇਹਾ, M_PI);

	//THEN: the correct bins in the map should be filled
	ਵਿੱਥ =  180 / bin_ਮਾਪ;

	ਲਈ (ਪੂਰਨ i = 0; i < distances_array_ਮਾਪ; i++) {
		ਜੇ (i >= ਵਿੱਥ + ਸ਼ੁਰੂ && i <= ਵਿੱਥ + ਸਿਰਾ) {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], 500);

		} ਨਹੀਂਤਾਂ {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], UINT16_MAX);
		}

		//reset array to UINT16_MAX
		cp.getObstacleMap().distances[i] = UINT16_MAX;
	}
}

TEST_F(CollisionPreventionTest, ਜੋੜੋ_Obstacle_Sensor_ਅੰਕੜੇ_ਵਿੱਥ_bodyframe)
{
	// GIVEN: a vehicle attitude and obstacle distance message
	TestCollisionPrevention cp;
	obstacle_ਦੂਰੀ_s obstacle_ਸੁਨੇਹਾ {};
	obstacle_ਸੁਨੇਹਾ.ਚੌਖਟਾ = obstacle_ਸੁਨੇਹਾ.MAV_ਚੌਖਟਾ_BODY_FRD; // Body Frame
	obstacle_ਸੁਨੇਹਾ.increment = 6.f;
	obstacle_ਸੁਨੇਹਾ.min_ਦੂਰੀ = 20;
	obstacle_ਸੁਨੇਹਾ.max_ਦੂਰੀ = 2000;
	obstacle_ਸੁਨੇਹਾ.angle_ਵਿੱਥ = 0.f;

	//obstacle at 363°-39° deg world frame, distance 5 meters
	memset(&obstacle_ਸੁਨੇਹਾ.distances[0], UINT16_MAX, ਆਕਾਰ(obstacle_ਸੁਨੇਹਾ.distances));

	ਲਈ (ਪੂਰਨ i = 0; i <= 6 ; i++) { // 36° at 6° increment
		obstacle_ਸੁਨੇਹਾ.distances[i] = 500;
	}

	//WHEN: we add distance sensor data
	cp.test_ਜੋੜੋ_Obstacle_Sensor_ਅੰਕੜੇ(obstacle_ਸੁਨੇਹਾ, 0.f);

	//THEN: the the bins from 0 to 40 in map should be filled, which correspond to the angles from -2.5° to 42.5°
	ਪੂਰਨ distances_array_ਮਾਪ = ਆਕਾਰ(cp.getObstacleMap().distances) / ਆਕਾਰ(cp.getObstacleMap().distances[0]);

	ਲਈ (ਪੂਰਨ i = 0; i < distances_array_ਮਾਪ; i++) {
		ਜੇ (i >= 0 && i <= 8) {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], 500) << i;

		} ਨਹੀਂਤਾਂ {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], UINT16_MAX) << i;
		}

		//reset array to UINT16_MAX
		cp.getObstacleMap().distances[i] = UINT16_MAX;
	}

	//WHEN: we add the same obstacle distance sensor data with an angle offset of 30.5°
	obstacle_ਸੁਨੇਹਾ.angle_ਵਿੱਥ = 30.5f;
	// This then means our obstacle is between 27.5° and 69.5°
	cp.test_ਜੋੜੋ_Obstacle_Sensor_ਅੰਕੜੇ(obstacle_ਸੁਨੇਹਾ, 0.f);

	//THEN: the bins from 30° to 70° in map should be filled, which correspond to the angles from 27.5° to 72.5°
	ਲਈ (ਪੂਰਨ i = 0; i < distances_array_ਮਾਪ; i++) {
		ਜੇ (i >= 6 && i <= 14) {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], 500) << i;

		} ਨਹੀਂਤਾਂ {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], UINT16_MAX) << i;
		}

		//reset array to UINT16_MAX
		cp.getObstacleMap().distances[i] = UINT16_MAX;
	}

	//WHEN: we increase the offset to -30.5°
	obstacle_ਸੁਨੇਹਾ.angle_ਵਿੱਥ = -30.5f;
	// This then means our obstacle is between 326.5° and 8.5°
	cp.test_ਜੋੜੋ_Obstacle_Sensor_ਅੰਕੜੇ(obstacle_ਸੁਨੇਹਾ, 0.f);

	//THEN: the bins from 325° to 10° in map should be filled, which correspond to the angles from 322.5° to 12.5°

	ਲਈ (ਪੂਰਨ i = 0; i < distances_array_ਮਾਪ; i++) {
		ਜੇ (i >= 65 || i <= 2) {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], 500) << i;

		} ਨਹੀਂਤਾਂ {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], UINT16_MAX) << i;
		}

		//reset array to UINT16_MAX
		cp.getObstacleMap().distances[i] = UINT16_MAX;
	}
}

TEST_F(CollisionPreventionTest, ਜੋੜੋ_Obstacle_Sensor_ਅੰਕੜੇ_bodyframe)
{
	// GIVEN: a vehicle attitude and obstacle distance message
	TestCollisionPrevention cp;
	obstacle_ਦੂਰੀ_s obstacle_ਸੁਨੇਹਾ {};
	obstacle_ਸੁਨੇਹਾ.ਚੌਖਟਾ = obstacle_ਸੁਨੇਹਾ.MAV_ਚੌਖਟਾ_BODY_FRD; //north aligned
	obstacle_ਸੁਨੇਹਾ.increment = 5.f;
	obstacle_ਸੁਨੇਹਾ.min_ਦੂਰੀ = 20;
	obstacle_ਸੁਨੇਹਾ.max_ਦੂਰੀ = 2000;
	obstacle_ਸੁਨੇਹਾ.angle_ਵਿੱਥ = 0.f;

	//obstacle at 10-30 deg body frame, distance 5 meters
	memset(&obstacle_ਸੁਨੇਹਾ.distances[0], UINT16_MAX, ਆਕਾਰ(obstacle_ਸੁਨੇਹਾ.distances));
	ਪੂਰਨ ਸ਼ੁਰੂ = 2;
	ਪੂਰਨ ਸਿਰਾ = 6;

	ਲਈ (ਪੂਰਨ i = ਸ਼ੁਰੂ; i <= ਸਿਰਾ ; i++) {
		obstacle_ਸੁਨੇਹਾ.distances[i] = 500;
	}


	//THEN: at initialization the internal obstacle map should only contain UINT16_MAX
	ਪੂਰਨ distances_array_ਮਾਪ = ਆਕਾਰ(cp.getObstacleMap().distances) / ਆਕਾਰ(cp.getObstacleMap().distances[0]);

	ਲਈ (ਪੂਰਨ i = 0; i < distances_array_ਮਾਪ; i++) {
		EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], UINT16_MAX);
	}

	//WHEN: we add obstacle data while vehicle has zero yaw
	cp.test_ਜੋੜੋ_Obstacle_Sensor_ਅੰਕੜੇ(obstacle_ਸੁਨੇਹਾ, 0.f);

	//THEN: the correct bins in the map should be filled

	ਲਈ (ਪੂਰਨ i = 0; i < distances_array_ਮਾਪ; i++) {
		ਜੇ (i >= ਸ਼ੁਰੂ && i <= ਸਿਰਾ) {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], 500) << i;

		} ਨਹੀਂਤਾਂ {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], UINT16_MAX) << i;
		}

		//reset array to UINT16_MAX
		cp.getObstacleMap().distances[i] = UINT16_MAX;
	}

	//WHEN: we add obstacle data while vehicle yaw 90deg to the right
	cp.test_ਜੋੜੋ_Obstacle_Sensor_ਅੰਕੜੇ(obstacle_ਸੁਨੇਹਾ, M_PI_2);

	//THEN: the correct bins in the map should be filled
	ਲਈ (ਪੂਰਨ i = 0; i < distances_array_ਮਾਪ; i++) {
		ਜੇ (i >= ਸ਼ੁਰੂ && i <= ਸਿਰਾ) {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], 500) << i;

		} ਨਹੀਂਤਾਂ {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], UINT16_MAX) << i;
		}

		//reset array to UINT16_MAX
		cp.getObstacleMap().distances[i] = UINT16_MAX;
	}

	//WHEN: we add obstacle data while vehicle yaw 45deg to the left
	cp.test_ਜੋੜੋ_Obstacle_Sensor_ਅੰਕੜੇ(obstacle_ਸੁਨੇਹਾ, -M_PI_4);

	//THEN: the correct bins in the map should be filled
	ਲਈ (ਪੂਰਨ i = 0; i < distances_array_ਮਾਪ; i++) {
		ਜੇ (i >= ਸ਼ੁਰੂ && i <= ਸਿਰਾ) {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], 500) << i;

		} ਨਹੀਂਤਾਂ {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], UINT16_MAX) << i;
		}

		//reset array to UINT16_MAX
		cp.getObstacleMap().distances[i] = UINT16_MAX;
	}

	//WHEN: we add obstacle data while vehicle yaw 180deg
	cp.test_ਜੋੜੋ_Obstacle_Sensor_ਅੰਕੜੇ(obstacle_ਸੁਨੇਹਾ, M_PI);

	//THEN: the correct bins in the map should be filled
	ਲਈ (ਪੂਰਨ i = 0; i < distances_array_ਮਾਪ; i++) {
		ਜੇ (i >= ਸ਼ੁਰੂ && i <= ਸਿਰਾ) {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], 500) << i;

		} ਨਹੀਂਤਾਂ {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], UINT16_MAX) << i;
		}

		//reset array to UINT16_MAX
		cp.getObstacleMap().distances[i] = UINT16_MAX;
	}

}


TEST_F(CollisionPreventionTest, ਜੋੜੋ_Obstacle_Sensor_ਅੰਕੜੇ_resolution_ਵਿੱਥ)
{
	// GIVEN: a vehicle attitude and obstacle distance message
	TestCollisionPrevention cp;
	obstacle_ਦੂਰੀ_s obstacle_ਸੁਨੇਹਾ {};
	obstacle_ਸੁਨੇਹਾ.ਚੌਖਟਾ = obstacle_ਸੁਨੇਹਾ.MAV_ਚੌਖਟਾ_GLOBAL; //north aligned
	obstacle_ਸੁਨੇਹਾ.increment = 6.f;
	obstacle_ਸੁਨੇਹਾ.min_ਦੂਰੀ = 20;
	obstacle_ਸੁਨੇਹਾ.max_ਦੂਰੀ = 2000;
	obstacle_ਸੁਨੇਹਾ.angle_ਵਿੱਥ = 0.f;

	//obstacle at 363°-39° deg world frame, distance 5 meters
	memset(&obstacle_ਸੁਨੇਹਾ.distances[0], UINT16_MAX, ਆਕਾਰ(obstacle_ਸੁਨੇਹਾ.distances));

	ਲਈ (ਪੂਰਨ i = 0; i <= 6 ; i++) { // 36° at 6° increment
		obstacle_ਸੁਨੇਹਾ.distances[i] = 500;
	}

	//WHEN: we add distance sensor data
	cp.test_ਜੋੜੋ_Obstacle_Sensor_ਅੰਕੜੇ(obstacle_ਸੁਨੇਹਾ, 0.f);

	//THEN: the the bins from 0 to 40 in map should be filled, which correspond to the angles from -2.5° to 42.5°
	ਪੂਰਨ distances_array_ਮਾਪ = ਆਕਾਰ(cp.getObstacleMap().distances) / ਆਕਾਰ(cp.getObstacleMap().distances[0]);

	ਲਈ (ਪੂਰਨ i = 0; i < distances_array_ਮਾਪ; i++) {
		ਜੇ (i >= 0 && i <= 8) {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], 500) << i;

		} ਨਹੀਂਤਾਂ {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], UINT16_MAX) << i;
		}

		//reset array to UINT16_MAX
		cp.getObstacleMap().distances[i] = UINT16_MAX;
	}

	//WHEN: we add the same obstacle distance sensor data with an angle offset of 30.5°
	obstacle_ਸੁਨੇਹਾ.angle_ਵਿੱਥ = 30.5f;
	// This then means our obstacle is between 27.5° and 69.5°
	cp.test_ਜੋੜੋ_Obstacle_Sensor_ਅੰਕੜੇ(obstacle_ਸੁਨੇਹਾ, 0.f);

	//THEN: the bins from 30° to 70° in map should be filled, which correspond to the angles from 27.5° to 72.5°
	ਲਈ (ਪੂਰਨ i = 0; i < distances_array_ਮਾਪ; i++) {
		ਜੇ (i >= 6 && i <= 14) {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], 500) << i;

		} ਨਹੀਂਤਾਂ {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], UINT16_MAX) << i;
		}

		//reset array to UINT16_MAX
		cp.getObstacleMap().distances[i] = UINT16_MAX;
	}

	//WHEN: we increase the offset to -30.5°
	obstacle_ਸੁਨੇਹਾ.angle_ਵਿੱਥ = -30.5f;
	// This then means our obstacle is between 326.5° and 8.5°
	cp.test_ਜੋੜੋ_Obstacle_Sensor_ਅੰਕੜੇ(obstacle_ਸੁਨੇਹਾ, 0.f);

	//THEN: the bins from 325° to 10° in map should be filled, which correspond to the angles from 322.5° to 12.5°

	ਲਈ (ਪੂਰਨ i = 0; i < distances_array_ਮਾਪ; i++) {
		ਜੇ (i >= 65 || i <= 2) {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], 500) << i;

		} ਨਹੀਂਤਾਂ {
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
	obstacle_ਦੂਰੀ_s obstacle_ਸੁਨੇਹਾ {};
	obstacle_ਸੁਨੇਹਾ.ਚੌਖਟਾ = obstacle_ਸੁਨੇਹਾ.MAV_ਚੌਖਟਾ_GLOBAL; //north aligned
	obstacle_ਸੁਨੇਹਾ.increment = 5.f;
	obstacle_ਸੁਨੇਹਾ.min_ਦੂਰੀ = 20;
	obstacle_ਸੁਨੇਹਾ.max_ਦੂਰੀ = 2000;
	obstacle_ਸੁਨੇਹਾ.angle_ਵਿੱਥ = 0.f;

	ਅਚਲ ਦਸ਼ਮਲਵ vehicle_yaw = 0.f;

	//obstacle at 0-30 deg world frame, distance 5 meters
	memset(&obstacle_ਸੁਨੇਹਾ.distances[0], UINT16_MAX, ਆਕਾਰ(obstacle_ਸੁਨੇਹਾ.distances));

	ਲਈ (ਪੂਰਨ i = 0; i <= 6 ; i++) {
		obstacle_ਸੁਨੇਹਾ.distances[i] = 500;
	}

	obstacle_ਸੁਨੇਹਾ.distances[2] = 1000;

	//define setpoint
	Vector2f setpoint_dir(1, 0);
	ਦਸ਼ਮਲਵ sp_angle_body_ਚੌਖਟਾ = atan2f(setpoint_dir(1), setpoint_dir(0)) - vehicle_yaw;
	ਦਸ਼ਮਲਵ sp_angle_with_ਵਿੱਥ_deg = wrap(math::degrees(sp_angle_body_ਚੌਖਟਾ) - cp.getObstacleMap().angle_ਵਿੱਥ,
					      0.f, 360.f);
	ਪੂਰਨ sp_ਸੂਚਕਾਂਕ = floor(sp_angle_with_ਵਿੱਥ_deg / cp.getObstacleMap().increment);

	//set parameter
	param_t param = param_handle(px4::params::CP_DIST);
	ਦਸ਼ਮਲਵ value = 3; // try to keep 10m away from obstacles
	param_ਸੈੱਟ(param, &value);
	cp.params_ਬਦਲਿਆ();

	//WHEN: we add distance sensor data
	cp.test_ਜੋੜੋ_Obstacle_Sensor_ਅੰਕੜੇ(obstacle_ਸੁਨੇਹਾ, vehicle_yaw);
	cp.test_adaptSetpointDirection(setpoint_dir, sp_ਸੂਚਕਾਂਕ, vehicle_yaw);

	//THEN: the setpoint direction should be modified correctly
	EXPECT_EQ(sp_ਸੂਚਕਾਂਕ, 2);
	EXPECT_FLOAT_EQ(setpoint_dir(0), 0.98480773f);
	EXPECT_FLOAT_EQ(setpoint_dir(1), 0.17364818f);
}

TEST_F(CollisionPreventionTest, adaptSetpointDirection_flat_minimum)
{
	// GIVEN: a vehicle attitude and obstacle distance message
	TestCollisionPrevention cp;
	obstacle_ਦੂਰੀ_s obstacle_ਸੁਨੇਹਾ {};
	obstacle_ਸੁਨੇਹਾ.ਚੌਖਟਾ = obstacle_ਸੁਨੇਹਾ.MAV_ਚੌਖਟਾ_GLOBAL; //north aligned
	obstacle_ਸੁਨੇਹਾ.increment = 5.f;
	obstacle_ਸੁਨੇਹਾ.min_ਦੂਰੀ = 20;
	obstacle_ਸੁਨੇਹਾ.max_ਦੂਰੀ = 2000;
	obstacle_ਸੁਨੇਹਾ.angle_ਵਿੱਥ = 0.f;

	ਅਚਲ ਦਸ਼ਮਲਵ vehicle_yaw = 0.f;

	//obstacle at 0-30 deg world frame, distance 5 meters
	memset(&obstacle_ਸੁਨੇਹਾ.distances[0], UINT16_MAX, ਆਕਾਰ(obstacle_ਸੁਨੇਹਾ.distances));

	ਲਈ (ਪੂਰਨ i = 0; i < 7 ; i++) {
		obstacle_ਸੁਨੇਹਾ.distances[i] = 500;
	}

	obstacle_ਸੁਨੇਹਾ.distances[1] = 1000;
	obstacle_ਸੁਨੇਹਾ.distances[2] = 1000;
	obstacle_ਸੁਨੇਹਾ.distances[3] = 1000;

	//define setpoint
	Vector2f setpoint_dir(1, 0);
	ਦਸ਼ਮਲਵ sp_angle_body_ਚੌਖਟਾ = atan2f(setpoint_dir(1), setpoint_dir(0)) - vehicle_yaw;
	ਦਸ਼ਮਲਵ sp_angle_with_ਵਿੱਥ_deg = wrap(math::degrees(sp_angle_body_ਚੌਖਟਾ) - cp.getObstacleMap().angle_ਵਿੱਥ,
					      0.f, 360.f);
	ਪੂਰਨ sp_ਸੂਚਕਾਂਕ = floor(sp_angle_with_ਵਿੱਥ_deg / cp.getObstacleMap().increment);

	//set parameter
	param_t param = param_handle(px4::params::CP_DIST);
	ਦਸ਼ਮਲਵ value = 3; // try to keep 10m away from obstacles
	param_ਸੈੱਟ(param, &value);
	cp.params_ਬਦਲਿਆ();

	//WHEN: we add distance sensor data
	cp.test_ਜੋੜੋ_Obstacle_Sensor_ਅੰਕੜੇ(obstacle_ਸੁਨੇਹਾ, vehicle_yaw);
	cp.test_adaptSetpointDirection(setpoint_dir, sp_ਸੂਚਕਾਂਕ, vehicle_yaw);

	//THEN: the setpoint direction should be modified correctly
	EXPECT_EQ(sp_ਸੂਚਕਾਂਕ, 2);
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
	attitude.timestamp = hrt_absolute_ਸਮਾਂ();
	attitude.q[0] = 1.0f;
	attitude.q[1] = 0.0f;
	attitude.q[2] = 0.0f;
	attitude.q[3] = 0.0f;

	// AND: a parameter handle
	param_t param = param_handle(px4::params::CP_DIST);
	ਦਸ਼ਮਲਵ value = 10; // try to keep 10m distance
	param_ਸੈੱਟ(param, &value);
	cp.params_ਬਦਲਿਆ();

	// AND: an obstacle message for a short range and a long range sensor
	obstacle_ਦੂਰੀ_s short_range_ਸੁਨੇਹਾ, short_range_ਸੁਨੇਹਾ_no_obstacle, long_range_ਸੁਨੇਹਾ;
	memset(&short_range_ਸੁਨੇਹਾ, 0xDEAD, ਆਕਾਰ(short_range_ਸੁਨੇਹਾ));
	short_range_ਸੁਨੇਹਾ.ਚੌਖਟਾ = short_range_ਸੁਨੇਹਾ.MAV_ਚੌਖਟਾ_GLOBAL; //north aligned
	short_range_ਸੁਨੇਹਾ.angle_ਵਿੱਥ = 0;
	short_range_ਸੁਨੇਹਾ.timestamp = hrt_absolute_ਸਮਾਂ();
	ਪੂਰਨ distances_array_ਮਾਪ = ਆਕਾਰ(short_range_ਸੁਨੇਹਾ.distances) / ਆਕਾਰ(short_range_ਸੁਨੇਹਾ.distances[0]);
	short_range_ਸੁਨੇਹਾ.increment = 360 / distances_array_ਮਾਪ;
	long_range_ਸੁਨੇਹਾ = short_range_ਸੁਨੇਹਾ;
	long_range_ਸੁਨੇਹਾ.min_ਦੂਰੀ = 100;
	long_range_ਸੁਨੇਹਾ.max_ਦੂਰੀ = 1000;
	short_range_ਸੁਨੇਹਾ.min_ਦੂਰੀ = 20;
	short_range_ਸੁਨੇਹਾ.max_ਦੂਰੀ = 200;
	short_range_ਸੁਨੇਹਾ_no_obstacle = short_range_ਸੁਨੇਹਾ;


	ਲਈ (ਪੂਰਨ i = 0; i < distances_array_ਮਾਪ; i++) {
		ਜੇ (i < 10) {
			short_range_ਸੁਨੇਹਾ_no_obstacle.distances[i] = 201;
			short_range_ਸੁਨੇਹਾ.distances[i] = 150;
			long_range_ਸੁਨੇਹਾ.distances[i] = 500;

		} ਨਹੀਂਤਾਂ {
			short_range_ਸੁਨੇਹਾ_no_obstacle.distances[i] = UINT16_MAX;
			short_range_ਸੁਨੇਹਾ.distances[i] = UINT16_MAX;
			long_range_ਸੁਨੇਹਾ.distances[i] = UINT16_MAX;
		}
	}


	// CASE 1
	//WHEN: we publish the long range sensor message
	orb_advert_t obstacle_ਦੂਰੀ_pub = orb_advertise(ORB_ID(obstacle_ਦੂਰੀ), &long_range_ਸੁਨੇਹਾ);
	orb_advert_t vehicle_attitude_pub = orb_advertise(ORB_ID(vehicle_attitude), &attitude);
	orb_publish(ORB_ID(obstacle_ਦੂਰੀ), obstacle_ਦੂਰੀ_pub, &long_range_ਸੁਨੇਹਾ);
	orb_publish(ORB_ID(vehicle_attitude), vehicle_attitude_pub, &attitude);
	Vector2f modified_setpoint = original_setpoint;
	cp.modifySetpoint(modified_setpoint, curr_vel);

	// THEN: the internal map data should contain the long range measurement
	EXPECT_EQ(500, cp.getObstacleMap().distances[2]);

	// CASE 2
	// WHEN: we publish the short range message followed by a long range message
	short_range_ਸੁਨੇਹਾ.timestamp = hrt_absolute_ਸਮਾਂ();
	orb_publish(ORB_ID(obstacle_ਦੂਰੀ), obstacle_ਦੂਰੀ_pub, &short_range_ਸੁਨੇਹਾ);
	cp.modifySetpoint(modified_setpoint, curr_vel);
	long_range_ਸੁਨੇਹਾ.timestamp = hrt_absolute_ਸਮਾਂ();
	orb_publish(ORB_ID(obstacle_ਦੂਰੀ), obstacle_ਦੂਰੀ_pub, &long_range_ਸੁਨੇਹਾ);
	cp.modifySetpoint(modified_setpoint, curr_vel);

	// THEN: the internal map data should contain the short range measurement
	EXPECT_EQ(150, cp.getObstacleMap().distances[2]);

	// CASE 3
	// WHEN: we publish the short range message with values out of range followed by a long range message
	short_range_ਸੁਨੇਹਾ_no_obstacle.timestamp = hrt_absolute_ਸਮਾਂ();
	orb_publish(ORB_ID(obstacle_ਦੂਰੀ), obstacle_ਦੂਰੀ_pub, &short_range_ਸੁਨੇਹਾ_no_obstacle);
	cp.modifySetpoint(modified_setpoint, curr_vel);
	long_range_ਸੁਨੇਹਾ.timestamp = hrt_absolute_ਸਮਾਂ();
	orb_publish(ORB_ID(obstacle_ਦੂਰੀ), obstacle_ਦੂਰੀ_pub, &long_range_ਸੁਨੇਹਾ);
	cp.modifySetpoint(modified_setpoint, curr_vel);

	// THEN: the internal map data should contain the short range measurement
	EXPECT_EQ(500, cp.getObstacleMap().distances[2]);

	orb_unadvertise(obstacle_ਦੂਰੀ_pub);
	orb_unadvertise(vehicle_attitude_pub);
}

TEST_F(CollisionPreventionTest, enter_ਅੰਕੜੇ_2)
{
	// GIVEN: a simple setup condition
	TestCollisionPrevention cp;
	Quaternion<ਦਸ਼ਮਲਵ> vehicle_attitude(1, 0, 0, 0); //unit transform

	//THEN: just after initialization all bins are at UINT16_MAX and any data should be accepted
	EXPECT_TRUE(cp.test_enter_ਅੰਕੜੇ(16, 2.f, 1.5f)); //shorter range, reading in range
	EXPECT_TRUE(cp.test_enter_ਅੰਕੜੇ(16, 2.f, 3.f)); //shorter range, reading out of range
	EXPECT_TRUE(cp.test_enter_ਅੰਕੜੇ(16, 20.f, 1.5f)); //same range, reading in range
	EXPECT_TRUE(cp.test_enter_ਅੰਕੜੇ(16, 20.f, 21.f)); //same range, reading out of range
	EXPECT_TRUE(cp.test_enter_ਅੰਕੜੇ(16, 30.f, 1.5f)); //longer range, reading in range
	EXPECT_TRUE(cp.test_enter_ਅੰਕੜੇ(16, 30.f, 31.f)); //longer range, reading out of range

	//WHEN: we add distance sensor data to the right with a valid reading
	ਦੂਰੀ_sensor_s ਦੂਰੀ_sensor {};
	ਦੂਰੀ_sensor.min_ਦੂਰੀ = 0.2f;
	ਦੂਰੀ_sensor.max_ਦੂਰੀ = 20.f;
	ਦੂਰੀ_sensor.orientation = ਦੂਰੀ_sensor_s::ROTATION_ਸੱਜਾ_FACING;
	ਦੂਰੀ_sensor.h_fov = math::radians(19.99f);
	ਦੂਰੀ_sensor.current_ਦੂਰੀ = 5.f;
	cp.test_ਜੋੜੋ_ਦੂਰੀ_Sensor_ਅੰਕੜੇ(ਦੂਰੀ_sensor, vehicle_attitude);

	//THEN: the internal map should contain the distance sensor readings
	ਲਈ (ਪੂਰਨ i = 16; i < 20; i++) {
		EXPECT_EQ(500, cp.getObstacleMap().distances[i]) << i;
	}

	//THEN: bins 8 & 9 contain valid readings
	// a valid reading should only be accepted from sensors with shorter or equal range
	// a out of range reading should only be accepted from sensors with the same range

	EXPECT_TRUE(cp.test_enter_ਅੰਕੜੇ(16, 2.f, 1.5f)); //shorter range, reading in range
	EXPECT_FALSE(cp.test_enter_ਅੰਕੜੇ(16, 2.f, 3.f)); //shorter range, reading out of range
	EXPECT_TRUE(cp.test_enter_ਅੰਕੜੇ(16, 20.f, 1.5f)); //same range, reading in range
	EXPECT_TRUE(cp.test_enter_ਅੰਕੜੇ(16, 20.f, 21.f)); //same range, reading out of range
	EXPECT_FALSE(cp.test_enter_ਅੰਕੜੇ(16, 30.f, 1.5f)); //longer range, reading in range
	EXPECT_FALSE(cp.test_enter_ਅੰਕੜੇ(16, 30.f, 31.f)); //longer range, reading out of range

	//WHEN: we add distance sensor data to the right with an out of range reading
	ਦੂਰੀ_sensor.current_ਦੂਰੀ = 21.f;
	cp.test_ਜੋੜੋ_ਦੂਰੀ_Sensor_ਅੰਕੜੇ(ਦੂਰੀ_sensor, vehicle_attitude);

	//THEN: the internal map should contain the distance sensor readings
	ਲਈ (ਪੂਰਨ i = 16; i < 20; i++) {
		EXPECT_EQ(2000, cp.getObstacleMap().distances[i]) << i;
	}

	//THEN: bins 8 & 9 contain readings out of range
	// a reading in range will be accepted in any case
	// out of range readings will only be accepted from sensors with bigger or equal range

	EXPECT_TRUE(cp.test_enter_ਅੰਕੜੇ(16, 2.f, 1.5f)); //shorter range, reading in range
	EXPECT_FALSE(cp.test_enter_ਅੰਕੜੇ(16, 2.f, 3.f)); //shorter range, reading out of range
	EXPECT_TRUE(cp.test_enter_ਅੰਕੜੇ(16, 20.f, 1.5f)); //same range, reading in range
	EXPECT_TRUE(cp.test_enter_ਅੰਕੜੇ(16, 20.f, 21.f)); //same range, reading out of range
	EXPECT_TRUE(cp.test_enter_ਅੰਕੜੇ(16, 30.f, 1.5f)); //longer range, reading in range
	EXPECT_TRUE(cp.test_enter_ਅੰਕੜੇ(16, 30.f, 31.f)); //longer range, reading out of range
}
