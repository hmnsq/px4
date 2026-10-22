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

#അന്തര്ഭാവ <limits.h>

#അന്തര്ഭാവ <mixer_module/output_functions.hpp>

#അന്തര്ഭാവ <uORB/Subscription.hpp>
#അന്തര്ഭാവ <uORB/SubscriptionCallback.hpp>

#അന്തര്ഭാവ <px4_platform_common/px4_work_queue/ScheduledWorkItem.hpp>

വർഗ്ഗം FunctionProviderBase
{
പൊതു:
	ഘടന Context {
		px4::WorkItem &work_item;
		സ്ഥിരം ദശാംശം &thrust_factor;
	};

	FunctionProviderBase() = സ്ഥിരസ്ഥിതി;
	മായാ ~FunctionProviderBase() = സ്ഥിരസ്ഥിതി;

	മായാ ശൂന്യം update() = 0;

	/**
	 * Get the current output value for a given function
	 * @return NAN (=disarmed) or value in range [-1, 1]
	 */
	മായാ ദശാംശം value(OutputFunction func) = 0;

	മായാ ദശാംശം default_Failsafe_മൂല്യം(OutputFunction func) സ്ഥിരം { തിരികെ NAN; }
	മായാ തർക്കം allowPrearmControl() സ്ഥിരം { തിരികെ സത്യം; }

	മായാ uORB::SubscriptionCallbackWorkItem *subscriptionCallback() { തിരികെ nullptr; }

	മായാ തർക്കം getLatestSampleTimestamp(hrt_abstime &t) സ്ഥിരം { തിരികെ അസത്യം; }

	/**
	 * Check whether the output (motor) is configured to be reversible
	 */
	മായാ തർക്കം reversible(OutputFunction func) സ്ഥിരം { തിരികെ അസത്യം; }
};
