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
 * @file BlockOutput.hpp
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
 * An output trim/ saturation block
 */
رده __EXPORT BlockOutput: عمومی SuperBlock
{
عمومی:
// methods
	BlockOutput(SuperBlock *parent, ثابت نویسه *name) :
		SuperBlock(parent, name),
		_trim(این, "TRIM"),
		_limit(این, ""),
		_val(0)
	{
		update(0);
	}

	مجازی ~BlockOutput() = پیشفرض;
	تهی update(اعشاری درون_داد)
	{
		_val = _limit.update(درون_داد + getTrim());
	}
// accessors
	اعشاری getMin() { بازگشت _limit.getMin(); }
	اعشاری getMax() { بازگشت _limit.getMax(); }
	اعشاری getTrim() { بازگشت _trim.get(); }
	اعشاری get() { بازگشت _val; }
خصوصی:
// attributes
	control::BlockParamFloat _trim;
	BlockLimit _limit;
	اعشاری _val;
};

} // namespace control
