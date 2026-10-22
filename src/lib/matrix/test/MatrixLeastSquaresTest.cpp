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

#அந்தர்பாவ <gtest/gtest.h>
#அந்தர்பாவ <matrix/math.hpp>

using நாமாகாஶ matrix;

முழு test_4x3(வெறுமை);
template<typename Type> முழு test_4x4(வெறுமை);
முழு test_4x4_type_double(வெறுமை);
முழு test_div_zero(வெறுமை);

TEST(MatrixLeastSquaresTest, 4x3)
{
	// Start with an (m x n) A matrix
	தசம data[12] = {20.f, -10.f, -13.f,
			  17.f,  16.f, -18.f,
			  0.7f,  -0.8f,   0.9f,
			  -1.f,  -1.1f,  -1.2f
			 };
	Matrix<தசம, 4, 3> A(data);
	Vector4f b(2.0f, 3.0f, 4.0f, 5.0f);
	Vector3f x_check(-0.69168233f, -0.26227593f, -1.03767522f);

	LeastSquaresSolver<தசம, 4, 3> qrd = LeastSquaresSolver<தசம, 4, 3>(A);

	Vector3f x = qrd.solve(b);
	EXPECT_EQ(x, x_check);
}

TEST(MatrixLeastSquaresTest, 4x4)
{
	// Start with an (m x n) A matrix
	மாறிலி தசம data[16] = { 20.f, -10.f, -13.f,  21.f,
				 17.f,  16.f, -18.f, -14.f,
				 0.7f,  -0.8f,   0.9f,  -0.5f,
				 -1.f,  -1.1f,  -1.2f,  -1.3f
			       };
	Matrix<தசம, 4, 4> A(data);
	Vector4f b(2.0f, 3.0f, 4.0f, 5.0f);
	Vector4f x_check(0.97893433f, -2.80798701f, -0.03175765f, -2.19387649f);

	LeastSquaresSolver<தசம, 4, 4> qrd = LeastSquaresSolver<தசம, 4, 4>(A);

	Vector4f x = qrd.solve(b);
	EXPECT_EQ(x, x_check);
}

TEST(MatrixLeastSquaresTest, ZeroDivision)
{
	தசம data[4] = {0.0f, 0.0f, 0.0f, 0.0f};
	Matrix<தசம, 2, 2> A(data);

	தசம b_data[2] = {1.0f, 1.0f};
	Vector<தசம, 2> b(b_data);

	// Implement such that x returns zeros if it reaches div by zero
	தசம x_check_data[2] = {0.0f, 0.0f};
	Vector<தசம, 2> x_check(x_check_data);

	LeastSquaresSolver<தசம, 2, 2> qrd = LeastSquaresSolver<தசம, 2, 2>(A);

	Vector<தசம, 2> x = qrd.solve(b);
	EXPECT_EQ(x, x_check);
}
