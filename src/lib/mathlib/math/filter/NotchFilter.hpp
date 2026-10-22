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

#ଅନ୍ତର୍ଭାଵ <mathlib/math/Functions.hpp>
#ଅନ୍ତର୍ଭାଵ <cmath>
#ଅନ୍ତର୍ଭାଵ <float.h>
#ଅନ୍ତର୍ଭାଵ <matrix/math.hpp>

ନାମାକାଶ math
{

template<typename T>
ଶ୍ରେଣୀ NotchFilter
{
ସର୍ବସାଧାରଣ:
	NotchFilter() = ପୂର୍ବନିର୍ଦ୍ଧାରିତ;
	~NotchFilter() = ପୂର୍ବନିର୍ଦ୍ଧାରିତ;

	ତାର୍କିକ setParameters(ଦଶମିକ sample_freq, ଦଶମିକ notch_freq, ଦଶମିକ bandwidth);

	/**
	 * Add a new raw value to the filter using the Direct Form I
	 *
	 * @return retrieve the filtered result
	 */
	ଧାଡ଼ିଭିତରେ T apply(ଅଚଳ T &sample)
	{
		ଯଦି (!_initialized) {
			reset(sample);
			_initialized = ସତ୍ୟ;
		}

		ଫେରନ୍ତୁ applyInternal(sample);
	}

	// Filter array of samples in place using the direct form I
	ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ applyArray(T samples[], ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ_samples)
	{
		ଯଦି (!_initialized) {
			reset(samples[0]);
			_initialized = ସତ୍ୟ;
		}

		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା n = 0; n < ଅଙ୍କ_samples; n++) {
			samples[n] = applyInternal(samples[n]);
		}
	}

	ଦଶମିକ getNotchFreq() ଅଚଳ { ଫେରନ୍ତୁ _notch_freq; }
	ଦଶମିକ getBandwidth() ଅଚଳ { ଫେରନ୍ତୁ _bandwidth; }

	// Used in unit test only
	ଶୂନ୍ୟ getCoefficients(ଦଶମିକ a[3], ଦଶମିକ b[3]) ଅଚଳ
	{
		a[0] = 1.f;
		a[1] = _a1;
		a[2] = _a2;
		b[0] = _b0;
		b[1] = _b1;
		b[2] = _b2;
	}

	ଦଶମିକ getMagnitudeResponse(ଦଶମିକ frequency) ଅଚଳ
	{
		ଦଶମିକ w = 2.f * M_PI_F * frequency / _sample_freq;

		ଦଶମିକ numerator = _b0 * _b0 + _b1 * _b1 + _b2 * _b2
				  + 2.f * (_b0 * _b1 + _b1 * _b2) * cosf(w) + 2.f * _b0 * _b2 * cosf(2.f * w);

		ଦଶମିକ denominator = 1.f + _a1 * _a1 + _a2 * _a2 + 2.f * (_a1 + _a1 * _a2) * cosf(w) + 2.f * _a2 * cosf(2.f * w);

		ଫେରନ୍ତୁ sqrtf(numerator / denominator);
	}

	/**
	 * Bypasses the filter update to directly set different filter coefficients.
	 * Note: the filtered frequency and quality factor saved on the filter lose their
	 * physical meaning if you use this method to change the filter's coefficients.
	 * Used for creating clones of a specific filter.
	 */
	ଶୂନ୍ୟ setCoefficients(ଦଶମିକ a[2], ଦଶମିକ b[3])
	{
		_a1 = a[0];
		_a2 = a[1];
		_b0 = b[0];
		_b1 = b[1];
		_b2 = b[2];
	}

	ତାର୍କିକ initialized() ଅଚଳ { ଫେରନ୍ତୁ _initialized; }

	ଶୂନ୍ୟ reset() { _initialized = ମିଥ୍ୟା; }

	ଶୂନ୍ୟ reset(ଅଚଳ T &sample)
	{
		ଅଚଳ T ନିବେଶ = isFinite(sample) ? sample : T{};

		_delay_element_1 = _delay_element_2 = ନିବେଶ;
		_delay_element_output_1 = _delay_element_output_2 = ନିବେଶ * (_b0 + _b1 + _b2) / (1 + _a1 + _a2);

		ଯଦି (!isFinite(_delay_element_1) || !isFinite(_delay_element_2)) {
			_delay_element_output_1 = _delay_element_output_2 = {};
		}

		_initialized = ସତ୍ୟ;
	}

	ଶୂନ୍ୟ disable()
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

		_initialized = ମିଥ୍ୟା;
	}

ସୁରକ୍ଷିତ:

	/**
	 * Add a new raw value to the filter using the Direct Form I
	 *
	 * @return retrieve the filtered result
	 */
	ଧାଡ଼ିଭିତରେ T applyInternal(ଅଚଳ T &sample)
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

		ଫେରନ୍ତୁ output;
	}

	T _delay_element_1{};
	T _delay_element_2{};
	T _delay_element_output_1{};
	T _delay_element_output_2{};

	// All the coefficients are normalized by a0, so a0 becomes 1 here
	ଦଶମିକ _a1{0.f};
	ଦଶମିକ _a2{0.f};

	ଦଶମିକ _b0{1.f};
	ଦଶମିକ _b1{0.f};
	ଦଶମିକ _b2{0.f};

	ଦଶମିକ _notch_freq{};
	ଦଶମିକ _bandwidth{};
	ଦଶମିକ _sample_freq{};

	ତାର୍କିକ _initialized{ମିଥ୍ୟା};
};

/**
 * Initialises the filter by setting its parameters and coefficients.
 * Using the direct form I method, allows to dynamically
 * update the filtered frequency, refresh rate and quality factor while
 * conserving the filter's history
 */
template<typename T>
ତାର୍କିକ NotchFilter<T>::setParameters(ଦଶମିକ sample_freq, ଦଶମିକ notch_freq, ଦଶମିକ bandwidth)
{
	ଯଦି ((sample_freq <= 0.f) || (notch_freq <= 0.f) || (bandwidth <= 0.f) || (notch_freq >= sample_freq / 2)
	    || !isFinite(sample_freq) || !isFinite(notch_freq) || !isFinite(bandwidth)) {

		disable();
		ଫେରନ୍ତୁ ମିଥ୍ୟା;
	}

	ଅଚଳ ଦଶମିକ freq_min = sample_freq * 0.001f;

	ଅଚଳ ଦଶମିକ notch_freq_new = math::max(notch_freq, freq_min);
	ଅଚଳ ଦଶମିକ bandwidth_new = math::max(bandwidth, freq_min);

	ଅଚଳ ଦଶମିକ sample_freq_diff = fabsf(sample_freq - _sample_freq);
	ଅଚଳ ଦଶମିକ bandwidth_diff = fabsf(bandwidth_new - _bandwidth);

	ଅଚଳ ତାର୍କିକ sample_freq_change = (sample_freq_diff > FLT_EPSILON);
	ଅଚଳ ତାର୍କିକ bandwidth_change = (bandwidth_diff > FLT_EPSILON);

	ଯଦି (!sample_freq_change && !bandwidth_change) {
		ଅଚଳ ଦଶମିକ notch_freq_diff = fabsf(notch_freq_new - _notch_freq);

		ଯଦି (notch_freq_diff > FLT_EPSILON) {
			// only notch frequency has changed
			_notch_freq = notch_freq_new;

			ଅଚଳ ଦଶମିକ beta = -cosf(2.f * M_PI_F * _notch_freq / _sample_freq);

			_b1 = 2.f * beta * _b0;
			_a1 = _b1;

			ଯଦି (notch_freq_diff > _bandwidth) {
				// force reset
				_initialized = ମିଥ୍ୟା;
			}

			ଯଦି (!isFinite(_b1)) {
				disable();
				ଫେରନ୍ତୁ ମିଥ୍ୟା;
			}

			ଫେରନ୍ତୁ ସତ୍ୟ;

		} ଅନ୍ୟଥା {
			// no change, do nothing
			ଫେରନ୍ତୁ ସତ୍ୟ;
		}
	}

	_sample_freq = sample_freq;
	_notch_freq = notch_freq_new;
	_bandwidth = bandwidth_new;

	ଅଚଳ ଦଶମିକ alpha = tanf(M_PI_F * _bandwidth / _sample_freq);
	ଅଚଳ ଦଶମିକ beta = -cosf(2.f * M_PI_F * _notch_freq / _sample_freq);
	ଅଚଳ ଦଶମିକ a0_inv = 1.f / (alpha + 1.f);

	_b0 = a0_inv;
	_b1 = 2.f * beta * a0_inv;
	_b2 = a0_inv;

	_a1 = _b1;
	_a2 = (1.f - alpha) * a0_inv;

	ଯଦି (!isFinite(_b0) || !isFinite(_b1) || !isFinite(_b2) || !isFinite(_a2)) {
		disable();
		ଫେରନ୍ତୁ ମିଥ୍ୟା;
	}

	// force reset if bandwidth or sample frequency changed by more than 1%
	ଯଦି (bandwidth_diff > 0.01f * _bandwidth) {
		_initialized = ମିଥ୍ୟା;

	} ଅନ୍ୟଥା ଯଦି (sample_freq_diff > 0.01f * _sample_freq) {
		_initialized = ମିଥ୍ୟା;
	}

	ଫେରନ୍ତୁ ସତ୍ୟ;
}

} // namespace math
