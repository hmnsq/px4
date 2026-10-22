#అన్తర్భావ <matrix/math.hpp>
#అన్తర్భావ <gtest/gtest.h>

ఉపయోగించి నామాకాశ మాతృక;

TEST(మాతృక_Unwrap_Test, UnwrapFloats)
{
	స్థిరరాశి దశాంశం M_TWO_PI_F = దశాంశం(M_PI_F * 2);

	దశాంశం unwrapped_angles[6] = {0.0, 0.25, 0.5, 0.75, 1.0, 1.25};
	దశాంశం wrapped_angles[6] = {0.0, 0.25, 0.5, -0.25, 0.0, 0.25};

	కొరకు (పూర్ణాంకం i = 0; i < 6; i++) {
		unwrapped_angles[i] *= M_TWO_PI_F;
		wrapped_angles[i] *= M_TWO_PI_F;
	}

	// positive unwrapping
	దశాంశం చివరి_angle = wrapped_angles[0];

	కొరకు (పూర్ణాంకం i = 1; i < 6; i++) {
		చివరి_angle = unwrap_pi(చివరి_angle, wrapped_angles[i]);
		EXPECT_FLOAT_EQ(చివరి_angle, unwrapped_angles[i]);
	}

	// negative unwrapping
	చివరి_angle = -wrapped_angles[0];

	కొరకు (పూర్ణాంకం i = 1; i < 6; i++) {
		చివరి_angle = unwrap_pi(చివరి_angle, -wrapped_angles[i]);
		EXPECT_FLOAT_EQ(చివరి_angle, -unwrapped_angles[i]);
	}
}

TEST(మాతృక_Unwrap_Test, UnwrapDoubles)
{
	స్థిరరాశి ద్విగుణ M_TWO_PI = M_PI * 2;

	ద్విగుణ unwrapped_angles[6] = {0.0, 0.25, 0.5, 0.75, 1.0, 1.25};
	ద్విగుణ wrapped_angles[6] = {0.0, 0.25, 0.5, -0.25, 0.0, 0.25};

	కొరకు (పూర్ణాంకం i = 0; i < 6; i++) {
		unwrapped_angles[i] *= M_TWO_PI;
		wrapped_angles[i] *= M_TWO_PI;
	}

	// positive unwrapping
	ద్విగుణ చివరి_angle = wrapped_angles[0];

	కొరకు (పూర్ణాంకం i = 1; i < 6; i++) {
		చివరి_angle = unwrap_pi(చివరి_angle, wrapped_angles[i]);
		EXPECT_DOUBLE_EQ(చివరి_angle, unwrapped_angles[i]);
	}

	// negative unwrapping
	చివరి_angle = -wrapped_angles[0];

	కొరకు (పూర్ణాంకం i = 1; i < 6; i++) {
		చివరి_angle = unwrap_pi(చివరి_angle, -wrapped_angles[i]);
		EXPECT_DOUBLE_EQ(చివరి_angle, -unwrapped_angles[i]);
	}
}
