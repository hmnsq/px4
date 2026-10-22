#ਅਨ੍ਤਰ੍ਭਾਵ <matrix/math.hpp>
#ਅਨ੍ਤਰ੍ਭਾਵ <gtest/gtest.h>

ਵਰਤੋਂ ਨਾਮਾਕਾਸ਼ ਮੈਟ੍ਰਿਕਸ;

TEST(ਮੈਟ੍ਰਿਕਸ_Unwrap_Test, UnwrapFloats)
{
	ਅਚਲ ਦਸ਼ਮਲਵ M_TWO_PI_F = ਦਸ਼ਮਲਵ(M_PI_F * 2);

	ਦਸ਼ਮਲਵ unwrapped_angles[6] = {0.0, 0.25, 0.5, 0.75, 1.0, 1.25};
	ਦਸ਼ਮਲਵ wrapped_angles[6] = {0.0, 0.25, 0.5, -0.25, 0.0, 0.25};

	ਲਈ (ਪੂਰਨ i = 0; i < 6; i++) {
		unwrapped_angles[i] *= M_TWO_PI_F;
		wrapped_angles[i] *= M_TWO_PI_F;
	}

	// positive unwrapping
	ਦਸ਼ਮਲਵ ਆਖਰੀ_angle = wrapped_angles[0];

	ਲਈ (ਪੂਰਨ i = 1; i < 6; i++) {
		ਆਖਰੀ_angle = unwrap_pi(ਆਖਰੀ_angle, wrapped_angles[i]);
		EXPECT_FLOAT_EQ(ਆਖਰੀ_angle, unwrapped_angles[i]);
	}

	// negative unwrapping
	ਆਖਰੀ_angle = -wrapped_angles[0];

	ਲਈ (ਪੂਰਨ i = 1; i < 6; i++) {
		ਆਖਰੀ_angle = unwrap_pi(ਆਖਰੀ_angle, -wrapped_angles[i]);
		EXPECT_FLOAT_EQ(ਆਖਰੀ_angle, -unwrapped_angles[i]);
	}
}

TEST(ਮੈਟ੍ਰਿਕਸ_Unwrap_Test, UnwrapDoubles)
{
	ਅਚਲ ਦੁੱਗਣਾ M_TWO_PI = M_PI * 2;

	ਦੁੱਗਣਾ unwrapped_angles[6] = {0.0, 0.25, 0.5, 0.75, 1.0, 1.25};
	ਦੁੱਗਣਾ wrapped_angles[6] = {0.0, 0.25, 0.5, -0.25, 0.0, 0.25};

	ਲਈ (ਪੂਰਨ i = 0; i < 6; i++) {
		unwrapped_angles[i] *= M_TWO_PI;
		wrapped_angles[i] *= M_TWO_PI;
	}

	// positive unwrapping
	ਦੁੱਗਣਾ ਆਖਰੀ_angle = wrapped_angles[0];

	ਲਈ (ਪੂਰਨ i = 1; i < 6; i++) {
		ਆਖਰੀ_angle = unwrap_pi(ਆਖਰੀ_angle, wrapped_angles[i]);
		EXPECT_DOUBLE_EQ(ਆਖਰੀ_angle, unwrapped_angles[i]);
	}

	// negative unwrapping
	ਆਖਰੀ_angle = -wrapped_angles[0];

	ਲਈ (ਪੂਰਨ i = 1; i < 6; i++) {
		ਆਖਰੀ_angle = unwrap_pi(ਆਖਰੀ_angle, -wrapped_angles[i]);
		EXPECT_DOUBLE_EQ(ਆਖਰੀ_angle, -unwrapped_angles[i]);
	}
}
