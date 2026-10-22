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

#సూచన once

#అన్తర్భావ "FunctionProviderBase.hpp"

#అన్తర్భావ <uORB/topics/actuator_servos.h>

/**
 * Functions: Servo1 ... ServoMax
 */
తరగతి FunctionServos : బహిరంగ Function_Provider_పునాది
{
బహిరంగ:
	static_assert(actuator_servos_s::అంకె_CONTROLS == (పూర్ణాంకం)OutputFunction::ServoMax - (పూర్ణాంకం)OutputFunction::Servo1 + 1,
		      "Unexpected num servos");

	FunctionServos(స్థిరరాశి Context &context) :
		_topic(&context.work_అంశం, ORB_ID(actuator_servos))
	{
		కొరకు (పూర్ణాంకం i = 0; i < actuator_servos_s::అంకె_CONTROLS; ++i) {
			దత్తాంశం_3.control[i] = NAN;
		}
	}

	స్థిర Function_Provider_పునాది *allocate(స్థిరరాశి Context &context) { తిరిగి కొత్త FunctionServos(context); }

	శూన్య update() override { _topic.update(&దత్తాంశం_3); }
	దశాంశం value(OutputFunction func) override { తిరిగి దత్తాంశం_3.control[(పూర్ణాంకం)func - (పూర్ణాంకం)OutputFunction::Servo1]; }

	uORB::Subscription_Callback_Work_అంశం *subscriptionCallback() override { తిరిగి &_topic; }

	దశాంశం default_Failsafe_విలువ(OutputFunction func) స్థిరరాశి override { తిరిగి 0.f; }

అంతరంగ:
	uORB::Subscription_Callback_Work_అంశం _topic;
	actuator_servos_s దత్తాంశం_3{};
};
