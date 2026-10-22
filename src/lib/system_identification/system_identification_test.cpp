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

#അന്തര്ഭാവ <gtest/gtest.h>
#അന്തര്ഭാവ <matrix/matrix/math.hpp>

#അന്തര്ഭാവ "system_identification.hpp"
#അന്തര്ഭാവ "test_data.h"

using നാമാകാശ matrix;

വർഗ്ഗം SystemIdentificationTest : പൊതു ::testing::Test
{
പൊതു:
	SystemIdentificationTest() {};
	ദശാംശം apply(ദശാംശം sample);
	ശൂന്യം setCoefficients(ദശാംശം a1, ദശാംശം a2, ദശാംശം b0, ദശാംശം b1, ദശാംശം b2)
	{
		_a1 = a1;
		_a2 = a2;
		_b0 = b0;
		_b1 = b1;
		_b2 = b2;
	}

സ്വകാര്യ:
	ദശാംശം _a1{};
	ദശാംശം _a2{};
	ദശാംശം _b0{};
	ദശാംശം _b1{};
	ദശാംശം _b2{};
	ദശാംശം _delay_element_1{};
	ദശാംശം _delay_element_2{};
};

ദശാംശം SystemIdentificationTest::apply(ദശാംശം sample)
{
	// Direct Form II implementation
	സ്ഥിരം ദശാംശം delay_element_0{sample - _delay_element_1 *_a1 - _delay_element_2 * _a2};
	സ്ഥിരം ദശാംശം output{delay_element_0 *_b0 + _delay_element_1 *_b1 + _delay_element_2 * _b2};

	_delay_element_2 = _delay_element_1;
	_delay_element_1 = delay_element_0;

	തിരികെ output;
}

TEST_F(SystemIdentificationTest, basicTest)
{
	constexpr ദശാംശം fs = 800.f;

	SystemIdentification _sys_id;
	_sys_id.setHpfCutoffFrequency(fs, 0.05f);
	_sys_id.setLpfCutoffFrequency(fs, 30.f);
	_sys_id.setForgettingFactor(80.f, 1.f / fs);

	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < 5; i++) {
		// Fill the buffers with zeros
		_sys_id.update(0.f, 0.f);
	}

	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < 10; i += 2) {
		_sys_id.update(ദശാംശം(i), ദശാംശം(i + 1));
	}

	സ്ഥിരം Vector<ദശാംശം, 5> coefficients = _sys_id.getCoefficients();
	ദശാംശം data_check[] = {-3.96f, 1.30f, -2.28f, -0.33f, 0.369f};
	സ്ഥിരം Vector<ദശാംശം, 5> coefficients_check(data_check);
	ദശാംശം eps = 1e-2;
	EXPECT_TRUE((coefficients - coefficients_check).abs().max() < eps);
	coefficients.print();
	_sys_id.getVariances().print();
}

TEST_F(SystemIdentificationTest, resetTest)
{
	constexpr ദശാംശം fs = 800.f;

	SystemIdentification _sys_id;
	_sys_id.setHpfCutoffFrequency(fs, 0.05f);
	_sys_id.setLpfCutoffFrequency(fs, 30.f);
	_sys_id.setForgettingFactor(80.f, 1.f / fs);

	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < 10; i += 2) {
		_sys_id.update(ദശാംശം(i), ദശാംശം(i + 1));
	}

	സ്ഥിരം Vector<ദശാംശം, 5> coefficients = _sys_id.getCoefficients();

	// WHEN: resetting
	_sys_id.reset();

	// THEN: the variances and coefficients should be properly reset
	EXPECT_TRUE(_sys_id.getCoefficients().abs().max() < 1e-8f);
	EXPECT_TRUE(_sys_id.getVariances().min() > 9e3f);

	// AND WHEN: running the same sequence of inputs-outputs

	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < 10; i += 2) {
		_sys_id.update(ദശാംശം(i), ദശാംശം(i + 1));
	}

	// THEN: the result should be exactly the same
	EXPECT_TRUE((coefficients - _sys_id.getCoefficients()).abs().max() < 1e-8f);
	coefficients.print();
	_sys_id.getVariances().print();
}

TEST_F(SystemIdentificationTest, simulatedModelTest)
{
	constexpr ദശാംശം fs = 200.f;
	സ്ഥിരം ദശാംശം gyro_lpf_cutoff = 30.f;

	SystemIdentification _sys_id;
	_sys_id.setHpfCutoffFrequency(fs, 0.05f);
	_sys_id.setLpfCutoffFrequency(fs, gyro_lpf_cutoff);
	_sys_id.setForgettingFactor(60.f, 1.f / fs);

	// Simulated model with integrator
	സ്ഥിരം ദശാംശം a1 = -1.77f;
	സ്ഥിരം ദശാംശം a2 = 0.77f;
	സ്ഥിരം ദശാംശം b0 = 0.3812f;
	സ്ഥിരം ദശാംശം b1 = -0.25f;
	സ്ഥിരം ദശാംശം b2 = 0.2f;
	setCoefficients(a1, a2, b0, b1, b2);
	സ്ഥിരം ദശാംശം u_bias = -0.1f; // constant control offset
	സ്ഥിരം ദശാംശം y_bias = 0.2f; // measurement bias

	സ്ഥിരം ദശാംശം dt = 1.f / fs;
	സ്ഥിരം ദശാംശം duration = 2.f;
	ദശാംശം u = 0.f;
	ദശാംശം y = 0.f;

	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < static_cast<പൂർണ്ണസംഖ്യ>(duration / dt); i++) {

		// Generate square input signal
		എങ്കിൽ (_sys_id.areFiltersInitialized()
		    && (i % 30 == 0)) {
			എങ്കിൽ (u > 0.f) {
				u = -1.f;

			} അല്ലെങ്കിൽ {
				u = 1.f;
			}
		}

		_sys_id.update(u + u_bias, y + y_bias); // apply new input and previous output

		y = apply(u);
#എങ്കിൽ 0
		സ്ഥിരം ദശാംശം t = i * dt;
		printf("%.6f, %.6f, %.6f\n", (ഇരട്ട)t, (ഇരട്ട)u, (ഇരട്ട)y);
#endif
	}

	സ്ഥിരം Vector<ദശാംശം, 5> coefficients = _sys_id.getCoefficients();
	ദശാംശം data_check[] = {a1, a2, b0, b1, b2};
	സ്ഥിരം Vector<ദശാംശം, 5> coefficients_check(data_check);
	ദശാംശം eps = 1e-3;
	EXPECT_TRUE((coefficients - coefficients_check).abs().max() < eps);
	coefficients.print();
	_sys_id.getVariances().print();
}

TEST_F(SystemIdentificationTest, realDataTest)
{
	സ്ഥിതം constexpr പൂർണ്ണസംഖ്യ n_samples_dt_avg = 100;
	സ്ഥിതം constexpr പൂർണ്ണസംഖ്യ n_samples_test_data = വലുപ്പം(u_data) / വലുപ്പം(u_data[0]);
	ദശാംശം dt_sum = 0.f;

	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 1; i <= n_samples_dt_avg; i++) {
		dt_sum += t_data[i] - t_data[i - 1];
	}

	സ്ഥിരം ദശാംശം dt = dt_sum / n_samples_dt_avg;
	സ്ഥിരം ദശാംശം fs = 1.f / dt;

	printf("dt = %.6f fs = %.3f\n", (ഇരട്ട)dt, (ഇരട്ട)(1.f / dt));

	സ്ഥിരം ദശാംശം gyro_lpf_cutoff = 30.f;

	SystemIdentification _sys_id;
	_sys_id.setHpfCutoffFrequency(fs, 0.05f);
	_sys_id.setLpfCutoffFrequency(fs, gyro_lpf_cutoff);
	_sys_id.setForgettingFactor(60.f, dt);

	ദശാംശം u = 0.f;
	ദശാംശം y_prev = 0.f;

	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < n_samples_test_data; i++) {
		u = u_data[i];
		_sys_id.update(u, y_prev); // apply new input and previous output

		y_prev = y_data[i];

#എങ്കിൽ 0
		printf("%.6f, %.6f, %.6f\n", (ഇരട്ട)t_data[i], (ഇരട്ട)u, (ഇരട്ട)y);
#endif
	}

	സ്ഥിരം Vector<ദശാംശം, 5> coefficients = _sys_id.getCoefficients();
	ദശാംശം data_check[] = {-1.916f, 0.916, -0.017f, -0.001f, 0.036f};
	സ്ഥിരം Vector<ദശാംശം, 5> coefficients_check(data_check);
	ദശാംശം eps = 1e-3;
	EXPECT_TRUE((coefficients - coefficients_check).abs().max() < eps);
	coefficients.print();
	_sys_id.getVariances().print();
}
