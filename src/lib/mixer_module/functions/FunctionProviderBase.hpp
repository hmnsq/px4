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

#અન્તર્ભાવ <limits.h>

#અન્તર્ભાવ <mixer_module/output_functions.hpp>

#અન્તર્ભાવ <uORB/Subscription.hpp>
#અન્તર્ભાવ <uORB/SubscriptionCallback.hpp>

#અન્તર્ભાવ <px4_platform_common/px4_work_queue/ScheduledWorkItem.hpp>

વર્ગ FunctionProviderBase
{
જાહેર:
	સંરચના Context {
		px4::WorkItem &work_item;
		અચલ દશાંશ &thrust_factor;
	};

	FunctionProviderBase() = મૂળભૂત;
	આભાસી ~FunctionProviderBase() = મૂળભૂત;

	આભાસી શૂન્ય update() = 0;

	/**
	 * Get the current output value for a given function
	 * @return NAN (=disarmed) or value in range [-1, 1]
	 */
	આભાસી દશાંશ value(OutputFunction func) = 0;

	આભાસી દશાંશ default_Failsafe_કિંમત(OutputFunction func) અચલ { પાછા NAN; }
	આભાસી તાર્કિક allowPrearmControl() અચલ { પાછા સાચું; }

	આભાસી uORB::SubscriptionCallbackWorkItem *subscriptionCallback() { પાછા nullptr; }

	આભાસી તાર્કિક getLatestSampleTimestamp(hrt_abstime &t) અચલ { પાછા ખોટું; }

	/**
	 * Check whether the output (motor) is configured to be reversible
	 */
	આભાસી તાર્કિક reversible(OutputFunction func) અચલ { પાછા ખોટું; }
};
