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

جماعت __EXPORT بلاک_Rand_Gauss: عوامی بلاک
{
عوامی:
// methods
	بلاک_Rand_Gauss(Super_بلاک *والد,
		       ثابت حرف *name) :
		بلاک(والد, name),
		اوسط_2(یہ, "MEAN"),
		_stdDev(یہ, "DEV")
	{
		// seed should be initialized somewhere
		// in main program for all calls to rand
		// XXX currently in nuttx if you seed to 0, rand breaks
	}
	مجازی ~بلاک_Rand_Gauss() = طےشدہ;
	اعشاری update()
	{
		ساکن اعشاری V1, V2, S;
		ساکن صحیح phase = 0;
		اعشاری X;

		اگر (phase == 0) {
			کرو {
				اعشاری U1 = static_cast<اعشاری>(rand()) / static_cast<اعشاری>(RAND_MAX);
				اعشاری U2 = static_cast<اعشاری>(rand()) / static_cast<اعشاری>(RAND_MAX);
				V1 = 2 * U1 - 1;
				V2 = 2 * U2 - 1;
				S = V1 * V1 + V2 * V2;
			} جب (S >= 1 || fabsf(S) < 1e-8f);

			X = V1 * اعشاری(sqrtf(-2 * اعشاری(logf(S)) / S));

		} ورنہ {
			X = V2 * اعشاری(sqrtf(-2 * اعشاری(logf(S)) / S));
		}

		phase = 1 - phase;
		واپس X * getStdDev() + get_اوسط();
	}
// accessors
	اعشاری get_اوسط() { واپس اوسط_2.get(); }
	اعشاری getStdDev() { واپس _stdDev.get(); }
نجی:
//اوصاف
	control::بلاک_Param_Float اوسط_2;
	control::بلاک_Param_Float _stdDev;
};

} // namespace control
