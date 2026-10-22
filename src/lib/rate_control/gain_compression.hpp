/****************************************************************************
 *
 *   Copyright (c) 2025 PX4 Development Team. All rights reserved.
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

/* Description:
 * This class produces a multiplicative gain that can be used on the output of a controller
 * to dynamically reduce the loop gain when oscillations are detected.
 *
 * Algorithm based on
 * Orr, Jeb, and Tannen Van Zwieten. "Robust, practical adaptive control for launch vehicles."
 * AIAA Guidance, Navigation, and Control Conference. 2012.
 * https://ntrs.nasa.gov/api/citations/20120015662/downloads/20120015662.pdf
 */

#pragma once

// PX4 includes
#ଅନ୍ତର୍ଭାଵ <drivers/drv_hrt.h>

// Libraries
#ଅନ୍ତର୍ଭାଵ <math.h>
#ଅନ୍ତର୍ଭାଵ <lib/mathlib/math/filter/AlphaFilter.hpp>
#ଅନ୍ତର୍ଭାଵ <matrix/matrix/math.hpp>

// uORB includes
#ଅନ୍ତର୍ଭାଵ <uORB/Publication.hpp>
#ଅନ୍ତର୍ଭାଵ <uORB/topics/gain_compression.h>

ଶ୍ରେଣୀ GainCompression
{
ସର୍ବସାଧାରଣ:
	ଶୂନ୍ୟ reset()
	{
		_compression_gain = 1.f;
		_hpf = 0.f;
		_lpf.reset(0.f);
		ନିବେଶ_initialized = ମିଥ୍ୟା;
	}
	ଦଶମିକ update(ଦଶମିକ ନିବେଶ, ଦଶମିକ dt);

	ଶୂନ୍ୟ setLpfCutoffFrequency(ଦଶମିକ sample_freq, ଦଶମିକ cutoff)
	{
		_lpf.setCutoffFreq(sample_freq, cutoff);
	}
	ଶୂନ୍ୟ setHpfCutoffFrequency(ଦଶମିକ sample_freq, ଦଶମିକ cutoff) { _alpha_hpf = sample_freq / (sample_freq + 2.f * M_PI_F * cutoff); }

	ଦଶମିକ getSpectralDamperHpf() ଅଚଳ { ଫେରନ୍ତୁ _hpf * _hpf; }
	ଦଶମିକ getSpectralDamperLpf() ଅଚଳ { ଫେରନ୍ତୁ _lpf.getState(); }
	ଶୂନ୍ୟ setCompressionGainMin(ଦଶମିକ gain_min) { _compression_gain_min = gain_min; }

ବ୍ୟକ୍ତିଗତ:
	ଦଶମିକ _compression_gain{1.f};
	ଦଶମିକ _compression_gain_min{0.3f};

	ଦଶମିକ _alpha_hpf{0.f};
	ଦଶମିକ _hpf{0.f};

	ଦଶମିକ ନିବେଶ_prev{0.f};
	ତାର୍କିକ ନିବେଶ_initialized{ମିଥ୍ୟା};

	AlphaFilter<ଦଶମିକ> _lpf;

	ସ୍ଥିର constexpr ଦଶମିକ _kSpectralDamperGain{200.f}; // tuned based on flight test data to obtain a quick response
	ସ୍ଥିର constexpr ଦଶମିକ _kLeakageGain{0.1f}; // 1/time_constant, slow enough to not interfere with the controller

};

ଶ୍ରେଣୀ GainCompression3d
{
ସର୍ବସାଧାରଣ:
	GainCompression3d();
	~GainCompression3d() = ପୂର୍ବନିର୍ଦ୍ଧାରିତ;

	ଶୂନ୍ୟ reset();
	ଶୂନ୍ୟ update(ଅଚଳ matrix::Vector3f &ନିବେଶ, ଦଶମିକ dt);
	ଅଚଳ matrix::Vector3f &getGains() ଅଚଳ { ଫେରନ୍ତୁ _gains; };

	ଶୂନ୍ୟ setEnabled(ତାର୍କିକ enabled) { _enabled = enabled; }
	ଶୂନ୍ୟ setCompressionGainMin(ଦଶମିକ gain_min);

ବ୍ୟକ୍ତିଗତ:
	// uORB publications
	uORB::Publication<gain_compression_s> _gain_compression_pub{ORB_ID(gain_compression)};

	GainCompression _compression_gains[3];
	matrix::Vector3f _gains{1.f, 1.f, 1.f};

	ତାର୍କିକ _enabled{ମିଥ୍ୟା};

	hrt_abstime _time_last_publication{0};

	ସ୍ଥିର constexpr ଦଶମିକ _kLpfCutoffFrequency{5.f}; // Just above the control bandwidth of most UAVs
	ସ୍ଥିର constexpr ଦଶମିକ _kHpfCutoffFrequency{2.f * _kLpfCutoffFrequency}; // 1 Octave above LPF cutoff, as recommended by the reference paper
};
