/****************************************************************************
 *
 *   Copyright (C) 2019 PX4 Development Team. All rights reserved.
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
 * Test code for the Notch filter
 * Run this test only using make tests TESTFILTER=NotchFilter
 */

#अन्तर्भाव <gtest/gtest.h>
#अन्तर्भाव <matrix/matrix/math.hpp>

#अन्तर्भाव <lib/mathlib/math/filter/NotchFilter.hpp>

using नामाकाश math;
using matrix::Vector3f;

वर्ग NotchFilterTest : सार्वजनिक ::testing::Test
{
सार्वजनिक:
	NotchFilter<दशांश> _notch_float;
	NotchFilter<Vector3f> _notch_vector3f;
	स्थिरांक दशांश _sample_freq = 1000.f;
	स्थिरांक दशांश _notch_freq = 50.f;
	स्थिरांक दशांश _bandwidth = 15.f;

	स्थिरांक दशांश _epsilon_near = 1e-6f;
};

TEST_F(NotchFilterTest, simple)
{
	_notch_float.setParameters(_sample_freq, _notch_freq, _bandwidth);
	EXPECT_EQ(_notch_float.getBandwidth(), _bandwidth);
	EXPECT_EQ(_notch_float.getNotchFreq(), _notch_freq);

	दशांश a[3];
	दशांश b[3];

	// a and b coefficients computed from an external python script
	स्थिरांक दशांश b_expected[3] = {0.95496499f, -1.81645136f, 0.95496499f};
	स्थिरांक दशांश a_expected[3] = {1.f, -1.81645136f, 0.90992999f};

	_notch_float.getCoefficients(a, b);

	साठी (पूर्णांक i = 0; i < 3; i++) {
		EXPECT_NEAR(a[i], a_expected[i], _epsilon_near);
		EXPECT_NEAR(b[i], b_expected[i], _epsilon_near);
	}
}

TEST_F(NotchFilterTest, filtering_खालचा_Side)
{
	// Send a 25Hz sinusoidal signal into a 50Hz notch filter
	_notch_float.reset(0.0f);
	_notch_float.setParameters(_sample_freq, _notch_freq, _bandwidth);
	स्थिरांक दशांश signal_freq = 25.f;
	स्थिरांक दशांश omega = 2.f * M_PI_F * signal_freq;
	दशांश phase_delay = 11.4f * M_PI_F / 180.f; // Given by simulation
	दशांश t = 0.f;
	दशांश dt = 1.f / _sample_freq;
	दशांश out = 0.f;

	साठी (पूर्णांक i = 0; i < 1000; i++) {
		दशांश आदान = sinf(omega * t);
		दशांश output_expected = sinf(omega * t - phase_delay);
		out = _notch_float.apply(आदान);
		t = i * dt;

		// Let some time for the filter to settle
		जर (i > 30) {
			EXPECT_NEAR(out, output_expected, 0.05f);
		}
	}
}

TEST_F(NotchFilterTest, filtering_खालचा_Side_DF1)
{
	// Send a 25Hz sinusoidal signal into a 50Hz notch filter
	_notch_float.reset(0.0f);
	_notch_float.setParameters(_sample_freq, _notch_freq, _bandwidth);
	स्थिरांक दशांश signal_freq = 25.f;
	स्थिरांक दशांश omega = 2.f * M_PI_F * signal_freq;
	दशांश phase_delay = 11.4f * M_PI_F / 180.f; // Given by simulation
	दशांश t = 0.f;
	दशांश dt = 1.f / _sample_freq;
	दशांश out = 0.f;

	साठी (पूर्णांक i = 0; i < 1000; i++) {
		दशांश आदान = sinf(omega * t);
		दशांश output_expected = sinf(omega * t - phase_delay);
		out = _notch_float.apply(आदान);
		t = i * dt;

		// Let some time for the filter to settle
		जर (i > 30) {
			EXPECT_NEAR(out, output_expected, 0.05f);
		}
	}
}

TEST_F(NotchFilterTest, filtering_वरचा_Side)
{
	// Send a 98 sinusoidal signal into a 50Hz notch filter
	_notch_float.reset(0.0f);
	_notch_float.setParameters(_sample_freq, _notch_freq, _bandwidth);
	स्थिरांक दशांश signal_freq = 98.4f;
	स्थिरांक दशांश omega = 2.f * M_PI_F * signal_freq;
	दशांश phase_delay = 11.4f * M_PI_F / 180.f; // Given by simulation
	दशांश t = 0.f;
	दशांश dt = 1.f / _sample_freq;
	दशांश out = 0.f;

	साठी (पूर्णांक i = 0; i < 1000; i++) {
		दशांश आदान = sinf(omega * t);
		दशांश output_expected = sinf(omega * t + phase_delay);
		out = _notch_float.apply(आदान);
		t = i * dt;

		// Let some time for the filter to settle
		जर (i > 30) {
			EXPECT_NEAR(out, output_expected, 0.05f);
		}
	}
}

TEST_F(NotchFilterTest, filtering_वरचा_Side_DF1)
{
	// Send a 98 sinusoidal signal into a 50Hz notch filter
	_notch_float.reset(0.0f);
	_notch_float.setParameters(_sample_freq, _notch_freq, _bandwidth);
	स्थिरांक दशांश signal_freq = 98.4f;
	स्थिरांक दशांश omega = 2.f * M_PI_F * signal_freq;
	दशांश phase_delay = 11.4f * M_PI_F / 180.f; // Given by simulation
	दशांश t = 0.f;
	दशांश dt = 1.f / _sample_freq;
	दशांश out = 0.f;

	साठी (पूर्णांक i = 0; i < 1000; i++) {
		दशांश आदान = sinf(omega * t);
		दशांश output_expected = sinf(omega * t + phase_delay);
		out = _notch_float.apply(आदान);
		t = i * dt;

		// Let some time for the filter to settle
		जर (i > 30) {
			EXPECT_NEAR(out, output_expected, 0.05f);
		}
	}
}

TEST_F(NotchFilterTest, filterOnNotch)
{
	// Send a 50 sinusoidal signal into a 50Hz notch filter
	_notch_float.reset(0.0f);
	_notch_float.setParameters(_sample_freq, _notch_freq, _bandwidth);
	स्थिरांक दशांश signal_freq = 50.0f;
	स्थिरांक दशांश omega = 2.f * M_PI_F * signal_freq;
	दशांश t = 0.f;
	दशांश dt = 1.f / _sample_freq;
	दशांश out = 0.f;

	साठी (पूर्णांक i = 0; i < 1000; i++) {
		दशांश आदान = sinf(omega * t);
		out = _notch_float.apply(आदान);
		t = i * dt;

		// Let some time for the filter to settle
		जर (i > 50) {
			EXPECT_NEAR(out, 0.f, 0.1f);
		}
	}
}

TEST_F(NotchFilterTest, filterOnNotchDF1)
{
	// Send a 50 sinusoidal signal into a 50Hz notch filter
	_notch_float.reset(0.0f);
	_notch_float.setParameters(_sample_freq, _notch_freq, _bandwidth);
	स्थिरांक दशांश signal_freq = 50.0f;
	स्थिरांक दशांश omega = 2.f * M_PI_F * signal_freq;
	दशांश t = 0.f;
	दशांश dt = 1.f / _sample_freq;
	दशांश out = 0.f;

	साठी (पूर्णांक i = 0; i < 1000; i++) {
		दशांश आदान = sinf(omega * t);
		out = _notch_float.apply(आदान);
		t = i * dt;

		// Let some time for the filter to settle
		जर (i > 50) {
			EXPECT_NEAR(out, 0.f, 0.1f);
		}
	}
}

TEST_F(NotchFilterTest, updateFilter)
{
	_notch_float.reset(0.0f);
	_notch_float.setParameters(_sample_freq, _notch_freq, _bandwidth);
	दशांश new_notch_freq = 100.f;
	दशांश new_bandwidth = 10.f;
	_notch_float.setParameters(_sample_freq, new_notch_freq, new_bandwidth);

	EXPECT_EQ(new_notch_freq, _notch_float.getNotchFreq());
	EXPECT_EQ(new_bandwidth, _notch_float.getBandwidth());

}

TEST_F(NotchFilterTest, filterVector3f)
{
	// Send three sinusoidal signals (25, 50 and 98.5Hz) into a 50Hz triple notch filter
	_notch_vector3f.setParameters(_sample_freq, _notch_freq, _bandwidth);

	स्थिरांक Vector3f signal_freq(25.f, 50.f, 98.4f);
	स्थिरांक Vector3f omega = 2.f * M_PI_F * signal_freq;
	स्थिरांक Vector3f phase_delay = Vector3f(-11.4f, 0.f, 11.4f) * M_PI_F / 180.f;

	दशांश t = 0.f;
	दशांश dt = 1.f / _sample_freq;
	Vector3f out{};

	साठी (पूर्णांक i = 0; i < 1000; i++) {
		स्थिरांक Vector3f आदान(sinf(omega(0) * t), sinf(omega(1) * t), sinf(omega(2) * t));
		स्थिरांक Vector3f arg = omega * t + phase_delay;
		स्थिरांक Vector3f output_expected(sinf(arg(0)), 0.f, sinf(arg(2)));
		out = _notch_vector3f.apply(आदान);
		t = i * dt;

		// Let some time for the filter to settle
		जर (i > 50) {
			EXPECT_NEAR(out(0), output_expected(0), 0.1f);
			EXPECT_NEAR(out(1), output_expected(1), 0.1f);
			EXPECT_NEAR(out(2), output_expected(2), 0.1f);
		}
	}
}

TEST_F(NotchFilterTest, filterVector3fDF1)
{
	// Send three sinusoidal signals (25, 50 and 98.5Hz) into a 50Hz triple notch filter
	_notch_vector3f.setParameters(_sample_freq, _notch_freq, _bandwidth);

	स्थिरांक Vector3f signal_freq(25.f, 50.f, 98.4f);
	स्थिरांक Vector3f omega = 2.f * M_PI_F * signal_freq;
	स्थिरांक Vector3f phase_delay = Vector3f(-11.4f, 0.f, 11.4f) * M_PI_F / 180.f;

	दशांश t = 0.f;
	दशांश dt = 1.f / _sample_freq;
	Vector3f out{};

	साठी (पूर्णांक i = 0; i < 1000; i++) {
		स्थिरांक Vector3f आदान(sinf(omega(0) * t), sinf(omega(1) * t), sinf(omega(2) * t));
		स्थिरांक Vector3f arg = omega * t + phase_delay;
		स्थिरांक Vector3f output_expected(sinf(arg(0)), 0.f, sinf(arg(2)));
		out = _notch_vector3f.apply(आदान);
		t = i * dt;

		// Let some time for the filter to settle
		जर (i > 50) {
			EXPECT_NEAR(out(0), output_expected(0), 0.1f);
			EXPECT_NEAR(out(1), output_expected(1), 0.1f);
			EXPECT_NEAR(out(2), output_expected(2), 0.1f);
		}
	}
}

TEST_F(NotchFilterTest, disabled)
{
	स्थिरांक दशांश zero_notch_freq = 0.f;
	_notch_float.setParameters(_sample_freq, zero_notch_freq, _bandwidth);

	दशांश a[3];
	दशांश b[3];

	// The filter should just be a static gain of 1
	स्थिरांक दशांश b_expected[3] = {1.f, 0.f, 0.f};
	स्थिरांक दशांश a_expected[3] = {1.f, 0.f, 0.f};

	_notch_float.getCoefficients(a, b);

	साठी (पूर्णांक i = 0; i < 3; i++) {
		EXPECT_EQ(a[i], a_expected[i]);
		EXPECT_EQ(b[i], b_expected[i]);
	}

	// Run the filter with a 1Hz sinusoid
	स्थिरांक दशांश signal_freq = 1.0f;
	स्थिरांक दशांश omega = 2.f * M_PI_F * signal_freq;
	दशांश t = 0.f;
	दशांश dt = 1.f / _sample_freq;
	दशांश out = 0.f;

	साठी (पूर्णांक i = 0; i < 10; i++) {
		दशांश आदान = sinf(omega * t);
		out = _notch_float.apply(आदान);
		t = i * dt;

		// The filter should not do anything to the input
		EXPECT_EQ(out, आदान);
	}
}

TEST_F(NotchFilterTest, setCoefficients)
{

	दशांश a_new[2] = {1.f, 2.f};
	दशांश b_new[3] = {1.f, 2.f, 3.f};
	दशांश a[3];
	दशांश b[3];

	_notch_float.setCoefficients(a_new, b_new);
	_notch_float.getCoefficients(a, b);

	साठी (पूर्णांक i = 0; i < 3; i++) {
		जर (i >= 1) {EXPECT_EQ(a[i], a_new[i - 1]);} //a0 is not part of set function

		EXPECT_EQ(b[i], b_new[i]);
	}
}
