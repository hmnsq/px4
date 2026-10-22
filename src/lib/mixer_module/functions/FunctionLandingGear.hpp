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

#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ "FunctionProviderBase.hpp"

#ಅನ್ತರ್ಭಾವ <uORB/topics/landing_gear.h>

/**
 * Functions: Landing_Gear
 */
ವರ್ಗ FunctionLandingGear : ಸಾರ್ವಜನಿಕ Function_Provider_ತಳ
{
ಸಾರ್ವಜನಿಕ:
	FunctionLandingGear() = ಪೂರ್ವನಿಯೋಜಿತ;
	ಸ್ಥಿರ Function_Provider_ತಳ *allocate(ಸ್ಥಿರಾಂಕ Context &context) { ಹಿಂತಿರುಗಿ ಹೊಸ FunctionLandingGear(); }

	ಶೂನ್ಯ update() override
	{
		landing_gear_s landing_gear;

		ಆದರೆ (_topic.update(&landing_gear)) {
			ಆದರೆ (landing_gear.landing_gear == landing_gear_s::GEAR_ಕೆಳಕ್ಕೆ) {
				ದತ್ತಾಂಶ_3 = -1.f;

			} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (landing_gear.landing_gear == landing_gear_s::GEAR_ಮೇಲೆ) {
				ದತ್ತಾಂಶ_3 = 1.f;
			}
		}
	}

	ದಶಮಾಂಶ value(OutputFunction func) override { ಹಿಂತಿರುಗಿ ದತ್ತಾಂಶ_3; }

ಖಾಸಗಿ:
	uORB::Subscription _topic{ORB_ID(landing_gear)};
	ದಶಮಾಂಶ ದತ್ತಾಂಶ_3{-1.f};
};
