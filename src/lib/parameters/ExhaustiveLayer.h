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

#অন্তর্ভাৱ <px4_platform_common/atomic_bitset.h>
#অন্তর্ভাৱ "ParamLayer.h"

class ExhaustiveLayer : সার্ৱজনিক ParamLayer
{
সার্ৱজনিক:
	ExhaustiveLayer(ParamLayer *parent) : ParamLayer(parent)
	{
		// refresh all values
		বাবে (param_t i = 0; i < PARAM_গুনতি; i++) {
			মানসমূহ_3[i] = parent->get(i);
		}
	}

	bool store(param_t param, param_মান_u মান) override
	{
		যদি (param >= PARAM_গুনতি) {
			ঘূৰাই false;
		}

		{
			ধ্রুৱ AtomicTransaction transaction;
			মানসমূহ_3[param] = মান;
			_ownership_set.set(param);
		}

		ঘূৰাই true;
	}

	bool contains(param_t param) ধ্রুৱ override
	{
		ঘূৰাই param < PARAM_গুনতি && _ownership_set[param];
	}

	px4::AtomicBitset<PARAM_গুনতি> containedAsBitset() ধ্রুৱ override
	{
		ঘূৰাই _ownership_set;
	}

	param_মান_u get(param_t param) ধ্রুৱ override
	{
		যদি (param >= PARAM_গুনতি) {
			ঘূৰাই {0};
		}

		ধ্রুৱ AtomicTransaction transaction;
		// We assume to have the correct values for all params, even without ownership.
		// We expect that refresh was called when underlying defaults changed
		ঘূৰাই মানসমূহ_3[param];
	}

	শূন্য reset(param_t param) override
	{
		যদি (param >= PARAM_গুনতি) {
			ঘূৰাই;
		}

		ধ্রুৱ AtomicTransaction transaction;
		মানসমূহ_3[param] = _parent->get(param);
		_ownership_set.set(param, false);
	}

	শূন্য refresh(param_t param) override
	{
		_parent->refresh(param);
		// in case we don't have ownership, and it changed below, we have to refresh our cache.
		{
			ধ্রুৱ AtomicTransaction transaction;

			যদি (!contains(param)) {
				মানসমূহ_3[param] = _parent->get(param);
			}
		}
	}

	পূর্ণাঙ্ক size() ধ্রুৱ override
	{
		ঘূৰাই _ownership_set.গুনতি();
	}

	পূর্ণাঙ্ক byteSize() ধ্রুৱ override
	{
		ঘূৰাই PARAM_গুনতি * পরিমাণ(param_মান_u);
	}

গুপ্ত:
	param_মান_u মানসমূহ_3[PARAM_গুনতি];
	px4::AtomicBitset<PARAM_গুনতি> _ownership_set;
};
