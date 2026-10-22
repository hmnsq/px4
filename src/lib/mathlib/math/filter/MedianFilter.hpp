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

#ہدایت once

#شامل <stdio.h>
#شامل <stdlib.h>
#شامل <stdint.h>
#شامل <string.h>

نامخلا math
{

سانچہ<قسم_نام U> ساخت is_floating_point { ساکن constexpr منطقی value = جھوٹ; };
سانچہ<> ساخت is_floating_point<اعشاری> { ساکن constexpr منطقی value = سچ; };
سانچہ<> ساخت is_floating_point<دوہرا> { ساکن constexpr منطقی value = سچ; };
سانچہ<> ساخت is_floating_point<طویل دوہرا> { ساکن constexpr منطقی value = سچ; };

سانچہ<قسم_نام T, صحیح کھڑکی = 3>
جماعت MedianFilter
{
عوامی:
	static_assert(کھڑکی >= 3, "MedianFilter window size must be >= 3");
	static_assert(کھڑکی % 2, "MedianFilter window size must be odd"); // odd

	MedianFilter() = طےشدہ;

	خالی داخل_کرو(ثابت T &sample)
	{
		سر = (سر + 1) % کھڑکی;
		بفر[سر] = sample;
	}

	T median()
	{
		T sorted[کھڑکی];
		حافظہ_نقل(sorted, بفر, حجم(بفر));
		qsort(&sorted, کھڑکی, حجم(T), cmp);

		واپس sorted[کھڑکی / 2];
	}

	T apply(ثابت T &sample)
	{
		داخل_کرو(sample);
		واپس median();
	}

نجی:

	ساکن صحیح cmp(ثابت خالی *a, ثابت خالی *b)
	{
		ثابت T va = *(ثابت T *)a;
		ثابت T vb = *(ثابت T *)b;

		اگر constexpr(is_floating_point<T>::value) {
			اگر (!__builtin_isfinite(va) && !__builtin_isfinite(vb)) {
				واپس 0;

			} ورنہ اگر (!__builtin_isfinite(va)) {
				واپس  1;

			} ورنہ اگر (!__builtin_isfinite(vb)) {
				واپس -1;
			}
		}

		اگر (va < vb) {
			واپس -1;
		}

		اگر (va > vb) {
			واپس  1;
		}

		واپس 0;
	}

	T بفر[کھڑکی] {};
	uint8_t سر{0};
};

} // namespace math
