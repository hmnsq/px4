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

#ଅନ୍ତର୍ଭାଵ <stdlib.h>
#ଅନ୍ତର୍ଭାଵ "ParamLayer.h"

template <ପୂର୍ଣ୍ଣସଂଖ୍ୟା N_SLOTS>
class StaticSparseLayer : ସର୍ବସାଧାରଣ ParamLayer
{
ସର୍ବସାଧାରଣ:
	StaticSparseLayer(ParamLayer *parent) : ParamLayer(parent)
	{
		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < N_SLOTS; i++) {
			_slots[i] = {UINT16_MAX, param_ମୂଲ୍ୟ_u{}};
		}
	}

	ଆଭାସୀ ~StaticSparseLayer() = ପୂର୍ବନିର୍ଦ୍ଧାରିତ;

	bool store(param_t param, param_ମୂଲ୍ୟ_u ମୂଲ୍ୟ) override
	{
		ଅଚଳ AtomicTransaction transaction;
		ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା index = _getIndex(param);

		ଯଦି (index < _next_slot) { // already exists
			_slots[index].ମୂଲ୍ୟ = ମୂଲ୍ୟ;

		} ଅନ୍ୟଥା ଯଦି (_next_slot < N_SLOTS) {
			_slots[_next_slot++] = {param, ମୂଲ୍ୟ};
			_sort();

		} ଅନ୍ୟଥା {
			ଫେରନ୍ତୁ false;
		}

		ଫେରନ୍ତୁ true;
	}

	bool contains(param_t param) ଅଚଳ override
	{
		ଅଚଳ AtomicTransaction transaction;
		ଫେରନ୍ତୁ _getIndex(param) < _next_slot;
	}

	px4::AtomicBitset<PARAM_ଗଣନା> containedAsBitset() ଅଚଳ override
	{
		px4::AtomicBitset<PARAM_ଗଣନା> set;
		ଅଚଳ AtomicTransaction transaction;

		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < _next_slot; i++) {
			set.set(_slots[i].param);
		}

		ଫେରନ୍ତୁ set;
	}

	param_ମୂଲ୍ୟ_u get(param_t param) ଅଚଳ override
	{
		ଅଚଳ AtomicTransaction transaction;
		ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା index = _getIndex(param);

		ଯଦି (index < _next_slot) { // exists in this layer
			ଫେରନ୍ତୁ _slots[index].ମୂଲ୍ୟ;
		}

		ଫେରନ୍ତୁ _parent->get(param);
	}

	ଶୂନ୍ୟ reset(param_t param) override
	{
		ଅଚଳ AtomicTransaction transaction;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା index = _getIndex(param);

		ଯଦି (index < _next_slot) {
			_slots[index] = {UINT16_MAX, param_ମୂଲ୍ୟ_u{}};
			_sort();
			_next_slot--;
		}
	}

	ଶୂନ୍ୟ refresh(param_t param) override
	{
		_parent->refresh(param);
	}

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା size() ଅଚଳ override
	{
		ଫେରନ୍ତୁ _next_slot;
	}

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା byteSize() ଅଚଳ override
	{
		ଫେରନ୍ତୁ N_SLOTS * ଆକାର(Slot);
	}

ବ୍ୟକ୍ତିଗତ:
	ସଂରଚନା Slot {
		param_t param;
		param_ମୂଲ୍ୟ_u ମୂଲ୍ୟ;
	};

	ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା _slotCompare(ଅଚଳ ଶୂନ୍ୟ *a, ଅଚଳ ଶୂନ୍ୟ *b)
	{
		ଫେରନ୍ତୁ ((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)((Slot *)a)->param) - ((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)((Slot *)b)->param);
	}

	ଶୂନ୍ୟ _sort()
	{
		qsort(_slots, N_SLOTS, ଆକାର(Slot), &_slotCompare);
	}

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା _getIndex(param_t param) ଅଚଳ
	{
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା ବାମ = 0;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଡାହାଣ = _next_slot - 1;

		ଯେତେବେଳେ (ବାମ <= ଡାହାଣ) {
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା mid = (ବାମ + ଡାହାଣ) / 2;

			ଯଦି (_slots[mid].param == param) {
				ଫେରନ୍ତୁ mid;

			} ଅନ୍ୟଥା ଯଦି (_slots[mid].param < param) {
				ବାମ = mid + 1;

			} ଅନ୍ୟଥା {
				ଡାହାଣ = mid - 1;
			}
		}

		ଫେରନ୍ତୁ _next_slot;
	}

	Slot _slots[N_SLOTS];
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା _next_slot = 0;
};
