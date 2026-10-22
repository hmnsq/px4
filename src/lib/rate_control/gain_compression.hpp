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
#અન્તર્ભાવ <drivers/drv_hrt.h>

// Libraries
#અન્તર્ભાવ <math.h>
#અન્તર્ભાવ <lib/mathlib/math/filter/AlphaFilter.hpp>
#અન્તર્ભાવ <matrix/matrix/math.hpp>

// uORB includes
#અન્તર્ભાવ <uORB/Publication.hpp>
#અન્તર્ભાવ <uORB/topics/gain_compression.h>

વર્ગ GainCompression
{
જાહેર:
	શૂન્ય reset()
	{
		_compression_gain = 1.f;
		_hpf = 0.f;
		_lpf.reset(0.f);
		આગત_initialized = ખોટું;
	}
	દશાંશ update(દશાંશ આગત, દશાંશ dt);

	શૂન્ય setLpfCutoffFrequency(દશાંશ sample_freq, દશાંશ cutoff)
	{
		_lpf.setCutoffFreq(sample_freq, cutoff);
	}
	શૂન્ય setHpfCutoffFrequency(દશાંશ sample_freq, દશાંશ cutoff) { _alpha_hpf = sample_freq / (sample_freq + 2.f * M_PI_F * cutoff); }

	દશાંશ getSpectralDamperHpf() અચલ { પાછા _hpf * _hpf; }
	દશાંશ getSpectralDamperLpf() અચલ { પાછા _lpf.getState(); }
	શૂન્ય setCompressionGainMin(દશાંશ gain_min) { _compression_gain_min = gain_min; }

ખાનગી:
	દશાંશ _compression_gain{1.f};
	દશાંશ _compression_gain_min{0.3f};

	દશાંશ _alpha_hpf{0.f};
	દશાંશ _hpf{0.f};

	દશાંશ આગત_prev{0.f};
	તાર્કિક આગત_initialized{ખોટું};

	AlphaFilter<દશાંશ> _lpf;

	સ્થિર constexpr દશાંશ _kSpectralDamperGain{200.f}; // tuned based on flight test data to obtain a quick response
	સ્થિર constexpr દશાંશ _kLeakageGain{0.1f}; // 1/time_constant, slow enough to not interfere with the controller

};

વર્ગ GainCompression3d
{
જાહેર:
	GainCompression3d();
	~GainCompression3d() = મૂળભૂત;

	શૂન્ય reset();
	શૂન્ય update(અચલ matrix::Vector3f &આગત, દશાંશ dt);
	અચલ matrix::Vector3f &getGains() અચલ { પાછા _gains; };

	શૂન્ય setEnabled(તાર્કિક enabled) { _enabled = enabled; }
	શૂન્ય setCompressionGainMin(દશાંશ gain_min);

ખાનગી:
	// uORB publications
	uORB::Publication<gain_compression_s> _gain_compression_pub{ORB_ID(gain_compression)};

	GainCompression _compression_gains[3];
	matrix::Vector3f _gains{1.f, 1.f, 1.f};

	તાર્કિક _enabled{ખોટું};

	hrt_abstime _time_last_publication{0};

	સ્થિર constexpr દશાંશ _kLpfCutoffFrequency{5.f}; // Just above the control bandwidth of most UAVs
	સ્થિર constexpr દશાંશ _kHpfCutoffFrequency{2.f * _kLpfCutoffFrequency}; // 1 Octave above LPF cutoff, as recommended by the reference paper
};
