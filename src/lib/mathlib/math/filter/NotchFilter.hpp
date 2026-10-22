/****************************************************************************
 *
 *   Copyright (C) 2019-2021 PX4 Development Team. All rights reserved.
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

/*
 * @file NotchFilter.hpp
 *
 * @brief Implementation of a Notch filter.
 *
 * @author Mathieu Bresciani <brescianimathieu@gmail.com>
 * @author Samuel Garcin <samuel.garcin@wecorpindustries.com>
 */

#pragma once

#অন্তর্ভাব <mathlib/math/Functions.hpp>
#অন্তর্ভাব <cmath>
#অন্তর্ভাব <float.h>
#অন্তর্ভাব <matrix/math.hpp>

নামাকাশ math
{

template<typename T>
শ্রেণী NotchFilter
{
সর্বজনীন:
	NotchFilter() = পূর্বনির্ধারিত;
	~NotchFilter() = পূর্বনির্ধারিত;

	যৌক্তিক setParameters(দশমিক sample_freq, দশমিক notch_freq, দশমিক bandwidth);

	/**
	 * Add a new raw value to the filter using the Direct Form I
	 *
	 * @return retrieve the filtered result
	 */
	অন্তঃরেখ T apply(ধ্রুবক T &sample)
	{
		যদি (!_initialized) {
			reset(sample);
			_initialized = সত্য;
		}

		ফেরত applyInternal(sample);
	}

	// Filter array of samples in place using the direct form I
	অন্তঃরেখ শূন্য applyArray(T samples[], পূর্ণসংখ্যা অঙ্ক_samples)
	{
		যদি (!_initialized) {
			reset(samples[0]);
			_initialized = সত্য;
		}

		জন্য (পূর্ণসংখ্যা n = 0; n < অঙ্ক_samples; n++) {
			samples[n] = applyInternal(samples[n]);
		}
	}

	দশমিক getNotchFreq() ধ্রুবক { ফেরত _notch_freq; }
	দশমিক getBandwidth() ধ্রুবক { ফেরত _bandwidth; }

	// Used in unit test only
	শূন্য getCoefficients(দশমিক a[3], দশমিক b[3]) ধ্রুবক
	{
		a[0] = 1.f;
		a[1] = _a1;
		a[2] = _a2;
		b[0] = _b0;
		b[1] = _b1;
		b[2] = _b2;
	}

	দশমিক getMagnitudeResponse(দশমিক frequency) ধ্রুবক
	{
		দশমিক w = 2.f * M_PI_F * frequency / _sample_freq;

		দশমিক numerator = _b0 * _b0 + _b1 * _b1 + _b2 * _b2
				  + 2.f * (_b0 * _b1 + _b1 * _b2) * cosf(w) + 2.f * _b0 * _b2 * cosf(2.f * w);

		দশমিক denominator = 1.f + _a1 * _a1 + _a2 * _a2 + 2.f * (_a1 + _a1 * _a2) * cosf(w) + 2.f * _a2 * cosf(2.f * w);

		ফেরত sqrtf(numerator / denominator);
	}

	/**
	 * Bypasses the filter update to directly set different filter coefficients.
	 * Note: the filtered frequency and quality factor saved on the filter lose their
	 * physical meaning if you use this method to change the filter's coefficients.
	 * Used for creating clones of a specific filter.
	 */
	শূন্য setCoefficients(দশমিক a[2], দশমিক b[3])
	{
		_a1 = a[0];
		_a2 = a[1];
		_b0 = b[0];
		_b1 = b[1];
		_b2 = b[2];
	}

	যৌক্তিক initialized() ধ্রুবক { ফেরত _initialized; }

	শূন্য reset() { _initialized = মিথ্যা; }

	শূন্য reset(ধ্রুবক T &sample)
	{
		ধ্রুবক T প্রবেশ = isFinite(sample) ? sample : T{};

		_delay_element_1 = _delay_element_2 = প্রবেশ;
		_delay_element_output_1 = _delay_element_output_2 = প্রবেশ * (_b0 + _b1 + _b2) / (1 + _a1 + _a2);

		যদি (!isFinite(_delay_element_1) || !isFinite(_delay_element_2)) {
			_delay_element_output_1 = _delay_element_output_2 = {};
		}

		_initialized = সত্য;
	}

	শূন্য disable()
	{
		// no filtering
		_notch_freq = 0.f;
		_bandwidth = 0.f;
		_sample_freq = 0.f;

		_b0 = 1.f;
		_b1 = 0.f;
		_b2 = 0.f;

		_a1 = 0.f;
		_a2 = 0.f;

		_initialized = মিথ্যা;
	}

সুরক্ষিত:

	/**
	 * Add a new raw value to the filter using the Direct Form I
	 *
	 * @return retrieve the filtered result
	 */
	অন্তঃরেখ T applyInternal(ধ্রুবক T &sample)
	{
		// Direct Form I implementation
		T output = _b0 * sample + _b1 * _delay_element_1 + _b2 * _delay_element_2 - _a1 * _delay_element_output_1 - _a2 *
			   _delay_element_output_2;

		// shift inputs
		_delay_element_2 = _delay_element_1;
		_delay_element_1 = sample;

		// shift outputs
		_delay_element_output_2 = _delay_element_output_1;
		_delay_element_output_1 = output;

		ফেরত output;
	}

	T _delay_element_1{};
	T _delay_element_2{};
	T _delay_element_output_1{};
	T _delay_element_output_2{};

	// All the coefficients are normalized by a0, so a0 becomes 1 here
	দশমিক _a1{0.f};
	দশমিক _a2{0.f};

	দশমিক _b0{1.f};
	দশমিক _b1{0.f};
	দশমিক _b2{0.f};

	দশমিক _notch_freq{};
	দশমিক _bandwidth{};
	দশমিক _sample_freq{};

	যৌক্তিক _initialized{মিথ্যা};
};

/**
 * Initialises the filter by setting its parameters and coefficients.
 * Using the direct form I method, allows to dynamically
 * update the filtered frequency, refresh rate and quality factor while
 * conserving the filter's history
 */
template<typename T>
যৌক্তিক NotchFilter<T>::setParameters(দশমিক sample_freq, দশমিক notch_freq, দশমিক bandwidth)
{
	যদি ((sample_freq <= 0.f) || (notch_freq <= 0.f) || (bandwidth <= 0.f) || (notch_freq >= sample_freq / 2)
	    || !isFinite(sample_freq) || !isFinite(notch_freq) || !isFinite(bandwidth)) {

		disable();
		ফেরত মিথ্যা;
	}

	ধ্রুবক দশমিক freq_min = sample_freq * 0.001f;

	ধ্রুবক দশমিক notch_freq_new = math::max(notch_freq, freq_min);
	ধ্রুবক দশমিক bandwidth_new = math::max(bandwidth, freq_min);

	ধ্রুবক দশমিক sample_freq_diff = fabsf(sample_freq - _sample_freq);
	ধ্রুবক দশমিক bandwidth_diff = fabsf(bandwidth_new - _bandwidth);

	ধ্রুবক যৌক্তিক sample_freq_change = (sample_freq_diff > FLT_EPSILON);
	ধ্রুবক যৌক্তিক bandwidth_change = (bandwidth_diff > FLT_EPSILON);

	যদি (!sample_freq_change && !bandwidth_change) {
		ধ্রুবক দশমিক notch_freq_diff = fabsf(notch_freq_new - _notch_freq);

		যদি (notch_freq_diff > FLT_EPSILON) {
			// only notch frequency has changed
			_notch_freq = notch_freq_new;

			ধ্রুবক দশমিক beta = -cosf(2.f * M_PI_F * _notch_freq / _sample_freq);

			_b1 = 2.f * beta * _b0;
			_a1 = _b1;

			যদি (notch_freq_diff > _bandwidth) {
				// force reset
				_initialized = মিথ্যা;
			}

			যদি (!isFinite(_b1)) {
				disable();
				ফেরত মিথ্যা;
			}

			ফেরত সত্য;

		} অন্যথা {
			// no change, do nothing
			ফেরত সত্য;
		}
	}

	_sample_freq = sample_freq;
	_notch_freq = notch_freq_new;
	_bandwidth = bandwidth_new;

	ধ্রুবক দশমিক alpha = tanf(M_PI_F * _bandwidth / _sample_freq);
	ধ্রুবক দশমিক beta = -cosf(2.f * M_PI_F * _notch_freq / _sample_freq);
	ধ্রুবক দশমিক a0_inv = 1.f / (alpha + 1.f);

	_b0 = a0_inv;
	_b1 = 2.f * beta * a0_inv;
	_b2 = a0_inv;

	_a1 = _b1;
	_a2 = (1.f - alpha) * a0_inv;

	যদি (!isFinite(_b0) || !isFinite(_b1) || !isFinite(_b2) || !isFinite(_a2)) {
		disable();
		ফেরত মিথ্যা;
	}

	// force reset if bandwidth or sample frequency changed by more than 1%
	যদি (bandwidth_diff > 0.01f * _bandwidth) {
		_initialized = মিথ্যা;

	} অন্যথা যদি (sample_freq_diff > 0.01f * _sample_freq) {
		_initialized = মিথ্যা;
	}

	ফেরত সত্য;
}

} // namespace math
