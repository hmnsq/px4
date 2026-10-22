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

/**
 * A proportional-integral-derivative controller.
 * @link http://en.wikipedia.org/wiki/PID_controller
 */
מחלקה __EXPORT BlockPID: عام SuperBlock
{
عام:
// methods
	BlockPID(SuperBlock *parent, ثابت حرف *name) :
		SuperBlock(parent, name),
		_integral(هذا, "I"),
		_derivative(هذا, "D"),
		_kP(هذا, "P"),
		_kI(هذا, "I"),
		_kD(هذا, "D")
	{}
	virtual ~BlockPID() = افتراضي;
	عائم update(عائم input)
	{
		החזר getKP() * input +
		       getKI() * getIntegral().update(input) +
		       getKD() * getDerivative().update(input);
	}
// accessors
	عائم getKP() { החזר _kP.get(); }
	عائم getKI() { החזר _kI.get(); }
	عائم getKD() { החזר _kD.get(); }
	BlockIntegral &getIntegral() { החזר _integral; }
	BlockDerivative &getDerivative() { החזר _derivative; }
خاص:
// attributes
	BlockIntegral _integral;
	BlockDerivative _derivative;
	control::BlockParamFloat _kP;
	control::BlockParamFloat _kI;
	control::BlockParamFloat _kD;
};

} // namespace control
