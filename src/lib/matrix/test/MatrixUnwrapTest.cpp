#অন্তর্ভাব <matrix/math.hpp>
#অন্তর্ভাব <gtest/gtest.h>

using নামাকাশ matrix;

TEST(MatrixUnwrapTest, UnwrapFloats)
{
	ধ্রুবক দশমিক M_TWO_PI_F = দশমিক(M_PI_F * 2);

	দশমিক unwrapped_angles[6] = {0.0, 0.25, 0.5, 0.75, 1.0, 1.25};
	দশমিক wrapped_angles[6] = {0.0, 0.25, 0.5, -0.25, 0.0, 0.25};

	জন্য (পূর্ণসংখ্যা i = 0; i < 6; i++) {
		unwrapped_angles[i] *= M_TWO_PI_F;
		wrapped_angles[i] *= M_TWO_PI_F;
	}

	// positive unwrapping
	দশমিক last_angle = wrapped_angles[0];

	জন্য (পূর্ণসংখ্যা i = 1; i < 6; i++) {
		last_angle = unwrap_pi(last_angle, wrapped_angles[i]);
		EXPECT_FLOAT_EQ(last_angle, unwrapped_angles[i]);
	}

	// negative unwrapping
	last_angle = -wrapped_angles[0];

	জন্য (পূর্ণসংখ্যা i = 1; i < 6; i++) {
		last_angle = unwrap_pi(last_angle, -wrapped_angles[i]);
		EXPECT_FLOAT_EQ(last_angle, -unwrapped_angles[i]);
	}
}

TEST(MatrixUnwrapTest, UnwrapDoubles)
{
	ধ্রুবক দ্বিগুণ M_TWO_PI = M_PI * 2;

	দ্বিগুণ unwrapped_angles[6] = {0.0, 0.25, 0.5, 0.75, 1.0, 1.25};
	দ্বিগুণ wrapped_angles[6] = {0.0, 0.25, 0.5, -0.25, 0.0, 0.25};

	জন্য (পূর্ণসংখ্যা i = 0; i < 6; i++) {
		unwrapped_angles[i] *= M_TWO_PI;
		wrapped_angles[i] *= M_TWO_PI;
	}

	// positive unwrapping
	দ্বিগুণ last_angle = wrapped_angles[0];

	জন্য (পূর্ণসংখ্যা i = 1; i < 6; i++) {
		last_angle = unwrap_pi(last_angle, wrapped_angles[i]);
		EXPECT_DOUBLE_EQ(last_angle, unwrapped_angles[i]);
	}

	// negative unwrapping
	last_angle = -wrapped_angles[0];

	জন্য (পূর্ণসংখ্যা i = 1; i < 6; i++) {
		last_angle = unwrap_pi(last_angle, -wrapped_angles[i]);
		EXPECT_DOUBLE_EQ(last_angle, -unwrapped_angles[i]);
	}
}
