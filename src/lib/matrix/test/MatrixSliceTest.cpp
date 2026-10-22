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

TEST(ಮಾತೃಕೆ_ತುಂಡು_Test, ತುಂಡು_2)
{
	ದಶಮಾಂಶ ದತ್ತಾಂಶ[9] = {0, 2, 3,
			 4, 5, 6,
			 7, 8, 10
			};
	ವರ್ಗಫಲ_ಮಾತೃಕೆ<ದಶಮಾಂಶ, 3> A(ದತ್ತಾಂಶ);

	// Test row slicing
	ಮಾತೃಕೆ_2<ದಶಮಾಂಶ, 2, 3> B_rowslice(A.ತುಂಡು<2, 3>(1, 0));
	ದಶಮಾಂಶ ದತ್ತಾಂಶ_check_rowslice[6] = {
		4, 5, 6,
		7, 8, 10
	};
	ಮಾತೃಕೆ_2<ದಶಮಾಂಶ, 2, 3> B_check_rowslice(ದತ್ತಾಂಶ_check_rowslice);
	EXPECT_EQ(B_rowslice, B_check_rowslice);

	// Test column slicing
	ಮಾತೃಕೆ_2<ದಶಮಾಂಶ, 3, 2> B_colslice(A.ತುಂಡು<3, 2>(0, 1));
	ದಶಮಾಂಶ ದತ್ತಾಂಶ_check_colslice[6] = {
		2, 3,
		5, 6,
		8, 10
	};
	ಮಾತೃಕೆ_2<ದಶಮಾಂಶ, 3, 2> B_check_colslice(ದತ್ತಾಂಶ_check_colslice);
	EXPECT_EQ(B_colslice, B_check_colslice);

	// Test slicing both
	ಮಾತೃಕೆ_2<ದಶಮಾಂಶ, 2, 2> B_bothslice(A.ತುಂಡು<2, 2>(1, 1));
	ದಶಮಾಂಶ ದತ್ತಾಂಶ_check_bothslice[4] = {
		5, 6,
		8, 10
	};
	ಮಾತೃಕೆ_2<ದಶಮಾಂಶ, 2, 2> B_check_bothslice(ದತ್ತಾಂಶ_check_bothslice);
	EXPECT_EQ(B_bothslice, B_check_bothslice);

	//Test block writing
	ದಶಮಾಂಶ ದತ್ತಾಂಶ_2_2[4] = {
		11, 12,
		13, 14
	};

	ಮಾತೃಕೆ_2<ದಶಮಾಂಶ, 2, 2> C(ದತ್ತಾಂಶ_2_2);
	A.ತುಂಡು<2, 2>(1, 1) = C;

	ದಶಮಾಂಶ ದತ್ತಾಂಶ_2_check[9] = {
		0, 2, 3,
		4, 11, 12,
		7, 13, 14
	};
	ಮಾತೃಕೆ_2<ದಶಮಾಂಶ, 3, 3> D(ದತ್ತಾಂಶ_2_check);
	EXPECT_EQ(A, D);

	//Test writing to slices
	ಮಾತೃಕೆ_2<ದಶಮಾಂಶ, 3, 1> E;
	E(0, 0) = -1;
	E(1, 0) = 1;
	E(2, 0) = 3;

	ಮಾತೃಕೆ_2<ದಶಮಾಂಶ, 2, 1> F;
	F(0, 0) = 9;
	F(1, 0) = 11;

	E.ತುಂಡು<2, 1>(0, 0) = F;

	ದಶಮಾಂಶ ದತ್ತಾಂಶ_3_check[3] = {9, 11, 3};
	ಮಾತೃಕೆ_2<ದಶಮಾಂಶ, 3, 1> G(ದತ್ತಾಂಶ_3_check);
	EXPECT_EQ(E, G);
	EXPECT_EQ(E, (ಮಾತೃಕೆ_2<ದಶಮಾಂಶ, 3, 1>(E.ತುಂಡು<3, 1>(0, 0))));

	ಮಾತೃಕೆ_2<ದಶಮಾಂಶ, 2, 1> H = E.ತುಂಡು<2, 1>(0, 0);
	EXPECT_EQ(H, F);

	ದಶಮಾಂಶ ದತ್ತಾಂಶ_4_check[5] = {3, 11, 9, 0, 0};
	{
		// assigning row slices to each other
		ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ_2<ದಶಮಾಂಶ, 3, 1> J(ದತ್ತಾಂಶ_3_check);
		ಮಾತೃಕೆ_2<ದಶಮಾಂಶ, 5, 1> K;
		K.ಅಡ್ಡಸಾಲು(2) = J.ಅಡ್ಡಸಾಲು(0);
		K.ಅಡ್ಡಸಾಲು(1) = J.ಅಡ್ಡಸಾಲು(1);
		K.ಅಡ್ಡಸಾಲು(0) = J.ಅಡ್ಡಸಾಲು(2);

		ಮಾತೃಕೆ_2<ದಶಮಾಂಶ, 5, 1> K_check(ದತ್ತಾಂಶ_4_check);
		EXPECT_EQ(K, K_check);
	}
	{
		// assigning col slices to each other
		ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ_2<ದಶಮಾಂಶ, 1, 3> J(ದತ್ತಾಂಶ_3_check);
		ಮಾತೃಕೆ_2<ದಶಮಾಂಶ, 1, 5> K;
		K.ಲಂಬಸಾಲು(2) = J.ಲಂಬಸಾಲು(0);
		K.ಲಂಬಸಾಲು(1) = J.ಲಂಬಸಾಲು(1);
		K.ಲಂಬಸಾಲು(0) = J.ಲಂಬಸಾಲು(2);

		ಮಾತೃಕೆ_2<ದಶಮಾಂಶ, 1, 5> K_check(ದತ್ತಾಂಶ_4_check);
		EXPECT_EQ(K, K_check);
	}

	// check that slice of a slice works for reading
	ಸ್ಥಿರಾಂಕ ಮಾತೃಕೆ_2<ದಶಮಾಂಶ, 3, 3> cm33(ದತ್ತಾಂಶ);
	ಮಾತೃಕೆ_2<ದಶಮಾಂಶ, 2, 1> ತುದಿ_ಬಲ = cm33.ತುಂಡು<2, 3>(0, 0).ತುಂಡು<2, 1>(0, 2);
	ದಶಮಾಂಶ ತುದಿ_ಬಲ_check[2] = {3, 6};
	EXPECT_EQ(ತುದಿ_ಬಲ, (ಮಾತೃಕೆ_2<ದಶಮಾಂಶ, 2, 1>(ತುದಿ_ಬಲ_check)));

	// check that slice of a slice works for writing
	ಮಾತೃಕೆ_2<ದಶಮಾಂಶ, 3, 3> m33(ದತ್ತಾಂಶ);
	m33.ತುಂಡು<2, 3>(0, 0).ತುಂಡು<2, 1>(0, 2) = ಮಾತೃಕೆ_2<ದಶಮಾಂಶ, 2, 1>();
	ಸ್ಥಿರಾಂಕ ದಶಮಾಂಶ ದತ್ತಾಂಶ_check[9] = {0, 2, 0,
				     4, 5, 0,
				     7, 8, 10
				    };
	EXPECT_EQ(m33, (ಮಾತೃಕೆ_2<ದಶಮಾಂಶ, 3, 3>(ದತ್ತಾಂಶ_check)));

	// longerThan
	Vector3f v5;
	v5(0) = 3;
	v5(1) = 4;
	v5(2) = 9;
	EXPECT_TRUE(v5.xy().longerThan(4.99f));
	EXPECT_FALSE(v5.xy().longerThan(5.f));
	EXPECT_FLOAT_EQ(v5.xy().norm(), 5.f);

	// min/max
	EXPECT_FLOAT_EQ(m33.ಅಡ್ಡಸಾಲು(1).max(), 5.f);
	EXPECT_FLOAT_EQ(m33.ಲಂಬಸಾಲು(0).min(), 0.f);
	EXPECT_FLOAT_EQ((m33.ತುಂಡು<2, 2>(1, 1).max()), 10.f);

	// assign scalar value to slice
	ಮಾತೃಕೆ_2<ದಶಮಾಂಶ, 3, 1> L;
	L(0, 0) = -1;
	L(1, 0) = 1;
	L(2, 0) = 3;

	L.ತುಂಡು<2, 1>(0, 0) = 0.0f;

	ದಶಮಾಂಶ ದತ್ತಾಂಶ_5_check[3] = {0, 0, 3};
	ಮಾತೃಕೆ_2<ದಶಮಾಂಶ, 3, 1> M(ದತ್ತಾಂಶ_5_check);
	EXPECT_EQ(L, M);

	// return diagonal elements
	ದಶಮಾಂಶ ದತ್ತಾಂಶ_6[9] = {0, 2, 3,
			   4, 5, 6,
			   7, 8, 10
			  };
	ವರ್ಗಫಲ_ಮಾತೃಕೆ<ದಶಮಾಂಶ, 3> N(ದತ್ತಾಂಶ_6);

	Vector3f v6 = N.ತುಂಡು<3, 3>(0, 0).diag();
	Vector3f v6_check = {0, 5, 10};
	EXPECT_EQ(v6, v6_check);
	Vector2f v7 = N.ತುಂಡು<2, 3>(1, 0).diag();
	Vector2f v7_check = {4, 8};
	EXPECT_EQ(v7, v7_check);
	Vector2f v8 = N.ತುಂಡು<3, 2>(0, 1).diag();
	Vector2f v8_check = {2, 6};
	EXPECT_EQ(v8, v8_check);
	Vector2f v9(N.ತುಂಡು<1, 2>(1, 1));
	Vector2f v9_check = {5, 6};
	EXPECT_EQ(v9, v9_check);
	Vector3f v10(N.ತುಂಡು<1, 3>(1, 0));
	Vector3f v10_check = {4, 5, 6};
	EXPECT_EQ(v10, v10_check);

	// Different assignment operators
	ವರ್ಗಫಲ_Matrix3f O(ದತ್ತಾಂಶ);
	ದಶಮಾಂಶ ಕಾರ್ಯಾಂಶ_ದತ್ತಾಂಶ [4] = {2, 1, -3, -1};
	ಸ್ಥಿರಾಂಕ ವರ್ಗಫಲ_ಮಾತೃಕೆ<ದಶಮಾಂಶ, 2> ಕಾರ್ಯಾಂಶ(ಕಾರ್ಯಾಂಶ_ದತ್ತಾಂಶ);

	O.ತುಂಡು<2, 2>(1, 0) += ಕಾರ್ಯಾಂಶ;
	ದಶಮಾಂಶ O_check_ದತ್ತಾಂಶ_1 [9] = {0, 2, 3,  6, 6, 6,  4, 7, 10};
	EXPECT_EQ(O, ವರ್ಗಫಲ_Matrix3f(O_check_ದತ್ತಾಂಶ_1));

	O = ವರ್ಗಫಲ_Matrix3f(ದತ್ತಾಂಶ);
	O.ತುಂಡು<2, 1>(1, 1) += ಕಾರ್ಯಾಂಶ.ತುಂಡು<2, 1>(0, 0);
	ದಶಮಾಂಶ O_check_ದತ್ತಾಂಶ_2 [9] = {0, 2, 3,  4, 7, 6,  7, 5, 10};
	EXPECT_EQ(O, ವರ್ಗಫಲ_Matrix3f(O_check_ದತ್ತಾಂಶ_2));

	O = ವರ್ಗಫಲ_Matrix3f(ದತ್ತಾಂಶ);
	O.ತುಂಡು<3, 3>(0, 0) += -1;
	ದಶಮಾಂಶ O_check_ದತ್ತಾಂಶ_3 [9] = {-1, 1, 2,  3, 4, 5,  6, 7, 9};
	EXPECT_EQ(O, ವರ್ಗಫಲ_Matrix3f(O_check_ದತ್ತಾಂಶ_3));

	O = ವರ್ಗಫಲ_Matrix3f(ದತ್ತಾಂಶ);
	O.ಲಂಬಸಾಲು(1) += Vector3f{1, -2, 3};
	ದಶಮಾಂಶ O_check_ದತ್ತಾಂಶ_4 [9] = {0, 3, 3,  4, 3, 6,  7, 11, 10};
	EXPECT_EQ(O, ವರ್ಗಫಲ_Matrix3f(O_check_ದತ್ತಾಂಶ_4));

	O = ವರ್ಗಫಲ_Matrix3f(ದತ್ತಾಂಶ);
	O.ತುಂಡು<2, 2>(1, 0) -= ಕಾರ್ಯಾಂಶ;
	ದಶಮಾಂಶ O_check_ದತ್ತಾಂಶ_5 [9] = {0, 2, 3,  2, 4, 6,  10, 9, 10};
	EXPECT_EQ(O, ವರ್ಗಫಲ_Matrix3f(O_check_ದತ್ತಾಂಶ_5));

	O = ವರ್ಗಫಲ_Matrix3f(ದತ್ತಾಂಶ);
	O.ತುಂಡು<2, 1>(1, 1) -= ಕಾರ್ಯಾಂಶ.ತುಂಡು<2, 1>(0, 0);
	ದಶಮಾಂಶ O_check_ದತ್ತಾಂಶ_6 [9] = {0, 2, 3,  4, 3, 6,  7, 11, 10};
	EXPECT_EQ(O, ವರ್ಗಫಲ_Matrix3f(O_check_ದತ್ತಾಂಶ_6));

	O = ವರ್ಗಫಲ_Matrix3f(ದತ್ತಾಂಶ);
	O.ತುಂಡು<3, 3>(0, 0) -= -1;
	ದಶಮಾಂಶ O_check_ದತ್ತಾಂಶ_7 [9] = {1, 3, 4,  5, 6, 7,  8, 9, 11};
	EXPECT_EQ(O, ವರ್ಗಫಲ_Matrix3f(O_check_ದತ್ತಾಂಶ_7));

	O = ವರ್ಗಫಲ_Matrix3f(ದತ್ತಾಂಶ);
	O.ಲಂಬಸಾಲು(1) -= Vector3f{1, -2, 3};
	ದಶಮಾಂಶ O_check_ದತ್ತಾಂಶ_8 [9] = {0, 1, 3,  4, 7, 6,  7, 5, 10};
	EXPECT_EQ(O, ವರ್ಗಫಲ_Matrix3f(O_check_ದತ್ತಾಂಶ_8));

	O = ವರ್ಗಫಲ_Matrix3f(ದತ್ತಾಂಶ);
	O.ತುಂಡು<2, 1>(1, 1) *= 5.f;
	ದಶಮಾಂಶ O_check_ದತ್ತಾಂಶ_9 [9] = {0, 2, 3,  4, 25, 6,  7, 40, 10};
	EXPECT_EQ(O, ವರ್ಗಫಲ_Matrix3f(O_check_ದತ್ತಾಂಶ_9));

	O = ವರ್ಗಫಲ_Matrix3f(ದತ್ತಾಂಶ);
	O.ತುಂಡು<2, 1>(1, 1) /= 2.f;
	ದಶಮಾಂಶ O_check_ದತ್ತಾಂಶ_10 [9] = {0, 2, 3,  4, 2.5, 6,  7, 4, 10};
	EXPECT_EQ(O, ವರ್ಗಫಲ_Matrix3f(O_check_ದತ್ತಾಂಶ_10));

	// Different operations
	O = ವರ್ಗಫಲ_Matrix3f(ದತ್ತಾಂಶ);
	ವರ್ಗಫಲ_ಮಾತೃಕೆ<ದಶಮಾಂಶ, 2> res_11(O.ತುಂಡು<2, 2>(1, 1) * 2.f);
	ದಶಮಾಂಶ O_check_ದತ್ತಾಂಶ_11 [4] = {10, 12, 16, 20};
	EXPECT_EQ(res_11, (ವರ್ಗಫಲ_ಮಾತೃಕೆ<ದಶಮಾಂಶ, 2>(O_check_ದತ್ತಾಂಶ_11)));

	O = ವರ್ಗಫಲ_Matrix3f(ದತ್ತಾಂಶ);
	ವರ್ಗಫಲ_ಮಾತೃಕೆ<ದಶಮಾಂಶ, 2> res_12(O.ತುಂಡು<2, 2>(1, 1) / 2.f);
	ದಶಮಾಂಶ O_check_ದತ್ತಾಂಶ_12 [4] = {2.5, 3, 4, 5};
	EXPECT_EQ(res_12, (ವರ್ಗಫಲ_ಮಾತೃಕೆ<ದಶಮಾಂಶ, 2>(O_check_ದತ್ತಾಂಶ_12)));
}
TEST(ಮಾತೃಕೆ_ತುಂಡು_Test, ತುಂಡು_Additions)
{
	ದಶಮಾಂಶ ದತ್ತಾಂಶ[9] = {0, 2, 3,
			 4, 5, 6,
			 7, 8, 10
			};
	ವರ್ಗಫಲ_Matrix3f A{ದತ್ತಾಂಶ};

	ದಶಮಾಂಶ ಕಾರ್ಯಾಂಶ_ದತ್ತಾಂಶ [4] = {2, 1,
				  -3, -1
				 };
	ಸ್ಥಿರಾಂಕ ವರ್ಗಫಲ_Matrix2f ಕಾರ್ಯಾಂಶ(ಕಾರ್ಯಾಂಶ_ದತ್ತಾಂಶ);

	// 2x2 Slice + 2x2 Matrix
	ವರ್ಗಫಲ_Matrix2f res_1 = A.ತುಂಡು<2, 2>(1, 0) + ಕಾರ್ಯಾಂಶ;
	ದಶಮಾಂಶ res_1_check_ದತ್ತಾಂಶ[4] = {6, 6,
				     4, 7
				    };
	EXPECT_EQ(res_1, (ವರ್ಗಫಲ_Matrix2f(res_1_check_ದತ್ತಾಂಶ)));

	// 2x1 Slice + 2x1 Slice
	Vector2f res_2 = A.ತುಂಡು<2, 1>(1, 1) + ಕಾರ್ಯಾಂಶ.ತುಂಡು<2, 1>(0, 0);
	EXPECT_EQ(res_2, Vector2f(7, 5));

	// 3x3 Slice + Scalar
	ವರ್ಗಫಲ_Matrix3f res_3 = A.ತುಂಡು<3, 3>(0, 0) + (-1);
	ದಶಮಾಂಶ res_3_check_ದತ್ತಾಂಶ[9] = {-1, 1, 2,
				     3, 4, 5,
				     6, 7, 9
				    };
	EXPECT_EQ(res_3, (ವರ್ಗಫಲ_Matrix3f(res_3_check_ದತ್ತಾಂಶ)));

	// 3x1 Slice + 3 Vector
	Vector3f res_4 = A.ಲಂಬಸಾಲು(1) + Vector3f(1, -2, 3);
	EXPECT_EQ(res_4, Vector3f(3, 3, 11));

}
TEST(ಮಾತೃಕೆ_ತುಂಡು_Test, ತುಂಡು_Subtractions)
{
	ದಶಮಾಂಶ ದತ್ತಾಂಶ[9] = {0, 2, 3,
			 4, 5, 6,
			 7, 8, 10
			};
	ವರ್ಗಫಲ_Matrix3f A{ದತ್ತಾಂಶ};

	ದಶಮಾಂಶ ಕಾರ್ಯಾಂಶ_ದತ್ತಾಂಶ[4] = {2, 1,
				 -3, -1
				};
	ಸ್ಥಿರಾಂಕ ವರ್ಗಫಲ_Matrix2f ಕಾರ್ಯಾಂಶ(ಕಾರ್ಯಾಂಶ_ದತ್ತಾಂಶ);

	// 2x2 Slice - 2x2 Matrix
	ವರ್ಗಫಲ_Matrix2f res_1 = A.ತುಂಡು<2, 2>(1, 0) - ಕಾರ್ಯಾಂಶ;
	ದಶಮಾಂಶ res_1_check_ದತ್ತಾಂಶ[4] = {2, 4,
				     10, 9
				    };
	EXPECT_EQ(res_1, (ವರ್ಗಫಲ_Matrix2f(res_1_check_ದತ್ತಾಂಶ)));

	// 2x1 Slice - 2x1 Slice
	Vector2f res_2 = A.ತುಂಡು<2, 1>(1, 1) - ಕಾರ್ಯಾಂಶ.ತುಂಡು<2, 1>(0, 0);
	EXPECT_EQ(res_2, Vector2f(3, 11));

	// 3x3 Slice - Scalar
	ವರ್ಗಫಲ_Matrix3f res_3 = A.ತುಂಡು<3, 3>(0, 0) - (-1);
	ದಶಮಾಂಶ res_3_check_ದತ್ತಾಂಶ[9] = {1, 3, 4,
				     5, 6, 7,
				     8, 9, 11
				    };
	EXPECT_EQ(res_3, (ವರ್ಗಫಲ_Matrix3f(res_3_check_ದತ್ತಾಂಶ)));

	// 3x1 Slice - 3 Vector
	Vector3f res_4 = A.ಲಂಬಸಾಲು(1) - Vector3f(1, -2, 3);
	EXPECT_EQ(res_4, Vector3f(1, 7, 5));
}

TEST(ಮಾತೃಕೆ_ತುಂಡು_Test, XYAssignmentTest)
{
	Vector3f a(1, 2, 3);
	Vector3f b(4, 5, 6);
	// Assign first two elements from b to first two slot of a
	a.xy() = b.xy();
	EXPECT_EQ(a, Vector3f(4, 5, 3));
}

TEST(ಮಾತೃಕೆ_ತುಂಡು_Test, Dot)
{
	ದಶಮಾಂಶ ದತ್ತಾಂಶ[9] = {0, 2, 3,
			 4, 5, 6,
			 7, 8, 10
			};
	ವರ್ಗಫಲ_Matrix3f A(ದತ್ತಾಂಶ);

	// row . row, within the same matrix
	EXPECT_FLOAT_EQ(A.ಅಡ್ಡಸಾಲು(1).dot(A.ಅಡ್ಡಸಾಲು(2)), 4.f * 7.f + 5.f * 8.f + 6.f * 10.f);

	// col . col, within the same matrix
	EXPECT_FLOAT_EQ(A.ಲಂಬಸಾಲು(0).dot(A.ಲಂಬಸಾಲು(1)), 0.f * 2.f + 4.f * 5.f + 7.f * 8.f);

	// self dot matches norm_squared()
	EXPECT_FLOAT_EQ(A.ಅಡ್ಡಸಾಲು(1).dot(A.ಅಡ್ಡಸಾಲು(1)), A.ಅಡ್ಡಸಾಲು(1).norm_squared());
}
