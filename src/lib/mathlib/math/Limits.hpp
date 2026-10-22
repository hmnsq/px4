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

#અન્તર્ભાવ <float.h>
#અન્તર્ભાવ <math.h>
#અન્તર્ભાવ <stdint.h>

#ifndef MATH_PI
#define MATH_PI		3.141592653589793238462643383280
#endif

નામાકાશ math
{

#જો defined(__PX4_QURT)
નામાકાશ HexagonDSPMath
{
સ્થિર અંતઃરેખ constexpr તાર્કિક isNan(દશાંશ val) { પાછા __builtin_isnan(val); }
સ્થિર અંતઃરેખ constexpr તાર્કિક isNan(દ્વિગુણ val) { પાછા __builtin_isnan(val); }
સ્થિર અંતઃરેખ constexpr તાર્કિક isNan(long દ્વિગુણ val) { પાછા __builtin_isnan(val); }

template<typename _Tp>
સ્થિર અંતઃરેખ constexpr તાર્કિક isNan(_Tp)
{
	પાછા ખોટું;
}
} // namespace HexagonDSPMath
#endif // __PX4_QURT

template<typename _Tp>
constexpr _Tp min(_Tp a, _Tp b)
{
#જો defined(__PX4_QURT)

	// The Hexagon compiler can lower this pattern to sfmin, which does not preserve NaN semantics.
	જો (HexagonDSPMath::isNan(a) || HexagonDSPMath::isNan(b)) {
		પાછા b;
	}

#endif // __PX4_QURT

	પાછા (a < b) ? a : b;
}

template<typename _Tp>
constexpr _Tp min(_Tp a, _Tp b, _Tp c)
{
	પાછા min(min(a, b), c);
}

template<typename _Tp>
constexpr _Tp max(_Tp a, _Tp b)
{
#જો defined(__PX4_QURT)

	// The Hexagon compiler can lower this pattern to sfmax, which does not preserve NaN semantics.
	જો (HexagonDSPMath::isNan(a) || HexagonDSPMath::isNan(b)) {
		પાછા b;
	}

#endif // __PX4_QURT

	પાછા (a > b) ? a : b;
}

template<typename _Tp>
constexpr _Tp max(_Tp a, _Tp b, _Tp c)
{
	પાછા max(max(a, b), c);
}

template<typename _Tp>
constexpr _Tp constrain(_Tp val, _Tp min_val, _Tp max_val)
{
#જો defined(__PX4_QURT)

	// The Hexagon compiler lowers this clamp pattern to sfmin, which does not preserve NaN semantics.
	જો (HexagonDSPMath::isNan(val)) {
		પાછા val;
	}

#endif // __PX4_QURT

	પાછા (val < min_val) ? min_val : ((val > max_val) ? max_val : val);
}

/** Constrain float values to valid values for int16_t.
 * Invalid values are just clipped to be in the range for int16_t. */
constexpr int16_t constrainFloatToInt16(દશાંશ value)
{
	પાછા (int16_t)math::constrain(value, (દશાંશ)INT16_MIN, (દશાંશ)INT16_MAX);
}

template<typename _Tp>
constexpr તાર્કિક isInRange(_Tp val, _Tp min_val, _Tp max_val)
{
	પાછા (min_val <= val) && (val <= max_val);
}

template<typename T>
constexpr T radians(T degrees)
{
	પાછા degrees * (static_cast<T>(MATH_PI) / static_cast<T>(180));
}

template<typename T>
constexpr T degrees(T radians)
{
	પાછા radians * (static_cast<T>(180) / static_cast<T>(MATH_PI));
}

/** Safe way to check if float is zero */
અંતઃરેખ તાર્કિક isZero(દશાંશ val)
{
	પાછા fabsf(val - 0.0f) < FLT_EPSILON;
}

/** Safe way to check if double is zero */
અંતઃરેખ તાર્કિક isZero(દ્વિગુણ val)
{
	પાછા fabs(val - 0.0) < DBL_EPSILON;
}

}
