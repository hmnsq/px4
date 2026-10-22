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

#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ <stdlib.h>
#ಅನ್ತರ್ಭಾವ <stdint.h>
#ಅನ್ತರ್ಭಾವ <string.h>

ನಾಮಾಕಾಶ math
{

template<typename U> ಸಂರಚನಾ is_floating_point { ಸ್ಥಿರ constexpr ತಾರ್ಕಿಕ value = ಅಸತ್ಯ; };
template<> ಸಂರಚನಾ is_floating_point<ಭಿನ್ನ> { ಸ್ಥಿರ constexpr ತಾರ್ಕಿಕ value = ಸತ್ಯ; };
template<> ಸಂರಚನಾ is_floating_point<ದ್ವಿಗುಣ> { ಸ್ಥಿರ constexpr ತಾರ್ಕಿಕ value = ಸತ್ಯ; };
template<> ಸಂರಚನಾ is_floating_point<long ದ್ವಿಗುಣ> { ಸ್ಥಿರ constexpr ತಾರ್ಕಿಕ value = ಸತ್ಯ; };

template<typename T, ಪೂರ್ಣಾಙ್ಕ WINDOW = 3>
ವರ್ಗ MedianFilter
{
ಸಾರ್ವಜನಿಕ:
	static_assert(WINDOW >= 3, "MedianFilter window size must be >= 3");
	static_assert(WINDOW % 2, "MedianFilter window size must be odd"); // odd

	MedianFilter() = ಸ್ವಭಾವ;

	ಶೂನ್ಯ ಸೇರಿಸು(ಧ್ರುವ T &sample)
	{
		_head = (_head + 1) % WINDOW;
		_buffer[_head] = sample;
	}

	T median()
	{
		T sorted[WINDOW];
		memcpy(sorted, _buffer, ಪರಿಮಾಣ(_buffer));
		qsort(&sorted, WINDOW, ಪರಿಮಾಣ(T), cmp);

		ಪ್ರತ್ಯಾಗಮ sorted[WINDOW / 2];
	}

	T apply(ಧ್ರುವ T &sample)
	{
		ಸೇರಿಸು(sample);
		ಪ್ರತ್ಯಾಗಮ median();
	}

ಗುಪ್ತ:

	ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ cmp(ಧ್ರುವ ಶೂನ್ಯ *a, ಧ್ರುವ ಶೂನ್ಯ *b)
	{
		ಧ್ರುವ T va = *(ಧ್ರುವ T *)a;
		ಧ್ರುವ T vb = *(ಧ್ರುವ T *)b;

		ಯದಿ constexpr(is_floating_point<T>::value) {
			ಯದಿ (!__builtin_isfinite(va) && !__builtin_isfinite(vb)) {
				ಪ್ರತ್ಯಾಗಮ 0;

			} ಅನ್ಯಥಾ ಯದಿ (!__builtin_isfinite(va)) {
				ಪ್ರತ್ಯಾಗಮ  1;

			} ಅನ್ಯಥಾ ಯದಿ (!__builtin_isfinite(vb)) {
				ಪ್ರತ್ಯಾಗಮ -1;
			}
		}

		ಯದಿ (va < vb) {
			ಪ್ರತ್ಯಾಗಮ -1;
		}

		ಯದಿ (va > vb) {
			ಪ್ರತ್ಯಾಗಮ  1;
		}

		ಪ್ರತ್ಯಾಗಮ 0;
	}

	T _buffer[WINDOW] {};
	uint8_t _head{0};
};

} // namespace math
