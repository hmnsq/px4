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

#అన్తర్భావ <gtest/gtest.h>
#అన్తర్భావ "CollisionPrevention.hpp"

ఉపయోగించి నామాకాశ మాతృక;

// to run: make tests TESTFILTER=CollisionPrevention
hrt_abstime mocked_సమయం = 0;
స్థిరరాశి uint bin_కొలత = CollisionPrevention::BIN_కొలత;
స్థిరరాశి uint bin_లెక్క = CollisionPrevention::BIN_లెక్క;

తరగతి CollisionPreventionTest : బహిరంగ ::testing::Test
{
బహిరంగ:
	శూన్య సమితి_పైకి() override
	{
		param_control_autosave(అబద్ధం);
		param_reset_all();
	}
};

తరగతి TestCollisionPrevention : బహిరంగ CollisionPrevention
{
బహిరంగ:
	TestCollisionPrevention() : CollisionPrevention(శూన్యసూచిక) {}
	శూన్య params_మారింది() {CollisionPrevention::updateParamsImpl();}
	obstacle_దూరం_s &getObstacleMap() {తిరిగి obstacle_map_body_చట్రం;}
	శూన్య test_కూడు_దూరం_Sensor_దత్తాంశం(దూరం_sensor_s &దూరం_sensor, స్థిరరాశి Quatf &attitude)
	{
		కూడు_దూరం_Sensor_దత్తాంశం(దూరం_sensor, attitude);
	}
	శూన్య test_కూడు_Obstacle_Sensor_దత్తాంశం(స్థిరరాశి obstacle_దూరం_s &obstacle, స్థిరరాశి దశాంశం vehicle_yaw)
	{
		కూడు_Obstacle_Sensor_దత్తాంశం(obstacle, vehicle_yaw);
	}
	శూన్య test_adaptSetpointDirection(Vector2f &setpoint_dir, పూర్ణాంకం &setpoint_సూచీ_2,
					 దశాంశం vehicle_yaw_angle_rad)
	{
		_adaptSetpointDirection(setpoint_dir, setpoint_సూచీ_2, vehicle_yaw_angle_rad);
	}
	తార్కిక test_enter_దత్తాంశం(పూర్ణాంకం map_సూచీ, దశాంశం sensor_range, దశాంశం sensor_reading)
	{
		తిరిగి enter_దత్తాంశం(map_సూచీ, sensor_range, sensor_reading);
	}
};

తరగతి TestTimingCollisionPrevention : బహిరంగ TestCollisionPrevention
{
బహిరంగ:
	TestTimingCollisionPrevention() : TestCollisionPrevention() {}
రక్షిత:
	hrt_abstime get_సమయం() override
	{
		తిరిగి mocked_సమయం;
	}

	hrt_abstime get_Elapsed_సమయం(స్థిరరాశి hrt_abstime *ptr) override
	{
		తిరిగి mocked_సమయం - *ptr;
	}
};

TEST_F(CollisionPreventionTest, instantiation) { CollisionPrevention cp(శూన్యసూచిక); }

TEST_F(CollisionPreventionTest, behaviorOff)
{
	// GIVEN: a simple setup condition
	TestCollisionPrevention cp;

	// THEN: the collision prevention should be turned off by default
	EXPECT_FALSE(cp.is_చురుకైన());
}

TEST_F(CollisionPreventionTest, no_Sensor_దత్తాంశం)
{
	// GIVEN: a simple setup condition
	TestCollisionPrevention cp;
	Vector2f original_setpoint(10, 0);
	Vector2f curr_vel(2, 0);

	// AND: a parameter handle
	param_t param = param_handle(px4::params::CP_DIST);

	// WHEN: we set the parameter check then apply the setpoint modification
	దశాంశం value = 10; // try to keep 10m away from obstacles
	param_సమితి(param, &value);
	cp.params_మారింది();

	Vector2f modified_setpoint = original_setpoint;
	cp.modifySetpoint(modified_setpoint, curr_vel);

	// THEN: collision prevention should be enabled and limit the speed to zero
	EXPECT_TRUE(cp.is_చురుకైన());
	EXPECT_FLOAT_EQ(0.f, modified_setpoint.norm());
}

TEST_F(CollisionPreventionTest, test_Behavior_On_With_Obstacle_సందేశం)
{
	// GIVEN: a simple setup condition
	TestCollisionPrevention cp;
	Vector2f original_setpoint1(10, 0);
	Vector2f original_setpoint2(-10, 0);
	Vector2f curr_vel(2, 0);
	vehicle_attitude_s attitude;
	attitude.timestamp = hrt_absolute_సమయం();
	attitude.q[0] = 1.0f;
	attitude.q[1] = 0.0f;
	attitude.q[2] = 0.0f;
	attitude.q[3] = 0.0f;

	// AND: a parameter handle
	param_t param1 = param_handle(px4::params::CP_DIST);
	దశాంశం value1 = 10; // try to keep 10m distance
	param_సమితి(param1, &value1);
	param_t param2 = param_handle(px4::params::CP_GUIDE_ANG);
	దశాంశం value2 = 0; // dont guide sideways
	param_సమితి(param2, &value2);
	cp.params_మారింది();

	// AND: an obstacle message
	obstacle_దూరం_s సందేశం_3;
	memset(&సందేశం_3, 0xDEAD, పరిమాణం(సందేశం_3));
	సందేశం_3.చట్రం = సందేశం_3.MAV_చట్రం_GLOBAL; //north aligned
	సందేశం_3.min_దూరం = 100;
	సందేశం_3.max_దూరం = 10000;
	సందేశం_3.angle_స్థానభ్రంశం = 0;
	సందేశం_3.timestamp = hrt_absolute_సమయం();
	పూర్ణాంకం distances_array_కొలత = పరిమాణం(సందేశం_3.distances) / పరిమాణం(సందేశం_3.distances[0]);
	సందేశం_3.increment = 360 / distances_array_కొలత;

	కొరకు (పూర్ణాంకం i = 0; i < distances_array_కొలత; i++) {
		ఒకవేళ (i < 10) {
			సందేశం_3.distances[i] = 101;

		} లేకపోతే {
			సందేశం_3.distances[i] = 10001;
		}
	}

	// WHEN: we publish the message and set the parameter and then run the setpoint modification
	orb_advert_t obstacle_దూరం_pub = orb_advertise(ORB_ID(obstacle_దూరం), &సందేశం_3);
	orb_advert_t vehicle_attitude_pub = orb_advertise(ORB_ID(vehicle_attitude), &attitude);
	orb_publish(ORB_ID(obstacle_దూరం), obstacle_దూరం_pub, &సందేశం_3);
	orb_publish(ORB_ID(vehicle_attitude), vehicle_attitude_pub, &attitude);
	Vector2f modified_setpoint1 = original_setpoint1;
	Vector2f modified_setpoint2 = original_setpoint2;
	cp.modifySetpoint(modified_setpoint1, curr_vel);
	cp.modifySetpoint(modified_setpoint2, curr_vel);
	orb_unadvertise(obstacle_దూరం_pub);
	orb_unadvertise(vehicle_attitude_pub);

	// THEN: the internal map should know the obstacle
	// case 1: the acceleration setpoint should be negative as its pushing you away from the obstacle, and sideways acceleration should be low
	// case 2: the acceleration setpoint should be lower
	EXPECT_FLOAT_EQ(cp.getObstacleMap().min_దూరం, 100);
	EXPECT_FLOAT_EQ(cp.getObstacleMap().max_దూరం, 10000);
	EXPECT_GT(0.f, modified_setpoint1(0)) << modified_setpoint1(0);
	EXPECT_EQ(0.f, fabsf(modified_setpoint1(1))) << modified_setpoint1(1);
	EXPECT_GT(0.f, modified_setpoint2(0))  << original_setpoint2(0);
	EXPECT_EQ(0.f, fabsf(modified_setpoint2(1))) << modified_setpoint2(1);
}

TEST_F(CollisionPreventionTest, test_Behavior_On_With_దూరం_సందేశం)
{
	// GIVEN: a simple setup condition
	TestCollisionPrevention cp;
	Vector2f original_setpoint1(10, 0);
	Vector2f original_setpoint2(-10, 0);
	Vector2f curr_vel(2, 0);
	vehicle_attitude_s attitude;
	attitude.timestamp = hrt_absolute_సమయం();
	attitude.q[0] = 1.0f;
	attitude.q[1] = 0.0f;
	attitude.q[2] = 0.0f;
	attitude.q[3] = 0.0f;

	// AND: a parameter handle
	param_t param = param_handle(px4::params::CP_DIST);
	దశాంశం value = 10; // try to keep 10m distance
	param_సమితి(param, &value);
	cp.params_మారింది();

	// AND: an obstacle message
	దూరం_sensor_s సందేశం_3;
	సందేశం_3.timestamp = hrt_absolute_సమయం();
	సందేశం_3.min_దూరం = 1.f;
	సందేశం_3.max_దూరం = 100.f;
	సందేశం_3.current_దూరం = 1.1f;

	సందేశం_3.variance = 0.1f;
	సందేశం_3.signal_quality = 100;
	సందేశం_3.type = దూరం_sensor_s::MAV_దూరం_SENSOR_LASER;
	సందేశం_3.orientation = దూరం_sensor_s::ROTATION_FORWARD_FACING;
	సందేశం_3.h_fov = math::radians(50.f);
	సందేశం_3.v_fov = math::radians(30.f);

	// WHEN: we publish the message and set the parameter and then run the setpoint modification
	orb_advert_t దూరం_sensor_pub = orb_advertise(ORB_ID(దూరం_sensor), &సందేశం_3);
	orb_advert_t vehicle_attitude_pub = orb_advertise(ORB_ID(vehicle_attitude), &attitude);
	orb_publish(ORB_ID(దూరం_sensor), దూరం_sensor_pub, &సందేశం_3);
	orb_publish(ORB_ID(vehicle_attitude), vehicle_attitude_pub, &attitude);

	//WHEN:  We run the setpoint modification
	Vector2f modified_setpoint1 = original_setpoint1;
	Vector2f modified_setpoint2 = original_setpoint2;
	cp.modifySetpoint(modified_setpoint1, curr_vel);
	cp.modifySetpoint(modified_setpoint2, curr_vel);
	orb_unadvertise(దూరం_sensor_pub);
	orb_unadvertise(vehicle_attitude_pub);

	// THEN: the internal map should know the obstacle
	// case 1: the acceleration setpoint should be negative as its pushing you away from the obstacle, and sideways acceleration should be low
	// case 2: the acceleration setpoint should be lower
	EXPECT_FLOAT_EQ(cp.getObstacleMap().min_దూరం, 100);
	EXPECT_FLOAT_EQ(cp.getObstacleMap().max_దూరం, 10000);

	EXPECT_FLOAT_EQ(cp.getObstacleMap().min_దూరం, 100);
	EXPECT_FLOAT_EQ(cp.getObstacleMap().max_దూరం, 10000);

	EXPECT_GT(0.f, modified_setpoint1(0)) << modified_setpoint1(0);
	EXPECT_EQ(0.f, fabsf(modified_setpoint1(1))) << modified_setpoint1(1);
	EXPECT_GT(0.f, modified_setpoint2(0))  << original_setpoint2(0);
	EXPECT_EQ(0.f, fabsf(modified_setpoint2(1))) << modified_setpoint2(1);
}

TEST_F(CollisionPreventionTest, test_Purge_Old_దత్తాంశం)
{
	// GIVEN: a simple setup condition
	TestTimingCollisionPrevention cp;
	hrt_abstime ప్రారంభం_సమయం = hrt_absolute_సమయం();
	mocked_సమయం = ప్రారంభం_సమయం;
	Vector2f original_setpoint(10, 0);
	Vector2f curr_vel(2, 0);
	vehicle_attitude_s attitude;
	attitude.timestamp = ప్రారంభం_సమయం;
	attitude.q[0] = 1.0f;
	attitude.q[1] = 0.0f;
	attitude.q[2] = 0.0f;
	attitude.q[3] = 0.0f;

	// AND: a parameter handle
	param_t param = param_handle(px4::params::CP_DIST);
	దశాంశం value = 1; // try to keep 10m distance
	param_సమితి(param, &value);
	cp.params_మారింది();

	// AND: an obstacle message
	obstacle_దూరం_s సందేశం_3, సందేశం_కోల్పోయిన_దత్తాంశం;
	memset(&సందేశం_3, 0xDEAD, పరిమాణం(సందేశం_3));
	సందేశం_3.చట్రం = సందేశం_3.MAV_చట్రం_GLOBAL; //north aligned
	సందేశం_3.min_దూరం = 100;
	సందేశం_3.max_దూరం = 10000;
	సందేశం_3.angle_స్థానభ్రంశం = 0;
	సందేశం_3.timestamp = ప్రారంభం_సమయం;
	పూర్ణాంకం distances_array_కొలత = పరిమాణం(సందేశం_3.distances) / పరిమాణం(సందేశం_3.distances[0]);
	సందేశం_3.increment = 360 / distances_array_కొలత;
	సందేశం_కోల్పోయిన_దత్తాంశం = సందేశం_3;

	కొరకు (పూర్ణాంకం i = 0; i < distances_array_కొలత; i++) {
		ఒకవేళ (i < 10) {
			సందేశం_3.distances[i] = 10001;
			సందేశం_కోల్పోయిన_దత్తాంశం.distances[i] = UINT16_MAX;

		} లేకపోతే ఒకవేళ (i > 15 && i < 18) {
			సందేశం_3.distances[i] = 10001;
			సందేశం_కోల్పోయిన_దత్తాంశం.distances[i] = 10001;

		} లేకపోతే {
			సందేశం_3.distances[i] = UINT16_MAX;
			సందేశం_కోల్పోయిన_దత్తాంశం.distances[i] = UINT16_MAX;
		}
	}

	// WHEN: we publish the message and set the parameter and then run the setpoint modification
	orb_advert_t obstacle_దూరం_pub = orb_advertise(ORB_ID(obstacle_దూరం), &సందేశం_3);
	orb_advert_t vehicle_attitude_pub = orb_advertise(ORB_ID(vehicle_attitude), &attitude);
	orb_publish(ORB_ID(obstacle_దూరం), obstacle_దూరం_pub, &సందేశం_3);
	orb_publish(ORB_ID(vehicle_attitude), vehicle_attitude_pub, &attitude);

	కొరకు (పూర్ణాంకం i = 0; i < 10; i++) {
		Vector2f modified_setpoint = original_setpoint;
		cp.modifySetpoint(modified_setpoint, curr_vel);

		mocked_సమయం = mocked_సమయం + 100000; //advance time by 0.1 seconds
		సందేశం_కోల్పోయిన_దత్తాంశం.timestamp = mocked_సమయం;
		orb_publish(ORB_ID(obstacle_దూరం), obstacle_దూరం_pub, &సందేశం_కోల్పోయిన_దత్తాంశం);

		//at iteration 8 change the CP_GO_NO_DATA to True
		ఒకవేళ (i == 8) {
			param_t param_allow = param_handle(px4::params::CP_GO_NO_దత్తాంశం);
			దశాంశం విలువ_allow = 1;
			param_సమితి(param_allow, &విలువ_allow);
			cp.params_మారింది();
		}

		ఒకవేళ (i < 6) {
			// THEN: If the data is new enough, the velocity setpoint should stay the same as the input
			// Note: direction will change slightly due to guidance
			EXPECT_FLOAT_EQ(original_setpoint.norm(), modified_setpoint.norm());

		} లేకపోతే {
			// THEN: If the data is expired, the velocity setpoint should be cut down to zero because there is no data
			//(even if CP_GO_NO_DATA is set to true, because we once had data in those bins and now lost the sensor)
			EXPECT_FLOAT_EQ(0.f, modified_setpoint.norm()) << modified_setpoint(0) << "," << modified_setpoint(1);
		}
	}

	orb_unadvertise(obstacle_దూరం_pub);
	orb_unadvertise(vehicle_attitude_pub);
}

TEST_F(CollisionPreventionTest, test_No_Range_దత్తాంశం)
{
	// GIVEN: a simple setup condition
	TestTimingCollisionPrevention cp;
	hrt_abstime ప్రారంభం_సమయం = hrt_absolute_సమయం();
	mocked_సమయం = ప్రారంభం_సమయం;
	Vector2f original_setpoint(10, 0);
	Vector2f curr_vel(2, 0);
	vehicle_attitude_s attitude;
	attitude.timestamp = ప్రారంభం_సమయం;
	attitude.q[0] = 1.0f;
	attitude.q[1] = 0.0f;
	attitude.q[2] = 0.0f;
	attitude.q[3] = 0.0f;

	// AND: a parameter handle
	param_t param = param_handle(px4::params::CP_DIST);
	దశాంశం value = 10; // try to keep 10m distance
	param_సమితి(param, &value);
	cp.params_మారింది();

	// AND: an obstacle message without any obstacle
	obstacle_దూరం_s సందేశం_3;
	memset(&సందేశం_3, 0xDEAD, పరిమాణం(సందేశం_3));
	సందేశం_3.చట్రం = సందేశం_3.MAV_చట్రం_GLOBAL; //north aligned
	సందేశం_3.min_దూరం = 100;
	సందేశం_3.max_దూరం = 10000;
	సందేశం_3.angle_స్థానభ్రంశం = 0;
	సందేశం_3.timestamp = ప్రారంభం_సమయం;
	పూర్ణాంకం distances_array_కొలత = పరిమాణం(సందేశం_3.distances) / పరిమాణం(సందేశం_3.distances[0]);
	సందేశం_3.increment = 360 / distances_array_కొలత;

	కొరకు (పూర్ణాంకం i = 0; i < distances_array_కొలత; i++) {
		సందేశం_3.distances[i] = 9000;
	}


	// WHEN: we publish the message and set the parameter and then run the setpoint modification
	orb_advert_t obstacle_దూరం_pub = orb_advertise(ORB_ID(obstacle_దూరం), &సందేశం_3);
	orb_advert_t vehicle_attitude_pub = orb_advertise(ORB_ID(vehicle_attitude), &attitude);
	orb_publish(ORB_ID(obstacle_దూరం), obstacle_దూరం_pub, &సందేశం_3);
	orb_publish(ORB_ID(vehicle_attitude), vehicle_attitude_pub, &attitude);

	కొరకు (పూర్ణాంకం i = 0; i < 10; i++) {
		Vector2f modified_setpoint = original_setpoint;
		cp.modifySetpoint(modified_setpoint, curr_vel);

		//advance time by 0.1 seconds but no new message comes in
		mocked_సమయం = mocked_సమయం + 100000;

		ఒకవేళ (i < 5) {
			// THEN: If the data is new enough, the velocity setpoint should stay the same as the input
			// Note: direction will change slightly due to guidance
			EXPECT_FLOAT_EQ(original_setpoint.norm(), modified_setpoint.norm());

		} లేకపోతే {
			// THEN: If the data is expired, the velocity setpoint should be cut down to zero because there is no data
			EXPECT_FLOAT_EQ(0.f, modified_setpoint.norm()) << modified_setpoint(0) << "," << modified_setpoint(1);
		}
	}

	orb_unadvertise(obstacle_దూరం_pub);
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
	దశాంశం value = 2; // try to keep 2m distance
	param_సమితి(param, &value);
	cp.params_మారింది();

	// AND: an obstacle message
	obstacle_దూరం_s సందేశం_3;
	memset(&సందేశం_3, 0xDEAD, పరిమాణం(సందేశం_3));
	సందేశం_3.min_దూరం = 100;
	సందేశం_3.max_దూరం = 2000;
	సందేశం_3.timestamp = hrt_absolute_సమయం();
	సందేశం_3.చట్రం = సందేశం_3.MAV_చట్రం_GLOBAL; //north aligned
	పూర్ణాంకం distances_array_కొలత = పరిమాణం(సందేశం_3.distances) / పరిమాణం(సందేశం_3.distances[0]);
	సందేశం_3.increment = 360 / distances_array_కొలత;

	కొరకు (పూర్ణాంకం i = 0; i < distances_array_కొలత; i++) {
		సందేశం_3.distances[i] = 700;
	}

	// WHEN: we publish the message and set the parameter and then run the setpoint modification
	orb_advert_t obstacle_దూరం_pub = orb_advertise(ORB_ID(obstacle_దూరం), &సందేశం_3);
	orb_publish(ORB_ID(obstacle_దూరం), obstacle_దూరం_pub, &సందేశం_3);
	Vector2f modified_setpoint = original_setpoint;
	cp.modifySetpoint(modified_setpoint, curr_vel);
	orb_unadvertise(obstacle_దూరం_pub);

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
	దశాంశం value = 5; // try to keep 5m distance
	param_సమితి(param, &value);
	cp.params_మారింది();

	// AND: an obstacle message
	obstacle_దూరం_s సందేశం_3;
	memset(&సందేశం_3, 0xDEAD, పరిమాణం(సందేశం_3));
	సందేశం_3.చట్రం = సందేశం_3.MAV_చట్రం_GLOBAL; //north aligned
	సందేశం_3.min_దూరం = 100;
	సందేశం_3.max_దూరం = 2000;
	పూర్ణాంకం distances_array_కొలత = పరిమాణం(సందేశం_3.distances) / పరిమాణం(సందేశం_3.distances[0]);
	సందేశం_3.increment = 360.f / distances_array_కొలత;

	//fov from 45deg to 225deg
	కొరకు (పూర్ణాంకం i = 0; i < distances_array_కొలత; i++) {
		దశాంశం angle = i * సందేశం_3.increment;

		ఒకవేళ (angle > 45.f && angle < 225.f) {
			సందేశం_3.distances[i] = 700;

		} లేకపోతే {
			సందేశం_3.distances[i] = UINT16_MAX;
		}
	}

	// WHEN: we publish the message and modify the setpoint for different demanded setpoints
	orb_advert_t obstacle_దూరం_pub = orb_advertise(ORB_ID(obstacle_దూరం), &సందేశం_3);

	కొరకు (పూర్ణాంకం i = 0; i < distances_array_కొలత; i++) {
		దశాంశం angle_deg = (దశాంశం)i * సందేశం_3.increment;
		దశాంశం angle_rad = math::radians(angle_deg);
		Vector2f original_setpoint = {10.f * cosf(angle_rad), 10.f * sinf(angle_rad)};
		Vector2f modified_setpoint = original_setpoint;
		సందేశం_3.timestamp = hrt_absolute_సమయం();
		orb_publish(ORB_ID(obstacle_దూరం), obstacle_దూరం_pub, &సందేశం_3);
		cp.modifySetpoint(modified_setpoint, curr_vel);

		//THEN: if the resulting setpoint demands velocities bigger zero, it must lie inside the FOV
		దశాంశం setpoint_నిడివి = modified_setpoint.norm();

		ఒకవేళ (setpoint_నిడివి > 0.f) {
			Vector2f setpoint_dir = modified_setpoint / setpoint_నిడివి;
			దశాంశం sp_angle_body_చట్రం = atan2(setpoint_dir(1), setpoint_dir(0));
			దశాంశం sp_angle_deg = math::degrees(wrap_2pi(sp_angle_body_చట్రం));
			EXPECT_GE(sp_angle_deg, 45.f);
			EXPECT_LE(sp_angle_deg, 225.f);
		}
	}

	orb_unadvertise(obstacle_దూరం_pub);
}

TEST_F(CollisionPreventionTest, go_No_దత్తాంశం)
{
	// GIVEN: a simple setup condition with the initial state (no distance data)
	TestCollisionPrevention cp;
	Vector2f curr_vel(2, 0);

	// AND: an obstacle message
	obstacle_దూరం_s సందేశం_3;
	memset(&సందేశం_3, 0xDEAD, పరిమాణం(సందేశం_3));
	సందేశం_3.చట్రం = సందేశం_3.MAV_చట్రం_GLOBAL; //north aligned
	సందేశం_3.min_దూరం = 100;
	సందేశం_3.max_దూరం = 2000;
	పూర్ణాంకం distances_array_కొలత = పరిమాణం(సందేశం_3.distances) / పరిమాణం(సందేశం_3.distances[0]);
	సందేశం_3.increment = 360.f / distances_array_కొలత;

	//fov from 0deg to 20deg
	కొరకు (పూర్ణాంకం i = 0; i < distances_array_కొలత; i++) {
		దశాంశం angle = i * సందేశం_3.increment;

		ఒకవేళ (angle > 0.f && angle < 40.f) {
			సందేశం_3.distances[i] = 1000;

		} లేకపోతే {
			సందేశం_3.distances[i] = UINT16_MAX;
		}
	}

	// AND: a parameter handle
	param_t param = param_handle(px4::params::CP_DIST);
	దశాంశం value = 2; // try to keep 5m distance
	param_సమితి(param, &value);
	cp.params_మారింది();

	// AND: a setpoint outside the field of view
	Vector2f original_setpoint = {-5, 0};
	Vector2f modified_setpoint = original_setpoint;

	//THEN: the modified setpoint should be zero acceleration
	cp.modifySetpoint(modified_setpoint, curr_vel);
	EXPECT_FLOAT_EQ(modified_setpoint.norm(), 0.f);

	//WHEN: we change the parameter CP_GO_NO_DATA to allow flying ouside the FOV
	param_t param_allow = param_handle(px4::params::CP_GO_NO_దత్తాంశం);
	దశాంశం విలువ_allow = 1;
	param_సమితి(param_allow, &విలువ_allow);
	cp.params_మారింది();

	//THEN: When all bins contain UINT_16MAX the setpoint should be zero even if CP_GO_NO_DATA=1
	modified_setpoint = original_setpoint;
	cp.modifySetpoint(modified_setpoint, curr_vel);
	EXPECT_FLOAT_EQ(modified_setpoint.norm(), 0.f);

	//THEN: As soon as the range data contains any valid number, flying outside the FOV is allowed
	సందేశం_3.timestamp = hrt_absolute_సమయం();
	orb_advert_t obstacle_దూరం_pub = orb_advertise(ORB_ID(obstacle_దూరం), &సందేశం_3);
	orb_publish(ORB_ID(obstacle_దూరం), obstacle_దూరం_pub, &సందేశం_3);

	modified_setpoint = original_setpoint;
	cp.modifySetpoint(modified_setpoint, curr_vel);
	EXPECT_FLOAT_EQ(modified_setpoint.norm(), original_setpoint.norm());
	orb_unadvertise(obstacle_దూరం_pub);
}

TEST_F(CollisionPreventionTest, jerk_హద్దు)
{
	// GIVEN: a simple setup condition
	TestCollisionPrevention cp;
	Vector2f original_setpoint(10, 0);
	Vector2f curr_vel(2, 0);

	// AND: distance set to 5m
	param_t param = param_handle(px4::params::CP_DIST);
	దశాంశం value = 5; // try to keep 5m distance
	param_సమితి(param, &value);
	cp.params_మారింది();

	// AND: an obstacle message
	obstacle_దూరం_s సందేశం_3;
	memset(&సందేశం_3, 0xDEAD, పరిమాణం(సందేశం_3));
	సందేశం_3.min_దూరం = 100;
	సందేశం_3.max_దూరం = 2000;
	సందేశం_3.timestamp = hrt_absolute_సమయం();
	సందేశం_3.చట్రం = సందేశం_3.MAV_చట్రం_GLOBAL; //north aligned
	పూర్ణాంకం distances_array_కొలత = పరిమాణం(సందేశం_3.distances) / పరిమాణం(సందేశం_3.distances[0]);
	సందేశం_3.increment = 360 / distances_array_కొలత;

	కొరకు (పూర్ణాంకం i = 0; i < distances_array_కొలత; i++) {
		సందేశం_3.distances[i] = 700;
	}

	// AND: we publish the message and set the parameter and then run the setpoint modification
	orb_advert_t obstacle_దూరం_pub = orb_advertise(ORB_ID(obstacle_దూరం), &సందేశం_3);
	orb_publish(ORB_ID(obstacle_దూరం), obstacle_దూరం_pub, &సందేశం_3);
	Vector2f modified_setpoint_default_jerk = original_setpoint;
	cp.modifySetpoint(modified_setpoint_default_jerk, curr_vel);
	orb_unadvertise(obstacle_దూరం_pub);

	// AND: we now set max jerk to 0.1
	param = param_handle(px4::params::MPC_JERK_MAX);
	value = 0.1; // 0.1 maximum jerk
	param_సమితి(param, &value);
	cp.params_మారింది();

	// WHEN: we run the setpoint modification again
	Vector2f modified_setpoint_limited_jerk = original_setpoint;
	cp.modifySetpoint(modified_setpoint_limited_jerk, curr_vel);

	// THEN: the new setpoint should be much higher than the one with default jerk, as the rate of change in acceleration is more limmited
	EXPECT_GT(modified_setpoint_limited_jerk.norm(), modified_setpoint_default_jerk.norm());

}
TEST_F(CollisionPreventionTest, కూడు_బహిర్గతం_యొక్క_Range_దూరం_Sensor_దత్తాంశం)
{
	// GIVEN: a vehicle attitude and a distance sensor message
	TestCollisionPrevention cp;
	Quaternion<దశాంశం> vehicle_attitude(1, 0, 0, 0); //unit transform
	దూరం_sensor_s దూరం_sensor {};
	దూరం_sensor.min_దూరం = 0.2f;
	దూరం_sensor.max_దూరం = 20.f;
	దూరం_sensor.orientation = దూరం_sensor_s::ROTATION_FORWARD_FACING;
	// Distance is out of Range
	దూరం_sensor.current_దూరం = -1.f;
	దూరం_sensor.signal_quality = 0;

	uint32_t distances_array_కొలత = పరిమాణం(cp.getObstacleMap().distances) / పరిమాణం(cp.getObstacleMap().distances[0]);

	cp.test_కూడు_దూరం_Sensor_దత్తాంశం(దూరం_sensor, vehicle_attitude);

	//THEN: the correct bins in the map should be filled
	కొరకు (uint32_t i = 0; i < distances_array_కొలత; i++) {
		ఒకవేళ (i == 0) {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], దూరం_sensor.max_దూరం * 100.f);

		} లేకపోతే {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], UINT16_MAX);
		}
	}
}

TEST_F(CollisionPreventionTest, కూడు_దూరం_Sensor_దత్తాంశం_Narrow)
{
	// GIVEN: a vehicle attitude and a distance sensor message
	TestCollisionPrevention cp;
	Quaternion<దశాంశం> vehicle_attitude(1, 0, 0, 0); //unit transform
	దూరం_sensor_s దూరం_sensor {};
	దూరం_sensor.min_దూరం = 0.2f;
	దూరం_sensor.max_దూరం = 20.f;
	దూరం_sensor.current_దూరం = 5.f;
	దూరం_sensor.orientation = దూరం_sensor_s::ROTATION_FORWARD_FACING;
	దూరం_sensor.h_fov = math::radians(0.1 * bin_కొలత);

	uint32_t distances_array_కొలత = పరిమాణం(cp.getObstacleMap().distances) / పరిమాణం(cp.getObstacleMap().distances[0]);

	// WHEN the sensor has a very narrow field of view
	cp.test_కూడు_దూరం_Sensor_దత్తాంశం(దూరం_sensor, vehicle_attitude);

	//THEN: the correct bins in the map should be filled
	కొరకు (uint32_t i = 0; i < distances_array_కొలత; i++) {
		ఒకవేళ (i == 0) {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], 500) << i;

		} లేకపోతే {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], UINT16_MAX) << i;
		}
	}
}
TEST_F(CollisionPreventionTest, కూడు_దూరం_Sensor_దత్తాంశం_Slightly_Larger)
{
	// GIVEN: a vehicle attitude and a distance sensor message
	TestCollisionPrevention cp;
	Quaternion<దశాంశం> vehicle_attitude(1, 0, 0, 0); //unit transform
	దూరం_sensor_s దూరం_sensor {};
	దూరం_sensor.min_దూరం = 0.2f;
	దూరం_sensor.max_దూరం = 20.f;
	దూరం_sensor.current_దూరం = 5.f;
	దూరం_sensor.orientation = దూరం_sensor_s::ROTATION_FORWARD_FACING;
	దూరం_sensor.h_fov = math::radians(1.1 * bin_కొలత);

	uint32_t distances_array_కొలత = పరిమాణం(cp.getObstacleMap().distances) / పరిమాణం(cp.getObstacleMap().distances[0]);

	// WHEN the sensor has a very narrow field of view
	cp.test_కూడు_దూరం_Sensor_దత్తాంశం(దూరం_sensor, vehicle_attitude);

	//THEN: the the bins corresponding to -5°, 0° and 5° should be filled
	కొరకు (uint32_t i = 0; i < distances_array_కొలత; i++) {
		ఒకవేళ (i == 71 || i <= 1) {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], 500) << i;

		} లేకపోతే {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], UINT16_MAX) << i;
		}
	}
}

TEST_F(CollisionPreventionTest, కూడు_దూరం_Sensor_దత్తాంశం_2)
{
	// GIVEN: a vehicle attitude and a distance sensor message
	TestCollisionPrevention cp;
	Quaternion<దశాంశం> vehicle_attitude(1, 0, 0, 0); //unit transform
	దూరం_sensor_s దూరం_sensor {};
	దూరం_sensor.min_దూరం = 0.2f;
	దూరం_sensor.max_దూరం = 20.f;
	దూరం_sensor.current_దూరం = 5.f;

	//THEN: at initialization the internal obstacle map should only contain UINT16_MAX
	uint32_t distances_array_కొలత = పరిమాణం(cp.getObstacleMap().distances) / పరిమాణం(cp.getObstacleMap().distances[0]);

	కొరకు (uint32_t i = 0; i < distances_array_కొలత; i++) {
		EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], UINT16_MAX) << i;
	}

	//WHEN: we add distance sensor data to the right
	దూరం_sensor.orientation = దూరం_sensor_s::ROTATION_కుడి_FACING;
	దూరం_sensor.h_fov = math::radians(19.99f);
	cp.test_కూడు_దూరం_Sensor_దత్తాంశం(దూరం_sensor, vehicle_attitude);
	uint fov = round(దూరం_sensor.h_fov * M_RAD_గమ్యం_DEG_F / 2);
	uint ప్రారంభం = (90 - fov) / bin_కొలత;
	uint చివర = (90 + fov) / bin_కొలత;

	//THEN: the correct bins in the map should be filled
	కొరకు (uint32_t i = 0; i < distances_array_కొలత; i++) {
		ఒకవేళ (i >= ప్రారంభం && i <= చివర) {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], 500) << i;

		} లేకపోతే {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], UINT16_MAX) << i;
		}
	}

	//WHEN: we add additionally distance sensor data to the left
	దూరం_sensor.orientation = దూరం_sensor_s::ROTATION_ఎడమ_FACING;
	దూరం_sensor.h_fov = math::radians(50.f);
	దూరం_sensor.current_దూరం = 8.f;
	cp.test_కూడు_దూరం_Sensor_దత్తాంశం(దూరం_sensor, vehicle_attitude);
	fov = round(దూరం_sensor.h_fov * M_RAD_గమ్యం_DEG_F / 2);
	uint start2 = (270 - fov) / bin_కొలత;
	uint end2 = (270 + fov) / bin_కొలత;

	//THEN: the correct bins in the map should be filled
	కొరకు (uint32_t i = 0; i < distances_array_కొలత; i++) {
		ఒకవేళ (i >= ప్రారంభం && i <= చివర) {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], 500) << i;

		} లేకపోతే ఒకవేళ (i >= start2 && i <= end2) {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], 800) << i;

		} లేకపోతే {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], UINT16_MAX) << i;
		}
	}

	//WHEN: we add additionally distance sensor data to the front
	దూరం_sensor.orientation = దూరం_sensor_s::ROTATION_FORWARD_FACING;
	దూరం_sensor.h_fov = math::radians(10.1f);
	దూరం_sensor.current_దూరం = 3.f;
	cp.test_కూడు_దూరం_Sensor_దత్తాంశం(దూరం_sensor, vehicle_attitude);
	fov = round(దూరం_sensor.h_fov * M_RAD_గమ్యం_DEG_F / 2);
	uint start3 = (360 - fov) / bin_కొలత;
	uint end3 = (fov) / bin_కొలత;

	//THEN: the correct bins in the map should be filled
	కొరకు (uint32_t i = 0; i < distances_array_కొలత; i++) {
		ఒకవేళ (i >= ప్రారంభం && i <= చివర) {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], 500)  << i;

		} లేకపోతే ఒకవేళ (i >= start2 && i <= end2) {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], 800) << i;

		} లేకపోతే ఒకవేళ (i >= start3 || i <= end3) {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], 300) << i;

		} లేకపోతే {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], UINT16_MAX) << i;
		}
	}


}

TEST_F(CollisionPreventionTest, కూడు_Obstacle_Sensor_దత్తాంశం_attitude)
{
	// GIVEN: a vehicle attitude and obstacle distance message
	TestCollisionPrevention cp;
	obstacle_దూరం_s obstacle_సందేశం {};
	obstacle_సందేశం.చట్రం = obstacle_సందేశం.MAV_చట్రం_GLOBAL; //north aligned
	obstacle_సందేశం.increment = 5.f;
	obstacle_సందేశం.min_దూరం = 20;
	obstacle_సందేశం.max_దూరం = 2000;
	obstacle_సందేశం.angle_స్థానభ్రంశం = 0.f;

	//obstacle at 10-30 deg world frame, distance 5 meters
	memset(&obstacle_సందేశం.distances[0], UINT16_MAX, పరిమాణం(obstacle_సందేశం.distances));

	పూర్ణాంకం ప్రారంభం = 2;
	పూర్ణాంకం చివర = 6;

	కొరకు (పూర్ణాంకం i = ప్రారంభం; i <= చివర ; i++) {
		obstacle_సందేశం.distances[i] = 500;
	}



	//THEN: at initialization the internal obstacle map should only contain UINT16_MAX
	పూర్ణాంకం distances_array_కొలత = పరిమాణం(cp.getObstacleMap().distances) / పరిమాణం(cp.getObstacleMap().distances[0]);

	కొరకు (పూర్ణాంకం i = 0; i < distances_array_కొలత; i++) {
		EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], UINT16_MAX);
	}

	//WHEN: we add distance sensor data while vehicle has zero yaw
	cp.test_కూడు_Obstacle_Sensor_దత్తాంశం(obstacle_సందేశం, 0.f);

	//THEN: the correct bins in the map should be filled
	కొరకు (పూర్ణాంకం i = 0; i < distances_array_కొలత; i++) {
		ఒకవేళ (i >= ప్రారంభం && i <= చివర) {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], 500) << i;

		} లేకపోతే {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], UINT16_MAX) << i;
		}

		//reset array to UINT16_MAX
		cp.getObstacleMap().distances[i] = UINT16_MAX;
	}


	//WHEN: we add obstacle distance sensor data while vehicle yaw 90deg to the right
	cp.test_కూడు_Obstacle_Sensor_దత్తాంశం(obstacle_సందేశం, M_PI_2);

	//THEN: the correct bins in the map should be filled
	పూర్ణాంకం స్థానభ్రంశం =  bin_లెక్క - 90 / bin_కొలత;

	కొరకు (పూర్ణాంకం i = 0; i < distances_array_కొలత; i++) {
		ఒకవేళ (i >= స్థానభ్రంశం + ప్రారంభం && i <= స్థానభ్రంశం + చివర) {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], 500) << i;

		} లేకపోతే {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], UINT16_MAX) << i;
		}

		//reset array to UINT16_MAX
		cp.getObstacleMap().distances[i] = UINT16_MAX;
	}

	//WHEN: we add obstacle distance sensor data while vehicle yaw 45deg to the left
	cp.test_కూడు_Obstacle_Sensor_దత్తాంశం(obstacle_సందేశం, -M_PI_4);

	//THEN: the correct bins in the map should be filled
	స్థానభ్రంశం =  45 / bin_కొలత;

	కొరకు (పూర్ణాంకం i = 0; i < distances_array_కొలత; i++) {
		ఒకవేళ (i >= స్థానభ్రంశం + ప్రారంభం && i <= స్థానభ్రంశం + చివర) {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], 500) << i;

		} లేకపోతే {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], UINT16_MAX) << i;
		}

		//reset array to UINT16_MAX
		cp.getObstacleMap().distances[i] = UINT16_MAX;
	}

	//WHEN: we add obstacle distance sensor data while vehicle yaw 180deg
	cp.test_కూడు_Obstacle_Sensor_దత్తాంశం(obstacle_సందేశం, M_PI);

	//THEN: the correct bins in the map should be filled
	స్థానభ్రంశం =  180 / bin_కొలత;

	కొరకు (పూర్ణాంకం i = 0; i < distances_array_కొలత; i++) {
		ఒకవేళ (i >= స్థానభ్రంశం + ప్రారంభం && i <= స్థానభ్రంశం + చివర) {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], 500);

		} లేకపోతే {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], UINT16_MAX);
		}

		//reset array to UINT16_MAX
		cp.getObstacleMap().distances[i] = UINT16_MAX;
	}
}

TEST_F(CollisionPreventionTest, కూడు_Obstacle_Sensor_దత్తాంశం_స్థానభ్రంశం_bodyframe)
{
	// GIVEN: a vehicle attitude and obstacle distance message
	TestCollisionPrevention cp;
	obstacle_దూరం_s obstacle_సందేశం {};
	obstacle_సందేశం.చట్రం = obstacle_సందేశం.MAV_చట్రం_BODY_FRD; // Body Frame
	obstacle_సందేశం.increment = 6.f;
	obstacle_సందేశం.min_దూరం = 20;
	obstacle_సందేశం.max_దూరం = 2000;
	obstacle_సందేశం.angle_స్థానభ్రంశం = 0.f;

	//obstacle at 363°-39° deg world frame, distance 5 meters
	memset(&obstacle_సందేశం.distances[0], UINT16_MAX, పరిమాణం(obstacle_సందేశం.distances));

	కొరకు (పూర్ణాంకం i = 0; i <= 6 ; i++) { // 36° at 6° increment
		obstacle_సందేశం.distances[i] = 500;
	}

	//WHEN: we add distance sensor data
	cp.test_కూడు_Obstacle_Sensor_దత్తాంశం(obstacle_సందేశం, 0.f);

	//THEN: the the bins from 0 to 40 in map should be filled, which correspond to the angles from -2.5° to 42.5°
	పూర్ణాంకం distances_array_కొలత = పరిమాణం(cp.getObstacleMap().distances) / పరిమాణం(cp.getObstacleMap().distances[0]);

	కొరకు (పూర్ణాంకం i = 0; i < distances_array_కొలత; i++) {
		ఒకవేళ (i >= 0 && i <= 8) {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], 500) << i;

		} లేకపోతే {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], UINT16_MAX) << i;
		}

		//reset array to UINT16_MAX
		cp.getObstacleMap().distances[i] = UINT16_MAX;
	}

	//WHEN: we add the same obstacle distance sensor data with an angle offset of 30.5°
	obstacle_సందేశం.angle_స్థానభ్రంశం = 30.5f;
	// This then means our obstacle is between 27.5° and 69.5°
	cp.test_కూడు_Obstacle_Sensor_దత్తాంశం(obstacle_సందేశం, 0.f);

	//THEN: the bins from 30° to 70° in map should be filled, which correspond to the angles from 27.5° to 72.5°
	కొరకు (పూర్ణాంకం i = 0; i < distances_array_కొలత; i++) {
		ఒకవేళ (i >= 6 && i <= 14) {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], 500) << i;

		} లేకపోతే {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], UINT16_MAX) << i;
		}

		//reset array to UINT16_MAX
		cp.getObstacleMap().distances[i] = UINT16_MAX;
	}

	//WHEN: we increase the offset to -30.5°
	obstacle_సందేశం.angle_స్థానభ్రంశం = -30.5f;
	// This then means our obstacle is between 326.5° and 8.5°
	cp.test_కూడు_Obstacle_Sensor_దత్తాంశం(obstacle_సందేశం, 0.f);

	//THEN: the bins from 325° to 10° in map should be filled, which correspond to the angles from 322.5° to 12.5°

	కొరకు (పూర్ణాంకం i = 0; i < distances_array_కొలత; i++) {
		ఒకవేళ (i >= 65 || i <= 2) {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], 500) << i;

		} లేకపోతే {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], UINT16_MAX) << i;
		}

		//reset array to UINT16_MAX
		cp.getObstacleMap().distances[i] = UINT16_MAX;
	}
}

TEST_F(CollisionPreventionTest, కూడు_Obstacle_Sensor_దత్తాంశం_bodyframe)
{
	// GIVEN: a vehicle attitude and obstacle distance message
	TestCollisionPrevention cp;
	obstacle_దూరం_s obstacle_సందేశం {};
	obstacle_సందేశం.చట్రం = obstacle_సందేశం.MAV_చట్రం_BODY_FRD; //north aligned
	obstacle_సందేశం.increment = 5.f;
	obstacle_సందేశం.min_దూరం = 20;
	obstacle_సందేశం.max_దూరం = 2000;
	obstacle_సందేశం.angle_స్థానభ్రంశం = 0.f;

	//obstacle at 10-30 deg body frame, distance 5 meters
	memset(&obstacle_సందేశం.distances[0], UINT16_MAX, పరిమాణం(obstacle_సందేశం.distances));
	పూర్ణాంకం ప్రారంభం = 2;
	పూర్ణాంకం చివర = 6;

	కొరకు (పూర్ణాంకం i = ప్రారంభం; i <= చివర ; i++) {
		obstacle_సందేశం.distances[i] = 500;
	}


	//THEN: at initialization the internal obstacle map should only contain UINT16_MAX
	పూర్ణాంకం distances_array_కొలత = పరిమాణం(cp.getObstacleMap().distances) / పరిమాణం(cp.getObstacleMap().distances[0]);

	కొరకు (పూర్ణాంకం i = 0; i < distances_array_కొలత; i++) {
		EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], UINT16_MAX);
	}

	//WHEN: we add obstacle data while vehicle has zero yaw
	cp.test_కూడు_Obstacle_Sensor_దత్తాంశం(obstacle_సందేశం, 0.f);

	//THEN: the correct bins in the map should be filled

	కొరకు (పూర్ణాంకం i = 0; i < distances_array_కొలత; i++) {
		ఒకవేళ (i >= ప్రారంభం && i <= చివర) {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], 500) << i;

		} లేకపోతే {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], UINT16_MAX) << i;
		}

		//reset array to UINT16_MAX
		cp.getObstacleMap().distances[i] = UINT16_MAX;
	}

	//WHEN: we add obstacle data while vehicle yaw 90deg to the right
	cp.test_కూడు_Obstacle_Sensor_దత్తాంశం(obstacle_సందేశం, M_PI_2);

	//THEN: the correct bins in the map should be filled
	కొరకు (పూర్ణాంకం i = 0; i < distances_array_కొలత; i++) {
		ఒకవేళ (i >= ప్రారంభం && i <= చివర) {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], 500) << i;

		} లేకపోతే {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], UINT16_MAX) << i;
		}

		//reset array to UINT16_MAX
		cp.getObstacleMap().distances[i] = UINT16_MAX;
	}

	//WHEN: we add obstacle data while vehicle yaw 45deg to the left
	cp.test_కూడు_Obstacle_Sensor_దత్తాంశం(obstacle_సందేశం, -M_PI_4);

	//THEN: the correct bins in the map should be filled
	కొరకు (పూర్ణాంకం i = 0; i < distances_array_కొలత; i++) {
		ఒకవేళ (i >= ప్రారంభం && i <= చివర) {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], 500) << i;

		} లేకపోతే {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], UINT16_MAX) << i;
		}

		//reset array to UINT16_MAX
		cp.getObstacleMap().distances[i] = UINT16_MAX;
	}

	//WHEN: we add obstacle data while vehicle yaw 180deg
	cp.test_కూడు_Obstacle_Sensor_దత్తాంశం(obstacle_సందేశం, M_PI);

	//THEN: the correct bins in the map should be filled
	కొరకు (పూర్ణాంకం i = 0; i < distances_array_కొలత; i++) {
		ఒకవేళ (i >= ప్రారంభం && i <= చివర) {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], 500) << i;

		} లేకపోతే {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], UINT16_MAX) << i;
		}

		//reset array to UINT16_MAX
		cp.getObstacleMap().distances[i] = UINT16_MAX;
	}

}


TEST_F(CollisionPreventionTest, కూడు_Obstacle_Sensor_దత్తాంశం_resolution_స్థానభ్రంశం)
{
	// GIVEN: a vehicle attitude and obstacle distance message
	TestCollisionPrevention cp;
	obstacle_దూరం_s obstacle_సందేశం {};
	obstacle_సందేశం.చట్రం = obstacle_సందేశం.MAV_చట్రం_GLOBAL; //north aligned
	obstacle_సందేశం.increment = 6.f;
	obstacle_సందేశం.min_దూరం = 20;
	obstacle_సందేశం.max_దూరం = 2000;
	obstacle_సందేశం.angle_స్థానభ్రంశం = 0.f;

	//obstacle at 363°-39° deg world frame, distance 5 meters
	memset(&obstacle_సందేశం.distances[0], UINT16_MAX, పరిమాణం(obstacle_సందేశం.distances));

	కొరకు (పూర్ణాంకం i = 0; i <= 6 ; i++) { // 36° at 6° increment
		obstacle_సందేశం.distances[i] = 500;
	}

	//WHEN: we add distance sensor data
	cp.test_కూడు_Obstacle_Sensor_దత్తాంశం(obstacle_సందేశం, 0.f);

	//THEN: the the bins from 0 to 40 in map should be filled, which correspond to the angles from -2.5° to 42.5°
	పూర్ణాంకం distances_array_కొలత = పరిమాణం(cp.getObstacleMap().distances) / పరిమాణం(cp.getObstacleMap().distances[0]);

	కొరకు (పూర్ణాంకం i = 0; i < distances_array_కొలత; i++) {
		ఒకవేళ (i >= 0 && i <= 8) {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], 500) << i;

		} లేకపోతే {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], UINT16_MAX) << i;
		}

		//reset array to UINT16_MAX
		cp.getObstacleMap().distances[i] = UINT16_MAX;
	}

	//WHEN: we add the same obstacle distance sensor data with an angle offset of 30.5°
	obstacle_సందేశం.angle_స్థానభ్రంశం = 30.5f;
	// This then means our obstacle is between 27.5° and 69.5°
	cp.test_కూడు_Obstacle_Sensor_దత్తాంశం(obstacle_సందేశం, 0.f);

	//THEN: the bins from 30° to 70° in map should be filled, which correspond to the angles from 27.5° to 72.5°
	కొరకు (పూర్ణాంకం i = 0; i < distances_array_కొలత; i++) {
		ఒకవేళ (i >= 6 && i <= 14) {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], 500) << i;

		} లేకపోతే {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], UINT16_MAX) << i;
		}

		//reset array to UINT16_MAX
		cp.getObstacleMap().distances[i] = UINT16_MAX;
	}

	//WHEN: we increase the offset to -30.5°
	obstacle_సందేశం.angle_స్థానభ్రంశం = -30.5f;
	// This then means our obstacle is between 326.5° and 8.5°
	cp.test_కూడు_Obstacle_Sensor_దత్తాంశం(obstacle_సందేశం, 0.f);

	//THEN: the bins from 325° to 10° in map should be filled, which correspond to the angles from 322.5° to 12.5°

	కొరకు (పూర్ణాంకం i = 0; i < distances_array_కొలత; i++) {
		ఒకవేళ (i >= 65 || i <= 2) {
			EXPECT_FLOAT_EQ(cp.getObstacleMap().distances[i], 500) << i;

		} లేకపోతే {
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
	obstacle_దూరం_s obstacle_సందేశం {};
	obstacle_సందేశం.చట్రం = obstacle_సందేశం.MAV_చట్రం_GLOBAL; //north aligned
	obstacle_సందేశం.increment = 5.f;
	obstacle_సందేశం.min_దూరం = 20;
	obstacle_సందేశం.max_దూరం = 2000;
	obstacle_సందేశం.angle_స్థానభ్రంశం = 0.f;

	స్థిరరాశి దశాంశం vehicle_yaw = 0.f;

	//obstacle at 0-30 deg world frame, distance 5 meters
	memset(&obstacle_సందేశం.distances[0], UINT16_MAX, పరిమాణం(obstacle_సందేశం.distances));

	కొరకు (పూర్ణాంకం i = 0; i <= 6 ; i++) {
		obstacle_సందేశం.distances[i] = 500;
	}

	obstacle_సందేశం.distances[2] = 1000;

	//define setpoint
	Vector2f setpoint_dir(1, 0);
	దశాంశం sp_angle_body_చట్రం = atan2f(setpoint_dir(1), setpoint_dir(0)) - vehicle_yaw;
	దశాంశం sp_angle_with_స్థానభ్రంశం_deg = wrap(math::degrees(sp_angle_body_చట్రం) - cp.getObstacleMap().angle_స్థానభ్రంశం,
					      0.f, 360.f);
	పూర్ణాంకం sp_సూచీ = floor(sp_angle_with_స్థానభ్రంశం_deg / cp.getObstacleMap().increment);

	//set parameter
	param_t param = param_handle(px4::params::CP_DIST);
	దశాంశం value = 3; // try to keep 10m away from obstacles
	param_సమితి(param, &value);
	cp.params_మారింది();

	//WHEN: we add distance sensor data
	cp.test_కూడు_Obstacle_Sensor_దత్తాంశం(obstacle_సందేశం, vehicle_yaw);
	cp.test_adaptSetpointDirection(setpoint_dir, sp_సూచీ, vehicle_yaw);

	//THEN: the setpoint direction should be modified correctly
	EXPECT_EQ(sp_సూచీ, 2);
	EXPECT_FLOAT_EQ(setpoint_dir(0), 0.98480773f);
	EXPECT_FLOAT_EQ(setpoint_dir(1), 0.17364818f);
}

TEST_F(CollisionPreventionTest, adaptSetpointDirection_flat_minimum)
{
	// GIVEN: a vehicle attitude and obstacle distance message
	TestCollisionPrevention cp;
	obstacle_దూరం_s obstacle_సందేశం {};
	obstacle_సందేశం.చట్రం = obstacle_సందేశం.MAV_చట్రం_GLOBAL; //north aligned
	obstacle_సందేశం.increment = 5.f;
	obstacle_సందేశం.min_దూరం = 20;
	obstacle_సందేశం.max_దూరం = 2000;
	obstacle_సందేశం.angle_స్థానభ్రంశం = 0.f;

	స్థిరరాశి దశాంశం vehicle_yaw = 0.f;

	//obstacle at 0-30 deg world frame, distance 5 meters
	memset(&obstacle_సందేశం.distances[0], UINT16_MAX, పరిమాణం(obstacle_సందేశం.distances));

	కొరకు (పూర్ణాంకం i = 0; i < 7 ; i++) {
		obstacle_సందేశం.distances[i] = 500;
	}

	obstacle_సందేశం.distances[1] = 1000;
	obstacle_సందేశం.distances[2] = 1000;
	obstacle_సందేశం.distances[3] = 1000;

	//define setpoint
	Vector2f setpoint_dir(1, 0);
	దశాంశం sp_angle_body_చట్రం = atan2f(setpoint_dir(1), setpoint_dir(0)) - vehicle_yaw;
	దశాంశం sp_angle_with_స్థానభ్రంశం_deg = wrap(math::degrees(sp_angle_body_చట్రం) - cp.getObstacleMap().angle_స్థానభ్రంశం,
					      0.f, 360.f);
	పూర్ణాంకం sp_సూచీ = floor(sp_angle_with_స్థానభ్రంశం_deg / cp.getObstacleMap().increment);

	//set parameter
	param_t param = param_handle(px4::params::CP_DIST);
	దశాంశం value = 3; // try to keep 10m away from obstacles
	param_సమితి(param, &value);
	cp.params_మారింది();

	//WHEN: we add distance sensor data
	cp.test_కూడు_Obstacle_Sensor_దత్తాంశం(obstacle_సందేశం, vehicle_yaw);
	cp.test_adaptSetpointDirection(setpoint_dir, sp_సూచీ, vehicle_yaw);

	//THEN: the setpoint direction should be modified correctly
	EXPECT_EQ(sp_సూచీ, 2);
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
	attitude.timestamp = hrt_absolute_సమయం();
	attitude.q[0] = 1.0f;
	attitude.q[1] = 0.0f;
	attitude.q[2] = 0.0f;
	attitude.q[3] = 0.0f;

	// AND: a parameter handle
	param_t param = param_handle(px4::params::CP_DIST);
	దశాంశం value = 10; // try to keep 10m distance
	param_సమితి(param, &value);
	cp.params_మారింది();

	// AND: an obstacle message for a short range and a long range sensor
	obstacle_దూరం_s short_range_సందేశం, short_range_సందేశం_no_obstacle, long_range_సందేశం;
	memset(&short_range_సందేశం, 0xDEAD, పరిమాణం(short_range_సందేశం));
	short_range_సందేశం.చట్రం = short_range_సందేశం.MAV_చట్రం_GLOBAL; //north aligned
	short_range_సందేశం.angle_స్థానభ్రంశం = 0;
	short_range_సందేశం.timestamp = hrt_absolute_సమయం();
	పూర్ణాంకం distances_array_కొలత = పరిమాణం(short_range_సందేశం.distances) / పరిమాణం(short_range_సందేశం.distances[0]);
	short_range_సందేశం.increment = 360 / distances_array_కొలత;
	long_range_సందేశం = short_range_సందేశం;
	long_range_సందేశం.min_దూరం = 100;
	long_range_సందేశం.max_దూరం = 1000;
	short_range_సందేశం.min_దూరం = 20;
	short_range_సందేశం.max_దూరం = 200;
	short_range_సందేశం_no_obstacle = short_range_సందేశం;


	కొరకు (పూర్ణాంకం i = 0; i < distances_array_కొలత; i++) {
		ఒకవేళ (i < 10) {
			short_range_సందేశం_no_obstacle.distances[i] = 201;
			short_range_సందేశం.distances[i] = 150;
			long_range_సందేశం.distances[i] = 500;

		} లేకపోతే {
			short_range_సందేశం_no_obstacle.distances[i] = UINT16_MAX;
			short_range_సందేశం.distances[i] = UINT16_MAX;
			long_range_సందేశం.distances[i] = UINT16_MAX;
		}
	}


	// CASE 1
	//WHEN: we publish the long range sensor message
	orb_advert_t obstacle_దూరం_pub = orb_advertise(ORB_ID(obstacle_దూరం), &long_range_సందేశం);
	orb_advert_t vehicle_attitude_pub = orb_advertise(ORB_ID(vehicle_attitude), &attitude);
	orb_publish(ORB_ID(obstacle_దూరం), obstacle_దూరం_pub, &long_range_సందేశం);
	orb_publish(ORB_ID(vehicle_attitude), vehicle_attitude_pub, &attitude);
	Vector2f modified_setpoint = original_setpoint;
	cp.modifySetpoint(modified_setpoint, curr_vel);

	// THEN: the internal map data should contain the long range measurement
	EXPECT_EQ(500, cp.getObstacleMap().distances[2]);

	// CASE 2
	// WHEN: we publish the short range message followed by a long range message
	short_range_సందేశం.timestamp = hrt_absolute_సమయం();
	orb_publish(ORB_ID(obstacle_దూరం), obstacle_దూరం_pub, &short_range_సందేశం);
	cp.modifySetpoint(modified_setpoint, curr_vel);
	long_range_సందేశం.timestamp = hrt_absolute_సమయం();
	orb_publish(ORB_ID(obstacle_దూరం), obstacle_దూరం_pub, &long_range_సందేశం);
	cp.modifySetpoint(modified_setpoint, curr_vel);

	// THEN: the internal map data should contain the short range measurement
	EXPECT_EQ(150, cp.getObstacleMap().distances[2]);

	// CASE 3
	// WHEN: we publish the short range message with values out of range followed by a long range message
	short_range_సందేశం_no_obstacle.timestamp = hrt_absolute_సమయం();
	orb_publish(ORB_ID(obstacle_దూరం), obstacle_దూరం_pub, &short_range_సందేశం_no_obstacle);
	cp.modifySetpoint(modified_setpoint, curr_vel);
	long_range_సందేశం.timestamp = hrt_absolute_సమయం();
	orb_publish(ORB_ID(obstacle_దూరం), obstacle_దూరం_pub, &long_range_సందేశం);
	cp.modifySetpoint(modified_setpoint, curr_vel);

	// THEN: the internal map data should contain the short range measurement
	EXPECT_EQ(500, cp.getObstacleMap().distances[2]);

	orb_unadvertise(obstacle_దూరం_pub);
	orb_unadvertise(vehicle_attitude_pub);
}

TEST_F(CollisionPreventionTest, enter_దత్తాంశం_2)
{
	// GIVEN: a simple setup condition
	TestCollisionPrevention cp;
	Quaternion<దశాంశం> vehicle_attitude(1, 0, 0, 0); //unit transform

	//THEN: just after initialization all bins are at UINT16_MAX and any data should be accepted
	EXPECT_TRUE(cp.test_enter_దత్తాంశం(16, 2.f, 1.5f)); //shorter range, reading in range
	EXPECT_TRUE(cp.test_enter_దత్తాంశం(16, 2.f, 3.f)); //shorter range, reading out of range
	EXPECT_TRUE(cp.test_enter_దత్తాంశం(16, 20.f, 1.5f)); //same range, reading in range
	EXPECT_TRUE(cp.test_enter_దత్తాంశం(16, 20.f, 21.f)); //same range, reading out of range
	EXPECT_TRUE(cp.test_enter_దత్తాంశం(16, 30.f, 1.5f)); //longer range, reading in range
	EXPECT_TRUE(cp.test_enter_దత్తాంశం(16, 30.f, 31.f)); //longer range, reading out of range

	//WHEN: we add distance sensor data to the right with a valid reading
	దూరం_sensor_s దూరం_sensor {};
	దూరం_sensor.min_దూరం = 0.2f;
	దూరం_sensor.max_దూరం = 20.f;
	దూరం_sensor.orientation = దూరం_sensor_s::ROTATION_కుడి_FACING;
	దూరం_sensor.h_fov = math::radians(19.99f);
	దూరం_sensor.current_దూరం = 5.f;
	cp.test_కూడు_దూరం_Sensor_దత్తాంశం(దూరం_sensor, vehicle_attitude);

	//THEN: the internal map should contain the distance sensor readings
	కొరకు (పూర్ణాంకం i = 16; i < 20; i++) {
		EXPECT_EQ(500, cp.getObstacleMap().distances[i]) << i;
	}

	//THEN: bins 8 & 9 contain valid readings
	// a valid reading should only be accepted from sensors with shorter or equal range
	// a out of range reading should only be accepted from sensors with the same range

	EXPECT_TRUE(cp.test_enter_దత్తాంశం(16, 2.f, 1.5f)); //shorter range, reading in range
	EXPECT_FALSE(cp.test_enter_దత్తాంశం(16, 2.f, 3.f)); //shorter range, reading out of range
	EXPECT_TRUE(cp.test_enter_దత్తాంశం(16, 20.f, 1.5f)); //same range, reading in range
	EXPECT_TRUE(cp.test_enter_దత్తాంశం(16, 20.f, 21.f)); //same range, reading out of range
	EXPECT_FALSE(cp.test_enter_దత్తాంశం(16, 30.f, 1.5f)); //longer range, reading in range
	EXPECT_FALSE(cp.test_enter_దత్తాంశం(16, 30.f, 31.f)); //longer range, reading out of range

	//WHEN: we add distance sensor data to the right with an out of range reading
	దూరం_sensor.current_దూరం = 21.f;
	cp.test_కూడు_దూరం_Sensor_దత్తాంశం(దూరం_sensor, vehicle_attitude);

	//THEN: the internal map should contain the distance sensor readings
	కొరకు (పూర్ణాంకం i = 16; i < 20; i++) {
		EXPECT_EQ(2000, cp.getObstacleMap().distances[i]) << i;
	}

	//THEN: bins 8 & 9 contain readings out of range
	// a reading in range will be accepted in any case
	// out of range readings will only be accepted from sensors with bigger or equal range

	EXPECT_TRUE(cp.test_enter_దత్తాంశం(16, 2.f, 1.5f)); //shorter range, reading in range
	EXPECT_FALSE(cp.test_enter_దత్తాంశం(16, 2.f, 3.f)); //shorter range, reading out of range
	EXPECT_TRUE(cp.test_enter_దత్తాంశం(16, 20.f, 1.5f)); //same range, reading in range
	EXPECT_TRUE(cp.test_enter_దత్తాంశం(16, 20.f, 21.f)); //same range, reading out of range
	EXPECT_TRUE(cp.test_enter_దత్తాంశం(16, 30.f, 1.5f)); //longer range, reading in range
	EXPECT_TRUE(cp.test_enter_దత్తాంశం(16, 30.f, 31.f)); //longer range, reading out of range
}
