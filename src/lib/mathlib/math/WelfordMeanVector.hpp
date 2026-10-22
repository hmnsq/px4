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

নামাকাশ math
{

template <typename Type, size_t N>
বর্গ WelfordMeanVector
{
সার্বজনিক:
	// For a new value, compute the new count, new mean, the new M2.
	তার্কিক update(ধ্রুব matrix::Vector<Type, N> &new_মান)
	{
		যদি (গুনতি_3 == 0) {
			reset();
			গুনতি_3 = 1;
			_mean = new_মান;
			প্রত্যাগম অসত্য;

		} অন্যথা যদি (গুনতি_3 == UINT16_MAX) {
			// count overflow
			//  reset count, but maintain mean and variance
			_M2 = _M2 / গুনতি_3;
			_M2_accum.zero();

			গুনতি_3 = 1;

		} অন্যথা {
			গুনতি_3++;
		}

		// mean
		//  accumulates the mean of the entire dataset
		//  use Kahan summation because delta can be very small compared to the mean
		ধ্রুব matrix::Vector<Type, N> delta{new_মান - _mean};
		{
			ধ্রুব matrix::Vector<Type, N> y = (delta / গুনতি_3) - _mean_accum;
			ধ্রুব matrix::Vector<Type, N> t = _mean + y;
			_mean_accum = (t - _mean) - y;
			_mean = t;
		}

		যদি (!_mean.isAllFinite()) {
			reset();
			প্রত্যাগম অসত্য;
		}

		// covariance
		//  Kahan summation (upper triangle only)
		{
			// eg C(x,y) += dx * (y - mean_y)
			matrix::SquareMatrix<Type, N> m2_change{};

			প্রতি (size_t r = 0; r < N; r++) {
				প্রতি (size_t c = r; c < N; c++) {
					m2_change(r, c) = delta(r) * (new_মান(c) - _mean(c));
				}
			}

			প্রতি (size_t r = 0; r < N; r++) {
				প্রতি (size_t c = r; c < N; c++) {

					ধ্রুব Type y = m2_change(r, c) - _M2_accum(r, c);
					ধ্রুব Type t = _M2(r, c) + y;
					_M2_accum(r, c) = (t - _M2(r, c)) - y;

					_M2(r, c) = t;
				}

				// protect against floating point precision causing negative variances
				যদি (_M2(r, r) < 0) {
					_M2(r, r) = 0;
				}
			}

			// make symmetric
			প্রতি (size_t r = 0; r < N; r++) {
				প্রতি (size_t c = r + 1; c < N; c++) {
					_M2(c, r) = _M2(r, c);
				}
			}
		}

		যদি (!_M2.isAllFinite()) {
			reset();
			প্রত্যাগম অসত্য;
		}

		প্রত্যাগম valid();
	}

	তার্কিক valid() ধ্রুব { প্রত্যাগম গুনতি_3 > 2; }
	auto গুনতি() ধ্রুব { প্রত্যাগম গুনতি_3; }

	শূন্য reset()
	{
		গুনতি_3 = 0;
		_mean.zero();
		_M2.zero();

		_mean_accum.zero();
		_M2_accum.zero();
	}

	matrix::Vector<Type, N> mean() ধ্রুব { প্রত্যাগম _mean; }
	matrix::Vector<Type, N> variance() ধ্রুব { প্রত্যাগম _M2.diag() / (গুনতি_3 - 1); }
	matrix::SquareMatrix<Type, N> covariance() ধ্রুব { প্রত্যাগম _M2 / (গুনতি_3 - 1); }

	Type covariance(পূর্ণাঙ্ক x, পূর্ণাঙ্ক y) ধ্রুব { প্রত্যাগম _M2(x, y) / (গুনতি_3 - 1); }

গুপ্ত:

	matrix::Vector<Type, N> _mean{};
	matrix::Vector<Type, N> _mean_accum{};  ///< kahan summation algorithm accumulator for mean

	matrix::SquareMatrix<Type, N> _M2{};
	matrix::SquareMatrix<Type, N> _M2_accum{};    ///< kahan summation algorithm accumulator for M2

	uint16_t গুনতি_3{0};
};

} // namespace math
