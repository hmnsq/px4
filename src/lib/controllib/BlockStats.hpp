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
سانچہ<جماعت Type, size_t M>
جماعت __EXPORT بلاک_Stats: عوامی بلاک
{

عوامی:
// methods
	بلاک_Stats(Super_بلاک *والد,
		   ثابت حرف *name) :
		بلاک(والد, name),
		مجموع_2(),
		مجموع_Sq(),
		گنتی_3(0)
	{}
	مجازی ~بلاک_Stats() = طےشدہ;
	خالی update(ثابت میٹرکس::سمتیہ<Type, M> &u)
	{
		مجموع_2 += u;
		مجموع_Sq += u.emult(u);
		گنتی_3 += 1;
	}
	خالی reset()
	{
		مجموع_2.سیٹ_Zero();
		مجموع_Sq.سیٹ_Zero();
		گنتی_3 = 0;
	}
// accessors
	size_t get_گنتی() { واپس گنتی_3; }
	میٹرکس::سمتیہ<Type, M> get_اوسط() { واپس مجموع_2 / گنتی_3; }
	میٹرکس::سمتیہ<Type, M> getVar()
	{
		واپس (مجموع_Sq - مجموع_2.emult(مجموع_2) / گنتی_3) / گنتی_3;
	}
	میٹرکس::سمتیہ<Type, M> getStdDev()
	{
		واپس getVar().جذر();
	}
نجی:
//اوصاف
	میٹرکس::سمتیہ<Type, M> مجموع_2;
	میٹرکس::سمتیہ<Type, M> مجموع_Sq;
	size_t گنتی_3;
};

} // namespace control
