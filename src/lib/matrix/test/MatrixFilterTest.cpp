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

#അന്തര്ഭാവ <gtest/gtest.h>
#അന്തര്ഭാവ <matrix/filter.hpp>
#അന്തര്ഭാവ <matrix/Vector.hpp>

using നാമാകാശ matrix;

TEST(MatrixFilterTest, Filter)
{
	സ്ഥിരം size_t n_x = 6;
	സ്ഥിരം size_t n_y = 5;
	SquareMatrix<ദശാംശം, n_x> P = eye<ദശാംശം, n_x>();
	SquareMatrix<ദശാംശം, n_y> R = eye<ദശാംശം, n_y>();
	Matrix<ദശാംശം, n_y, n_x> C;
	C.setIdentity();
	ദശാംശം data[] = {1, 2, 3, 4, 5};
	Vector<ദശാംശം, n_y> r(data);

	Vector<ദശാംശം, n_x> dx;
	SquareMatrix<ദശാംശം, n_x> dP;
	ദശാംശം beta = 0;
	kalman_correct<ദശാംശം, 6, 5>(P, C, R, r, dx, dP, beta);

	ദശാംശം data_check[] = {0.5, 1, 1.5, 2, 2.5, 0};
	Vector<ദശാംശം, n_x> dx_check(data_check);
	EXPECT_EQ(dx, dx_check);
}
