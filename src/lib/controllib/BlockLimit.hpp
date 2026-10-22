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
 * @file BlockLimit.hpp
 *
 * Controller library code
 */

#pragma once

#অন্তর্ভাৱ <px4_platform_common/defines.h>
#অন্তর্ভাৱ <assert.h>
#অন্তর্ভাৱ <time.h>
#অন্তর্ভাৱ <stdlib.h>
#অন্তর্ভাৱ <math.h>
#অন্তর্ভাৱ <mathlib/math/test/test.hpp>
#অন্তর্ভাৱ <mathlib/math/filter/LowPassFilter2p.hpp>

#অন্তর্ভাৱ "block/Block.hpp"
#অন্তর্ভাৱ "block/BlockParam.hpp"

#অন্তর্ভাৱ "matrix/math.hpp"

নামাকাশ control
{

/**
 * A limiter/ saturation.
 * The output of update is the input, bounded
 * by min/max.
 */
শ্ৰেণী __EXPORT BlockLimit : সার্ৱজনিক Block
{
সার্ৱজনিক:
// methods
	BlockLimit(SuperBlock *parent, ধ্রুৱ ৱর্ণ *name) :
		Block(parent, name),
		_min(এতত্, "MIN"),
		_max(এতত্, "MAX")
	{}
	আভাস ~BlockLimit() = স্ৱভাৱ;
	ভিন্ন update(ভিন্ন প্ৰবেশ);
// accessors
	ভিন্ন getMin() { ঘূৰাই _min.get(); }
	ভিন্ন getMax() { ঘূৰাই _max.get(); }
রক্ষিত:
// attributes
	control::BlockParamFloat _min;
	control::BlockParamFloat _max;
};

} // namespace control
