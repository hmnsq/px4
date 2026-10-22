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

#അന്തര്ഭാവ "gain_compression.hpp"

using matrix::Vector3f;
using നാമാകാശ time_literals;

GainCompression3d::GainCompression3d()
{
	_gain_compression_pub.advertise();
}

ശൂന്യം GainCompression3d::reset()
{
	വേണ്ടി (unsigned i = 0; i < 3; i++) {
		_compression_gains[i].reset();
	}

	_gains.setOne();
}

ശൂന്യം GainCompression3d::setCompressionGainMin(സ്ഥിരം ദശാംശം gain_min)
{
	വേണ്ടി (unsigned i = 0; i < 3; i++) {
		_compression_gains[i].setCompressionGainMin(gain_min);
	}
}

ശൂന്യം GainCompression3d::update(സ്ഥിരം Vector3f &നിവേശം, സ്ഥിരം ദശാംശം dt)
{
	എങ്കിൽ (!_enabled) {
		reset();
		തിരികെ;
	}

	Vector3f hpf;
	Vector3f lpf;
	സ്ഥിരം ദശാംശം sample_freq = 1.f / math::constrain(dt, 0.125e-3f, 100e-3f); // supports rate loops up to 8kHz

	വേണ്ടി (unsigned i = 0; i < 3; i++) {
		_compression_gains[i].setLpfCutoffFrequency(sample_freq, _kLpfCutoffFrequency);
		_compression_gains[i].setHpfCutoffFrequency(sample_freq, _kHpfCutoffFrequency);

		_gains(i) = _compression_gains[i].update(നിവേശം(i), dt);

		hpf(i) = _compression_gains[i].getSpectralDamperHpf();
		lpf(i) = _compression_gains[i].getSpectralDamperLpf();
	}

	സ്ഥിരം hrt_abstime now = hrt_absolute_time();

	എങ്കിൽ ((now - _time_last_publication) > 100_ms) {
		gain_compression_s സന്ദേശം;
		സന്ദേശം.timestamp = now;
		_gains.copyTo(സന്ദേശം.compression_gains);
		hpf.copyTo(സന്ദേശം.spectral_damper_hpf);
		lpf.copyTo(സന്ദേശം.spectral_damper_out);
		_gain_compression_pub.publish(സന്ദേശം);

		_time_last_publication = now;
	}
}

ദശാംശം GainCompression::update(സ്ഥിരം ദശാംശം നിവേശം, സ്ഥിരം ദശാംശം dt)
{
	എങ്കിൽ (!PX4_ISFINITE(നിവേശം)) {
		തിരികെ _compression_gain;
	}

	എങ്കിൽ (!നിവേശം_initialized) {
		// seed the high-pass filter to avoid detecting a step on the first sample after a reset
		നിവേശം_prev = നിവേശം;
		നിവേശം_initialized = സത്യം;
	}

	_hpf = _alpha_hpf * _hpf + _alpha_hpf * (നിവേശം - നിവേശം_prev);
	_lpf.update(_hpf * _hpf);

	നിവേശം_prev = നിവേശം;

	സ്ഥിരം ദശാംശം ka = fmaxf(_compression_gain - _compression_gain_min, 0.f);
	സ്ഥിരം ദശാംശം spectral_damping = -_kSpectralDamperGain * ka * _lpf.getState();

	സ്ഥിരം ദശാംശം leakage = _kLeakageGain * (1.f - _compression_gain);

	സ്ഥിരം ദശാംശം ka_dot = spectral_damping + leakage;
	_compression_gain = math::constrain(_compression_gain + ka_dot * dt, _compression_gain_min, 1.f);

	തിരികെ _compression_gain;
}
