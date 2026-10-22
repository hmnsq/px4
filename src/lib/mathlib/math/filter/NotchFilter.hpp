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

#അന്തര്ഭാവ <mathlib/math/Functions.hpp>
#അന്തര്ഭാവ <cmath>
#അന്തര്ഭാവ <float.h>
#അന്തര്ഭാവ <matrix/math.hpp>

നാമാകാശ math
{

template<typename T>
വർഗ്ഗം NotchFilter
{
പൊതു:
	NotchFilter() = സ്ഥിരസ്ഥിതി;
	~NotchFilter() = സ്ഥിരസ്ഥിതി;

	തർക്കം setParameters(ദശാംശം sample_freq, ദശാംശം notch_freq, ദശാംശം bandwidth);

	/**
	 * Add a new raw value to the filter using the Direct Form I
	 *
	 * @return retrieve the filtered result
	 */
	വരിയിൽ T apply(സ്ഥിരം T &sample)
	{
		എങ്കിൽ (!_initialized) {
			reset(sample);
			_initialized = സത്യം;
		}

		തിരികെ applyInternal(sample);
	}

	// Filter array of samples in place using the direct form I
	വരിയിൽ ശൂന്യം applyArray(T samples[], പൂർണ്ണസംഖ്യ അക്കം_samples)
	{
		എങ്കിൽ (!_initialized) {
			reset(samples[0]);
			_initialized = സത്യം;
		}

		വേണ്ടി (പൂർണ്ണസംഖ്യ n = 0; n < അക്കം_samples; n++) {
			samples[n] = applyInternal(samples[n]);
		}
	}

	ദശാംശം getNotchFreq() സ്ഥിരം { തിരികെ _notch_freq; }
	ദശാംശം getBandwidth() സ്ഥിരം { തിരികെ _bandwidth; }

	// Used in unit test only
	ശൂന്യം getCoefficients(ദശാംശം a[3], ദശാംശം b[3]) സ്ഥിരം
	{
		a[0] = 1.f;
		a[1] = _a1;
		a[2] = _a2;
		b[0] = _b0;
		b[1] = _b1;
		b[2] = _b2;
	}

	ദശാംശം getMagnitudeResponse(ദശാംശം frequency) സ്ഥിരം
	{
		ദശാംശം w = 2.f * M_PI_F * frequency / _sample_freq;

		ദശാംശം numerator = _b0 * _b0 + _b1 * _b1 + _b2 * _b2
				  + 2.f * (_b0 * _b1 + _b1 * _b2) * cosf(w) + 2.f * _b0 * _b2 * cosf(2.f * w);

		ദശാംശം denominator = 1.f + _a1 * _a1 + _a2 * _a2 + 2.f * (_a1 + _a1 * _a2) * cosf(w) + 2.f * _a2 * cosf(2.f * w);

		തിരികെ sqrtf(numerator / denominator);
	}

	/**
	 * Bypasses the filter update to directly set different filter coefficients.
	 * Note: the filtered frequency and quality factor saved on the filter lose their
	 * physical meaning if you use this method to change the filter's coefficients.
	 * Used for creating clones of a specific filter.
	 */
	ശൂന്യം setCoefficients(ദശാംശം a[2], ദശാംശം b[3])
	{
		_a1 = a[0];
		_a2 = a[1];
		_b0 = b[0];
		_b1 = b[1];
		_b2 = b[2];
	}

	തർക്കം initialized() സ്ഥിരം { തിരികെ _initialized; }

	ശൂന്യം reset() { _initialized = അസത്യം; }

	ശൂന്യം reset(സ്ഥിരം T &sample)
	{
		സ്ഥിരം T നിവേശം = isFinite(sample) ? sample : T{};

		_delay_element_1 = _delay_element_2 = നിവേശം;
		_delay_element_output_1 = _delay_element_output_2 = നിവേശം * (_b0 + _b1 + _b2) / (1 + _a1 + _a2);

		എങ്കിൽ (!isFinite(_delay_element_1) || !isFinite(_delay_element_2)) {
			_delay_element_output_1 = _delay_element_output_2 = {};
		}

		_initialized = സത്യം;
	}

	ശൂന്യം disable()
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

		_initialized = അസത്യം;
	}

സംരക്ഷിത:

	/**
	 * Add a new raw value to the filter using the Direct Form I
	 *
	 * @return retrieve the filtered result
	 */
	വരിയിൽ T applyInternal(സ്ഥിരം T &sample)
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

		തിരികെ output;
	}

	T _delay_element_1{};
	T _delay_element_2{};
	T _delay_element_output_1{};
	T _delay_element_output_2{};

	// All the coefficients are normalized by a0, so a0 becomes 1 here
	ദശാംശം _a1{0.f};
	ദശാംശം _a2{0.f};

	ദശാംശം _b0{1.f};
	ദശാംശം _b1{0.f};
	ദശാംശം _b2{0.f};

	ദശാംശം _notch_freq{};
	ദശാംശം _bandwidth{};
	ദശാംശം _sample_freq{};

	തർക്കം _initialized{അസത്യം};
};

/**
 * Initialises the filter by setting its parameters and coefficients.
 * Using the direct form I method, allows to dynamically
 * update the filtered frequency, refresh rate and quality factor while
 * conserving the filter's history
 */
template<typename T>
തർക്കം NotchFilter<T>::setParameters(ദശാംശം sample_freq, ദശാംശം notch_freq, ദശാംശം bandwidth)
{
	എങ്കിൽ ((sample_freq <= 0.f) || (notch_freq <= 0.f) || (bandwidth <= 0.f) || (notch_freq >= sample_freq / 2)
	    || !isFinite(sample_freq) || !isFinite(notch_freq) || !isFinite(bandwidth)) {

		disable();
		തിരികെ അസത്യം;
	}

	സ്ഥിരം ദശാംശം freq_min = sample_freq * 0.001f;

	സ്ഥിരം ദശാംശം notch_freq_new = math::max(notch_freq, freq_min);
	സ്ഥിരം ദശാംശം bandwidth_new = math::max(bandwidth, freq_min);

	സ്ഥിരം ദശാംശം sample_freq_diff = fabsf(sample_freq - _sample_freq);
	സ്ഥിരം ദശാംശം bandwidth_diff = fabsf(bandwidth_new - _bandwidth);

	സ്ഥിരം തർക്കം sample_freq_change = (sample_freq_diff > FLT_EPSILON);
	സ്ഥിരം തർക്കം bandwidth_change = (bandwidth_diff > FLT_EPSILON);

	എങ്കിൽ (!sample_freq_change && !bandwidth_change) {
		സ്ഥിരം ദശാംശം notch_freq_diff = fabsf(notch_freq_new - _notch_freq);

		എങ്കിൽ (notch_freq_diff > FLT_EPSILON) {
			// only notch frequency has changed
			_notch_freq = notch_freq_new;

			സ്ഥിരം ദശാംശം beta = -cosf(2.f * M_PI_F * _notch_freq / _sample_freq);

			_b1 = 2.f * beta * _b0;
			_a1 = _b1;

			എങ്കിൽ (notch_freq_diff > _bandwidth) {
				// force reset
				_initialized = അസത്യം;
			}

			എങ്കിൽ (!isFinite(_b1)) {
				disable();
				തിരികെ അസത്യം;
			}

			തിരികെ സത്യം;

		} അല്ലെങ്കിൽ {
			// no change, do nothing
			തിരികെ സത്യം;
		}
	}

	_sample_freq = sample_freq;
	_notch_freq = notch_freq_new;
	_bandwidth = bandwidth_new;

	സ്ഥിരം ദശാംശം alpha = tanf(M_PI_F * _bandwidth / _sample_freq);
	സ്ഥിരം ദശാംശം beta = -cosf(2.f * M_PI_F * _notch_freq / _sample_freq);
	സ്ഥിരം ദശാംശം a0_inv = 1.f / (alpha + 1.f);

	_b0 = a0_inv;
	_b1 = 2.f * beta * a0_inv;
	_b2 = a0_inv;

	_a1 = _b1;
	_a2 = (1.f - alpha) * a0_inv;

	എങ്കിൽ (!isFinite(_b0) || !isFinite(_b1) || !isFinite(_b2) || !isFinite(_a2)) {
		disable();
		തിരികെ അസത്യം;
	}

	// force reset if bandwidth or sample frequency changed by more than 1%
	എങ്കിൽ (bandwidth_diff > 0.01f * _bandwidth) {
		_initialized = അസത്യം;

	} അല്ലെങ്കിൽ എങ്കിൽ (sample_freq_diff > 0.01f * _sample_freq) {
		_initialized = അസത്യം;
	}

	തിരികെ സത്യം;
}

} // namespace math
