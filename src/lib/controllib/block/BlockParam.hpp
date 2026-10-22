/****************************************************************************
 *
 *   Copyright (C) 2012-2017 PX4 Development Team. All rights reserved.
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

/**
 * @file BlockParam.hpp
 *
 * Controller library code
 */

#pragma once

#অন্তর্ভাৱ <containers/List.hpp>
#অন্তর্ভাৱ <px4_platform_common/defines.h>
#অন্তর্ভাৱ <parameters/param.h>

নামাকাশ control
{

শ্ৰেণী Block;

// A base class for block params that enables traversing linked list.
শ্ৰেণী BlockParamBase : সার্ৱজনিক List_গ্ৰন্থি<BlockParamBase *>
{
সার্ৱজনিক:
	/**
	 * Instantiate a block param base.
	 *
	 * @param parent_prefix Set to true to include the parent name in the parameter name
	 */
	BlockParamBase(Block *parent, ধ্রুৱ ৱর্ণ *name, তার্কিক parent_prefix = সঁচা);
	আভাস ~BlockParamBase() = স্ৱভাৱ;

	আভাস তার্কিক update() = 0;
	ধ্রুৱ ৱর্ণ *getName() ধ্রুৱ { ঘূৰাই param_name(_handle); }

রক্ষিত:
	param_t _handle{PARAM_INVALID};
};

// Parameters that are tied to blocks for updating and naming.
template <শ্ৰেণী T>
শ্ৰেণী __EXPORT BlockParam final : সার্ৱজনিক BlockParamBase
{
সার্ৱজনিক:
	BlockParam(Block *block, ধ্রুৱ ৱর্ণ *name, তার্কিক parent_prefix = সঁচা);
	BlockParam(Block *block, ধ্রুৱ ৱর্ণ *name, তার্কিক parent_prefix, T &extern_val);

	~BlockParam() override = স্ৱভাৱ;

	// no copy, assignment, move, move assignment
	BlockParam(ধ্রুৱ BlockParam &) = delete;
	BlockParam &operator=(ধ্রুৱ BlockParam &) = delete;
	BlockParam(BlockParam &&) = delete;
	BlockParam &operator=(BlockParam &&) = delete;

	T get() ধ্রুৱ { ঘূৰাই _val; }

	// Store the parameter value to the parameter storage (@see param_set())
	তার্কিক commit() { ঘূৰাই (param_set(_handle, &_val) == PX4_OK); }

	// Store the parameter value to the parameter storage, w/o notifying the system (@see param_set_no_notification())
	তার্কিক commit_no_notification() { ঘূৰাই (param_set_no_notification(_handle, &_val) == PX4_OK); }

	শূন্য set(T val) { _val = val; }

	তার্কিক update() override { ঘূৰাই (param_get(_handle, &_val) == PX4_OK); }

রক্ষিত:
	T _val;
};

template <>
তার্কিক BlockParam<তার্কিক>::update();

প্রকারনির্দেশ BlockParam<ভিন্ন> BlockParamFloat;
প্রকারনির্দেশ BlockParam<int32_t> BlockParamInt;
প্রকারনির্দেশ BlockParam<তার্কিক> BlockParamBool;
প্রকারনির্দেশ BlockParam<ভিন্ন &> BlockParamExtFloat;
প্রকারনির্দেশ BlockParam<int32_t &> BlockParamExtInt;

} // namespace control
