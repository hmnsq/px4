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
 * @file BlockPI.hpp
 *
 * Controller library code
 */

#pragma once

#অন্তর্ভাব <px4_platform_common/defines.h>
#অন্তর্ভাব <assert.h>
#অন্তর্ভাব <time.h>
#অন্তর্ভাব <stdlib.h>
#অন্তর্ভাব <math.h>
#অন্তর্ভাব <mathlib/math/test/test.hpp>
#অন্তর্ভাব <mathlib/math/filter/LowPassFilter2p.hpp>

#অন্তর্ভাব "block/Block.hpp"
#অন্তর্ভাব "block/BlockParam.hpp"

#অন্তর্ভাব "matrix/math.hpp"

নামাকাশ control
{

/**
 * A proportional-integral controller.
 * @link http://en.wikipedia.org/wiki/PID_controller
 */
বর্গ __EXPORT BlockPI: সার্বজনিক SuperBlock
{
সার্বজনিক:
// methods
	BlockPI(SuperBlock *parent, ধ্রুব বর্ণ *name) :
		SuperBlock(parent, name),
		_integral(এতত্, "I"),
		_kP(এতত্, "P"),
		_kI(এতত্, "I")
	{}
	আভাস ~BlockPI() = স্বভাব;
	ভিন্ন update(ভিন্ন প্রবেশ)
	{
		প্রত্যাগম getKP() * প্রবেশ +
		       getKI() * getIntegral().update(প্রবেশ);
	}
// accessors
	ভিন্ন getKP() { প্রত্যাগম _kP.get(); }
	ভিন্ন getKI() { প্রত্যাগম _kI.get(); }
	BlockIntegral &getIntegral() { প্রত্যাগম _integral; }
গুপ্ত:
	BlockIntegral _integral;
	control::BlockParamFloat _kP;
	control::BlockParamFloat _kI;
};

} // namespace control
