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

#निर्देशः once

#अन्तर्भाव <mathlib/math/Functions.hpp>
#अन्तर्भाव <cmath>
#अन्तर्भाव <float.h>
#अन्तर्भाव <matrix/math.hpp>

नामाकाश math
{

प्रतिरूपम्<प्रकारनाम T>
वर्ग NotchFilter
{
सार्वजनिक:
	NotchFilter() = स्वभाव;
	~NotchFilter() = स्वभाव;

	तार्किक समुच्चयः_Parameters(भिन्न sample_freq, भिन्न notch_freq, भिन्न bandwidth);

	/**
	 * Add a new raw value to the filter using the Direct Form I
	 *
	 * @return retrieve the filtered result
	 */
	अन्तःस्थ T apply(ध्रुव T &sample)
	{
		यदि (!_initialized) {
			reset(sample);
			_initialized = सत्य;
		}

		प्रत्यागम apply_आन्तरिकम्(sample);
	}

	// Filter array of samples in place using the direct form I
	अन्तःस्थ शून्य applyArray(T samples[], पूर्णाङ्क अङ्कः_samples)
	{
		यदि (!_initialized) {
			reset(samples[0]);
			_initialized = सत्य;
		}

		प्रति (पूर्णाङ्क n = 0; n < अङ्कः_samples; n++) {
			samples[n] = apply_आन्तरिकम्(samples[n]);
		}
	}

	भिन्न getNotchFreq() ध्रुव { प्रत्यागम _notch_freq; }
	भिन्न getBandwidth() ध्रुव { प्रत्यागम _bandwidth; }

	// Used in unit test only
	शून्य getCoefficients(भिन्न a[3], भिन्न b[3]) ध्रुव
	{
		a[0] = 1.f;
		a[1] = _a1;
		a[2] = _a2;
		b[0] = _b0;
		b[1] = _b1;
		b[2] = _b2;
	}

	भिन्न getMagnitudeResponse(भिन्न frequency) ध्रुव
	{
		भिन्न w = 2.f * M_PI_F * frequency / _sample_freq;

		भिन्न numerator = _b0 * _b0 + _b1 * _b1 + _b2 * _b2
				  + 2.f * (_b0 * _b1 + _b1 * _b2) * cosf(w) + 2.f * _b0 * _b2 * cosf(2.f * w);

		भिन्न denominator = 1.f + _a1 * _a1 + _a2 * _a2 + 2.f * (_a1 + _a1 * _a2) * cosf(w) + 2.f * _a2 * cosf(2.f * w);

		प्रत्यागम sqrtf(numerator / denominator);
	}

	/**
	 * Bypasses the filter update to directly set different filter coefficients.
	 * Note: the filtered frequency and quality factor saved on the filter lose their
	 * physical meaning if you use this method to change the filter's coefficients.
	 * Used for creating clones of a specific filter.
	 */
	शून्य समुच्चयः_Coefficients(भिन्न a[2], भिन्न b[3])
	{
		_a1 = a[0];
		_a2 = a[1];
		_b0 = b[0];
		_b1 = b[1];
		_b2 = b[2];
	}

	तार्किक initialized() ध्रुव { प्रत्यागम _initialized; }

	शून्य reset() { _initialized = असत्य; }

	शून्य reset(ध्रुव T &sample)
	{
		ध्रुव T निवेशः = isFinite(sample) ? sample : T{};

		_delay_element_1 = _delay_element_2 = निवेशः;
		_delay_element_output_1 = _delay_element_output_2 = निवेशः * (_b0 + _b1 + _b2) / (1 + _a1 + _a2);

		यदि (!isFinite(_delay_element_1) || !isFinite(_delay_element_2)) {
			_delay_element_output_1 = _delay_element_output_2 = {};
		}

		_initialized = सत्य;
	}

	शून्य disable()
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

		_initialized = असत्य;
	}

रक्षित:

	/**
	 * Add a new raw value to the filter using the Direct Form I
	 *
	 * @return retrieve the filtered result
	 */
	अन्तःस्थ T apply_आन्तरिकम्(ध्रुव T &sample)
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

		प्रत्यागम output;
	}

	T _delay_element_1{};
	T _delay_element_2{};
	T _delay_element_output_1{};
	T _delay_element_output_2{};

	// All the coefficients are normalized by a0, so a0 becomes 1 here
	भिन्न _a1{0.f};
	भिन्न _a2{0.f};

	भिन्न _b0{1.f};
	भिन्न _b1{0.f};
	भिन्न _b2{0.f};

	भिन्न _notch_freq{};
	भिन्न _bandwidth{};
	भिन्न _sample_freq{};

	तार्किक _initialized{असत्य};
};

/**
 * Initialises the filter by setting its parameters and coefficients.
 * Using the direct form I method, allows to dynamically
 * update the filtered frequency, refresh rate and quality factor while
 * conserving the filter's history
 */
प्रतिरूपम्<प्रकारनाम T>
तार्किक NotchFilter<T>::समुच्चयः_Parameters(भिन्न sample_freq, भिन्न notch_freq, भिन्न bandwidth)
{
	यदि ((sample_freq <= 0.f) || (notch_freq <= 0.f) || (bandwidth <= 0.f) || (notch_freq >= sample_freq / 2)
	    || !isFinite(sample_freq) || !isFinite(notch_freq) || !isFinite(bandwidth)) {

		disable();
		प्रत्यागम असत्य;
	}

	ध्रुव भिन्न freq_min = sample_freq * 0.001f;

	ध्रुव भिन्न notch_freq_new = math::max(notch_freq, freq_min);
	ध्रुव भिन्न bandwidth_new = math::max(bandwidth, freq_min);

	ध्रुव भिन्न sample_freq_diff = fabsf(sample_freq - _sample_freq);
	ध्रुव भिन्न bandwidth_diff = fabsf(bandwidth_new - _bandwidth);

	ध्रुव तार्किक sample_freq_change = (sample_freq_diff > FLT_EPSILON);
	ध्रुव तार्किक bandwidth_change = (bandwidth_diff > FLT_EPSILON);

	यदि (!sample_freq_change && !bandwidth_change) {
		ध्रुव भिन्न notch_freq_diff = fabsf(notch_freq_new - _notch_freq);

		यदि (notch_freq_diff > FLT_EPSILON) {
			// only notch frequency has changed
			_notch_freq = notch_freq_new;

			ध्रुव भिन्न beta = -cosf(2.f * M_PI_F * _notch_freq / _sample_freq);

			_b1 = 2.f * beta * _b0;
			_a1 = _b1;

			यदि (notch_freq_diff > _bandwidth) {
				// force reset
				_initialized = असत्य;
			}

			यदि (!isFinite(_b1)) {
				disable();
				प्रत्यागम असत्य;
			}

			प्रत्यागम सत्य;

		} अन्यथा {
			// no change, do nothing
			प्रत्यागम सत्य;
		}
	}

	_sample_freq = sample_freq;
	_notch_freq = notch_freq_new;
	_bandwidth = bandwidth_new;

	ध्रुव भिन्न alpha = tanf(M_PI_F * _bandwidth / _sample_freq);
	ध्रुव भिन्न beta = -cosf(2.f * M_PI_F * _notch_freq / _sample_freq);
	ध्रुव भिन्न a0_inv = 1.f / (alpha + 1.f);

	_b0 = a0_inv;
	_b1 = 2.f * beta * a0_inv;
	_b2 = a0_inv;

	_a1 = _b1;
	_a2 = (1.f - alpha) * a0_inv;

	यदि (!isFinite(_b0) || !isFinite(_b1) || !isFinite(_b2) || !isFinite(_a2)) {
		disable();
		प्रत्यागम असत्य;
	}

	// force reset if bandwidth or sample frequency changed by more than 1%
	यदि (bandwidth_diff > 0.01f * _bandwidth) {
		_initialized = असत्य;

	} अन्यथा यदि (sample_freq_diff > 0.01f * _sample_freq) {
		_initialized = असत्य;
	}

	प्रत्यागम सत्य;
}

} // namespace math
