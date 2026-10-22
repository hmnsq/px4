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

#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ "Limits.hpp"

#ಅನ್ತರ್ಭಾವ <px4_platform_common/defines.h>
#ಅನ್ತರ್ಭಾವ <matrix/matrix/math.hpp>

ನಾಮಾಕಾಶ math
{

// Type-safe signum function with zero treated as positive
ಮಾದರಿ<ಬಗೆ_ಹೆಸರು T>
ಪೂರ್ಣಾಂಕ signNoZero(T val)
{
	ಹಿಂತಿರುಗಿ (T(0) <= val) - (val < T(0));
}

/**
 * Sign function based on a boolean
 *
 * @param[in] positive Truth value to take the sign from
 * @return 1 if positive is true, -1 if positive is false
 */
ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ signFromBool(ತಾರ್ಕಿಕ ಧನಾತ್ಮಕ)
{
	ಹಿಂತಿರುಗಿ ಧನಾತ್ಮಕ ? 1 : -1;
}

ಮಾದರಿ<ಬಗೆ_ಹೆಸರು T>
T sq(T val)
{
	ಹಿಂತಿರುಗಿ val * val;
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
ಮಾದರಿ<ಬಗೆ_ಹೆಸರು T>
ಸ್ಥಿರಾಂಕ T expo(ಸ್ಥಿರಾಂಕ T &value, ಸ್ಥಿರಾಂಕ T &e)
{
	T x = constrain(value, (T) - 1, (T) 1);
	T ec = constrain(e, (T) 0, (T) 1);
	ಹಿಂತಿರುಗಿ (1 - ec) * x + ec * x * x * x;
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
ಮಾದರಿ<ಬಗೆ_ಹೆಸರು T>
ಸ್ಥಿರಾಂಕ T superexpo(ಸ್ಥಿರಾಂಕ T &value, ಸ್ಥಿರಾಂಕ T &e, ಸ್ಥಿರಾಂಕ T &g)
{
	T x = constrain(value, (T) - 1, (T) 1);
	T gc = constrain(g, (T) 0, (T) 0.99);
	ಹಿಂತಿರುಗಿ expo(x, e) * (1 - gc) / (1 - fabsf(x) * gc);
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
ಮಾದರಿ<ಬಗೆ_ಹೆಸರು T>
ಸ್ಥಿರಾಂಕ T deadzone(ಸ್ಥಿರಾಂಕ T &value, ಸ್ಥಿರಾಂಕ T &dz)
{
	T x = constrain(value, (T) - 1, (T) 1);
	T dzc = constrain(dz, (T) 0, (T) 0.99);
	// Rescale the input such that we get a piecewise linear function that will be continuous with applied deadzone
	T ಹೊರಹೋಗು = (x - ಮಾತೃಕೆ::sign(x) * dzc) / (1 - dzc);
	// apply the deadzone (values zero around the middle)
	ಹಿಂತಿರುಗಿ ಹೊರಹೋಗು * (fabsf(x) > dzc);
}

ಮಾದರಿ<ಬಗೆ_ಹೆಸರು T>
ಸ್ಥಿರಾಂಕ T expo_deadzone(ಸ್ಥಿರಾಂಕ T &value, ಸ್ಥಿರಾಂಕ T &e, ಸ್ಥಿರಾಂಕ T &dz)
{
	ಹಿಂತಿರುಗಿ expo(deadzone(value, dz), e);
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
ಮಾದರಿ<ಬಗೆ_ಹೆಸರು T>
ಸ್ಥಿರಾಂಕ T interpolate(ಸ್ಥಿರಾಂಕ T &value, ಸ್ಥಿರಾಂಕ T &x_ಕೆಳಗಿನ, ಸ್ಥಿರಾಂಕ T &x_ಮೇಲಿನ, ಸ್ಥಿರಾಂಕ T &y_ಕೆಳಗಿನ, ಸ್ಥಿರಾಂಕ T &y_ಮೇಲಿನ)
{
	ಆದರೆ (value <= x_ಕೆಳಗಿನ) {
		ಹಿಂತಿರುಗಿ y_ಕೆಳಗಿನ;

	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (value > x_ಮೇಲಿನ) {
		ಹಿಂತಿರುಗಿ y_ಮೇಲಿನ;

	} ಇಲ್ಲದಿದ್ದರೆ {
		/* linear function between the two points */
		T a = (y_ಮೇಲಿನ - y_ಕೆಳಗಿನ) / (x_ಮೇಲಿನ - x_ಕೆಳಗಿನ);
		T b = y_ಕೆಳಗಿನ - (a * x_ಕೆಳಗಿನ);
		ಹಿಂತಿರುಗಿ (a * value) + b;
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
ಮಾದರಿ<ಬಗೆ_ಹೆಸರು T, size_t N>
ಸ್ಥಿರಾಂಕ T interpolateN(ಸ್ಥಿರಾಂಕ T &value, ಸ್ಥಿರಾಂಕ T(&y)[N])
{
	size_t ಸೂಚ್ಯಂಕ = constrain((ಪೂರ್ಣಾಂಕ)(value * (N - 1)), 0, (ಪೂರ್ಣಾಂಕ)(N - 2));
	ಹಿಂತಿರುಗಿ interpolate(value, (T)ಸೂಚ್ಯಂಕ / (T)(N - 1), (T)(ಸೂಚ್ಯಂಕ + 1) / (T)(N - 1), y[ಸೂಚ್ಯಂಕ], y[ಸೂಚ್ಯಂಕ + 1]);
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
ಮಾದರಿ<ಬಗೆ_ಹೆಸರು T, size_t N>
ಸ್ಥಿರಾಂಕ T interpolateNXY(ಸ್ಥಿರಾಂಕ T &value, ಸ್ಥಿರಾಂಕ T(&x)[N], ಸ್ಥಿರಾಂಕ T(&y)[N])
{
	size_t ಸೂಚ್ಯಂಕ = 0;

	ಆಗ ((value > x[ಸೂಚ್ಯಂಕ + 1]) && (ಸೂಚ್ಯಂಕ < (N - 2))) {
		ಸೂಚ್ಯಂಕ++;
	}

	ಹಿಂತಿರುಗಿ interpolate(value, x[ಸೂಚ್ಯಂಕ], x[ಸೂಚ್ಯಂಕ + 1], y[ಸೂಚ್ಯಂಕ], y[ಸೂಚ್ಯಂಕ + 1]);
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
ಮಾದರಿ<ಬಗೆ_ಹೆಸರು T>
ಸ್ಥಿರಾಂಕ T sqrt_ರೇಖೀಯ(ಸ್ಥಿರಾಂಕ T &value)
{
	ಆದರೆ (value < static_cast<T>(0)) {
		ಹಿಂತಿರುಗಿ static_cast<T>(0);

	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (value < static_cast<T>(1)) {
		ಹಿಂತಿರುಗಿ sqrtf(value);

	} ಇಲ್ಲದಿದ್ದರೆ {
		ಹಿಂತಿರುಗಿ value;
	}
}

/*
 * Linear interpolation between 2 points a, and b.
 * s=0 return a
 * s=1 returns b
 * Any value for s is valid.
 */
ಮಾದರಿ<ಬಗೆ_ಹೆಸರು T>
ಸ್ಥಿರಾಂಕ T lerp(ಸ್ಥಿರಾಂಕ T &a, ಸ್ಥಿರಾಂಕ T &b, ಸ್ಥಿರಾಂಕ T &s)
{
	ಹಿಂತಿರುಗಿ (static_cast<T>(1) - s) * a + s * b;
}

ಮಾದರಿ<ಬಗೆ_ಹೆಸರು T>
constexpr T negate(T value)
{
	static_assert(ಗಾತ್ರ(T) > 2, "implement for T");
	ಹಿಂತಿರುಗಿ -value;
}

ಮಾದರಿ<>
constexpr int16_t negate<int16_t>(int16_t value)
{
	ಆದರೆ (value == INT16_MAX) {
		ಹಿಂತಿರುಗಿ INT16_MIN;

	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (value == INT16_MIN) {
		ಹಿಂತಿರುಗಿ INT16_MAX;
	}

	ಹಿಂತಿರುಗಿ -value;
}

/*
 * This function calculates the Hamming weight, i.e. counts the number of bits that are set
 * in a given integer.
 */

ಮಾದರಿ<ಬಗೆ_ಹೆಸರು T>
ಪೂರ್ಣಾಂಕ ಎಣಿಕೆ_ಗಣ_ಬಿಟ್ಟುಗಳು(T n)
{
	ಹಿಂತಿರುಗಿ __builtin_popcount(n);
}

ಮಾದರಿ<ಬಗೆ_ಹೆಸರು T>
ಪೂರ್ಣಾಂಕ ಎಣಿಕೆ_Trailing_Zeros(T n)
{
	ಹಿಂತಿರುಗಿ n == 0 ? (ಪೂರ್ಣಾಂಕ)(ಗಾತ್ರ(T) * 8) : __builtin_ctz(n);
}

ಸಾಲೊಳಗೆ ತಾರ್ಕಿಕ isFinite(ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ &value)
{
	ಹಿಂತಿರುಗಿ PX4_ISFINITE(value);
}

ಸಾಲೊಳಗೆ ತಾರ್ಕಿಕ isFinite(ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::Vector2f &value)
{
	ಹಿಂತಿರುಗಿ value.isAllFinite();
}

ಸಾಲೊಳಗೆ ತಾರ್ಕಿಕ isFinite(ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::Vector3f &value)
{
	ಹಿಂತಿರುಗಿ value.isAllFinite();
}

} /* namespace math */
