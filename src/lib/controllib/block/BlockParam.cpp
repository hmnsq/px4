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
 * @file BlockParam.cpp
 *
 * Controller library code
 */

#અન્તર્ભાવ "Block.hpp"
#અન્તર્ભાવ "BlockParam.hpp"

#અન્તર્ભાવ <cstdio>
#અન્તર્ભાવ <cstring>

#અન્તર્ભાવ <containers/List.hpp>

#અન્તર્ભાવ <px4_platform_common/log.h>

નામાકાશ control
{

BlockParamBase::BlockParamBase(Block *parent, અચલ અક્ષર *name, તાર્કિક parent_prefix)
{
	અક્ષર fullname[blockNameLengthMax];

	જો (parent == nullptr) {
		strncpy(fullname, name, blockNameLengthMax - 1);
		fullname[કદ(fullname) - 1] = '\0';

	} નહીંતર {
		અક્ષર parentName[blockNameLengthMax];
		parent->getName(parentName, blockNameLengthMax);

		જો (strcmp(name, "") == 0) {
			strncpy(fullname, parentName, blockNameLengthMax);
			// ensure string is terminated
			fullname[કદ(fullname) - 1] = '\0';

		} નહીંતર જો (parent_prefix) {
			જો (snprintf(fullname, blockNameLengthMax, "%s_%s", parentName, name) >= blockNameLengthMax) {
				PX4_ERR("param too long: %s", name);
			}

		} નહીંતર {
			strncpy(fullname, name, blockNameLengthMax);
			// ensure string is terminated
			fullname[કદ(fullname) - 1] = '\0';
		}

		parent->getParams().add(આ);
	}

	_handle = param_find(fullname);

	જો (_handle == PARAM_INVALID) {
		PX4_ERR("error finding param: %s", fullname);
	}
};

template <>
BlockParam<તાર્કિક>::BlockParam(Block *block, અચલ અક્ષર *name, તાર્કિક parent_prefix) :
	BlockParamBase(block, name, parent_prefix),
	_val()
{
	update();
}

template <>
તાર્કિક BlockParam<તાર્કિક>::update()
{
	int32_t tmp = 0;
	પૂર્ણાંક ret = param_get(_handle, &tmp);

	જો (tmp == 1) {
		_val = સાચું;

	} નહીંતર {
		_val = ખોટું;
	}

	પાછા (ret == PX4_OK);
}

template <>
BlockParam<int32_t>::BlockParam(Block *block, અચલ અક્ષર *name, તાર્કિક parent_prefix) :
	BlockParamBase(block, name, parent_prefix),
	_val()
{
	update();
}

template <>
BlockParam<દશાંશ>::BlockParam(Block *block, અચલ અક્ષર *name, તાર્કિક parent_prefix) :
	BlockParamBase(block, name, parent_prefix),
	_val()
{
	update();
}

template <>
BlockParam<int32_t &>::BlockParam(Block *block, અચલ અક્ષર *name, તાર્કિક parent_prefix, int32_t &extern_val) :
	BlockParamBase(block, name, parent_prefix),
	_val(extern_val)
{
	update();
}

template <>
BlockParam<દશાંશ &>::BlockParam(Block *block, અચલ અક્ષર *name, તાર્કિક parent_prefix, દશાંશ &extern_val) :
	BlockParamBase(block, name, parent_prefix),
	_val(extern_val)
{
	update();
}

} // namespace control
