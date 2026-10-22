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

#رهنمود once

#ضمن <float.h>
#ضمن <stdint.h>
#ضمن <mathlib/math/Functions.hpp>

استفاده نطاق math;

قالب <نام_نوع T>
صنف AlphaFilter
{
عام:
	AlphaFilter() = افتراضي;
	صریح AlphaFilter(عائم sample_interval_s, عائم زمان_constant_s) { مجموعه_Parameters(sample_interval_s, زمان_constant_s); }
	صریح AlphaFilter(uint64_t sample_interval_us, uint64_t زمان_constant_us) { مجموعه_Parameters(sample_interval_us, زمان_constant_us); }
	صریح AlphaFilter(عائم زمان_constant_s) : زمان_constant(زمان_constant_s) {};
	صریح AlphaFilter(uint64_t زمان_constant_us) : زمان_constant(static_cast<عائم>(زمان_constant_us) * 1e-6f) {};

	~AlphaFilter() = افتراضي;

	/**
	 * Set filter parameters for time abstraction
	 *
	 * @param sample_interval_s interval between two samples in seconds
	 * @param time_constant_s filter time constant determining convergence in seconds
	 */
	فراغ مجموعه_Parameters(عائم sample_interval_s, عائم زمان_constant_s)
	{
		ثابت عائم denominator = زمان_constant_s + sample_interval_s;

		إذا (denominator > FLT_EPSILON) {
			مجموعه_Alpha(sample_interval_s / denominator);
		}

		زمان_constant = زمان_constant_s;
	}

	/**
	 * @param sample_interval_us interval between two samples in microseconds
	 * @param time_constant_us filter time constant determining convergence in microseconds
	 */
	فراغ مجموعه_Parameters(uint64_t sample_interval_us, uint64_t زمان_constant_us)
	{
		مجموعه_Parameters(static_cast<عائم>(sample_interval_us) * 1e-6f,
			      static_cast<عائم>(زمان_constant_us) * 1e-6f);
	}

	منطقي مجموعه_Cutoff_Freq(عائم sample_freq, عائم cutoff_freq)
	{
		إذا ((sample_freq <= 0.f) || (cutoff_freq <= 0.f) || (cutoff_freq >= sample_freq / 2.f)
		    || !isFinite(sample_freq) || !isFinite(cutoff_freq)) {

			// Invalid parameters
			أرجع خطأ;
		}

		مجموعه_Parameters(1.f / sample_freq, 1.f / (M_TWOPI_F * cutoff_freq));
		أرجع درست;
	}

	فراغ مجموعه_Cutoff_Freq(عائم cutoff_freq)
	{
		إذا (cutoff_freq > FLT_EPSILON) {
			زمان_constant = 1.f / (M_TWOPI_F * cutoff_freq);

		} وإلا {
			زمان_constant = 0.f;
		}
	}

	/**
	 * Set filter parameter alpha directly without time abstraction
	 *
	 * @param alpha [0,1] filter weight for the previous state. High value - long time constant.
	 */
	فراغ مجموعه_Alpha(عائم alpha) { _alpha = alpha; }

	/**
	 * Set filter state to an initial value
	 *
	 * @param sample new initial value
	 */
	فراغ reset(ثابت T &sample) { _filter_state = sample; }

	/**
	 * Add a new raw value to the filter
	 *
	 * @return retrieve the filtered result
	 */
	ثابت T &update(ثابت T &sample)
	{
		_filter_state = updateCalculation(sample);
		أرجع _filter_state;
	}

	ثابت T update(ثابت T &sample, عائم dt_s)
	{
		مجموعه_Parameters(dt_s, زمان_constant);
		أرجع update(sample);
	}

	ثابت T update(ثابت T &sample, uint64_t dt_us)
	{
		مجموعه_Parameters(static_cast<عائم>(dt_us) * 1e-6f, زمان_constant);
		أرجع update(sample);
	}

	ثابت T &getState() ثابت { أرجع _filter_state; }
	// a time constant of 0 means no filtering, i.e. an infinite cutoff frequency
	عائم getCutoffFreq() ثابت { أرجع (زمان_constant > FLT_EPSILON) ? 1.f / (M_TWOPI_F * زمان_constant) : INFINITY; }

	// delete mixing seconds with microseconds explicitly -> better compiler error
	AlphaFilter(uint64_t sample_interval_us, عائم زمان_constant_s) = حذف;
	AlphaFilter(عائم sample_interval_s, uint64_t زمان_constant_us) = حذف;
	فراغ مجموعه_Parameters(uint64_t sample_interval_us, عائم زمان_constant_s) = حذف;
	فراغ مجموعه_Parameters(عائم sample_interval_s, uint64_t زمان_constant_us) = حذف;

محمي:
	T updateCalculation(ثابت T &sample);

	عائم زمان_constant{0.f};
	عائم _alpha{0.f};
	T _filter_state{};
};

قالب <نام_نوع T>
T AlphaFilter<T>::updateCalculation(ثابت T &sample) { أرجع _filter_state + _alpha * (sample - _filter_state); }

/* Specialization for 3D rotations
 * The filter is computed on the 3-sphere of unit quaternions instead of the cartesian space
 * Additions and subtractions are done using the quaternion multiplication and
 * the error is scaled on the tangent space.
 */
قالب <> مضمن
ماتریس::Quatf AlphaFilter<ماتریس::Quatf>::updateCalculation(ثابت ماتریس::Quatf &sample)
{
	ماتریس::Quatf q_error(_filter_state.inversed() * sample);
	q_error.canonicalize(); // prevent unwrapping
	أرجع _filter_state * ماتریس::Quatf(ماتریس::AxisAnglef(_alpha * ماتریس::AxisAnglef(q_error)));
}
