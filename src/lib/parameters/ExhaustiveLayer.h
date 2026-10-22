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

#رهنمود once

#ضمن <px4_platform_common/atomic_bitset.h>
#ضمن "ParamLayer.h"

class ExhaustiveLayer : عام ParamLayer
{
عام:
	ExhaustiveLayer(ParamLayer *والد) : ParamLayer(والد)
	{
		// refresh all values
		لكل (param_t i = 0; i < PARAM_شمار; i++) {
			مقادیر_3[i] = والد->get(i);
		}
	}

	bool store(param_t param, param_مقدار_u مقدار) override
	{
		إذا (param >= PARAM_شمار) {
			أرجع false;
		}

		{
			ثابت AtomicTransaction transaction;
			مقادیر_3[param] = مقدار;
			ownership_مجموعه.مجموعه(param);
		}

		أرجع true;
	}

	bool شامل_است(param_t param) ثابت override
	{
		أرجع param < PARAM_شمار && ownership_مجموعه[param];
	}

	px4::AtomicBitset<PARAM_شمار> containedAsBitset() ثابت override
	{
		أرجع ownership_مجموعه;
	}

	param_مقدار_u get(param_t param) ثابت override
	{
		إذا (param >= PARAM_شمار) {
			أرجع {0};
		}

		ثابت AtomicTransaction transaction;
		// We assume to have the correct values for all params, even without ownership.
		// We expect that refresh was called when underlying defaults changed
		أرجع مقادیر_3[param];
	}

	فراغ reset(param_t param) override
	{
		إذا (param >= PARAM_شمار) {
			أرجع;
		}

		ثابت AtomicTransaction transaction;
		مقادیر_3[param] = والد_2->get(param);
		ownership_مجموعه.مجموعه(param, false);
	}

	فراغ refresh(param_t param) override
	{
		والد_2->refresh(param);
		// in case we don't have ownership, and it changed below, we have to refresh our cache.
		{
			ثابت AtomicTransaction transaction;

			إذا (!شامل_است(param)) {
				مقادیر_3[param] = والد_2->get(param);
			}
		}
	}

	صحيح اندازه() ثابت override
	{
		أرجع ownership_مجموعه.شمار();
	}

	صحيح byte_اندازه() ثابت override
	{
		أرجع PARAM_شمار * حجم(param_مقدار_u);
	}

خاص:
	param_مقدار_u مقادیر_3[PARAM_شمار];
	px4::AtomicBitset<PARAM_شمار> ownership_مجموعه;
};
