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

#అన్తర్భావ <math.h>
#అన్తర్భావ <stdio.h>
#అన్తర్భావ <float.h>

#అన్తర్భావ <controllib/blocks.hpp>

ఉపయోగించి నామాకాశ control;

#నిర్వచించు ASSERT_CL(T) ఒకవేళ (!(T)) { రాయి("FAIL\n"); తిరిగి -1; }

పూర్ణాంకం basic_ఖండాలు_Test();
పూర్ణాంకం ఖండం_హద్దు_Test();
పూర్ణాంకం ఖండం_హద్దు_Sym_Test();
పూర్ణాంకం ఖండం_దిగువ_Pass_Test();
పూర్ణాంకం ఖండం_ఎగువ_Pass_Test();
పూర్ణాంకం ఖండం_దిగువ_Pass2_Test();
పూర్ణాంకం ఖండం_Integral_Test();
పూర్ణాంకం ఖండం_Integral_Trap_Test();
పూర్ణాంకం ఖండం_Derivative_Test();
పూర్ణాంకం ఖండం_PTest();
పూర్ణాంకం ఖండం_PITest();
పూర్ణాంకం ఖండం_PDTest();
పూర్ణాంకం ఖండం_PIDTest();
పూర్ణాంకం ఖండం_Output_Test();
పూర్ణాంకం ఖండం_Rand_ఏకరీతి_Test();
పూర్ణాంకం ఖండం_Rand_Gauss_Test();
పూర్ణాంకం ఖండం_Stats_Test();
పూర్ణాంకం ఖండం_Delay_Test();

పూర్ణాంకం basic_ఖండాలు_Test()
{
	తార్కిక failed = అబద్ధం;
	failed = failed || ఖండం_హద్దు_Test() < 0;
	failed = failed || ఖండం_హద్దు_Sym_Test() < 0;
	failed = failed || ఖండం_దిగువ_Pass_Test() < 0;
	failed = failed || ఖండం_ఎగువ_Pass_Test() < 0;
	failed = failed || ఖండం_దిగువ_Pass2_Test() < 0;
	failed = failed || ఖండం_Integral_Test() < 0;
	failed = failed || ఖండం_Integral_Trap_Test() < 0;
	failed = failed || ఖండం_Derivative_Test() < 0;
	failed = failed || ఖండం_PTest() < 0;
	failed = failed || ఖండం_PITest() < 0;
	failed = failed || ఖండం_PDTest() < 0;
	failed = failed || ఖండం_PIDTest() < 0;
	failed = failed || ఖండం_Output_Test() < 0;
	failed = failed || ఖండం_Rand_ఏకరీతి_Test() < 0;
	failed = failed || ఖండం_Rand_Gauss_Test() < 0;
	failed = failed || ఖండం_Stats_Test() < 0;
	failed = failed || ఖండం_Delay_Test() < 0;
	తిరిగి failed ? -1 : 0;
}

పూర్ణాంకం ఖండం_హద్దు_Test()
{
	రాయి("Test BlockLimit\t\t\t: ");
	ఖండం_హద్దు హద్దు_2(శూన్యసూచిక, "TEST");
	// initial state
	ASSERT_CL(equal(1.0f, హద్దు_2.getMax()));
	ASSERT_CL(equal(-1.0f, హద్దు_2.getMin()));
	ASSERT_CL(equal(0.0f, హద్దు_2.getDt()));
	// update
	ASSERT_CL(equal(-1.0f, హద్దు_2.update(-2.0f)));
	ASSERT_CL(equal(1.0f, హద్దు_2.update(2.0f)));
	ASSERT_CL(equal(0.0f, హద్దు_2.update(0.0f)));
	రాయి("PASS\n");
	తిరిగి 0;
}

పూర్ణాంకం ఖండం_హద్దు_Sym_Test()
{
	రాయి("Test BlockLimitSym\t\t: ");
	ఖండం_హద్దు_Sym హద్దు_2(శూన్యసూచిక, "TEST");
	// initial state
	ASSERT_CL(equal(1.0f, హద్దు_2.getMax()));
	ASSERT_CL(equal(0.0f, హద్దు_2.getDt()));
	// update
	ASSERT_CL(equal(-1.0f, హద్దు_2.update(-2.0f)));
	ASSERT_CL(equal(1.0f, హద్దు_2.update(2.0f)));
	ASSERT_CL(equal(0.0f, హద్దు_2.update(0.0f)));
	రాయి("PASS\n");
	తిరిగి 0;
}

పూర్ణాంకం ఖండం_దిగువ_Pass_Test()
{
	రాయి("Test BlockLowPass\t\t: ");
	ఖండం_దిగువ_Pass దిగువ_Pass_2(శూన్యసూచిక, "TEST_LP");
	// test initial state
	ASSERT_CL(equal(10.0f, దిగువ_Pass_2.getFCut()));
	ASSERT_CL(equal(0.0f, దిగువ_Pass_2.getState()));
	ASSERT_CL(equal(0.0f, దిగువ_Pass_2.getDt()));
	// set dt
	దిగువ_Pass_2.సమితి_Dt(0.1f);
	ASSERT_CL(equal(0.1f, దిగువ_Pass_2.getDt()));
	// set state
	దిగువ_Pass_2.సమితి_State(1.0f);
	ASSERT_CL(equal(1.0f, దిగువ_Pass_2.getState()));
	// test update
	ASSERT_CL(equal(1.8626974f, దిగువ_Pass_2.update(2.0f)));

	// test end condition
	కొరకు (పూర్ణాంకం i = 0; i < 100; i++) {
		దిగువ_Pass_2.update(2.0f);
	}

	ASSERT_CL(equal(2.0f, దిగువ_Pass_2.getState()));
	ASSERT_CL(equal(2.0f, దిగువ_Pass_2.update(2.0f)));
	రాయి("PASS\n");
	తిరిగి 0;
};

పూర్ణాంకం ఖండం_ఎగువ_Pass_Test()
{
	రాయి("Test BlockHighPass\t\t: ");
	ఖండం_ఎగువ_Pass ఎగువ_Pass(శూన్యసూచిక, "TEST_HP");
	// test initial state
	ASSERT_CL(equal(10.0f, ఎగువ_Pass.getFCut()));
	ASSERT_CL(equal(0.0f, ఎగువ_Pass.getU()));
	ASSERT_CL(equal(0.0f, ఎగువ_Pass.getY()));
	ASSERT_CL(equal(0.0f, ఎగువ_Pass.getDt()));
	// set dt
	ఎగువ_Pass.సమితి_Dt(0.1f);
	ASSERT_CL(equal(0.1f, ఎగువ_Pass.getDt()));
	// set state
	ఎగువ_Pass.సమితి_U(1.0f);
	ASSERT_CL(equal(1.0f, ఎగువ_Pass.getU()));
	ఎగువ_Pass.సమితి_Y(1.0f);
	ASSERT_CL(equal(1.0f, ఎగువ_Pass.getY()));
	// test update
	ASSERT_CL(equal(0.2746051f, ఎగువ_Pass.update(2.0f)));

	// test end condition
	కొరకు (పూర్ణాంకం i = 0; i < 100; i++) {
		ఎగువ_Pass.update(2.0f);
	}

	ASSERT_CL(equal(0.0f, ఎగువ_Pass.getY()));
	ASSERT_CL(equal(0.0f, ఎగువ_Pass.update(2.0f)));
	రాయి("PASS\n");
	తిరిగి 0;
}

పూర్ణాంకం ఖండం_దిగువ_Pass2_Test()
{
	రాయి("Test BlockLowPass2\t\t: ");
	ఖండం_దిగువ_Pass2 దిగువ_Pass_2(శూన్యసూచిక, "TEST_LP", 100);
	// test initial state
	ASSERT_CL(equal(10.0f, దిగువ_Pass_2.getFCutParam()));
	ASSERT_CL(equal(0.0f, దిగువ_Pass_2.getState()));
	ASSERT_CL(equal(0.0f, దిగువ_Pass_2.getDt()));
	// set dt
	దిగువ_Pass_2.సమితి_Dt(0.1f);
	ASSERT_CL(equal(0.1f, దిగువ_Pass_2.getDt()));
	// set state
	దిగువ_Pass_2.సమితి_State(1.0f);
	ASSERT_CL(equal(1.0f, దిగువ_Pass_2.getState()));
	// test update
	ASSERT_CL(equal(1.06745527f, దిగువ_Pass_2.update(2.0f)));

	// test end condition
	కొరకు (పూర్ణాంకం i = 0; i < 100; i++) {
		దిగువ_Pass_2.update(2.0f);
	}

	ASSERT_CL(equal(2.0f, దిగువ_Pass_2.getState()));
	ASSERT_CL(equal(2.0f, దిగువ_Pass_2.update(2.0f)));
	రాయి("PASS\n");
	తిరిగి 0;
};

పూర్ణాంకం ఖండం_Integral_Test()
{
	రాయి("Test BlockIntegral\t\t: ");
	ఖండం_Integral integral(శూన్యసూచిక, "TEST_I");
	// test initial state
	ASSERT_CL(equal(1.0f, integral.getMax()));
	ASSERT_CL(equal(0.0f, integral.getDt()));
	// set dt
	integral.సమితి_Dt(0.1f);
	ASSERT_CL(equal(0.1f, integral.getDt()));
	// set Y
	integral.సమితి_Y(0.9f);
	ASSERT_CL(equal(0.9f, integral.getY()));

	// test exceed max
	కొరకు (పూర్ణాంకం i = 0; i < 100; i++) {
		integral.update(1.0f);
	}

	ASSERT_CL(equal(1.0f, integral.update(1.0f)));
	// test exceed min
	integral.సమితి_Y(-0.9f);
	ASSERT_CL(equal(-0.9f, integral.getY()));

	కొరకు (పూర్ణాంకం i = 0; i < 100; i++) {
		integral.update(-1.0f);
	}

	ASSERT_CL(equal(-1.0f, integral.update(-1.0f)));
	// test update
	integral.సమితి_Y(0.1f);
	ASSERT_CL(equal(0.2f, integral.update(1.0)));
	ASSERT_CL(equal(0.2f, integral.getY()));
	రాయి("PASS\n");
	తిరిగి 0;
}

పూర్ణాంకం ఖండం_Integral_Trap_Test()
{
	రాయి("Test BlockIntegralTrap\t\t: ");
	ఖండం_Integral_Trap integral(శూన్యసూచిక, "TEST_I");
	// test initial state
	ASSERT_CL(equal(1.0f, integral.getMax()));
	ASSERT_CL(equal(0.0f, integral.getDt()));
	// set dt
	integral.సమితి_Dt(0.1f);
	ASSERT_CL(equal(0.1f, integral.getDt()));
	// set U
	integral.సమితి_U(1.0f);
	ASSERT_CL(equal(1.0f, integral.getU()));
	// set Y
	integral.సమితి_Y(0.9f);
	ASSERT_CL(equal(0.9f, integral.getY()));

	// test exceed max
	కొరకు (పూర్ణాంకం i = 0; i < 100; i++) {
		integral.update(1.0f);
	}

	ASSERT_CL(equal(1.0f, integral.update(1.0f)));
	// test exceed min
	integral.సమితి_U(-1.0f);
	integral.సమితి_Y(-0.9f);
	ASSERT_CL(equal(-0.9f, integral.getY()));

	కొరకు (పూర్ణాంకం i = 0; i < 100; i++) {
		integral.update(-1.0f);
	}

	ASSERT_CL(equal(-1.0f, integral.update(-1.0f)));
	// test update
	integral.సమితి_U(2.0f);
	integral.సమితి_Y(0.1f);
	ASSERT_CL(equal(0.25f, integral.update(1.0)));
	ASSERT_CL(equal(0.25f, integral.getY()));
	ASSERT_CL(equal(1.0f, integral.getU()));
	రాయి("PASS\n");
	తిరిగి 0;
}

పూర్ణాంకం ఖండం_Derivative_Test()
{
	రాయి("Test BlockDerivative\t\t: ");
	ఖండం_Derivative derivative(శూన్యసూచిక, "TEST_D");
	// test initial state
	ASSERT_CL(equal(0.0f, derivative.getU()));
	ASSERT_CL(equal(10.0f, derivative.getLP()));
	// set dt
	derivative.సమితి_Dt(0.1f);
	ASSERT_CL(equal(0.1f, derivative.getDt()));
	// set U
	derivative.సమితి_U(1.0f);
	ASSERT_CL(equal(1.0f, derivative.getU()));
	// perform one update so initialized is set
	derivative.update(1.0);
	ASSERT_CL(equal(1.0f, derivative.getU()));
	// test  update
	ASSERT_CL(equal(8.6269744f, derivative.update(2.0f)));
	ASSERT_CL(equal(2.0f, derivative.getU()));
	రాయి("PASS\n");
	తిరిగి 0;
}

పూర్ణాంకం ఖండం_PTest()
{
	రాయి("Test BlockP\t\t\t: ");
	ఖండం_P ఖండం_P_2(శూన్యసూచిక, "TEST_P");
	// test initial state
	ASSERT_CL(equal(0.2f, ఖండం_P_2.getKP()));
	ASSERT_CL(equal(0.0f, ఖండం_P_2.getDt()));
	// set dt
	ఖండం_P_2.సమితి_Dt(0.1f);
	ASSERT_CL(equal(0.1f, ఖండం_P_2.getDt()));
	// test  update
	ASSERT_CL(equal(0.4f, ఖండం_P_2.update(2.0f)));
	రాయి("PASS\n");
	తిరిగి 0;
}

పూర్ణాంకం ఖండం_PITest()
{
	రాయి("Test BlockPI\t\t\t: ");
	ఖండం_PI ఖండం_PI_2(శూన్యసూచిక, "TEST");
	// test initial state
	ASSERT_CL(equal(0.2f, ఖండం_PI_2.getKP()));
	ASSERT_CL(equal(0.1f, ఖండం_PI_2.getKI()));
	ASSERT_CL(equal(0.0f, ఖండం_PI_2.getDt()));
	ASSERT_CL(equal(1.0f, ఖండం_PI_2.getIntegral().getMax()));
	// set dt
	ఖండం_PI_2.సమితి_Dt(0.1f);
	ASSERT_CL(equal(0.1f, ఖండం_PI_2.getDt()));
	// set integral state
	ఖండం_PI_2.getIntegral().సమితి_Y(0.1f);
	ASSERT_CL(equal(0.1f, ఖండం_PI_2.getIntegral().getY()));
	// test  update
	// 0.2*2 + 0.1*(2*0.1 + 0.1) = 0.43
	ASSERT_CL(equal(0.43f, ఖండం_PI_2.update(2.0f)));
	రాయి("PASS\n");
	తిరిగి 0;
}

పూర్ణాంకం ఖండం_PDTest()
{
	రాయి("Test BlockPD\t\t\t: ");
	ఖండం_PD ఖండం_PD_2(శూన్యసూచిక, "TEST");
	// test initial state
	ASSERT_CL(equal(0.2f, ఖండం_PD_2.getKP()));
	ASSERT_CL(equal(0.01f, ఖండం_PD_2.getKD()));
	ASSERT_CL(equal(0.0f, ఖండం_PD_2.getDt()));
	ASSERT_CL(equal(10.0f, ఖండం_PD_2.getDerivative().getLP()));
	// set dt
	ఖండం_PD_2.సమితి_Dt(0.1f);
	ASSERT_CL(equal(0.1f, ఖండం_PD_2.getDt()));
	// set derivative state
	ఖండం_PD_2.getDerivative().సమితి_U(1.0f);
	ASSERT_CL(equal(1.0f, ఖండం_PD_2.getDerivative().getU()));
	// perform one update so initialized is set
	ఖండం_PD_2.getDerivative().update(1.0);
	ASSERT_CL(equal(1.0f, ఖండం_PD_2.getDerivative().getU()));
	// test  update
	// 0.2*2 + 0.1*(0.1*8.626...) = 0.486269744
	ASSERT_CL(equal(0.486269744f, ఖండం_PD_2.update(2.0f)));
	రాయి("PASS\n");
	తిరిగి 0;
}

పూర్ణాంకం ఖండం_PIDTest()
{
	రాయి("Test BlockPID\t\t\t: ");
	ఖండం_PID ఖండం_PID_2(శూన్యసూచిక, "TEST");
	// test initial state
	ASSERT_CL(equal(0.2f, ఖండం_PID_2.getKP()));
	ASSERT_CL(equal(0.1f, ఖండం_PID_2.getKI()));
	ASSERT_CL(equal(0.01f, ఖండం_PID_2.getKD()));
	ASSERT_CL(equal(0.0f, ఖండం_PID_2.getDt()));
	ASSERT_CL(equal(10.0f, ఖండం_PID_2.getDerivative().getLP()));
	ASSERT_CL(equal(1.0f, ఖండం_PID_2.getIntegral().getMax()));
	// set dt
	ఖండం_PID_2.సమితి_Dt(0.1f);
	ASSERT_CL(equal(0.1f, ఖండం_PID_2.getDt()));
	// set derivative state
	ఖండం_PID_2.getDerivative().సమితి_U(1.0f);
	ASSERT_CL(equal(1.0f, ఖండం_PID_2.getDerivative().getU()));
	// perform one update so initialized is set
	ఖండం_PID_2.getDerivative().update(1.0);
	ASSERT_CL(equal(1.0f, ఖండం_PID_2.getDerivative().getU()));
	// set integral state
	ఖండం_PID_2.getIntegral().సమితి_Y(0.1f);
	ASSERT_CL(equal(0.1f, ఖండం_PID_2.getIntegral().getY()));
	// test  update
	// 0.2*2 + 0.1*(2*0.1 + 0.1) + 0.1*(0.1*8.626...) = 0.5162697
	ASSERT_CL(equal(0.5162697f, ఖండం_PID_2.update(2.0f)));
	రాయి("PASS\n");
	తిరిగి 0;
}

పూర్ణాంకం ఖండం_Output_Test()
{
	రాయి("Test BlockOutput\t\t: ");
	ఖండం_Output ఖండం_Output_2(శూన్యసూచిక, "TEST");
	// test initial state
	ASSERT_CL(equal(0.0f, ఖండం_Output_2.getDt()));
	ASSERT_CL(equal(0.5f, ఖండం_Output_2.get()));
	ASSERT_CL(equal(-1.0f, ఖండం_Output_2.getMin()));
	ASSERT_CL(equal(1.0f, ఖండం_Output_2.getMax()));
	// test update below min
	ఖండం_Output_2.update(-2.0f);
	ASSERT_CL(equal(-1.0f, ఖండం_Output_2.get()));
	// test update above max
	ఖండం_Output_2.update(2.0f);
	ASSERT_CL(equal(1.0f, ఖండం_Output_2.get()));
	// test trim
	ఖండం_Output_2.update(0.0f);
	ASSERT_CL(equal(0.5f, ఖండం_Output_2.get()));
	రాయి("PASS\n");
	తిరిగి 0;
}

పూర్ణాంకం ఖండం_Rand_ఏకరీతి_Test()
{
	srand(1234);
	రాయి("Test BlockRandUniform\t\t: ");
	ఖండం_Rand_ఏకరీతి ఖండం_Rand_ఏకరీతి_2(శూన్యసూచిక, "TEST");
	// test initial state
	ASSERT_CL(equal(0.0f, ఖండం_Rand_ఏకరీతి_2.getDt()));
	ASSERT_CL(equal(-1.0f, ఖండం_Rand_ఏకరీతి_2.getMin()));
	ASSERT_CL(equal(1.0f, ఖండం_Rand_ఏకరీతి_2.getMax()));
	// test update
	పూర్ణాంకం n = 10000;
	దశాంశం సగటు = ఖండం_Rand_ఏకరీతి_2.update();

	కొరకు (పూర్ణాంకం i = 2; i < n + 1; i++) {
		దశాంశం val = ఖండం_Rand_ఏకరీతి_2.update();
		సగటు += (val - సగటు) / i;
		ASSERT_CL(less_than_or_equal(val, ఖండం_Rand_ఏకరీతి_2.getMax()));
		ASSERT_CL(greater_than_or_equal(val, ఖండం_Rand_ఏకరీతి_2.getMin()));
	}

	ASSERT_CL(equal(సగటు, (ఖండం_Rand_ఏకరీతి_2.getMin() +
			       ఖండం_Rand_ఏకరీతి_2.getMax()) / 2, 1e-1));
	రాయి("PASS\n");
	తిరిగి 0;
}

పూర్ణాంకం ఖండం_Rand_Gauss_Test()
{
	srand(1234);
	రాయి("Test BlockRandGauss\t\t: ");
	ఖండం_Rand_Gauss ఖండం_Rand_Gauss_2(శూన్యసూచిక, "TEST");
	// test initial state
	ASSERT_CL(equal(0.0f, ఖండం_Rand_Gauss_2.getDt()));
	ASSERT_CL(equal(1.0f, ఖండం_Rand_Gauss_2.get_సగటు()));
	ASSERT_CL(equal(2.0f, ఖండం_Rand_Gauss_2.getStdDev()));
	// test update
	పూర్ణాంకం n = 10000;
	దశాంశం సగటు = ఖండం_Rand_Gauss_2.update();
	దశాంశం కూడిక = 0;

	// recursive mean, stdev algorithm from Knuth
	కొరకు (పూర్ణాంకం i = 2; i < n + 1; i++) {
		దశాంశం val = ఖండం_Rand_Gauss_2.update();
		దశాంశం new_సగటు = సగటు + (val - సగటు) / i;
		కూడిక += (val - సగటు) * (val - new_సగటు);
		సగటు = new_సగటు;
	}

	దశాంశం stdDev = sqrtf(కూడిక / (n - 1));
	(శూన్య)(stdDev);
	ASSERT_CL(equal(సగటు, ఖండం_Rand_Gauss_2.get_సగటు(), 1e-1));
	ASSERT_CL(equal(stdDev, ఖండం_Rand_Gauss_2.getStdDev(), 1e-1));
	రాయి("PASS\n");
	తిరిగి 0;
}

పూర్ణాంకం ఖండం_Stats_Test()
{
	రాయి("Test BlockStats\t\t\t: ");
	ఖండం_Stats<దశాంశం, 1> stats(శూన్యసూచిక, "TEST");
	ASSERT_CL(equal(0.0f, stats.get_సగటు()(0)));
	ASSERT_CL(equal(0.0f, stats.getStdDev()(0)));
	stats.update(మాతృక::Scalar<దశాంశం>(1.0f));
	stats.update(మాతృక::Scalar<దశాంశం>(2));
	ASSERT_CL(equal(1.5f, stats.get_సగటు()(0)));
	ASSERT_CL(equal(0.5f, stats.getStdDev()(0)));
	stats.reset();
	ASSERT_CL(equal(0.0f, stats.get_సగటు()(0)));
	ASSERT_CL(equal(0.0f, stats.getStdDev()(0)));
	రాయి("PASS\n");
	తిరిగి 0;
}

పూర్ణాంకం ఖండం_Delay_Test()
{
	రాయి("Test BlockDelay\t\t\t: ");
	ఉపయోగించి నామాకాశ మాతృక;
	ఖండం_Delay<దశాంశం, 2, 1, 3> delay(శూన్యసూచిక, "TEST");
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
	రాయి("PASS\n");
	తిరిగి 0;
}

బాహ్య "C" __EXPORT పూర్ణాంకం controllib_test_main(పూర్ణాంకం argc, అక్షరం *argv[]);

పూర్ణాంకం controllib_test_main(పూర్ణాంకం argc, అక్షరం *argv[])
{
	తిరిగి basic_ఖండాలు_Test();
}
