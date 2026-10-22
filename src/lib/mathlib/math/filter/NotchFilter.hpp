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

#अन्तर्भाव <mathlib/math/Functions.hpp>
#अन्तर्भाव <cmath>
#अन्तर्भाव <float.h>
#अन्तर्भाव <matrix/math.hpp>

नामाकाश math
{

ढाँचा<typename T>
वर्ग NotchFilter
{
सार्वजनिक:
	NotchFilter() = पूर्वनिर्धारित;
	~NotchFilter() = पूर्वनिर्धारित;

	तार्किक setParameters(दशांश sample_freq, दशांश notch_freq, दशांश bandwidth);

	/**
	 * Add a new raw value to the filter using the Direct Form I
	 *
	 * @return retrieve the filtered result
	 */
	ओळीत T apply(स्थिरांक T &sample)
	{
		जर (!_initialized) {
			reset(sample);
			_initialized = सत्य;
		}

		परत applyInternal(sample);
	}

	// Filter array of samples in place using the direct form I
	ओळीत शून्य applyArray(T samples[], पूर्णांक अंक_samples)
	{
		जर (!_initialized) {
			reset(samples[0]);
			_initialized = सत्य;
		}

		साठी (पूर्णांक n = 0; n < अंक_samples; n++) {
			samples[n] = applyInternal(samples[n]);
		}
	}

	दशांश getNotchFreq() स्थिरांक { परत _notch_freq; }
	दशांश getBandwidth() स्थिरांक { परत _bandwidth; }

	// Used in unit test only
	शून्य getCoefficients(दशांश a[3], दशांश b[3]) स्थिरांक
	{
		a[0] = 1.f;
		a[1] = _a1;
		a[2] = _a2;
		b[0] = _b0;
		b[1] = _b1;
		b[2] = _b2;
	}

	दशांश getMagnitudeResponse(दशांश frequency) स्थिरांक
	{
		दशांश w = 2.f * M_PI_F * frequency / _sample_freq;

		दशांश numerator = _b0 * _b0 + _b1 * _b1 + _b2 * _b2
				  + 2.f * (_b0 * _b1 + _b1 * _b2) * cosf(w) + 2.f * _b0 * _b2 * cosf(2.f * w);

		दशांश denominator = 1.f + _a1 * _a1 + _a2 * _a2 + 2.f * (_a1 + _a1 * _a2) * cosf(w) + 2.f * _a2 * cosf(2.f * w);

		परत sqrtf(numerator / denominator);
	}

	/**
	 * Bypasses the filter update to directly set different filter coefficients.
	 * Note: the filtered frequency and quality factor saved on the filter lose their
	 * physical meaning if you use this method to change the filter's coefficients.
	 * Used for creating clones of a specific filter.
	 */
	शून्य setCoefficients(दशांश a[2], दशांश b[3])
	{
		_a1 = a[0];
		_a2 = a[1];
		_b0 = b[0];
		_b1 = b[1];
		_b2 = b[2];
	}

	तार्किक initialized() स्थिरांक { परत _initialized; }

	शून्य reset() { _initialized = असत्य; }

	शून्य reset(स्थिरांक T &sample)
	{
		स्थिरांक T आदान = isFinite(sample) ? sample : T{};

		_delay_element_1 = _delay_element_2 = आदान;
		_delay_element_output_1 = _delay_element_output_2 = आदान * (_b0 + _b1 + _b2) / (1 + _a1 + _a2);

		जर (!isFinite(_delay_element_1) || !isFinite(_delay_element_2)) {
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

संरक्षित:

	/**
	 * Add a new raw value to the filter using the Direct Form I
	 *
	 * @return retrieve the filtered result
	 */
	ओळीत T applyInternal(स्थिरांक T &sample)
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

		परत output;
	}

	T _delay_element_1{};
	T _delay_element_2{};
	T _delay_element_output_1{};
	T _delay_element_output_2{};

	// All the coefficients are normalized by a0, so a0 becomes 1 here
	दशांश _a1{0.f};
	दशांश _a2{0.f};

	दशांश _b0{1.f};
	दशांश _b1{0.f};
	दशांश _b2{0.f};

	दशांश _notch_freq{};
	दशांश _bandwidth{};
	दशांश _sample_freq{};

	तार्किक _initialized{असत्य};
};

/**
 * Initialises the filter by setting its parameters and coefficients.
 * Using the direct form I method, allows to dynamically
 * update the filtered frequency, refresh rate and quality factor while
 * conserving the filter's history
 */
ढाँचा<typename T>
तार्किक NotchFilter<T>::setParameters(दशांश sample_freq, दशांश notch_freq, दशांश bandwidth)
{
	जर ((sample_freq <= 0.f) || (notch_freq <= 0.f) || (bandwidth <= 0.f) || (notch_freq >= sample_freq / 2)
	    || !isFinite(sample_freq) || !isFinite(notch_freq) || !isFinite(bandwidth)) {

		disable();
		परत असत्य;
	}

	स्थिरांक दशांश freq_min = sample_freq * 0.001f;

	स्थिरांक दशांश notch_freq_new = math::max(notch_freq, freq_min);
	स्थिरांक दशांश bandwidth_new = math::max(bandwidth, freq_min);

	स्थिरांक दशांश sample_freq_diff = fabsf(sample_freq - _sample_freq);
	स्थिरांक दशांश bandwidth_diff = fabsf(bandwidth_new - _bandwidth);

	स्थिरांक तार्किक sample_freq_change = (sample_freq_diff > भग्न_अंतर);
	स्थिरांक तार्किक bandwidth_change = (bandwidth_diff > भग्न_अंतर);

	जर (!sample_freq_change && !bandwidth_change) {
		स्थिरांक दशांश notch_freq_diff = fabsf(notch_freq_new - _notch_freq);

		जर (notch_freq_diff > भग्न_अंतर) {
			// only notch frequency has changed
			_notch_freq = notch_freq_new;

			स्थिरांक दशांश beta = -cosf(2.f * M_PI_F * _notch_freq / _sample_freq);

			_b1 = 2.f * beta * _b0;
			_a1 = _b1;

			जर (notch_freq_diff > _bandwidth) {
				// force reset
				_initialized = असत्य;
			}

			जर (!isFinite(_b1)) {
				disable();
				परत असत्य;
			}

			परत सत्य;

		} अन्यथा {
			// no change, do nothing
			परत सत्य;
		}
	}

	_sample_freq = sample_freq;
	_notch_freq = notch_freq_new;
	_bandwidth = bandwidth_new;

	स्थिरांक दशांश alpha = tanf(M_PI_F * _bandwidth / _sample_freq);
	स्थिरांक दशांश beta = -cosf(2.f * M_PI_F * _notch_freq / _sample_freq);
	स्थिरांक दशांश a0_inv = 1.f / (alpha + 1.f);

	_b0 = a0_inv;
	_b1 = 2.f * beta * a0_inv;
	_b2 = a0_inv;

	_a1 = _b1;
	_a2 = (1.f - alpha) * a0_inv;

	जर (!isFinite(_b0) || !isFinite(_b1) || !isFinite(_b2) || !isFinite(_a2)) {
		disable();
		परत असत्य;
	}

	// force reset if bandwidth or sample frequency changed by more than 1%
	जर (bandwidth_diff > 0.01f * _bandwidth) {
		_initialized = असत्य;

	} अन्यथा जर (sample_freq_diff > 0.01f * _sample_freq) {
		_initialized = असत्य;
	}

	परत सत्य;
}

} // namespace math
