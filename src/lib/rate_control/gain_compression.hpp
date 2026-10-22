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
#অন্তর্ভাৱ <drivers/drv_hrt.h>

// Libraries
#অন্তর্ভাৱ <math.h>
#অন্তর্ভাৱ <lib/mathlib/math/filter/AlphaFilter.hpp>
#অন্তর্ভাৱ <matrix/matrix/math.hpp>

// uORB includes
#অন্তর্ভাৱ <uORB/Publication.hpp>
#অন্তর্ভাৱ <uORB/topics/gain_compression.h>

শ্ৰেণী GainCompression
{
সার্ৱজনিক:
	শূন্য reset()
	{
		_compression_gain = 1.f;
		_hpf = 0.f;
		_lpf.reset(0.f);
		প্ৰবেশ_initialized = মিছা;
	}
	ভিন্ন update(ভিন্ন প্ৰবেশ, ভিন্ন dt);

	শূন্য setLpfCutoffFrequency(ভিন্ন sample_freq, ভিন্ন cutoff)
	{
		_lpf.setCutoffFreq(sample_freq, cutoff);
	}
	শূন্য setHpfCutoffFrequency(ভিন্ন sample_freq, ভিন্ন cutoff) { _alpha_hpf = sample_freq / (sample_freq + 2.f * M_PI_F * cutoff); }

	ভিন্ন getSpectralDamperHpf() ধ্রুৱ { ঘূৰাই _hpf * _hpf; }
	ভিন্ন getSpectralDamperLpf() ধ্রুৱ { ঘূৰাই _lpf.getState(); }
	শূন্য setCompressionGainMin(ভিন্ন gain_min) { _compression_gain_min = gain_min; }

গুপ্ত:
	ভিন্ন _compression_gain{1.f};
	ভিন্ন _compression_gain_min{0.3f};

	ভিন্ন _alpha_hpf{0.f};
	ভিন্ন _hpf{0.f};

	ভিন্ন প্ৰবেশ_prev{0.f};
	তার্কিক প্ৰবেশ_initialized{মিছা};

	AlphaFilter<ভিন্ন> _lpf;

	স্থির constexpr ভিন্ন _kSpectralDamperGain{200.f}; // tuned based on flight test data to obtain a quick response
	স্থির constexpr ভিন্ন _kLeakageGain{0.1f}; // 1/time_constant, slow enough to not interfere with the controller

};

শ্ৰেণী GainCompression3d
{
সার্ৱজনিক:
	GainCompression3d();
	~GainCompression3d() = স্ৱভাৱ;

	শূন্য reset();
	শূন্য update(ধ্রুৱ matrix::Vector3f &প্ৰবেশ, ভিন্ন dt);
	ধ্রুৱ matrix::Vector3f &getGains() ধ্রুৱ { ঘূৰাই _gains; };

	শূন্য setEnabled(তার্কিক enabled) { _enabled = enabled; }
	শূন্য setCompressionGainMin(ভিন্ন gain_min);

গুপ্ত:
	// uORB publications
	uORB::Publication<gain_compression_s> _gain_compression_pub{ORB_ID(gain_compression)};

	GainCompression _compression_gains[3];
	matrix::Vector3f _gains{1.f, 1.f, 1.f};

	তার্কিক _enabled{মিছা};

	hrt_abstime _time_last_publication{0};

	স্থির constexpr ভিন্ন _kLpfCutoffFrequency{5.f}; // Just above the control bandwidth of most UAVs
	স্থির constexpr ভিন্ন _kHpfCutoffFrequency{2.f * _kLpfCutoffFrequency}; // 1 Octave above LPF cutoff, as recommended by the reference paper
};
