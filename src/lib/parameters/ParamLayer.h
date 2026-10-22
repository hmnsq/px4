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

#अन्तर्भाव <px4_platform_common/atomic_bitset.h>
#अन्तर्भाव "atomic_transaction.h"
#अन्तर्भाव "param.h"
#अन्तर्भाव <stdlib.h>

class ParamLayer
{
सार्वजनिक:
	स्थिर constexpr param_t PARAM_गिनती = परिमाण(px4::parameters) / परिमाण(param_info_s);

	ParamLayer(ParamLayer *parent = nullptr) : _parent(parent) {}

	ParamLayer(ध्रुव ParamLayer &) = delete;
	ParamLayer &operator=(ध्रुव ParamLayer &) = delete;
	ParamLayer(ParamLayer &&) = delete;
	ParamLayer &operator=(ParamLayer &&) = delete;


	आभास bool store(param_t param, param_मूल्य_u मूल्य) = 0;

	आभास bool contains(param_t param) ध्रुव = 0;

	आभास px4::AtomicBitset<PARAM_गिनती> containedAsBitset() ध्रुव = 0;

	आभास param_मूल्य_u get(param_t param) ध्रुव = 0;

	आभास शून्य reset(param_t param) = 0;

	आभास शून्य refresh(param_t param) = 0;

	आभास पूर्णाङ्क size() ध्रुव = 0;

	आभास पूर्णाङ्क byteSize() ध्रुव = 0;

रक्षित:
	ParamLayer *ध्रुव _parent;
};
