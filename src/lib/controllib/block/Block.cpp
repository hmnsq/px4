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
 * @file Block.cpp
 *
 * Controller library code
 */

#ضمن "Block.hpp"
#ضمن "BlockParam.hpp"

#ضمن <cstdio>
#ضمن <cstring>

#ضمن <px4_platform_common/log.h>

نطاق control
{

بلوک::بلوک(Super_بلوک *والد, ثابت حرف *name) :
	_name(name),
	والد_2(والد)
{
	إذا (get_والد() != اشاره_تهی) {
		get_والد()->getChildren().جمع_کن(هذا);
	}
}

فراغ بلوک::getName(حرف *buf, size_t n)
{
	إذا (get_والد() == اشاره_تهی) {
		strncpy(buf, _name, n);
		// ensure string is terminated
		buf[n - 1] = '\0';

	} وإلا {
		حرف والد_Name[بلوک_Name_درازی_Max];
		get_والد()->getName(والد_Name, n);

		إذا (!strcmp(_name, "")) {
			strncpy(buf, والد_Name, n);
			// ensure string is terminated
			buf[n - 1] = '\0';

		} وإلا {
			snprintf(buf, n, "%s_%s", والد_Name, _name);
		}
	}
}

فراغ بلوک::updateParams()
{
	بلوک_Param_پایه *param = getParams().get_سر();
	صحيح شمار = 0;

	طالما (param != اشاره_تهی) {
		إذا (شمار++ > max_Params_در_هر_بلوک) {
			حرف name[بلوک_Name_درازی_Max];
			getName(name, بلوک_Name_درازی_Max);
			PX4_ERR("exceeded max params for block: %s", name);
			اكسر;
		}

		//printf("updating param: %s\n", param->getName());
		param->update();
		param = param->getSibling();
	}

	updateParamsSubclass();
}

فراغ Super_بلوک::مجموعه_Dt(عائم dt)
{
	بلوک::مجموعه_Dt(dt);
	بلوک *فرزند = getChildren().get_سر();
	صحيح شمار = 0;

	طالما (فرزند != اشاره_تهی) {
		إذا (شمار++ > max_Children_در_هر_بلوک) {
			حرف name[بلوک_Name_درازی_Max];
			getName(name, بلوک_Name_درازی_Max);
			PX4_ERR("exceeded max children for block: %s", name);
			اكسر;
		}

		فرزند->مجموعه_Dt(dt);
		فرزند = فرزند->getSibling();
	}
}

فراغ Super_بلوک::update_فرزند_Params()
{
	بلوک *فرزند = getChildren().get_سر();
	صحيح شمار = 0;

	طالما (فرزند != اشاره_تهی) {
		إذا (شمار++ > max_Children_در_هر_بلوک) {
			حرف name[بلوک_Name_درازی_Max];
			getName(name, بلوک_Name_درازی_Max);
			PX4_ERR("exceeded max children for block: %s", name);
			اكسر;
		}

		فرزند->updateParams();
		فرزند = فرزند->getSibling();
	}
}

} // namespace control

قالب صنف List<control::بلوک_Param_پایه *>;
