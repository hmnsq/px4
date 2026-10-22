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

#هدايت once

#ضمن <px4_platform_common/atomic_bitset.h>
#ضمن "atomic_transaction.h"
#ضمن "param.h"
#ضمن <stdlib.h>

class ParamLayer
{
عام:
	ساكن constexpr param_t PARAM_ڳڻپ = حجم(px4::parameters) / حجم(param_info_s);

	ParamLayer(ParamLayer *والد = nullptr) : والد_2(والد) {}

	ParamLayer(ثابت ParamLayer &) = delete;
	ParamLayer &عامل=(ثابت ParamLayer &) = delete;
	ParamLayer(ParamLayer &&) = delete;
	ParamLayer &عامل=(ParamLayer &&) = delete;


	مجازي bool store(param_t param, param_قدر_u قدر) = 0;

	مجازي bool شامل_آهي(param_t param) ثابت = 0;

	مجازي px4::AtomicBitset<PARAM_ڳڻپ> containedAsBitset() ثابت = 0;

	مجازي param_قدر_u get(param_t param) ثابت = 0;

	مجازي فراغ reset(param_t param) = 0;

	مجازي فراغ refresh(param_t param) = 0;

	مجازي صحيح سائيز() ثابت = 0;

	مجازي صحيح byte_سائيز() ثابت = 0;

محمي:
	ParamLayer *ثابت والد_2;
};
