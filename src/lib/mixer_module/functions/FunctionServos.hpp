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

#अन्तर्भाव "FunctionProviderBase.hpp"

#अन्तर्भाव <uORB/topics/actuator_servos.h>

/**
 * Functions: Servo1 ... ServoMax
 */
वर्ग FunctionServos : सार्वजनिक FunctionProviderBase
{
सार्वजनिक:
	static_assert(actuator_servos_s::अंक_CONTROLS == (पूर्णाङ्क)OutputFunction::ServoMax - (पूर्णाङ्क)OutputFunction::Servo1 + 1,
		      "Unexpected num servos");

	FunctionServos(ध्रुव Context &context) :
		_topic(&context.work_item, ORB_ID(actuator_servos))
	{
		प्रति (पूर्णाङ्क i = 0; i < actuator_servos_s::अंक_CONTROLS; ++i) {
			_data.control[i] = न_अंक;
		}
	}

	स्थिर FunctionProviderBase *allocate(ध्रुव Context &context) { प्रत्यागम नूतन FunctionServos(context); }

	शून्य update() override { _topic.update(&_data); }
	भिन्न value(OutputFunction func) override { प्रत्यागम _data.control[(पूर्णाङ्क)func - (पूर्णाङ्क)OutputFunction::Servo1]; }

	uORB::SubscriptionCallbackWorkItem *subscriptionCallback() override { प्रत्यागम &_topic; }

	भिन्न default_Failsafe_मूल्य(OutputFunction func) ध्रुव override { प्रत्यागम 0.f; }

गुप्त:
	uORB::SubscriptionCallbackWorkItem _topic;
	actuator_servos_s _data{};
};
