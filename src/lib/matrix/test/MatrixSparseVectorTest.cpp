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

TEST(ಮಾತೃಕೆ_Sparse_ಸದಿಶ_Test, defaultConstruction)
{
	SparseVectorf<24, 4, 6> a;
	EXPECT_EQ(a.non_zeros(), 2);
	EXPECT_EQ(a.ಸೂಚ್ಯಂಕ(0), 4);
	EXPECT_EQ(a.ಸೂಚ್ಯಂಕ(1), 6);
	a.at<4>() = 1.f;
	a.at<6>() = 2.f;
}

TEST(ಮಾತೃಕೆ_Sparse_ಸದಿಶ_Test, initialization_With_ದತ್ತಾಂಶ)
{
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ ದತ್ತಾಂಶ[3] = {1.f, 2.f, 3.f};
	SparseVectorf<24, 4, 6, 22> a(ದತ್ತಾಂಶ);
	EXPECT_EQ(a.non_zeros(), 3);
	EXPECT_EQ(a.ಸೂಚ್ಯಂಕ(0), 4);
	EXPECT_EQ(a.ಸೂಚ್ಯಂಕ(1), 6);
	EXPECT_EQ(a.ಸೂಚ್ಯಂಕ(2), 22);
	EXPECT_FLOAT_EQ(a.at<4>(), ದತ್ತಾಂಶ[0]);
	EXPECT_FLOAT_EQ(a.at<6>(), ದತ್ತಾಂಶ[1]);
	EXPECT_FLOAT_EQ(a.at<22>(), ದತ್ತಾಂಶ[2]);
}

TEST(ಮಾತೃಕೆ_Sparse_ಸದಿಶ_Test, initialisation_From_ಸದಿಶ)
{
	ಸ್ಥಿರಾಂಕ Vector3f vec(1.f, 2.f, 3.f);
	ಸ್ಥಿರಾಂಕ SparseVectorf<3, 0, 2> a(vec);
	EXPECT_FLOAT_EQ(a.at<0>(), vec(0));
	EXPECT_FLOAT_EQ(a.at<2>(), vec(2));
}

TEST(ಮಾತೃಕೆ_Sparse_ಸದಿಶ_Test, access_ದತ್ತಾಂಶ_With_Compressed_Indices)
{
	ಸ್ಥಿರಾಂಕ Vector3f vec(1.f, 2.f, 3.f);
	SparseVectorf<3, 0, 2> a(vec);

	ಗಾಗಿ (size_t i = 0; i < a.non_zeros(); i++) {
		a.at_Compressed_ಸೂಚ್ಯಂಕ(i) = static_cast<ದಶಮಾಂಶ>(i);
	}

	EXPECT_FLOAT_EQ(a.at<0>(), a.at_Compressed_ಸೂಚ್ಯಂಕ(0));
	EXPECT_FLOAT_EQ(a.at<2>(), a.at_Compressed_ಸೂಚ್ಯಂಕ(1));
}

TEST(ಮಾತೃಕೆ_Sparse_ಸದಿಶ_Test, ಗಣ_Zero)
{
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ ದತ್ತಾಂಶ[3] = {1.f, 2.f, 3.f};
	SparseVectorf<24, 4, 6, 22> a(ದತ್ತಾಂಶ);
	a.ಗಣ_Zero();
	EXPECT_FLOAT_EQ(a.at<4>(), 0.f);
	EXPECT_FLOAT_EQ(a.at<6>(), 0.f);
	EXPECT_FLOAT_EQ(a.at<22>(), 0.f);
}

TEST(ಮಾತೃಕೆ_Sparse_ಸದಿಶ_Test, addition_With_Dense_ಸದಿಶ)
{
	Vector4f dense_vec;
	dense_vec.ಗಣ_All(1.f);
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ ದತ್ತಾಂಶ[3] = {1.f, 2.f, 3.f};
	ಸ್ಥಿರಾಂಕ SparseVectorf<4, 1, 2, 3> sparse_vec(ದತ್ತಾಂಶ);
	ಸ್ಥಿರಾಂಕ Vector4f res = sparse_vec + dense_vec;
	EXPECT_FLOAT_EQ(res(0), 1.f);
	EXPECT_FLOAT_EQ(res(1), 2.f);
	EXPECT_FLOAT_EQ(res(2), 3.f);
	EXPECT_FLOAT_EQ(res(3), 4.f);
}

TEST(ಮಾತೃಕೆ_Sparse_ಸದಿಶ_Test, ಕೂಡಿಸು_Scalar)
{
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ ದತ್ತಾಂಶ[3] = {1.f, 2.f, 3.f};
	SparseVectorf<4, 1, 2, 3> sparse_vec(ದತ್ತಾಂಶ);
	sparse_vec += 2.f;
	EXPECT_FLOAT_EQ(sparse_vec.at<1>(), 3.f);
	EXPECT_FLOAT_EQ(sparse_vec.at<2>(), 4.f);
	EXPECT_FLOAT_EQ(sparse_vec.at<3>(), 5.f);
}

TEST(ಮಾತೃಕೆ_Sparse_ಸದಿಶ_Test, dot_ಗುಣಲಬ್ಧ_With_Dense_ಸದಿಶ)
{
	Vector4f dense_vec;
	dense_vec.ಗಣ_All(3.f);
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ ದತ್ತಾಂಶ[3] = {1.f, 2.f, 3.f};
	ಸ್ಥಿರಾಂಕ SparseVectorf<4, 1, 2, 3> sparse_vec(ದತ್ತಾಂಶ);
	ದಶಮಾಂಶ res = sparse_vec.dot(dense_vec);
	EXPECT_FLOAT_EQ(res, 18.f);
}

TEST(ಮಾತೃಕೆ_Sparse_ಸದಿಶ_Test, multiplication_With_Dense_ಮಾತೃಕೆ)
{
	ಮಾತೃಕೆ_2<ದಶಮಾಂಶ, 2, 3> dense_ಮಾತೃಕೆ;
	dense_ಮಾತೃಕೆ.ಗಣ_All(2.f);
	dense_ಮಾತೃಕೆ(1, 1) = 3.f;
	ಸ್ಥಿರಾಂಕ Vector3f dense_vec(0.f, 1.f, 5.f);
	ಸ್ಥಿರಾಂಕ SparseVectorf<3, 1, 2> sparse_vec(dense_vec);
	ಸ್ಥಿರಾಂಕ ಸದಿಶ<ದಶಮಾಂಶ, 2> res_sparse = dense_ಮಾತೃಕೆ * sparse_vec;
	ಸ್ಥಿರಾಂಕ ಸದಿಶ<ದಶಮಾಂಶ, 2> res_dense = dense_ಮಾತೃಕೆ * dense_vec;
	EXPECT_TRUE(isEqual(res_dense, res_sparse));
}

TEST(ಮಾತೃಕೆ_Sparse_ಸದಿಶ_Test, quadraticForm)
{
	ದಶಮಾಂಶ ಮಾತೃಕೆ_ದತ್ತಾಂಶ[9] = {1, 2, 3,
				2, 4, 5,
				3, 5, 6
			       };
	ಸ್ಥಿರಾಂಕ ವರ್ಗಫಲ_ಮಾತೃಕೆ<ದಶಮಾಂಶ, 3> dense_ಮಾತೃಕೆ(ಮಾತೃಕೆ_ದತ್ತಾಂಶ);
	ಸ್ಥಿರಾಂಕ Vector3f dense_vec(0.f, 1.f, 5.f);
	ಸ್ಥಿರಾಂಕ SparseVectorf<3, 1, 2> sparse_vec(dense_vec);
	EXPECT_FLOAT_EQ(quadraticForm(dense_ಮಾತೃಕೆ, sparse_vec), 204.f);
}

TEST(ಮಾತೃಕೆ_Sparse_ಸದಿಶ_Test, norms)
{
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ ದತ್ತಾಂಶ[2] = {3.f, 4.f};
	ಸ್ಥಿರಾಂಕ SparseVectorf<4, 1, 3> sparse_vec(ದತ್ತಾಂಶ);
	EXPECT_FLOAT_EQ(sparse_vec.norm_squared(), 25.f);
	EXPECT_FLOAT_EQ(sparse_vec.norm(), 5.f);
	EXPECT_TRUE(sparse_vec.longerThan(4.5f));
	EXPECT_FALSE(sparse_vec.longerThan(5.5f));
}
