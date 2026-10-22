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

#അന്തര്ഭാവ <containers/List.hpp>
#അന്തര്ഭാവ <px4_platform_common/defines.h>
#അന്തര്ഭാവ <parameters/param.h>

നാമാകാശ control
{

വർഗ്ഗം Block;

// A base class for block params that enables traversing linked list.
വർഗ്ഗം BlockParamBase : പൊതു List_കണ്ണി<BlockParamBase *>
{
പൊതു:
	/**
	 * Instantiate a block param base.
	 *
	 * @param parent_prefix Set to true to include the parent name in the parameter name
	 */
	BlockParamBase(Block *parent, സ്ഥിരം അക്ഷരം *name, തർക്കം parent_prefix = സത്യം);
	മായാ ~BlockParamBase() = സ്ഥിരസ്ഥിതി;

	മായാ തർക്കം update() = 0;
	സ്ഥിരം അക്ഷരം *getName() സ്ഥിരം { തിരികെ param_name(_handle); }

സംരക്ഷിത:
	param_t _handle{PARAM_INVALID};
};

// Parameters that are tied to blocks for updating and naming.
template <വർഗ്ഗം T>
വർഗ്ഗം __EXPORT BlockParam final : പൊതു BlockParamBase
{
പൊതു:
	BlockParam(Block *block, സ്ഥിരം അക്ഷരം *name, തർക്കം parent_prefix = സത്യം);
	BlockParam(Block *block, സ്ഥിരം അക്ഷരം *name, തർക്കം parent_prefix, T &extern_val);

	~BlockParam() override = സ്ഥിരസ്ഥിതി;

	// no copy, assignment, move, move assignment
	BlockParam(സ്ഥിരം BlockParam &) = delete;
	BlockParam &operator=(സ്ഥിരം BlockParam &) = delete;
	BlockParam(BlockParam &&) = delete;
	BlockParam &operator=(BlockParam &&) = delete;

	T get() സ്ഥിരം { തിരികെ _val; }

	// Store the parameter value to the parameter storage (@see param_set())
	തർക്കം commit() { തിരികെ (param_set(_handle, &_val) == PX4_OK); }

	// Store the parameter value to the parameter storage, w/o notifying the system (@see param_set_no_notification())
	തർക്കം commit_no_notification() { തിരികെ (param_set_no_notification(_handle, &_val) == PX4_OK); }

	ശൂന്യം set(T val) { _val = val; }

	തർക്കം update() override { തിരികെ (param_get(_handle, &_val) == PX4_OK); }

സംരക്ഷിത:
	T _val;
};

template <>
തർക്കം BlockParam<തർക്കം>::update();

തരനിർവചനം BlockParam<ദശാംശം> BlockParamFloat;
തരനിർവചനം BlockParam<int32_t> BlockParamInt;
തരനിർവചനം BlockParam<തർക്കം> BlockParamBool;
തരനിർവചനം BlockParam<ദശാംശം &> BlockParamExtFloat;
തരനിർവചനം BlockParam<int32_t &> BlockParamExtInt;

} // namespace control
