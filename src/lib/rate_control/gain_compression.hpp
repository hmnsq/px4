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
#অন্তর্ভাব <drivers/drv_hrt.h>

// Libraries
#অন্তর্ভাব <math.h>
#অন্তর্ভাব <lib/mathlib/math/filter/AlphaFilter.hpp>
#অন্তর্ভাব <matrix/matrix/math.hpp>

// uORB includes
#অন্তর্ভাব <uORB/Publication.hpp>
#অন্তর্ভাব <uORB/topics/gain_compression.h>

শ্রেণী GainCompression
{
সর্বজনীন:
	শূন্য reset()
	{
		_compression_gain = 1.f;
		_hpf = 0.f;
		_lpf.reset(0.f);
		প্রবেশ_initialized = মিথ্যা;
	}
	দশমিক update(দশমিক প্রবেশ, দশমিক dt);

	শূন্য setLpfCutoffFrequency(দশমিক sample_freq, দশমিক cutoff)
	{
		_lpf.setCutoffFreq(sample_freq, cutoff);
	}
	শূন্য setHpfCutoffFrequency(দশমিক sample_freq, দশমিক cutoff) { _alpha_hpf = sample_freq / (sample_freq + 2.f * M_PI_F * cutoff); }

	দশমিক getSpectralDamperHpf() ধ্রুবক { ফেরত _hpf * _hpf; }
	দশমিক getSpectralDamperLpf() ধ্রুবক { ফেরত _lpf.getState(); }
	শূন্য setCompressionGainMin(দশমিক gain_min) { _compression_gain_min = gain_min; }

ব্যক্তিগত:
	দশমিক _compression_gain{1.f};
	দশমিক _compression_gain_min{0.3f};

	দশমিক _alpha_hpf{0.f};
	দশমিক _hpf{0.f};

	দশমিক প্রবেশ_পূর্ব{0.f};
	যৌক্তিক প্রবেশ_initialized{মিথ্যা};

	AlphaFilter<দশমিক> _lpf;

	স্থির constexpr দশমিক _kSpectralDamperGain{200.f}; // tuned based on flight test data to obtain a quick response
	স্থির constexpr দশমিক _kLeakageGain{0.1f}; // 1/time_constant, slow enough to not interfere with the controller

};

শ্রেণী GainCompression3d
{
সর্বজনীন:
	GainCompression3d();
	~GainCompression3d() = পূর্বনির্ধারিত;

	শূন্য reset();
	শূন্য update(ধ্রুবক matrix::Vector3f &প্রবেশ, দশমিক dt);
	ধ্রুবক matrix::Vector3f &getGains() ধ্রুবক { ফেরত _gains; };

	শূন্য setEnabled(যৌক্তিক enabled) { _enabled = enabled; }
	শূন্য setCompressionGainMin(দশমিক gain_min);

ব্যক্তিগত:
	// uORB publications
	uORB::Publication<gain_compression_s> _gain_compression_pub{ORB_ID(gain_compression)};

	GainCompression _compression_gains[3];
	matrix::Vector3f _gains{1.f, 1.f, 1.f};

	যৌক্তিক _enabled{মিথ্যা};

	hrt_abstime _time_last_publication{0};

	স্থির constexpr দশমিক _kLpfCutoffFrequency{5.f}; // Just above the control bandwidth of most UAVs
	স্থির constexpr দশমিক _kHpfCutoffFrequency{2.f * _kLpfCutoffFrequency}; // 1 Octave above LPF cutoff, as recommended by the reference paper
};
