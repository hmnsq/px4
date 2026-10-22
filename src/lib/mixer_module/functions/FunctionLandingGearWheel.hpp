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

#ہدایت once

#شامل "FunctionProviderBase.hpp"

#شامل <uORB/topics/landing_gear_wheel.h>

/**
 * Functions: Landing_Gear_Wheel
 */
جماعت FunctionLandingGearWheel : عوامی Function_Provider_بنیاد
{
عوامی:
	FunctionLandingGearWheel() = طےشدہ;
	ساکن Function_Provider_بنیاد *allocate(ثابت Context &context) { واپس نیا FunctionLandingGearWheel(); }

	خالی update() override
	{
		landing_gear_wheel_s landing_gear_wheel;

		اگر (_topic.update(&landing_gear_wheel)) {
			ڈیٹا_3 = landing_gear_wheel.normalized_wheel_setpoint;
		}
	}

	اعشاری value(OutputFunction func) override { واپس ڈیٹا_3; }

نجی:
	uORB::Subscription _topic{ORB_ID(landing_gear_wheel)};
	اعشاری ڈیٹا_3{0.f};
};
