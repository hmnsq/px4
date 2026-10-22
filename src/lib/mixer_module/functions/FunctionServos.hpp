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

#ಅನ್ತರ್ಭಾವ <uORB/topics/actuator_servos.h>

/**
 * Functions: Servo1 ... ServoMax
 */
ವರ್ಗ FunctionServos : ಸಾರ್ವಜನಿಕ FunctionProviderBase
{
ಸಾರ್ವಜನಿಕ:
	static_assert(actuator_servos_s::ಅಂಕೆ_CONTROLS == (ಪೂರ್ಣಾಙ್ಕ)OutputFunction::ServoMax - (ಪೂರ್ಣಾಙ್ಕ)OutputFunction::Servo1 + 1,
		      "Unexpected num servos");

	FunctionServos(ಧ್ರುವ Context &context) :
		_topic(&context.work_item, ORB_ID(actuator_servos))
	{
		ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < actuator_servos_s::ಅಂಕೆ_CONTROLS; ++i) {
			_data.control[i] = NAN;
		}
	}

	ಸ್ಥಿರ FunctionProviderBase *allocate(ಧ್ರುವ Context &context) { ಪ್ರತ್ಯಾಗಮ ನೂತನ FunctionServos(context); }

	ಶೂನ್ಯ update() override { _topic.update(&_data); }
	ಭಿನ್ನ value(OutputFunction func) override { ಪ್ರತ್ಯಾಗಮ _data.control[(ಪೂರ್ಣಾಙ್ಕ)func - (ಪೂರ್ಣಾಙ್ಕ)OutputFunction::Servo1]; }

	uORB::SubscriptionCallbackWorkItem *subscriptionCallback() override { ಪ್ರತ್ಯಾಗಮ &_topic; }

	ಭಿನ್ನ default_Failsafe_ಮೌಲ್ಯ(OutputFunction func) ಧ್ರುವ override { ಪ್ರತ್ಯಾಗಮ 0.f; }

ಗುಪ್ತ:
	uORB::SubscriptionCallbackWorkItem _topic;
	actuator_servos_s _data{};
};
