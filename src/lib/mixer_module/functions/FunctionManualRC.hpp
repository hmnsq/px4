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

#ಅನ್ತರ್ಭಾವ "FunctionProviderBase.hpp"

#ಅನ್ತರ್ಭಾವ <uORB/topics/manual_control_setpoint.h>

/**
 * Functions: RC_Roll .. RCAUX_Max
 */
ವರ್ಗ FunctionManualRC : ಸಾರ್ವಜನಿಕ FunctionProviderBase
{
ಸಾರ್ವಜನಿಕ:
	FunctionManualRC()
	{
		reset_All_To_Disarmed_ಮೌಲ್ಯ();
	}

	ಸ್ಥಿರ FunctionProviderBase *allocate(ಧ್ರುವ Context &context) { ಪ್ರತ್ಯಾಗಮ ನೂತನ FunctionManualRC(); }

	ಶೂನ್ಯ update() override
	{
		manual_control_setpoint_s manual_control_setpoint;

		ಯದಿ (_topic.update(&manual_control_setpoint)) {
			ಯದಿ (manual_control_setpoint.valid) {
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

			} ಅನ್ಯಥಾ {
				reset_All_To_Disarmed_ಮೌಲ್ಯ();
			}
		}
	}

	ಭಿನ್ನ value(OutputFunction func) override { ಪ್ರತ್ಯಾಗಮ _data[(ಪೂರ್ಣಾಙ್ಕ)func - (ಪೂರ್ಣಾಙ್ಕ)OutputFunction::RC_Roll]; }

ಗುಪ್ತ:
	ಸ್ಥಿರ constexpr ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ_data_points = 11;

	ಶೂನ್ಯ reset_All_To_Disarmed_ಮೌಲ್ಯ()
	{
		ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < ಅಂಕೆ_data_points; ++i) {
			_data[i] = NAN;
		}
	}

	static_assert(ಅಂಕೆ_data_points == (ಪೂರ್ಣಾಙ್ಕ)OutputFunction::RC_AUXMax - (ಪೂರ್ಣಾಙ್ಕ)OutputFunction::RC_Roll + 1,
		      "number of functions mismatch");

	uORB::Subscription _topic{ORB_ID(manual_control_setpoint)};
	ಭಿನ್ನ _data[ಅಂಕೆ_data_points] {};
};
