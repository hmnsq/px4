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

#અન્તર્ભાવ <gtest/gtest.h>
#અન્તર્ભાવ <matrix/integration.hpp>

using નામાકાશ matrix;

Vector<દશાંશ, 6> f(દશાંશ t, અચલ Matrix<દશાંશ, 6, 1> &  /*y*/, અચલ Matrix<દશાંશ, 3, 1> &  /*u*/)
{
	દશાંશ v = -sin(t);
	પાછા v * ones<દશાંશ, 6, 1>();
}

TEST(MatrixIntegralTest, Integral)
{
	Vector<દશાંશ, 6> y = ones<દશાંશ, 6, 1>();
	Vector<દશાંશ, 3> u = ones<દશાંશ, 3, 1>();
	દશાંશ t0 = 0;
	દશાંશ tf = 2;
	દશાંશ h = 0.001f;
	integrate_rk4(f, y, u, t0, tf, h, y);
	દશાંશ v = 1 + std::cos(tf) - std::cos(t0);
	EXPECT_EQ(y, (ones<દશાંશ, 6, 1>()*v));
}
