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

#لارښوونه once

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
 * A limiter/ saturation.
 * The output of update is the input, bounded
 * by min/max.
 */
صنف __EXPORT بلاک_حد : عام بلاک
{
عام:
// methods
	بلاک_حد(Super_بلاک *پلار, ثابت حرف *name) :
		بلاک(پلار, name),
		_min(هذا, "MIN"),
		_max(هذا, "MAX")
	{}
	مجازي ~بلاک_حد() = افتراضي;
	عائم update(عائم ننوت);
// accessors
	عائم getMin() { أرجع _min.get(); }
	عائم getMax() { أرجع _max.get(); }
محمي:
//ځانګړتیاوې
	control::بلاک_Param_Float _min;
	control::بلاک_Param_Float _max;
};

} // namespace control
