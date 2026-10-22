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

#ଅନ୍ତର୍ଭାଵ <gtest/gtest.h>
#ଅନ୍ତର୍ଭାଵ <matrix/matrix/math.hpp>

#ଅନ୍ତର୍ଭାଵ "system_identification.hpp"
#ଅନ୍ତର୍ଭାଵ "test_data.h"

using ନାମାକାଶ matrix;

ଶ୍ରେଣୀ SystemIdentificationTest : ସର୍ବସାଧାରଣ ::testing::Test
{
ସର୍ବସାଧାରଣ:
	SystemIdentificationTest() {};
	ଦଶମିକ apply(ଦଶମିକ sample);
	ଶୂନ୍ୟ setCoefficients(ଦଶମିକ a1, ଦଶମିକ a2, ଦଶମିକ b0, ଦଶମିକ b1, ଦଶମିକ b2)
	{
		_a1 = a1;
		_a2 = a2;
		_b0 = b0;
		_b1 = b1;
		_b2 = b2;
	}

ବ୍ୟକ୍ତିଗତ:
	ଦଶମିକ _a1{};
	ଦଶମିକ _a2{};
	ଦଶମିକ _b0{};
	ଦଶମିକ _b1{};
	ଦଶମିକ _b2{};
	ଦଶମିକ _delay_element_1{};
	ଦଶମିକ _delay_element_2{};
};

ଦଶମିକ SystemIdentificationTest::apply(ଦଶମିକ sample)
{
	// Direct Form II implementation
	ଅଚଳ ଦଶମିକ delay_element_0{sample - _delay_element_1 *_a1 - _delay_element_2 * _a2};
	ଅଚଳ ଦଶମିକ output{delay_element_0 *_b0 + _delay_element_1 *_b1 + _delay_element_2 * _b2};

	_delay_element_2 = _delay_element_1;
	_delay_element_1 = delay_element_0;

	ଫେରନ୍ତୁ output;
}

TEST_F(SystemIdentificationTest, basicTest)
{
	constexpr ଦଶମିକ fs = 800.f;

	SystemIdentification _sys_id;
	_sys_id.setHpfCutoffFrequency(fs, 0.05f);
	_sys_id.setLpfCutoffFrequency(fs, 30.f);
	_sys_id.setForgettingFactor(80.f, 1.f / fs);

	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < 5; i++) {
		// Fill the buffers with zeros
		_sys_id.update(0.f, 0.f);
	}

	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < 10; i += 2) {
		_sys_id.update(ଦଶମିକ(i), ଦଶମିକ(i + 1));
	}

	ଅଚଳ Vector<ଦଶମିକ, 5> coefficients = _sys_id.getCoefficients();
	ଦଶମିକ data_check[] = {-3.96f, 1.30f, -2.28f, -0.33f, 0.369f};
	ଅଚଳ Vector<ଦଶମିକ, 5> coefficients_check(data_check);
	ଦଶମିକ eps = 1e-2;
	EXPECT_TRUE((coefficients - coefficients_check).abs().max() < eps);
	coefficients.print();
	_sys_id.getVariances().print();
}

TEST_F(SystemIdentificationTest, resetTest)
{
	constexpr ଦଶମିକ fs = 800.f;

	SystemIdentification _sys_id;
	_sys_id.setHpfCutoffFrequency(fs, 0.05f);
	_sys_id.setLpfCutoffFrequency(fs, 30.f);
	_sys_id.setForgettingFactor(80.f, 1.f / fs);

	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < 10; i += 2) {
		_sys_id.update(ଦଶମିକ(i), ଦଶମିକ(i + 1));
	}

	ଅଚଳ Vector<ଦଶମିକ, 5> coefficients = _sys_id.getCoefficients();

	// WHEN: resetting
	_sys_id.reset();

	// THEN: the variances and coefficients should be properly reset
	EXPECT_TRUE(_sys_id.getCoefficients().abs().max() < 1e-8f);
	EXPECT_TRUE(_sys_id.getVariances().min() > 9e3f);

	// AND WHEN: running the same sequence of inputs-outputs

	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < 10; i += 2) {
		_sys_id.update(ଦଶମିକ(i), ଦଶମିକ(i + 1));
	}

	// THEN: the result should be exactly the same
	EXPECT_TRUE((coefficients - _sys_id.getCoefficients()).abs().max() < 1e-8f);
	coefficients.print();
	_sys_id.getVariances().print();
}

TEST_F(SystemIdentificationTest, simulatedModelTest)
{
	constexpr ଦଶମିକ fs = 200.f;
	ଅଚଳ ଦଶମିକ gyro_lpf_cutoff = 30.f;

	SystemIdentification _sys_id;
	_sys_id.setHpfCutoffFrequency(fs, 0.05f);
	_sys_id.setLpfCutoffFrequency(fs, gyro_lpf_cutoff);
	_sys_id.setForgettingFactor(60.f, 1.f / fs);

	// Simulated model with integrator
	ଅଚଳ ଦଶମିକ a1 = -1.77f;
	ଅଚଳ ଦଶମିକ a2 = 0.77f;
	ଅଚଳ ଦଶମିକ b0 = 0.3812f;
	ଅଚଳ ଦଶମିକ b1 = -0.25f;
	ଅଚଳ ଦଶମିକ b2 = 0.2f;
	setCoefficients(a1, a2, b0, b1, b2);
	ଅଚଳ ଦଶମିକ u_bias = -0.1f; // constant control offset
	ଅଚଳ ଦଶମିକ y_bias = 0.2f; // measurement bias

	ଅଚଳ ଦଶମିକ dt = 1.f / fs;
	ଅଚଳ ଦଶମିକ duration = 2.f;
	ଦଶମିକ u = 0.f;
	ଦଶମିକ y = 0.f;

	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < static_cast<ପୂର୍ଣ୍ଣସଂଖ୍ୟା>(duration / dt); i++) {

		// Generate square input signal
		ଯଦି (_sys_id.areFiltersInitialized()
		    && (i % 30 == 0)) {
			ଯଦି (u > 0.f) {
				u = -1.f;

			} ଅନ୍ୟଥା {
				u = 1.f;
			}
		}

		_sys_id.update(u + u_bias, y + y_bias); // apply new input and previous output

		y = apply(u);
#ଯଦି 0
		ଅଚଳ ଦଶମିକ t = i * dt;
		printf("%.6f, %.6f, %.6f\n", (ଦ୍ୱିଗୁଣ)t, (ଦ୍ୱିଗୁଣ)u, (ଦ୍ୱିଗୁଣ)y);
#endif
	}

	ଅଚଳ Vector<ଦଶମିକ, 5> coefficients = _sys_id.getCoefficients();
	ଦଶମିକ data_check[] = {a1, a2, b0, b1, b2};
	ଅଚଳ Vector<ଦଶମିକ, 5> coefficients_check(data_check);
	ଦଶମିକ eps = 1e-3;
	EXPECT_TRUE((coefficients - coefficients_check).abs().max() < eps);
	coefficients.print();
	_sys_id.getVariances().print();
}

TEST_F(SystemIdentificationTest, realDataTest)
{
	ସ୍ଥିର constexpr ପୂର୍ଣ୍ଣସଂଖ୍ୟା n_samples_dt_avg = 100;
	ସ୍ଥିର constexpr ପୂର୍ଣ୍ଣସଂଖ୍ୟା n_samples_test_data = ଆକାର(u_data) / ଆକାର(u_data[0]);
	ଦଶମିକ dt_sum = 0.f;

	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 1; i <= n_samples_dt_avg; i++) {
		dt_sum += t_data[i] - t_data[i - 1];
	}

	ଅଚଳ ଦଶମିକ dt = dt_sum / n_samples_dt_avg;
	ଅଚଳ ଦଶମିକ fs = 1.f / dt;

	printf("dt = %.6f fs = %.3f\n", (ଦ୍ୱିଗୁଣ)dt, (ଦ୍ୱିଗୁଣ)(1.f / dt));

	ଅଚଳ ଦଶମିକ gyro_lpf_cutoff = 30.f;

	SystemIdentification _sys_id;
	_sys_id.setHpfCutoffFrequency(fs, 0.05f);
	_sys_id.setLpfCutoffFrequency(fs, gyro_lpf_cutoff);
	_sys_id.setForgettingFactor(60.f, dt);

	ଦଶମିକ u = 0.f;
	ଦଶମିକ y_prev = 0.f;

	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < n_samples_test_data; i++) {
		u = u_data[i];
		_sys_id.update(u, y_prev); // apply new input and previous output

		y_prev = y_data[i];

#ଯଦି 0
		printf("%.6f, %.6f, %.6f\n", (ଦ୍ୱିଗୁଣ)t_data[i], (ଦ୍ୱିଗୁଣ)u, (ଦ୍ୱିଗୁଣ)y);
#endif
	}

	ଅଚଳ Vector<ଦଶମିକ, 5> coefficients = _sys_id.getCoefficients();
	ଦଶମିକ data_check[] = {-1.916f, 0.916, -0.017f, -0.001f, 0.036f};
	ଅଚଳ Vector<ଦଶମିକ, 5> coefficients_check(data_check);
	ଦଶମିକ eps = 1e-3;
	EXPECT_TRUE((coefficients - coefficients_check).abs().max() < eps);
	coefficients.print();
	_sys_id.getVariances().print();
}
