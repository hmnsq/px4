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

ढाँचा<typename U> संरचना is_floating_point { स्थिर constexpr तार्किक value = असत्य; };
ढाँचा<> संरचना is_floating_point<दशांश> { स्थिर constexpr तार्किक value = सत्य; };
ढाँचा<> संरचना is_floating_point<द्विगुण> { स्थिर constexpr तार्किक value = सत्य; };
ढाँचा<> संरचना is_floating_point<दीर्घ द्विगुण> { स्थिर constexpr तार्किक value = सत्य; };

ढाँचा<typename T, पूर्णांक WINDOW = 3>
वर्ग MedianFilter
{
सार्वजनिक:
	static_assert(WINDOW >= 3, "MedianFilter window size must be >= 3");
	static_assert(WINDOW % 2, "MedianFilter window size must be odd"); // odd

	MedianFilter() = पूर्वनिर्धारित;

	शून्य घाला(स्थिरांक T &sample)
	{
		_head = (_head + 1) % WINDOW;
		_buffer[_head] = sample;
	}

	T median()
	{
		T sorted[WINDOW];
		स_नकल(sorted, _buffer, आकार(_buffer));
		क्विक(&sorted, WINDOW, आकार(T), cmp);

		परत sorted[WINDOW / 2];
	}

	T apply(स्थिरांक T &sample)
	{
		घाला(sample);
		परत median();
	}

खाजगी:

	स्थिर पूर्णांक cmp(स्थिरांक शून्य *a, स्थिरांक शून्य *b)
	{
		स्थिरांक T va = *(स्थिरांक T *)a;
		स्थिरांक T vb = *(स्थिरांक T *)b;

		जर constexpr(is_floating_point<T>::value) {
			जर (!__builtin_isfinite(va) && !__builtin_isfinite(vb)) {
				परत 0;

			} अन्यथा जर (!__builtin_isfinite(va)) {
				परत  1;

			} अन्यथा जर (!__builtin_isfinite(vb)) {
				परत -1;
			}
		}

		जर (va < vb) {
			परत -1;
		}

		जर (va > vb) {
			परत  1;
		}

		परत 0;
	}

	T _buffer[WINDOW] {};
	uint8_t _head{0};
};

} // namespace math
