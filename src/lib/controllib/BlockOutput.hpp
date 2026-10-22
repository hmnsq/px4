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

#அந்தர்பாவ <px4_platform_common/defines.h>
#அந்தர்பாவ <assert.h>
#அந்தர்பாவ <time.h>
#அந்தர்பாவ <stdlib.h>
#அந்தர்பாவ <math.h>
#அந்தர்பாவ <mathlib/math/test/test.hpp>
#அந்தர்பாவ <mathlib/math/filter/LowPassFilter2p.hpp>

#அந்தர்பாவ "block/Block.hpp"
#அந்தர்பாவ "block/BlockParam.hpp"

#அந்தர்பாவ "matrix/math.hpp"

நாமாகாஶ control
{
/**
 * An output trim/ saturation block
 */
வகுப்பு __EXPORT BlockOutput: பொது SuperBlock
{
பொது:
// methods
	BlockOutput(SuperBlock *parent, மாறிலி எழுத்து *name) :
		SuperBlock(parent, name),
		_trim(இது, "TRIM"),
		_limit(இது, ""),
		_val(0)
	{
		update(0);
	}

	மெய்நிகர் ~BlockOutput() = இயல்பு;
	வெறுமை update(தசம உள்ளீடு)
	{
		_val = _limit.update(உள்ளீடு + getTrim());
	}
// accessors
	தசம getMin() { திரும்பு _limit.getMin(); }
	தசம getMax() { திரும்பு _limit.getMax(); }
	தசம getTrim() { திரும்பு _trim.get(); }
	தசம get() { திரும்பு _val; }
தனிப்பட்ட:
// attributes
	control::BlockParamFloat _trim;
	BlockLimit _limit;
	தசம _val;
};

} // namespace control
