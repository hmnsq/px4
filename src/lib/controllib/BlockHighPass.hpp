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
 * @file BlockHighPass.hpp
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
 * A high pass filter as described here:
 * http://en.wikipedia.org/wiki/High-pass_filter.
 */
वर्ग __EXPORT Block_माथिल्लो_Pass : सार्वजनिक Block
{
सार्वजनिक:
// methods
	Block_माथिल्लो_Pass(SuperBlock *parent, अचर वर्ण *name) :
		Block(parent, name),
		_u(0),
		_y(0),
		_fCut(एतत्, "") // only one parameter, no need to name
	{}
	आभास ~Block_माथिल्लो_Pass() = स्वभाव;
	भिन्न update(भिन्न आगत);
// accessors
	भिन्न getU() {फर्काऊ _u;}
	भिन्न getY() {फर्काऊ _y;}
	भिन्न getFCut() {फर्काऊ _fCut.get();}
	शून्य setU(भिन्न u) {_u = u;}
	शून्य setY(भिन्न y) {_y = y;}
रक्षित:
// attributes
	भिन्न _u; /**< previous input */
	भिन्न _y; /**< previous output */
	control::BlockParamFloat _fCut; /**< cut-off frequency, Hz */
};

} // namespace control
