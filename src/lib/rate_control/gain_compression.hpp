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
#ಅನ್ತರ್ಭಾವ <drivers/drv_hrt.h>

// Libraries
#ಅನ್ತರ್ಭಾವ <math.h>
#ಅನ್ತರ್ಭಾವ <lib/mathlib/math/filter/AlphaFilter.hpp>
#ಅನ್ತರ್ಭಾವ <matrix/matrix/math.hpp>

// uORB includes
#ಅನ್ತರ್ಭಾವ <uORB/Publication.hpp>
#ಅನ್ತರ್ಭಾವ <uORB/topics/gain_compression.h>

ವರ್ಗ GainCompression
{
ಸಾರ್ವಜನಿಕ:
	ಶೂನ್ಯ reset()
	{
		_compression_gain = 1.f;
		_hpf = 0.f;
		_lpf.reset(0.f);
		ಒಳಹರಿವು_initialized = ಅಸತ್ಯ;
	}
	ಭಿನ್ನ update(ಭಿನ್ನ ಒಳಹರಿವು, ಭಿನ್ನ dt);

	ಶೂನ್ಯ setLpfCutoffFrequency(ಭಿನ್ನ sample_freq, ಭಿನ್ನ cutoff)
	{
		_lpf.setCutoffFreq(sample_freq, cutoff);
	}
	ಶೂನ್ಯ setHpfCutoffFrequency(ಭಿನ್ನ sample_freq, ಭಿನ್ನ cutoff) { _alpha_hpf = sample_freq / (sample_freq + 2.f * M_PI_F * cutoff); }

	ಭಿನ್ನ getSpectralDamperHpf() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ _hpf * _hpf; }
	ಭಿನ್ನ getSpectralDamperLpf() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ _lpf.getState(); }
	ಶೂನ್ಯ setCompressionGainMin(ಭಿನ್ನ gain_min) { _compression_gain_min = gain_min; }

ಗುಪ್ತ:
	ಭಿನ್ನ _compression_gain{1.f};
	ಭಿನ್ನ _compression_gain_min{0.3f};

	ಭಿನ್ನ _alpha_hpf{0.f};
	ಭಿನ್ನ _hpf{0.f};

	ಭಿನ್ನ ಒಳಹರಿವು_prev{0.f};
	ತಾರ್ಕಿಕ ಒಳಹರಿವು_initialized{ಅಸತ್ಯ};

	AlphaFilter<ಭಿನ್ನ> _lpf;

	ಸ್ಥಿರ constexpr ಭಿನ್ನ _kSpectralDamperGain{200.f}; // tuned based on flight test data to obtain a quick response
	ಸ್ಥಿರ constexpr ಭಿನ್ನ _kLeakageGain{0.1f}; // 1/time_constant, slow enough to not interfere with the controller

};

ವರ್ಗ GainCompression3d
{
ಸಾರ್ವಜನಿಕ:
	GainCompression3d();
	~GainCompression3d() = ಸ್ವಭಾವ;

	ಶೂನ್ಯ reset();
	ಶೂನ್ಯ update(ಧ್ರುವ matrix::Vector3f &ಒಳಹರಿವು, ಭಿನ್ನ dt);
	ಧ್ರುವ matrix::Vector3f &getGains() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ _gains; };

	ಶೂನ್ಯ setEnabled(ತಾರ್ಕಿಕ enabled) { _enabled = enabled; }
	ಶೂನ್ಯ setCompressionGainMin(ಭಿನ್ನ gain_min);

ಗುಪ್ತ:
	// uORB publications
	uORB::Publication<gain_compression_s> _gain_compression_pub{ORB_ID(gain_compression)};

	GainCompression _compression_gains[3];
	matrix::Vector3f _gains{1.f, 1.f, 1.f};

	ತಾರ್ಕಿಕ _enabled{ಅಸತ್ಯ};

	hrt_abstime _time_last_publication{0};

	ಸ್ಥಿರ constexpr ಭಿನ್ನ _kLpfCutoffFrequency{5.f}; // Just above the control bandwidth of most UAVs
	ಸ್ಥಿರ constexpr ಭಿನ್ನ _kHpfCutoffFrequency{2.f * _kLpfCutoffFrequency}; // 1 Octave above LPF cutoff, as recommended by the reference paper
};
