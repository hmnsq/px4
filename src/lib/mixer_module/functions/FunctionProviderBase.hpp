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

#अन्तर्भाव <limits.h>

#अन्तर्भाव <mixer_module/output_functions.hpp>

#अन्तर्भाव <uORB/Subscription.hpp>
#अन्तर्भाव <uORB/SubscriptionCallback.hpp>

#अन्तर्भाव <px4_platform_common/px4_work_queue/ScheduledWorkItem.hpp>

वर्ग FunctionProviderBase
{
सार्वजनिक:
	संरचना Context {
		px4::WorkItem &work_item;
		अचर भिन्न &thrust_factor;
	};

	FunctionProviderBase() = स्वभाव;
	आभास ~FunctionProviderBase() = स्वभाव;

	आभास शून्य update() = 0;

	/**
	 * Get the current output value for a given function
	 * @return NAN (=disarmed) or value in range [-1, 1]
	 */
	आभास भिन्न value(OutputFunction func) = 0;

	आभास भिन्न default_Failsafe_मान(OutputFunction func) अचर { फर्काऊ न_अंक; }
	आभास तार्किक allowPrearmControl() अचर { फर्काऊ सत्य; }

	आभास uORB::SubscriptionCallbackWorkItem *subscriptionCallback() { फर्काऊ nullptr; }

	आभास तार्किक getLatestSampleTimestamp(hrt_abstime &t) अचर { फर्काऊ असत्य; }

	/**
	 * Check whether the output (motor) is configured to be reversible
	 */
	आभास तार्किक reversible(OutputFunction func) अचर { फर्काऊ असत्य; }
};
