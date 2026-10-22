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

#ہدایت once

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
سانچا<صنف Type, size_t M>
صنف __EXPORT بلاک_Stats: عام بلاک
{

عام:
// methods
	بلاک_Stats(Super_بلاک *ماپا,
		   ثابت حرف *name) :
		بلاک(ماپا, name),
		جوڑ_2(),
		جوڑ_Sq(),
		گنتی_3(0)
	{}
	مجازی ~بلاک_Stats() = افتراضي;
	فراغ update(ثابت میٹرکس::سمتیہ<Type, M> &u)
	{
		جوڑ_2 += u;
		جوڑ_Sq += u.emult(u);
		گنتی_3 += 1;
	}
	فراغ reset()
	{
		جوڑ_2.سیٹ_Zero();
		جوڑ_Sq.سیٹ_Zero();
		گنتی_3 = 0;
	}
// accessors
	size_t get_گنتی() { أرجع گنتی_3; }
	میٹرکس::سمتیہ<Type, M> get_اوسط() { أرجع جوڑ_2 / گنتی_3; }
	میٹرکس::سمتیہ<Type, M> getVar()
	{
		أرجع (جوڑ_Sq - جوڑ_2.emult(جوڑ_2) / گنتی_3) / گنتی_3;
	}
	میٹرکس::سمتیہ<Type, M> getStdDev()
	{
		أرجع getVar().جذر();
	}
خاص:
//گن
	میٹرکس::سمتیہ<Type, M> جوڑ_2;
	میٹرکس::سمتیہ<Type, M> جوڑ_Sq;
	size_t گنتی_3;
};

} // namespace control
