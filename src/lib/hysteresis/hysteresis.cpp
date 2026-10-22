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

#شامل "hysteresis.h"

فضاینام systemlib
{

Hysteresis::Hysteresis(منطقی initial_state)
{
	_state = _requested_state = initial_state;
}

تهی Hysteresis::set_hysteresis_time_from(ثابت منطقی from_state, ثابت hrt_abstime new_hysteresis_time_us)
{
	اگر (from_state) {
		_time_from_true_us = new_hysteresis_time_us;

	} وگرنه {
		_time_from_false_us = new_hysteresis_time_us;
	}
}

تهی Hysteresis::set_state_and_update(ثابت منطقی new_state, ثابت hrt_abstime &now_us)
{
	اگر (new_state != _state) {
		اگر (new_state != _requested_state) {
			_requested_state = new_state;
			_last_time_to_change_state = now_us;
		}

	} وگرنه {
		_requested_state = _state;
	}

	update(now_us);
}

تهی Hysteresis::update(ثابت hrt_abstime &now_us)
{
	اگر (_requested_state != _state) {

		اگر (_state && !_requested_state) {
			// true -> false
			اگر (now_us >= _last_time_to_change_state + _time_from_true_us) {
				_state = نادرست;
			}

		} وگرنه اگر (!_state && _requested_state) {
			// false -> true
			اگر (now_us >= _last_time_to_change_state + _time_from_false_us) {
				_state = درست;
			}
		}
	}
}

} // namespace systemlib
