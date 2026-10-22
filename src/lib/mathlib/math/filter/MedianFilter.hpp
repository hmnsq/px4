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

#অন্তর্ভাব <stdio.h>
#অন্তর্ভাব <stdlib.h>
#অন্তর্ভাব <stdint.h>
#অন্তর্ভাব <string.h>

নামাকাশ math
{

template<typename U> সংরচনা is_floating_point { স্থির constexpr তার্কিক value = অসত্য; };
template<> সংরচনা is_floating_point<ভিন্ন> { স্থির constexpr তার্কিক value = সত্য; };
template<> সংরচনা is_floating_point<দ্বিগুণ> { স্থির constexpr তার্কিক value = সত্য; };
template<> সংরচনা is_floating_point<long দ্বিগুণ> { স্থির constexpr তার্কিক value = সত্য; };

template<typename T, পূর্ণাঙ্ক WINDOW = 3>
বর্গ MedianFilter
{
সার্বজনিক:
	static_assert(WINDOW >= 3, "MedianFilter window size must be >= 3");
	static_assert(WINDOW % 2, "MedianFilter window size must be odd"); // odd

	MedianFilter() = স্বভাব;

	শূন্য ঢোকাও(ধ্রুব T &sample)
	{
		_head = (_head + 1) % WINDOW;
		_buffer[_head] = sample;
	}

	T median()
	{
		T sorted[WINDOW];
		memcpy(sorted, _buffer, পরিমাণ(_buffer));
		qsort(&sorted, WINDOW, পরিমাণ(T), cmp);

		প্রত্যাগম sorted[WINDOW / 2];
	}

	T apply(ধ্রুব T &sample)
	{
		ঢোকাও(sample);
		প্রত্যাগম median();
	}

গুপ্ত:

	স্থির পূর্ণাঙ্ক cmp(ধ্রুব শূন্য *a, ধ্রুব শূন্য *b)
	{
		ধ্রুব T va = *(ধ্রুব T *)a;
		ধ্রুব T vb = *(ধ্রুব T *)b;

		যদি constexpr(is_floating_point<T>::value) {
			যদি (!__builtin_isfinite(va) && !__builtin_isfinite(vb)) {
				প্রত্যাগম 0;

			} অন্যথা যদি (!__builtin_isfinite(va)) {
				প্রত্যাগম  1;

			} অন্যথা যদি (!__builtin_isfinite(vb)) {
				প্রত্যাগম -1;
			}
		}

		যদি (va < vb) {
			প্রত্যাগম -1;
		}

		যদি (va > vb) {
			প্রত্যাগম  1;
		}

		প্রত্যাগম 0;
	}

	T _buffer[WINDOW] {};
	uint8_t _head{0};
};

} // namespace math
