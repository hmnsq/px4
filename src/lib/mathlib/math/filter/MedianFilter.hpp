/****************************************************************************
 *
 *   Copyright (C) 2020 PX4 Development Team. All rights reserved.
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

/*
 * @file MedianFilter.hpp
 *
 * @brief Implementation of a median filter with C qsort.
 *
 */

#لارښوونه once

#ضمن <stdio.h>
#ضمن <stdlib.h>
#ضمن <stdint.h>
#ضمن <string.h>

نطاق math
{

کينډۍ<ډول_نوم U> بنية is_floating_point { ساكن constexpr منطقي value = خطأ; };
کينډۍ<> بنية is_floating_point<عائم> { ساكن constexpr منطقي value = ریښتیا; };
کينډۍ<> بنية is_floating_point<مزدوج> { ساكن constexpr منطقي value = ریښتیا; };
کينډۍ<> بنية is_floating_point<اوږد مزدوج> { ساكن constexpr منطقي value = ریښتیا; };

کينډۍ<ډول_نوم T, صحيح کړکۍ = 3>
صنف MedianFilter
{
عام:
	static_assert(کړکۍ >= 3, "MedianFilter window size must be >= 3");
	static_assert(کړکۍ % 2, "MedianFilter window size must be odd"); // odd

	MedianFilter() = افتراضي;

	فراغ ورننباسه(ثابت T &sample)
	{
		سر_2 = (سر_2 + 1) % کړکۍ;
		بفر[سر_2] = sample;
	}

	T median()
	{
		T sorted[کړکۍ];
		حافظه_نقل(sorted, بفر, حجم(بفر));
		qsort(&sorted, کړکۍ, حجم(T), cmp);

		أرجع sorted[کړکۍ / 2];
	}

	T apply(ثابت T &sample)
	{
		ورننباسه(sample);
		أرجع median();
	}

خاص:

	ساكن صحيح cmp(ثابت فراغ *a, ثابت فراغ *b)
	{
		ثابت T va = *(ثابت T *)a;
		ثابت T vb = *(ثابت T *)b;

		إذا constexpr(is_floating_point<T>::value) {
			إذا (!__builtin_isfinite(va) && !__builtin_isfinite(vb)) {
				أرجع 0;

			} وإلا إذا (!__builtin_isfinite(va)) {
				أرجع  1;

			} وإلا إذا (!__builtin_isfinite(vb)) {
				أرجع -1;
			}
		}

		إذا (va < vb) {
			أرجع -1;
		}

		إذا (va > vb) {
			أرجع  1;
		}

		أرجع 0;
	}

	T بفر[کړکۍ] {};
	uint8_t سر_2{0};
};

} // namespace math
