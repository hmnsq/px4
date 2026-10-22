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

#ہدایت once

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
 * An output trim/ saturation block
 */
صنف __EXPORT بلاک_Output: عام Super_بلاک
{
عام:
// methods
	بلاک_Output(Super_بلاک *ماپا, ثابت حرف *name) :
		Super_بلاک(ماپا, name),
		_trim(هذا, "TRIM"),
		حد_بندی(هذا, ""),
		_val(0)
	{
		update(0);
	}

	مجازی ~بلاک_Output() = افتراضي;
	فراغ update(عائم داخلہ)
	{
		_val = حد_بندی.update(داخلہ + getTrim());
	}
// accessors
	عائم getMin() { أرجع حد_بندی.getMin(); }
	عائم getMax() { أرجع حد_بندی.getMax(); }
	عائم getTrim() { أرجع _trim.get(); }
	عائم get() { أرجع _val; }
خاص:
//گن
	control::بلاک_Param_Float _trim;
	بلاک_حد_بندی حد_بندی;
	عائم _val;
};

} // namespace control
