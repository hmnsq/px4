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
#شامل <drivers/drv_hrt.h>

// Libraries
#شامل <math.h>
#شامل <lib/mathlib/math/filter/AlphaFilter.hpp>
#شامل <matrix/matrix/math.hpp>

// uORB includes
#شامل <uORB/Publication.hpp>
#شامل <uORB/topics/gain_compression.h>

رده GainCompression
{
عمومی:
	تهی reset()
	{
		_compression_gain = 1.f;
		_hpf = 0.f;
		_lpf.reset(0.f);
		درون_داد_initialized = نادرست;
	}
	اعشاری update(اعشاری درون_داد, اعشاری dt);

	تهی setLpfCutoffFrequency(اعشاری sample_freq, اعشاری cutoff)
	{
		_lpf.setCutoffFreq(sample_freq, cutoff);
	}
	تهی setHpfCutoffFrequency(اعشاری sample_freq, اعشاری cutoff) { _alpha_hpf = sample_freq / (sample_freq + 2.f * M_PI_F * cutoff); }

	اعشاری getSpectralDamperHpf() ثابت { بازگشت _hpf * _hpf; }
	اعشاری getSpectralDamperLpf() ثابت { بازگشت _lpf.getState(); }
	تهی setCompressionGainMin(اعشاری gain_min) { _compression_gain_min = gain_min; }

خصوصی:
	اعشاری _compression_gain{1.f};
	اعشاری _compression_gain_min{0.3f};

	اعشاری _alpha_hpf{0.f};
	اعشاری _hpf{0.f};

	اعشاری درون_داد_prev{0.f};
	منطقی درون_داد_initialized{نادرست};

	AlphaFilter<اعشاری> _lpf;

	ایستا constexpr اعشاری _kSpectralDamperGain{200.f}; // tuned based on flight test data to obtain a quick response
	ایستا constexpr اعشاری _kLeakageGain{0.1f}; // 1/time_constant, slow enough to not interfere with the controller

};

رده GainCompression3d
{
عمومی:
	GainCompression3d();
	~GainCompression3d() = پیشفرض;

	تهی reset();
	تهی update(ثابت matrix::Vector3f &درون_داد, اعشاری dt);
	ثابت matrix::Vector3f &getGains() ثابت { بازگشت _gains; };

	تهی setEnabled(منطقی enabled) { _enabled = enabled; }
	تهی setCompressionGainMin(اعشاری gain_min);

خصوصی:
	// uORB publications
	uORB::Publication<gain_compression_s> _gain_compression_pub{ORB_ID(gain_compression)};

	GainCompression _compression_gains[3];
	matrix::Vector3f _gains{1.f, 1.f, 1.f};

	منطقی _enabled{نادرست};

	hrt_abstime _time_last_publication{0};

	ایستا constexpr اعشاری _kLpfCutoffFrequency{5.f}; // Just above the control bandwidth of most UAVs
	ایستا constexpr اعشاری _kHpfCutoffFrequency{2.f * _kLpfCutoffFrequency}; // 1 Octave above LPF cutoff, as recommended by the reference paper
};
