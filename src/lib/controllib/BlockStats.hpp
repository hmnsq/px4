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

#لارښوونه once

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
کينډۍ<صنف Type, size_t M>
صنف __EXPORT بلاک_Stats: عام بلاک
{

عام:
// methods
	بلاک_Stats(Super_بلاک *پلار,
		   ثابت حرف *name) :
		بلاک(پلار, name),
		مجموعه_2(),
		مجموعه_Sq(),
		شمېر_3(0)
	{}
	مجازي ~بلاک_Stats() = افتراضي;
	فراغ update(ثابت ماتریکس::ویکټور<Type, M> &u)
	{
		مجموعه_2 += u;
		مجموعه_Sq += u.emult(u);
		شمېر_3 += 1;
	}
	فراغ reset()
	{
		مجموعه_2.سېټ_Zero();
		مجموعه_Sq.سېټ_Zero();
		شمېر_3 = 0;
	}
// accessors
	size_t get_شمېر() { أرجع شمېر_3; }
	ماتریکس::ویکټور<Type, M> get_منځنۍ() { أرجع مجموعه_2 / شمېر_3; }
	ماتریکس::ویکټور<Type, M> getVar()
	{
		أرجع (مجموعه_Sq - مجموعه_2.emult(مجموعه_2) / شمېر_3) / شمېر_3;
	}
	ماتریکس::ویکټور<Type, M> getStdDev()
	{
		أرجع getVar().جذر();
	}
خاص:
//ځانګړتیاوې
	ماتریکس::ویکټور<Type, M> مجموعه_2;
	ماتریکس::ویکټور<Type, M> مجموعه_Sq;
	size_t شمېر_3;
};

} // namespace control
