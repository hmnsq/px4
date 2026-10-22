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
#अन्तर्भाव <drivers/drv_hrt.h>

// Libraries
#अन्तर्भाव <math.h>
#अन्तर्भाव <lib/mathlib/math/filter/AlphaFilter.hpp>
#अन्तर्भाव <matrix/matrix/math.hpp>

// uORB includes
#अन्तर्भाव <uORB/Publication.hpp>
#अन्तर्भाव <uORB/topics/gain_compression.h>

वर्ग GainCompression
{
सार्वजनिक:
	शून्य reset()
	{
		_compression_gain = 1.f;
		_hpf = 0.f;
		_lpf.reset(0.f);
		आगत_initialized = असत्य;
	}
	भिन्न update(भिन्न आगत, भिन्न dt);

	शून्य setLpfCutoffFrequency(भिन्न sample_freq, भिन्न cutoff)
	{
		_lpf.setCutoffFreq(sample_freq, cutoff);
	}
	शून्य setHpfCutoffFrequency(भिन्न sample_freq, भिन्न cutoff) { _alpha_hpf = sample_freq / (sample_freq + 2.f * M_PI_F * cutoff); }

	भिन्न getSpectralDamperHpf() अचर { फर्काऊ _hpf * _hpf; }
	भिन्न getSpectralDamperLpf() अचर { फर्काऊ _lpf.getState(); }
	शून्य setCompressionGainMin(भिन्न gain_min) { _compression_gain_min = gain_min; }

गुप्त:
	भिन्न _compression_gain{1.f};
	भिन्न _compression_gain_min{0.3f};

	भिन्न _alpha_hpf{0.f};
	भिन्न _hpf{0.f};

	भिन्न आगत_prev{0.f};
	तार्किक आगत_initialized{असत्य};

	AlphaFilter<भिन्न> _lpf;

	स्थिर constexpr भिन्न _kSpectralDamperGain{200.f}; // tuned based on flight test data to obtain a quick response
	स्थिर constexpr भिन्न _kLeakageGain{0.1f}; // 1/time_constant, slow enough to not interfere with the controller

};

वर्ग GainCompression3d
{
सार्वजनिक:
	GainCompression3d();
	~GainCompression3d() = स्वभाव;

	शून्य reset();
	शून्य update(अचर matrix::Vector3f &आगत, भिन्न dt);
	अचर matrix::Vector3f &getGains() अचर { फर्काऊ _gains; };

	शून्य setEnabled(तार्किक enabled) { _enabled = enabled; }
	शून्य setCompressionGainMin(भिन्न gain_min);

गुप्त:
	// uORB publications
	uORB::Publication<gain_compression_s> _gain_compression_pub{ORB_ID(gain_compression)};

	GainCompression _compression_gains[3];
	matrix::Vector3f _gains{1.f, 1.f, 1.f};

	तार्किक _enabled{असत्य};

	hrt_abstime _time_last_publication{0};

	स्थिर constexpr भिन्न _kLpfCutoffFrequency{5.f}; // Just above the control bandwidth of most UAVs
	स्थिर constexpr भिन्न _kHpfCutoffFrequency{2.f * _kLpfCutoffFrequency}; // 1 Octave above LPF cutoff, as recommended by the reference paper
};
