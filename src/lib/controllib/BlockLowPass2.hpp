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

#ہدایت once

#شامل <px4_platform_common/defines.h>
#شامل <assert.h>
#شامل <time.h>
#شامل <stdlib.h>
#شامل <math.h>
#شامل <mathlib/math/test/test.hpp>
#شامل <mathlib/math/filter/LowPassFilter2p.hpp>

#شامل "block/Block.hpp"
#شامل "block/BlockParam.hpp"

#شامل "matrix/math.hpp"

نامخلا control
{

/**
 * A 2nd order low pass filter block which uses the default px4 2nd order low pass filter
 */
جماعت __EXPORT بلاک_کم_Pass2 : عوامی بلاک
{
عوامی:
// methods
	بلاک_کم_Pass2(Super_بلاک *والد, ثابت حرف *name, اعشاری sample_freq) :
		بلاک(والد, name),
		_state(0.0 / 0.0 /* initialize to invalid val, force into is_finite() check on first call */),
		_fCut(یہ, ""), // only one parameter, no need to name
		_fs(sample_freq),
		_lp(_fs, _fCut.get())
	{}
	مجازی ~بلاک_کم_Pass2() = طےشدہ;
	اعشاری update(اعشاری داخلہ);
// accessors
	اعشاری getState() { واپس _state; }
	اعشاری getFCutParam() { واپس _fCut.get(); }
	خالی سیٹ_State(اعشاری state) { _state = _lp.reset(state); }
محفوظ:
//اوصاف
	اعشاری _state;
	control::بلاک_Param_Float _fCut;
	اعشاری _fs;
	math::کم_Pass_Filter2p<اعشاری> _lp;
};

} // namespace control
