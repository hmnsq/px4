#अन्तर्भाव <matrix/math.hpp>
#अन्तर्भाव <gtest/gtest.h>

using नामाकाश matrix;

TEST(MatrixUnwrapTest, UnwrapFloats)
{
	स्थिरांक दशांश M_TWO_PI_F = दशांश(M_PI_F * 2);

	दशांश unwrapped_angles[6] = {0.0, 0.25, 0.5, 0.75, 1.0, 1.25};
	दशांश wrapped_angles[6] = {0.0, 0.25, 0.5, -0.25, 0.0, 0.25};

	साठी (पूर्णांक i = 0; i < 6; i++) {
		unwrapped_angles[i] *= M_TWO_PI_F;
		wrapped_angles[i] *= M_TWO_PI_F;
	}

	// positive unwrapping
	दशांश last_angle = wrapped_angles[0];

	साठी (पूर्णांक i = 1; i < 6; i++) {
		last_angle = unwrap_pi(last_angle, wrapped_angles[i]);
		EXPECT_FLOAT_EQ(last_angle, unwrapped_angles[i]);
	}

	// negative unwrapping
	last_angle = -wrapped_angles[0];

	साठी (पूर्णांक i = 1; i < 6; i++) {
		last_angle = unwrap_pi(last_angle, -wrapped_angles[i]);
		EXPECT_FLOAT_EQ(last_angle, -unwrapped_angles[i]);
	}
}

TEST(MatrixUnwrapTest, UnwrapDoubles)
{
	स्थिरांक द्विगुण M_TWO_PI = M_PI * 2;

	द्विगुण unwrapped_angles[6] = {0.0, 0.25, 0.5, 0.75, 1.0, 1.25};
	द्विगुण wrapped_angles[6] = {0.0, 0.25, 0.5, -0.25, 0.0, 0.25};

	साठी (पूर्णांक i = 0; i < 6; i++) {
		unwrapped_angles[i] *= M_TWO_PI;
		wrapped_angles[i] *= M_TWO_PI;
	}

	// positive unwrapping
	द्विगुण last_angle = wrapped_angles[0];

	साठी (पूर्णांक i = 1; i < 6; i++) {
		last_angle = unwrap_pi(last_angle, wrapped_angles[i]);
		EXPECT_DOUBLE_EQ(last_angle, unwrapped_angles[i]);
	}

	// negative unwrapping
	last_angle = -wrapped_angles[0];

	साठी (पूर्णांक i = 1; i < 6; i++) {
		last_angle = unwrap_pi(last_angle, -wrapped_angles[i]);
		EXPECT_DOUBLE_EQ(last_angle, -unwrapped_angles[i]);
	}
}
