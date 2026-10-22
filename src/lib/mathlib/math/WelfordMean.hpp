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

#సూచన once

#అన్తర్భావ <lib/mathlib/mathlib.h>

నామాకాశ math
{

మూస <రకం_పేరు Type = దశాంశం>
తరగతి Welford_సగటు
{
బహిరంగ:
	// For a new value, compute the new count, new mean, the new M2.
	తార్కిక update(స్థిరరాశి Type &new_విలువ)
	{
		ఒకవేళ (లెక్క_3 == 0) {
			reset();
			లెక్క_3 = 1;
			సగటు_2 = new_విలువ;
			తిరిగి అబద్ధం;

		} లేకపోతే ఒకవేళ (లెక్క_3 == UINT16_MAX) {
			// count overflow
			//  reset count, but maintain mean and variance
			_M2 = _M2 / లెక్క_3;
			_M2_accum = 0;

			లెక్క_3 = 1;

		} లేకపోతే {
			లెక్క_3++;
		}

		// mean accumulates the mean of the entire dataset
		// delta can be very small compared to the mean, use algorithm to minimise numerical error
		స్థిరరాశి Type delta{new_విలువ - సగటు_2};
		స్థిరరాశి Type సగటు_change = delta / లెక్క_3;
		సగటు_2 = kahanSummation(సగటు_2, సగటు_change, సగటు_accum);

		// M2 aggregates the squared distance from the mean
		// count aggregates the number of samples seen so far
		స్థిరరాశి Type M2_change = delta * (new_విలువ - సగటు_2);
		_M2 = kahanSummation(_M2, M2_change, _M2_accum);

		// protect against floating point precision causing negative variances
		_M2 = math::max(_M2, 0.f);

		ఒకవేళ (!PX4_ISFINITE(సగటు_2) || !PX4_ISFINITE(_M2)) {
			reset();
			తిరిగి అబద్ధం;
		}

		తిరిగి valid();
	}

	తార్కిక valid() స్థిరరాశి { తిరిగి లెక్క_3 > 2; }
	స్వయంచాలక లెక్క() స్థిరరాశి { తిరిగి లెక్క_3; }

	శూన్య reset()
	{
		లెక్క_3 = 0;
		సగటు_2 = 0;
		_M2 = 0;

		సగటు_accum = 0;
		_M2_accum = 0;
	}

	Type సగటు() స్థిరరాశి { తిరిగి సగటు_2; }
	Type variance() స్థిరరాశి { తిరిగి _M2 / (లెక్క_3 - 1); }
	Type standard_deviation() స్థిరరాశి { తిరిగి std::వర్గమూలం(variance()); }

అంతరంగ:

	// Use Kahan summation algorithm to get the sum of "sum_previous" and "input".
	// This function relies on the caller to be responsible for keeping a copy of
	// "accumulator" and passing this value at the next iteration.
	// Ref: https://en.wikipedia.org/wiki/Kahan_summation_algorithm
	అంతఃవరుస Type kahanSummation(Type కూడిక_మునుపటి, Type ఆదానం, Type &సంచాయకం)
	{
		స్థిరరాశి Type y = ఆదానం - సంచాయకం;
		స్థిరరాశి Type t = కూడిక_మునుపటి + y;
		సంచాయకం = (t - కూడిక_మునుపటి) - y;
		తిరిగి t;
	}

	Type సగటు_2{};
	Type _M2{};

	Type సగటు_accum{};  ///< kahan summation algorithm accumulator for mean
	Type _M2_accum{};    ///< kahan summation algorithm accumulator for M2

	uint16_t లెక్క_3{0};
};

} // namespace math
