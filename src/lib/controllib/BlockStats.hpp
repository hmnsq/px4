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

#અન્તર્ભાવ <px4_platform_common/defines.h>
#અન્તર્ભાવ <assert.h>
#અન્તર્ભાવ <time.h>
#અન્તર્ભાવ <stdlib.h>
#અન્તર્ભાવ <math.h>
#અન્તર્ભાવ <mathlib/math/test/test.hpp>
#અન્તર્ભાવ <mathlib/math/filter/LowPassFilter2p.hpp>

#અન્તર્ભાવ "block/Block.hpp"
#અન્તર્ભાવ "block/BlockParam.hpp"

#અન્તર્ભાવ "matrix/math.hpp"

નામાકાશ control
{
template<વર્ગ Type, size_t M>
વર્ગ __EXPORT BlockStats: જાહેર Block
{

જાહેર:
// methods
	BlockStats(SuperBlock *parent,
		   અચલ અક્ષર *name) :
		Block(parent, name),
		_sum(),
		_sumSq(),
		ગણતરી_3(0)
	{}
	આભાસી ~BlockStats() = મૂળભૂત;
	શૂન્ય update(અચલ matrix::Vector<Type, M> &u)
	{
		_sum += u;
		_sumSq += u.emult(u);
		ગણતરી_3 += 1;
	}
	શૂન્ય reset()
	{
		_sum.setZero();
		_sumSq.setZero();
		ગણતરી_3 = 0;
	}
// accessors
	size_t get_ગણતરી() { પાછા ગણતરી_3; }
	matrix::Vector<Type, M> getMean() { પાછા _sum / ગણતરી_3; }
	matrix::Vector<Type, M> getVar()
	{
		પાછા (_sumSq - _sum.emult(_sum) / ગણતરી_3) / ગણતરી_3;
	}
	matrix::Vector<Type, M> getStdDev()
	{
		પાછા getVar().sqrt();
	}
ખાનગી:
// attributes
	matrix::Vector<Type, M> _sum;
	matrix::Vector<Type, M> _sumSq;
	size_t ગણતરી_3;
};

} // namespace control
