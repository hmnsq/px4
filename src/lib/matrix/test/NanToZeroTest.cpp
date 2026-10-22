/****************************************************************************
 *
 *   Copyright (C) 2026 PX4 Development Team. All rights reserved.
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

using ನಾಮಾಕಾಶ matrix;

TEST(NanToZeroTest, NanToZero)
{
	ಧ್ರುವ ಭಿನ್ನ data[9]     = {-1.f, 0.f, +INFINITY, NAN, 0.f, -INFINITY, 1.f, 0.f, -NAN};
	ಧ್ರುವ ಭಿನ್ನ expected[9] = {-1.f, 0.f, +INFINITY, 0.f, 0.f, -INFINITY, 1.f, 0.f, 0.f};

	Matrix<ಭಿನ್ನ, 3, 3> mat(data);
	mat.nanToZero();

	ಧ್ರುವ Matrix<ಭಿನ್ನ, 3, 3> mat_expected(expected);

	ಪ್ರತಿ (size_t i = 0; i < 3; i++) {
		ಪ್ರತಿ (size_t j = 0; j < 3; j++) {
			EXPECT_FLOAT_EQ(mat(i, j), mat_expected(i, j));
		}
	}
}

TEST(NanToZeroTest, NoNanUnchanged)
{
	ಧ್ರುವ ಭಿನ್ನ data[4] = {-2.5f, 0.f, 3.14f, +INFINITY};

	Matrix<ಭಿನ್ನ, 2, 2> mat(data);
	mat.nanToZero();

	ಧ್ರುವ Matrix<ಭಿನ್ನ, 2, 2> mat_orig(data);

	ಪ್ರತಿ (size_t i = 0; i < 2; i++) {
		ಪ್ರತಿ (size_t j = 0; j < 2; j++) {
			EXPECT_FLOAT_EQ(mat(i, j), mat_orig(i, j));
		}
	}
}
