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

#pragma once

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

فضاینام control
{

رده __EXPORT BlockRandGauss: عمومی Block
{
عمومی:
// methods
	BlockRandGauss(SuperBlock *parent,
		       ثابت نویسه *name) :
		Block(parent, name),
		_mean(این, "MEAN"),
		_stdDev(این, "DEV")
	{
		// seed should be initialized somewhere
		// in main program for all calls to rand
		// XXX currently in nuttx if you seed to 0, rand breaks
	}
	مجازی ~BlockRandGauss() = پیشفرض;
	اعشاری update()
	{
		ایستا اعشاری V1, V2, S;
		ایستا صحیح phase = 0;
		اعشاری X;

		اگر (phase == 0) {
			انجامبده {
				اعشاری U1 = static_cast<اعشاری>(rand()) / static_cast<اعشاری>(RAND_MAX);
				اعشاری U2 = static_cast<اعشاری>(rand()) / static_cast<اعشاری>(RAND_MAX);
				V1 = 2 * U1 - 1;
				V2 = 2 * U2 - 1;
				S = V1 * V1 + V2 * V2;
			} وقتی (S >= 1 || fabsf(S) < 1e-8f);

			X = V1 * اعشاری(sqrtf(-2 * اعشاری(logf(S)) / S));

		} وگرنه {
			X = V2 * اعشاری(sqrtf(-2 * اعشاری(logf(S)) / S));
		}

		phase = 1 - phase;
		بازگشت X * getStdDev() + getMean();
	}
// accessors
	اعشاری getMean() { بازگشت _mean.get(); }
	اعشاری getStdDev() { بازگشت _stdDev.get(); }
خصوصی:
// attributes
	control::BlockParamFloat _mean;
	control::BlockParamFloat _stdDev;
};

} // namespace control
