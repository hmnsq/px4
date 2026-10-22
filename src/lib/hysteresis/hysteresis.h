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
 * @file hysteresis.h
 *
 * Hysteresis of a boolean value.
 *
 * @author Julian Oes <julian@oes.ch>
 */

#رهنمود once

#ضمن <drivers/drv_hrt.h>

namespace systemlib
{

class Hysteresis
{
عام:
	Hysteresis() = افتراضي;
	explicit Hysteresis(bool initial_state);
	~Hysteresis() = افتراضي;

	bool get_state() ثابت { أرجع _state; }

	فراغ مجموعه_hysteresis_زمان_from(ثابت bool from_state, ثابت hrt_abstime new_hysteresis_زمان_us);

	فراغ مجموعه_state_and_update(ثابت bool new_state, ثابت hrt_abstime &now_us);

	فراغ update(ثابت hrt_abstime &now_us);

خاص:

	hrt_abstime آخرین_زمان_مقصد_change_state = 0;

	hrt_abstime زمان_from_true_us = 0;
	hrt_abstime زمان_from_false_us = 0;

	bool _state = false;
	bool _requested_state = false;
};

} // namespace systemlib
