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

#هدايت once

#ضمن <px4_platform_common/atomic_bitset.h>
#ضمن "ParamLayer.h"

class ExhaustiveLayer : عام ParamLayer
{
عام:
	ExhaustiveLayer(ParamLayer *والد) : ParamLayer(والد)
	{
		// refresh all values
		لكل (param_t i = 0; i < PARAM_ڳڻپ; i++) {
			قدرون_3[i] = والد->get(i);
		}
	}

	bool store(param_t param, param_قدر_u قدر) override
	{
		إذا (param >= PARAM_ڳڻپ) {
			أرجع false;
		}

		{
			ثابت AtomicTransaction transaction;
			قدرون_3[param] = قدر;
			ownership_سيٽ.سيٽ(param);
		}

		أرجع true;
	}

	bool شامل_آهي(param_t param) ثابت override
	{
		أرجع param < PARAM_ڳڻپ && ownership_سيٽ[param];
	}

	px4::AtomicBitset<PARAM_ڳڻپ> containedAsBitset() ثابت override
	{
		أرجع ownership_سيٽ;
	}

	param_قدر_u get(param_t param) ثابت override
	{
		إذا (param >= PARAM_ڳڻپ) {
			أرجع {0};
		}

		ثابت AtomicTransaction transaction;
		// We assume to have the correct values for all params, even without ownership.
		// We expect that refresh was called when underlying defaults changed
		أرجع قدرون_3[param];
	}

	فراغ reset(param_t param) override
	{
		إذا (param >= PARAM_ڳڻپ) {
			أرجع;
		}

		ثابت AtomicTransaction transaction;
		قدرون_3[param] = والد_2->get(param);
		ownership_سيٽ.سيٽ(param, false);
	}

	فراغ refresh(param_t param) override
	{
		والد_2->refresh(param);
		// in case we don't have ownership, and it changed below, we have to refresh our cache.
		{
			ثابت AtomicTransaction transaction;

			إذا (!شامل_آهي(param)) {
				قدرون_3[param] = والد_2->get(param);
			}
		}
	}

	صحيح سائيز() ثابت override
	{
		أرجع ownership_سيٽ.ڳڻپ();
	}

	صحيح byte_سائيز() ثابت override
	{
		أرجع PARAM_ڳڻپ * حجم(param_قدر_u);
	}

خاص:
	param_قدر_u قدرون_3[PARAM_ڳڻپ];
	px4::AtomicBitset<PARAM_ڳڻپ> ownership_سيٽ;
};
