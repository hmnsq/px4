/****************************************************************************
 *
 *   Copyright (c) 2021-2022 PX4 Development Team. All rights reserved.
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

#pragma once

#അന്തര്ഭാവ "FunctionProviderBase.hpp"

#അന്തര്ഭാവ <uORB/topics/gimbal_controls.h>

/**
 * Functions: Gimbal_Roll .. Gimbal_Yaw
 */
വർഗ്ഗം FunctionGimbal : പൊതു FunctionProviderBase
{
പൊതു:
	FunctionGimbal() = സ്ഥിരസ്ഥിതി;
	സ്ഥിതം FunctionProviderBase *allocate(സ്ഥിരം Context &context) { തിരികെ പുതിയ FunctionGimbal(); }

	ശൂന്യം update() override
	{
		gimbal_controls_s gimbal_controls;

		എങ്കിൽ (_topic.update(&gimbal_controls)) {
			_data[0] = gimbal_controls.control[gimbal_controls_s::INDEX_ROLL];
			_data[1] = gimbal_controls.control[gimbal_controls_s::INDEX_PITCH];
			_data[2] = gimbal_controls.control[gimbal_controls_s::INDEX_YAW];
		}
	}

	ദശാംശം value(OutputFunction func) override { തിരികെ _data[(പൂർണ്ണസംഖ്യ)func - (പൂർണ്ണസംഖ്യ)OutputFunction::Gimbal_Roll]; }

സ്വകാര്യ:
	uORB::Subscription _topic{ORB_ID(gimbal_controls)};
	ദശാംശം _data[3] { NAN, NAN, NAN };
};
