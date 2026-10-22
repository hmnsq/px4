/****************************************************************************
 *
 *   Copyright (c) 2012-2022 PX4 Development Team. All rights reserved.
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

#شامل <string.h>

/**
 * @file parameters_common.cpp
 *
 * Global parameter store, functions common to kernel and user sides
 *
 */

ایستا constexpr uint16_t param_info_شمار = اندازه(px4::parameters) / اندازه(param_info_s);

/**
 * Test whether a param_t is value.
 *
 * @param param			The parameter handle to test.
 * @return			True if the handle is valid.
 */
ایستا constexpr منطقی handle_in_range(param_t param) { بازگشت (param < param_info_شمار); }

unsigned param_شمار()
{
	بازگشت param_info_شمار;
}

صحیح param_get_index(param_t param)
{
	اگر (handle_in_range(param)) {
		بازگشت (unsigned)param;
	}

	بازگشت -1;
}

param_t param_for_index(unsigned index)
{
	اگر (index < param_info_شمار) {
		بازگشت (param_t)index;
	}

	بازگشت PARAM_INVALID;
}

ثابت نویسه *param_name(param_t param)
{
	بازگشت handle_in_range(param) ? px4::parameters[param].name : nullptr;
}

param_type_t param_type(param_t param)
{
	بازگشت handle_in_range(param) ? px4::parameters_type[param] : PARAM_TYPE_UNKNOWN;
}

منطقی param_is_volatile(param_t param)
{
	اگر (handle_in_range(param)) {
		برای (ثابت auto &p : px4::parameters_volatile) {
			اگر (static_cast<px4::params>(param) == p) {
				بازگشت درست;
			}
		}
	}

	بازگشت نادرست;
}

ایستا منطقی params_locked = نادرست;

منطقی param_is_readonly(param_t param)
{
	اگر (params_locked && handle_in_range(param)) {
		برای (ثابت auto &p : px4::parameters_readonly) {
			اگر (static_cast<px4::params>(param) == p) {
				بازگشت درست;
			}
		}
	}

	بازگشت نادرست;
}

تهی param_lock_readonly()
{
	params_locked = درست;
}

size_t param_size(param_t param)
{
	اگر (handle_in_range(param)) {
		انتخاب (param_type(param)) {
		حالت PARAM_TYPE_INT32:
		حالت PARAM_TYPE_FLOAT:
			بازگشت 4;

		پیشفرض:
			بازگشت 0;
		}
	}

	بازگشت 0;
}

صحیح
param_get_system_default_مقدار(param_t param, تهی *default_val)
{
	اگر (!handle_in_range(param)) {
		بازگشت PX4_ERROR;
	}

	صحیح ret = PX4_OK;

	انتخاب (param_type(param)) {
	حالت PARAM_TYPE_INT32:
		memcpy(default_val, &px4::parameters[param].val.i, param_size(param));
		بشکن;

	حالت PARAM_TYPE_FLOAT:
		memcpy(default_val, &px4::parameters[param].val.f, param_size(param));
		بشکن;

	پیشفرض:
		ret = PX4_ERROR;
		بشکن;
	}

	بازگشت ret;
}

صحیح
param_get_mark_used(param_t param, تهی *val)
{
	param_set_used(param);
	بازگشت param_get(param, val);
}
