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

#pragma once

#অন্তর্ভাব "Limits.hpp"

#অন্তর্ভাব <px4_platform_common/defines.h>
#অন্তর্ভাব <matrix/matrix/math.hpp>

নামাকাশ math
{

// Type-safe signum function with zero treated as positive
template<typename T>
পূর্ণসংখ্যা signNoZero(T val)
{
	ফেরত (T(0) <= val) - (val < T(0));
}

/**
 * Sign function based on a boolean
 *
 * @param[in] positive Truth value to take the sign from
 * @return 1 if positive is true, -1 if positive is false
 */
অন্তঃরেখ পূর্ণসংখ্যা signFromBool(যৌক্তিক positive)
{
	ফেরত positive ? 1 : -1;
}

template<typename T>
T sq(T val)
{
	ফেরত val * val;
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
template<typename T>
ধ্রুবক T expo(ধ্রুবক T &value, ধ্রুবক T &e)
{
	T x = constrain(value, (T) - 1, (T) 1);
	T ec = constrain(e, (T) 0, (T) 1);
	ফেরত (1 - ec) * x + ec * x * x * x;
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
template<typename T>
ধ্রুবক T superexpo(ধ্রুবক T &value, ধ্রুবক T &e, ধ্রুবক T &g)
{
	T x = constrain(value, (T) - 1, (T) 1);
	T gc = constrain(g, (T) 0, (T) 0.99);
	ফেরত expo(x, e) * (1 - gc) / (1 - fabsf(x) * gc);
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
template<typename T>
ধ্রুবক T deadzone(ধ্রুবক T &value, ধ্রুবক T &dz)
{
	T x = constrain(value, (T) - 1, (T) 1);
	T dzc = constrain(dz, (T) 0, (T) 0.99);
	// Rescale the input such that we get a piecewise linear function that will be continuous with applied deadzone
	T out = (x - matrix::sign(x) * dzc) / (1 - dzc);
	// apply the deadzone (values zero around the middle)
	ফেরত out * (fabsf(x) > dzc);
}

template<typename T>
ধ্রুবক T expo_deadzone(ধ্রুবক T &value, ধ্রুবক T &e, ধ্রুবক T &dz)
{
	ফেরত expo(deadzone(value, dz), e);
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
template<typename T>
ধ্রুবক T interpolate(ধ্রুবক T &value, ধ্রুবক T &x_নিচের, ধ্রুবক T &x_উপরের, ধ্রুবক T &y_নিচের, ধ্রুবক T &y_উপরের)
{
	যদি (value <= x_নিচের) {
		ফেরত y_নিচের;

	} অন্যথা যদি (value > x_উপরের) {
		ফেরত y_উপরের;

	} অন্যথা {
		/* linear function between the two points */
		T a = (y_উপরের - y_নিচের) / (x_উপরের - x_নিচের);
		T b = y_নিচের - (a * x_নিচের);
		ফেরত (a * value) + b;
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
template<typename T, size_t N>
ধ্রুবক T interpolateN(ধ্রুবক T &value, ধ্রুবক T(&y)[N])
{
	size_t সূচক = constrain((পূর্ণসংখ্যা)(value * (N - 1)), 0, (পূর্ণসংখ্যা)(N - 2));
	ফেরত interpolate(value, (T)সূচক / (T)(N - 1), (T)(সূচক + 1) / (T)(N - 1), y[সূচক], y[সূচক + 1]);
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
template<typename T, size_t N>
ধ্রুবক T interpolateNXY(ধ্রুবক T &value, ধ্রুবক T(&x)[N], ধ্রুবক T(&y)[N])
{
	size_t সূচক = 0;

	যখন ((value > x[সূচক + 1]) && (সূচক < (N - 2))) {
		সূচক++;
	}

	ফেরত interpolate(value, x[সূচক], x[সূচক + 1], y[সূচক], y[সূচক + 1]);
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
template<typename T>
ধ্রুবক T sqrt_linear(ধ্রুবক T &value)
{
	যদি (value < static_cast<T>(0)) {
		ফেরত static_cast<T>(0);

	} অন্যথা যদি (value < static_cast<T>(1)) {
		ফেরত sqrtf(value);

	} অন্যথা {
		ফেরত value;
	}
}

/*
 * Linear interpolation between 2 points a, and b.
 * s=0 return a
 * s=1 returns b
 * Any value for s is valid.
 */
template<typename T>
ধ্রুবক T lerp(ধ্রুবক T &a, ধ্রুবক T &b, ধ্রুবক T &s)
{
	ফেরত (static_cast<T>(1) - s) * a + s * b;
}

template<typename T>
constexpr T negate(T value)
{
	static_assert(আকার(T) > 2, "implement for T");
	ফেরত -value;
}

template<>
constexpr int16_t negate<int16_t>(int16_t value)
{
	যদি (value == INT16_MAX) {
		ফেরত INT16_MIN;

	} অন্যথা যদি (value == INT16_MIN) {
		ফেরত INT16_MAX;
	}

	ফেরত -value;
}

/*
 * This function calculates the Hamming weight, i.e. counts the number of bits that are set
 * in a given integer.
 */

template<typename T>
পূর্ণসংখ্যা গুনতি_Set_Bits(T n)
{
	ফেরত __builtin_popcount(n);
}

template<typename T>
পূর্ণসংখ্যা গুনতি_Trailing_Zeros(T n)
{
	ফেরত n == 0 ? (পূর্ণসংখ্যা)(আকার(T) * 8) : __builtin_ctz(n);
}

অন্তঃরেখ যৌক্তিক isFinite(ধ্রুবক দশমিক &value)
{
	ফেরত PX4_ISFINITE(value);
}

অন্তঃরেখ যৌক্তিক isFinite(ধ্রুবক matrix::Vector2f &value)
{
	ফেরত value.isAllFinite();
}

অন্তঃরেখ যৌক্তিক isFinite(ধ্রুবক matrix::Vector3f &value)
{
	ফেরত value.isAllFinite();
}

} /* namespace math */
