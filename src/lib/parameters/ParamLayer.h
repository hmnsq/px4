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

#অন্তর্ভাব <px4_platform_common/atomic_bitset.h>
#অন্তর্ভাব "atomic_transaction.h"
#অন্তর্ভাব "param.h"
#অন্তর্ভাব <stdlib.h>

class ParamLayer
{
সর্বজনীন:
	স্থির constexpr param_t PARAM_গুনতি = আকার(px4::parameters) / আকার(param_info_s);

	ParamLayer(ParamLayer *parent = nullptr) : _parent(parent) {}

	ParamLayer(ধ্রুবক ParamLayer &) = delete;
	ParamLayer &operator=(ধ্রুবক ParamLayer &) = delete;
	ParamLayer(ParamLayer &&) = delete;
	ParamLayer &operator=(ParamLayer &&) = delete;


	আভাসী bool store(param_t param, param_মান_u মান) = 0;

	আভাসী bool contains(param_t param) ধ্রুবক = 0;

	আভাসী px4::AtomicBitset<PARAM_গুনতি> containedAsBitset() ধ্রুবক = 0;

	আভাসী param_মান_u get(param_t param) ধ্রুবক = 0;

	আভাসী শূন্য reset(param_t param) = 0;

	আভাসী শূন্য refresh(param_t param) = 0;

	আভাসী পূর্ণসংখ্যা আকার_2() ধ্রুবক = 0;

	আভাসী পূর্ণসংখ্যা byte_আকার() ধ্রুবক = 0;

সুরক্ষিত:
	ParamLayer *ধ্রুবক _parent;
};
