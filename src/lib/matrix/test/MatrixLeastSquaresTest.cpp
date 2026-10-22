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

#ಅನ್ತರ್ಭಾವ <gtest/gtest.h>
#ಅನ್ತರ್ಭಾವ <matrix/math.hpp>

ಬಳಸಿ ನಾಮಾಕಾಶ ಮಾತೃಕೆ;

ಪೂರ್ಣಾಂಕ test_4x3(ಶೂನ್ಯ);
ಮಾದರಿ<ಬಗೆ_ಹೆಸರು Type> ಪೂರ್ಣಾಂಕ test_4x4(ಶೂನ್ಯ);
ಪೂರ್ಣಾಂಕ test_4x4_type_double(ಶೂನ್ಯ);
ಪೂರ್ಣಾಂಕ test_div_zero(ಶೂನ್ಯ);

TEST(ಮಾತೃಕೆ_ಕನಿಷ್ಠ_Squares_Test, 4x3)
{
	// Start with an (m x n) A matrix
	ದಶಮಾಂಶ ದತ್ತಾಂಶ[12] = {20.f, -10.f, -13.f,
			  17.f,  16.f, -18.f,
			  0.7f,  -0.8f,   0.9f,
			  -1.f,  -1.1f,  -1.2f
			 };
	ಮಾತೃಕೆ_2<ದಶಮಾಂಶ, 4, 3> A(ದತ್ತಾಂಶ);
	Vector4f b(2.0f, 3.0f, 4.0f, 5.0f);
	Vector3f x_check(-0.69168233f, -0.26227593f, -1.03767522f);

	ಕನಿಷ್ಠ_Squares_Solver<ದಶಮಾಂಶ, 4, 3> qrd = ಕನಿಷ್ಠ_Squares_Solver<ದಶಮಾಂಶ, 4, 3>(A);

	Vector3f x = qrd.solve(b);
	EXPECT_EQ(x, x_check);
}

TEST(ಮಾತೃಕೆ_ಕನಿಷ್ಠ_Squares_Test, 4x4)
{
	// Start with an (m x n) A matrix
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ ದತ್ತಾಂಶ[16] = { 20.f, -10.f, -13.f,  21.f,
				 17.f,  16.f, -18.f, -14.f,
				 0.7f,  -0.8f,   0.9f,  -0.5f,
				 -1.f,  -1.1f,  -1.2f,  -1.3f
			       };
	ಮಾತೃಕೆ_2<ದಶಮಾಂಶ, 4, 4> A(ದತ್ತಾಂಶ);
	Vector4f b(2.0f, 3.0f, 4.0f, 5.0f);
	Vector4f x_check(0.97893433f, -2.80798701f, -0.03175765f, -2.19387649f);

	ಕನಿಷ್ಠ_Squares_Solver<ದಶಮಾಂಶ, 4, 4> qrd = ಕನಿಷ್ಠ_Squares_Solver<ದಶಮಾಂಶ, 4, 4>(A);

	Vector4f x = qrd.solve(b);
	EXPECT_EQ(x, x_check);
}

TEST(ಮಾತೃಕೆ_ಕನಿಷ್ಠ_Squares_Test, ZeroDivision)
{
	ದಶಮಾಂಶ ದತ್ತಾಂಶ[4] = {0.0f, 0.0f, 0.0f, 0.0f};
	ಮಾತೃಕೆ_2<ದಶಮಾಂಶ, 2, 2> A(ದತ್ತಾಂಶ);

	ದಶಮಾಂಶ b_ದತ್ತಾಂಶ[2] = {1.0f, 1.0f};
	ಸದಿಶ<ದಶಮಾಂಶ, 2> b(b_ದತ್ತಾಂಶ);

	// Implement such that x returns zeros if it reaches div by zero
	ದಶಮಾಂಶ x_check_ದತ್ತಾಂಶ[2] = {0.0f, 0.0f};
	ಸದಿಶ<ದಶಮಾಂಶ, 2> x_check(x_check_ದತ್ತಾಂಶ);

	ಕನಿಷ್ಠ_Squares_Solver<ದಶಮಾಂಶ, 2, 2> qrd = ಕನಿಷ್ಠ_Squares_Solver<ದಶಮಾಂಶ, 2, 2>(A);

	ಸದಿಶ<ದಶಮಾಂಶ, 2> x = qrd.solve(b);
	EXPECT_EQ(x, x_check);
}
