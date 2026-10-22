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

#شامل <stdlib.h>
#شامل "ParamLayer.h"

سانچہ <صحیح N_SLOTS>
class StaticSparseLayer : عوامی ParamLayer
{
عوامی:
	StaticSparseLayer(ParamLayer *والد) : ParamLayer(والد)
	{
		برائے (صحیح i = 0; i < N_SLOTS; i++) {
			_slots[i] = {UINT16_MAX, param_قدر_u{}};
		}
	}

	مجازی ~StaticSparseLayer() = طےشدہ;

	bool store(param_t param, param_قدر_u قدر) override
	{
		ثابت AtomicTransaction transaction;
		ثابت صحیح اشاریہ = get_اشاریہ(param);

		اگر (اشاریہ < اگلا_خانہ) { // already exists
			_slots[اشاریہ].قدر = قدر;

		} ورنہ اگر (اگلا_خانہ < N_SLOTS) {
			_slots[اگلا_خانہ++] = {param, قدر};
			ترتیب_دو();

		} ورنہ {
			واپس false;
		}

		واپس true;
	}

	bool شامل_ہے(param_t param) ثابت override
	{
		ثابت AtomicTransaction transaction;
		واپس get_اشاریہ(param) < اگلا_خانہ;
	}

	px4::AtomicBitset<PARAM_گنتی> containedAsBitset() ثابت override
	{
		px4::AtomicBitset<PARAM_گنتی> سیٹ;
		ثابت AtomicTransaction transaction;

		برائے (صحیح i = 0; i < اگلا_خانہ; i++) {
			سیٹ.سیٹ(_slots[i].param);
		}

		واپس سیٹ;
	}

	param_قدر_u get(param_t param) ثابت override
	{
		ثابت AtomicTransaction transaction;
		ثابت صحیح اشاریہ = get_اشاریہ(param);

		اگر (اشاریہ < اگلا_خانہ) { // exists in this layer
			واپس _slots[اشاریہ].قدر;
		}

		واپس والد_2->get(param);
	}

	خالی reset(param_t param) override
	{
		ثابت AtomicTransaction transaction;
		صحیح اشاریہ = get_اشاریہ(param);

		اگر (اشاریہ < اگلا_خانہ) {
			_slots[اشاریہ] = {UINT16_MAX, param_قدر_u{}};
			ترتیب_دو();
			اگلا_خانہ--;
		}
	}

	خالی refresh(param_t param) override
	{
		والد_2->refresh(param);
	}

	صحیح سائز() ثابت override
	{
		واپس اگلا_خانہ;
	}

	صحیح byte_سائز() ثابت override
	{
		واپس N_SLOTS * حجم(خانہ_2);
	}

نجی:
	ساخت خانہ_2 {
		param_t param;
		param_قدر_u قدر;
	};

	ساکن صحیح خانہ_Compare(ثابت خالی *a, ثابت خالی *b)
	{
		واپس ((صحیح)((خانہ_2 *)a)->param) - ((صحیح)((خانہ_2 *)b)->param);
	}

	خالی ترتیب_دو()
	{
		qsort(_slots, N_SLOTS, حجم(خانہ_2), &خانہ_Compare);
	}

	صحیح get_اشاریہ(param_t param) ثابت
	{
		صحیح بایاں = 0;
		صحیح دایاں = اگلا_خانہ - 1;

		جب (بایاں <= دایاں) {
			صحیح mid = (بایاں + دایاں) / 2;

			اگر (_slots[mid].param == param) {
				واپس mid;

			} ورنہ اگر (_slots[mid].param < param) {
				بایاں = mid + 1;

			} ورنہ {
				دایاں = mid - 1;
			}
		}

		واپس اگلا_خانہ;
	}

	خانہ_2 _slots[N_SLOTS];
	صحیح اگلا_خانہ = 0;
};
