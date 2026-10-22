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

#അന്തര്ഭാവ <stdio.h>
#അന്തര്ഭാവ <stdlib.h>
#അന്തര്ഭാവ <stdint.h>
#അന്തര്ഭാവ <string.h>

നാമാകാശ math
{

template<typename U> ഘടന is_floating_point { സ്ഥിതം constexpr തർക്കം value = അസത്യം; };
template<> ഘടന is_floating_point<ദശാംശം> { സ്ഥിതം constexpr തർക്കം value = സത്യം; };
template<> ഘടന is_floating_point<ഇരട്ട> { സ്ഥിതം constexpr തർക്കം value = സത്യം; };
template<> ഘടന is_floating_point<long ഇരട്ട> { സ്ഥിതം constexpr തർക്കം value = സത്യം; };

template<typename T, പൂർണ്ണസംഖ്യ WINDOW = 3>
വർഗ്ഗം MedianFilter
{
പൊതു:
	static_assert(WINDOW >= 3, "MedianFilter window size must be >= 3");
	static_assert(WINDOW % 2, "MedianFilter window size must be odd"); // odd

	MedianFilter() = സ്ഥിരസ്ഥിതി;

	ശൂന്യം തിരുകുക(സ്ഥിരം T &sample)
	{
		_head = (_head + 1) % WINDOW;
		_buffer[_head] = sample;
	}

	T median()
	{
		T sorted[WINDOW];
		memcpy(sorted, _buffer, വലുപ്പം(_buffer));
		qsort(&sorted, WINDOW, വലുപ്പം(T), cmp);

		തിരികെ sorted[WINDOW / 2];
	}

	T apply(സ്ഥിരം T &sample)
	{
		തിരുകുക(sample);
		തിരികെ median();
	}

സ്വകാര്യ:

	സ്ഥിതം പൂർണ്ണസംഖ്യ cmp(സ്ഥിരം ശൂന്യം *a, സ്ഥിരം ശൂന്യം *b)
	{
		സ്ഥിരം T va = *(സ്ഥിരം T *)a;
		സ്ഥിരം T vb = *(സ്ഥിരം T *)b;

		എങ്കിൽ constexpr(is_floating_point<T>::value) {
			എങ്കിൽ (!__builtin_isfinite(va) && !__builtin_isfinite(vb)) {
				തിരികെ 0;

			} അല്ലെങ്കിൽ എങ്കിൽ (!__builtin_isfinite(va)) {
				തിരികെ  1;

			} അല്ലെങ്കിൽ എങ്കിൽ (!__builtin_isfinite(vb)) {
				തിരികെ -1;
			}
		}

		എങ്കിൽ (va < vb) {
			തിരികെ -1;
		}

		എങ്കിൽ (va > vb) {
			തിരികെ  1;
		}

		തിരികെ 0;
	}

	T _buffer[WINDOW] {};
	uint8_t _head{0};
};

} // namespace math
