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

#అన్తర్భావ <gtest/gtest.h>
#అన్తర్భావ <matrix/math.hpp>

ఉపయోగించి నామాకాశ మాతృక;

పూర్ణాంకం test_4x3(శూన్య);
మూస<రకం_పేరు Type> పూర్ణాంకం test_4x4(శూన్య);
పూర్ణాంకం test_4x4_type_double(శూన్య);
పూర్ణాంకం test_div_zero(శూన్య);

TEST(మాతృక_కనిష్ఠ_Squares_Test, 4x3)
{
	// Start with an (m x n) A matrix
	దశాంశం దత్తాంశం[12] = {20.f, -10.f, -13.f,
			  17.f,  16.f, -18.f,
			  0.7f,  -0.8f,   0.9f,
			  -1.f,  -1.1f,  -1.2f
			 };
	మాతృక_2<దశాంశం, 4, 3> A(దత్తాంశం);
	Vector4f b(2.0f, 3.0f, 4.0f, 5.0f);
	Vector3f x_check(-0.69168233f, -0.26227593f, -1.03767522f);

	కనిష్ఠ_Squares_Solver<దశాంశం, 4, 3> qrd = కనిష్ఠ_Squares_Solver<దశాంశం, 4, 3>(A);

	Vector3f x = qrd.solve(b);
	EXPECT_EQ(x, x_check);
}

TEST(మాతృక_కనిష్ఠ_Squares_Test, 4x4)
{
	// Start with an (m x n) A matrix
	స్థిరరాశి దశాంశం దత్తాంశం[16] = { 20.f, -10.f, -13.f,  21.f,
				 17.f,  16.f, -18.f, -14.f,
				 0.7f,  -0.8f,   0.9f,  -0.5f,
				 -1.f,  -1.1f,  -1.2f,  -1.3f
			       };
	మాతృక_2<దశాంశం, 4, 4> A(దత్తాంశం);
	Vector4f b(2.0f, 3.0f, 4.0f, 5.0f);
	Vector4f x_check(0.97893433f, -2.80798701f, -0.03175765f, -2.19387649f);

	కనిష్ఠ_Squares_Solver<దశాంశం, 4, 4> qrd = కనిష్ఠ_Squares_Solver<దశాంశం, 4, 4>(A);

	Vector4f x = qrd.solve(b);
	EXPECT_EQ(x, x_check);
}

TEST(మాతృక_కనిష్ఠ_Squares_Test, ZeroDivision)
{
	దశాంశం దత్తాంశం[4] = {0.0f, 0.0f, 0.0f, 0.0f};
	మాతృక_2<దశాంశం, 2, 2> A(దత్తాంశం);

	దశాంశం b_దత్తాంశం[2] = {1.0f, 1.0f};
	సదిశ<దశాంశం, 2> b(b_దత్తాంశం);

	// Implement such that x returns zeros if it reaches div by zero
	దశాంశం x_check_దత్తాంశం[2] = {0.0f, 0.0f};
	సదిశ<దశాంశం, 2> x_check(x_check_దత్తాంశం);

	కనిష్ఠ_Squares_Solver<దశాంశం, 2, 2> qrd = కనిష్ఠ_Squares_Solver<దశాంశం, 2, 2>(A);

	సదిశ<దశాంశం, 2> x = qrd.solve(b);
	EXPECT_EQ(x, x_check);
}
