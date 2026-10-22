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
 * @file BlockPID.hpp
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

/**
 * A proportional-integral-derivative controller.
 * @link http://en.wikipedia.org/wiki/PID_controller
 */
رده __EXPORT BlockPID: عمومی SuperBlock
{
عمومی:
// methods
	BlockPID(SuperBlock *parent, ثابت نویسه *name) :
		SuperBlock(parent, name),
		_integral(این, "I"),
		_derivative(این, "D"),
		_kP(این, "P"),
		_kI(این, "I"),
		_kD(این, "D")
	{}
	مجازی ~BlockPID() = پیشفرض;
	اعشاری update(اعشاری درون_داد)
	{
		بازگشت getKP() * درون_داد +
		       getKI() * getIntegral().update(درون_داد) +
		       getKD() * getDerivative().update(درون_داد);
	}
// accessors
	اعشاری getKP() { بازگشت _kP.get(); }
	اعشاری getKI() { بازگشت _kI.get(); }
	اعشاری getKD() { بازگشت _kD.get(); }
	BlockIntegral &getIntegral() { بازگشت _integral; }
	BlockDerivative &getDerivative() { بازگشت _derivative; }
خصوصی:
// attributes
	BlockIntegral _integral;
	BlockDerivative _derivative;
	control::BlockParamFloat _kP;
	control::BlockParamFloat _kI;
	control::BlockParamFloat _kD;
};

} // namespace control
