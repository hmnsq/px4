/****************************************************************************
 *
 *   Copyright (C) 2012 PX4 Development Team. All rights reserved.
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

/**
 * @file blocks.cpp
 *
 * Controller library code
 */

#ضمن <math.h>
#ضمن <stdio.h>
#ضمن <float.h>

#ضمن <controllib/blocks.hpp>

#تعریف_کرو ASSERT_CL(T) إذا (!(T)) { لکھو("FAIL\n"); أرجع -1; }

نطاق control
{

صحيح basic_بلاکاں_Test();
صحيح بلاک_حد_بندی_Test();
صحيح بلاک_حد_بندی_Sym_Test();
صحيح بلاک_کم_Pass_Test();
صحيح بلاک_بلند_Pass_Test();
صحيح بلاک_کم_Pass2_Test();
صحيح بلاک_Integral_Test();
صحيح بلاک_Integral_Trap_Test();
صحيح بلاک_Derivative_Test();
صحيح بلاک_PTest();
صحيح بلاک_PITest();
صحيح بلاک_PDTest();
صحيح بلاک_PIDTest();
صحيح بلاک_Output_Test();
صحيح بلاک_Rand_اکسار_Test();
صحيح بلاک_Rand_Gauss_Test();
صحيح بلاک_Stats_Test();
صحيح بلاک_Delay_Test();

صحيح basic_بلاکاں_Test()
{
	بلاک_حد_بندی_Test();
	بلاک_حد_بندی_Sym_Test();
	بلاک_کم_Pass_Test();
	بلاک_بلند_Pass_Test();
	بلاک_کم_Pass2_Test();
	بلاک_Integral_Test();
	بلاک_Integral_Trap_Test();
	بلاک_Derivative_Test();
	بلاک_PTest();
	بلاک_PITest();
	بلاک_PDTest();
	بلاک_PIDTest();
	بلاک_Output_Test();
	//blockRandUniformTest();
	// known failures
	// blockRandGaussTest();
	بلاک_Stats_Test();
	بلاک_Delay_Test();
	أرجع 0;
}

صحيح بلاک_حد_بندی_Test()
{
	لکھو("Test BlockLimit\t\t\t: ");
	بلاک_حد_بندی حد_بندی_2(عدم, "TEST");
	// initial state
	ASSERT_CL(equal(1.0f, حد_بندی_2.getMax()));
	ASSERT_CL(equal(-1.0f, حد_بندی_2.getMin()));
	ASSERT_CL(equal(0.0f, حد_بندی_2.getDt()));
	// update
	ASSERT_CL(equal(-1.0f, حد_بندی_2.update(-2.0f)));
	ASSERT_CL(equal(1.0f, حد_بندی_2.update(2.0f)));
	ASSERT_CL(equal(0.0f, حد_بندی_2.update(0.0f)));
	لکھو("PASS\n");
	أرجع 0;
}

صحيح بلاک_حد_بندی_Sym_Test()
{
	لکھو("Test BlockLimitSym\t\t: ");
	بلاک_حد_بندی_Sym حد_بندی_2(عدم, "TEST");
	// initial state
	ASSERT_CL(equal(1.0f, حد_بندی_2.getMax()));
	ASSERT_CL(equal(0.0f, حد_بندی_2.getDt()));
	// update
	ASSERT_CL(equal(-1.0f, حد_بندی_2.update(-2.0f)));
	ASSERT_CL(equal(1.0f, حد_بندی_2.update(2.0f)));
	ASSERT_CL(equal(0.0f, حد_بندی_2.update(0.0f)));
	لکھو("PASS\n");
	أرجع 0;
}

صحيح بلاک_کم_Pass_Test()
{
	لکھو("Test BlockLowPass\t\t: ");
	بلاک_کم_Pass کم_Pass_2(عدم, "TEST_LP");
	// test initial state
	ASSERT_CL(equal(10.0f, کم_Pass_2.getFCut()));
	ASSERT_CL(equal(0.0f, کم_Pass_2.getState()));
	ASSERT_CL(equal(0.0f, کم_Pass_2.getDt()));
	// set dt
	کم_Pass_2.سیٹ_Dt(0.1f);
	ASSERT_CL(equal(0.1f, کم_Pass_2.getDt()));
	// set state
	کم_Pass_2.سیٹ_State(1.0f);
	ASSERT_CL(equal(1.0f, کم_Pass_2.getState()));
	// test update
	ASSERT_CL(equal(1.8626974f, کم_Pass_2.update(2.0f)));

	// test end condition
	لكل (صحيح i = 0; i < 100; i++) {
		کم_Pass_2.update(2.0f);
	}

	ASSERT_CL(equal(2.0f, کم_Pass_2.getState()));
	ASSERT_CL(equal(2.0f, کم_Pass_2.update(2.0f)));
	لکھو("PASS\n");
	أرجع 0;
};

صحيح بلاک_بلند_Pass_Test()
{
	لکھو("Test BlockHighPass\t\t: ");
	بلاک_بلند_Pass بلند_Pass(عدم, "TEST_HP");
	// test initial state
	ASSERT_CL(equal(10.0f, بلند_Pass.getFCut()));
	ASSERT_CL(equal(0.0f, بلند_Pass.getU()));
	ASSERT_CL(equal(0.0f, بلند_Pass.getY()));
	ASSERT_CL(equal(0.0f, بلند_Pass.getDt()));
	// set dt
	بلند_Pass.سیٹ_Dt(0.1f);
	ASSERT_CL(equal(0.1f, بلند_Pass.getDt()));
	// set state
	بلند_Pass.سیٹ_U(1.0f);
	ASSERT_CL(equal(1.0f, بلند_Pass.getU()));
	بلند_Pass.سیٹ_Y(1.0f);
	ASSERT_CL(equal(1.0f, بلند_Pass.getY()));
	// test update
	ASSERT_CL(equal(0.2746051f, بلند_Pass.update(2.0f)));

	// test end condition
	لكل (صحيح i = 0; i < 100; i++) {
		بلند_Pass.update(2.0f);
	}

	ASSERT_CL(equal(0.0f, بلند_Pass.getY()));
	ASSERT_CL(equal(0.0f, بلند_Pass.update(2.0f)));
	لکھو("PASS\n");
	أرجع 0;
}

صحيح بلاک_کم_Pass2_Test()
{
	لکھو("Test BlockLowPass2\t\t: ");
	بلاک_کم_Pass2 کم_Pass_2(عدم, "TEST_LP", 100);
	// test initial state
	ASSERT_CL(equal(10.0f, کم_Pass_2.getFCutParam()));
	ASSERT_CL(equal(0.0f, کم_Pass_2.getState()));
	ASSERT_CL(equal(0.0f, کم_Pass_2.getDt()));
	// set dt
	کم_Pass_2.سیٹ_Dt(0.1f);
	ASSERT_CL(equal(0.1f, کم_Pass_2.getDt()));
	// set state
	کم_Pass_2.سیٹ_State(1.0f);
	ASSERT_CL(equal(1.0f, کم_Pass_2.getState()));
	// test update
	ASSERT_CL(equal(1.06745527f, کم_Pass_2.update(2.0f)));

	// test end condition
	لكل (صحيح i = 0; i < 100; i++) {
		کم_Pass_2.update(2.0f);
	}

	ASSERT_CL(equal(2.0f, کم_Pass_2.getState()));
	ASSERT_CL(equal(2.0f, کم_Pass_2.update(2.0f)));
	لکھو("PASS\n");
	أرجع 0;
};

صحيح بلاک_Integral_Test()
{
	لکھو("Test BlockIntegral\t\t: ");
	بلاک_Integral integral(عدم, "TEST_I");
	// test initial state
	ASSERT_CL(equal(1.0f, integral.getMax()));
	ASSERT_CL(equal(0.0f, integral.getDt()));
	// set dt
	integral.سیٹ_Dt(0.1f);
	ASSERT_CL(equal(0.1f, integral.getDt()));
	// set Y
	integral.سیٹ_Y(0.9f);
	ASSERT_CL(equal(0.9f, integral.getY()));

	// test exceed max
	لكل (صحيح i = 0; i < 100; i++) {
		integral.update(1.0f);
	}

	ASSERT_CL(equal(1.0f, integral.update(1.0f)));
	// test exceed min
	integral.سیٹ_Y(-0.9f);
	ASSERT_CL(equal(-0.9f, integral.getY()));

	لكل (صحيح i = 0; i < 100; i++) {
		integral.update(-1.0f);
	}

	ASSERT_CL(equal(-1.0f, integral.update(-1.0f)));
	// test update
	integral.سیٹ_Y(0.1f);
	ASSERT_CL(equal(0.2f, integral.update(1.0)));
	ASSERT_CL(equal(0.2f, integral.getY()));
	لکھو("PASS\n");
	أرجع 0;
}

صحيح بلاک_Integral_Trap_Test()
{
	لکھو("Test BlockIntegralTrap\t\t: ");
	بلاک_Integral_Trap integral(عدم, "TEST_I");
	// test initial state
	ASSERT_CL(equal(1.0f, integral.getMax()));
	ASSERT_CL(equal(0.0f, integral.getDt()));
	// set dt
	integral.سیٹ_Dt(0.1f);
	ASSERT_CL(equal(0.1f, integral.getDt()));
	// set U
	integral.سیٹ_U(1.0f);
	ASSERT_CL(equal(1.0f, integral.getU()));
	// set Y
	integral.سیٹ_Y(0.9f);
	ASSERT_CL(equal(0.9f, integral.getY()));

	// test exceed max
	لكل (صحيح i = 0; i < 100; i++) {
		integral.update(1.0f);
	}

	ASSERT_CL(equal(1.0f, integral.update(1.0f)));
	// test exceed min
	integral.سیٹ_U(-1.0f);
	integral.سیٹ_Y(-0.9f);
	ASSERT_CL(equal(-0.9f, integral.getY()));

	لكل (صحيح i = 0; i < 100; i++) {
		integral.update(-1.0f);
	}

	ASSERT_CL(equal(-1.0f, integral.update(-1.0f)));
	// test update
	integral.سیٹ_U(2.0f);
	integral.سیٹ_Y(0.1f);
	ASSERT_CL(equal(0.25f, integral.update(1.0)));
	ASSERT_CL(equal(0.25f, integral.getY()));
	ASSERT_CL(equal(1.0f, integral.getU()));
	لکھو("PASS\n");
	أرجع 0;
}

صحيح بلاک_Derivative_Test()
{
	لکھو("Test BlockDerivative\t\t: ");
	بلاک_Derivative derivative(عدم, "TEST_D");
	// test initial state
	ASSERT_CL(equal(0.0f, derivative.getU()));
	ASSERT_CL(equal(10.0f, derivative.getLP()));
	// set dt
	derivative.سیٹ_Dt(0.1f);
	ASSERT_CL(equal(0.1f, derivative.getDt()));
	// set U
	derivative.سیٹ_U(1.0f);
	ASSERT_CL(equal(1.0f, derivative.getU()));
	// perform one update so initialized is set
	derivative.update(1.0);
	ASSERT_CL(equal(1.0f, derivative.getU()));
	// test  update
	ASSERT_CL(equal(8.6269744f, derivative.update(2.0f)));
	ASSERT_CL(equal(2.0f, derivative.getU()));
	لکھو("PASS\n");
	أرجع 0;
}

صحيح بلاک_PTest()
{
	لکھو("Test BlockP\t\t\t: ");
	بلاک_P بلاک_P_2(عدم, "TEST_P");
	// test initial state
	ASSERT_CL(equal(0.2f, بلاک_P_2.getKP()));
	ASSERT_CL(equal(0.0f, بلاک_P_2.getDt()));
	// set dt
	بلاک_P_2.سیٹ_Dt(0.1f);
	ASSERT_CL(equal(0.1f, بلاک_P_2.getDt()));
	// test  update
	ASSERT_CL(equal(0.4f, بلاک_P_2.update(2.0f)));
	لکھو("PASS\n");
	أرجع 0;
}

صحيح بلاک_PITest()
{
	لکھو("Test BlockPI\t\t\t: ");
	بلاک_PI بلاک_PI_2(عدم, "TEST");
	// test initial state
	ASSERT_CL(equal(0.2f, بلاک_PI_2.getKP()));
	ASSERT_CL(equal(0.1f, بلاک_PI_2.getKI()));
	ASSERT_CL(equal(0.0f, بلاک_PI_2.getDt()));
	ASSERT_CL(equal(1.0f, بلاک_PI_2.getIntegral().getMax()));
	// set dt
	بلاک_PI_2.سیٹ_Dt(0.1f);
	ASSERT_CL(equal(0.1f, بلاک_PI_2.getDt()));
	// set integral state
	بلاک_PI_2.getIntegral().سیٹ_Y(0.1f);
	ASSERT_CL(equal(0.1f, بلاک_PI_2.getIntegral().getY()));
	// test  update
	// 0.2*2 + 0.1*(2*0.1 + 0.1) = 0.43
	ASSERT_CL(equal(0.43f, بلاک_PI_2.update(2.0f)));
	لکھو("PASS\n");
	أرجع 0;
}

صحيح بلاک_PDTest()
{
	لکھو("Test BlockPD\t\t\t: ");
	بلاک_PD بلاک_PD_2(عدم, "TEST");
	// test initial state
	ASSERT_CL(equal(0.2f, بلاک_PD_2.getKP()));
	ASSERT_CL(equal(0.01f, بلاک_PD_2.getKD()));
	ASSERT_CL(equal(0.0f, بلاک_PD_2.getDt()));
	ASSERT_CL(equal(10.0f, بلاک_PD_2.getDerivative().getLP()));
	// set dt
	بلاک_PD_2.سیٹ_Dt(0.1f);
	ASSERT_CL(equal(0.1f, بلاک_PD_2.getDt()));
	// set derivative state
	بلاک_PD_2.getDerivative().سیٹ_U(1.0f);
	ASSERT_CL(equal(1.0f, بلاک_PD_2.getDerivative().getU()));
	// perform one update so initialized is set
	بلاک_PD_2.getDerivative().update(1.0);
	ASSERT_CL(equal(1.0f, بلاک_PD_2.getDerivative().getU()));
	// test  update
	// 0.2*2 + 0.1*(0.1*8.626...) = 0.486269744
	ASSERT_CL(equal(0.486269744f, بلاک_PD_2.update(2.0f)));
	لکھو("PASS\n");
	أرجع 0;
}

صحيح بلاک_PIDTest()
{
	لکھو("Test BlockPID\t\t\t: ");
	بلاک_PID بلاک_PID_2(عدم, "TEST");
	// test initial state
	ASSERT_CL(equal(0.2f, بلاک_PID_2.getKP()));
	ASSERT_CL(equal(0.1f, بلاک_PID_2.getKI()));
	ASSERT_CL(equal(0.01f, بلاک_PID_2.getKD()));
	ASSERT_CL(equal(0.0f, بلاک_PID_2.getDt()));
	ASSERT_CL(equal(10.0f, بلاک_PID_2.getDerivative().getLP()));
	ASSERT_CL(equal(1.0f, بلاک_PID_2.getIntegral().getMax()));
	// set dt
	بلاک_PID_2.سیٹ_Dt(0.1f);
	ASSERT_CL(equal(0.1f, بلاک_PID_2.getDt()));
	// set derivative state
	بلاک_PID_2.getDerivative().سیٹ_U(1.0f);
	ASSERT_CL(equal(1.0f, بلاک_PID_2.getDerivative().getU()));
	// perform one update so initialized is set
	بلاک_PID_2.getDerivative().update(1.0);
	ASSERT_CL(equal(1.0f, بلاک_PID_2.getDerivative().getU()));
	// set integral state
	بلاک_PID_2.getIntegral().سیٹ_Y(0.1f);
	ASSERT_CL(equal(0.1f, بلاک_PID_2.getIntegral().getY()));
	// test  update
	// 0.2*2 + 0.1*(2*0.1 + 0.1) + 0.1*(0.1*8.626...) = 0.5162697
	ASSERT_CL(equal(0.5162697f, بلاک_PID_2.update(2.0f)));
	لکھو("PASS\n");
	أرجع 0;
}

صحيح بلاک_Output_Test()
{
	لکھو("Test BlockOutput\t\t: ");
	بلاک_Output بلاک_Output_2(عدم, "TEST");
	// test initial state
	ASSERT_CL(equal(0.0f, بلاک_Output_2.getDt()));
	ASSERT_CL(equal(0.5f, بلاک_Output_2.get()));
	ASSERT_CL(equal(-1.0f, بلاک_Output_2.getMin()));
	ASSERT_CL(equal(1.0f, بلاک_Output_2.getMax()));
	// test update below min
	بلاک_Output_2.update(-2.0f);
	ASSERT_CL(equal(-1.0f, بلاک_Output_2.get()));
	// test update above max
	بلاک_Output_2.update(2.0f);
	ASSERT_CL(equal(1.0f, بلاک_Output_2.get()));
	// test trim
	بلاک_Output_2.update(0.0f);
	ASSERT_CL(equal(0.5f, بلاک_Output_2.get()));
	لکھو("PASS\n");
	أرجع 0;
}

صحيح بلاک_Rand_اکسار_Test()
{
	srand(1234);
	لکھو("Test BlockRandUniform\t\t: ");
	بلاک_Rand_اکسار بلاک_Rand_اکسار_2(عدم, "TEST");
	// test initial state
	ASSERT_CL(equal(0.0f, بلاک_Rand_اکسار_2.getDt()));
	ASSERT_CL(equal(-1.0f, بلاک_Rand_اکسار_2.getMin()));
	ASSERT_CL(equal(1.0f, بلاک_Rand_اکسار_2.getMax()));
	// test update
	صحيح n = 10000;
	عائم اوسط = بلاک_Rand_اکسار_2.update();

	لكل (صحيح i = 2; i < n + 1; i++) {
		عائم val = بلاک_Rand_اکسار_2.update();
		اوسط += (val - اوسط) / i;
		ASSERT_CL(less_than_or_equal(val, بلاک_Rand_اکسار_2.getMax()));
		ASSERT_CL(greater_than_or_equal(val, بلاک_Rand_اکسار_2.getMin()));
	}

	ASSERT_CL(equal(اوسط, (بلاک_Rand_اکسار_2.getMin() +
			       بلاک_Rand_اکسار_2.getMax()) / 2, 1e-1));
	لکھو("PASS\n");
	أرجع 0;
}

صحيح بلاک_Rand_Gauss_Test()
{
	srand(1234);
	لکھو("Test BlockRandGauss\t\t: ");
	بلاک_Rand_Gauss بلاک_Rand_Gauss_2(عدم, "TEST");
	// test initial state
	ASSERT_CL(equal(0.0f, بلاک_Rand_Gauss_2.getDt()));
	ASSERT_CL(equal(1.0f, بلاک_Rand_Gauss_2.get_اوسط()));
	ASSERT_CL(equal(2.0f, بلاک_Rand_Gauss_2.getStdDev()));
	// test update
	صحيح n = 10000;
	عائم اوسط = بلاک_Rand_Gauss_2.update();
	عائم جوڑ = 0;

	// recursive mean, stdev algorithm from Knuth
	لكل (صحيح i = 2; i < n + 1; i++) {
		عائم val = بلاک_Rand_Gauss_2.update();
		عائم new_اوسط = اوسط + (val - اوسط) / i;
		جوڑ += (val - اوسط) * (val - new_اوسط);
		اوسط = new_اوسط;
	}

	عائم stdDev = جذر(جوڑ / (n - 1));
	(فراغ)(stdDev);
	ASSERT_CL(equal(اوسط, بلاک_Rand_Gauss_2.get_اوسط(), 1e-1));
	ASSERT_CL(equal(stdDev, بلاک_Rand_Gauss_2.getStdDev(), 1e-1));
	لکھو("PASS\n");
	أرجع 0;
}

صحيح بلاک_Stats_Test()
{
	لکھو("Test BlockStats\t\t\t: ");
	بلاک_Stats<عائم, 1> stats(عدم, "TEST");
	ASSERT_CL(equal(0.0f, stats.get_اوسط()(0)));
	ASSERT_CL(equal(0.0f, stats.getStdDev()(0)));
	stats.update(میٹرکس::Scalar<عائم>(1.0f));
	stats.update(میٹرکس::Scalar<عائم>(2));
	ASSERT_CL(equal(1.5f, stats.get_اوسط()(0)));
	ASSERT_CL(equal(0.5f, stats.getStdDev()(0)));
	stats.reset();
	ASSERT_CL(equal(0.0f, stats.get_اوسط()(0)));
	ASSERT_CL(equal(0.0f, stats.getStdDev()(0)));
	لکھو("PASS\n");
	أرجع 0;
}

صحيح بلاک_Delay_Test()
{
	لکھو("Test BlockDelay\t\t\t: ");
	ورتوں نطاق میٹرکس;
	بلاک_Delay<عائم, 2, 1, 3> delay(عدم, "TEST");
	Vector2f u1(1, 2);
	Vector2f y1 = delay.update(u1);
	ASSERT_CL(equal(y1(0), u1(0)));
	ASSERT_CL(equal(y1(1), u1(1)));

	Vector2f u2(4, 5);
	Vector2f y2 = delay.update(u2);
	ASSERT_CL(equal(y2(0), u1(0)));
	ASSERT_CL(equal(y2(1), u1(1)));

	Vector2f u3(7, 8);
	Vector2f y3 = delay.update(u3);
	ASSERT_CL(equal(y3(0), u1(0)));
	ASSERT_CL(equal(y3(1), u1(1)));

	Vector2f u4(9, 10);
	Vector2f y4 = delay.update(u4);
	ASSERT_CL(equal(y4(0), u2(0)));
	ASSERT_CL(equal(y4(1), u2(1)));
	لکھو("PASS\n");
	أرجع 0;
}

} // namespace control
