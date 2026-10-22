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

#لارښوونه once

نطاق math
{

کينډۍ <ډول_نوم Type, size_t N>
صنف Welford_منځنۍ_ویکټور
{
عام:
	// For a new value, compute the new count, new mean, the new M2.
	منطقي update(ثابت ماتریکس::ویکټور<Type, N> &new_ارزښت)
	{
		إذا (شمېر_3 == 0) {
			reset();
			شمېر_3 = 1;
			منځنۍ_2 = new_ارزښت;
			أرجع خطأ;

		} وإلا إذا (شمېر_3 == UINT16_MAX) {
			// count overflow
			//  reset count, but maintain mean and variance
			_M2 = _M2 / شمېر_3;
			_M2_accum.zero();

			شمېر_3 = 1;

		} وإلا {
			شمېر_3++;
		}

		//منځنۍ
		//  accumulates the mean of the entire dataset
		//  use Kahan summation because delta can be very small compared to the mean
		ثابت ماتریکس::ویکټور<Type, N> delta{new_ارزښت - منځنۍ_2};
		{
			ثابت ماتریکس::ویکټور<Type, N> y = (delta / شمېر_3) - منځنۍ_accum;
			ثابت ماتریکس::ویکټور<Type, N> t = منځنۍ_2 + y;
			منځنۍ_accum = (t - منځنۍ_2) - y;
			منځنۍ_2 = t;
		}

		إذا (!منځنۍ_2.isAllFinite()) {
			reset();
			أرجع خطأ;
		}

		// covariance
		//  Kahan summation (upper triangle only)
		{
			// eg C(x,y) += dx * (y - mean_y)
			ماتریکس::مربع_ماتریکس<Type, N> m2_change{};

			لكل (size_t r = 0; r < N; r++) {
				لكل (size_t c = r; c < N; c++) {
					m2_change(r, c) = delta(r) * (new_ارزښت(c) - منځنۍ_2(c));
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

	منطقي valid() ثابت { أرجع شمېر_3 > 2; }
	خپلکاری شمېر() ثابت { أرجع شمېر_3; }

	فراغ reset()
	{
		شمېر_3 = 0;
		منځنۍ_2.zero();
		_M2.zero();

		منځنۍ_accum.zero();
		_M2_accum.zero();
	}

	ماتریکس::ویکټور<Type, N> منځنۍ() ثابت { أرجع منځنۍ_2; }
	ماتریکس::ویکټور<Type, N> variance() ثابت { أرجع _M2.diag() / (شمېر_3 - 1); }
	ماتریکس::مربع_ماتریکس<Type, N> covariance() ثابت { أرجع _M2 / (شمېر_3 - 1); }

	Type covariance(صحيح x, صحيح y) ثابت { أرجع _M2(x, y) / (شمېر_3 - 1); }

خاص:

	ماتریکس::ویکټور<Type, N> منځنۍ_2{};
	ماتریکس::ویکټور<Type, N> منځنۍ_accum{};  ///< kahan summation algorithm accumulator for mean

	ماتریکس::مربع_ماتریکس<Type, N> _M2{};
	ماتریکس::مربع_ماتریکس<Type, N> _M2_accum{};    ///< kahan summation algorithm accumulator for M2

	uint16_t شمېر_3{0};
};

} // namespace math
