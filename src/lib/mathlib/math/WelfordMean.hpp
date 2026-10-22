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

#رهنمود once

#ضمن <lib/mathlib/mathlib.h>

نطاق math
{

قالب <نام_نوع Type = عائم>
صنف Welford_میانگین
{
عام:
	// For a new value, compute the new count, new mean, the new M2.
	منطقي update(ثابت Type &new_مقدار)
	{
		إذا (شمار_3 == 0) {
			reset();
			شمار_3 = 1;
			میانگین_2 = new_مقدار;
			أرجع خطأ;

		} وإلا إذا (شمار_3 == UINT16_MAX) {
			// count overflow
			//  reset count, but maintain mean and variance
			_M2 = _M2 / شمار_3;
			_M2_accum = 0;

			شمار_3 = 1;

		} وإلا {
			شمار_3++;
		}

		// mean accumulates the mean of the entire dataset
		// delta can be very small compared to the mean, use algorithm to minimise numerical error
		ثابت Type delta{new_مقدار - میانگین_2};
		ثابت Type میانگین_change = delta / شمار_3;
		میانگین_2 = kahanSummation(میانگین_2, میانگین_change, میانگین_accum);

		// M2 aggregates the squared distance from the mean
		// count aggregates the number of samples seen so far
		ثابت Type M2_change = delta * (new_مقدار - میانگین_2);
		_M2 = kahanSummation(_M2, M2_change, _M2_accum);

		// protect against floating point precision causing negative variances
		_M2 = math::max(_M2, 0.f);

		إذا (!PX4_ISFINITE(میانگین_2) || !PX4_ISFINITE(_M2)) {
			reset();
			أرجع خطأ;
		}

		أرجع valid();
	}

	منطقي valid() ثابت { أرجع شمار_3 > 2; }
	خودکار شمار() ثابت { أرجع شمار_3; }

	فراغ reset()
	{
		شمار_3 = 0;
		میانگین_2 = 0;
		_M2 = 0;

		میانگین_accum = 0;
		_M2_accum = 0;
	}

	Type میانگین() ثابت { أرجع میانگین_2; }
	Type variance() ثابت { أرجع _M2 / (شمار_3 - 1); }
	Type standard_deviation() ثابت { أرجع std::جذر(variance()); }

خاص:

	// Use Kahan summation algorithm to get the sum of "sum_previous" and "input".
	// This function relies on the caller to be responsible for keeping a copy of
	// "accumulator" and passing this value at the next iteration.
	// Ref: https://en.wikipedia.org/wiki/Kahan_summation_algorithm
	مضمن Type kahanSummation(Type جمع_قبلی, Type درون_داد, Type &انباشتگر)
	{
		ثابت Type y = درون_داد - انباشتگر;
		ثابت Type t = جمع_قبلی + y;
		انباشتگر = (t - جمع_قبلی) - y;
		أرجع t;
	}

	Type میانگین_2{};
	Type _M2{};

	Type میانگین_accum{};  ///< kahan summation algorithm accumulator for mean
	Type _M2_accum{};    ///< kahan summation algorithm accumulator for M2

	uint16_t شمار_3{0};
};

} // namespace math
