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

#ଅନ୍ତର୍ଭାଵ "FunctionProviderBase.hpp"

#ଅନ୍ତର୍ଭାଵ <uORB/topics/landing_gear.h>

/**
 * Functions: Landing_Gear
 */
ଶ୍ରେଣୀ FunctionLandingGear : ସର୍ବସାଧାରଣ FunctionProviderBase
{
ସର୍ବସାଧାରଣ:
	FunctionLandingGear() = ପୂର୍ବନିର୍ଦ୍ଧାରିତ;
	ସ୍ଥିର FunctionProviderBase *allocate(ଅଚଳ Context &context) { ଫେରନ୍ତୁ ନୂତନ FunctionLandingGear(); }

	ଶୂନ୍ୟ update() override
	{
		landing_gear_s landing_gear;

		ଯଦି (_topic.update(&landing_gear)) {
			ଯଦି (landing_gear.landing_gear == landing_gear_s::GEAR_DOWN) {
				_data = -1.f;

			} ଅନ୍ୟଥା ଯଦି (landing_gear.landing_gear == landing_gear_s::GEAR_UP) {
				_data = 1.f;
			}
		}
	}

	ଦଶମିକ value(OutputFunction func) override { ଫେରନ୍ତୁ _data; }

ବ୍ୟକ୍ତିଗତ:
	uORB::Subscription _topic{ORB_ID(landing_gear)};
	ଦଶମିକ _data{-1.f};
};
