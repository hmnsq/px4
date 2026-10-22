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
 * @file BlockStats.hpp
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
صنف __EXPORT بلاڪ_Stats: عام بلاڪ
{

عام:
// methods
	بلاڪ_Stats(Super_بلاڪ *والد,
		   ثابت حرف *name) :
		بلاڪ(والد, name),
		جوڙ_2(),
		جوڙ_Sq(),
		ڳڻپ_3(0)
	{}
	مجازي ~بلاڪ_Stats() = افتراضي;
	فراغ update(ثابت ميٽرڪس::ويڪٽر<Type, M> &u)
	{
		جوڙ_2 += u;
		جوڙ_Sq += u.emult(u);
		ڳڻپ_3 += 1;
	}
	فراغ reset()
	{
		جوڙ_2.سيٽ_Zero();
		جوڙ_Sq.سيٽ_Zero();
		ڳڻپ_3 = 0;
	}
// accessors
	size_t get_ڳڻپ() { أرجع ڳڻپ_3; }
	ميٽرڪس::ويڪٽر<Type, M> get_سراسري() { أرجع جوڙ_2 / ڳڻپ_3; }
	ميٽرڪس::ويڪٽر<Type, M> getVar()
	{
		أرجع (جوڙ_Sq - جوڙ_2.emult(جوڙ_2) / ڳڻپ_3) / ڳڻپ_3;
	}
	ميٽرڪس::ويڪٽر<Type, M> getStdDev()
	{
		أرجع getVar().جذر();
	}
خاص:
//خاصيتون
	ميٽرڪس::ويڪٽر<Type, M> جوڙ_2;
	ميٽرڪس::ويڪٽر<Type, M> جوڙ_Sq;
	size_t ڳڻپ_3;
};

} // namespace control
