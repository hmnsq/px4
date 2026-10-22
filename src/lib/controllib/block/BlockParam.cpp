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

بلوک_Param_پایه::بلوک_Param_پایه(بلوک *والد, ثابت حرف *name, منطقي والد_پیشوند)
{
	حرف fullname[بلوک_Name_درازی_Max];

	إذا (والد == اشاره_تهی) {
		strncpy(fullname, name, بلوک_Name_درازی_Max - 1);
		fullname[حجم(fullname) - 1] = '\0';

	} وإلا {
		حرف والد_Name[بلوک_Name_درازی_Max];
		والد->getName(والد_Name, بلوک_Name_درازی_Max);

		إذا (strcmp(name, "") == 0) {
			strncpy(fullname, والد_Name, بلوک_Name_درازی_Max);
			// ensure string is terminated
			fullname[حجم(fullname) - 1] = '\0';

		} وإلا إذا (والد_پیشوند) {
			إذا (snprintf(fullname, بلوک_Name_درازی_Max, "%s_%s", والد_Name, name) >= بلوک_Name_درازی_Max) {
				PX4_ERR("param too long: %s", name);
			}

		} وإلا {
			strncpy(fullname, name, بلوک_Name_درازی_Max);
			// ensure string is terminated
			fullname[حجم(fullname) - 1] = '\0';
		}

		والد->getParams().جمع_کن(هذا);
	}

	_handle = param_پیدا_کن(fullname);

	إذا (_handle == PARAM_INVALID) {
		PX4_ERR("error finding param: %s", fullname);
	}
};

قالب <>
بلوک_Param<منطقي>::بلوک_Param(بلوک *بلوک_2, ثابت حرف *name, منطقي والد_پیشوند) :
	بلوک_Param_پایه(بلوک_2, name, والد_پیشوند),
	_val()
{
	update();
}

قالب <>
منطقي بلوک_Param<منطقي>::update()
{
	int32_t tmp = 0;
	صحيح ret = param_get(_handle, &tmp);

	إذا (tmp == 1) {
		_val = درست;

	} وإلا {
		_val = خطأ;
	}

	أرجع (ret == PX4_OK);
}

قالب <>
بلوک_Param<int32_t>::بلوک_Param(بلوک *بلوک_2, ثابت حرف *name, منطقي والد_پیشوند) :
	بلوک_Param_پایه(بلوک_2, name, والد_پیشوند),
	_val()
{
	update();
}

قالب <>
بلوک_Param<عائم>::بلوک_Param(بلوک *بلوک_2, ثابت حرف *name, منطقي والد_پیشوند) :
	بلوک_Param_پایه(بلوک_2, name, والد_پیشوند),
	_val()
{
	update();
}

قالب <>
بلوک_Param<int32_t &>::بلوک_Param(بلوک *بلوک_2, ثابت حرف *name, منطقي والد_پیشوند, int32_t &extern_val) :
	بلوک_Param_پایه(بلوک_2, name, والد_پیشوند),
	_val(extern_val)
{
	update();
}

قالب <>
بلوک_Param<عائم &>::بلوک_Param(بلوک *بلوک_2, ثابت حرف *name, منطقي والد_پیشوند, عائم &extern_val) :
	بلوک_Param_پایه(بلوک_2, name, والد_پیشوند),
	_val(extern_val)
{
	update();
}

} // namespace control
