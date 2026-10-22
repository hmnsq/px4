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

#لارښوونه once

#ضمن <px4_platform_common/atomic_bitset.h>
#ضمن "ParamLayer.h"

class ExhaustiveLayer : عام ParamLayer
{
عام:
	ExhaustiveLayer(ParamLayer *پلار) : ParamLayer(پلار)
	{
		// refresh all values
		لكل (param_t i = 0; i < PARAM_شمېر; i++) {
			ارزښتونه_3[i] = پلار->get(i);
		}
	}

	bool store(param_t param, param_ارزښت_u ارزښت) override
	{
		إذا (param >= PARAM_شمېر) {
			أرجع false;
		}

		{
			ثابت AtomicTransaction transaction;
			ارزښتونه_3[param] = ارزښت;
			ownership_سېټ.سېټ(param);
		}

		أرجع true;
	}

	bool لري(param_t param) ثابت override
	{
		أرجع param < PARAM_شمېر && ownership_سېټ[param];
	}

	px4::AtomicBitset<PARAM_شمېر> containedAsBitset() ثابت override
	{
		أرجع ownership_سېټ;
	}

	param_ارزښت_u get(param_t param) ثابت override
	{
		إذا (param >= PARAM_شمېر) {
			أرجع {0};
		}

		ثابت AtomicTransaction transaction;
		// We assume to have the correct values for all params, even without ownership.
		// We expect that refresh was called when underlying defaults changed
		أرجع ارزښتونه_3[param];
	}

	فراغ reset(param_t param) override
	{
		إذا (param >= PARAM_شمېر) {
			أرجع;
		}

		ثابت AtomicTransaction transaction;
		ارزښتونه_3[param] = پلار_2->get(param);
		ownership_سېټ.سېټ(param, false);
	}

	فراغ refresh(param_t param) override
	{
		پلار_2->refresh(param);
		// in case we don't have ownership, and it changed below, we have to refresh our cache.
		{
			ثابت AtomicTransaction transaction;

			إذا (!لري(param)) {
				ارزښتونه_3[param] = پلار_2->get(param);
			}
		}
	}

	صحيح اندازه() ثابت override
	{
		أرجع ownership_سېټ.شمېر();
	}

	صحيح byte_اندازه() ثابت override
	{
		أرجع PARAM_شمېر * حجم(param_ارزښت_u);
	}

خاص:
	param_ارزښت_u ارزښتونه_3[PARAM_شمېر];
	px4::AtomicBitset<PARAM_شمېر> ownership_سېټ;
};
