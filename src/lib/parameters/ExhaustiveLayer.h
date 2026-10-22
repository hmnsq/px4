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

#అన్తర్భావ <px4_platform_common/atomic_bitset.h>
#అన్తర్భావ "ParamLayer.h"

class ExhaustiveLayer : బహిరంగ ParamLayer
{
బహిరంగ:
	ExhaustiveLayer(ParamLayer *మాతృ) : ParamLayer(మాతృ)
	{
		// refresh all values
		కొరకు (param_t i = 0; i < PARAM_లెక్క; i++) {
			విలువలు_3[i] = మాతృ->get(i);
		}
	}

	bool store(param_t param, param_విలువ_u విలువ) override
	{
		ఒకవేళ (param >= PARAM_లెక్క) {
			తిరిగి false;
		}

		{
			స్థిరరాశి AtomicTransaction transaction;
			విలువలు_3[param] = విలువ;
			ownership_సమితి.సమితి(param);
		}

		తిరిగి true;
	}

	bool కలిగియుంది(param_t param) స్థిరరాశి override
	{
		తిరిగి param < PARAM_లెక్క && ownership_సమితి[param];
	}

	px4::AtomicBitset<PARAM_లెక్క> containedAsBitset() స్థిరరాశి override
	{
		తిరిగి ownership_సమితి;
	}

	param_విలువ_u get(param_t param) స్థిరరాశి override
	{
		ఒకవేళ (param >= PARAM_లెక్క) {
			తిరిగి {0};
		}

		స్థిరరాశి AtomicTransaction transaction;
		// We assume to have the correct values for all params, even without ownership.
		// We expect that refresh was called when underlying defaults changed
		తిరిగి విలువలు_3[param];
	}

	శూన్య reset(param_t param) override
	{
		ఒకవేళ (param >= PARAM_లెక్క) {
			తిరిగి;
		}

		స్థిరరాశి AtomicTransaction transaction;
		విలువలు_3[param] = మాతృ_2->get(param);
		ownership_సమితి.సమితి(param, false);
	}

	శూన్య refresh(param_t param) override
	{
		మాతృ_2->refresh(param);
		// in case we don't have ownership, and it changed below, we have to refresh our cache.
		{
			స్థిరరాశి AtomicTransaction transaction;

			ఒకవేళ (!కలిగియుంది(param)) {
				విలువలు_3[param] = మాతృ_2->get(param);
			}
		}
	}

	పూర్ణాంకం కొలత() స్థిరరాశి override
	{
		తిరిగి ownership_సమితి.లెక్క();
	}

	పూర్ణాంకం byte_కొలత() స్థిరరాశి override
	{
		తిరిగి PARAM_లెక్క * పరిమాణం(param_విలువ_u);
	}

అంతరంగ:
	param_విలువ_u విలువలు_3[PARAM_లెక్క];
	px4::AtomicBitset<PARAM_లెక్క> ownership_సమితి;
};
