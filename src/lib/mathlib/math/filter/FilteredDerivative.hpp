/****************************************************************************
 *
 *   Copyright (c) 2024-2026 PX4 Development Team. All rights reserved.
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
 * @file FilteredDerivative.hpp
 *
 * @brief Derivative function passed through a first order "alpha" IIR digital filter
 *
 * @author Silvan Fuhrer <silvan@auterion.com>
 */

#pragma once

// #include <float.h>
// #include <mathlib/math/Functions.hpp>
#অন্তর্ভাব <mathlib/math/filter/AlphaFilter.hpp>

using নামাকাশ math;

template <typename T>
বর্গ FilteredDerivative
{
সার্বজনিক:
	FilteredDerivative() = স্বভাব;
	~FilteredDerivative() = স্বভাব;

	/**
	 * Set filter parameters for time abstraction
	 *
	 * @param sample_interval_s interval between two samples in seconds
	 * @param time_constant_s filter time constant determining convergence in seconds
	 */
	শূন্য setParameters(ভিন্ন sample_interval_s, ভিন্ন time_constant_s)
	{
		_alpha_filter.setParameters(sample_interval_s, time_constant_s);
		_sample_interval = sample_interval_s;
	}

	/**
	 * @param sample_interval_us interval between two samples in microseconds
	 * @param time_constant_us filter time constant determining convergence in microseconds
	 */
	শূন্য setParameters(uint64_t sample_interval_us, uint64_t time_constant_us)
	{
		_alpha_filter.setParameters(sample_interval_us, time_constant_us);
		_sample_interval = static_cast<ভিন্ন>(sample_interval_us) * 1e-6f;
	}

	/**
	 * Set filter state to an initial value
	 *
	 * @param sample new initial value
	 */
	শূন্য reset(ধ্রুব T &sample)
	{
		_alpha_filter.reset(sample);
		_initialized = অসত্য;
	}

	/**
	 * Add a new raw value to the filter
	 *
	 * @return retrieve the filtered result
	 */
	ধ্রুব T &update(ধ্রুব T &sample)
	{
		যদি (_initialized) {
			যদি (_sample_interval > FLT_EPSILON) {
				_alpha_filter.update((sample - _previous_sample) / _sample_interval);

			} অন্যথা {
				_initialized = অসত্য;
			}

		} অন্যথা {
			// don't update in the first iteration
			_initialized = সত্য;
		}

		_previous_sample = sample;
		প্রত্যাগম _alpha_filter.getState();
	}

	ধ্রুব T &getState() ধ্রুব { প্রত্যাগম _alpha_filter.getState(); }

	// delete mixing seconds with microseconds explicitly -> better compiler error
	শূন্য setParameters(uint64_t sample_interval_us, ভিন্ন time_constant_s) = delete;
	শূন্য setParameters(ভিন্ন sample_interval_s, uint64_t time_constant_us) = delete;

গুপ্ত:
	AlphaFilter<T> _alpha_filter;
	ভিন্ন _sample_interval{0.f};
	T _previous_sample{0.f};
	তার্কিক _initialized{অসত্য};
};
