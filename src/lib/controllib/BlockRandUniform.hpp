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
 * @file BlockRandUniform.hpp
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

/**
 * A uniform random number generator
 */
वर्ग __EXPORT BlockRandUniform: सार्वजनिक Block
{
सार्वजनिक:
// methods
	BlockRandUniform(SuperBlock *parent,
			 स्थिरांक अक्षर *name) :
		Block(parent, name),
		_min(हे, "MIN"),
		_max(हे, "MAX")
	{
		// seed should be initialized somewhere
		// in main program for all calls to rand
		// XXX currently in nuttx if you seed to 0, rand breaks
	}
	आभासी ~BlockRandUniform() = पूर्वनिर्धारित;
	दशांश update()
	{
		स्थिर दशांश rand_max = static_cast<दशांश>(अक्रम_उच्च);
		दशांश rand_val = अक्रम();
		दशांश bounds = getMax() - getMin();
		परत getMin() + (rand_val * bounds) / rand_max;
	}
// accessors
	दशांश getMin() { परत _min.get(); }
	दशांश getMax() { परत _max.get(); }
खाजगी:
// attributes
	control::BlockParamFloat _min;
	control::BlockParamFloat _max;
};

} // namespace control
