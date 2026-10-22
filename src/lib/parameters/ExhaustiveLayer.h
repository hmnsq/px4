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

#ہدایت once

#شامل <px4_platform_common/atomic_bitset.h>
#شامل "ParamLayer.h"

class ExhaustiveLayer : عوامی ParamLayer
{
عوامی:
	ExhaustiveLayer(ParamLayer *والد) : ParamLayer(والد)
	{
		// refresh all values
		برائے (param_t i = 0; i < PARAM_گنتی; i++) {
			اقدار_3[i] = والد->get(i);
		}
	}

	bool store(param_t param, param_قدر_u قدر) override
	{
		اگر (param >= PARAM_گنتی) {
			واپس false;
		}

		{
			ثابت AtomicTransaction transaction;
			اقدار_3[param] = قدر;
			ownership_سیٹ.سیٹ(param);
		}

		واپس true;
	}

	bool شامل_ہے(param_t param) ثابت override
	{
		واپس param < PARAM_گنتی && ownership_سیٹ[param];
	}

	px4::AtomicBitset<PARAM_گنتی> containedAsBitset() ثابت override
	{
		واپس ownership_سیٹ;
	}

	param_قدر_u get(param_t param) ثابت override
	{
		اگر (param >= PARAM_گنتی) {
			واپس {0};
		}

		ثابت AtomicTransaction transaction;
		// We assume to have the correct values for all params, even without ownership.
		// We expect that refresh was called when underlying defaults changed
		واپس اقدار_3[param];
	}

	خالی reset(param_t param) override
	{
		اگر (param >= PARAM_گنتی) {
			واپس;
		}

		ثابت AtomicTransaction transaction;
		اقدار_3[param] = والد_2->get(param);
		ownership_سیٹ.سیٹ(param, false);
	}

	خالی refresh(param_t param) override
	{
		والد_2->refresh(param);
		// in case we don't have ownership, and it changed below, we have to refresh our cache.
		{
			ثابت AtomicTransaction transaction;

			اگر (!شامل_ہے(param)) {
				اقدار_3[param] = والد_2->get(param);
			}
		}
	}

	صحیح سائز() ثابت override
	{
		واپس ownership_سیٹ.گنتی();
	}

	صحیح byte_سائز() ثابت override
	{
		واپس PARAM_گنتی * حجم(param_قدر_u);
	}

نجی:
	param_قدر_u اقدار_3[PARAM_گنتی];
	px4::AtomicBitset<PARAM_گنتی> ownership_سیٹ;
};
