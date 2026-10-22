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

#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ "FunctionProviderBase.hpp"

#ਅਨ੍ਤਰ੍ਭਾਵ <uORB/topics/internal_combustion_engine_control.h>

/**
 * Functions: ICE...
 */
ਵਰਗ FunctionICEControl : ਜਨਤਕ Function_Provider_ਅਧਾਰ
{
ਜਨਤਕ:
	FunctionICEControl()
	{
		reset_All_ਮੰਜ਼ਿਲ_Disarmed_ਮੁੱਲ();
	}

	ਸਥਿਰ Function_Provider_ਅਧਾਰ *allocate(ਅਚਲ Context &context) { ਵਾਪਸ ਨਵਾਂ FunctionICEControl(); }

	ਖਾਲੀ update() override
	{
		ਅੰਦਰੂਨੀ_combustion_engine_control_s ਅੰਦਰੂਨੀ_combustion_engine_control;

		// map [0, 1] to [-1, 1] which is the interface for non-motor PWM channels
		// NAN is mapped to disarmed
		ਜੇ (ਅੰਦਰੂਨੀ_combustion_engine_control_sub.update(&ਅੰਦਰੂਨੀ_combustion_engine_control)) {
			ਅੰਕੜੇ_3[0] = ਅੰਦਰੂਨੀ_combustion_engine_control.ignition_on * 2.f - 1.f;
			ਅੰਕੜੇ_3[1] = ਅੰਦਰੂਨੀ_combustion_engine_control.throttle_control * 2.f - 1.f;
			ਅੰਕੜੇ_3[2] = ਅੰਦਰੂਨੀ_combustion_engine_control.choke_control * 2.f - 1.f;
			ਅੰਕੜੇ_3[3] = ਅੰਦਰੂਨੀ_combustion_engine_control.starter_engine_control * 2.f - 1.f;
		}
	}

	ਦਸ਼ਮਲਵ value(OutputFunction func) override { ਵਾਪਸ ਅੰਕੜੇ_3[(ਪੂਰਨ)func - (ਪੂਰਨ)OutputFunction::IC_Engine_Ignition]; }

ਨਿਜੀ:
	ਸਥਿਰ constexpr ਪੂਰਨ ਅੰਕ_ਅੰਕੜੇ_points = 4;

	ਖਾਲੀ reset_All_ਮੰਜ਼ਿਲ_Disarmed_ਮੁੱਲ()
	{
		ਲਈ (ਪੂਰਨ i = 0; i < ਅੰਕ_ਅੰਕੜੇ_points; ++i) {
			ਅੰਕੜੇ_3[i] = NAN;
		}
	}

	static_assert(ਅੰਕ_ਅੰਕੜੇ_points == (ਪੂਰਨ)OutputFunction::IC_Engine_Starter - (ਪੂਰਨ)OutputFunction::IC_Engine_Ignition + 1,
		      "number of functions mismatch");

	uORB::Subscription ਅੰਦਰੂਨੀ_combustion_engine_control_sub{ORB_ID(ਅੰਦਰੂਨੀ_combustion_engine_control)};
	ਦਸ਼ਮਲਵ ਅੰਕੜੇ_3[ਅੰਕ_ਅੰਕੜੇ_points] {};
};
