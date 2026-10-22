#ଅନ୍ତର୍ଭାଵ <matrix/math.hpp>
#ଅନ୍ତର୍ଭାଵ <gtest/gtest.h>

using ନାମାକାଶ matrix;

TEST(MatrixUnwrapTest, UnwrapFloats)
{
	ଅଚଳ ଦଶମିକ M_TWO_PI_F = ଦଶମିକ(M_PI_F * 2);

	ଦଶମିକ unwrapped_angles[6] = {0.0, 0.25, 0.5, 0.75, 1.0, 1.25};
	ଦଶମିକ wrapped_angles[6] = {0.0, 0.25, 0.5, -0.25, 0.0, 0.25};

	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < 6; i++) {
		unwrapped_angles[i] *= M_TWO_PI_F;
		wrapped_angles[i] *= M_TWO_PI_F;
	}

	// positive unwrapping
	ଦଶମିକ last_angle = wrapped_angles[0];

	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 1; i < 6; i++) {
		last_angle = unwrap_pi(last_angle, wrapped_angles[i]);
		EXPECT_FLOAT_EQ(last_angle, unwrapped_angles[i]);
	}

	// negative unwrapping
	last_angle = -wrapped_angles[0];

	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 1; i < 6; i++) {
		last_angle = unwrap_pi(last_angle, -wrapped_angles[i]);
		EXPECT_FLOAT_EQ(last_angle, -unwrapped_angles[i]);
	}
}

TEST(MatrixUnwrapTest, UnwrapDoubles)
{
	ଅଚଳ ଦ୍ୱିଗୁଣ M_TWO_PI = M_PI * 2;

	ଦ୍ୱିଗୁଣ unwrapped_angles[6] = {0.0, 0.25, 0.5, 0.75, 1.0, 1.25};
	ଦ୍ୱିଗୁଣ wrapped_angles[6] = {0.0, 0.25, 0.5, -0.25, 0.0, 0.25};

	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < 6; i++) {
		unwrapped_angles[i] *= M_TWO_PI;
		wrapped_angles[i] *= M_TWO_PI;
	}

	// positive unwrapping
	ଦ୍ୱିଗୁଣ last_angle = wrapped_angles[0];

	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 1; i < 6; i++) {
		last_angle = unwrap_pi(last_angle, wrapped_angles[i]);
		EXPECT_DOUBLE_EQ(last_angle, unwrapped_angles[i]);
	}

	// negative unwrapping
	last_angle = -wrapped_angles[0];

	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 1; i < 6; i++) {
		last_angle = unwrap_pi(last_angle, -wrapped_angles[i]);
		EXPECT_DOUBLE_EQ(last_angle, -unwrapped_angles[i]);
	}
}
