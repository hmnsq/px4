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

#ଅନ୍ତର୍ଭାଵ <px4_platform_common/defines.h>
#ଅନ୍ତର୍ଭାଵ <assert.h>
#ଅନ୍ତର୍ଭାଵ <time.h>
#ଅନ୍ତର୍ଭାଵ <stdlib.h>
#ଅନ୍ତର୍ଭାଵ <math.h>
#ଅନ୍ତର୍ଭାଵ <mathlib/math/test/test.hpp>
#ଅନ୍ତର୍ଭାଵ <mathlib/math/filter/LowPassFilter2p.hpp>

#ଅନ୍ତର୍ଭାଵ "block/Block.hpp"
#ଅନ୍ତର୍ଭାଵ "block/BlockParam.hpp"

#ଅନ୍ତର୍ଭାଵ "matrix/math.hpp"

ନାମାକାଶ control
{

/**
 * A limiter/ saturation.
 * The output of update is the input, bounded
 * by min/max.
 */
ଶ୍ରେଣୀ __EXPORT BlockLimit : ସର୍ବସାଧାରଣ Block
{
ସର୍ବସାଧାରଣ:
// methods
	BlockLimit(SuperBlock *parent, ଅଚଳ ଅକ୍ଷର *name) :
		Block(parent, name),
		_min(ଏହି, "MIN"),
		_max(ଏହି, "MAX")
	{}
	ଆଭାସୀ ~BlockLimit() = ପୂର୍ବନିର୍ଦ୍ଧାରିତ;
	ଦଶମିକ update(ଦଶମିକ ନିବେଶ);
// accessors
	ଦଶମିକ getMin() { ଫେରନ୍ତୁ _min.get(); }
	ଦଶମିକ getMax() { ଫେରନ୍ତୁ _max.get(); }
ସୁରକ୍ଷିତ:
// attributes
	control::BlockParamFloat _min;
	control::BlockParamFloat _max;
};

} // namespace control
