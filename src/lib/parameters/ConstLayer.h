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

#অন্তর্ভাব "ParamLayer.h"

class ConstLayer : সার্বজনিক ParamLayer
{
সার্বজনিক:

	ConstLayer() = স্বভাব;

	bool store(param_t param, param_মান_u মান) override
	{
		প্রত্যাগম false;
	}

	bool contains(param_t param) ধ্রুব override
	{
		প্রত্যাগম param < PARAM_গুনতি;
	}

	px4::AtomicBitset<PARAM_গুনতি> containedAsBitset() ধ্রুব override
	{
		px4::AtomicBitset<PARAM_গুনতি> set;

		প্রতি (পূর্ণাঙ্ক i = 0; i < PARAM_গুনতি; i++) {
			set.set(i);
		}

		প্রত্যাগম set;
	}

	param_মান_u get(param_t param) ধ্রুব override
	{
		যদি (param >= PARAM_গুনতি) {
			প্রত্যাগম {0};
		}

		প্রত্যাগম px4::parameters[param].val;
	}

	শূন্য reset(param_t param) override
	{
		// Do nothing
	}

	শূন্য refresh(param_t param) override
	{
		// Do nothing
	}

	পূর্ণাঙ্ক size() ধ্রুব override
	{
		প্রত্যাগম PARAM_গুনতি;
	}

	পূর্ণাঙ্ক byteSize() ধ্রুব override
	{
		প্রত্যাগম PARAM_গুনতি * পরিমাণ(param_info_s);
	}
};
