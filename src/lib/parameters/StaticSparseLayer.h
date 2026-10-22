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

template <целое N_SLOTS>
class StaticSparseLayer : public ParamLayer
{
public:
	StaticSparseLayer(ParamLayer *parent) : ParamLayer(parent)
	{
		для (целое i = 0; i < N_SLOTS; i++) {
			_slots[i] = {UINT16_MAX, param_value_u{}};
		}
	}

	virtual ~StaticSparseLayer() = default;

	bool store(param_t param, param_value_u value) override
	{
		const AtomicTransaction transaction;
		const целое index = _getIndex(param);

		если (index < _next_slot) { // already exists
			_slots[index].value = value;

		} иначе если (_next_slot < N_SLOTS) {
			_slots[_next_slot++] = {param, value};
			_sort();

		} иначе {
			вернуть false;
		}

		вернуть true;
	}

	bool contains(param_t param) const override
	{
		const AtomicTransaction transaction;
		вернуть _getIndex(param) < _next_slot;
	}

	px4::AtomicBitset<PARAM_COUNT> containedAsBitset() const override
	{
		px4::AtomicBitset<PARAM_COUNT> set;
		const AtomicTransaction transaction;

		для (целое i = 0; i < _next_slot; i++) {
			set.set(_slots[i].param);
		}

		вернуть set;
	}

	param_value_u get(param_t param) const override
	{
		const AtomicTransaction transaction;
		const целое index = _getIndex(param);

		если (index < _next_slot) { // exists in this layer
			вернуть _slots[index].value;
		}

		вернуть _parent->get(param);
	}

	void reset(param_t param) override
	{
		const AtomicTransaction transaction;
		целое index = _getIndex(param);

		если (index < _next_slot) {
			_slots[index] = {UINT16_MAX, param_value_u{}};
			_sort();
			_next_slot--;
		}
	}

	void refresh(param_t param) override
	{
		_parent->refresh(param);
	}

	целое size() const override
	{
		вернуть _next_slot;
	}

	целое byteSize() const override
	{
		вернуть N_SLOTS * sizeof(Slot);
	}

private:
	struct Slot {
		param_t param;
		param_value_u value;
	};

	static целое _slotCompare(const void *a, const void *b)
	{
		вернуть ((целое)((Slot *)a)->param) - ((целое)((Slot *)b)->param);
	}

	void _sort()
	{
		qsort(_slots, N_SLOTS, sizeof(Slot), &_slotCompare);
	}

	целое _getIndex(param_t param) const
	{
		целое left = 0;
		целое right = _next_slot - 1;

		пока (left <= right) {
			целое mid = (left + right) / 2;

			если (_slots[mid].param == param) {
				вернуть mid;

			} иначе если (_slots[mid].param < param) {
				left = mid + 1;

			} иначе {
				right = mid - 1;
			}
		}

		вернуть _next_slot;
	}

	Slot _slots[N_SLOTS];
	целое _next_slot = 0;
};
