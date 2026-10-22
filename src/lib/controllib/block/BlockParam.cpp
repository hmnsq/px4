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

بلاک_Param_بنسټ::بلاک_Param_بنسټ(بلاک *پلار, ثابت حرف *name, منطقي پلار_مختاړی)
{
	حرف fullname[بلاک_Name_اوږدتیا_Max];

	إذا (پلار == تش_اشاره) {
		strncpy(fullname, name, بلاک_Name_اوږدتیا_Max - 1);
		fullname[حجم(fullname) - 1] = '\0';

	} وإلا {
		حرف پلار_Name[بلاک_Name_اوږدتیا_Max];
		پلار->getName(پلار_Name, بلاک_Name_اوږدتیا_Max);

		إذا (strcmp(name, "") == 0) {
			strncpy(fullname, پلار_Name, بلاک_Name_اوږدتیا_Max);
			// ensure string is terminated
			fullname[حجم(fullname) - 1] = '\0';

		} وإلا إذا (پلار_مختاړی) {
			إذا (snprintf(fullname, بلاک_Name_اوږدتیا_Max, "%s_%s", پلار_Name, name) >= بلاک_Name_اوږدتیا_Max) {
				PX4_ERR("param too long: %s", name);
			}

		} وإلا {
			strncpy(fullname, name, بلاک_Name_اوږدتیا_Max);
			// ensure string is terminated
			fullname[حجم(fullname) - 1] = '\0';
		}

		پلار->getParams().جمع_کړه(هذا);
	}

	_handle = param_پیدا_کړه(fullname);

	إذا (_handle == PARAM_INVALID) {
		PX4_ERR("error finding param: %s", fullname);
	}
};

کينډۍ <>
بلاک_Param<منطقي>::بلاک_Param(بلاک *بلاک_2, ثابت حرف *name, منطقي پلار_مختاړی) :
	بلاک_Param_بنسټ(بلاک_2, name, پلار_مختاړی),
	_val()
{
	update();
}

کينډۍ <>
منطقي بلاک_Param<منطقي>::update()
{
	int32_t tmp = 0;
	صحيح ret = param_get(_handle, &tmp);

	إذا (tmp == 1) {
		_val = ریښتیا;

	} وإلا {
		_val = خطأ;
	}

	أرجع (ret == PX4_OK);
}

کينډۍ <>
بلاک_Param<int32_t>::بلاک_Param(بلاک *بلاک_2, ثابت حرف *name, منطقي پلار_مختاړی) :
	بلاک_Param_بنسټ(بلاک_2, name, پلار_مختاړی),
	_val()
{
	update();
}

کينډۍ <>
بلاک_Param<عائم>::بلاک_Param(بلاک *بلاک_2, ثابت حرف *name, منطقي پلار_مختاړی) :
	بلاک_Param_بنسټ(بلاک_2, name, پلار_مختاړی),
	_val()
{
	update();
}

کينډۍ <>
بلاک_Param<int32_t &>::بلاک_Param(بلاک *بلاک_2, ثابت حرف *name, منطقي پلار_مختاړی, int32_t &extern_val) :
	بلاک_Param_بنسټ(بلاک_2, name, پلار_مختاړی),
	_val(extern_val)
{
	update();
}

کينډۍ <>
بلاک_Param<عائم &>::بلاک_Param(بلاک *بلاک_2, ثابت حرف *name, منطقي پلار_مختاړی, عائم &extern_val) :
	بلاک_Param_بنسټ(بلاک_2, name, پلار_مختاړی),
	_val(extern_val)
{
	update();
}

} // namespace control
