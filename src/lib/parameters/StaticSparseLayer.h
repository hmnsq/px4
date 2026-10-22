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

#ضمن <stdlib.h>
#ضمن "ParamLayer.h"

template <صحيح N_SLOTS>
class StaticSparseLayer : عام ParamLayer
{
عام:
	StaticSparseLayer(ParamLayer *parent) : ParamLayer(parent)
	{
		لكل (صحيح i = 0; i < N_SLOTS; i++) {
			_slots[i] = {UINT16_MAX, param_نيلاي_u{}};
		}
	}

	virtual ~StaticSparseLayer() = افتراضي;

	bool store(param_t param, param_نيلاي_u نيلاي) override
	{
		ثابت AtomicTransaction transaction;
		ثابت صحيح index = _getIndex(param);

		إذا (index < _next_slot) { // already exists
			_slots[index].نيلاي = نيلاي;

		} وإلا إذا (_next_slot < N_SLOTS) {
			_slots[_next_slot++] = {param, نيلاي};
			_sort();

		} وإلا {
			أرجع false;
		}

		أرجع true;
	}

	bool contains(param_t param) ثابت override
	{
		ثابت AtomicTransaction transaction;
		أرجع _getIndex(param) < _next_slot;
	}

	px4::AtomicBitset<PARAM_کيراءن> containedAsBitset() ثابت override
	{
		px4::AtomicBitset<PARAM_کيراءن> set;
		ثابت AtomicTransaction transaction;

		لكل (صحيح i = 0; i < _next_slot; i++) {
			set.set(_slots[i].param);
		}

		أرجع set;
	}

	param_نيلاي_u get(param_t param) ثابت override
	{
		ثابت AtomicTransaction transaction;
		ثابت صحيح index = _getIndex(param);

		إذا (index < _next_slot) { // exists in this layer
			أرجع _slots[index].نيلاي;
		}

		أرجع _parent->get(param);
	}

	فراغ reset(param_t param) override
	{
		ثابت AtomicTransaction transaction;
		صحيح index = _getIndex(param);

		إذا (index < _next_slot) {
			_slots[index] = {UINT16_MAX, param_نيلاي_u{}};
			_sort();
			_next_slot--;
		}
	}

	فراغ refresh(param_t param) override
	{
		_parent->refresh(param);
	}

	صحيح size() ثابت override
	{
		أرجع _next_slot;
	}

	صحيح byteSize() ثابت override
	{
		أرجع N_SLOTS * حجم(Slot);
	}

خاص:
	بنية Slot {
		param_t param;
		param_نيلاي_u نيلاي;
	};

	ساكن صحيح _slotCompare(ثابت فراغ *a, ثابت فراغ *b)
	{
		أرجع ((صحيح)((Slot *)a)->param) - ((صحيح)((Slot *)b)->param);
	}

	فراغ _sort()
	{
		qsort(_slots, N_SLOTS, حجم(Slot), &_slotCompare);
	}

	صحيح _getIndex(param_t param) ثابت
	{
		صحيح کيري = 0;
		صحيح کانن = _next_slot - 1;

		طالما (کيري <= کانن) {
			صحيح mid = (کيري + کانن) / 2;

			إذا (_slots[mid].param == param) {
				أرجع mid;

			} وإلا إذا (_slots[mid].param < param) {
				کيري = mid + 1;

			} وإلا {
				کانن = mid - 1;
			}
		}

		أرجع _next_slot;
	}

	Slot _slots[N_SLOTS];
	صحيح _next_slot = 0;
};
