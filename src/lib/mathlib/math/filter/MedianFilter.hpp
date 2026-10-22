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

#ضمن <stdio.h>
#ضمن <stdlib.h>
#ضمن <stdint.h>
#ضمن <string.h>

نطاق math
{

template<typename U> بنية is_floating_point { ساكن constexpr منطقي value = خطأ; };
template<> بنية is_floating_point<عائم> { ساكن constexpr منطقي value = true; };
template<> بنية is_floating_point<مزدوج> { ساكن constexpr منطقي value = true; };
template<> بنية is_floating_point<long مزدوج> { ساكن constexpr منطقي value = true; };

template<typename T, صحيح WINDOW = 3>
صنف MedianFilter
{
عام:
	static_assert(WINDOW >= 3, "MedianFilter window size must be >= 3");
	static_assert(WINDOW % 2, "MedianFilter window size must be odd"); // odd

	MedianFilter() = افتراضي;

	فراغ أدرج(ثابت T &sample)
	{
		_head = (_head + 1) % WINDOW;
		_buffer[_head] = sample;
	}

	T median()
	{
		T sorted[WINDOW];
		memcpy(sorted, _buffer, حجم(_buffer));
		qsort(&sorted, WINDOW, حجم(T), cmp);

		إرجاع sorted[WINDOW / 2];
	}

	T apply(ثابت T &sample)
	{
		أدرج(sample);
		إرجاع median();
	}

خاص:

	ساكن صحيح cmp(ثابت فراغ *a, ثابت فراغ *b)
	{
		ثابت T va = *(ثابت T *)a;
		ثابت T vb = *(ثابت T *)b;

		إذا constexpr(is_floating_point<T>::value) {
			إذا (!__builtin_isfinite(va) && !__builtin_isfinite(vb)) {
				إرجاع 0;

			} وإلا إذا (!__builtin_isfinite(va)) {
				إرجاع  1;

			} وإلا إذا (!__builtin_isfinite(vb)) {
				إرجاع -1;
			}
		}

		إذا (va < vb) {
			إرجاع -1;
		}

		إذا (va > vb) {
			إرجاع  1;
		}

		إرجاع 0;
	}

	T _buffer[WINDOW] {};
	uint8_t _head{0};
};

} // namespace math
