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
#अन्तर्भाव <mathlib/math/filter/AlphaFilter.hpp>

using नामाकाश math;

ढाँचा <typename T>
वर्ग FilteredDerivative
{
सार्वजनिक:
	FilteredDerivative() = स्वभाव;
	~FilteredDerivative() = स्वभाव;

	/**
	 * Set filter parameters for time abstraction
	 *
	 * @param sample_interval_s interval between two samples in seconds
	 * @param time_constant_s filter time constant determining convergence in seconds
	 */
	शून्य setParameters(भिन्न sample_interval_s, भिन्न time_constant_s)
	{
		_alpha_filter.setParameters(sample_interval_s, time_constant_s);
		_sample_interval = sample_interval_s;
	}

	/**
	 * @param sample_interval_us interval between two samples in microseconds
	 * @param time_constant_us filter time constant determining convergence in microseconds
	 */
	शून्य setParameters(uint64_t sample_interval_us, uint64_t time_constant_us)
	{
		_alpha_filter.setParameters(sample_interval_us, time_constant_us);
		_sample_interval = static_cast<भिन्न>(sample_interval_us) * 1e-6f;
	}

	/**
	 * Set filter state to an initial value
	 *
	 * @param sample new initial value
	 */
	शून्य reset(ध्रुव T &sample)
	{
		_alpha_filter.reset(sample);
		_initialized = असत्य;
	}

	/**
	 * Add a new raw value to the filter
	 *
	 * @return retrieve the filtered result
	 */
	ध्रुव T &update(ध्रुव T &sample)
	{
		यदि (_initialized) {
			यदि (_sample_interval > भग्न_अंतर) {
				_alpha_filter.update((sample - _previous_sample) / _sample_interval);

			} अन्यथा {
				_initialized = असत्य;
			}

		} अन्यथा {
			// don't update in the first iteration
			_initialized = सत्य;
		}

		_previous_sample = sample;
		प्रत्यागम _alpha_filter.getState();
	}

	ध्रुव T &getState() ध्रुव { प्रत्यागम _alpha_filter.getState(); }

	// delete mixing seconds with microseconds explicitly -> better compiler error
	शून्य setParameters(uint64_t sample_interval_us, भिन्न time_constant_s) = delete;
	शून्य setParameters(भिन्न sample_interval_s, uint64_t time_constant_us) = delete;

गुप्त:
	AlphaFilter<T> _alpha_filter;
	भिन्न _sample_interval{0.f};
	T _previous_sample{0.f};
	तार्किक _initialized{असत्य};
};
