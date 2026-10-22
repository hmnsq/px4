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

#include <stdlib.h>
#include "ParamLayer.h"

template <จำนวนเต็ม N_SLOTS>
class StaticSparseLayer : public ParamLayer
{
public:
	StaticSparseLayer(ParamLayer *parent) : ParamLayer(parent)
	{
		สำหรับ (จำนวนเต็ม i = 0; i < N_SLOTS; i++) {
			_slots[i] = {UINT16_MAX, param_value_u{}};
		}
	}

	virtual ~StaticSparseLayer() = default;

	bool store(param_t param, param_value_u value) override
	{
		const AtomicTransaction transaction;
		const จำนวนเต็ม index = _getIndex(param);

		ถ้า (index < _next_slot) { // already exists
			_slots[index].value = value;

		} มิฉะนั้น ถ้า (_next_slot < N_SLOTS) {
			_slots[_next_slot++] = {param, value};
			_sort();

		} มิฉะนั้น {
			คืน false;
		}

		คืน true;
	}

	bool contains(param_t param) const override
	{
		const AtomicTransaction transaction;
		คืน _getIndex(param) < _next_slot;
	}

	px4::AtomicBitset<PARAM_COUNT> containedAsBitset() const override
	{
		px4::AtomicBitset<PARAM_COUNT> set;
		const AtomicTransaction transaction;

		สำหรับ (จำนวนเต็ม i = 0; i < _next_slot; i++) {
			set.set(_slots[i].param);
		}

		คืน set;
	}

	param_value_u get(param_t param) const override
	{
		const AtomicTransaction transaction;
		const จำนวนเต็ม index = _getIndex(param);

		ถ้า (index < _next_slot) { // exists in this layer
			คืน _slots[index].value;
		}

		คืน _parent->get(param);
	}

	void reset(param_t param) override
	{
		const AtomicTransaction transaction;
		จำนวนเต็ม index = _getIndex(param);

		ถ้า (index < _next_slot) {
			_slots[index] = {UINT16_MAX, param_value_u{}};
			_sort();
			_next_slot--;
		}
	}

	void refresh(param_t param) override
	{
		_parent->refresh(param);
	}

	จำนวนเต็ม size() const override
	{
		คืน _next_slot;
	}

	จำนวนเต็ม byteSize() const override
	{
		คืน N_SLOTS * parimaa_na(Slot);
	}

private:
	sa_nracanaa Slot {
		param_t param;
		param_value_u value;
	};

	static จำนวนเต็ม _slotCompare(const void *a, const void *b)
	{
		คืน ((จำนวนเต็ม)((Slot *)a)->param) - ((จำนวนเต็ม)((Slot *)b)->param);
	}

	void _sort()
	{
		qsort(_slots, N_SLOTS, parimaa_na(Slot), &_slotCompare);
	}

	จำนวนเต็ม _getIndex(param_t param) const
	{
		จำนวนเต็ม left = 0;
		จำนวนเต็ม right = _next_slot - 1;

		ขณะ (left <= right) {
			จำนวนเต็ม mid = (left + right) / 2;

			ถ้า (_slots[mid].param == param) {
				คืน mid;

			} มิฉะนั้น ถ้า (_slots[mid].param < param) {
				left = mid + 1;

			} มิฉะนั้น {
				right = mid - 1;
			}
		}

		คืน _next_slot;
	}

	Slot _slots[N_SLOTS];
	จำนวนเต็ม _next_slot = 0;
};
