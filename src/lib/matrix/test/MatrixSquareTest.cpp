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

TEST(ಮಾತೃಕೆ_ವರ್ಗಫಲ_Test, ವರ್ಗಫಲ_2)
{
	ದಶಮಾಂಶ ದತ್ತಾಂಶ[9] = {1, 2, 3,
			 4, 5, 6,
			 7, 8, 10
			};
	ವರ್ಗಫಲ_ಮಾತೃಕೆ<ದಶಮಾಂಶ, 3> A(ದತ್ತಾಂಶ);
	Vector3<ದಶಮಾಂಶ> diag_check(1, 5, 10);

	EXPECT_EQ(A.diag(), diag_check);
	EXPECT_FLOAT_EQ(A.trace(), 16);
	EXPECT_FLOAT_EQ(A.trace<2>(1), 15);

	ದಶಮಾಂಶ ದತ್ತಾಂಶ_check[9] = {
		1.01158503f,  0.02190432f,  0.03238144f,
		0.04349195f,  1.05428524f,  0.06539627f,
		0.07576783f,  0.08708946f,  1.10894048f
	};

	ದಶಮಾಂಶ dt = 0.01f;
	ವರ್ಗಫಲ_ಮಾತೃಕೆ<ದಶಮಾಂಶ, 3> eA = expm(ವರ್ಗಫಲ_ಮಾತೃಕೆ<ದಶಮಾಂಶ, 3>(A * dt), 5);
	ವರ್ಗಫಲ_ಮಾತೃಕೆ<ದಶಮಾಂಶ, 3> eA_check(ದತ್ತಾಂಶ_check);
	EXPECT_TRUE(isEqual(eA, eA_check, 1e-3f));

	ವರ್ಗಫಲ_ಮಾತೃಕೆ<ದಶಮಾಂಶ, 2> A_bottomright = A.ತುಂಡು<2, 2>(1, 1);
	ವರ್ಗಫಲ_ಮಾತೃಕೆ<ದಶಮಾಂಶ, 2> A_bottomright2;
	A_bottomright2 = A.ತುಂಡು<2, 2>(1, 1);

	ದಶಮಾಂಶ ದತ್ತಾಂಶ_bottomright[4] = {5, 6,
				     8, 10
				    };
	ವರ್ಗಫಲ_ಮಾತೃಕೆ<ದಶಮಾಂಶ, 2> bottomright_check(ದತ್ತಾಂಶ_bottomright);
	EXPECT_EQ(A_bottomright, bottomright_check);
	EXPECT_EQ(A_bottomright2, bottomright_check);

	// test diagonal functions
	ದಶಮಾಂಶ ದತ್ತಾಂಶ_4x4[16] = {1, 2, 3, 4,
			      5, 6, 7, 8,
			      9, 10, 11, 12,
			      13, 14, 15, 16
			     };

	ವರ್ಗಫಲ_ಮಾತೃಕೆ<ದಶಮಾಂಶ, 4> B(ದತ್ತಾಂಶ_4x4);
	B.uncorrelateCovariance<1>(1);
	ದಶಮಾಂಶ ದತ್ತಾಂಶ_B_check[16] = {1, 0, 3, 4,
				  0, 6, 0, 0,
				  9, 0, 11, 12,
				  13, 0, 15, 16
				 };
	ವರ್ಗಫಲ_ಮಾತೃಕೆ<ದಶಮಾಂಶ, 4> B_check(ದತ್ತಾಂಶ_B_check);
	EXPECT_EQ(B, B_check);

	ವರ್ಗಫಲ_ಮಾತೃಕೆ<ದಶಮಾಂಶ, 4> C(ದತ್ತಾಂಶ_4x4);
	C.uncorrelateCovariance<2>(1);
	ದಶಮಾಂಶ ದತ್ತಾಂಶ_C_check[16] = {1, 0, 0, 4,
				  0, 6, 0, 0,
				  0, 0, 11, 0,
				  13, 0, 0, 16
				 };
	ವರ್ಗಫಲ_ಮಾತೃಕೆ<ದಶಮಾಂಶ, 4> C_check(ದತ್ತಾಂಶ_C_check);
	EXPECT_EQ(C, C_check);

	ವರ್ಗಫಲ_ಮಾತೃಕೆ<ದಶಮಾಂಶ, 4> D(ದತ್ತಾಂಶ_4x4);
	D.uncorrelate_Covariance_ಗಣ_Variance<2>(0, Vector2f{20, 21});
	ದಶಮಾಂಶ ದತ್ತಾಂಶ_D_check[16] = {20, 0, 0, 0,
				  0, 21, 0, 0,
				  0, 0, 11, 12,
				  0, 0, 15, 16
				 };
	ವರ್ಗಫಲ_ಮಾತೃಕೆ<ದಶಮಾಂಶ, 4> D_check(ದತ್ತಾಂಶ_D_check);
	EXPECT_EQ(D, D_check);

	ವರ್ಗಫಲ_ಮಾತೃಕೆ<ದಶಮಾಂಶ, 4> E(ದತ್ತಾಂಶ_4x4);
	E.uncorrelate_Covariance_ಗಣ_Variance<3>(1, 33);
	ದಶಮಾಂಶ ದತ್ತಾಂಶ_E_check[16] = {1, 0, 0, 0,
				  0, 33, 0, 0,
				  0, 0, 33, 0,
				  0, 0, 0, 33
				 };
	ವರ್ಗಫಲ_ಮಾತೃಕೆ<ದಶಮಾಂಶ, 4> E_check(ದತ್ತಾಂಶ_E_check);
	EXPECT_EQ(E, E_check);

	ವರ್ಗಫಲ_ಮಾತೃಕೆ<ದಶಮಾಂಶ, 4> A_ಖಂಡ(ದತ್ತಾಂಶ_4x4);
	A_ಖಂಡ.uncorrelate_Covariance_ಖಂಡ<2>(1);
	ದಶಮಾಂಶ ದತ್ತಾಂಶ_A_ಖಂಡ_check[16] = {1, 0, 0, 4,
					0, 6, 7, 0,
					0, 10, 11, 0,
					13, 0, 0, 16
				       };
	ವರ್ಗಫಲ_ಮಾತೃಕೆ<ದಶಮಾಂಶ, 4> A_ಖಂಡ_check(ದತ್ತಾಂಶ_A_ಖಂಡ_check);
	EXPECT_EQ(A_ಖಂಡ, A_ಖಂಡ_check);

	// test symmetric functions
	ವರ್ಗಫಲ_ಮಾತೃಕೆ<ದಶಮಾಂಶ, 4> F(ದತ್ತಾಂಶ_4x4);
	F.make_ಖಂಡ_ಸಮಮಿತ<2>(1);
	ದಶಮಾಂಶ ದತ್ತಾಂಶ_F_check[16] = {1, 2, 3, 4,
				  5, 6, 8.5, 8,
				  9, 8.5, 11, 12,
				  13, 14, 15, 16
				 };
	ವರ್ಗಫಲ_ಮಾತೃಕೆ<ದಶಮಾಂಶ, 4> F_check(ದತ್ತಾಂಶ_F_check);
	EXPECT_EQ(F, F_check);
	EXPECT_TRUE(F.is_ಖಂಡ_ಸಮಮಿತ<2>(1));
	EXPECT_FALSE(F.is_ಅಡ್ಡಸಾಲು_ಲಂಬಸಾಲು_ಸಮಮಿತ<2>(1));

	ವರ್ಗಫಲ_ಮಾತೃಕೆ<ದಶಮಾಂಶ, 4> G(ದತ್ತಾಂಶ_4x4);
	G.make_ಅಡ್ಡಸಾಲು_ಲಂಬಸಾಲು_ಸಮಮಿತ<2>(1);
	ದಶಮಾಂಶ ದತ್ತಾಂಶ_G_check[16] = {1, 3.5, 6, 4,
				  3.5, 6, 8.5, 11,
				  6, 8.5, 11, 13.5,
				  13, 11, 13.5, 16
				 };
	ವರ್ಗಫಲ_ಮಾತೃಕೆ<ದಶಮಾಂಶ, 4> G_check(ದತ್ತಾಂಶ_G_check);
	EXPECT_EQ(G, G_check);
	EXPECT_TRUE(G.is_ಖಂಡ_ಸಮಮಿತ<2>(1));
	EXPECT_TRUE(G.is_ಅಡ್ಡಸಾಲು_ಲಂಬಸಾಲು_ಸಮಮಿತ<2>(1));

	ವರ್ಗಫಲ_ಮಾತೃಕೆ<ದಶಮಾಂಶ, 4> H(ದತ್ತಾಂಶ_4x4);
	H.make_ಖಂಡ_ಸಮಮಿತ<1>(1);
	ದಶಮಾಂಶ ದತ್ತಾಂಶ_H_check[16] = {1, 2, 3, 4,
				  5, 6, 7, 8,
				  9, 10, 11, 12,
				  13, 14, 15, 16
				 };
	ವರ್ಗಫಲ_ಮಾತೃಕೆ<ದಶಮಾಂಶ, 4> H_check(ದತ್ತಾಂಶ_H_check);
	EXPECT_EQ(H, H_check);
	EXPECT_TRUE(H.is_ಖಂಡ_ಸಮಮಿತ<1>(1));
	EXPECT_FALSE(H.is_ಅಡ್ಡಸಾಲು_ಲಂಬಸಾಲು_ಸಮಮಿತ<1>(1));

	ವರ್ಗಫಲ_ಮಾತೃಕೆ<ದಶಮಾಂಶ, 4> J(ದತ್ತಾಂಶ_4x4);
	J.make_ಅಡ್ಡಸಾಲು_ಲಂಬಸಾಲು_ಸಮಮಿತ<1>(1);
	ದಶಮಾಂಶ ದತ್ತಾಂಶ_J_check[16] = {1, 3.5, 3, 4,
				  3.5, 6, 8.5, 11,
				  9, 8.5, 11, 12,
				  13, 11, 15, 16
				 };
	ವರ್ಗಫಲ_ಮಾತೃಕೆ<ದಶಮಾಂಶ, 4> J_check(ದತ್ತಾಂಶ_J_check);
	EXPECT_EQ(J, J_check);
	EXPECT_TRUE(J.is_ಖಂಡ_ಸಮಮಿತ<1>(1));
	EXPECT_TRUE(J.is_ಅಡ್ಡಸಾಲು_ಲಂಬಸಾಲು_ಸಮಮಿತ<1>(1));
	EXPECT_FALSE(J.is_ಖಂಡ_ಸಮಮಿತ<3>(1));

	ದಶಮಾಂಶ ದತ್ತಾಂಶ_K[16] = {1, 2, 3, 4,
			    2, 3, 4, 11,
			    3, 4, 11, 12,
			    4, 11, 15, 16
			   };
	ವರ್ಗಫಲ_ಮಾತೃಕೆ<ದಶಮಾಂಶ, 4> K(ದತ್ತಾಂಶ_K);
	EXPECT_FALSE(K.is_ಅಡ್ಡಸಾಲು_ಲಂಬಸಾಲು_ಸಮಮಿತ<1>(2));

	ದಶಮಾಂಶ ದತ್ತಾಂಶ_L[16] = {1, 0, 0, 0,
			    2, 3, 0, 0,
			    3, 4, 11, 0,
			    4, 11, 15, 16
			   };
	ದಶಮಾಂಶ ದತ್ತಾಂಶ_L_check[16] = {1, 2, 3, 4,
				  2, 3, 4, 11,
				  3, 4, 11, 15,
				  4, 11, 15, 16
				 };
	ವರ್ಗಫಲ_ಮಾತೃಕೆ<ದಶಮಾಂಶ, 4> L(ದತ್ತಾಂಶ_L);
	L.copy_ಅಧೋ_ಗಮ್ಯ_ಊರ್ಧ್ವ_Triangle();
	ವರ್ಗಫಲ_ಮಾತೃಕೆ<ದಶಮಾಂಶ, 4> L_check(ದತ್ತಾಂಶ_L_check);
	EXPECT_EQ(L, L_check);

	ದಶಮಾಂಶ ದತ್ತಾಂಶ_M[16] = {1, 2, 3, 4,
			    0, 3, 4, 11,
			    0, 0, 11, 15,
			    0, 0, 0, 16
			   };
	ವರ್ಗಫಲ_ಮಾತೃಕೆ<ದಶಮಾಂಶ, 4> M(ದತ್ತಾಂಶ_M);
	M.copy_ಊರ್ಧ್ವ_ಗಮ್ಯ_ಅಧೋ_Triangle();
	EXPECT_EQ(M, L_check);
}
