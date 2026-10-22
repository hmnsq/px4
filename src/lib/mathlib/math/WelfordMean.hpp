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

#ಅನ್ತರ್ಭಾವ <lib/mathlib/mathlib.h>

ನಾಮಾಕಾಶ math
{

template <typename Type = ಭಿನ್ನ>
ವರ್ಗ WelfordMean
{
ಸಾರ್ವಜನಿಕ:
	// For a new value, compute the new count, new mean, the new M2.
	ತಾರ್ಕಿಕ update(ಧ್ರುವ Type &new_ಮೌಲ್ಯ)
	{
		ಯದಿ (ಎಣಿಕೆ_3 == 0) {
			reset();
			ಎಣಿಕೆ_3 = 1;
			_mean = new_ಮೌಲ್ಯ;
			ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;

		} ಅನ್ಯಥಾ ಯದಿ (ಎಣಿಕೆ_3 == UINT16_MAX) {
			// count overflow
			//  reset count, but maintain mean and variance
			_M2 = _M2 / ಎಣಿಕೆ_3;
			_M2_accum = 0;

			ಎಣಿಕೆ_3 = 1;

		} ಅನ್ಯಥಾ {
			ಎಣಿಕೆ_3++;
		}

		// mean accumulates the mean of the entire dataset
		// delta can be very small compared to the mean, use algorithm to minimise numerical error
		ಧ್ರುವ Type delta{new_ಮೌಲ್ಯ - _mean};
		ಧ್ರುವ Type mean_change = delta / ಎಣಿಕೆ_3;
		_mean = kahanSummation(_mean, mean_change, _mean_accum);

		// M2 aggregates the squared distance from the mean
		// count aggregates the number of samples seen so far
		ಧ್ರುವ Type M2_change = delta * (new_ಮೌಲ್ಯ - _mean);
		_M2 = kahanSummation(_M2, M2_change, _M2_accum);

		// protect against floating point precision causing negative variances
		_M2 = math::max(_M2, 0.f);

		ಯದಿ (!PX4_ISFINITE(_mean) || !PX4_ISFINITE(_M2)) {
			reset();
			ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
		}

		ಪ್ರತ್ಯಾಗಮ valid();
	}

	ತಾರ್ಕಿಕ valid() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ ಎಣಿಕೆ_3 > 2; }
	auto ಎಣಿಕೆ() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ ಎಣಿಕೆ_3; }

	ಶೂನ್ಯ reset()
	{
		ಎಣಿಕೆ_3 = 0;
		_mean = 0;
		_M2 = 0;

		_mean_accum = 0;
		_M2_accum = 0;
	}

	Type mean() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ _mean; }
	Type variance() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ _M2 / (ಎಣಿಕೆ_3 - 1); }
	Type standard_deviation() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ std::sqrt(variance()); }

ಗುಪ್ತ:

	// Use Kahan summation algorithm to get the sum of "sum_previous" and "input".
	// This function relies on the caller to be responsible for keeping a copy of
	// "accumulator" and passing this value at the next iteration.
	// Ref: https://en.wikipedia.org/wiki/Kahan_summation_algorithm
	ಅನ್ತಃಸ್ಥ Type kahanSummation(Type sum_previous, Type ಒಳಹರಿವು, Type &accumulator)
	{
		ಧ್ರುವ Type y = ಒಳಹರಿವು - accumulator;
		ಧ್ರುವ Type t = sum_previous + y;
		accumulator = (t - sum_previous) - y;
		ಪ್ರತ್ಯಾಗಮ t;
	}

	Type _mean{};
	Type _M2{};

	Type _mean_accum{};  ///< kahan summation algorithm accumulator for mean
	Type _M2_accum{};    ///< kahan summation algorithm accumulator for M2

	uint16_t ಎಣಿಕೆ_3{0};
};

} // namespace math
