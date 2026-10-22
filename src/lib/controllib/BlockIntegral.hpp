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

#సూచన once

#అన్తర్భావ "BlockLimitSym.hpp"

#అన్తర్భావ "block/Block.hpp"
#అన్తర్భావ "block/BlockParam.hpp"

#అన్తర్భావ "matrix/math.hpp"

నామాకాశ control
{

/**
 * A rectangular integrator.
 * A limiter is built into the class to bound the
 * integral's internal state. This is important
 * for windup protection.
 * @see Limit
 */
తరగతి __EXPORT ఖండం_Integral: బహిరంగ Super_ఖండం
{
బహిరంగ:
// methods
	ఖండం_Integral(Super_ఖండం *మాతృ, స్థిరరాశి అక్షరం *name) :
		Super_ఖండం(మాతృ, name),
		_y(0),
		హద్దు(ఇది, "") {}
	మాయా ~ఖండం_Integral() = అప్రమేయ;
	దశాంశం update(దశాంశం ఆదానం);
// accessors
	దశాంశం getY() {తిరిగి _y;}
	దశాంశం getMax() {తిరిగి హద్దు.getMax();}
	శూన్య సమితి_Y(దశాంశం y) {_y = y;}
రక్షిత:
//లక్షణాలు
	దశాంశం _y; /**< previous output */
	ఖండం_హద్దు_Sym హద్దు; /**< limiter */
};

} // namespace control
