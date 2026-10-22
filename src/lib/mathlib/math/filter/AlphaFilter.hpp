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

#شامل <float.h>
#شامل <stdint.h>
#شامل <mathlib/math/Functions.hpp>

using فضاینام math;

template <typename T>
رده AlphaFilter
{
عمومی:
	AlphaFilter() = پیشفرض;
	explicit AlphaFilter(اعشاری sample_interval_s, اعشاری time_constant_s) { setParameters(sample_interval_s, time_constant_s); }
	explicit AlphaFilter(uint64_t sample_interval_us, uint64_t time_constant_us) { setParameters(sample_interval_us, time_constant_us); }
	explicit AlphaFilter(اعشاری time_constant_s) : _time_constant(time_constant_s) {};
	explicit AlphaFilter(uint64_t time_constant_us) : _time_constant(static_cast<اعشاری>(time_constant_us) * 1e-6f) {};

	~AlphaFilter() = پیشفرض;

	/**
	 * Set filter parameters for time abstraction
	 *
	 * @param sample_interval_s interval between two samples in seconds
	 * @param time_constant_s filter time constant determining convergence in seconds
	 */
	تهی setParameters(اعشاری sample_interval_s, اعشاری time_constant_s)
	{
		ثابت اعشاری denominator = time_constant_s + sample_interval_s;

		اگر (denominator > FLT_EPSILON) {
			setAlpha(sample_interval_s / denominator);
		}

		_time_constant = time_constant_s;
	}

	/**
	 * @param sample_interval_us interval between two samples in microseconds
	 * @param time_constant_us filter time constant determining convergence in microseconds
	 */
	تهی setParameters(uint64_t sample_interval_us, uint64_t time_constant_us)
	{
		setParameters(static_cast<اعشاری>(sample_interval_us) * 1e-6f,
			      static_cast<اعشاری>(time_constant_us) * 1e-6f);
	}

	منطقی setCutoffFreq(اعشاری sample_freq, اعشاری cutoff_freq)
	{
		اگر ((sample_freq <= 0.f) || (cutoff_freq <= 0.f) || (cutoff_freq >= sample_freq / 2.f)
		    || !isFinite(sample_freq) || !isFinite(cutoff_freq)) {

			// Invalid parameters
			بازگشت نادرست;
		}

		setParameters(1.f / sample_freq, 1.f / (M_TWOPI_F * cutoff_freq));
		بازگشت درست;
	}

	تهی setCutoffFreq(اعشاری cutoff_freq)
	{
		اگر (cutoff_freq > FLT_EPSILON) {
			_time_constant = 1.f / (M_TWOPI_F * cutoff_freq);

		} وگرنه {
			_time_constant = 0.f;
		}
	}

	/**
	 * Set filter parameter alpha directly without time abstraction
	 *
	 * @param alpha [0,1] filter weight for the previous state. High value - long time constant.
	 */
	تهی setAlpha(اعشاری alpha) { _alpha = alpha; }

	/**
	 * Set filter state to an initial value
	 *
	 * @param sample new initial value
	 */
	تهی reset(ثابت T &sample) { _filter_state = sample; }

	/**
	 * Add a new raw value to the filter
	 *
	 * @return retrieve the filtered result
	 */
	ثابت T &update(ثابت T &sample)
	{
		_filter_state = updateCalculation(sample);
		بازگشت _filter_state;
	}

	ثابت T update(ثابت T &sample, اعشاری dt_s)
	{
		setParameters(dt_s, _time_constant);
		بازگشت update(sample);
	}

	ثابت T update(ثابت T &sample, uint64_t dt_us)
	{
		setParameters(static_cast<اعشاری>(dt_us) * 1e-6f, _time_constant);
		بازگشت update(sample);
	}

	ثابت T &getState() ثابت { بازگشت _filter_state; }
	// a time constant of 0 means no filtering, i.e. an infinite cutoff frequency
	اعشاری getCutoffFreq() ثابت { بازگشت (_time_constant > FLT_EPSILON) ? 1.f / (M_TWOPI_F * _time_constant) : INFINITY; }

	// delete mixing seconds with microseconds explicitly -> better compiler error
	AlphaFilter(uint64_t sample_interval_us, اعشاری time_constant_s) = delete;
	AlphaFilter(اعشاری sample_interval_s, uint64_t time_constant_us) = delete;
	تهی setParameters(uint64_t sample_interval_us, اعشاری time_constant_s) = delete;
	تهی setParameters(اعشاری sample_interval_s, uint64_t time_constant_us) = delete;

محافظتشده:
	T updateCalculation(ثابت T &sample);

	اعشاری _time_constant{0.f};
	اعشاری _alpha{0.f};
	T _filter_state{};
};

template <typename T>
T AlphaFilter<T>::updateCalculation(ثابت T &sample) { بازگشت _filter_state + _alpha * (sample - _filter_state); }

/* Specialization for 3D rotations
 * The filter is computed on the 3-sphere of unit quaternions instead of the cartesian space
 * Additions and subtractions are done using the quaternion multiplication and
 * the error is scaled on the tangent space.
 */
template <> درونخطی
matrix::Quatf AlphaFilter<matrix::Quatf>::updateCalculation(ثابت matrix::Quatf &sample)
{
	matrix::Quatf q_error(_filter_state.inversed() * sample);
	q_error.canonicalize(); // prevent unwrapping
	بازگشت _filter_state * matrix::Quatf(matrix::AxisAnglef(_alpha * matrix::AxisAnglef(q_error)));
}
