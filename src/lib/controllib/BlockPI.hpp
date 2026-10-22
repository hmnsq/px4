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
 * @file BlockPI.hpp
 *
 * Controller library code
 */

#సూచన once

#అన్తర్భావ <px4_platform_common/defines.h>
#అన్తర్భావ <assert.h>
#అన్తర్భావ <time.h>
#అన్తర్భావ <stdlib.h>
#అన్తర్భావ <math.h>
#అన్తర్భావ <mathlib/math/test/test.hpp>
#అన్తర్భావ <mathlib/math/filter/LowPassFilter2p.hpp>

#అన్తర్భావ "block/Block.hpp"
#అన్తర్భావ "block/BlockParam.hpp"

#అన్తర్భావ "matrix/math.hpp"

నామాకాశ control
{

/**
 * A proportional-integral controller.
 * @link http://en.wikipedia.org/wiki/PID_controller
 */
తరగతి __EXPORT ఖండం_PI: బహిరంగ Super_ఖండం
{
బహిరంగ:
// methods
	ఖండం_PI(Super_ఖండం *మాతృ, స్థిరరాశి అక్షరం *name) :
		Super_ఖండం(మాతృ, name),
		_integral(ఇది, "I"),
		_kP(ఇది, "P"),
		_kI(ఇది, "I")
	{}
	మాయా ~ఖండం_PI() = అప్రమేయ;
	దశాంశం update(దశాంశం ఆదానం)
	{
		తిరిగి getKP() * ఆదానం +
		       getKI() * getIntegral().update(ఆదానం);
	}
// accessors
	దశాంశం getKP() { తిరిగి _kP.get(); }
	దశాంశం getKI() { తిరిగి _kI.get(); }
	ఖండం_Integral &getIntegral() { తిరిగి _integral; }
అంతరంగ:
	ఖండం_Integral _integral;
	control::ఖండం_Param_Float _kP;
	control::ఖండం_Param_Float _kI;
};

} // namespace control
