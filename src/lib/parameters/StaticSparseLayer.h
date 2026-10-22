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

#ਅਨ੍ਤਰ੍ਭਾਵ <stdlib.h>
#ਅਨ੍ਤਰ੍ਭਾਵ "ParamLayer.h"

ਸਾਂਚਾ <ਪੂਰਨ N_SLOTS>
class StaticSparseLayer : ਜਨਤਕ ParamLayer
{
ਜਨਤਕ:
	StaticSparseLayer(ParamLayer *ਮਾਪਾ) : ParamLayer(ਮਾਪਾ)
	{
		ਲਈ (ਪੂਰਨ i = 0; i < N_SLOTS; i++) {
			_slots[i] = {UINT16_MAX, param_ਮੁੱਲ_u{}};
		}
	}

	ਆਭਾਸੀ ~StaticSparseLayer() = ਮੂਲ;

	bool store(param_t param, param_ਮੁੱਲ_u ਮੁੱਲ) override
	{
		ਅਚਲ AtomicTransaction transaction;
		ਅਚਲ ਪੂਰਨ ਸੂਚਕਾਂਕ = get_ਸੂਚਕਾਂਕ(param);

		ਜੇ (ਸੂਚਕਾਂਕ < ਅਗਲਾ_ਥਾਂ) { // already exists
			_slots[ਸੂਚਕਾਂਕ].ਮੁੱਲ = ਮੁੱਲ;

		} ਨਹੀਂਤਾਂ ਜੇ (ਅਗਲਾ_ਥਾਂ < N_SLOTS) {
			_slots[ਅਗਲਾ_ਥਾਂ++] = {param, ਮੁੱਲ};
			ਛਾਂਟੋ();

		} ਨਹੀਂਤਾਂ {
			ਵਾਪਸ false;
		}

		ਵਾਪਸ true;
	}

	bool ਸ਼ਾਮਲ_ਹੈ(param_t param) ਅਚਲ override
	{
		ਅਚਲ AtomicTransaction transaction;
		ਵਾਪਸ get_ਸੂਚਕਾਂਕ(param) < ਅਗਲਾ_ਥਾਂ;
	}

	px4::AtomicBitset<PARAM_ਗਿਣਤੀ> containedAsBitset() ਅਚਲ override
	{
		px4::AtomicBitset<PARAM_ਗਿਣਤੀ> ਸੈੱਟ;
		ਅਚਲ AtomicTransaction transaction;

		ਲਈ (ਪੂਰਨ i = 0; i < ਅਗਲਾ_ਥਾਂ; i++) {
			ਸੈੱਟ.ਸੈੱਟ(_slots[i].param);
		}

		ਵਾਪਸ ਸੈੱਟ;
	}

	param_ਮੁੱਲ_u get(param_t param) ਅਚਲ override
	{
		ਅਚਲ AtomicTransaction transaction;
		ਅਚਲ ਪੂਰਨ ਸੂਚਕਾਂਕ = get_ਸੂਚਕਾਂਕ(param);

		ਜੇ (ਸੂਚਕਾਂਕ < ਅਗਲਾ_ਥਾਂ) { // exists in this layer
			ਵਾਪਸ _slots[ਸੂਚਕਾਂਕ].ਮੁੱਲ;
		}

		ਵਾਪਸ ਮਾਪਾ_2->get(param);
	}

	ਖਾਲੀ reset(param_t param) override
	{
		ਅਚਲ AtomicTransaction transaction;
		ਪੂਰਨ ਸੂਚਕਾਂਕ = get_ਸੂਚਕਾਂਕ(param);

		ਜੇ (ਸੂਚਕਾਂਕ < ਅਗਲਾ_ਥਾਂ) {
			_slots[ਸੂਚਕਾਂਕ] = {UINT16_MAX, param_ਮੁੱਲ_u{}};
			ਛਾਂਟੋ();
			ਅਗਲਾ_ਥਾਂ--;
		}
	}

	ਖਾਲੀ refresh(param_t param) override
	{
		ਮਾਪਾ_2->refresh(param);
	}

	ਪੂਰਨ ਮਾਪ() ਅਚਲ override
	{
		ਵਾਪਸ ਅਗਲਾ_ਥਾਂ;
	}

	ਪੂਰਨ byte_ਮਾਪ() ਅਚਲ override
	{
		ਵਾਪਸ N_SLOTS * ਆਕਾਰ(ਥਾਂ_2);
	}

ਨਿਜੀ:
	ਬਣਤਰ ਥਾਂ_2 {
		param_t param;
		param_ਮੁੱਲ_u ਮੁੱਲ;
	};

	ਸਥਿਰ ਪੂਰਨ ਥਾਂ_Compare(ਅਚਲ ਖਾਲੀ *a, ਅਚਲ ਖਾਲੀ *b)
	{
		ਵਾਪਸ ((ਪੂਰਨ)((ਥਾਂ_2 *)a)->param) - ((ਪੂਰਨ)((ਥਾਂ_2 *)b)->param);
	}

	ਖਾਲੀ ਛਾਂਟੋ()
	{
		qsort(_slots, N_SLOTS, ਆਕਾਰ(ਥਾਂ_2), &ਥਾਂ_Compare);
	}

	ਪੂਰਨ get_ਸੂਚਕਾਂਕ(param_t param) ਅਚਲ
	{
		ਪੂਰਨ ਖੱਬਾ = 0;
		ਪੂਰਨ ਸੱਜਾ = ਅਗਲਾ_ਥਾਂ - 1;

		ਜਦੋਂ (ਖੱਬਾ <= ਸੱਜਾ) {
			ਪੂਰਨ mid = (ਖੱਬਾ + ਸੱਜਾ) / 2;

			ਜੇ (_slots[mid].param == param) {
				ਵਾਪਸ mid;

			} ਨਹੀਂਤਾਂ ਜੇ (_slots[mid].param < param) {
				ਖੱਬਾ = mid + 1;

			} ਨਹੀਂਤਾਂ {
				ਸੱਜਾ = mid - 1;
			}
		}

		ਵਾਪਸ ਅਗਲਾ_ਥਾਂ;
	}

	ਥਾਂ_2 _slots[N_SLOTS];
	ਪੂਰਨ ਅਗਲਾ_ਥਾਂ = 0;
};
