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

using ನಾಮಾಕಾಶ matrix;

ವರ್ಗ SystemIdentificationTest : ಸಾರ್ವಜನಿಕ ::testing::Test
{
ಸಾರ್ವಜನಿಕ:
	SystemIdentificationTest() {};
	ಭಿನ್ನ apply(ಭಿನ್ನ sample);
	ಶೂನ್ಯ setCoefficients(ಭಿನ್ನ a1, ಭಿನ್ನ a2, ಭಿನ್ನ b0, ಭಿನ್ನ b1, ಭಿನ್ನ b2)
	{
		_a1 = a1;
		_a2 = a2;
		_b0 = b0;
		_b1 = b1;
		_b2 = b2;
	}

ಗುಪ್ತ:
	ಭಿನ್ನ _a1{};
	ಭಿನ್ನ _a2{};
	ಭಿನ್ನ _b0{};
	ಭಿನ್ನ _b1{};
	ಭಿನ್ನ _b2{};
	ಭಿನ್ನ _delay_element_1{};
	ಭಿನ್ನ _delay_element_2{};
};

ಭಿನ್ನ SystemIdentificationTest::apply(ಭಿನ್ನ sample)
{
	// Direct Form II implementation
	ಧ್ರುವ ಭಿನ್ನ delay_element_0{sample - _delay_element_1 *_a1 - _delay_element_2 * _a2};
	ಧ್ರುವ ಭಿನ್ನ output{delay_element_0 *_b0 + _delay_element_1 *_b1 + _delay_element_2 * _b2};

	_delay_element_2 = _delay_element_1;
	_delay_element_1 = delay_element_0;

	ಪ್ರತ್ಯಾಗಮ output;
}

TEST_F(SystemIdentificationTest, basicTest)
{
	constexpr ಭಿನ್ನ fs = 800.f;

	SystemIdentification _sys_id;
	_sys_id.setHpfCutoffFrequency(fs, 0.05f);
	_sys_id.setLpfCutoffFrequency(fs, 30.f);
	_sys_id.setForgettingFactor(80.f, 1.f / fs);

	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < 5; i++) {
		// Fill the buffers with zeros
		_sys_id.update(0.f, 0.f);
	}

	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < 10; i += 2) {
		_sys_id.update(ಭಿನ್ನ(i), ಭಿನ್ನ(i + 1));
	}

	ಧ್ರುವ Vector<ಭಿನ್ನ, 5> coefficients = _sys_id.getCoefficients();
	ಭಿನ್ನ data_check[] = {-3.96f, 1.30f, -2.28f, -0.33f, 0.369f};
	ಧ್ರುವ Vector<ಭಿನ್ನ, 5> coefficients_check(data_check);
	ಭಿನ್ನ eps = 1e-2;
	EXPECT_TRUE((coefficients - coefficients_check).abs().max() < eps);
	coefficients.print();
	_sys_id.getVariances().print();
}

TEST_F(SystemIdentificationTest, resetTest)
{
	constexpr ಭಿನ್ನ fs = 800.f;

	SystemIdentification _sys_id;
	_sys_id.setHpfCutoffFrequency(fs, 0.05f);
	_sys_id.setLpfCutoffFrequency(fs, 30.f);
	_sys_id.setForgettingFactor(80.f, 1.f / fs);

	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < 10; i += 2) {
		_sys_id.update(ಭಿನ್ನ(i), ಭಿನ್ನ(i + 1));
	}

	ಧ್ರುವ Vector<ಭಿನ್ನ, 5> coefficients = _sys_id.getCoefficients();

	// WHEN: resetting
	_sys_id.reset();

	// THEN: the variances and coefficients should be properly reset
	EXPECT_TRUE(_sys_id.getCoefficients().abs().max() < 1e-8f);
	EXPECT_TRUE(_sys_id.getVariances().min() > 9e3f);

	// AND WHEN: running the same sequence of inputs-outputs

	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < 10; i += 2) {
		_sys_id.update(ಭಿನ್ನ(i), ಭಿನ್ನ(i + 1));
	}

	// THEN: the result should be exactly the same
	EXPECT_TRUE((coefficients - _sys_id.getCoefficients()).abs().max() < 1e-8f);
	coefficients.print();
	_sys_id.getVariances().print();
}

TEST_F(SystemIdentificationTest, simulatedModelTest)
{
	constexpr ಭಿನ್ನ fs = 200.f;
	ಧ್ರುವ ಭಿನ್ನ gyro_lpf_cutoff = 30.f;

	SystemIdentification _sys_id;
	_sys_id.setHpfCutoffFrequency(fs, 0.05f);
	_sys_id.setLpfCutoffFrequency(fs, gyro_lpf_cutoff);
	_sys_id.setForgettingFactor(60.f, 1.f / fs);

	// Simulated model with integrator
	ಧ್ರುವ ಭಿನ್ನ a1 = -1.77f;
	ಧ್ರುವ ಭಿನ್ನ a2 = 0.77f;
	ಧ್ರುವ ಭಿನ್ನ b0 = 0.3812f;
	ಧ್ರುವ ಭಿನ್ನ b1 = -0.25f;
	ಧ್ರುವ ಭಿನ್ನ b2 = 0.2f;
	setCoefficients(a1, a2, b0, b1, b2);
	ಧ್ರುವ ಭಿನ್ನ u_bias = -0.1f; // constant control offset
	ಧ್ರುವ ಭಿನ್ನ y_bias = 0.2f; // measurement bias

	ಧ್ರುವ ಭಿನ್ನ dt = 1.f / fs;
	ಧ್ರುವ ಭಿನ್ನ duration = 2.f;
	ಭಿನ್ನ u = 0.f;
	ಭಿನ್ನ y = 0.f;

	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < static_cast<ಪೂರ್ಣಾಙ್ಕ>(duration / dt); i++) {

		// Generate square input signal
		ಯದಿ (_sys_id.areFiltersInitialized()
		    && (i % 30 == 0)) {
			ಯದಿ (u > 0.f) {
				u = -1.f;

			} ಅನ್ಯಥಾ {
				u = 1.f;
			}
		}

		_sys_id.update(u + u_bias, y + y_bias); // apply new input and previous output

		y = apply(u);
#ಯದಿ 0
		ಧ್ರುವ ಭಿನ್ನ t = i * dt;
		printf("%.6f, %.6f, %.6f\n", (ದ್ವಿಗುಣ)t, (ದ್ವಿಗುಣ)u, (ದ್ವಿಗುಣ)y);
#endif
	}

	ಧ್ರುವ Vector<ಭಿನ್ನ, 5> coefficients = _sys_id.getCoefficients();
	ಭಿನ್ನ data_check[] = {a1, a2, b0, b1, b2};
	ಧ್ರುವ Vector<ಭಿನ್ನ, 5> coefficients_check(data_check);
	ಭಿನ್ನ eps = 1e-3;
	EXPECT_TRUE((coefficients - coefficients_check).abs().max() < eps);
	coefficients.print();
	_sys_id.getVariances().print();
}

TEST_F(SystemIdentificationTest, realDataTest)
{
	ಸ್ಥಿರ constexpr ಪೂರ್ಣಾಙ್ಕ n_samples_dt_avg = 100;
	ಸ್ಥಿರ constexpr ಪೂರ್ಣಾಙ್ಕ n_samples_test_data = ಪರಿಮಾಣ(u_data) / ಪರಿಮಾಣ(u_data[0]);
	ಭಿನ್ನ dt_sum = 0.f;

	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 1; i <= n_samples_dt_avg; i++) {
		dt_sum += t_data[i] - t_data[i - 1];
	}

	ಧ್ರುವ ಭಿನ್ನ dt = dt_sum / n_samples_dt_avg;
	ಧ್ರುವ ಭಿನ್ನ fs = 1.f / dt;

	printf("dt = %.6f fs = %.3f\n", (ದ್ವಿಗುಣ)dt, (ದ್ವಿಗುಣ)(1.f / dt));

	ಧ್ರುವ ಭಿನ್ನ gyro_lpf_cutoff = 30.f;

	SystemIdentification _sys_id;
	_sys_id.setHpfCutoffFrequency(fs, 0.05f);
	_sys_id.setLpfCutoffFrequency(fs, gyro_lpf_cutoff);
	_sys_id.setForgettingFactor(60.f, dt);

	ಭಿನ್ನ u = 0.f;
	ಭಿನ್ನ y_prev = 0.f;

	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < n_samples_test_data; i++) {
		u = u_data[i];
		_sys_id.update(u, y_prev); // apply new input and previous output

		y_prev = y_data[i];

#ಯದಿ 0
		printf("%.6f, %.6f, %.6f\n", (ದ್ವಿಗುಣ)t_data[i], (ದ್ವಿಗುಣ)u, (ದ್ವಿಗುಣ)y);
#endif
	}

	ಧ್ರುವ Vector<ಭಿನ್ನ, 5> coefficients = _sys_id.getCoefficients();
	ಭಿನ್ನ data_check[] = {-1.916f, 0.916, -0.017f, -0.001f, 0.036f};
	ಧ್ರುವ Vector<ಭಿನ್ನ, 5> coefficients_check(data_check);
	ಭಿನ್ನ eps = 1e-3;
	EXPECT_TRUE((coefficients - coefficients_check).abs().max() < eps);
	coefficients.print();
	_sys_id.getVariances().print();
}
