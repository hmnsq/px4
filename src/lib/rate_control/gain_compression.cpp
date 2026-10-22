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

#ضمن "gain_compression.hpp"

استعمال ميٽرڪس::Vector3f;
استعمال نطاق وقت_literals;

GainCompression3d::GainCompression3d()
{
	_gain_compression_pub.advertise();
}

فراغ GainCompression3d::reset()
{
	لكل (بنانشان i = 0; i < 3; i++) {
		_compression_gains[i].reset();
	}

	_gains.سيٽ_One();
}

فراغ GainCompression3d::سيٽ_Compression_Gain_Min(ثابت عائم gain_min)
{
	لكل (بنانشان i = 0; i < 3; i++) {
		_compression_gains[i].سيٽ_Compression_Gain_Min(gain_min);
	}
}

فراغ GainCompression3d::update(ثابت Vector3f &داخلا, ثابت عائم dt)
{
	إذا (!_enabled) {
		reset();
		أرجع;
	}

	Vector3f hpf;
	Vector3f lpf;
	ثابت عائم sample_freq = 1.f / math::constrain(dt, 0.125e-3f, 100e-3f); // supports rate loops up to 8kHz

	لكل (بنانشان i = 0; i < 3; i++) {
		_compression_gains[i].سيٽ_Lpf_Cutoff_Frequency(sample_freq, _kLpfCutoffFrequency);
		_compression_gains[i].سيٽ_Hpf_Cutoff_Frequency(sample_freq, _kHpfCutoffFrequency);

		_gains(i) = _compression_gains[i].update(داخلا(i), dt);

		hpf(i) = _compression_gains[i].getSpectralDamperHpf();
		lpf(i) = _compression_gains[i].getSpectralDamperLpf();
	}

	ثابت hrt_abstime now = hrt_absolute_وقت();

	إذا ((now - وقت_آخري_publication) > 100_ms) {
		gain_compression_s نياپو;
		نياپو.timestamp = now;
		_gains.copy_منزل(نياپو.compression_gains);
		hpf.copy_منزل(نياپو.spectral_damper_hpf);
		lpf.copy_منزل(نياپو.spectral_damper_ٻاهريون);
		_gain_compression_pub.publish(نياپو);

		وقت_آخري_publication = now;
	}
}

عائم GainCompression::update(ثابت عائم داخلا, ثابت عائم dt)
{
	إذا (!PX4_ISFINITE(داخلا)) {
		أرجع _compression_gain;
	}

	إذا (!داخلا_initialized) {
		// seed the high-pass filter to avoid detecting a step on the first sample after a reset
		داخلا_prev = داخلا;
		داخلا_initialized = سچ;
	}

	_hpf = _alpha_hpf * _hpf + _alpha_hpf * (داخلا - داخلا_prev);
	_lpf.update(_hpf * _hpf);

	داخلا_prev = داخلا;

	ثابت عائم ka = fmaxf(_compression_gain - _compression_gain_min, 0.f);
	ثابت عائم spectral_damping = -_kSpectralDamperGain * ka * _lpf.getState();

	ثابت عائم leakage = _kLeakageGain * (1.f - _compression_gain);

	ثابت عائم ka_dot = spectral_damping + leakage;
	_compression_gain = math::constrain(_compression_gain + ka_dot * dt, _compression_gain_min, 1.f);

	أرجع _compression_gain;
}
