/****************************************************************************
 *
 *   Copyright (C) 2020-2021 PX4 Development Team. All rights reserved.
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
 * Test code for the SystemIdentification class
 * Run this test only using make tests TESTFILTER=system_identification
 */

#ಅನ್ತರ್ಭಾವ <gtest/gtest.h>
#ಅನ್ತರ್ಭಾವ <matrix/matrix/math.hpp>

#ಅನ್ತರ್ಭಾವ "system_identification.hpp"
#ಅನ್ತರ್ಭಾವ "test_data.h"

ಬಳಸಿ ನಾಮಾಕಾಶ ಮಾತೃಕೆ;

ವರ್ಗ SystemIdentificationTest : ಸಾರ್ವಜನಿಕ ::testing::Test
{
ಸಾರ್ವಜನಿಕ:
	SystemIdentificationTest() {};
	ದಶಮಾಂಶ apply(ದಶಮಾಂಶ sample);
	ಶೂನ್ಯ ಗಣ_Coefficients(ದಶಮಾಂಶ a1, ದಶಮಾಂಶ a2, ದಶಮಾಂಶ b0, ದಶಮಾಂಶ b1, ದಶಮಾಂಶ b2)
	{
		_a1 = a1;
		_a2 = a2;
		_b0 = b0;
		_b1 = b1;
		_b2 = b2;
	}

ಖಾಸಗಿ:
	ದಶಮಾಂಶ _a1{};
	ದಶಮಾಂಶ _a2{};
	ದಶಮಾಂಶ _b0{};
	ದಶಮಾಂಶ _b1{};
	ದಶಮಾಂಶ _b2{};
	ದಶಮಾಂಶ _delay_element_1{};
	ದಶಮಾಂಶ _delay_element_2{};
};

ದಶಮಾಂಶ SystemIdentificationTest::apply(ದಶಮಾಂಶ sample)
{
	// Direct Form II implementation
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ delay_element_0{sample - _delay_element_1 *_a1 - _delay_element_2 * _a2};
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ output{delay_element_0 *_b0 + _delay_element_1 *_b1 + _delay_element_2 * _b2};

	_delay_element_2 = _delay_element_1;
	_delay_element_1 = delay_element_0;

	ಹಿಂತಿರುಗಿ output;
}

TEST_F(SystemIdentificationTest, basicTest)
{
	constexpr ದಶಮಾಂಶ fs = 800.f;

	SystemIdentification _sys_id;
	_sys_id.ಗಣ_Hpf_Cutoff_Frequency(fs, 0.05f);
	_sys_id.ಗಣ_Lpf_Cutoff_Frequency(fs, 30.f);
	_sys_id.ಗಣ_Forgetting_ಗುಣಕ(80.f, 1.f / fs);

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < 5; i++) {
		// Fill the buffers with zeros
		_sys_id.update(0.f, 0.f);
	}

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < 10; i += 2) {
		_sys_id.update(ದಶಮಾಂಶ(i), ದಶಮಾಂಶ(i + 1));
	}

	ಸ್ಥಿರಾಂಕ ಸದಿಶ<ದಶಮಾಂಶ, 5> coefficients = _sys_id.getCoefficients();
	ದಶಮಾಂಶ ದತ್ತಾಂಶ_check[] = {-3.96f, 1.30f, -2.28f, -0.33f, 0.369f};
	ಸ್ಥಿರಾಂಕ ಸದಿಶ<ದಶಮಾಂಶ, 5> coefficients_check(ದತ್ತಾಂಶ_check);
	ದಶಮಾಂಶ eps = 1e-2;
	EXPECT_TRUE((coefficients - coefficients_check).ನಿರಪೇಕ್ಷ().max() < eps);
	coefficients.print();
	_sys_id.getVariances().print();
}

TEST_F(SystemIdentificationTest, resetTest)
{
	constexpr ದಶಮಾಂಶ fs = 800.f;

	SystemIdentification _sys_id;
	_sys_id.ಗಣ_Hpf_Cutoff_Frequency(fs, 0.05f);
	_sys_id.ಗಣ_Lpf_Cutoff_Frequency(fs, 30.f);
	_sys_id.ಗಣ_Forgetting_ಗುಣಕ(80.f, 1.f / fs);

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < 10; i += 2) {
		_sys_id.update(ದಶಮಾಂಶ(i), ದಶಮಾಂಶ(i + 1));
	}

	ಸ್ಥಿರಾಂಕ ಸದಿಶ<ದಶಮಾಂಶ, 5> coefficients = _sys_id.getCoefficients();

	// WHEN: resetting
	_sys_id.reset();

	// THEN: the variances and coefficients should be properly reset
	EXPECT_TRUE(_sys_id.getCoefficients().ನಿರಪೇಕ್ಷ().max() < 1e-8f);
	EXPECT_TRUE(_sys_id.getVariances().min() > 9e3f);

	// AND WHEN: running the same sequence of inputs-outputs

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < 10; i += 2) {
		_sys_id.update(ದಶಮಾಂಶ(i), ದಶಮಾಂಶ(i + 1));
	}

	// THEN: the result should be exactly the same
	EXPECT_TRUE((coefficients - _sys_id.getCoefficients()).ನಿರಪೇಕ್ಷ().max() < 1e-8f);
	coefficients.print();
	_sys_id.getVariances().print();
}

TEST_F(SystemIdentificationTest, simulatedModelTest)
{
	constexpr ದಶಮಾಂಶ fs = 200.f;
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ gyro_lpf_cutoff = 30.f;

	SystemIdentification _sys_id;
	_sys_id.ಗಣ_Hpf_Cutoff_Frequency(fs, 0.05f);
	_sys_id.ಗಣ_Lpf_Cutoff_Frequency(fs, gyro_lpf_cutoff);
	_sys_id.ಗಣ_Forgetting_ಗುಣಕ(60.f, 1.f / fs);

	// Simulated model with integrator
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ a1 = -1.77f;
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ a2 = 0.77f;
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ b0 = 0.3812f;
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ b1 = -0.25f;
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ b2 = 0.2f;
	ಗಣ_Coefficients(a1, a2, b0, b1, b2);
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ u_bias = -0.1f; // constant control offset
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ y_bias = 0.2f; // measurement bias

	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ dt = 1.f / fs;
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ duration = 2.f;
	ದಶಮಾಂಶ u = 0.f;
	ದಶಮಾಂಶ y = 0.f;

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < static_cast<ಪೂರ್ಣಾಂಕ>(duration / dt); i++) {

		// Generate square input signal
		ಆದರೆ (_sys_id.areFiltersInitialized()
		    && (i % 30 == 0)) {
			ಆದರೆ (u > 0.f) {
				u = -1.f;

			} ಇಲ್ಲದಿದ್ದರೆ {
				u = 1.f;
			}
		}

		_sys_id.update(u + u_bias, y + y_bias); // apply new input and previous output

		y = apply(u);
#ಆದರೆ 0
		ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ t = i * dt;
		ಬರೆ("%.6f, %.6f, %.6f\n", (ದ್ವಿಗುಣ)t, (ದ್ವಿಗುಣ)u, (ದ್ವಿಗುಣ)y);
#ಆದರೆ_ಅಂತ್ಯ
	}

	ಸ್ಥಿರಾಂಕ ಸದಿಶ<ದಶಮಾಂಶ, 5> coefficients = _sys_id.getCoefficients();
	ದಶಮಾಂಶ ದತ್ತಾಂಶ_check[] = {a1, a2, b0, b1, b2};
	ಸ್ಥಿರಾಂಕ ಸದಿಶ<ದಶಮಾಂಶ, 5> coefficients_check(ದತ್ತಾಂಶ_check);
	ದಶಮಾಂಶ eps = 1e-3;
	EXPECT_TRUE((coefficients - coefficients_check).ನಿರಪೇಕ್ಷ().max() < eps);
	coefficients.print();
	_sys_id.getVariances().print();
}

TEST_F(SystemIdentificationTest, real_ದತ್ತಾಂಶ_Test)
{
	ಸ್ಥಿರ constexpr ಪೂರ್ಣಾಂಕ n_samples_dt_avg = 100;
	ಸ್ಥಿರ constexpr ಪೂರ್ಣಾಂಕ n_samples_test_ದತ್ತಾಂಶ = ಗಾತ್ರ(u_ದತ್ತಾಂಶ) / ಗಾತ್ರ(u_ದತ್ತಾಂಶ[0]);
	ದಶಮಾಂಶ dt_ಮೊತ್ತ = 0.f;

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 1; i <= n_samples_dt_avg; i++) {
		dt_ಮೊತ್ತ += t_ದತ್ತಾಂಶ[i] - t_ದತ್ತಾಂಶ[i - 1];
	}

	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ dt = dt_ಮೊತ್ತ / n_samples_dt_avg;
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ fs = 1.f / dt;

	ಬರೆ("dt = %.6f fs = %.3f\n", (ದ್ವಿಗುಣ)dt, (ದ್ವಿಗುಣ)(1.f / dt));

	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ gyro_lpf_cutoff = 30.f;

	SystemIdentification _sys_id;
	_sys_id.ಗಣ_Hpf_Cutoff_Frequency(fs, 0.05f);
	_sys_id.ಗಣ_Lpf_Cutoff_Frequency(fs, gyro_lpf_cutoff);
	_sys_id.ಗಣ_Forgetting_ಗುಣಕ(60.f, dt);

	ದಶಮಾಂಶ u = 0.f;
	ದಶಮಾಂಶ y_prev = 0.f;

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < n_samples_test_ದತ್ತಾಂಶ; i++) {
		u = u_ದತ್ತಾಂಶ[i];
		_sys_id.update(u, y_prev); // apply new input and previous output

		y_prev = y_ದತ್ತಾಂಶ[i];

#ಆದರೆ 0
		ಬರೆ("%.6f, %.6f, %.6f\n", (ದ್ವಿಗುಣ)t_ದತ್ತಾಂಶ[i], (ದ್ವಿಗುಣ)u, (ದ್ವಿಗುಣ)y);
#ಆದರೆ_ಅಂತ್ಯ
	}

	ಸ್ಥಿರಾಂಕ ಸದಿಶ<ದಶಮಾಂಶ, 5> coefficients = _sys_id.getCoefficients();
	ದಶಮಾಂಶ ದತ್ತಾಂಶ_check[] = {-1.916f, 0.916, -0.017f, -0.001f, 0.036f};
	ಸ್ಥಿರಾಂಕ ಸದಿಶ<ದಶಮಾಂಶ, 5> coefficients_check(ದತ್ತಾಂಶ_check);
	ದಶಮಾಂಶ eps = 1e-3;
	EXPECT_TRUE((coefficients - coefficients_check).ನಿರಪೇಕ್ಷ().max() < eps);
	coefficients.print();
	_sys_id.getVariances().print();
}
