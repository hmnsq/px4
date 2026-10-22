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

#அந்தர்பாவ <mathlib/math/Functions.hpp>
#அந்தர்பாவ <cmath>
#அந்தர்பாவ <float.h>
#அந்தர்பாவ <matrix/math.hpp>

நாமாகாஶ math
{

template<typename T>
வகுப்பு NotchFilter
{
பொது:
	NotchFilter() = இயல்பு;
	~NotchFilter() = இயல்பு;

	தருக்க setParameters(தசம sample_freq, தசம notch_freq, தசம bandwidth);

	/**
	 * Add a new raw value to the filter using the Direct Form I
	 *
	 * @return retrieve the filtered result
	 */
	உள்வரி T apply(மாறிலி T &sample)
	{
		எனில் (!_initialized) {
			reset(sample);
			_initialized = உண்மை;
		}

		திரும்பு applyInternal(sample);
	}

	// Filter array of samples in place using the direct form I
	உள்வரி வெறுமை applyArray(T samples[], முழு இலக்கம்_samples)
	{
		எனில் (!_initialized) {
			reset(samples[0]);
			_initialized = உண்மை;
		}

		க்கு (முழு n = 0; n < இலக்கம்_samples; n++) {
			samples[n] = applyInternal(samples[n]);
		}
	}

	தசம getNotchFreq() மாறிலி { திரும்பு _notch_freq; }
	தசம getBandwidth() மாறிலி { திரும்பு _bandwidth; }

	// Used in unit test only
	வெறுமை getCoefficients(தசம a[3], தசம b[3]) மாறிலி
	{
		a[0] = 1.f;
		a[1] = _a1;
		a[2] = _a2;
		b[0] = _b0;
		b[1] = _b1;
		b[2] = _b2;
	}

	தசம getMagnitudeResponse(தசம frequency) மாறிலி
	{
		தசம w = 2.f * M_PI_F * frequency / _sample_freq;

		தசம numerator = _b0 * _b0 + _b1 * _b1 + _b2 * _b2
				  + 2.f * (_b0 * _b1 + _b1 * _b2) * cosf(w) + 2.f * _b0 * _b2 * cosf(2.f * w);

		தசம denominator = 1.f + _a1 * _a1 + _a2 * _a2 + 2.f * (_a1 + _a1 * _a2) * cosf(w) + 2.f * _a2 * cosf(2.f * w);

		திரும்பு sqrtf(numerator / denominator);
	}

	/**
	 * Bypasses the filter update to directly set different filter coefficients.
	 * Note: the filtered frequency and quality factor saved on the filter lose their
	 * physical meaning if you use this method to change the filter's coefficients.
	 * Used for creating clones of a specific filter.
	 */
	வெறுமை setCoefficients(தசம a[2], தசம b[3])
	{
		_a1 = a[0];
		_a2 = a[1];
		_b0 = b[0];
		_b1 = b[1];
		_b2 = b[2];
	}

	தருக்க initialized() மாறிலி { திரும்பு _initialized; }

	வெறுமை reset() { _initialized = பொய்; }

	வெறுமை reset(மாறிலி T &sample)
	{
		மாறிலி T உள்ளீடு = isFinite(sample) ? sample : T{};

		_delay_element_1 = _delay_element_2 = உள்ளீடு;
		_delay_element_output_1 = _delay_element_output_2 = உள்ளீடு * (_b0 + _b1 + _b2) / (1 + _a1 + _a2);

		எனில் (!isFinite(_delay_element_1) || !isFinite(_delay_element_2)) {
			_delay_element_output_1 = _delay_element_output_2 = {};
		}

		_initialized = உண்மை;
	}

	வெறுமை disable()
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

		_initialized = பொய்;
	}

பாதுகாக்கப்பட்ட:

	/**
	 * Add a new raw value to the filter using the Direct Form I
	 *
	 * @return retrieve the filtered result
	 */
	உள்வரி T applyInternal(மாறிலி T &sample)
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

		திரும்பு output;
	}

	T _delay_element_1{};
	T _delay_element_2{};
	T _delay_element_output_1{};
	T _delay_element_output_2{};

	// All the coefficients are normalized by a0, so a0 becomes 1 here
	தசம _a1{0.f};
	தசம _a2{0.f};

	தசம _b0{1.f};
	தசம _b1{0.f};
	தசம _b2{0.f};

	தசம _notch_freq{};
	தசம _bandwidth{};
	தசம _sample_freq{};

	தருக்க _initialized{பொய்};
};

/**
 * Initialises the filter by setting its parameters and coefficients.
 * Using the direct form I method, allows to dynamically
 * update the filtered frequency, refresh rate and quality factor while
 * conserving the filter's history
 */
template<typename T>
தருக்க NotchFilter<T>::setParameters(தசம sample_freq, தசம notch_freq, தசம bandwidth)
{
	எனில் ((sample_freq <= 0.f) || (notch_freq <= 0.f) || (bandwidth <= 0.f) || (notch_freq >= sample_freq / 2)
	    || !isFinite(sample_freq) || !isFinite(notch_freq) || !isFinite(bandwidth)) {

		disable();
		திரும்பு பொய்;
	}

	மாறிலி தசம freq_min = sample_freq * 0.001f;

	மாறிலி தசம notch_freq_new = math::max(notch_freq, freq_min);
	மாறிலி தசம bandwidth_new = math::max(bandwidth, freq_min);

	மாறிலி தசம sample_freq_diff = fabsf(sample_freq - _sample_freq);
	மாறிலி தசம bandwidth_diff = fabsf(bandwidth_new - _bandwidth);

	மாறிலி தருக்க sample_freq_change = (sample_freq_diff > FLT_EPSILON);
	மாறிலி தருக்க bandwidth_change = (bandwidth_diff > FLT_EPSILON);

	எனில் (!sample_freq_change && !bandwidth_change) {
		மாறிலி தசம notch_freq_diff = fabsf(notch_freq_new - _notch_freq);

		எனில் (notch_freq_diff > FLT_EPSILON) {
			// only notch frequency has changed
			_notch_freq = notch_freq_new;

			மாறிலி தசம beta = -cosf(2.f * M_PI_F * _notch_freq / _sample_freq);

			_b1 = 2.f * beta * _b0;
			_a1 = _b1;

			எனில் (notch_freq_diff > _bandwidth) {
				// force reset
				_initialized = பொய்;
			}

			எனில் (!isFinite(_b1)) {
				disable();
				திரும்பு பொய்;
			}

			திரும்பு உண்மை;

		} இல்லையெனில் {
			// no change, do nothing
			திரும்பு உண்மை;
		}
	}

	_sample_freq = sample_freq;
	_notch_freq = notch_freq_new;
	_bandwidth = bandwidth_new;

	மாறிலி தசம alpha = tanf(M_PI_F * _bandwidth / _sample_freq);
	மாறிலி தசம beta = -cosf(2.f * M_PI_F * _notch_freq / _sample_freq);
	மாறிலி தசம a0_inv = 1.f / (alpha + 1.f);

	_b0 = a0_inv;
	_b1 = 2.f * beta * a0_inv;
	_b2 = a0_inv;

	_a1 = _b1;
	_a2 = (1.f - alpha) * a0_inv;

	எனில் (!isFinite(_b0) || !isFinite(_b1) || !isFinite(_b2) || !isFinite(_a2)) {
		disable();
		திரும்பு பொய்;
	}

	// force reset if bandwidth or sample frequency changed by more than 1%
	எனில் (bandwidth_diff > 0.01f * _bandwidth) {
		_initialized = பொய்;

	} இல்லையெனில் எனில் (sample_freq_diff > 0.01f * _sample_freq) {
		_initialized = பொய்;
	}

	திரும்பு உண்மை;
}

} // namespace math
