#അന്തര്ഭാവ <matrix/math.hpp>
#അന്തര്ഭാവ <gtest/gtest.h>

using നാമാകാശ matrix;

TEST(MatrixUnwrapTest, UnwrapFloats)
{
	സ്ഥിരം ദശാംശം M_TWO_PI_F = ദശാംശം(M_PI_F * 2);

	ദശാംശം unwrapped_angles[6] = {0.0, 0.25, 0.5, 0.75, 1.0, 1.25};
	ദശാംശം wrapped_angles[6] = {0.0, 0.25, 0.5, -0.25, 0.0, 0.25};

	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < 6; i++) {
		unwrapped_angles[i] *= M_TWO_PI_F;
		wrapped_angles[i] *= M_TWO_PI_F;
	}

	// positive unwrapping
	ദശാംശം last_angle = wrapped_angles[0];

	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 1; i < 6; i++) {
		last_angle = unwrap_pi(last_angle, wrapped_angles[i]);
		EXPECT_FLOAT_EQ(last_angle, unwrapped_angles[i]);
	}

	// negative unwrapping
	last_angle = -wrapped_angles[0];

	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 1; i < 6; i++) {
		last_angle = unwrap_pi(last_angle, -wrapped_angles[i]);
		EXPECT_FLOAT_EQ(last_angle, -unwrapped_angles[i]);
	}
}

TEST(MatrixUnwrapTest, UnwrapDoubles)
{
	സ്ഥിരം ഇരട്ട M_TWO_PI = M_PI * 2;

	ഇരട്ട unwrapped_angles[6] = {0.0, 0.25, 0.5, 0.75, 1.0, 1.25};
	ഇരട്ട wrapped_angles[6] = {0.0, 0.25, 0.5, -0.25, 0.0, 0.25};

	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < 6; i++) {
		unwrapped_angles[i] *= M_TWO_PI;
		wrapped_angles[i] *= M_TWO_PI;
	}

	// positive unwrapping
	ഇരട്ട last_angle = wrapped_angles[0];

	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 1; i < 6; i++) {
		last_angle = unwrap_pi(last_angle, wrapped_angles[i]);
		EXPECT_DOUBLE_EQ(last_angle, unwrapped_angles[i]);
	}

	// negative unwrapping
	last_angle = -wrapped_angles[0];

	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 1; i < 6; i++) {
		last_angle = unwrap_pi(last_angle, -wrapped_angles[i]);
		EXPECT_DOUBLE_EQ(last_angle, -unwrapped_angles[i]);
	}
}
