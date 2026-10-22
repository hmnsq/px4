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

#رهنمود once

#ضمن <stdio.h>
#ضمن <stdlib.h>
#ضمن <stdint.h>
#ضمن <string.h>

نطاق math
{

قالب<نام_نوع U> بنية is_floating_point { ساكن constexpr منطقي value = خطأ; };
قالب<> بنية is_floating_point<عائم> { ساكن constexpr منطقي value = درست; };
قالب<> بنية is_floating_point<مزدوج> { ساكن constexpr منطقي value = درست; };
قالب<> بنية is_floating_point<دراز مزدوج> { ساكن constexpr منطقي value = درست; };

قالب<نام_نوع T, صحيح پنجره = 3>
صنف MedianFilter
{
عام:
	static_assert(پنجره >= 3, "MedianFilter window size must be >= 3");
	static_assert(پنجره % 2, "MedianFilter window size must be odd"); // odd

	MedianFilter() = افتراضي;

	فراغ درج(ثابت T &sample)
	{
		سر = (سر + 1) % پنجره;
		میانگیر[سر] = sample;
	}

	T median()
	{
		T sorted[پنجره];
		کپی_حافظه(sorted, میانگیر, حجم(میانگیر));
		qsort(&sorted, پنجره, حجم(T), cmp);

		أرجع sorted[پنجره / 2];
	}

	T apply(ثابت T &sample)
	{
		درج(sample);
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

	T میانگیر[پنجره] {};
	uint8_t سر{0};
};

} // namespace math
