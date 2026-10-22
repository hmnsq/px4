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

#pragma once

#अन्तर्भाव <mathlib/mathlib.h>
#अन्तर्भाव <matrix/math.hpp>

ढाँचा<typename Type>
वर्ग SlewRate
{
सार्वजनिक:
	SlewRate() = स्वभाव;
	SlewRate(Type initial_मान) { set_Forced_मान(initial_मान); }
	~SlewRate() = स्वभाव;

	/**
	 * Set maximum rate of change for the value
	 * @param slew_rate maximum rate of change
	 */
	शून्य setSlewRate(अचर Type slew_rate) { _slew_rate = slew_rate; }

	/**
	 * Set value ignoring slew rate for initialization purpose
	 * @param value new applied value
	 */
	शून्य set_Forced_मान(अचर Type value) { मान_2 = value; }

	/**
	 * Get value from last update of the slew rate
	 * @return current value the slew rate is at
	 */
	Type getState() अचर { फर्काऊ मान_2; }

	/**
	 * Update slewrate
	 * @param new_value desired new value
	 * @param deltatime time in seconds since last update
	 * @return actual value that complies with the slew rate
	 */
	Type update(अचर Type new_मान, अचर भिन्न deltatime)
	{
		// Limit the rate of change of the value
		अचर Type dvalue_desired = new_मान - मान_2;
		अचर Type dvalue_max = _slew_rate * deltatime;
		अचर Type dvalue = math::constrain(dvalue_desired, -dvalue_max, dvalue_max);
		मान_2 += dvalue;
		फर्काऊ मान_2;
	}

रक्षित:
	Type _slew_rate{}; ///< maximum rate of change for the value
	Type मान_2{}; ///< state to keep last value of the slew rate
};
