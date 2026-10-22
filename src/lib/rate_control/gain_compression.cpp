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

#अन्तर्भाव "gain_compression.hpp"

using matrix::Vector3f;
using नामाकाश time_literals;

GainCompression3d::GainCompression3d()
{
	_gain_compression_pub.advertise();
}

शून्य GainCompression3d::reset()
{
	लागि (अचिन्हित i = 0; i < 3; i++) {
		_compression_gains[i].reset();
	}

	_gains.setOne();
}

शून्य GainCompression3d::setCompressionGainMin(अचर भिन्न gain_min)
{
	लागि (अचिन्हित i = 0; i < 3; i++) {
		_compression_gains[i].setCompressionGainMin(gain_min);
	}
}

शून्य GainCompression3d::update(अचर Vector3f &आगत, अचर भिन्न dt)
{
	यदि (!_enabled) {
		reset();
		फर्काऊ;
	}

	Vector3f hpf;
	Vector3f lpf;
	अचर भिन्न sample_freq = 1.f / math::constrain(dt, 0.125e-3f, 100e-3f); // supports rate loops up to 8kHz

	लागि (अचिन्हित i = 0; i < 3; i++) {
		_compression_gains[i].setLpfCutoffFrequency(sample_freq, _kLpfCutoffFrequency);
		_compression_gains[i].setHpfCutoffFrequency(sample_freq, _kHpfCutoffFrequency);

		_gains(i) = _compression_gains[i].update(आगत(i), dt);

		hpf(i) = _compression_gains[i].getSpectralDamperHpf();
		lpf(i) = _compression_gains[i].getSpectralDamperLpf();
	}

	अचर hrt_abstime now = hrt_absolute_time();

	यदि ((now - _time_last_publication) > 100_ms) {
		gain_compression_s सन्देश;
		सन्देश.timestamp = now;
		_gains.copyTo(सन्देश.compression_gains);
		hpf.copyTo(सन्देश.spectral_damper_hpf);
		lpf.copyTo(सन्देश.spectral_damper_out);
		_gain_compression_pub.publish(सन्देश);

		_time_last_publication = now;
	}
}

भिन्न GainCompression::update(अचर भिन्न आगत, अचर भिन्न dt)
{
	यदि (!PX4_ISFINITE(आगत)) {
		फर्काऊ _compression_gain;
	}

	यदि (!आगत_initialized) {
		// seed the high-pass filter to avoid detecting a step on the first sample after a reset
		आगत_prev = आगत;
		आगत_initialized = सत्य;
	}

	_hpf = _alpha_hpf * _hpf + _alpha_hpf * (आगत - आगत_prev);
	_lpf.update(_hpf * _hpf);

	आगत_prev = आगत;

	अचर भिन्न ka = fmaxf(_compression_gain - _compression_gain_min, 0.f);
	अचर भिन्न spectral_damping = -_kSpectralDamperGain * ka * _lpf.getState();

	अचर भिन्न leakage = _kLeakageGain * (1.f - _compression_gain);

	अचर भिन्न ka_dot = spectral_damping + leakage;
	_compression_gain = math::constrain(_compression_gain + ka_dot * dt, _compression_gain_min, 1.f);

	फर्काऊ _compression_gain;
}
