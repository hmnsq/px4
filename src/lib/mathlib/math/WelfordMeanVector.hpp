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

ढाँचा <typename Type, माप_प्रकार N>
वर्ग WelfordMeanVector
{
सार्वजनिक:
	// For a new value, compute the new count, new mean, the new M2.
	तार्किक update(अचर matrix::Vector<Type, N> &new_मान)
	{
		यदि (गन्ती_3 == 0) {
			reset();
			गन्ती_3 = 1;
			_mean = new_मान;
			फर्काऊ असत्य;

		} अन्यथा यदि (गन्ती_3 == UINT16_MAX) {
			// count overflow
			//  reset count, but maintain mean and variance
			_M2 = _M2 / गन्ती_3;
			_M2_accum.zero();

			गन्ती_3 = 1;

		} अन्यथा {
			गन्ती_3++;
		}

		// mean
		//  accumulates the mean of the entire dataset
		//  use Kahan summation because delta can be very small compared to the mean
		अचर matrix::Vector<Type, N> delta{new_मान - _mean};
		{
			अचर matrix::Vector<Type, N> y = (delta / गन्ती_3) - _mean_accum;
			अचर matrix::Vector<Type, N> t = _mean + y;
			_mean_accum = (t - _mean) - y;
			_mean = t;
		}

		यदि (!_mean.isAllFinite()) {
			reset();
			फर्काऊ असत्य;
		}

		// covariance
		//  Kahan summation (upper triangle only)
		{
			// eg C(x,y) += dx * (y - mean_y)
			matrix::SquareMatrix<Type, N> m2_change{};

			लागि (माप_प्रकार r = 0; r < N; r++) {
				लागि (माप_प्रकार c = r; c < N; c++) {
					m2_change(r, c) = delta(r) * (new_मान(c) - _mean(c));
				}
			}

			लागि (माप_प्रकार r = 0; r < N; r++) {
				लागि (माप_प्रकार c = r; c < N; c++) {

					अचर Type y = m2_change(r, c) - _M2_accum(r, c);
					अचर Type t = _M2(r, c) + y;
					_M2_accum(r, c) = (t - _M2(r, c)) - y;

					_M2(r, c) = t;
				}

				// protect against floating point precision causing negative variances
				यदि (_M2(r, r) < 0) {
					_M2(r, r) = 0;
				}
			}

			// make symmetric
			लागि (माप_प्रकार r = 0; r < N; r++) {
				लागि (माप_प्रकार c = r + 1; c < N; c++) {
					_M2(c, r) = _M2(r, c);
				}
			}
		}

		यदि (!_M2.isAllFinite()) {
			reset();
			फर्काऊ असत्य;
		}

		फर्काऊ valid();
	}

	तार्किक valid() अचर { फर्काऊ गन्ती_3 > 2; }
	स्वतः गन्ती() अचर { फर्काऊ गन्ती_3; }

	शून्य reset()
	{
		गन्ती_3 = 0;
		_mean.zero();
		_M2.zero();

		_mean_accum.zero();
		_M2_accum.zero();
	}

	matrix::Vector<Type, N> mean() अचर { फर्काऊ _mean; }
	matrix::Vector<Type, N> variance() अचर { फर्काऊ _M2.diag() / (गन्ती_3 - 1); }
	matrix::SquareMatrix<Type, N> covariance() अचर { फर्काऊ _M2 / (गन्ती_3 - 1); }

	Type covariance(पूर्णाङ्क x, पूर्णाङ्क y) अचर { फर्काऊ _M2(x, y) / (गन्ती_3 - 1); }

गुप्त:

	matrix::Vector<Type, N> _mean{};
	matrix::Vector<Type, N> _mean_accum{};  ///< kahan summation algorithm accumulator for mean

	matrix::SquareMatrix<Type, N> _M2{};
	matrix::SquareMatrix<Type, N> _M2_accum{};    ///< kahan summation algorithm accumulator for M2

	uint16_t गन्ती_3{0};
};

} // namespace math
