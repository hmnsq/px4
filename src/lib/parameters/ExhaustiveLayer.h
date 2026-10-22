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

#அந்தர்பாவ <px4_platform_common/atomic_bitset.h>
#அந்தர்பாவ "ParamLayer.h"

class ExhaustiveLayer : பொது ParamLayer
{
பொது:
	ExhaustiveLayer(ParamLayer *parent) : ParamLayer(parent)
	{
		// refresh all values
		க்கு (param_t i = 0; i < PARAM_மொத்தம்; i++) {
			மதிப்புகள்_3[i] = parent->get(i);
		}
	}

	bool store(param_t param, param_மதிப்பு_u மதிப்பு) override
	{
		எனில் (param >= PARAM_மொத்தம்) {
			திரும்பு false;
		}

		{
			மாறிலி AtomicTransaction transaction;
			மதிப்புகள்_3[param] = மதிப்பு;
			_ownership_set.set(param);
		}

		திரும்பு true;
	}

	bool contains(param_t param) மாறிலி override
	{
		திரும்பு param < PARAM_மொத்தம் && _ownership_set[param];
	}

	px4::AtomicBitset<PARAM_மொத்தம்> containedAsBitset() மாறிலி override
	{
		திரும்பு _ownership_set;
	}

	param_மதிப்பு_u get(param_t param) மாறிலி override
	{
		எனில் (param >= PARAM_மொத்தம்) {
			திரும்பு {0};
		}

		மாறிலி AtomicTransaction transaction;
		// We assume to have the correct values for all params, even without ownership.
		// We expect that refresh was called when underlying defaults changed
		திரும்பு மதிப்புகள்_3[param];
	}

	வெறுமை reset(param_t param) override
	{
		எனில் (param >= PARAM_மொத்தம்) {
			திரும்பு;
		}

		மாறிலி AtomicTransaction transaction;
		மதிப்புகள்_3[param] = _parent->get(param);
		_ownership_set.set(param, false);
	}

	வெறுமை refresh(param_t param) override
	{
		_parent->refresh(param);
		// in case we don't have ownership, and it changed below, we have to refresh our cache.
		{
			மாறிலி AtomicTransaction transaction;

			எனில் (!contains(param)) {
				மதிப்புகள்_3[param] = _parent->get(param);
			}
		}
	}

	முழு size() மாறிலி override
	{
		திரும்பு _ownership_set.மொத்தம்();
	}

	முழு byteSize() மாறிலி override
	{
		திரும்பு PARAM_மொத்தம் * அளவு(param_மதிப்பு_u);
	}

தனிப்பட்ட:
	param_மதிப்பு_u மதிப்புகள்_3[PARAM_மொத்தம்];
	px4::AtomicBitset<PARAM_மொத்தம்> _ownership_set;
};
