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

#অন্তর্ভাৱ <math.h>
#অন্তর্ভাৱ <stdio.h>
#অন্তর্ভাৱ <float.h>

#অন্তর্ভাৱ <controllib/blocks.hpp>

#define ASSERT_CL(T) যদি (!(T)) { printf("FAIL\n"); ঘূৰাই -1; }

নামাকাশ control
{

পূর্ণাঙ্ক basicBlocksTest();
পূর্ণাঙ্ক blockLimitTest();
পূর্ণাঙ্ক blockLimitSymTest();
পূর্ণাঙ্ক block_নিচেৰ_Pass_Test();
পূর্ণাঙ্ক block_উপৰেৰ_Pass_Test();
পূর্ণাঙ্ক block_নিচেৰ_Pass2_Test();
পূর্ণাঙ্ক blockIntegralTest();
পূর্ণাঙ্ক blockIntegralTrapTest();
পূর্ণাঙ্ক blockDerivativeTest();
পূর্ণাঙ্ক blockPTest();
পূর্ণাঙ্ক blockPITest();
পূর্ণাঙ্ক blockPDTest();
পূর্ণাঙ্ক blockPIDTest();
পূর্ণাঙ্ক blockOutputTest();
পূর্ণাঙ্ক blockRandUniformTest();
পূর্ণাঙ্ক blockRandGaussTest();
পূর্ণাঙ্ক blockStatsTest();
পূর্ণাঙ্ক blockDelayTest();

পূর্ণাঙ্ক basicBlocksTest()
{
	blockLimitTest();
	blockLimitSymTest();
	block_নিচেৰ_Pass_Test();
	block_উপৰেৰ_Pass_Test();
	block_নিচেৰ_Pass2_Test();
	blockIntegralTest();
	blockIntegralTrapTest();
	blockDerivativeTest();
	blockPTest();
	blockPITest();
	blockPDTest();
	blockPIDTest();
	blockOutputTest();
	//blockRandUniformTest();
	// known failures
	// blockRandGaussTest();
	blockStatsTest();
	blockDelayTest();
	ঘূৰাই 0;
}

পূর্ণাঙ্ক blockLimitTest()
{
	printf("Test BlockLimit\t\t\t: ");
	BlockLimit limit(রিক্ত, "TEST");
	// initial state
	ASSERT_CL(equal(1.0f, limit.getMax()));
	ASSERT_CL(equal(-1.0f, limit.getMin()));
	ASSERT_CL(equal(0.0f, limit.getDt()));
	// update
	ASSERT_CL(equal(-1.0f, limit.update(-2.0f)));
	ASSERT_CL(equal(1.0f, limit.update(2.0f)));
	ASSERT_CL(equal(0.0f, limit.update(0.0f)));
	printf("PASS\n");
	ঘূৰাই 0;
}

পূর্ণাঙ্ক blockLimitSymTest()
{
	printf("Test BlockLimitSym\t\t: ");
	BlockLimitSym limit(রিক্ত, "TEST");
	// initial state
	ASSERT_CL(equal(1.0f, limit.getMax()));
	ASSERT_CL(equal(0.0f, limit.getDt()));
	// update
	ASSERT_CL(equal(-1.0f, limit.update(-2.0f)));
	ASSERT_CL(equal(1.0f, limit.update(2.0f)));
	ASSERT_CL(equal(0.0f, limit.update(0.0f)));
	printf("PASS\n");
	ঘূৰাই 0;
}

পূর্ণাঙ্ক block_নিচেৰ_Pass_Test()
{
	printf("Test BlockLowPass\t\t: ");
	Block_নিচেৰ_Pass নিচেৰ_Pass_2(রিক্ত, "TEST_LP");
	// test initial state
	ASSERT_CL(equal(10.0f, নিচেৰ_Pass_2.getFCut()));
	ASSERT_CL(equal(0.0f, নিচেৰ_Pass_2.getState()));
	ASSERT_CL(equal(0.0f, নিচেৰ_Pass_2.getDt()));
	// set dt
	নিচেৰ_Pass_2.setDt(0.1f);
	ASSERT_CL(equal(0.1f, নিচেৰ_Pass_2.getDt()));
	// set state
	নিচেৰ_Pass_2.setState(1.0f);
	ASSERT_CL(equal(1.0f, নিচেৰ_Pass_2.getState()));
	// test update
	ASSERT_CL(equal(1.8626974f, নিচেৰ_Pass_2.update(2.0f)));

	// test end condition
	বাবে (পূর্ণাঙ্ক i = 0; i < 100; i++) {
		নিচেৰ_Pass_2.update(2.0f);
	}

	ASSERT_CL(equal(2.0f, নিচেৰ_Pass_2.getState()));
	ASSERT_CL(equal(2.0f, নিচেৰ_Pass_2.update(2.0f)));
	printf("PASS\n");
	ঘূৰাই 0;
};

পূর্ণাঙ্ক block_উপৰেৰ_Pass_Test()
{
	printf("Test BlockHighPass\t\t: ");
	Block_উপৰেৰ_Pass উপৰেৰ_Pass(রিক্ত, "TEST_HP");
	// test initial state
	ASSERT_CL(equal(10.0f, উপৰেৰ_Pass.getFCut()));
	ASSERT_CL(equal(0.0f, উপৰেৰ_Pass.getU()));
	ASSERT_CL(equal(0.0f, উপৰেৰ_Pass.getY()));
	ASSERT_CL(equal(0.0f, উপৰেৰ_Pass.getDt()));
	// set dt
	উপৰেৰ_Pass.setDt(0.1f);
	ASSERT_CL(equal(0.1f, উপৰেৰ_Pass.getDt()));
	// set state
	উপৰেৰ_Pass.setU(1.0f);
	ASSERT_CL(equal(1.0f, উপৰেৰ_Pass.getU()));
	উপৰেৰ_Pass.setY(1.0f);
	ASSERT_CL(equal(1.0f, উপৰেৰ_Pass.getY()));
	// test update
	ASSERT_CL(equal(0.2746051f, উপৰেৰ_Pass.update(2.0f)));

	// test end condition
	বাবে (পূর্ণাঙ্ক i = 0; i < 100; i++) {
		উপৰেৰ_Pass.update(2.0f);
	}

	ASSERT_CL(equal(0.0f, উপৰেৰ_Pass.getY()));
	ASSERT_CL(equal(0.0f, উপৰেৰ_Pass.update(2.0f)));
	printf("PASS\n");
	ঘূৰাই 0;
}

পূর্ণাঙ্ক block_নিচেৰ_Pass2_Test()
{
	printf("Test BlockLowPass2\t\t: ");
	Block_নিচেৰ_Pass2 নিচেৰ_Pass_2(রিক্ত, "TEST_LP", 100);
	// test initial state
	ASSERT_CL(equal(10.0f, নিচেৰ_Pass_2.getFCutParam()));
	ASSERT_CL(equal(0.0f, নিচেৰ_Pass_2.getState()));
	ASSERT_CL(equal(0.0f, নিচেৰ_Pass_2.getDt()));
	// set dt
	নিচেৰ_Pass_2.setDt(0.1f);
	ASSERT_CL(equal(0.1f, নিচেৰ_Pass_2.getDt()));
	// set state
	নিচেৰ_Pass_2.setState(1.0f);
	ASSERT_CL(equal(1.0f, নিচেৰ_Pass_2.getState()));
	// test update
	ASSERT_CL(equal(1.06745527f, নিচেৰ_Pass_2.update(2.0f)));

	// test end condition
	বাবে (পূর্ণাঙ্ক i = 0; i < 100; i++) {
		নিচেৰ_Pass_2.update(2.0f);
	}

	ASSERT_CL(equal(2.0f, নিচেৰ_Pass_2.getState()));
	ASSERT_CL(equal(2.0f, নিচেৰ_Pass_2.update(2.0f)));
	printf("PASS\n");
	ঘূৰাই 0;
};

পূর্ণাঙ্ক blockIntegralTest()
{
	printf("Test BlockIntegral\t\t: ");
	BlockIntegral integral(রিক্ত, "TEST_I");
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
	বাবে (পূর্ণাঙ্ক i = 0; i < 100; i++) {
		integral.update(1.0f);
	}

	ASSERT_CL(equal(1.0f, integral.update(1.0f)));
	// test exceed min
	integral.setY(-0.9f);
	ASSERT_CL(equal(-0.9f, integral.getY()));

	বাবে (পূর্ণাঙ্ক i = 0; i < 100; i++) {
		integral.update(-1.0f);
	}

	ASSERT_CL(equal(-1.0f, integral.update(-1.0f)));
	// test update
	integral.setY(0.1f);
	ASSERT_CL(equal(0.2f, integral.update(1.0)));
	ASSERT_CL(equal(0.2f, integral.getY()));
	printf("PASS\n");
	ঘূৰাই 0;
}

পূর্ণাঙ্ক blockIntegralTrapTest()
{
	printf("Test BlockIntegralTrap\t\t: ");
	BlockIntegralTrap integral(রিক্ত, "TEST_I");
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
	বাবে (পূর্ণাঙ্ক i = 0; i < 100; i++) {
		integral.update(1.0f);
	}

	ASSERT_CL(equal(1.0f, integral.update(1.0f)));
	// test exceed min
	integral.setU(-1.0f);
	integral.setY(-0.9f);
	ASSERT_CL(equal(-0.9f, integral.getY()));

	বাবে (পূর্ণাঙ্ক i = 0; i < 100; i++) {
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
	ঘূৰাই 0;
}

পূর্ণাঙ্ক blockDerivativeTest()
{
	printf("Test BlockDerivative\t\t: ");
	BlockDerivative derivative(রিক্ত, "TEST_D");
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
	ঘূৰাই 0;
}

পূর্ণাঙ্ক blockPTest()
{
	printf("Test BlockP\t\t\t: ");
	BlockP blockP(রিক্ত, "TEST_P");
	// test initial state
	ASSERT_CL(equal(0.2f, blockP.getKP()));
	ASSERT_CL(equal(0.0f, blockP.getDt()));
	// set dt
	blockP.setDt(0.1f);
	ASSERT_CL(equal(0.1f, blockP.getDt()));
	// test  update
	ASSERT_CL(equal(0.4f, blockP.update(2.0f)));
	printf("PASS\n");
	ঘূৰাই 0;
}

পূর্ণাঙ্ক blockPITest()
{
	printf("Test BlockPI\t\t\t: ");
	BlockPI blockPI(রিক্ত, "TEST");
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
	ঘূৰাই 0;
}

পূর্ণাঙ্ক blockPDTest()
{
	printf("Test BlockPD\t\t\t: ");
	BlockPD blockPD(রিক্ত, "TEST");
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
	ঘূৰাই 0;
}

পূর্ণাঙ্ক blockPIDTest()
{
	printf("Test BlockPID\t\t\t: ");
	BlockPID blockPID(রিক্ত, "TEST");
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
	ঘূৰাই 0;
}

পূর্ণাঙ্ক blockOutputTest()
{
	printf("Test BlockOutput\t\t: ");
	BlockOutput blockOutput(রিক্ত, "TEST");
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
	ঘূৰাই 0;
}

পূর্ণাঙ্ক blockRandUniformTest()
{
	srand(1234);
	printf("Test BlockRandUniform\t\t: ");
	BlockRandUniform blockRandUniform(রিক্ত, "TEST");
	// test initial state
	ASSERT_CL(equal(0.0f, blockRandUniform.getDt()));
	ASSERT_CL(equal(-1.0f, blockRandUniform.getMin()));
	ASSERT_CL(equal(1.0f, blockRandUniform.getMax()));
	// test update
	পূর্ণাঙ্ক n = 10000;
	ভিন্ন mean = blockRandUniform.update();

	বাবে (পূর্ণাঙ্ক i = 2; i < n + 1; i++) {
		ভিন্ন val = blockRandUniform.update();
		mean += (val - mean) / i;
		ASSERT_CL(less_than_or_equal(val, blockRandUniform.getMax()));
		ASSERT_CL(greater_than_or_equal(val, blockRandUniform.getMin()));
	}

	ASSERT_CL(equal(mean, (blockRandUniform.getMin() +
			       blockRandUniform.getMax()) / 2, 1e-1));
	printf("PASS\n");
	ঘূৰাই 0;
}

পূর্ণাঙ্ক blockRandGaussTest()
{
	srand(1234);
	printf("Test BlockRandGauss\t\t: ");
	BlockRandGauss blockRandGauss(রিক্ত, "TEST");
	// test initial state
	ASSERT_CL(equal(0.0f, blockRandGauss.getDt()));
	ASSERT_CL(equal(1.0f, blockRandGauss.getMean()));
	ASSERT_CL(equal(2.0f, blockRandGauss.getStdDev()));
	// test update
	পূর্ণাঙ্ক n = 10000;
	ভিন্ন mean = blockRandGauss.update();
	ভিন্ন sum = 0;

	// recursive mean, stdev algorithm from Knuth
	বাবে (পূর্ণাঙ্ক i = 2; i < n + 1; i++) {
		ভিন্ন val = blockRandGauss.update();
		ভিন্ন newMean = mean + (val - mean) / i;
		sum += (val - mean) * (val - newMean);
		mean = newMean;
	}

	ভিন্ন stdDev = sqrt(sum / (n - 1));
	(শূন্য)(stdDev);
	ASSERT_CL(equal(mean, blockRandGauss.getMean(), 1e-1));
	ASSERT_CL(equal(stdDev, blockRandGauss.getStdDev(), 1e-1));
	printf("PASS\n");
	ঘূৰাই 0;
}

পূর্ণাঙ্ক blockStatsTest()
{
	printf("Test BlockStats\t\t\t: ");
	BlockStats<ভিন্ন, 1> stats(রিক্ত, "TEST");
	ASSERT_CL(equal(0.0f, stats.getMean()(0)));
	ASSERT_CL(equal(0.0f, stats.getStdDev()(0)));
	stats.update(matrix::Scalar<ভিন্ন>(1.0f));
	stats.update(matrix::Scalar<ভিন্ন>(2));
	ASSERT_CL(equal(1.5f, stats.getMean()(0)));
	ASSERT_CL(equal(0.5f, stats.getStdDev()(0)));
	stats.reset();
	ASSERT_CL(equal(0.0f, stats.getMean()(0)));
	ASSERT_CL(equal(0.0f, stats.getStdDev()(0)));
	printf("PASS\n");
	ঘূৰাই 0;
}

পূর্ণাঙ্ক blockDelayTest()
{
	printf("Test BlockDelay\t\t\t: ");
	using নামাকাশ matrix;
	BlockDelay<ভিন্ন, 2, 1, 3> delay(রিক্ত, "TEST");
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
	ঘূৰাই 0;
}

} // namespace control
