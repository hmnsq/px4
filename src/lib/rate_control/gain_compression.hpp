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
#ضمن <drivers/drv_hrt.h>

// Libraries
#ضمن <math.h>
#ضمن <lib/mathlib/math/filter/AlphaFilter.hpp>
#ضمن <matrix/matrix/math.hpp>

// uORB includes
#ضمن <uORB/Publication.hpp>
#ضمن <uORB/topics/gain_compression.h>

صنف GainCompression
{
عام:
	فراغ reset()
	{
		_compression_gain = 1.f;
		_hpf = 0.f;
		_lpf.reset(0.f);
		ماسوقن_initialized = خطأ;
	}
	عائم update(عائم ماسوقن, عائم dt);

	فراغ setLpfCutoffFrequency(عائم sample_freq, عائم cutoff)
	{
		_lpf.setCutoffFreq(sample_freq, cutoff);
	}
	فراغ setHpfCutoffFrequency(عائم sample_freq, عائم cutoff) { _alpha_hpf = sample_freq / (sample_freq + 2.f * M_PI_F * cutoff); }

	عائم getSpectralDamperHpf() ثابت { أرجع _hpf * _hpf; }
	عائم getSpectralDamperLpf() ثابت { أرجع _lpf.getState(); }
	فراغ setCompressionGainMin(عائم gain_min) { _compression_gain_min = gain_min; }

خاص:
	عائم _compression_gain{1.f};
	عائم _compression_gain_min{0.3f};

	عائم _alpha_hpf{0.f};
	عائم _hpf{0.f};

	عائم ماسوقن_prev{0.f};
	منطقي ماسوقن_initialized{خطأ};

	AlphaFilter<عائم> _lpf;

	ساكن constexpr عائم _kSpectralDamperGain{200.f}; // tuned based on flight test data to obtain a quick response
	ساكن constexpr عائم _kLeakageGain{0.1f}; // 1/time_constant, slow enough to not interfere with the controller

};

صنف GainCompression3d
{
عام:
	GainCompression3d();
	~GainCompression3d() = افتراضي;

	فراغ reset();
	فراغ update(ثابت matrix::Vector3f &ماسوقن, عائم dt);
	ثابت matrix::Vector3f &getGains() ثابت { أرجع _gains; };

	فراغ setEnabled(منطقي enabled) { _enabled = enabled; }
	فراغ setCompressionGainMin(عائم gain_min);

خاص:
	// uORB publications
	uORB::Publication<gain_compression_s> _gain_compression_pub{ORB_ID(gain_compression)};

	GainCompression _compression_gains[3];
	matrix::Vector3f _gains{1.f, 1.f, 1.f};

	منطقي _enabled{خطأ};

	hrt_abstime _time_last_publication{0};

	ساكن constexpr عائم _kLpfCutoffFrequency{5.f}; // Just above the control bandwidth of most UAVs
	ساكن constexpr عائم _kHpfCutoffFrequency{2.f * _kLpfCutoffFrequency}; // 1 Octave above LPF cutoff, as recommended by the reference paper
};
