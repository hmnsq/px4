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

ढाँचा <पूर्णाङ्क N_SLOTS>
class StaticSparseLayer : सार्वजनिक ParamLayer
{
सार्वजनिक:
	StaticSparseLayer(ParamLayer *parent) : ParamLayer(parent)
	{
		प्रति (पूर्णाङ्क i = 0; i < N_SLOTS; i++) {
			_slots[i] = {UINT16_MAX, param_मूल्य_u{}};
		}
	}

	आभास ~StaticSparseLayer() = स्वभाव;

	bool store(param_t param, param_मूल्य_u मूल्य) override
	{
		ध्रुव AtomicTransaction transaction;
		ध्रुव पूर्णाङ्क index = _getIndex(param);

		यदि (index < _next_slot) { // already exists
			_slots[index].मूल्य = मूल्य;

		} अन्यथा यदि (_next_slot < N_SLOTS) {
			_slots[_next_slot++] = {param, मूल्य};
			_sort();

		} अन्यथा {
			प्रत्यागम false;
		}

		प्रत्यागम true;
	}

	bool contains(param_t param) ध्रुव override
	{
		ध्रुव AtomicTransaction transaction;
		प्रत्यागम _getIndex(param) < _next_slot;
	}

	px4::AtomicBitset<PARAM_गिनती> containedAsBitset() ध्रुव override
	{
		px4::AtomicBitset<PARAM_गिनती> set;
		ध्रुव AtomicTransaction transaction;

		प्रति (पूर्णाङ्क i = 0; i < _next_slot; i++) {
			set.set(_slots[i].param);
		}

		प्रत्यागम set;
	}

	param_मूल्य_u get(param_t param) ध्रुव override
	{
		ध्रुव AtomicTransaction transaction;
		ध्रुव पूर्णाङ्क index = _getIndex(param);

		यदि (index < _next_slot) { // exists in this layer
			प्रत्यागम _slots[index].मूल्य;
		}

		प्रत्यागम _parent->get(param);
	}

	शून्य reset(param_t param) override
	{
		ध्रुव AtomicTransaction transaction;
		पूर्णाङ्क index = _getIndex(param);

		यदि (index < _next_slot) {
			_slots[index] = {UINT16_MAX, param_मूल्य_u{}};
			_sort();
			_next_slot--;
		}
	}

	शून्य refresh(param_t param) override
	{
		_parent->refresh(param);
	}

	पूर्णाङ्क size() ध्रुव override
	{
		प्रत्यागम _next_slot;
	}

	पूर्णाङ्क byteSize() ध्रुव override
	{
		प्रत्यागम N_SLOTS * परिमाण(Slot);
	}

गुप्त:
	संरचना Slot {
		param_t param;
		param_मूल्य_u मूल्य;
	};

	स्थिर पूर्णाङ्क _slotCompare(ध्रुव शून्य *a, ध्रुव शून्य *b)
	{
		प्रत्यागम ((पूर्णाङ्क)((Slot *)a)->param) - ((पूर्णाङ्क)((Slot *)b)->param);
	}

	शून्य _sort()
	{
		क्विक(_slots, N_SLOTS, परिमाण(Slot), &_slotCompare);
	}

	पूर्णाङ्क _getIndex(param_t param) ध्रुव
	{
		पूर्णाङ्क बायाँ = 0;
		पूर्णाङ्क दायाँ = _next_slot - 1;

		यावत् (बायाँ <= दायाँ) {
			पूर्णाङ्क mid = (बायाँ + दायाँ) / 2;

			यदि (_slots[mid].param == param) {
				प्रत्यागम mid;

			} अन्यथा यदि (_slots[mid].param < param) {
				बायाँ = mid + 1;

			} अन्यथा {
				दायाँ = mid - 1;
			}
		}

		प्रत्यागम _next_slot;
	}

	Slot _slots[N_SLOTS];
	पूर्णाङ्क _next_slot = 0;
};
