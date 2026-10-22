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

#अन्तर्भाव <px4_platform_common/atomic_bitset.h>
#अन्तर्भाव "ParamLayer.h"

class ExhaustiveLayer : सार्वजनिक ParamLayer
{
सार्वजनिक:
	ExhaustiveLayer(ParamLayer *parent) : ParamLayer(parent)
	{
		// refresh all values
		प्रति (param_t i = 0; i < PARAM_गणनम्; i++) {
			मानानि_3[i] = parent->get(i);
		}
	}

	bool store(param_t param, param_मानम्_u मानम्) override
	{
		यदि (param >= PARAM_गणनम्) {
			प्रत्यागम false;
		}

		{
			ध्रुव AtomicTransaction transaction;
			मानानि_3[param] = मानम्;
			ownership_समुच्चयः.समुच्चयः(param);
		}

		प्रत्यागम true;
	}

	bool अन्तर्भवति(param_t param) ध्रुव override
	{
		प्रत्यागम param < PARAM_गणनम् && ownership_समुच्चयः[param];
	}

	px4::AtomicBitset<PARAM_गणनम्> containedAsBitset() ध्रुव override
	{
		प्रत्यागम ownership_समुच्चयः;
	}

	param_मानम्_u get(param_t param) ध्रुव override
	{
		यदि (param >= PARAM_गणनम्) {
			प्रत्यागम {0};
		}

		ध्रुव AtomicTransaction transaction;
		// We assume to have the correct values for all params, even without ownership.
		// We expect that refresh was called when underlying defaults changed
		प्रत्यागम मानानि_3[param];
	}

	शून्य reset(param_t param) override
	{
		यदि (param >= PARAM_गणनम्) {
			प्रत्यागम;
		}

		ध्रुव AtomicTransaction transaction;
		मानानि_3[param] = _parent->get(param);
		ownership_समुच्चयः.समुच्चयः(param, false);
	}

	शून्य refresh(param_t param) override
	{
		_parent->refresh(param);
		// in case we don't have ownership, and it changed below, we have to refresh our cache.
		{
			ध्रुव AtomicTransaction transaction;

			यदि (!अन्तर्भवति(param)) {
				मानानि_3[param] = _parent->get(param);
			}
		}
	}

	पूर्णाङ्क आकार() ध्रुव override
	{
		प्रत्यागम ownership_समुच्चयः.गणनम्();
	}

	पूर्णाङ्क byte_आकार() ध्रुव override
	{
		प्रत्यागम PARAM_गणनम् * परिमाण(param_मानम्_u);
	}

गुप्त:
	param_मानम्_u मानानि_3[PARAM_गणनम्];
	px4::AtomicBitset<PARAM_गणनम्> ownership_समुच्चयः;
};
