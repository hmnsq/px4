/****************************************************************************
 *
 *   Copyright (C) 2022 PX4 Development Team. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name PX4 nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

#ಅನ್ತರ್ಭಾವ <cmath>

#ಅನ್ತರ್ಭಾವ <gtest/gtest.h>
#ಅನ್ತರ್ಭಾವ <matrix/math.hpp>
#ಅನ್ತರ್ಭಾವ <iostream>

ಬಳಸಿ ನಾಮಾಕಾಶ ಮಾತೃಕೆ;

ಮಾದರಿ <ಬಗೆ_ಹೆಸರು T>
T testFunction(ಸ್ಥಿರಾಂಕ ಸದಿಶ<T, 3> &ಬಿಂದು)
{
	// function is f(x,y,z) = x^2 + 2xy + 3y^2 + z
	ಹಿಂತಿರುಗಿ ಬಿಂದು(0) * ಬಿಂದು(0)
	       + 2.f * ಬಿಂದು(0) * ಬಿಂದು(1)
	       + 3.f * ಬಿಂದು(1) * ಬಿಂದು(1)
	       + ಬಿಂದು(2);
}

ಮಾದರಿ <ಬಗೆ_ಹೆಸರು Scalar>
ಸದಿಶ<Scalar, 3> positionError(ಸ್ಥಿರಾಂಕ ಸದಿಶ<Scalar, 3> &positionState,
				ಸ್ಥಿರಾಂಕ ಸದಿಶ<Scalar, 3> &velocityStateBody,
				ಸ್ಥಿರಾಂಕ Quaternion<Scalar> &bodyOrientation,
				ಸ್ಥಿರಾಂಕ ಸದಿಶ<Scalar, 3> &positionMeasurement,
				Scalar dt
			       )
{
	ಹಿಂತಿರುಗಿ positionMeasurement - (positionState +  bodyOrientation.rotate_ಸದಿಶ(velocityStateBody) * dt);
}

TEST(ಮಾತೃಕೆ_Dual_Test, Dual)
{
	ಸ್ಥಿರಾಂಕ Dual<ದಶಮಾಂಶ, 1> a(3, 0);
	ಸ್ಥಿರಾಂಕ Dual<ದಶಮಾಂಶ, 1> b(6, 0);

	{
		EXPECT_FLOAT_EQ(a.value, 3.f);
		EXPECT_FLOAT_EQ(a.derivative(0), 1.f);
	}

	{
		// addition
		Dual<ದಶಮಾಂಶ, 1> c = a + b;
		EXPECT_FLOAT_EQ(c.value, 9.f);
		EXPECT_FLOAT_EQ(c.derivative(0), 2.f);

		Dual<ದಶಮಾಂಶ, 1> d = +a;
		EXPECT_EQ(d, a);
		d += b;
		EXPECT_EQ(d, c);

		Dual<ದಶಮಾಂಶ, 1> e = a;
		e += b.value;
		EXPECT_FLOAT_EQ(e.value, c.value);
		EXPECT_EQ(e.derivative, a.derivative);

		Dual<ದಶಮಾಂಶ, 1> f = b.value + a;
		EXPECT_EQ(f, e);
	}

	{
		// subtraction
		Dual<ದಶಮಾಂಶ, 1> c = b - a;
		EXPECT_FLOAT_EQ(c.value, 3.f);
		EXPECT_FLOAT_EQ(c.derivative(0), 0.f);

		Dual<ದಶಮಾಂಶ, 1> d = b;
		EXPECT_EQ(d, b);
		d -= a;
		EXPECT_EQ(d, c);

		Dual<ದಶಮಾಂಶ, 1> e = b;
		e -= a.value;
		EXPECT_FLOAT_EQ(e.value, c.value);
		EXPECT_EQ(e.derivative, b.derivative);

		Dual<ದಶಮಾಂಶ, 1> f = a.value - b;
		EXPECT_EQ(f, -e);
	}

	{
		// multiplication
		Dual<ದಶಮಾಂಶ, 1> c = a * b;
		EXPECT_FLOAT_EQ(c.value, 18.f);
		EXPECT_FLOAT_EQ(c.derivative(0), 9.f);

		Dual<ದಶಮಾಂಶ, 1> d = a;
		EXPECT_EQ(d, a);
		d *= b;
		EXPECT_EQ(d, c);

		Dual<ದಶಮಾಂಶ, 1> e = a;
		e *= b.value;
		EXPECT_FLOAT_EQ(e.value, c.value);
		EXPECT_EQ(e.derivative, a.derivative * b.value);

		Dual<ದಶಮಾಂಶ, 1> f = b.value * a;
		EXPECT_EQ(f, e);
	}

	{
		// division
		Dual<ದಶಮಾಂಶ, 1> c = b / a;
		EXPECT_FLOAT_EQ(c.value, 2.f);
		EXPECT_FLOAT_EQ(c.derivative(0), -1.f / 3.f);

		Dual<ದಶಮಾಂಶ, 1> d = b;
		EXPECT_EQ(d, b);
		d /= a;
		EXPECT_EQ(d, c);

		Dual<ದಶಮಾಂಶ, 1> e = b;
		e /= a.value;
		EXPECT_FLOAT_EQ(e.value, c.value);
		EXPECT_EQ(e.derivative, b.derivative / a.value);

		Dual<ದಶಮಾಂಶ, 1> f = a.value / b;
		EXPECT_EQ(f, 1.f / e);
	}

	{
		Dual<ದಶಮಾಂಶ, 1> blank;
		EXPECT_FLOAT_EQ(blank.value, 0.f);
		EXPECT_FLOAT_EQ(blank.derivative(0), 0.f);
	}

	{
		// sqrt
		EXPECT_FLOAT_EQ(ವರ್ಗಮೂಲ(a).value, std::ವರ್ಗಮೂಲ(a.value));
		EXPECT_FLOAT_EQ(ವರ್ಗಮೂಲ(a).derivative(0), 1.f / ವರ್ಗಮೂಲ(12.f));
	}

	{
		// abs
		EXPECT_EQ(a, ನಿರಪೇಕ್ಷ(-a));
		EXPECT_NE(-a, ನಿರಪೇಕ್ಷ(a));
		EXPECT_EQ(-a, -ನಿರಪೇಕ್ಷ(a));
	}

	{
		// ceil
		Dual<ದಶಮಾಂಶ, 1> c(1.5, 0);
		EXPECT_FLOAT_EQ(ceil(c).value, ceil(c.value));
		EXPECT_FLOAT_EQ(ceil(c).derivative(0), 0.f);
	}

	{
		// floor
		Dual<ದಶಮಾಂಶ, 1> c(1.5, 0);
		EXPECT_FLOAT_EQ(floor(c).value, floor(c.value));
		EXPECT_FLOAT_EQ(floor(c).derivative(0), 0.f);
	}

	{
		// fmod
		EXPECT_FLOAT_EQ(fmod(a, 0.8f).value, fmod(a.value, 0.8f));
		EXPECT_EQ(fmod(a, 0.8f).derivative, a.derivative);
	}

	{
		// max/min
		EXPECT_EQ(b, max(a, b));
		EXPECT_EQ(a, min(a, b));
	}

	{
		// isnan
		EXPECT_FALSE(IsNan(a));
		Dual<ದಶಮಾಂಶ, 1> c(std::ವರ್ಗಮೂಲ(-1.f), 0);
		EXPECT_TRUE(IsNan(c));
	}

	{
		// isfinite/isinf
		EXPECT_TRUE(IsFinite(a));
		EXPECT_FALSE(IsInf(a));
		Dual<ದಶಮಾಂಶ, 1> c(std::ವರ್ಗಮೂಲ(-1.f), 0);
		EXPECT_FALSE(IsFinite(c));
		EXPECT_FALSE(IsInf(c));
		Dual<ದಶಮಾಂಶ, 1> d(INFINITY, 0);
		EXPECT_FALSE(IsFinite(d));
		EXPECT_TRUE(IsInf(d));
	}

	{
		// sin/cos/tan
		EXPECT_FLOAT_EQ(ಜ್ಯಾ(a).value, ಜ್ಯಾ(a.value));
		EXPECT_FLOAT_EQ(ಜ್ಯಾ(a).derivative(0), std::ಕೋಜ್ಯಾ(a.value)); // sin'(x) = cos(x)

		EXPECT_FLOAT_EQ(ಕೋಜ್ಯಾ(a).value, ಕೋಜ್ಯಾ(a.value));
		EXPECT_FLOAT_EQ(ಕೋಜ್ಯಾ(a).derivative(0), -std::ಜ್ಯಾ(a.value)); // cos'(x) = -sin(x)

		EXPECT_FLOAT_EQ(tan(a).value, tan(a.value));
		EXPECT_FLOAT_EQ(tan(a).derivative(0), 1.f + std::tan(a.value)*std::tan(a.value)); // tan'(x) = 1 + tan^2(x)
	}

	{
		// asin/acos/atan
		Dual<ದಶಮಾಂಶ, 1> c(0.3f, 0);
		EXPECT_FLOAT_EQ(asin(c).value, std::asin(c.value));
		EXPECT_FLOAT_EQ(asin(c).derivative(0), 1.f / std::ವರ್ಗಮೂಲ(1.f - 0.3f * 0.3f)); // asin'(x) = 1/sqrt(1-x^2)

		EXPECT_FLOAT_EQ(acos(c).value, std::acos(c.value));
		EXPECT_FLOAT_EQ(acos(c).derivative(0), -1.f / std::ವರ್ಗಮೂಲ(1.f - 0.3f * 0.3f)); // acos'(x) = -1/sqrt(1-x^2)

		EXPECT_FLOAT_EQ(atan(c).value, std::atan(c.value));
		EXPECT_FLOAT_EQ(atan(c).derivative(0), 1.f / (1.f + 0.3f * 0.3f)); // tan'(x) = 1 + x^2
	}

	{
		// atan2
		EXPECT_FLOAT_EQ(atan2(a, b).value, atan2(a.value, b.value));
		EXPECT_EQ(atan2(a, Dual<ದಶಮಾಂಶ, 1>(b.value)), atan(a / b.value)); // atan2'(y, x) = atan'(y/x)
	}

	{
		// partial derivatives
		// function is f(x,y,z) = x^2 + 2xy + 3y^2 + z, we need with respect to d/dx and d/dy at the point (0.5, -0.8, 2)

		ಬಳಸಿ D = Dual<ದಶಮಾಂಶ, 2>;

		// set our starting point, requesting partial derivatives of x and y in column 0 and 1
		Vector3<D> dual_ಬಿಂದು(D(0.5f, 0), D(-0.8f, 1), D(2.f));

		Dual<ದಶಮಾಂಶ, 2> dual_ಫಲಿತಾಂಶ = testFunction(dual_ಬಿಂದು);

		// compare to a numerical derivative:
		ಸದಿಶ<ದಶಮಾಂಶ, 3> float_ಬಿಂದು = collectReals(dual_ಬಿಂದು);
		ದಶಮಾಂಶ float_ಫಲಿತಾಂಶ = testFunction(float_ಬಿಂದು);

		ದಶಮಾಂಶ h = 0.0001f;
		ಸದಿಶ<ದಶಮಾಂಶ, 3> float_ಬಿಂದು_plus_DX = float_ಬಿಂದು;
		float_ಬಿಂದು_plus_DX(0) += h;
		ದಶಮಾಂಶ float_ಫಲಿತಾಂಶ_plus_DX = testFunction(float_ಬಿಂದು_plus_DX);

		ಸದಿಶ<ದಶಮಾಂಶ, 3> float_ಬಿಂದು_plus_DY = float_ಬಿಂದು;
		float_ಬಿಂದು_plus_DY(1) += h;
		ದಶಮಾಂಶ float_ಫಲಿತಾಂಶ_plus_DY = testFunction(float_ಬಿಂದು_plus_DY);

		Vector2f numerical_derivative((float_ಫಲಿತಾಂಶ_plus_DX - float_ಫಲಿತಾಂಶ) / h,
					      (float_ಫಲಿತಾಂಶ_plus_DY - float_ಫಲಿತಾಂಶ) / h);

		EXPECT_EQ(dual_ಫಲಿತಾಂಶ.value, float_ಫಲಿತಾಂಶ);
		EXPECT_TRUE(isEqual(dual_ಫಲಿತಾಂಶ.derivative, numerical_derivative, 1e-2f));
	}

	{
		// jacobian
		// get residual of x/y/z with partial derivatives of rotation

		Vector3f ನೇರ_error;
		ಮಾತೃಕೆ_2<ದಶಮಾಂಶ, 3, 4> numerical_jacobian;
		{
			Vector3f positionState(5, 6, 7);
			Vector3f velocityState(-1, 0, 1);
			Quaternionf velocityOrientation(0.2f, -0.1f, 0, 1);
			Vector3f positionMeasurement(4.5f, 6.2f, 7.9f);
			ದಶಮಾಂಶ dt = 0.1f;

			ನೇರ_error = positionError(positionState,
						     velocityState,
						     velocityOrientation,
						     positionMeasurement,
						     dt);
			ದಶಮಾಂಶ h = 0.001f;

			ಗಾಗಿ (size_t i = 0; i < 4; i++) {
				Quaternion<ದಶಮಾಂಶ> h4 = velocityOrientation;
				h4(i) += h;
				numerical_jacobian.ಲಂಬಸಾಲು(i) = (positionError(positionState,
							     velocityState,
							     h4,
							     positionMeasurement,
							     dt)
							     - ನೇರ_error) / h;
			}
		}
		Vector3f auto_error;
		ಮಾತೃಕೆ_2<ದಶಮಾಂಶ, 3, 4> auto_jacobian;
		{
			ಬಳಸಿ D4 = Dual<ದಶಮಾಂಶ, 4>;
			ಬಳಸಿ Vector3d4 = Vector3<D4>;
			Vector3d4 positionState(D4(5), D4(6), D4(7));
			Vector3d4 velocityState(D4(-1), D4(0), D4(1));

			// request partial derivatives of velocity orientation
			// by setting these variables' derivatives in corresponding columns [0...3]
			Quaternion<D4> velocityOrientation(D4(0.2f, 0), D4(-0.1f, 1), D4(0, 2), D4(1, 3));

			Vector3d4 positionMeasurement(D4(4.5f), D4(6.2f), D4(7.9f));
			D4 dt(0.1f);


			Vector3d4 error = positionError(positionState,
							velocityState,
							velocityOrientation,
							positionMeasurement,
							dt);
			auto_error = collectReals(error);
			auto_jacobian = collectDerivatives(error);
		}
		EXPECT_EQ(ನೇರ_error, auto_error);
		EXPECT_TRUE(isEqual(numerical_jacobian, auto_jacobian, 1e-3f));
	}
}
