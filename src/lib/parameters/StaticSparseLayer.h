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

#अन्तर्भाव <stdlib.h>
#अन्तर्भाव "ParamLayer.h"

ढाँचा <पूर्णांक N_SLOTS>
class StaticSparseLayer : सार्वजनिक ParamLayer
{
सार्वजनिक:
	StaticSparseLayer(ParamLayer *parent) : ParamLayer(parent)
	{
		साठी (पूर्णांक i = 0; i < N_SLOTS; i++) {
			_slots[i] = {UINT16_MAX, param_मूल्य_u{}};
		}
	}

	आभासी ~StaticSparseLayer() = पूर्वनिर्धारित;

	bool store(param_t param, param_मूल्य_u मूल्य) override
	{
		स्थिरांक AtomicTransaction transaction;
		स्थिरांक पूर्णांक index = _getIndex(param);

		जर (index < _next_slot) { // already exists
			_slots[index].मूल्य = मूल्य;

		} अन्यथा जर (_next_slot < N_SLOTS) {
			_slots[_next_slot++] = {param, मूल्य};
			_sort();

		} अन्यथा {
			परत false;
		}

		परत true;
	}

	bool contains(param_t param) स्थिरांक override
	{
		स्थिरांक AtomicTransaction transaction;
		परत _getIndex(param) < _next_slot;
	}

	px4::AtomicBitset<PARAM_मोजणी> containedAsBitset() स्थिरांक override
	{
		px4::AtomicBitset<PARAM_मोजणी> set;
		स्थिरांक AtomicTransaction transaction;

		साठी (पूर्णांक i = 0; i < _next_slot; i++) {
			set.set(_slots[i].param);
		}

		परत set;
	}

	param_मूल्य_u get(param_t param) स्थिरांक override
	{
		स्थिरांक AtomicTransaction transaction;
		स्थिरांक पूर्णांक index = _getIndex(param);

		जर (index < _next_slot) { // exists in this layer
			परत _slots[index].मूल्य;
		}

		परत _parent->get(param);
	}

	शून्य reset(param_t param) override
	{
		स्थिरांक AtomicTransaction transaction;
		पूर्णांक index = _getIndex(param);

		जर (index < _next_slot) {
			_slots[index] = {UINT16_MAX, param_मूल्य_u{}};
			_sort();
			_next_slot--;
		}
	}

	शून्य refresh(param_t param) override
	{
		_parent->refresh(param);
	}

	पूर्णांक size() स्थिरांक override
	{
		परत _next_slot;
	}

	पूर्णांक byteSize() स्थिरांक override
	{
		परत N_SLOTS * आकार(Slot);
	}

खाजगी:
	संरचना Slot {
		param_t param;
		param_मूल्य_u मूल्य;
	};

	स्थिर पूर्णांक _slotCompare(स्थिरांक शून्य *a, स्थिरांक शून्य *b)
	{
		परत ((पूर्णांक)((Slot *)a)->param) - ((पूर्णांक)((Slot *)b)->param);
	}

	शून्य _sort()
	{
		क्विक(_slots, N_SLOTS, आकार(Slot), &_slotCompare);
	}

	पूर्णांक _getIndex(param_t param) स्थिरांक
	{
		पूर्णांक डावा = 0;
		पूर्णांक उजवा = _next_slot - 1;

		पर्यंत (डावा <= उजवा) {
			पूर्णांक mid = (डावा + उजवा) / 2;

			जर (_slots[mid].param == param) {
				परत mid;

			} अन्यथा जर (_slots[mid].param < param) {
				डावा = mid + 1;

			} अन्यथा {
				उजवा = mid - 1;
			}
		}

		परत _next_slot;
	}

	Slot _slots[N_SLOTS];
	पूर्णांक _next_slot = 0;
};
