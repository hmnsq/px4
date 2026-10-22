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

#అన్తర్భావ "ParamLayer.h"

class ConstLayer : బహిరంగ ParamLayer
{
బహిరంగ:

	ConstLayer() = అప్రమేయ;

	bool store(param_t param, param_విలువ_u విలువ) override
	{
		తిరిగి false;
	}

	bool కలిగియుంది(param_t param) స్థిరరాశి override
	{
		తిరిగి param < PARAM_లెక్క;
	}

	px4::AtomicBitset<PARAM_లెక్క> containedAsBitset() స్థిరరాశి override
	{
		px4::AtomicBitset<PARAM_లెక్క> సమితి;

		కొరకు (పూర్ణాంకం i = 0; i < PARAM_లెక్క; i++) {
			సమితి.సమితి(i);
		}

		తిరిగి సమితి;
	}

	param_విలువ_u get(param_t param) స్థిరరాశి override
	{
		ఒకవేళ (param >= PARAM_లెక్క) {
			తిరిగి {0};
		}

		తిరిగి px4::parameters[param].val;
	}

	శూన్య reset(param_t param) override
	{
		// Do nothing
	}

	శూన్య refresh(param_t param) override
	{
		// Do nothing
	}

	పూర్ణాంకం కొలత() స్థిరరాశి override
	{
		తిరిగి PARAM_లెక్క;
	}

	పూర్ణాంకం byte_కొలత() స్థిరరాశి override
	{
		తిరిగి PARAM_లెక్క * పరిమాణం(param_info_s);
	}
};
