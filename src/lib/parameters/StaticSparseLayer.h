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

#ضمن <stdlib.h>
#ضمن "ParamLayer.h"

کينډۍ <صحيح N_SLOTS>
class StaticSparseLayer : عام ParamLayer
{
عام:
	StaticSparseLayer(ParamLayer *پلار) : ParamLayer(پلار)
	{
		لكل (صحيح i = 0; i < N_SLOTS; i++) {
			_slots[i] = {UINT16_MAX, param_ارزښت_u{}};
		}
	}

	مجازي ~StaticSparseLayer() = افتراضي;

	bool store(param_t param, param_ارزښت_u ارزښت) override
	{
		ثابت AtomicTransaction transaction;
		ثابت صحيح شاخص = get_شاخص(param);

		إذا (شاخص < بل_خونه) { // already exists
			_slots[شاخص].ارزښت = ارزښت;

		} وإلا إذا (بل_خونه < N_SLOTS) {
			_slots[بل_خونه++] = {param, ارزښت};
			ترتیبول();

		} وإلا {
			أرجع false;
		}

		أرجع true;
	}

	bool لري(param_t param) ثابت override
	{
		ثابت AtomicTransaction transaction;
		أرجع get_شاخص(param) < بل_خونه;
	}

	px4::AtomicBitset<PARAM_شمېر> containedAsBitset() ثابت override
	{
		px4::AtomicBitset<PARAM_شمېر> سېټ;
		ثابت AtomicTransaction transaction;

		لكل (صحيح i = 0; i < بل_خونه; i++) {
			سېټ.سېټ(_slots[i].param);
		}

		أرجع سېټ;
	}

	param_ارزښت_u get(param_t param) ثابت override
	{
		ثابت AtomicTransaction transaction;
		ثابت صحيح شاخص = get_شاخص(param);

		إذا (شاخص < بل_خونه) { // exists in this layer
			أرجع _slots[شاخص].ارزښت;
		}

		أرجع پلار_2->get(param);
	}

	فراغ reset(param_t param) override
	{
		ثابت AtomicTransaction transaction;
		صحيح شاخص = get_شاخص(param);

		إذا (شاخص < بل_خونه) {
			_slots[شاخص] = {UINT16_MAX, param_ارزښت_u{}};
			ترتیبول();
			بل_خونه--;
		}
	}

	فراغ refresh(param_t param) override
	{
		پلار_2->refresh(param);
	}

	صحيح اندازه() ثابت override
	{
		أرجع بل_خونه;
	}

	صحيح byte_اندازه() ثابت override
	{
		أرجع N_SLOTS * حجم(خونه_2);
	}

خاص:
	بنية خونه_2 {
		param_t param;
		param_ارزښت_u ارزښت;
	};

	ساكن صحيح خونه_Compare(ثابت فراغ *a, ثابت فراغ *b)
	{
		أرجع ((صحيح)((خونه_2 *)a)->param) - ((صحيح)((خونه_2 *)b)->param);
	}

	فراغ ترتیبول()
	{
		qsort(_slots, N_SLOTS, حجم(خونه_2), &خونه_Compare);
	}

	صحيح get_شاخص(param_t param) ثابت
	{
		صحيح کیڼ_2 = 0;
		صحيح ښی_2 = بل_خونه - 1;

		طالما (کیڼ_2 <= ښی_2) {
			صحيح mid = (کیڼ_2 + ښی_2) / 2;

			إذا (_slots[mid].param == param) {
				أرجع mid;

			} وإلا إذا (_slots[mid].param < param) {
				کیڼ_2 = mid + 1;

			} وإلا {
				ښی_2 = mid - 1;
			}
		}

		أرجع بل_خونه;
	}

	خونه_2 _slots[N_SLOTS];
	صحيح بل_خونه = 0;
};
