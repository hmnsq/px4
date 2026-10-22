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

#અન્તર્ભાવ <stdio.h>
#અન્તર્ભાવ <stdlib.h>
#અન્તર્ભાવ <stdint.h>
#અન્તર્ભાવ <string.h>

નામાકાશ math
{

template<typename U> સંરચના is_floating_point { સ્થિર constexpr તાર્કિક value = ખોટું; };
template<> સંરચના is_floating_point<દશાંશ> { સ્થિર constexpr તાર્કિક value = સાચું; };
template<> સંરચના is_floating_point<દ્વિગુણ> { સ્થિર constexpr તાર્કિક value = સાચું; };
template<> સંરચના is_floating_point<long દ્વિગુણ> { સ્થિર constexpr તાર્કિક value = સાચું; };

template<typename T, પૂર્ણાંક WINDOW = 3>
વર્ગ MedianFilter
{
જાહેર:
	static_assert(WINDOW >= 3, "MedianFilter window size must be >= 3");
	static_assert(WINDOW % 2, "MedianFilter window size must be odd"); // odd

	MedianFilter() = મૂળભૂત;

	શૂન્ય ઉમેરો(અચલ T &sample)
	{
		_head = (_head + 1) % WINDOW;
		_buffer[_head] = sample;
	}

	T median()
	{
		T sorted[WINDOW];
		memcpy(sorted, _buffer, કદ(_buffer));
		qsort(&sorted, WINDOW, કદ(T), cmp);

		પાછા sorted[WINDOW / 2];
	}

	T apply(અચલ T &sample)
	{
		ઉમેરો(sample);
		પાછા median();
	}

ખાનગી:

	સ્થિર પૂર્ણાંક cmp(અચલ શૂન્ય *a, અચલ શૂન્ય *b)
	{
		અચલ T va = *(અચલ T *)a;
		અચલ T vb = *(અચલ T *)b;

		જો constexpr(is_floating_point<T>::value) {
			જો (!__builtin_isfinite(va) && !__builtin_isfinite(vb)) {
				પાછા 0;

			} નહીંતર જો (!__builtin_isfinite(va)) {
				પાછા  1;

			} નહીંતર જો (!__builtin_isfinite(vb)) {
				પાછા -1;
			}
		}

		જો (va < vb) {
			પાછા -1;
		}

		જો (va > vb) {
			પાછા  1;
		}

		પાછા 0;
	}

	T _buffer[WINDOW] {};
	uint8_t _head{0};
};

} // namespace math
