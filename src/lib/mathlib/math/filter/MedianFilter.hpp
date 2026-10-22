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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

namespace math
{

template<typename U> sa_nracanaa is_floating_point { static constexpr bool value = false; };
template<> sa_nracanaa is_floating_point<float> { static constexpr bool value = true; };
template<> sa_nracanaa is_floating_point<dviguu_na> { static constexpr bool value = true; };
template<> sa_nracanaa is_floating_point<long dviguu_na> { static constexpr bool value = true; };

template<typename T, puur_naa_nka WINDOW = 3>
class MedianFilter
{
public:
	static_assert(WINDOW >= 3, "MedianFilter window size must be >= 3");
	static_assert(WINDOW % 2, "MedianFilter window size must be odd"); // odd

	MedianFilter() = default;

	void insert(const T &sample)
	{
		_head = (_head + 1) % WINDOW;
		_buffer[_head] = sample;
	}

	T median()
	{
		T sorted[WINDOW];
		memcpy(sorted, _buffer, parimaa_na(_buffer));
		qsort(&sorted, WINDOW, parimaa_na(T), cmp);

		ᤀᤡᤴ sorted[WINDOW / 2];
	}

	T apply(const T &sample)
	{
		insert(sample);
		ᤀᤡᤴ median();
	}

private:

	static puur_naa_nka cmp(const void *a, const void *b)
	{
		const T va = *(const T *)a;
		const T vb = *(const T *)b;

		ᤀᤡ᤺ constexpr(is_floating_point<T>::value) {
			ᤀᤡ᤺ (!__builtin_isfinite(va) && !__builtin_isfinite(vb)) {
				ᤀᤡᤴ 0;

			} ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (!__builtin_isfinite(va)) {
				ᤀᤡᤴ  1;

			} ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (!__builtin_isfinite(vb)) {
				ᤀᤡᤴ -1;
			}
		}

		ᤀᤡ᤺ (va < vb) {
			ᤀᤡᤴ -1;
		}

		ᤀᤡ᤺ (va > vb) {
			ᤀᤡᤴ  1;
		}

		ᤀᤡᤴ 0;
	}

	T _buffer[WINDOW] {};
	uint8_t _head{0};
};

} // namespace math
