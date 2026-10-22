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

#અન્તર્ભાવ <mathlib/math/Functions.hpp>
#અન્તર્ભાવ <cmath>
#અન્તર્ભાવ <float.h>
#અન્તર્ભાવ <matrix/math.hpp>

નામાકાશ math
{

template<typename T>
વર્ગ NotchFilter
{
જાહેર:
	NotchFilter() = મૂળભૂત;
	~NotchFilter() = મૂળભૂત;

	તાર્કિક setParameters(દશાંશ sample_freq, દશાંશ notch_freq, દશાંશ bandwidth);

	/**
	 * Add a new raw value to the filter using the Direct Form I
	 *
	 * @return retrieve the filtered result
	 */
	અંતઃરેખ T apply(અચલ T &sample)
	{
		જો (!_initialized) {
			reset(sample);
			_initialized = સાચું;
		}

		પાછા applyInternal(sample);
	}

	// Filter array of samples in place using the direct form I
	અંતઃરેખ શૂન્ય applyArray(T samples[], પૂર્ણાંક અંક_samples)
	{
		જો (!_initialized) {
			reset(samples[0]);
			_initialized = સાચું;
		}

		માટે (પૂર્ણાંક n = 0; n < અંક_samples; n++) {
			samples[n] = applyInternal(samples[n]);
		}
	}

	દશાંશ getNotchFreq() અચલ { પાછા _notch_freq; }
	દશાંશ getBandwidth() અચલ { પાછા _bandwidth; }

	// Used in unit test only
	શૂન્ય getCoefficients(દશાંશ a[3], દશાંશ b[3]) અચલ
	{
		a[0] = 1.f;
		a[1] = _a1;
		a[2] = _a2;
		b[0] = _b0;
		b[1] = _b1;
		b[2] = _b2;
	}

	દશાંશ getMagnitudeResponse(દશાંશ frequency) અચલ
	{
		દશાંશ w = 2.f * M_PI_F * frequency / _sample_freq;

		દશાંશ numerator = _b0 * _b0 + _b1 * _b1 + _b2 * _b2
				  + 2.f * (_b0 * _b1 + _b1 * _b2) * cosf(w) + 2.f * _b0 * _b2 * cosf(2.f * w);

		દશાંશ denominator = 1.f + _a1 * _a1 + _a2 * _a2 + 2.f * (_a1 + _a1 * _a2) * cosf(w) + 2.f * _a2 * cosf(2.f * w);

		પાછા sqrtf(numerator / denominator);
	}

	/**
	 * Bypasses the filter update to directly set different filter coefficients.
	 * Note: the filtered frequency and quality factor saved on the filter lose their
	 * physical meaning if you use this method to change the filter's coefficients.
	 * Used for creating clones of a specific filter.
	 */
	શૂન્ય setCoefficients(દશાંશ a[2], દશાંશ b[3])
	{
		_a1 = a[0];
		_a2 = a[1];
		_b0 = b[0];
		_b1 = b[1];
		_b2 = b[2];
	}

	તાર્કિક initialized() અચલ { પાછા _initialized; }

	શૂન્ય reset() { _initialized = ખોટું; }

	શૂન્ય reset(અચલ T &sample)
	{
		અચલ T આગત = isFinite(sample) ? sample : T{};

		_delay_element_1 = _delay_element_2 = આગત;
		_delay_element_output_1 = _delay_element_output_2 = આગત * (_b0 + _b1 + _b2) / (1 + _a1 + _a2);

		જો (!isFinite(_delay_element_1) || !isFinite(_delay_element_2)) {
			_delay_element_output_1 = _delay_element_output_2 = {};
		}

		_initialized = સાચું;
	}

	શૂન્ય disable()
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

		_initialized = ખોટું;
	}

સુરક્ષિત:

	/**
	 * Add a new raw value to the filter using the Direct Form I
	 *
	 * @return retrieve the filtered result
	 */
	અંતઃરેખ T applyInternal(અચલ T &sample)
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

		પાછા output;
	}

	T _delay_element_1{};
	T _delay_element_2{};
	T _delay_element_output_1{};
	T _delay_element_output_2{};

	// All the coefficients are normalized by a0, so a0 becomes 1 here
	દશાંશ _a1{0.f};
	દશાંશ _a2{0.f};

	દશાંશ _b0{1.f};
	દશાંશ _b1{0.f};
	દશાંશ _b2{0.f};

	દશાંશ _notch_freq{};
	દશાંશ _bandwidth{};
	દશાંશ _sample_freq{};

	તાર્કિક _initialized{ખોટું};
};

/**
 * Initialises the filter by setting its parameters and coefficients.
 * Using the direct form I method, allows to dynamically
 * update the filtered frequency, refresh rate and quality factor while
 * conserving the filter's history
 */
template<typename T>
તાર્કિક NotchFilter<T>::setParameters(દશાંશ sample_freq, દશાંશ notch_freq, દશાંશ bandwidth)
{
	જો ((sample_freq <= 0.f) || (notch_freq <= 0.f) || (bandwidth <= 0.f) || (notch_freq >= sample_freq / 2)
	    || !isFinite(sample_freq) || !isFinite(notch_freq) || !isFinite(bandwidth)) {

		disable();
		પાછા ખોટું;
	}

	અચલ દશાંશ freq_min = sample_freq * 0.001f;

	અચલ દશાંશ notch_freq_new = math::max(notch_freq, freq_min);
	અચલ દશાંશ bandwidth_new = math::max(bandwidth, freq_min);

	અચલ દશાંશ sample_freq_diff = fabsf(sample_freq - _sample_freq);
	અચલ દશાંશ bandwidth_diff = fabsf(bandwidth_new - _bandwidth);

	અચલ તાર્કિક sample_freq_change = (sample_freq_diff > FLT_EPSILON);
	અચલ તાર્કિક bandwidth_change = (bandwidth_diff > FLT_EPSILON);

	જો (!sample_freq_change && !bandwidth_change) {
		અચલ દશાંશ notch_freq_diff = fabsf(notch_freq_new - _notch_freq);

		જો (notch_freq_diff > FLT_EPSILON) {
			// only notch frequency has changed
			_notch_freq = notch_freq_new;

			અચલ દશાંશ beta = -cosf(2.f * M_PI_F * _notch_freq / _sample_freq);

			_b1 = 2.f * beta * _b0;
			_a1 = _b1;

			જો (notch_freq_diff > _bandwidth) {
				// force reset
				_initialized = ખોટું;
			}

			જો (!isFinite(_b1)) {
				disable();
				પાછા ખોટું;
			}

			પાછા સાચું;

		} નહીંતર {
			// no change, do nothing
			પાછા સાચું;
		}
	}

	_sample_freq = sample_freq;
	_notch_freq = notch_freq_new;
	_bandwidth = bandwidth_new;

	અચલ દશાંશ alpha = tanf(M_PI_F * _bandwidth / _sample_freq);
	અચલ દશાંશ beta = -cosf(2.f * M_PI_F * _notch_freq / _sample_freq);
	અચલ દશાંશ a0_inv = 1.f / (alpha + 1.f);

	_b0 = a0_inv;
	_b1 = 2.f * beta * a0_inv;
	_b2 = a0_inv;

	_a1 = _b1;
	_a2 = (1.f - alpha) * a0_inv;

	જો (!isFinite(_b0) || !isFinite(_b1) || !isFinite(_b2) || !isFinite(_a2)) {
		disable();
		પાછા ખોટું;
	}

	// force reset if bandwidth or sample frequency changed by more than 1%
	જો (bandwidth_diff > 0.01f * _bandwidth) {
		_initialized = ખોટું;

	} નહીંતર જો (sample_freq_diff > 0.01f * _sample_freq) {
		_initialized = ખોટું;
	}

	પાછા સાચું;
}

} // namespace math
