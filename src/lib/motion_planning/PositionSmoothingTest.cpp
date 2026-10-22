#ਅਨ੍ਤਰ੍ਭਾਵ <gtest/gtest.h>

#ਅਨ੍ਤਰ੍ਭਾਵ <motion_planning/PositionSmoothing.hpp>

TEST(PositionSmoothingBasicTest, AllZeroCase)
{
	PositionSmoothing position_smoothing;
	PositionSmoothing::PositionSmoothingSetpoints ਬਾਹਰਲਾ;

	position_smoothing.generateSetpoints(
		Vector3f(),
	{Vector3f(), Vector3f(), Vector3f()},
	Vector3f(),
	0.f,
	ਝੂਠ,
	ਬਾਹਰਲਾ
	);

	EXPECT_EQ(ਬਾਹਰਲਾ.jerk, Vector3f());
	EXPECT_EQ(ਬਾਹਰਲਾ.acceleration, Vector3f());
	EXPECT_EQ(ਬਾਹਰਲਾ.velocity, Vector3f());
	EXPECT_EQ(ਬਾਹਰਲਾ.position, Vector3f());
	EXPECT_EQ(ਬਾਹਰਲਾ.unsmoothed_velocity, Vector3f());
}

ਸਥਿਰ constexpr ਦਸ਼ਮਲਵ MAX_JERK = 4.f;
ਸਥਿਰ constexpr ਦਸ਼ਮਲਵ MAX_ACCELERATION = 3.f;

ਸਥਿਰ constexpr ਦਸ਼ਮਲਵ MAX_ALLOWED_HOR_ERR = 2.f;
ਸਥਿਰ constexpr ਦਸ਼ਮਲਵ VERTICAL_ACCEPTANCE_RADIUS = 0.8f;
ਸਥਿਰ constexpr ਦਸ਼ਮਲਵ CRUISE_ਰਫਤਾਰ = 5.f;
ਸਥਿਰ constexpr ਦਸ਼ਮਲਵ MAX_VELOCITY = CRUISE_ਰਫਤਾਰ;

ਸਥਿਰ constexpr ਦਸ਼ਮਲਵ HORIZONTAL_TRAJECTORY_GAIN = 0.5f;
ਸਥਿਰ constexpr ਦਸ਼ਮਲਵ ਨਿਸ਼ਾਨਾ_ACCEPTANCE_RADIUS = 0.5f;


ਵਰਗ PositionSmoothingTest : ਜਨਤਕ ::testing::Test
{

ਜਨਤਕ:
	PositionSmoothing _position_smoothing;

	PositionSmoothingTest()
	{
		_position_smoothing.ਸੈੱਟ_Max_Jerk(MAX_JERK);
		_position_smoothing.ਸੈੱਟ_Max_Acceleration({MAX_ACCELERATION, MAX_ACCELERATION, MAX_ACCELERATION});
		_position_smoothing.ਸੈੱਟ_Max_Velocity({MAX_VELOCITY, MAX_VELOCITY, MAX_VELOCITY});
		_position_smoothing.ਸੈੱਟ_Max_Allowed_Horizontal_Error(MAX_ALLOWED_HOR_ERR);
		_position_smoothing.ਸੈੱਟ_Vertical_Acceptance_Radius(VERTICAL_ACCEPTANCE_RADIUS);
		_position_smoothing.ਸੈੱਟ_Cruise_ਰਫਤਾਰ(CRUISE_ਰਫਤਾਰ);
		_position_smoothing.ਸੈੱਟ_Horizontal_Trajectory_Gain(HORIZONTAL_TRAJECTORY_GAIN);
		_position_smoothing.ਸੈੱਟ_ਨਿਸ਼ਾਨਾ_Acceptance_Radius(ਨਿਸ਼ਾਨਾ_ACCEPTANCE_RADIUS);

		_position_smoothing.reset({0.f, 0.f, 0.f}, {0.f, 0.f, 0.f}, {0.f, 0.f, 0.f});
	}

	ਸਥਿਰ ਖਾਲੀ expectDynamicsLimitsRespected(ਅਚਲ PositionSmoothing::PositionSmoothingSetpoints &setpoints)
	{
		EXPECT_LE(fabsf(setpoints.velocity(0)), MAX_VELOCITY) << "Velocity in x too high\n";
		EXPECT_LE(fabsf(setpoints.velocity(1)), MAX_VELOCITY) << "Velocity in y too high\n";
		EXPECT_LE(fabsf(setpoints.velocity(2)), MAX_VELOCITY) << "Velocity in z too high\n";
		EXPECT_LE(fabsf(setpoints.acceleration(0)), MAX_ACCELERATION) << "Acceleration in x too high\n";
		EXPECT_LE(fabsf(setpoints.acceleration(1)), MAX_ACCELERATION) << "Acceleration in y too high\n";
		EXPECT_LE(fabsf(setpoints.acceleration(2)), MAX_ACCELERATION) << "Acceleration in z too high\n";
		EXPECT_LE(fabsf(setpoints.jerk(0)), MAX_JERK) << "Jerk in x too high\n";
		EXPECT_LE(fabsf(setpoints.jerk(1)), MAX_JERK) << "Jerk in y too high\n";
		EXPECT_LE(fabsf(setpoints.jerk(2)), MAX_JERK) << "Jerk in z too high\n";
	}
};


TEST_F(PositionSmoothingTest, reaches_ਨਿਸ਼ਾਨਾ_Position_Setpoint)
{
	ਅਚਲ ਪੂਰਨ N_ITER = 2000;
	ਅਚਲ ਦਸ਼ਮਲਵ DELTA_T = 0.02f;
	ਅਚਲ Vector3f INITIAL_POSITION{0.f, 0.f, 0.f};
	ਅਚਲ Vector3f FF_VELOCITY{0.f, 0.f, 0.f};
	ਅਚਲ Vector3f ਨਿਸ਼ਾਨਾ_2{12.f, 17.f, 8.f};

	Vector3f waypoints[3] = {INITIAL_POSITION, ਨਿਸ਼ਾਨਾ_2, ਨਿਸ਼ਾਨਾ_2};

	Vector3f position{0.f, 0.f, 0.f};

	PositionSmoothing::PositionSmoothingSetpoints ਬਾਹਰਲਾ;

	ਪੂਰਨ iteration = 0;

	ਲਈ (; iteration < N_ITER; iteration++) {
		_position_smoothing.generateSetpoints(
			position,
			waypoints,
			FF_VELOCITY,
			DELTA_T,
			ਝੂਠ,
			ਬਾਹਰਲਾ
		);
		position = ਬਾਹਰਲਾ.position;
		expectDynamicsLimitsRespected(ਬਾਹਰਲਾ);

		ਜੇ (position == ਨਿਸ਼ਾਨਾ_2) {
			ਲਿਖੋ("Converged in %d iterations\n", iteration);
			ਰੋਕੋ;
		}
	}

	EXPECT_EQ(ਨਿਸ਼ਾਨਾ_2, position);
	EXPECT_LT(iteration, N_ITER) << "Took too long to converge\n";
}


TEST_F(PositionSmoothingTest, reaches_ਨਿਸ਼ਾਨਾ_Velocity_Integration)
{
	ਅਚਲ ਪੂਰਨ N_ITER = 2000;
	ਅਚਲ ਦਸ਼ਮਲਵ DELTA_T = 0.02f;
	ਅਚਲ Vector3f INITIAL_POSITION{0.f, 0.f, 0.f};
	ਅਚਲ Vector3f FF_VELOCITY{0.f, 0.f, 0.f};
	ਅਚਲ Vector3f ਨਿਸ਼ਾਨਾ_2{12.f, 17.f, 8.f};

	Vector3f waypoints[3] = {INITIAL_POSITION, ਨਿਸ਼ਾਨਾ_2, ਨਿਸ਼ਾਨਾ_2};

	Vector3f position{0.f, 0.f, 0.f};

	PositionSmoothing::PositionSmoothingSetpoints ਬਾਹਰਲਾ;

	ਪੂਰਨ iteration = 0;

	ਲਈ (; iteration < N_ITER; iteration++) {
		_position_smoothing.generateSetpoints(
			position,
			waypoints,
			FF_VELOCITY,
			DELTA_T,
			ਝੂਠ,
			ਬਾਹਰਲਾ
		);
		position += ਬਾਹਰਲਾ.velocity * DELTA_T;
		expectDynamicsLimitsRespected(ਬਾਹਰਲਾ);


		ਜੇ (position == ਨਿਸ਼ਾਨਾ_2) {
			ਲਿਖੋ("Converged in %d iterations\n", iteration);
			ਰੋਕੋ;
		}
	}

	EXPECT_EQ(ਨਿਸ਼ਾਨਾ_2, position);
	EXPECT_LT(iteration, N_ITER) << "Took too long to converge\n";
}


TEST_F(PositionSmoothingTest, reaches_ਨਿਸ਼ਾਨਾ_Initial_Velocity)
{
	ਅਚਲ ਪੂਰਨ N_ITER = 20000;
	ਅਚਲ ਦਸ਼ਮਲਵ DELTA_T = 0.02f;
	ਅਚਲ Vector3f INITIAL_POSITION{0.f, 0.f, 0.f};
	ਅਚਲ Vector3f ਨਿਸ਼ਾਨਾ_2{12.f, 17.f, 8.f};
	ਅਚਲ Vector3f ਅਗਲਾ_ਨਿਸ਼ਾਨਾ_2{8.f, 12.f, 80.f};

	ਅਚਲ ਦਸ਼ਮਲਵ XY_ACC_RAD = 10.f;
	ਅਚਲ ਦਸ਼ਮਲਵ Z_ACC_RAD = 0.8f;


	Vector3f waypoints[3] = {INITIAL_POSITION, ਨਿਸ਼ਾਨਾ_2, ਨਿਸ਼ਾਨਾ_2};
	Vector3f ff_velocity{1.f, 0.1f, 0.3f};

	Vector3f position{0.f, 0.f, 0.f};

	PositionSmoothing::PositionSmoothingSetpoints ਬਾਹਰਲਾ;

	ਪੂਰਨ iteration = 0;

	ਲਈ (; iteration < N_ITER; iteration++) {
		_position_smoothing.generateSetpoints(
			position,
			waypoints,
			ff_velocity,
			DELTA_T,
			ਝੂਠ,
			ਬਾਹਰਲਾ
		);
		position = ਬਾਹਰਲਾ.position;
		ff_velocity = {0.f, 0.f, 0.f};
		expectDynamicsLimitsRespected(ਬਾਹਰਲਾ);

		ਜੇ (Vector2f(position.xy() - ਨਿਸ਼ਾਨਾ_2.xy()).norm() < XY_ACC_RAD && fabsf(position(2) - ਨਿਸ਼ਾਨਾ_2(2)) < Z_ACC_RAD) {
			ਲਿਖੋ("Converged in %d iterations\n", iteration);
			ਰੋਕੋ;
		}
	}

	EXPECT_LT(Vector2f(position.xy() - ਨਿਸ਼ਾਨਾ_2.xy()).norm(), XY_ACC_RAD);
	EXPECT_LT(fabsf(position(2) - ਨਿਸ਼ਾਨਾ_2(2)), Z_ACC_RAD);
	EXPECT_LT(iteration, N_ITER) << "Took too long to converge\n";
}


// Fly through a waypoint whose triplet never advances (e.g. an overshoot the navigator
// can't accept). The look-ahead point must not keep marching down the extended leg: the
// vehicle has to brake and come back to the target instead of drifting away forever.
TEST_F(PositionSmoothingTest, doesNotDriftPastUnreachedWaypoint)
{
	ਅਚਲ ਪੂਰਨ N_ITER = 3000; // 60 s at 50 Hz
	ਅਚਲ ਦਸ਼ਮਲਵ DELTA_T = 0.02f;

	ਅਚਲ Vector3f PREV{0.f, 0.f, 0.f};
	ਅਚਲ Vector3f ਨਿਸ਼ਾਨਾ_2{20.f, 0.f, 0.f};
	ਅਚਲ Vector3f ਅਗਲਾ_2{40.f, 2.f, 0.f}; // near-collinear next leg -> high corner speed -> clear fly-through

	// Triplet stays fixed for the whole run: the navigator never advances past TARGET.
	Vector3f waypoints[3] = {PREV, ਨਿਸ਼ਾਨਾ_2, ਅਗਲਾ_2};

	ਅਚਲ Vector3f u_leg = (ਨਿਸ਼ਾਨਾ_2 - PREV).unit_or_zero();
	ਅਚਲ ਦਸ਼ਮਲਵ leg_ਲੰਮਾਈ = (ਨਿਸ਼ਾਨਾ_2 - PREV).ਲੰਮਾਈ();

	Vector3f position{0.f, 0.f, 0.f};
	PositionSmoothing::PositionSmoothingSetpoints ਬਾਹਰਲਾ;

	ਤਾਰਕਿਕ reached_ਨਿਸ਼ਾਨਾ = ਝੂਠ;
	ਤਾਰਕਿਕ came_ਪਿੱਠ = ਝੂਠ;
	ਦਸ਼ਮਲਵ max_ਦੂਰੀ_past_ਨਿਸ਼ਾਨਾ = 0.f;

	ਲਈ (ਪੂਰਨ i = 0; i < N_ITER; i++) {
		_position_smoothing.generateSetpoints(position, waypoints, Vector3f{}, DELTA_T, ਝੂਠ, ਬਾਹਰਲਾ);
		position = ਬਾਹਰਲਾ.position;
		expectDynamicsLimitsRespected(ਬਾਹਰਲਾ);

		ਅਚਲ ਦਸ਼ਮਲਵ along_track = Vector3f(position - PREV) * u_leg;

		ਜੇ (Vector3f(position - ਨਿਸ਼ਾਨਾ_2).ਲੰਮਾਈ() < 1.f) {
			reached_ਨਿਸ਼ਾਨਾ = ਸੱਚ;
		}

		ਜੇ (reached_ਨਿਸ਼ਾਨਾ) {
			max_ਦੂਰੀ_past_ਨਿਸ਼ਾਨਾ = fmaxf(max_ਦੂਰੀ_past_ਨਿਸ਼ਾਨਾ, along_track - leg_ਲੰਮਾਈ);

			// Once the vehicle has gone past the target, the fix must turn it around.
			ਜੇ (along_track > leg_ਲੰਮਾਈ + 1.f && ਬਾਹਰਲਾ.velocity * u_leg < -0.1f) {
				came_ਪਿੱਠ = ਸੱਚ;
			}
		}
	}

	EXPECT_TRUE(reached_ਨਿਸ਼ਾਨਾ) << "Vehicle never reached the target waypoint\n";
	EXPECT_TRUE(came_ਪਿੱਠ) << "Vehicle never turned back toward the unreached waypoint (it drifted away)\n";
	// Without the fix the look-ahead marches down the extended leg and this grows unbounded.
	EXPECT_LT(max_ਦੂਰੀ_past_ਨਿਸ਼ਾਨਾ, 10.f) << "Vehicle drifted too far past the unreached waypoint\n";
}

ਵਰਗ PositionSmoothingStraightLegTest : ਜਨਤਕ ::testing::Test
{
ਜਨਤਕ:
	// A descending leg: 100 m horizontal, 50 m down (NED), 6 m/s XY, 1.5 m/s Z
	ਸਥਿਰ constexpr ਦਸ਼ਮਲਵ XY_ਰਫਤਾਰ = 6.f;
	ਸਥਿਰ constexpr ਦਸ਼ਮਲਵ Z_ਰਫਤਾਰ = 1.5f;
	ਅਚਲ Vector3f ਸ਼ੁਰੂ_2{0.f, 0.f, 0.f};
	ਅਚਲ Vector3f ਨਿਸ਼ਾਨਾ_2{100.f, 0.f, 50.f};

	PositionSmoothing _position_smoothing;

	PositionSmoothingStraightLegTest()
	{
		_position_smoothing.ਸੈੱਟ_Max_Jerk(MAX_JERK);
		_position_smoothing.ਸੈੱਟ_Max_Acceleration({MAX_ACCELERATION, MAX_ACCELERATION, MAX_ACCELERATION});
		_position_smoothing.ਸੈੱਟ_Max_Velocity({XY_ਰਫਤਾਰ, XY_ਰਫਤਾਰ, Z_ਰਫਤਾਰ});
		_position_smoothing.ਸੈੱਟ_Max_Allowed_Horizontal_Error(MAX_ALLOWED_HOR_ERR);
		_position_smoothing.ਸੈੱਟ_Vertical_Acceptance_Radius(VERTICAL_ACCEPTANCE_RADIUS);
		_position_smoothing.ਸੈੱਟ_Cruise_ਰਫਤਾਰ(XY_ਰਫਤਾਰ);
		_position_smoothing.ਸੈੱਟ_Horizontal_Trajectory_Gain(HORIZONTAL_TRAJECTORY_GAIN);
		_position_smoothing.ਸੈੱਟ_ਨਿਸ਼ਾਨਾ_Acceptance_Radius(ਨਿਸ਼ਾਨਾ_ACCEPTANCE_RADIUS);

		_position_smoothing.reset({0.f, 0.f, 0.f}, {0.f, 0.f, 0.f}, ਸ਼ੁਰੂ_2);
	}

	PositionSmoothing::PositionSmoothingSetpoints ਪਹਿਲਾ_Setpoint()
	{
		Vector3f waypoints[3] = {ਸ਼ੁਰੂ_2, ਨਿਸ਼ਾਨਾ_2, ਨਿਸ਼ਾਨਾ_2};
		PositionSmoothing::PositionSmoothingSetpoints ਬਾਹਰਲਾ;
		_position_smoothing.generateSetpoints(ਸ਼ੁਰੂ_2, waypoints, Vector3f{0.f, 0.f, 0.f}, 0.02f, ਝੂਠ, ਬਾਹਰਲਾ);
		ਵਾਪਸ ਬਾਹਰਲਾ;
	}

	// Fly the leg and return the largest vertical distance from the straight line
	ਦਸ਼ਮਲਵ flyLegMaxAltitudeError()
	{
		Vector3f waypoints[3] = {ਸ਼ੁਰੂ_2, ਨਿਸ਼ਾਨਾ_2, ਨਿਸ਼ਾਨਾ_2};
		Vector3f position = ਸ਼ੁਰੂ_2;
		PositionSmoothing::PositionSmoothingSetpoints ਬਾਹਰਲਾ;
		ਅਚਲ ਦਸ਼ਮਲਵ ਢਲਾਨ = (ਨਿਸ਼ਾਨਾ_2(2) - ਸ਼ੁਰੂ_2(2)) / (ਨਿਸ਼ਾਨਾ_2 - ਸ਼ੁਰੂ_2).xy().norm();
		ਦਸ਼ਮਲਵ max_error = 0.f;

		ਲਈ (ਪੂਰਨ i = 0; i < 5000 && (ਨਿਸ਼ਾਨਾ_2 - position).norm() > 0.1f; i++) {
			_position_smoothing.generateSetpoints(position, waypoints, Vector3f{0.f, 0.f, 0.f}, 0.02f, ਝੂਠ, ਬਾਹਰਲਾ);
			position = ਬਾਹਰਲਾ.position;
			ਅਚਲ ਦਸ਼ਮਲਵ z_on_ਸਤਰ = ਸ਼ੁਰੂ_2(2) + ਢਲਾਨ * (position - ਸ਼ੁਰੂ_2).xy().norm();
			max_error = fmaxf(max_error, fabsf(position(2) - z_on_ਸਤਰ));
		}

		EXPECT_LT((ਨਿਸ਼ਾਨਾ_2 - position).norm(), 0.1f) << "Did not reach the target\n";
		ਵਾਪਸ max_error;
	}
};

TEST_F(PositionSmoothingStraightLegTest, velocityPointsAlongTheLeg)
{
	ਅਚਲ Vector3f vel = ਪਹਿਲਾ_Setpoint().unsmoothed_velocity;

	// Parallel to the leg: vz / |vxy| equals the leg slope, the slower Z axis limits XY
	EXPECT_NEAR(vel(2) / vel.xy().norm(), 0.5f, 1e-3f);
	EXPECT_NEAR(vel(2), Z_ਰਫਤਾਰ, 1e-3f);
	EXPECT_NEAR(vel.xy().norm(), 2.f * Z_ਰਫਤਾਰ, 1e-3f);
}

TEST_F(PositionSmoothingStraightLegTest, fliesAStraightLeg)
{
	EXPECT_LT(flyLegMaxAltitudeError(), 1.f);
}
