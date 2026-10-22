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
 * @file BlockStats.hpp
 *
 * Controller library code
 */

#pragma once

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
template<صنف Type, size_t M>
صنف __EXPORT BlockStats: عام Block
{

عام:
// methods
	BlockStats(SuperBlock *parent,
		   ثابت حرف *name) :
		Block(parent, name),
		_sum(),
		_sumSq(),
		تعداد_4(0)
	{}
	virtual ~BlockStats() = افتراضي;
	فراغ update(ثابت matrix::Vector<Type, M> &u)
	{
		_sum += u;
		_sumSq += u.emult(u);
		تعداد_4 += 1;
	}
	فراغ reset()
	{
		_sum.setZero();
		_sumSq.setZero();
		تعداد_4 = 0;
	}
// accessors
	size_t get_تعداد() { إرجاع تعداد_4; }
	matrix::Vector<Type, M> getMean() { إرجاع _sum / تعداد_4; }
	matrix::Vector<Type, M> getVar()
	{
		إرجاع (_sumSq - _sum.emult(_sum) / تعداد_4) / تعداد_4;
	}
	matrix::Vector<Type, M> getStdDev()
	{
		إرجاع getVar().sqrt();
	}
خاص:
// attributes
	matrix::Vector<Type, M> _sum;
	matrix::Vector<Type, M> _sumSq;
	size_t تعداد_4;
};

} // namespace control
