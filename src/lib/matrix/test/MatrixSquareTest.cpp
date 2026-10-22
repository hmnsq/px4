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

TEST(Matrix_वर्गफलम्_Test, वर्गफलम्_2)
{
	भिन्न दत्त[9] = {1, 2, 3,
			 4, 5, 6,
			 7, 8, 10
			};
	वर्गफलम्_Matrix<भिन्न, 3> A(दत्त);
	Vector3<भिन्न> diag_check(1, 5, 10);

	EXPECT_EQ(A.diag(), diag_check);
	EXPECT_FLOAT_EQ(A.trace(), 16);
	EXPECT_FLOAT_EQ(A.trace<2>(1), 15);

	भिन्न दत्त_check[9] = {
		1.01158503f,  0.02190432f,  0.03238144f,
		0.04349195f,  1.05428524f,  0.06539627f,
		0.07576783f,  0.08708946f,  1.10894048f
	};

	भिन्न dt = 0.01f;
	वर्गफलम्_Matrix<भिन्न, 3> eA = expm(वर्गफलम्_Matrix<भिन्न, 3>(A * dt), 5);
	वर्गफलम्_Matrix<भिन्न, 3> eA_check(दत्त_check);
	EXPECT_TRUE(isEqual(eA, eA_check, 1e-3f));

	वर्गफलम्_Matrix<भिन्न, 2> A_bottomright = A.खण्डिका<2, 2>(1, 1);
	वर्गफलम्_Matrix<भिन्न, 2> A_bottomright2;
	A_bottomright2 = A.खण्डिका<2, 2>(1, 1);

	भिन्न दत्त_bottomright[4] = {5, 6,
				     8, 10
				    };
	वर्गफलम्_Matrix<भिन्न, 2> bottomright_check(दत्त_bottomright);
	EXPECT_EQ(A_bottomright, bottomright_check);
	EXPECT_EQ(A_bottomright2, bottomright_check);

	// test diagonal functions
	भिन्न दत्त_4x4[16] = {1, 2, 3, 4,
			      5, 6, 7, 8,
			      9, 10, 11, 12,
			      13, 14, 15, 16
			     };

	वर्गफलम्_Matrix<भिन्न, 4> B(दत्त_4x4);
	B.uncorrelateCovariance<1>(1);
	भिन्न दत्त_B_check[16] = {1, 0, 3, 4,
				  0, 6, 0, 0,
				  9, 0, 11, 12,
				  13, 0, 15, 16
				 };
	वर्गफलम्_Matrix<भिन्न, 4> B_check(दत्त_B_check);
	EXPECT_EQ(B, B_check);

	वर्गफलम्_Matrix<भिन्न, 4> C(दत्त_4x4);
	C.uncorrelateCovariance<2>(1);
	भिन्न दत्त_C_check[16] = {1, 0, 0, 4,
				  0, 6, 0, 0,
				  0, 0, 11, 0,
				  13, 0, 0, 16
				 };
	वर्गफलम्_Matrix<भिन्न, 4> C_check(दत्त_C_check);
	EXPECT_EQ(C, C_check);

	वर्गफलम्_Matrix<भिन्न, 4> D(दत्त_4x4);
	D.uncorrelate_Covariance_समुच्चयः_Variance<2>(0, Vector2f{20, 21});
	भिन्न दत्त_D_check[16] = {20, 0, 0, 0,
				  0, 21, 0, 0,
				  0, 0, 11, 12,
				  0, 0, 15, 16
				 };
	वर्गफलम्_Matrix<भिन्न, 4> D_check(दत्त_D_check);
	EXPECT_EQ(D, D_check);

	वर्गफलम्_Matrix<भिन्न, 4> E(दत्त_4x4);
	E.uncorrelate_Covariance_समुच्चयः_Variance<3>(1, 33);
	भिन्न दत्त_E_check[16] = {1, 0, 0, 0,
				  0, 33, 0, 0,
				  0, 0, 33, 0,
				  0, 0, 0, 33
				 };
	वर्गफलम्_Matrix<भिन्न, 4> E_check(दत्त_E_check);
	EXPECT_EQ(E, E_check);

	वर्गफलम्_Matrix<भिन्न, 4> A_खण्डम्(दत्त_4x4);
	A_खण्डम्.uncorrelate_Covariance_खण्डम्<2>(1);
	भिन्न दत्त_A_खण्डम्_check[16] = {1, 0, 0, 4,
					0, 6, 7, 0,
					0, 10, 11, 0,
					13, 0, 0, 16
				       };
	वर्गफलम्_Matrix<भिन्न, 4> A_खण्डम्_check(दत्त_A_खण्डम्_check);
	EXPECT_EQ(A_खण्डम्, A_खण्डम्_check);

	// test symmetric functions
	वर्गफलम्_Matrix<भिन्न, 4> F(दत्त_4x4);
	F.make_खण्डम्_सममितम्<2>(1);
	भिन्न दत्त_F_check[16] = {1, 2, 3, 4,
				  5, 6, 8.5, 8,
				  9, 8.5, 11, 12,
				  13, 14, 15, 16
				 };
	वर्गफलम्_Matrix<भिन्न, 4> F_check(दत्त_F_check);
	EXPECT_EQ(F, F_check);
	EXPECT_TRUE(F.is_खण्डम्_सममितम्<2>(1));
	EXPECT_FALSE(F.is_Row_Col_सममितम्<2>(1));

	वर्गफलम्_Matrix<भिन्न, 4> G(दत्त_4x4);
	G.make_Row_Col_सममितम्<2>(1);
	भिन्न दत्त_G_check[16] = {1, 3.5, 6, 4,
				  3.5, 6, 8.5, 11,
				  6, 8.5, 11, 13.5,
				  13, 11, 13.5, 16
				 };
	वर्गफलम्_Matrix<भिन्न, 4> G_check(दत्त_G_check);
	EXPECT_EQ(G, G_check);
	EXPECT_TRUE(G.is_खण्डम्_सममितम्<2>(1));
	EXPECT_TRUE(G.is_Row_Col_सममितम्<2>(1));

	वर्गफलम्_Matrix<भिन्न, 4> H(दत्त_4x4);
	H.make_खण्डम्_सममितम्<1>(1);
	भिन्न दत्त_H_check[16] = {1, 2, 3, 4,
				  5, 6, 7, 8,
				  9, 10, 11, 12,
				  13, 14, 15, 16
				 };
	वर्गफलम्_Matrix<भिन्न, 4> H_check(दत्त_H_check);
	EXPECT_EQ(H, H_check);
	EXPECT_TRUE(H.is_खण्डम्_सममितम्<1>(1));
	EXPECT_FALSE(H.is_Row_Col_सममितम्<1>(1));

	वर्गफलम्_Matrix<भिन्न, 4> J(दत्त_4x4);
	J.make_Row_Col_सममितम्<1>(1);
	भिन्न दत्त_J_check[16] = {1, 3.5, 3, 4,
				  3.5, 6, 8.5, 11,
				  9, 8.5, 11, 12,
				  13, 11, 15, 16
				 };
	वर्गफलम्_Matrix<भिन्न, 4> J_check(दत्त_J_check);
	EXPECT_EQ(J, J_check);
	EXPECT_TRUE(J.is_खण्डम्_सममितम्<1>(1));
	EXPECT_TRUE(J.is_Row_Col_सममितम्<1>(1));
	EXPECT_FALSE(J.is_खण्डम्_सममितम्<3>(1));

	भिन्न दत्त_K[16] = {1, 2, 3, 4,
			    2, 3, 4, 11,
			    3, 4, 11, 12,
			    4, 11, 15, 16
			   };
	वर्गफलम्_Matrix<भिन्न, 4> K(दत्त_K);
	EXPECT_FALSE(K.is_Row_Col_सममितम्<1>(2));

	भिन्न दत्त_L[16] = {1, 0, 0, 0,
			    2, 3, 0, 0,
			    3, 4, 11, 0,
			    4, 11, 15, 16
			   };
	भिन्न दत्त_L_check[16] = {1, 2, 3, 4,
				  2, 3, 4, 11,
				  3, 4, 11, 15,
				  4, 11, 15, 16
				 };
	वर्गफलम्_Matrix<भिन्न, 4> L(दत्त_L);
	L.copy_अधस्तनम्_गन्तव्यम्_ऊर्ध्वस्थम्_Triangle();
	वर्गफलम्_Matrix<भिन्न, 4> L_check(दत्त_L_check);
	EXPECT_EQ(L, L_check);

	भिन्न दत्त_M[16] = {1, 2, 3, 4,
			    0, 3, 4, 11,
			    0, 0, 11, 15,
			    0, 0, 0, 16
			   };
	वर्गफलम्_Matrix<भिन्न, 4> M(दत्त_M);
	M.copy_ऊर्ध्वस्थम्_गन्तव्यम्_अधस्तनम्_Triangle();
	EXPECT_EQ(M, L_check);
}
