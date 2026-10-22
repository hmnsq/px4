#شامل <matrix/math.hpp>
#شامل <gtest/gtest.h>

استعمال نامخلا میٹرکس;

TEST(میٹرکس_Unwrap_Test, UnwrapFloats)
{
	ثابت اعشاری M_TWO_PI_F = اعشاری(M_PI_F * 2);

	اعشاری unwrapped_angles[6] = {0.0, 0.25, 0.5, 0.75, 1.0, 1.25};
	اعشاری wrapped_angles[6] = {0.0, 0.25, 0.5, -0.25, 0.0, 0.25};

	برائے (صحیح i = 0; i < 6; i++) {
		unwrapped_angles[i] *= M_TWO_PI_F;
		wrapped_angles[i] *= M_TWO_PI_F;
	}

	// positive unwrapping
	اعشاری آخری_angle = wrapped_angles[0];

	برائے (صحیح i = 1; i < 6; i++) {
		آخری_angle = unwrap_pi(آخری_angle, wrapped_angles[i]);
		EXPECT_FLOAT_EQ(آخری_angle, unwrapped_angles[i]);
	}

	// negative unwrapping
	آخری_angle = -wrapped_angles[0];

	برائے (صحیح i = 1; i < 6; i++) {
		آخری_angle = unwrap_pi(آخری_angle, -wrapped_angles[i]);
		EXPECT_FLOAT_EQ(آخری_angle, -unwrapped_angles[i]);
	}
}

TEST(میٹرکس_Unwrap_Test, UnwrapDoubles)
{
	ثابت دوہرا M_TWO_PI = M_PI * 2;

	دوہرا unwrapped_angles[6] = {0.0, 0.25, 0.5, 0.75, 1.0, 1.25};
	دوہرا wrapped_angles[6] = {0.0, 0.25, 0.5, -0.25, 0.0, 0.25};

	برائے (صحیح i = 0; i < 6; i++) {
		unwrapped_angles[i] *= M_TWO_PI;
		wrapped_angles[i] *= M_TWO_PI;
	}

	// positive unwrapping
	دوہرا آخری_angle = wrapped_angles[0];

	برائے (صحیح i = 1; i < 6; i++) {
		آخری_angle = unwrap_pi(آخری_angle, wrapped_angles[i]);
		EXPECT_DOUBLE_EQ(آخری_angle, unwrapped_angles[i]);
	}

	// negative unwrapping
	آخری_angle = -wrapped_angles[0];

	برائے (صحیح i = 1; i < 6; i++) {
		آخری_angle = unwrap_pi(آخری_angle, -wrapped_angles[i]);
		EXPECT_DOUBLE_EQ(آخری_angle, -unwrapped_angles[i]);
	}
}
