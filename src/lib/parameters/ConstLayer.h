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

#ಅನ್ತರ್ಭಾವ "ParamLayer.h"

class ConstLayer : ಸಾರ್ವಜನಿಕ ParamLayer
{
ಸಾರ್ವಜನಿಕ:

	ConstLayer() = ಸ್ವಭಾವ;

	bool store(param_t param, param_ಮೌಲ್ಯ_u ಮೌಲ್ಯ) override
	{
		ಪ್ರತ್ಯಾಗಮ false;
	}

	bool contains(param_t param) ಧ್ರುವ override
	{
		ಪ್ರತ್ಯಾಗಮ param < PARAM_ಎಣಿಕೆ;
	}

	px4::AtomicBitset<PARAM_ಎಣಿಕೆ> containedAsBitset() ಧ್ರುವ override
	{
		px4::AtomicBitset<PARAM_ಎಣಿಕೆ> set;

		ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < PARAM_ಎಣಿಕೆ; i++) {
			set.set(i);
		}

		ಪ್ರತ್ಯಾಗಮ set;
	}

	param_ಮೌಲ್ಯ_u get(param_t param) ಧ್ರುವ override
	{
		ಯದಿ (param >= PARAM_ಎಣಿಕೆ) {
			ಪ್ರತ್ಯಾಗಮ {0};
		}

		ಪ್ರತ್ಯಾಗಮ px4::parameters[param].val;
	}

	ಶೂನ್ಯ reset(param_t param) override
	{
		// Do nothing
	}

	ಶೂನ್ಯ refresh(param_t param) override
	{
		// Do nothing
	}

	ಪೂರ್ಣಾಙ್ಕ size() ಧ್ರುವ override
	{
		ಪ್ರತ್ಯಾಗಮ PARAM_ಎಣಿಕೆ;
	}

	ಪೂರ್ಣಾಙ್ಕ byteSize() ಧ್ರುವ override
	{
		ಪ್ರತ್ಯಾಗಮ PARAM_ಎಣಿಕೆ * ಪರಿಮಾಣ(param_info_s);
	}
};
