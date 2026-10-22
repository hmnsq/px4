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

#pragma once

#અન્તર્ભાવ <lib/mathlib/mathlib.h>

નામાકાશ math
{

template <typename Type = દશાંશ>
વર્ગ WelfordMean
{
જાહેર:
	// For a new value, compute the new count, new mean, the new M2.
	તાર્કિક update(અચલ Type &new_કિંમત)
	{
		જો (ગણતરી_3 == 0) {
			reset();
			ગણતરી_3 = 1;
			_mean = new_કિંમત;
			પાછા ખોટું;

		} નહીંતર જો (ગણતરી_3 == UINT16_MAX) {
			// count overflow
			//  reset count, but maintain mean and variance
			_M2 = _M2 / ગણતરી_3;
			_M2_accum = 0;

			ગણતરી_3 = 1;

		} નહીંતર {
			ગણતરી_3++;
		}

		// mean accumulates the mean of the entire dataset
		// delta can be very small compared to the mean, use algorithm to minimise numerical error
		અચલ Type delta{new_કિંમત - _mean};
		અચલ Type mean_change = delta / ગણતરી_3;
		_mean = kahanSummation(_mean, mean_change, _mean_accum);

		// M2 aggregates the squared distance from the mean
		// count aggregates the number of samples seen so far
		અચલ Type M2_change = delta * (new_કિંમત - _mean);
		_M2 = kahanSummation(_M2, M2_change, _M2_accum);

		// protect against floating point precision causing negative variances
		_M2 = math::max(_M2, 0.f);

		જો (!PX4_ISFINITE(_mean) || !PX4_ISFINITE(_M2)) {
			reset();
			પાછા ખોટું;
		}

		પાછા valid();
	}

	તાર્કિક valid() અચલ { પાછા ગણતરી_3 > 2; }
	auto ગણતરી() અચલ { પાછા ગણતરી_3; }

	શૂન્ય reset()
	{
		ગણતરી_3 = 0;
		_mean = 0;
		_M2 = 0;

		_mean_accum = 0;
		_M2_accum = 0;
	}

	Type mean() અચલ { પાછા _mean; }
	Type variance() અચલ { પાછા _M2 / (ગણતરી_3 - 1); }
	Type standard_deviation() અચલ { પાછા std::sqrt(variance()); }

ખાનગી:

	// Use Kahan summation algorithm to get the sum of "sum_previous" and "input".
	// This function relies on the caller to be responsible for keeping a copy of
	// "accumulator" and passing this value at the next iteration.
	// Ref: https://en.wikipedia.org/wiki/Kahan_summation_algorithm
	અંતઃરેખ Type kahanSummation(Type sum_previous, Type આગત, Type &accumulator)
	{
		અચલ Type y = આગત - accumulator;
		અચલ Type t = sum_previous + y;
		accumulator = (t - sum_previous) - y;
		પાછા t;
	}

	Type _mean{};
	Type _M2{};

	Type _mean_accum{};  ///< kahan summation algorithm accumulator for mean
	Type _M2_accum{};    ///< kahan summation algorithm accumulator for M2

	uint16_t ગણતરી_3{0};
};

} // namespace math
