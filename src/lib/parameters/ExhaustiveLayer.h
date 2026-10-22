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

#ಅನ್ತರ್ಭಾವ <px4_platform_common/atomic_bitset.h>
#ಅನ್ತರ್ಭಾವ "ParamLayer.h"

class ExhaustiveLayer : ಸಾರ್ವಜನಿಕ ParamLayer
{
ಸಾರ್ವಜನಿಕ:
	ExhaustiveLayer(ParamLayer *ಪೋಷಕ) : ParamLayer(ಪೋಷಕ)
	{
		// refresh all values
		ಗಾಗಿ (param_t i = 0; i < PARAM_ಎಣಿಕೆ; i++) {
			ಮೌಲ್ಯಗಳು_3[i] = ಪೋಷಕ->get(i);
		}
	}

	bool store(param_t param, param_ಮೌಲ್ಯ_u ಮೌಲ್ಯ) override
	{
		ಆದರೆ (param >= PARAM_ಎಣಿಕೆ) {
			ಹಿಂತಿರುಗಿ false;
		}

		{
			ಸ್ಥಿರಾಂಕ AtomicTransaction transaction;
			ಮೌಲ್ಯಗಳು_3[param] = ಮೌಲ್ಯ;
			ownership_ಗಣ.ಗಣ(param);
		}

		ಹಿಂತಿರುಗಿ true;
	}

	bool ಒಳಗೊಂಡಿದೆ(param_t param) ಸ್ಥಿರಾಂಕ override
	{
		ಹಿಂತಿರುಗಿ param < PARAM_ಎಣಿಕೆ && ownership_ಗಣ[param];
	}

	px4::AtomicBitset<PARAM_ಎಣಿಕೆ> containedAsBitset() ಸ್ಥಿರಾಂಕ override
	{
		ಹಿಂತಿರುಗಿ ownership_ಗಣ;
	}

	param_ಮೌಲ್ಯ_u get(param_t param) ಸ್ಥಿರಾಂಕ override
	{
		ಆದರೆ (param >= PARAM_ಎಣಿಕೆ) {
			ಹಿಂತಿರುಗಿ {0};
		}

		ಸ್ಥಿರಾಂಕ AtomicTransaction transaction;
		// We assume to have the correct values for all params, even without ownership.
		// We expect that refresh was called when underlying defaults changed
		ಹಿಂತಿರುಗಿ ಮೌಲ್ಯಗಳು_3[param];
	}

	ಶೂನ್ಯ reset(param_t param) override
	{
		ಆದರೆ (param >= PARAM_ಎಣಿಕೆ) {
			ಹಿಂತಿರುಗಿ;
		}

		ಸ್ಥಿರಾಂಕ AtomicTransaction transaction;
		ಮೌಲ್ಯಗಳು_3[param] = ಪೋಷಕ_2->get(param);
		ownership_ಗಣ.ಗಣ(param, false);
	}

	ಶೂನ್ಯ refresh(param_t param) override
	{
		ಪೋಷಕ_2->refresh(param);
		// in case we don't have ownership, and it changed below, we have to refresh our cache.
		{
			ಸ್ಥಿರಾಂಕ AtomicTransaction transaction;

			ಆದರೆ (!ಒಳಗೊಂಡಿದೆ(param)) {
				ಮೌಲ್ಯಗಳು_3[param] = ಪೋಷಕ_2->get(param);
			}
		}
	}

	ಪೂರ್ಣಾಂಕ ಗಾತ್ರ_2() ಸ್ಥಿರಾಂಕ override
	{
		ಹಿಂತಿರುಗಿ ownership_ಗಣ.ಎಣಿಕೆ_2();
	}

	ಪೂರ್ಣಾಂಕ byte_ಗಾತ್ರ() ಸ್ಥಿರಾಂಕ override
	{
		ಹಿಂತಿರುಗಿ PARAM_ಎಣಿಕೆ * ಗಾತ್ರ(param_ಮೌಲ್ಯ_u);
	}

ಖಾಸಗಿ:
	param_ಮೌಲ್ಯ_u ಮೌಲ್ಯಗಳು_3[PARAM_ಎಣಿಕೆ];
	px4::AtomicBitset<PARAM_ಎಣಿಕೆ> ownership_ಗಣ;
};
