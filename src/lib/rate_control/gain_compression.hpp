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

#సూచన once

// PX4 includes
#అన్తర్భావ <drivers/drv_hrt.h>

// Libraries
#అన్తర్భావ <math.h>
#అన్తర్భావ <lib/mathlib/math/filter/AlphaFilter.hpp>
#అన్తర్భావ <matrix/matrix/math.hpp>

// uORB includes
#అన్తర్భావ <uORB/Publication.hpp>
#అన్తర్భావ <uORB/topics/gain_compression.h>

తరగతి GainCompression
{
బహిరంగ:
	శూన్య reset()
	{
		_compression_gain = 1.f;
		_hpf = 0.f;
		_lpf.reset(0.f);
		ఆదానం_initialized = అబద్ధం;
	}
	దశాంశం update(దశాంశం ఆదానం, దశాంశం dt);

	శూన్య సమితి_Lpf_Cutoff_Frequency(దశాంశం sample_freq, దశాంశం cutoff)
	{
		_lpf.సమితి_Cutoff_Freq(sample_freq, cutoff);
	}
	శూన్య సమితి_Hpf_Cutoff_Frequency(దశాంశం sample_freq, దశాంశం cutoff) { _alpha_hpf = sample_freq / (sample_freq + 2.f * M_PI_F * cutoff); }

	దశాంశం getSpectralDamperHpf() స్థిరరాశి { తిరిగి _hpf * _hpf; }
	దశాంశం getSpectralDamperLpf() స్థిరరాశి { తిరిగి _lpf.getState(); }
	శూన్య సమితి_Compression_Gain_Min(దశాంశం gain_min) { _compression_gain_min = gain_min; }

అంతరంగ:
	దశాంశం _compression_gain{1.f};
	దశాంశం _compression_gain_min{0.3f};

	దశాంశం _alpha_hpf{0.f};
	దశాంశం _hpf{0.f};

	దశాంశం ఆదానం_prev{0.f};
	తార్కిక ఆదానం_initialized{అబద్ధం};

	AlphaFilter<దశాంశం> _lpf;

	స్థిర constexpr దశాంశం _kSpectralDamperGain{200.f}; // tuned based on flight test data to obtain a quick response
	స్థిర constexpr దశాంశం _kLeakageGain{0.1f}; // 1/time_constant, slow enough to not interfere with the controller

};

తరగతి GainCompression3d
{
బహిరంగ:
	GainCompression3d();
	~GainCompression3d() = అప్రమేయ;

	శూన్య reset();
	శూన్య update(స్థిరరాశి మాతృక::Vector3f &ఆదానం, దశాంశం dt);
	స్థిరరాశి మాతృక::Vector3f &getGains() స్థిరరాశి { తిరిగి _gains; };

	శూన్య సమితి_Enabled(తార్కిక enabled) { _enabled = enabled; }
	శూన్య సమితి_Compression_Gain_Min(దశాంశం gain_min);

అంతరంగ:
	// uORB publications
	uORB::Publication<gain_compression_s> _gain_compression_pub{ORB_ID(gain_compression)};

	GainCompression _compression_gains[3];
	మాతృక::Vector3f _gains{1.f, 1.f, 1.f};

	తార్కిక _enabled{అబద్ధం};

	hrt_abstime సమయం_చివరి_publication{0};

	స్థిర constexpr దశాంశం _kLpfCutoffFrequency{5.f}; // Just above the control bandwidth of most UAVs
	స్థిర constexpr దశాంశం _kHpfCutoffFrequency{2.f * _kLpfCutoffFrequency}; // 1 Octave above LPF cutoff, as recommended by the reference paper
};
