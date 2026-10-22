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
 * @file BlockRandGauss.hpp
 *
 * Controller library code
 */

#pragma once

#અન્તર્ભાવ <px4_platform_common/defines.h>
#અન્તર્ભાવ <assert.h>
#અન્તર્ભાવ <time.h>
#અન્તર્ભાવ <stdlib.h>
#અન્તર્ભાવ <math.h>
#અન્તર્ભાવ <mathlib/math/test/test.hpp>
#અન્તર્ભાવ <mathlib/math/filter/LowPassFilter2p.hpp>

#અન્તર્ભાવ "block/Block.hpp"
#અન્તર્ભાવ "block/BlockParam.hpp"

#અન્તર્ભાવ "matrix/math.hpp"

નામાકાશ control
{

વર્ગ __EXPORT BlockRandGauss: જાહેર Block
{
જાહેર:
// methods
	BlockRandGauss(SuperBlock *parent,
		       અચલ અક્ષર *name) :
		Block(parent, name),
		_mean(આ, "MEAN"),
		_stdDev(આ, "DEV")
	{
		// seed should be initialized somewhere
		// in main program for all calls to rand
		// XXX currently in nuttx if you seed to 0, rand breaks
	}
	આભાસી ~BlockRandGauss() = મૂળભૂત;
	દશાંશ update()
	{
		સ્થિર દશાંશ V1, V2, S;
		સ્થિર પૂર્ણાંક phase = 0;
		દશાંશ X;

		જો (phase == 0) {
			કરો {
				દશાંશ U1 = static_cast<દશાંશ>(rand()) / static_cast<દશાંશ>(RAND_MAX);
				દશાંશ U2 = static_cast<દશાંશ>(rand()) / static_cast<દશાંશ>(RAND_MAX);
				V1 = 2 * U1 - 1;
				V2 = 2 * U2 - 1;
				S = V1 * V1 + V2 * V2;
			} જ્યારે (S >= 1 || fabsf(S) < 1e-8f);

			X = V1 * દશાંશ(sqrtf(-2 * દશાંશ(logf(S)) / S));

		} નહીંતર {
			X = V2 * દશાંશ(sqrtf(-2 * દશાંશ(logf(S)) / S));
		}

		phase = 1 - phase;
		પાછા X * getStdDev() + getMean();
	}
// accessors
	દશાંશ getMean() { પાછા _mean.get(); }
	દશાંશ getStdDev() { પાછા _stdDev.get(); }
ખાનગી:
// attributes
	control::BlockParamFloat _mean;
	control::BlockParamFloat _stdDev;
};

} // namespace control
