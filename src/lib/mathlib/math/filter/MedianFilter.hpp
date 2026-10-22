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

#निर्देशः once

#अन्तर्भाव <stdio.h>
#अन्तर्भाव <stdlib.h>
#अन्तर्भाव <stdint.h>
#अन्तर्भाव <string.h>

नामाकाश math
{

प्रतिरूपम्<प्रकारनाम U> संरचना is_floating_point { स्थिर constexpr तार्किक value = असत्य; };
प्रतिरूपम्<> संरचना is_floating_point<भिन्न> { स्थिर constexpr तार्किक value = सत्य; };
प्रतिरूपम्<> संरचना is_floating_point<द्विगुण> { स्थिर constexpr तार्किक value = सत्य; };
प्रतिरूपम्<> संरचना is_floating_point<दीर्घ द्विगुण> { स्थिर constexpr तार्किक value = सत्य; };

प्रतिरूपम्<प्रकारनाम T, पूर्णाङ्क गवाक्षः = 3>
वर्ग MedianFilter
{
सार्वजनिक:
	static_assert(गवाक्षः >= 3, "MedianFilter window size must be >= 3");
	static_assert(गवाक्षः % 2, "MedianFilter window size must be odd"); // odd

	MedianFilter() = स्वभाव;

	शून्य निवेशय(ध्रुव T &sample)
	{
		शीर्ष = (शीर्ष + 1) % गवाक्षः;
		बफर[शीर्ष] = sample;
	}

	T median()
	{
		T sorted[गवाक्षः];
		स्मृतिं_प्रतिलिख(sorted, बफर, परिमाण(बफर));
		शीघ्रं_क्रमय(&sorted, गवाक्षः, परिमाण(T), cmp);

		प्रत्यागम sorted[गवाक्षः / 2];
	}

	T apply(ध्रुव T &sample)
	{
		निवेशय(sample);
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

	T बफर[गवाक्षः] {};
	uint8_t शीर्ष{0};
};

} // namespace math
