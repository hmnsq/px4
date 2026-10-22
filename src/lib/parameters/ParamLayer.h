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

#અન્તર્ભાવ <px4_platform_common/atomic_bitset.h>
#અન્તર્ભાવ "atomic_transaction.h"
#અન્તર્ભાવ "param.h"
#અન્તર્ભાવ <stdlib.h>

class ParamLayer
{
જાહેર:
	સ્થિર constexpr param_t PARAM_ગણતરી = કદ(px4::parameters) / કદ(param_info_s);

	ParamLayer(ParamLayer *parent = nullptr) : _parent(parent) {}

	ParamLayer(અચલ ParamLayer &) = delete;
	ParamLayer &operator=(અચલ ParamLayer &) = delete;
	ParamLayer(ParamLayer &&) = delete;
	ParamLayer &operator=(ParamLayer &&) = delete;


	આભાસી bool store(param_t param, param_કિંમત_u કિંમત) = 0;

	આભાસી bool contains(param_t param) અચલ = 0;

	આભાસી px4::AtomicBitset<PARAM_ગણતરી> containedAsBitset() અચલ = 0;

	આભાસી param_કિંમત_u get(param_t param) અચલ = 0;

	આભાસી શૂન્ય reset(param_t param) = 0;

	આભાસી શૂન્ય refresh(param_t param) = 0;

	આભાસી પૂર્ણાંક size() અચલ = 0;

	આભાસી પૂર્ણાંક byteSize() અચલ = 0;

સુરક્ષિત:
	ParamLayer *અચલ _parent;
};
