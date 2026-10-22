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

#அந்தர்பாவ "gain_compression.hpp"

using matrix::Vector3f;
using நாமாகாஶ time_literals;

GainCompression3d::GainCompression3d()
{
	_gain_compression_pub.advertise();
}

வெறுமை GainCompression3d::reset()
{
	க்கு (unsigned i = 0; i < 3; i++) {
		_compression_gains[i].reset();
	}

	_gains.setOne();
}

வெறுமை GainCompression3d::setCompressionGainMin(மாறிலி தசம gain_min)
{
	க்கு (unsigned i = 0; i < 3; i++) {
		_compression_gains[i].setCompressionGainMin(gain_min);
	}
}

வெறுமை GainCompression3d::update(மாறிலி Vector3f &உள்ளீடு, மாறிலி தசம dt)
{
	எனில் (!_enabled) {
		reset();
		திரும்பு;
	}

	Vector3f hpf;
	Vector3f lpf;
	மாறிலி தசம sample_freq = 1.f / math::constrain(dt, 0.125e-3f, 100e-3f); // supports rate loops up to 8kHz

	க்கு (unsigned i = 0; i < 3; i++) {
		_compression_gains[i].setLpfCutoffFrequency(sample_freq, _kLpfCutoffFrequency);
		_compression_gains[i].setHpfCutoffFrequency(sample_freq, _kHpfCutoffFrequency);

		_gains(i) = _compression_gains[i].update(உள்ளீடு(i), dt);

		hpf(i) = _compression_gains[i].getSpectralDamperHpf();
		lpf(i) = _compression_gains[i].getSpectralDamperLpf();
	}

	மாறிலி hrt_abstime now = hrt_absolute_time();

	எனில் ((now - _time_last_publication) > 100_ms) {
		gain_compression_s செய்தி;
		செய்தி.timestamp = now;
		_gains.copyTo(செய்தி.compression_gains);
		hpf.copyTo(செய்தி.spectral_damper_hpf);
		lpf.copyTo(செய்தி.spectral_damper_out);
		_gain_compression_pub.publish(செய்தி);

		_time_last_publication = now;
	}
}

தசம GainCompression::update(மாறிலி தசம உள்ளீடு, மாறிலி தசம dt)
{
	எனில் (!PX4_ISFINITE(உள்ளீடு)) {
		திரும்பு _compression_gain;
	}

	எனில் (!உள்ளீடு_initialized) {
		// seed the high-pass filter to avoid detecting a step on the first sample after a reset
		உள்ளீடு_prev = உள்ளீடு;
		உள்ளீடு_initialized = உண்மை;
	}

	_hpf = _alpha_hpf * _hpf + _alpha_hpf * (உள்ளீடு - உள்ளீடு_prev);
	_lpf.update(_hpf * _hpf);

	உள்ளீடு_prev = உள்ளீடு;

	மாறிலி தசம ka = fmaxf(_compression_gain - _compression_gain_min, 0.f);
	மாறிலி தசம spectral_damping = -_kSpectralDamperGain * ka * _lpf.getState();

	மாறிலி தசம leakage = _kLeakageGain * (1.f - _compression_gain);

	மாறிலி தசம ka_dot = spectral_damping + leakage;
	_compression_gain = math::constrain(_compression_gain + ka_dot * dt, _compression_gain_min, 1.f);

	திரும்பு _compression_gain;
}
