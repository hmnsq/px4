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

#अन्तर्भाव <gtest/gtest.h>
#अन्तर्भाव <matrix/math.hpp>

उपयुज्य नामाकाश matrix;

TEST(Matrix_Sparse_सदिशम्_Test, defaultConstruction)
{
	SparseVectorf<24, 4, 6> a;
	EXPECT_EQ(a.non_zeros(), 2);
	EXPECT_EQ(a.सूचकांक(0), 4);
	EXPECT_EQ(a.सूचकांक(1), 6);
	a.at<4>() = 1.f;
	a.at<6>() = 2.f;
}

TEST(Matrix_Sparse_सदिशम्_Test, initialization_With_दत्त)
{
	ध्रुव भिन्न दत्त[3] = {1.f, 2.f, 3.f};
	SparseVectorf<24, 4, 6, 22> a(दत्त);
	EXPECT_EQ(a.non_zeros(), 3);
	EXPECT_EQ(a.सूचकांक(0), 4);
	EXPECT_EQ(a.सूचकांक(1), 6);
	EXPECT_EQ(a.सूचकांक(2), 22);
	EXPECT_FLOAT_EQ(a.at<4>(), दत्त[0]);
	EXPECT_FLOAT_EQ(a.at<6>(), दत्त[1]);
	EXPECT_FLOAT_EQ(a.at<22>(), दत्त[2]);
}

TEST(Matrix_Sparse_सदिशम्_Test, initialisation_From_सदिशम्)
{
	ध्रुव Vector3f vec(1.f, 2.f, 3.f);
	ध्रुव SparseVectorf<3, 0, 2> a(vec);
	EXPECT_FLOAT_EQ(a.at<0>(), vec(0));
	EXPECT_FLOAT_EQ(a.at<2>(), vec(2));
}

TEST(Matrix_Sparse_सदिशम्_Test, access_दत्त_With_Compressed_Indices)
{
	ध्रुव Vector3f vec(1.f, 2.f, 3.f);
	SparseVectorf<3, 0, 2> a(vec);

	प्रति (size_t i = 0; i < a.non_zeros(); i++) {
		a.at_Compressed_सूचकांक(i) = static_cast<भिन्न>(i);
	}

	EXPECT_FLOAT_EQ(a.at<0>(), a.at_Compressed_सूचकांक(0));
	EXPECT_FLOAT_EQ(a.at<2>(), a.at_Compressed_सूचकांक(1));
}

TEST(Matrix_Sparse_सदिशम्_Test, समुच्चयः_Zero)
{
	ध्रुव भिन्न दत्त[3] = {1.f, 2.f, 3.f};
	SparseVectorf<24, 4, 6, 22> a(दत्त);
	a.समुच्चयः_Zero();
	EXPECT_FLOAT_EQ(a.at<4>(), 0.f);
	EXPECT_FLOAT_EQ(a.at<6>(), 0.f);
	EXPECT_FLOAT_EQ(a.at<22>(), 0.f);
}

TEST(Matrix_Sparse_सदिशम्_Test, addition_With_Dense_सदिशम्)
{
	Vector4f dense_vec;
	dense_vec.समुच्चयः_All(1.f);
	ध्रुव भिन्न दत्त[3] = {1.f, 2.f, 3.f};
	ध्रुव SparseVectorf<4, 1, 2, 3> sparse_vec(दत्त);
	ध्रुव Vector4f res = sparse_vec + dense_vec;
	EXPECT_FLOAT_EQ(res(0), 1.f);
	EXPECT_FLOAT_EQ(res(1), 2.f);
	EXPECT_FLOAT_EQ(res(2), 3.f);
	EXPECT_FLOAT_EQ(res(3), 4.f);
}

TEST(Matrix_Sparse_सदिशम्_Test, सङ्कलय_Scalar)
{
	ध्रुव भिन्न दत्त[3] = {1.f, 2.f, 3.f};
	SparseVectorf<4, 1, 2, 3> sparse_vec(दत्त);
	sparse_vec += 2.f;
	EXPECT_FLOAT_EQ(sparse_vec.at<1>(), 3.f);
	EXPECT_FLOAT_EQ(sparse_vec.at<2>(), 4.f);
	EXPECT_FLOAT_EQ(sparse_vec.at<3>(), 5.f);
}

TEST(Matrix_Sparse_सदिशम्_Test, dot_गुणनफलम्_With_Dense_सदिशम्)
{
	Vector4f dense_vec;
	dense_vec.समुच्चयः_All(3.f);
	ध्रुव भिन्न दत्त[3] = {1.f, 2.f, 3.f};
	ध्रुव SparseVectorf<4, 1, 2, 3> sparse_vec(दत्त);
	भिन्न res = sparse_vec.dot(dense_vec);
	EXPECT_FLOAT_EQ(res, 18.f);
}

TEST(Matrix_Sparse_सदिशम्_Test, multiplicationWithDenseMatrix)
{
	Matrix<भिन्न, 2, 3> dense_matrix;
	dense_matrix.समुच्चयः_All(2.f);
	dense_matrix(1, 1) = 3.f;
	ध्रुव Vector3f dense_vec(0.f, 1.f, 5.f);
	ध्रुव SparseVectorf<3, 1, 2> sparse_vec(dense_vec);
	ध्रुव सदिशम्<भिन्न, 2> res_sparse = dense_matrix * sparse_vec;
	ध्रुव सदिशम्<भिन्न, 2> res_dense = dense_matrix * dense_vec;
	EXPECT_TRUE(isEqual(res_dense, res_sparse));
}

TEST(Matrix_Sparse_सदिशम्_Test, quadraticForm)
{
	भिन्न matrix_दत्त[9] = {1, 2, 3,
				2, 4, 5,
				3, 5, 6
			       };
	ध्रुव वर्गफलम्_Matrix<भिन्न, 3> dense_matrix(matrix_दत्त);
	ध्रुव Vector3f dense_vec(0.f, 1.f, 5.f);
	ध्रुव SparseVectorf<3, 1, 2> sparse_vec(dense_vec);
	EXPECT_FLOAT_EQ(quadraticForm(dense_matrix, sparse_vec), 204.f);
}

TEST(Matrix_Sparse_सदिशम्_Test, norms)
{
	ध्रुव भिन्न दत्त[2] = {3.f, 4.f};
	ध्रुव SparseVectorf<4, 1, 3> sparse_vec(दत्त);
	EXPECT_FLOAT_EQ(sparse_vec.norm_squared(), 25.f);
	EXPECT_FLOAT_EQ(sparse_vec.norm(), 5.f);
	EXPECT_TRUE(sparse_vec.longerThan(4.5f));
	EXPECT_FALSE(sparse_vec.longerThan(5.5f));
}
