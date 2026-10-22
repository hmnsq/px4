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
	तार्किक update(स्थिरांक matrix::Vector<Type, N> &new_मूल्य)
	{
		जर (मोजणी_3 == 0) {
			reset();
			मोजणी_3 = 1;
			_mean = new_मूल्य;
			परत असत्य;

		} अन्यथा जर (मोजणी_3 == UINT16_MAX) {
			// count overflow
			//  reset count, but maintain mean and variance
			_M2 = _M2 / मोजणी_3;
			_M2_accum.zero();

			मोजणी_3 = 1;

		} अन्यथा {
			मोजणी_3++;
		}

		// mean
		//  accumulates the mean of the entire dataset
		//  use Kahan summation because delta can be very small compared to the mean
		स्थिरांक matrix::Vector<Type, N> delta{new_मूल्य - _mean};
		{
			स्थिरांक matrix::Vector<Type, N> y = (delta / मोजणी_3) - _mean_accum;
			स्थिरांक matrix::Vector<Type, N> t = _mean + y;
			_mean_accum = (t - _mean) - y;
			_mean = t;
		}

		जर (!_mean.isAllFinite()) {
			reset();
			परत असत्य;
		}

		// covariance
		//  Kahan summation (upper triangle only)
		{
			// eg C(x,y) += dx * (y - mean_y)
			matrix::SquareMatrix<Type, N> m2_change{};

			साठी (माप_प्रकार r = 0; r < N; r++) {
				साठी (माप_प्रकार c = r; c < N; c++) {
					m2_change(r, c) = delta(r) * (new_मूल्य(c) - _mean(c));
				}
			}

			साठी (माप_प्रकार r = 0; r < N; r++) {
				साठी (माप_प्रकार c = r; c < N; c++) {

					स्थिरांक Type y = m2_change(r, c) - _M2_accum(r, c);
					स्थिरांक Type t = _M2(r, c) + y;
					_M2_accum(r, c) = (t - _M2(r, c)) - y;

					_M2(r, c) = t;
				}

				// protect against floating point precision causing negative variances
				जर (_M2(r, r) < 0) {
					_M2(r, r) = 0;
				}
			}

			// make symmetric
			साठी (माप_प्रकार r = 0; r < N; r++) {
				साठी (माप_प्रकार c = r + 1; c < N; c++) {
					_M2(c, r) = _M2(r, c);
				}
			}
		}

		जर (!_M2.isAllFinite()) {
			reset();
			परत असत्य;
		}

		परत valid();
	}

	तार्किक valid() स्थिरांक { परत मोजणी_3 > 2; }
	स्वतः मोजणी() स्थिरांक { परत मोजणी_3; }

	शून्य reset()
	{
		मोजणी_3 = 0;
		_mean.zero();
		_M2.zero();

		_mean_accum.zero();
		_M2_accum.zero();
	}

	matrix::Vector<Type, N> mean() स्थिरांक { परत _mean; }
	matrix::Vector<Type, N> variance() स्थिरांक { परत _M2.diag() / (मोजणी_3 - 1); }
	matrix::SquareMatrix<Type, N> covariance() स्थिरांक { परत _M2 / (मोजणी_3 - 1); }

	Type covariance(पूर्णांक x, पूर्णांक y) स्थिरांक { परत _M2(x, y) / (मोजणी_3 - 1); }

खाजगी:

	matrix::Vector<Type, N> _mean{};
	matrix::Vector<Type, N> _mean_accum{};  ///< kahan summation algorithm accumulator for mean

	matrix::SquareMatrix<Type, N> _M2{};
	matrix::SquareMatrix<Type, N> _M2_accum{};    ///< kahan summation algorithm accumulator for M2

	uint16_t मोजणी_3{0};
};

} // namespace math
