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

#هدايت once

نطاق math
{

سانچو <قسم_نالو Type, size_t N>
صنف Welford_سراسري_ويڪٽر
{
عام:
	// For a new value, compute the new count, new mean, the new M2.
	منطقي update(ثابت ميٽرڪس::ويڪٽر<Type, N> &new_قدر)
	{
		إذا (ڳڻپ_3 == 0) {
			reset();
			ڳڻپ_3 = 1;
			سراسري_2 = new_قدر;
			أرجع خطأ;

		} وإلا إذا (ڳڻپ_3 == UINT16_MAX) {
			// count overflow
			//  reset count, but maintain mean and variance
			_M2 = _M2 / ڳڻپ_3;
			_M2_accum.zero();

			ڳڻپ_3 = 1;

		} وإلا {
			ڳڻپ_3++;
		}

		//سراسري
		//  accumulates the mean of the entire dataset
		//  use Kahan summation because delta can be very small compared to the mean
		ثابت ميٽرڪس::ويڪٽر<Type, N> delta{new_قدر - سراسري_2};
		{
			ثابت ميٽرڪس::ويڪٽر<Type, N> y = (delta / ڳڻپ_3) - سراسري_accum;
			ثابت ميٽرڪس::ويڪٽر<Type, N> t = سراسري_2 + y;
			سراسري_accum = (t - سراسري_2) - y;
			سراسري_2 = t;
		}

		إذا (!سراسري_2.isAllFinite()) {
			reset();
			أرجع خطأ;
		}

		// covariance
		//  Kahan summation (upper triangle only)
		{
			// eg C(x,y) += dx * (y - mean_y)
			ميٽرڪس::چورس_ميٽرڪس<Type, N> m2_change{};

			لكل (size_t r = 0; r < N; r++) {
				لكل (size_t c = r; c < N; c++) {
					m2_change(r, c) = delta(r) * (new_قدر(c) - سراسري_2(c));
				}
			}

			لكل (size_t r = 0; r < N; r++) {
				لكل (size_t c = r; c < N; c++) {

					ثابت Type y = m2_change(r, c) - _M2_accum(r, c);
					ثابت Type t = _M2(r, c) + y;
					_M2_accum(r, c) = (t - _M2(r, c)) - y;

					_M2(r, c) = t;
				}

				// protect against floating point precision causing negative variances
				إذا (_M2(r, r) < 0) {
					_M2(r, r) = 0;
				}
			}

			// make symmetric
			لكل (size_t r = 0; r < N; r++) {
				لكل (size_t c = r + 1; c < N; c++) {
					_M2(c, r) = _M2(r, c);
				}
			}
		}

		إذا (!_M2.isAllFinite()) {
			reset();
			أرجع خطأ;
		}

		أرجع valid();
	}

	منطقي valid() ثابت { أرجع ڳڻپ_3 > 2; }
	خودڪار ڳڻپ() ثابت { أرجع ڳڻپ_3; }

	فراغ reset()
	{
		ڳڻپ_3 = 0;
		سراسري_2.zero();
		_M2.zero();

		سراسري_accum.zero();
		_M2_accum.zero();
	}

	ميٽرڪس::ويڪٽر<Type, N> سراسري() ثابت { أرجع سراسري_2; }
	ميٽرڪس::ويڪٽر<Type, N> variance() ثابت { أرجع _M2.diag() / (ڳڻپ_3 - 1); }
	ميٽرڪس::چورس_ميٽرڪس<Type, N> covariance() ثابت { أرجع _M2 / (ڳڻپ_3 - 1); }

	Type covariance(صحيح x, صحيح y) ثابت { أرجع _M2(x, y) / (ڳڻپ_3 - 1); }

خاص:

	ميٽرڪس::ويڪٽر<Type, N> سراسري_2{};
	ميٽرڪس::ويڪٽر<Type, N> سراسري_accum{};  ///< kahan summation algorithm accumulator for mean

	ميٽرڪس::چورس_ميٽرڪس<Type, N> _M2{};
	ميٽرڪس::چورس_ميٽرڪس<Type, N> _M2_accum{};    ///< kahan summation algorithm accumulator for M2

	uint16_t ڳڻپ_3{0};
};

} // namespace math
