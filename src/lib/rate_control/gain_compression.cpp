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

#ਅਨ੍ਤਰ੍ਭਾਵ "gain_compression.hpp"

ਵਰਤੋਂ ਮੈਟ੍ਰਿਕਸ::Vector3f;
ਵਰਤੋਂ ਨਾਮਾਕਾਸ਼ ਸਮਾਂ_literals;

GainCompression3d::GainCompression3d()
{
	_gain_compression_pub.advertise();
}

ਖਾਲੀ GainCompression3d::reset()
{
	ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ i = 0; i < 3; i++) {
		_compression_gains[i].reset();
	}

	_gains.ਸੈੱਟ_One();
}

ਖਾਲੀ GainCompression3d::ਸੈੱਟ_Compression_Gain_Min(ਅਚਲ ਦਸ਼ਮਲਵ gain_min)
{
	ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ i = 0; i < 3; i++) {
		_compression_gains[i].ਸੈੱਟ_Compression_Gain_Min(gain_min);
	}
}

ਖਾਲੀ GainCompression3d::update(ਅਚਲ Vector3f &ਨਿਵੇਸ਼, ਅਚਲ ਦਸ਼ਮਲਵ dt)
{
	ਜੇ (!_enabled) {
		reset();
		ਵਾਪਸ;
	}

	Vector3f hpf;
	Vector3f lpf;
	ਅਚਲ ਦਸ਼ਮਲਵ sample_freq = 1.f / math::constrain(dt, 0.125e-3f, 100e-3f); // supports rate loops up to 8kHz

	ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ i = 0; i < 3; i++) {
		_compression_gains[i].ਸੈੱਟ_Lpf_Cutoff_Frequency(sample_freq, _kLpfCutoffFrequency);
		_compression_gains[i].ਸੈੱਟ_Hpf_Cutoff_Frequency(sample_freq, _kHpfCutoffFrequency);

		_gains(i) = _compression_gains[i].update(ਨਿਵੇਸ਼(i), dt);

		hpf(i) = _compression_gains[i].getSpectralDamperHpf();
		lpf(i) = _compression_gains[i].getSpectralDamperLpf();
	}

	ਅਚਲ hrt_abstime now = hrt_absolute_ਸਮਾਂ();

	ਜੇ ((now - ਸਮਾਂ_ਆਖਰੀ_publication) > 100_ms) {
		gain_compression_s ਸੁਨੇਹਾ_2;
		ਸੁਨੇਹਾ_2.timestamp = now;
		_gains.copy_ਮੰਜ਼ਿਲ(ਸੁਨੇਹਾ_2.compression_gains);
		hpf.copy_ਮੰਜ਼ਿਲ(ਸੁਨੇਹਾ_2.spectral_damper_hpf);
		lpf.copy_ਮੰਜ਼ਿਲ(ਸੁਨੇਹਾ_2.spectral_damper_ਬਾਹਰਲਾ);
		_gain_compression_pub.publish(ਸੁਨੇਹਾ_2);

		ਸਮਾਂ_ਆਖਰੀ_publication = now;
	}
}

ਦਸ਼ਮਲਵ GainCompression::update(ਅਚਲ ਦਸ਼ਮਲਵ ਨਿਵੇਸ਼, ਅਚਲ ਦਸ਼ਮਲਵ dt)
{
	ਜੇ (!PX4_ISFINITE(ਨਿਵੇਸ਼)) {
		ਵਾਪਸ _compression_gain;
	}

	ਜੇ (!ਨਿਵੇਸ਼_initialized) {
		// seed the high-pass filter to avoid detecting a step on the first sample after a reset
		ਨਿਵੇਸ਼_prev = ਨਿਵੇਸ਼;
		ਨਿਵੇਸ਼_initialized = ਸੱਚ;
	}

	_hpf = _alpha_hpf * _hpf + _alpha_hpf * (ਨਿਵੇਸ਼ - ਨਿਵੇਸ਼_prev);
	_lpf.update(_hpf * _hpf);

	ਨਿਵੇਸ਼_prev = ਨਿਵੇਸ਼;

	ਅਚਲ ਦਸ਼ਮਲਵ ka = fmaxf(_compression_gain - _compression_gain_min, 0.f);
	ਅਚਲ ਦਸ਼ਮਲਵ spectral_damping = -_kSpectralDamperGain * ka * _lpf.getState();

	ਅਚਲ ਦਸ਼ਮਲਵ leakage = _kLeakageGain * (1.f - _compression_gain);

	ਅਚਲ ਦਸ਼ਮਲਵ ka_dot = spectral_damping + leakage;
	_compression_gain = math::constrain(_compression_gain + ka_dot * dt, _compression_gain_min, 1.f);

	ਵਾਪਸ _compression_gain;
}
