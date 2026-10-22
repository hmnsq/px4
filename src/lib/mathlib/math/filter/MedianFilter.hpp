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

template<typename U> কাঠামো is_floating_point { স্থির constexpr যৌক্তিক value = মিথ্যা; };
template<> কাঠামো is_floating_point<দশমিক> { স্থির constexpr যৌক্তিক value = সত্য; };
template<> কাঠামো is_floating_point<দ্বিগুণ> { স্থির constexpr যৌক্তিক value = সত্য; };
template<> কাঠামো is_floating_point<long দ্বিগুণ> { স্থির constexpr যৌক্তিক value = সত্য; };

template<typename T, পূর্ণসংখ্যা WINDOW = 3>
শ্রেণী MedianFilter
{
সর্বজনীন:
	static_assert(WINDOW >= 3, "MedianFilter window size must be >= 3");
	static_assert(WINDOW % 2, "MedianFilter window size must be odd"); // odd

	MedianFilter() = পূর্বনির্ধারিত;

	শূন্য ঢোকাও(ধ্রুবক T &sample)
	{
		শীর্ষ_2 = (শীর্ষ_2 + 1) % WINDOW;
		বাফার[শীর্ষ_2] = sample;
	}

	T median()
	{
		T sorted[WINDOW];
		memcpy(sorted, বাফার, আকার(বাফার));
		qsort(&sorted, WINDOW, আকার(T), cmp);

		ফেরত sorted[WINDOW / 2];
	}

	T apply(ধ্রুবক T &sample)
	{
		ঢোকাও(sample);
		ফেরত median();
	}

ব্যক্তিগত:

	স্থির পূর্ণসংখ্যা cmp(ধ্রুবক শূন্য *a, ধ্রুবক শূন্য *b)
	{
		ধ্রুবক T va = *(ধ্রুবক T *)a;
		ধ্রুবক T vb = *(ধ্রুবক T *)b;

		যদি constexpr(is_floating_point<T>::value) {
			যদি (!__builtin_isfinite(va) && !__builtin_isfinite(vb)) {
				ফেরত 0;

			} অন্যথা যদি (!__builtin_isfinite(va)) {
				ফেরত  1;

			} অন্যথা যদি (!__builtin_isfinite(vb)) {
				ফেরত -1;
			}
		}

		যদি (va < vb) {
			ফেরত -1;
		}

		যদি (va > vb) {
			ফেরত  1;
		}

		ফেরত 0;
	}

	T বাফার[WINDOW] {};
	uint8_t শীর্ষ_2{0};
};

} // namespace math
