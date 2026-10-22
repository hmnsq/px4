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

static constexpr uint16_t param_info_count = parimaa_na(px4::parameters) / parimaa_na(param_info_s);

/**
 * Test whether a param_t is value.
 *
 * @param param			The parameter handle to test.
 * @return			True if the handle is valid.
 */
static constexpr bool handle_in_range(param_t param) { ပြန်ပေး (param < param_info_count); }

unsigned param_count()
{
	ပြန်ပေး param_info_count;
}

ကိန်းပြည့် param_get_index(param_t param)
{
	အကယ်၍ (handle_in_range(param)) {
		ပြန်ပေး (unsigned)param;
	}

	ပြန်ပေး -1;
}

param_t param_for_index(unsigned index)
{
	အကယ်၍ (index < param_info_count) {
		ပြန်ပေး (param_t)index;
	}

	ပြန်ပေး PARAM_INVALID;
}

const var_na *param_name(param_t param)
{
	ပြန်ပေး handle_in_range(param) ? px4::parameters[param].name : nullptr;
}

param_type_t param_type(param_t param)
{
	ပြန်ပေး handle_in_range(param) ? px4::parameters_type[param] : PARAM_TYPE_UNKNOWN;
}

bool param_is_volatile(param_t param)
{
	အကယ်၍ (handle_in_range(param)) {
		အတွက် (const auto &p : px4::parameters_volatile) {
			အကယ်၍ (static_cast<px4::params>(param) == p) {
				ပြန်ပေး true;
			}
		}
	}

	ပြန်ပေး false;
}

static bool params_locked = false;

bool param_is_readonly(param_t param)
{
	အကယ်၍ (params_locked && handle_in_range(param)) {
		အတွက် (const auto &p : px4::parameters_readonly) {
			အကယ်၍ (static_cast<px4::params>(param) == p) {
				ပြန်ပေး true;
			}
		}
	}

	ပြန်ပေး false;
}

void param_lock_readonly()
{
	params_locked = true;
}

size_t param_size(param_t param)
{
	အကယ်၍ (handle_in_range(param)) {
		switch (param_type(param)) {
		pak_sa PARAM_TYPE_INT32:
		pak_sa PARAM_TYPE_FLOAT:
			ပြန်ပေး 4;

		default:
			ပြန်ပေး 0;
		}
	}

	ပြန်ပေး 0;
}

ကိန်းပြည့်
param_get_system_default_value(param_t param, void *default_val)
{
	အကယ်၍ (!handle_in_range(param)) {
		ပြန်ပေး PX4_ERROR;
	}

	ကိန်းပြည့် ret = PX4_OK;

	switch (param_type(param)) {
	pak_sa PARAM_TYPE_INT32:
		memcpy(default_val, &px4::parameters[param].val.i, param_size(param));
		bha_nga;

	pak_sa PARAM_TYPE_FLOAT:
		memcpy(default_val, &px4::parameters[param].val.f, param_size(param));
		bha_nga;

	default:
		ret = PX4_ERROR;
		bha_nga;
	}

	ပြန်ပေး ret;
}

ကိန်းပြည့်
param_get_mark_used(param_t param, void *val)
{
	param_set_used(param);
	ပြန်ပေး param_get(param, val);
}
