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

#அந்தர்பாவ <stdio.h>
#அந்தர்பாவ <stdlib.h>
#அந்தர்பாவ <stdint.h>
#அந்தர்பாவ <string.h>

நாமாகாஶ math
{

template<typename U> கட்டமைப்பு is_floating_point { நிலையான constexpr தருக்க value = பொய்; };
template<> கட்டமைப்பு is_floating_point<தசம> { நிலையான constexpr தருக்க value = உண்மை; };
template<> கட்டமைப்பு is_floating_point<இரட்டை> { நிலையான constexpr தருக்க value = உண்மை; };
template<> கட்டமைப்பு is_floating_point<long இரட்டை> { நிலையான constexpr தருக்க value = உண்மை; };

template<typename T, முழு WINDOW = 3>
வகுப்பு MedianFilter
{
பொது:
	static_assert(WINDOW >= 3, "MedianFilter window size must be >= 3");
	static_assert(WINDOW % 2, "MedianFilter window size must be odd"); // odd

	MedianFilter() = இயல்பு;

	வெறுமை செருகு(மாறிலி T &sample)
	{
		_head = (_head + 1) % WINDOW;
		_buffer[_head] = sample;
	}

	T median()
	{
		T sorted[WINDOW];
		memcpy(sorted, _buffer, அளவு(_buffer));
		qsort(&sorted, WINDOW, அளவு(T), cmp);

		திரும்பு sorted[WINDOW / 2];
	}

	T apply(மாறிலி T &sample)
	{
		செருகு(sample);
		திரும்பு median();
	}

தனிப்பட்ட:

	நிலையான முழு cmp(மாறிலி வெறுமை *a, மாறிலி வெறுமை *b)
	{
		மாறிலி T va = *(மாறிலி T *)a;
		மாறிலி T vb = *(மாறிலி T *)b;

		எனில் constexpr(is_floating_point<T>::value) {
			எனில் (!__builtin_isfinite(va) && !__builtin_isfinite(vb)) {
				திரும்பு 0;

			} இல்லையெனில் எனில் (!__builtin_isfinite(va)) {
				திரும்பு  1;

			} இல்லையெனில் எனில் (!__builtin_isfinite(vb)) {
				திரும்பு -1;
			}
		}

		எனில் (va < vb) {
			திரும்பு -1;
		}

		எனில் (va > vb) {
			திரும்பு  1;
		}

		திரும்பு 0;
	}

	T _buffer[WINDOW] {};
	uint8_t _head{0};
};

} // namespace math
