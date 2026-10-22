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

#ಅನ್ತರ್ಭಾವ <px4_platform_common/atomic_bitset.h>
#ಅನ್ತರ್ಭಾವ "ParamLayer.h"

class ExhaustiveLayer : ಸಾರ್ವಜನಿಕ ParamLayer
{
ಸಾರ್ವಜನಿಕ:
	ExhaustiveLayer(ParamLayer *parent) : ParamLayer(parent)
	{
		// refresh all values
		ಪ್ರತಿ (param_t i = 0; i < PARAM_ಎಣಿಕೆ; i++) {
			ಮೌಲ್ಯಗಳು_3[i] = parent->get(i);
		}
	}

	bool store(param_t param, param_ಮೌಲ್ಯ_u ಮೌಲ್ಯ) override
	{
		ಯದಿ (param >= PARAM_ಎಣಿಕೆ) {
			ಪ್ರತ್ಯಾಗಮ false;
		}

		{
			ಧ್ರುವ AtomicTransaction transaction;
			ಮೌಲ್ಯಗಳು_3[param] = ಮೌಲ್ಯ;
			_ownership_set.set(param);
		}

		ಪ್ರತ್ಯಾಗಮ true;
	}

	bool contains(param_t param) ಧ್ರುವ override
	{
		ಪ್ರತ್ಯಾಗಮ param < PARAM_ಎಣಿಕೆ && _ownership_set[param];
	}

	px4::AtomicBitset<PARAM_ಎಣಿಕೆ> containedAsBitset() ಧ್ರುವ override
	{
		ಪ್ರತ್ಯಾಗಮ _ownership_set;
	}

	param_ಮೌಲ್ಯ_u get(param_t param) ಧ್ರುವ override
	{
		ಯದಿ (param >= PARAM_ಎಣಿಕೆ) {
			ಪ್ರತ್ಯಾಗಮ {0};
		}

		ಧ್ರುವ AtomicTransaction transaction;
		// We assume to have the correct values for all params, even without ownership.
		// We expect that refresh was called when underlying defaults changed
		ಪ್ರತ್ಯಾಗಮ ಮೌಲ್ಯಗಳು_3[param];
	}

	ಶೂನ್ಯ reset(param_t param) override
	{
		ಯದಿ (param >= PARAM_ಎಣಿಕೆ) {
			ಪ್ರತ್ಯಾಗಮ;
		}

		ಧ್ರುವ AtomicTransaction transaction;
		ಮೌಲ್ಯಗಳು_3[param] = _parent->get(param);
		_ownership_set.set(param, false);
	}

	ಶೂನ್ಯ refresh(param_t param) override
	{
		_parent->refresh(param);
		// in case we don't have ownership, and it changed below, we have to refresh our cache.
		{
			ಧ್ರುವ AtomicTransaction transaction;

			ಯದಿ (!contains(param)) {
				ಮೌಲ್ಯಗಳು_3[param] = _parent->get(param);
			}
		}
	}

	ಪೂರ್ಣಾಙ್ಕ size() ಧ್ರುವ override
	{
		ಪ್ರತ್ಯಾಗಮ _ownership_set.ಎಣಿಕೆ();
	}

	ಪೂರ್ಣಾಙ್ಕ byteSize() ಧ್ರುವ override
	{
		ಪ್ರತ್ಯಾಗಮ PARAM_ಎಣಿಕೆ * ಪರಿಮಾಣ(param_ಮೌಲ್ಯ_u);
	}

ಗುಪ್ತ:
	param_ಮೌಲ್ಯ_u ಮೌಲ್ಯಗಳು_3[PARAM_ಎಣಿಕೆ];
	px4::AtomicBitset<PARAM_ಎಣಿಕೆ> _ownership_set;
};
