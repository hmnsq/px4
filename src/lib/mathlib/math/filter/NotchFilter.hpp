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

#సూచన once

#అన్తర్భావ <mathlib/math/Functions.hpp>
#అన్తర్భావ <cmath>
#అన్తర్భావ <float.h>
#అన్తర్భావ <matrix/math.hpp>

నామాకాశ math
{

మూస<రకం_పేరు T>
తరగతి NotchFilter
{
బహిరంగ:
	NotchFilter() = అప్రమేయ;
	~NotchFilter() = అప్రమేయ;

	తార్కిక సమితి_Parameters(దశాంశం sample_freq, దశాంశం notch_freq, దశాంశం bandwidth);

	/**
	 * Add a new raw value to the filter using the Direct Form I
	 *
	 * @return retrieve the filtered result
	 */
	అంతఃవరుస T apply(స్థిరరాశి T &sample)
	{
		ఒకవేళ (!_initialized) {
			reset(sample);
			_initialized = నిజం;
		}

		తిరిగి apply_అంతర్గత(sample);
	}

	// Filter array of samples in place using the direct form I
	అంతఃవరుస శూన్య applyArray(T samples[], పూర్ణాంకం అంకె_samples)
	{
		ఒకవేళ (!_initialized) {
			reset(samples[0]);
			_initialized = నిజం;
		}

		కొరకు (పూర్ణాంకం n = 0; n < అంకె_samples; n++) {
			samples[n] = apply_అంతర్గత(samples[n]);
		}
	}

	దశాంశం getNotchFreq() స్థిరరాశి { తిరిగి _notch_freq; }
	దశాంశం getBandwidth() స్థిరరాశి { తిరిగి _bandwidth; }

	// Used in unit test only
	శూన్య getCoefficients(దశాంశం a[3], దశాంశం b[3]) స్థిరరాశి
	{
		a[0] = 1.f;
		a[1] = _a1;
		a[2] = _a2;
		b[0] = _b0;
		b[1] = _b1;
		b[2] = _b2;
	}

	దశాంశం getMagnitudeResponse(దశాంశం frequency) స్థిరరాశి
	{
		దశాంశం w = 2.f * M_PI_F * frequency / _sample_freq;

		దశాంశం numerator = _b0 * _b0 + _b1 * _b1 + _b2 * _b2
				  + 2.f * (_b0 * _b1 + _b1 * _b2) * cosf(w) + 2.f * _b0 * _b2 * cosf(2.f * w);

		దశాంశం denominator = 1.f + _a1 * _a1 + _a2 * _a2 + 2.f * (_a1 + _a1 * _a2) * cosf(w) + 2.f * _a2 * cosf(2.f * w);

		తిరిగి sqrtf(numerator / denominator);
	}

	/**
	 * Bypasses the filter update to directly set different filter coefficients.
	 * Note: the filtered frequency and quality factor saved on the filter lose their
	 * physical meaning if you use this method to change the filter's coefficients.
	 * Used for creating clones of a specific filter.
	 */
	శూన్య సమితి_Coefficients(దశాంశం a[2], దశాంశం b[3])
	{
		_a1 = a[0];
		_a2 = a[1];
		_b0 = b[0];
		_b1 = b[1];
		_b2 = b[2];
	}

	తార్కిక initialized() స్థిరరాశి { తిరిగి _initialized; }

	శూన్య reset() { _initialized = అబద్ధం; }

	శూన్య reset(స్థిరరాశి T &sample)
	{
		స్థిరరాశి T ఆదానం = isFinite(sample) ? sample : T{};

		_delay_element_1 = _delay_element_2 = ఆదానం;
		_delay_element_output_1 = _delay_element_output_2 = ఆదానం * (_b0 + _b1 + _b2) / (1 + _a1 + _a2);

		ఒకవేళ (!isFinite(_delay_element_1) || !isFinite(_delay_element_2)) {
			_delay_element_output_1 = _delay_element_output_2 = {};
		}

		_initialized = నిజం;
	}

	శూన్య disable()
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

		_initialized = అబద్ధం;
	}

రక్షిత:

	/**
	 * Add a new raw value to the filter using the Direct Form I
	 *
	 * @return retrieve the filtered result
	 */
	అంతఃవరుస T apply_అంతర్గత(స్థిరరాశి T &sample)
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

		తిరిగి output;
	}

	T _delay_element_1{};
	T _delay_element_2{};
	T _delay_element_output_1{};
	T _delay_element_output_2{};

	// All the coefficients are normalized by a0, so a0 becomes 1 here
	దశాంశం _a1{0.f};
	దశాంశం _a2{0.f};

	దశాంశం _b0{1.f};
	దశాంశం _b1{0.f};
	దశాంశం _b2{0.f};

	దశాంశం _notch_freq{};
	దశాంశం _bandwidth{};
	దశాంశం _sample_freq{};

	తార్కిక _initialized{అబద్ధం};
};

/**
 * Initialises the filter by setting its parameters and coefficients.
 * Using the direct form I method, allows to dynamically
 * update the filtered frequency, refresh rate and quality factor while
 * conserving the filter's history
 */
మూస<రకం_పేరు T>
తార్కిక NotchFilter<T>::సమితి_Parameters(దశాంశం sample_freq, దశాంశం notch_freq, దశాంశం bandwidth)
{
	ఒకవేళ ((sample_freq <= 0.f) || (notch_freq <= 0.f) || (bandwidth <= 0.f) || (notch_freq >= sample_freq / 2)
	    || !isFinite(sample_freq) || !isFinite(notch_freq) || !isFinite(bandwidth)) {

		disable();
		తిరిగి అబద్ధం;
	}

	స్థిరరాశి దశాంశం freq_min = sample_freq * 0.001f;

	స్థిరరాశి దశాంశం notch_freq_new = math::max(notch_freq, freq_min);
	స్థిరరాశి దశాంశం bandwidth_new = math::max(bandwidth, freq_min);

	స్థిరరాశి దశాంశం sample_freq_diff = fabsf(sample_freq - _sample_freq);
	స్థిరరాశి దశాంశం bandwidth_diff = fabsf(bandwidth_new - _bandwidth);

	స్థిరరాశి తార్కిక sample_freq_change = (sample_freq_diff > FLT_EPSILON);
	స్థిరరాశి తార్కిక bandwidth_change = (bandwidth_diff > FLT_EPSILON);

	ఒకవేళ (!sample_freq_change && !bandwidth_change) {
		స్థిరరాశి దశాంశం notch_freq_diff = fabsf(notch_freq_new - _notch_freq);

		ఒకవేళ (notch_freq_diff > FLT_EPSILON) {
			// only notch frequency has changed
			_notch_freq = notch_freq_new;

			స్థిరరాశి దశాంశం beta = -cosf(2.f * M_PI_F * _notch_freq / _sample_freq);

			_b1 = 2.f * beta * _b0;
			_a1 = _b1;

			ఒకవేళ (notch_freq_diff > _bandwidth) {
				// force reset
				_initialized = అబద్ధం;
			}

			ఒకవేళ (!isFinite(_b1)) {
				disable();
				తిరిగి అబద్ధం;
			}

			తిరిగి నిజం;

		} లేకపోతే {
			// no change, do nothing
			తిరిగి నిజం;
		}
	}

	_sample_freq = sample_freq;
	_notch_freq = notch_freq_new;
	_bandwidth = bandwidth_new;

	స్థిరరాశి దశాంశం alpha = tanf(M_PI_F * _bandwidth / _sample_freq);
	స్థిరరాశి దశాంశం beta = -cosf(2.f * M_PI_F * _notch_freq / _sample_freq);
	స్థిరరాశి దశాంశం a0_inv = 1.f / (alpha + 1.f);

	_b0 = a0_inv;
	_b1 = 2.f * beta * a0_inv;
	_b2 = a0_inv;

	_a1 = _b1;
	_a2 = (1.f - alpha) * a0_inv;

	ఒకవేళ (!isFinite(_b0) || !isFinite(_b1) || !isFinite(_b2) || !isFinite(_a2)) {
		disable();
		తిరిగి అబద్ధం;
	}

	// force reset if bandwidth or sample frequency changed by more than 1%
	ఒకవేళ (bandwidth_diff > 0.01f * _bandwidth) {
		_initialized = అబద్ధం;

	} లేకపోతే ఒకవేళ (sample_freq_diff > 0.01f * _sample_freq) {
		_initialized = అబద్ధం;
	}

	తిరిగి నిజం;
}

} // namespace math
