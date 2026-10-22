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

#ہدایت once

// PX4 includes
#شامل <drivers/drv_hrt.h>

// Libraries
#شامل <math.h>
#شامل <lib/mathlib/math/filter/AlphaFilter.hpp>
#شامل <matrix/matrix/math.hpp>

// uORB includes
#شامل <uORB/Publication.hpp>
#شامل <uORB/topics/gain_compression.h>

جماعت GainCompression
{
عوامی:
	خالی reset()
	{
		_compression_gain = 1.f;
		_hpf = 0.f;
		_lpf.reset(0.f);
		داخلہ_initialized = جھوٹ;
	}
	اعشاری update(اعشاری داخلہ, اعشاری dt);

	خالی سیٹ_Lpf_Cutoff_Frequency(اعشاری sample_freq, اعشاری cutoff)
	{
		_lpf.سیٹ_Cutoff_Freq(sample_freq, cutoff);
	}
	خالی سیٹ_Hpf_Cutoff_Frequency(اعشاری sample_freq, اعشاری cutoff) { _alpha_hpf = sample_freq / (sample_freq + 2.f * M_PI_F * cutoff); }

	اعشاری getSpectralDamperHpf() ثابت { واپس _hpf * _hpf; }
	اعشاری getSpectralDamperLpf() ثابت { واپس _lpf.getState(); }
	خالی سیٹ_Compression_Gain_Min(اعشاری gain_min) { _compression_gain_min = gain_min; }

نجی:
	اعشاری _compression_gain{1.f};
	اعشاری _compression_gain_min{0.3f};

	اعشاری _alpha_hpf{0.f};
	اعشاری _hpf{0.f};

	اعشاری داخلہ_پچھلا{0.f};
	منطقی داخلہ_initialized{جھوٹ};

	AlphaFilter<اعشاری> _lpf;

	ساکن constexpr اعشاری _kSpectralDamperGain{200.f}; // tuned based on flight test data to obtain a quick response
	ساکن constexpr اعشاری _kLeakageGain{0.1f}; // 1/time_constant, slow enough to not interfere with the controller

};

جماعت GainCompression3d
{
عوامی:
	GainCompression3d();
	~GainCompression3d() = طےشدہ;

	خالی reset();
	خالی update(ثابت میٹرکس::Vector3f &داخلہ, اعشاری dt);
	ثابت میٹرکس::Vector3f &getGains() ثابت { واپس _gains; };

	خالی سیٹ_Enabled(منطقی enabled) { _enabled = enabled; }
	خالی سیٹ_Compression_Gain_Min(اعشاری gain_min);

نجی:
	// uORB publications
	uORB::Publication<gain_compression_s> _gain_compression_pub{ORB_ID(gain_compression)};

	GainCompression _compression_gains[3];
	میٹرکس::Vector3f _gains{1.f, 1.f, 1.f};

	منطقی _enabled{جھوٹ};

	hrt_abstime وقت_آخری_publication{0};

	ساکن constexpr اعشاری _kLpfCutoffFrequency{5.f}; // Just above the control bandwidth of most UAVs
	ساکن constexpr اعشاری _kHpfCutoffFrequency{2.f * _kLpfCutoffFrequency}; // 1 Octave above LPF cutoff, as recommended by the reference paper
};
