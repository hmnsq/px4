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
 * @file BlockLowPass2.hpp
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

/**
 * A 2nd order low pass filter block which uses the default px4 2nd order low pass filter
 */
વર્ગ __EXPORT Block_નીચલું_Pass2 : જાહેર Block
{
જાહેર:
// methods
	Block_નીચલું_Pass2(SuperBlock *parent, અચલ અક્ષર *name, દશાંશ sample_freq) :
		Block(parent, name),
		_state(0.0 / 0.0 /* initialize to invalid val, force into is_finite() check on first call */),
		_fCut(આ, ""), // only one parameter, no need to name
		_fs(sample_freq),
		_lp(_fs, _fCut.get())
	{}
	આભાસી ~Block_નીચલું_Pass2() = મૂળભૂત;
	દશાંશ update(દશાંશ આગત);
// accessors
	દશાંશ getState() { પાછા _state; }
	દશાંશ getFCutParam() { પાછા _fCut.get(); }
	શૂન્ય setState(દશાંશ state) { _state = _lp.reset(state); }
સુરક્ષિત:
// attributes
	દશાંશ _state;
	control::BlockParamFloat _fCut;
	દશાંશ _fs;
	math::નીચલું_Pass_Filter2p<દશાંશ> _lp;
};

} // namespace control
