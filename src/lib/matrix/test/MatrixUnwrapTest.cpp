#ಅನ್ತರ್ಭಾವ <matrix/math.hpp>
#ಅನ್ತರ್ಭಾವ <gtest/gtest.h>

using ನಾಮಾಕಾಶ matrix;

TEST(MatrixUnwrapTest, UnwrapFloats)
{
	ಧ್ರುವ ಭಿನ್ನ M_TWO_PI_F = ಭಿನ್ನ(M_PI_F * 2);

	ಭಿನ್ನ unwrapped_angles[6] = {0.0, 0.25, 0.5, 0.75, 1.0, 1.25};
	ಭಿನ್ನ wrapped_angles[6] = {0.0, 0.25, 0.5, -0.25, 0.0, 0.25};

	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < 6; i++) {
		unwrapped_angles[i] *= M_TWO_PI_F;
		wrapped_angles[i] *= M_TWO_PI_F;
	}

	// positive unwrapping
	ಭಿನ್ನ last_angle = wrapped_angles[0];

	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 1; i < 6; i++) {
		last_angle = unwrap_pi(last_angle, wrapped_angles[i]);
		EXPECT_FLOAT_EQ(last_angle, unwrapped_angles[i]);
	}

	// negative unwrapping
	last_angle = -wrapped_angles[0];

	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 1; i < 6; i++) {
		last_angle = unwrap_pi(last_angle, -wrapped_angles[i]);
		EXPECT_FLOAT_EQ(last_angle, -unwrapped_angles[i]);
	}
}

TEST(MatrixUnwrapTest, UnwrapDoubles)
{
	ಧ್ರುವ ದ್ವಿಗುಣ M_TWO_PI = M_PI * 2;

	ದ್ವಿಗುಣ unwrapped_angles[6] = {0.0, 0.25, 0.5, 0.75, 1.0, 1.25};
	ದ್ವಿಗುಣ wrapped_angles[6] = {0.0, 0.25, 0.5, -0.25, 0.0, 0.25};

	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < 6; i++) {
		unwrapped_angles[i] *= M_TWO_PI;
		wrapped_angles[i] *= M_TWO_PI;
	}

	// positive unwrapping
	ದ್ವಿಗುಣ last_angle = wrapped_angles[0];

	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 1; i < 6; i++) {
		last_angle = unwrap_pi(last_angle, wrapped_angles[i]);
		EXPECT_DOUBLE_EQ(last_angle, unwrapped_angles[i]);
	}

	// negative unwrapping
	last_angle = -wrapped_angles[0];

	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 1; i < 6; i++) {
		last_angle = unwrap_pi(last_angle, -wrapped_angles[i]);
		EXPECT_DOUBLE_EQ(last_angle, -unwrapped_angles[i]);
	}
}
