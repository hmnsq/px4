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

#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ <mathlib/math/Functions.hpp>
#ਅਨ੍ਤਰ੍ਭਾਵ <cmath>
#ਅਨ੍ਤਰ੍ਭਾਵ <float.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <matrix/math.hpp>

ਨਾਮਾਕਾਸ਼ math
{

ਸਾਂਚਾ<ਕਿਸਮ_ਨਾਂ T>
ਵਰਗ NotchFilter
{
ਜਨਤਕ:
	NotchFilter() = ਮੂਲ;
	~NotchFilter() = ਮੂਲ;

	ਤਾਰਕਿਕ ਸੈੱਟ_Parameters(ਦਸ਼ਮਲਵ sample_freq, ਦਸ਼ਮਲਵ notch_freq, ਦਸ਼ਮਲਵ bandwidth);

	/**
	 * Add a new raw value to the filter using the Direct Form I
	 *
	 * @return retrieve the filtered result
	 */
	ਸਤਰਅੰਦਰ T apply(ਅਚਲ T &sample)
	{
		ਜੇ (!_initialized) {
			reset(sample);
			_initialized = ਸੱਚ;
		}

		ਵਾਪਸ apply_ਅੰਦਰੂਨੀ(sample);
	}

	// Filter array of samples in place using the direct form I
	ਸਤਰਅੰਦਰ ਖਾਲੀ applyArray(T samples[], ਪੂਰਨ ਅੰਕ_samples)
	{
		ਜੇ (!_initialized) {
			reset(samples[0]);
			_initialized = ਸੱਚ;
		}

		ਲਈ (ਪੂਰਨ n = 0; n < ਅੰਕ_samples; n++) {
			samples[n] = apply_ਅੰਦਰੂਨੀ(samples[n]);
		}
	}

	ਦਸ਼ਮਲਵ getNotchFreq() ਅਚਲ { ਵਾਪਸ _notch_freq; }
	ਦਸ਼ਮਲਵ getBandwidth() ਅਚਲ { ਵਾਪਸ _bandwidth; }

	// Used in unit test only
	ਖਾਲੀ getCoefficients(ਦਸ਼ਮਲਵ a[3], ਦਸ਼ਮਲਵ b[3]) ਅਚਲ
	{
		a[0] = 1.f;
		a[1] = _a1;
		a[2] = _a2;
		b[0] = _b0;
		b[1] = _b1;
		b[2] = _b2;
	}

	ਦਸ਼ਮਲਵ getMagnitudeResponse(ਦਸ਼ਮਲਵ frequency) ਅਚਲ
	{
		ਦਸ਼ਮਲਵ w = 2.f * M_PI_F * frequency / _sample_freq;

		ਦਸ਼ਮਲਵ numerator = _b0 * _b0 + _b1 * _b1 + _b2 * _b2
				  + 2.f * (_b0 * _b1 + _b1 * _b2) * cosf(w) + 2.f * _b0 * _b2 * cosf(2.f * w);

		ਦਸ਼ਮਲਵ denominator = 1.f + _a1 * _a1 + _a2 * _a2 + 2.f * (_a1 + _a1 * _a2) * cosf(w) + 2.f * _a2 * cosf(2.f * w);

		ਵਾਪਸ sqrtf(numerator / denominator);
	}

	/**
	 * Bypasses the filter update to directly set different filter coefficients.
	 * Note: the filtered frequency and quality factor saved on the filter lose their
	 * physical meaning if you use this method to change the filter's coefficients.
	 * Used for creating clones of a specific filter.
	 */
	ਖਾਲੀ ਸੈੱਟ_Coefficients(ਦਸ਼ਮਲਵ a[2], ਦਸ਼ਮਲਵ b[3])
	{
		_a1 = a[0];
		_a2 = a[1];
		_b0 = b[0];
		_b1 = b[1];
		_b2 = b[2];
	}

	ਤਾਰਕਿਕ initialized() ਅਚਲ { ਵਾਪਸ _initialized; }

	ਖਾਲੀ reset() { _initialized = ਝੂਠ; }

	ਖਾਲੀ reset(ਅਚਲ T &sample)
	{
		ਅਚਲ T ਨਿਵੇਸ਼ = isFinite(sample) ? sample : T{};

		_delay_element_1 = _delay_element_2 = ਨਿਵੇਸ਼;
		_delay_element_output_1 = _delay_element_output_2 = ਨਿਵੇਸ਼ * (_b0 + _b1 + _b2) / (1 + _a1 + _a2);

		ਜੇ (!isFinite(_delay_element_1) || !isFinite(_delay_element_2)) {
			_delay_element_output_1 = _delay_element_output_2 = {};
		}

		_initialized = ਸੱਚ;
	}

	ਖਾਲੀ disable()
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

		_initialized = ਝੂਠ;
	}

ਸੁਰੱਖਿਅਤ:

	/**
	 * Add a new raw value to the filter using the Direct Form I
	 *
	 * @return retrieve the filtered result
	 */
	ਸਤਰਅੰਦਰ T apply_ਅੰਦਰੂਨੀ(ਅਚਲ T &sample)
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

		ਵਾਪਸ output;
	}

	T _delay_element_1{};
	T _delay_element_2{};
	T _delay_element_output_1{};
	T _delay_element_output_2{};

	// All the coefficients are normalized by a0, so a0 becomes 1 here
	ਦਸ਼ਮਲਵ _a1{0.f};
	ਦਸ਼ਮਲਵ _a2{0.f};

	ਦਸ਼ਮਲਵ _b0{1.f};
	ਦਸ਼ਮਲਵ _b1{0.f};
	ਦਸ਼ਮਲਵ _b2{0.f};

	ਦਸ਼ਮਲਵ _notch_freq{};
	ਦਸ਼ਮਲਵ _bandwidth{};
	ਦਸ਼ਮਲਵ _sample_freq{};

	ਤਾਰਕਿਕ _initialized{ਝੂਠ};
};

/**
 * Initialises the filter by setting its parameters and coefficients.
 * Using the direct form I method, allows to dynamically
 * update the filtered frequency, refresh rate and quality factor while
 * conserving the filter's history
 */
ਸਾਂਚਾ<ਕਿਸਮ_ਨਾਂ T>
ਤਾਰਕਿਕ NotchFilter<T>::ਸੈੱਟ_Parameters(ਦਸ਼ਮਲਵ sample_freq, ਦਸ਼ਮਲਵ notch_freq, ਦਸ਼ਮਲਵ bandwidth)
{
	ਜੇ ((sample_freq <= 0.f) || (notch_freq <= 0.f) || (bandwidth <= 0.f) || (notch_freq >= sample_freq / 2)
	    || !isFinite(sample_freq) || !isFinite(notch_freq) || !isFinite(bandwidth)) {

		disable();
		ਵਾਪਸ ਝੂਠ;
	}

	ਅਚਲ ਦਸ਼ਮਲਵ freq_min = sample_freq * 0.001f;

	ਅਚਲ ਦਸ਼ਮਲਵ notch_freq_new = math::max(notch_freq, freq_min);
	ਅਚਲ ਦਸ਼ਮਲਵ bandwidth_new = math::max(bandwidth, freq_min);

	ਅਚਲ ਦਸ਼ਮਲਵ sample_freq_diff = fabsf(sample_freq - _sample_freq);
	ਅਚਲ ਦਸ਼ਮਲਵ bandwidth_diff = fabsf(bandwidth_new - _bandwidth);

	ਅਚਲ ਤਾਰਕਿਕ sample_freq_change = (sample_freq_diff > FLT_EPSILON);
	ਅਚਲ ਤਾਰਕਿਕ bandwidth_change = (bandwidth_diff > FLT_EPSILON);

	ਜੇ (!sample_freq_change && !bandwidth_change) {
		ਅਚਲ ਦਸ਼ਮਲਵ notch_freq_diff = fabsf(notch_freq_new - _notch_freq);

		ਜੇ (notch_freq_diff > FLT_EPSILON) {
			// only notch frequency has changed
			_notch_freq = notch_freq_new;

			ਅਚਲ ਦਸ਼ਮਲਵ beta = -cosf(2.f * M_PI_F * _notch_freq / _sample_freq);

			_b1 = 2.f * beta * _b0;
			_a1 = _b1;

			ਜੇ (notch_freq_diff > _bandwidth) {
				// force reset
				_initialized = ਝੂਠ;
			}

			ਜੇ (!isFinite(_b1)) {
				disable();
				ਵਾਪਸ ਝੂਠ;
			}

			ਵਾਪਸ ਸੱਚ;

		} ਨਹੀਂਤਾਂ {
			// no change, do nothing
			ਵਾਪਸ ਸੱਚ;
		}
	}

	_sample_freq = sample_freq;
	_notch_freq = notch_freq_new;
	_bandwidth = bandwidth_new;

	ਅਚਲ ਦਸ਼ਮਲਵ alpha = tanf(M_PI_F * _bandwidth / _sample_freq);
	ਅਚਲ ਦਸ਼ਮਲਵ beta = -cosf(2.f * M_PI_F * _notch_freq / _sample_freq);
	ਅਚਲ ਦਸ਼ਮਲਵ a0_inv = 1.f / (alpha + 1.f);

	_b0 = a0_inv;
	_b1 = 2.f * beta * a0_inv;
	_b2 = a0_inv;

	_a1 = _b1;
	_a2 = (1.f - alpha) * a0_inv;

	ਜੇ (!isFinite(_b0) || !isFinite(_b1) || !isFinite(_b2) || !isFinite(_a2)) {
		disable();
		ਵਾਪਸ ਝੂਠ;
	}

	// force reset if bandwidth or sample frequency changed by more than 1%
	ਜੇ (bandwidth_diff > 0.01f * _bandwidth) {
		_initialized = ਝੂਠ;

	} ਨਹੀਂਤਾਂ ਜੇ (sample_freq_diff > 0.01f * _sample_freq) {
		_initialized = ਝੂਠ;
	}

	ਵਾਪਸ ਸੱਚ;
}

} // namespace math
