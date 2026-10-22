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

#ਹਦਾਇਤ once

// PX4 includes
#ਅਨ੍ਤਰ੍ਭਾਵ <drivers/drv_hrt.h>

// Libraries
#ਅਨ੍ਤਰ੍ਭਾਵ <math.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <lib/mathlib/math/filter/AlphaFilter.hpp>
#ਅਨ੍ਤਰ੍ਭਾਵ <matrix/matrix/math.hpp>

// uORB includes
#ਅਨ੍ਤਰ੍ਭਾਵ <uORB/Publication.hpp>
#ਅਨ੍ਤਰ੍ਭਾਵ <uORB/topics/gain_compression.h>

ਵਰਗ GainCompression
{
ਜਨਤਕ:
	ਖਾਲੀ reset()
	{
		_compression_gain = 1.f;
		_hpf = 0.f;
		_lpf.reset(0.f);
		ਨਿਵੇਸ਼_initialized = ਝੂਠ;
	}
	ਦਸ਼ਮਲਵ update(ਦਸ਼ਮਲਵ ਨਿਵੇਸ਼, ਦਸ਼ਮਲਵ dt);

	ਖਾਲੀ ਸੈੱਟ_Lpf_Cutoff_Frequency(ਦਸ਼ਮਲਵ sample_freq, ਦਸ਼ਮਲਵ cutoff)
	{
		_lpf.ਸੈੱਟ_Cutoff_Freq(sample_freq, cutoff);
	}
	ਖਾਲੀ ਸੈੱਟ_Hpf_Cutoff_Frequency(ਦਸ਼ਮਲਵ sample_freq, ਦਸ਼ਮਲਵ cutoff) { _alpha_hpf = sample_freq / (sample_freq + 2.f * M_PI_F * cutoff); }

	ਦਸ਼ਮਲਵ getSpectralDamperHpf() ਅਚਲ { ਵਾਪਸ _hpf * _hpf; }
	ਦਸ਼ਮਲਵ getSpectralDamperLpf() ਅਚਲ { ਵਾਪਸ _lpf.getState(); }
	ਖਾਲੀ ਸੈੱਟ_Compression_Gain_Min(ਦਸ਼ਮਲਵ gain_min) { _compression_gain_min = gain_min; }

ਨਿਜੀ:
	ਦਸ਼ਮਲਵ _compression_gain{1.f};
	ਦਸ਼ਮਲਵ _compression_gain_min{0.3f};

	ਦਸ਼ਮਲਵ _alpha_hpf{0.f};
	ਦਸ਼ਮਲਵ _hpf{0.f};

	ਦਸ਼ਮਲਵ ਨਿਵੇਸ਼_prev{0.f};
	ਤਾਰਕਿਕ ਨਿਵੇਸ਼_initialized{ਝੂਠ};

	AlphaFilter<ਦਸ਼ਮਲਵ> _lpf;

	ਸਥਿਰ constexpr ਦਸ਼ਮਲਵ _kSpectralDamperGain{200.f}; // tuned based on flight test data to obtain a quick response
	ਸਥਿਰ constexpr ਦਸ਼ਮਲਵ _kLeakageGain{0.1f}; // 1/time_constant, slow enough to not interfere with the controller

};

ਵਰਗ GainCompression3d
{
ਜਨਤਕ:
	GainCompression3d();
	~GainCompression3d() = ਮੂਲ;

	ਖਾਲੀ reset();
	ਖਾਲੀ update(ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector3f &ਨਿਵੇਸ਼, ਦਸ਼ਮਲਵ dt);
	ਅਚਲ ਮੈਟ੍ਰਿਕਸ::Vector3f &getGains() ਅਚਲ { ਵਾਪਸ _gains; };

	ਖਾਲੀ ਸੈੱਟ_Enabled(ਤਾਰਕਿਕ enabled) { _enabled = enabled; }
	ਖਾਲੀ ਸੈੱਟ_Compression_Gain_Min(ਦਸ਼ਮਲਵ gain_min);

ਨਿਜੀ:
	// uORB publications
	uORB::Publication<gain_compression_s> _gain_compression_pub{ORB_ID(gain_compression)};

	GainCompression _compression_gains[3];
	ਮੈਟ੍ਰਿਕਸ::Vector3f _gains{1.f, 1.f, 1.f};

	ਤਾਰਕਿਕ _enabled{ਝੂਠ};

	hrt_abstime ਸਮਾਂ_ਆਖਰੀ_publication{0};

	ਸਥਿਰ constexpr ਦਸ਼ਮਲਵ _kLpfCutoffFrequency{5.f}; // Just above the control bandwidth of most UAVs
	ਸਥਿਰ constexpr ਦਸ਼ਮਲਵ _kHpfCutoffFrequency{2.f * _kLpfCutoffFrequency}; // 1 Octave above LPF cutoff, as recommended by the reference paper
};
