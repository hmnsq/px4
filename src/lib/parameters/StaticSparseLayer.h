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

#شامل <stdlib.h>
#شامل "ParamLayer.h"

template <صحیح N_SLOTS>
class StaticSparseLayer : عمومی ParamLayer
{
عمومی:
	StaticSparseLayer(ParamLayer *parent) : ParamLayer(parent)
	{
		برای (صحیح i = 0; i < N_SLOTS; i++) {
			_slots[i] = {UINT16_MAX, param_مقدار_u{}};
		}
	}

	مجازی ~StaticSparseLayer() = پیشفرض;

	bool store(param_t param, param_مقدار_u مقدار) override
	{
		ثابت AtomicTransaction transaction;
		ثابت صحیح index = _getIndex(param);

		اگر (index < _next_slot) { // already exists
			_slots[index].مقدار = مقدار;

		} وگرنه اگر (_next_slot < N_SLOTS) {
			_slots[_next_slot++] = {param, مقدار};
			_sort();

		} وگرنه {
			بازگشت false;
		}

		بازگشت true;
	}

	bool contains(param_t param) ثابت override
	{
		ثابت AtomicTransaction transaction;
		بازگشت _getIndex(param) < _next_slot;
	}

	px4::AtomicBitset<PARAM_شمار> containedAsBitset() ثابت override
	{
		px4::AtomicBitset<PARAM_شمار> set;
		ثابت AtomicTransaction transaction;

		برای (صحیح i = 0; i < _next_slot; i++) {
			set.set(_slots[i].param);
		}

		بازگشت set;
	}

	param_مقدار_u get(param_t param) ثابت override
	{
		ثابت AtomicTransaction transaction;
		ثابت صحیح index = _getIndex(param);

		اگر (index < _next_slot) { // exists in this layer
			بازگشت _slots[index].مقدار;
		}

		بازگشت _parent->get(param);
	}

	تهی reset(param_t param) override
	{
		ثابت AtomicTransaction transaction;
		صحیح index = _getIndex(param);

		اگر (index < _next_slot) {
			_slots[index] = {UINT16_MAX, param_مقدار_u{}};
			_sort();
			_next_slot--;
		}
	}

	تهی refresh(param_t param) override
	{
		_parent->refresh(param);
	}

	صحیح size() ثابت override
	{
		بازگشت _next_slot;
	}

	صحیح byteSize() ثابت override
	{
		بازگشت N_SLOTS * اندازه(Slot);
	}

خصوصی:
	ساختار Slot {
		param_t param;
		param_مقدار_u مقدار;
	};

	ایستا صحیح _slotCompare(ثابت تهی *a, ثابت تهی *b)
	{
		بازگشت ((صحیح)((Slot *)a)->param) - ((صحیح)((Slot *)b)->param);
	}

	تهی _sort()
	{
		qsort(_slots, N_SLOTS, اندازه(Slot), &_slotCompare);
	}

	صحیح _getIndex(param_t param) ثابت
	{
		صحیح چپ = 0;
		صحیح راست = _next_slot - 1;

		وقتی (چپ <= راست) {
			صحیح mid = (چپ + راست) / 2;

			اگر (_slots[mid].param == param) {
				بازگشت mid;

			} وگرنه اگر (_slots[mid].param < param) {
				چپ = mid + 1;

			} وگرنه {
				راست = mid - 1;
			}
		}

		بازگشت _next_slot;
	}

	Slot _slots[N_SLOTS];
	صحیح _next_slot = 0;
};
