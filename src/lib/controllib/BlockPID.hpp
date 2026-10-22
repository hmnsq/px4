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

#ಅನ್ತರ್ಭಾವ <px4_platform_common/defines.h>
#ಅನ್ತರ್ಭಾವ <assert.h>
#ಅನ್ತರ್ಭಾವ <time.h>
#ಅನ್ತರ್ಭಾವ <stdlib.h>
#ಅನ್ತರ್ಭಾವ <math.h>
#ಅನ್ತರ್ಭಾವ <mathlib/math/test/test.hpp>
#ಅನ್ತರ್ಭಾವ <mathlib/math/filter/LowPassFilter2p.hpp>

#ಅನ್ತರ್ಭಾವ "block/Block.hpp"
#ಅನ್ತರ್ಭಾವ "block/BlockParam.hpp"

#ಅನ್ತರ್ಭಾವ "matrix/math.hpp"

ನಾಮಾಕಾಶ control
{

/**
 * A proportional-integral-derivative controller.
 * @link http://en.wikipedia.org/wiki/PID_controller
 */
ವರ್ಗ __EXPORT BlockPID: ಸಾರ್ವಜನಿಕ SuperBlock
{
ಸಾರ್ವಜನಿಕ:
// methods
	BlockPID(SuperBlock *parent, ಧ್ರುವ ವರ್ಣ *name) :
		SuperBlock(parent, name),
		_integral(ಏತತ್, "I"),
		_derivative(ಏತತ್, "D"),
		_kP(ಏತತ್, "P"),
		_kI(ಏತತ್, "I"),
		_kD(ಏತತ್, "D")
	{}
	ಆಭಾಸ ~BlockPID() = ಸ್ವಭಾವ;
	ಭಿನ್ನ update(ಭಿನ್ನ ಒಳಹರಿವು)
	{
		ಪ್ರತ್ಯಾಗಮ getKP() * ಒಳಹರಿವು +
		       getKI() * getIntegral().update(ಒಳಹರಿವು) +
		       getKD() * getDerivative().update(ಒಳಹರಿವು);
	}
// accessors
	ಭಿನ್ನ getKP() { ಪ್ರತ್ಯಾಗಮ _kP.get(); }
	ಭಿನ್ನ getKI() { ಪ್ರತ್ಯಾಗಮ _kI.get(); }
	ಭಿನ್ನ getKD() { ಪ್ರತ್ಯಾಗಮ _kD.get(); }
	BlockIntegral &getIntegral() { ಪ್ರತ್ಯಾಗಮ _integral; }
	BlockDerivative &getDerivative() { ಪ್ರತ್ಯಾಗಮ _derivative; }
ಗುಪ್ತ:
// attributes
	BlockIntegral _integral;
	BlockDerivative _derivative;
	control::BlockParamFloat _kP;
	control::BlockParamFloat _kI;
	control::BlockParamFloat _kD;
};

} // namespace control
