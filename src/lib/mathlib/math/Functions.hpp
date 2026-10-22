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

#അന്തര്ഭാവ "Limits.hpp"

#അന്തര്ഭാവ <px4_platform_common/defines.h>
#അന്തര്ഭാവ <matrix/matrix/math.hpp>

നാമാകാശ math
{

// Type-safe signum function with zero treated as positive
template<typename T>
പൂർണ്ണസംഖ്യ signNoZero(T val)
{
	തിരികെ (T(0) <= val) - (val < T(0));
}

/**
 * Sign function based on a boolean
 *
 * @param[in] positive Truth value to take the sign from
 * @return 1 if positive is true, -1 if positive is false
 */
വരിയിൽ പൂർണ്ണസംഖ്യ signFromBool(തർക്കം positive)
{
	തിരികെ positive ? 1 : -1;
}

template<typename T>
T sq(T val)
{
	തിരികെ val * val;
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
സ്ഥിരം T expo(സ്ഥിരം T &value, സ്ഥിരം T &e)
{
	T x = constrain(value, (T) - 1, (T) 1);
	T ec = constrain(e, (T) 0, (T) 1);
	തിരികെ (1 - ec) * x + ec * x * x * x;
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
സ്ഥിരം T superexpo(സ്ഥിരം T &value, സ്ഥിരം T &e, സ്ഥിരം T &g)
{
	T x = constrain(value, (T) - 1, (T) 1);
	T gc = constrain(g, (T) 0, (T) 0.99);
	തിരികെ expo(x, e) * (1 - gc) / (1 - fabsf(x) * gc);
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
സ്ഥിരം T deadzone(സ്ഥിരം T &value, സ്ഥിരം T &dz)
{
	T x = constrain(value, (T) - 1, (T) 1);
	T dzc = constrain(dz, (T) 0, (T) 0.99);
	// Rescale the input such that we get a piecewise linear function that will be continuous with applied deadzone
	T out = (x - matrix::sign(x) * dzc) / (1 - dzc);
	// apply the deadzone (values zero around the middle)
	തിരികെ out * (fabsf(x) > dzc);
}

template<typename T>
സ്ഥിരം T expo_deadzone(സ്ഥിരം T &value, സ്ഥിരം T &e, സ്ഥിരം T &dz)
{
	തിരികെ expo(deadzone(value, dz), e);
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
സ്ഥിരം T interpolate(സ്ഥിരം T &value, സ്ഥിരം T &x_താഴെയുള്ള, സ്ഥിരം T &x_മുകളിലെ, സ്ഥിരം T &y_താഴെയുള്ള, സ്ഥിരം T &y_മുകളിലെ)
{
	എങ്കിൽ (value <= x_താഴെയുള്ള) {
		തിരികെ y_താഴെയുള്ള;

	} അല്ലെങ്കിൽ എങ്കിൽ (value > x_മുകളിലെ) {
		തിരികെ y_മുകളിലെ;

	} അല്ലെങ്കിൽ {
		/* linear function between the two points */
		T a = (y_മുകളിലെ - y_താഴെയുള്ള) / (x_മുകളിലെ - x_താഴെയുള്ള);
		T b = y_താഴെയുള്ള - (a * x_താഴെയുള്ള);
		തിരികെ (a * value) + b;
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
സ്ഥിരം T interpolateN(സ്ഥിരം T &value, സ്ഥിരം T(&y)[N])
{
	size_t index = constrain((പൂർണ്ണസംഖ്യ)(value * (N - 1)), 0, (പൂർണ്ണസംഖ്യ)(N - 2));
	തിരികെ interpolate(value, (T)index / (T)(N - 1), (T)(index + 1) / (T)(N - 1), y[index], y[index + 1]);
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
സ്ഥിരം T interpolateNXY(സ്ഥിരം T &value, സ്ഥിരം T(&x)[N], സ്ഥിരം T(&y)[N])
{
	size_t index = 0;

	ആകുമ്പോൾ ((value > x[index + 1]) && (index < (N - 2))) {
		index++;
	}

	തിരികെ interpolate(value, x[index], x[index + 1], y[index], y[index + 1]);
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
സ്ഥിരം T sqrt_linear(സ്ഥിരം T &value)
{
	എങ്കിൽ (value < static_cast<T>(0)) {
		തിരികെ static_cast<T>(0);

	} അല്ലെങ്കിൽ എങ്കിൽ (value < static_cast<T>(1)) {
		തിരികെ sqrtf(value);

	} അല്ലെങ്കിൽ {
		തിരികെ value;
	}
}

/*
 * Linear interpolation between 2 points a, and b.
 * s=0 return a
 * s=1 returns b
 * Any value for s is valid.
 */
template<typename T>
സ്ഥിരം T lerp(സ്ഥിരം T &a, സ്ഥിരം T &b, സ്ഥിരം T &s)
{
	തിരികെ (static_cast<T>(1) - s) * a + s * b;
}

template<typename T>
constexpr T negate(T value)
{
	static_assert(വലുപ്പം(T) > 2, "implement for T");
	തിരികെ -value;
}

template<>
constexpr int16_t negate<int16_t>(int16_t value)
{
	എങ്കിൽ (value == INT16_MAX) {
		തിരികെ INT16_MIN;

	} അല്ലെങ്കിൽ എങ്കിൽ (value == INT16_MIN) {
		തിരികെ INT16_MAX;
	}

	തിരികെ -value;
}

/*
 * This function calculates the Hamming weight, i.e. counts the number of bits that are set
 * in a given integer.
 */

template<typename T>
പൂർണ്ണസംഖ്യ എണ്ണം_Set_Bits(T n)
{
	തിരികെ __builtin_popcount(n);
}

template<typename T>
പൂർണ്ണസംഖ്യ എണ്ണം_Trailing_Zeros(T n)
{
	തിരികെ n == 0 ? (പൂർണ്ണസംഖ്യ)(വലുപ്പം(T) * 8) : __builtin_ctz(n);
}

വരിയിൽ തർക്കം isFinite(സ്ഥിരം ദശാംശം &value)
{
	തിരികെ PX4_ISFINITE(value);
}

വരിയിൽ തർക്കം isFinite(സ്ഥിരം matrix::Vector2f &value)
{
	തിരികെ value.isAllFinite();
}

വരിയിൽ തർക്കം isFinite(സ്ഥിരം matrix::Vector3f &value)
{
	തിരികെ value.isAllFinite();
}

} /* namespace math */
