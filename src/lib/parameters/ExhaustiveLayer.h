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
		प्रति (param_t i = 0; i < PARAM_मोजणी; i++) {
			मूल्ये_3[i] = parent->get(i);
		}
	}

	bool store(param_t param, param_मूल्य_u मूल्य) override
	{
		यदि (param >= PARAM_मोजणी) {
			प्रत्यागम false;
		}

		{
			ध्रुव AtomicTransaction transaction;
			मूल्ये_3[param] = मूल्य;
			_ownership_set.set(param);
		}

		प्रत्यागम true;
	}

	bool contains(param_t param) ध्रुव override
	{
		प्रत्यागम param < PARAM_मोजणी && _ownership_set[param];
	}

	px4::AtomicBitset<PARAM_मोजणी> containedAsBitset() ध्रुव override
	{
		प्रत्यागम _ownership_set;
	}

	param_मूल्य_u get(param_t param) ध्रुव override
	{
		यदि (param >= PARAM_मोजणी) {
			प्रत्यागम {0};
		}

		ध्रुव AtomicTransaction transaction;
		// We assume to have the correct values for all params, even without ownership.
		// We expect that refresh was called when underlying defaults changed
		प्रत्यागम मूल्ये_3[param];
	}

	शून्य reset(param_t param) override
	{
		यदि (param >= PARAM_मोजणी) {
			प्रत्यागम;
		}

		ध्रुव AtomicTransaction transaction;
		मूल्ये_3[param] = _parent->get(param);
		_ownership_set.set(param, false);
	}

	शून्य refresh(param_t param) override
	{
		_parent->refresh(param);
		// in case we don't have ownership, and it changed below, we have to refresh our cache.
		{
			ध्रुव AtomicTransaction transaction;

			यदि (!contains(param)) {
				मूल्ये_3[param] = _parent->get(param);
			}
		}
	}

	पूर्णाङ्क size() ध्रुव override
	{
		प्रत्यागम _ownership_set.मोजणी();
	}

	पूर्णाङ्क byteSize() ध्रुव override
	{
		प्रत्यागम PARAM_मोजणी * परिमाण(param_मूल्य_u);
	}

गुप्त:
	param_मूल्य_u मूल्ये_3[PARAM_मोजणी];
	px4::AtomicBitset<PARAM_मोजणी> _ownership_set;
};
