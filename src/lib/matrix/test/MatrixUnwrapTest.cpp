#அந்தர்பாவ <matrix/math.hpp>
#அந்தர்பாவ <gtest/gtest.h>

using நாமாகாஶ matrix;

TEST(MatrixUnwrapTest, UnwrapFloats)
{
	மாறிலி தசம M_TWO_PI_F = தசம(M_PI_F * 2);

	தசம unwrapped_angles[6] = {0.0, 0.25, 0.5, 0.75, 1.0, 1.25};
	தசம wrapped_angles[6] = {0.0, 0.25, 0.5, -0.25, 0.0, 0.25};

	க்கு (முழு i = 0; i < 6; i++) {
		unwrapped_angles[i] *= M_TWO_PI_F;
		wrapped_angles[i] *= M_TWO_PI_F;
	}

	// positive unwrapping
	தசம last_angle = wrapped_angles[0];

	க்கு (முழு i = 1; i < 6; i++) {
		last_angle = unwrap_pi(last_angle, wrapped_angles[i]);
		EXPECT_FLOAT_EQ(last_angle, unwrapped_angles[i]);
	}

	// negative unwrapping
	last_angle = -wrapped_angles[0];

	க்கு (முழு i = 1; i < 6; i++) {
		last_angle = unwrap_pi(last_angle, -wrapped_angles[i]);
		EXPECT_FLOAT_EQ(last_angle, -unwrapped_angles[i]);
	}
}

TEST(MatrixUnwrapTest, UnwrapDoubles)
{
	மாறிலி இரட்டை M_TWO_PI = M_PI * 2;

	இரட்டை unwrapped_angles[6] = {0.0, 0.25, 0.5, 0.75, 1.0, 1.25};
	இரட்டை wrapped_angles[6] = {0.0, 0.25, 0.5, -0.25, 0.0, 0.25};

	க்கு (முழு i = 0; i < 6; i++) {
		unwrapped_angles[i] *= M_TWO_PI;
		wrapped_angles[i] *= M_TWO_PI;
	}

	// positive unwrapping
	இரட்டை last_angle = wrapped_angles[0];

	க்கு (முழு i = 1; i < 6; i++) {
		last_angle = unwrap_pi(last_angle, wrapped_angles[i]);
		EXPECT_DOUBLE_EQ(last_angle, unwrapped_angles[i]);
	}

	// negative unwrapping
	last_angle = -wrapped_angles[0];

	க்கு (முழு i = 1; i < 6; i++) {
		last_angle = unwrap_pi(last_angle, -wrapped_angles[i]);
		EXPECT_DOUBLE_EQ(last_angle, -unwrapped_angles[i]);
	}
}
