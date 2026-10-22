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

#include <string.h>

/**
 * @file parameters_common.cpp
 *
 * Global parameter store, functions common to kernel and user sides
 *
 */

static constexpr uint16_t param_info_count = sizeof(px4::parameters) / sizeof(param_info_s);

/**
 * Test whether a param_t is value.
 *
 * @param param			The parameter handle to test.
 * @return			True if the handle is valid.
 */
static constexpr bool handle_in_range(param_t param) { 返回 (param < param_info_count); }

unsigned param_count()
{
	返回 param_info_count;
}

整数 param_get_index(param_t param)
{
	如果 (handle_in_range(param)) {
		返回 (unsigned)param;
	}

	返回 -1;
}

param_t param_for_index(unsigned index)
{
	如果 (index < param_info_count) {
		返回 (param_t)index;
	}

	返回 PARAM_INVALID;
}

const char *param_name(param_t param)
{
	返回 handle_in_range(param) ? px4::parameters[param].name : nullptr;
}

param_type_t param_type(param_t param)
{
	返回 handle_in_range(param) ? px4::parameters_type[param] : PARAM_TYPE_UNKNOWN;
}

bool param_is_volatile(param_t param)
{
	如果 (handle_in_range(param)) {
		为 (const auto &p : px4::parameters_volatile) {
			如果 (static_cast<px4::params>(param) == p) {
				返回 true;
			}
		}
	}

	返回 false;
}

static bool params_locked = false;

bool param_is_readonly(param_t param)
{
	如果 (params_locked && handle_in_range(param)) {
		为 (const auto &p : px4::parameters_readonly) {
			如果 (static_cast<px4::params>(param) == p) {
				返回 true;
			}
		}
	}

	返回 false;
}

void param_lock_readonly()
{
	params_locked = true;
}

size_t param_size(param_t param)
{
	如果 (handle_in_range(param)) {
		switch (param_type(param)) {
		case PARAM_TYPE_INT32:
		case PARAM_TYPE_FLOAT:
			返回 4;

		default:
			返回 0;
		}
	}

	返回 0;
}

整数
param_get_system_default_value(param_t param, void *default_val)
{
	如果 (!handle_in_range(param)) {
		返回 PX4_ERROR;
	}

	整数 ret = PX4_OK;

	switch (param_type(param)) {
	case PARAM_TYPE_INT32:
		memcpy(default_val, &px4::parameters[param].val.i, param_size(param));
		break;

	case PARAM_TYPE_FLOAT:
		memcpy(default_val, &px4::parameters[param].val.f, param_size(param));
		break;

	default:
		ret = PX4_ERROR;
		break;
	}

	返回 ret;
}

整数
param_get_mark_used(param_t param, void *val)
{
	param_set_used(param);
	返回 param_get(param, val);
}
