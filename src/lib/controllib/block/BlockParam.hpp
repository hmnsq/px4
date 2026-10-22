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

#अन्तर्भाव <containers/List.hpp>
#अन्तर्भाव <px4_platform_common/defines.h>
#अन्तर्भाव <parameters/param.h>

नामाकाश control
{

वग्ग Block;

// A base class for block params that enables traversing linked list.
वग्ग BlockParamBase : सार्वजनिक List_ग्रन्थिः<BlockParamBase *>
{
सार्वजनिक:
	/**
	 * Instantiate a block param base.
	 *
	 * @param parent_prefix Set to true to include the parent name in the parameter name
	 */
	BlockParamBase(Block *parent, ध्रुव वर्ण *name, तार्किक parent_prefix = सच्च);
	आभास ~BlockParamBase() = स्वभाव;

	आभास तार्किक update() = 0;
	ध्रुव वर्ण *getName() ध्रुव { पटिनिवत्त param_name(_handle); }

रक्षित:
	param_t _handle{PARAM_INVALID};
};

// Parameters that are tied to blocks for updating and naming.
template <वग्ग T>
वग्ग __EXPORT BlockParam final : सार्वजनिक BlockParamBase
{
सार्वजनिक:
	BlockParam(Block *block, ध्रुव वर्ण *name, तार्किक parent_prefix = सच्च);
	BlockParam(Block *block, ध्रुव वर्ण *name, तार्किक parent_prefix, T &extern_val);

	~BlockParam() override = स्वभाव;

	// no copy, assignment, move, move assignment
	BlockParam(ध्रुव BlockParam &) = delete;
	BlockParam &operator=(ध्रुव BlockParam &) = delete;
	BlockParam(BlockParam &&) = delete;
	BlockParam &operator=(BlockParam &&) = delete;

	T get() ध्रुव { पटिनिवत्त _val; }

	// Store the parameter value to the parameter storage (@see param_set())
	तार्किक commit() { पटिनिवत्त (param_set(_handle, &_val) == PX4_OK); }

	// Store the parameter value to the parameter storage, w/o notifying the system (@see param_set_no_notification())
	तार्किक commit_no_notification() { पटिनिवत्त (param_set_no_notification(_handle, &_val) == PX4_OK); }

	शून्य set(T val) { _val = val; }

	तार्किक update() override { पटिनिवत्त (param_get(_handle, &_val) == PX4_OK); }

रक्षित:
	T _val;
};

template <>
तार्किक BlockParam<तार्किक>::update();

प्रकारनिर्देश BlockParam<भिन्न> BlockParamFloat;
प्रकारनिर्देश BlockParam<int32_t> BlockParamInt;
प्रकारनिर्देश BlockParam<तार्किक> BlockParamBool;
प्रकारनिर्देश BlockParam<भिन्न &> BlockParamExtFloat;
प्रकारनिर्देश BlockParam<int32_t &> BlockParamExtInt;

} // namespace control
