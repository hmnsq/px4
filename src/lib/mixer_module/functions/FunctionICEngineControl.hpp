/****************************************************************************
 *
 *   Copyright (c) 2025 PX4 Development Team. All rights reserved.
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

#ಅನ್ತರ್ಭಾವ <uORB/topics/internal_combustion_engine_control.h>

/**
 * Functions: ICE...
 */
ವರ್ಗ FunctionICEControl : ಸಾರ್ವಜನಿಕ Function_Provider_ತಳ
{
ಸಾರ್ವಜನಿಕ:
	FunctionICEControl()
	{
		reset_All_ಗಮ್ಯ_Disarmed_ಮೌಲ್ಯ();
	}

	ಸ್ಥಿರ Function_Provider_ತಳ *allocate(ಸ್ಥಿರಾಂಕ Context &context) { ಹಿಂತಿರುಗಿ ಹೊಸ FunctionICEControl(); }

	ಶೂನ್ಯ update() override
	{
		ಆಂತರಿಕ_combustion_engine_control_s ಆಂತರಿಕ_combustion_engine_control;

		// map [0, 1] to [-1, 1] which is the interface for non-motor PWM channels
		// NAN is mapped to disarmed
		ಆದರೆ (ಆಂತರಿಕ_combustion_engine_control_sub.update(&ಆಂತರಿಕ_combustion_engine_control)) {
			ದತ್ತಾಂಶ_3[0] = ಆಂತರಿಕ_combustion_engine_control.ignition_on * 2.f - 1.f;
			ದತ್ತಾಂಶ_3[1] = ಆಂತರಿಕ_combustion_engine_control.throttle_control * 2.f - 1.f;
			ದತ್ತಾಂಶ_3[2] = ಆಂತರಿಕ_combustion_engine_control.choke_control * 2.f - 1.f;
			ದತ್ತಾಂಶ_3[3] = ಆಂತರಿಕ_combustion_engine_control.starter_engine_control * 2.f - 1.f;
		}
	}

	ದಶಮಾಂಶ value(OutputFunction func) override { ಹಿಂತಿರುಗಿ ದತ್ತಾಂಶ_3[(ಪೂರ್ಣಾಂಕ)func - (ಪೂರ್ಣಾಂಕ)OutputFunction::IC_Engine_Ignition]; }

ಖಾಸಗಿ:
	ಸ್ಥಿರ constexpr ಪೂರ್ಣಾಂಕ ಅಂಕೆ_ದತ್ತಾಂಶ_points = 4;

	ಶೂನ್ಯ reset_All_ಗಮ್ಯ_Disarmed_ಮೌಲ್ಯ()
	{
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ಅಂಕೆ_ದತ್ತಾಂಶ_points; ++i) {
			ದತ್ತಾಂಶ_3[i] = NAN;
		}
	}

	static_assert(ಅಂಕೆ_ದತ್ತಾಂಶ_points == (ಪೂರ್ಣಾಂಕ)OutputFunction::IC_Engine_Starter - (ಪೂರ್ಣಾಂಕ)OutputFunction::IC_Engine_Ignition + 1,
		      "number of functions mismatch");

	uORB::Subscription ಆಂತರಿಕ_combustion_engine_control_sub{ORB_ID(ಆಂತರಿಕ_combustion_engine_control)};
	ದಶಮಾಂಶ ದತ್ತಾಂಶ_3[ಅಂಕೆ_ದತ್ತಾಂಶ_points] {};
};
