/****************************************************************************
 *
 *   Copyright (c) 2013 PX4 Development Team. All rights reserved.
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
 * @file Limits.hpp
 *
 * Limiting / constrain helper functions
 */

#pragma once

#ଅନ୍ତର୍ଭାଵ <float.h>
#ଅନ୍ତର୍ଭାଵ <math.h>
#ଅନ୍ତର୍ଭାଵ <stdint.h>

#ifndef MATH_PI
#define MATH_PI		3.141592653589793238462643383280
#endif

ନାମାକାଶ math
{

#ଯଦି defined(__PX4_QURT)
ନାମାକାଶ HexagonDSPMath
{
ସ୍ଥିର ଧାଡ଼ିଭିତରେ constexpr ତାର୍କିକ isNan(ଦଶମିକ val) { ଫେରନ୍ତୁ __builtin_isnan(val); }
ସ୍ଥିର ଧାଡ଼ିଭିତରେ constexpr ତାର୍କିକ isNan(ଦ୍ୱିଗୁଣ val) { ଫେରନ୍ତୁ __builtin_isnan(val); }
ସ୍ଥିର ଧାଡ଼ିଭିତରେ constexpr ତାର୍କିକ isNan(long ଦ୍ୱିଗୁଣ val) { ଫେରନ୍ତୁ __builtin_isnan(val); }

template<typename _Tp>
ସ୍ଥିର ଧାଡ଼ିଭିତରେ constexpr ତାର୍କିକ isNan(_Tp)
{
	ଫେରନ୍ତୁ ମିଥ୍ୟା;
}
} // namespace HexagonDSPMath
#endif // __PX4_QURT

template<typename _Tp>
constexpr _Tp min(_Tp a, _Tp b)
{
#ଯଦି defined(__PX4_QURT)

	// The Hexagon compiler can lower this pattern to sfmin, which does not preserve NaN semantics.
	ଯଦି (HexagonDSPMath::isNan(a) || HexagonDSPMath::isNan(b)) {
		ଫେରନ୍ତୁ b;
	}

#endif // __PX4_QURT

	ଫେରନ୍ତୁ (a < b) ? a : b;
}

template<typename _Tp>
constexpr _Tp min(_Tp a, _Tp b, _Tp c)
{
	ଫେରନ୍ତୁ min(min(a, b), c);
}

template<typename _Tp>
constexpr _Tp max(_Tp a, _Tp b)
{
#ଯଦି defined(__PX4_QURT)

	// The Hexagon compiler can lower this pattern to sfmax, which does not preserve NaN semantics.
	ଯଦି (HexagonDSPMath::isNan(a) || HexagonDSPMath::isNan(b)) {
		ଫେରନ୍ତୁ b;
	}

#endif // __PX4_QURT

	ଫେରନ୍ତୁ (a > b) ? a : b;
}

template<typename _Tp>
constexpr _Tp max(_Tp a, _Tp b, _Tp c)
{
	ଫେରନ୍ତୁ max(max(a, b), c);
}

template<typename _Tp>
constexpr _Tp constrain(_Tp val, _Tp min_val, _Tp max_val)
{
#ଯଦି defined(__PX4_QURT)

	// The Hexagon compiler lowers this clamp pattern to sfmin, which does not preserve NaN semantics.
	ଯଦି (HexagonDSPMath::isNan(val)) {
		ଫେରନ୍ତୁ val;
	}

#endif // __PX4_QURT

	ଫେରନ୍ତୁ (val < min_val) ? min_val : ((val > max_val) ? max_val : val);
}

/** Constrain float values to valid values for int16_t.
 * Invalid values are just clipped to be in the range for int16_t. */
constexpr int16_t constrainFloatToInt16(ଦଶମିକ value)
{
	ଫେରନ୍ତୁ (int16_t)math::constrain(value, (ଦଶମିକ)INT16_MIN, (ଦଶମିକ)INT16_MAX);
}

template<typename _Tp>
constexpr ତାର୍କିକ isInRange(_Tp val, _Tp min_val, _Tp max_val)
{
	ଫେରନ୍ତୁ (min_val <= val) && (val <= max_val);
}

template<typename T>
constexpr T radians(T degrees)
{
	ଫେରନ୍ତୁ degrees * (static_cast<T>(MATH_PI) / static_cast<T>(180));
}

template<typename T>
constexpr T degrees(T radians)
{
	ଫେରନ୍ତୁ radians * (static_cast<T>(180) / static_cast<T>(MATH_PI));
}

/** Safe way to check if float is zero */
ଧାଡ଼ିଭିତରେ ତାର୍କିକ isZero(ଦଶମିକ val)
{
	ଫେରନ୍ତୁ fabsf(val - 0.0f) < FLT_EPSILON;
}

/** Safe way to check if double is zero */
ଧାଡ଼ିଭିତରେ ତାର୍କିକ isZero(ଦ୍ୱିଗୁଣ val)
{
	ଫେରନ୍ତୁ fabs(val - 0.0) < DBL_EPSILON;
}

}
