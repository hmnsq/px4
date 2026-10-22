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

नामाकाश math
{

template <typename Type, size_t N>
वग्ग WelfordMeanVector
{
सार्वजनिक:
	// For a new value, compute the new count, new mean, the new M2.
	तार्किक update(ध्रुव matrix::Vector<Type, N> &new_मानम्)
	{
		सचे (गणनम्_3 == 0) {
			reset();
			गणनम्_3 = 1;
			_mean = new_मानम्;
			पटिनिवत्त मिच्छा;

		} अञ्ञथा सचे (गणनम्_3 == UINT16_MAX) {
			// count overflow
			//  reset count, but maintain mean and variance
			_M2 = _M2 / गणनम्_3;
			_M2_accum.zero();

			गणनम्_3 = 1;

		} अञ्ञथा {
			गणनम्_3++;
		}

		// mean
		//  accumulates the mean of the entire dataset
		//  use Kahan summation because delta can be very small compared to the mean
		ध्रुव matrix::Vector<Type, N> delta{new_मानम् - _mean};
		{
			ध्रुव matrix::Vector<Type, N> y = (delta / गणनम्_3) - _mean_accum;
			ध्रुव matrix::Vector<Type, N> t = _mean + y;
			_mean_accum = (t - _mean) - y;
			_mean = t;
		}

		सचे (!_mean.isAllFinite()) {
			reset();
			पटिनिवत्त मिच्छा;
		}

		// covariance
		//  Kahan summation (upper triangle only)
		{
			// eg C(x,y) += dx * (y - mean_y)
			matrix::SquareMatrix<Type, N> m2_change{};

			अत्थाय (size_t r = 0; r < N; r++) {
				अत्थाय (size_t c = r; c < N; c++) {
					m2_change(r, c) = delta(r) * (new_मानम्(c) - _mean(c));
				}
			}

			अत्थाय (size_t r = 0; r < N; r++) {
				अत्थाय (size_t c = r; c < N; c++) {

					ध्रुव Type y = m2_change(r, c) - _M2_accum(r, c);
					ध्रुव Type t = _M2(r, c) + y;
					_M2_accum(r, c) = (t - _M2(r, c)) - y;

					_M2(r, c) = t;
				}

				// protect against floating point precision causing negative variances
				सचे (_M2(r, r) < 0) {
					_M2(r, r) = 0;
				}
			}

			// make symmetric
			अत्थाय (size_t r = 0; r < N; r++) {
				अत्थाय (size_t c = r + 1; c < N; c++) {
					_M2(c, r) = _M2(r, c);
				}
			}
		}

		सचे (!_M2.isAllFinite()) {
			reset();
			पटिनिवत्त मिच्छा;
		}

		पटिनिवत्त valid();
	}

	तार्किक valid() ध्रुव { पटिनिवत्त गणनम्_3 > 2; }
	auto गणनम्() ध्रुव { पटिनिवत्त गणनम्_3; }

	शून्य reset()
	{
		गणनम्_3 = 0;
		_mean.zero();
		_M2.zero();

		_mean_accum.zero();
		_M2_accum.zero();
	}

	matrix::Vector<Type, N> mean() ध्रुव { पटिनिवत्त _mean; }
	matrix::Vector<Type, N> variance() ध्रुव { पटिनिवत्त _M2.diag() / (गणनम्_3 - 1); }
	matrix::SquareMatrix<Type, N> covariance() ध्रुव { पटिनिवत्त _M2 / (गणनम्_3 - 1); }

	Type covariance(पूर्णाङ्क x, पूर्णाङ्क y) ध्रुव { पटिनिवत्त _M2(x, y) / (गणनम्_3 - 1); }

गुप्त:

	matrix::Vector<Type, N> _mean{};
	matrix::Vector<Type, N> _mean_accum{};  ///< kahan summation algorithm accumulator for mean

	matrix::SquareMatrix<Type, N> _M2{};
	matrix::SquareMatrix<Type, N> _M2_accum{};    ///< kahan summation algorithm accumulator for M2

	uint16_t गणनम्_3{0};
};

} // namespace math
