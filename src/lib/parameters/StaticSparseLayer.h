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

#ہدایت once

#ضمن <stdlib.h>
#ضمن "ParamLayer.h"

سانچا <صحيح N_SLOTS>
class StaticSparseLayer : عام ParamLayer
{
عام:
	StaticSparseLayer(ParamLayer *ماپا) : ParamLayer(ماپا)
	{
		لكل (صحيح i = 0; i < N_SLOTS; i++) {
			_slots[i] = {UINT16_MAX, param_قدر_u{}};
		}
	}

	مجازی ~StaticSparseLayer() = افتراضي;

	bool store(param_t param, param_قدر_u قدر) override
	{
		ثابت AtomicTransaction transaction;
		ثابت صحيح اشاریہ = get_اشاریہ(param);

		إذا (اشاریہ < اگلا_تھاں) { // already exists
			_slots[اشاریہ].قدر = قدر;

		} وإلا إذا (اگلا_تھاں < N_SLOTS) {
			_slots[اگلا_تھاں++] = {param, قدر};
			ترتیب_دیو();

		} وإلا {
			أرجع false;
		}

		أرجع true;
	}

	bool شامل_اے(param_t param) ثابت override
	{
		ثابت AtomicTransaction transaction;
		أرجع get_اشاریہ(param) < اگلا_تھاں;
	}

	px4::AtomicBitset<PARAM_گنتی> containedAsBitset() ثابت override
	{
		px4::AtomicBitset<PARAM_گنتی> سیٹ;
		ثابت AtomicTransaction transaction;

		لكل (صحيح i = 0; i < اگلا_تھاں; i++) {
			سیٹ.سیٹ(_slots[i].param);
		}

		أرجع سیٹ;
	}

	param_قدر_u get(param_t param) ثابت override
	{
		ثابت AtomicTransaction transaction;
		ثابت صحيح اشاریہ = get_اشاریہ(param);

		إذا (اشاریہ < اگلا_تھاں) { // exists in this layer
			أرجع _slots[اشاریہ].قدر;
		}

		أرجع ماپا_2->get(param);
	}

	فراغ reset(param_t param) override
	{
		ثابت AtomicTransaction transaction;
		صحيح اشاریہ = get_اشاریہ(param);

		إذا (اشاریہ < اگلا_تھاں) {
			_slots[اشاریہ] = {UINT16_MAX, param_قدر_u{}};
			ترتیب_دیو();
			اگلا_تھاں--;
		}
	}

	فراغ refresh(param_t param) override
	{
		ماپا_2->refresh(param);
	}

	صحيح ناپ() ثابت override
	{
		أرجع اگلا_تھاں;
	}

	صحيح byte_ناپ() ثابت override
	{
		أرجع N_SLOTS * حجم(تھاں_2);
	}

خاص:
	بنية تھاں_2 {
		param_t param;
		param_قدر_u قدر;
	};

	ساكن صحيح تھاں_Compare(ثابت فراغ *a, ثابت فراغ *b)
	{
		أرجع ((صحيح)((تھاں_2 *)a)->param) - ((صحيح)((تھاں_2 *)b)->param);
	}

	فراغ ترتیب_دیو()
	{
		qsort(_slots, N_SLOTS, حجم(تھاں_2), &تھاں_Compare);
	}

	صحيح get_اشاریہ(param_t param) ثابت
	{
		صحيح بایاں = 0;
		صحيح دایاں = اگلا_تھاں - 1;

		طالما (بایاں <= دایاں) {
			صحيح mid = (بایاں + دایاں) / 2;

			إذا (_slots[mid].param == param) {
				أرجع mid;

			} وإلا إذا (_slots[mid].param < param) {
				بایاں = mid + 1;

			} وإلا {
				دایاں = mid - 1;
			}
		}

		أرجع اگلا_تھاں;
	}

	تھاں_2 _slots[N_SLOTS];
	صحيح اگلا_تھاں = 0;
};
