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

#అన్తర్భావ "gain_compression.hpp"

ఉపయోగించి మాతృక::Vector3f;
ఉపయోగించి నామాకాశ సమయం_literals;

GainCompression3d::GainCompression3d()
{
	_gain_compression_pub.advertise();
}

శూన్య GainCompression3d::reset()
{
	కొరకు (గుర్తులేని i = 0; i < 3; i++) {
		_compression_gains[i].reset();
	}

	_gains.సమితి_One();
}

శూన్య GainCompression3d::సమితి_Compression_Gain_Min(స్థిరరాశి దశాంశం gain_min)
{
	కొరకు (గుర్తులేని i = 0; i < 3; i++) {
		_compression_gains[i].సమితి_Compression_Gain_Min(gain_min);
	}
}

శూన్య GainCompression3d::update(స్థిరరాశి Vector3f &ఆదానం, స్థిరరాశి దశాంశం dt)
{
	ఒకవేళ (!_enabled) {
		reset();
		తిరిగి;
	}

	Vector3f hpf;
	Vector3f lpf;
	స్థిరరాశి దశాంశం sample_freq = 1.f / math::constrain(dt, 0.125e-3f, 100e-3f); // supports rate loops up to 8kHz

	కొరకు (గుర్తులేని i = 0; i < 3; i++) {
		_compression_gains[i].సమితి_Lpf_Cutoff_Frequency(sample_freq, _kLpfCutoffFrequency);
		_compression_gains[i].సమితి_Hpf_Cutoff_Frequency(sample_freq, _kHpfCutoffFrequency);

		_gains(i) = _compression_gains[i].update(ఆదానం(i), dt);

		hpf(i) = _compression_gains[i].getSpectralDamperHpf();
		lpf(i) = _compression_gains[i].getSpectralDamperLpf();
	}

	స్థిరరాశి hrt_abstime now = hrt_absolute_సమయం();

	ఒకవేళ ((now - సమయం_చివరి_publication) > 100_ms) {
		gain_compression_s సందేశం_2;
		సందేశం_2.timestamp = now;
		_gains.copy_గమ్యం(సందేశం_2.compression_gains);
		hpf.copy_గమ్యం(సందేశం_2.spectral_damper_hpf);
		lpf.copy_గమ్యం(సందేశం_2.spectral_damper_బహిర్గతం);
		_gain_compression_pub.publish(సందేశం_2);

		సమయం_చివరి_publication = now;
	}
}

దశాంశం GainCompression::update(స్థిరరాశి దశాంశం ఆదానం, స్థిరరాశి దశాంశం dt)
{
	ఒకవేళ (!PX4_ISFINITE(ఆదానం)) {
		తిరిగి _compression_gain;
	}

	ఒకవేళ (!ఆదానం_initialized) {
		// seed the high-pass filter to avoid detecting a step on the first sample after a reset
		ఆదానం_prev = ఆదానం;
		ఆదానం_initialized = నిజం;
	}

	_hpf = _alpha_hpf * _hpf + _alpha_hpf * (ఆదానం - ఆదానం_prev);
	_lpf.update(_hpf * _hpf);

	ఆదానం_prev = ఆదానం;

	స్థిరరాశి దశాంశం ka = fmaxf(_compression_gain - _compression_gain_min, 0.f);
	స్థిరరాశి దశాంశం spectral_damping = -_kSpectralDamperGain * ka * _lpf.getState();

	స్థిరరాశి దశాంశం leakage = _kLeakageGain * (1.f - _compression_gain);

	స్థిరరాశి దశాంశం ka_dot = spectral_damping + leakage;
	_compression_gain = math::constrain(_compression_gain + ka_dot * dt, _compression_gain_min, 1.f);

	తిరిగి _compression_gain;
}
