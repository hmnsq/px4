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
#അന്തര്ഭാവ "atomic_transaction.h"
#അന്തര്ഭാവ "param.h"
#അന്തര്ഭാവ <stdlib.h>

class ParamLayer
{
പൊതു:
	സ്ഥിതം constexpr param_t PARAM_എണ്ണം = വലുപ്പം(px4::parameters) / വലുപ്പം(param_info_s);

	ParamLayer(ParamLayer *parent = nullptr) : _parent(parent) {}

	ParamLayer(സ്ഥിരം ParamLayer &) = delete;
	ParamLayer &operator=(സ്ഥിരം ParamLayer &) = delete;
	ParamLayer(ParamLayer &&) = delete;
	ParamLayer &operator=(ParamLayer &&) = delete;


	മായാ bool store(param_t param, param_മൂല്യം_u മൂല്യം) = 0;

	മായാ bool contains(param_t param) സ്ഥിരം = 0;

	മായാ px4::AtomicBitset<PARAM_എണ്ണം> containedAsBitset() സ്ഥിരം = 0;

	മായാ param_മൂല്യം_u get(param_t param) സ്ഥിരം = 0;

	മായാ ശൂന്യം reset(param_t param) = 0;

	മായാ ശൂന്യം refresh(param_t param) = 0;

	മായാ പൂർണ്ണസംഖ്യ size() സ്ഥിരം = 0;

	മായാ പൂർണ്ണസംഖ്യ byteSize() സ്ഥിരം = 0;

സംരക്ഷിത:
	ParamLayer *സ്ഥിരം _parent;
};
