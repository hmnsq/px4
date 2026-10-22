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

#وصف_ڪريو ASSERT_CL(T) إذا (!(T)) { لکو("FAIL\n"); أرجع -1; }

نطاق control
{

صحيح basic_بلاڪون_Test();
صحيح بلاڪ_حد_بندي_Test();
صحيح بلاڪ_حد_بندي_Sym_Test();
صحيح بلاڪ_هيٺيون_Pass_Test();
صحيح بلاڪ_مٿاهون_Pass_Test();
صحيح بلاڪ_هيٺيون_Pass2_Test();
صحيح بلاڪ_Integral_Test();
صحيح بلاڪ_Integral_Trap_Test();
صحيح بلاڪ_Derivative_Test();
صحيح بلاڪ_PTest();
صحيح بلاڪ_PITest();
صحيح بلاڪ_PDTest();
صحيح بلاڪ_PIDTest();
صحيح بلاڪ_Output_Test();
صحيح بلاڪ_Rand_هڪجهڙو_Test();
صحيح بلاڪ_Rand_Gauss_Test();
صحيح بلاڪ_Stats_Test();
صحيح بلاڪ_Delay_Test();

صحيح basic_بلاڪون_Test()
{
	بلاڪ_حد_بندي_Test();
	بلاڪ_حد_بندي_Sym_Test();
	بلاڪ_هيٺيون_Pass_Test();
	بلاڪ_مٿاهون_Pass_Test();
	بلاڪ_هيٺيون_Pass2_Test();
	بلاڪ_Integral_Test();
	بلاڪ_Integral_Trap_Test();
	بلاڪ_Derivative_Test();
	بلاڪ_PTest();
	بلاڪ_PITest();
	بلاڪ_PDTest();
	بلاڪ_PIDTest();
	بلاڪ_Output_Test();
	//blockRandUniformTest();
	// known failures
	// blockRandGaussTest();
	بلاڪ_Stats_Test();
	بلاڪ_Delay_Test();
	أرجع 0;
}

صحيح بلاڪ_حد_بندي_Test()
{
	لکو("Test BlockLimit\t\t\t: ");
	بلاڪ_حد_بندي حد_بندي_2(عدم, "TEST");
	// initial state
	ASSERT_CL(equal(1.0f, حد_بندي_2.getMax()));
	ASSERT_CL(equal(-1.0f, حد_بندي_2.getMin()));
	ASSERT_CL(equal(0.0f, حد_بندي_2.getDt()));
	// update
	ASSERT_CL(equal(-1.0f, حد_بندي_2.update(-2.0f)));
	ASSERT_CL(equal(1.0f, حد_بندي_2.update(2.0f)));
	ASSERT_CL(equal(0.0f, حد_بندي_2.update(0.0f)));
	لکو("PASS\n");
	أرجع 0;
}

صحيح بلاڪ_حد_بندي_Sym_Test()
{
	لکو("Test BlockLimitSym\t\t: ");
	بلاڪ_حد_بندي_Sym حد_بندي_2(عدم, "TEST");
	// initial state
	ASSERT_CL(equal(1.0f, حد_بندي_2.getMax()));
	ASSERT_CL(equal(0.0f, حد_بندي_2.getDt()));
	// update
	ASSERT_CL(equal(-1.0f, حد_بندي_2.update(-2.0f)));
	ASSERT_CL(equal(1.0f, حد_بندي_2.update(2.0f)));
	ASSERT_CL(equal(0.0f, حد_بندي_2.update(0.0f)));
	لکو("PASS\n");
	أرجع 0;
}

صحيح بلاڪ_هيٺيون_Pass_Test()
{
	لکو("Test BlockLowPass\t\t: ");
	بلاڪ_هيٺيون_Pass هيٺيون_Pass_2(عدم, "TEST_LP");
	// test initial state
	ASSERT_CL(equal(10.0f, هيٺيون_Pass_2.getFCut()));
	ASSERT_CL(equal(0.0f, هيٺيون_Pass_2.getState()));
	ASSERT_CL(equal(0.0f, هيٺيون_Pass_2.getDt()));
	// set dt
	هيٺيون_Pass_2.سيٽ_Dt(0.1f);
	ASSERT_CL(equal(0.1f, هيٺيون_Pass_2.getDt()));
	// set state
	هيٺيون_Pass_2.سيٽ_State(1.0f);
	ASSERT_CL(equal(1.0f, هيٺيون_Pass_2.getState()));
	// test update
	ASSERT_CL(equal(1.8626974f, هيٺيون_Pass_2.update(2.0f)));

	// test end condition
	لكل (صحيح i = 0; i < 100; i++) {
		هيٺيون_Pass_2.update(2.0f);
	}

	ASSERT_CL(equal(2.0f, هيٺيون_Pass_2.getState()));
	ASSERT_CL(equal(2.0f, هيٺيون_Pass_2.update(2.0f)));
	لکو("PASS\n");
	أرجع 0;
};

صحيح بلاڪ_مٿاهون_Pass_Test()
{
	لکو("Test BlockHighPass\t\t: ");
	بلاڪ_مٿاهون_Pass مٿاهون_Pass(عدم, "TEST_HP");
	// test initial state
	ASSERT_CL(equal(10.0f, مٿاهون_Pass.getFCut()));
	ASSERT_CL(equal(0.0f, مٿاهون_Pass.getU()));
	ASSERT_CL(equal(0.0f, مٿاهون_Pass.getY()));
	ASSERT_CL(equal(0.0f, مٿاهون_Pass.getDt()));
	// set dt
	مٿاهون_Pass.سيٽ_Dt(0.1f);
	ASSERT_CL(equal(0.1f, مٿاهون_Pass.getDt()));
	// set state
	مٿاهون_Pass.سيٽ_U(1.0f);
	ASSERT_CL(equal(1.0f, مٿاهون_Pass.getU()));
	مٿاهون_Pass.سيٽ_Y(1.0f);
	ASSERT_CL(equal(1.0f, مٿاهون_Pass.getY()));
	// test update
	ASSERT_CL(equal(0.2746051f, مٿاهون_Pass.update(2.0f)));

	// test end condition
	لكل (صحيح i = 0; i < 100; i++) {
		مٿاهون_Pass.update(2.0f);
	}

	ASSERT_CL(equal(0.0f, مٿاهون_Pass.getY()));
	ASSERT_CL(equal(0.0f, مٿاهون_Pass.update(2.0f)));
	لکو("PASS\n");
	أرجع 0;
}

صحيح بلاڪ_هيٺيون_Pass2_Test()
{
	لکو("Test BlockLowPass2\t\t: ");
	بلاڪ_هيٺيون_Pass2 هيٺيون_Pass_2(عدم, "TEST_LP", 100);
	// test initial state
	ASSERT_CL(equal(10.0f, هيٺيون_Pass_2.getFCutParam()));
	ASSERT_CL(equal(0.0f, هيٺيون_Pass_2.getState()));
	ASSERT_CL(equal(0.0f, هيٺيون_Pass_2.getDt()));
	// set dt
	هيٺيون_Pass_2.سيٽ_Dt(0.1f);
	ASSERT_CL(equal(0.1f, هيٺيون_Pass_2.getDt()));
	// set state
	هيٺيون_Pass_2.سيٽ_State(1.0f);
	ASSERT_CL(equal(1.0f, هيٺيون_Pass_2.getState()));
	// test update
	ASSERT_CL(equal(1.06745527f, هيٺيون_Pass_2.update(2.0f)));

	// test end condition
	لكل (صحيح i = 0; i < 100; i++) {
		هيٺيون_Pass_2.update(2.0f);
	}

	ASSERT_CL(equal(2.0f, هيٺيون_Pass_2.getState()));
	ASSERT_CL(equal(2.0f, هيٺيون_Pass_2.update(2.0f)));
	لکو("PASS\n");
	أرجع 0;
};

صحيح بلاڪ_Integral_Test()
{
	لکو("Test BlockIntegral\t\t: ");
	بلاڪ_Integral integral(عدم, "TEST_I");
	// test initial state
	ASSERT_CL(equal(1.0f, integral.getMax()));
	ASSERT_CL(equal(0.0f, integral.getDt()));
	// set dt
	integral.سيٽ_Dt(0.1f);
	ASSERT_CL(equal(0.1f, integral.getDt()));
	// set Y
	integral.سيٽ_Y(0.9f);
	ASSERT_CL(equal(0.9f, integral.getY()));

	// test exceed max
	لكل (صحيح i = 0; i < 100; i++) {
		integral.update(1.0f);
	}

	ASSERT_CL(equal(1.0f, integral.update(1.0f)));
	// test exceed min
	integral.سيٽ_Y(-0.9f);
	ASSERT_CL(equal(-0.9f, integral.getY()));

	لكل (صحيح i = 0; i < 100; i++) {
		integral.update(-1.0f);
	}

	ASSERT_CL(equal(-1.0f, integral.update(-1.0f)));
	// test update
	integral.سيٽ_Y(0.1f);
	ASSERT_CL(equal(0.2f, integral.update(1.0)));
	ASSERT_CL(equal(0.2f, integral.getY()));
	لکو("PASS\n");
	أرجع 0;
}

صحيح بلاڪ_Integral_Trap_Test()
{
	لکو("Test BlockIntegralTrap\t\t: ");
	بلاڪ_Integral_Trap integral(عدم, "TEST_I");
	// test initial state
	ASSERT_CL(equal(1.0f, integral.getMax()));
	ASSERT_CL(equal(0.0f, integral.getDt()));
	// set dt
	integral.سيٽ_Dt(0.1f);
	ASSERT_CL(equal(0.1f, integral.getDt()));
	// set U
	integral.سيٽ_U(1.0f);
	ASSERT_CL(equal(1.0f, integral.getU()));
	// set Y
	integral.سيٽ_Y(0.9f);
	ASSERT_CL(equal(0.9f, integral.getY()));

	// test exceed max
	لكل (صحيح i = 0; i < 100; i++) {
		integral.update(1.0f);
	}

	ASSERT_CL(equal(1.0f, integral.update(1.0f)));
	// test exceed min
	integral.سيٽ_U(-1.0f);
	integral.سيٽ_Y(-0.9f);
	ASSERT_CL(equal(-0.9f, integral.getY()));

	لكل (صحيح i = 0; i < 100; i++) {
		integral.update(-1.0f);
	}

	ASSERT_CL(equal(-1.0f, integral.update(-1.0f)));
	// test update
	integral.سيٽ_U(2.0f);
	integral.سيٽ_Y(0.1f);
	ASSERT_CL(equal(0.25f, integral.update(1.0)));
	ASSERT_CL(equal(0.25f, integral.getY()));
	ASSERT_CL(equal(1.0f, integral.getU()));
	لکو("PASS\n");
	أرجع 0;
}

صحيح بلاڪ_Derivative_Test()
{
	لکو("Test BlockDerivative\t\t: ");
	بلاڪ_Derivative derivative(عدم, "TEST_D");
	// test initial state
	ASSERT_CL(equal(0.0f, derivative.getU()));
	ASSERT_CL(equal(10.0f, derivative.getLP()));
	// set dt
	derivative.سيٽ_Dt(0.1f);
	ASSERT_CL(equal(0.1f, derivative.getDt()));
	// set U
	derivative.سيٽ_U(1.0f);
	ASSERT_CL(equal(1.0f, derivative.getU()));
	// perform one update so initialized is set
	derivative.update(1.0);
	ASSERT_CL(equal(1.0f, derivative.getU()));
	// test  update
	ASSERT_CL(equal(8.6269744f, derivative.update(2.0f)));
	ASSERT_CL(equal(2.0f, derivative.getU()));
	لکو("PASS\n");
	أرجع 0;
}

صحيح بلاڪ_PTest()
{
	لکو("Test BlockP\t\t\t: ");
	بلاڪ_P بلاڪ_P_2(عدم, "TEST_P");
	// test initial state
	ASSERT_CL(equal(0.2f, بلاڪ_P_2.getKP()));
	ASSERT_CL(equal(0.0f, بلاڪ_P_2.getDt()));
	// set dt
	بلاڪ_P_2.سيٽ_Dt(0.1f);
	ASSERT_CL(equal(0.1f, بلاڪ_P_2.getDt()));
	// test  update
	ASSERT_CL(equal(0.4f, بلاڪ_P_2.update(2.0f)));
	لکو("PASS\n");
	أرجع 0;
}

صحيح بلاڪ_PITest()
{
	لکو("Test BlockPI\t\t\t: ");
	بلاڪ_PI بلاڪ_PI_2(عدم, "TEST");
	// test initial state
	ASSERT_CL(equal(0.2f, بلاڪ_PI_2.getKP()));
	ASSERT_CL(equal(0.1f, بلاڪ_PI_2.getKI()));
	ASSERT_CL(equal(0.0f, بلاڪ_PI_2.getDt()));
	ASSERT_CL(equal(1.0f, بلاڪ_PI_2.getIntegral().getMax()));
	// set dt
	بلاڪ_PI_2.سيٽ_Dt(0.1f);
	ASSERT_CL(equal(0.1f, بلاڪ_PI_2.getDt()));
	// set integral state
	بلاڪ_PI_2.getIntegral().سيٽ_Y(0.1f);
	ASSERT_CL(equal(0.1f, بلاڪ_PI_2.getIntegral().getY()));
	// test  update
	// 0.2*2 + 0.1*(2*0.1 + 0.1) = 0.43
	ASSERT_CL(equal(0.43f, بلاڪ_PI_2.update(2.0f)));
	لکو("PASS\n");
	أرجع 0;
}

صحيح بلاڪ_PDTest()
{
	لکو("Test BlockPD\t\t\t: ");
	بلاڪ_PD بلاڪ_PD_2(عدم, "TEST");
	// test initial state
	ASSERT_CL(equal(0.2f, بلاڪ_PD_2.getKP()));
	ASSERT_CL(equal(0.01f, بلاڪ_PD_2.getKD()));
	ASSERT_CL(equal(0.0f, بلاڪ_PD_2.getDt()));
	ASSERT_CL(equal(10.0f, بلاڪ_PD_2.getDerivative().getLP()));
	// set dt
	بلاڪ_PD_2.سيٽ_Dt(0.1f);
	ASSERT_CL(equal(0.1f, بلاڪ_PD_2.getDt()));
	// set derivative state
	بلاڪ_PD_2.getDerivative().سيٽ_U(1.0f);
	ASSERT_CL(equal(1.0f, بلاڪ_PD_2.getDerivative().getU()));
	// perform one update so initialized is set
	بلاڪ_PD_2.getDerivative().update(1.0);
	ASSERT_CL(equal(1.0f, بلاڪ_PD_2.getDerivative().getU()));
	// test  update
	// 0.2*2 + 0.1*(0.1*8.626...) = 0.486269744
	ASSERT_CL(equal(0.486269744f, بلاڪ_PD_2.update(2.0f)));
	لکو("PASS\n");
	أرجع 0;
}

صحيح بلاڪ_PIDTest()
{
	لکو("Test BlockPID\t\t\t: ");
	بلاڪ_PID بلاڪ_PID_2(عدم, "TEST");
	// test initial state
	ASSERT_CL(equal(0.2f, بلاڪ_PID_2.getKP()));
	ASSERT_CL(equal(0.1f, بلاڪ_PID_2.getKI()));
	ASSERT_CL(equal(0.01f, بلاڪ_PID_2.getKD()));
	ASSERT_CL(equal(0.0f, بلاڪ_PID_2.getDt()));
	ASSERT_CL(equal(10.0f, بلاڪ_PID_2.getDerivative().getLP()));
	ASSERT_CL(equal(1.0f, بلاڪ_PID_2.getIntegral().getMax()));
	// set dt
	بلاڪ_PID_2.سيٽ_Dt(0.1f);
	ASSERT_CL(equal(0.1f, بلاڪ_PID_2.getDt()));
	// set derivative state
	بلاڪ_PID_2.getDerivative().سيٽ_U(1.0f);
	ASSERT_CL(equal(1.0f, بلاڪ_PID_2.getDerivative().getU()));
	// perform one update so initialized is set
	بلاڪ_PID_2.getDerivative().update(1.0);
	ASSERT_CL(equal(1.0f, بلاڪ_PID_2.getDerivative().getU()));
	// set integral state
	بلاڪ_PID_2.getIntegral().سيٽ_Y(0.1f);
	ASSERT_CL(equal(0.1f, بلاڪ_PID_2.getIntegral().getY()));
	// test  update
	// 0.2*2 + 0.1*(2*0.1 + 0.1) + 0.1*(0.1*8.626...) = 0.5162697
	ASSERT_CL(equal(0.5162697f, بلاڪ_PID_2.update(2.0f)));
	لکو("PASS\n");
	أرجع 0;
}

صحيح بلاڪ_Output_Test()
{
	لکو("Test BlockOutput\t\t: ");
	بلاڪ_Output بلاڪ_Output_2(عدم, "TEST");
	// test initial state
	ASSERT_CL(equal(0.0f, بلاڪ_Output_2.getDt()));
	ASSERT_CL(equal(0.5f, بلاڪ_Output_2.get()));
	ASSERT_CL(equal(-1.0f, بلاڪ_Output_2.getMin()));
	ASSERT_CL(equal(1.0f, بلاڪ_Output_2.getMax()));
	// test update below min
	بلاڪ_Output_2.update(-2.0f);
	ASSERT_CL(equal(-1.0f, بلاڪ_Output_2.get()));
	// test update above max
	بلاڪ_Output_2.update(2.0f);
	ASSERT_CL(equal(1.0f, بلاڪ_Output_2.get()));
	// test trim
	بلاڪ_Output_2.update(0.0f);
	ASSERT_CL(equal(0.5f, بلاڪ_Output_2.get()));
	لکو("PASS\n");
	أرجع 0;
}

صحيح بلاڪ_Rand_هڪجهڙو_Test()
{
	srand(1234);
	لکو("Test BlockRandUniform\t\t: ");
	بلاڪ_Rand_هڪجهڙو بلاڪ_Rand_هڪجهڙو_2(عدم, "TEST");
	// test initial state
	ASSERT_CL(equal(0.0f, بلاڪ_Rand_هڪجهڙو_2.getDt()));
	ASSERT_CL(equal(-1.0f, بلاڪ_Rand_هڪجهڙو_2.getMin()));
	ASSERT_CL(equal(1.0f, بلاڪ_Rand_هڪجهڙو_2.getMax()));
	// test update
	صحيح n = 10000;
	عائم سراسري = بلاڪ_Rand_هڪجهڙو_2.update();

	لكل (صحيح i = 2; i < n + 1; i++) {
		عائم val = بلاڪ_Rand_هڪجهڙو_2.update();
		سراسري += (val - سراسري) / i;
		ASSERT_CL(less_than_or_equal(val, بلاڪ_Rand_هڪجهڙو_2.getMax()));
		ASSERT_CL(greater_than_or_equal(val, بلاڪ_Rand_هڪجهڙو_2.getMin()));
	}

	ASSERT_CL(equal(سراسري, (بلاڪ_Rand_هڪجهڙو_2.getMin() +
			       بلاڪ_Rand_هڪجهڙو_2.getMax()) / 2, 1e-1));
	لکو("PASS\n");
	أرجع 0;
}

صحيح بلاڪ_Rand_Gauss_Test()
{
	srand(1234);
	لکو("Test BlockRandGauss\t\t: ");
	بلاڪ_Rand_Gauss بلاڪ_Rand_Gauss_2(عدم, "TEST");
	// test initial state
	ASSERT_CL(equal(0.0f, بلاڪ_Rand_Gauss_2.getDt()));
	ASSERT_CL(equal(1.0f, بلاڪ_Rand_Gauss_2.get_سراسري()));
	ASSERT_CL(equal(2.0f, بلاڪ_Rand_Gauss_2.getStdDev()));
	// test update
	صحيح n = 10000;
	عائم سراسري = بلاڪ_Rand_Gauss_2.update();
	عائم جوڙ = 0;

	// recursive mean, stdev algorithm from Knuth
	لكل (صحيح i = 2; i < n + 1; i++) {
		عائم val = بلاڪ_Rand_Gauss_2.update();
		عائم new_سراسري = سراسري + (val - سراسري) / i;
		جوڙ += (val - سراسري) * (val - new_سراسري);
		سراسري = new_سراسري;
	}

	عائم stdDev = جذر(جوڙ / (n - 1));
	(فراغ)(stdDev);
	ASSERT_CL(equal(سراسري, بلاڪ_Rand_Gauss_2.get_سراسري(), 1e-1));
	ASSERT_CL(equal(stdDev, بلاڪ_Rand_Gauss_2.getStdDev(), 1e-1));
	لکو("PASS\n");
	أرجع 0;
}

صحيح بلاڪ_Stats_Test()
{
	لکو("Test BlockStats\t\t\t: ");
	بلاڪ_Stats<عائم, 1> stats(عدم, "TEST");
	ASSERT_CL(equal(0.0f, stats.get_سراسري()(0)));
	ASSERT_CL(equal(0.0f, stats.getStdDev()(0)));
	stats.update(ميٽرڪس::Scalar<عائم>(1.0f));
	stats.update(ميٽرڪس::Scalar<عائم>(2));
	ASSERT_CL(equal(1.5f, stats.get_سراسري()(0)));
	ASSERT_CL(equal(0.5f, stats.getStdDev()(0)));
	stats.reset();
	ASSERT_CL(equal(0.0f, stats.get_سراسري()(0)));
	ASSERT_CL(equal(0.0f, stats.getStdDev()(0)));
	لکو("PASS\n");
	أرجع 0;
}

صحيح بلاڪ_Delay_Test()
{
	لکو("Test BlockDelay\t\t\t: ");
	استعمال نطاق ميٽرڪس;
	بلاڪ_Delay<عائم, 2, 1, 3> delay(عدم, "TEST");
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
	لکو("PASS\n");
	أرجع 0;
}

} // namespace control
