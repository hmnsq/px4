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

#अन्तर्भाव <stdio.h>
#अन्तर्भाव <stdlib.h>
#अन्तर्भाव <stdint.h>
#अन्तर्भाव <string.h>

नामाकाश math
{

template<typename U> संरचना is_floating_point { स्थिर constexpr तार्किक value = असत्य; };
template<> संरचना is_floating_point<भिन्न> { स्थिर constexpr तार्किक value = सत्य; };
template<> संरचना is_floating_point<द्विगुण> { स्थिर constexpr तार्किक value = सत्य; };
template<> संरचना is_floating_point<long द्विगुण> { स्थिर constexpr तार्किक value = सत्य; };

template<typename T, पूर्णाङ्क WINDOW = 3>
वर्ग MedianFilter
{
सार्वजनिक:
	static_assert(WINDOW >= 3, "MedianFilter window size must be >= 3");
	static_assert(WINDOW % 2, "MedianFilter window size must be odd"); // odd

	MedianFilter() = स्वभाव;

	शून्य जोड़ो(ध्रुव T &sample)
	{
		_head = (_head + 1) % WINDOW;
		_buffer[_head] = sample;
	}

	T median()
	{
		T sorted[WINDOW];
		memcpy(sorted, _buffer, परिमाण(_buffer));
		qsort(&sorted, WINDOW, परिमाण(T), cmp);

		प्रत्यागम sorted[WINDOW / 2];
	}

	T apply(ध्रुव T &sample)
	{
		जोड़ो(sample);
		प्रत्यागम median();
	}

गुप्त:

	स्थिर पूर्णाङ्क cmp(ध्रुव शून्य *a, ध्रुव शून्य *b)
	{
		ध्रुव T va = *(ध्रुव T *)a;
		ध्रुव T vb = *(ध्रुव T *)b;

		यदि constexpr(is_floating_point<T>::value) {
			यदि (!__builtin_isfinite(va) && !__builtin_isfinite(vb)) {
				प्रत्यागम 0;

			} अन्यथा यदि (!__builtin_isfinite(va)) {
				प्रत्यागम  1;

			} अन्यथा यदि (!__builtin_isfinite(vb)) {
				प्रत्यागम -1;
			}
		}

		यदि (va < vb) {
			प्रत्यागम -1;
		}

		यदि (va > vb) {
			प्रत्यागम  1;
		}

		प्रत्यागम 0;
	}

	T _buffer[WINDOW] {};
	uint8_t _head{0};
};

} // namespace math
