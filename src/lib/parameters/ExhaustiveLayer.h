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

#अन्तर्भाव <px4_platform_common/atomic_bitset.h>
#अन्तर्भाव "ParamLayer.h"

class ExhaustiveLayer : सार्वजनिक ParamLayer
{
सार्वजनिक:
	ExhaustiveLayer(ParamLayer *parent) : ParamLayer(parent)
	{
		// refresh all values
		साठी (param_t i = 0; i < PARAM_मोजणी; i++) {
			मूल्ये_3[i] = parent->get(i);
		}
	}

	bool store(param_t param, param_मूल्य_u मूल्य) override
	{
		जर (param >= PARAM_मोजणी) {
			परत false;
		}

		{
			स्थिरांक AtomicTransaction transaction;
			मूल्ये_3[param] = मूल्य;
			_ownership_set.set(param);
		}

		परत true;
	}

	bool contains(param_t param) स्थिरांक override
	{
		परत param < PARAM_मोजणी && _ownership_set[param];
	}

	px4::AtomicBitset<PARAM_मोजणी> containedAsBitset() स्थिरांक override
	{
		परत _ownership_set;
	}

	param_मूल्य_u get(param_t param) स्थिरांक override
	{
		जर (param >= PARAM_मोजणी) {
			परत {0};
		}

		स्थिरांक AtomicTransaction transaction;
		// We assume to have the correct values for all params, even without ownership.
		// We expect that refresh was called when underlying defaults changed
		परत मूल्ये_3[param];
	}

	शून्य reset(param_t param) override
	{
		जर (param >= PARAM_मोजणी) {
			परत;
		}

		स्थिरांक AtomicTransaction transaction;
		मूल्ये_3[param] = _parent->get(param);
		_ownership_set.set(param, false);
	}

	शून्य refresh(param_t param) override
	{
		_parent->refresh(param);
		// in case we don't have ownership, and it changed below, we have to refresh our cache.
		{
			स्थिरांक AtomicTransaction transaction;

			जर (!contains(param)) {
				मूल्ये_3[param] = _parent->get(param);
			}
		}
	}

	पूर्णांक size() स्थिरांक override
	{
		परत _ownership_set.मोजणी();
	}

	पूर्णांक byteSize() स्थिरांक override
	{
		परत PARAM_मोजणी * आकार(param_मूल्य_u);
	}

खाजगी:
	param_मूल्य_u मूल्ये_3[PARAM_मोजणी];
	px4::AtomicBitset<PARAM_मोजणी> _ownership_set;
};
