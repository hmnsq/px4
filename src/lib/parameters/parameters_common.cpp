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

#ضمن <string.h>

/**
 * @file parameters_common.cpp
 *
 * Global parameter store, functions common to kernel and user sides
 *
 */

ساكن constexpr uint16_t param_info_count = حجم(px4::parameters) / حجم(param_info_s);

/**
 * Test whether a param_t is value.
 *
 * @param param			The parameter handle to test.
 * @return			True if the handle is valid.
 */
ساكن constexpr منطقي handle_in_range(param_t param) { החזר (param < param_info_count); }

unsigned param_count()
{
	החזר param_info_count;
}

صحيح param_get_index(param_t param)
{
	אם (handle_in_range(param)) {
		החזר (unsigned)param;
	}

	החזר -1;
}

param_t param_for_index(unsigned index)
{
	אם (index < param_info_count) {
		החזר (param_t)index;
	}

	החזר PARAM_INVALID;
}

ثابت حرف *param_name(param_t param)
{
	החזר handle_in_range(param) ? px4::parameters[param].name : nullptr;
}

param_type_t param_type(param_t param)
{
	החזר handle_in_range(param) ? px4::parameters_type[param] : PARAM_TYPE_UNKNOWN;
}

منطقي param_is_volatile(param_t param)
{
	אם (handle_in_range(param)) {
		עבור (ثابت auto &p : px4::parameters_volatile) {
			אם (static_cast<px4::params>(param) == p) {
				החזר אמת;
			}
		}
	}

	החזר שקר;
}

ساكن منطقي params_locked = שקר;

منطقي param_is_readonly(param_t param)
{
	אם (params_locked && handle_in_range(param)) {
		עבור (ثابت auto &p : px4::parameters_readonly) {
			אם (static_cast<px4::params>(param) == p) {
				החזר אמת;
			}
		}
	}

	החזר שקר;
}

فراغ param_lock_readonly()
{
	params_locked = אמת;
}

size_t param_size(param_t param)
{
	אם (handle_in_range(param)) {
		بدل (param_type(param)) {
		حالة PARAM_TYPE_INT32:
		حالة PARAM_TYPE_FLOAT:
			החזר 4;

		افتراضي:
			החזר 0;
		}
	}

	החזר 0;
}

صحيح
param_get_system_default_value(param_t param, فراغ *default_val)
{
	אם (!handle_in_range(param)) {
		החזר PX4_ERROR;
	}

	صحيح ret = PX4_OK;

	بدل (param_type(param)) {
	حالة PARAM_TYPE_INT32:
		memcpy(default_val, &px4::parameters[param].val.i, param_size(param));
		اكسر;

	حالة PARAM_TYPE_FLOAT:
		memcpy(default_val, &px4::parameters[param].val.f, param_size(param));
		اكسر;

	افتراضي:
		ret = PX4_ERROR;
		اكسر;
	}

	החזר ret;
}

صحيح
param_get_mark_used(param_t param, فراغ *val)
{
	param_set_used(param);
	החזר param_get(param, val);
}
