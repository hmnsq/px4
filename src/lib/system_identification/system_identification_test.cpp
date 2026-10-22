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

उपयुज्य नामाकाश matrix;

वर्ग SystemIdentificationTest : सार्वजनिक ::testing::Test
{
सार्वजनिक:
	SystemIdentificationTest() {};
	भिन्न apply(भिन्न sample);
	शून्य समुच्चयः_Coefficients(भिन्न a1, भिन्न a2, भिन्न b0, भिन्न b1, भिन्न b2)
	{
		_a1 = a1;
		_a2 = a2;
		_b0 = b0;
		_b1 = b1;
		_b2 = b2;
	}

गुप्त:
	भिन्न _a1{};
	भिन्न _a2{};
	भिन्न _b0{};
	भिन्न _b1{};
	भिन्न _b2{};
	भिन्न _delay_element_1{};
	भिन्न _delay_element_2{};
};

भिन्न SystemIdentificationTest::apply(भिन्न sample)
{
	// Direct Form II implementation
	ध्रुव भिन्न delay_element_0{sample - _delay_element_1 *_a1 - _delay_element_2 * _a2};
	ध्रुव भिन्न output{delay_element_0 *_b0 + _delay_element_1 *_b1 + _delay_element_2 * _b2};

	_delay_element_2 = _delay_element_1;
	_delay_element_1 = delay_element_0;

	प्रत्यागम output;
}

TEST_F(SystemIdentificationTest, basicTest)
{
	constexpr भिन्न fs = 800.f;

	SystemIdentification _sys_id;
	_sys_id.समुच्चयः_Hpf_Cutoff_Frequency(fs, 0.05f);
	_sys_id.समुच्चयः_Lpf_Cutoff_Frequency(fs, 30.f);
	_sys_id.समुच्चयः_Forgetting_गुणकः(80.f, 1.f / fs);

	प्रति (पूर्णाङ्क i = 0; i < 5; i++) {
		// Fill the buffers with zeros
		_sys_id.update(0.f, 0.f);
	}

	प्रति (पूर्णाङ्क i = 0; i < 10; i += 2) {
		_sys_id.update(भिन्न(i), भिन्न(i + 1));
	}

	ध्रुव सदिशम्<भिन्न, 5> coefficients = _sys_id.getCoefficients();
	भिन्न दत्त_check[] = {-3.96f, 1.30f, -2.28f, -0.33f, 0.369f};
	ध्रुव सदिशम्<भिन्न, 5> coefficients_check(दत्त_check);
	भिन्न eps = 1e-2;
	EXPECT_TRUE((coefficients - coefficients_check).निरपेक्षम्().max() < eps);
	coefficients.print();
	_sys_id.getVariances().print();
}

TEST_F(SystemIdentificationTest, resetTest)
{
	constexpr भिन्न fs = 800.f;

	SystemIdentification _sys_id;
	_sys_id.समुच्चयः_Hpf_Cutoff_Frequency(fs, 0.05f);
	_sys_id.समुच्चयः_Lpf_Cutoff_Frequency(fs, 30.f);
	_sys_id.समुच्चयः_Forgetting_गुणकः(80.f, 1.f / fs);

	प्रति (पूर्णाङ्क i = 0; i < 10; i += 2) {
		_sys_id.update(भिन्न(i), भिन्न(i + 1));
	}

	ध्रुव सदिशम्<भिन्न, 5> coefficients = _sys_id.getCoefficients();

	// WHEN: resetting
	_sys_id.reset();

	// THEN: the variances and coefficients should be properly reset
	EXPECT_TRUE(_sys_id.getCoefficients().निरपेक्षम्().max() < 1e-8f);
	EXPECT_TRUE(_sys_id.getVariances().min() > 9e3f);

	// AND WHEN: running the same sequence of inputs-outputs

	प्रति (पूर्णाङ्क i = 0; i < 10; i += 2) {
		_sys_id.update(भिन्न(i), भिन्न(i + 1));
	}

	// THEN: the result should be exactly the same
	EXPECT_TRUE((coefficients - _sys_id.getCoefficients()).निरपेक्षम्().max() < 1e-8f);
	coefficients.print();
	_sys_id.getVariances().print();
}

TEST_F(SystemIdentificationTest, simulatedModelTest)
{
	constexpr भिन्न fs = 200.f;
	ध्रुव भिन्न gyro_lpf_cutoff = 30.f;

	SystemIdentification _sys_id;
	_sys_id.समुच्चयः_Hpf_Cutoff_Frequency(fs, 0.05f);
	_sys_id.समुच्चयः_Lpf_Cutoff_Frequency(fs, gyro_lpf_cutoff);
	_sys_id.समुच्चयः_Forgetting_गुणकः(60.f, 1.f / fs);

	// Simulated model with integrator
	ध्रुव भिन्न a1 = -1.77f;
	ध्रुव भिन्न a2 = 0.77f;
	ध्रुव भिन्न b0 = 0.3812f;
	ध्रुव भिन्न b1 = -0.25f;
	ध्रुव भिन्न b2 = 0.2f;
	समुच्चयः_Coefficients(a1, a2, b0, b1, b2);
	ध्रुव भिन्न u_bias = -0.1f; // constant control offset
	ध्रुव भिन्न y_bias = 0.2f; // measurement bias

	ध्रुव भिन्न dt = 1.f / fs;
	ध्रुव भिन्न duration = 2.f;
	भिन्न u = 0.f;
	भिन्न y = 0.f;

	प्रति (पूर्णाङ्क i = 0; i < static_cast<पूर्णाङ्क>(duration / dt); i++) {

		// Generate square input signal
		यदि (_sys_id.areFiltersInitialized()
		    && (i % 30 == 0)) {
			यदि (u > 0.f) {
				u = -1.f;

			} अन्यथा {
				u = 1.f;
			}
		}

		_sys_id.update(u + u_bias, y + y_bias); // apply new input and previous output

		y = apply(u);
#यदि 0
		ध्रुव भिन्न t = i * dt;
		प्रारूप_लिख("%.6f, %.6f, %.6f\n", (द्विगुण)t, (द्विगुण)u, (द्विगुण)y);
#यदि_अन्तः
	}

	ध्रुव सदिशम्<भिन्न, 5> coefficients = _sys_id.getCoefficients();
	भिन्न दत्त_check[] = {a1, a2, b0, b1, b2};
	ध्रुव सदिशम्<भिन्न, 5> coefficients_check(दत्त_check);
	भिन्न eps = 1e-3;
	EXPECT_TRUE((coefficients - coefficients_check).निरपेक्षम्().max() < eps);
	coefficients.print();
	_sys_id.getVariances().print();
}

TEST_F(SystemIdentificationTest, real_दत्त_Test)
{
	स्थिर constexpr पूर्णाङ्क n_samples_dt_avg = 100;
	स्थिर constexpr पूर्णाङ्क n_samples_test_दत्त = परिमाण(u_दत्त) / परिमाण(u_दत्त[0]);
	भिन्न dt_योग = 0.f;

	प्रति (पूर्णाङ्क i = 1; i <= n_samples_dt_avg; i++) {
		dt_योग += t_दत्त[i] - t_दत्त[i - 1];
	}

	ध्रुव भिन्न dt = dt_योग / n_samples_dt_avg;
	ध्रुव भिन्न fs = 1.f / dt;

	प्रारूप_लिख("dt = %.6f fs = %.3f\n", (द्विगुण)dt, (द्विगुण)(1.f / dt));

	ध्रुव भिन्न gyro_lpf_cutoff = 30.f;

	SystemIdentification _sys_id;
	_sys_id.समुच्चयः_Hpf_Cutoff_Frequency(fs, 0.05f);
	_sys_id.समुच्चयः_Lpf_Cutoff_Frequency(fs, gyro_lpf_cutoff);
	_sys_id.समुच्चयः_Forgetting_गुणकः(60.f, dt);

	भिन्न u = 0.f;
	भिन्न y_पूर्व_2 = 0.f;

	प्रति (पूर्णाङ्क i = 0; i < n_samples_test_दत्त; i++) {
		u = u_दत्त[i];
		_sys_id.update(u, y_पूर्व_2); // apply new input and previous output

		y_पूर्व_2 = y_दत्त[i];

#यदि 0
		प्रारूप_लिख("%.6f, %.6f, %.6f\n", (द्विगुण)t_दत्त[i], (द्विगुण)u, (द्विगुण)y);
#यदि_अन्तः
	}

	ध्रुव सदिशम्<भिन्न, 5> coefficients = _sys_id.getCoefficients();
	भिन्न दत्त_check[] = {-1.916f, 0.916, -0.017f, -0.001f, 0.036f};
	ध्रुव सदिशम्<भिन्न, 5> coefficients_check(दत्त_check);
	भिन्न eps = 1e-3;
	EXPECT_TRUE((coefficients - coefficients_check).निरपेक्षम्().max() < eps);
	coefficients.print();
	_sys_id.getVariances().print();
}
