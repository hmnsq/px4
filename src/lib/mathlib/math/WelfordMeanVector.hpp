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

#ਹਦਾਇਤ once

ਨਾਮਾਕਾਸ਼ math
{

ਸਾਂਚਾ <ਕਿਸਮ_ਨਾਂ Type, size_t N>
ਵਰਗ Welford_ਔਸਤ_ਸਦਿਸ਼
{
ਜਨਤਕ:
	// For a new value, compute the new count, new mean, the new M2.
	ਤਾਰਕਿਕ update(ਅਚਲ ਮੈਟ੍ਰਿਕਸ::ਸਦਿਸ਼<Type, N> &new_ਮੁੱਲ)
	{
		ਜੇ (ਗਿਣਤੀ_4 == 0) {
			reset();
			ਗਿਣਤੀ_4 = 1;
			ਔਸਤ_2 = new_ਮੁੱਲ;
			ਵਾਪਸ ਝੂਠ;

		} ਨਹੀਂਤਾਂ ਜੇ (ਗਿਣਤੀ_4 == UINT16_MAX) {
			// count overflow
			//  reset count, but maintain mean and variance
			_M2 = _M2 / ਗਿਣਤੀ_4;
			_M2_accum.zero();

			ਗਿਣਤੀ_4 = 1;

		} ਨਹੀਂਤਾਂ {
			ਗਿਣਤੀ_4++;
		}

		//ਔਸਤ
		//  accumulates the mean of the entire dataset
		//  use Kahan summation because delta can be very small compared to the mean
		ਅਚਲ ਮੈਟ੍ਰਿਕਸ::ਸਦਿਸ਼<Type, N> delta{new_ਮੁੱਲ - ਔਸਤ_2};
		{
			ਅਚਲ ਮੈਟ੍ਰਿਕਸ::ਸਦਿਸ਼<Type, N> y = (delta / ਗਿਣਤੀ_4) - ਔਸਤ_accum;
			ਅਚਲ ਮੈਟ੍ਰਿਕਸ::ਸਦਿਸ਼<Type, N> t = ਔਸਤ_2 + y;
			ਔਸਤ_accum = (t - ਔਸਤ_2) - y;
			ਔਸਤ_2 = t;
		}

		ਜੇ (!ਔਸਤ_2.isAllFinite()) {
			reset();
			ਵਾਪਸ ਝੂਠ;
		}

		// covariance
		//  Kahan summation (upper triangle only)
		{
			// eg C(x,y) += dx * (y - mean_y)
			ਮੈਟ੍ਰਿਕਸ::ਵਰਗਫਲ_ਮੈਟ੍ਰਿਕਸ<Type, N> m2_change{};

			ਲਈ (size_t r = 0; r < N; r++) {
				ਲਈ (size_t c = r; c < N; c++) {
					m2_change(r, c) = delta(r) * (new_ਮੁੱਲ(c) - ਔਸਤ_2(c));
				}
			}

			ਲਈ (size_t r = 0; r < N; r++) {
				ਲਈ (size_t c = r; c < N; c++) {

					ਅਚਲ Type y = m2_change(r, c) - _M2_accum(r, c);
					ਅਚਲ Type t = _M2(r, c) + y;
					_M2_accum(r, c) = (t - _M2(r, c)) - y;

					_M2(r, c) = t;
				}

				// protect against floating point precision causing negative variances
				ਜੇ (_M2(r, r) < 0) {
					_M2(r, r) = 0;
				}
			}

			// make symmetric
			ਲਈ (size_t r = 0; r < N; r++) {
				ਲਈ (size_t c = r + 1; c < N; c++) {
					_M2(c, r) = _M2(r, c);
				}
			}
		}

		ਜੇ (!_M2.isAllFinite()) {
			reset();
			ਵਾਪਸ ਝੂਠ;
		}

		ਵਾਪਸ valid();
	}

	ਤਾਰਕਿਕ valid() ਅਚਲ { ਵਾਪਸ ਗਿਣਤੀ_4 > 2; }
	ਸਵੈਚਾਲਿਤ ਗਿਣਤੀ_2() ਅਚਲ { ਵਾਪਸ ਗਿਣਤੀ_4; }

	ਖਾਲੀ reset()
	{
		ਗਿਣਤੀ_4 = 0;
		ਔਸਤ_2.zero();
		_M2.zero();

		ਔਸਤ_accum.zero();
		_M2_accum.zero();
	}

	ਮੈਟ੍ਰਿਕਸ::ਸਦਿਸ਼<Type, N> ਔਸਤ() ਅਚਲ { ਵਾਪਸ ਔਸਤ_2; }
	ਮੈਟ੍ਰਿਕਸ::ਸਦਿਸ਼<Type, N> variance() ਅਚਲ { ਵਾਪਸ _M2.diag() / (ਗਿਣਤੀ_4 - 1); }
	ਮੈਟ੍ਰਿਕਸ::ਵਰਗਫਲ_ਮੈਟ੍ਰਿਕਸ<Type, N> covariance() ਅਚਲ { ਵਾਪਸ _M2 / (ਗਿਣਤੀ_4 - 1); }

	Type covariance(ਪੂਰਨ x, ਪੂਰਨ y) ਅਚਲ { ਵਾਪਸ _M2(x, y) / (ਗਿਣਤੀ_4 - 1); }

ਨਿਜੀ:

	ਮੈਟ੍ਰਿਕਸ::ਸਦਿਸ਼<Type, N> ਔਸਤ_2{};
	ਮੈਟ੍ਰਿਕਸ::ਸਦਿਸ਼<Type, N> ਔਸਤ_accum{};  ///< kahan summation algorithm accumulator for mean

	ਮੈਟ੍ਰਿਕਸ::ਵਰਗਫਲ_ਮੈਟ੍ਰਿਕਸ<Type, N> _M2{};
	ਮੈਟ੍ਰਿਕਸ::ਵਰਗਫਲ_ਮੈਟ੍ਰਿਕਸ<Type, N> _M2_accum{};    ///< kahan summation algorithm accumulator for M2

	uint16_t ਗਿਣਤੀ_4{0};
};

} // namespace math
