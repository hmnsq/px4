/****************************************************************************
 *
 *   Copyright (c) 2023 PX4 Development Team. All rights reserved.
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

#ଅନ୍ତର୍ଭାଵ "ParamLayer.h"

class ConstLayer : ସର୍ବସାଧାରଣ ParamLayer
{
ସର୍ବସାଧାରଣ:

	ConstLayer() = ପୂର୍ବନିର୍ଦ୍ଧାରିତ;

	bool store(param_t param, param_ମୂଲ୍ୟ_u ମୂଲ୍ୟ) override
	{
		ଫେରନ୍ତୁ false;
	}

	bool contains(param_t param) ଅଚଳ override
	{
		ଫେରନ୍ତୁ param < PARAM_ଗଣନା;
	}

	px4::AtomicBitset<PARAM_ଗଣନା> containedAsBitset() ଅଚଳ override
	{
		px4::AtomicBitset<PARAM_ଗଣନା> set;

		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < PARAM_ଗଣନା; i++) {
			set.set(i);
		}

		ଫେରନ୍ତୁ set;
	}

	param_ମୂଲ୍ୟ_u get(param_t param) ଅଚଳ override
	{
		ଯଦି (param >= PARAM_ଗଣନା) {
			ଫେରନ୍ତୁ {0};
		}

		ଫେରନ୍ତୁ px4::parameters[param].val;
	}

	ଶୂନ୍ୟ reset(param_t param) override
	{
		// Do nothing
	}

	ଶୂନ୍ୟ refresh(param_t param) override
	{
		// Do nothing
	}

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା size() ଅଚଳ override
	{
		ଫେରନ୍ତୁ PARAM_ଗଣନା;
	}

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା byteSize() ଅଚଳ override
	{
		ଫେରନ୍ତୁ PARAM_ଗଣନା * ଆକାର(param_info_s);
	}
};
