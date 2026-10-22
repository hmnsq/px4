#ضمن <matrix/math.hpp>
#ضمن <gtest/gtest.h>

استعمال نطاق ميٽرڪس;

TEST(ميٽرڪس_Unwrap_Test, UnwrapFloats)
{
	ثابت عائم M_TWO_PI_F = عائم(M_PI_F * 2);

	عائم unwrapped_angles[6] = {0.0, 0.25, 0.5, 0.75, 1.0, 1.25};
	عائم wrapped_angles[6] = {0.0, 0.25, 0.5, -0.25, 0.0, 0.25};

	لكل (صحيح i = 0; i < 6; i++) {
		unwrapped_angles[i] *= M_TWO_PI_F;
		wrapped_angles[i] *= M_TWO_PI_F;
	}

	// positive unwrapping
	عائم آخري_angle = wrapped_angles[0];

	لكل (صحيح i = 1; i < 6; i++) {
		آخري_angle = unwrap_pi(آخري_angle, wrapped_angles[i]);
		EXPECT_FLOAT_EQ(آخري_angle, unwrapped_angles[i]);
	}

	// negative unwrapping
	آخري_angle = -wrapped_angles[0];

	لكل (صحيح i = 1; i < 6; i++) {
		آخري_angle = unwrap_pi(آخري_angle, -wrapped_angles[i]);
		EXPECT_FLOAT_EQ(آخري_angle, -unwrapped_angles[i]);
	}
}

TEST(ميٽرڪس_Unwrap_Test, UnwrapDoubles)
{
	ثابت مزدوج M_TWO_PI = M_PI * 2;

	مزدوج unwrapped_angles[6] = {0.0, 0.25, 0.5, 0.75, 1.0, 1.25};
	مزدوج wrapped_angles[6] = {0.0, 0.25, 0.5, -0.25, 0.0, 0.25};

	لكل (صحيح i = 0; i < 6; i++) {
		unwrapped_angles[i] *= M_TWO_PI;
		wrapped_angles[i] *= M_TWO_PI;
	}

	// positive unwrapping
	مزدوج آخري_angle = wrapped_angles[0];

	لكل (صحيح i = 1; i < 6; i++) {
		آخري_angle = unwrap_pi(آخري_angle, wrapped_angles[i]);
		EXPECT_DOUBLE_EQ(آخري_angle, unwrapped_angles[i]);
	}

	// negative unwrapping
	آخري_angle = -wrapped_angles[0];

	لكل (صحيح i = 1; i < 6; i++) {
		آخري_angle = unwrap_pi(آخري_angle, -wrapped_angles[i]);
		EXPECT_DOUBLE_EQ(آخري_angle, -unwrapped_angles[i]);
	}
}
