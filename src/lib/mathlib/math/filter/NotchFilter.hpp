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

#شامل <mathlib/math/Functions.hpp>
#شامل <cmath>
#شامل <float.h>
#شامل <matrix/math.hpp>

فضاینام math
{

template<typename T>
رده NotchFilter
{
عمومی:
	NotchFilter() = پیشفرض;
	~NotchFilter() = پیشفرض;

	منطقی setParameters(اعشاری sample_freq, اعشاری notch_freq, اعشاری bandwidth);

	/**
	 * Add a new raw value to the filter using the Direct Form I
	 *
	 * @return retrieve the filtered result
	 */
	درونخطی T apply(ثابت T &sample)
	{
		اگر (!_initialized) {
			reset(sample);
			_initialized = درست;
		}

		بازگشت applyInternal(sample);
	}

	// Filter array of samples in place using the direct form I
	درونخطی تهی applyArray(T samples[], صحیح رقم_samples)
	{
		اگر (!_initialized) {
			reset(samples[0]);
			_initialized = درست;
		}

		برای (صحیح n = 0; n < رقم_samples; n++) {
			samples[n] = applyInternal(samples[n]);
		}
	}

	اعشاری getNotchFreq() ثابت { بازگشت _notch_freq; }
	اعشاری getBandwidth() ثابت { بازگشت _bandwidth; }

	// Used in unit test only
	تهی getCoefficients(اعشاری a[3], اعشاری b[3]) ثابت
	{
		a[0] = 1.f;
		a[1] = _a1;
		a[2] = _a2;
		b[0] = _b0;
		b[1] = _b1;
		b[2] = _b2;
	}

	اعشاری getMagnitudeResponse(اعشاری frequency) ثابت
	{
		اعشاری w = 2.f * M_PI_F * frequency / _sample_freq;

		اعشاری numerator = _b0 * _b0 + _b1 * _b1 + _b2 * _b2
				  + 2.f * (_b0 * _b1 + _b1 * _b2) * cosf(w) + 2.f * _b0 * _b2 * cosf(2.f * w);

		اعشاری denominator = 1.f + _a1 * _a1 + _a2 * _a2 + 2.f * (_a1 + _a1 * _a2) * cosf(w) + 2.f * _a2 * cosf(2.f * w);

		بازگشت sqrtf(numerator / denominator);
	}

	/**
	 * Bypasses the filter update to directly set different filter coefficients.
	 * Note: the filtered frequency and quality factor saved on the filter lose their
	 * physical meaning if you use this method to change the filter's coefficients.
	 * Used for creating clones of a specific filter.
	 */
	تهی setCoefficients(اعشاری a[2], اعشاری b[3])
	{
		_a1 = a[0];
		_a2 = a[1];
		_b0 = b[0];
		_b1 = b[1];
		_b2 = b[2];
	}

	منطقی initialized() ثابت { بازگشت _initialized; }

	تهی reset() { _initialized = نادرست; }

	تهی reset(ثابت T &sample)
	{
		ثابت T درون_داد = isFinite(sample) ? sample : T{};

		_delay_element_1 = _delay_element_2 = درون_داد;
		_delay_element_output_1 = _delay_element_output_2 = درون_داد * (_b0 + _b1 + _b2) / (1 + _a1 + _a2);

		اگر (!isFinite(_delay_element_1) || !isFinite(_delay_element_2)) {
			_delay_element_output_1 = _delay_element_output_2 = {};
		}

		_initialized = درست;
	}

	تهی disable()
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

		_initialized = نادرست;
	}

محافظتشده:

	/**
	 * Add a new raw value to the filter using the Direct Form I
	 *
	 * @return retrieve the filtered result
	 */
	درونخطی T applyInternal(ثابت T &sample)
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

		بازگشت output;
	}

	T _delay_element_1{};
	T _delay_element_2{};
	T _delay_element_output_1{};
	T _delay_element_output_2{};

	// All the coefficients are normalized by a0, so a0 becomes 1 here
	اعشاری _a1{0.f};
	اعشاری _a2{0.f};

	اعشاری _b0{1.f};
	اعشاری _b1{0.f};
	اعشاری _b2{0.f};

	اعشاری _notch_freq{};
	اعشاری _bandwidth{};
	اعشاری _sample_freq{};

	منطقی _initialized{نادرست};
};

/**
 * Initialises the filter by setting its parameters and coefficients.
 * Using the direct form I method, allows to dynamically
 * update the filtered frequency, refresh rate and quality factor while
 * conserving the filter's history
 */
template<typename T>
منطقی NotchFilter<T>::setParameters(اعشاری sample_freq, اعشاری notch_freq, اعشاری bandwidth)
{
	اگر ((sample_freq <= 0.f) || (notch_freq <= 0.f) || (bandwidth <= 0.f) || (notch_freq >= sample_freq / 2)
	    || !isFinite(sample_freq) || !isFinite(notch_freq) || !isFinite(bandwidth)) {

		disable();
		بازگشت نادرست;
	}

	ثابت اعشاری freq_min = sample_freq * 0.001f;

	ثابت اعشاری notch_freq_new = math::max(notch_freq, freq_min);
	ثابت اعشاری bandwidth_new = math::max(bandwidth, freq_min);

	ثابت اعشاری sample_freq_diff = fabsf(sample_freq - _sample_freq);
	ثابت اعشاری bandwidth_diff = fabsf(bandwidth_new - _bandwidth);

	ثابت منطقی sample_freq_change = (sample_freq_diff > FLT_EPSILON);
	ثابت منطقی bandwidth_change = (bandwidth_diff > FLT_EPSILON);

	اگر (!sample_freq_change && !bandwidth_change) {
		ثابت اعشاری notch_freq_diff = fabsf(notch_freq_new - _notch_freq);

		اگر (notch_freq_diff > FLT_EPSILON) {
			// only notch frequency has changed
			_notch_freq = notch_freq_new;

			ثابت اعشاری beta = -cosf(2.f * M_PI_F * _notch_freq / _sample_freq);

			_b1 = 2.f * beta * _b0;
			_a1 = _b1;

			اگر (notch_freq_diff > _bandwidth) {
				// force reset
				_initialized = نادرست;
			}

			اگر (!isFinite(_b1)) {
				disable();
				بازگشت نادرست;
			}

			بازگشت درست;

		} وگرنه {
			// no change, do nothing
			بازگشت درست;
		}
	}

	_sample_freq = sample_freq;
	_notch_freq = notch_freq_new;
	_bandwidth = bandwidth_new;

	ثابت اعشاری alpha = tanf(M_PI_F * _bandwidth / _sample_freq);
	ثابت اعشاری beta = -cosf(2.f * M_PI_F * _notch_freq / _sample_freq);
	ثابت اعشاری a0_inv = 1.f / (alpha + 1.f);

	_b0 = a0_inv;
	_b1 = 2.f * beta * a0_inv;
	_b2 = a0_inv;

	_a1 = _b1;
	_a2 = (1.f - alpha) * a0_inv;

	اگر (!isFinite(_b0) || !isFinite(_b1) || !isFinite(_b2) || !isFinite(_a2)) {
		disable();
		بازگشت نادرست;
	}

	// force reset if bandwidth or sample frequency changed by more than 1%
	اگر (bandwidth_diff > 0.01f * _bandwidth) {
		_initialized = نادرست;

	} وگرنه اگر (sample_freq_diff > 0.01f * _sample_freq) {
		_initialized = نادرست;
	}

	بازگشت درست;
}

} // namespace math
