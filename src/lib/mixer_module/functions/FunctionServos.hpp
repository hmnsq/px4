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

#അന്തര്ഭാവ <uORB/topics/actuator_servos.h>

/**
 * Functions: Servo1 ... ServoMax
 */
വർഗ്ഗം FunctionServos : പൊതു FunctionProviderBase
{
പൊതു:
	static_assert(actuator_servos_s::അക്കം_CONTROLS == (പൂർണ്ണസംഖ്യ)OutputFunction::ServoMax - (പൂർണ്ണസംഖ്യ)OutputFunction::Servo1 + 1,
		      "Unexpected num servos");

	FunctionServos(സ്ഥിരം Context &context) :
		_topic(&context.work_item, ORB_ID(actuator_servos))
	{
		വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < actuator_servos_s::അക്കം_CONTROLS; ++i) {
			_data.control[i] = NAN;
		}
	}

	സ്ഥിതം FunctionProviderBase *allocate(സ്ഥിരം Context &context) { തിരികെ പുതിയ FunctionServos(context); }

	ശൂന്യം update() override { _topic.update(&_data); }
	ദശാംശം value(OutputFunction func) override { തിരികെ _data.control[(പൂർണ്ണസംഖ്യ)func - (പൂർണ്ണസംഖ്യ)OutputFunction::Servo1]; }

	uORB::SubscriptionCallbackWorkItem *subscriptionCallback() override { തിരികെ &_topic; }

	ദശാംശം default_Failsafe_മൂല്യം(OutputFunction func) സ്ഥിരം override { തിരികെ 0.f; }

സ്വകാര്യ:
	uORB::SubscriptionCallbackWorkItem _topic;
	actuator_servos_s _data{};
};
