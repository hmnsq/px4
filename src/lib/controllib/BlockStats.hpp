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

#ಅನ್ತರ್ಭಾವ <px4_platform_common/defines.h>
#ಅನ್ತರ್ಭಾವ <assert.h>
#ಅನ್ತರ್ಭಾವ <time.h>
#ಅನ್ತರ್ಭಾವ <stdlib.h>
#ಅನ್ತರ್ಭಾವ <math.h>
#ಅನ್ತರ್ಭಾವ <mathlib/math/test/test.hpp>
#ಅನ್ತರ್ಭಾವ <mathlib/math/filter/LowPassFilter2p.hpp>

#ಅನ್ತರ್ಭಾವ "block/Block.hpp"
#ಅನ್ತರ್ಭಾವ "block/BlockParam.hpp"

#ಅನ್ತರ್ಭಾವ "matrix/math.hpp"

ನಾಮಾಕಾಶ control
{
template<ವರ್ಗ Type, size_t M>
ವರ್ಗ __EXPORT BlockStats: ಸಾರ್ವಜನಿಕ Block
{

ಸಾರ್ವಜನಿಕ:
// methods
	BlockStats(SuperBlock *parent,
		   ಧ್ರುವ ವರ್ಣ *name) :
		Block(parent, name),
		_sum(),
		_sumSq(),
		ಎಣಿಕೆ_3(0)
	{}
	ಆಭಾಸ ~BlockStats() = ಸ್ವಭಾವ;
	ಶೂನ್ಯ update(ಧ್ರುವ matrix::Vector<Type, M> &u)
	{
		_sum += u;
		_sumSq += u.emult(u);
		ಎಣಿಕೆ_3 += 1;
	}
	ಶೂನ್ಯ reset()
	{
		_sum.setZero();
		_sumSq.setZero();
		ಎಣಿಕೆ_3 = 0;
	}
// accessors
	size_t get_ಎಣಿಕೆ() { ಪ್ರತ್ಯಾಗಮ ಎಣಿಕೆ_3; }
	matrix::Vector<Type, M> getMean() { ಪ್ರತ್ಯಾಗಮ _sum / ಎಣಿಕೆ_3; }
	matrix::Vector<Type, M> getVar()
	{
		ಪ್ರತ್ಯಾಗಮ (_sumSq - _sum.emult(_sum) / ಎಣಿಕೆ_3) / ಎಣಿಕೆ_3;
	}
	matrix::Vector<Type, M> getStdDev()
	{
		ಪ್ರತ್ಯಾಗಮ getVar().sqrt();
	}
ಗುಪ್ತ:
// attributes
	matrix::Vector<Type, M> _sum;
	matrix::Vector<Type, M> _sumSq;
	size_t ಎಣಿಕೆ_3;
};

} // namespace control
