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

#अन्तर्भाव <gtest/gtest.h>
#अन्तर्भाव <matrix/matrix/math.hpp>

#अन्तर्भाव "system_identification.hpp"
#अन्तर्भाव "test_data.h"

using नामाकाश matrix;

वर्ग SystemIdentificationTest : सार्वजनिक ::testing::Test
{
सार्वजनिक:
	SystemIdentificationTest() {};
	दशांश apply(दशांश sample);
	शून्य setCoefficients(दशांश a1, दशांश a2, दशांश b0, दशांश b1, दशांश b2)
	{
		_a1 = a1;
		_a2 = a2;
		_b0 = b0;
		_b1 = b1;
		_b2 = b2;
	}

खाजगी:
	दशांश _a1{};
	दशांश _a2{};
	दशांश _b0{};
	दशांश _b1{};
	दशांश _b2{};
	दशांश _delay_element_1{};
	दशांश _delay_element_2{};
};

दशांश SystemIdentificationTest::apply(दशांश sample)
{
	// Direct Form II implementation
	स्थिरांक दशांश delay_element_0{sample - _delay_element_1 *_a1 - _delay_element_2 * _a2};
	स्थिरांक दशांश output{delay_element_0 *_b0 + _delay_element_1 *_b1 + _delay_element_2 * _b2};

	_delay_element_2 = _delay_element_1;
	_delay_element_1 = delay_element_0;

	परत output;
}

TEST_F(SystemIdentificationTest, basicTest)
{
	constexpr दशांश fs = 800.f;

	SystemIdentification _sys_id;
	_sys_id.setHpfCutoffFrequency(fs, 0.05f);
	_sys_id.setLpfCutoffFrequency(fs, 30.f);
	_sys_id.setForgettingFactor(80.f, 1.f / fs);

	साठी (पूर्णांक i = 0; i < 5; i++) {
		// Fill the buffers with zeros
		_sys_id.update(0.f, 0.f);
	}

	साठी (पूर्णांक i = 0; i < 10; i += 2) {
		_sys_id.update(दशांश(i), दशांश(i + 1));
	}

	स्थिरांक Vector<दशांश, 5> coefficients = _sys_id.getCoefficients();
	दशांश data_check[] = {-3.96f, 1.30f, -2.28f, -0.33f, 0.369f};
	स्थिरांक Vector<दशांश, 5> coefficients_check(data_check);
	दशांश eps = 1e-2;
	EXPECT_TRUE((coefficients - coefficients_check).असल().max() < eps);
	coefficients.print();
	_sys_id.getVariances().print();
}

TEST_F(SystemIdentificationTest, resetTest)
{
	constexpr दशांश fs = 800.f;

	SystemIdentification _sys_id;
	_sys_id.setHpfCutoffFrequency(fs, 0.05f);
	_sys_id.setLpfCutoffFrequency(fs, 30.f);
	_sys_id.setForgettingFactor(80.f, 1.f / fs);

	साठी (पूर्णांक i = 0; i < 10; i += 2) {
		_sys_id.update(दशांश(i), दशांश(i + 1));
	}

	स्थिरांक Vector<दशांश, 5> coefficients = _sys_id.getCoefficients();

	// WHEN: resetting
	_sys_id.reset();

	// THEN: the variances and coefficients should be properly reset
	EXPECT_TRUE(_sys_id.getCoefficients().असल().max() < 1e-8f);
	EXPECT_TRUE(_sys_id.getVariances().min() > 9e3f);

	// AND WHEN: running the same sequence of inputs-outputs

	साठी (पूर्णांक i = 0; i < 10; i += 2) {
		_sys_id.update(दशांश(i), दशांश(i + 1));
	}

	// THEN: the result should be exactly the same
	EXPECT_TRUE((coefficients - _sys_id.getCoefficients()).असल().max() < 1e-8f);
	coefficients.print();
	_sys_id.getVariances().print();
}

TEST_F(SystemIdentificationTest, simulatedModelTest)
{
	constexpr दशांश fs = 200.f;
	स्थिरांक दशांश gyro_lpf_cutoff = 30.f;

	SystemIdentification _sys_id;
	_sys_id.setHpfCutoffFrequency(fs, 0.05f);
	_sys_id.setLpfCutoffFrequency(fs, gyro_lpf_cutoff);
	_sys_id.setForgettingFactor(60.f, 1.f / fs);

	// Simulated model with integrator
	स्थिरांक दशांश a1 = -1.77f;
	स्थिरांक दशांश a2 = 0.77f;
	स्थिरांक दशांश b0 = 0.3812f;
	स्थिरांक दशांश b1 = -0.25f;
	स्थिरांक दशांश b2 = 0.2f;
	setCoefficients(a1, a2, b0, b1, b2);
	स्थिरांक दशांश u_bias = -0.1f; // constant control offset
	स्थिरांक दशांश y_bias = 0.2f; // measurement bias

	स्थिरांक दशांश dt = 1.f / fs;
	स्थिरांक दशांश duration = 2.f;
	दशांश u = 0.f;
	दशांश y = 0.f;

	साठी (पूर्णांक i = 0; i < static_cast<पूर्णांक>(duration / dt); i++) {

		// Generate square input signal
		जर (_sys_id.areFiltersInitialized()
		    && (i % 30 == 0)) {
			जर (u > 0.f) {
				u = -1.f;

			} अन्यथा {
				u = 1.f;
			}
		}

		_sys_id.update(u + u_bias, y + y_bias); // apply new input and previous output

		y = apply(u);
#जर 0
		स्थिरांक दशांश t = i * dt;
		म_लिखो("%.6f, %.6f, %.6f\n", (द्विगुण)t, (द्विगुण)u, (द्विगुण)y);
#endif
	}

	स्थिरांक Vector<दशांश, 5> coefficients = _sys_id.getCoefficients();
	दशांश data_check[] = {a1, a2, b0, b1, b2};
	स्थिरांक Vector<दशांश, 5> coefficients_check(data_check);
	दशांश eps = 1e-3;
	EXPECT_TRUE((coefficients - coefficients_check).असल().max() < eps);
	coefficients.print();
	_sys_id.getVariances().print();
}

TEST_F(SystemIdentificationTest, realDataTest)
{
	स्थिर constexpr पूर्णांक n_samples_dt_avg = 100;
	स्थिर constexpr पूर्णांक n_samples_test_data = आकार(u_data) / आकार(u_data[0]);
	दशांश dt_sum = 0.f;

	साठी (पूर्णांक i = 1; i <= n_samples_dt_avg; i++) {
		dt_sum += t_data[i] - t_data[i - 1];
	}

	स्थिरांक दशांश dt = dt_sum / n_samples_dt_avg;
	स्थिरांक दशांश fs = 1.f / dt;

	म_लिखो("dt = %.6f fs = %.3f\n", (द्विगुण)dt, (द्विगुण)(1.f / dt));

	स्थिरांक दशांश gyro_lpf_cutoff = 30.f;

	SystemIdentification _sys_id;
	_sys_id.setHpfCutoffFrequency(fs, 0.05f);
	_sys_id.setLpfCutoffFrequency(fs, gyro_lpf_cutoff);
	_sys_id.setForgettingFactor(60.f, dt);

	दशांश u = 0.f;
	दशांश y_prev = 0.f;

	साठी (पूर्णांक i = 0; i < n_samples_test_data; i++) {
		u = u_data[i];
		_sys_id.update(u, y_prev); // apply new input and previous output

		y_prev = y_data[i];

#जर 0
		म_लिखो("%.6f, %.6f, %.6f\n", (द्विगुण)t_data[i], (द्विगुण)u, (द्विगुण)y);
#endif
	}

	स्थिरांक Vector<दशांश, 5> coefficients = _sys_id.getCoefficients();
	दशांश data_check[] = {-1.916f, 0.916, -0.017f, -0.001f, 0.036f};
	स्थिरांक Vector<दशांश, 5> coefficients_check(data_check);
	दशांश eps = 1e-3;
	EXPECT_TRUE((coefficients - coefficients_check).असल().max() < eps);
	coefficients.print();
	_sys_id.getVariances().print();
}
