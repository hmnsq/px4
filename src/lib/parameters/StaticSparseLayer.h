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

#ضمن <stdlib.h>
#ضمن "ParamLayer.h"

قالب <صحيح N_SLOTS>
class StaticSparseLayer : عام ParamLayer
{
عام:
	StaticSparseLayer(ParamLayer *والد) : ParamLayer(والد)
	{
		لكل (صحيح i = 0; i < N_SLOTS; i++) {
			_slots[i] = {UINT16_MAX, param_مقدار_u{}};
		}
	}

	مجازی ~StaticSparseLayer() = افتراضي;

	bool store(param_t param, param_مقدار_u مقدار) override
	{
		ثابت AtomicTransaction transaction;
		ثابت صحيح شاخص = get_شاخص(param);

		إذا (شاخص < بعدی_جایگاه) { // already exists
			_slots[شاخص].مقدار = مقدار;

		} وإلا إذا (بعدی_جایگاه < N_SLOTS) {
			_slots[بعدی_جایگاه++] = {param, مقدار};
			مرتب_سازی();

		} وإلا {
			أرجع false;
		}

		أرجع true;
	}

	bool شامل_است(param_t param) ثابت override
	{
		ثابت AtomicTransaction transaction;
		أرجع get_شاخص(param) < بعدی_جایگاه;
	}

	px4::AtomicBitset<PARAM_شمار> containedAsBitset() ثابت override
	{
		px4::AtomicBitset<PARAM_شمار> مجموعه;
		ثابت AtomicTransaction transaction;

		لكل (صحيح i = 0; i < بعدی_جایگاه; i++) {
			مجموعه.مجموعه(_slots[i].param);
		}

		أرجع مجموعه;
	}

	param_مقدار_u get(param_t param) ثابت override
	{
		ثابت AtomicTransaction transaction;
		ثابت صحيح شاخص = get_شاخص(param);

		إذا (شاخص < بعدی_جایگاه) { // exists in this layer
			أرجع _slots[شاخص].مقدار;
		}

		أرجع والد_2->get(param);
	}

	فراغ reset(param_t param) override
	{
		ثابت AtomicTransaction transaction;
		صحيح شاخص = get_شاخص(param);

		إذا (شاخص < بعدی_جایگاه) {
			_slots[شاخص] = {UINT16_MAX, param_مقدار_u{}};
			مرتب_سازی();
			بعدی_جایگاه--;
		}
	}

	فراغ refresh(param_t param) override
	{
		والد_2->refresh(param);
	}

	صحيح اندازه() ثابت override
	{
		أرجع بعدی_جایگاه;
	}

	صحيح byte_اندازه() ثابت override
	{
		أرجع N_SLOTS * حجم(جایگاه_2);
	}

خاص:
	بنية جایگاه_2 {
		param_t param;
		param_مقدار_u مقدار;
	};

	ساكن صحيح جایگاه_Compare(ثابت فراغ *a, ثابت فراغ *b)
	{
		أرجع ((صحيح)((جایگاه_2 *)a)->param) - ((صحيح)((جایگاه_2 *)b)->param);
	}

	فراغ مرتب_سازی()
	{
		qsort(_slots, N_SLOTS, حجم(جایگاه_2), &جایگاه_Compare);
	}

	صحيح get_شاخص(param_t param) ثابت
	{
		صحيح چپ_2 = 0;
		صحيح راست_2 = بعدی_جایگاه - 1;

		طالما (چپ_2 <= راست_2) {
			صحيح mid = (چپ_2 + راست_2) / 2;

			إذا (_slots[mid].param == param) {
				أرجع mid;

			} وإلا إذا (_slots[mid].param < param) {
				چپ_2 = mid + 1;

			} وإلا {
				راست_2 = mid - 1;
			}
		}

		أرجع بعدی_جایگاه;
	}

	جایگاه_2 _slots[N_SLOTS];
	صحيح بعدی_جایگاه = 0;
};
