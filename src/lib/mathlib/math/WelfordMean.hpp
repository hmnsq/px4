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

#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ <lib/mathlib/mathlib.h>

ನಾಮಾಕಾಶ math
{

ಮಾದರಿ <ಬಗೆ_ಹೆಸರು Type = ದಶಮಾಂಶ>
ವರ್ಗ Welford_ಸರಾಸರಿ
{
ಸಾರ್ವಜನಿಕ:
	// For a new value, compute the new count, new mean, the new M2.
	ತಾರ್ಕಿಕ update(ಸ್ಥಿರಾಂಕ Type &new_ಮೌಲ್ಯ)
	{
		ಆದರೆ (ಎಣಿಕೆ_4 == 0) {
			reset();
			ಎಣಿಕೆ_4 = 1;
			ಸರಾಸರಿ_2 = new_ಮೌಲ್ಯ;
			ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;

		} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಎಣಿಕೆ_4 == UINT16_MAX) {
			// count overflow
			//  reset count, but maintain mean and variance
			_M2 = _M2 / ಎಣಿಕೆ_4;
			_M2_accum = 0;

			ಎಣಿಕೆ_4 = 1;

		} ಇಲ್ಲದಿದ್ದರೆ {
			ಎಣಿಕೆ_4++;
		}

		// mean accumulates the mean of the entire dataset
		// delta can be very small compared to the mean, use algorithm to minimise numerical error
		ಸ್ಥಿರಾಂಕ Type delta{new_ಮೌಲ್ಯ - ಸರಾಸರಿ_2};
		ಸ್ಥಿರಾಂಕ Type ಸರಾಸರಿ_change = delta / ಎಣಿಕೆ_4;
		ಸರಾಸರಿ_2 = kahanSummation(ಸರಾಸರಿ_2, ಸರಾಸರಿ_change, ಸರಾಸರಿ_accum);

		// M2 aggregates the squared distance from the mean
		// count aggregates the number of samples seen so far
		ಸ್ಥಿರಾಂಕ Type M2_change = delta * (new_ಮೌಲ್ಯ - ಸರಾಸರಿ_2);
		_M2 = kahanSummation(_M2, M2_change, _M2_accum);

		// protect against floating point precision causing negative variances
		_M2 = math::max(_M2, 0.f);

		ಆದರೆ (!PX4_ISFINITE(ಸರಾಸರಿ_2) || !PX4_ISFINITE(_M2)) {
			reset();
			ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
		}

		ಹಿಂತಿರುಗಿ valid();
	}

	ತಾರ್ಕಿಕ valid() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ಎಣಿಕೆ_4 > 2; }
	ಸ್ವಯಂಚಾಲಿತ ಎಣಿಕೆ_2() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ಎಣಿಕೆ_4; }

	ಶೂನ್ಯ reset()
	{
		ಎಣಿಕೆ_4 = 0;
		ಸರಾಸರಿ_2 = 0;
		_M2 = 0;

		ಸರಾಸರಿ_accum = 0;
		_M2_accum = 0;
	}

	Type ಸರಾಸರಿ() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ಸರಾಸರಿ_2; }
	Type variance() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ _M2 / (ಎಣಿಕೆ_4 - 1); }
	Type standard_deviation() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ std::ವರ್ಗಮೂಲ(variance()); }

ಖಾಸಗಿ:

	// Use Kahan summation algorithm to get the sum of "sum_previous" and "input".
	// This function relies on the caller to be responsible for keeping a copy of
	// "accumulator" and passing this value at the next iteration.
	// Ref: https://en.wikipedia.org/wiki/Kahan_summation_algorithm
	ಸಾಲೊಳಗೆ Type kahanSummation(Type ಮೊತ್ತ_ಹಿಂದಿನ, Type ಒಳಹರಿವು, Type &ಸಂಚಯಕ)
	{
		ಸ್ಥಿರಾಂಕ Type y = ಒಳಹರಿವು - ಸಂಚಯಕ;
		ಸ್ಥಿರಾಂಕ Type t = ಮೊತ್ತ_ಹಿಂದಿನ + y;
		ಸಂಚಯಕ = (t - ಮೊತ್ತ_ಹಿಂದಿನ) - y;
		ಹಿಂತಿರುಗಿ t;
	}

	Type ಸರಾಸರಿ_2{};
	Type _M2{};

	Type ಸರಾಸರಿ_accum{};  ///< kahan summation algorithm accumulator for mean
	Type _M2_accum{};    ///< kahan summation algorithm accumulator for M2

	uint16_t ಎಣಿಕೆ_4{0};
};

} // namespace math
