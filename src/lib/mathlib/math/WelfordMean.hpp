/****************************************************************************
 *
 *   Copyright (c) 2021-2022 PX4 Development Team. All rights reserved.
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

/**
 * @file WelfordMean.hpp
 *
 * Welford's online algorithm for computing mean and variance.
 */

#निर्देशः once

#अन्तर्भाव <lib/mathlib/mathlib.h>

नामाकाश math
{

प्रतिरूपम् <प्रकारनाम Type = भिन्न>
वर्ग Welford_माध्यम्
{
सार्वजनिक:
	// For a new value, compute the new count, new mean, the new M2.
	तार्किक update(ध्रुव Type &new_मानम्)
	{
		यदि (गणनम्_3 == 0) {
			reset();
			गणनम्_3 = 1;
			माध्यम्_2 = new_मानम्;
			प्रत्यागम असत्य;

		} अन्यथा यदि (गणनम्_3 == UINT16_MAX) {
			// count overflow
			//  reset count, but maintain mean and variance
			_M2 = _M2 / गणनम्_3;
			_M2_accum = 0;

			गणनम्_3 = 1;

		} अन्यथा {
			गणनम्_3++;
		}

		// mean accumulates the mean of the entire dataset
		// delta can be very small compared to the mean, use algorithm to minimise numerical error
		ध्रुव Type delta{new_मानम् - माध्यम्_2};
		ध्रुव Type माध्यम्_change = delta / गणनम्_3;
		माध्यम्_2 = kahanSummation(माध्यम्_2, माध्यम्_change, माध्यम्_accum);

		// M2 aggregates the squared distance from the mean
		// count aggregates the number of samples seen so far
		ध्रुव Type M2_change = delta * (new_मानम् - माध्यम्_2);
		_M2 = kahanSummation(_M2, M2_change, _M2_accum);

		// protect against floating point precision causing negative variances
		_M2 = math::max(_M2, 0.f);

		यदि (!PX4_ISFINITE(माध्यम्_2) || !PX4_ISFINITE(_M2)) {
			reset();
			प्रत्यागम असत्य;
		}

		प्रत्यागम valid();
	}

	तार्किक valid() ध्रुव { प्रत्यागम गणनम्_3 > 2; }
	स्वचालित गणनम्() ध्रुव { प्रत्यागम गणनम्_3; }

	शून्य reset()
	{
		गणनम्_3 = 0;
		माध्यम्_2 = 0;
		_M2 = 0;

		माध्यम्_accum = 0;
		_M2_accum = 0;
	}

	Type माध्यम्() ध्रुव { प्रत्यागम माध्यम्_2; }
	Type variance() ध्रुव { प्रत्यागम _M2 / (गणनम्_3 - 1); }
	Type standard_deviation() ध्रुव { प्रत्यागम std::वर्गमूलम्(variance()); }

गुप्त:

	// Use Kahan summation algorithm to get the sum of "sum_previous" and "input".
	// This function relies on the caller to be responsible for keeping a copy of
	// "accumulator" and passing this value at the next iteration.
	// Ref: https://en.wikipedia.org/wiki/Kahan_summation_algorithm
	अन्तःस्थ Type kahanSummation(Type योग_previous, Type निवेशः, Type &सञ्चायकः)
	{
		ध्रुव Type y = निवेशः - सञ्चायकः;
		ध्रुव Type t = योग_previous + y;
		सञ्चायकः = (t - योग_previous) - y;
		प्रत्यागम t;
	}

	Type माध्यम्_2{};
	Type _M2{};

	Type माध्यम्_accum{};  ///< kahan summation algorithm accumulator for mean
	Type _M2_accum{};    ///< kahan summation algorithm accumulator for M2

	uint16_t गणनम्_3{0};
};

} // namespace math
