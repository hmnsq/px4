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

#ਅਨ੍ਤਰ੍ਭਾਵ <gtest/gtest.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <matrix/math.hpp>

ਵਰਤੋਂ ਨਾਮਾਕਾਸ਼ ਮੈਟ੍ਰਿਕਸ;

TEST(ਮੈਟ੍ਰਿਕਸ_ਟੁਕੜਾ_Test, ਟੁਕੜਾ_2)
{
	ਦਸ਼ਮਲਵ ਅੰਕੜੇ[9] = {0, 2, 3,
			 4, 5, 6,
			 7, 8, 10
			};
	ਵਰਗਫਲ_ਮੈਟ੍ਰਿਕਸ<ਦਸ਼ਮਲਵ, 3> A(ਅੰਕੜੇ);

	// Test row slicing
	ਮੈਟ੍ਰਿਕਸ_2<ਦਸ਼ਮਲਵ, 2, 3> B_rowslice(A.ਟੁਕੜਾ<2, 3>(1, 0));
	ਦਸ਼ਮਲਵ ਅੰਕੜੇ_check_rowslice[6] = {
		4, 5, 6,
		7, 8, 10
	};
	ਮੈਟ੍ਰਿਕਸ_2<ਦਸ਼ਮਲਵ, 2, 3> B_check_rowslice(ਅੰਕੜੇ_check_rowslice);
	EXPECT_EQ(B_rowslice, B_check_rowslice);

	// Test column slicing
	ਮੈਟ੍ਰਿਕਸ_2<ਦਸ਼ਮਲਵ, 3, 2> B_colslice(A.ਟੁਕੜਾ<3, 2>(0, 1));
	ਦਸ਼ਮਲਵ ਅੰਕੜੇ_check_colslice[6] = {
		2, 3,
		5, 6,
		8, 10
	};
	ਮੈਟ੍ਰਿਕਸ_2<ਦਸ਼ਮਲਵ, 3, 2> B_check_colslice(ਅੰਕੜੇ_check_colslice);
	EXPECT_EQ(B_colslice, B_check_colslice);

	// Test slicing both
	ਮੈਟ੍ਰਿਕਸ_2<ਦਸ਼ਮਲਵ, 2, 2> B_bothslice(A.ਟੁਕੜਾ<2, 2>(1, 1));
	ਦਸ਼ਮਲਵ ਅੰਕੜੇ_check_bothslice[4] = {
		5, 6,
		8, 10
	};
	ਮੈਟ੍ਰਿਕਸ_2<ਦਸ਼ਮਲਵ, 2, 2> B_check_bothslice(ਅੰਕੜੇ_check_bothslice);
	EXPECT_EQ(B_bothslice, B_check_bothslice);

	//Test block writing
	ਦਸ਼ਮਲਵ ਅੰਕੜੇ_2_2[4] = {
		11, 12,
		13, 14
	};

	ਮੈਟ੍ਰਿਕਸ_2<ਦਸ਼ਮਲਵ, 2, 2> C(ਅੰਕੜੇ_2_2);
	A.ਟੁਕੜਾ<2, 2>(1, 1) = C;

	ਦਸ਼ਮਲਵ ਅੰਕੜੇ_2_check[9] = {
		0, 2, 3,
		4, 11, 12,
		7, 13, 14
	};
	ਮੈਟ੍ਰਿਕਸ_2<ਦਸ਼ਮਲਵ, 3, 3> D(ਅੰਕੜੇ_2_check);
	EXPECT_EQ(A, D);

	//Test writing to slices
	ਮੈਟ੍ਰਿਕਸ_2<ਦਸ਼ਮਲਵ, 3, 1> E;
	E(0, 0) = -1;
	E(1, 0) = 1;
	E(2, 0) = 3;

	ਮੈਟ੍ਰਿਕਸ_2<ਦਸ਼ਮਲਵ, 2, 1> F;
	F(0, 0) = 9;
	F(1, 0) = 11;

	E.ਟੁਕੜਾ<2, 1>(0, 0) = F;

	ਦਸ਼ਮਲਵ ਅੰਕੜੇ_3_check[3] = {9, 11, 3};
	ਮੈਟ੍ਰਿਕਸ_2<ਦਸ਼ਮਲਵ, 3, 1> G(ਅੰਕੜੇ_3_check);
	EXPECT_EQ(E, G);
	EXPECT_EQ(E, (ਮੈਟ੍ਰਿਕਸ_2<ਦਸ਼ਮਲਵ, 3, 1>(E.ਟੁਕੜਾ<3, 1>(0, 0))));

	ਮੈਟ੍ਰਿਕਸ_2<ਦਸ਼ਮਲਵ, 2, 1> H = E.ਟੁਕੜਾ<2, 1>(0, 0);
	EXPECT_EQ(H, F);

	ਦਸ਼ਮਲਵ ਅੰਕੜੇ_4_check[5] = {3, 11, 9, 0, 0};
	{
		// assigning row slices to each other
		ਅਚਲ ਮੈਟ੍ਰਿਕਸ_2<ਦਸ਼ਮਲਵ, 3, 1> J(ਅੰਕੜੇ_3_check);
		ਮੈਟ੍ਰਿਕਸ_2<ਦਸ਼ਮਲਵ, 5, 1> K;
		K.ਪੰਕਤੀ(2) = J.ਪੰਕਤੀ(0);
		K.ਪੰਕਤੀ(1) = J.ਪੰਕਤੀ(1);
		K.ਪੰਕਤੀ(0) = J.ਪੰਕਤੀ(2);

		ਮੈਟ੍ਰਿਕਸ_2<ਦਸ਼ਮਲਵ, 5, 1> K_check(ਅੰਕੜੇ_4_check);
		EXPECT_EQ(K, K_check);
	}
	{
		// assigning col slices to each other
		ਅਚਲ ਮੈਟ੍ਰਿਕਸ_2<ਦਸ਼ਮਲਵ, 1, 3> J(ਅੰਕੜੇ_3_check);
		ਮੈਟ੍ਰਿਕਸ_2<ਦਸ਼ਮਲਵ, 1, 5> K;
		K.ਖਾਨਾ(2) = J.ਖਾਨਾ(0);
		K.ਖਾਨਾ(1) = J.ਖਾਨਾ(1);
		K.ਖਾਨਾ(0) = J.ਖਾਨਾ(2);

		ਮੈਟ੍ਰਿਕਸ_2<ਦਸ਼ਮਲਵ, 1, 5> K_check(ਅੰਕੜੇ_4_check);
		EXPECT_EQ(K, K_check);
	}

	// check that slice of a slice works for reading
	ਅਚਲ ਮੈਟ੍ਰਿਕਸ_2<ਦਸ਼ਮਲਵ, 3, 3> cm33(ਅੰਕੜੇ);
	ਮੈਟ੍ਰਿਕਸ_2<ਦਸ਼ਮਲਵ, 2, 1> ਸਿਖਰ_ਸੱਜਾ = cm33.ਟੁਕੜਾ<2, 3>(0, 0).ਟੁਕੜਾ<2, 1>(0, 2);
	ਦਸ਼ਮਲਵ ਸਿਖਰ_ਸੱਜਾ_check[2] = {3, 6};
	EXPECT_EQ(ਸਿਖਰ_ਸੱਜਾ, (ਮੈਟ੍ਰਿਕਸ_2<ਦਸ਼ਮਲਵ, 2, 1>(ਸਿਖਰ_ਸੱਜਾ_check)));

	// check that slice of a slice works for writing
	ਮੈਟ੍ਰਿਕਸ_2<ਦਸ਼ਮਲਵ, 3, 3> m33(ਅੰਕੜੇ);
	m33.ਟੁਕੜਾ<2, 3>(0, 0).ਟੁਕੜਾ<2, 1>(0, 2) = ਮੈਟ੍ਰਿਕਸ_2<ਦਸ਼ਮਲਵ, 2, 1>();
	ਅਚਲ ਦਸ਼ਮਲਵ ਅੰਕੜੇ_check[9] = {0, 2, 0,
				     4, 5, 0,
				     7, 8, 10
				    };
	EXPECT_EQ(m33, (ਮੈਟ੍ਰਿਕਸ_2<ਦਸ਼ਮਲਵ, 3, 3>(ਅੰਕੜੇ_check)));

	// longerThan
	Vector3f v5;
	v5(0) = 3;
	v5(1) = 4;
	v5(2) = 9;
	EXPECT_TRUE(v5.xy().longerThan(4.99f));
	EXPECT_FALSE(v5.xy().longerThan(5.f));
	EXPECT_FLOAT_EQ(v5.xy().norm(), 5.f);

	// min/max
	EXPECT_FLOAT_EQ(m33.ਪੰਕਤੀ(1).max(), 5.f);
	EXPECT_FLOAT_EQ(m33.ਖਾਨਾ(0).min(), 0.f);
	EXPECT_FLOAT_EQ((m33.ਟੁਕੜਾ<2, 2>(1, 1).max()), 10.f);

	// assign scalar value to slice
	ਮੈਟ੍ਰਿਕਸ_2<ਦਸ਼ਮਲਵ, 3, 1> L;
	L(0, 0) = -1;
	L(1, 0) = 1;
	L(2, 0) = 3;

	L.ਟੁਕੜਾ<2, 1>(0, 0) = 0.0f;

	ਦਸ਼ਮਲਵ ਅੰਕੜੇ_5_check[3] = {0, 0, 3};
	ਮੈਟ੍ਰਿਕਸ_2<ਦਸ਼ਮਲਵ, 3, 1> M(ਅੰਕੜੇ_5_check);
	EXPECT_EQ(L, M);

	// return diagonal elements
	ਦਸ਼ਮਲਵ ਅੰਕੜੇ_6[9] = {0, 2, 3,
			   4, 5, 6,
			   7, 8, 10
			  };
	ਵਰਗਫਲ_ਮੈਟ੍ਰਿਕਸ<ਦਸ਼ਮਲਵ, 3> N(ਅੰਕੜੇ_6);

	Vector3f v6 = N.ਟੁਕੜਾ<3, 3>(0, 0).diag();
	Vector3f v6_check = {0, 5, 10};
	EXPECT_EQ(v6, v6_check);
	Vector2f v7 = N.ਟੁਕੜਾ<2, 3>(1, 0).diag();
	Vector2f v7_check = {4, 8};
	EXPECT_EQ(v7, v7_check);
	Vector2f v8 = N.ਟੁਕੜਾ<3, 2>(0, 1).diag();
	Vector2f v8_check = {2, 6};
	EXPECT_EQ(v8, v8_check);
	Vector2f v9(N.ਟੁਕੜਾ<1, 2>(1, 1));
	Vector2f v9_check = {5, 6};
	EXPECT_EQ(v9, v9_check);
	Vector3f v10(N.ਟੁਕੜਾ<1, 3>(1, 0));
	Vector3f v10_check = {4, 5, 6};
	EXPECT_EQ(v10, v10_check);

	// Different assignment operators
	ਵਰਗਫਲ_Matrix3f O(ਅੰਕੜੇ);
	ਦਸ਼ਮਲਵ ਸੰਕਾਰਜ_ਅੰਕੜੇ [4] = {2, 1, -3, -1};
	ਅਚਲ ਵਰਗਫਲ_ਮੈਟ੍ਰਿਕਸ<ਦਸ਼ਮਲਵ, 2> ਸੰਕਾਰਜ(ਸੰਕਾਰਜ_ਅੰਕੜੇ);

	O.ਟੁਕੜਾ<2, 2>(1, 0) += ਸੰਕਾਰਜ;
	ਦਸ਼ਮਲਵ O_check_ਅੰਕੜੇ_1 [9] = {0, 2, 3,  6, 6, 6,  4, 7, 10};
	EXPECT_EQ(O, ਵਰਗਫਲ_Matrix3f(O_check_ਅੰਕੜੇ_1));

	O = ਵਰਗਫਲ_Matrix3f(ਅੰਕੜੇ);
	O.ਟੁਕੜਾ<2, 1>(1, 1) += ਸੰਕਾਰਜ.ਟੁਕੜਾ<2, 1>(0, 0);
	ਦਸ਼ਮਲਵ O_check_ਅੰਕੜੇ_2 [9] = {0, 2, 3,  4, 7, 6,  7, 5, 10};
	EXPECT_EQ(O, ਵਰਗਫਲ_Matrix3f(O_check_ਅੰਕੜੇ_2));

	O = ਵਰਗਫਲ_Matrix3f(ਅੰਕੜੇ);
	O.ਟੁਕੜਾ<3, 3>(0, 0) += -1;
	ਦਸ਼ਮਲਵ O_check_ਅੰਕੜੇ_3 [9] = {-1, 1, 2,  3, 4, 5,  6, 7, 9};
	EXPECT_EQ(O, ਵਰਗਫਲ_Matrix3f(O_check_ਅੰਕੜੇ_3));

	O = ਵਰਗਫਲ_Matrix3f(ਅੰਕੜੇ);
	O.ਖਾਨਾ(1) += Vector3f{1, -2, 3};
	ਦਸ਼ਮਲਵ O_check_ਅੰਕੜੇ_4 [9] = {0, 3, 3,  4, 3, 6,  7, 11, 10};
	EXPECT_EQ(O, ਵਰਗਫਲ_Matrix3f(O_check_ਅੰਕੜੇ_4));

	O = ਵਰਗਫਲ_Matrix3f(ਅੰਕੜੇ);
	O.ਟੁਕੜਾ<2, 2>(1, 0) -= ਸੰਕਾਰਜ;
	ਦਸ਼ਮਲਵ O_check_ਅੰਕੜੇ_5 [9] = {0, 2, 3,  2, 4, 6,  10, 9, 10};
	EXPECT_EQ(O, ਵਰਗਫਲ_Matrix3f(O_check_ਅੰਕੜੇ_5));

	O = ਵਰਗਫਲ_Matrix3f(ਅੰਕੜੇ);
	O.ਟੁਕੜਾ<2, 1>(1, 1) -= ਸੰਕਾਰਜ.ਟੁਕੜਾ<2, 1>(0, 0);
	ਦਸ਼ਮਲਵ O_check_ਅੰਕੜੇ_6 [9] = {0, 2, 3,  4, 3, 6,  7, 11, 10};
	EXPECT_EQ(O, ਵਰਗਫਲ_Matrix3f(O_check_ਅੰਕੜੇ_6));

	O = ਵਰਗਫਲ_Matrix3f(ਅੰਕੜੇ);
	O.ਟੁਕੜਾ<3, 3>(0, 0) -= -1;
	ਦਸ਼ਮਲਵ O_check_ਅੰਕੜੇ_7 [9] = {1, 3, 4,  5, 6, 7,  8, 9, 11};
	EXPECT_EQ(O, ਵਰਗਫਲ_Matrix3f(O_check_ਅੰਕੜੇ_7));

	O = ਵਰਗਫਲ_Matrix3f(ਅੰਕੜੇ);
	O.ਖਾਨਾ(1) -= Vector3f{1, -2, 3};
	ਦਸ਼ਮਲਵ O_check_ਅੰਕੜੇ_8 [9] = {0, 1, 3,  4, 7, 6,  7, 5, 10};
	EXPECT_EQ(O, ਵਰਗਫਲ_Matrix3f(O_check_ਅੰਕੜੇ_8));

	O = ਵਰਗਫਲ_Matrix3f(ਅੰਕੜੇ);
	O.ਟੁਕੜਾ<2, 1>(1, 1) *= 5.f;
	ਦਸ਼ਮਲਵ O_check_ਅੰਕੜੇ_9 [9] = {0, 2, 3,  4, 25, 6,  7, 40, 10};
	EXPECT_EQ(O, ਵਰਗਫਲ_Matrix3f(O_check_ਅੰਕੜੇ_9));

	O = ਵਰਗਫਲ_Matrix3f(ਅੰਕੜੇ);
	O.ਟੁਕੜਾ<2, 1>(1, 1) /= 2.f;
	ਦਸ਼ਮਲਵ O_check_ਅੰਕੜੇ_10 [9] = {0, 2, 3,  4, 2.5, 6,  7, 4, 10};
	EXPECT_EQ(O, ਵਰਗਫਲ_Matrix3f(O_check_ਅੰਕੜੇ_10));

	// Different operations
	O = ਵਰਗਫਲ_Matrix3f(ਅੰਕੜੇ);
	ਵਰਗਫਲ_ਮੈਟ੍ਰਿਕਸ<ਦਸ਼ਮਲਵ, 2> res_11(O.ਟੁਕੜਾ<2, 2>(1, 1) * 2.f);
	ਦਸ਼ਮਲਵ O_check_ਅੰਕੜੇ_11 [4] = {10, 12, 16, 20};
	EXPECT_EQ(res_11, (ਵਰਗਫਲ_ਮੈਟ੍ਰਿਕਸ<ਦਸ਼ਮਲਵ, 2>(O_check_ਅੰਕੜੇ_11)));

	O = ਵਰਗਫਲ_Matrix3f(ਅੰਕੜੇ);
	ਵਰਗਫਲ_ਮੈਟ੍ਰਿਕਸ<ਦਸ਼ਮਲਵ, 2> res_12(O.ਟੁਕੜਾ<2, 2>(1, 1) / 2.f);
	ਦਸ਼ਮਲਵ O_check_ਅੰਕੜੇ_12 [4] = {2.5, 3, 4, 5};
	EXPECT_EQ(res_12, (ਵਰਗਫਲ_ਮੈਟ੍ਰਿਕਸ<ਦਸ਼ਮਲਵ, 2>(O_check_ਅੰਕੜੇ_12)));
}
TEST(ਮੈਟ੍ਰਿਕਸ_ਟੁਕੜਾ_Test, ਟੁਕੜਾ_Additions)
{
	ਦਸ਼ਮਲਵ ਅੰਕੜੇ[9] = {0, 2, 3,
			 4, 5, 6,
			 7, 8, 10
			};
	ਵਰਗਫਲ_Matrix3f A{ਅੰਕੜੇ};

	ਦਸ਼ਮਲਵ ਸੰਕਾਰਜ_ਅੰਕੜੇ [4] = {2, 1,
				  -3, -1
				 };
	ਅਚਲ ਵਰਗਫਲ_Matrix2f ਸੰਕਾਰਜ(ਸੰਕਾਰਜ_ਅੰਕੜੇ);

	// 2x2 Slice + 2x2 Matrix
	ਵਰਗਫਲ_Matrix2f res_1 = A.ਟੁਕੜਾ<2, 2>(1, 0) + ਸੰਕਾਰਜ;
	ਦਸ਼ਮਲਵ res_1_check_ਅੰਕੜੇ[4] = {6, 6,
				     4, 7
				    };
	EXPECT_EQ(res_1, (ਵਰਗਫਲ_Matrix2f(res_1_check_ਅੰਕੜੇ)));

	// 2x1 Slice + 2x1 Slice
	Vector2f res_2 = A.ਟੁਕੜਾ<2, 1>(1, 1) + ਸੰਕਾਰਜ.ਟੁਕੜਾ<2, 1>(0, 0);
	EXPECT_EQ(res_2, Vector2f(7, 5));

	// 3x3 Slice + Scalar
	ਵਰਗਫਲ_Matrix3f res_3 = A.ਟੁਕੜਾ<3, 3>(0, 0) + (-1);
	ਦਸ਼ਮਲਵ res_3_check_ਅੰਕੜੇ[9] = {-1, 1, 2,
				     3, 4, 5,
				     6, 7, 9
				    };
	EXPECT_EQ(res_3, (ਵਰਗਫਲ_Matrix3f(res_3_check_ਅੰਕੜੇ)));

	// 3x1 Slice + 3 Vector
	Vector3f res_4 = A.ਖਾਨਾ(1) + Vector3f(1, -2, 3);
	EXPECT_EQ(res_4, Vector3f(3, 3, 11));

}
TEST(ਮੈਟ੍ਰਿਕਸ_ਟੁਕੜਾ_Test, ਟੁਕੜਾ_Subtractions)
{
	ਦਸ਼ਮਲਵ ਅੰਕੜੇ[9] = {0, 2, 3,
			 4, 5, 6,
			 7, 8, 10
			};
	ਵਰਗਫਲ_Matrix3f A{ਅੰਕੜੇ};

	ਦਸ਼ਮਲਵ ਸੰਕਾਰਜ_ਅੰਕੜੇ[4] = {2, 1,
				 -3, -1
				};
	ਅਚਲ ਵਰਗਫਲ_Matrix2f ਸੰਕਾਰਜ(ਸੰਕਾਰਜ_ਅੰਕੜੇ);

	// 2x2 Slice - 2x2 Matrix
	ਵਰਗਫਲ_Matrix2f res_1 = A.ਟੁਕੜਾ<2, 2>(1, 0) - ਸੰਕਾਰਜ;
	ਦਸ਼ਮਲਵ res_1_check_ਅੰਕੜੇ[4] = {2, 4,
				     10, 9
				    };
	EXPECT_EQ(res_1, (ਵਰਗਫਲ_Matrix2f(res_1_check_ਅੰਕੜੇ)));

	// 2x1 Slice - 2x1 Slice
	Vector2f res_2 = A.ਟੁਕੜਾ<2, 1>(1, 1) - ਸੰਕਾਰਜ.ਟੁਕੜਾ<2, 1>(0, 0);
	EXPECT_EQ(res_2, Vector2f(3, 11));

	// 3x3 Slice - Scalar
	ਵਰਗਫਲ_Matrix3f res_3 = A.ਟੁਕੜਾ<3, 3>(0, 0) - (-1);
	ਦਸ਼ਮਲਵ res_3_check_ਅੰਕੜੇ[9] = {1, 3, 4,
				     5, 6, 7,
				     8, 9, 11
				    };
	EXPECT_EQ(res_3, (ਵਰਗਫਲ_Matrix3f(res_3_check_ਅੰਕੜੇ)));

	// 3x1 Slice - 3 Vector
	Vector3f res_4 = A.ਖਾਨਾ(1) - Vector3f(1, -2, 3);
	EXPECT_EQ(res_4, Vector3f(1, 7, 5));
}

TEST(ਮੈਟ੍ਰਿਕਸ_ਟੁਕੜਾ_Test, XYAssignmentTest)
{
	Vector3f a(1, 2, 3);
	Vector3f b(4, 5, 6);
	// Assign first two elements from b to first two slot of a
	a.xy() = b.xy();
	EXPECT_EQ(a, Vector3f(4, 5, 3));
}

TEST(ਮੈਟ੍ਰਿਕਸ_ਟੁਕੜਾ_Test, Dot)
{
	ਦਸ਼ਮਲਵ ਅੰਕੜੇ[9] = {0, 2, 3,
			 4, 5, 6,
			 7, 8, 10
			};
	ਵਰਗਫਲ_Matrix3f A(ਅੰਕੜੇ);

	// row . row, within the same matrix
	EXPECT_FLOAT_EQ(A.ਪੰਕਤੀ(1).dot(A.ਪੰਕਤੀ(2)), 4.f * 7.f + 5.f * 8.f + 6.f * 10.f);

	// col . col, within the same matrix
	EXPECT_FLOAT_EQ(A.ਖਾਨਾ(0).dot(A.ਖਾਨਾ(1)), 0.f * 2.f + 4.f * 5.f + 7.f * 8.f);

	// self dot matches norm_squared()
	EXPECT_FLOAT_EQ(A.ਪੰਕਤੀ(1).dot(A.ਪੰਕਤੀ(1)), A.ਪੰਕਤੀ(1).norm_squared());
}
