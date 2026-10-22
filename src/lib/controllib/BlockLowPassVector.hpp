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

#هدايت once

#ضمن <px4_platform_common/defines.h>
#ضمن <assert.h>
#ضمن <time.h>
#ضمن <stdlib.h>
#ضمن <math.h>
#ضمن <mathlib/math/test/test.hpp>
#ضمن <mathlib/math/filter/LowPassFilter2p.hpp>

#ضمن "block/Block.hpp"
#ضمن "block/BlockParam.hpp"

#ضمن "matrix/math.hpp"

نطاق control
{

سانچو<صنف Type, size_t M>
صنف __EXPORT بلاڪ_هيٺيون_Pass_ويڪٽر: عام بلاڪ
{
عام:
// methods
	بلاڪ_هيٺيون_Pass_ويڪٽر(Super_بلاڪ *والد,
			   ثابت حرف *name) :
		بلاڪ(والد, name),
		_state(),
		_fCut(هذا, "") // only one parameter, no need to name
	{
		لكل (size_t i = 0; i < M; i++) {
			_state(i) = 0.0f / 0.0f;
		}
	}
	مجازي ~بلاڪ_هيٺيون_Pass_ويڪٽر() = افتراضي;
	ميٽرڪس::ويڪٽر<Type, M> update(ثابت ميٽرڪس::ميٽرڪس_2<Type, M, 1> &داخلا)
	{
		لكل (size_t i = 0; i < M; i++) {
			إذا (!PX4_ISFINITE(getState()(i))) {
				سيٽ_State(داخلا);
			}
		}

		عائم b = 2 * M_PI_F * getFCut() * getDt();
		عائم a = b / (1 + b);
		سيٽ_State(داخلا * a + getState() * (1 - a));
		أرجع getState();
	}
// accessors
	ميٽرڪس::ويڪٽر<Type, M> getState() { أرجع _state; }
	عائم getFCut() { أرجع _fCut.get(); }
	فراغ سيٽ_State(ثابت ميٽرڪس::ويڪٽر<Type, M> &state) { _state = state; }
خاص:
//خاصيتون
	ميٽرڪس::ويڪٽر<Type, M> _state;
	control::بلاڪ_Param_Float _fCut;
};

} // namespace control
