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

#अन्तर्भाव <cmath>

#अन्तर्भाव <gtest/gtest.h>
#अन्तर्भाव <matrix/math.hpp>
#अन्तर्भाव <iostream>

using नामाकाश matrix;

ढाँचा <typename T>
T testFunction(ध्रुव Vector<T, 3> &point)
{
	// function is f(x,y,z) = x^2 + 2xy + 3y^2 + z
	प्रत्यागम point(0) * point(0)
	       + 2.f * point(0) * point(1)
	       + 3.f * point(1) * point(1)
	       + point(2);
}

ढाँचा <typename Scalar>
Vector<Scalar, 3> positionError(ध्रुव Vector<Scalar, 3> &positionState,
				ध्रुव Vector<Scalar, 3> &velocityStateBody,
				ध्रुव Quaternion<Scalar> &bodyOrientation,
				ध्रुव Vector<Scalar, 3> &positionMeasurement,
				Scalar dt
			       )
{
	प्रत्यागम positionMeasurement - (positionState +  bodyOrientation.rotateVector(velocityStateBody) * dt);
}

TEST(MatrixDualTest, Dual)
{
	ध्रुव Dual<भिन्न, 1> a(3, 0);
	ध्रुव Dual<भिन्न, 1> b(6, 0);

	{
		EXPECT_FLOAT_EQ(a.value, 3.f);
		EXPECT_FLOAT_EQ(a.derivative(0), 1.f);
	}

	{
		// addition
		Dual<भिन्न, 1> c = a + b;
		EXPECT_FLOAT_EQ(c.value, 9.f);
		EXPECT_FLOAT_EQ(c.derivative(0), 2.f);

		Dual<भिन्न, 1> d = +a;
		EXPECT_EQ(d, a);
		d += b;
		EXPECT_EQ(d, c);

		Dual<भिन्न, 1> e = a;
		e += b.value;
		EXPECT_FLOAT_EQ(e.value, c.value);
		EXPECT_EQ(e.derivative, a.derivative);

		Dual<भिन्न, 1> f = b.value + a;
		EXPECT_EQ(f, e);
	}

	{
		// subtraction
		Dual<भिन्न, 1> c = b - a;
		EXPECT_FLOAT_EQ(c.value, 3.f);
		EXPECT_FLOAT_EQ(c.derivative(0), 0.f);

		Dual<भिन्न, 1> d = b;
		EXPECT_EQ(d, b);
		d -= a;
		EXPECT_EQ(d, c);

		Dual<भिन्न, 1> e = b;
		e -= a.value;
		EXPECT_FLOAT_EQ(e.value, c.value);
		EXPECT_EQ(e.derivative, b.derivative);

		Dual<भिन्न, 1> f = a.value - b;
		EXPECT_EQ(f, -e);
	}

	{
		// multiplication
		Dual<भिन्न, 1> c = a * b;
		EXPECT_FLOAT_EQ(c.value, 18.f);
		EXPECT_FLOAT_EQ(c.derivative(0), 9.f);

		Dual<भिन्न, 1> d = a;
		EXPECT_EQ(d, a);
		d *= b;
		EXPECT_EQ(d, c);

		Dual<भिन्न, 1> e = a;
		e *= b.value;
		EXPECT_FLOAT_EQ(e.value, c.value);
		EXPECT_EQ(e.derivative, a.derivative * b.value);

		Dual<भिन्न, 1> f = b.value * a;
		EXPECT_EQ(f, e);
	}

	{
		// division
		Dual<भिन्न, 1> c = b / a;
		EXPECT_FLOAT_EQ(c.value, 2.f);
		EXPECT_FLOAT_EQ(c.derivative(0), -1.f / 3.f);

		Dual<भिन्न, 1> d = b;
		EXPECT_EQ(d, b);
		d /= a;
		EXPECT_EQ(d, c);

		Dual<भिन्न, 1> e = b;
		e /= a.value;
		EXPECT_FLOAT_EQ(e.value, c.value);
		EXPECT_EQ(e.derivative, b.derivative / a.value);

		Dual<भिन्न, 1> f = a.value / b;
		EXPECT_EQ(f, 1.f / e);
	}

	{
		Dual<भिन्न, 1> blank;
		EXPECT_FLOAT_EQ(blank.value, 0.f);
		EXPECT_FLOAT_EQ(blank.derivative(0), 0.f);
	}

	{
		// sqrt
		EXPECT_FLOAT_EQ(वर्ग_मूल(a).value, std::वर्ग_मूल(a.value));
		EXPECT_FLOAT_EQ(वर्ग_मूल(a).derivative(0), 1.f / वर्ग_मूल(12.f));
	}

	{
		// abs
		EXPECT_EQ(a, असल(-a));
		EXPECT_NE(-a, असल(a));
		EXPECT_EQ(-a, -असल(a));
	}

	{
		// ceil
		Dual<भिन्न, 1> c(1.5, 0);
		EXPECT_FLOAT_EQ(उच्चमान(c).value, उच्चमान(c.value));
		EXPECT_FLOAT_EQ(उच्चमान(c).derivative(0), 0.f);
	}

	{
		// floor
		Dual<भिन्न, 1> c(1.5, 0);
		EXPECT_FLOAT_EQ(न्यूनमान(c).value, न्यूनमान(c.value));
		EXPECT_FLOAT_EQ(न्यूनमान(c).derivative(0), 0.f);
	}

	{
		// fmod
		EXPECT_FLOAT_EQ(भ_शेष(a, 0.8f).value, भ_शेष(a.value, 0.8f));
		EXPECT_EQ(भ_शेष(a, 0.8f).derivative, a.derivative);
	}

	{
		// max/min
		EXPECT_EQ(b, max(a, b));
		EXPECT_EQ(a, min(a, b));
	}

	{
		// isnan
		EXPECT_FALSE(IsNan(a));
		Dual<भिन्न, 1> c(std::वर्ग_मूल(-1.f), 0);
		EXPECT_TRUE(IsNan(c));
	}

	{
		// isfinite/isinf
		EXPECT_TRUE(IsFinite(a));
		EXPECT_FALSE(IsInf(a));
		Dual<भिन्न, 1> c(std::वर्ग_मूल(-1.f), 0);
		EXPECT_FALSE(IsFinite(c));
		EXPECT_FALSE(IsInf(c));
		Dual<भिन्न, 1> d(अनन्त, 0);
		EXPECT_FALSE(IsFinite(d));
		EXPECT_TRUE(IsInf(d));
	}

	{
		// sin/cos/tan
		EXPECT_FLOAT_EQ(sin(a).value, sin(a.value));
		EXPECT_FLOAT_EQ(sin(a).derivative(0), std::cos(a.value)); // sin'(x) = cos(x)

		EXPECT_FLOAT_EQ(cos(a).value, cos(a.value));
		EXPECT_FLOAT_EQ(cos(a).derivative(0), -std::sin(a.value)); // cos'(x) = -sin(x)

		EXPECT_FLOAT_EQ(tan(a).value, tan(a.value));
		EXPECT_FLOAT_EQ(tan(a).derivative(0), 1.f + std::tan(a.value)*std::tan(a.value)); // tan'(x) = 1 + tan^2(x)
	}

	{
		// asin/acos/atan
		Dual<भिन्न, 1> c(0.3f, 0);
		EXPECT_FLOAT_EQ(asin(c).value, std::asin(c.value));
		EXPECT_FLOAT_EQ(asin(c).derivative(0), 1.f / std::वर्ग_मूल(1.f - 0.3f * 0.3f)); // asin'(x) = 1/sqrt(1-x^2)

		EXPECT_FLOAT_EQ(acos(c).value, std::acos(c.value));
		EXPECT_FLOAT_EQ(acos(c).derivative(0), -1.f / std::वर्ग_मूल(1.f - 0.3f * 0.3f)); // acos'(x) = -1/sqrt(1-x^2)

		EXPECT_FLOAT_EQ(atan(c).value, std::atan(c.value));
		EXPECT_FLOAT_EQ(atan(c).derivative(0), 1.f / (1.f + 0.3f * 0.3f)); // tan'(x) = 1 + x^2
	}

	{
		// atan2
		EXPECT_FLOAT_EQ(atan2(a, b).value, atan2(a.value, b.value));
		EXPECT_EQ(atan2(a, Dual<भिन्न, 1>(b.value)), atan(a / b.value)); // atan2'(y, x) = atan'(y/x)
	}

	{
		// partial derivatives
		// function is f(x,y,z) = x^2 + 2xy + 3y^2 + z, we need with respect to d/dx and d/dy at the point (0.5, -0.8, 2)

		using D = Dual<भिन्न, 2>;

		// set our starting point, requesting partial derivatives of x and y in column 0 and 1
		Vector3<D> dualPoint(D(0.5f, 0), D(-0.8f, 1), D(2.f));

		Dual<भिन्न, 2> dualResult = testFunction(dualPoint);

		// compare to a numerical derivative:
		Vector<भिन्न, 3> floatPoint = collectReals(dualPoint);
		भिन्न floatResult = testFunction(floatPoint);

		भिन्न h = 0.0001f;
		Vector<भिन्न, 3> floatPoint_plusDX = floatPoint;
		floatPoint_plusDX(0) += h;
		भिन्न floatResult_plusDX = testFunction(floatPoint_plusDX);

		Vector<भिन्न, 3> floatPoint_plusDY = floatPoint;
		floatPoint_plusDY(1) += h;
		भिन्न floatResult_plusDY = testFunction(floatPoint_plusDY);

		Vector2f numerical_derivative((floatResult_plusDX - floatResult) / h,
					      (floatResult_plusDY - floatResult) / h);

		EXPECT_EQ(dualResult.value, floatResult);
		EXPECT_TRUE(isEqual(dualResult.derivative, numerical_derivative, 1e-2f));
	}

	{
		// jacobian
		// get residual of x/y/z with partial derivatives of rotation

		Vector3f direct_error;
		Matrix<भिन्न, 3, 4> numerical_jacobian;
		{
			Vector3f positionState(5, 6, 7);
			Vector3f velocityState(-1, 0, 1);
			Quaternionf velocityOrientation(0.2f, -0.1f, 0, 1);
			Vector3f positionMeasurement(4.5f, 6.2f, 7.9f);
			भिन्न dt = 0.1f;

			direct_error = positionError(positionState,
						     velocityState,
						     velocityOrientation,
						     positionMeasurement,
						     dt);
			भिन्न h = 0.001f;

			प्रति (माप_प्रकार i = 0; i < 4; i++) {
				Quaternion<भिन्न> h4 = velocityOrientation;
				h4(i) += h;
				numerical_jacobian.col(i) = (positionError(positionState,
							     velocityState,
							     h4,
							     positionMeasurement,
							     dt)
							     - direct_error) / h;
			}
		}
		Vector3f auto_error;
		Matrix<भिन्न, 3, 4> auto_jacobian;
		{
			using D4 = Dual<भिन्न, 4>;
			using Vector3d4 = Vector3<D4>;
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
		EXPECT_EQ(direct_error, auto_error);
		EXPECT_TRUE(isEqual(numerical_jacobian, auto_jacobian, 1e-3f));
	}
}
