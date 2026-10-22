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

उपयुज्य नामाकाश matrix;

प्रतिरूपम् <प्रकारनाम T>
T testFunction(ध्रुव सदिशम्<T, 3> &point)
{
	// function is f(x,y,z) = x^2 + 2xy + 3y^2 + z
	प्रत्यागम point(0) * point(0)
	       + 2.f * point(0) * point(1)
	       + 3.f * point(1) * point(1)
	       + point(2);
}

प्रतिरूपम् <प्रकारनाम Scalar>
सदिशम्<Scalar, 3> positionError(ध्रुव सदिशम्<Scalar, 3> &positionState,
				ध्रुव सदिशम्<Scalar, 3> &velocityStateBody,
				ध्रुव Quaternion<Scalar> &bodyOrientation,
				ध्रुव सदिशम्<Scalar, 3> &positionMeasurement,
				Scalar dt
			       )
{
	प्रत्यागम positionMeasurement - (positionState +  bodyOrientation.rotate_सदिशम्(velocityStateBody) * dt);
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
		EXPECT_FLOAT_EQ(वर्गमूलम्(a).value, std::वर्गमूलम्(a.value));
		EXPECT_FLOAT_EQ(वर्गमूलम्(a).derivative(0), 1.f / वर्गमूलम्(12.f));
	}

	{
		// abs
		EXPECT_EQ(a, निरपेक्षम्(-a));
		EXPECT_NE(-a, निरपेक्षम्(a));
		EXPECT_EQ(-a, -निरपेक्षम्(a));
	}

	{
		// ceil
		Dual<भिन्न, 1> c(1.5, 0);
		EXPECT_FLOAT_EQ(ceil(c).value, ceil(c.value));
		EXPECT_FLOAT_EQ(ceil(c).derivative(0), 0.f);
	}

	{
		// floor
		Dual<भिन्न, 1> c(1.5, 0);
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
		Dual<भिन्न, 1> c(std::वर्गमूलम्(-1.f), 0);
		EXPECT_TRUE(IsNan(c));
	}

	{
		// isfinite/isinf
		EXPECT_TRUE(IsFinite(a));
		EXPECT_FALSE(IsInf(a));
		Dual<भिन्न, 1> c(std::वर्गमूलम्(-1.f), 0);
		EXPECT_FALSE(IsFinite(c));
		EXPECT_FALSE(IsInf(c));
		Dual<भिन्न, 1> d(INFINITY, 0);
		EXPECT_FALSE(IsFinite(d));
		EXPECT_TRUE(IsInf(d));
	}

	{
		// sin/cos/tan
		EXPECT_FLOAT_EQ(ज्या(a).value, ज्या(a.value));
		EXPECT_FLOAT_EQ(ज्या(a).derivative(0), std::कोटिज्या(a.value)); // sin'(x) = cos(x)

		EXPECT_FLOAT_EQ(कोटिज्या(a).value, कोटिज्या(a.value));
		EXPECT_FLOAT_EQ(कोटिज्या(a).derivative(0), -std::ज्या(a.value)); // cos'(x) = -sin(x)

		EXPECT_FLOAT_EQ(tan(a).value, tan(a.value));
		EXPECT_FLOAT_EQ(tan(a).derivative(0), 1.f + std::tan(a.value)*std::tan(a.value)); // tan'(x) = 1 + tan^2(x)
	}

	{
		// asin/acos/atan
		Dual<भिन्न, 1> c(0.3f, 0);
		EXPECT_FLOAT_EQ(asin(c).value, std::asin(c.value));
		EXPECT_FLOAT_EQ(asin(c).derivative(0), 1.f / std::वर्गमूलम्(1.f - 0.3f * 0.3f)); // asin'(x) = 1/sqrt(1-x^2)

		EXPECT_FLOAT_EQ(acos(c).value, std::acos(c.value));
		EXPECT_FLOAT_EQ(acos(c).derivative(0), -1.f / std::वर्गमूलम्(1.f - 0.3f * 0.3f)); // acos'(x) = -1/sqrt(1-x^2)

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

		उपयुज्य D = Dual<भिन्न, 2>;

		// set our starting point, requesting partial derivatives of x and y in column 0 and 1
		Vector3<D> dualPoint(D(0.5f, 0), D(-0.8f, 1), D(2.f));

		Dual<भिन्न, 2> dual_परिणाम = testFunction(dualPoint);

		// compare to a numerical derivative:
		सदिशम्<भिन्न, 3> floatPoint = collectReals(dualPoint);
		भिन्न float_परिणाम = testFunction(floatPoint);

		भिन्न h = 0.0001f;
		सदिशम्<भिन्न, 3> floatPoint_plusDX = floatPoint;
		floatPoint_plusDX(0) += h;
		भिन्न float_परिणाम_plus_DX = testFunction(floatPoint_plusDX);

		सदिशम्<भिन्न, 3> floatPoint_plusDY = floatPoint;
		floatPoint_plusDY(1) += h;
		भिन्न float_परिणाम_plus_DY = testFunction(floatPoint_plusDY);

		Vector2f numerical_derivative((float_परिणाम_plus_DX - float_परिणाम) / h,
					      (float_परिणाम_plus_DY - float_परिणाम) / h);

		EXPECT_EQ(dual_परिणाम.value, float_परिणाम);
		EXPECT_TRUE(isEqual(dual_परिणाम.derivative, numerical_derivative, 1e-2f));
	}

	{
		// jacobian
		// get residual of x/y/z with partial derivatives of rotation

		Vector3f प्रत्यक्षम्_error;
		Matrix<भिन्न, 3, 4> numerical_jacobian;
		{
			Vector3f positionState(5, 6, 7);
			Vector3f velocityState(-1, 0, 1);
			Quaternionf velocityOrientation(0.2f, -0.1f, 0, 1);
			Vector3f positionMeasurement(4.5f, 6.2f, 7.9f);
			भिन्न dt = 0.1f;

			प्रत्यक्षम्_error = positionError(positionState,
						     velocityState,
						     velocityOrientation,
						     positionMeasurement,
						     dt);
			भिन्न h = 0.001f;

			प्रति (size_t i = 0; i < 4; i++) {
				Quaternion<भिन्न> h4 = velocityOrientation;
				h4(i) += h;
				numerical_jacobian.col(i) = (positionError(positionState,
							     velocityState,
							     h4,
							     positionMeasurement,
							     dt)
							     - प्रत्यक्षम्_error) / h;
			}
		}
		Vector3f auto_error;
		Matrix<भिन्न, 3, 4> auto_jacobian;
		{
			उपयुज्य D4 = Dual<भिन्न, 4>;
			उपयुज्य Vector3d4 = Vector3<D4>;
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
		EXPECT_EQ(प्रत्यक्षम्_error, auto_error);
		EXPECT_TRUE(isEqual(numerical_jacobian, auto_jacobian, 1e-3f));
	}
}
