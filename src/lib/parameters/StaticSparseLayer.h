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

#ضمن <stdlib.h>
#ضمن "ParamLayer.h"

سانچو <صحيح N_SLOTS>
class StaticSparseLayer : عام ParamLayer
{
عام:
	StaticSparseLayer(ParamLayer *والد) : ParamLayer(والد)
	{
		لكل (صحيح i = 0; i < N_SLOTS; i++) {
			_slots[i] = {UINT16_MAX, param_قدر_u{}};
		}
	}

	مجازي ~StaticSparseLayer() = افتراضي;

	bool store(param_t param, param_قدر_u قدر) override
	{
		ثابت AtomicTransaction transaction;
		ثابت صحيح اشاريو = get_اشاريو(param);

		إذا (اشاريو < ايندڙ_جڳهه) { // already exists
			_slots[اشاريو].قدر = قدر;

		} وإلا إذا (ايندڙ_جڳهه < N_SLOTS) {
			_slots[ايندڙ_جڳهه++] = {param, قدر};
			ترتيب_ڏيو();

		} وإلا {
			أرجع false;
		}

		أرجع true;
	}

	bool شامل_آهي(param_t param) ثابت override
	{
		ثابت AtomicTransaction transaction;
		أرجع get_اشاريو(param) < ايندڙ_جڳهه;
	}

	px4::AtomicBitset<PARAM_ڳڻپ> containedAsBitset() ثابت override
	{
		px4::AtomicBitset<PARAM_ڳڻپ> سيٽ;
		ثابت AtomicTransaction transaction;

		لكل (صحيح i = 0; i < ايندڙ_جڳهه; i++) {
			سيٽ.سيٽ(_slots[i].param);
		}

		أرجع سيٽ;
	}

	param_قدر_u get(param_t param) ثابت override
	{
		ثابت AtomicTransaction transaction;
		ثابت صحيح اشاريو = get_اشاريو(param);

		إذا (اشاريو < ايندڙ_جڳهه) { // exists in this layer
			أرجع _slots[اشاريو].قدر;
		}

		أرجع والد_2->get(param);
	}

	فراغ reset(param_t param) override
	{
		ثابت AtomicTransaction transaction;
		صحيح اشاريو = get_اشاريو(param);

		إذا (اشاريو < ايندڙ_جڳهه) {
			_slots[اشاريو] = {UINT16_MAX, param_قدر_u{}};
			ترتيب_ڏيو();
			ايندڙ_جڳهه--;
		}
	}

	فراغ refresh(param_t param) override
	{
		والد_2->refresh(param);
	}

	صحيح سائيز() ثابت override
	{
		أرجع ايندڙ_جڳهه;
	}

	صحيح byte_سائيز() ثابت override
	{
		أرجع N_SLOTS * حجم(جڳهه_2);
	}

خاص:
	بنية جڳهه_2 {
		param_t param;
		param_قدر_u قدر;
	};

	ساكن صحيح جڳهه_Compare(ثابت فراغ *a, ثابت فراغ *b)
	{
		أرجع ((صحيح)((جڳهه_2 *)a)->param) - ((صحيح)((جڳهه_2 *)b)->param);
	}

	فراغ ترتيب_ڏيو()
	{
		qsort(_slots, N_SLOTS, حجم(جڳهه_2), &جڳهه_Compare);
	}

	صحيح get_اشاريو(param_t param) ثابت
	{
		صحيح کاٻو = 0;
		صحيح ساڄو = ايندڙ_جڳهه - 1;

		طالما (کاٻو <= ساڄو) {
			صحيح mid = (کاٻو + ساڄو) / 2;

			إذا (_slots[mid].param == param) {
				أرجع mid;

			} وإلا إذا (_slots[mid].param < param) {
				کاٻو = mid + 1;

			} وإلا {
				ساڄو = mid - 1;
			}
		}

		أرجع ايندڙ_جڳهه;
	}

	جڳهه_2 _slots[N_SLOTS];
	صحيح ايندڙ_جڳهه = 0;
};
