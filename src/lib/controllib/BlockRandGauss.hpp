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

#अन्तर्भाव <px4_platform_common/defines.h>
#अन्तर्भाव <assert.h>
#अन्तर्भाव <time.h>
#अन्तर्भाव <stdlib.h>
#अन्तर्भाव <math.h>
#अन्तर्भाव <mathlib/math/test/test.hpp>
#अन्तर्भाव <mathlib/math/filter/LowPassFilter2p.hpp>

#अन्तर्भाव "block/Block.hpp"
#अन्तर्भाव "block/BlockParam.hpp"

#अन्तर्भाव "matrix/math.hpp"

नामाकाश control
{

वर्ग __EXPORT BlockRandGauss: सार्वजनिक Block
{
सार्वजनिक:
// methods
	BlockRandGauss(SuperBlock *parent,
		       अचर वर्ण *name) :
		Block(parent, name),
		_mean(एतत्, "MEAN"),
		_stdDev(एतत्, "DEV")
	{
		// seed should be initialized somewhere
		// in main program for all calls to rand
		// XXX currently in nuttx if you seed to 0, rand breaks
	}
	आभास ~BlockRandGauss() = स्वभाव;
	भिन्न update()
	{
		स्थिर भिन्न V1, V2, S;
		स्थिर पूर्णाङ्क phase = 0;
		भिन्न X;

		यदि (phase == 0) {
			गर {
				भिन्न U1 = static_cast<भिन्न>(अक्रम()) / static_cast<भिन्न>(अक्रम_उच्च);
				भिन्न U2 = static_cast<भिन्न>(अक्रम()) / static_cast<भिन्न>(अक्रम_उच्च);
				V1 = 2 * U1 - 1;
				V2 = 2 * U2 - 1;
				S = V1 * V1 + V2 * V2;
			} जबसम्म (S >= 1 || fabsf(S) < 1e-8f);

			X = V1 * भिन्न(sqrtf(-2 * भिन्न(logf(S)) / S));

		} अन्यथा {
			X = V2 * भिन्न(sqrtf(-2 * भिन्न(logf(S)) / S));
		}

		phase = 1 - phase;
		फर्काऊ X * getStdDev() + getMean();
	}
// accessors
	भिन्न getMean() { फर्काऊ _mean.get(); }
	भिन्न getStdDev() { फर्काऊ _stdDev.get(); }
गुप्त:
// attributes
	control::BlockParamFloat _mean;
	control::BlockParamFloat _stdDev;
};

} // namespace control
