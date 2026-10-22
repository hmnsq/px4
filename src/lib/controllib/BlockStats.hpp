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

#رهنمود once

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
قالب<صنف Type, size_t M>
صنف __EXPORT بلوک_Stats: عام بلوک
{

عام:
// methods
	بلوک_Stats(Super_بلوک *والد,
		   ثابت حرف *name) :
		بلوک(والد, name),
		جمع_2(),
		جمع_Sq(),
		شمار_3(0)
	{}
	مجازی ~بلوک_Stats() = افتراضي;
	فراغ update(ثابت ماتریس::وکتور<Type, M> &u)
	{
		جمع_2 += u;
		جمع_Sq += u.emult(u);
		شمار_3 += 1;
	}
	فراغ reset()
	{
		جمع_2.مجموعه_Zero();
		جمع_Sq.مجموعه_Zero();
		شمار_3 = 0;
	}
// accessors
	size_t get_شمار() { أرجع شمار_3; }
	ماتریس::وکتور<Type, M> get_میانگین() { أرجع جمع_2 / شمار_3; }
	ماتریس::وکتور<Type, M> getVar()
	{
		أرجع (جمع_Sq - جمع_2.emult(جمع_2) / شمار_3) / شمار_3;
	}
	ماتریس::وکتور<Type, M> getStdDev()
	{
		أرجع getVar().جذر();
	}
خاص:
//صفات
	ماتریس::وکتور<Type, M> جمع_2;
	ماتریس::وکتور<Type, M> جمع_Sq;
	size_t شمار_3;
};

} // namespace control
