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
ढाँचा<typename T>
पूर्णांक signNoZero(T val)
{
	परत (T(0) <= val) - (val < T(0));
}

/**
 * Sign function based on a boolean
 *
 * @param[in] positive Truth value to take the sign from
 * @return 1 if positive is true, -1 if positive is false
 */
ओळीत पूर्णांक signFromBool(तार्किक positive)
{
	परत positive ? 1 : -1;
}

ढाँचा<typename T>
T sq(T val)
{
	परत val * val;
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
ढाँचा<typename T>
स्थिरांक T expo(स्थिरांक T &value, स्थिरांक T &e)
{
	T x = constrain(value, (T) - 1, (T) 1);
	T ec = constrain(e, (T) 0, (T) 1);
	परत (1 - ec) * x + ec * x * x * x;
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
ढाँचा<typename T>
स्थिरांक T superexpo(स्थिरांक T &value, स्थिरांक T &e, स्थिरांक T &g)
{
	T x = constrain(value, (T) - 1, (T) 1);
	T gc = constrain(g, (T) 0, (T) 0.99);
	परत expo(x, e) * (1 - gc) / (1 - fabsf(x) * gc);
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
ढाँचा<typename T>
स्थिरांक T deadzone(स्थिरांक T &value, स्थिरांक T &dz)
{
	T x = constrain(value, (T) - 1, (T) 1);
	T dzc = constrain(dz, (T) 0, (T) 0.99);
	// Rescale the input such that we get a piecewise linear function that will be continuous with applied deadzone
	T out = (x - matrix::sign(x) * dzc) / (1 - dzc);
	// apply the deadzone (values zero around the middle)
	परत out * (fabsf(x) > dzc);
}

ढाँचा<typename T>
स्थिरांक T expo_deadzone(स्थिरांक T &value, स्थिरांक T &e, स्थिरांक T &dz)
{
	परत expo(deadzone(value, dz), e);
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
ढाँचा<typename T>
स्थिरांक T interpolate(स्थिरांक T &value, स्थिरांक T &x_खालचा, स्थिरांक T &x_वरचा, स्थिरांक T &y_खालचा, स्थिरांक T &y_वरचा)
{
	जर (value <= x_खालचा) {
		परत y_खालचा;

	} अन्यथा जर (value > x_वरचा) {
		परत y_वरचा;

	} अन्यथा {
		/* linear function between the two points */
		T a = (y_वरचा - y_खालचा) / (x_वरचा - x_खालचा);
		T b = y_खालचा - (a * x_खालचा);
		परत (a * value) + b;
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
ढाँचा<typename T, माप_प्रकार N>
स्थिरांक T interpolateN(स्थिरांक T &value, स्थिरांक T(&y)[N])
{
	माप_प्रकार index = constrain((पूर्णांक)(value * (N - 1)), 0, (पूर्णांक)(N - 2));
	परत interpolate(value, (T)index / (T)(N - 1), (T)(index + 1) / (T)(N - 1), y[index], y[index + 1]);
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
ढाँचा<typename T, माप_प्रकार N>
स्थिरांक T interpolateNXY(स्थिरांक T &value, स्थिरांक T(&x)[N], स्थिरांक T(&y)[N])
{
	माप_प्रकार index = 0;

	पर्यंत ((value > x[index + 1]) && (index < (N - 2))) {
		index++;
	}

	परत interpolate(value, x[index], x[index + 1], y[index], y[index + 1]);
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
ढाँचा<typename T>
स्थिरांक T sqrt_linear(स्थिरांक T &value)
{
	जर (value < static_cast<T>(0)) {
		परत static_cast<T>(0);

	} अन्यथा जर (value < static_cast<T>(1)) {
		परत sqrtf(value);

	} अन्यथा {
		परत value;
	}
}

/*
 * Linear interpolation between 2 points a, and b.
 * s=0 return a
 * s=1 returns b
 * Any value for s is valid.
 */
ढाँचा<typename T>
स्थिरांक T lerp(स्थिरांक T &a, स्थिरांक T &b, स्थिरांक T &s)
{
	परत (static_cast<T>(1) - s) * a + s * b;
}

ढाँचा<typename T>
constexpr T negate(T value)
{
	static_assert(आकार(T) > 2, "implement for T");
	परत -value;
}

ढाँचा<>
constexpr int16_t negate<int16_t>(int16_t value)
{
	जर (value == INT16_MAX) {
		परत INT16_MIN;

	} अन्यथा जर (value == INT16_MIN) {
		परत INT16_MAX;
	}

	परत -value;
}

/*
 * This function calculates the Hamming weight, i.e. counts the number of bits that are set
 * in a given integer.
 */

ढाँचा<typename T>
पूर्णांक मोजणी_Set_Bits(T n)
{
	परत __builtin_popcount(n);
}

ढाँचा<typename T>
पूर्णांक मोजणी_Trailing_Zeros(T n)
{
	परत n == 0 ? (पूर्णांक)(आकार(T) * 8) : __builtin_ctz(n);
}

ओळीत तार्किक isFinite(स्थिरांक दशांश &value)
{
	परत PX4_ISFINITE(value);
}

ओळीत तार्किक isFinite(स्थिरांक matrix::Vector2f &value)
{
	परत value.isAllFinite();
}

ओळीत तार्किक isFinite(स्थिरांक matrix::Vector3f &value)
{
	परत value.isAllFinite();
}

} /* namespace math */
