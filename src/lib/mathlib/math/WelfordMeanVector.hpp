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

#ಸೂಚನೆ once

ನಾಮಾಕಾಶ math
{

ಮಾದರಿ <ಬಗೆ_ಹೆಸರು Type, size_t N>
ವರ್ಗ Welford_ಸರಾಸರಿ_ಸದಿಶ
{
ಸಾರ್ವಜನಿಕ:
	// For a new value, compute the new count, new mean, the new M2.
	ತಾರ್ಕಿಕ update(ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::ಸದಿಶ<Type, N> &new_ಮೌಲ್ಯ)
	{
		ಆದರೆ (ಎಣಿಕೆ_4 == 0) {
			reset();
			ಎಣಿಕೆ_4 = 1;
			ಸರಾಸರಿ_2 = new_ಮೌಲ್ಯ;
			ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;

		} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಎಣಿಕೆ_4 == UINT16_MAX) {
			// count overflow
			//  reset count, but maintain mean and variance
			_M2 = _M2 / ಎಣಿಕೆ_4;
			_M2_accum.zero();

			ಎಣಿಕೆ_4 = 1;

		} ಇಲ್ಲದಿದ್ದರೆ {
			ಎಣಿಕೆ_4++;
		}

		//ಸರಾಸರಿ
		//  accumulates the mean of the entire dataset
		//  use Kahan summation because delta can be very small compared to the mean
		ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::ಸದಿಶ<Type, N> delta{new_ಮೌಲ್ಯ - ಸರಾಸರಿ_2};
		{
			ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::ಸದಿಶ<Type, N> y = (delta / ಎಣಿಕೆ_4) - ಸರಾಸರಿ_accum;
			ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ::ಸದಿಶ<Type, N> t = ಸರಾಸರಿ_2 + y;
			ಸರಾಸರಿ_accum = (t - ಸರಾಸರಿ_2) - y;
			ಸರಾಸರಿ_2 = t;
		}

		ಆದರೆ (!ಸರಾಸರಿ_2.isAllFinite()) {
			reset();
			ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
		}

		// covariance
		//  Kahan summation (upper triangle only)
		{
			// eg C(x,y) += dx * (y - mean_y)
			ಮಾತೃಕೆ::ವರ್ಗಫಲ_ಮಾತೃಕೆ<Type, N> m2_change{};

			ಗಾಗಿ (size_t r = 0; r < N; r++) {
				ಗಾಗಿ (size_t c = r; c < N; c++) {
					m2_change(r, c) = delta(r) * (new_ಮೌಲ್ಯ(c) - ಸರಾಸರಿ_2(c));
				}
			}

			ಗಾಗಿ (size_t r = 0; r < N; r++) {
				ಗಾಗಿ (size_t c = r; c < N; c++) {

					ಸ್ಥಿರಾಂಕ Type y = m2_change(r, c) - _M2_accum(r, c);
					ಸ್ಥಿರಾಂಕ Type t = _M2(r, c) + y;
					_M2_accum(r, c) = (t - _M2(r, c)) - y;

					_M2(r, c) = t;
				}

				// protect against floating point precision causing negative variances
				ಆದರೆ (_M2(r, r) < 0) {
					_M2(r, r) = 0;
				}
			}

			// make symmetric
			ಗಾಗಿ (size_t r = 0; r < N; r++) {
				ಗಾಗಿ (size_t c = r + 1; c < N; c++) {
					_M2(c, r) = _M2(r, c);
				}
			}
		}

		ಆದರೆ (!_M2.isAllFinite()) {
			reset();
			ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
		}

		ಹಿಂತಿರುಗಿ valid();
	}

	ತಾರ್ಕಿಕ valid() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ಎಣಿಕೆ_4 > 2; }
	ಸ್ವಯಂಚಾಲಿತ ಎಣಿಕೆ_2() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ಎಣಿಕೆ_4; }

	ಶೂನ್ಯ reset()
	{
		ಎಣಿಕೆ_4 = 0;
		ಸರಾಸರಿ_2.zero();
		_M2.zero();

		ಸರಾಸರಿ_accum.zero();
		_M2_accum.zero();
	}

	ಮಾತೃಕೆ::ಸದಿಶ<Type, N> ಸರಾಸರಿ() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ಸರಾಸರಿ_2; }
	ಮಾತೃಕೆ::ಸದಿಶ<Type, N> variance() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ _M2.diag() / (ಎಣಿಕೆ_4 - 1); }
	ಮಾತೃಕೆ::ವರ್ಗಫಲ_ಮಾತೃಕೆ<Type, N> covariance() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ _M2 / (ಎಣಿಕೆ_4 - 1); }

	Type covariance(ಪೂರ್ಣಾಂಕ x, ಪೂರ್ಣಾಂಕ y) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ _M2(x, y) / (ಎಣಿಕೆ_4 - 1); }

ಖಾಸಗಿ:

	ಮಾತೃಕೆ::ಸದಿಶ<Type, N> ಸರಾಸರಿ_2{};
	ಮಾತೃಕೆ::ಸದಿಶ<Type, N> ಸರಾಸರಿ_accum{};  ///< kahan summation algorithm accumulator for mean

	ಮಾತೃಕೆ::ವರ್ಗಫಲ_ಮಾತೃಕೆ<Type, N> _M2{};
	ಮಾತೃಕೆ::ವರ್ಗಫಲ_ಮಾತೃಕೆ<Type, N> _M2_accum{};    ///< kahan summation algorithm accumulator for M2

	uint16_t ಎಣಿಕೆ_4{0};
};

} // namespace math
