/****************************************************************************
 *
 *   Copyright (c) 2013-2015 PX4 Development Team. All rights reserved.
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
 * @file controllib_test_main.cpp
 * Unit testing for controllib.
 *
 * @author James Goppert <james.goppert@gmail.com>
 */

#شامل <math.h>
#شامل <stdio.h>
#شامل <float.h>

#شامل <controllib/blocks.hpp>

استعمال نامخلا control;

#تعریف_کرو ASSERT_CL(T) اگر (!(T)) { لکھو("FAIL\n"); واپس -1; }

صحیح basic_بلاکیں_Test();
صحیح بلاک_حد_بندی_Test();
صحیح بلاک_حد_بندی_Sym_Test();
صحیح بلاک_کم_Pass_Test();
صحیح بلاک_بلند_Pass_Test();
صحیح بلاک_کم_Pass2_Test();
صحیح بلاک_Integral_Test();
صحیح بلاک_Integral_Trap_Test();
صحیح بلاک_Derivative_Test();
صحیح بلاک_PTest();
صحیح بلاک_PITest();
صحیح بلاک_PDTest();
صحیح بلاک_PIDTest();
صحیح بلاک_Output_Test();
صحیح بلاک_Rand_یکساں_Test();
صحیح بلاک_Rand_Gauss_Test();
صحیح بلاک_Stats_Test();
صحیح بلاک_Delay_Test();

صحیح basic_بلاکیں_Test()
{
	منطقی failed = جھوٹ;
	failed = failed || بلاک_حد_بندی_Test() < 0;
	failed = failed || بلاک_حد_بندی_Sym_Test() < 0;
	failed = failed || بلاک_کم_Pass_Test() < 0;
	failed = failed || بلاک_بلند_Pass_Test() < 0;
	failed = failed || بلاک_کم_Pass2_Test() < 0;
	failed = failed || بلاک_Integral_Test() < 0;
	failed = failed || بلاک_Integral_Trap_Test() < 0;
	failed = failed || بلاک_Derivative_Test() < 0;
	failed = failed || بلاک_PTest() < 0;
	failed = failed || بلاک_PITest() < 0;
	failed = failed || بلاک_PDTest() < 0;
	failed = failed || بلاک_PIDTest() < 0;
	failed = failed || بلاک_Output_Test() < 0;
	failed = failed || بلاک_Rand_یکساں_Test() < 0;
	failed = failed || بلاک_Rand_Gauss_Test() < 0;
	failed = failed || بلاک_Stats_Test() < 0;
	failed = failed || بلاک_Delay_Test() < 0;
	واپس failed ? -1 : 0;
}

صحیح بلاک_حد_بندی_Test()
{
	لکھو("Test BlockLimit\t\t\t: ");
	بلاک_حد_بندی حد_بندی_2(خالی_اشاریہ, "TEST");
	// initial state
	ASSERT_CL(equal(1.0f, حد_بندی_2.getMax()));
	ASSERT_CL(equal(-1.0f, حد_بندی_2.getMin()));
	ASSERT_CL(equal(0.0f, حد_بندی_2.getDt()));
	// update
	ASSERT_CL(equal(-1.0f, حد_بندی_2.update(-2.0f)));
	ASSERT_CL(equal(1.0f, حد_بندی_2.update(2.0f)));
	ASSERT_CL(equal(0.0f, حد_بندی_2.update(0.0f)));
	لکھو("PASS\n");
	واپس 0;
}

صحیح بلاک_حد_بندی_Sym_Test()
{
	لکھو("Test BlockLimitSym\t\t: ");
	بلاک_حد_بندی_Sym حد_بندی_2(خالی_اشاریہ, "TEST");
	// initial state
	ASSERT_CL(equal(1.0f, حد_بندی_2.getMax()));
	ASSERT_CL(equal(0.0f, حد_بندی_2.getDt()));
	// update
	ASSERT_CL(equal(-1.0f, حد_بندی_2.update(-2.0f)));
	ASSERT_CL(equal(1.0f, حد_بندی_2.update(2.0f)));
	ASSERT_CL(equal(0.0f, حد_بندی_2.update(0.0f)));
	لکھو("PASS\n");
	واپس 0;
}

صحیح بلاک_کم_Pass_Test()
{
	لکھو("Test BlockLowPass\t\t: ");
	بلاک_کم_Pass کم_Pass_2(خالی_اشاریہ, "TEST_LP");
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
	برائے (صحیح i = 0; i < 100; i++) {
		کم_Pass_2.update(2.0f);
	}

	ASSERT_CL(equal(2.0f, کم_Pass_2.getState()));
	ASSERT_CL(equal(2.0f, کم_Pass_2.update(2.0f)));
	لکھو("PASS\n");
	واپس 0;
};

صحیح بلاک_بلند_Pass_Test()
{
	لکھو("Test BlockHighPass\t\t: ");
	بلاک_بلند_Pass بلند_Pass(خالی_اشاریہ, "TEST_HP");
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
	برائے (صحیح i = 0; i < 100; i++) {
		بلند_Pass.update(2.0f);
	}

	ASSERT_CL(equal(0.0f, بلند_Pass.getY()));
	ASSERT_CL(equal(0.0f, بلند_Pass.update(2.0f)));
	لکھو("PASS\n");
	واپس 0;
}

صحیح بلاک_کم_Pass2_Test()
{
	لکھو("Test BlockLowPass2\t\t: ");
	بلاک_کم_Pass2 کم_Pass_2(خالی_اشاریہ, "TEST_LP", 100);
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
	برائے (صحیح i = 0; i < 100; i++) {
		کم_Pass_2.update(2.0f);
	}

	ASSERT_CL(equal(2.0f, کم_Pass_2.getState()));
	ASSERT_CL(equal(2.0f, کم_Pass_2.update(2.0f)));
	لکھو("PASS\n");
	واپس 0;
};

صحیح بلاک_Integral_Test()
{
	لکھو("Test BlockIntegral\t\t: ");
	بلاک_Integral integral(خالی_اشاریہ, "TEST_I");
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
	برائے (صحیح i = 0; i < 100; i++) {
		integral.update(1.0f);
	}

	ASSERT_CL(equal(1.0f, integral.update(1.0f)));
	// test exceed min
	integral.سیٹ_Y(-0.9f);
	ASSERT_CL(equal(-0.9f, integral.getY()));

	برائے (صحیح i = 0; i < 100; i++) {
		integral.update(-1.0f);
	}

	ASSERT_CL(equal(-1.0f, integral.update(-1.0f)));
	// test update
	integral.سیٹ_Y(0.1f);
	ASSERT_CL(equal(0.2f, integral.update(1.0)));
	ASSERT_CL(equal(0.2f, integral.getY()));
	لکھو("PASS\n");
	واپس 0;
}

صحیح بلاک_Integral_Trap_Test()
{
	لکھو("Test BlockIntegralTrap\t\t: ");
	بلاک_Integral_Trap integral(خالی_اشاریہ, "TEST_I");
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
	برائے (صحیح i = 0; i < 100; i++) {
		integral.update(1.0f);
	}

	ASSERT_CL(equal(1.0f, integral.update(1.0f)));
	// test exceed min
	integral.سیٹ_U(-1.0f);
	integral.سیٹ_Y(-0.9f);
	ASSERT_CL(equal(-0.9f, integral.getY()));

	برائے (صحیح i = 0; i < 100; i++) {
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
	واپس 0;
}

صحیح بلاک_Derivative_Test()
{
	لکھو("Test BlockDerivative\t\t: ");
	بلاک_Derivative derivative(خالی_اشاریہ, "TEST_D");
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
	واپس 0;
}

صحیح بلاک_PTest()
{
	لکھو("Test BlockP\t\t\t: ");
	بلاک_P بلاک_P_2(خالی_اشاریہ, "TEST_P");
	// test initial state
	ASSERT_CL(equal(0.2f, بلاک_P_2.getKP()));
	ASSERT_CL(equal(0.0f, بلاک_P_2.getDt()));
	// set dt
	بلاک_P_2.سیٹ_Dt(0.1f);
	ASSERT_CL(equal(0.1f, بلاک_P_2.getDt()));
	// test  update
	ASSERT_CL(equal(0.4f, بلاک_P_2.update(2.0f)));
	لکھو("PASS\n");
	واپس 0;
}

صحیح بلاک_PITest()
{
	لکھو("Test BlockPI\t\t\t: ");
	بلاک_PI بلاک_PI_2(خالی_اشاریہ, "TEST");
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
	واپس 0;
}

صحیح بلاک_PDTest()
{
	لکھو("Test BlockPD\t\t\t: ");
	بلاک_PD بلاک_PD_2(خالی_اشاریہ, "TEST");
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
	واپس 0;
}

صحیح بلاک_PIDTest()
{
	لکھو("Test BlockPID\t\t\t: ");
	بلاک_PID بلاک_PID_2(خالی_اشاریہ, "TEST");
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
	واپس 0;
}

صحیح بلاک_Output_Test()
{
	لکھو("Test BlockOutput\t\t: ");
	بلاک_Output بلاک_Output_2(خالی_اشاریہ, "TEST");
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
	واپس 0;
}

صحیح بلاک_Rand_یکساں_Test()
{
	srand(1234);
	لکھو("Test BlockRandUniform\t\t: ");
	بلاک_Rand_یکساں بلاک_Rand_یکساں_2(خالی_اشاریہ, "TEST");
	// test initial state
	ASSERT_CL(equal(0.0f, بلاک_Rand_یکساں_2.getDt()));
	ASSERT_CL(equal(-1.0f, بلاک_Rand_یکساں_2.getMin()));
	ASSERT_CL(equal(1.0f, بلاک_Rand_یکساں_2.getMax()));
	// test update
	صحیح n = 10000;
	اعشاری اوسط = بلاک_Rand_یکساں_2.update();

	برائے (صحیح i = 2; i < n + 1; i++) {
		اعشاری val = بلاک_Rand_یکساں_2.update();
		اوسط += (val - اوسط) / i;
		ASSERT_CL(less_than_or_equal(val, بلاک_Rand_یکساں_2.getMax()));
		ASSERT_CL(greater_than_or_equal(val, بلاک_Rand_یکساں_2.getMin()));
	}

	ASSERT_CL(equal(اوسط, (بلاک_Rand_یکساں_2.getMin() +
			       بلاک_Rand_یکساں_2.getMax()) / 2, 1e-1));
	لکھو("PASS\n");
	واپس 0;
}

صحیح بلاک_Rand_Gauss_Test()
{
	srand(1234);
	لکھو("Test BlockRandGauss\t\t: ");
	بلاک_Rand_Gauss بلاک_Rand_Gauss_2(خالی_اشاریہ, "TEST");
	// test initial state
	ASSERT_CL(equal(0.0f, بلاک_Rand_Gauss_2.getDt()));
	ASSERT_CL(equal(1.0f, بلاک_Rand_Gauss_2.get_اوسط()));
	ASSERT_CL(equal(2.0f, بلاک_Rand_Gauss_2.getStdDev()));
	// test update
	صحیح n = 10000;
	اعشاری اوسط = بلاک_Rand_Gauss_2.update();
	اعشاری مجموع = 0;

	// recursive mean, stdev algorithm from Knuth
	برائے (صحیح i = 2; i < n + 1; i++) {
		اعشاری val = بلاک_Rand_Gauss_2.update();
		اعشاری new_اوسط = اوسط + (val - اوسط) / i;
		مجموع += (val - اوسط) * (val - new_اوسط);
		اوسط = new_اوسط;
	}

	اعشاری stdDev = sqrtf(مجموع / (n - 1));
	(خالی)(stdDev);
	ASSERT_CL(equal(اوسط, بلاک_Rand_Gauss_2.get_اوسط(), 1e-1));
	ASSERT_CL(equal(stdDev, بلاک_Rand_Gauss_2.getStdDev(), 1e-1));
	لکھو("PASS\n");
	واپس 0;
}

صحیح بلاک_Stats_Test()
{
	لکھو("Test BlockStats\t\t\t: ");
	بلاک_Stats<اعشاری, 1> stats(خالی_اشاریہ, "TEST");
	ASSERT_CL(equal(0.0f, stats.get_اوسط()(0)));
	ASSERT_CL(equal(0.0f, stats.getStdDev()(0)));
	stats.update(میٹرکس::Scalar<اعشاری>(1.0f));
	stats.update(میٹرکس::Scalar<اعشاری>(2));
	ASSERT_CL(equal(1.5f, stats.get_اوسط()(0)));
	ASSERT_CL(equal(0.5f, stats.getStdDev()(0)));
	stats.reset();
	ASSERT_CL(equal(0.0f, stats.get_اوسط()(0)));
	ASSERT_CL(equal(0.0f, stats.getStdDev()(0)));
	لکھو("PASS\n");
	واپس 0;
}

صحیح بلاک_Delay_Test()
{
	لکھو("Test BlockDelay\t\t\t: ");
	استعمال نامخلا میٹرکس;
	بلاک_Delay<اعشاری, 2, 1, 3> delay(خالی_اشاریہ, "TEST");
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
	واپس 0;
}

خارجی "C" __EXPORT صحیح controllib_test_main(صحیح argc, حرف *argv[]);

صحیح controllib_test_main(صحیح argc, حرف *argv[])
{
	واپس basic_بلاکیں_Test();
}
