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

#ಅನ್ತರ್ಭಾವ <gtest/gtest.h>
#ಅನ್ತರ್ಭಾವ "CollisionPrevention.hpp"

ಬಳಸಿ ನಾಮಾಕಾಶ ಮಾತೃಕೆ;

// to run: make tests TESTFILTER=CollisionPrevention
hrt_abstime mocked_ಸಮಯ = 0;
ಸ್ಥಿರಾಂಕ uint bin_ಗಾತ್ರ = CollisionPrevention::BIN_ಗಾತ್ರ;
ಸ್ಥಿರಾಂಕ uint bin_ಎಣಿಕೆ = CollisionPrevention::BIN_ಎಣಿಕೆ;

ವರ್ಗ CollisionPreventionTest : ಸಾರ್ವಜನಿಕ ::testing::Test
{
ಸಾರ್ವಜನಿಕ:
	ಶೂನ್ಯ ಗಣ_ಮೇಲೆ() override
	{
		param_control_autosave(ಸುಳ್ಳು);
		param_reset_all();
	}
};

ವರ್ಗ TestCollisionPrevention : ಸಾರ್ವಜನಿಕ CollisionPrevention
{
ಸಾರ್ವಜನಿಕ:
	TestCollisionPrevention() : CollisionPrevention(ಶೂನ್ಯಸೂಚಕ) {}
	ಶೂನ್ಯ params_ಬದಲಾಯಿತು() {CollisionPrevention::updateParamsImpl();}
	obstacle_ದೂರ_s &getObstacleMap() {ಹಿಂತಿರುಗಿ obstacle_map_body_ಚೌಕಟ್ಟು;}
	ಶೂನ್ಯ test_ಕೂಡಿಸು_ದೂರ_Sensor_ದತ್ತಾಂಶ(ದೂರ_sensor_s &ದೂರ_sensor, ಸ್ಥಿರಾಂಕ Quatf &attitude)
	{
		ಕೂಡಿಸು_ದೂರ_Sensor_ದತ್ತಾಂಶ(ದೂರ_sensor, attitude);
	}
	ಶೂನ್ಯ test_ಕೂಡಿಸು_Obstacle_Sensor_ದತ್ತಾಂಶ(ಸ್ಥಿರಾಂಕ obstacle_ದೂರ_s &obstacle, ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ vehicle_yaw)
	{
		ಕೂಡಿಸು_Obstacle_Sensor_ದತ್ತಾಂಶ(obstacle, vehicle_yaw);
	}
	ಶೂನ್ಯ test_adaptSetpointDirection(Vector2f &setpoint_dir, ಪೂರ್ಣಾಂಕ &setpoint_ಸೂಚ್ಯಂಕ_2,
					 ದಶಮಾಂಶ vehicle_yaw_angle_rad)
	{
		_adaptSetpointDirection(setpoint_dir, setpoint_ಸೂಚ್ಯಂಕ_2, vehicle_yaw_angle_rad);
	}
	ತಾರ್ಕಿಕ test_enter_ದತ್ತಾಂಶ(ಪೂರ್ಣಾಂಕ map_ಸೂಚ್ಯಂಕ, ದಶಮಾಂಶ sensor_range, ದಶಮಾಂಶ sensor_reading)
	{
		ಹಿಂತಿರುಗಿ enter_ದತ್ತಾಂಶ(map_ಸೂಚ್ಯಂಕ, sensor_range, sensor_reading);
	}
};

ವರ್ಗ TestTimingCollisionPrevention : ಸಾರ್ವಜನಿಕ TestCollisionPrevention
{
ಸಾರ್ವಜನಿಕ:
	TestTimingCollisionPrevention() : TestCollisionPrevention() {}
ಸಂರಕ್ಷಿತ:
	hrt_abstime get_ಸಮಯ() override
	{
		ಹಿಂತಿರುಗಿ mocked_ಸಮಯ;
	}

	hrt_abstime get_Elapsed_ಸಮಯ(ಸ್ಥಿರಾಂಕ hrt_abstime *ptr) override
	{
		ಹಿಂತಿರುಗಿ mocked_ಸಮಯ - *ptr;
	}
};

TEST_F(CollisionPreventionTest, instantiation) { CollisionPrevention cp(ಶೂನ್ಯಸೂಚಕ); }

TEST_F(CollisionPreventionTest, behaviorOff)
{
	// GIVEN: a simple setup condition
	TestCollisionPrevention cp;

	// THEN: the collision prevention should be turned off by default
	EXPECT_FALSE(cp.is_ಸಕ್ರಿಯ());
}

TEST_F(CollisionPreventionTest, no_Sensor_ದತ್ತಾಂಶ)
{
	// GIVEN: a simple setup condition
	TestCollisionPrevention cp;
	Vector2f original_setpoint(10, 0);
	Vector2f curr_vel(2, 0);

	// AND: a parameter handle
	param_t param = param_handle(px4::params::CP_DIST);

	// WHEN: we set the parameter check then apply the setpoint modification
	ದಶಮಾಂಶ value = 10; // try to keep 10m away from obstacles
	param_ಗಣ(param, &value);
	cp.params_ಬದಲಾಯಿತು();

	Vector2f modified_setpoint = original_setpoint;
	cp.modifySetpoint(modified_setpoint, curr_vel);

	// THEN: collision prevention should be enabled and limit the speed to zero
	EXPECT_TRUE(cp.is_ಸಕ್ರಿಯ());
	EXPECT_FLOAT_EQ(0.f, modified_setpoint.norm());
}

TEST_F(CollisionPreventionTest, test_Behavior_On_With_Obstacle_ಸಂದೇಶ)
{
	// GIVEN: a simple setup condition
	TestCollisionPrevention cp;
	Vector2f original_setpoint1(10, 0);
	Vector2f original_setpoint2(-10, 0);
	Vector2f curr_vel(2, 0);
	vehicle_attitude_s attitude;
	attitude.timestamp = hrt_absolute_ಸಮಯ();
	attitude.q[0] = 1.0f;
	attitude.q[1] = 0.0f;
	attitude.q[2] = 0.0f;
	attitude.q[3] = 0.0f;

	// AND: a parameter handle
	param_t param1 = param_handle(px4::params::CP_DIST);
	ದಶಮಾಂಶ value1 = 10; // try to keep 10m distance
	param_ಗಣ(param1, &value1);
	param_t param2 = param_handle(px4::params::CP_GUIDE_ANG);
	ದಶಮಾಂಶ value2 = 0; // dont guide sideways
	param_ಗಣ(param2, &value2);
	cp.params_ಬದಲಾಯಿತು();

	// AND: an obstacle message
	obstacle_ದೂರ_s ಸಂದೇಶ_3;
	memset(&ಸಂದೇಶ_3, 0xDEAD, ಗಾತ್ರ(ಸಂದೇಶ_3));
	ಸಂದೇಶ_3.ಚೌಕಟ್ಟು = ಸಂದೇಶ_3.MAV_ಚೌಕಟ್ಟು_GLOBAL; //north aligned
	ಸಂದೇಶ_3.min_ದೂರ = 100;
	ಸಂದೇಶ_3.max_ದೂರ = 10000;
	ಸಂದೇಶ_3.angle_ಸ್ಥಾನಾಂತರ = 0;
	ಸಂದೇಶ_3.timestamp = hrt_absolute_ಸಮಯ();
	ಪೂರ್ಣಾಂಕ distances_array_ಗಾತ್ರ = ಗಾತ್ರ(ಸಂದೇಶ_3.distances) / ಗಾತ್ರ(ಸಂದೇಶ_3.distances[0]);
	ಸಂದೇಶ_3.increment = 360 / distances_array_ಗಾತ್ರ;

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < distances_array_ಗಾತ್ರ; i++) {
		ಆದರೆ (i < 10) {
			ಸಂದೇಶ_3.distances[i] = 101;

		} ಇಲ್ಲದಿದ್ದರೆ {
			ಸಂದೇಶ_3.distances[i] = 10001;
		}
	}

	// WHEN: we publish the message and set the parameter and then run the setpoint modification
	orb_advert_t obstacle_ದೂರ_pub = orb_advertise(ORB_ID(obstacle_ದೂರ), &ಸಂದೇಶ_3);
	orb_advert_t vehicle_attitude_pub = orb_advertise(ORB_ID(vehicle_attitude), &attitude);
	orb_publish(ORB_ID(obstacle_ದೂರ), obstacle_ದೂರ_pub, &ಸಂದೇಶ_3);
	orb_publish(ORB_ID(vehicle_attitude), vehicle_attitude_pub, &attitude);
	Vector2f modified_setpoint1 = original_setpoint1;
	Vector2f modified_setpoint2 = original_setpoint2;
	cp.modifySetpoint(modified_setpoint1, curr_vel);
	cp.modifySetpoint(modified_setpoint2, curr_vel);
	orb_unadvertise(obstacle_ದೂರ_pub);
	orb_unadvertise(vehicle_attitude_pub);

	// THEN: the internal map should know the obstacle
	// case 1: the acceleration setpoint should be negative as its pushing you away from the obstacle, and sideways acceleration should be low
	// case 2: the acceleration setpoint should be lower
	EXPECT_FLOAT_EQ(cp.getObstacleMap().min_ದೂರ, 100);
	EXPECT_FLOAT_EQ(cp.getObstacleMap().max_ದೂರ, 10000);
	EXPECT_GT(0.f, modified_setpoint1(0)) << modified_setpoint1(0);
	EXPECT_EQ(0.f, fabsf(modified_setpoint1(1))) << modified_setpoint1(1);
	EXPECT_GT(0.f, modified_setpoint2(0))  << original_setpoint2(0);
	EXPECT_EQ(0.f, fabsf(modified_setpoint2(1))) << modified_setpoint2(1);
}

TEST_F(CollisionPreventionTest, test_Behavior_On_With_ದೂರ_ಸಂದೇಶ)
{
	// GIVEN: a simple setup condition
	TestCollisionPrevention cp;
	Vector2f original_setpoint1(10, 0);
	Vector2f original_setpoint2(-10, 0);
	Vector2f curr_vel(2, 0);
	vehicle_attitude_s attitude;
	attitude.timestamp = hrt_absolute_ಸಮಯ();
	attitude.q[0] = 1.0f;
	attitude.q[1] = 0.0f;
	attitude.q[2] = 0.0f;
	attitude.q[3] = 0.0f;

	// AND: a parameter handle
	param_t param = param_handle(px4::params::CP_DIST);
	ದಶಮಾಂಶ value = 10; // try to keep 10m distance
	param_ಗಣ(param, &value);
	cp.params_ಬದಲಾಯಿತು();

	// AND: an obstacle message
	ದೂರ_sensor_s ಸಂದೇಶ_3;
	ಸಂದೇಶ_3.timestamp = hrt_absolute_ಸಮಯ();
	ಸಂದೇಶ_3.min_ದೂರ = 1.f;
	ಸಂದೇಶ_3.max_ದೂರ = 100.f;
	ಸಂದೇಶ_3.current_ದೂರ = 1.1f;

	ಸಂದೇಶ_3.variance = 0.1f;
	ಸಂದೇಶ_3.signal_quality = 100;
	ಸಂದೇಶ_3.type = ದೂರ_sensor_s::MAV_ದೂರ_SENSOR_LASER;
	ಸಂದೇಶ_3.orientation = ದೂರ_sensor_s::ROTATION_FORWARD_FACING;
	ಸಂದೇಶ_3.h_fov = math::radians(50.f);
	ಸಂದೇಶ_3.v_fov = math::radians(30.f);

	// WHEN: we publish the message and set the parameter and then run the setpoint modification
	orb_advert_t ದೂರ_sensor_pub = orb_advertise(ORB_ID(ದೂರ_sensor), &ಸಂದೇಶ_3);
	orb_advert_t vehicle_attitude_pub = orb_advertise(ORB_ID(vehicle_attitude), &attitude);
	orb_publish(ORB_ID(ದೂರ_sensor), ದೂರ_sensor_pub, &ಸಂದೇಶ_3);
	orb_publish(ORB_ID(vehicle_attitude), vehicle_attitude_pub, &attitude);

	//WHEN:  We run the setpoint modification
	Vector2f modified_setpoint1 = original_setpoint1;
	Vector2f modified_setpoint2 = original_setpoint2;
	cp.modifySetpoint(modified_setpoint1, curr_vel);
	cp.modifySetpoint(modified_setpoint2, curr_vel);
	orb_unadvertise(ದೂರ_sensor_pub);
	orb_unadvertise(vehicle_attitude_pub);

	// THEN: the internal map should know the obstacle
	// case 1: the acceleration setpoint should be negative as its pushing you away from the obstacle, and sideways acceleration should be low
	// case 2: the acceleration setpoint should be lower
	EXPECT_FLOAT_EQ(cp.getObstacleMap().min_ದೂರ, 100);
	EXPECT_FLOAT_EQ(cp.getObstacleMap().max_ದೂರ, 10000);

	EXPECT_FLOAT_EQ(cp.getObstacleMap().min_ದೂರ, 100);
	EXPECT_FLOAT_EQ(cp.getObstacleMap().max_ದೂರ, 10000);

	EXPECT_GT(0.f, modified_setpoint1(0)) << modified_setpoint1(0);
	EXPECT_EQ(0.f, fabsf(modified_setpoint1(1))) << modified_setpoint1(1);
	EXPECT_GT(0.f, modified_setpoint2(0))  << original_setpoint2(0);
	EXPECT_EQ(0.f, fabsf(modified_setpoint2(1))) << modified_setpoint2(1);
}

TEST_F(CollisionPreventionTest, test_Purge_Old_ದತ್ತಾಂಶ)
{
	// GIVEN: a simple setup condition
	TestTimingCollisionPrevention cp;
	hrt_abstime ಆರಂಭ_ಸಮಯ = hrt_absolute_ಸಮಯ();
	mocked_ಸಮಯ = ಆರಂಭ_ಸಮಯ;
	Vector2f original_setpoint(10, 0);
	Vector2f curr_vel(2, 0);
	vehicle_attitude_s attitude;
	attitude.timestamp = ಆರಂಭ_ಸಮಯ;
	attitude.q[0] = 1.0f;
	attitude.q[1] = 0.0f;
	attitude.q[2] = 0.0f;
	attitude.q[3] = 0.0f;

	// AND: a parameter handle
	param_t param = param_handle(px4::params::CP_DIST);
	ದಶಮಾಂಶ value = 1; // try to keep 10m distance
	param_ಗಣ(param, &value);
	cp.params_ಬದಲಾಯಿತು();

	// AND: an obstacle message
	obstacle_ದೂರ_s ಸಂದೇಶ_3, ಸಂದೇಶ_ಕಳೆದುಹೋದ_ದತ್ತಾಂಶ;
	memset(&ಸಂದೇಶ_3, 0xDEAD, ಗಾತ್ರ(ಸಂದೇಶ_3));
	ಸಂದೇಶ_3.ಚೌಕಟ್ಟು = ಸಂದೇಶ_3.MAV_ಚೌಕಟ್ಟು_GLOBAL; //north aligned
	ಸಂದೇಶ_3.min_ದೂರ = 100;
	ಸಂದೇಶ_3.max_ದೂರ = 10000;
	ಸಂದೇಶ_3.angle_ಸ್ಥಾನಾಂತರ = 0;
	ಸಂದೇಶ_3.timestamp = ಆರಂಭ_ಸಮಯ;
	ಪೂರ್ಣಾಂಕ distances_array_ಗಾತ್ರ = ಗಾತ್ರ(ಸಂದೇಶ_3.distances) / ಗಾತ್ರ(ಸಂದೇಶ_3.distances[0]);
	ಸಂದೇಶ_3.increment = 360 / distances_array_ಗಾತ್ರ;
	ಸಂದೇಶ_ಕಳೆದುಹೋದ_ದತ್ತಾಂಶ = ಸಂದೇಶ_3;

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < distances_array_ಗಾತ್ರ; i++) {
		ಆದರೆ (i < 10) {
			ಸಂದೇಶ_3.distances[i] = 10001;
			ಸಂದೇಶ_ಕಳೆದುಹೋದ_ದತ್ತಾಂಶ.distances[i] = UINT16_MAX;

		} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (i > 15 && i < 18) {
			ಸಂದೇಶ_3.distances[i] = 10001;
			ಸಂದೇಶ_ಕಳೆದುಹೋದ_ದತ್ತಾಂಶ.distances[i] = 10001;

		} ಇಲ್ಲದಿದ್ದರೆ {
			ಸಂದೇಶ_3.distances[i] = UINT16_MAX;
			ಸಂದೇಶ_ಕಳೆದುಹೋದ_ದತ್ತಾಂಶ.distances[i] = UINT16_MAX;
		}
	}

	// WHEN: we publish the message and set the parameter and then run the setpoint modification
	orb_advert_t obstacle_ದೂರ_pub = orb_advertise(ORB_ID(obstacle_ದೂರ), &ಸಂದೇಶ_3);
	orb_advert_t vehicle_attitude_pub = orb_advertise(ORB_ID(vehicle_attitude), &attitude);
	orb_publish(ORB_ID(obstacle_ದೂರ), obstacle_ದೂರ_pub, &ಸಂದೇಶ_3);
	orb_publish(ORB_ID(vehicle_attitude), vehicle_attitude_pub, &attitude);

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < 10; i++) {
		Vector2f modified_setpoint = original_setpoint;
		cp.modifySetpoint(modified_setpoint, curr_vel);

		mocked_ಸಮಯ = mocked_ಸಮಯ + 100000; //advance time by 0.1 seconds
		ಸಂದೇಶ_ಕಳೆದುಹೋದ_ದತ್ತಾಂಶ.timestamp = mocked_ಸಮಯ;
		orb_publish(ORB_ID(obstacle_ದೂರ), obstacle_ದೂರ_pub, &ಸಂದೇಶ_ಕಳೆದುಹೋದ_ದತ್ತಾಂಶ);

		//at iteration 8 change the CP_GO_NO_DATA to True
		ಆದರೆ (i == 8) {
			param_t param_allow = param_handle(px4::params::CP_GO_NO_ದತ್ತಾಂಶ);
			ದಶಮಾಂಶ ಮೌಲ್ಯ_allow = 1;
			param_ಗಣ(param_allow, &ಮೌಲ್ಯ_allow);
			cp.params_ಬದಲಾಯಿತು();
		}

		ಆದರೆ (i < 6) {
			// THEN: If the data is new enough, the velocity setpoint should stay the same as the input
			// Note: direction will change slightly due to guidance
			EXPECT_FLOAT_EQ(original_setpoint.norm(), modified_setpoint.norm());

		} ಇಲ್ಲದಿದ್ದರೆ {
			// THEN: If the data is expired, the velocity setpoint should be cut down to zero because there is no data
			//(even if CP_GO_NO_DATA is set to true, because we once had data in those bins and now lost the sensor)
			EXPECT_FLOAT_EQ(0.f, modified_setpoint.norm()) << modified_setpoint(0) << "," << modified_setpoint(1);
		}
	}

	orb_unadvertise(obstacle_ದೂರ_pub);
	orb_unadvertise(vehicle_attitude_pub);
}

TEST_F(CollisionPreventionTest, test_No_Range_ದತ್ತಾಂಶ)
{
	// GIVEN: a simple setup condition
	TestTimingCollisionPrevention cp;
	hrt_abstime ಆರಂಭ_ಸಮಯ = hrt_absolute_ಸಮಯ();
	mocked_ಸಮಯ = ಆರಂಭ_ಸಮಯ;
	Vector2f original_setpoint(10, 0);
	Vector2f curr_vel(2, 0);
	vehicle_attitude_s attitude;
	attitude.timestamp = ಆರಂಭ_ಸಮಯ;
	attitude.q[0] = 1.0f;
	attitude.q[1] = 0.0f;
	attitude.q[2] = 0.0f;
	attitude.q[3] = 0.0f;

	// AND: a parameter handle
	param_t param = param_handle(px4::params::CP_DIST);
	ದಶಮಾಂಶ value = 10; // try to keep 10m distance
	param_ಗಣ(param, &value);
	cp.params_ಬದಲಾಯಿತು();

	// AND: an obstacle message without any obstacle
	obstacle_ದೂರ_s ಸಂದೇಶ_3;
	memset(&ಸಂದೇಶ_3, 0xDEAD, ಗಾತ್ರ(ಸಂದೇಶ_3));
	ಸಂದೇಶ_3.ಚೌಕಟ್ಟು = ಸಂದೇಶ_3.MAV_ಚೌಕಟ್ಟು_GLOBAL; //north aligned
	ಸಂದೇಶ_3.min_ದೂರ = 100;
	ಸಂದೇಶ_3.max_ದೂರ = 10000;
	ಸಂದೇಶ_3.angle_ಸ್ಥಾನಾಂತರ = 0;
	ಸಂದೇಶ_3.timestamp = ಆರಂಭ_ಸಮಯ;
	ಪೂರ್ಣಾಂಕ distances_array_ಗಾತ್ರ = ಗಾತ್ರ(ಸಂದೇಶ_3.distances) / ಗಾತ್ರ(ಸಂದೇಶ_3.distances[0]);
	ಸಂದೇಶ_3.increment = 360 / distances_array_ಗಾತ್ರ;

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < distances_array_ಗಾತ್ರ; i++) {
		ಸಂದೇಶ_3.distances[i] = 9000;
	}


	// WHEN: we publish the message and set the parameter and then run the setpoint modification
	orb_advert_t obstacle_ದೂರ_pub = orb_advertise(ORB_ID(obstacle_ದೂರ), &ಸಂದೇಶ_3);
	orb_advert_t vehicle_attitude_pub = orb_advertise(ORB_ID(vehicle_attitude), &attitude);
	orb_publish(ORB_ID(obstacle_ದೂರ), obstacle_ದೂರ_pub, &ಸಂದೇಶ_3);
	orb_publish(ORB_ID(vehicle_attitude), vehicle_attitude_pub, &attitude);

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < 10; i++) {
		Vector2f modified_setpoint = original_setpoint;
		cp.modifySetpoint(modified_setpoint, curr_vel);

		//advance time by 0.1 seconds but no new message comes in
		mocked_ಸಮಯ = mocked_ಸಮಯ + 100000;

		ಆದರೆ (i < 5) {
			// THEN: If the data is new enough, the velocity setpoint should stay the same as the input
			// Note: direction will change slightly due to guidance
			EXPECT_FLOAT_EQ(original_setpoint.norm(), modified_setpoint.norm());

		} ಇಲ್ಲದಿದ್ದರೆ {
			// THEN: If the data is expired, the velocity setpoint should be cut down to zero because there is no data
			EXPECT_FLOAT_EQ(0.f, modified_setpoint.norm()) << modified_setpoint(0) << "," << modified_setpoint(1);
		}
	}

	orb_unadvertise(obstacle_ದೂರ_pub);
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
	ದಶಮಾಂಶ value = 2; // try to keep 2m distance
	param_ಗಣ(param, &value);
	cp.params_ಬದಲಾಯಿತು();

	// AND: an obstacle message
	obstacle_ದೂರ_s ಸಂದೇಶ_3;
	memset(&ಸಂದೇಶ_3, 0xDEAD, ಗಾತ್ರ(ಸಂದೇಶ_3));
	ಸಂದೇಶ_3.min_ದೂರ = 100;
	ಸಂದೇಶ_3.max_ದೂರ = 2000;
	ಸಂದೇಶ_3.timestamp = hrt_absolute_ಸಮಯ();
	ಸಂದೇಶ_3.ಚೌಕಟ್ಟು = ಸಂದೇಶ_3.MAV_ಚೌಕಟ್ಟು_GLOBAL; //north aligned
	ಪೂರ್ಣಾಂಕ distances_array_ಗಾತ್ರ = ಗಾತ್ರ(ಸಂದೇಶ_3.distances) / ಗಾತ್ರ(ಸಂದೇಶ_3.distances[0]);
	ಸಂದೇಶ_3.increment = 360 / distances_array_ಗಾತ್ರ;

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < distances_array_ಗಾತ್ರ; i++) {
		ಸಂದೇಶ_3.distances[i] = 700;
	}

	// WHEN: we publish the message and set the parameter and then run the setpoint modification
	orb_advert_t obstacle_ದೂರ_pub = orb_advertise(ORB_ID(obstacle_ದೂರ), &ಸಂದೇಶ_3);
	orb_publish(ORB_ID(obstacle_ದೂರ), obstacle_ದೂರ_pub, &ಸಂದೇಶ_3);
	Vector2f modified_setpoint = original_setpoint;
	cp.modifySetpoint(modified_setpoint, curr_vel);
	orb_unadvertise(obstacle_ದೂರ_pub);

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
	ದಶಮಾಂಶ value = 5; // try to keep 5m distance
	param_ಗಣ(param, &value);
	cp.params_ಬದಲಾಯಿತು();

	// AND: an obstacle message
	obstacle_ದೂರ_s ಸಂದೇಶ_3;
	memset(&ಸಂದೇಶ_3, 0xDEAD, ಗಾತ್ರ(ಸಂದೇಶ_3));
	ಸಂದೇಶ_3.ಚೌಕಟ್ಟು = ಸಂದೇಶ_3.MAV_ಚೌಕಟ್ಟು_GLOBAL; //north aligned
	ಸಂದೇಶ_3.min_ದೂರ = 100;
	ಸಂದೇಶ_3.max_ದೂರ = 2000;
	ಪೂರ್ಣಾಂಕ distances_array_ಗಾತ್ರ = ಗಾತ್ರ(ಸಂದೇಶ_3.distances) / ಗಾತ್ರ(ಸಂದೇಶ_3.distances[0]);
	ಸಂದೇಶ_3.increment = 360.f / distances_array_ಗಾತ್ರ;

	//fov from 45deg to 225deg
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < distances_array_ಗಾತ್ರ; i++) {
		ದಶಮಾಂಶ angle = i * ಸಂದೇಶ_3.increment;

		ಆದರೆ (angle > 45.f && angle < 225.f) {
			ಸಂದೇಶ_3.distances[i] = 700;

		} ಇಲ್ಲದಿದ್ದರೆ {
			ಸಂದೇಶ_3.distances[i] = UINT16_MAX;
		}
	}

	// WHEN: we publish the message and modify the setpoint for different demanded setpoints
	orb_advert_t obstacle_ದೂರ_pub = orb_advertise(ORB_ID(obstacle_ದೂರ), &ಸಂದೇಶ_3);

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < distances_array_ಗಾತ್ರ; i++) {
		ದಶಮಾಂಶ angle_deg = (ದಶಮಾಂಶ)i * ಸಂದೇಶ_3.increment;
		ದಶಮಾಂಶ angle_rad = math::radians(angle_deg);
		Vector2f original_setpoint = {10.f * cosf(angle_rad), 10.f * sinf(angle_rad)};
		Vector2f modified_setpoint = original_setpoint;
		ಸಂದೇಶ_3.timestamp = hrt_absolute_ಸಮಯ();
		orb_publish(ORB_ID(obstacle_ದೂರ), obstacle_ದೂರ_pub, &ಸಂದೇಶ_3);
		cp.modifySetpoint(modified_setpoint, curr_vel);

		//THEN: if the resulting setpoint demands velocities bigger zero, it must lie inside the FOV
		ದಶಮಾಂಶ setpoint_ಉದ್ದಳತೆ = modified_setpoint.norm();

		ಆದರೆ (setpoint_ಉದ್ದಳತೆ > 0.f) {
			Vector2f setpoint_dir = modified_setpoint / setpoint_ಉದ್ದಳತೆ;
			ದಶಮಾಂಶ sp_angle_body_ಚೌಕಟ್ಟು = atan2(setpoint_dir(1), setpoint_dir(0));
			ದಶಮಾಂಶ sp_angle_deg = math::degrees(wrap_2pi(sp_angle_body_ಚೌಕಟ್ಟು));
			EXPECT_GE(sp_angle_deg, 45.f);
			EXPECT_LE(sp_angle_deg, 225.f);
		}
	}

	orb_unadvertise(obstacle_ದೂರ_pub);
}

TEST_F(CollisionPreventionTest, go_No_ದತ್ತಾಂಶ)
{
	// GIVEN: a simple setup condition with the initial state (no distance data)
	TestCollisionPrevention cp;
	Vector2f curr_vel(2, 0);

	// AND: an obstacle message
	obstacle_ದೂರ_s ಸಂದೇಶ_3;
	memset(&ಸಂದೇಶ_3, 0xDEAD, ಗಾತ್ರ(ಸಂದೇಶ_3));
	ಸಂದೇಶ_3.ಚೌಕಟ್ಟು = ಸಂದೇಶ_3.MAV_ಚೌಕಟ್ಟು_GLOBAL; //north aligned
	ಸಂದೇಶ_3.min_ದೂರ = 100;
	ಸಂದೇಶ_3.max_ದೂರ = 2000;
	ಪೂರ್ಣಾಂಕ distances_array_ಗಾತ್ರ = ಗಾತ್ರ(ಸಂದೇಶ_3.distances) / ಗಾತ್ರ(ಸಂದೇಶ_3.distances[0]);
	ಸಂದೇಶ_3.increment = 360.f / distances_array_ಗಾತ್ರ;

	//fov from 0deg to 20deg
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < distances_array_ಗಾತ್ರ; i++) {
		ದಶಮಾಂಶ angle = i * ಸಂದೇಶ_3.increment;

		ಆದರೆ (angle > 0.f && angle < 40.f) {
			ಸಂದೇಶ_3.distances[i] = 1000;

		} ಇಲ್ಲದಿದ್ದರೆ {
			ಸಂದೇಶ_3.distances[i] = UINT16_MAX;
		}
	}

	// AND: a parameter handle
	param_t param = param_handle(px4::params::CP_DIST);
	ದಶಮಾಂಶ value = 2; // try to keep 5m distance
	param_ಗಣ(param, &value);
	cp.params_ಬದಲಾಯಿತು();

	// AND: a setpoint outside the field of view
	Vector2f original_setpoint = {-5, 0};
	Vector2f modified_setpoint = original_setpoint;

	//THEN: the modified setpoint should be zero acceleration
	cp.modifySetpoint(modified_setpoint, curr_vel);
	EXPECT_FLOAT_EQ(modified_setpoint.norm(), 0.f);

	//WHEN: we change the parameter CP_GO_NO_DATA to allow flying ouside the FOV
	param_t param_allow = param_handle(px4::params::CP_GO_NO_ದತ್ತಾಂಶ);
	ದಶಮಾಂಶ ಮೌಲ್ಯ_allow = 1;
	param_ಗಣ(param_allow, &ಮೌಲ್ಯ_allow);
	cp.params_ಬದಲಾಯಿತು();

	//THEN: When all bins contain UINT_16MAX the setpoint should be zero even if CP_GO_NO_DATA=1
	modified_setpoint = original_setpoint;
	cp.modifySetpoint(modified_setpoint, curr_vel);
	EXPECT_FLOAT_EQ(modified_setpoint.norm(), 0.f);

	//THEN: As soon as the range data contains any valid number, flying outside the FOV is allowed
	ಸಂದೇಶ_3.timestamp = hrt_absolute_ಸಮಯ();
	orb_advert_t obstacle_ದೂರ_pub = orb_advertise(ORB_ID(obstacle_ದೂರ), &ಸಂದೇಶ_3);
	orb_publish(ORB_ID(obstacle_ದೂರ), obstacle_ದೂರ_pub, &ಸಂದೇಶ_3);

	modified_setpoint = original_setpoint;
	cp.modifySetpoint(modified_setpoint, curr_vel);
	EXPECT_FLOAT_EQ(modified_setpoint.norm(), original_setpoint.norm());
	orb_unadvertise(obstacle_ದೂರ_pub);
}

TEST_F(CollisionPreventionTest, jerk_ಎಲ್ಲೆ)
{
	// GIVEN: a simple setup condition
	TestCollisionPrevention cp;
	Vector2f original_setpoint(10, 0);
	Vector2f curr_vel(2, 0);

	// AND: distance set to 5m
	param_t param = param_handle(px4::params::CP_DIST);
	ದಶಮಾಂಶ value = 5; // try to keep 5m distance
	param_ಗಣ(param, &value);
	cp.params_ಬದಲಾಯಿತು();

	// AND: an obstacle message
	obstacle_ದೂರ_s ಸಂದೇಶ_3;
	memset(&ಸಂದೇಶ_3, 0xDEAD, ಗಾತ್ರ(ಸಂದೇಶ_3));
	ಸಂದೇಶ_3.min_ದೂರ = 100;
	ಸಂದೇಶ_3.max_ದೂರ = 2000;
	ಸಂದೇಶ_3.timestamp = hrt_absolute_ಸಮಯ();
	ಸಂದೇಶ_3.ಚೌಕಟ್ಟು = ಸಂದೇಶ_3.MAV_ಚೌಕಟ್ಟು_GLOBAL; //north aligned
	ಪೂರ್ಣಾಂಕ distances_array_ಗಾತ್ರ = ಗಾತ್ರ(ಸಂದೇಶ_3.distances) / ಗಾತ್ರ(ಸಂದೇಶ_3.distances[0]);
	ಸಂದೇಶ_3.increment = 360 / distances_array_ಗಾತ್ರ;

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < distances_array_ಗಾತ್ರ; i++) {
		ಸಂದೇಶ_3.distances[i] = 700;
	}

	// AND: we publish the message and set the parameter and then run the setpoint modification
	orb_advert_t obstacle_ದೂರ_pub = orb_advertise(ORB_ID(obstacle_ದೂರ), &ಸಂದೇಶ_3);
	orb_publish(ORB_ID(obstacle_ದೂರ), obstacle_ದೂರ_pub, &ಸಂದೇಶ_3);
	Vector2f modified_setpoint_default_jerk = original_setpoint;
	cp.modifySetpoint(modified_setpoint_default_jerk, curr_vel);
	orb_unadvertise(obstacle_ದೂರ_pub);

	// AND: we now set max jerk to 0.1
	param = param_handle(px4::params::MPC_JERK_MAX);
	value = 0.1; // 0.1 maximum jerk
	param_ಗಣ(param, &value);
	cp.params_ಬದಲಾಯಿತು();

	// WHEN: we run the setpoint modification again
	Vector2f modified_setpoint_limited_jerk = original_setpoint;
	cp.modifySetpoint(modified_setpoint_limited_jerk, curr_vel);

	// THEN: the new setpoint should be much higher than the one with default jerk, as the rate of change in acceleration is more limmited
	EXPECT_GT(modified_setpoint_limited_jerk.norm(), modified_setpoint_default_jerk.norm());

}
TEST_F(CollisionPreventionTest, ಕೂಡಿಸು_ಹೊರಹೋಗು_ದ_Range_ದೂರ_Sensor_ದತ್ತಾಂಶ)
{
	// GIVEN: a vehicle attitude and a distance sensor message
	TestCollisionPrevention cp;
	Quaternion<ದಶಮಾಂಶ> vehicle_attitude(1, 0, 0, 0); //unit transform
	ದೂರ_sensor_s ದೂರ_sensor {};
	ದೂರ_sensor.min_ದೂರ = 0.2f;
	ದೂರ_sensor.max_ದೂರ = 20.f;
	ದೂರ_sensor.orientation = ದೂರ_sensor_s::ROTATION_FORWARD_FACING;
	// Distance is out of Range
	ದೂರ_sensor.current_ದೂರ = -1.f;
	ದೂರ_sensor.signal_quality = 0;

	uint32_t distances_array_ಗಾತ್ರ = ಗಾತ್ರ(cp.getObstacleMap().distances) / ಗಾತ್ರ(cp.getObstacleMap().distances[0]);

	cp.test_ಕೂಡಿಸು_ದೂರ_Sensor_ದತ್ತಾಂಶ(ದೂರ_sensor, vehicle_attitude);

	//THEN: the correct bins in the map should be filled
	ಗಾಗಿ (uint32_t i = 0; i < distances_array_ಗಾತ್ರ; i++) {
		ಆದರೆ (i == 0) {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], ದೂರ_sensor.max_ದೂರ * 100.f);

		} ಇಲ್ಲದಿದ್ದರೆ {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], UINT16_MAX);
		}
	}
}

TEST_F(CollisionPreventionTest, ಕೂಡಿಸು_ದೂರ_Sensor_ದತ್ತಾಂಶ_Narrow)
{
	// GIVEN: a vehicle attitude and a distance sensor message
	TestCollisionPrevention cp;
	Quaternion<ದಶಮಾಂಶ> vehicle_attitude(1, 0, 0, 0); //unit transform
	ದೂರ_sensor_s ದೂರ_sensor {};
	ದೂರ_sensor.min_ದೂರ = 0.2f;
	ದೂರ_sensor.max_ದೂರ = 20.f;
	ದೂರ_sensor.current_ದೂರ = 5.f;
	ದೂರ_sensor.orientation = ದೂರ_sensor_s::ROTATION_FORWARD_FACING;
	ದೂರ_sensor.h_fov = math::radians(0.1 * bin_ಗಾತ್ರ);

	uint32_t distances_array_ಗಾತ್ರ = ಗಾತ್ರ(cp.getObstacleMap().distances) / ಗಾತ್ರ(cp.getObstacleMap().distances[0]);

	// WHEN the sensor has a very narrow field of view
	cp.test_ಕೂಡಿಸು_ದೂರ_Sensor_ದತ್ತಾಂಶ(ದೂರ_sensor, vehicle_attitude);

	//THEN: the correct bins in the map should be filled
	ಗಾಗಿ (uint32_t i = 0; i < distances_array_ಗಾತ್ರ; i++) {
		ಆದರೆ (i == 0) {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], 500) << i;

		} ಇಲ್ಲದಿದ್ದರೆ {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], UINT16_MAX) << i;
		}
	}
}
TEST_F(CollisionPreventionTest, ಕೂಡಿಸು_ದೂರ_Sensor_ದತ್ತಾಂಶ_Slightly_Larger)
{
	// GIVEN: a vehicle attitude and a distance sensor message
	TestCollisionPrevention cp;
	Quaternion<ದಶಮಾಂಶ> vehicle_attitude(1, 0, 0, 0); //unit transform
	ದೂರ_sensor_s ದೂರ_sensor {};
	ದೂರ_sensor.min_ದೂರ = 0.2f;
	ದೂರ_sensor.max_ದೂರ = 20.f;
	ದೂರ_sensor.current_ದೂರ = 5.f;
	ದೂರ_sensor.orientation = ದೂರ_sensor_s::ROTATION_FORWARD_FACING;
	ದೂರ_sensor.h_fov = math::radians(1.1 * bin_ಗಾತ್ರ);

	uint32_t distances_array_ಗಾತ್ರ = ಗಾತ್ರ(cp.getObstacleMap().distances) / ಗಾತ್ರ(cp.getObstacleMap().distances[0]);

	// WHEN the sensor has a very narrow field of view
	cp.test_ಕೂಡಿಸು_ದೂರ_Sensor_ದತ್ತಾಂಶ(ದೂರ_sensor, vehicle_attitude);

	//THEN: the the bins corresponding to -5°, 0° and 5° should be filled
	ಗಾಗಿ (uint32_t i = 0; i < distances_array_ಗಾತ್ರ; i++) {
		ಆದರೆ (i == 71 || i <= 1) {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], 500) << i;

		} ಇಲ್ಲದಿದ್ದರೆ {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], UINT16_MAX) << i;
		}
	}
}

TEST_F(CollisionPreventionTest, ಕೂಡಿಸು_ದೂರ_Sensor_ದತ್ತಾಂಶ_2)
{
	// GIVEN: a vehicle attitude and a distance sensor message
	TestCollisionPrevention cp;
	Quaternion<ದಶಮಾಂಶ> vehicle_attitude(1, 0, 0, 0); //unit transform
	ದೂರ_sensor_s ದೂರ_sensor {};
	ದೂರ_sensor.min_ದೂರ = 0.2f;
	ದೂರ_sensor.max_ದೂರ = 20.f;
	ದೂರ_sensor.current_ದೂರ = 5.f;

	//THEN: at initialization the internal obstacle map should only contain UINT16_MAX
	uint32_t distances_array_ಗಾತ್ರ = ಗಾತ್ರ(cp.getObstacleMap().distances) / ಗಾತ್ರ(cp.getObstacleMap().distances[0]);

	ಗಾಗಿ (uint32_t i = 0; i < distances_array_ಗಾತ್ರ; i++) {
		EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], UINT16_MAX) << i;
	}

	//WHEN: we add distance sensor data to the right
	ದೂರ_sensor.orientation = ದೂರ_sensor_s::ROTATION_ಬಲ_FACING;
	ದೂರ_sensor.h_fov = math::radians(19.99f);
	cp.test_ಕೂಡಿಸು_ದೂರ_Sensor_ದತ್ತಾಂಶ(ದೂರ_sensor, vehicle_attitude);
	uint fov = round(ದೂರ_sensor.h_fov * M_RAD_ಗಮ್ಯ_DEG_F / 2);
	uint ಆರಂಭ = (90 - fov) / bin_ಗಾತ್ರ;
	uint ಕೊನೆ = (90 + fov) / bin_ಗಾತ್ರ;

	//THEN: the correct bins in the map should be filled
	ಗಾಗಿ (uint32_t i = 0; i < distances_array_ಗಾತ್ರ; i++) {
		ಆದರೆ (i >= ಆರಂಭ && i <= ಕೊನೆ) {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], 500) << i;

		} ಇಲ್ಲದಿದ್ದರೆ {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], UINT16_MAX) << i;
		}
	}

	//WHEN: we add additionally distance sensor data to the left
	ದೂರ_sensor.orientation = ದೂರ_sensor_s::ROTATION_ಎಡ_FACING;
	ದೂರ_sensor.h_fov = math::radians(50.f);
	ದೂರ_sensor.current_ದೂರ = 8.f;
	cp.test_ಕೂಡಿಸು_ದೂರ_Sensor_ದತ್ತಾಂಶ(ದೂರ_sensor, vehicle_attitude);
	fov = round(ದೂರ_sensor.h_fov * M_RAD_ಗಮ್ಯ_DEG_F / 2);
	uint start2 = (270 - fov) / bin_ಗಾತ್ರ;
	uint end2 = (270 + fov) / bin_ಗಾತ್ರ;

	//THEN: the correct bins in the map should be filled
	ಗಾಗಿ (uint32_t i = 0; i < distances_array_ಗಾತ್ರ; i++) {
		ಆದರೆ (i >= ಆರಂಭ && i <= ಕೊನೆ) {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], 500) << i;

		} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (i >= start2 && i <= end2) {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], 800) << i;

		} ಇಲ್ಲದಿದ್ದರೆ {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], UINT16_MAX) << i;
		}
	}

	//WHEN: we add additionally distance sensor data to the front
	ದೂರ_sensor.orientation = ದೂರ_sensor_s::ROTATION_FORWARD_FACING;
	ದೂರ_sensor.h_fov = math::radians(10.1f);
	ದೂರ_sensor.current_ದೂರ = 3.f;
	cp.test_ಕೂಡಿಸು_ದೂರ_Sensor_ದತ್ತಾಂಶ(ದೂರ_sensor, vehicle_attitude);
	fov = round(ದೂರ_sensor.h_fov * M_RAD_ಗಮ್ಯ_DEG_F / 2);
	uint start3 = (360 - fov) / bin_ಗಾತ್ರ;
	uint end3 = (fov) / bin_ಗಾತ್ರ;

	//THEN: the correct bins in the map should be filled
	ಗಾಗಿ (uint32_t i = 0; i < distances_array_ಗಾತ್ರ; i++) {
		ಆದರೆ (i >= ಆರಂಭ && i <= ಕೊನೆ) {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], 500)  << i;

		} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (i >= start2 && i <= end2) {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], 800) << i;

		} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (i >= start3 || i <= end3) {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], 300) << i;

		} ಇಲ್ಲದಿದ್ದರೆ {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], UINT16_MAX) << i;
		}
	}


}

TEST_F(CollisionPreventionTest, ಕೂಡಿಸು_Obstacle_Sensor_ದತ್ತಾಂಶ_attitude)
{
	// GIVEN: a vehicle attitude and obstacle distance message
	TestCollisionPrevention cp;
	obstacle_ದೂರ_s obstacle_ಸಂದೇಶ {};
	obstacle_ಸಂದೇಶ.ಚೌಕಟ್ಟು = obstacle_ಸಂದೇಶ.MAV_ಚೌಕಟ್ಟು_GLOBAL; //north aligned
	obstacle_ಸಂದೇಶ.increment = 5.f;
	obstacle_ಸಂದೇಶ.min_ದೂರ = 20;
	obstacle_ಸಂದೇಶ.max_ದೂರ = 2000;
	obstacle_ಸಂದೇಶ.angle_ಸ್ಥಾನಾಂತರ = 0.f;

	//obstacle at 10-30 deg world frame, distance 5 meters
	memset(&obstacle_ಸಂದೇಶ.distances[0], UINT16_MAX, ಗಾತ್ರ(obstacle_ಸಂದೇಶ.distances));

	ಪೂರ್ಣಾಂಕ ಆರಂಭ = 2;
	ಪೂರ್ಣಾಂಕ ಕೊನೆ = 6;

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = ಆರಂಭ; i <= ಕೊನೆ ; i++) {
		obstacle_ಸಂದೇಶ.distances[i] = 500;
	}



	//THEN: at initialization the internal obstacle map should only contain UINT16_MAX
	ಪೂರ್ಣಾಂಕ distances_array_ಗಾತ್ರ = ಗಾತ್ರ(cp.getObstacleMap().distances) / ಗಾತ್ರ(cp.getObstacleMap().distances[0]);

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < distances_array_ಗಾತ್ರ; i++) {
		EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], UINT16_MAX);
	}

	//WHEN: we add distance sensor data while vehicle has zero yaw
	cp.test_ಕೂಡಿಸು_Obstacle_Sensor_ದತ್ತಾಂಶ(obstacle_ಸಂದೇಶ, 0.f);

	//THEN: the correct bins in the map should be filled
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < distances_array_ಗಾತ್ರ; i++) {
		ಆದರೆ (i >= ಆರಂಭ && i <= ಕೊನೆ) {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], 500) << i;

		} ಇಲ್ಲದಿದ್ದರೆ {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], UINT16_MAX) << i;
		}

		//reset array to UINT16_MAX
		cp.getObstacleMap().distances[i] = UINT16_MAX;
	}


	//WHEN: we add obstacle distance sensor data while vehicle yaw 90deg to the right
	cp.test_ಕೂಡಿಸು_Obstacle_Sensor_ದತ್ತಾಂಶ(obstacle_ಸಂದೇಶ, M_PI_2);

	//THEN: the correct bins in the map should be filled
	ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ =  bin_ಎಣಿಕೆ - 90 / bin_ಗಾತ್ರ;

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < distances_array_ಗಾತ್ರ; i++) {
		ಆದರೆ (i >= ಸ್ಥಾನಾಂತರ + ಆರಂಭ && i <= ಸ್ಥಾನಾಂತರ + ಕೊನೆ) {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], 500) << i;

		} ಇಲ್ಲದಿದ್ದರೆ {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], UINT16_MAX) << i;
		}

		//reset array to UINT16_MAX
		cp.getObstacleMap().distances[i] = UINT16_MAX;
	}

	//WHEN: we add obstacle distance sensor data while vehicle yaw 45deg to the left
	cp.test_ಕೂಡಿಸು_Obstacle_Sensor_ದತ್ತಾಂಶ(obstacle_ಸಂದೇಶ, -M_PI_4);

	//THEN: the correct bins in the map should be filled
	ಸ್ಥಾನಾಂತರ =  45 / bin_ಗಾತ್ರ;

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < distances_array_ಗಾತ್ರ; i++) {
		ಆದರೆ (i >= ಸ್ಥಾನಾಂತರ + ಆರಂಭ && i <= ಸ್ಥಾನಾಂತರ + ಕೊನೆ) {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], 500) << i;

		} ಇಲ್ಲದಿದ್ದರೆ {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], UINT16_MAX) << i;
		}

		//reset array to UINT16_MAX
		cp.getObstacleMap().distances[i] = UINT16_MAX;
	}

	//WHEN: we add obstacle distance sensor data while vehicle yaw 180deg
	cp.test_ಕೂಡಿಸು_Obstacle_Sensor_ದತ್ತಾಂಶ(obstacle_ಸಂದೇಶ, M_PI);

	//THEN: the correct bins in the map should be filled
	ಸ್ಥಾನಾಂತರ =  180 / bin_ಗಾತ್ರ;

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < distances_array_ಗಾತ್ರ; i++) {
		ಆದರೆ (i >= ಸ್ಥಾನಾಂತರ + ಆರಂಭ && i <= ಸ್ಥಾನಾಂತರ + ಕೊನೆ) {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], 500);

		} ಇಲ್ಲದಿದ್ದರೆ {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], UINT16_MAX);
		}

		//reset array to UINT16_MAX
		cp.getObstacleMap().distances[i] = UINT16_MAX;
	}
}

TEST_F(CollisionPreventionTest, ಕೂಡಿಸು_Obstacle_Sensor_ದತ್ತಾಂಶ_ಸ್ಥಾನಾಂತರ_bodyframe)
{
	// GIVEN: a vehicle attitude and obstacle distance message
	TestCollisionPrevention cp;
	obstacle_ದೂರ_s obstacle_ಸಂದೇಶ {};
	obstacle_ಸಂದೇಶ.ಚೌಕಟ್ಟು = obstacle_ಸಂದೇಶ.MAV_ಚೌಕಟ್ಟು_BODY_FRD; // Body Frame
	obstacle_ಸಂದೇಶ.increment = 6.f;
	obstacle_ಸಂದೇಶ.min_ದೂರ = 20;
	obstacle_ಸಂದೇಶ.max_ದೂರ = 2000;
	obstacle_ಸಂದೇಶ.angle_ಸ್ಥಾನಾಂತರ = 0.f;

	//obstacle at 363°-39° deg world frame, distance 5 meters
	memset(&obstacle_ಸಂದೇಶ.distances[0], UINT16_MAX, ಗಾತ್ರ(obstacle_ಸಂದೇಶ.distances));

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i <= 6 ; i++) { // 36° at 6° increment
		obstacle_ಸಂದೇಶ.distances[i] = 500;
	}

	//WHEN: we add distance sensor data
	cp.test_ಕೂಡಿಸು_Obstacle_Sensor_ದತ್ತಾಂಶ(obstacle_ಸಂದೇಶ, 0.f);

	//THEN: the the bins from 0 to 40 in map should be filled, which correspond to the angles from -2.5° to 42.5°
	ಪೂರ್ಣಾಂಕ distances_array_ಗಾತ್ರ = ಗಾತ್ರ(cp.getObstacleMap().distances) / ಗಾತ್ರ(cp.getObstacleMap().distances[0]);

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < distances_array_ಗಾತ್ರ; i++) {
		ಆದರೆ (i >= 0 && i <= 8) {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], 500) << i;

		} ಇಲ್ಲದಿದ್ದರೆ {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], UINT16_MAX) << i;
		}

		//reset array to UINT16_MAX
		cp.getObstacleMap().distances[i] = UINT16_MAX;
	}

	//WHEN: we add the same obstacle distance sensor data with an angle offset of 30.5°
	obstacle_ಸಂದೇಶ.angle_ಸ್ಥಾನಾಂತರ = 30.5f;
	// This then means our obstacle is between 27.5° and 69.5°
	cp.test_ಕೂಡಿಸು_Obstacle_Sensor_ದತ್ತಾಂಶ(obstacle_ಸಂದೇಶ, 0.f);

	//THEN: the bins from 30° to 70° in map should be filled, which correspond to the angles from 27.5° to 72.5°
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < distances_array_ಗಾತ್ರ; i++) {
		ಆದರೆ (i >= 6 && i <= 14) {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], 500) << i;

		} ಇಲ್ಲದಿದ್ದರೆ {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], UINT16_MAX) << i;
		}

		//reset array to UINT16_MAX
		cp.getObstacleMap().distances[i] = UINT16_MAX;
	}

	//WHEN: we increase the offset to -30.5°
	obstacle_ಸಂದೇಶ.angle_ಸ್ಥಾನಾಂತರ = -30.5f;
	// This then means our obstacle is between 326.5° and 8.5°
	cp.test_ಕೂಡಿಸು_Obstacle_Sensor_ದತ್ತಾಂಶ(obstacle_ಸಂದೇಶ, 0.f);

	//THEN: the bins from 325° to 10° in map should be filled, which correspond to the angles from 322.5° to 12.5°

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < distances_array_ಗಾತ್ರ; i++) {
		ಆದರೆ (i >= 65 || i <= 2) {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], 500) << i;

		} ಇಲ್ಲದಿದ್ದರೆ {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], UINT16_MAX) << i;
		}

		//reset array to UINT16_MAX
		cp.getObstacleMap().distances[i] = UINT16_MAX;
	}
}

TEST_F(CollisionPreventionTest, ಕೂಡಿಸು_Obstacle_Sensor_ದತ್ತಾಂಶ_bodyframe)
{
	// GIVEN: a vehicle attitude and obstacle distance message
	TestCollisionPrevention cp;
	obstacle_ದೂರ_s obstacle_ಸಂದೇಶ {};
	obstacle_ಸಂದೇಶ.ಚೌಕಟ್ಟು = obstacle_ಸಂದೇಶ.MAV_ಚೌಕಟ್ಟು_BODY_FRD; //north aligned
	obstacle_ಸಂದೇಶ.increment = 5.f;
	obstacle_ಸಂದೇಶ.min_ದೂರ = 20;
	obstacle_ಸಂದೇಶ.max_ದೂರ = 2000;
	obstacle_ಸಂದೇಶ.angle_ಸ್ಥಾನಾಂತರ = 0.f;

	//obstacle at 10-30 deg body frame, distance 5 meters
	memset(&obstacle_ಸಂದೇಶ.distances[0], UINT16_MAX, ಗಾತ್ರ(obstacle_ಸಂದೇಶ.distances));
	ಪೂರ್ಣಾಂಕ ಆರಂಭ = 2;
	ಪೂರ್ಣಾಂಕ ಕೊನೆ = 6;

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = ಆರಂಭ; i <= ಕೊನೆ ; i++) {
		obstacle_ಸಂದೇಶ.distances[i] = 500;
	}


	//THEN: at initialization the internal obstacle map should only contain UINT16_MAX
	ಪೂರ್ಣಾಂಕ distances_array_ಗಾತ್ರ = ಗಾತ್ರ(cp.getObstacleMap().distances) / ಗಾತ್ರ(cp.getObstacleMap().distances[0]);

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < distances_array_ಗಾತ್ರ; i++) {
		EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], UINT16_MAX);
	}

	//WHEN: we add obstacle data while vehicle has zero yaw
	cp.test_ಕೂಡಿಸು_Obstacle_Sensor_ದತ್ತಾಂಶ(obstacle_ಸಂದೇಶ, 0.f);

	//THEN: the correct bins in the map should be filled

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < distances_array_ಗಾತ್ರ; i++) {
		ಆದರೆ (i >= ಆರಂಭ && i <= ಕೊನೆ) {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], 500) << i;

		} ಇಲ್ಲದಿದ್ದರೆ {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], UINT16_MAX) << i;
		}

		//reset array to UINT16_MAX
		cp.getObstacleMap().distances[i] = UINT16_MAX;
	}

	//WHEN: we add obstacle data while vehicle yaw 90deg to the right
	cp.test_ಕೂಡಿಸು_Obstacle_Sensor_ದತ್ತಾಂಶ(obstacle_ಸಂದೇಶ, M_PI_2);

	//THEN: the correct bins in the map should be filled
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < distances_array_ಗಾತ್ರ; i++) {
		ಆದರೆ (i >= ಆರಂಭ && i <= ಕೊನೆ) {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], 500) << i;

		} ಇಲ್ಲದಿದ್ದರೆ {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], UINT16_MAX) << i;
		}

		//reset array to UINT16_MAX
		cp.getObstacleMap().distances[i] = UINT16_MAX;
	}

	//WHEN: we add obstacle data while vehicle yaw 45deg to the left
	cp.test_ಕೂಡಿಸು_Obstacle_Sensor_ದತ್ತಾಂಶ(obstacle_ಸಂದೇಶ, -M_PI_4);

	//THEN: the correct bins in the map should be filled
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < distances_array_ಗಾತ್ರ; i++) {
		ಆದರೆ (i >= ಆರಂಭ && i <= ಕೊನೆ) {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], 500) << i;

		} ಇಲ್ಲದಿದ್ದರೆ {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], UINT16_MAX) << i;
		}

		//reset array to UINT16_MAX
		cp.getObstacleMap().distances[i] = UINT16_MAX;
	}

	//WHEN: we add obstacle data while vehicle yaw 180deg
	cp.test_ಕೂಡಿಸು_Obstacle_Sensor_ದತ್ತಾಂಶ(obstacle_ಸಂದೇಶ, M_PI);

	//THEN: the correct bins in the map should be filled
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < distances_array_ಗಾತ್ರ; i++) {
		ಆದರೆ (i >= ಆರಂಭ && i <= ಕೊನೆ) {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], 500) << i;

		} ಇಲ್ಲದಿದ್ದರೆ {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], UINT16_MAX) << i;
		}

		//reset array to UINT16_MAX
		cp.getObstacleMap().distances[i] = UINT16_MAX;
	}

}


TEST_F(CollisionPreventionTest, ಕೂಡಿಸು_Obstacle_Sensor_ದತ್ತಾಂಶ_resolution_ಸ್ಥಾನಾಂತರ)
{
	// GIVEN: a vehicle attitude and obstacle distance message
	TestCollisionPrevention cp;
	obstacle_ದೂರ_s obstacle_ಸಂದೇಶ {};
	obstacle_ಸಂದೇಶ.ಚೌಕಟ್ಟು = obstacle_ಸಂದೇಶ.MAV_ಚೌಕಟ್ಟು_GLOBAL; //north aligned
	obstacle_ಸಂದೇಶ.increment = 6.f;
	obstacle_ಸಂದೇಶ.min_ದೂರ = 20;
	obstacle_ಸಂದೇಶ.max_ದೂರ = 2000;
	obstacle_ಸಂದೇಶ.angle_ಸ್ಥಾನಾಂತರ = 0.f;

	//obstacle at 363°-39° deg world frame, distance 5 meters
	memset(&obstacle_ಸಂದೇಶ.distances[0], UINT16_MAX, ಗಾತ್ರ(obstacle_ಸಂದೇಶ.distances));

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i <= 6 ; i++) { // 36° at 6° increment
		obstacle_ಸಂದೇಶ.distances[i] = 500;
	}

	//WHEN: we add distance sensor data
	cp.test_ಕೂಡಿಸು_Obstacle_Sensor_ದತ್ತಾಂಶ(obstacle_ಸಂದೇಶ, 0.f);

	//THEN: the the bins from 0 to 40 in map should be filled, which correspond to the angles from -2.5° to 42.5°
	ಪೂರ್ಣಾಂಕ distances_array_ಗಾತ್ರ = ಗಾತ್ರ(cp.getObstacleMap().distances) / ಗಾತ್ರ(cp.getObstacleMap().distances[0]);

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < distances_array_ಗಾತ್ರ; i++) {
		ಆದರೆ (i >= 0 && i <= 8) {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], 500) << i;

		} ಇಲ್ಲದಿದ್ದರೆ {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], UINT16_MAX) << i;
		}

		//reset array to UINT16_MAX
		cp.getObstacleMap().distances[i] = UINT16_MAX;
	}

	//WHEN: we add the same obstacle distance sensor data with an angle offset of 30.5°
	obstacle_ಸಂದೇಶ.angle_ಸ್ಥಾನಾಂತರ = 30.5f;
	// This then means our obstacle is between 27.5° and 69.5°
	cp.test_ಕೂಡಿಸು_Obstacle_Sensor_ದತ್ತಾಂಶ(obstacle_ಸಂದೇಶ, 0.f);

	//THEN: the bins from 30° to 70° in map should be filled, which correspond to the angles from 27.5° to 72.5°
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < distances_array_ಗಾತ್ರ; i++) {
		ಆದರೆ (i >= 6 && i <= 14) {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], 500) << i;

		} ಇಲ್ಲದಿದ್ದರೆ {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], UINT16_MAX) << i;
		}

		//reset array to UINT16_MAX
		cp.getObstacleMap().distances[i] = UINT16_MAX;
	}

	//WHEN: we increase the offset to -30.5°
	obstacle_ಸಂದೇಶ.angle_ಸ್ಥಾನಾಂತರ = -30.5f;
	// This then means our obstacle is between 326.5° and 8.5°
	cp.test_ಕೂಡಿಸು_Obstacle_Sensor_ದತ್ತಾಂಶ(obstacle_ಸಂದೇಶ, 0.f);

	//THEN: the bins from 325° to 10° in map should be filled, which correspond to the angles from 322.5° to 12.5°

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < distances_array_ಗಾತ್ರ; i++) {
		ಆದರೆ (i >= 65 || i <= 2) {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], 500) << i;

		} ಇಲ್ಲದಿದ್ದರೆ {
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
	obstacle_ದೂರ_s obstacle_ಸಂದೇಶ {};
	obstacle_ಸಂದೇಶ.ಚೌಕಟ್ಟು = obstacle_ಸಂದೇಶ.MAV_ಚೌಕಟ್ಟು_GLOBAL; //north aligned
	obstacle_ಸಂದೇಶ.increment = 5.f;
	obstacle_ಸಂದೇಶ.min_ದೂರ = 20;
	obstacle_ಸಂದೇಶ.max_ದೂರ = 2000;
	obstacle_ಸಂದೇಶ.angle_ಸ್ಥಾನಾಂತರ = 0.f;

	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ vehicle_yaw = 0.f;

	//obstacle at 0-30 deg world frame, distance 5 meters
	memset(&obstacle_ಸಂದೇಶ.distances[0], UINT16_MAX, ಗಾತ್ರ(obstacle_ಸಂದೇಶ.distances));

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i <= 6 ; i++) {
		obstacle_ಸಂದೇಶ.distances[i] = 500;
	}

	obstacle_ಸಂದೇಶ.distances[2] = 1000;

	//define setpoint
	Vector2f setpoint_dir(1, 0);
	ದಶಮಾಂಶ sp_angle_body_ಚೌಕಟ್ಟು = atan2f(setpoint_dir(1), setpoint_dir(0)) - vehicle_yaw;
	ದಶಮಾಂಶ sp_angle_with_ಸ್ಥಾನಾಂತರ_deg = wrap(math::degrees(sp_angle_body_ಚೌಕಟ್ಟು) - cp.getObstacleMap().angle_ಸ್ಥಾನಾಂತರ,
					      0.f, 360.f);
	ಪೂರ್ಣಾಂಕ sp_ಸೂಚ್ಯಂಕ = floor(sp_angle_with_ಸ್ಥಾನಾಂತರ_deg / cp.getObstacleMap().increment);

	//set parameter
	param_t param = param_handle(px4::params::CP_DIST);
	ದಶಮಾಂಶ value = 3; // try to keep 10m away from obstacles
	param_ಗಣ(param, &value);
	cp.params_ಬದಲಾಯಿತು();

	//WHEN: we add distance sensor data
	cp.test_ಕೂಡಿಸು_Obstacle_Sensor_ದತ್ತಾಂಶ(obstacle_ಸಂದೇಶ, vehicle_yaw);
	cp.test_adaptSetpointDirection(setpoint_dir, sp_ಸೂಚ್ಯಂಕ, vehicle_yaw);

	//THEN: the setpoint direction should be modified correctly
	EXPECT_EQ(sp_ಸೂಚ್ಯಂಕ, 2);
	EXPECT_FLOAT_EQ(setpoint_dir(0), 0.98480773f);
	EXPECT_FLOAT_EQ(setpoint_dir(1), 0.17364818f);
}

TEST_F(CollisionPreventionTest, adaptSetpointDirection_flat_minimum)
{
	// GIVEN: a vehicle attitude and obstacle distance message
	TestCollisionPrevention cp;
	obstacle_ದೂರ_s obstacle_ಸಂದೇಶ {};
	obstacle_ಸಂದೇಶ.ಚೌಕಟ್ಟು = obstacle_ಸಂದೇಶ.MAV_ಚೌಕಟ್ಟು_GLOBAL; //north aligned
	obstacle_ಸಂದೇಶ.increment = 5.f;
	obstacle_ಸಂದೇಶ.min_ದೂರ = 20;
	obstacle_ಸಂದೇಶ.max_ದೂರ = 2000;
	obstacle_ಸಂದೇಶ.angle_ಸ್ಥಾನಾಂತರ = 0.f;

	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ vehicle_yaw = 0.f;

	//obstacle at 0-30 deg world frame, distance 5 meters
	memset(&obstacle_ಸಂದೇಶ.distances[0], UINT16_MAX, ಗಾತ್ರ(obstacle_ಸಂದೇಶ.distances));

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < 7 ; i++) {
		obstacle_ಸಂದೇಶ.distances[i] = 500;
	}

	obstacle_ಸಂದೇಶ.distances[1] = 1000;
	obstacle_ಸಂದೇಶ.distances[2] = 1000;
	obstacle_ಸಂದೇಶ.distances[3] = 1000;

	//define setpoint
	Vector2f setpoint_dir(1, 0);
	ದಶಮಾಂಶ sp_angle_body_ಚೌಕಟ್ಟು = atan2f(setpoint_dir(1), setpoint_dir(0)) - vehicle_yaw;
	ದಶಮಾಂಶ sp_angle_with_ಸ್ಥಾನಾಂತರ_deg = wrap(math::degrees(sp_angle_body_ಚೌಕಟ್ಟು) - cp.getObstacleMap().angle_ಸ್ಥಾನಾಂತರ,
					      0.f, 360.f);
	ಪೂರ್ಣಾಂಕ sp_ಸೂಚ್ಯಂಕ = floor(sp_angle_with_ಸ್ಥಾನಾಂತರ_deg / cp.getObstacleMap().increment);

	//set parameter
	param_t param = param_handle(px4::params::CP_DIST);
	ದಶಮಾಂಶ value = 3; // try to keep 10m away from obstacles
	param_ಗಣ(param, &value);
	cp.params_ಬದಲಾಯಿತು();

	//WHEN: we add distance sensor data
	cp.test_ಕೂಡಿಸು_Obstacle_Sensor_ದತ್ತಾಂಶ(obstacle_ಸಂದೇಶ, vehicle_yaw);
	cp.test_adaptSetpointDirection(setpoint_dir, sp_ಸೂಚ್ಯಂಕ, vehicle_yaw);

	//THEN: the setpoint direction should be modified correctly
	EXPECT_EQ(sp_ಸೂಚ್ಯಂಕ, 2);
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
	attitude.timestamp = hrt_absolute_ಸಮಯ();
	attitude.q[0] = 1.0f;
	attitude.q[1] = 0.0f;
	attitude.q[2] = 0.0f;
	attitude.q[3] = 0.0f;

	// AND: a parameter handle
	param_t param = param_handle(px4::params::CP_DIST);
	ದಶಮಾಂಶ value = 10; // try to keep 10m distance
	param_ಗಣ(param, &value);
	cp.params_ಬದಲಾಯಿತು();

	// AND: an obstacle message for a short range and a long range sensor
	obstacle_ದೂರ_s short_range_ಸಂದೇಶ, short_range_ಸಂದೇಶ_no_obstacle, long_range_ಸಂದೇಶ;
	memset(&short_range_ಸಂದೇಶ, 0xDEAD, ಗಾತ್ರ(short_range_ಸಂದೇಶ));
	short_range_ಸಂದೇಶ.ಚೌಕಟ್ಟು = short_range_ಸಂದೇಶ.MAV_ಚೌಕಟ್ಟು_GLOBAL; //north aligned
	short_range_ಸಂದೇಶ.angle_ಸ್ಥಾನಾಂತರ = 0;
	short_range_ಸಂದೇಶ.timestamp = hrt_absolute_ಸಮಯ();
	ಪೂರ್ಣಾಂಕ distances_array_ಗಾತ್ರ = ಗಾತ್ರ(short_range_ಸಂದೇಶ.distances) / ಗಾತ್ರ(short_range_ಸಂದೇಶ.distances[0]);
	short_range_ಸಂದೇಶ.increment = 360 / distances_array_ಗಾತ್ರ;
	long_range_ಸಂದೇಶ = short_range_ಸಂದೇಶ;
	long_range_ಸಂದೇಶ.min_ದೂರ = 100;
	long_range_ಸಂದೇಶ.max_ದೂರ = 1000;
	short_range_ಸಂದೇಶ.min_ದೂರ = 20;
	short_range_ಸಂದೇಶ.max_ದೂರ = 200;
	short_range_ಸಂದೇಶ_no_obstacle = short_range_ಸಂದೇಶ;


	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < distances_array_ಗಾತ್ರ; i++) {
		ಆದರೆ (i < 10) {
			short_range_ಸಂದೇಶ_no_obstacle.distances[i] = 201;
			short_range_ಸಂದೇಶ.distances[i] = 150;
			long_range_ಸಂದೇಶ.distances[i] = 500;

		} ಇಲ್ಲದಿದ್ದರೆ {
			short_range_ಸಂದೇಶ_no_obstacle.distances[i] = UINT16_MAX;
			short_range_ಸಂದೇಶ.distances[i] = UINT16_MAX;
			long_range_ಸಂದೇಶ.distances[i] = UINT16_MAX;
		}
	}


	// CASE 1
	//WHEN: we publish the long range sensor message
	orb_advert_t obstacle_ದೂರ_pub = orb_advertise(ORB_ID(obstacle_ದೂರ), &long_range_ಸಂದೇಶ);
	orb_advert_t vehicle_attitude_pub = orb_advertise(ORB_ID(vehicle_attitude), &attitude);
	orb_publish(ORB_ID(obstacle_ದೂರ), obstacle_ದೂರ_pub, &long_range_ಸಂದೇಶ);
	orb_publish(ORB_ID(vehicle_attitude), vehicle_attitude_pub, &attitude);
	Vector2f modified_setpoint = original_setpoint;
	cp.modifySetpoint(modified_setpoint, curr_vel);

	// THEN: the internal map data should contain the long range measurement
	EXPECT_EQ(500, cp.getObstacleMap().distances[2]);

	// CASE 2
	// WHEN: we publish the short range message followed by a long range message
	short_range_ಸಂದೇಶ.timestamp = hrt_absolute_ಸಮಯ();
	orb_publish(ORB_ID(obstacle_ದೂರ), obstacle_ದೂರ_pub, &short_range_ಸಂದೇಶ);
	cp.modifySetpoint(modified_setpoint, curr_vel);
	long_range_ಸಂದೇಶ.timestamp = hrt_absolute_ಸಮಯ();
	orb_publish(ORB_ID(obstacle_ದೂರ), obstacle_ದೂರ_pub, &long_range_ಸಂದೇಶ);
	cp.modifySetpoint(modified_setpoint, curr_vel);

	// THEN: the internal map data should contain the short range measurement
	EXPECT_EQ(150, cp.getObstacleMap().distances[2]);

	// CASE 3
	// WHEN: we publish the short range message with values out of range followed by a long range message
	short_range_ಸಂದೇಶ_no_obstacle.timestamp = hrt_absolute_ಸಮಯ();
	orb_publish(ORB_ID(obstacle_ದೂರ), obstacle_ದೂರ_pub, &short_range_ಸಂದೇಶ_no_obstacle);
	cp.modifySetpoint(modified_setpoint, curr_vel);
	long_range_ಸಂದೇಶ.timestamp = hrt_absolute_ಸಮಯ();
	orb_publish(ORB_ID(obstacle_ದೂರ), obstacle_ದೂರ_pub, &long_range_ಸಂದೇಶ);
	cp.modifySetpoint(modified_setpoint, curr_vel);

	// THEN: the internal map data should contain the short range measurement
	EXPECT_EQ(500, cp.getObstacleMap().distances[2]);

	orb_unadvertise(obstacle_ದೂರ_pub);
	orb_unadvertise(vehicle_attitude_pub);
}

TEST_F(CollisionPreventionTest, enter_ದತ್ತಾಂಶ_2)
{
	// GIVEN: a simple setup condition
	TestCollisionPrevention cp;
	Quaternion<ದಶಮಾಂಶ> vehicle_attitude(1, 0, 0, 0); //unit transform

	//THEN: just after initialization all bins are at UINT16_MAX and any data should be accepted
	EXPECT_TRUE(cp.test_enter_ದತ್ತಾಂಶ(16, 2.f, 1.5f)); //shorter range, reading in range
	EXPECT_TRUE(cp.test_enter_ದತ್ತಾಂಶ(16, 2.f, 3.f)); //shorter range, reading out of range
	EXPECT_TRUE(cp.test_enter_ದತ್ತಾಂಶ(16, 20.f, 1.5f)); //same range, reading in range
	EXPECT_TRUE(cp.test_enter_ದತ್ತಾಂಶ(16, 20.f, 21.f)); //same range, reading out of range
	EXPECT_TRUE(cp.test_enter_ದತ್ತಾಂಶ(16, 30.f, 1.5f)); //longer range, reading in range
	EXPECT_TRUE(cp.test_enter_ದತ್ತಾಂಶ(16, 30.f, 31.f)); //longer range, reading out of range

	//WHEN: we add distance sensor data to the right with a valid reading
	ದೂರ_sensor_s ದೂರ_sensor {};
	ದೂರ_sensor.min_ದೂರ = 0.2f;
	ದೂರ_sensor.max_ದೂರ = 20.f;
	ದೂರ_sensor.orientation = ದೂರ_sensor_s::ROTATION_ಬಲ_FACING;
	ದೂರ_sensor.h_fov = math::radians(19.99f);
	ದೂರ_sensor.current_ದೂರ = 5.f;
	cp.test_ಕೂಡಿಸು_ದೂರ_Sensor_ದತ್ತಾಂಶ(ದೂರ_sensor, vehicle_attitude);

	//THEN: the internal map should contain the distance sensor readings
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 16; i < 20; i++) {
		EXPECT_EQ(500, cp.getObstacleMap().distances[i]) << i;
	}

	//THEN: bins 8 & 9 contain valid readings
	// a valid reading should only be accepted from sensors with shorter or equal range
	// a out of range reading should only be accepted from sensors with the same range

	EXPECT_TRUE(cp.test_enter_ದತ್ತಾಂಶ(16, 2.f, 1.5f)); //shorter range, reading in range
	EXPECT_FALSE(cp.test_enter_ದತ್ತಾಂಶ(16, 2.f, 3.f)); //shorter range, reading out of range
	EXPECT_TRUE(cp.test_enter_ದತ್ತಾಂಶ(16, 20.f, 1.5f)); //same range, reading in range
	EXPECT_TRUE(cp.test_enter_ದತ್ತಾಂಶ(16, 20.f, 21.f)); //same range, reading out of range
	EXPECT_FALSE(cp.test_enter_ದತ್ತಾಂಶ(16, 30.f, 1.5f)); //longer range, reading in range
	EXPECT_FALSE(cp.test_enter_ದತ್ತಾಂಶ(16, 30.f, 31.f)); //longer range, reading out of range

	//WHEN: we add distance sensor data to the right with an out of range reading
	ದೂರ_sensor.current_ದೂರ = 21.f;
	cp.test_ಕೂಡಿಸು_ದೂರ_Sensor_ದತ್ತಾಂಶ(ದೂರ_sensor, vehicle_attitude);

	//THEN: the internal map should contain the distance sensor readings
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 16; i < 20; i++) {
		EXPECT_EQ(2000, cp.getObstacleMap().distances[i]) << i;
	}

	//THEN: bins 8 & 9 contain readings out of range
	// a reading in range will be accepted in any case
	// out of range readings will only be accepted from sensors with bigger or equal range

	EXPECT_TRUE(cp.test_enter_ದತ್ತಾಂಶ(16, 2.f, 1.5f)); //shorter range, reading in range
	EXPECT_FALSE(cp.test_enter_ದತ್ತಾಂಶ(16, 2.f, 3.f)); //shorter range, reading out of range
	EXPECT_TRUE(cp.test_enter_ದತ್ತಾಂಶ(16, 20.f, 1.5f)); //same range, reading in range
	EXPECT_TRUE(cp.test_enter_ದತ್ತಾಂಶ(16, 20.f, 21.f)); //same range, reading out of range
	EXPECT_TRUE(cp.test_enter_ದತ್ತಾಂಶ(16, 30.f, 1.5f)); //longer range, reading in range
	EXPECT_TRUE(cp.test_enter_ದತ್ತಾಂಶ(16, 30.f, 31.f)); //longer range, reading out of range
}
