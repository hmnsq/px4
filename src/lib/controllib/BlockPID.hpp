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

/**
 * A proportional-integral-derivative controller.
 * @link http://en.wikipedia.org/wiki/PID_controller
 */
صنف __EXPORT بلوک_PID: عام Super_بلوک
{
عام:
// methods
	بلوک_PID(Super_بلوک *والد, ثابت حرف *name) :
		Super_بلوک(والد, name),
		_integral(هذا, "I"),
		_derivative(هذا, "D"),
		_kP(هذا, "P"),
		_kI(هذا, "I"),
		_kD(هذا, "D")
	{}
	مجازی ~بلوک_PID() = افتراضي;
	عائم update(عائم درون_داد)
	{
		أرجع getKP() * درون_داد +
		       getKI() * getIntegral().update(درون_داد) +
		       getKD() * getDerivative().update(درون_داد);
	}
// accessors
	عائم getKP() { أرجع _kP.get(); }
	عائم getKI() { أرجع _kI.get(); }
	عائم getKD() { أرجع _kD.get(); }
	بلوک_Integral &getIntegral() { أرجع _integral; }
	بلوک_Derivative &getDerivative() { أرجع _derivative; }
خاص:
//صفات
	بلوک_Integral _integral;
	بلوک_Derivative _derivative;
	control::بلوک_Param_Float _kP;
	control::بلوک_Param_Float _kI;
	control::بلوک_Param_Float _kD;
};

} // namespace control
