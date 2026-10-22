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

#pragma once

ନାମାକାଶ math
{

template <typename Type, size_t N>
ଶ୍ରେଣୀ WelfordMeanVector
{
ସର୍ବସାଧାରଣ:
	// For a new value, compute the new count, new mean, the new M2.
	ତାର୍କିକ update(ଅଚଳ matrix::Vector<Type, N> &new_ମୂଲ୍ୟ)
	{
		ଯଦି (ଗଣନା_4 == 0) {
			reset();
			ଗଣନା_4 = 1;
			_mean = new_ମୂଲ୍ୟ;
			ଫେରନ୍ତୁ ମିଥ୍ୟା;

		} ଅନ୍ୟଥା ଯଦି (ଗଣନା_4 == UINT16_MAX) {
			// count overflow
			//  reset count, but maintain mean and variance
			_M2 = _M2 / ଗଣନା_4;
			_M2_accum.zero();

			ଗଣନା_4 = 1;

		} ଅନ୍ୟଥା {
			ଗଣନା_4++;
		}

		// mean
		//  accumulates the mean of the entire dataset
		//  use Kahan summation because delta can be very small compared to the mean
		ଅଚଳ matrix::Vector<Type, N> delta{new_ମୂଲ୍ୟ - _mean};
		{
			ଅଚଳ matrix::Vector<Type, N> y = (delta / ଗଣନା_4) - _mean_accum;
			ଅଚଳ matrix::Vector<Type, N> t = _mean + y;
			_mean_accum = (t - _mean) - y;
			_mean = t;
		}

		ଯଦି (!_mean.isAllFinite()) {
			reset();
			ଫେରନ୍ତୁ ମିଥ୍ୟା;
		}

		// covariance
		//  Kahan summation (upper triangle only)
		{
			// eg C(x,y) += dx * (y - mean_y)
			matrix::SquareMatrix<Type, N> m2_change{};

			ପାଇଁ (size_t r = 0; r < N; r++) {
				ପାଇଁ (size_t c = r; c < N; c++) {
					m2_change(r, c) = delta(r) * (new_ମୂଲ୍ୟ(c) - _mean(c));
				}
			}

			ପାଇଁ (size_t r = 0; r < N; r++) {
				ପାଇଁ (size_t c = r; c < N; c++) {

					ଅଚଳ Type y = m2_change(r, c) - _M2_accum(r, c);
					ଅଚଳ Type t = _M2(r, c) + y;
					_M2_accum(r, c) = (t - _M2(r, c)) - y;

					_M2(r, c) = t;
				}

				// protect against floating point precision causing negative variances
				ଯଦି (_M2(r, r) < 0) {
					_M2(r, r) = 0;
				}
			}

			// make symmetric
			ପାଇଁ (size_t r = 0; r < N; r++) {
				ପାଇଁ (size_t c = r + 1; c < N; c++) {
					_M2(c, r) = _M2(r, c);
				}
			}
		}

		ଯଦି (!_M2.isAllFinite()) {
			reset();
			ଫେରନ୍ତୁ ମିଥ୍ୟା;
		}

		ଫେରନ୍ତୁ valid();
	}

	ତାର୍କିକ valid() ଅଚଳ { ଫେରନ୍ତୁ ଗଣନା_4 > 2; }
	auto ଗଣନା_2() ଅଚଳ { ଫେରନ୍ତୁ ଗଣନା_4; }

	ଶୂନ୍ୟ reset()
	{
		ଗଣନା_4 = 0;
		_mean.zero();
		_M2.zero();

		_mean_accum.zero();
		_M2_accum.zero();
	}

	matrix::Vector<Type, N> mean() ଅଚଳ { ଫେରନ୍ତୁ _mean; }
	matrix::Vector<Type, N> variance() ଅଚଳ { ଫେରନ୍ତୁ _M2.diag() / (ଗଣନା_4 - 1); }
	matrix::SquareMatrix<Type, N> covariance() ଅଚଳ { ଫେରନ୍ତୁ _M2 / (ଗଣନା_4 - 1); }

	Type covariance(ପୂର୍ଣ୍ଣସଂଖ୍ୟା x, ପୂର୍ଣ୍ଣସଂଖ୍ୟା y) ଅଚଳ { ଫେରନ୍ତୁ _M2(x, y) / (ଗଣନା_4 - 1); }

ବ୍ୟକ୍ତିଗତ:

	matrix::Vector<Type, N> _mean{};
	matrix::Vector<Type, N> _mean_accum{};  ///< kahan summation algorithm accumulator for mean

	matrix::SquareMatrix<Type, N> _M2{};
	matrix::SquareMatrix<Type, N> _M2_accum{};    ///< kahan summation algorithm accumulator for M2

	uint16_t ଗଣନା_4{0};
};

} // namespace math
