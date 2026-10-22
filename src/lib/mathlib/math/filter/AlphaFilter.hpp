/****************************************************************************
 *
 *   Copyright (c) 2019-2026 PX4 Development Team. All rights reserved.
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
 * @file AlphaFilter.hpp
 *
 * @brief First order "alpha" IIR digital filter also known as leaky integrator or forgetting average.
 *
 * @author Mathieu Bresciani <brescianimathieu@gmail.com>
 * @author Matthias Grob <maetugr@gmail.com>
 */

#pragma once

#अन्तर्भाव <float.h>
#अन्तर्भाव <stdint.h>
#अन्तर्भाव <mathlib/math/Functions.hpp>

using नामाकाश math;

ढाँचा <typename T>
वर्ग AlphaFilter
{
सार्वजनिक:
	AlphaFilter() = स्वभाव;
	explicit AlphaFilter(भिन्न sample_interval_s, भिन्न time_constant_s) { setParameters(sample_interval_s, time_constant_s); }
	explicit AlphaFilter(uint64_t sample_interval_us, uint64_t time_constant_us) { setParameters(sample_interval_us, time_constant_us); }
	explicit AlphaFilter(भिन्न time_constant_s) : _time_constant(time_constant_s) {};
	explicit AlphaFilter(uint64_t time_constant_us) : _time_constant(static_cast<भिन्न>(time_constant_us) * 1e-6f) {};

	~AlphaFilter() = स्वभाव;

	/**
	 * Set filter parameters for time abstraction
	 *
	 * @param sample_interval_s interval between two samples in seconds
	 * @param time_constant_s filter time constant determining convergence in seconds
	 */
	शून्य setParameters(भिन्न sample_interval_s, भिन्न time_constant_s)
	{
		ध्रुव भिन्न denominator = time_constant_s + sample_interval_s;

		यदि (denominator > भग्न_अंतर) {
			setAlpha(sample_interval_s / denominator);
		}

		_time_constant = time_constant_s;
	}

	/**
	 * @param sample_interval_us interval between two samples in microseconds
	 * @param time_constant_us filter time constant determining convergence in microseconds
	 */
	शून्य setParameters(uint64_t sample_interval_us, uint64_t time_constant_us)
	{
		setParameters(static_cast<भिन्न>(sample_interval_us) * 1e-6f,
			      static_cast<भिन्न>(time_constant_us) * 1e-6f);
	}

	तार्किक setCutoffFreq(भिन्न sample_freq, भिन्न cutoff_freq)
	{
		यदि ((sample_freq <= 0.f) || (cutoff_freq <= 0.f) || (cutoff_freq >= sample_freq / 2.f)
		    || !isFinite(sample_freq) || !isFinite(cutoff_freq)) {

			// Invalid parameters
			प्रत्यागम असत्य;
		}

		setParameters(1.f / sample_freq, 1.f / (M_TWOPI_F * cutoff_freq));
		प्रत्यागम सत्य;
	}

	शून्य setCutoffFreq(भिन्न cutoff_freq)
	{
		यदि (cutoff_freq > भग्न_अंतर) {
			_time_constant = 1.f / (M_TWOPI_F * cutoff_freq);

		} अन्यथा {
			_time_constant = 0.f;
		}
	}

	/**
	 * Set filter parameter alpha directly without time abstraction
	 *
	 * @param alpha [0,1] filter weight for the previous state. High value - long time constant.
	 */
	शून्य setAlpha(भिन्न alpha) { _alpha = alpha; }

	/**
	 * Set filter state to an initial value
	 *
	 * @param sample new initial value
	 */
	शून्य reset(ध्रुव T &sample) { _filter_state = sample; }

	/**
	 * Add a new raw value to the filter
	 *
	 * @return retrieve the filtered result
	 */
	ध्रुव T &update(ध्रुव T &sample)
	{
		_filter_state = updateCalculation(sample);
		प्रत्यागम _filter_state;
	}

	ध्रुव T update(ध्रुव T &sample, भिन्न dt_s)
	{
		setParameters(dt_s, _time_constant);
		प्रत्यागम update(sample);
	}

	ध्रुव T update(ध्रुव T &sample, uint64_t dt_us)
	{
		setParameters(static_cast<भिन्न>(dt_us) * 1e-6f, _time_constant);
		प्रत्यागम update(sample);
	}

	ध्रुव T &getState() ध्रुव { प्रत्यागम _filter_state; }
	// a time constant of 0 means no filtering, i.e. an infinite cutoff frequency
	भिन्न getCutoffFreq() ध्रुव { प्रत्यागम (_time_constant > भग्न_अंतर) ? 1.f / (M_TWOPI_F * _time_constant) : अनन्त; }

	// delete mixing seconds with microseconds explicitly -> better compiler error
	AlphaFilter(uint64_t sample_interval_us, भिन्न time_constant_s) = delete;
	AlphaFilter(भिन्न sample_interval_s, uint64_t time_constant_us) = delete;
	शून्य setParameters(uint64_t sample_interval_us, भिन्न time_constant_s) = delete;
	शून्य setParameters(भिन्न sample_interval_s, uint64_t time_constant_us) = delete;

रक्षित:
	T updateCalculation(ध्रुव T &sample);

	भिन्न _time_constant{0.f};
	भिन्न _alpha{0.f};
	T _filter_state{};
};

ढाँचा <typename T>
T AlphaFilter<T>::updateCalculation(ध्रुव T &sample) { प्रत्यागम _filter_state + _alpha * (sample - _filter_state); }

/* Specialization for 3D rotations
 * The filter is computed on the 3-sphere of unit quaternions instead of the cartesian space
 * Additions and subtractions are done using the quaternion multiplication and
 * the error is scaled on the tangent space.
 */
ढाँचा <> अन्तःस्थ
matrix::Quatf AlphaFilter<matrix::Quatf>::updateCalculation(ध्रुव matrix::Quatf &sample)
{
	matrix::Quatf q_error(_filter_state.inversed() * sample);
	q_error.canonicalize(); // prevent unwrapping
	प्रत्यागम _filter_state * matrix::Quatf(matrix::AxisAnglef(_alpha * matrix::AxisAnglef(q_error)));
}
