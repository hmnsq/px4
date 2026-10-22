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

#அந்தர்பாவ <px4_platform_common/atomic_bitset.h>
#அந்தர்பாவ "atomic_transaction.h"
#அந்தர்பாவ "param.h"
#அந்தர்பாவ <stdlib.h>

class ParamLayer
{
பொது:
	நிலையான constexpr param_t PARAM_மொத்தம் = அளவு(px4::parameters) / அளவு(param_info_s);

	ParamLayer(ParamLayer *parent = nullptr) : _parent(parent) {}

	ParamLayer(மாறிலி ParamLayer &) = delete;
	ParamLayer &operator=(மாறிலி ParamLayer &) = delete;
	ParamLayer(ParamLayer &&) = delete;
	ParamLayer &operator=(ParamLayer &&) = delete;


	மெய்நிகர் bool store(param_t param, param_மதிப்பு_u மதிப்பு) = 0;

	மெய்நிகர் bool contains(param_t param) மாறிலி = 0;

	மெய்நிகர் px4::AtomicBitset<PARAM_மொத்தம்> containedAsBitset() மாறிலி = 0;

	மெய்நிகர் param_மதிப்பு_u get(param_t param) மாறிலி = 0;

	மெய்நிகர் வெறுமை reset(param_t param) = 0;

	மெய்நிகர் வெறுமை refresh(param_t param) = 0;

	மெய்நிகர் முழு size() மாறிலி = 0;

	மெய்நிகர் முழு byteSize() மாறிலி = 0;

பாதுகாக்கப்பட்ட:
	ParamLayer *மாறிலி _parent;
};
