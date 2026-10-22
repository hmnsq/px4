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

صنف __EXPORT بلاڪ_Rand_Gauss: عام بلاڪ
{
عام:
// methods
	بلاڪ_Rand_Gauss(Super_بلاڪ *والد,
		       ثابت حرف *name) :
		بلاڪ(والد, name),
		سراسري_2(هذا, "MEAN"),
		_stdDev(هذا, "DEV")
	{
		// seed should be initialized somewhere
		// in main program for all calls to rand
		// XXX currently in nuttx if you seed to 0, rand breaks
	}
	مجازي ~بلاڪ_Rand_Gauss() = افتراضي;
	عائم update()
	{
		ساكن عائم V1, V2, S;
		ساكن صحيح phase = 0;
		عائم X;

		إذا (phase == 0) {
			افعل {
				عائم U1 = static_cast<عائم>(rand()) / static_cast<عائم>(RAND_MAX);
				عائم U2 = static_cast<عائم>(rand()) / static_cast<عائم>(RAND_MAX);
				V1 = 2 * U1 - 1;
				V2 = 2 * U2 - 1;
				S = V1 * V1 + V2 * V2;
			} طالما (S >= 1 || fabsf(S) < 1e-8f);

			X = V1 * عائم(sqrtf(-2 * عائم(logf(S)) / S));

		} وإلا {
			X = V2 * عائم(sqrtf(-2 * عائم(logf(S)) / S));
		}

		phase = 1 - phase;
		أرجع X * getStdDev() + get_سراسري();
	}
// accessors
	عائم get_سراسري() { أرجع سراسري_2.get(); }
	عائم getStdDev() { أرجع _stdDev.get(); }
خاص:
//خاصيتون
	control::بلاڪ_Param_Float سراسري_2;
	control::بلاڪ_Param_Float _stdDev;
};

} // namespace control
