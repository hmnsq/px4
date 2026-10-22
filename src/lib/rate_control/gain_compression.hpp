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
#അന്തര്ഭാവ <drivers/drv_hrt.h>

// Libraries
#അന്തര്ഭാവ <math.h>
#അന്തര്ഭാവ <lib/mathlib/math/filter/AlphaFilter.hpp>
#അന്തര്ഭാവ <matrix/matrix/math.hpp>

// uORB includes
#അന്തര്ഭാവ <uORB/Publication.hpp>
#അന്തര്ഭാവ <uORB/topics/gain_compression.h>

വർഗ്ഗം GainCompression
{
പൊതു:
	ശൂന്യം reset()
	{
		_compression_gain = 1.f;
		_hpf = 0.f;
		_lpf.reset(0.f);
		നിവേശം_initialized = അസത്യം;
	}
	ദശാംശം update(ദശാംശം നിവേശം, ദശാംശം dt);

	ശൂന്യം setLpfCutoffFrequency(ദശാംശം sample_freq, ദശാംശം cutoff)
	{
		_lpf.setCutoffFreq(sample_freq, cutoff);
	}
	ശൂന്യം setHpfCutoffFrequency(ദശാംശം sample_freq, ദശാംശം cutoff) { _alpha_hpf = sample_freq / (sample_freq + 2.f * M_PI_F * cutoff); }

	ദശാംശം getSpectralDamperHpf() സ്ഥിരം { തിരികെ _hpf * _hpf; }
	ദശാംശം getSpectralDamperLpf() സ്ഥിരം { തിരികെ _lpf.getState(); }
	ശൂന്യം setCompressionGainMin(ദശാംശം gain_min) { _compression_gain_min = gain_min; }

സ്വകാര്യ:
	ദശാംശം _compression_gain{1.f};
	ദശാംശം _compression_gain_min{0.3f};

	ദശാംശം _alpha_hpf{0.f};
	ദശാംശം _hpf{0.f};

	ദശാംശം നിവേശം_prev{0.f};
	തർക്കം നിവേശം_initialized{അസത്യം};

	AlphaFilter<ദശാംശം> _lpf;

	സ്ഥിതം constexpr ദശാംശം _kSpectralDamperGain{200.f}; // tuned based on flight test data to obtain a quick response
	സ്ഥിതം constexpr ദശാംശം _kLeakageGain{0.1f}; // 1/time_constant, slow enough to not interfere with the controller

};

വർഗ്ഗം GainCompression3d
{
പൊതു:
	GainCompression3d();
	~GainCompression3d() = സ്ഥിരസ്ഥിതി;

	ശൂന്യം reset();
	ശൂന്യം update(സ്ഥിരം matrix::Vector3f &നിവേശം, ദശാംശം dt);
	സ്ഥിരം matrix::Vector3f &getGains() സ്ഥിരം { തിരികെ _gains; };

	ശൂന്യം setEnabled(തർക്കം enabled) { _enabled = enabled; }
	ശൂന്യം setCompressionGainMin(ദശാംശം gain_min);

സ്വകാര്യ:
	// uORB publications
	uORB::Publication<gain_compression_s> _gain_compression_pub{ORB_ID(gain_compression)};

	GainCompression _compression_gains[3];
	matrix::Vector3f _gains{1.f, 1.f, 1.f};

	തർക്കം _enabled{അസത്യം};

	hrt_abstime _time_last_publication{0};

	സ്ഥിതം constexpr ദശാംശം _kLpfCutoffFrequency{5.f}; // Just above the control bandwidth of most UAVs
	സ്ഥിതം constexpr ദശാംശം _kHpfCutoffFrequency{2.f * _kLpfCutoffFrequency}; // 1 Octave above LPF cutoff, as recommended by the reference paper
};
