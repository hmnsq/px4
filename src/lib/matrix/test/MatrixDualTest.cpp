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

#ضمن <cmath>

#ضمن <gtest/gtest.h>
#ضمن <matrix/math.hpp>
#ضمن <iostream>

استعمال نطاق ميٽرڪس;

سانچو <قسم_نالو T>
T testFunction(ثابت ويڪٽر<T, 3> &نقطو)
{
	// function is f(x,y,z) = x^2 + 2xy + 3y^2 + z
	أرجع نقطو(0) * نقطو(0)
	       + 2.f * نقطو(0) * نقطو(1)
	       + 3.f * نقطو(1) * نقطو(1)
	       + نقطو(2);
}

سانچو <قسم_نالو Scalar>
ويڪٽر<Scalar, 3> positionError(ثابت ويڪٽر<Scalar, 3> &positionState,
				ثابت ويڪٽر<Scalar, 3> &velocityStateBody,
				ثابت Quaternion<Scalar> &bodyOrientation,
				ثابت ويڪٽر<Scalar, 3> &positionMeasurement,
				Scalar dt
			       )
{
	أرجع positionMeasurement - (positionState +  bodyOrientation.rotate_ويڪٽر(velocityStateBody) * dt);
}

TEST(ميٽرڪس_Dual_Test, Dual)
{
	ثابت Dual<عائم, 1> a(3, 0);
	ثابت Dual<عائم, 1> b(6, 0);

	{
		EXPECT_FLOAT_EQ(a.value, 3.f);
		EXPECT_FLOAT_EQ(a.derivative(0), 1.f);
	}

	{
		// addition
		Dual<عائم, 1> c = a + b;
		EXPECT_FLOAT_EQ(c.value, 9.f);
		EXPECT_FLOAT_EQ(c.derivative(0), 2.f);

		Dual<عائم, 1> d = +a;
		EXPECT_EQ(d, a);
		d += b;
		EXPECT_EQ(d, c);

		Dual<عائم, 1> e = a;
		e += b.value;
		EXPECT_FLOAT_EQ(e.value, c.value);
		EXPECT_EQ(e.derivative, a.derivative);

		Dual<عائم, 1> f = b.value + a;
		EXPECT_EQ(f, e);
	}

	{
		// subtraction
		Dual<عائم, 1> c = b - a;
		EXPECT_FLOAT_EQ(c.value, 3.f);
		EXPECT_FLOAT_EQ(c.derivative(0), 0.f);

		Dual<عائم, 1> d = b;
		EXPECT_EQ(d, b);
		d -= a;
		EXPECT_EQ(d, c);

		Dual<عائم, 1> e = b;
		e -= a.value;
		EXPECT_FLOAT_EQ(e.value, c.value);
		EXPECT_EQ(e.derivative, b.derivative);

		Dual<عائم, 1> f = a.value - b;
		EXPECT_EQ(f, -e);
	}

	{
		// multiplication
		Dual<عائم, 1> c = a * b;
		EXPECT_FLOAT_EQ(c.value, 18.f);
		EXPECT_FLOAT_EQ(c.derivative(0), 9.f);

		Dual<عائم, 1> d = a;
		EXPECT_EQ(d, a);
		d *= b;
		EXPECT_EQ(d, c);

		Dual<عائم, 1> e = a;
		e *= b.value;
		EXPECT_FLOAT_EQ(e.value, c.value);
		EXPECT_EQ(e.derivative, a.derivative * b.value);

		Dual<عائم, 1> f = b.value * a;
		EXPECT_EQ(f, e);
	}

	{
		// division
		Dual<عائم, 1> c = b / a;
		EXPECT_FLOAT_EQ(c.value, 2.f);
		EXPECT_FLOAT_EQ(c.derivative(0), -1.f / 3.f);

		Dual<عائم, 1> d = b;
		EXPECT_EQ(d, b);
		d /= a;
		EXPECT_EQ(d, c);

		Dual<عائم, 1> e = b;
		e /= a.value;
		EXPECT_FLOAT_EQ(e.value, c.value);
		EXPECT_EQ(e.derivative, b.derivative / a.value);

		Dual<عائم, 1> f = a.value / b;
		EXPECT_EQ(f, 1.f / e);
	}

	{
		Dual<عائم, 1> blank;
		EXPECT_FLOAT_EQ(blank.value, 0.f);
		EXPECT_FLOAT_EQ(blank.derivative(0), 0.f);
	}

	{
		// sqrt
		EXPECT_FLOAT_EQ(جذر(a).value, std::جذر(a.value));
		EXPECT_FLOAT_EQ(جذر(a).derivative(0), 1.f / جذر(12.f));
	}

	{
		// abs
		EXPECT_EQ(a, مطلق_قدر(-a));
		EXPECT_NE(-a, مطلق_قدر(a));
		EXPECT_EQ(-a, -مطلق_قدر(a));
	}

	{
		// ceil
		Dual<عائم, 1> c(1.5, 0);
		EXPECT_FLOAT_EQ(ceil(c).value, ceil(c.value));
		EXPECT_FLOAT_EQ(ceil(c).derivative(0), 0.f);
	}

	{
		// floor
		Dual<عائم, 1> c(1.5, 0);
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
		Dual<عائم, 1> c(std::جذر(-1.f), 0);
		EXPECT_TRUE(IsNan(c));
	}

	{
		// isfinite/isinf
		EXPECT_TRUE(IsFinite(a));
		EXPECT_FALSE(IsInf(a));
		Dual<عائم, 1> c(std::جذر(-1.f), 0);
		EXPECT_FALSE(IsFinite(c));
		EXPECT_FALSE(IsInf(c));
		Dual<عائم, 1> d(INFINITY, 0);
		EXPECT_FALSE(IsFinite(d));
		EXPECT_TRUE(IsInf(d));
	}

	{
		// sin/cos/tan
		EXPECT_FLOAT_EQ(جيب(a).value, جيب(a.value));
		EXPECT_FLOAT_EQ(جيب(a).derivative(0), std::جيب_تمام(a.value)); // sin'(x) = cos(x)

		EXPECT_FLOAT_EQ(جيب_تمام(a).value, جيب_تمام(a.value));
		EXPECT_FLOAT_EQ(جيب_تمام(a).derivative(0), -std::جيب(a.value)); // cos'(x) = -sin(x)

		EXPECT_FLOAT_EQ(tan(a).value, tan(a.value));
		EXPECT_FLOAT_EQ(tan(a).derivative(0), 1.f + std::tan(a.value)*std::tan(a.value)); // tan'(x) = 1 + tan^2(x)
	}

	{
		// asin/acos/atan
		Dual<عائم, 1> c(0.3f, 0);
		EXPECT_FLOAT_EQ(asin(c).value, std::asin(c.value));
		EXPECT_FLOAT_EQ(asin(c).derivative(0), 1.f / std::جذر(1.f - 0.3f * 0.3f)); // asin'(x) = 1/sqrt(1-x^2)

		EXPECT_FLOAT_EQ(acos(c).value, std::acos(c.value));
		EXPECT_FLOAT_EQ(acos(c).derivative(0), -1.f / std::جذر(1.f - 0.3f * 0.3f)); // acos'(x) = -1/sqrt(1-x^2)

		EXPECT_FLOAT_EQ(atan(c).value, std::atan(c.value));
		EXPECT_FLOAT_EQ(atan(c).derivative(0), 1.f / (1.f + 0.3f * 0.3f)); // tan'(x) = 1 + x^2
	}

	{
		// atan2
		EXPECT_FLOAT_EQ(atan2(a, b).value, atan2(a.value, b.value));
		EXPECT_EQ(atan2(a, Dual<عائم, 1>(b.value)), atan(a / b.value)); // atan2'(y, x) = atan'(y/x)
	}

	{
		// partial derivatives
		// function is f(x,y,z) = x^2 + 2xy + 3y^2 + z, we need with respect to d/dx and d/dy at the point (0.5, -0.8, 2)

		استعمال D = Dual<عائم, 2>;

		// set our starting point, requesting partial derivatives of x and y in column 0 and 1
		Vector3<D> dual_نقطو(D(0.5f, 0), D(-0.8f, 1), D(2.f));

		Dual<عائم, 2> dual_نتيجو = testFunction(dual_نقطو);

		// compare to a numerical derivative:
		ويڪٽر<عائم, 3> float_نقطو = collectReals(dual_نقطو);
		عائم float_نتيجو = testFunction(float_نقطو);

		عائم h = 0.0001f;
		ويڪٽر<عائم, 3> float_نقطو_plus_DX = float_نقطو;
		float_نقطو_plus_DX(0) += h;
		عائم float_نتيجو_plus_DX = testFunction(float_نقطو_plus_DX);

		ويڪٽر<عائم, 3> float_نقطو_plus_DY = float_نقطو;
		float_نقطو_plus_DY(1) += h;
		عائم float_نتيجو_plus_DY = testFunction(float_نقطو_plus_DY);

		Vector2f numerical_derivative((float_نتيجو_plus_DX - float_نتيجو) / h,
					      (float_نتيجو_plus_DY - float_نتيجو) / h);

		EXPECT_EQ(dual_نتيجو.value, float_نتيجو);
		EXPECT_TRUE(isEqual(dual_نتيجو.derivative, numerical_derivative, 1e-2f));
	}

	{
		// jacobian
		// get residual of x/y/z with partial derivatives of rotation

		Vector3f سڌو_error;
		ميٽرڪس_2<عائم, 3, 4> numerical_jacobian;
		{
			Vector3f positionState(5, 6, 7);
			Vector3f velocityState(-1, 0, 1);
			Quaternionf velocityOrientation(0.2f, -0.1f, 0, 1);
			Vector3f positionMeasurement(4.5f, 6.2f, 7.9f);
			عائم dt = 0.1f;

			سڌو_error = positionError(positionState,
						     velocityState,
						     velocityOrientation,
						     positionMeasurement,
						     dt);
			عائم h = 0.001f;

			لكل (size_t i = 0; i < 4; i++) {
				Quaternion<عائم> h4 = velocityOrientation;
				h4(i) += h;
				numerical_jacobian.کانو(i) = (positionError(positionState,
							     velocityState,
							     h4,
							     positionMeasurement,
							     dt)
							     - سڌو_error) / h;
			}
		}
		Vector3f auto_error;
		ميٽرڪس_2<عائم, 3, 4> auto_jacobian;
		{
			استعمال D4 = Dual<عائم, 4>;
			استعمال Vector3d4 = Vector3<D4>;
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
		EXPECT_EQ(سڌو_error, auto_error);
		EXPECT_TRUE(isEqual(numerical_jacobian, auto_jacobian, 1e-3f));
	}
}
