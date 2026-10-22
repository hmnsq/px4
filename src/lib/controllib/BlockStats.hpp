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
		   स्थिरांक अक्षर *name) :
		Block(parent, name),
		_sum(),
		_sumSq(),
		मोजणी_3(0)
	{}
	आभासी ~BlockStats() = पूर्वनिर्धारित;
	शून्य update(स्थिरांक matrix::Vector<Type, M> &u)
	{
		_sum += u;
		_sumSq += u.emult(u);
		मोजणी_3 += 1;
	}
	शून्य reset()
	{
		_sum.setZero();
		_sumSq.setZero();
		मोजणी_3 = 0;
	}
// accessors
	माप_प्रकार get_मोजणी() { परत मोजणी_3; }
	matrix::Vector<Type, M> getMean() { परत _sum / मोजणी_3; }
	matrix::Vector<Type, M> getVar()
	{
		परत (_sumSq - _sum.emult(_sum) / मोजणी_3) / मोजणी_3;
	}
	matrix::Vector<Type, M> getStdDev()
	{
		परत getVar().वर्ग_मूल();
	}
खाजगी:
// attributes
	matrix::Vector<Type, M> _sum;
	matrix::Vector<Type, M> _sumSq;
	माप_प्रकार मोजणी_3;
};

} // namespace control
