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

#ଅନ୍ତର୍ଭାଵ "gain_compression.hpp"

using matrix::Vector3f;
using ନାମାକାଶ time_literals;

GainCompression3d::GainCompression3d()
{
	_gain_compression_pub.advertise();
}

ଶୂନ୍ୟ GainCompression3d::reset()
{
	ପାଇଁ (unsigned i = 0; i < 3; i++) {
		_compression_gains[i].reset();
	}

	_gains.setOne();
}

ଶୂନ୍ୟ GainCompression3d::setCompressionGainMin(ଅଚଳ ଦଶମିକ gain_min)
{
	ପାଇଁ (unsigned i = 0; i < 3; i++) {
		_compression_gains[i].setCompressionGainMin(gain_min);
	}
}

ଶୂନ୍ୟ GainCompression3d::update(ଅଚଳ Vector3f &ନିବେଶ, ଅଚଳ ଦଶମିକ dt)
{
	ଯଦି (!_enabled) {
		reset();
		ଫେରନ୍ତୁ;
	}

	Vector3f hpf;
	Vector3f lpf;
	ଅଚଳ ଦଶମିକ sample_freq = 1.f / math::constrain(dt, 0.125e-3f, 100e-3f); // supports rate loops up to 8kHz

	ପାଇଁ (unsigned i = 0; i < 3; i++) {
		_compression_gains[i].setLpfCutoffFrequency(sample_freq, _kLpfCutoffFrequency);
		_compression_gains[i].setHpfCutoffFrequency(sample_freq, _kHpfCutoffFrequency);

		_gains(i) = _compression_gains[i].update(ନିବେଶ(i), dt);

		hpf(i) = _compression_gains[i].getSpectralDamperHpf();
		lpf(i) = _compression_gains[i].getSpectralDamperLpf();
	}

	ଅଚଳ hrt_abstime now = hrt_absolute_time();

	ଯଦି ((now - _time_last_publication) > 100_ms) {
		gain_compression_s ସନ୍ଦେଶ;
		ସନ୍ଦେଶ.timestamp = now;
		_gains.copyTo(ସନ୍ଦେଶ.compression_gains);
		hpf.copyTo(ସନ୍ଦେଶ.spectral_damper_hpf);
		lpf.copyTo(ସନ୍ଦେଶ.spectral_damper_out);
		_gain_compression_pub.publish(ସନ୍ଦେଶ);

		_time_last_publication = now;
	}
}

ଦଶମିକ GainCompression::update(ଅଚଳ ଦଶମିକ ନିବେଶ, ଅଚଳ ଦଶମିକ dt)
{
	ଯଦି (!PX4_ISFINITE(ନିବେଶ)) {
		ଫେରନ୍ତୁ _compression_gain;
	}

	ଯଦି (!ନିବେଶ_initialized) {
		// seed the high-pass filter to avoid detecting a step on the first sample after a reset
		ନିବେଶ_prev = ନିବେଶ;
		ନିବେଶ_initialized = ସତ୍ୟ;
	}

	_hpf = _alpha_hpf * _hpf + _alpha_hpf * (ନିବେଶ - ନିବେଶ_prev);
	_lpf.update(_hpf * _hpf);

	ନିବେଶ_prev = ନିବେଶ;

	ଅଚଳ ଦଶମିକ ka = fmaxf(_compression_gain - _compression_gain_min, 0.f);
	ଅଚଳ ଦଶମିକ spectral_damping = -_kSpectralDamperGain * ka * _lpf.getState();

	ଅଚଳ ଦଶମିକ leakage = _kLeakageGain * (1.f - _compression_gain);

	ଅଚଳ ଦଶମିକ ka_dot = spectral_damping + leakage;
	_compression_gain = math::constrain(_compression_gain + ka_dot * dt, _compression_gain_min, 1.f);

	ଫେରନ୍ତୁ _compression_gain;
}
