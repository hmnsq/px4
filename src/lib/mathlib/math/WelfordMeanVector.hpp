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

#ہدایت once

نامخلا math
{

سانچہ <قسم_نام Type, size_t N>
جماعت Welford_اوسط_سمتیہ
{
عوامی:
	// For a new value, compute the new count, new mean, the new M2.
	منطقی update(ثابت میٹرکس::سمتیہ<Type, N> &new_قدر)
	{
		اگر (گنتی_3 == 0) {
			reset();
			گنتی_3 = 1;
			اوسط_2 = new_قدر;
			واپس جھوٹ;

		} ورنہ اگر (گنتی_3 == UINT16_MAX) {
			// count overflow
			//  reset count, but maintain mean and variance
			_M2 = _M2 / گنتی_3;
			_M2_accum.zero();

			گنتی_3 = 1;

		} ورنہ {
			گنتی_3++;
		}

		//اوسط
		//  accumulates the mean of the entire dataset
		//  use Kahan summation because delta can be very small compared to the mean
		ثابت میٹرکس::سمتیہ<Type, N> delta{new_قدر - اوسط_2};
		{
			ثابت میٹرکس::سمتیہ<Type, N> y = (delta / گنتی_3) - اوسط_accum;
			ثابت میٹرکس::سمتیہ<Type, N> t = اوسط_2 + y;
			اوسط_accum = (t - اوسط_2) - y;
			اوسط_2 = t;
		}

		اگر (!اوسط_2.isAllFinite()) {
			reset();
			واپس جھوٹ;
		}

		// covariance
		//  Kahan summation (upper triangle only)
		{
			// eg C(x,y) += dx * (y - mean_y)
			میٹرکس::مربع_میٹرکس<Type, N> m2_change{};

			برائے (size_t r = 0; r < N; r++) {
				برائے (size_t c = r; c < N; c++) {
					m2_change(r, c) = delta(r) * (new_قدر(c) - اوسط_2(c));
				}
			}

			برائے (size_t r = 0; r < N; r++) {
				برائے (size_t c = r; c < N; c++) {

					ثابت Type y = m2_change(r, c) - _M2_accum(r, c);
					ثابت Type t = _M2(r, c) + y;
					_M2_accum(r, c) = (t - _M2(r, c)) - y;

					_M2(r, c) = t;
				}

				// protect against floating point precision causing negative variances
				اگر (_M2(r, r) < 0) {
					_M2(r, r) = 0;
				}
			}

			// make symmetric
			برائے (size_t r = 0; r < N; r++) {
				برائے (size_t c = r + 1; c < N; c++) {
					_M2(c, r) = _M2(r, c);
				}
			}
		}

		اگر (!_M2.isAllFinite()) {
			reset();
			واپس جھوٹ;
		}

		واپس valid();
	}

	منطقی valid() ثابت { واپس گنتی_3 > 2; }
	خودکار گنتی() ثابت { واپس گنتی_3; }

	خالی reset()
	{
		گنتی_3 = 0;
		اوسط_2.zero();
		_M2.zero();

		اوسط_accum.zero();
		_M2_accum.zero();
	}

	میٹرکس::سمتیہ<Type, N> اوسط() ثابت { واپس اوسط_2; }
	میٹرکس::سمتیہ<Type, N> variance() ثابت { واپس _M2.diag() / (گنتی_3 - 1); }
	میٹرکس::مربع_میٹرکس<Type, N> covariance() ثابت { واپس _M2 / (گنتی_3 - 1); }

	Type covariance(صحیح x, صحیح y) ثابت { واپس _M2(x, y) / (گنتی_3 - 1); }

نجی:

	میٹرکس::سمتیہ<Type, N> اوسط_2{};
	میٹرکس::سمتیہ<Type, N> اوسط_accum{};  ///< kahan summation algorithm accumulator for mean

	میٹرکس::مربع_میٹرکس<Type, N> _M2{};
	میٹرکس::مربع_میٹرکس<Type, N> _M2_accum{};    ///< kahan summation algorithm accumulator for M2

	uint16_t گنتی_3{0};
};

} // namespace math
