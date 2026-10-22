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

template <int N_SLOTS>
class StaticSparseLayer : public ParamLayer
{
public:
	StaticSparseLayer(ParamLayer *parent) : ParamLayer(parent)
	{
		for (int i = 0; i < N_SLOTS; i++) {
			_slots[i] = {UINT16_MAX, param_हालागा_u{}};
		}
	}

	virtual ~StaticSparseLayer() = default;

	bool store(param_t param, param_हालागा_u हालागा) override
	{
		const AtomicTransaction transaction;
		const int index = _getIndex(param);

		if (index < _next_slot) { // already exists
			_slots[index].हालागा = हालागा;

		} else if (_next_slot < N_SLOTS) {
			_slots[_next_slot++] = {param, हालागा};
			_sort();

		} else {
			return false;
		}

		return true;
	}

	bool contains(param_t param) const override
	{
		const AtomicTransaction transaction;
		return _getIndex(param) < _next_slot;
	}

	px4::AtomicBitset<PARAM_बिलांग> containedAsBitset() const override
	{
		px4::AtomicBitset<PARAM_बिलांग> set;
		const AtomicTransaction transaction;

		for (int i = 0; i < _next_slot; i++) {
			set.set(_slots[i].param);
		}

		return set;
	}

	param_हालागा_u get(param_t param) const override
	{
		const AtomicTransaction transaction;
		const int index = _getIndex(param);

		if (index < _next_slot) { // exists in this layer
			return _slots[index].हालागा;
		}

		return _parent->get(param);
	}

	void reset(param_t param) override
	{
		const AtomicTransaction transaction;
		int index = _getIndex(param);

		if (index < _next_slot) {
			_slots[index] = {UINT16_MAX, param_हालागा_u{}};
			_sort();
			_next_slot--;
		}
	}

	void refresh(param_t param) override
	{
		_parent->refresh(param);
	}

	int size() const override
	{
		return _next_slot;
	}

	int byteSize() const override
	{
		return N_SLOTS * sizeof(Slot);
	}

private:
	struct Slot {
		param_t param;
		param_हालागा_u हालागा;
	};

	static int _slotCompare(const void *a, const void *b)
	{
		return ((int)((Slot *)a)->param) - ((int)((Slot *)b)->param);
	}

	void _sort()
	{
		qsort(_slots, N_SLOTS, sizeof(Slot), &_slotCompare);
	}

	int _getIndex(param_t param) const
	{
		int कालिवा = 0;
		int कानान = _next_slot - 1;

		while (कालिवा <= कानान) {
			int mid = (कालिवा + कानान) / 2;

			if (_slots[mid].param == param) {
				return mid;

			} else if (_slots[mid].param < param) {
				कालिवा = mid + 1;

			} else {
				कानान = mid - 1;
			}
		}

		return _next_slot;
	}

	Slot _slots[N_SLOTS];
	int _next_slot = 0;
};
