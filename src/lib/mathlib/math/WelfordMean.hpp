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

#অন্তর্ভাৱ <lib/mathlib/mathlib.h>

নামাকাশ math
{

template <typename Type = ভিন্ন>
শ্ৰেণী WelfordMean
{
সার্ৱজনিক:
	// For a new value, compute the new count, new mean, the new M2.
	তার্কিক update(ধ্রুৱ Type &new_মান)
	{
		যদি (গুনতি_3 == 0) {
			reset();
			গুনতি_3 = 1;
			_mean = new_মান;
			ঘূৰাই মিছা;

		} অন্যথা যদি (গুনতি_3 == UINT16_MAX) {
			// count overflow
			//  reset count, but maintain mean and variance
			_M2 = _M2 / গুনতি_3;
			_M2_accum = 0;

			গুনতি_3 = 1;

		} অন্যথা {
			গুনতি_3++;
		}

		// mean accumulates the mean of the entire dataset
		// delta can be very small compared to the mean, use algorithm to minimise numerical error
		ধ্রুৱ Type delta{new_মান - _mean};
		ধ্রুৱ Type mean_change = delta / গুনতি_3;
		_mean = kahanSummation(_mean, mean_change, _mean_accum);

		// M2 aggregates the squared distance from the mean
		// count aggregates the number of samples seen so far
		ধ্রুৱ Type M2_change = delta * (new_মান - _mean);
		_M2 = kahanSummation(_M2, M2_change, _M2_accum);

		// protect against floating point precision causing negative variances
		_M2 = math::max(_M2, 0.f);

		যদি (!PX4_ISFINITE(_mean) || !PX4_ISFINITE(_M2)) {
			reset();
			ঘূৰাই মিছা;
		}

		ঘূৰাই valid();
	}

	তার্কিক valid() ধ্রুৱ { ঘূৰাই গুনতি_3 > 2; }
	auto গুনতি() ধ্রুৱ { ঘূৰাই গুনতি_3; }

	শূন্য reset()
	{
		গুনতি_3 = 0;
		_mean = 0;
		_M2 = 0;

		_mean_accum = 0;
		_M2_accum = 0;
	}

	Type mean() ধ্রুৱ { ঘূৰাই _mean; }
	Type variance() ধ্রুৱ { ঘূৰাই _M2 / (গুনতি_3 - 1); }
	Type standard_deviation() ধ্রুৱ { ঘূৰাই std::sqrt(variance()); }

গুপ্ত:

	// Use Kahan summation algorithm to get the sum of "sum_previous" and "input".
	// This function relies on the caller to be responsible for keeping a copy of
	// "accumulator" and passing this value at the next iteration.
	// Ref: https://en.wikipedia.org/wiki/Kahan_summation_algorithm
	অন্তঃস্থ Type kahanSummation(Type sum_previous, Type প্ৰবেশ, Type &accumulator)
	{
		ধ্রুৱ Type y = প্ৰবেশ - accumulator;
		ধ্রুৱ Type t = sum_previous + y;
		accumulator = (t - sum_previous) - y;
		ঘূৰাই t;
	}

	Type _mean{};
	Type _M2{};

	Type _mean_accum{};  ///< kahan summation algorithm accumulator for mean
	Type _M2_accum{};    ///< kahan summation algorithm accumulator for M2

	uint16_t গুনতি_3{0};
};

} // namespace math
