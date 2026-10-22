#ಅನ್ತರ್ಭಾವ <matrix/math.hpp>
#ಅನ್ತರ್ಭಾವ <gtest/gtest.h>

ಬಳಸಿ ನಾಮಾಕಾಶ ಮಾತೃಕೆ;

TEST(ಮಾತೃಕೆ_Unwrap_Test, UnwrapFloats)
{
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ M_TWO_PI_F = ದಶಮಾಂಶ(M_PI_F * 2);

	ದಶಮಾಂಶ unwrapped_angles[6] = {0.0, 0.25, 0.5, 0.75, 1.0, 1.25};
	ದಶಮಾಂಶ wrapped_angles[6] = {0.0, 0.25, 0.5, -0.25, 0.0, 0.25};

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < 6; i++) {
		unwrapped_angles[i] *= M_TWO_PI_F;
		wrapped_angles[i] *= M_TWO_PI_F;
	}

	// positive unwrapping
	ದಶಮಾಂಶ ಕೊನೆಯ_angle = wrapped_angles[0];

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 1; i < 6; i++) {
		ಕೊನೆಯ_angle = unwrap_pi(ಕೊನೆಯ_angle, wrapped_angles[i]);
		EXPECT_FLOAT_EQ(ಕೊನೆಯ_angle, unwrapped_angles[i]);
	}

	// negative unwrapping
	ಕೊನೆಯ_angle = -wrapped_angles[0];

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 1; i < 6; i++) {
		ಕೊನೆಯ_angle = unwrap_pi(ಕೊನೆಯ_angle, -wrapped_angles[i]);
		EXPECT_FLOAT_EQ(ಕೊನೆಯ_angle, -unwrapped_angles[i]);
	}
}

TEST(ಮಾತೃಕೆ_Unwrap_Test, UnwrapDoubles)
{
	ಸ್ಥಿರಾಂಕ ದ್ವಿಗುಣ M_TWO_PI = M_PI * 2;

	ದ್ವಿಗುಣ unwrapped_angles[6] = {0.0, 0.25, 0.5, 0.75, 1.0, 1.25};
	ದ್ವಿಗುಣ wrapped_angles[6] = {0.0, 0.25, 0.5, -0.25, 0.0, 0.25};

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < 6; i++) {
		unwrapped_angles[i] *= M_TWO_PI;
		wrapped_angles[i] *= M_TWO_PI;
	}

	// positive unwrapping
	ದ್ವಿಗುಣ ಕೊನೆಯ_angle = wrapped_angles[0];

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 1; i < 6; i++) {
		ಕೊನೆಯ_angle = unwrap_pi(ಕೊನೆಯ_angle, wrapped_angles[i]);
		EXPECT_DOUBLE_EQ(ಕೊನೆಯ_angle, unwrapped_angles[i]);
	}

	// negative unwrapping
	ಕೊನೆಯ_angle = -wrapped_angles[0];

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 1; i < 6; i++) {
		ಕೊನೆಯ_angle = unwrap_pi(ಕೊನೆಯ_angle, -wrapped_angles[i]);
		EXPECT_DOUBLE_EQ(ಕೊನೆಯ_angle, -unwrapped_angles[i]);
	}
}
