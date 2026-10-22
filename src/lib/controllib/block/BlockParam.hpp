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

#அந்தர்பாவ <containers/List.hpp>
#அந்தர்பாவ <px4_platform_common/defines.h>
#அந்தர்பாவ <parameters/param.h>

நாமாகாஶ control
{

வகுப்பு Block;

// A base class for block params that enables traversing linked list.
வகுப்பு BlockParamBase : பொது List_முனை<BlockParamBase *>
{
பொது:
	/**
	 * Instantiate a block param base.
	 *
	 * @param parent_prefix Set to true to include the parent name in the parameter name
	 */
	BlockParamBase(Block *parent, மாறிலி எழுத்து *name, தருக்க parent_prefix = உண்மை);
	மெய்நிகர் ~BlockParamBase() = இயல்பு;

	மெய்நிகர் தருக்க update() = 0;
	மாறிலி எழுத்து *getName() மாறிலி { திரும்பு param_name(_handle); }

பாதுகாக்கப்பட்ட:
	param_t _handle{PARAM_INVALID};
};

// Parameters that are tied to blocks for updating and naming.
template <வகுப்பு T>
வகுப்பு __EXPORT BlockParam final : பொது BlockParamBase
{
பொது:
	BlockParam(Block *block, மாறிலி எழுத்து *name, தருக்க parent_prefix = உண்மை);
	BlockParam(Block *block, மாறிலி எழுத்து *name, தருக்க parent_prefix, T &extern_val);

	~BlockParam() override = இயல்பு;

	// no copy, assignment, move, move assignment
	BlockParam(மாறிலி BlockParam &) = delete;
	BlockParam &operator=(மாறிலி BlockParam &) = delete;
	BlockParam(BlockParam &&) = delete;
	BlockParam &operator=(BlockParam &&) = delete;

	T get() மாறிலி { திரும்பு _val; }

	// Store the parameter value to the parameter storage (@see param_set())
	தருக்க commit() { திரும்பு (param_set(_handle, &_val) == PX4_OK); }

	// Store the parameter value to the parameter storage, w/o notifying the system (@see param_set_no_notification())
	தருக்க commit_no_notification() { திரும்பு (param_set_no_notification(_handle, &_val) == PX4_OK); }

	வெறுமை set(T val) { _val = val; }

	தருக்க update() override { திரும்பு (param_get(_handle, &_val) == PX4_OK); }

பாதுகாக்கப்பட்ட:
	T _val;
};

template <>
தருக்க BlockParam<தருக்க>::update();

வகைவரை BlockParam<தசம> BlockParamFloat;
வகைவரை BlockParam<int32_t> BlockParamInt;
வகைவரை BlockParam<தருக்க> BlockParamBool;
வகைவரை BlockParam<தசம &> BlockParamExtFloat;
வகைவரை BlockParam<int32_t &> BlockParamExtInt;

} // namespace control
