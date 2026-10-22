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

#ಅನ್ತರ್ಭಾವ <containers/List.hpp>
#ಅನ್ತರ್ಭಾವ <px4_platform_common/defines.h>
#ಅನ್ತರ್ಭಾವ <parameters/param.h>

ನಾಮಾಕಾಶ control
{

ವರ್ಗ Block;

// A base class for block params that enables traversing linked list.
ವರ್ಗ BlockParamBase : ಸಾರ್ವಜನಿಕ List_ಗಂಟು<BlockParamBase *>
{
ಸಾರ್ವಜನಿಕ:
	/**
	 * Instantiate a block param base.
	 *
	 * @param parent_prefix Set to true to include the parent name in the parameter name
	 */
	BlockParamBase(Block *parent, ಧ್ರುವ ವರ್ಣ *name, ತಾರ್ಕಿಕ parent_prefix = ಸತ್ಯ);
	ಆಭಾಸ ~BlockParamBase() = ಸ್ವಭಾವ;

	ಆಭಾಸ ತಾರ್ಕಿಕ update() = 0;
	ಧ್ರುವ ವರ್ಣ *getName() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ param_name(_handle); }

ರಕ್ಷಿತ:
	param_t _handle{PARAM_INVALID};
};

// Parameters that are tied to blocks for updating and naming.
template <ವರ್ಗ T>
ವರ್ಗ __EXPORT BlockParam final : ಸಾರ್ವಜನಿಕ BlockParamBase
{
ಸಾರ್ವಜನಿಕ:
	BlockParam(Block *block, ಧ್ರುವ ವರ್ಣ *name, ತಾರ್ಕಿಕ parent_prefix = ಸತ್ಯ);
	BlockParam(Block *block, ಧ್ರುವ ವರ್ಣ *name, ತಾರ್ಕಿಕ parent_prefix, T &extern_val);

	~BlockParam() override = ಸ್ವಭಾವ;

	// no copy, assignment, move, move assignment
	BlockParam(ಧ್ರುವ BlockParam &) = delete;
	BlockParam &operator=(ಧ್ರುವ BlockParam &) = delete;
	BlockParam(BlockParam &&) = delete;
	BlockParam &operator=(BlockParam &&) = delete;

	T get() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ _val; }

	// Store the parameter value to the parameter storage (@see param_set())
	ತಾರ್ಕಿಕ commit() { ಪ್ರತ್ಯಾಗಮ (param_set(_handle, &_val) == PX4_OK); }

	// Store the parameter value to the parameter storage, w/o notifying the system (@see param_set_no_notification())
	ತಾರ್ಕಿಕ commit_no_notification() { ಪ್ರತ್ಯಾಗಮ (param_set_no_notification(_handle, &_val) == PX4_OK); }

	ಶೂನ್ಯ set(T val) { _val = val; }

	ತಾರ್ಕಿಕ update() override { ಪ್ರತ್ಯಾಗಮ (param_get(_handle, &_val) == PX4_OK); }

ರಕ್ಷಿತ:
	T _val;
};

template <>
ತಾರ್ಕಿಕ BlockParam<ತಾರ್ಕಿಕ>::update();

ಪ್ರಕಾರನಿರ್ದೇಶ BlockParam<ಭಿನ್ನ> BlockParamFloat;
ಪ್ರಕಾರನಿರ್ದೇಶ BlockParam<int32_t> BlockParamInt;
ಪ್ರಕಾರನಿರ್ದೇಶ BlockParam<ತಾರ್ಕಿಕ> BlockParamBool;
ಪ್ರಕಾರನಿರ್ದೇಶ BlockParam<ಭಿನ್ನ &> BlockParamExtFloat;
ಪ್ರಕಾರನಿರ್ದೇಶ BlockParam<int32_t &> BlockParamExtInt;

} // namespace control
