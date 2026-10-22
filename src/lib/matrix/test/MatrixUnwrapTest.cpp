#شامل <matrix/math.hpp>
#شامل <gtest/gtest.h>

using فضاینام matrix;

TEST(MatrixUnwrapTest, UnwrapFloats)
{
	ثابت اعشاری M_TWO_PI_F = اعشاری(M_PI_F * 2);

	اعشاری unwrapped_angles[6] = {0.0, 0.25, 0.5, 0.75, 1.0, 1.25};
	اعشاری wrapped_angles[6] = {0.0, 0.25, 0.5, -0.25, 0.0, 0.25};

	برای (صحیح i = 0; i < 6; i++) {
		unwrapped_angles[i] *= M_TWO_PI_F;
		wrapped_angles[i] *= M_TWO_PI_F;
	}

	// positive unwrapping
	اعشاری last_angle = wrapped_angles[0];

	برای (صحیح i = 1; i < 6; i++) {
		last_angle = unwrap_pi(last_angle, wrapped_angles[i]);
		EXPECT_FLOAT_EQ(last_angle, unwrapped_angles[i]);
	}

	// negative unwrapping
	last_angle = -wrapped_angles[0];

	برای (صحیح i = 1; i < 6; i++) {
		last_angle = unwrap_pi(last_angle, -wrapped_angles[i]);
		EXPECT_FLOAT_EQ(last_angle, -unwrapped_angles[i]);
	}
}

TEST(MatrixUnwrapTest, UnwrapDoubles)
{
	ثابت دوگانه M_TWO_PI = M_PI * 2;

	دوگانه unwrapped_angles[6] = {0.0, 0.25, 0.5, 0.75, 1.0, 1.25};
	دوگانه wrapped_angles[6] = {0.0, 0.25, 0.5, -0.25, 0.0, 0.25};

	برای (صحیح i = 0; i < 6; i++) {
		unwrapped_angles[i] *= M_TWO_PI;
		wrapped_angles[i] *= M_TWO_PI;
	}

	// positive unwrapping
	دوگانه last_angle = wrapped_angles[0];

	برای (صحیح i = 1; i < 6; i++) {
		last_angle = unwrap_pi(last_angle, wrapped_angles[i]);
		EXPECT_DOUBLE_EQ(last_angle, unwrapped_angles[i]);
	}

	// negative unwrapping
	last_angle = -wrapped_angles[0];

	برای (صحیح i = 1; i < 6; i++) {
		last_angle = unwrap_pi(last_angle, -wrapped_angles[i]);
		EXPECT_DOUBLE_EQ(last_angle, -unwrapped_angles[i]);
	}
}
