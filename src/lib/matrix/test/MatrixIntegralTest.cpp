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

#ضمن <gtest/gtest.h>
#ضمن <matrix/integration.hpp>

ورتوں نطاق میٹرکس;

سمتیہ<عائم, 6> f(عائم t, ثابت میٹرکس_2<عائم, 6, 1> &  /*y*/, ثابت میٹرکس_2<عائم, 3, 1> &  /*u*/)
{
	عائم v = -جیب(t);
	أرجع v * ones<عائم, 6, 1>();
}

TEST(میٹرکس_Integral_Test, Integral)
{
	سمتیہ<عائم, 6> y = ones<عائم, 6, 1>();
	سمتیہ<عائم, 3> u = ones<عائم, 3, 1>();
	عائم t0 = 0;
	عائم tf = 2;
	عائم h = 0.001f;
	integrate_rk4(f, y, u, t0, tf, h, y);
	عائم v = 1 + std::جیب_تمام(tf) - std::جیب_تمام(t0);
	EXPECT_EQ(y, (ones<عائم, 6, 1>()*v));
}
