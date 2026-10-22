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

#அந்தர்பாவ "ParamLayer.h"

class ConstLayer : பொது ParamLayer
{
பொது:

	ConstLayer() = இயல்பு;

	bool store(param_t param, param_மதிப்பு_u மதிப்பு) override
	{
		திரும்பு false;
	}

	bool contains(param_t param) மாறிலி override
	{
		திரும்பு param < PARAM_மொத்தம்;
	}

	px4::AtomicBitset<PARAM_மொத்தம்> containedAsBitset() மாறிலி override
	{
		px4::AtomicBitset<PARAM_மொத்தம்> set;

		க்கு (முழு i = 0; i < PARAM_மொத்தம்; i++) {
			set.set(i);
		}

		திரும்பு set;
	}

	param_மதிப்பு_u get(param_t param) மாறிலி override
	{
		எனில் (param >= PARAM_மொத்தம்) {
			திரும்பு {0};
		}

		திரும்பு px4::parameters[param].val;
	}

	வெறுமை reset(param_t param) override
	{
		// Do nothing
	}

	வெறுமை refresh(param_t param) override
	{
		// Do nothing
	}

	முழு size() மாறிலி override
	{
		திரும்பு PARAM_மொத்தம்;
	}

	முழு byteSize() மாறிலி override
	{
		திரும்பு PARAM_மொத்தம் * அளவு(param_info_s);
	}
};
