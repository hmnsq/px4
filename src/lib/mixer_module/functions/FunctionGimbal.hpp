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

#ਅਨ੍ਤਰ੍ਭਾਵ "FunctionProviderBase.hpp"

#ਅਨ੍ਤਰ੍ਭਾਵ <uORB/topics/gimbal_controls.h>

/**
 * Functions: Gimbal_Roll .. Gimbal_Yaw
 */
ਵਰਗ FunctionGimbal : ਜਨਤਕ Function_Provider_ਅਧਾਰ
{
ਜਨਤਕ:
	FunctionGimbal() = ਮੂਲ;
	ਸਥਿਰ Function_Provider_ਅਧਾਰ *allocate(ਅਚਲ Context &context) { ਵਾਪਸ ਨਵਾਂ FunctionGimbal(); }

	ਖਾਲੀ update() override
	{
		gimbal_controls_s gimbal_controls;

		ਜੇ (_topic.update(&gimbal_controls)) {
			ਅੰਕੜੇ_3[0] = gimbal_controls.control[gimbal_controls_s::ਸੂਚਕਾਂਕ_ROLL];
			ਅੰਕੜੇ_3[1] = gimbal_controls.control[gimbal_controls_s::ਸੂਚਕਾਂਕ_PITCH];
			ਅੰਕੜੇ_3[2] = gimbal_controls.control[gimbal_controls_s::ਸੂਚਕਾਂਕ_YAW];
		}
	}

	ਦਸ਼ਮਲਵ value(OutputFunction func) override { ਵਾਪਸ ਅੰਕੜੇ_3[(ਪੂਰਨ)func - (ਪੂਰਨ)OutputFunction::Gimbal_Roll]; }

ਨਿਜੀ:
	uORB::Subscription _topic{ORB_ID(gimbal_controls)};
	ਦਸ਼ਮਲਵ ਅੰਕੜੇ_3[3] { NAN, NAN, NAN };
};
