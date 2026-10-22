#અન્તર્ભાવ <matrix/math.hpp>
#અન્તર્ભાવ <gtest/gtest.h>

using નામાકાશ matrix;

TEST(MatrixUnwrapTest, UnwrapFloats)
{
	અચલ દશાંશ M_TWO_PI_F = દશાંશ(M_PI_F * 2);

	દશાંશ unwrapped_angles[6] = {0.0, 0.25, 0.5, 0.75, 1.0, 1.25};
	દશાંશ wrapped_angles[6] = {0.0, 0.25, 0.5, -0.25, 0.0, 0.25};

	માટે (પૂર્ણાંક i = 0; i < 6; i++) {
		unwrapped_angles[i] *= M_TWO_PI_F;
		wrapped_angles[i] *= M_TWO_PI_F;
	}

	// positive unwrapping
	દશાંશ last_angle = wrapped_angles[0];

	માટે (પૂર્ણાંક i = 1; i < 6; i++) {
		last_angle = unwrap_pi(last_angle, wrapped_angles[i]);
		EXPECT_FLOAT_EQ(last_angle, unwrapped_angles[i]);
	}

	// negative unwrapping
	last_angle = -wrapped_angles[0];

	માટે (પૂર્ણાંક i = 1; i < 6; i++) {
		last_angle = unwrap_pi(last_angle, -wrapped_angles[i]);
		EXPECT_FLOAT_EQ(last_angle, -unwrapped_angles[i]);
	}
}

TEST(MatrixUnwrapTest, UnwrapDoubles)
{
	અચલ દ્વિગુણ M_TWO_PI = M_PI * 2;

	દ્વિગુણ unwrapped_angles[6] = {0.0, 0.25, 0.5, 0.75, 1.0, 1.25};
	દ્વિગુણ wrapped_angles[6] = {0.0, 0.25, 0.5, -0.25, 0.0, 0.25};

	માટે (પૂર્ણાંક i = 0; i < 6; i++) {
		unwrapped_angles[i] *= M_TWO_PI;
		wrapped_angles[i] *= M_TWO_PI;
	}

	// positive unwrapping
	દ્વિગુણ last_angle = wrapped_angles[0];

	માટે (પૂર્ણાંક i = 1; i < 6; i++) {
		last_angle = unwrap_pi(last_angle, wrapped_angles[i]);
		EXPECT_DOUBLE_EQ(last_angle, unwrapped_angles[i]);
	}

	// negative unwrapping
	last_angle = -wrapped_angles[0];

	માટે (પૂર્ણાંક i = 1; i < 6; i++) {
		last_angle = unwrap_pi(last_angle, -wrapped_angles[i]);
		EXPECT_DOUBLE_EQ(last_angle, -unwrapped_angles[i]);
	}
}
