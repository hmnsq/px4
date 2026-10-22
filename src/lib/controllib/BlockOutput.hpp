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
 * @file BlockOutput.hpp
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
 * An output trim/ saturation block
 */
తరగతి __EXPORT ఖండం_Output: బహిరంగ Super_ఖండం
{
బహిరంగ:
// methods
	ఖండం_Output(Super_ఖండం *మాతృ, స్థిరరాశి అక్షరం *name) :
		Super_ఖండం(మాతృ, name),
		_trim(ఇది, "TRIM"),
		హద్దు(ఇది, ""),
		_val(0)
	{
		update(0);
	}

	మాయా ~ఖండం_Output() = అప్రమేయ;
	శూన్య update(దశాంశం ఆదానం)
	{
		_val = హద్దు.update(ఆదానం + getTrim());
	}
// accessors
	దశాంశం getMin() { తిరిగి హద్దు.getMin(); }
	దశాంశం getMax() { తిరిగి హద్దు.getMax(); }
	దశాంశం getTrim() { తిరిగి _trim.get(); }
	దశాంశం get() { తిరిగి _val; }
అంతరంగ:
//లక్షణాలు
	control::ఖండం_Param_Float _trim;
	ఖండం_హద్దు హద్దు;
	దశాంశం _val;
};

} // namespace control
