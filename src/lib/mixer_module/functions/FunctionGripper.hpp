/****************************************************************************
 *
 *   Copyright (c) 2022 PX4 Development Team. All rights reserved.
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

#ਅਨ੍ਤਰ੍ਭਾਵ <uORB/topics/gripper.h>

/**
 * @brief Function: Gripper (Used for actuating a Gripper)
 */
ਵਰਗ FunctionGripper : ਜਨਤਕ Function_Provider_ਅਧਾਰ
{
ਜਨਤਕ:
	FunctionGripper() = ਮੂਲ;
	ਸਥਿਰ Function_Provider_ਅਧਾਰ *allocate(ਅਚਲ Context &context) { ਵਾਪਸ ਨਵਾਂ FunctionGripper(); }

	ਖਾਲੀ update() override
	{
		gripper_s gripper;

		ਜੇ (_gripper_sub.update(&gripper)) {
			ਜੇ (gripper.command == gripper_s::COMMAND_RELEASE) {
				ਅੰਕੜੇ_3 = -1.f; // Minimum command for release

			} ਨਹੀਂਤਾਂ ਜੇ (gripper.command == gripper_s::COMMAND_GRAB) {
				ਅੰਕੜੇ_3 = 1.f; // Maximum command for grab

			}
		}
	}

	ਦਸ਼ਮਲਵ value(OutputFunction func) override { ਵਾਪਸ ਅੰਕੜੇ_3; }

ਨਿਜੀ:
	uORB::Subscription _gripper_sub{ORB_ID(gripper)};
	ਦਸ਼ਮਲਵ ਅੰਕੜੇ_3{-1.f};
};
