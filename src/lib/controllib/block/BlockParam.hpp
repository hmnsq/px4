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

#ଅନ୍ତର୍ଭାଵ <containers/List.hpp>
#ଅନ୍ତର୍ଭାଵ <px4_platform_common/defines.h>
#ଅନ୍ତର୍ଭାଵ <parameters/param.h>

ନାମାକାଶ control
{

ଶ୍ରେଣୀ Block;

// A base class for block params that enables traversing linked list.
ଶ୍ରେଣୀ BlockParamBase : ସର୍ବସାଧାରଣ List_ଗଣ୍ଠି<BlockParamBase *>
{
ସର୍ବସାଧାରଣ:
	/**
	 * Instantiate a block param base.
	 *
	 * @param parent_prefix Set to true to include the parent name in the parameter name
	 */
	BlockParamBase(Block *parent, ଅଚଳ ଅକ୍ଷର *name, ତାର୍କିକ parent_prefix = ସତ୍ୟ);
	ଆଭାସୀ ~BlockParamBase() = ପୂର୍ବନିର୍ଦ୍ଧାରିତ;

	ଆଭାସୀ ତାର୍କିକ update() = 0;
	ଅଚଳ ଅକ୍ଷର *getName() ଅଚଳ { ଫେରନ୍ତୁ param_name(_handle); }

ସୁରକ୍ଷିତ:
	param_t _handle{PARAM_INVALID};
};

// Parameters that are tied to blocks for updating and naming.
template <ଶ୍ରେଣୀ T>
ଶ୍ରେଣୀ __EXPORT BlockParam final : ସର୍ବସାଧାରଣ BlockParamBase
{
ସର୍ବସାଧାରଣ:
	BlockParam(Block *block, ଅଚଳ ଅକ୍ଷର *name, ତାର୍କିକ parent_prefix = ସତ୍ୟ);
	BlockParam(Block *block, ଅଚଳ ଅକ୍ଷର *name, ତାର୍କିକ parent_prefix, T &extern_val);

	~BlockParam() override = ପୂର୍ବନିର୍ଦ୍ଧାରିତ;

	// no copy, assignment, move, move assignment
	BlockParam(ଅଚଳ BlockParam &) = delete;
	BlockParam &operator=(ଅଚଳ BlockParam &) = delete;
	BlockParam(BlockParam &&) = delete;
	BlockParam &operator=(BlockParam &&) = delete;

	T get() ଅଚଳ { ଫେରନ୍ତୁ _val; }

	// Store the parameter value to the parameter storage (@see param_set())
	ତାର୍କିକ commit() { ଫେରନ୍ତୁ (param_set(_handle, &_val) == PX4_OK); }

	// Store the parameter value to the parameter storage, w/o notifying the system (@see param_set_no_notification())
	ତାର୍କିକ commit_no_notification() { ଫେରନ୍ତୁ (param_set_no_notification(_handle, &_val) == PX4_OK); }

	ଶୂନ୍ୟ set(T val) { _val = val; }

	ତାର୍କିକ update() override { ଫେରନ୍ତୁ (param_get(_handle, &_val) == PX4_OK); }

ସୁରକ୍ଷିତ:
	T _val;
};

template <>
ତାର୍କିକ BlockParam<ତାର୍କିକ>::update();

ପ୍ରକାରବ୍ୟାଖ୍ୟା BlockParam<ଦଶମିକ> BlockParamFloat;
ପ୍ରକାରବ୍ୟାଖ୍ୟା BlockParam<int32_t> BlockParamInt;
ପ୍ରକାରବ୍ୟାଖ୍ୟା BlockParam<ତାର୍କିକ> BlockParamBool;
ପ୍ରକାରବ୍ୟାଖ୍ୟା BlockParam<ଦଶମିକ &> BlockParamExtFloat;
ପ୍ରକାରବ୍ୟାଖ୍ୟା BlockParam<int32_t &> BlockParamExtInt;

} // namespace control
