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
 * @file BlockIntegral.hpp
 *
 * Controller library code
 */

#لارښوونه once

#ضمن "BlockLimitSym.hpp"

#ضمن "block/Block.hpp"
#ضمن "block/BlockParam.hpp"

#ضمن "matrix/math.hpp"

نطاق control
{

/**
 * A rectangular integrator.
 * A limiter is built into the class to bound the
 * integral's internal state. This is important
 * for windup protection.
 * @see Limit
 */
صنف __EXPORT بلاک_Integral: عام Super_بلاک
{
عام:
// methods
	بلاک_Integral(Super_بلاک *پلار, ثابت حرف *name) :
		Super_بلاک(پلار, name),
		_y(0),
		حد(هذا, "") {}
	مجازي ~بلاک_Integral() = افتراضي;
	عائم update(عائم ننوت);
// accessors
	عائم getY() {أرجع _y;}
	عائم getMax() {أرجع حد.getMax();}
	فراغ سېټ_Y(عائم y) {_y = y;}
محمي:
//ځانګړتیاوې
	عائم _y; /**< previous output */
	بلاک_حد_Sym حد; /**< limiter */
};

} // namespace control
