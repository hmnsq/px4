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

TEST(ਮੈਟ੍ਰਿਕਸ_ਵਰਗਫਲ_Test, ਵਰਗਫਲ_2)
{
	ਦਸ਼ਮਲਵ ਅੰਕੜੇ[9] = {1, 2, 3,
			 4, 5, 6,
			 7, 8, 10
			};
	ਵਰਗਫਲ_ਮੈਟ੍ਰਿਕਸ<ਦਸ਼ਮਲਵ, 3> A(ਅੰਕੜੇ);
	Vector3<ਦਸ਼ਮਲਵ> diag_check(1, 5, 10);

	EXPECT_EQ(A.diag(), diag_check);
	EXPECT_FLOAT_EQ(A.trace(), 16);
	EXPECT_FLOAT_EQ(A.trace<2>(1), 15);

	ਦਸ਼ਮਲਵ ਅੰਕੜੇ_check[9] = {
		1.01158503f,  0.02190432f,  0.03238144f,
		0.04349195f,  1.05428524f,  0.06539627f,
		0.07576783f,  0.08708946f,  1.10894048f
	};

	ਦਸ਼ਮਲਵ dt = 0.01f;
	ਵਰਗਫਲ_ਮੈਟ੍ਰਿਕਸ<ਦਸ਼ਮਲਵ, 3> eA = expm(ਵਰਗਫਲ_ਮੈਟ੍ਰਿਕਸ<ਦਸ਼ਮਲਵ, 3>(A * dt), 5);
	ਵਰਗਫਲ_ਮੈਟ੍ਰਿਕਸ<ਦਸ਼ਮਲਵ, 3> eA_check(ਅੰਕੜੇ_check);
	EXPECT_TRUE(isEqual(eA, eA_check, 1e-3f));

	ਵਰਗਫਲ_ਮੈਟ੍ਰਿਕਸ<ਦਸ਼ਮਲਵ, 2> A_bottomright = A.ਟੁਕੜਾ<2, 2>(1, 1);
	ਵਰਗਫਲ_ਮੈਟ੍ਰਿਕਸ<ਦਸ਼ਮਲਵ, 2> A_bottomright2;
	A_bottomright2 = A.ਟੁਕੜਾ<2, 2>(1, 1);

	ਦਸ਼ਮਲਵ ਅੰਕੜੇ_bottomright[4] = {5, 6,
				     8, 10
				    };
	ਵਰਗਫਲ_ਮੈਟ੍ਰਿਕਸ<ਦਸ਼ਮਲਵ, 2> bottomright_check(ਅੰਕੜੇ_bottomright);
	EXPECT_EQ(A_bottomright, bottomright_check);
	EXPECT_EQ(A_bottomright2, bottomright_check);

	// test diagonal functions
	ਦਸ਼ਮਲਵ ਅੰਕੜੇ_4x4[16] = {1, 2, 3, 4,
			      5, 6, 7, 8,
			      9, 10, 11, 12,
			      13, 14, 15, 16
			     };

	ਵਰਗਫਲ_ਮੈਟ੍ਰਿਕਸ<ਦਸ਼ਮਲਵ, 4> B(ਅੰਕੜੇ_4x4);
	B.uncorrelateCovariance<1>(1);
	ਦਸ਼ਮਲਵ ਅੰਕੜੇ_B_check[16] = {1, 0, 3, 4,
				  0, 6, 0, 0,
				  9, 0, 11, 12,
				  13, 0, 15, 16
				 };
	ਵਰਗਫਲ_ਮੈਟ੍ਰਿਕਸ<ਦਸ਼ਮਲਵ, 4> B_check(ਅੰਕੜੇ_B_check);
	EXPECT_EQ(B, B_check);

	ਵਰਗਫਲ_ਮੈਟ੍ਰਿਕਸ<ਦਸ਼ਮਲਵ, 4> C(ਅੰਕੜੇ_4x4);
	C.uncorrelateCovariance<2>(1);
	ਦਸ਼ਮਲਵ ਅੰਕੜੇ_C_check[16] = {1, 0, 0, 4,
				  0, 6, 0, 0,
				  0, 0, 11, 0,
				  13, 0, 0, 16
				 };
	ਵਰਗਫਲ_ਮੈਟ੍ਰਿਕਸ<ਦਸ਼ਮਲਵ, 4> C_check(ਅੰਕੜੇ_C_check);
	EXPECT_EQ(C, C_check);

	ਵਰਗਫਲ_ਮੈਟ੍ਰਿਕਸ<ਦਸ਼ਮਲਵ, 4> D(ਅੰਕੜੇ_4x4);
	D.uncorrelate_Covariance_ਸੈੱਟ_Variance<2>(0, Vector2f{20, 21});
	ਦਸ਼ਮਲਵ ਅੰਕੜੇ_D_check[16] = {20, 0, 0, 0,
				  0, 21, 0, 0,
				  0, 0, 11, 12,
				  0, 0, 15, 16
				 };
	ਵਰਗਫਲ_ਮੈਟ੍ਰਿਕਸ<ਦਸ਼ਮਲਵ, 4> D_check(ਅੰਕੜੇ_D_check);
	EXPECT_EQ(D, D_check);

	ਵਰਗਫਲ_ਮੈਟ੍ਰਿਕਸ<ਦਸ਼ਮਲਵ, 4> E(ਅੰਕੜੇ_4x4);
	E.uncorrelate_Covariance_ਸੈੱਟ_Variance<3>(1, 33);
	ਦਸ਼ਮਲਵ ਅੰਕੜੇ_E_check[16] = {1, 0, 0, 0,
				  0, 33, 0, 0,
				  0, 0, 33, 0,
				  0, 0, 0, 33
				 };
	ਵਰਗਫਲ_ਮੈਟ੍ਰਿਕਸ<ਦਸ਼ਮਲਵ, 4> E_check(ਅੰਕੜੇ_E_check);
	EXPECT_EQ(E, E_check);

	ਵਰਗਫਲ_ਮੈਟ੍ਰਿਕਸ<ਦਸ਼ਮਲਵ, 4> A_ਖੰਡ(ਅੰਕੜੇ_4x4);
	A_ਖੰਡ.uncorrelate_Covariance_ਖੰਡ<2>(1);
	ਦਸ਼ਮਲਵ ਅੰਕੜੇ_A_ਖੰਡ_check[16] = {1, 0, 0, 4,
					0, 6, 7, 0,
					0, 10, 11, 0,
					13, 0, 0, 16
				       };
	ਵਰਗਫਲ_ਮੈਟ੍ਰਿਕਸ<ਦਸ਼ਮਲਵ, 4> A_ਖੰਡ_check(ਅੰਕੜੇ_A_ਖੰਡ_check);
	EXPECT_EQ(A_ਖੰਡ, A_ਖੰਡ_check);

	// test symmetric functions
	ਵਰਗਫਲ_ਮੈਟ੍ਰਿਕਸ<ਦਸ਼ਮਲਵ, 4> F(ਅੰਕੜੇ_4x4);
	F.make_ਖੰਡ_ਸਮਮਿਤ<2>(1);
	ਦਸ਼ਮਲਵ ਅੰਕੜੇ_F_check[16] = {1, 2, 3, 4,
				  5, 6, 8.5, 8,
				  9, 8.5, 11, 12,
				  13, 14, 15, 16
				 };
	ਵਰਗਫਲ_ਮੈਟ੍ਰਿਕਸ<ਦਸ਼ਮਲਵ, 4> F_check(ਅੰਕੜੇ_F_check);
	EXPECT_EQ(F, F_check);
	EXPECT_TRUE(F.is_ਖੰਡ_ਸਮਮਿਤ<2>(1));
	EXPECT_FALSE(F.is_ਪੰਕਤੀ_ਖਾਨਾ_ਸਮਮਿਤ<2>(1));

	ਵਰਗਫਲ_ਮੈਟ੍ਰਿਕਸ<ਦਸ਼ਮਲਵ, 4> G(ਅੰਕੜੇ_4x4);
	G.make_ਪੰਕਤੀ_ਖਾਨਾ_ਸਮਮਿਤ<2>(1);
	ਦਸ਼ਮਲਵ ਅੰਕੜੇ_G_check[16] = {1, 3.5, 6, 4,
				  3.5, 6, 8.5, 11,
				  6, 8.5, 11, 13.5,
				  13, 11, 13.5, 16
				 };
	ਵਰਗਫਲ_ਮੈਟ੍ਰਿਕਸ<ਦਸ਼ਮਲਵ, 4> G_check(ਅੰਕੜੇ_G_check);
	EXPECT_EQ(G, G_check);
	EXPECT_TRUE(G.is_ਖੰਡ_ਸਮਮਿਤ<2>(1));
	EXPECT_TRUE(G.is_ਪੰਕਤੀ_ਖਾਨਾ_ਸਮਮਿਤ<2>(1));

	ਵਰਗਫਲ_ਮੈਟ੍ਰਿਕਸ<ਦਸ਼ਮਲਵ, 4> H(ਅੰਕੜੇ_4x4);
	H.make_ਖੰਡ_ਸਮਮਿਤ<1>(1);
	ਦਸ਼ਮਲਵ ਅੰਕੜੇ_H_check[16] = {1, 2, 3, 4,
				  5, 6, 7, 8,
				  9, 10, 11, 12,
				  13, 14, 15, 16
				 };
	ਵਰਗਫਲ_ਮੈਟ੍ਰਿਕਸ<ਦਸ਼ਮਲਵ, 4> H_check(ਅੰਕੜੇ_H_check);
	EXPECT_EQ(H, H_check);
	EXPECT_TRUE(H.is_ਖੰਡ_ਸਮਮਿਤ<1>(1));
	EXPECT_FALSE(H.is_ਪੰਕਤੀ_ਖਾਨਾ_ਸਮਮਿਤ<1>(1));

	ਵਰਗਫਲ_ਮੈਟ੍ਰਿਕਸ<ਦਸ਼ਮਲਵ, 4> J(ਅੰਕੜੇ_4x4);
	J.make_ਪੰਕਤੀ_ਖਾਨਾ_ਸਮਮਿਤ<1>(1);
	ਦਸ਼ਮਲਵ ਅੰਕੜੇ_J_check[16] = {1, 3.5, 3, 4,
				  3.5, 6, 8.5, 11,
				  9, 8.5, 11, 12,
				  13, 11, 15, 16
				 };
	ਵਰਗਫਲ_ਮੈਟ੍ਰਿਕਸ<ਦਸ਼ਮਲਵ, 4> J_check(ਅੰਕੜੇ_J_check);
	EXPECT_EQ(J, J_check);
	EXPECT_TRUE(J.is_ਖੰਡ_ਸਮਮਿਤ<1>(1));
	EXPECT_TRUE(J.is_ਪੰਕਤੀ_ਖਾਨਾ_ਸਮਮਿਤ<1>(1));
	EXPECT_FALSE(J.is_ਖੰਡ_ਸਮਮਿਤ<3>(1));

	ਦਸ਼ਮਲਵ ਅੰਕੜੇ_K[16] = {1, 2, 3, 4,
			    2, 3, 4, 11,
			    3, 4, 11, 12,
			    4, 11, 15, 16
			   };
	ਵਰਗਫਲ_ਮੈਟ੍ਰਿਕਸ<ਦਸ਼ਮਲਵ, 4> K(ਅੰਕੜੇ_K);
	EXPECT_FALSE(K.is_ਪੰਕਤੀ_ਖਾਨਾ_ਸਮਮਿਤ<1>(2));

	ਦਸ਼ਮਲਵ ਅੰਕੜੇ_L[16] = {1, 0, 0, 0,
			    2, 3, 0, 0,
			    3, 4, 11, 0,
			    4, 11, 15, 16
			   };
	ਦਸ਼ਮਲਵ ਅੰਕੜੇ_L_check[16] = {1, 2, 3, 4,
				  2, 3, 4, 11,
				  3, 4, 11, 15,
				  4, 11, 15, 16
				 };
	ਵਰਗਫਲ_ਮੈਟ੍ਰਿਕਸ<ਦਸ਼ਮਲਵ, 4> L(ਅੰਕੜੇ_L);
	L.copy_ਹੇਠਲਾ_ਮੰਜ਼ਿਲ_ਉੱਤਲਾ_Triangle();
	ਵਰਗਫਲ_ਮੈਟ੍ਰਿਕਸ<ਦਸ਼ਮਲਵ, 4> L_check(ਅੰਕੜੇ_L_check);
	EXPECT_EQ(L, L_check);

	ਦਸ਼ਮਲਵ ਅੰਕੜੇ_M[16] = {1, 2, 3, 4,
			    0, 3, 4, 11,
			    0, 0, 11, 15,
			    0, 0, 0, 16
			   };
	ਵਰਗਫਲ_ਮੈਟ੍ਰਿਕਸ<ਦਸ਼ਮਲਵ, 4> M(ਅੰਕੜੇ_M);
	M.copy_ਉੱਤਲਾ_ਮੰਜ਼ਿਲ_ਹੇਠਲਾ_Triangle();
	EXPECT_EQ(M, L_check);
}
