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
 * @file WelfordMeanVector.hpp
 *
 * Welford's online algorithm for computing mean and covariance of a vector.
 */

#సూచన once

నామాకాశ math
{

మూస <రకం_పేరు Type, size_t N>
తరగతి Welford_సగటు_సదిశ
{
బహిరంగ:
	// For a new value, compute the new count, new mean, the new M2.
	తార్కిక update(స్థిరరాశి మాతృక::సదిశ<Type, N> &new_విలువ)
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
			_M2_accum.zero();

			లెక్క_3 = 1;

		} లేకపోతే {
			లెక్క_3++;
		}

		//సగటు
		//  accumulates the mean of the entire dataset
		//  use Kahan summation because delta can be very small compared to the mean
		స్థిరరాశి మాతృక::సదిశ<Type, N> delta{new_విలువ - సగటు_2};
		{
			స్థిరరాశి మాతృక::సదిశ<Type, N> y = (delta / లెక్క_3) - సగటు_accum;
			స్థిరరాశి మాతృక::సదిశ<Type, N> t = సగటు_2 + y;
			సగటు_accum = (t - సగటు_2) - y;
			సగటు_2 = t;
		}

		ఒకవేళ (!సగటు_2.isAllFinite()) {
			reset();
			తిరిగి అబద్ధం;
		}

		// covariance
		//  Kahan summation (upper triangle only)
		{
			// eg C(x,y) += dx * (y - mean_y)
			మాతృక::వర్గం_మాతృక<Type, N> m2_change{};

			కొరకు (size_t r = 0; r < N; r++) {
				కొరకు (size_t c = r; c < N; c++) {
					m2_change(r, c) = delta(r) * (new_విలువ(c) - సగటు_2(c));
				}
			}

			కొరకు (size_t r = 0; r < N; r++) {
				కొరకు (size_t c = r; c < N; c++) {

					స్థిరరాశి Type y = m2_change(r, c) - _M2_accum(r, c);
					స్థిరరాశి Type t = _M2(r, c) + y;
					_M2_accum(r, c) = (t - _M2(r, c)) - y;

					_M2(r, c) = t;
				}

				// protect against floating point precision causing negative variances
				ఒకవేళ (_M2(r, r) < 0) {
					_M2(r, r) = 0;
				}
			}

			// make symmetric
			కొరకు (size_t r = 0; r < N; r++) {
				కొరకు (size_t c = r + 1; c < N; c++) {
					_M2(c, r) = _M2(r, c);
				}
			}
		}

		ఒకవేళ (!_M2.isAllFinite()) {
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
		సగటు_2.zero();
		_M2.zero();

		సగటు_accum.zero();
		_M2_accum.zero();
	}

	మాతృక::సదిశ<Type, N> సగటు() స్థిరరాశి { తిరిగి సగటు_2; }
	మాతృక::సదిశ<Type, N> variance() స్థిరరాశి { తిరిగి _M2.diag() / (లెక్క_3 - 1); }
	మాతృక::వర్గం_మాతృక<Type, N> covariance() స్థిరరాశి { తిరిగి _M2 / (లెక్క_3 - 1); }

	Type covariance(పూర్ణాంకం x, పూర్ణాంకం y) స్థిరరాశి { తిరిగి _M2(x, y) / (లెక్క_3 - 1); }

అంతరంగ:

	మాతృక::సదిశ<Type, N> సగటు_2{};
	మాతృక::సదిశ<Type, N> సగటు_accum{};  ///< kahan summation algorithm accumulator for mean

	మాతృక::వర్గం_మాతృక<Type, N> _M2{};
	మాతృక::వర్గం_మాతృక<Type, N> _M2_accum{};    ///< kahan summation algorithm accumulator for M2

	uint16_t లెక్క_3{0};
};

} // namespace math
