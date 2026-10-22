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

#ଅନ୍ତର୍ଭାଵ <limits.h>

#ଅନ୍ତର୍ଭାଵ <mixer_module/output_functions.hpp>

#ଅନ୍ତର୍ଭାଵ <uORB/Subscription.hpp>
#ଅନ୍ତର୍ଭାଵ <uORB/SubscriptionCallback.hpp>

#ଅନ୍ତର୍ଭାଵ <px4_platform_common/px4_work_queue/ScheduledWorkItem.hpp>

ଶ୍ରେଣୀ FunctionProviderBase
{
ସର୍ବସାଧାରଣ:
	ସଂରଚନା Context {
		px4::WorkItem &work_item;
		ଅଚଳ ଦଶମିକ &thrust_factor;
	};

	FunctionProviderBase() = ପୂର୍ବନିର୍ଦ୍ଧାରିତ;
	ଆଭାସୀ ~FunctionProviderBase() = ପୂର୍ବନିର୍ଦ୍ଧାରିତ;

	ଆଭାସୀ ଶୂନ୍ୟ update() = 0;

	/**
	 * Get the current output value for a given function
	 * @return NAN (=disarmed) or value in range [-1, 1]
	 */
	ଆଭାସୀ ଦଶମିକ value(OutputFunction func) = 0;

	ଆଭାସୀ ଦଶମିକ default_Failsafe_ମୂଲ୍ୟ(OutputFunction func) ଅଚଳ { ଫେରନ୍ତୁ NAN; }
	ଆଭାସୀ ତାର୍କିକ allowPrearmControl() ଅଚଳ { ଫେରନ୍ତୁ ସତ୍ୟ; }

	ଆଭାସୀ uORB::SubscriptionCallbackWorkItem *subscriptionCallback() { ଫେରନ୍ତୁ nullptr; }

	ଆଭାସୀ ତାର୍କିକ getLatestSampleTimestamp(hrt_abstime &t) ଅଚଳ { ଫେରନ୍ତୁ ମିଥ୍ୟା; }

	/**
	 * Check whether the output (motor) is configured to be reversible
	 */
	ଆଭାସୀ ତାର୍କିକ reversible(OutputFunction func) ଅଚଳ { ଫେରନ୍ତୁ ମିଥ୍ୟା; }
};
