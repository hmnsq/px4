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

#അന്തര്ഭാവ "ParamLayer.h"

class ConstLayer : പൊതു ParamLayer
{
പൊതു:

	ConstLayer() = സ്ഥിരസ്ഥിതി;

	bool store(param_t param, param_മൂല്യം_u മൂല്യം) override
	{
		തിരികെ false;
	}

	bool contains(param_t param) സ്ഥിരം override
	{
		തിരികെ param < PARAM_എണ്ണം;
	}

	px4::AtomicBitset<PARAM_എണ്ണം> containedAsBitset() സ്ഥിരം override
	{
		px4::AtomicBitset<PARAM_എണ്ണം> set;

		വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < PARAM_എണ്ണം; i++) {
			set.set(i);
		}

		തിരികെ set;
	}

	param_മൂല്യം_u get(param_t param) സ്ഥിരം override
	{
		എങ്കിൽ (param >= PARAM_എണ്ണം) {
			തിരികെ {0};
		}

		തിരികെ px4::parameters[param].val;
	}

	ശൂന്യം reset(param_t param) override
	{
		// Do nothing
	}

	ശൂന്യം refresh(param_t param) override
	{
		// Do nothing
	}

	പൂർണ്ണസംഖ്യ size() സ്ഥിരം override
	{
		തിരികെ PARAM_എണ്ണം;
	}

	പൂർണ്ണസംഖ്യ byteSize() സ്ഥിരം override
	{
		തിരികെ PARAM_എണ്ണം * വലുപ്പം(param_info_s);
	}
};
