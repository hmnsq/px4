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
 * @file BlockLowPassVector.hpp
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

ढाँचा<वर्ग Type, माप_प्रकार M>
वर्ग __EXPORT Block_खालचा_Pass_Vector: सार्वजनिक Block
{
सार्वजनिक:
// methods
	Block_खालचा_Pass_Vector(SuperBlock *parent,
			   स्थिरांक अक्षर *name) :
		Block(parent, name),
		_state(),
		_fCut(हे, "") // only one parameter, no need to name
	{
		साठी (माप_प्रकार i = 0; i < M; i++) {
			_state(i) = 0.0f / 0.0f;
		}
	}
	आभासी ~Block_खालचा_Pass_Vector() = पूर्वनिर्धारित;
	matrix::Vector<Type, M> update(स्थिरांक matrix::Matrix<Type, M, 1> &आदान)
	{
		साठी (माप_प्रकार i = 0; i < M; i++) {
			जर (!PX4_ISFINITE(getState()(i))) {
				setState(आदान);
			}
		}

		दशांश b = 2 * M_PI_F * getFCut() * getDt();
		दशांश a = b / (1 + b);
		setState(आदान * a + getState() * (1 - a));
		परत getState();
	}
// accessors
	matrix::Vector<Type, M> getState() { परत _state; }
	दशांश getFCut() { परत _fCut.get(); }
	शून्य setState(स्थिरांक matrix::Vector<Type, M> &state) { _state = state; }
खाजगी:
// attributes
	matrix::Vector<Type, M> _state;
	control::BlockParamFloat _fCut;
};

} // namespace control
