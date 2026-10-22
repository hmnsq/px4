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

#അന്തര്ഭാവ <lib/mathlib/mathlib.h>

നാമാകാശ math
{

template <typename Type = ദശാംശം>
വർഗ്ഗം WelfordMean
{
പൊതു:
	// For a new value, compute the new count, new mean, the new M2.
	തർക്കം update(സ്ഥിരം Type &new_മൂല്യം)
	{
		എങ്കിൽ (എണ്ണം_3 == 0) {
			reset();
			എണ്ണം_3 = 1;
			_mean = new_മൂല്യം;
			തിരികെ അസത്യം;

		} അല്ലെങ്കിൽ എങ്കിൽ (എണ്ണം_3 == UINT16_MAX) {
			// count overflow
			//  reset count, but maintain mean and variance
			_M2 = _M2 / എണ്ണം_3;
			_M2_accum = 0;

			എണ്ണം_3 = 1;

		} അല്ലെങ്കിൽ {
			എണ്ണം_3++;
		}

		// mean accumulates the mean of the entire dataset
		// delta can be very small compared to the mean, use algorithm to minimise numerical error
		സ്ഥിരം Type delta{new_മൂല്യം - _mean};
		സ്ഥിരം Type mean_change = delta / എണ്ണം_3;
		_mean = kahanSummation(_mean, mean_change, _mean_accum);

		// M2 aggregates the squared distance from the mean
		// count aggregates the number of samples seen so far
		സ്ഥിരം Type M2_change = delta * (new_മൂല്യം - _mean);
		_M2 = kahanSummation(_M2, M2_change, _M2_accum);

		// protect against floating point precision causing negative variances
		_M2 = math::max(_M2, 0.f);

		എങ്കിൽ (!PX4_ISFINITE(_mean) || !PX4_ISFINITE(_M2)) {
			reset();
			തിരികെ അസത്യം;
		}

		തിരികെ valid();
	}

	തർക്കം valid() സ്ഥിരം { തിരികെ എണ്ണം_3 > 2; }
	auto എണ്ണം() സ്ഥിരം { തിരികെ എണ്ണം_3; }

	ശൂന്യം reset()
	{
		എണ്ണം_3 = 0;
		_mean = 0;
		_M2 = 0;

		_mean_accum = 0;
		_M2_accum = 0;
	}

	Type mean() സ്ഥിരം { തിരികെ _mean; }
	Type variance() സ്ഥിരം { തിരികെ _M2 / (എണ്ണം_3 - 1); }
	Type standard_deviation() സ്ഥിരം { തിരികെ std::sqrt(variance()); }

സ്വകാര്യ:

	// Use Kahan summation algorithm to get the sum of "sum_previous" and "input".
	// This function relies on the caller to be responsible for keeping a copy of
	// "accumulator" and passing this value at the next iteration.
	// Ref: https://en.wikipedia.org/wiki/Kahan_summation_algorithm
	വരിയിൽ Type kahanSummation(Type sum_previous, Type നിവേശം, Type &accumulator)
	{
		സ്ഥിരം Type y = നിവേശം - accumulator;
		സ്ഥിരം Type t = sum_previous + y;
		accumulator = (t - sum_previous) - y;
		തിരികെ t;
	}

	Type _mean{};
	Type _M2{};

	Type _mean_accum{};  ///< kahan summation algorithm accumulator for mean
	Type _M2_accum{};    ///< kahan summation algorithm accumulator for M2

	uint16_t എണ്ണം_3{0};
};

} // namespace math
