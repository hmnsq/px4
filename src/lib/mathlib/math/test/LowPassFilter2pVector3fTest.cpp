/****************************************************************************
 *
 *   Copyright (C) 2021 PX4 Development Team. All rights reserved.
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

/**
 * Test code for the 2nd order Butterworth low-pass filter
 * Run this test only using make tests TESTFILTER=LowPassFilter2pVector3f
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <gtest/gtest.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <matrix/matrix/math.hpp>
#ਅਨ੍ਤਰ੍ਭਾਵ <px4_platform_common/defines.h>

#ਅਨ੍ਤਰ੍ਭਾਵ <lib/mathlib/math/filter/LowPassFilter2p.hpp>

ਵਰਤੋਂ ਮੈਟ੍ਰਿਕਸ::Vector3f;

ਵਰਗ ਨੀਵਾਂ_Pass_Filter2p_Vector3f_Test : ਜਨਤਕ ::testing::Test
{
ਜਨਤਕ:
	ਖਾਲੀ ਲੜੀਵਾਰ_Simulated_Filter(ਅਚਲ Vector3f &signal_freq_hz, ਅਚਲ Vector3f &phase_delay_deg, ਅਚਲ Vector3f &gain_db);

	math::ਨੀਵਾਂ_Pass_Filter2p<Vector3f> _lpf{800.f, 30.f};

	ਅਚਲ ਦਸ਼ਮਲਵ _epsilon_near = 0.08f;
};

ਖਾਲੀ ਨੀਵਾਂ_Pass_Filter2p_Vector3f_Test::ਲੜੀਵਾਰ_Simulated_Filter(ਅਚਲ Vector3f &signal_freq_hz, ਅਚਲ Vector3f &phase_delay_deg,
		ਅਚਲ Vector3f &gain_db)
{
	ਅਚਲ Vector3f phase_delay = phase_delay_deg * M_PI_F / 180.f;
	ਅਚਲ Vector3f omega = 2.f * M_PI_F * signal_freq_hz;
	Vector3f gain;

	ਲਈ (ਪੂਰਨ i = 0; i < 3; i++) {
		gain(i) = powf(10.f, gain_db(i) / 20.f);
	}

	ਅਚਲ ਦਸ਼ਮਲਵ dt = 1.f / _lpf.get_sample_freq();
	ਅਚਲ ਪੂਰਨ n_ਕਦਮਾਂ = roundf(1.f / dt); // run for 1 second

	ਦਸ਼ਮਲਵ t = 0.f;

	ਲਈ (ਪੂਰਨ i = 0; i < n_ਕਦਮਾਂ; i++) {
		Vector3f ਨਿਵੇਸ਼{0.f, sinf(omega(1) * t), -sinf(omega(2) * t)};
		Vector3f output_expected{0.f,
					 gain(1) *sinf(omega(1) * t - phase_delay(1)),
					 -gain(2) *sinf(omega(2) * t - phase_delay(2))};
		Vector3f ਬਾਹਰਲਾ = _lpf.apply(ਨਿਵੇਸ਼);
		t = i * dt;

		// Let some time for the filter to settle
		ਜੇ (t > 0.3f) {
			EXPECT_EQ(ਬਾਹਰਲਾ(0), 0.f);
			EXPECT_NEAR(ਬਾਹਰਲਾ(1), output_expected(1), _epsilon_near);
			EXPECT_NEAR(ਬਾਹਰਲਾ(2), output_expected(2), _epsilon_near);
		}
	}
}

TEST_F(ਨੀਵਾਂ_Pass_Filter2p_Vector3f_Test, ਸੈੱਟ_Get)
{
	ਅਚਲ ਦਸ਼ਮਲਵ sample_freq = 1000.f;
	ਅਚਲ ਦਸ਼ਮਲਵ cutoff_freq = 80.f;

	_lpf.ਸੈੱਟ_cutoff_frequency(sample_freq, cutoff_freq);
	EXPECT_EQ(_lpf.get_sample_freq(), sample_freq);
	EXPECT_EQ(_lpf.get_cutoff_freq(), cutoff_freq);
}

TEST_F(ਨੀਵਾਂ_Pass_Filter2p_Vector3f_Test, simulate80HzCutoff)
{
	ਅਚਲ ਦਸ਼ਮਲਵ sample_freqs[4] = {400.f, 1000.f, 8000.f, 16000.f};
	ਅਚਲ ਦਸ਼ਮਲਵ cutoff_freq = 80.f;

	ਅਚਲ Vector3f signal_freq_hz{0.f, 10.f, 100.f};
	ਅਚਲ Vector3f phase_delay_deg = Vector3f{0.f, 10.4f, 108.5f}; // Given by simulation
	ਅਚਲ Vector3f gain_db{0.f, 0.f, -5.66f}; // given by simulation

	ਲਈ (ਦਸ਼ਮਲਵ sample_freq : sample_freqs) {
		_lpf.ਸੈੱਟ_cutoff_frequency(sample_freq, cutoff_freq);
		ਲੜੀਵਾਰ_Simulated_Filter(signal_freq_hz, phase_delay_deg, gain_db);
	}
}
