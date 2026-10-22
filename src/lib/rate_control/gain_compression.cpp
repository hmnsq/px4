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

#شامل "gain_compression.hpp"

استعمال میٹرکس::Vector3f;
استعمال نامخلا وقت_literals;

GainCompression3d::GainCompression3d()
{
	_gain_compression_pub.advertise();
}

خالی GainCompression3d::reset()
{
	برائے (غیرعلامتی i = 0; i < 3; i++) {
		_compression_gains[i].reset();
	}

	_gains.سیٹ_One();
}

خالی GainCompression3d::سیٹ_Compression_Gain_Min(ثابت اعشاری gain_min)
{
	برائے (غیرعلامتی i = 0; i < 3; i++) {
		_compression_gains[i].سیٹ_Compression_Gain_Min(gain_min);
	}
}

خالی GainCompression3d::update(ثابت Vector3f &داخلہ, ثابت اعشاری dt)
{
	اگر (!_enabled) {
		reset();
		واپس;
	}

	Vector3f hpf;
	Vector3f lpf;
	ثابت اعشاری sample_freq = 1.f / math::constrain(dt, 0.125e-3f, 100e-3f); // supports rate loops up to 8kHz

	برائے (غیرعلامتی i = 0; i < 3; i++) {
		_compression_gains[i].سیٹ_Lpf_Cutoff_Frequency(sample_freq, _kLpfCutoffFrequency);
		_compression_gains[i].سیٹ_Hpf_Cutoff_Frequency(sample_freq, _kHpfCutoffFrequency);

		_gains(i) = _compression_gains[i].update(داخلہ(i), dt);

		hpf(i) = _compression_gains[i].getSpectralDamperHpf();
		lpf(i) = _compression_gains[i].getSpectralDamperLpf();
	}

	ثابت hrt_abstime now = hrt_absolute_وقت();

	اگر ((now - وقت_آخری_publication) > 100_ms) {
		gain_compression_s پیغام_2;
		پیغام_2.timestamp = now;
		_gains.copy_مقصد(پیغام_2.compression_gains);
		hpf.copy_مقصد(پیغام_2.spectral_damper_hpf);
		lpf.copy_مقصد(پیغام_2.spectral_damper_برآمد);
		_gain_compression_pub.publish(پیغام_2);

		وقت_آخری_publication = now;
	}
}

اعشاری GainCompression::update(ثابت اعشاری داخلہ, ثابت اعشاری dt)
{
	اگر (!PX4_ISFINITE(داخلہ)) {
		واپس _compression_gain;
	}

	اگر (!داخلہ_initialized) {
		// seed the high-pass filter to avoid detecting a step on the first sample after a reset
		داخلہ_پچھلا = داخلہ;
		داخلہ_initialized = سچ;
	}

	_hpf = _alpha_hpf * _hpf + _alpha_hpf * (داخلہ - داخلہ_پچھلا);
	_lpf.update(_hpf * _hpf);

	داخلہ_پچھلا = داخلہ;

	ثابت اعشاری ka = fmaxf(_compression_gain - _compression_gain_min, 0.f);
	ثابت اعشاری spectral_damping = -_kSpectralDamperGain * ka * _lpf.getState();

	ثابت اعشاری leakage = _kLeakageGain * (1.f - _compression_gain);

	ثابت اعشاری ka_dot = spectral_damping + leakage;
	_compression_gain = math::constrain(_compression_gain + ka_dot * dt, _compression_gain_min, 1.f);

	واپس _compression_gain;
}
