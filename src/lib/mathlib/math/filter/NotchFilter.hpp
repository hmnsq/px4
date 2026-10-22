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

#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ <mathlib/math/Functions.hpp>
#ಅನ್ತರ್ಭಾವ <cmath>
#ಅನ್ತರ್ಭಾವ <float.h>
#ಅನ್ತರ್ಭಾವ <matrix/math.hpp>

ನಾಮಾಕಾಶ math
{

ಮಾದರಿ<ಬಗೆ_ಹೆಸರು T>
ವರ್ಗ NotchFilter
{
ಸಾರ್ವಜನಿಕ:
	NotchFilter() = ಪೂರ್ವನಿಯೋಜಿತ;
	~NotchFilter() = ಪೂರ್ವನಿಯೋಜಿತ;

	ತಾರ್ಕಿಕ ಗಣ_Parameters(ದಶಮಾಂಶ sample_freq, ದಶಮಾಂಶ notch_freq, ದಶಮಾಂಶ bandwidth);

	/**
	 * Add a new raw value to the filter using the Direct Form I
	 *
	 * @return retrieve the filtered result
	 */
	ಸಾಲೊಳಗೆ T apply(ಸ್ಥಿರಾಂಕ T &sample)
	{
		ಆದರೆ (!_initialized) {
			reset(sample);
			_initialized = ನಿಜ;
		}

		ಹಿಂತಿರುಗಿ apply_ಆಂತರಿಕ(sample);
	}

	// Filter array of samples in place using the direct form I
	ಸಾಲೊಳಗೆ ಶೂನ್ಯ applyArray(T samples[], ಪೂರ್ಣಾಂಕ ಅಂಕೆ_samples)
	{
		ಆದರೆ (!_initialized) {
			reset(samples[0]);
			_initialized = ನಿಜ;
		}

		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ n = 0; n < ಅಂಕೆ_samples; n++) {
			samples[n] = apply_ಆಂತರಿಕ(samples[n]);
		}
	}

	ದಶಮಾಂಶ getNotchFreq() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ _notch_freq; }
	ದಶಮಾಂಶ getBandwidth() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ _bandwidth; }

	// Used in unit test only
	ಶೂನ್ಯ getCoefficients(ದಶಮಾಂಶ a[3], ದಶಮಾಂಶ b[3]) ಸ್ಥಿರಾಂಕ
	{
		a[0] = 1.f;
		a[1] = _a1;
		a[2] = _a2;
		b[0] = _b0;
		b[1] = _b1;
		b[2] = _b2;
	}

	ದಶಮಾಂಶ getMagnitudeResponse(ದಶಮಾಂಶ frequency) ಸ್ಥಿರಾಂಕ
	{
		ದಶಮಾಂಶ w = 2.f * M_PI_F * frequency / _sample_freq;

		ದಶಮಾಂಶ numerator = _b0 * _b0 + _b1 * _b1 + _b2 * _b2
				  + 2.f * (_b0 * _b1 + _b1 * _b2) * cosf(w) + 2.f * _b0 * _b2 * cosf(2.f * w);

		ದಶಮಾಂಶ denominator = 1.f + _a1 * _a1 + _a2 * _a2 + 2.f * (_a1 + _a1 * _a2) * cosf(w) + 2.f * _a2 * cosf(2.f * w);

		ಹಿಂತಿರುಗಿ sqrtf(numerator / denominator);
	}

	/**
	 * Bypasses the filter update to directly set different filter coefficients.
	 * Note: the filtered frequency and quality factor saved on the filter lose their
	 * physical meaning if you use this method to change the filter's coefficients.
	 * Used for creating clones of a specific filter.
	 */
	ಶೂನ್ಯ ಗಣ_Coefficients(ದಶಮಾಂಶ a[2], ದಶಮಾಂಶ b[3])
	{
		_a1 = a[0];
		_a2 = a[1];
		_b0 = b[0];
		_b1 = b[1];
		_b2 = b[2];
	}

	ತಾರ್ಕಿಕ initialized() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ _initialized; }

	ಶೂನ್ಯ reset() { _initialized = ಸುಳ್ಳು; }

	ಶೂನ್ಯ reset(ಸ್ಥಿರಾಂಕ T &sample)
	{
		ಸ್ಥಿರಾಂಕ T ಒಳಹರಿವು = isFinite(sample) ? sample : T{};

		_delay_element_1 = _delay_element_2 = ಒಳಹರಿವು;
		_delay_element_output_1 = _delay_element_output_2 = ಒಳಹರಿವು * (_b0 + _b1 + _b2) / (1 + _a1 + _a2);

		ಆದರೆ (!isFinite(_delay_element_1) || !isFinite(_delay_element_2)) {
			_delay_element_output_1 = _delay_element_output_2 = {};
		}

		_initialized = ನಿಜ;
	}

	ಶೂನ್ಯ disable()
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

		_initialized = ಸುಳ್ಳು;
	}

ಸಂರಕ್ಷಿತ:

	/**
	 * Add a new raw value to the filter using the Direct Form I
	 *
	 * @return retrieve the filtered result
	 */
	ಸಾಲೊಳಗೆ T apply_ಆಂತರಿಕ(ಸ್ಥಿರಾಂಕ T &sample)
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

		ಹಿಂತಿರುಗಿ output;
	}

	T _delay_element_1{};
	T _delay_element_2{};
	T _delay_element_output_1{};
	T _delay_element_output_2{};

	// All the coefficients are normalized by a0, so a0 becomes 1 here
	ದಶಮಾಂಶ _a1{0.f};
	ದಶಮಾಂಶ _a2{0.f};

	ದಶಮಾಂಶ _b0{1.f};
	ದಶಮಾಂಶ _b1{0.f};
	ದಶಮಾಂಶ _b2{0.f};

	ದಶಮಾಂಶ _notch_freq{};
	ದಶಮಾಂಶ _bandwidth{};
	ದಶಮಾಂಶ _sample_freq{};

	ತಾರ್ಕಿಕ _initialized{ಸುಳ್ಳು};
};

/**
 * Initialises the filter by setting its parameters and coefficients.
 * Using the direct form I method, allows to dynamically
 * update the filtered frequency, refresh rate and quality factor while
 * conserving the filter's history
 */
ಮಾದರಿ<ಬಗೆ_ಹೆಸರು T>
ತಾರ್ಕಿಕ NotchFilter<T>::ಗಣ_Parameters(ದಶಮಾಂಶ sample_freq, ದಶಮಾಂಶ notch_freq, ದಶಮಾಂಶ bandwidth)
{
	ಆದರೆ ((sample_freq <= 0.f) || (notch_freq <= 0.f) || (bandwidth <= 0.f) || (notch_freq >= sample_freq / 2)
	    || !isFinite(sample_freq) || !isFinite(notch_freq) || !isFinite(bandwidth)) {

		disable();
		ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
	}

	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ freq_min = sample_freq * 0.001f;

	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ notch_freq_new = math::max(notch_freq, freq_min);
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ bandwidth_new = math::max(bandwidth, freq_min);

	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ sample_freq_diff = fabsf(sample_freq - _sample_freq);
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ bandwidth_diff = fabsf(bandwidth_new - _bandwidth);

	ಸ್ಥಿರಾಂಕ ತಾರ್ಕಿಕ sample_freq_change = (sample_freq_diff > FLT_EPSILON);
	ಸ್ಥಿರಾಂಕ ತಾರ್ಕಿಕ bandwidth_change = (bandwidth_diff > FLT_EPSILON);

	ಆದರೆ (!sample_freq_change && !bandwidth_change) {
		ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ notch_freq_diff = fabsf(notch_freq_new - _notch_freq);

		ಆದರೆ (notch_freq_diff > FLT_EPSILON) {
			// only notch frequency has changed
			_notch_freq = notch_freq_new;

			ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ beta = -cosf(2.f * M_PI_F * _notch_freq / _sample_freq);

			_b1 = 2.f * beta * _b0;
			_a1 = _b1;

			ಆದರೆ (notch_freq_diff > _bandwidth) {
				// force reset
				_initialized = ಸುಳ್ಳು;
			}

			ಆದರೆ (!isFinite(_b1)) {
				disable();
				ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
			}

			ಹಿಂತಿರುಗಿ ನಿಜ;

		} ಇಲ್ಲದಿದ್ದರೆ {
			// no change, do nothing
			ಹಿಂತಿರುಗಿ ನಿಜ;
		}
	}

	_sample_freq = sample_freq;
	_notch_freq = notch_freq_new;
	_bandwidth = bandwidth_new;

	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ alpha = tanf(M_PI_F * _bandwidth / _sample_freq);
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ beta = -cosf(2.f * M_PI_F * _notch_freq / _sample_freq);
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ a0_inv = 1.f / (alpha + 1.f);

	_b0 = a0_inv;
	_b1 = 2.f * beta * a0_inv;
	_b2 = a0_inv;

	_a1 = _b1;
	_a2 = (1.f - alpha) * a0_inv;

	ಆದರೆ (!isFinite(_b0) || !isFinite(_b1) || !isFinite(_b2) || !isFinite(_a2)) {
		disable();
		ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
	}

	// force reset if bandwidth or sample frequency changed by more than 1%
	ಆದರೆ (bandwidth_diff > 0.01f * _bandwidth) {
		_initialized = ಸುಳ್ಳು;

	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (sample_freq_diff > 0.01f * _sample_freq) {
		_initialized = ಸುಳ್ಳು;
	}

	ಹಿಂತಿರುಗಿ ನಿಜ;
}

} // namespace math
