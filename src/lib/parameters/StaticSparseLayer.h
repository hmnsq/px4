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

#సూచన once

#అన్తర్భావ <stdlib.h>
#అన్తర్భావ "ParamLayer.h"

మూస <పూర్ణాంకం N_SLOTS>
class StaticSparseLayer : బహిరంగ ParamLayer
{
బహిరంగ:
	StaticSparseLayer(ParamLayer *మాతృ) : ParamLayer(మాతృ)
	{
		కొరకు (పూర్ణాంకం i = 0; i < N_SLOTS; i++) {
			_slots[i] = {UINT16_MAX, param_విలువ_u{}};
		}
	}

	మాయా ~StaticSparseLayer() = అప్రమేయ;

	bool store(param_t param, param_విలువ_u విలువ) override
	{
		స్థిరరాశి AtomicTransaction transaction;
		స్థిరరాశి పూర్ణాంకం సూచీ = get_సూచీ(param);

		ఒకవేళ (సూచీ < తదుపరి_స్థానం) { // already exists
			_slots[సూచీ].విలువ = విలువ;

		} లేకపోతే ఒకవేళ (తదుపరి_స్థానం < N_SLOTS) {
			_slots[తదుపరి_స్థానం++] = {param, విలువ};
			క్రమీకరించు();

		} లేకపోతే {
			తిరిగి false;
		}

		తిరిగి true;
	}

	bool కలిగియుంది(param_t param) స్థిరరాశి override
	{
		స్థిరరాశి AtomicTransaction transaction;
		తిరిగి get_సూచీ(param) < తదుపరి_స్థానం;
	}

	px4::AtomicBitset<PARAM_లెక్క> containedAsBitset() స్థిరరాశి override
	{
		px4::AtomicBitset<PARAM_లెక్క> సమితి;
		స్థిరరాశి AtomicTransaction transaction;

		కొరకు (పూర్ణాంకం i = 0; i < తదుపరి_స్థానం; i++) {
			సమితి.సమితి(_slots[i].param);
		}

		తిరిగి సమితి;
	}

	param_విలువ_u get(param_t param) స్థిరరాశి override
	{
		స్థిరరాశి AtomicTransaction transaction;
		స్థిరరాశి పూర్ణాంకం సూచీ = get_సూచీ(param);

		ఒకవేళ (సూచీ < తదుపరి_స్థానం) { // exists in this layer
			తిరిగి _slots[సూచీ].విలువ;
		}

		తిరిగి మాతృ_2->get(param);
	}

	శూన్య reset(param_t param) override
	{
		స్థిరరాశి AtomicTransaction transaction;
		పూర్ణాంకం సూచీ = get_సూచీ(param);

		ఒకవేళ (సూచీ < తదుపరి_స్థానం) {
			_slots[సూచీ] = {UINT16_MAX, param_విలువ_u{}};
			క్రమీకరించు();
			తదుపరి_స్థానం--;
		}
	}

	శూన్య refresh(param_t param) override
	{
		మాతృ_2->refresh(param);
	}

	పూర్ణాంకం కొలత() స్థిరరాశి override
	{
		తిరిగి తదుపరి_స్థానం;
	}

	పూర్ణాంకం byte_కొలత() స్థిరరాశి override
	{
		తిరిగి N_SLOTS * పరిమాణం(స్థానం_2);
	}

అంతరంగ:
	నిర్మాణం స్థానం_2 {
		param_t param;
		param_విలువ_u విలువ;
	};

	స్థిర పూర్ణాంకం స్థానం_Compare(స్థిరరాశి శూన్య *a, స్థిరరాశి శూన్య *b)
	{
		తిరిగి ((పూర్ణాంకం)((స్థానం_2 *)a)->param) - ((పూర్ణాంకం)((స్థానం_2 *)b)->param);
	}

	శూన్య క్రమీకరించు()
	{
		qsort(_slots, N_SLOTS, పరిమాణం(స్థానం_2), &స్థానం_Compare);
	}

	పూర్ణాంకం get_సూచీ(param_t param) స్థిరరాశి
	{
		పూర్ణాంకం ఎడమ_2 = 0;
		పూర్ణాంకం కుడి_2 = తదుపరి_స్థానం - 1;

		అయితే (ఎడమ_2 <= కుడి_2) {
			పూర్ణాంకం mid = (ఎడమ_2 + కుడి_2) / 2;

			ఒకవేళ (_slots[mid].param == param) {
				తిరిగి mid;

			} లేకపోతే ఒకవేళ (_slots[mid].param < param) {
				ఎడమ_2 = mid + 1;

			} లేకపోతే {
				కుడి_2 = mid - 1;
			}
		}

		తిరిగి తదుపరి_స్థానం;
	}

	స్థానం_2 _slots[N_SLOTS];
	పూర్ణాంకం తదుపరి_స్థానం = 0;
};
