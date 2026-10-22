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

#अन्तर्भाव "Limits.hpp"

#अन्तर्भाव <px4_platform_common/defines.h>
#अन्तर्भाव <matrix/matrix/math.hpp>

नामाकाश math
{

// Type-safe signum function with zero treated as positive
template<typename T>
पूर्णाङ्क signNoZero(T val)
{
	प्रत्यागम (T(0) <= val) - (val < T(0));
}

/**
 * Sign function based on a boolean
 *
 * @param[in] positive Truth value to take the sign from
 * @return 1 if positive is true, -1 if positive is false
 */
अन्तःस्थ पूर्णाङ्क signFromBool(तार्किक positive)
{
	प्रत्यागम positive ? 1 : -1;
}

template<typename T>
T sq(T val)
{
	प्रत्यागम val * val;
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
ध्रुव T expo(ध्रुव T &value, ध्रुव T &e)
{
	T x = constrain(value, (T) - 1, (T) 1);
	T ec = constrain(e, (T) 0, (T) 1);
	प्रत्यागम (1 - ec) * x + ec * x * x * x;
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
ध्रुव T superexpo(ध्रुव T &value, ध्रुव T &e, ध्रुव T &g)
{
	T x = constrain(value, (T) - 1, (T) 1);
	T gc = constrain(g, (T) 0, (T) 0.99);
	प्रत्यागम expo(x, e) * (1 - gc) / (1 - fabsf(x) * gc);
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
ध्रुव T deadzone(ध्रुव T &value, ध्रुव T &dz)
{
	T x = constrain(value, (T) - 1, (T) 1);
	T dzc = constrain(dz, (T) 0, (T) 0.99);
	// Rescale the input such that we get a piecewise linear function that will be continuous with applied deadzone
	T out = (x - matrix::sign(x) * dzc) / (1 - dzc);
	// apply the deadzone (values zero around the middle)
	प्रत्यागम out * (fabsf(x) > dzc);
}

template<typename T>
ध्रुव T expo_deadzone(ध्रुव T &value, ध्रुव T &e, ध्रुव T &dz)
{
	प्रत्यागम expo(deadzone(value, dz), e);
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
ध्रुव T interpolate(ध्रुव T &value, ध्रुव T &x_नीचः, ध्रुव T &x_उच्चः, ध्रुव T &y_नीचः, ध्रुव T &y_उच्चः)
{
	जइ (value <= x_नीचः) {
		प्रत्यागम y_नीचः;

	} अण्णहा जइ (value > x_उच्चः) {
		प्रत्यागम y_उच्चः;

	} अण्णहा {
		/* linear function between the two points */
		T a = (y_उच्चः - y_नीचः) / (x_उच्चः - x_नीचः);
		T b = y_नीचः - (a * x_नीचः);
		प्रत्यागम (a * value) + b;
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
ध्रुव T interpolateN(ध्रुव T &value, ध्रुव T(&y)[N])
{
	size_t index = constrain((पूर्णाङ्क)(value * (N - 1)), 0, (पूर्णाङ्क)(N - 2));
	प्रत्यागम interpolate(value, (T)index / (T)(N - 1), (T)(index + 1) / (T)(N - 1), y[index], y[index + 1]);
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
ध्रुव T interpolateNXY(ध्रुव T &value, ध्रुव T(&x)[N], ध्रुव T(&y)[N])
{
	size_t index = 0;

	जाव ((value > x[index + 1]) && (index < (N - 2))) {
		index++;
	}

	प्रत्यागम interpolate(value, x[index], x[index + 1], y[index], y[index + 1]);
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
ध्रुव T sqrt_linear(ध्रुव T &value)
{
	जइ (value < static_cast<T>(0)) {
		प्रत्यागम static_cast<T>(0);

	} अण्णहा जइ (value < static_cast<T>(1)) {
		प्रत्यागम sqrtf(value);

	} अण्णहा {
		प्रत्यागम value;
	}
}

/*
 * Linear interpolation between 2 points a, and b.
 * s=0 return a
 * s=1 returns b
 * Any value for s is valid.
 */
template<typename T>
ध्रुव T lerp(ध्रुव T &a, ध्रुव T &b, ध्रुव T &s)
{
	प्रत्यागम (static_cast<T>(1) - s) * a + s * b;
}

template<typename T>
constexpr T negate(T value)
{
	static_assert(परिमाण(T) > 2, "implement for T");
	प्रत्यागम -value;
}

template<>
constexpr int16_t negate<int16_t>(int16_t value)
{
	जइ (value == INT16_MAX) {
		प्रत्यागम INT16_MIN;

	} अण्णहा जइ (value == INT16_MIN) {
		प्रत्यागम INT16_MAX;
	}

	प्रत्यागम -value;
}

/*
 * This function calculates the Hamming weight, i.e. counts the number of bits that are set
 * in a given integer.
 */

template<typename T>
पूर्णाङ्क गणनम्_Set_Bits(T n)
{
	प्रत्यागम __builtin_popcount(n);
}

template<typename T>
पूर्णाङ्क गणनम्_Trailing_Zeros(T n)
{
	प्रत्यागम n == 0 ? (पूर्णाङ्क)(परिमाण(T) * 8) : __builtin_ctz(n);
}

अन्तःस्थ तार्किक isFinite(ध्रुव भिन्न &value)
{
	प्रत्यागम PX4_ISFINITE(value);
}

अन्तःस्थ तार्किक isFinite(ध्रुव matrix::Vector2f &value)
{
	प्रत्यागम value.isAllFinite();
}

अन्तःस्थ तार्किक isFinite(ध्रुव matrix::Vector3f &value)
{
	प्रत्यागम value.isAllFinite();
}

} /* namespace math */
