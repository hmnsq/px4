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
static constexpr bool handle_in_range(param_t param) { ສົ່ງຄືນ (param < param_info_count); }

unsigned param_count()
{
	ສົ່ງຄືນ param_info_count;
}

ຈຳນວນເຕັມ param_get_index(param_t param)
{
	ຖ້າ (handle_in_range(param)) {
		ສົ່ງຄືນ (unsigned)param;
	}

	ສົ່ງຄືນ -1;
}

param_t param_for_index(unsigned index)
{
	ຖ້າ (index < param_info_count) {
		ສົ່ງຄືນ (param_t)index;
	}

	ສົ່ງຄືນ PARAM_INVALID;
}

const var_na *param_name(param_t param)
{
	ສົ່ງຄືນ handle_in_range(param) ? px4::parameters[param].name : nullptr;
}

param_type_t param_type(param_t param)
{
	ສົ່ງຄືນ handle_in_range(param) ? px4::parameters_type[param] : PARAM_TYPE_UNKNOWN;
}

bool param_is_volatile(param_t param)
{
	ຖ້າ (handle_in_range(param)) {
		ສຳລັບ (const auto &p : px4::parameters_volatile) {
			ຖ້າ (static_cast<px4::params>(param) == p) {
				ສົ່ງຄືນ true;
			}
		}
	}

	ສົ່ງຄືນ false;
}

static bool params_locked = false;

bool param_is_readonly(param_t param)
{
	ຖ້າ (params_locked && handle_in_range(param)) {
		ສຳລັບ (const auto &p : px4::parameters_readonly) {
			ຖ້າ (static_cast<px4::params>(param) == p) {
				ສົ່ງຄືນ true;
			}
		}
	}

	ສົ່ງຄືນ false;
}

void param_lock_readonly()
{
	params_locked = true;
}

size_t param_size(param_t param)
{
	ຖ້າ (handle_in_range(param)) {
		switch (param_type(param)) {
		pak_sa PARAM_TYPE_INT32:
		pak_sa PARAM_TYPE_FLOAT:
			ສົ່ງຄືນ 4;

		default:
			ສົ່ງຄືນ 0;
		}
	}

	ສົ່ງຄືນ 0;
}

ຈຳນວນເຕັມ
param_get_system_default_value(param_t param, void *default_val)
{
	ຖ້າ (!handle_in_range(param)) {
		ສົ່ງຄືນ PX4_ERROR;
	}

	ຈຳນວນເຕັມ ret = PX4_OK;

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

	ສົ່ງຄືນ ret;
}

ຈຳນວນເຕັມ
param_get_mark_used(param_t param, void *val)
{
	param_set_used(param);
	ສົ່ງຄືນ param_get(param, val);
}
