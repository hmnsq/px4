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

#અન્તર્ભાવ <containers/List.hpp>
#અન્તર્ભાવ <px4_platform_common/defines.h>
#અન્તર્ભાવ <parameters/param.h>

નામાકાશ control
{

વર્ગ Block;

// A base class for block params that enables traversing linked list.
વર્ગ BlockParamBase : જાહેર List_ગાંઠ<BlockParamBase *>
{
જાહેર:
	/**
	 * Instantiate a block param base.
	 *
	 * @param parent_prefix Set to true to include the parent name in the parameter name
	 */
	BlockParamBase(Block *parent, અચલ અક્ષર *name, તાર્કિક parent_prefix = સાચું);
	આભાસી ~BlockParamBase() = મૂળભૂત;

	આભાસી તાર્કિક update() = 0;
	અચલ અક્ષર *getName() અચલ { પાછા param_name(_handle); }

સુરક્ષિત:
	param_t _handle{PARAM_INVALID};
};

// Parameters that are tied to blocks for updating and naming.
template <વર્ગ T>
વર્ગ __EXPORT BlockParam final : જાહેર BlockParamBase
{
જાહેર:
	BlockParam(Block *block, અચલ અક્ષર *name, તાર્કિક parent_prefix = સાચું);
	BlockParam(Block *block, અચલ અક્ષર *name, તાર્કિક parent_prefix, T &extern_val);

	~BlockParam() override = મૂળભૂત;

	// no copy, assignment, move, move assignment
	BlockParam(અચલ BlockParam &) = delete;
	BlockParam &operator=(અચલ BlockParam &) = delete;
	BlockParam(BlockParam &&) = delete;
	BlockParam &operator=(BlockParam &&) = delete;

	T get() અચલ { પાછા _val; }

	// Store the parameter value to the parameter storage (@see param_set())
	તાર્કિક commit() { પાછા (param_set(_handle, &_val) == PX4_OK); }

	// Store the parameter value to the parameter storage, w/o notifying the system (@see param_set_no_notification())
	તાર્કિક commit_no_notification() { પાછા (param_set_no_notification(_handle, &_val) == PX4_OK); }

	શૂન્ય set(T val) { _val = val; }

	તાર્કિક update() override { પાછા (param_get(_handle, &_val) == PX4_OK); }

સુરક્ષિત:
	T _val;
};

template <>
તાર્કિક BlockParam<તાર્કિક>::update();

પ્રકારવ્યાખ્યા BlockParam<દશાંશ> BlockParamFloat;
પ્રકારવ્યાખ્યા BlockParam<int32_t> BlockParamInt;
પ્રકારવ્યાખ્યા BlockParam<તાર્કિક> BlockParamBool;
પ્રકારવ્યાખ્યા BlockParam<દશાંશ &> BlockParamExtFloat;
પ્રકારવ્યાખ્યા BlockParam<int32_t &> BlockParamExtInt;

} // namespace control
