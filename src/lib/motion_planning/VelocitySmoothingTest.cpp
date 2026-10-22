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

/**
 * Test code for the Velocity Smoothing library
 * Run this test only using make tests TESTFILTER=VelocitySmoothing
 */

#ಅನ್ತರ್ಭಾವ <gtest/gtest.h>
#ಅನ್ತರ್ಭಾವ <matrix/matrix/math.hpp>

#ಅನ್ತರ್ಭಾವ "VelocitySmoothing.hpp"

ಬಳಸಿ ನಾಮಾಕಾಶ ಮಾತೃಕೆ;

TEST(VelocitySmoothingBasicTest, AllZeroCase)
{
	// GIVEN: An unconfigured VelocitySmoothing instance
	VelocitySmoothing trajectory;

	// WHEN: We update it
	trajectory.updateDurations(0.f);
	trajectory.updateTraj(0.f);

	// THEN: All the trajectories should still be zero
	EXPECT_FLOAT_EQ(trajectory.get_ಒಟ್ಟು_ಸಮಯ(), 0.f);
	EXPECT_FLOAT_EQ(trajectory.getCurrentJerk(), 0.f);
	EXPECT_FLOAT_EQ(trajectory.getCurrentAcceleration(), 0.f);
	EXPECT_FLOAT_EQ(trajectory.getCurrentVelocity(), 0.f);
	EXPECT_FLOAT_EQ(trajectory.getCurrentPosition(), 0.f);
}

TEST(VelocitySmoothingBasicTest, DivisionByZeroComputeT1)
{
	// GIVEN: A try to trigger division by zero in computeT1()
	// zero jerk but enough input to go that code path
	VelocitySmoothing trajectory;
	trajectory.ಗಣ_Max_Jerk(0.f);
	trajectory.ಗಣ_Max_Accel(0.f);
	trajectory.ಗಣ_Max_Vel(1.f);

	// WHEN: We update it
	trajectory.updateDurations(1.f);
	trajectory.updateTraj(1.f);

	// THEN: All the trajectories should still be zero
	EXPECT_FLOAT_EQ(trajectory.get_ಒಟ್ಟು_ಸಮಯ(), 0.f);
	EXPECT_FLOAT_EQ(trajectory.getCurrentJerk(), 0.f);
	EXPECT_FLOAT_EQ(trajectory.getCurrentAcceleration(), 0.f);
	EXPECT_FLOAT_EQ(trajectory.getCurrentVelocity(), 0.f);
	EXPECT_FLOAT_EQ(trajectory.getCurrentPosition(), 0.f);
}

TEST(VelocitySmoothingBasicTest, DivisionByZeroSaturateT1ForAccel)
{
	// GIVEN: We trigger division by zero in saturateT1ForAccel()
	// zero jerk but enough input to go that code path
	VelocitySmoothing trajectory(1.f, 0.f, 0.f);
	trajectory.ಗಣ_Max_Jerk(0.f);
	trajectory.ಗಣ_Max_Accel(0.f);
	trajectory.ಗಣ_Max_Vel(1.f);

	// WHEN: We update it
	trajectory.updateDurations(1.f);
	trajectory.updateTraj(1.f);

	// THEN: All the trajectories should still be zero
	EXPECT_FLOAT_EQ(trajectory.get_ಒಟ್ಟು_ಸಮಯ(), 0.f);
	EXPECT_FLOAT_EQ(trajectory.getCurrentJerk(), 0.f);
	EXPECT_FLOAT_EQ(trajectory.getCurrentAcceleration(), 0.f);
	EXPECT_FLOAT_EQ(trajectory.getCurrentVelocity(), 0.f);
	EXPECT_FLOAT_EQ(trajectory.getCurrentPosition(), 0.f);
}

ವರ್ಗ VelocitySmoothingTest : ಸಾರ್ವಜನಿಕ ::testing::Test
{
ಸಾರ್ವಜನಿಕ:
	ಶೂನ್ಯ ಗಣ_Constraints(ದಶಮಾಂಶ j_max, ದಶಮಾಂಶ a_max, ದಶಮಾಂಶ v_max);
	ಶೂನ್ಯ ಗಣ_Initial_Conditions(Vector3f acc, Vector3f vel, Vector3f pos);
	ಶೂನ್ಯ updateTrajectories(ದಶಮಾಂಶ dt, Vector3f velocity_setpoints);

	VelocitySmoothing _trajectories[3];
};

ಶೂನ್ಯ VelocitySmoothingTest::ಗಣ_Constraints(ದಶಮಾಂಶ j_max, ದಶಮಾಂಶ a_max, ದಶಮಾಂಶ v_max)
{
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < 3; i++) {
		_trajectories[i].ಗಣ_Max_Jerk(j_max);
		_trajectories[i].ಗಣ_Max_Accel(a_max);
		_trajectories[i].ಗಣ_Max_Vel(v_max);
	}
}

ಶೂನ್ಯ VelocitySmoothingTest::ಗಣ_Initial_Conditions(Vector3f a0, Vector3f v0, Vector3f x0)
{
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < 3; i++) {
		_trajectories[i].ಗಣ_Current_Acceleration(a0(i));
		_trajectories[i].ಗಣ_Current_Velocity(v0(i));
		_trajectories[i].ಗಣ_Current_Position(x0(i));
	}
}

ಶೂನ್ಯ VelocitySmoothingTest::updateTrajectories(ದಶಮಾಂಶ dt, Vector3f velocity_setpoints)
{
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < 3; i++) {
		_trajectories[i].updateTraj(dt);
		EXPECT_LE(fabsf(_trajectories[i].getCurrentJerk()), _trajectories[i].getMaxJerk());
		EXPECT_LE(fabsf(_trajectories[i].getCurrentAcceleration()), _trajectories[i].getMaxAccel());
		EXPECT_LE(fabsf(_trajectories[i].getCurrentVelocity()), _trajectories[i].getMaxVel());
	}

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < 3; i++) {
		_trajectories[i].updateDurations(velocity_setpoints(i));
	}

	VelocitySmoothing::ಸಮಯ_Synchronization(_trajectories, 2);
}

TEST_F(VelocitySmoothingTest, test_ಸಮಯ_Synchronization)
{
	// GIVEN: A set of constraints
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ j_max = 55.2f;
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ a_max = 6.f;
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ v_max = 6.f;

	ಗಣ_Constraints(j_max, a_max, v_max);

	// AND: A set of initial conditions
	Vector3f a0(0.22f, 0.f, 0.22f);
	Vector3f v0(2.47f, -5.59e-6f, 2.47f);
	Vector3f x0(0.f, 0.f, 0.f);

	ಗಣ_Initial_Conditions(a0, v0, x0);

	// WHEN: We generate trajectories (time synchronized in XY) with constant setpoints and dt
	Vector3f velocity_setpoints(-3.f, 1.f, 0.f);
	updateTrajectories(0.f, velocity_setpoints);


	// THEN: The X and Y trajectories should have the same total time (= time sunchronized)
	EXPECT_LE(fabsf(_trajectories[0].get_ಒಟ್ಟು_ಸಮಯ() - _trajectories[1].get_ಒಟ್ಟು_ಸಮಯ()), 0.0001);
}

TEST_F(VelocitySmoothingTest, test_ಸಮಯ_Synchronization_Same_Delta)
{
	// GIVEN: a set of initial conditions
	Vector3f a0(0.f, 0.f, 0.f);
	Vector3f v0(0.5f, -0.2f, 0.f);
	Vector3f x0(0.f, 0.f, 0.f);

	ಗಣ_Initial_Conditions(a0, v0, x0);

	// WHEN: the same delta velocity is set to the XY-axes
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ delta_v = 0.3f;
	Vector3f velocity_setpoints{v0(0) + delta_v, v0(1) + delta_v, 0.f};

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < 3; i++) {
		_trajectories[i].updateDurations(velocity_setpoints(i));
	}

	VelocitySmoothing::ಸಮಯ_Synchronization(_trajectories, 3);

	// THEN: they should have the same T1, T2 and T3 dirations
	EXPECT_FLOAT_EQ(_trajectories[0].get_ಒಟ್ಟು_ಸಮಯ(), _trajectories[1].get_ಒಟ್ಟು_ಸಮಯ());
	EXPECT_FLOAT_EQ(_trajectories[0].getT1(), _trajectories[1].getT1());
	EXPECT_FLOAT_EQ(_trajectories[0].getT2(), _trajectories[1].getT2());
	EXPECT_FLOAT_EQ(_trajectories[0].getT3(), _trajectories[1].getT3());

	// AND: the Z axis should have the same duration but spend all its time in T2 (constant phase)
	EXPECT_FLOAT_EQ(_trajectories[2].get_ಒಟ್ಟು_ಸಮಯ(), _trajectories[0].get_ಒಟ್ಟು_ಸಮಯ());
	EXPECT_FLOAT_EQ(_trajectories[2].getT1(), 0.f);
	EXPECT_FLOAT_EQ(_trajectories[2].getT2(), _trajectories[0].get_ಒಟ್ಟು_ಸಮಯ());
	EXPECT_FLOAT_EQ(_trajectories[2].getT3(), 0.f);
}

TEST_F(VelocitySmoothingTest, testConstantSetpoint)
{
	// GIVEN: A set of constraints
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ j_max = 55.2f;
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ a_max = 6.f;
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ v_max = 6.f;

	ಗಣ_Constraints(j_max, a_max, v_max);

	// AND: A set of initial conditions
	Vector3f a0(0.f, 0.f, 0.f);
	Vector3f v0(0.f, 0.f, 0.f);
	Vector3f x0(0.f, 0.f, 0.f);

	ಗಣ_Initial_Conditions(a0, v0, x0);

	// WHEN: We generate trajectories with constant setpoints and dt
	Vector3f velocity_setpoints(-3.f, 0.f, -1.f);

	// Compute the number of steps required to reach desired value
	// The updateTrajectories is first called once to compute the total time
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ dt = 0.01;
	updateTrajectories(0.f, velocity_setpoints);
	ದಶಮಾಂಶ t123 = _trajectories[0].get_ಒಟ್ಟು_ಸಮಯ();
	ಪೂರ್ಣಾಂಕ nb_ಹೆಜ್ಜೆಗಳು = ceilf(t123 / dt);

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < nb_ಹೆಜ್ಜೆಗಳು; i++) {
		updateTrajectories(dt, velocity_setpoints);
	}

	// THEN: All the trajectories should have reached their
	// final state: desired velocity target and zero acceleration
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < 3; i++) {
		EXPECT_LE(fabsf(_trajectories[i].getCurrentVelocity() - velocity_setpoints(i)), 0.01f);
		EXPECT_LE(fabsf(_trajectories[i].getCurrentAcceleration()), 0.0001f);
	}
}

TEST_F(VelocitySmoothingTest, testZeroSetpoint)
{
	// GIVEN: A set of null initial conditions
	Vector3f a0(0.f, 0.f, 0.f);
	Vector3f v0(0.f, 0.f, 0.f);
	Vector3f x0(0.f, 0.f, 0.f);

	ಗಣ_Initial_Conditions(a0, v0, x0);

	// AND: Zero setpoints
	Vector3f velocity_setpoints(0.f, 0.f, 0.f);
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ dt = 0.01f;

	// WHEN: We run a few times the algorithm
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < 60; i++) {
		updateTrajectories(dt, velocity_setpoints);
	}

	// THEN: All the trajectories should still be zero
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < 3; i++) {
		EXPECT_FLOAT_EQ(_trajectories[i].getCurrentJerk(), 0.f);
		EXPECT_FLOAT_EQ(_trajectories[i].getCurrentAcceleration(), 0.f);
		EXPECT_FLOAT_EQ(_trajectories[i].getCurrentVelocity(), 0.f);
		EXPECT_FLOAT_EQ(_trajectories[i].getCurrentPosition(), 0.f);
	}
}
