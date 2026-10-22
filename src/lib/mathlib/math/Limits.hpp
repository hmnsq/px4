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

#رهنمود once

#ضمن <float.h>
#ضمن <math.h>
#ضمن <stdint.h>

#اگر_تعریف_نشده MATH_PI
#تعریف_کن MATH_PI		3.141592653589793238462643383280
#پایان_اگر

نطاق math
{

#إذا defined(__PX4_QURT)
نطاق HexagonDSPMath
{
ساكن مضمن constexpr منطقي isNan(عائم val) { أرجع __builtin_isnan(val); }
ساكن مضمن constexpr منطقي isNan(مزدوج val) { أرجع __builtin_isnan(val); }
ساكن مضمن constexpr منطقي isNan(دراز مزدوج val) { أرجع __builtin_isnan(val); }

قالب<نام_نوع _Tp>
ساكن مضمن constexpr منطقي isNan(_Tp)
{
	أرجع خطأ;
}
} // namespace HexagonDSPMath
#پایان_اگر // __PX4_QURT

قالب<نام_نوع _Tp>
constexpr _Tp min(_Tp a, _Tp b)
{
#إذا defined(__PX4_QURT)

	// The Hexagon compiler can lower this pattern to sfmin, which does not preserve NaN semantics.
	إذا (HexagonDSPMath::isNan(a) || HexagonDSPMath::isNan(b)) {
		أرجع b;
	}

#پایان_اگر // __PX4_QURT

	أرجع (a < b) ? a : b;
}

قالب<نام_نوع _Tp>
constexpr _Tp min(_Tp a, _Tp b, _Tp c)
{
	أرجع min(min(a, b), c);
}

قالب<نام_نوع _Tp>
constexpr _Tp max(_Tp a, _Tp b)
{
#إذا defined(__PX4_QURT)

	// The Hexagon compiler can lower this pattern to sfmax, which does not preserve NaN semantics.
	إذا (HexagonDSPMath::isNan(a) || HexagonDSPMath::isNan(b)) {
		أرجع b;
	}

#پایان_اگر // __PX4_QURT

	أرجع (a > b) ? a : b;
}

قالب<نام_نوع _Tp>
constexpr _Tp max(_Tp a, _Tp b, _Tp c)
{
	أرجع max(max(a, b), c);
}

قالب<نام_نوع _Tp>
constexpr _Tp constrain(_Tp val, _Tp min_val, _Tp max_val)
{
#إذا defined(__PX4_QURT)

	// The Hexagon compiler lowers this clamp pattern to sfmin, which does not preserve NaN semantics.
	إذا (HexagonDSPMath::isNan(val)) {
		أرجع val;
	}

#پایان_اگر // __PX4_QURT

	أرجع (val < min_val) ? min_val : ((val > max_val) ? max_val : val);
}

/** Constrain float values to valid values for int16_t.
 * Invalid values are just clipped to be in the range for int16_t. */
constexpr int16_t constrain_Float_مقصد_Int16(عائم value)
{
	أرجع (int16_t)math::constrain(value, (عائم)INT16_MIN, (عائم)INT16_MAX);
}

قالب<نام_نوع _Tp>
constexpr منطقي is_درون_Range(_Tp val, _Tp min_val, _Tp max_val)
{
	أرجع (min_val <= val) && (val <= max_val);
}

قالب<نام_نوع T>
constexpr T radians(T degrees)
{
	أرجع degrees * (static_cast<T>(MATH_PI) / static_cast<T>(180));
}

قالب<نام_نوع T>
constexpr T degrees(T radians)
{
	أرجع radians * (static_cast<T>(180) / static_cast<T>(MATH_PI));
}

/** Safe way to check if float is zero */
مضمن منطقي isZero(عائم val)
{
	أرجع fabsf(val - 0.0f) < FLT_EPSILON;
}

/** Safe way to check if double is zero */
مضمن منطقي isZero(مزدوج val)
{
	أرجع fabs(val - 0.0) < DBL_EPSILON;
}

}
