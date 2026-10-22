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

#अन्तर्भाव <math.h>
#अन्तर्भाव <stdio.h>
#अन्तर्भाव <float.h>

#अन्तर्भाव <controllib/blocks.hpp>

using नामाकाश control;

#define ASSERT_CL(T) यदि (!(T)) { printf("FAIL\n"); प्रत्यागम -1; }

पूर्णाङ्क basicBlocksTest();
पूर्णाङ्क blockLimitTest();
पूर्णाङ्क blockLimitSymTest();
पूर्णाङ्क block_खालचा_Pass_Test();
पूर्णाङ्क block_वरचा_Pass_Test();
पूर्णाङ्क block_खालचा_Pass2_Test();
पूर्णाङ्क blockIntegralTest();
पूर्णाङ्क blockIntegralTrapTest();
पूर्णाङ्क blockDerivativeTest();
पूर्णाङ्क blockPTest();
पूर्णाङ्क blockPITest();
पूर्णाङ्क blockPDTest();
पूर्णाङ्क blockPIDTest();
पूर्णाङ्क blockOutputTest();
पूर्णाङ्क blockRandUniformTest();
पूर्णाङ्क blockRandGaussTest();
पूर्णाङ्क blockStatsTest();
पूर्णाङ्क blockDelayTest();

पूर्णाङ्क basicBlocksTest()
{
	तार्किक failed = असत्य;
	failed = failed || blockLimitTest() < 0;
	failed = failed || blockLimitSymTest() < 0;
	failed = failed || block_खालचा_Pass_Test() < 0;
	failed = failed || block_वरचा_Pass_Test() < 0;
	failed = failed || block_खालचा_Pass2_Test() < 0;
	failed = failed || blockIntegralTest() < 0;
	failed = failed || blockIntegralTrapTest() < 0;
	failed = failed || blockDerivativeTest() < 0;
	failed = failed || blockPTest() < 0;
	failed = failed || blockPITest() < 0;
	failed = failed || blockPDTest() < 0;
	failed = failed || blockPIDTest() < 0;
	failed = failed || blockOutputTest() < 0;
	failed = failed || blockRandUniformTest() < 0;
	failed = failed || blockRandGaussTest() < 0;
	failed = failed || blockStatsTest() < 0;
	failed = failed || blockDelayTest() < 0;
	प्रत्यागम failed ? -1 : 0;
}

पूर्णाङ्क blockLimitTest()
{
	printf("Test BlockLimit\t\t\t: ");
	BlockLimit limit(nullptr, "TEST");
	// initial state
	ASSERT_CL(equal(1.0f, limit.getMax()));
	ASSERT_CL(equal(-1.0f, limit.getMin()));
	ASSERT_CL(equal(0.0f, limit.getDt()));
	// update
	ASSERT_CL(equal(-1.0f, limit.update(-2.0f)));
	ASSERT_CL(equal(1.0f, limit.update(2.0f)));
	ASSERT_CL(equal(0.0f, limit.update(0.0f)));
	printf("PASS\n");
	प्रत्यागम 0;
}

पूर्णाङ्क blockLimitSymTest()
{
	printf("Test BlockLimitSym\t\t: ");
	BlockLimitSym limit(nullptr, "TEST");
	// initial state
	ASSERT_CL(equal(1.0f, limit.getMax()));
	ASSERT_CL(equal(0.0f, limit.getDt()));
	// update
	ASSERT_CL(equal(-1.0f, limit.update(-2.0f)));
	ASSERT_CL(equal(1.0f, limit.update(2.0f)));
	ASSERT_CL(equal(0.0f, limit.update(0.0f)));
	printf("PASS\n");
	प्रत्यागम 0;
}

पूर्णाङ्क block_खालचा_Pass_Test()
{
	printf("Test BlockLowPass\t\t: ");
	Block_खालचा_Pass खालचा_Pass_2(nullptr, "TEST_LP");
	// test initial state
	ASSERT_CL(equal(10.0f, खालचा_Pass_2.getFCut()));
	ASSERT_CL(equal(0.0f, खालचा_Pass_2.getState()));
	ASSERT_CL(equal(0.0f, खालचा_Pass_2.getDt()));
	// set dt
	खालचा_Pass_2.setDt(0.1f);
	ASSERT_CL(equal(0.1f, खालचा_Pass_2.getDt()));
	// set state
	खालचा_Pass_2.setState(1.0f);
	ASSERT_CL(equal(1.0f, खालचा_Pass_2.getState()));
	// test update
	ASSERT_CL(equal(1.8626974f, खालचा_Pass_2.update(2.0f)));

	// test end condition
	प्रति (पूर्णाङ्क i = 0; i < 100; i++) {
		खालचा_Pass_2.update(2.0f);
	}

	ASSERT_CL(equal(2.0f, खालचा_Pass_2.getState()));
	ASSERT_CL(equal(2.0f, खालचा_Pass_2.update(2.0f)));
	printf("PASS\n");
	प्रत्यागम 0;
};

पूर्णाङ्क block_वरचा_Pass_Test()
{
	printf("Test BlockHighPass\t\t: ");
	Block_वरचा_Pass वरचा_Pass(nullptr, "TEST_HP");
	// test initial state
	ASSERT_CL(equal(10.0f, वरचा_Pass.getFCut()));
	ASSERT_CL(equal(0.0f, वरचा_Pass.getU()));
	ASSERT_CL(equal(0.0f, वरचा_Pass.getY()));
	ASSERT_CL(equal(0.0f, वरचा_Pass.getDt()));
	// set dt
	वरचा_Pass.setDt(0.1f);
	ASSERT_CL(equal(0.1f, वरचा_Pass.getDt()));
	// set state
	वरचा_Pass.setU(1.0f);
	ASSERT_CL(equal(1.0f, वरचा_Pass.getU()));
	वरचा_Pass.setY(1.0f);
	ASSERT_CL(equal(1.0f, वरचा_Pass.getY()));
	// test update
	ASSERT_CL(equal(0.2746051f, वरचा_Pass.update(2.0f)));

	// test end condition
	प्रति (पूर्णाङ्क i = 0; i < 100; i++) {
		वरचा_Pass.update(2.0f);
	}

	ASSERT_CL(equal(0.0f, वरचा_Pass.getY()));
	ASSERT_CL(equal(0.0f, वरचा_Pass.update(2.0f)));
	printf("PASS\n");
	प्रत्यागम 0;
}

पूर्णाङ्क block_खालचा_Pass2_Test()
{
	printf("Test BlockLowPass2\t\t: ");
	Block_खालचा_Pass2 खालचा_Pass_2(nullptr, "TEST_LP", 100);
	// test initial state
	ASSERT_CL(equal(10.0f, खालचा_Pass_2.getFCutParam()));
	ASSERT_CL(equal(0.0f, खालचा_Pass_2.getState()));
	ASSERT_CL(equal(0.0f, खालचा_Pass_2.getDt()));
	// set dt
	खालचा_Pass_2.setDt(0.1f);
	ASSERT_CL(equal(0.1f, खालचा_Pass_2.getDt()));
	// set state
	खालचा_Pass_2.setState(1.0f);
	ASSERT_CL(equal(1.0f, खालचा_Pass_2.getState()));
	// test update
	ASSERT_CL(equal(1.06745527f, खालचा_Pass_2.update(2.0f)));

	// test end condition
	प्रति (पूर्णाङ्क i = 0; i < 100; i++) {
		खालचा_Pass_2.update(2.0f);
	}

	ASSERT_CL(equal(2.0f, खालचा_Pass_2.getState()));
	ASSERT_CL(equal(2.0f, खालचा_Pass_2.update(2.0f)));
	printf("PASS\n");
	प्रत्यागम 0;
};

पूर्णाङ्क blockIntegralTest()
{
	printf("Test BlockIntegral\t\t: ");
	BlockIntegral integral(nullptr, "TEST_I");
	// test initial state
	ASSERT_CL(equal(1.0f, integral.getMax()));
	ASSERT_CL(equal(0.0f, integral.getDt()));
	// set dt
	integral.setDt(0.1f);
	ASSERT_CL(equal(0.1f, integral.getDt()));
	// set Y
	integral.setY(0.9f);
	ASSERT_CL(equal(0.9f, integral.getY()));

	// test exceed max
	प्रति (पूर्णाङ्क i = 0; i < 100; i++) {
		integral.update(1.0f);
	}

	ASSERT_CL(equal(1.0f, integral.update(1.0f)));
	// test exceed min
	integral.setY(-0.9f);
	ASSERT_CL(equal(-0.9f, integral.getY()));

	प्रति (पूर्णाङ्क i = 0; i < 100; i++) {
		integral.update(-1.0f);
	}

	ASSERT_CL(equal(-1.0f, integral.update(-1.0f)));
	// test update
	integral.setY(0.1f);
	ASSERT_CL(equal(0.2f, integral.update(1.0)));
	ASSERT_CL(equal(0.2f, integral.getY()));
	printf("PASS\n");
	प्रत्यागम 0;
}

पूर्णाङ्क blockIntegralTrapTest()
{
	printf("Test BlockIntegralTrap\t\t: ");
	BlockIntegralTrap integral(nullptr, "TEST_I");
	// test initial state
	ASSERT_CL(equal(1.0f, integral.getMax()));
	ASSERT_CL(equal(0.0f, integral.getDt()));
	// set dt
	integral.setDt(0.1f);
	ASSERT_CL(equal(0.1f, integral.getDt()));
	// set U
	integral.setU(1.0f);
	ASSERT_CL(equal(1.0f, integral.getU()));
	// set Y
	integral.setY(0.9f);
	ASSERT_CL(equal(0.9f, integral.getY()));

	// test exceed max
	प्रति (पूर्णाङ्क i = 0; i < 100; i++) {
		integral.update(1.0f);
	}

	ASSERT_CL(equal(1.0f, integral.update(1.0f)));
	// test exceed min
	integral.setU(-1.0f);
	integral.setY(-0.9f);
	ASSERT_CL(equal(-0.9f, integral.getY()));

	प्रति (पूर्णाङ्क i = 0; i < 100; i++) {
		integral.update(-1.0f);
	}

	ASSERT_CL(equal(-1.0f, integral.update(-1.0f)));
	// test update
	integral.setU(2.0f);
	integral.setY(0.1f);
	ASSERT_CL(equal(0.25f, integral.update(1.0)));
	ASSERT_CL(equal(0.25f, integral.getY()));
	ASSERT_CL(equal(1.0f, integral.getU()));
	printf("PASS\n");
	प्रत्यागम 0;
}

पूर्णाङ्क blockDerivativeTest()
{
	printf("Test BlockDerivative\t\t: ");
	BlockDerivative derivative(nullptr, "TEST_D");
	// test initial state
	ASSERT_CL(equal(0.0f, derivative.getU()));
	ASSERT_CL(equal(10.0f, derivative.getLP()));
	// set dt
	derivative.setDt(0.1f);
	ASSERT_CL(equal(0.1f, derivative.getDt()));
	// set U
	derivative.setU(1.0f);
	ASSERT_CL(equal(1.0f, derivative.getU()));
	// perform one update so initialized is set
	derivative.update(1.0);
	ASSERT_CL(equal(1.0f, derivative.getU()));
	// test  update
	ASSERT_CL(equal(8.6269744f, derivative.update(2.0f)));
	ASSERT_CL(equal(2.0f, derivative.getU()));
	printf("PASS\n");
	प्रत्यागम 0;
}

पूर्णाङ्क blockPTest()
{
	printf("Test BlockP\t\t\t: ");
	BlockP blockP(nullptr, "TEST_P");
	// test initial state
	ASSERT_CL(equal(0.2f, blockP.getKP()));
	ASSERT_CL(equal(0.0f, blockP.getDt()));
	// set dt
	blockP.setDt(0.1f);
	ASSERT_CL(equal(0.1f, blockP.getDt()));
	// test  update
	ASSERT_CL(equal(0.4f, blockP.update(2.0f)));
	printf("PASS\n");
	प्रत्यागम 0;
}

पूर्णाङ्क blockPITest()
{
	printf("Test BlockPI\t\t\t: ");
	BlockPI blockPI(nullptr, "TEST");
	// test initial state
	ASSERT_CL(equal(0.2f, blockPI.getKP()));
	ASSERT_CL(equal(0.1f, blockPI.getKI()));
	ASSERT_CL(equal(0.0f, blockPI.getDt()));
	ASSERT_CL(equal(1.0f, blockPI.getIntegral().getMax()));
	// set dt
	blockPI.setDt(0.1f);
	ASSERT_CL(equal(0.1f, blockPI.getDt()));
	// set integral state
	blockPI.getIntegral().setY(0.1f);
	ASSERT_CL(equal(0.1f, blockPI.getIntegral().getY()));
	// test  update
	// 0.2*2 + 0.1*(2*0.1 + 0.1) = 0.43
	ASSERT_CL(equal(0.43f, blockPI.update(2.0f)));
	printf("PASS\n");
	प्रत्यागम 0;
}

पूर्णाङ्क blockPDTest()
{
	printf("Test BlockPD\t\t\t: ");
	BlockPD blockPD(nullptr, "TEST");
	// test initial state
	ASSERT_CL(equal(0.2f, blockPD.getKP()));
	ASSERT_CL(equal(0.01f, blockPD.getKD()));
	ASSERT_CL(equal(0.0f, blockPD.getDt()));
	ASSERT_CL(equal(10.0f, blockPD.getDerivative().getLP()));
	// set dt
	blockPD.setDt(0.1f);
	ASSERT_CL(equal(0.1f, blockPD.getDt()));
	// set derivative state
	blockPD.getDerivative().setU(1.0f);
	ASSERT_CL(equal(1.0f, blockPD.getDerivative().getU()));
	// perform one update so initialized is set
	blockPD.getDerivative().update(1.0);
	ASSERT_CL(equal(1.0f, blockPD.getDerivative().getU()));
	// test  update
	// 0.2*2 + 0.1*(0.1*8.626...) = 0.486269744
	ASSERT_CL(equal(0.486269744f, blockPD.update(2.0f)));
	printf("PASS\n");
	प्रत्यागम 0;
}

पूर्णाङ्क blockPIDTest()
{
	printf("Test BlockPID\t\t\t: ");
	BlockPID blockPID(nullptr, "TEST");
	// test initial state
	ASSERT_CL(equal(0.2f, blockPID.getKP()));
	ASSERT_CL(equal(0.1f, blockPID.getKI()));
	ASSERT_CL(equal(0.01f, blockPID.getKD()));
	ASSERT_CL(equal(0.0f, blockPID.getDt()));
	ASSERT_CL(equal(10.0f, blockPID.getDerivative().getLP()));
	ASSERT_CL(equal(1.0f, blockPID.getIntegral().getMax()));
	// set dt
	blockPID.setDt(0.1f);
	ASSERT_CL(equal(0.1f, blockPID.getDt()));
	// set derivative state
	blockPID.getDerivative().setU(1.0f);
	ASSERT_CL(equal(1.0f, blockPID.getDerivative().getU()));
	// perform one update so initialized is set
	blockPID.getDerivative().update(1.0);
	ASSERT_CL(equal(1.0f, blockPID.getDerivative().getU()));
	// set integral state
	blockPID.getIntegral().setY(0.1f);
	ASSERT_CL(equal(0.1f, blockPID.getIntegral().getY()));
	// test  update
	// 0.2*2 + 0.1*(2*0.1 + 0.1) + 0.1*(0.1*8.626...) = 0.5162697
	ASSERT_CL(equal(0.5162697f, blockPID.update(2.0f)));
	printf("PASS\n");
	प्रत्यागम 0;
}

पूर्णाङ्क blockOutputTest()
{
	printf("Test BlockOutput\t\t: ");
	BlockOutput blockOutput(nullptr, "TEST");
	// test initial state
	ASSERT_CL(equal(0.0f, blockOutput.getDt()));
	ASSERT_CL(equal(0.5f, blockOutput.get()));
	ASSERT_CL(equal(-1.0f, blockOutput.getMin()));
	ASSERT_CL(equal(1.0f, blockOutput.getMax()));
	// test update below min
	blockOutput.update(-2.0f);
	ASSERT_CL(equal(-1.0f, blockOutput.get()));
	// test update above max
	blockOutput.update(2.0f);
	ASSERT_CL(equal(1.0f, blockOutput.get()));
	// test trim
	blockOutput.update(0.0f);
	ASSERT_CL(equal(0.5f, blockOutput.get()));
	printf("PASS\n");
	प्रत्यागम 0;
}

पूर्णाङ्क blockRandUniformTest()
{
	srand(1234);
	printf("Test BlockRandUniform\t\t: ");
	BlockRandUniform blockRandUniform(nullptr, "TEST");
	// test initial state
	ASSERT_CL(equal(0.0f, blockRandUniform.getDt()));
	ASSERT_CL(equal(-1.0f, blockRandUniform.getMin()));
	ASSERT_CL(equal(1.0f, blockRandUniform.getMax()));
	// test update
	पूर्णाङ्क n = 10000;
	भिन्न mean = blockRandUniform.update();

	प्रति (पूर्णाङ्क i = 2; i < n + 1; i++) {
		भिन्न val = blockRandUniform.update();
		mean += (val - mean) / i;
		ASSERT_CL(less_than_or_equal(val, blockRandUniform.getMax()));
		ASSERT_CL(greater_than_or_equal(val, blockRandUniform.getMin()));
	}

	ASSERT_CL(equal(mean, (blockRandUniform.getMin() +
			       blockRandUniform.getMax()) / 2, 1e-1));
	printf("PASS\n");
	प्रत्यागम 0;
}

पूर्णाङ्क blockRandGaussTest()
{
	srand(1234);
	printf("Test BlockRandGauss\t\t: ");
	BlockRandGauss blockRandGauss(nullptr, "TEST");
	// test initial state
	ASSERT_CL(equal(0.0f, blockRandGauss.getDt()));
	ASSERT_CL(equal(1.0f, blockRandGauss.getMean()));
	ASSERT_CL(equal(2.0f, blockRandGauss.getStdDev()));
	// test update
	पूर्णाङ्क n = 10000;
	भिन्न mean = blockRandGauss.update();
	भिन्न sum = 0;

	// recursive mean, stdev algorithm from Knuth
	प्रति (पूर्णाङ्क i = 2; i < n + 1; i++) {
		भिन्न val = blockRandGauss.update();
		भिन्न newMean = mean + (val - mean) / i;
		sum += (val - mean) * (val - newMean);
		mean = newMean;
	}

	भिन्न stdDev = sqrtf(sum / (n - 1));
	(शून्य)(stdDev);
	ASSERT_CL(equal(mean, blockRandGauss.getMean(), 1e-1));
	ASSERT_CL(equal(stdDev, blockRandGauss.getStdDev(), 1e-1));
	printf("PASS\n");
	प्रत्यागम 0;
}

पूर्णाङ्क blockStatsTest()
{
	printf("Test BlockStats\t\t\t: ");
	BlockStats<भिन्न, 1> stats(nullptr, "TEST");
	ASSERT_CL(equal(0.0f, stats.getMean()(0)));
	ASSERT_CL(equal(0.0f, stats.getStdDev()(0)));
	stats.update(matrix::Scalar<भिन्न>(1.0f));
	stats.update(matrix::Scalar<भिन्न>(2));
	ASSERT_CL(equal(1.5f, stats.getMean()(0)));
	ASSERT_CL(equal(0.5f, stats.getStdDev()(0)));
	stats.reset();
	ASSERT_CL(equal(0.0f, stats.getMean()(0)));
	ASSERT_CL(equal(0.0f, stats.getStdDev()(0)));
	printf("PASS\n");
	प्रत्यागम 0;
}

पूर्णाङ्क blockDelayTest()
{
	printf("Test BlockDelay\t\t\t: ");
	using नामाकाश matrix;
	BlockDelay<भिन्न, 2, 1, 3> delay(nullptr, "TEST");
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
	printf("PASS\n");
	प्रत्यागम 0;
}

बाह्य "C" __EXPORT पूर्णाङ्क controllib_test_main(पूर्णाङ्क argc, वर्ण *argv[]);

पूर्णाङ्क controllib_test_main(पूर्णाङ्क argc, वर्ण *argv[])
{
	प्रत्यागम basicBlocksTest();
}
