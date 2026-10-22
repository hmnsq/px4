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

#અન્તર્ભાવ <px4_platform_common/atomic_bitset.h>
#અન્તર્ભાવ "ParamLayer.h"

class ExhaustiveLayer : જાહેર ParamLayer
{
જાહેર:
	ExhaustiveLayer(ParamLayer *parent) : ParamLayer(parent)
	{
		// refresh all values
		માટે (param_t i = 0; i < PARAM_ગણતરી; i++) {
			કિંમતો_3[i] = parent->get(i);
		}
	}

	bool store(param_t param, param_કિંમત_u કિંમત) override
	{
		જો (param >= PARAM_ગણતરી) {
			પાછા false;
		}

		{
			અચલ AtomicTransaction transaction;
			કિંમતો_3[param] = કિંમત;
			_ownership_set.set(param);
		}

		પાછા true;
	}

	bool contains(param_t param) અચલ override
	{
		પાછા param < PARAM_ગણતરી && _ownership_set[param];
	}

	px4::AtomicBitset<PARAM_ગણતરી> containedAsBitset() અચલ override
	{
		પાછા _ownership_set;
	}

	param_કિંમત_u get(param_t param) અચલ override
	{
		જો (param >= PARAM_ગણતરી) {
			પાછા {0};
		}

		અચલ AtomicTransaction transaction;
		// We assume to have the correct values for all params, even without ownership.
		// We expect that refresh was called when underlying defaults changed
		પાછા કિંમતો_3[param];
	}

	શૂન્ય reset(param_t param) override
	{
		જો (param >= PARAM_ગણતરી) {
			પાછા;
		}

		અચલ AtomicTransaction transaction;
		કિંમતો_3[param] = _parent->get(param);
		_ownership_set.set(param, false);
	}

	શૂન્ય refresh(param_t param) override
	{
		_parent->refresh(param);
		// in case we don't have ownership, and it changed below, we have to refresh our cache.
		{
			અચલ AtomicTransaction transaction;

			જો (!contains(param)) {
				કિંમતો_3[param] = _parent->get(param);
			}
		}
	}

	પૂર્ણાંક size() અચલ override
	{
		પાછા _ownership_set.ગણતરી();
	}

	પૂર્ણાંક byteSize() અચલ override
	{
		પાછા PARAM_ગણતરી * કદ(param_કિંમત_u);
	}

ખાનગી:
	param_કિંમત_u કિંમતો_3[PARAM_ગણતરી];
	px4::AtomicBitset<PARAM_ગણતરી> _ownership_set;
};
