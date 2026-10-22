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

#ضمن <math.h>
#ضمن <stdio.h>
#ضمن <float.h>

#ضمن <controllib/blocks.hpp>

کارول نطاق control;

#تعریف_کړه ASSERT_CL(T) إذا (!(T)) { چاپ_کړه("FAIL\n"); أرجع -1; }

صحيح basic_بلاکونه_Test();
صحيح بلاک_حد_Test();
صحيح بلاک_حد_Sym_Test();
صحيح بلاک_ټیټ_Pass_Test();
صحيح بلاک_لوړ_Pass_Test();
صحيح بلاک_ټیټ_Pass2_Test();
صحيح بلاک_Integral_Test();
صحيح بلاک_Integral_Trap_Test();
صحيح بلاک_Derivative_Test();
صحيح بلاک_PTest();
صحيح بلاک_PITest();
صحيح بلاک_PDTest();
صحيح بلاک_PIDTest();
صحيح بلاک_Output_Test();
صحيح بلاک_Rand_یوشان_Test();
صحيح بلاک_Rand_Gauss_Test();
صحيح بلاک_Stats_Test();
صحيح بلاک_Delay_Test();

صحيح basic_بلاکونه_Test()
{
	منطقي failed = خطأ;
	failed = failed || بلاک_حد_Test() < 0;
	failed = failed || بلاک_حد_Sym_Test() < 0;
	failed = failed || بلاک_ټیټ_Pass_Test() < 0;
	failed = failed || بلاک_لوړ_Pass_Test() < 0;
	failed = failed || بلاک_ټیټ_Pass2_Test() < 0;
	failed = failed || بلاک_Integral_Test() < 0;
	failed = failed || بلاک_Integral_Trap_Test() < 0;
	failed = failed || بلاک_Derivative_Test() < 0;
	failed = failed || بلاک_PTest() < 0;
	failed = failed || بلاک_PITest() < 0;
	failed = failed || بلاک_PDTest() < 0;
	failed = failed || بلاک_PIDTest() < 0;
	failed = failed || بلاک_Output_Test() < 0;
	failed = failed || بلاک_Rand_یوشان_Test() < 0;
	failed = failed || بلاک_Rand_Gauss_Test() < 0;
	failed = failed || بلاک_Stats_Test() < 0;
	failed = failed || بلاک_Delay_Test() < 0;
	أرجع failed ? -1 : 0;
}

صحيح بلاک_حد_Test()
{
	چاپ_کړه("Test BlockLimit\t\t\t: ");
	بلاک_حد حد_2(تش_اشاره, "TEST");
	// initial state
	ASSERT_CL(equal(1.0f, حد_2.getMax()));
	ASSERT_CL(equal(-1.0f, حد_2.getMin()));
	ASSERT_CL(equal(0.0f, حد_2.getDt()));
	// update
	ASSERT_CL(equal(-1.0f, حد_2.update(-2.0f)));
	ASSERT_CL(equal(1.0f, حد_2.update(2.0f)));
	ASSERT_CL(equal(0.0f, حد_2.update(0.0f)));
	چاپ_کړه("PASS\n");
	أرجع 0;
}

صحيح بلاک_حد_Sym_Test()
{
	چاپ_کړه("Test BlockLimitSym\t\t: ");
	بلاک_حد_Sym حد_2(تش_اشاره, "TEST");
	// initial state
	ASSERT_CL(equal(1.0f, حد_2.getMax()));
	ASSERT_CL(equal(0.0f, حد_2.getDt()));
	// update
	ASSERT_CL(equal(-1.0f, حد_2.update(-2.0f)));
	ASSERT_CL(equal(1.0f, حد_2.update(2.0f)));
	ASSERT_CL(equal(0.0f, حد_2.update(0.0f)));
	چاپ_کړه("PASS\n");
	أرجع 0;
}

صحيح بلاک_ټیټ_Pass_Test()
{
	چاپ_کړه("Test BlockLowPass\t\t: ");
	بلاک_ټیټ_Pass ټیټ_Pass_2(تش_اشاره, "TEST_LP");
	// test initial state
	ASSERT_CL(equal(10.0f, ټیټ_Pass_2.getFCut()));
	ASSERT_CL(equal(0.0f, ټیټ_Pass_2.getState()));
	ASSERT_CL(equal(0.0f, ټیټ_Pass_2.getDt()));
	// set dt
	ټیټ_Pass_2.سېټ_Dt(0.1f);
	ASSERT_CL(equal(0.1f, ټیټ_Pass_2.getDt()));
	// set state
	ټیټ_Pass_2.سېټ_State(1.0f);
	ASSERT_CL(equal(1.0f, ټیټ_Pass_2.getState()));
	// test update
	ASSERT_CL(equal(1.8626974f, ټیټ_Pass_2.update(2.0f)));

	// test end condition
	لكل (صحيح i = 0; i < 100; i++) {
		ټیټ_Pass_2.update(2.0f);
	}

	ASSERT_CL(equal(2.0f, ټیټ_Pass_2.getState()));
	ASSERT_CL(equal(2.0f, ټیټ_Pass_2.update(2.0f)));
	چاپ_کړه("PASS\n");
	أرجع 0;
};

صحيح بلاک_لوړ_Pass_Test()
{
	چاپ_کړه("Test BlockHighPass\t\t: ");
	بلاک_لوړ_Pass لوړ_Pass(تش_اشاره, "TEST_HP");
	// test initial state
	ASSERT_CL(equal(10.0f, لوړ_Pass.getFCut()));
	ASSERT_CL(equal(0.0f, لوړ_Pass.getU()));
	ASSERT_CL(equal(0.0f, لوړ_Pass.getY()));
	ASSERT_CL(equal(0.0f, لوړ_Pass.getDt()));
	// set dt
	لوړ_Pass.سېټ_Dt(0.1f);
	ASSERT_CL(equal(0.1f, لوړ_Pass.getDt()));
	// set state
	لوړ_Pass.سېټ_U(1.0f);
	ASSERT_CL(equal(1.0f, لوړ_Pass.getU()));
	لوړ_Pass.سېټ_Y(1.0f);
	ASSERT_CL(equal(1.0f, لوړ_Pass.getY()));
	// test update
	ASSERT_CL(equal(0.2746051f, لوړ_Pass.update(2.0f)));

	// test end condition
	لكل (صحيح i = 0; i < 100; i++) {
		لوړ_Pass.update(2.0f);
	}

	ASSERT_CL(equal(0.0f, لوړ_Pass.getY()));
	ASSERT_CL(equal(0.0f, لوړ_Pass.update(2.0f)));
	چاپ_کړه("PASS\n");
	أرجع 0;
}

صحيح بلاک_ټیټ_Pass2_Test()
{
	چاپ_کړه("Test BlockLowPass2\t\t: ");
	بلاک_ټیټ_Pass2 ټیټ_Pass_2(تش_اشاره, "TEST_LP", 100);
	// test initial state
	ASSERT_CL(equal(10.0f, ټیټ_Pass_2.getFCutParam()));
	ASSERT_CL(equal(0.0f, ټیټ_Pass_2.getState()));
	ASSERT_CL(equal(0.0f, ټیټ_Pass_2.getDt()));
	// set dt
	ټیټ_Pass_2.سېټ_Dt(0.1f);
	ASSERT_CL(equal(0.1f, ټیټ_Pass_2.getDt()));
	// set state
	ټیټ_Pass_2.سېټ_State(1.0f);
	ASSERT_CL(equal(1.0f, ټیټ_Pass_2.getState()));
	// test update
	ASSERT_CL(equal(1.06745527f, ټیټ_Pass_2.update(2.0f)));

	// test end condition
	لكل (صحيح i = 0; i < 100; i++) {
		ټیټ_Pass_2.update(2.0f);
	}

	ASSERT_CL(equal(2.0f, ټیټ_Pass_2.getState()));
	ASSERT_CL(equal(2.0f, ټیټ_Pass_2.update(2.0f)));
	چاپ_کړه("PASS\n");
	أرجع 0;
};

صحيح بلاک_Integral_Test()
{
	چاپ_کړه("Test BlockIntegral\t\t: ");
	بلاک_Integral integral(تش_اشاره, "TEST_I");
	// test initial state
	ASSERT_CL(equal(1.0f, integral.getMax()));
	ASSERT_CL(equal(0.0f, integral.getDt()));
	// set dt
	integral.سېټ_Dt(0.1f);
	ASSERT_CL(equal(0.1f, integral.getDt()));
	// set Y
	integral.سېټ_Y(0.9f);
	ASSERT_CL(equal(0.9f, integral.getY()));

	// test exceed max
	لكل (صحيح i = 0; i < 100; i++) {
		integral.update(1.0f);
	}

	ASSERT_CL(equal(1.0f, integral.update(1.0f)));
	// test exceed min
	integral.سېټ_Y(-0.9f);
	ASSERT_CL(equal(-0.9f, integral.getY()));

	لكل (صحيح i = 0; i < 100; i++) {
		integral.update(-1.0f);
	}

	ASSERT_CL(equal(-1.0f, integral.update(-1.0f)));
	// test update
	integral.سېټ_Y(0.1f);
	ASSERT_CL(equal(0.2f, integral.update(1.0)));
	ASSERT_CL(equal(0.2f, integral.getY()));
	چاپ_کړه("PASS\n");
	أرجع 0;
}

صحيح بلاک_Integral_Trap_Test()
{
	چاپ_کړه("Test BlockIntegralTrap\t\t: ");
	بلاک_Integral_Trap integral(تش_اشاره, "TEST_I");
	// test initial state
	ASSERT_CL(equal(1.0f, integral.getMax()));
	ASSERT_CL(equal(0.0f, integral.getDt()));
	// set dt
	integral.سېټ_Dt(0.1f);
	ASSERT_CL(equal(0.1f, integral.getDt()));
	// set U
	integral.سېټ_U(1.0f);
	ASSERT_CL(equal(1.0f, integral.getU()));
	// set Y
	integral.سېټ_Y(0.9f);
	ASSERT_CL(equal(0.9f, integral.getY()));

	// test exceed max
	لكل (صحيح i = 0; i < 100; i++) {
		integral.update(1.0f);
	}

	ASSERT_CL(equal(1.0f, integral.update(1.0f)));
	// test exceed min
	integral.سېټ_U(-1.0f);
	integral.سېټ_Y(-0.9f);
	ASSERT_CL(equal(-0.9f, integral.getY()));

	لكل (صحيح i = 0; i < 100; i++) {
		integral.update(-1.0f);
	}

	ASSERT_CL(equal(-1.0f, integral.update(-1.0f)));
	// test update
	integral.سېټ_U(2.0f);
	integral.سېټ_Y(0.1f);
	ASSERT_CL(equal(0.25f, integral.update(1.0)));
	ASSERT_CL(equal(0.25f, integral.getY()));
	ASSERT_CL(equal(1.0f, integral.getU()));
	چاپ_کړه("PASS\n");
	أرجع 0;
}

صحيح بلاک_Derivative_Test()
{
	چاپ_کړه("Test BlockDerivative\t\t: ");
	بلاک_Derivative derivative(تش_اشاره, "TEST_D");
	// test initial state
	ASSERT_CL(equal(0.0f, derivative.getU()));
	ASSERT_CL(equal(10.0f, derivative.getLP()));
	// set dt
	derivative.سېټ_Dt(0.1f);
	ASSERT_CL(equal(0.1f, derivative.getDt()));
	// set U
	derivative.سېټ_U(1.0f);
	ASSERT_CL(equal(1.0f, derivative.getU()));
	// perform one update so initialized is set
	derivative.update(1.0);
	ASSERT_CL(equal(1.0f, derivative.getU()));
	// test  update
	ASSERT_CL(equal(8.6269744f, derivative.update(2.0f)));
	ASSERT_CL(equal(2.0f, derivative.getU()));
	چاپ_کړه("PASS\n");
	أرجع 0;
}

صحيح بلاک_PTest()
{
	چاپ_کړه("Test BlockP\t\t\t: ");
	بلاک_P بلاک_P_2(تش_اشاره, "TEST_P");
	// test initial state
	ASSERT_CL(equal(0.2f, بلاک_P_2.getKP()));
	ASSERT_CL(equal(0.0f, بلاک_P_2.getDt()));
	// set dt
	بلاک_P_2.سېټ_Dt(0.1f);
	ASSERT_CL(equal(0.1f, بلاک_P_2.getDt()));
	// test  update
	ASSERT_CL(equal(0.4f, بلاک_P_2.update(2.0f)));
	چاپ_کړه("PASS\n");
	أرجع 0;
}

صحيح بلاک_PITest()
{
	چاپ_کړه("Test BlockPI\t\t\t: ");
	بلاک_PI بلاک_PI_2(تش_اشاره, "TEST");
	// test initial state
	ASSERT_CL(equal(0.2f, بلاک_PI_2.getKP()));
	ASSERT_CL(equal(0.1f, بلاک_PI_2.getKI()));
	ASSERT_CL(equal(0.0f, بلاک_PI_2.getDt()));
	ASSERT_CL(equal(1.0f, بلاک_PI_2.getIntegral().getMax()));
	// set dt
	بلاک_PI_2.سېټ_Dt(0.1f);
	ASSERT_CL(equal(0.1f, بلاک_PI_2.getDt()));
	// set integral state
	بلاک_PI_2.getIntegral().سېټ_Y(0.1f);
	ASSERT_CL(equal(0.1f, بلاک_PI_2.getIntegral().getY()));
	// test  update
	// 0.2*2 + 0.1*(2*0.1 + 0.1) = 0.43
	ASSERT_CL(equal(0.43f, بلاک_PI_2.update(2.0f)));
	چاپ_کړه("PASS\n");
	أرجع 0;
}

صحيح بلاک_PDTest()
{
	چاپ_کړه("Test BlockPD\t\t\t: ");
	بلاک_PD بلاک_PD_2(تش_اشاره, "TEST");
	// test initial state
	ASSERT_CL(equal(0.2f, بلاک_PD_2.getKP()));
	ASSERT_CL(equal(0.01f, بلاک_PD_2.getKD()));
	ASSERT_CL(equal(0.0f, بلاک_PD_2.getDt()));
	ASSERT_CL(equal(10.0f, بلاک_PD_2.getDerivative().getLP()));
	// set dt
	بلاک_PD_2.سېټ_Dt(0.1f);
	ASSERT_CL(equal(0.1f, بلاک_PD_2.getDt()));
	// set derivative state
	بلاک_PD_2.getDerivative().سېټ_U(1.0f);
	ASSERT_CL(equal(1.0f, بلاک_PD_2.getDerivative().getU()));
	// perform one update so initialized is set
	بلاک_PD_2.getDerivative().update(1.0);
	ASSERT_CL(equal(1.0f, بلاک_PD_2.getDerivative().getU()));
	// test  update
	// 0.2*2 + 0.1*(0.1*8.626...) = 0.486269744
	ASSERT_CL(equal(0.486269744f, بلاک_PD_2.update(2.0f)));
	چاپ_کړه("PASS\n");
	أرجع 0;
}

صحيح بلاک_PIDTest()
{
	چاپ_کړه("Test BlockPID\t\t\t: ");
	بلاک_PID بلاک_PID_2(تش_اشاره, "TEST");
	// test initial state
	ASSERT_CL(equal(0.2f, بلاک_PID_2.getKP()));
	ASSERT_CL(equal(0.1f, بلاک_PID_2.getKI()));
	ASSERT_CL(equal(0.01f, بلاک_PID_2.getKD()));
	ASSERT_CL(equal(0.0f, بلاک_PID_2.getDt()));
	ASSERT_CL(equal(10.0f, بلاک_PID_2.getDerivative().getLP()));
	ASSERT_CL(equal(1.0f, بلاک_PID_2.getIntegral().getMax()));
	// set dt
	بلاک_PID_2.سېټ_Dt(0.1f);
	ASSERT_CL(equal(0.1f, بلاک_PID_2.getDt()));
	// set derivative state
	بلاک_PID_2.getDerivative().سېټ_U(1.0f);
	ASSERT_CL(equal(1.0f, بلاک_PID_2.getDerivative().getU()));
	// perform one update so initialized is set
	بلاک_PID_2.getDerivative().update(1.0);
	ASSERT_CL(equal(1.0f, بلاک_PID_2.getDerivative().getU()));
	// set integral state
	بلاک_PID_2.getIntegral().سېټ_Y(0.1f);
	ASSERT_CL(equal(0.1f, بلاک_PID_2.getIntegral().getY()));
	// test  update
	// 0.2*2 + 0.1*(2*0.1 + 0.1) + 0.1*(0.1*8.626...) = 0.5162697
	ASSERT_CL(equal(0.5162697f, بلاک_PID_2.update(2.0f)));
	چاپ_کړه("PASS\n");
	أرجع 0;
}

صحيح بلاک_Output_Test()
{
	چاپ_کړه("Test BlockOutput\t\t: ");
	بلاک_Output بلاک_Output_2(تش_اشاره, "TEST");
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
	چاپ_کړه("PASS\n");
	أرجع 0;
}

صحيح بلاک_Rand_یوشان_Test()
{
	srand(1234);
	چاپ_کړه("Test BlockRandUniform\t\t: ");
	بلاک_Rand_یوشان بلاک_Rand_یوشان_2(تش_اشاره, "TEST");
	// test initial state
	ASSERT_CL(equal(0.0f, بلاک_Rand_یوشان_2.getDt()));
	ASSERT_CL(equal(-1.0f, بلاک_Rand_یوشان_2.getMin()));
	ASSERT_CL(equal(1.0f, بلاک_Rand_یوشان_2.getMax()));
	// test update
	صحيح n = 10000;
	عائم منځنۍ = بلاک_Rand_یوشان_2.update();

	لكل (صحيح i = 2; i < n + 1; i++) {
		عائم val = بلاک_Rand_یوشان_2.update();
		منځنۍ += (val - منځنۍ) / i;
		ASSERT_CL(less_than_or_equal(val, بلاک_Rand_یوشان_2.getMax()));
		ASSERT_CL(greater_than_or_equal(val, بلاک_Rand_یوشان_2.getMin()));
	}

	ASSERT_CL(equal(منځنۍ, (بلاک_Rand_یوشان_2.getMin() +
			       بلاک_Rand_یوشان_2.getMax()) / 2, 1e-1));
	چاپ_کړه("PASS\n");
	أرجع 0;
}

صحيح بلاک_Rand_Gauss_Test()
{
	srand(1234);
	چاپ_کړه("Test BlockRandGauss\t\t: ");
	بلاک_Rand_Gauss بلاک_Rand_Gauss_2(تش_اشاره, "TEST");
	// test initial state
	ASSERT_CL(equal(0.0f, بلاک_Rand_Gauss_2.getDt()));
	ASSERT_CL(equal(1.0f, بلاک_Rand_Gauss_2.get_منځنۍ()));
	ASSERT_CL(equal(2.0f, بلاک_Rand_Gauss_2.getStdDev()));
	// test update
	صحيح n = 10000;
	عائم منځنۍ = بلاک_Rand_Gauss_2.update();
	عائم مجموعه = 0;

	// recursive mean, stdev algorithm from Knuth
	لكل (صحيح i = 2; i < n + 1; i++) {
		عائم val = بلاک_Rand_Gauss_2.update();
		عائم new_منځنۍ = منځنۍ + (val - منځنۍ) / i;
		مجموعه += (val - منځنۍ) * (val - new_منځنۍ);
		منځنۍ = new_منځنۍ;
	}

	عائم stdDev = sqrtf(مجموعه / (n - 1));
	(فراغ)(stdDev);
	ASSERT_CL(equal(منځنۍ, بلاک_Rand_Gauss_2.get_منځنۍ(), 1e-1));
	ASSERT_CL(equal(stdDev, بلاک_Rand_Gauss_2.getStdDev(), 1e-1));
	چاپ_کړه("PASS\n");
	أرجع 0;
}

صحيح بلاک_Stats_Test()
{
	چاپ_کړه("Test BlockStats\t\t\t: ");
	بلاک_Stats<عائم, 1> stats(تش_اشاره, "TEST");
	ASSERT_CL(equal(0.0f, stats.get_منځنۍ()(0)));
	ASSERT_CL(equal(0.0f, stats.getStdDev()(0)));
	stats.update(ماتریکس::Scalar<عائم>(1.0f));
	stats.update(ماتریکس::Scalar<عائم>(2));
	ASSERT_CL(equal(1.5f, stats.get_منځنۍ()(0)));
	ASSERT_CL(equal(0.5f, stats.getStdDev()(0)));
	stats.reset();
	ASSERT_CL(equal(0.0f, stats.get_منځنۍ()(0)));
	ASSERT_CL(equal(0.0f, stats.getStdDev()(0)));
	چاپ_کړه("PASS\n");
	أرجع 0;
}

صحيح بلاک_Delay_Test()
{
	چاپ_کړه("Test BlockDelay\t\t\t: ");
	کارول نطاق ماتریکس;
	بلاک_Delay<عائم, 2, 1, 3> delay(تش_اشاره, "TEST");
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
	چاپ_کړه("PASS\n");
	أرجع 0;
}

خارجي "C" __EXPORT صحيح controllib_test_main(صحيح argc, حرف *argv[]);

صحيح controllib_test_main(صحيح argc, حرف *argv[])
{
	أرجع basic_بلاکونه_Test();
}
