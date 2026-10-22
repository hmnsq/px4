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

#अन्तर्भाव <px4_platform_common/defines.h>
#अन्तर्भाव <assert.h>
#अन्तर्भाव <time.h>
#अन्तर्भाव <stdlib.h>
#अन्तर्भाव <math.h>
#अन्तर्भाव <mathlib/math/test/test.hpp>
#अन्तर्भाव <mathlib/math/filter/LowPassFilter2p.hpp>

#अन्तर्भाव "block/Block.hpp"
#अन्तर्भाव "block/BlockParam.hpp"

#अन्तर्भाव "matrix/math.hpp"

नामाकाश control
{
ढाँचा<वर्ग Type, माप_प्रकार M>
वर्ग __EXPORT BlockStats: सार्वजनिक Block
{

सार्वजनिक:
// methods
	BlockStats(SuperBlock *parent,
		   ध्रुव वर्ण *name) :
		Block(parent, name),
		_sum(),
		_sumSq(),
		गिनती_3(0)
	{}
	आभास ~BlockStats() = स्वभाव;
	शून्य update(ध्रुव matrix::Vector<Type, M> &u)
	{
		_sum += u;
		_sumSq += u.emult(u);
		गिनती_3 += 1;
	}
	शून्य reset()
	{
		_sum.setZero();
		_sumSq.setZero();
		गिनती_3 = 0;
	}
// accessors
	माप_प्रकार get_गिनती() { प्रत्यागम गिनती_3; }
	matrix::Vector<Type, M> getMean() { प्रत्यागम _sum / गिनती_3; }
	matrix::Vector<Type, M> getVar()
	{
		प्रत्यागम (_sumSq - _sum.emult(_sum) / गिनती_3) / गिनती_3;
	}
	matrix::Vector<Type, M> getStdDev()
	{
		प्रत्यागम getVar().वर्ग_मूल();
	}
गुप्त:
// attributes
	matrix::Vector<Type, M> _sum;
	matrix::Vector<Type, M> _sumSq;
	माप_प्रकार गिनती_3;
};

} // namespace control
