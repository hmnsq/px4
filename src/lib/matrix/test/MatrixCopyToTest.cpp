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

TEST(ಮಾತೃಕೆ_Copy_ಗಮ್ಯ_Test, Copy_ಗಮ್ಯ)
{
	// Vector3 copyTo
	ಸ್ಥಿರಾಂಕ Vector3f v(1, 2, 3);
	ದಶಮಾಂಶ dst3[3] = {};
	v.copy_ಗಮ್ಯ(dst3);

	ಗಾಗಿ (size_t i = 0; i < 3; i++) {
		EXPECT_FLOAT_EQ(v(i), dst3[i]);
	}

	// Quaternion copyTo
	Quatf q(1, 2, 3, 4);
	ದಶಮಾಂಶ dst4[4] = {};
	q.copy_ಗಮ್ಯ(dst4);

	ಗಾಗಿ (size_t i = 0; i < 4; i++) {
		EXPECT_FLOAT_EQ(q(i), dst4[i]);
	}

	// Matrix copyTo
	ಮಾತೃಕೆ_2<ದಶಮಾಂಶ, 2, 3> A;
	A(0, 0) = 1;
	A(0, 1) = 2;
	A(0, 2) = 3;
	A(1, 0) = 4;
	A(1, 1) = 5;
	A(1, 2) = 6;
	ದಶಮಾಂಶ array_A[6] = {};
	A.copy_ಗಮ್ಯ(array_A);
	ದಶಮಾಂಶ array_ಅಡ್ಡಸಾಲು[6] = {1, 2, 3, 4, 5, 6};

	ಗಾಗಿ (size_t i = 0; i < 6; i++) {
		EXPECT_FLOAT_EQ(array_A[i], array_ಅಡ್ಡಸಾಲು[i]);
	}

	// Matrix copyToColumnMajor
	A.copy_ಗಮ್ಯ_ಸ್ತಂಭ_Major(array_A);
	ದಶಮಾಂಶ array_ಸ್ತಂಭ[6] = {1, 4, 2, 5, 3, 6};

	ಗಾಗಿ (size_t i = 0; i < 6; i++) {
		EXPECT_FLOAT_EQ(array_A[i], array_ಸ್ತಂಭ[i]);
	}

	// Slice copyTo
	ದಶಮಾಂಶ dst5[2] = {};
	v.ತುಂಡು<2, 1>(0, 0).copy_ಗಮ್ಯ(dst5);

	ಗಾಗಿ (size_t i = 0; i < 2; i++) {
		EXPECT_FLOAT_EQ(v(i), dst5[i]);
	}

	ದಶಮಾಂಶ subarray_A[4] = {};
	A.ತುಂಡು<2, 2>(0, 0).copy_ಗಮ್ಯ_ಸ್ತಂಭ_Major(subarray_A);
	ದಶಮಾಂಶ subarray_ಸ್ತಂಭ[4] = {1, 4, 2, 5};

	ಗಾಗಿ (size_t i = 0; i < 4; i++) {
		EXPECT_FLOAT_EQ(subarray_A[i], subarray_ಸ್ತಂಭ[i]);
	}
}
