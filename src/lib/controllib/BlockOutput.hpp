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
 * @file BlockOutput.hpp
 *
 * Controller library code
 */

#pragma once

#അന്തര്ഭാവ <px4_platform_common/defines.h>
#അന്തര്ഭാവ <assert.h>
#അന്തര്ഭാവ <time.h>
#അന്തര്ഭാവ <stdlib.h>
#അന്തര്ഭാവ <math.h>
#അന്തര്ഭാവ <mathlib/math/test/test.hpp>
#അന്തര്ഭാവ <mathlib/math/filter/LowPassFilter2p.hpp>

#അന്തര്ഭാവ "block/Block.hpp"
#അന്തര്ഭാവ "block/BlockParam.hpp"

#അന്തര്ഭാവ "matrix/math.hpp"

നാമാകാശ control
{
/**
 * An output trim/ saturation block
 */
വർഗ്ഗം __EXPORT BlockOutput: പൊതു SuperBlock
{
പൊതു:
// methods
	BlockOutput(SuperBlock *parent, സ്ഥിരം അക്ഷരം *name) :
		SuperBlock(parent, name),
		_trim(ഇത്, "TRIM"),
		_limit(ഇത്, ""),
		_val(0)
	{
		update(0);
	}

	മായാ ~BlockOutput() = സ്ഥിരസ്ഥിതി;
	ശൂന്യം update(ദശാംശം നിവേശം)
	{
		_val = _limit.update(നിവേശം + getTrim());
	}
// accessors
	ദശാംശം getMin() { തിരികെ _limit.getMin(); }
	ദശാംശം getMax() { തിരികെ _limit.getMax(); }
	ദശാംശം getTrim() { തിരികെ _trim.get(); }
	ദശാംശം get() { തിരികെ _val; }
സ്വകാര്യ:
// attributes
	control::BlockParamFloat _trim;
	BlockLimit _limit;
	ദശാംശം _val;
};

} // namespace control
