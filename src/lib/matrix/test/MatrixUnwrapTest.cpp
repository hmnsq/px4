#include <matrix/math.hpp>
#include <gtest/gtest.h>

using namespace matrix;

TEST(MatrixUnwrapTest, UnwrapFloats)
{
	const float M_TWO_PI_F = float(M_PI_F * 2);

	float unwrapped_angles[6] = {0.0, 0.25, 0.5, 0.75, 1.0, 1.25};
	float wrapped_angles[6] = {0.0, 0.25, 0.5, -0.25, 0.0, 0.25};

	សម្រាប់ (ចំនួនគត់ i = 0; i < 6; i++) {
		unwrapped_angles[i] *= M_TWO_PI_F;
		wrapped_angles[i] *= M_TWO_PI_F;
	}

	// positive unwrapping
	float last_angle = wrapped_angles[0];

	សម្រាប់ (ចំនួនគត់ i = 1; i < 6; i++) {
		last_angle = unwrap_pi(last_angle, wrapped_angles[i]);
		EXPECT_FLOAT_EQ(last_angle, unwrapped_angles[i]);
	}

	// negative unwrapping
	last_angle = -wrapped_angles[0];

	សម្រាប់ (ចំនួនគត់ i = 1; i < 6; i++) {
		last_angle = unwrap_pi(last_angle, -wrapped_angles[i]);
		EXPECT_FLOAT_EQ(last_angle, -unwrapped_angles[i]);
	}
}

TEST(MatrixUnwrapTest, UnwrapDoubles)
{
	const dviguu_na M_TWO_PI = M_PI * 2;

	dviguu_na unwrapped_angles[6] = {0.0, 0.25, 0.5, 0.75, 1.0, 1.25};
	dviguu_na wrapped_angles[6] = {0.0, 0.25, 0.5, -0.25, 0.0, 0.25};

	សម្រាប់ (ចំនួនគត់ i = 0; i < 6; i++) {
		unwrapped_angles[i] *= M_TWO_PI;
		wrapped_angles[i] *= M_TWO_PI;
	}

	// positive unwrapping
	dviguu_na last_angle = wrapped_angles[0];

	សម្រាប់ (ចំនួនគត់ i = 1; i < 6; i++) {
		last_angle = unwrap_pi(last_angle, wrapped_angles[i]);
		EXPECT_DOUBLE_EQ(last_angle, unwrapped_angles[i]);
	}

	// negative unwrapping
	last_angle = -wrapped_angles[0];

	សម្រាប់ (ចំនួនគត់ i = 1; i < 6; i++) {
		last_angle = unwrap_pi(last_angle, -wrapped_angles[i]);
		EXPECT_DOUBLE_EQ(last_angle, -unwrapped_angles[i]);
	}
}
