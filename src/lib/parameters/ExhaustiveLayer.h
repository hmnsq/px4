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

#include <px4_platform_common/atomic_bitset.h>
#include "ParamLayer.h"

class ExhaustiveLayer : public ParamLayer
{
public:
	ExhaustiveLayer(ParamLayer *parent) : ParamLayer(parent)
	{
		// refresh all values
		for (param_t i = 0; i < PARAM_बिलांग; i++) {
			मगा_हालागा_3[i] = parent->get(i);
		}
	}

	bool store(param_t param, param_हालागा_u हालागा) override
	{
		if (param >= PARAM_बिलांग) {
			return false;
		}

		{
			const AtomicTransaction transaction;
			मगा_हालागा_3[param] = हालागा;
			_ownership_set.set(param);
		}

		return true;
	}

	bool contains(param_t param) const override
	{
		return param < PARAM_बिलांग && _ownership_set[param];
	}

	px4::AtomicBitset<PARAM_बिलांग> containedAsBitset() const override
	{
		return _ownership_set;
	}

	param_हालागा_u get(param_t param) const override
	{
		if (param >= PARAM_बिलांग) {
			return {0};
		}

		const AtomicTransaction transaction;
		// We assume to have the correct values for all params, even without ownership.
		// We expect that refresh was called when underlying defaults changed
		return मगा_हालागा_3[param];
	}

	void reset(param_t param) override
	{
		if (param >= PARAM_बिलांग) {
			return;
		}

		const AtomicTransaction transaction;
		मगा_हालागा_3[param] = _parent->get(param);
		_ownership_set.set(param, false);
	}

	void refresh(param_t param) override
	{
		_parent->refresh(param);
		// in case we don't have ownership, and it changed below, we have to refresh our cache.
		{
			const AtomicTransaction transaction;

			if (!contains(param)) {
				मगा_हालागा_3[param] = _parent->get(param);
			}
		}
	}

	int size() const override
	{
		return _ownership_set.बिलांग();
	}

	int byteSize() const override
	{
		return PARAM_बिलांग * sizeof(param_हालागा_u);
	}

private:
	param_हालागा_u मगा_हालागा_3[PARAM_बिलांग];
	px4::AtomicBitset<PARAM_बिलांग> _ownership_set;
};
