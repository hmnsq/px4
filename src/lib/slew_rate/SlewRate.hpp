/****************************************************************************
 *
 *   Copyright (c) 2020 PX4 Development Team. All rights reserved.
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
 * @file SlewRate.hpp
 *
 * Library limit the rate of change of a value with a maximum slew rate.
 *
 * @author Matthias Grob <maetugr@gmail.com>
 */

#సూచన once

#అన్తర్భావ <mathlib/mathlib.h>
#అన్తర్భావ <matrix/math.hpp>

మూస<రకం_పేరు Type>
తరగతి Slew_రేటు
{
బహిరంగ:
	Slew_రేటు() = అప్రమేయ;
	Slew_రేటు(Type initial_విలువ) { సమితి_Forced_విలువ(initial_విలువ); }
	~Slew_రేటు() = అప్రమేయ;

	/**
	 * Set maximum rate of change for the value
	 * @param slew_rate maximum rate of change
	 */
	శూన్య సమితి_Slew_రేటు(స్థిరరాశి Type slew_రేటు) { slew_రేటు_2 = slew_రేటు; }

	/**
	 * Set value ignoring slew rate for initialization purpose
	 * @param value new applied value
	 */
	శూన్య సమితి_Forced_విలువ(స్థిరరాశి Type value) { విలువ_2 = value; }

	/**
	 * Get value from last update of the slew rate
	 * @return current value the slew rate is at
	 */
	Type getState() స్థిరరాశి { తిరిగి విలువ_2; }

	/**
	 * Update slewrate
	 * @param new_value desired new value
	 * @param deltatime time in seconds since last update
	 * @return actual value that complies with the slew rate
	 */
	Type update(స్థిరరాశి Type new_విలువ, స్థిరరాశి దశాంశం deltatime)
	{
		// Limit the rate of change of the value
		స్థిరరాశి Type dvalue_desired = new_విలువ - విలువ_2;
		స్థిరరాశి Type dvalue_max = slew_రేటు_2 * deltatime;
		స్థిరరాశి Type dvalue = math::constrain(dvalue_desired, -dvalue_max, dvalue_max);
		విలువ_2 += dvalue;
		తిరిగి విలువ_2;
	}

రక్షిత:
	Type slew_రేటు_2{}; ///< maximum rate of change for the value
	Type విలువ_2{}; ///< state to keep last value of the slew rate
};
