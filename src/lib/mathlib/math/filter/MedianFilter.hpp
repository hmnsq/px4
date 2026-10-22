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

#ଅନ୍ତର୍ଭାଵ <stdio.h>
#ଅନ୍ତର୍ଭାଵ <stdlib.h>
#ଅନ୍ତର୍ଭାଵ <stdint.h>
#ଅନ୍ତର୍ଭାଵ <string.h>

ନାମାକାଶ math
{

template<typename U> ସଂରଚନା is_floating_point { ସ୍ଥିର constexpr ତାର୍କିକ value = ମିଥ୍ୟା; };
template<> ସଂରଚନା is_floating_point<ଦଶମିକ> { ସ୍ଥିର constexpr ତାର୍କିକ value = ସତ୍ୟ; };
template<> ସଂରଚନା is_floating_point<ଦ୍ୱିଗୁଣ> { ସ୍ଥିର constexpr ତାର୍କିକ value = ସତ୍ୟ; };
template<> ସଂରଚନା is_floating_point<long ଦ୍ୱିଗୁଣ> { ସ୍ଥିର constexpr ତାର୍କିକ value = ସତ୍ୟ; };

template<typename T, ପୂର୍ଣ୍ଣସଂଖ୍ୟା WINDOW = 3>
ଶ୍ରେଣୀ MedianFilter
{
ସର୍ବସାଧାରଣ:
	static_assert(WINDOW >= 3, "MedianFilter window size must be >= 3");
	static_assert(WINDOW % 2, "MedianFilter window size must be odd"); // odd

	MedianFilter() = ପୂର୍ବନିର୍ଦ୍ଧାରିତ;

	ଶୂନ୍ୟ ଭର୍ତ୍ତି(ଅଚଳ T &sample)
	{
		_head = (_head + 1) % WINDOW;
		_buffer[_head] = sample;
	}

	T median()
	{
		T sorted[WINDOW];
		memcpy(sorted, _buffer, ଆକାର(_buffer));
		qsort(&sorted, WINDOW, ଆକାର(T), cmp);

		ଫେରନ୍ତୁ sorted[WINDOW / 2];
	}

	T apply(ଅଚଳ T &sample)
	{
		ଭର୍ତ୍ତି(sample);
		ଫେରନ୍ତୁ median();
	}

ବ୍ୟକ୍ତିଗତ:

	ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା cmp(ଅଚଳ ଶୂନ୍ୟ *a, ଅଚଳ ଶୂନ୍ୟ *b)
	{
		ଅଚଳ T va = *(ଅଚଳ T *)a;
		ଅଚଳ T vb = *(ଅଚଳ T *)b;

		ଯଦି constexpr(is_floating_point<T>::value) {
			ଯଦି (!__builtin_isfinite(va) && !__builtin_isfinite(vb)) {
				ଫେରନ୍ତୁ 0;

			} ଅନ୍ୟଥା ଯଦି (!__builtin_isfinite(va)) {
				ଫେରନ୍ତୁ  1;

			} ଅନ୍ୟଥା ଯଦି (!__builtin_isfinite(vb)) {
				ଫେରନ୍ତୁ -1;
			}
		}

		ଯଦି (va < vb) {
			ଫେରନ୍ତୁ -1;
		}

		ଯଦି (va > vb) {
			ଫେରନ୍ତୁ  1;
		}

		ଫେରନ୍ତୁ 0;
	}

	T _buffer[WINDOW] {};
	uint8_t _head{0};
};

} // namespace math
