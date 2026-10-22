/****************************************************************************
 *
 *   Copyright (c) 2017-2022 PX4 Development Team. All rights reserved.
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

/**
 * @file Functions.hpp
 *
 * collection of rather simple mathematical functions that get used over and over again
 */

#ہدایت once

#ضمن "Limits.hpp"

#ضمن <px4_platform_common/defines.h>
#ضمن <matrix/matrix/math.hpp>

نطاق math
{

// Type-safe signum function with zero treated as positive
سانچا<قسم_ناں T>
صحيح signNoZero(T val)
{
	أرجع (T(0) <= val) - (val < T(0));
}

/**
 * Sign function based on a boolean
 *
 * @param[in] positive Truth value to take the sign from
 * @return 1 if positive is true, -1 if positive is false
 */
مضمن صحيح signFromBool(منطقي مثبت)
{
	أرجع مثبت ? 1 : -1;
}

سانچا<قسم_ناں T>
T sq(T val)
{
	أرجع val * val;
}

/*
 * So called exponential curve function implementation.
 * It is essentially a linear combination between a linear and a cubic function.
 * @param value [-1,1] input value to function
 * @param e [0,1] function parameter to set ratio between linear and cubic shape
 * 		0 - pure linear function
 * 		1 - pure cubic function
 * @return result of function output
 */
سانچا<قسم_ناں T>
ثابت T expo(ثابت T &value, ثابت T &e)
{
	T x = constrain(value, (T) - 1, (T) 1);
	T ec = constrain(e, (T) 0, (T) 1);
	أرجع (1 - ec) * x + ec * x * x * x;
}

/*
 * So called SuperExpo function implementation.
 * It is a 1/(1-x) function to further shape the rc input curve intuitively.
 * I enhanced it compared to other implementations to keep the scale between [-1,1].
 * @param value [-1,1] input value to function
 * @param e [0,1] function parameter to set ratio between linear and cubic shape (see expo)
 * @param g [0,1) function parameter to set SuperExpo shape
 * 		0 - pure expo function
 * 		0.99 - very strong bent curve, stays zero until maximum stick input
 * @return result of function output
 */
سانچا<قسم_ناں T>
ثابت T superexpo(ثابت T &value, ثابت T &e, ثابت T &g)
{
	T x = constrain(value, (T) - 1, (T) 1);
	T gc = constrain(g, (T) 0, (T) 0.99);
	أرجع expo(x, e) * (1 - gc) / (1 - fabsf(x) * gc);
}

/*
 * Deadzone function being linear and continuous outside of the deadzone
 * 1                ------
 *                /
 *             --
 *           /
 * -1 ------
 *        -1 -dz +dz 1
 * @param value [-1,1] input value to function
 * @param dz [0,1) ratio between deazone and complete span
 * 		0 - no deadzone, linear -1 to 1
 * 		0.5 - deadzone is half of the span [-0.5,0.5]
 * 		0.99 - almost entire span is deadzone
 */
سانچا<قسم_ناں T>
ثابت T deadzone(ثابت T &value, ثابت T &dz)
{
	T x = constrain(value, (T) - 1, (T) 1);
	T dzc = constrain(dz, (T) 0, (T) 0.99);
	// Rescale the input such that we get a piecewise linear function that will be continuous with applied deadzone
	T باہرلا = (x - میٹرکس::sign(x) * dzc) / (1 - dzc);
	// apply the deadzone (values zero around the middle)
	أرجع باہرلا * (fabsf(x) > dzc);
}

سانچا<قسم_ناں T>
ثابت T expo_deadzone(ثابت T &value, ثابت T &e, ثابت T &dz)
{
	أرجع expo(deadzone(value, dz), e);
}


/*
 * Constant, linear, constant function with the two corner points as parameters
 * y_high          -------
 *                /
 *               /
 *              /
 * y_low -------
 *         x_low   x_high
 */
سانچا<قسم_ناں T>
ثابت T interpolate(ثابت T &value, ثابت T &x_کم, ثابت T &x_بلند, ثابت T &y_کم, ثابت T &y_بلند)
{
	إذا (value <= x_کم) {
		أرجع y_کم;

	} وإلا إذا (value > x_بلند) {
		أرجع y_بلند;

	} وإلا {
		/* linear function between the two points */
		T a = (y_بلند - y_کم) / (x_بلند - x_کم);
		T b = y_کم - (a * x_کم);
		أرجع (a * value) + b;
	}
}

/*
 * Constant, piecewise linear, constant function with 1/N size intervalls and N corner points as parameters
 * y[N-1]               -------
 *                     /
 *                   /
 * y[1]            /
 *               /
 *              /
 *             /
 * y[0] -------
 *        0 1/(N-1) 2/(N-1) ... 1
 */
سانچا<قسم_ناں T, size_t N>
ثابت T interpolateN(ثابت T &value, ثابت T(&y)[N])
{
	size_t اشاریہ = constrain((صحيح)(value * (N - 1)), 0, (صحيح)(N - 2));
	أرجع interpolate(value, (T)اشاریہ / (T)(N - 1), (T)(اشاریہ + 1) / (T)(N - 1), y[اشاریہ], y[اشاریہ + 1]);
}

/*
 * Constant, piecewise linear, constant function with N corner points as parameters
 * y[N-1]               -------
 *                     /
 *                   /
 * y[1]            /
 *               /
 *              /
 *             /
 * y[0] -------
 *          x[0] x[1] ... x[N-1]
 * Note: x[N] corner coordinates have to be sorted in ascending order
 */
سانچا<قسم_ناں T, size_t N>
ثابت T interpolateNXY(ثابت T &value, ثابت T(&x)[N], ثابت T(&y)[N])
{
	size_t اشاریہ = 0;

	طالما ((value > x[اشاریہ + 1]) && (اشاریہ < (N - 2))) {
		اشاریہ++;
	}

	أرجع interpolate(value, x[اشاریہ], x[اشاریہ + 1], y[اشاریہ], y[اشاریہ + 1]);
}

/*
 * Squareroot, linear function with fixed corner point at intersection (1,1)
 *                     /
 *      linear        /
 *                   /
 * 1                /
 *                /
 *      sqrt     |
 *              |
 * 0     -------
 *             0    1
 */
سانچا<قسم_ناں T>
ثابت T sqrt_خطی(ثابت T &value)
{
	إذا (value < static_cast<T>(0)) {
		أرجع static_cast<T>(0);

	} وإلا إذا (value < static_cast<T>(1)) {
		أرجع sqrtf(value);

	} وإلا {
		أرجع value;
	}
}

/*
 * Linear interpolation between 2 points a, and b.
 * s=0 return a
 * s=1 returns b
 * Any value for s is valid.
 */
سانچا<قسم_ناں T>
ثابت T lerp(ثابت T &a, ثابت T &b, ثابت T &s)
{
	أرجع (static_cast<T>(1) - s) * a + s * b;
}

سانچا<قسم_ناں T>
constexpr T negate(T value)
{
	static_assert(حجم(T) > 2, "implement for T");
	أرجع -value;
}

سانچا<>
constexpr int16_t negate<int16_t>(int16_t value)
{
	إذا (value == INT16_MAX) {
		أرجع INT16_MIN;

	} وإلا إذا (value == INT16_MIN) {
		أرجع INT16_MAX;
	}

	أرجع -value;
}

/*
 * This function calculates the Hamming weight, i.e. counts the number of bits that are set
 * in a given integer.
 */

سانچا<قسم_ناں T>
صحيح گنتی_سیٹ_بٹ(T n)
{
	أرجع __builtin_popcount(n);
}

سانچا<قسم_ناں T>
صحيح گنتی_Trailing_Zeros(T n)
{
	أرجع n == 0 ? (صحيح)(حجم(T) * 8) : __builtin_ctz(n);
}

مضمن منطقي isFinite(ثابت عائم &value)
{
	أرجع PX4_ISFINITE(value);
}

مضمن منطقي isFinite(ثابت میٹرکس::Vector2f &value)
{
	أرجع value.isAllFinite();
}

مضمن منطقي isFinite(ثابت میٹرکس::Vector3f &value)
{
	أرجع value.isAllFinite();
}

} /* namespace math */
