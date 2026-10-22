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

#अन्तर्भाव <float.h>
#अन्तर्भाव <math.h>
#अन्तर्भाव <stdint.h>

#ifndef MATH_PI
#define MATH_PI		3.141592653589793238462643383280
#endif

नामाकाश math
{

#जर defined(__PX4_QURT)
नामाकाश HexagonDSPMath
{
स्थिर ओळीत constexpr तार्किक isNan(दशांश val) { परत __builtin_isnan(val); }
स्थिर ओळीत constexpr तार्किक isNan(द्विगुण val) { परत __builtin_isnan(val); }
स्थिर ओळीत constexpr तार्किक isNan(दीर्घ द्विगुण val) { परत __builtin_isnan(val); }

ढाँचा<typename _Tp>
स्थिर ओळीत constexpr तार्किक isNan(_Tp)
{
	परत असत्य;
}
} // namespace HexagonDSPMath
#endif // __PX4_QURT

ढाँचा<typename _Tp>
constexpr _Tp min(_Tp a, _Tp b)
{
#जर defined(__PX4_QURT)

	// The Hexagon compiler can lower this pattern to sfmin, which does not preserve NaN semantics.
	जर (HexagonDSPMath::isNan(a) || HexagonDSPMath::isNan(b)) {
		परत b;
	}

#endif // __PX4_QURT

	परत (a < b) ? a : b;
}

ढाँचा<typename _Tp>
constexpr _Tp min(_Tp a, _Tp b, _Tp c)
{
	परत min(min(a, b), c);
}

ढाँचा<typename _Tp>
constexpr _Tp max(_Tp a, _Tp b)
{
#जर defined(__PX4_QURT)

	// The Hexagon compiler can lower this pattern to sfmax, which does not preserve NaN semantics.
	जर (HexagonDSPMath::isNan(a) || HexagonDSPMath::isNan(b)) {
		परत b;
	}

#endif // __PX4_QURT

	परत (a > b) ? a : b;
}

ढाँचा<typename _Tp>
constexpr _Tp max(_Tp a, _Tp b, _Tp c)
{
	परत max(max(a, b), c);
}

ढाँचा<typename _Tp>
constexpr _Tp constrain(_Tp val, _Tp min_val, _Tp max_val)
{
#जर defined(__PX4_QURT)

	// The Hexagon compiler lowers this clamp pattern to sfmin, which does not preserve NaN semantics.
	जर (HexagonDSPMath::isNan(val)) {
		परत val;
	}

#endif // __PX4_QURT

	परत (val < min_val) ? min_val : ((val > max_val) ? max_val : val);
}

/** Constrain float values to valid values for int16_t.
 * Invalid values are just clipped to be in the range for int16_t. */
constexpr int16_t constrainFloatToInt16(दशांश value)
{
	परत (int16_t)math::constrain(value, (दशांश)INT16_MIN, (दशांश)INT16_MAX);
}

ढाँचा<typename _Tp>
constexpr तार्किक isInRange(_Tp val, _Tp min_val, _Tp max_val)
{
	परत (min_val <= val) && (val <= max_val);
}

ढाँचा<typename T>
constexpr T radians(T degrees)
{
	परत degrees * (static_cast<T>(MATH_PI) / static_cast<T>(180));
}

ढाँचा<typename T>
constexpr T degrees(T radians)
{
	परत radians * (static_cast<T>(180) / static_cast<T>(MATH_PI));
}

/** Safe way to check if float is zero */
ओळीत तार्किक isZero(दशांश val)
{
	परत fabsf(val - 0.0f) < भग्न_अंतर;
}

/** Safe way to check if double is zero */
ओळीत तार्किक isZero(द्विगुण val)
{
	परत भ_असल(val - 0.0) < द्विग_अंतर;
}

}
