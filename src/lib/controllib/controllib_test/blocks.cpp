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

#تعریف_کن ASSERT_CL(T) إذا (!(T)) { چاپ_کن("FAIL\n"); أرجع -1; }

نطاق control
{

صحيح basic_بلوکها_Test();
صحيح بلوک_حد_Test();
صحيح بلوک_حد_Sym_Test();
صحيح بلوک_کم_Pass_Test();
صحيح بلوک_بالا_Pass_Test();
صحيح بلوک_کم_Pass2_Test();
صحيح بلوک_Integral_Test();
صحيح بلوک_Integral_Trap_Test();
صحيح بلوک_Derivative_Test();
صحيح بلوک_PTest();
صحيح بلوک_PITest();
صحيح بلوک_PDTest();
صحيح بلوک_PIDTest();
صحيح بلوک_Output_Test();
صحيح بلوک_Rand_یکنواخت_Test();
صحيح بلوک_Rand_Gauss_Test();
صحيح بلوک_Stats_Test();
صحيح بلوک_Delay_Test();

صحيح basic_بلوکها_Test()
{
	بلوک_حد_Test();
	بلوک_حد_Sym_Test();
	بلوک_کم_Pass_Test();
	بلوک_بالا_Pass_Test();
	بلوک_کم_Pass2_Test();
	بلوک_Integral_Test();
	بلوک_Integral_Trap_Test();
	بلوک_Derivative_Test();
	بلوک_PTest();
	بلوک_PITest();
	بلوک_PDTest();
	بلوک_PIDTest();
	بلوک_Output_Test();
	//blockRandUniformTest();
	// known failures
	// blockRandGaussTest();
	بلوک_Stats_Test();
	بلوک_Delay_Test();
	أرجع 0;
}

صحيح بلوک_حد_Test()
{
	چاپ_کن("Test BlockLimit\t\t\t: ");
	بلوک_حد حد_2(عدم, "TEST");
	// initial state
	ASSERT_CL(equal(1.0f, حد_2.getMax()));
	ASSERT_CL(equal(-1.0f, حد_2.getMin()));
	ASSERT_CL(equal(0.0f, حد_2.getDt()));
	// update
	ASSERT_CL(equal(-1.0f, حد_2.update(-2.0f)));
	ASSERT_CL(equal(1.0f, حد_2.update(2.0f)));
	ASSERT_CL(equal(0.0f, حد_2.update(0.0f)));
	چاپ_کن("PASS\n");
	أرجع 0;
}

صحيح بلوک_حد_Sym_Test()
{
	چاپ_کن("Test BlockLimitSym\t\t: ");
	بلوک_حد_Sym حد_2(عدم, "TEST");
	// initial state
	ASSERT_CL(equal(1.0f, حد_2.getMax()));
	ASSERT_CL(equal(0.0f, حد_2.getDt()));
	// update
	ASSERT_CL(equal(-1.0f, حد_2.update(-2.0f)));
	ASSERT_CL(equal(1.0f, حد_2.update(2.0f)));
	ASSERT_CL(equal(0.0f, حد_2.update(0.0f)));
	چاپ_کن("PASS\n");
	أرجع 0;
}

صحيح بلوک_کم_Pass_Test()
{
	چاپ_کن("Test BlockLowPass\t\t: ");
	بلوک_کم_Pass کم_Pass_2(عدم, "TEST_LP");
	// test initial state
	ASSERT_CL(equal(10.0f, کم_Pass_2.getFCut()));
	ASSERT_CL(equal(0.0f, کم_Pass_2.getState()));
	ASSERT_CL(equal(0.0f, کم_Pass_2.getDt()));
	// set dt
	کم_Pass_2.مجموعه_Dt(0.1f);
	ASSERT_CL(equal(0.1f, کم_Pass_2.getDt()));
	// set state
	کم_Pass_2.مجموعه_State(1.0f);
	ASSERT_CL(equal(1.0f, کم_Pass_2.getState()));
	// test update
	ASSERT_CL(equal(1.8626974f, کم_Pass_2.update(2.0f)));

	// test end condition
	لكل (صحيح i = 0; i < 100; i++) {
		کم_Pass_2.update(2.0f);
	}

	ASSERT_CL(equal(2.0f, کم_Pass_2.getState()));
	ASSERT_CL(equal(2.0f, کم_Pass_2.update(2.0f)));
	چاپ_کن("PASS\n");
	أرجع 0;
};

صحيح بلوک_بالا_Pass_Test()
{
	چاپ_کن("Test BlockHighPass\t\t: ");
	بلوک_بالا_Pass بالا_Pass(عدم, "TEST_HP");
	// test initial state
	ASSERT_CL(equal(10.0f, بالا_Pass.getFCut()));
	ASSERT_CL(equal(0.0f, بالا_Pass.getU()));
	ASSERT_CL(equal(0.0f, بالا_Pass.getY()));
	ASSERT_CL(equal(0.0f, بالا_Pass.getDt()));
	// set dt
	بالا_Pass.مجموعه_Dt(0.1f);
	ASSERT_CL(equal(0.1f, بالا_Pass.getDt()));
	// set state
	بالا_Pass.مجموعه_U(1.0f);
	ASSERT_CL(equal(1.0f, بالا_Pass.getU()));
	بالا_Pass.مجموعه_Y(1.0f);
	ASSERT_CL(equal(1.0f, بالا_Pass.getY()));
	// test update
	ASSERT_CL(equal(0.2746051f, بالا_Pass.update(2.0f)));

	// test end condition
	لكل (صحيح i = 0; i < 100; i++) {
		بالا_Pass.update(2.0f);
	}

	ASSERT_CL(equal(0.0f, بالا_Pass.getY()));
	ASSERT_CL(equal(0.0f, بالا_Pass.update(2.0f)));
	چاپ_کن("PASS\n");
	أرجع 0;
}

صحيح بلوک_کم_Pass2_Test()
{
	چاپ_کن("Test BlockLowPass2\t\t: ");
	بلوک_کم_Pass2 کم_Pass_2(عدم, "TEST_LP", 100);
	// test initial state
	ASSERT_CL(equal(10.0f, کم_Pass_2.getFCutParam()));
	ASSERT_CL(equal(0.0f, کم_Pass_2.getState()));
	ASSERT_CL(equal(0.0f, کم_Pass_2.getDt()));
	// set dt
	کم_Pass_2.مجموعه_Dt(0.1f);
	ASSERT_CL(equal(0.1f, کم_Pass_2.getDt()));
	// set state
	کم_Pass_2.مجموعه_State(1.0f);
	ASSERT_CL(equal(1.0f, کم_Pass_2.getState()));
	// test update
	ASSERT_CL(equal(1.06745527f, کم_Pass_2.update(2.0f)));

	// test end condition
	لكل (صحيح i = 0; i < 100; i++) {
		کم_Pass_2.update(2.0f);
	}

	ASSERT_CL(equal(2.0f, کم_Pass_2.getState()));
	ASSERT_CL(equal(2.0f, کم_Pass_2.update(2.0f)));
	چاپ_کن("PASS\n");
	أرجع 0;
};

صحيح بلوک_Integral_Test()
{
	چاپ_کن("Test BlockIntegral\t\t: ");
	بلوک_Integral integral(عدم, "TEST_I");
	// test initial state
	ASSERT_CL(equal(1.0f, integral.getMax()));
	ASSERT_CL(equal(0.0f, integral.getDt()));
	// set dt
	integral.مجموعه_Dt(0.1f);
	ASSERT_CL(equal(0.1f, integral.getDt()));
	// set Y
	integral.مجموعه_Y(0.9f);
	ASSERT_CL(equal(0.9f, integral.getY()));

	// test exceed max
	لكل (صحيح i = 0; i < 100; i++) {
		integral.update(1.0f);
	}

	ASSERT_CL(equal(1.0f, integral.update(1.0f)));
	// test exceed min
	integral.مجموعه_Y(-0.9f);
	ASSERT_CL(equal(-0.9f, integral.getY()));

	لكل (صحيح i = 0; i < 100; i++) {
		integral.update(-1.0f);
	}

	ASSERT_CL(equal(-1.0f, integral.update(-1.0f)));
	// test update
	integral.مجموعه_Y(0.1f);
	ASSERT_CL(equal(0.2f, integral.update(1.0)));
	ASSERT_CL(equal(0.2f, integral.getY()));
	چاپ_کن("PASS\n");
	أرجع 0;
}

صحيح بلوک_Integral_Trap_Test()
{
	چاپ_کن("Test BlockIntegralTrap\t\t: ");
	بلوک_Integral_Trap integral(عدم, "TEST_I");
	// test initial state
	ASSERT_CL(equal(1.0f, integral.getMax()));
	ASSERT_CL(equal(0.0f, integral.getDt()));
	// set dt
	integral.مجموعه_Dt(0.1f);
	ASSERT_CL(equal(0.1f, integral.getDt()));
	// set U
	integral.مجموعه_U(1.0f);
	ASSERT_CL(equal(1.0f, integral.getU()));
	// set Y
	integral.مجموعه_Y(0.9f);
	ASSERT_CL(equal(0.9f, integral.getY()));

	// test exceed max
	لكل (صحيح i = 0; i < 100; i++) {
		integral.update(1.0f);
	}

	ASSERT_CL(equal(1.0f, integral.update(1.0f)));
	// test exceed min
	integral.مجموعه_U(-1.0f);
	integral.مجموعه_Y(-0.9f);
	ASSERT_CL(equal(-0.9f, integral.getY()));

	لكل (صحيح i = 0; i < 100; i++) {
		integral.update(-1.0f);
	}

	ASSERT_CL(equal(-1.0f, integral.update(-1.0f)));
	// test update
	integral.مجموعه_U(2.0f);
	integral.مجموعه_Y(0.1f);
	ASSERT_CL(equal(0.25f, integral.update(1.0)));
	ASSERT_CL(equal(0.25f, integral.getY()));
	ASSERT_CL(equal(1.0f, integral.getU()));
	چاپ_کن("PASS\n");
	أرجع 0;
}

صحيح بلوک_Derivative_Test()
{
	چاپ_کن("Test BlockDerivative\t\t: ");
	بلوک_Derivative derivative(عدم, "TEST_D");
	// test initial state
	ASSERT_CL(equal(0.0f, derivative.getU()));
	ASSERT_CL(equal(10.0f, derivative.getLP()));
	// set dt
	derivative.مجموعه_Dt(0.1f);
	ASSERT_CL(equal(0.1f, derivative.getDt()));
	// set U
	derivative.مجموعه_U(1.0f);
	ASSERT_CL(equal(1.0f, derivative.getU()));
	// perform one update so initialized is set
	derivative.update(1.0);
	ASSERT_CL(equal(1.0f, derivative.getU()));
	// test  update
	ASSERT_CL(equal(8.6269744f, derivative.update(2.0f)));
	ASSERT_CL(equal(2.0f, derivative.getU()));
	چاپ_کن("PASS\n");
	أرجع 0;
}

صحيح بلوک_PTest()
{
	چاپ_کن("Test BlockP\t\t\t: ");
	بلوک_P بلوک_P_2(عدم, "TEST_P");
	// test initial state
	ASSERT_CL(equal(0.2f, بلوک_P_2.getKP()));
	ASSERT_CL(equal(0.0f, بلوک_P_2.getDt()));
	// set dt
	بلوک_P_2.مجموعه_Dt(0.1f);
	ASSERT_CL(equal(0.1f, بلوک_P_2.getDt()));
	// test  update
	ASSERT_CL(equal(0.4f, بلوک_P_2.update(2.0f)));
	چاپ_کن("PASS\n");
	أرجع 0;
}

صحيح بلوک_PITest()
{
	چاپ_کن("Test BlockPI\t\t\t: ");
	بلوک_PI بلوک_PI_2(عدم, "TEST");
	// test initial state
	ASSERT_CL(equal(0.2f, بلوک_PI_2.getKP()));
	ASSERT_CL(equal(0.1f, بلوک_PI_2.getKI()));
	ASSERT_CL(equal(0.0f, بلوک_PI_2.getDt()));
	ASSERT_CL(equal(1.0f, بلوک_PI_2.getIntegral().getMax()));
	// set dt
	بلوک_PI_2.مجموعه_Dt(0.1f);
	ASSERT_CL(equal(0.1f, بلوک_PI_2.getDt()));
	// set integral state
	بلوک_PI_2.getIntegral().مجموعه_Y(0.1f);
	ASSERT_CL(equal(0.1f, بلوک_PI_2.getIntegral().getY()));
	// test  update
	// 0.2*2 + 0.1*(2*0.1 + 0.1) = 0.43
	ASSERT_CL(equal(0.43f, بلوک_PI_2.update(2.0f)));
	چاپ_کن("PASS\n");
	أرجع 0;
}

صحيح بلوک_PDTest()
{
	چاپ_کن("Test BlockPD\t\t\t: ");
	بلوک_PD بلوک_PD_2(عدم, "TEST");
	// test initial state
	ASSERT_CL(equal(0.2f, بلوک_PD_2.getKP()));
	ASSERT_CL(equal(0.01f, بلوک_PD_2.getKD()));
	ASSERT_CL(equal(0.0f, بلوک_PD_2.getDt()));
	ASSERT_CL(equal(10.0f, بلوک_PD_2.getDerivative().getLP()));
	// set dt
	بلوک_PD_2.مجموعه_Dt(0.1f);
	ASSERT_CL(equal(0.1f, بلوک_PD_2.getDt()));
	// set derivative state
	بلوک_PD_2.getDerivative().مجموعه_U(1.0f);
	ASSERT_CL(equal(1.0f, بلوک_PD_2.getDerivative().getU()));
	// perform one update so initialized is set
	بلوک_PD_2.getDerivative().update(1.0);
	ASSERT_CL(equal(1.0f, بلوک_PD_2.getDerivative().getU()));
	// test  update
	// 0.2*2 + 0.1*(0.1*8.626...) = 0.486269744
	ASSERT_CL(equal(0.486269744f, بلوک_PD_2.update(2.0f)));
	چاپ_کن("PASS\n");
	أرجع 0;
}

صحيح بلوک_PIDTest()
{
	چاپ_کن("Test BlockPID\t\t\t: ");
	بلوک_PID بلوک_PID_2(عدم, "TEST");
	// test initial state
	ASSERT_CL(equal(0.2f, بلوک_PID_2.getKP()));
	ASSERT_CL(equal(0.1f, بلوک_PID_2.getKI()));
	ASSERT_CL(equal(0.01f, بلوک_PID_2.getKD()));
	ASSERT_CL(equal(0.0f, بلوک_PID_2.getDt()));
	ASSERT_CL(equal(10.0f, بلوک_PID_2.getDerivative().getLP()));
	ASSERT_CL(equal(1.0f, بلوک_PID_2.getIntegral().getMax()));
	// set dt
	بلوک_PID_2.مجموعه_Dt(0.1f);
	ASSERT_CL(equal(0.1f, بلوک_PID_2.getDt()));
	// set derivative state
	بلوک_PID_2.getDerivative().مجموعه_U(1.0f);
	ASSERT_CL(equal(1.0f, بلوک_PID_2.getDerivative().getU()));
	// perform one update so initialized is set
	بلوک_PID_2.getDerivative().update(1.0);
	ASSERT_CL(equal(1.0f, بلوک_PID_2.getDerivative().getU()));
	// set integral state
	بلوک_PID_2.getIntegral().مجموعه_Y(0.1f);
	ASSERT_CL(equal(0.1f, بلوک_PID_2.getIntegral().getY()));
	// test  update
	// 0.2*2 + 0.1*(2*0.1 + 0.1) + 0.1*(0.1*8.626...) = 0.5162697
	ASSERT_CL(equal(0.5162697f, بلوک_PID_2.update(2.0f)));
	چاپ_کن("PASS\n");
	أرجع 0;
}

صحيح بلوک_Output_Test()
{
	چاپ_کن("Test BlockOutput\t\t: ");
	بلوک_Output بلوک_Output_2(عدم, "TEST");
	// test initial state
	ASSERT_CL(equal(0.0f, بلوک_Output_2.getDt()));
	ASSERT_CL(equal(0.5f, بلوک_Output_2.get()));
	ASSERT_CL(equal(-1.0f, بلوک_Output_2.getMin()));
	ASSERT_CL(equal(1.0f, بلوک_Output_2.getMax()));
	// test update below min
	بلوک_Output_2.update(-2.0f);
	ASSERT_CL(equal(-1.0f, بلوک_Output_2.get()));
	// test update above max
	بلوک_Output_2.update(2.0f);
	ASSERT_CL(equal(1.0f, بلوک_Output_2.get()));
	// test trim
	بلوک_Output_2.update(0.0f);
	ASSERT_CL(equal(0.5f, بلوک_Output_2.get()));
	چاپ_کن("PASS\n");
	أرجع 0;
}

صحيح بلوک_Rand_یکنواخت_Test()
{
	srand(1234);
	چاپ_کن("Test BlockRandUniform\t\t: ");
	بلوک_Rand_یکنواخت بلوک_Rand_یکنواخت_2(عدم, "TEST");
	// test initial state
	ASSERT_CL(equal(0.0f, بلوک_Rand_یکنواخت_2.getDt()));
	ASSERT_CL(equal(-1.0f, بلوک_Rand_یکنواخت_2.getMin()));
	ASSERT_CL(equal(1.0f, بلوک_Rand_یکنواخت_2.getMax()));
	// test update
	صحيح n = 10000;
	عائم میانگین = بلوک_Rand_یکنواخت_2.update();

	لكل (صحيح i = 2; i < n + 1; i++) {
		عائم val = بلوک_Rand_یکنواخت_2.update();
		میانگین += (val - میانگین) / i;
		ASSERT_CL(less_than_or_equal(val, بلوک_Rand_یکنواخت_2.getMax()));
		ASSERT_CL(greater_than_or_equal(val, بلوک_Rand_یکنواخت_2.getMin()));
	}

	ASSERT_CL(equal(میانگین, (بلوک_Rand_یکنواخت_2.getMin() +
			       بلوک_Rand_یکنواخت_2.getMax()) / 2, 1e-1));
	چاپ_کن("PASS\n");
	أرجع 0;
}

صحيح بلوک_Rand_Gauss_Test()
{
	srand(1234);
	چاپ_کن("Test BlockRandGauss\t\t: ");
	بلوک_Rand_Gauss بلوک_Rand_Gauss_2(عدم, "TEST");
	// test initial state
	ASSERT_CL(equal(0.0f, بلوک_Rand_Gauss_2.getDt()));
	ASSERT_CL(equal(1.0f, بلوک_Rand_Gauss_2.get_میانگین()));
	ASSERT_CL(equal(2.0f, بلوک_Rand_Gauss_2.getStdDev()));
	// test update
	صحيح n = 10000;
	عائم میانگین = بلوک_Rand_Gauss_2.update();
	عائم جمع = 0;

	// recursive mean, stdev algorithm from Knuth
	لكل (صحيح i = 2; i < n + 1; i++) {
		عائم val = بلوک_Rand_Gauss_2.update();
		عائم new_میانگین = میانگین + (val - میانگین) / i;
		جمع += (val - میانگین) * (val - new_میانگین);
		میانگین = new_میانگین;
	}

	عائم stdDev = جذر(جمع / (n - 1));
	(فراغ)(stdDev);
	ASSERT_CL(equal(میانگین, بلوک_Rand_Gauss_2.get_میانگین(), 1e-1));
	ASSERT_CL(equal(stdDev, بلوک_Rand_Gauss_2.getStdDev(), 1e-1));
	چاپ_کن("PASS\n");
	أرجع 0;
}

صحيح بلوک_Stats_Test()
{
	چاپ_کن("Test BlockStats\t\t\t: ");
	بلوک_Stats<عائم, 1> stats(عدم, "TEST");
	ASSERT_CL(equal(0.0f, stats.get_میانگین()(0)));
	ASSERT_CL(equal(0.0f, stats.getStdDev()(0)));
	stats.update(ماتریس::Scalar<عائم>(1.0f));
	stats.update(ماتریس::Scalar<عائم>(2));
	ASSERT_CL(equal(1.5f, stats.get_میانگین()(0)));
	ASSERT_CL(equal(0.5f, stats.getStdDev()(0)));
	stats.reset();
	ASSERT_CL(equal(0.0f, stats.get_میانگین()(0)));
	ASSERT_CL(equal(0.0f, stats.getStdDev()(0)));
	چاپ_کن("PASS\n");
	أرجع 0;
}

صحيح بلوک_Delay_Test()
{
	چاپ_کن("Test BlockDelay\t\t\t: ");
	استفاده نطاق ماتریس;
	بلوک_Delay<عائم, 2, 1, 3> delay(عدم, "TEST");
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
	چاپ_کن("PASS\n");
	أرجع 0;
}

} // namespace control
