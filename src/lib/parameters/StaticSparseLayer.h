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

#निर्देशः once

#अन्तर्भाव <stdlib.h>
#अन्तर्भाव "ParamLayer.h"

प्रतिरूपम् <पूर्णाङ्क N_SLOTS>
class StaticSparseLayer : सार्वजनिक ParamLayer
{
सार्वजनिक:
	StaticSparseLayer(ParamLayer *parent) : ParamLayer(parent)
	{
		प्रति (पूर्णाङ्क i = 0; i < N_SLOTS; i++) {
			_slots[i] = {UINT16_MAX, param_मानम्_u{}};
		}
	}

	आभास ~StaticSparseLayer() = स्वभाव;

	bool store(param_t param, param_मानम्_u मानम्) override
	{
		ध्रुव AtomicTransaction transaction;
		ध्रुव पूर्णाङ्क सूचकांक = get_सूचकांक(param);

		यदि (सूचकांक < अग्र_slot) { // already exists
			_slots[सूचकांक].मानम् = मानम्;

		} अन्यथा यदि (अग्र_slot < N_SLOTS) {
			_slots[अग्र_slot++] = {param, मानम्};
			क्रमय();

		} अन्यथा {
			प्रत्यागम false;
		}

		प्रत्यागम true;
	}

	bool अन्तर्भवति(param_t param) ध्रुव override
	{
		ध्रुव AtomicTransaction transaction;
		प्रत्यागम get_सूचकांक(param) < अग्र_slot;
	}

	px4::AtomicBitset<PARAM_गणनम्> containedAsBitset() ध्रुव override
	{
		px4::AtomicBitset<PARAM_गणनम्> समुच्चयः;
		ध्रुव AtomicTransaction transaction;

		प्रति (पूर्णाङ्क i = 0; i < अग्र_slot; i++) {
			समुच्चयः.समुच्चयः(_slots[i].param);
		}

		प्रत्यागम समुच्चयः;
	}

	param_मानम्_u get(param_t param) ध्रुव override
	{
		ध्रुव AtomicTransaction transaction;
		ध्रुव पूर्णाङ्क सूचकांक = get_सूचकांक(param);

		यदि (सूचकांक < अग्र_slot) { // exists in this layer
			प्रत्यागम _slots[सूचकांक].मानम्;
		}

		प्रत्यागम _parent->get(param);
	}

	शून्य reset(param_t param) override
	{
		ध्रुव AtomicTransaction transaction;
		पूर्णाङ्क सूचकांक = get_सूचकांक(param);

		यदि (सूचकांक < अग्र_slot) {
			_slots[सूचकांक] = {UINT16_MAX, param_मानम्_u{}};
			क्रमय();
			अग्र_slot--;
		}
	}

	शून्य refresh(param_t param) override
	{
		_parent->refresh(param);
	}

	पूर्णाङ्क आकार() ध्रुव override
	{
		प्रत्यागम अग्र_slot;
	}

	पूर्णाङ्क byte_आकार() ध्रुव override
	{
		प्रत्यागम N_SLOTS * परिमाण(Slot);
	}

गुप्त:
	संरचना Slot {
		param_t param;
		param_मानम्_u मानम्;
	};

	स्थिर पूर्णाङ्क _slotCompare(ध्रुव शून्य *a, ध्रुव शून्य *b)
	{
		प्रत्यागम ((पूर्णाङ्क)((Slot *)a)->param) - ((पूर्णाङ्क)((Slot *)b)->param);
	}

	शून्य क्रमय()
	{
		शीघ्रं_क्रमय(_slots, N_SLOTS, परिमाण(Slot), &_slotCompare);
	}

	पूर्णाङ्क get_सूचकांक(param_t param) ध्रुव
	{
		पूर्णाङ्क वामः = 0;
		पूर्णाङ्क दक्षिणः = अग्र_slot - 1;

		यावत् (वामः <= दक्षिणः) {
			पूर्णाङ्क mid = (वामः + दक्षिणः) / 2;

			यदि (_slots[mid].param == param) {
				प्रत्यागम mid;

			} अन्यथा यदि (_slots[mid].param < param) {
				वामः = mid + 1;

			} अन्यथा {
				दक्षिणः = mid - 1;
			}
		}

		प्रत्यागम अग्र_slot;
	}

	Slot _slots[N_SLOTS];
	पूर्णाङ्क अग्र_slot = 0;
};
