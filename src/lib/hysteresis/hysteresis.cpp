/****************************************************************************
 *
 *   Copyright (c) 2016-2019 PX4 Development Team. All rights reserved.
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
 * @file hysteresis.cpp
 *
 * @author Julian Oes <julian@oes.ch>
 */

#అన్తర్భావ "hysteresis.h"

నామాకాశ systemlib
{

Hysteresis::Hysteresis(తార్కిక initial_state)
{
	_state = _requested_state = initial_state;
}

శూన్య Hysteresis::సమితి_hysteresis_సమయం_from(స్థిరరాశి తార్కిక from_state, స్థిరరాశి hrt_abstime new_hysteresis_సమయం_us)
{
	ఒకవేళ (from_state) {
		సమయం_from_true_us = new_hysteresis_సమయం_us;

	} లేకపోతే {
		సమయం_from_false_us = new_hysteresis_సమయం_us;
	}
}

శూన్య Hysteresis::సమితి_state_and_update(స్థిరరాశి తార్కిక new_state, స్థిరరాశి hrt_abstime &now_us)
{
	ఒకవేళ (new_state != _state) {
		ఒకవేళ (new_state != _requested_state) {
			_requested_state = new_state;
			చివరి_సమయం_గమ్యం_change_state = now_us;
		}

	} లేకపోతే {
		_requested_state = _state;
	}

	update(now_us);
}

శూన్య Hysteresis::update(స్థిరరాశి hrt_abstime &now_us)
{
	ఒకవేళ (_requested_state != _state) {

		ఒకవేళ (_state && !_requested_state) {
			// true -> false
			ఒకవేళ (now_us >= చివరి_సమయం_గమ్యం_change_state + సమయం_from_true_us) {
				_state = అబద్ధం;
			}

		} లేకపోతే ఒకవేళ (!_state && _requested_state) {
			// false -> true
			ఒకవేళ (now_us >= చివరి_సమయం_గమ్యం_change_state + సమయం_from_false_us) {
				_state = నిజం;
			}
		}
	}
}

} // namespace systemlib
