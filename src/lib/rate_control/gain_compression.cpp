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

#অন্তর্ভাৱ "gain_compression.hpp"

using matrix::Vector3f;
using নামাকাশ time_literals;

GainCompression3d::GainCompression3d()
{
	_gain_compression_pub.advertise();
}

শূন্য GainCompression3d::reset()
{
	বাবে (unsigned i = 0; i < 3; i++) {
		_compression_gains[i].reset();
	}

	_gains.setOne();
}

শূন্য GainCompression3d::setCompressionGainMin(ধ্রুৱ ভিন্ন gain_min)
{
	বাবে (unsigned i = 0; i < 3; i++) {
		_compression_gains[i].setCompressionGainMin(gain_min);
	}
}

শূন্য GainCompression3d::update(ধ্রুৱ Vector3f &প্ৰবেশ, ধ্রুৱ ভিন্ন dt)
{
	যদি (!_enabled) {
		reset();
		ঘূৰাই;
	}

	Vector3f hpf;
	Vector3f lpf;
	ধ্রুৱ ভিন্ন sample_freq = 1.f / math::constrain(dt, 0.125e-3f, 100e-3f); // supports rate loops up to 8kHz

	বাবে (unsigned i = 0; i < 3; i++) {
		_compression_gains[i].setLpfCutoffFrequency(sample_freq, _kLpfCutoffFrequency);
		_compression_gains[i].setHpfCutoffFrequency(sample_freq, _kHpfCutoffFrequency);

		_gains(i) = _compression_gains[i].update(প্ৰবেশ(i), dt);

		hpf(i) = _compression_gains[i].getSpectralDamperHpf();
		lpf(i) = _compression_gains[i].getSpectralDamperLpf();
	}

	ধ্রুৱ hrt_abstime now = hrt_absolute_time();

	যদি ((now - _time_last_publication) > 100_ms) {
		gain_compression_s বাৰ্তা;
		বাৰ্তা.timestamp = now;
		_gains.copyTo(বাৰ্তা.compression_gains);
		hpf.copyTo(বাৰ্তা.spectral_damper_hpf);
		lpf.copyTo(বাৰ্তা.spectral_damper_out);
		_gain_compression_pub.publish(বাৰ্তা);

		_time_last_publication = now;
	}
}

ভিন্ন GainCompression::update(ধ্রুৱ ভিন্ন প্ৰবেশ, ধ্রুৱ ভিন্ন dt)
{
	যদি (!PX4_ISFINITE(প্ৰবেশ)) {
		ঘূৰাই _compression_gain;
	}

	যদি (!প্ৰবেশ_initialized) {
		// seed the high-pass filter to avoid detecting a step on the first sample after a reset
		প্ৰবেশ_prev = প্ৰবেশ;
		প্ৰবেশ_initialized = সঁচা;
	}

	_hpf = _alpha_hpf * _hpf + _alpha_hpf * (প্ৰবেশ - প্ৰবেশ_prev);
	_lpf.update(_hpf * _hpf);

	প্ৰবেশ_prev = প্ৰবেশ;

	ধ্রুৱ ভিন্ন ka = fmaxf(_compression_gain - _compression_gain_min, 0.f);
	ধ্রুৱ ভিন্ন spectral_damping = -_kSpectralDamperGain * ka * _lpf.getState();

	ধ্রুৱ ভিন্ন leakage = _kLeakageGain * (1.f - _compression_gain);

	ধ্রুৱ ভিন্ন ka_dot = spectral_damping + leakage;
	_compression_gain = math::constrain(_compression_gain + ka_dot * dt, _compression_gain_min, 1.f);

	ঘূৰাই _compression_gain;
}
