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

TEST(Matrix_खण्डिका_Test, खण्डिका_2)
{
	भिन्न दत्त[9] = {0, 2, 3,
			 4, 5, 6,
			 7, 8, 10
			};
	वर्गफलम्_Matrix<भिन्न, 3> A(दत्त);

	// Test row slicing
	Matrix<भिन्न, 2, 3> B_rowslice(A.खण्डिका<2, 3>(1, 0));
	भिन्न दत्त_check_rowslice[6] = {
		4, 5, 6,
		7, 8, 10
	};
	Matrix<भिन्न, 2, 3> B_check_rowslice(दत्त_check_rowslice);
	EXPECT_EQ(B_rowslice, B_check_rowslice);

	// Test column slicing
	Matrix<भिन्न, 3, 2> B_colslice(A.खण्डिका<3, 2>(0, 1));
	भिन्न दत्त_check_colslice[6] = {
		2, 3,
		5, 6,
		8, 10
	};
	Matrix<भिन्न, 3, 2> B_check_colslice(दत्त_check_colslice);
	EXPECT_EQ(B_colslice, B_check_colslice);

	// Test slicing both
	Matrix<भिन्न, 2, 2> B_bothslice(A.खण्डिका<2, 2>(1, 1));
	भिन्न दत्त_check_bothslice[4] = {
		5, 6,
		8, 10
	};
	Matrix<भिन्न, 2, 2> B_check_bothslice(दत्त_check_bothslice);
	EXPECT_EQ(B_bothslice, B_check_bothslice);

	//Test block writing
	भिन्न दत्त_2_2[4] = {
		11, 12,
		13, 14
	};

	Matrix<भिन्न, 2, 2> C(दत्त_2_2);
	A.खण्डिका<2, 2>(1, 1) = C;

	भिन्न दत्त_2_check[9] = {
		0, 2, 3,
		4, 11, 12,
		7, 13, 14
	};
	Matrix<भिन्न, 3, 3> D(दत्त_2_check);
	EXPECT_EQ(A, D);

	//Test writing to slices
	Matrix<भिन्न, 3, 1> E;
	E(0, 0) = -1;
	E(1, 0) = 1;
	E(2, 0) = 3;

	Matrix<भिन्न, 2, 1> F;
	F(0, 0) = 9;
	F(1, 0) = 11;

	E.खण्डिका<2, 1>(0, 0) = F;

	भिन्न दत्त_3_check[3] = {9, 11, 3};
	Matrix<भिन्न, 3, 1> G(दत्त_3_check);
	EXPECT_EQ(E, G);
	EXPECT_EQ(E, (Matrix<भिन्न, 3, 1>(E.खण्डिका<3, 1>(0, 0))));

	Matrix<भिन्न, 2, 1> H = E.खण्डिका<2, 1>(0, 0);
	EXPECT_EQ(H, F);

	भिन्न दत्त_4_check[5] = {3, 11, 9, 0, 0};
	{
		// assigning row slices to each other
		ध्रुव Matrix<भिन्न, 3, 1> J(दत्त_3_check);
		Matrix<भिन्न, 5, 1> K;
		K.row(2) = J.row(0);
		K.row(1) = J.row(1);
		K.row(0) = J.row(2);

		Matrix<भिन्न, 5, 1> K_check(दत्त_4_check);
		EXPECT_EQ(K, K_check);
	}
	{
		// assigning col slices to each other
		ध्रुव Matrix<भिन्न, 1, 3> J(दत्त_3_check);
		Matrix<भिन्न, 1, 5> K;
		K.col(2) = J.col(0);
		K.col(1) = J.col(1);
		K.col(0) = J.col(2);

		Matrix<भिन्न, 1, 5> K_check(दत्त_4_check);
		EXPECT_EQ(K, K_check);
	}

	// check that slice of a slice works for reading
	ध्रुव Matrix<भिन्न, 3, 3> cm33(दत्त);
	Matrix<भिन्न, 2, 1> शिखरम्_दक्षिणः = cm33.खण्डिका<2, 3>(0, 0).खण्डिका<2, 1>(0, 2);
	भिन्न शिखरम्_दक्षिणः_check[2] = {3, 6};
	EXPECT_EQ(शिखरम्_दक्षिणः, (Matrix<भिन्न, 2, 1>(शिखरम्_दक्षिणः_check)));

	// check that slice of a slice works for writing
	Matrix<भिन्न, 3, 3> m33(दत्त);
	m33.खण्डिका<2, 3>(0, 0).खण्डिका<2, 1>(0, 2) = Matrix<भिन्न, 2, 1>();
	ध्रुव भिन्न दत्त_check[9] = {0, 2, 0,
				     4, 5, 0,
				     7, 8, 10
				    };
	EXPECT_EQ(m33, (Matrix<भिन्न, 3, 3>(दत्त_check)));

	// longerThan
	Vector3f v5;
	v5(0) = 3;
	v5(1) = 4;
	v5(2) = 9;
	EXPECT_TRUE(v5.xy().longerThan(4.99f));
	EXPECT_FALSE(v5.xy().longerThan(5.f));
	EXPECT_FLOAT_EQ(v5.xy().norm(), 5.f);

	// min/max
	EXPECT_FLOAT_EQ(m33.row(1).max(), 5.f);
	EXPECT_FLOAT_EQ(m33.col(0).min(), 0.f);
	EXPECT_FLOAT_EQ((m33.खण्डिका<2, 2>(1, 1).max()), 10.f);

	// assign scalar value to slice
	Matrix<भिन्न, 3, 1> L;
	L(0, 0) = -1;
	L(1, 0) = 1;
	L(2, 0) = 3;

	L.खण्डिका<2, 1>(0, 0) = 0.0f;

	भिन्न दत्त_5_check[3] = {0, 0, 3};
	Matrix<भिन्न, 3, 1> M(दत्त_5_check);
	EXPECT_EQ(L, M);

	// return diagonal elements
	भिन्न दत्त_6[9] = {0, 2, 3,
			   4, 5, 6,
			   7, 8, 10
			  };
	वर्गफलम्_Matrix<भिन्न, 3> N(दत्त_6);

	Vector3f v6 = N.खण्डिका<3, 3>(0, 0).diag();
	Vector3f v6_check = {0, 5, 10};
	EXPECT_EQ(v6, v6_check);
	Vector2f v7 = N.खण्डिका<2, 3>(1, 0).diag();
	Vector2f v7_check = {4, 8};
	EXPECT_EQ(v7, v7_check);
	Vector2f v8 = N.खण्डिका<3, 2>(0, 1).diag();
	Vector2f v8_check = {2, 6};
	EXPECT_EQ(v8, v8_check);
	Vector2f v9(N.खण्डिका<1, 2>(1, 1));
	Vector2f v9_check = {5, 6};
	EXPECT_EQ(v9, v9_check);
	Vector3f v10(N.खण्डिका<1, 3>(1, 0));
	Vector3f v10_check = {4, 5, 6};
	EXPECT_EQ(v10, v10_check);

	// Different assignment operators
	वर्गफलम्_Matrix3f O(दत्त);
	भिन्न सङ्कार्यम्_दत्त [4] = {2, 1, -3, -1};
	ध्रुव वर्गफलम्_Matrix<भिन्न, 2> सङ्कार्यम्(सङ्कार्यम्_दत्त);

	O.खण्डिका<2, 2>(1, 0) += सङ्कार्यम्;
	भिन्न O_check_दत्त_1 [9] = {0, 2, 3,  6, 6, 6,  4, 7, 10};
	EXPECT_EQ(O, वर्गफलम्_Matrix3f(O_check_दत्त_1));

	O = वर्गफलम्_Matrix3f(दत्त);
	O.खण्डिका<2, 1>(1, 1) += सङ्कार्यम्.खण्डिका<2, 1>(0, 0);
	भिन्न O_check_दत्त_2 [9] = {0, 2, 3,  4, 7, 6,  7, 5, 10};
	EXPECT_EQ(O, वर्गफलम्_Matrix3f(O_check_दत्त_2));

	O = वर्गफलम्_Matrix3f(दत्त);
	O.खण्डिका<3, 3>(0, 0) += -1;
	भिन्न O_check_दत्त_3 [9] = {-1, 1, 2,  3, 4, 5,  6, 7, 9};
	EXPECT_EQ(O, वर्गफलम्_Matrix3f(O_check_दत्त_3));

	O = वर्गफलम्_Matrix3f(दत्त);
	O.col(1) += Vector3f{1, -2, 3};
	भिन्न O_check_दत्त_4 [9] = {0, 3, 3,  4, 3, 6,  7, 11, 10};
	EXPECT_EQ(O, वर्गफलम्_Matrix3f(O_check_दत्त_4));

	O = वर्गफलम्_Matrix3f(दत्त);
	O.खण्डिका<2, 2>(1, 0) -= सङ्कार्यम्;
	भिन्न O_check_दत्त_5 [9] = {0, 2, 3,  2, 4, 6,  10, 9, 10};
	EXPECT_EQ(O, वर्गफलम्_Matrix3f(O_check_दत्त_5));

	O = वर्गफलम्_Matrix3f(दत्त);
	O.खण्डिका<2, 1>(1, 1) -= सङ्कार्यम्.खण्डिका<2, 1>(0, 0);
	भिन्न O_check_दत्त_6 [9] = {0, 2, 3,  4, 3, 6,  7, 11, 10};
	EXPECT_EQ(O, वर्गफलम्_Matrix3f(O_check_दत्त_6));

	O = वर्गफलम्_Matrix3f(दत्त);
	O.खण्डिका<3, 3>(0, 0) -= -1;
	भिन्न O_check_दत्त_7 [9] = {1, 3, 4,  5, 6, 7,  8, 9, 11};
	EXPECT_EQ(O, वर्गफलम्_Matrix3f(O_check_दत्त_7));

	O = वर्गफलम्_Matrix3f(दत्त);
	O.col(1) -= Vector3f{1, -2, 3};
	भिन्न O_check_दत्त_8 [9] = {0, 1, 3,  4, 7, 6,  7, 5, 10};
	EXPECT_EQ(O, वर्गफलम्_Matrix3f(O_check_दत्त_8));

	O = वर्गफलम्_Matrix3f(दत्त);
	O.खण्डिका<2, 1>(1, 1) *= 5.f;
	भिन्न O_check_दत्त_9 [9] = {0, 2, 3,  4, 25, 6,  7, 40, 10};
	EXPECT_EQ(O, वर्गफलम्_Matrix3f(O_check_दत्त_9));

	O = वर्गफलम्_Matrix3f(दत्त);
	O.खण्डिका<2, 1>(1, 1) /= 2.f;
	भिन्न O_check_दत्त_10 [9] = {0, 2, 3,  4, 2.5, 6,  7, 4, 10};
	EXPECT_EQ(O, वर्गफलम्_Matrix3f(O_check_दत्त_10));

	// Different operations
	O = वर्गफलम्_Matrix3f(दत्त);
	वर्गफलम्_Matrix<भिन्न, 2> res_11(O.खण्डिका<2, 2>(1, 1) * 2.f);
	भिन्न O_check_दत्त_11 [4] = {10, 12, 16, 20};
	EXPECT_EQ(res_11, (वर्गफलम्_Matrix<भिन्न, 2>(O_check_दत्त_11)));

	O = वर्गफलम्_Matrix3f(दत्त);
	वर्गफलम्_Matrix<भिन्न, 2> res_12(O.खण्डिका<2, 2>(1, 1) / 2.f);
	भिन्न O_check_दत्त_12 [4] = {2.5, 3, 4, 5};
	EXPECT_EQ(res_12, (वर्गफलम्_Matrix<भिन्न, 2>(O_check_दत्त_12)));
}
TEST(Matrix_खण्डिका_Test, खण्डिका_Additions)
{
	भिन्न दत्त[9] = {0, 2, 3,
			 4, 5, 6,
			 7, 8, 10
			};
	वर्गफलम्_Matrix3f A{दत्त};

	भिन्न सङ्कार्यम्_दत्त [4] = {2, 1,
				  -3, -1
				 };
	ध्रुव वर्गफलम्_Matrix2f सङ्कार्यम्(सङ्कार्यम्_दत्त);

	// 2x2 Slice + 2x2 Matrix
	वर्गफलम्_Matrix2f res_1 = A.खण्डिका<2, 2>(1, 0) + सङ्कार्यम्;
	भिन्न res_1_check_दत्त[4] = {6, 6,
				     4, 7
				    };
	EXPECT_EQ(res_1, (वर्गफलम्_Matrix2f(res_1_check_दत्त)));

	// 2x1 Slice + 2x1 Slice
	Vector2f res_2 = A.खण्डिका<2, 1>(1, 1) + सङ्कार्यम्.खण्डिका<2, 1>(0, 0);
	EXPECT_EQ(res_2, Vector2f(7, 5));

	// 3x3 Slice + Scalar
	वर्गफलम्_Matrix3f res_3 = A.खण्डिका<3, 3>(0, 0) + (-1);
	भिन्न res_3_check_दत्त[9] = {-1, 1, 2,
				     3, 4, 5,
				     6, 7, 9
				    };
	EXPECT_EQ(res_3, (वर्गफलम्_Matrix3f(res_3_check_दत्त)));

	// 3x1 Slice + 3 Vector
	Vector3f res_4 = A.col(1) + Vector3f(1, -2, 3);
	EXPECT_EQ(res_4, Vector3f(3, 3, 11));

}
TEST(Matrix_खण्डिका_Test, खण्डिका_Subtractions)
{
	भिन्न दत्त[9] = {0, 2, 3,
			 4, 5, 6,
			 7, 8, 10
			};
	वर्गफलम्_Matrix3f A{दत्त};

	भिन्न सङ्कार्यम्_दत्त[4] = {2, 1,
				 -3, -1
				};
	ध्रुव वर्गफलम्_Matrix2f सङ्कार्यम्(सङ्कार्यम्_दत्त);

	// 2x2 Slice - 2x2 Matrix
	वर्गफलम्_Matrix2f res_1 = A.खण्डिका<2, 2>(1, 0) - सङ्कार्यम्;
	भिन्न res_1_check_दत्त[4] = {2, 4,
				     10, 9
				    };
	EXPECT_EQ(res_1, (वर्गफलम्_Matrix2f(res_1_check_दत्त)));

	// 2x1 Slice - 2x1 Slice
	Vector2f res_2 = A.खण्डिका<2, 1>(1, 1) - सङ्कार्यम्.खण्डिका<2, 1>(0, 0);
	EXPECT_EQ(res_2, Vector2f(3, 11));

	// 3x3 Slice - Scalar
	वर्गफलम्_Matrix3f res_3 = A.खण्डिका<3, 3>(0, 0) - (-1);
	भिन्न res_3_check_दत्त[9] = {1, 3, 4,
				     5, 6, 7,
				     8, 9, 11
				    };
	EXPECT_EQ(res_3, (वर्गफलम्_Matrix3f(res_3_check_दत्त)));

	// 3x1 Slice - 3 Vector
	Vector3f res_4 = A.col(1) - Vector3f(1, -2, 3);
	EXPECT_EQ(res_4, Vector3f(1, 7, 5));
}

TEST(Matrix_खण्डिका_Test, XYAssignmentTest)
{
	Vector3f a(1, 2, 3);
	Vector3f b(4, 5, 6);
	// Assign first two elements from b to first two slot of a
	a.xy() = b.xy();
	EXPECT_EQ(a, Vector3f(4, 5, 3));
}

TEST(Matrix_खण्डिका_Test, Dot)
{
	भिन्न दत्त[9] = {0, 2, 3,
			 4, 5, 6,
			 7, 8, 10
			};
	वर्गफलम्_Matrix3f A(दत्त);

	// row . row, within the same matrix
	EXPECT_FLOAT_EQ(A.row(1).dot(A.row(2)), 4.f * 7.f + 5.f * 8.f + 6.f * 10.f);

	// col . col, within the same matrix
	EXPECT_FLOAT_EQ(A.col(0).dot(A.col(1)), 0.f * 2.f + 4.f * 5.f + 7.f * 8.f);

	// self dot matches norm_squared()
	EXPECT_FLOAT_EQ(A.row(1).dot(A.row(1)), A.row(1).norm_squared());
}
