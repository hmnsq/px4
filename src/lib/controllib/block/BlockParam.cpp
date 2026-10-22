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

#ضمن "Block.hpp"
#ضمن "BlockParam.hpp"

#ضمن <cstdio>
#ضمن <cstring>

#ضمن <containers/List.hpp>

#ضمن <px4_platform_common/log.h>

نطاق control
{

BlockParamBase::BlockParamBase(Block *parent, ثابت حرف *name, منطقي parent_prefix)
{
	حرف fullname[blockNameLengthMax];

	ܐܢ (parent == nullptr) {
		strncpy(fullname, name, blockNameLengthMax - 1);
		fullname[حجم(fullname) - 1] = '\0';

	} ܐܚܪܢܐ {
		حرف parentName[blockNameLengthMax];
		parent->getName(parentName, blockNameLengthMax);

		ܐܢ (strcmp(name, "") == 0) {
			strncpy(fullname, parentName, blockNameLengthMax);
			// ensure string is terminated
			fullname[حجم(fullname) - 1] = '\0';

		} ܐܚܪܢܐ ܐܢ (parent_prefix) {
			ܐܢ (snprintf(fullname, blockNameLengthMax, "%s_%s", parentName, name) >= blockNameLengthMax) {
				PX4_ERR("param too long: %s", name);
			}

		} ܐܚܪܢܐ {
			strncpy(fullname, name, blockNameLengthMax);
			// ensure string is terminated
			fullname[حجم(fullname) - 1] = '\0';
		}

		parent->getParams().add(هذا);
	}

	_handle = param_find(fullname);

	ܐܢ (_handle == PARAM_INVALID) {
		PX4_ERR("error finding param: %s", fullname);
	}
};

template <>
BlockParam<منطقي>::BlockParam(Block *block, ثابت حرف *name, منطقي parent_prefix) :
	BlockParamBase(block, name, parent_prefix),
	_val()
{
	update();
}

template <>
منطقي BlockParam<منطقي>::update()
{
	int32_t tmp = 0;
	صحيح ret = param_get(_handle, &tmp);

	ܐܢ (tmp == 1) {
		_val = ܫܪܪܐ;

	} ܐܚܪܢܐ {
		_val = ܕܓܠܐ;
	}

	ܦܢܐ (ret == PX4_OK);
}

template <>
BlockParam<int32_t>::BlockParam(Block *block, ثابت حرف *name, منطقي parent_prefix) :
	BlockParamBase(block, name, parent_prefix),
	_val()
{
	update();
}

template <>
BlockParam<عائم>::BlockParam(Block *block, ثابت حرف *name, منطقي parent_prefix) :
	BlockParamBase(block, name, parent_prefix),
	_val()
{
	update();
}

template <>
BlockParam<int32_t &>::BlockParam(Block *block, ثابت حرف *name, منطقي parent_prefix, int32_t &extern_val) :
	BlockParamBase(block, name, parent_prefix),
	_val(extern_val)
{
	update();
}

template <>
BlockParam<عائم &>::BlockParam(Block *block, ثابت حرف *name, منطقي parent_prefix, عائم &extern_val) :
	BlockParamBase(block, name, parent_prefix),
	_val(extern_val)
{
	update();
}

} // namespace control
