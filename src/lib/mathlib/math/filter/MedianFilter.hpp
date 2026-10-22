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

#pragma once

#شامل <stdio.h>
#شامل <stdlib.h>
#شامل <stdint.h>
#شامل <string.h>

فضاینام math
{

template<typename U> ساختار is_floating_point { ایستا constexpr منطقی value = نادرست; };
template<> ساختار is_floating_point<اعشاری> { ایستا constexpr منطقی value = درست; };
template<> ساختار is_floating_point<دوگانه> { ایستا constexpr منطقی value = درست; };
template<> ساختار is_floating_point<long دوگانه> { ایستا constexpr منطقی value = درست; };

template<typename T, صحیح WINDOW = 3>
رده MedianFilter
{
عمومی:
	static_assert(WINDOW >= 3, "MedianFilter window size must be >= 3");
	static_assert(WINDOW % 2, "MedianFilter window size must be odd"); // odd

	MedianFilter() = پیشفرض;

	تهی درج(ثابت T &sample)
	{
		_head = (_head + 1) % WINDOW;
		_buffer[_head] = sample;
	}

	T median()
	{
		T sorted[WINDOW];
		memcpy(sorted, _buffer, اندازه(_buffer));
		qsort(&sorted, WINDOW, اندازه(T), cmp);

		بازگشت sorted[WINDOW / 2];
	}

	T apply(ثابت T &sample)
	{
		درج(sample);
		بازگشت median();
	}

خصوصی:

	ایستا صحیح cmp(ثابت تهی *a, ثابت تهی *b)
	{
		ثابت T va = *(ثابت T *)a;
		ثابت T vb = *(ثابت T *)b;

		اگر constexpr(is_floating_point<T>::value) {
			اگر (!__builtin_isfinite(va) && !__builtin_isfinite(vb)) {
				بازگشت 0;

			} وگرنه اگر (!__builtin_isfinite(va)) {
				بازگشت  1;

			} وگرنه اگر (!__builtin_isfinite(vb)) {
				بازگشت -1;
			}
		}

		اگر (va < vb) {
			بازگشت -1;
		}

		اگر (va > vb) {
			بازگشت  1;
		}

		بازگشت 0;
	}

	T _buffer[WINDOW] {};
	uint8_t _head{0};
};

} // namespace math
