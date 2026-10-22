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
 * @file BlockDelay.hpp
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

template<صنف Type, size_t M, size_t N, size_t LEN>
صنف __EXPORT BlockDelay: عام Block
{
عام:
// methods
	BlockDelay(SuperBlock *parent, ثابت حرف *name) :
		Block(parent, name),
		_h(),
		_index(0),
		_delay(-1)
	{}
	virtual ~BlockDelay() = افتراضي;
	matrix::Matrix<Type, M, N> update(ثابت matrix::Matrix<Type, M, N> &u)
	{
		// store current value
		_h[_index] = u;

		// delay starts at zero, then increases to LEN
		_delay += 1;

		ܐܢ (_delay > (صحيح)(LEN - 1)) {
			_delay = LEN - 1;
		}

		// compute position of delayed value
		صحيح j = _index - _delay;

		ܐܢ (j < 0) {
			j += LEN;
		}

		// increment storage position
		_index += 1;

		ܐܢ (_index > (LEN - 1)) {
			_index  = 0;
		}

		// get delayed value
		ܦܢܐ _h[j];
	}
	matrix::Matrix<Type, M, N> get(size_t delay)
	{
		صحيح j = _index - delay;

		ܐܢ (j < 0) { j += LEN; }

		ܦܢܐ _h[j];
	}
خاص:
// attributes
	matrix::Matrix<Type, M, N> _h[LEN];
	size_t _index;
	صحيح _delay;
};

} // namespace control
