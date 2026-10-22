/****************************************************************************
 *
 *   Copyright (C) 2012-2021 PX4 Development Team. All rights reserved.
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

/// @file	LowPassFilter2p.hpp
/// @brief	A class to implement a second order low pass filter
/// Author: Leonard Hall <LeonardTHall@gmail.com>
/// Adapted for PX4 by Andrew Tridgell

#pragma once

#અન્તર્ભાવ <mathlib/math/Functions.hpp>
#અન્તર્ભાવ <float.h>
#અન્તર્ભાવ <matrix/math.hpp>

નામાકાશ math
{

template<typename T>
વર્ગ નીચલું_Pass_Filter2p
{
જાહેર:
	નીચલું_Pass_Filter2p() = મૂળભૂત;

	નીચલું_Pass_Filter2p(દશાંશ sample_freq, દશાંશ cutoff_freq)
	{
		// set initial parameters
		set_cutoff_frequency(sample_freq, cutoff_freq);
	}

	// Change filter parameters
	શૂન્ય set_cutoff_frequency(દશાંશ sample_freq, દશાંશ cutoff_freq)
	{
		જો ((sample_freq <= 0.f) || (cutoff_freq <= 0.f) || (cutoff_freq >= sample_freq / 2)
		    || !isFinite(sample_freq) || !isFinite(cutoff_freq)) {

			disable();
			પાછા;
		}

		// reset delay elements on filter change
		_delay_element_1 = {};
		_delay_element_2 = {};

		_cutoff_freq = math::max(cutoff_freq, sample_freq * 0.001f);
		_sample_freq = sample_freq;

		અચલ દશાંશ fr = _sample_freq / _cutoff_freq;
		અચલ દશાંશ ohm = tanf(M_PI_F / fr);
		અચલ દશાંશ c = 1.f + 2.f * cosf(M_PI_F / 4.f) * ohm + ohm * ohm;

		_b0 = ohm * ohm / c;
		_b1 = 2.f * _b0;
		_b2 = _b0;

		_a1 = 2.f * (ohm * ohm - 1.f) / c;
		_a2 = (1.f - 2.f * cosf(M_PI_F / 4.f) * ohm + ohm * ohm) / c;

		જો (!isFinite(_b0) || !isFinite(_b1) || !isFinite(_b2) || !isFinite(_a1) || !isFinite(_a2)) {
			disable();
		}
	}

	/**
	 * Add a new raw value to the filter
	 *
	 * @return retrieve the filtered result
	 */
	અંતઃરેખ T apply(અચલ T &sample)
	{
		// Direct Form II implementation
		T delay_element_0{sample - _delay_element_1 *_a1 - _delay_element_2 * _a2};

		T output{delay_element_0 *_b0 + _delay_element_1 *_b1 + _delay_element_2 * _b2};

		_delay_element_2 = _delay_element_1;
		_delay_element_1 = delay_element_0;

		પાછા output;
	}

	// Filter array of samples in place using the Direct form II.
	અંતઃરેખ શૂન્ય applyArray(T samples[], પૂર્ણાંક અંક_samples)
	{
		માટે (પૂર્ણાંક n = 0; n < અંક_samples; n++) {
			samples[n] = apply(samples[n]);
		}
	}

	// Return the cutoff frequency
	દશાંશ get_cutoff_freq() અચલ { પાછા _cutoff_freq; }

	// Return the sample frequency
	દશાંશ get_sample_freq() અચલ { પાછા _sample_freq; }

	દશાંશ getMagnitudeResponse(દશાંશ frequency) અચલ;

	// Reset the filter state to this value
	T reset(અચલ T &sample)
	{
		અચલ T આગત = isFinite(sample) ? sample : T{};

		જો (fabsf(1 + _a1 + _a2) > FLT_EPSILON) {
			_delay_element_1 = _delay_element_2 = આગત / (1 + _a1 + _a2);

			જો (!isFinite(_delay_element_1) || !isFinite(_delay_element_2)) {
				_delay_element_1 = _delay_element_2 = આગત;
			}

		} નહીંતર {
			_delay_element_1 = _delay_element_2 = આગત;
		}

		પાછા apply(આગત);
	}

	શૂન્ય disable()
	{
		// no filtering
		_sample_freq = 0.f;
		_cutoff_freq = 0.f;

		_delay_element_1 = {};
		_delay_element_2 = {};

		_b0 = 1.f;
		_b1 = 0.f;
		_b2 = 0.f;

		_a1 = 0.f;
		_a2 = 0.f;
	}

સુરક્ષિત:
	T _delay_element_1{}; // buffered sample -1
	T _delay_element_2{}; // buffered sample -2

	// All the coefficients are normalized by a0, so a0 becomes 1 here
	દશાંશ _a1{0.f};
	દશાંશ _a2{0.f};

	દશાંશ _b0{1.f};
	દશાંશ _b1{0.f};
	દશાંશ _b2{0.f};

	દશાંશ _cutoff_freq{0.f};
	દશાંશ _sample_freq{0.f};
};

} // namespace math
