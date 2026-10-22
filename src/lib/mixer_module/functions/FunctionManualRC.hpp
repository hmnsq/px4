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

#അന്തര്ഭാവ <uORB/topics/manual_control_setpoint.h>

/**
 * Functions: RC_Roll .. RCAUX_Max
 */
വർഗ്ഗം FunctionManualRC : പൊതു FunctionProviderBase
{
പൊതു:
	FunctionManualRC()
	{
		reset_All_To_Disarmed_മൂല്യം();
	}

	സ്ഥിതം FunctionProviderBase *allocate(സ്ഥിരം Context &context) { തിരികെ പുതിയ FunctionManualRC(); }

	ശൂന്യം update() override
	{
		manual_control_setpoint_s manual_control_setpoint;

		എങ്കിൽ (_topic.update(&manual_control_setpoint)) {
			എങ്കിൽ (manual_control_setpoint.valid) {
				_data[0] = manual_control_setpoint.roll;
				_data[1] = manual_control_setpoint.pitch;
				_data[2] = manual_control_setpoint.throttle;
				_data[3] = manual_control_setpoint.yaw;
				_data[4] = manual_control_setpoint.flaps;
				_data[5] = manual_control_setpoint.aux1;
				_data[6] = manual_control_setpoint.aux2;
				_data[7] = manual_control_setpoint.aux3;
				_data[8] = manual_control_setpoint.aux4;
				_data[9] = manual_control_setpoint.aux5;
				_data[10] = manual_control_setpoint.aux6;

			} അല്ലെങ്കിൽ {
				reset_All_To_Disarmed_മൂല്യം();
			}
		}
	}

	ദശാംശം value(OutputFunction func) override { തിരികെ _data[(പൂർണ്ണസംഖ്യ)func - (പൂർണ്ണസംഖ്യ)OutputFunction::RC_Roll]; }

സ്വകാര്യ:
	സ്ഥിതം constexpr പൂർണ്ണസംഖ്യ അക്കം_data_points = 11;

	ശൂന്യം reset_All_To_Disarmed_മൂല്യം()
	{
		വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < അക്കം_data_points; ++i) {
			_data[i] = NAN;
		}
	}

	static_assert(അക്കം_data_points == (പൂർണ്ണസംഖ്യ)OutputFunction::RC_AUXMax - (പൂർണ്ണസംഖ്യ)OutputFunction::RC_Roll + 1,
		      "number of functions mismatch");

	uORB::Subscription _topic{ORB_ID(manual_control_setpoint)};
	ദശാംശം _data[അക്കം_data_points] {};
};
