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

#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ <stdlib.h>
#ಅನ್ತರ್ಭಾವ "ParamLayer.h"

ಮಾದರಿ <ಪೂರ್ಣಾಂಕ N_SLOTS>
class StaticSparseLayer : ಸಾರ್ವಜನಿಕ ParamLayer
{
ಸಾರ್ವಜನಿಕ:
	StaticSparseLayer(ParamLayer *ಪೋಷಕ) : ParamLayer(ಪೋಷಕ)
	{
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < N_SLOTS; i++) {
			_slots[i] = {UINT16_MAX, param_ಮೌಲ್ಯ_u{}};
		}
	}

	ವಾಸ್ತವಿಕ ~StaticSparseLayer() = ಪೂರ್ವನಿಯೋಜಿತ;

	bool store(param_t param, param_ಮೌಲ್ಯ_u ಮೌಲ್ಯ) override
	{
		ಸ್ಥಿರಾಂಕ AtomicTransaction transaction;
		ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ = get_ಸೂಚ್ಯಂಕ(param);

		ಆದರೆ (ಸೂಚ್ಯಂಕ < ಮುಂದಿನ_ಸ್ಥಾನ) { // already exists
			_slots[ಸೂಚ್ಯಂಕ].ಮೌಲ್ಯ = ಮೌಲ್ಯ;

		} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಮುಂದಿನ_ಸ್ಥಾನ < N_SLOTS) {
			_slots[ಮುಂದಿನ_ಸ್ಥಾನ++] = {param, ಮೌಲ್ಯ};
			ವಿಂಗಡಿಸು();

		} ಇಲ್ಲದಿದ್ದರೆ {
			ಹಿಂತಿರುಗಿ false;
		}

		ಹಿಂತಿರುಗಿ true;
	}

	bool ಒಳಗೊಂಡಿದೆ(param_t param) ಸ್ಥಿರಾಂಕ override
	{
		ಸ್ಥಿರಾಂಕ AtomicTransaction transaction;
		ಹಿಂತಿರುಗಿ get_ಸೂಚ್ಯಂಕ(param) < ಮುಂದಿನ_ಸ್ಥಾನ;
	}

	px4::AtomicBitset<PARAM_ಎಣಿಕೆ> containedAsBitset() ಸ್ಥಿರಾಂಕ override
	{
		px4::AtomicBitset<PARAM_ಎಣಿಕೆ> ಗಣ;
		ಸ್ಥಿರಾಂಕ AtomicTransaction transaction;

		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ಮುಂದಿನ_ಸ್ಥಾನ; i++) {
			ಗಣ.ಗಣ(_slots[i].param);
		}

		ಹಿಂತಿರುಗಿ ಗಣ;
	}

	param_ಮೌಲ್ಯ_u get(param_t param) ಸ್ಥಿರಾಂಕ override
	{
		ಸ್ಥಿರಾಂಕ AtomicTransaction transaction;
		ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ = get_ಸೂಚ್ಯಂಕ(param);

		ಆದರೆ (ಸೂಚ್ಯಂಕ < ಮುಂದಿನ_ಸ್ಥಾನ) { // exists in this layer
			ಹಿಂತಿರುಗಿ _slots[ಸೂಚ್ಯಂಕ].ಮೌಲ್ಯ;
		}

		ಹಿಂತಿರುಗಿ ಪೋಷಕ_2->get(param);
	}

	ಶೂನ್ಯ reset(param_t param) override
	{
		ಸ್ಥಿರಾಂಕ AtomicTransaction transaction;
		ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ = get_ಸೂಚ್ಯಂಕ(param);

		ಆದರೆ (ಸೂಚ್ಯಂಕ < ಮುಂದಿನ_ಸ್ಥಾನ) {
			_slots[ಸೂಚ್ಯಂಕ] = {UINT16_MAX, param_ಮೌಲ್ಯ_u{}};
			ವಿಂಗಡಿಸು();
			ಮುಂದಿನ_ಸ್ಥಾನ--;
		}
	}

	ಶೂನ್ಯ refresh(param_t param) override
	{
		ಪೋಷಕ_2->refresh(param);
	}

	ಪೂರ್ಣಾಂಕ ಗಾತ್ರ_2() ಸ್ಥಿರಾಂಕ override
	{
		ಹಿಂತಿರುಗಿ ಮುಂದಿನ_ಸ್ಥಾನ;
	}

	ಪೂರ್ಣಾಂಕ byte_ಗಾತ್ರ() ಸ್ಥಿರಾಂಕ override
	{
		ಹಿಂತಿರುಗಿ N_SLOTS * ಗಾತ್ರ(ಸ್ಥಾನ_2);
	}

ಖಾಸಗಿ:
	ರಚನೆ ಸ್ಥಾನ_2 {
		param_t param;
		param_ಮೌಲ್ಯ_u ಮೌಲ್ಯ;
	};

	ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಸ್ಥಾನ_Compare(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *a, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *b)
	{
		ಹಿಂತಿರುಗಿ ((ಪೂರ್ಣಾಂಕ)((ಸ್ಥಾನ_2 *)a)->param) - ((ಪೂರ್ಣಾಂಕ)((ಸ್ಥಾನ_2 *)b)->param);
	}

	ಶೂನ್ಯ ವಿಂಗಡಿಸು()
	{
		qsort(_slots, N_SLOTS, ಗಾತ್ರ(ಸ್ಥಾನ_2), &ಸ್ಥಾನ_Compare);
	}

	ಪೂರ್ಣಾಂಕ get_ಸೂಚ್ಯಂಕ(param_t param) ಸ್ಥಿರಾಂಕ
	{
		ಪೂರ್ಣಾಂಕ ಎಡ_2 = 0;
		ಪೂರ್ಣಾಂಕ ಬಲ_2 = ಮುಂದಿನ_ಸ್ಥಾನ - 1;

		ಆಗ (ಎಡ_2 <= ಬಲ_2) {
			ಪೂರ್ಣಾಂಕ mid = (ಎಡ_2 + ಬಲ_2) / 2;

			ಆದರೆ (_slots[mid].param == param) {
				ಹಿಂತಿರುಗಿ mid;

			} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (_slots[mid].param < param) {
				ಎಡ_2 = mid + 1;

			} ಇಲ್ಲದಿದ್ದರೆ {
				ಬಲ_2 = mid - 1;
			}
		}

		ಹಿಂತಿರುಗಿ ಮುಂದಿನ_ಸ್ಥಾನ;
	}

	ಸ್ಥಾನ_2 _slots[N_SLOTS];
	ಪೂರ್ಣಾಂಕ ಮುಂದಿನ_ಸ್ಥಾನ = 0;
};
