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

template <පූර්ණ_සංඛ්‍යා N_SLOTS>
class StaticSparseLayer : public ParamLayer
{
public:
	StaticSparseLayer(ParamLayer *parent) : ParamLayer(parent)
	{
		සඳහා (පූර්ණ_සංඛ්‍යා i = 0; i < N_SLOTS; i++) {
			_slots[i] = {UINT16_MAX, param_value_u{}};
		}
	}

	virtual ~StaticSparseLayer() = default;

	bool store(param_t param, param_value_u value) override
	{
		const AtomicTransaction transaction;
		const පූර්ණ_සංඛ්‍යා index = _getIndex(param);

		නම් (index < _next_slot) { // already exists
			_slots[index].value = value;

		} නැතහොත් නම් (_next_slot < N_SLOTS) {
			_slots[_next_slot++] = {param, value};
			_sort();

		} නැතහොත් {
			ආපසු false;
		}

		ආපසු true;
	}

	bool contains(param_t param) const override
	{
		const AtomicTransaction transaction;
		ආපසු _getIndex(param) < _next_slot;
	}

	px4::AtomicBitset<PARAM_COUNT> containedAsBitset() const override
	{
		px4::AtomicBitset<PARAM_COUNT> set;
		const AtomicTransaction transaction;

		සඳහා (පූර්ණ_සංඛ්‍යා i = 0; i < _next_slot; i++) {
			set.set(_slots[i].param);
		}

		ආපසු set;
	}

	param_value_u get(param_t param) const override
	{
		const AtomicTransaction transaction;
		const පූර්ණ_සංඛ්‍යා index = _getIndex(param);

		නම් (index < _next_slot) { // exists in this layer
			ආපසු _slots[index].value;
		}

		ආපසු _parent->get(param);
	}

	void reset(param_t param) override
	{
		const AtomicTransaction transaction;
		පූර්ණ_සංඛ්‍යා index = _getIndex(param);

		නම් (index < _next_slot) {
			_slots[index] = {UINT16_MAX, param_value_u{}};
			_sort();
			_next_slot--;
		}
	}

	void refresh(param_t param) override
	{
		_parent->refresh(param);
	}

	පූර්ණ_සංඛ්‍යා size() const override
	{
		ආපසු _next_slot;
	}

	පූර්ණ_සංඛ්‍යා byteSize() const override
	{
		ආපසු N_SLOTS * parimaa_na(Slot);
	}

private:
	sa_nracanaa Slot {
		param_t param;
		param_value_u value;
	};

	static පූර්ණ_සංඛ්‍යා _slotCompare(const void *a, const void *b)
	{
		ආපසු ((පූර්ණ_සංඛ්‍යා)((Slot *)a)->param) - ((පූර්ණ_සංඛ්‍යා)((Slot *)b)->param);
	}

	void _sort()
	{
		qsort(_slots, N_SLOTS, parimaa_na(Slot), &_slotCompare);
	}

	පූර්ණ_සංඛ්‍යා _getIndex(param_t param) const
	{
		පූර්ණ_සංඛ්‍යා left = 0;
		පූර්ණ_සංඛ්‍යා right = _next_slot - 1;

		අතරතුර (left <= right) {
			පූර්ණ_සංඛ්‍යා mid = (left + right) / 2;

			නම් (_slots[mid].param == param) {
				ආපසු mid;

			} නැතහොත් නම් (_slots[mid].param < param) {
				left = mid + 1;

			} නැතහොත් {
				right = mid - 1;
			}
		}

		ආපසු _next_slot;
	}

	Slot _slots[N_SLOTS];
	පූර්ණ_සංඛ්‍යා _next_slot = 0;
};
