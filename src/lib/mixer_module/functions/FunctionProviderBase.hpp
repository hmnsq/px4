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

#ضمن <limits.h>

#ضمن <mixer_module/output_functions.hpp>

#ضمن <uORB/Subscription.hpp>
#ضمن <uORB/SubscriptionCallback.hpp>

#ضمن <px4_platform_common/px4_work_queue/ScheduledWorkItem.hpp>

صنف FunctionProviderBase
{
عام:
	بنية Context {
		px4::WorkItem &work_item;
		ثابت عائم &thrust_factor;
	};

	FunctionProviderBase() = افتراضي;
	virtual ~FunctionProviderBase() = افتراضي;

	virtual فراغ update() = 0;

	/**
	 * Get the current output value for a given function
	 * @return NAN (=disarmed) or value in range [-1, 1]
	 */
	virtual عائم value(OutputFunction func) = 0;

	virtual عائم defaultFailsafeValue(OutputFunction func) ثابت { أرجع NAN; }
	virtual منطقي allowPrearmControl() ثابت { أرجع true; }

	virtual uORB::SubscriptionCallbackWorkItem *subscriptionCallback() { أرجع nullptr; }

	virtual منطقي getLatestSampleTimestamp(hrt_abstime &t) ثابت { أرجع خطأ; }

	/**
	 * Check whether the output (motor) is configured to be reversible
	 */
	virtual منطقي reversible(OutputFunction func) ثابت { أرجع خطأ; }
};
