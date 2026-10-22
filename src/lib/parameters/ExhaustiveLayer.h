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

#ضمن <px4_platform_common/atomic_bitset.h>
#ضمن "ParamLayer.h"

class ExhaustiveLayer : عام ParamLayer
{
عام:
	ExhaustiveLayer(ParamLayer *parent) : ParamLayer(parent)
	{
		// refresh all values
		لكل (param_t i = 0; i < PARAM_ژماردن; i++) {
			بەهاکان_3[i] = parent->get(i);
		}
	}

	bool store(param_t param, param_بەها_u بەها) override
	{
		إذا (param >= PARAM_ژماردن) {
			أرجع false;
		}

		{
			ثابت AtomicTransaction transaction;
			بەهاکان_3[param] = بەها;
			_ownership_set.set(param);
		}

		أرجع true;
	}

	bool contains(param_t param) ثابت override
	{
		أرجع param < PARAM_ژماردن && _ownership_set[param];
	}

	px4::AtomicBitset<PARAM_ژماردن> containedAsBitset() ثابت override
	{
		أرجع _ownership_set;
	}

	param_بەها_u get(param_t param) ثابت override
	{
		إذا (param >= PARAM_ژماردن) {
			أرجع {0};
		}

		ثابت AtomicTransaction transaction;
		// We assume to have the correct values for all params, even without ownership.
		// We expect that refresh was called when underlying defaults changed
		أرجع بەهاکان_3[param];
	}

	فراغ reset(param_t param) override
	{
		إذا (param >= PARAM_ژماردن) {
			أرجع;
		}

		ثابت AtomicTransaction transaction;
		بەهاکان_3[param] = _parent->get(param);
		_ownership_set.set(param, false);
	}

	فراغ refresh(param_t param) override
	{
		_parent->refresh(param);
		// in case we don't have ownership, and it changed below, we have to refresh our cache.
		{
			ثابت AtomicTransaction transaction;

			إذا (!contains(param)) {
				بەهاکان_3[param] = _parent->get(param);
			}
		}
	}

	صحيح size() ثابت override
	{
		أرجع _ownership_set.ژماردن();
	}

	صحيح byteSize() ثابت override
	{
		أرجع PARAM_ژماردن * حجم(param_بەها_u);
	}

خاص:
	param_بەها_u بەهاکان_3[PARAM_ژماردن];
	px4::AtomicBitset<PARAM_ژماردن> _ownership_set;
};
