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
		       स्थिरांक अक्षर *name) :
		Block(parent, name),
		_mean(हे, "MEAN"),
		_stdDev(हे, "DEV")
	{
		// seed should be initialized somewhere
		// in main program for all calls to rand
		// XXX currently in nuttx if you seed to 0, rand breaks
	}
	आभासी ~BlockRandGauss() = पूर्वनिर्धारित;
	दशांश update()
	{
		स्थिर दशांश V1, V2, S;
		स्थिर पूर्णांक phase = 0;
		दशांश X;

		जर (phase == 0) {
			करा {
				दशांश U1 = static_cast<दशांश>(अक्रम()) / static_cast<दशांश>(अक्रम_उच्च);
				दशांश U2 = static_cast<दशांश>(अक्रम()) / static_cast<दशांश>(अक्रम_उच्च);
				V1 = 2 * U1 - 1;
				V2 = 2 * U2 - 1;
				S = V1 * V1 + V2 * V2;
			} पर्यंत (S >= 1 || fabsf(S) < 1e-8f);

			X = V1 * दशांश(sqrtf(-2 * दशांश(logf(S)) / S));

		} अन्यथा {
			X = V2 * दशांश(sqrtf(-2 * दशांश(logf(S)) / S));
		}

		phase = 1 - phase;
		परत X * getStdDev() + getMean();
	}
// accessors
	दशांश getMean() { परत _mean.get(); }
	दशांश getStdDev() { परत _stdDev.get(); }
खाजगी:
// attributes
	control::BlockParamFloat _mean;
	control::BlockParamFloat _stdDev;
};

} // namespace control
