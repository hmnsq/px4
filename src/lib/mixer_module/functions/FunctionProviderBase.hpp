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

#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ <limits.h>

#ਅਨ੍ਤਰ੍ਭਾਵ <mixer_module/output_functions.hpp>

#ਅਨ੍ਤਰ੍ਭਾਵ <uORB/Subscription.hpp>
#ਅਨ੍ਤਰ੍ਭਾਵ <uORB/SubscriptionCallback.hpp>

#ਅਨ੍ਤਰ੍ਭਾਵ <px4_platform_common/px4_work_queue/ScheduledWorkItem.hpp>

ਵਰਗ Function_Provider_ਅਧਾਰ
{
ਜਨਤਕ:
	ਬਣਤਰ Context {
		px4::Work_ਵਸਤੂ &work_ਵਸਤੂ;
		ਅਚਲ ਦਸ਼ਮਲਵ &thrust_ਗੁਣਕ_2;
	};

	Function_Provider_ਅਧਾਰ() = ਮੂਲ;
	ਆਭਾਸੀ ~Function_Provider_ਅਧਾਰ() = ਮੂਲ;

	ਆਭਾਸੀ ਖਾਲੀ update() = 0;

	/**
	 * Get the current output value for a given function
	 * @return NAN (=disarmed) or value in range [-1, 1]
	 */
	ਆਭਾਸੀ ਦਸ਼ਮਲਵ value(OutputFunction func) = 0;

	ਆਭਾਸੀ ਦਸ਼ਮਲਵ default_Failsafe_ਮੁੱਲ(OutputFunction func) ਅਚਲ { ਵਾਪਸ NAN; }
	ਆਭਾਸੀ ਤਾਰਕਿਕ allowPrearmControl() ਅਚਲ { ਵਾਪਸ ਸੱਚ; }

	ਆਭਾਸੀ uORB::Subscription_Callback_Work_ਵਸਤੂ *subscriptionCallback() { ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ; }

	ਆਭਾਸੀ ਤਾਰਕਿਕ getLatestSampleTimestamp(hrt_abstime &t) ਅਚਲ { ਵਾਪਸ ਝੂਠ; }

	/**
	 * Check whether the output (motor) is configured to be reversible
	 */
	ਆਭਾਸੀ ਤਾਰਕਿਕ reversible(OutputFunction func) ਅਚਲ { ਵਾਪਸ ਝੂਠ; }
};
