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

#અન્તર્ભાવ <gtest/gtest.h>
#અન્તર્ભાવ <matrix/matrix/math.hpp>

#અન્તર્ભાવ "system_identification.hpp"
#અન્તર્ભાવ "test_data.h"

using નામાકાશ matrix;

વર્ગ SystemIdentificationTest : જાહેર ::testing::Test
{
જાહેર:
	SystemIdentificationTest() {};
	દશાંશ apply(દશાંશ sample);
	શૂન્ય setCoefficients(દશાંશ a1, દશાંશ a2, દશાંશ b0, દશાંશ b1, દશાંશ b2)
	{
		_a1 = a1;
		_a2 = a2;
		_b0 = b0;
		_b1 = b1;
		_b2 = b2;
	}

ખાનગી:
	દશાંશ _a1{};
	દશાંશ _a2{};
	દશાંશ _b0{};
	દશાંશ _b1{};
	દશાંશ _b2{};
	દશાંશ _delay_element_1{};
	દશાંશ _delay_element_2{};
};

દશાંશ SystemIdentificationTest::apply(દશાંશ sample)
{
	// Direct Form II implementation
	અચલ દશાંશ delay_element_0{sample - _delay_element_1 *_a1 - _delay_element_2 * _a2};
	અચલ દશાંશ output{delay_element_0 *_b0 + _delay_element_1 *_b1 + _delay_element_2 * _b2};

	_delay_element_2 = _delay_element_1;
	_delay_element_1 = delay_element_0;

	પાછા output;
}

TEST_F(SystemIdentificationTest, basicTest)
{
	constexpr દશાંશ fs = 800.f;

	SystemIdentification _sys_id;
	_sys_id.setHpfCutoffFrequency(fs, 0.05f);
	_sys_id.setLpfCutoffFrequency(fs, 30.f);
	_sys_id.setForgettingFactor(80.f, 1.f / fs);

	માટે (પૂર્ણાંક i = 0; i < 5; i++) {
		// Fill the buffers with zeros
		_sys_id.update(0.f, 0.f);
	}

	માટે (પૂર્ણાંક i = 0; i < 10; i += 2) {
		_sys_id.update(દશાંશ(i), દશાંશ(i + 1));
	}

	અચલ Vector<દશાંશ, 5> coefficients = _sys_id.getCoefficients();
	દશાંશ data_check[] = {-3.96f, 1.30f, -2.28f, -0.33f, 0.369f};
	અચલ Vector<દશાંશ, 5> coefficients_check(data_check);
	દશાંશ eps = 1e-2;
	EXPECT_TRUE((coefficients - coefficients_check).abs().max() < eps);
	coefficients.print();
	_sys_id.getVariances().print();
}

TEST_F(SystemIdentificationTest, resetTest)
{
	constexpr દશાંશ fs = 800.f;

	SystemIdentification _sys_id;
	_sys_id.setHpfCutoffFrequency(fs, 0.05f);
	_sys_id.setLpfCutoffFrequency(fs, 30.f);
	_sys_id.setForgettingFactor(80.f, 1.f / fs);

	માટે (પૂર્ણાંક i = 0; i < 10; i += 2) {
		_sys_id.update(દશાંશ(i), દશાંશ(i + 1));
	}

	અચલ Vector<દશાંશ, 5> coefficients = _sys_id.getCoefficients();

	// WHEN: resetting
	_sys_id.reset();

	// THEN: the variances and coefficients should be properly reset
	EXPECT_TRUE(_sys_id.getCoefficients().abs().max() < 1e-8f);
	EXPECT_TRUE(_sys_id.getVariances().min() > 9e3f);

	// AND WHEN: running the same sequence of inputs-outputs

	માટે (પૂર્ણાંક i = 0; i < 10; i += 2) {
		_sys_id.update(દશાંશ(i), દશાંશ(i + 1));
	}

	// THEN: the result should be exactly the same
	EXPECT_TRUE((coefficients - _sys_id.getCoefficients()).abs().max() < 1e-8f);
	coefficients.print();
	_sys_id.getVariances().print();
}

TEST_F(SystemIdentificationTest, simulatedModelTest)
{
	constexpr દશાંશ fs = 200.f;
	અચલ દશાંશ gyro_lpf_cutoff = 30.f;

	SystemIdentification _sys_id;
	_sys_id.setHpfCutoffFrequency(fs, 0.05f);
	_sys_id.setLpfCutoffFrequency(fs, gyro_lpf_cutoff);
	_sys_id.setForgettingFactor(60.f, 1.f / fs);

	// Simulated model with integrator
	અચલ દશાંશ a1 = -1.77f;
	અચલ દશાંશ a2 = 0.77f;
	અચલ દશાંશ b0 = 0.3812f;
	અચલ દશાંશ b1 = -0.25f;
	અચલ દશાંશ b2 = 0.2f;
	setCoefficients(a1, a2, b0, b1, b2);
	અચલ દશાંશ u_bias = -0.1f; // constant control offset
	અચલ દશાંશ y_bias = 0.2f; // measurement bias

	અચલ દશાંશ dt = 1.f / fs;
	અચલ દશાંશ duration = 2.f;
	દશાંશ u = 0.f;
	દશાંશ y = 0.f;

	માટે (પૂર્ણાંક i = 0; i < static_cast<પૂર્ણાંક>(duration / dt); i++) {

		// Generate square input signal
		જો (_sys_id.areFiltersInitialized()
		    && (i % 30 == 0)) {
			જો (u > 0.f) {
				u = -1.f;

			} નહીંતર {
				u = 1.f;
			}
		}

		_sys_id.update(u + u_bias, y + y_bias); // apply new input and previous output

		y = apply(u);
#જો 0
		અચલ દશાંશ t = i * dt;
		printf("%.6f, %.6f, %.6f\n", (દ્વિગુણ)t, (દ્વિગુણ)u, (દ્વિગુણ)y);
#endif
	}

	અચલ Vector<દશાંશ, 5> coefficients = _sys_id.getCoefficients();
	દશાંશ data_check[] = {a1, a2, b0, b1, b2};
	અચલ Vector<દશાંશ, 5> coefficients_check(data_check);
	દશાંશ eps = 1e-3;
	EXPECT_TRUE((coefficients - coefficients_check).abs().max() < eps);
	coefficients.print();
	_sys_id.getVariances().print();
}

TEST_F(SystemIdentificationTest, realDataTest)
{
	સ્થિર constexpr પૂર્ણાંક n_samples_dt_avg = 100;
	સ્થિર constexpr પૂર્ણાંક n_samples_test_data = કદ(u_data) / કદ(u_data[0]);
	દશાંશ dt_sum = 0.f;

	માટે (પૂર્ણાંક i = 1; i <= n_samples_dt_avg; i++) {
		dt_sum += t_data[i] - t_data[i - 1];
	}

	અચલ દશાંશ dt = dt_sum / n_samples_dt_avg;
	અચલ દશાંશ fs = 1.f / dt;

	printf("dt = %.6f fs = %.3f\n", (દ્વિગુણ)dt, (દ્વિગુણ)(1.f / dt));

	અચલ દશાંશ gyro_lpf_cutoff = 30.f;

	SystemIdentification _sys_id;
	_sys_id.setHpfCutoffFrequency(fs, 0.05f);
	_sys_id.setLpfCutoffFrequency(fs, gyro_lpf_cutoff);
	_sys_id.setForgettingFactor(60.f, dt);

	દશાંશ u = 0.f;
	દશાંશ y_prev = 0.f;

	માટે (પૂર્ણાંક i = 0; i < n_samples_test_data; i++) {
		u = u_data[i];
		_sys_id.update(u, y_prev); // apply new input and previous output

		y_prev = y_data[i];

#જો 0
		printf("%.6f, %.6f, %.6f\n", (દ્વિગુણ)t_data[i], (દ્વિગુણ)u, (દ્વિગુણ)y);
#endif
	}

	અચલ Vector<દશાંશ, 5> coefficients = _sys_id.getCoefficients();
	દશાંશ data_check[] = {-1.916f, 0.916, -0.017f, -0.001f, 0.036f};
	અચલ Vector<દશાંશ, 5> coefficients_check(data_check);
	દશાંશ eps = 1e-3;
	EXPECT_TRUE((coefficients - coefficients_check).abs().max() < eps);
	coefficients.print();
	_sys_id.getVariances().print();
}
