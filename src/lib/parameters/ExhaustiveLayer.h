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

#അന്തര്ഭാവ <px4_platform_common/atomic_bitset.h>
#അന്തര്ഭാവ "ParamLayer.h"

class ExhaustiveLayer : പൊതു ParamLayer
{
പൊതു:
	ExhaustiveLayer(ParamLayer *parent) : ParamLayer(parent)
	{
		// refresh all values
		വേണ്ടി (param_t i = 0; i < PARAM_എണ്ണം; i++) {
			മൂല്യങ്ങൾ_3[i] = parent->get(i);
		}
	}

	bool store(param_t param, param_മൂല്യം_u മൂല്യം) override
	{
		എങ്കിൽ (param >= PARAM_എണ്ണം) {
			തിരികെ false;
		}

		{
			സ്ഥിരം AtomicTransaction transaction;
			മൂല്യങ്ങൾ_3[param] = മൂല്യം;
			_ownership_set.set(param);
		}

		തിരികെ true;
	}

	bool contains(param_t param) സ്ഥിരം override
	{
		തിരികെ param < PARAM_എണ്ണം && _ownership_set[param];
	}

	px4::AtomicBitset<PARAM_എണ്ണം> containedAsBitset() സ്ഥിരം override
	{
		തിരികെ _ownership_set;
	}

	param_മൂല്യം_u get(param_t param) സ്ഥിരം override
	{
		എങ്കിൽ (param >= PARAM_എണ്ണം) {
			തിരികെ {0};
		}

		സ്ഥിരം AtomicTransaction transaction;
		// We assume to have the correct values for all params, even without ownership.
		// We expect that refresh was called when underlying defaults changed
		തിരികെ മൂല്യങ്ങൾ_3[param];
	}

	ശൂന്യം reset(param_t param) override
	{
		എങ്കിൽ (param >= PARAM_എണ്ണം) {
			തിരികെ;
		}

		സ്ഥിരം AtomicTransaction transaction;
		മൂല്യങ്ങൾ_3[param] = _parent->get(param);
		_ownership_set.set(param, false);
	}

	ശൂന്യം refresh(param_t param) override
	{
		_parent->refresh(param);
		// in case we don't have ownership, and it changed below, we have to refresh our cache.
		{
			സ്ഥിരം AtomicTransaction transaction;

			എങ്കിൽ (!contains(param)) {
				മൂല്യങ്ങൾ_3[param] = _parent->get(param);
			}
		}
	}

	പൂർണ്ണസംഖ്യ size() സ്ഥിരം override
	{
		തിരികെ _ownership_set.എണ്ണം();
	}

	പൂർണ്ണസംഖ്യ byteSize() സ്ഥിരം override
	{
		തിരികെ PARAM_എണ്ണം * വലുപ്പം(param_മൂല്യം_u);
	}

സ്വകാര്യ:
	param_മൂല്യം_u മൂല്യങ്ങൾ_3[PARAM_എണ്ണം];
	px4::AtomicBitset<PARAM_എണ്ണം> _ownership_set;
};
