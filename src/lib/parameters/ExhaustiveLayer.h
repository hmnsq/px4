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

#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ <px4_platform_common/atomic_bitset.h>
#ਅਨ੍ਤਰ੍ਭਾਵ "ParamLayer.h"

class ExhaustiveLayer : ਜਨਤਕ ParamLayer
{
ਜਨਤਕ:
	ExhaustiveLayer(ParamLayer *ਮਾਪਾ) : ParamLayer(ਮਾਪਾ)
	{
		// refresh all values
		ਲਈ (param_t i = 0; i < PARAM_ਗਿਣਤੀ; i++) {
			ਮੁੱਲਾਂ_3[i] = ਮਾਪਾ->get(i);
		}
	}

	bool store(param_t param, param_ਮੁੱਲ_u ਮੁੱਲ) override
	{
		ਜੇ (param >= PARAM_ਗਿਣਤੀ) {
			ਵਾਪਸ false;
		}

		{
			ਅਚਲ AtomicTransaction transaction;
			ਮੁੱਲਾਂ_3[param] = ਮੁੱਲ;
			ownership_ਸੈੱਟ.ਸੈੱਟ(param);
		}

		ਵਾਪਸ true;
	}

	bool ਸ਼ਾਮਲ_ਹੈ(param_t param) ਅਚਲ override
	{
		ਵਾਪਸ param < PARAM_ਗਿਣਤੀ && ownership_ਸੈੱਟ[param];
	}

	px4::AtomicBitset<PARAM_ਗਿਣਤੀ> containedAsBitset() ਅਚਲ override
	{
		ਵਾਪਸ ownership_ਸੈੱਟ;
	}

	param_ਮੁੱਲ_u get(param_t param) ਅਚਲ override
	{
		ਜੇ (param >= PARAM_ਗਿਣਤੀ) {
			ਵਾਪਸ {0};
		}

		ਅਚਲ AtomicTransaction transaction;
		// We assume to have the correct values for all params, even without ownership.
		// We expect that refresh was called when underlying defaults changed
		ਵਾਪਸ ਮੁੱਲਾਂ_3[param];
	}

	ਖਾਲੀ reset(param_t param) override
	{
		ਜੇ (param >= PARAM_ਗਿਣਤੀ) {
			ਵਾਪਸ;
		}

		ਅਚਲ AtomicTransaction transaction;
		ਮੁੱਲਾਂ_3[param] = ਮਾਪਾ_2->get(param);
		ownership_ਸੈੱਟ.ਸੈੱਟ(param, false);
	}

	ਖਾਲੀ refresh(param_t param) override
	{
		ਮਾਪਾ_2->refresh(param);
		// in case we don't have ownership, and it changed below, we have to refresh our cache.
		{
			ਅਚਲ AtomicTransaction transaction;

			ਜੇ (!ਸ਼ਾਮਲ_ਹੈ(param)) {
				ਮੁੱਲਾਂ_3[param] = ਮਾਪਾ_2->get(param);
			}
		}
	}

	ਪੂਰਨ ਮਾਪ() ਅਚਲ override
	{
		ਵਾਪਸ ownership_ਸੈੱਟ.ਗਿਣਤੀ_2();
	}

	ਪੂਰਨ byte_ਮਾਪ() ਅਚਲ override
	{
		ਵਾਪਸ PARAM_ਗਿਣਤੀ * ਆਕਾਰ(param_ਮੁੱਲ_u);
	}

ਨਿਜੀ:
	param_ਮੁੱਲ_u ਮੁੱਲਾਂ_3[PARAM_ਗਿਣਤੀ];
	px4::AtomicBitset<PARAM_ਗਿਣਤੀ> ownership_ਸੈੱਟ;
};
