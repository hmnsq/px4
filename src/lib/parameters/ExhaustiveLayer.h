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

#شامل <px4_platform_common/atomic_bitset.h>
#شامل "ParamLayer.h"

class ExhaustiveLayer : عمومی ParamLayer
{
عمومی:
	ExhaustiveLayer(ParamLayer *parent) : ParamLayer(parent)
	{
		// refresh all values
		برای (param_t i = 0; i < PARAM_شمار; i++) {
			مقادیر_3[i] = parent->get(i);
		}
	}

	bool store(param_t param, param_مقدار_u مقدار) override
	{
		اگر (param >= PARAM_شمار) {
			بازگشت false;
		}

		{
			ثابت AtomicTransaction transaction;
			مقادیر_3[param] = مقدار;
			_ownership_set.set(param);
		}

		بازگشت true;
	}

	bool contains(param_t param) ثابت override
	{
		بازگشت param < PARAM_شمار && _ownership_set[param];
	}

	px4::AtomicBitset<PARAM_شمار> containedAsBitset() ثابت override
	{
		بازگشت _ownership_set;
	}

	param_مقدار_u get(param_t param) ثابت override
	{
		اگر (param >= PARAM_شمار) {
			بازگشت {0};
		}

		ثابت AtomicTransaction transaction;
		// We assume to have the correct values for all params, even without ownership.
		// We expect that refresh was called when underlying defaults changed
		بازگشت مقادیر_3[param];
	}

	تهی reset(param_t param) override
	{
		اگر (param >= PARAM_شمار) {
			بازگشت;
		}

		ثابت AtomicTransaction transaction;
		مقادیر_3[param] = _parent->get(param);
		_ownership_set.set(param, false);
	}

	تهی refresh(param_t param) override
	{
		_parent->refresh(param);
		// in case we don't have ownership, and it changed below, we have to refresh our cache.
		{
			ثابت AtomicTransaction transaction;

			اگر (!contains(param)) {
				مقادیر_3[param] = _parent->get(param);
			}
		}
	}

	صحیح size() ثابت override
	{
		بازگشت _ownership_set.شمار();
	}

	صحیح byteSize() ثابت override
	{
		بازگشت PARAM_شمار * اندازه(param_مقدار_u);
	}

خصوصی:
	param_مقدار_u مقادیر_3[PARAM_شمار];
	px4::AtomicBitset<PARAM_شمار> _ownership_set;
};
