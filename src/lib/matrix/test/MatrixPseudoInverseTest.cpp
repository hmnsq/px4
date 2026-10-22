/****************************************************************************
 *
 *   Copyright (C) 2022 PX4 Development Team. All rights reserved.
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

#شامل <gtest/gtest.h>
#شامل <matrix/PseudoInverse.hpp>

استعمال نامخلا میٹرکس;

ساکن constexpr size_t n_large = 20;

TEST(میٹرکس_Pseudo_Inverse_Test, PseudoInverse)
{
	// 3x4 Matrix test
	اعشاری data0[12] = {
		0.f, 1.f,  2.f,  3.f,
		4.f, 5.f,  6.f,  7.f,
		8.f, 9.f, 10.f, 11.f
	};

	اعشاری data0_check[12] = {
		-0.3375f, -0.1f,  0.1375f,
			-0.13333333f, -0.03333333f,  0.06666667f,
			0.07083333f,  0.03333333f, -0.00416667f,
			0.275f,  0.1f, -0.075f
		};

	میٹرکس_2<اعشاری, 3, 4> A0(data0);
	میٹرکس_2<اعشاری, 4, 3> A0_I;
	EXPECT_TRUE(geninv(A0, A0_I));
	میٹرکس_2<اعشاری, 4, 3> A0_I_check(data0_check);

	EXPECT_EQ(A0_I, A0_I_check);

	// 4x3 Matrix test
	اعشاری data1[12] = {
		0.f, 4.f, 8.f,
		1.f, 5.f, 9.f,
		2.f, 6.f, 10.f,
		3.f, 7.f, 11.f
	};

	اعشاری data1_check[12] = {
		-0.3375f, -0.13333333f,  0.07083333f,  0.275f,
			-0.1f, -0.03333333f,  0.03333333f,  0.1f,
			0.1375f,  0.06666667f, -0.00416667f, -0.075f
		};

	میٹرکس_2<اعشاری, 4, 3> A1(data1);
	میٹرکس_2<اعشاری, 3, 4> A1_I;
	EXPECT_TRUE(geninv(A1, A1_I));
	میٹرکس_2<اعشاری, 3, 4> A1_I_check(data1_check);

	EXPECT_EQ(A1_I, A1_I_check);

	// Stess test
	میٹرکس_2 < اعشاری, n_large, n_large - 1 > A_large;
	A_large.سیٹ_شناختی_عنصر();
	میٹرکس_2 < اعشاری, n_large - 1, n_large > A_large_I;

	برائے (size_t i = 0; i < n_large; i++) {
		EXPECT_TRUE(geninv(A_large, A_large_I));
		EXPECT_EQ(A_large, A_large_I.T());
	}

	// Square matrix test
	اعشاری data2[9] = {0, 2, 3,
			  4, 5, 6,
			  7, 8, 10
			 };
	اعشاری data2_check[9] = {
		-0.4f, -0.8f,  0.6f,
			-0.4f,  4.2f, -2.4f,
			0.6f, -2.8f,  1.6f
		};

	مربع_میٹرکس<اعشاری, 3> A2(data2);
	مربع_میٹرکس<اعشاری, 3> A2_I;
	EXPECT_TRUE(geninv(A2, A2_I));
	مربع_میٹرکس<اعشاری, 3> A2_I_check(data2_check);
	EXPECT_TRUE(isEqual(A2_I, A2_I_check, 1e-3f));

	// Null matrix test
	میٹرکس_2<اعشاری, 6, 16> A3;
	میٹرکس_2<اعشاری, 16, 6> A3_I;
	EXPECT_TRUE(geninv(A3, A3_I));
	میٹرکس_2<اعشاری, 16, 6> A3_I_check;
	EXPECT_EQ(A3_I, A3_I_check);

	// Mock-up effectiveness matrix
	ثابت اعشاری B_quad_w[6][16] = {
		{-0.5717536f,  0.43756646f,  0.5717536f, -0.43756646f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f},
		{ 0.35355328f, -0.35355328f,  0.35355328f, -0.35355328f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f},
		{ 0.28323701f,  0.28323701f, -0.28323701f, -0.28323701f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f},
		{ 0.f,  0.f,  0.f,  0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f},
		{ 0.f,  0.f,  0.f,  0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f},
		{-0.25f, -0.25f, -0.25f, -0.25f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f}
	};
	میٹرکس_2<اعشاری, 6, 16> B = میٹرکس_2<اعشاری, 6, 16>(B_quad_w);
	ثابت اعشاری A_quad_w[16][6] = {
		{ -0.495383f,  0.707107f,  0.765306f,  0.0f, 0.0f, -1.000000f },
		{  0.495383f, -0.707107f,  1.000000f,  0.0f, 0.0f, -1.000000f },
		{  0.495383f,  0.707107f, -0.765306f,  0.0f, 0.0f, -1.000000f },
		{ -0.495383f, -0.707107f, -1.000000f,  0.0f, 0.0f, -1.000000f },
		{ 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f},
		{ 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f},
		{ 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f},
		{ 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f},
		{ 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f},
		{ 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f},
		{ 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f},
		{ 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f},
		{ 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f},
		{ 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f},
		{ 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f},
		{ 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}
	};
	میٹرکس_2<اعشاری, 16, 6> A_check = میٹرکس_2<اعشاری, 16, 6>(A_quad_w);
	میٹرکس_2<اعشاری, 16, 6> A;
	EXPECT_TRUE(geninv(B, A));
	EXPECT_EQ(A, A_check);

	// Real-world test case
	ثابت اعشاری real_alloc[5][6] = {
		{ 0.794079,  0.794079,  0.794079,  0.794079,  0.0000,  0.0000},
		{ 0.607814,  0.607814,  0.607814,  0.607814,  1.0000,  1.0000},
		{-0.672516,  0.915642, -0.915642,  0.672516,  0.0000,  0.0000},
		{ 0.159704,  0.159704,  0.159704,  0.159704, -0.2500, -0.2500},
		{ 0.607814, -0.607814,  0.607814, -0.607814,  1.0000,  1.0000}
	};
	میٹرکس_2<اعشاری, 5, 6> real(real_alloc);
	میٹرکس_2<اعشاری, 6, 5> real_pinv;
	EXPECT_TRUE(geninv(real, real_pinv));

	// from SVD-based inverse
	ثابت اعشاری real_pinv_expected_alloc[6][5] = {
		{ 2.096205,  -2.722267,   2.056547,   1.503279,   3.098087},
		{ 1.612621,  -1.992694,   2.056547,   1.131090,   2.275467},
		{-1.062688,   2.043479,  -2.056547,  -0.927950,  -2.275467},
		{-1.546273,   2.773052,  -2.056547,  -1.300139,  -3.098087},
		{-0.293930,   0.443445,   0.000000,  -0.226222,   0.000000},
		{-0.293930,   0.443445,   0.000000,  -0.226222,   0.000000}
	};
	میٹرکس_2<اعشاری, 6, 5> real_pinv_expected(real_pinv_expected_alloc);
	EXPECT_EQ(real_pinv, real_pinv_expected);
}
