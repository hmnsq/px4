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

#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ <float.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <math.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdint.h>

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ MATH_PI
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MATH_PI		3.141592653589793238462643383280
#ਜੇ_ਅੰਤ

ਨਾਮਾਕਾਸ਼ math
{

#ਜੇ defined(__PX4_QURT)
ਨਾਮਾਕਾਸ਼ HexagonDSPMath
{
ਸਥਿਰ ਸਤਰਅੰਦਰ constexpr ਤਾਰਕਿਕ isNan(ਦਸ਼ਮਲਵ val) { ਵਾਪਸ __builtin_isnan(val); }
ਸਥਿਰ ਸਤਰਅੰਦਰ constexpr ਤਾਰਕਿਕ isNan(ਦੁੱਗਣਾ val) { ਵਾਪਸ __builtin_isnan(val); }
ਸਥਿਰ ਸਤਰਅੰਦਰ constexpr ਤਾਰਕਿਕ isNan(ਲੰਮਾ ਦੁੱਗਣਾ val) { ਵਾਪਸ __builtin_isnan(val); }

ਸਾਂਚਾ<ਕਿਸਮ_ਨਾਂ _Tp>
ਸਥਿਰ ਸਤਰਅੰਦਰ constexpr ਤਾਰਕਿਕ isNan(_Tp)
{
	ਵਾਪਸ ਝੂਠ;
}
} // namespace HexagonDSPMath
#ਜੇ_ਅੰਤ // __PX4_QURT

ਸਾਂਚਾ<ਕਿਸਮ_ਨਾਂ _Tp>
constexpr _Tp min(_Tp a, _Tp b)
{
#ਜੇ defined(__PX4_QURT)

	// The Hexagon compiler can lower this pattern to sfmin, which does not preserve NaN semantics.
	ਜੇ (HexagonDSPMath::isNan(a) || HexagonDSPMath::isNan(b)) {
		ਵਾਪਸ b;
	}

#ਜੇ_ਅੰਤ // __PX4_QURT

	ਵਾਪਸ (a < b) ? a : b;
}

ਸਾਂਚਾ<ਕਿਸਮ_ਨਾਂ _Tp>
constexpr _Tp min(_Tp a, _Tp b, _Tp c)
{
	ਵਾਪਸ min(min(a, b), c);
}

ਸਾਂਚਾ<ਕਿਸਮ_ਨਾਂ _Tp>
constexpr _Tp max(_Tp a, _Tp b)
{
#ਜੇ defined(__PX4_QURT)

	// The Hexagon compiler can lower this pattern to sfmax, which does not preserve NaN semantics.
	ਜੇ (HexagonDSPMath::isNan(a) || HexagonDSPMath::isNan(b)) {
		ਵਾਪਸ b;
	}

#ਜੇ_ਅੰਤ // __PX4_QURT

	ਵਾਪਸ (a > b) ? a : b;
}

ਸਾਂਚਾ<ਕਿਸਮ_ਨਾਂ _Tp>
constexpr _Tp max(_Tp a, _Tp b, _Tp c)
{
	ਵਾਪਸ max(max(a, b), c);
}

ਸਾਂਚਾ<ਕਿਸਮ_ਨਾਂ _Tp>
constexpr _Tp constrain(_Tp val, _Tp min_val, _Tp max_val)
{
#ਜੇ defined(__PX4_QURT)

	// The Hexagon compiler lowers this clamp pattern to sfmin, which does not preserve NaN semantics.
	ਜੇ (HexagonDSPMath::isNan(val)) {
		ਵਾਪਸ val;
	}

#ਜੇ_ਅੰਤ // __PX4_QURT

	ਵਾਪਸ (val < min_val) ? min_val : ((val > max_val) ? max_val : val);
}

/** Constrain float values to valid values for int16_t.
 * Invalid values are just clipped to be in the range for int16_t. */
constexpr int16_t constrain_Float_ਮੰਜ਼ਿਲ_Int16(ਦਸ਼ਮਲਵ value)
{
	ਵਾਪਸ (int16_t)math::constrain(value, (ਦਸ਼ਮਲਵ)INT16_MIN, (ਦਸ਼ਮਲਵ)INT16_MAX);
}

ਸਾਂਚਾ<ਕਿਸਮ_ਨਾਂ _Tp>
constexpr ਤਾਰਕਿਕ is_ਵਿੱਚ_Range(_Tp val, _Tp min_val, _Tp max_val)
{
	ਵਾਪਸ (min_val <= val) && (val <= max_val);
}

ਸਾਂਚਾ<ਕਿਸਮ_ਨਾਂ T>
constexpr T radians(T degrees)
{
	ਵਾਪਸ degrees * (static_cast<T>(MATH_PI) / static_cast<T>(180));
}

ਸਾਂਚਾ<ਕਿਸਮ_ਨਾਂ T>
constexpr T degrees(T radians)
{
	ਵਾਪਸ radians * (static_cast<T>(180) / static_cast<T>(MATH_PI));
}

/** Safe way to check if float is zero */
ਸਤਰਅੰਦਰ ਤਾਰਕਿਕ isZero(ਦਸ਼ਮਲਵ val)
{
	ਵਾਪਸ fabsf(val - 0.0f) < FLT_EPSILON;
}

/** Safe way to check if double is zero */
ਸਤਰਅੰਦਰ ਤਾਰਕਿਕ isZero(ਦੁੱਗਣਾ val)
{
	ਵਾਪਸ fabs(val - 0.0) < DBL_EPSILON;
}

}
