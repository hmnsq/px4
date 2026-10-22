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

template <ཧྲིལ N_SLOTS>
class StaticSparseLayer : public ParamLayer
{
public:
	StaticSparseLayer(ParamLayer *parent) : ParamLayer(parent)
	{
		ཆེད་དུ (ཧྲིལ i = 0; i < N_SLOTS; i++) {
			_slots[i] = {UINT16_MAX, param_value_u{}};
		}
	}

	virtual ~StaticSparseLayer() = default;

	bool store(param_t param, param_value_u value) override
	{
		const AtomicTransaction transaction;
		const ཧྲིལ index = _getIndex(param);

		གལ་ཏེ (index < _next_slot) { // already exists
			_slots[index].value = value;

		} དེ་མིན གལ་ཏེ (_next_slot < N_SLOTS) {
			_slots[_next_slot++] = {param, value};
			_sort();

		} དེ་མིན {
			ཕྱིར false;
		}

		ཕྱིར true;
	}

	bool contains(param_t param) const override
	{
		const AtomicTransaction transaction;
		ཕྱིར _getIndex(param) < _next_slot;
	}

	px4::AtomicBitset<PARAM_COUNT> containedAsBitset() const override
	{
		px4::AtomicBitset<PARAM_COUNT> set;
		const AtomicTransaction transaction;

		ཆེད་དུ (ཧྲིལ i = 0; i < _next_slot; i++) {
			set.set(_slots[i].param);
		}

		ཕྱིར set;
	}

	param_value_u get(param_t param) const override
	{
		const AtomicTransaction transaction;
		const ཧྲིལ index = _getIndex(param);

		གལ་ཏེ (index < _next_slot) { // exists in this layer
			ཕྱིར _slots[index].value;
		}

		ཕྱིར _parent->get(param);
	}

	void reset(param_t param) override
	{
		const AtomicTransaction transaction;
		ཧྲིལ index = _getIndex(param);

		གལ་ཏེ (index < _next_slot) {
			_slots[index] = {UINT16_MAX, param_value_u{}};
			_sort();
			_next_slot--;
		}
	}

	void refresh(param_t param) override
	{
		_parent->refresh(param);
	}

	ཧྲིལ size() const override
	{
		ཕྱིར _next_slot;
	}

	ཧྲིལ byteSize() const override
	{
		ཕྱིར N_SLOTS * parimaa_na(Slot);
	}

private:
	sa_nracanaa Slot {
		param_t param;
		param_value_u value;
	};

	static ཧྲིལ _slotCompare(const void *a, const void *b)
	{
		ཕྱིར ((ཧྲིལ)((Slot *)a)->param) - ((ཧྲིལ)((Slot *)b)->param);
	}

	void _sort()
	{
		qsort(_slots, N_SLOTS, parimaa_na(Slot), &_slotCompare);
	}

	ཧྲིལ _getIndex(param_t param) const
	{
		ཧྲིལ left = 0;
		ཧྲིལ right = _next_slot - 1;

		སྐབས (left <= right) {
			ཧྲིལ mid = (left + right) / 2;

			གལ་ཏེ (_slots[mid].param == param) {
				ཕྱིར mid;

			} དེ་མིན གལ་ཏེ (_slots[mid].param < param) {
				left = mid + 1;

			} དེ་མིན {
				right = mid - 1;
			}
		}

		ཕྱིར _next_slot;
	}

	Slot _slots[N_SLOTS];
	ཧྲིལ _next_slot = 0;
};
