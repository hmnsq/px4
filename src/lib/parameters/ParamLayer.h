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
#ਅਨ੍ਤਰ੍ਭਾਵ "atomic_transaction.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "param.h"
#ਅਨ੍ਤਰ੍ਭਾਵ <stdlib.h>

class ParamLayer
{
ਜਨਤਕ:
	ਸਥਿਰ constexpr param_t PARAM_ਗਿਣਤੀ = ਆਕਾਰ(px4::parameters) / ਆਕਾਰ(param_info_s);

	ParamLayer(ParamLayer *ਮਾਪਾ = nullptr) : ਮਾਪਾ_2(ਮਾਪਾ) {}

	ParamLayer(ਅਚਲ ParamLayer &) = delete;
	ParamLayer &ਸੰਚਾਲਕ=(ਅਚਲ ParamLayer &) = delete;
	ParamLayer(ParamLayer &&) = delete;
	ParamLayer &ਸੰਚਾਲਕ=(ParamLayer &&) = delete;


	ਆਭਾਸੀ bool store(param_t param, param_ਮੁੱਲ_u ਮੁੱਲ) = 0;

	ਆਭਾਸੀ bool ਸ਼ਾਮਲ_ਹੈ(param_t param) ਅਚਲ = 0;

	ਆਭਾਸੀ px4::AtomicBitset<PARAM_ਗਿਣਤੀ> containedAsBitset() ਅਚਲ = 0;

	ਆਭਾਸੀ param_ਮੁੱਲ_u get(param_t param) ਅਚਲ = 0;

	ਆਭਾਸੀ ਖਾਲੀ reset(param_t param) = 0;

	ਆਭਾਸੀ ਖਾਲੀ refresh(param_t param) = 0;

	ਆਭਾਸੀ ਪੂਰਨ ਮਾਪ() ਅਚਲ = 0;

	ਆਭਾਸੀ ਪੂਰਨ byte_ਮਾਪ() ਅਚਲ = 0;

ਸੁਰੱਖਿਅਤ:
	ParamLayer *ਅਚਲ ਮਾਪਾ_2;
};
