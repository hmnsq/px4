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

#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ <lib/mathlib/mathlib.h>

ਨਾਮਾਕਾਸ਼ math
{

ਸਾਂਚਾ <ਕਿਸਮ_ਨਾਂ Type = ਦਸ਼ਮਲਵ>
ਵਰਗ Welford_ਔਸਤ
{
ਜਨਤਕ:
	// For a new value, compute the new count, new mean, the new M2.
	ਤਾਰਕਿਕ update(ਅਚਲ Type &new_ਮੁੱਲ)
	{
		ਜੇ (ਗਿਣਤੀ_4 == 0) {
			reset();
			ਗਿਣਤੀ_4 = 1;
			ਔਸਤ_2 = new_ਮੁੱਲ;
			ਵਾਪਸ ਝੂਠ;

		} ਨਹੀਂਤਾਂ ਜੇ (ਗਿਣਤੀ_4 == UINT16_MAX) {
			// count overflow
			//  reset count, but maintain mean and variance
			_M2 = _M2 / ਗਿਣਤੀ_4;
			_M2_accum = 0;

			ਗਿਣਤੀ_4 = 1;

		} ਨਹੀਂਤਾਂ {
			ਗਿਣਤੀ_4++;
		}

		// mean accumulates the mean of the entire dataset
		// delta can be very small compared to the mean, use algorithm to minimise numerical error
		ਅਚਲ Type delta{new_ਮੁੱਲ - ਔਸਤ_2};
		ਅਚਲ Type ਔਸਤ_change = delta / ਗਿਣਤੀ_4;
		ਔਸਤ_2 = kahanSummation(ਔਸਤ_2, ਔਸਤ_change, ਔਸਤ_accum);

		// M2 aggregates the squared distance from the mean
		// count aggregates the number of samples seen so far
		ਅਚਲ Type M2_change = delta * (new_ਮੁੱਲ - ਔਸਤ_2);
		_M2 = kahanSummation(_M2, M2_change, _M2_accum);

		// protect against floating point precision causing negative variances
		_M2 = math::max(_M2, 0.f);

		ਜੇ (!PX4_ISFINITE(ਔਸਤ_2) || !PX4_ISFINITE(_M2)) {
			reset();
			ਵਾਪਸ ਝੂਠ;
		}

		ਵਾਪਸ valid();
	}

	ਤਾਰਕਿਕ valid() ਅਚਲ { ਵਾਪਸ ਗਿਣਤੀ_4 > 2; }
	ਸਵੈਚਾਲਿਤ ਗਿਣਤੀ_2() ਅਚਲ { ਵਾਪਸ ਗਿਣਤੀ_4; }

	ਖਾਲੀ reset()
	{
		ਗਿਣਤੀ_4 = 0;
		ਔਸਤ_2 = 0;
		_M2 = 0;

		ਔਸਤ_accum = 0;
		_M2_accum = 0;
	}

	Type ਔਸਤ() ਅਚਲ { ਵਾਪਸ ਔਸਤ_2; }
	Type variance() ਅਚਲ { ਵਾਪਸ _M2 / (ਗਿਣਤੀ_4 - 1); }
	Type standard_deviation() ਅਚਲ { ਵਾਪਸ std::ਵਰਗਮੂਲ(variance()); }

ਨਿਜੀ:

	// Use Kahan summation algorithm to get the sum of "sum_previous" and "input".
	// This function relies on the caller to be responsible for keeping a copy of
	// "accumulator" and passing this value at the next iteration.
	// Ref: https://en.wikipedia.org/wiki/Kahan_summation_algorithm
	ਸਤਰਅੰਦਰ Type kahanSummation(Type ਜੋੜ_ਪਿਛਲਾ, Type ਨਿਵੇਸ਼, Type &ਸੰਚਾਇਕ)
	{
		ਅਚਲ Type y = ਨਿਵੇਸ਼ - ਸੰਚਾਇਕ;
		ਅਚਲ Type t = ਜੋੜ_ਪਿਛਲਾ + y;
		ਸੰਚਾਇਕ = (t - ਜੋੜ_ਪਿਛਲਾ) - y;
		ਵਾਪਸ t;
	}

	Type ਔਸਤ_2{};
	Type _M2{};

	Type ਔਸਤ_accum{};  ///< kahan summation algorithm accumulator for mean
	Type _M2_accum{};    ///< kahan summation algorithm accumulator for M2

	uint16_t ਗਿਣਤੀ_4{0};
};

} // namespace math
