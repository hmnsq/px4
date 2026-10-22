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

#ਅਨ੍ਤਰ੍ਭਾਵ <math.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <float.h>

#ਅਨ੍ਤਰ੍ਭਾਵ <controllib/blocks.hpp>

ਵਰਤੋਂ ਨਾਮਾਕਾਸ਼ control;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ASSERT_CL(T) ਜੇ (!(T)) { ਲਿਖੋ("FAIL\n"); ਵਾਪਸ -1; }

ਪੂਰਨ basic_ਖੰਡਾਂ_Test();
ਪੂਰਨ ਖੰਡ_ਹੱਦਬੰਦੀ_Test();
ਪੂਰਨ ਖੰਡ_ਹੱਦਬੰਦੀ_Sym_Test();
ਪੂਰਨ ਖੰਡ_ਨੀਵਾਂ_Pass_Test();
ਪੂਰਨ ਖੰਡ_ਉੱਚਾ_Pass_Test();
ਪੂਰਨ ਖੰਡ_ਨੀਵਾਂ_Pass2_Test();
ਪੂਰਨ ਖੰਡ_Integral_Test();
ਪੂਰਨ ਖੰਡ_Integral_Trap_Test();
ਪੂਰਨ ਖੰਡ_Derivative_Test();
ਪੂਰਨ ਖੰਡ_PTest();
ਪੂਰਨ ਖੰਡ_PITest();
ਪੂਰਨ ਖੰਡ_PDTest();
ਪੂਰਨ ਖੰਡ_PIDTest();
ਪੂਰਨ ਖੰਡ_Output_Test();
ਪੂਰਨ ਖੰਡ_Rand_ਇਕਸਾਰ_Test();
ਪੂਰਨ ਖੰਡ_Rand_Gauss_Test();
ਪੂਰਨ ਖੰਡ_Stats_Test();
ਪੂਰਨ ਖੰਡ_Delay_Test();

ਪੂਰਨ basic_ਖੰਡਾਂ_Test()
{
	ਤਾਰਕਿਕ failed = ਝੂਠ;
	failed = failed || ਖੰਡ_ਹੱਦਬੰਦੀ_Test() < 0;
	failed = failed || ਖੰਡ_ਹੱਦਬੰਦੀ_Sym_Test() < 0;
	failed = failed || ਖੰਡ_ਨੀਵਾਂ_Pass_Test() < 0;
	failed = failed || ਖੰਡ_ਉੱਚਾ_Pass_Test() < 0;
	failed = failed || ਖੰਡ_ਨੀਵਾਂ_Pass2_Test() < 0;
	failed = failed || ਖੰਡ_Integral_Test() < 0;
	failed = failed || ਖੰਡ_Integral_Trap_Test() < 0;
	failed = failed || ਖੰਡ_Derivative_Test() < 0;
	failed = failed || ਖੰਡ_PTest() < 0;
	failed = failed || ਖੰਡ_PITest() < 0;
	failed = failed || ਖੰਡ_PDTest() < 0;
	failed = failed || ਖੰਡ_PIDTest() < 0;
	failed = failed || ਖੰਡ_Output_Test() < 0;
	failed = failed || ਖੰਡ_Rand_ਇਕਸਾਰ_Test() < 0;
	failed = failed || ਖੰਡ_Rand_Gauss_Test() < 0;
	failed = failed || ਖੰਡ_Stats_Test() < 0;
	failed = failed || ਖੰਡ_Delay_Test() < 0;
	ਵਾਪਸ failed ? -1 : 0;
}

ਪੂਰਨ ਖੰਡ_ਹੱਦਬੰਦੀ_Test()
{
	ਲਿਖੋ("Test BlockLimit\t\t\t: ");
	ਖੰਡ_ਹੱਦਬੰਦੀ ਹੱਦਬੰਦੀ_2(ਖਾਲੀ_ਸੂਚਕ, "TEST");
	// initial state
	ASSERT_CL(equal(1.0f, ਹੱਦਬੰਦੀ_2.getMax()));
	ASSERT_CL(equal(-1.0f, ਹੱਦਬੰਦੀ_2.getMin()));
	ASSERT_CL(equal(0.0f, ਹੱਦਬੰਦੀ_2.getDt()));
	// update
	ASSERT_CL(equal(-1.0f, ਹੱਦਬੰਦੀ_2.update(-2.0f)));
	ASSERT_CL(equal(1.0f, ਹੱਦਬੰਦੀ_2.update(2.0f)));
	ASSERT_CL(equal(0.0f, ਹੱਦਬੰਦੀ_2.update(0.0f)));
	ਲਿਖੋ("PASS\n");
	ਵਾਪਸ 0;
}

ਪੂਰਨ ਖੰਡ_ਹੱਦਬੰਦੀ_Sym_Test()
{
	ਲਿਖੋ("Test BlockLimitSym\t\t: ");
	ਖੰਡ_ਹੱਦਬੰਦੀ_Sym ਹੱਦਬੰਦੀ_2(ਖਾਲੀ_ਸੂਚਕ, "TEST");
	// initial state
	ASSERT_CL(equal(1.0f, ਹੱਦਬੰਦੀ_2.getMax()));
	ASSERT_CL(equal(0.0f, ਹੱਦਬੰਦੀ_2.getDt()));
	// update
	ASSERT_CL(equal(-1.0f, ਹੱਦਬੰਦੀ_2.update(-2.0f)));
	ASSERT_CL(equal(1.0f, ਹੱਦਬੰਦੀ_2.update(2.0f)));
	ASSERT_CL(equal(0.0f, ਹੱਦਬੰਦੀ_2.update(0.0f)));
	ਲਿਖੋ("PASS\n");
	ਵਾਪਸ 0;
}

ਪੂਰਨ ਖੰਡ_ਨੀਵਾਂ_Pass_Test()
{
	ਲਿਖੋ("Test BlockLowPass\t\t: ");
	ਖੰਡ_ਨੀਵਾਂ_Pass ਨੀਵਾਂ_Pass_2(ਖਾਲੀ_ਸੂਚਕ, "TEST_LP");
	// test initial state
	ASSERT_CL(equal(10.0f, ਨੀਵਾਂ_Pass_2.getFCut()));
	ASSERT_CL(equal(0.0f, ਨੀਵਾਂ_Pass_2.getState()));
	ASSERT_CL(equal(0.0f, ਨੀਵਾਂ_Pass_2.getDt()));
	// set dt
	ਨੀਵਾਂ_Pass_2.ਸੈੱਟ_Dt(0.1f);
	ASSERT_CL(equal(0.1f, ਨੀਵਾਂ_Pass_2.getDt()));
	// set state
	ਨੀਵਾਂ_Pass_2.ਸੈੱਟ_State(1.0f);
	ASSERT_CL(equal(1.0f, ਨੀਵਾਂ_Pass_2.getState()));
	// test update
	ASSERT_CL(equal(1.8626974f, ਨੀਵਾਂ_Pass_2.update(2.0f)));

	// test end condition
	ਲਈ (ਪੂਰਨ i = 0; i < 100; i++) {
		ਨੀਵਾਂ_Pass_2.update(2.0f);
	}

	ASSERT_CL(equal(2.0f, ਨੀਵਾਂ_Pass_2.getState()));
	ASSERT_CL(equal(2.0f, ਨੀਵਾਂ_Pass_2.update(2.0f)));
	ਲਿਖੋ("PASS\n");
	ਵਾਪਸ 0;
};

ਪੂਰਨ ਖੰਡ_ਉੱਚਾ_Pass_Test()
{
	ਲਿਖੋ("Test BlockHighPass\t\t: ");
	ਖੰਡ_ਉੱਚਾ_Pass ਉੱਚਾ_Pass(ਖਾਲੀ_ਸੂਚਕ, "TEST_HP");
	// test initial state
	ASSERT_CL(equal(10.0f, ਉੱਚਾ_Pass.getFCut()));
	ASSERT_CL(equal(0.0f, ਉੱਚਾ_Pass.getU()));
	ASSERT_CL(equal(0.0f, ਉੱਚਾ_Pass.getY()));
	ASSERT_CL(equal(0.0f, ਉੱਚਾ_Pass.getDt()));
	// set dt
	ਉੱਚਾ_Pass.ਸੈੱਟ_Dt(0.1f);
	ASSERT_CL(equal(0.1f, ਉੱਚਾ_Pass.getDt()));
	// set state
	ਉੱਚਾ_Pass.ਸੈੱਟ_U(1.0f);
	ASSERT_CL(equal(1.0f, ਉੱਚਾ_Pass.getU()));
	ਉੱਚਾ_Pass.ਸੈੱਟ_Y(1.0f);
	ASSERT_CL(equal(1.0f, ਉੱਚਾ_Pass.getY()));
	// test update
	ASSERT_CL(equal(0.2746051f, ਉੱਚਾ_Pass.update(2.0f)));

	// test end condition
	ਲਈ (ਪੂਰਨ i = 0; i < 100; i++) {
		ਉੱਚਾ_Pass.update(2.0f);
	}

	ASSERT_CL(equal(0.0f, ਉੱਚਾ_Pass.getY()));
	ASSERT_CL(equal(0.0f, ਉੱਚਾ_Pass.update(2.0f)));
	ਲਿਖੋ("PASS\n");
	ਵਾਪਸ 0;
}

ਪੂਰਨ ਖੰਡ_ਨੀਵਾਂ_Pass2_Test()
{
	ਲਿਖੋ("Test BlockLowPass2\t\t: ");
	ਖੰਡ_ਨੀਵਾਂ_Pass2 ਨੀਵਾਂ_Pass_2(ਖਾਲੀ_ਸੂਚਕ, "TEST_LP", 100);
	// test initial state
	ASSERT_CL(equal(10.0f, ਨੀਵਾਂ_Pass_2.getFCutParam()));
	ASSERT_CL(equal(0.0f, ਨੀਵਾਂ_Pass_2.getState()));
	ASSERT_CL(equal(0.0f, ਨੀਵਾਂ_Pass_2.getDt()));
	// set dt
	ਨੀਵਾਂ_Pass_2.ਸੈੱਟ_Dt(0.1f);
	ASSERT_CL(equal(0.1f, ਨੀਵਾਂ_Pass_2.getDt()));
	// set state
	ਨੀਵਾਂ_Pass_2.ਸੈੱਟ_State(1.0f);
	ASSERT_CL(equal(1.0f, ਨੀਵਾਂ_Pass_2.getState()));
	// test update
	ASSERT_CL(equal(1.06745527f, ਨੀਵਾਂ_Pass_2.update(2.0f)));

	// test end condition
	ਲਈ (ਪੂਰਨ i = 0; i < 100; i++) {
		ਨੀਵਾਂ_Pass_2.update(2.0f);
	}

	ASSERT_CL(equal(2.0f, ਨੀਵਾਂ_Pass_2.getState()));
	ASSERT_CL(equal(2.0f, ਨੀਵਾਂ_Pass_2.update(2.0f)));
	ਲਿਖੋ("PASS\n");
	ਵਾਪਸ 0;
};

ਪੂਰਨ ਖੰਡ_Integral_Test()
{
	ਲਿਖੋ("Test BlockIntegral\t\t: ");
	ਖੰਡ_Integral integral(ਖਾਲੀ_ਸੂਚਕ, "TEST_I");
	// test initial state
	ASSERT_CL(equal(1.0f, integral.getMax()));
	ASSERT_CL(equal(0.0f, integral.getDt()));
	// set dt
	integral.ਸੈੱਟ_Dt(0.1f);
	ASSERT_CL(equal(0.1f, integral.getDt()));
	// set Y
	integral.ਸੈੱਟ_Y(0.9f);
	ASSERT_CL(equal(0.9f, integral.getY()));

	// test exceed max
	ਲਈ (ਪੂਰਨ i = 0; i < 100; i++) {
		integral.update(1.0f);
	}

	ASSERT_CL(equal(1.0f, integral.update(1.0f)));
	// test exceed min
	integral.ਸੈੱਟ_Y(-0.9f);
	ASSERT_CL(equal(-0.9f, integral.getY()));

	ਲਈ (ਪੂਰਨ i = 0; i < 100; i++) {
		integral.update(-1.0f);
	}

	ASSERT_CL(equal(-1.0f, integral.update(-1.0f)));
	// test update
	integral.ਸੈੱਟ_Y(0.1f);
	ASSERT_CL(equal(0.2f, integral.update(1.0)));
	ASSERT_CL(equal(0.2f, integral.getY()));
	ਲਿਖੋ("PASS\n");
	ਵਾਪਸ 0;
}

ਪੂਰਨ ਖੰਡ_Integral_Trap_Test()
{
	ਲਿਖੋ("Test BlockIntegralTrap\t\t: ");
	ਖੰਡ_Integral_Trap integral(ਖਾਲੀ_ਸੂਚਕ, "TEST_I");
	// test initial state
	ASSERT_CL(equal(1.0f, integral.getMax()));
	ASSERT_CL(equal(0.0f, integral.getDt()));
	// set dt
	integral.ਸੈੱਟ_Dt(0.1f);
	ASSERT_CL(equal(0.1f, integral.getDt()));
	// set U
	integral.ਸੈੱਟ_U(1.0f);
	ASSERT_CL(equal(1.0f, integral.getU()));
	// set Y
	integral.ਸੈੱਟ_Y(0.9f);
	ASSERT_CL(equal(0.9f, integral.getY()));

	// test exceed max
	ਲਈ (ਪੂਰਨ i = 0; i < 100; i++) {
		integral.update(1.0f);
	}

	ASSERT_CL(equal(1.0f, integral.update(1.0f)));
	// test exceed min
	integral.ਸੈੱਟ_U(-1.0f);
	integral.ਸੈੱਟ_Y(-0.9f);
	ASSERT_CL(equal(-0.9f, integral.getY()));

	ਲਈ (ਪੂਰਨ i = 0; i < 100; i++) {
		integral.update(-1.0f);
	}

	ASSERT_CL(equal(-1.0f, integral.update(-1.0f)));
	// test update
	integral.ਸੈੱਟ_U(2.0f);
	integral.ਸੈੱਟ_Y(0.1f);
	ASSERT_CL(equal(0.25f, integral.update(1.0)));
	ASSERT_CL(equal(0.25f, integral.getY()));
	ASSERT_CL(equal(1.0f, integral.getU()));
	ਲਿਖੋ("PASS\n");
	ਵਾਪਸ 0;
}

ਪੂਰਨ ਖੰਡ_Derivative_Test()
{
	ਲਿਖੋ("Test BlockDerivative\t\t: ");
	ਖੰਡ_Derivative derivative(ਖਾਲੀ_ਸੂਚਕ, "TEST_D");
	// test initial state
	ASSERT_CL(equal(0.0f, derivative.getU()));
	ASSERT_CL(equal(10.0f, derivative.getLP()));
	// set dt
	derivative.ਸੈੱਟ_Dt(0.1f);
	ASSERT_CL(equal(0.1f, derivative.getDt()));
	// set U
	derivative.ਸੈੱਟ_U(1.0f);
	ASSERT_CL(equal(1.0f, derivative.getU()));
	// perform one update so initialized is set
	derivative.update(1.0);
	ASSERT_CL(equal(1.0f, derivative.getU()));
	// test  update
	ASSERT_CL(equal(8.6269744f, derivative.update(2.0f)));
	ASSERT_CL(equal(2.0f, derivative.getU()));
	ਲਿਖੋ("PASS\n");
	ਵਾਪਸ 0;
}

ਪੂਰਨ ਖੰਡ_PTest()
{
	ਲਿਖੋ("Test BlockP\t\t\t: ");
	ਖੰਡ_P ਖੰਡ_P_2(ਖਾਲੀ_ਸੂਚਕ, "TEST_P");
	// test initial state
	ASSERT_CL(equal(0.2f, ਖੰਡ_P_2.getKP()));
	ASSERT_CL(equal(0.0f, ਖੰਡ_P_2.getDt()));
	// set dt
	ਖੰਡ_P_2.ਸੈੱਟ_Dt(0.1f);
	ASSERT_CL(equal(0.1f, ਖੰਡ_P_2.getDt()));
	// test  update
	ASSERT_CL(equal(0.4f, ਖੰਡ_P_2.update(2.0f)));
	ਲਿਖੋ("PASS\n");
	ਵਾਪਸ 0;
}

ਪੂਰਨ ਖੰਡ_PITest()
{
	ਲਿਖੋ("Test BlockPI\t\t\t: ");
	ਖੰਡ_PI ਖੰਡ_PI_2(ਖਾਲੀ_ਸੂਚਕ, "TEST");
	// test initial state
	ASSERT_CL(equal(0.2f, ਖੰਡ_PI_2.getKP()));
	ASSERT_CL(equal(0.1f, ਖੰਡ_PI_2.getKI()));
	ASSERT_CL(equal(0.0f, ਖੰਡ_PI_2.getDt()));
	ASSERT_CL(equal(1.0f, ਖੰਡ_PI_2.getIntegral().getMax()));
	// set dt
	ਖੰਡ_PI_2.ਸੈੱਟ_Dt(0.1f);
	ASSERT_CL(equal(0.1f, ਖੰਡ_PI_2.getDt()));
	// set integral state
	ਖੰਡ_PI_2.getIntegral().ਸੈੱਟ_Y(0.1f);
	ASSERT_CL(equal(0.1f, ਖੰਡ_PI_2.getIntegral().getY()));
	// test  update
	// 0.2*2 + 0.1*(2*0.1 + 0.1) = 0.43
	ASSERT_CL(equal(0.43f, ਖੰਡ_PI_2.update(2.0f)));
	ਲਿਖੋ("PASS\n");
	ਵਾਪਸ 0;
}

ਪੂਰਨ ਖੰਡ_PDTest()
{
	ਲਿਖੋ("Test BlockPD\t\t\t: ");
	ਖੰਡ_PD ਖੰਡ_PD_2(ਖਾਲੀ_ਸੂਚਕ, "TEST");
	// test initial state
	ASSERT_CL(equal(0.2f, ਖੰਡ_PD_2.getKP()));
	ASSERT_CL(equal(0.01f, ਖੰਡ_PD_2.getKD()));
	ASSERT_CL(equal(0.0f, ਖੰਡ_PD_2.getDt()));
	ASSERT_CL(equal(10.0f, ਖੰਡ_PD_2.getDerivative().getLP()));
	// set dt
	ਖੰਡ_PD_2.ਸੈੱਟ_Dt(0.1f);
	ASSERT_CL(equal(0.1f, ਖੰਡ_PD_2.getDt()));
	// set derivative state
	ਖੰਡ_PD_2.getDerivative().ਸੈੱਟ_U(1.0f);
	ASSERT_CL(equal(1.0f, ਖੰਡ_PD_2.getDerivative().getU()));
	// perform one update so initialized is set
	ਖੰਡ_PD_2.getDerivative().update(1.0);
	ASSERT_CL(equal(1.0f, ਖੰਡ_PD_2.getDerivative().getU()));
	// test  update
	// 0.2*2 + 0.1*(0.1*8.626...) = 0.486269744
	ASSERT_CL(equal(0.486269744f, ਖੰਡ_PD_2.update(2.0f)));
	ਲਿਖੋ("PASS\n");
	ਵਾਪਸ 0;
}

ਪੂਰਨ ਖੰਡ_PIDTest()
{
	ਲਿਖੋ("Test BlockPID\t\t\t: ");
	ਖੰਡ_PID ਖੰਡ_PID_2(ਖਾਲੀ_ਸੂਚਕ, "TEST");
	// test initial state
	ASSERT_CL(equal(0.2f, ਖੰਡ_PID_2.getKP()));
	ASSERT_CL(equal(0.1f, ਖੰਡ_PID_2.getKI()));
	ASSERT_CL(equal(0.01f, ਖੰਡ_PID_2.getKD()));
	ASSERT_CL(equal(0.0f, ਖੰਡ_PID_2.getDt()));
	ASSERT_CL(equal(10.0f, ਖੰਡ_PID_2.getDerivative().getLP()));
	ASSERT_CL(equal(1.0f, ਖੰਡ_PID_2.getIntegral().getMax()));
	// set dt
	ਖੰਡ_PID_2.ਸੈੱਟ_Dt(0.1f);
	ASSERT_CL(equal(0.1f, ਖੰਡ_PID_2.getDt()));
	// set derivative state
	ਖੰਡ_PID_2.getDerivative().ਸੈੱਟ_U(1.0f);
	ASSERT_CL(equal(1.0f, ਖੰਡ_PID_2.getDerivative().getU()));
	// perform one update so initialized is set
	ਖੰਡ_PID_2.getDerivative().update(1.0);
	ASSERT_CL(equal(1.0f, ਖੰਡ_PID_2.getDerivative().getU()));
	// set integral state
	ਖੰਡ_PID_2.getIntegral().ਸੈੱਟ_Y(0.1f);
	ASSERT_CL(equal(0.1f, ਖੰਡ_PID_2.getIntegral().getY()));
	// test  update
	// 0.2*2 + 0.1*(2*0.1 + 0.1) + 0.1*(0.1*8.626...) = 0.5162697
	ASSERT_CL(equal(0.5162697f, ਖੰਡ_PID_2.update(2.0f)));
	ਲਿਖੋ("PASS\n");
	ਵਾਪਸ 0;
}

ਪੂਰਨ ਖੰਡ_Output_Test()
{
	ਲਿਖੋ("Test BlockOutput\t\t: ");
	ਖੰਡ_Output ਖੰਡ_Output_2(ਖਾਲੀ_ਸੂਚਕ, "TEST");
	// test initial state
	ASSERT_CL(equal(0.0f, ਖੰਡ_Output_2.getDt()));
	ASSERT_CL(equal(0.5f, ਖੰਡ_Output_2.get()));
	ASSERT_CL(equal(-1.0f, ਖੰਡ_Output_2.getMin()));
	ASSERT_CL(equal(1.0f, ਖੰਡ_Output_2.getMax()));
	// test update below min
	ਖੰਡ_Output_2.update(-2.0f);
	ASSERT_CL(equal(-1.0f, ਖੰਡ_Output_2.get()));
	// test update above max
	ਖੰਡ_Output_2.update(2.0f);
	ASSERT_CL(equal(1.0f, ਖੰਡ_Output_2.get()));
	// test trim
	ਖੰਡ_Output_2.update(0.0f);
	ASSERT_CL(equal(0.5f, ਖੰਡ_Output_2.get()));
	ਲਿਖੋ("PASS\n");
	ਵਾਪਸ 0;
}

ਪੂਰਨ ਖੰਡ_Rand_ਇਕਸਾਰ_Test()
{
	srand(1234);
	ਲਿਖੋ("Test BlockRandUniform\t\t: ");
	ਖੰਡ_Rand_ਇਕਸਾਰ ਖੰਡ_Rand_ਇਕਸਾਰ_2(ਖਾਲੀ_ਸੂਚਕ, "TEST");
	// test initial state
	ASSERT_CL(equal(0.0f, ਖੰਡ_Rand_ਇਕਸਾਰ_2.getDt()));
	ASSERT_CL(equal(-1.0f, ਖੰਡ_Rand_ਇਕਸਾਰ_2.getMin()));
	ASSERT_CL(equal(1.0f, ਖੰਡ_Rand_ਇਕਸਾਰ_2.getMax()));
	// test update
	ਪੂਰਨ n = 10000;
	ਦਸ਼ਮਲਵ ਔਸਤ = ਖੰਡ_Rand_ਇਕਸਾਰ_2.update();

	ਲਈ (ਪੂਰਨ i = 2; i < n + 1; i++) {
		ਦਸ਼ਮਲਵ val = ਖੰਡ_Rand_ਇਕਸਾਰ_2.update();
		ਔਸਤ += (val - ਔਸਤ) / i;
		ASSERT_CL(less_than_or_equal(val, ਖੰਡ_Rand_ਇਕਸਾਰ_2.getMax()));
		ASSERT_CL(greater_than_or_equal(val, ਖੰਡ_Rand_ਇਕਸਾਰ_2.getMin()));
	}

	ASSERT_CL(equal(ਔਸਤ, (ਖੰਡ_Rand_ਇਕਸਾਰ_2.getMin() +
			       ਖੰਡ_Rand_ਇਕਸਾਰ_2.getMax()) / 2, 1e-1));
	ਲਿਖੋ("PASS\n");
	ਵਾਪਸ 0;
}

ਪੂਰਨ ਖੰਡ_Rand_Gauss_Test()
{
	srand(1234);
	ਲਿਖੋ("Test BlockRandGauss\t\t: ");
	ਖੰਡ_Rand_Gauss ਖੰਡ_Rand_Gauss_2(ਖਾਲੀ_ਸੂਚਕ, "TEST");
	// test initial state
	ASSERT_CL(equal(0.0f, ਖੰਡ_Rand_Gauss_2.getDt()));
	ASSERT_CL(equal(1.0f, ਖੰਡ_Rand_Gauss_2.get_ਔਸਤ()));
	ASSERT_CL(equal(2.0f, ਖੰਡ_Rand_Gauss_2.getStdDev()));
	// test update
	ਪੂਰਨ n = 10000;
	ਦਸ਼ਮਲਵ ਔਸਤ = ਖੰਡ_Rand_Gauss_2.update();
	ਦਸ਼ਮਲਵ ਜੋੜ = 0;

	// recursive mean, stdev algorithm from Knuth
	ਲਈ (ਪੂਰਨ i = 2; i < n + 1; i++) {
		ਦਸ਼ਮਲਵ val = ਖੰਡ_Rand_Gauss_2.update();
		ਦਸ਼ਮਲਵ new_ਔਸਤ = ਔਸਤ + (val - ਔਸਤ) / i;
		ਜੋੜ += (val - ਔਸਤ) * (val - new_ਔਸਤ);
		ਔਸਤ = new_ਔਸਤ;
	}

	ਦਸ਼ਮਲਵ stdDev = sqrtf(ਜੋੜ / (n - 1));
	(ਖਾਲੀ)(stdDev);
	ASSERT_CL(equal(ਔਸਤ, ਖੰਡ_Rand_Gauss_2.get_ਔਸਤ(), 1e-1));
	ASSERT_CL(equal(stdDev, ਖੰਡ_Rand_Gauss_2.getStdDev(), 1e-1));
	ਲਿਖੋ("PASS\n");
	ਵਾਪਸ 0;
}

ਪੂਰਨ ਖੰਡ_Stats_Test()
{
	ਲਿਖੋ("Test BlockStats\t\t\t: ");
	ਖੰਡ_Stats<ਦਸ਼ਮਲਵ, 1> stats(ਖਾਲੀ_ਸੂਚਕ, "TEST");
	ASSERT_CL(equal(0.0f, stats.get_ਔਸਤ()(0)));
	ASSERT_CL(equal(0.0f, stats.getStdDev()(0)));
	stats.update(ਮੈਟ੍ਰਿਕਸ::Scalar<ਦਸ਼ਮਲਵ>(1.0f));
	stats.update(ਮੈਟ੍ਰਿਕਸ::Scalar<ਦਸ਼ਮਲਵ>(2));
	ASSERT_CL(equal(1.5f, stats.get_ਔਸਤ()(0)));
	ASSERT_CL(equal(0.5f, stats.getStdDev()(0)));
	stats.reset();
	ASSERT_CL(equal(0.0f, stats.get_ਔਸਤ()(0)));
	ASSERT_CL(equal(0.0f, stats.getStdDev()(0)));
	ਲਿਖੋ("PASS\n");
	ਵਾਪਸ 0;
}

ਪੂਰਨ ਖੰਡ_Delay_Test()
{
	ਲਿਖੋ("Test BlockDelay\t\t\t: ");
	ਵਰਤੋਂ ਨਾਮਾਕਾਸ਼ ਮੈਟ੍ਰਿਕਸ;
	ਖੰਡ_Delay<ਦਸ਼ਮਲਵ, 2, 1, 3> delay(ਖਾਲੀ_ਸੂਚਕ, "TEST");
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
	ਲਿਖੋ("PASS\n");
	ਵਾਪਸ 0;
}

ਬਾਹਰੀ "C" __EXPORT ਪੂਰਨ controllib_test_main(ਪੂਰਨ argc, ਅੱਖਰ *argv[]);

ਪੂਰਨ controllib_test_main(ਪੂਰਨ argc, ਅੱਖਰ *argv[])
{
	ਵਾਪਸ basic_ਖੰਡਾਂ_Test();
}
