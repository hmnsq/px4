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

#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ "Limits.hpp"

#ਅਨ੍ਤਰ੍ਭਾਵ <px4_platform_common/defines.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <matrix/matrix/math.hpp>

ਨਾਮਾਕਾਸ਼ math
{

// Type-safe signum function with zero treated as positive
ਸਾਂਚਾ<ਕਿਸਮ_ਨਾਂ T>
ਪੂਰਨ signNoZero(T val)
{
	ਵਾਪਸ (T(0) <= val) - (val < T(0));
}

/**
 * Sign function based on a boolean
 *
 * @param[in] positive Truth value to take the sign from
 * @return 1 if positive is true, -1 if positive is false
 */
ਸਤਰਅੰਦਰ ਪੂਰਨ signFromBool(ਤਾਰਕਿਕ ਸਕਾਰਾਤਮਕ)
{
	ਵਾਪਸ ਸਕਾਰਾਤਮਕ ? 1 : -1;
}

ਸਾਂਚਾ<ਕਿਸਮ_ਨਾਂ T>
T sq(T val)
{
	ਵਾਪਸ val * val;
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
ਸਾਂਚਾ<ਕਿਸਮ_ਨਾਂ T>
ਅਚਲ T expo(ਅਚਲ T &value, ਅਚਲ T &e)
{
	T x = constrain(value, (T) - 1, (T) 1);
	T ec = constrain(e, (T) 0, (T) 1);
	ਵਾਪਸ (1 - ec) * x + ec * x * x * x;
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
ਸਾਂਚਾ<ਕਿਸਮ_ਨਾਂ T>
ਅਚਲ T superexpo(ਅਚਲ T &value, ਅਚਲ T &e, ਅਚਲ T &g)
{
	T x = constrain(value, (T) - 1, (T) 1);
	T gc = constrain(g, (T) 0, (T) 0.99);
	ਵਾਪਸ expo(x, e) * (1 - gc) / (1 - fabsf(x) * gc);
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
ਸਾਂਚਾ<ਕਿਸਮ_ਨਾਂ T>
ਅਚਲ T deadzone(ਅਚਲ T &value, ਅਚਲ T &dz)
{
	T x = constrain(value, (T) - 1, (T) 1);
	T dzc = constrain(dz, (T) 0, (T) 0.99);
	// Rescale the input such that we get a piecewise linear function that will be continuous with applied deadzone
	T ਬਾਹਰਲਾ = (x - ਮੈਟ੍ਰਿਕਸ::sign(x) * dzc) / (1 - dzc);
	// apply the deadzone (values zero around the middle)
	ਵਾਪਸ ਬਾਹਰਲਾ * (fabsf(x) > dzc);
}

ਸਾਂਚਾ<ਕਿਸਮ_ਨਾਂ T>
ਅਚਲ T expo_deadzone(ਅਚਲ T &value, ਅਚਲ T &e, ਅਚਲ T &dz)
{
	ਵਾਪਸ expo(deadzone(value, dz), e);
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
ਸਾਂਚਾ<ਕਿਸਮ_ਨਾਂ T>
ਅਚਲ T interpolate(ਅਚਲ T &value, ਅਚਲ T &x_ਨੀਵਾਂ, ਅਚਲ T &x_ਉੱਚਾ, ਅਚਲ T &y_ਨੀਵਾਂ, ਅਚਲ T &y_ਉੱਚਾ)
{
	ਜੇ (value <= x_ਨੀਵਾਂ) {
		ਵਾਪਸ y_ਨੀਵਾਂ;

	} ਨਹੀਂਤਾਂ ਜੇ (value > x_ਉੱਚਾ) {
		ਵਾਪਸ y_ਉੱਚਾ;

	} ਨਹੀਂਤਾਂ {
		/* linear function between the two points */
		T a = (y_ਉੱਚਾ - y_ਨੀਵਾਂ) / (x_ਉੱਚਾ - x_ਨੀਵਾਂ);
		T b = y_ਨੀਵਾਂ - (a * x_ਨੀਵਾਂ);
		ਵਾਪਸ (a * value) + b;
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
ਸਾਂਚਾ<ਕਿਸਮ_ਨਾਂ T, size_t N>
ਅਚਲ T interpolateN(ਅਚਲ T &value, ਅਚਲ T(&y)[N])
{
	size_t ਸੂਚਕਾਂਕ = constrain((ਪੂਰਨ)(value * (N - 1)), 0, (ਪੂਰਨ)(N - 2));
	ਵਾਪਸ interpolate(value, (T)ਸੂਚਕਾਂਕ / (T)(N - 1), (T)(ਸੂਚਕਾਂਕ + 1) / (T)(N - 1), y[ਸੂਚਕਾਂਕ], y[ਸੂਚਕਾਂਕ + 1]);
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
ਸਾਂਚਾ<ਕਿਸਮ_ਨਾਂ T, size_t N>
ਅਚਲ T interpolateNXY(ਅਚਲ T &value, ਅਚਲ T(&x)[N], ਅਚਲ T(&y)[N])
{
	size_t ਸੂਚਕਾਂਕ = 0;

	ਜਦੋਂ ((value > x[ਸੂਚਕਾਂਕ + 1]) && (ਸੂਚਕਾਂਕ < (N - 2))) {
		ਸੂਚਕਾਂਕ++;
	}

	ਵਾਪਸ interpolate(value, x[ਸੂਚਕਾਂਕ], x[ਸੂਚਕਾਂਕ + 1], y[ਸੂਚਕਾਂਕ], y[ਸੂਚਕਾਂਕ + 1]);
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
ਸਾਂਚਾ<ਕਿਸਮ_ਨਾਂ T>
ਅਚਲ T sqrt_ਰੇਖਿਕ(ਅਚਲ T &value)
{
	ਜੇ (value < static_cast<T>(0)) {
		ਵਾਪਸ static_cast<T>(0);

	} ਨਹੀਂਤਾਂ ਜੇ (value < static_cast<T>(1)) {
		ਵਾਪਸ sqrtf(value);

	} ਨਹੀਂਤਾਂ {
		ਵਾਪਸ value;
	}
}

/*
 * Linear interpolation between 2 points a, and b.
 * s=0 return a
 * s=1 returns b
 * Any value for s is valid.
 */
ਸਾਂਚਾ<ਕਿਸਮ_ਨਾਂ T>
ਅਚਲ T lerp(ਅਚਲ T &a, ਅਚਲ T &b, ਅਚਲ T &s)
{
	ਵਾਪਸ (static_cast<T>(1) - s) * a + s * b;
}

ਸਾਂਚਾ<ਕਿਸਮ_ਨਾਂ T>
constexpr T negate(T value)
{
	static_assert(ਆਕਾਰ(T) > 2, "implement for T");
	ਵਾਪਸ -value;
}

ਸਾਂਚਾ<>
constexpr int16_t negate<int16_t>(int16_t value)
{
	ਜੇ (value == INT16_MAX) {
		ਵਾਪਸ INT16_MIN;

	} ਨਹੀਂਤਾਂ ਜੇ (value == INT16_MIN) {
		ਵਾਪਸ INT16_MAX;
	}

	ਵਾਪਸ -value;
}

/*
 * This function calculates the Hamming weight, i.e. counts the number of bits that are set
 * in a given integer.
 */

ਸਾਂਚਾ<ਕਿਸਮ_ਨਾਂ T>
ਪੂਰਨ ਗਿਣਤੀ_ਸੈੱਟ_ਬਿੱਟ(T n)
{
	ਵਾਪਸ __builtin_popcount(n);
}

ਸਾਂਚਾ<ਕਿਸਮ_ਨਾਂ T>
ਪੂਰਨ ਗਿਣਤੀ_Trailing_Zeros(T n)
{
	ਵਾਪਸ n == 0 ? (ਪੂਰਨ)(ਆਕਾਰ(T) * 8) : __builtin_ctz(n);
}

ਸਤਰਅੰਦਰ ਤਾਰਕਿਕ isFinite(ਅਚਲ ਦਸ਼ਮਲਵ &value)
{
	ਵਾਪਸ PX4_ISFINITE(value);
}

ਸਤਰਅੰਦਰ ਤਾਰਕਿਕ isFinite(ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector2f &value)
{
	ਵਾਪਸ value.isAllFinite();
}

ਸਤਰਅੰਦਰ ਤਾਰਕਿਕ isFinite(ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector3f &value)
{
	ਵਾਪਸ value.isAllFinite();
}

} /* namespace math */
