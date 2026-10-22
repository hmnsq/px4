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

#pragma once

#ಅನ್ತರ್ಭಾವ "FunctionProviderBase.hpp"

#ಅನ್ತರ್ಭಾವ <uORB/topics/gripper.h>

/**
 * @brief Function: Gripper (Used for actuating a Gripper)
 */
ವರ್ಗ FunctionGripper : ಸಾರ್ವಜನಿಕ FunctionProviderBase
{
ಸಾರ್ವಜನಿಕ:
	FunctionGripper() = ಸ್ವಭಾವ;
	ಸ್ಥಿರ FunctionProviderBase *allocate(ಧ್ರುವ Context &context) { ಪ್ರತ್ಯಾಗಮ ನೂತನ FunctionGripper(); }

	ಶೂನ್ಯ update() override
	{
		gripper_s gripper;

		ಯದಿ (_gripper_sub.update(&gripper)) {
			ಯದಿ (gripper.command == gripper_s::COMMAND_RELEASE) {
				_data = -1.f; // Minimum command for release

			} ಅನ್ಯಥಾ ಯದಿ (gripper.command == gripper_s::COMMAND_GRAB) {
				_data = 1.f; // Maximum command for grab

			}
		}
	}

	ಭಿನ್ನ value(OutputFunction func) override { ಪ್ರತ್ಯಾಗಮ _data; }

ಗುಪ್ತ:
	uORB::Subscription _gripper_sub{ORB_ID(gripper)};
	ಭಿನ್ನ _data{-1.f};
};
