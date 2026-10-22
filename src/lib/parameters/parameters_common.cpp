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

#અન્તર્ભાવ <string.h>

/**
 * @file parameters_common.cpp
 *
 * Global parameter store, functions common to kernel and user sides
 *
 */

સ્થિર constexpr uint16_t param_info_ગણતરી = કદ(px4::parameters) / કદ(param_info_s);

/**
 * Test whether a param_t is value.
 *
 * @param param			The parameter handle to test.
 * @return			True if the handle is valid.
 */
સ્થિર constexpr તાર્કિક handle_in_range(param_t param) { પાછા (param < param_info_ગણતરી); }

unsigned param_ગણતરી()
{
	પાછા param_info_ગણતરી;
}

પૂર્ણાંક param_get_index(param_t param)
{
	જો (handle_in_range(param)) {
		પાછા (unsigned)param;
	}

	પાછા -1;
}

param_t param_for_index(unsigned index)
{
	જો (index < param_info_ગણતરી) {
		પાછા (param_t)index;
	}

	પાછા PARAM_INVALID;
}

અચલ અક્ષર *param_name(param_t param)
{
	પાછા handle_in_range(param) ? px4::parameters[param].name : nullptr;
}

param_type_t param_type(param_t param)
{
	પાછા handle_in_range(param) ? px4::parameters_type[param] : PARAM_TYPE_UNKNOWN;
}

તાર્કિક param_is_volatile(param_t param)
{
	જો (handle_in_range(param)) {
		માટે (અચલ auto &p : px4::parameters_volatile) {
			જો (static_cast<px4::params>(param) == p) {
				પાછા સાચું;
			}
		}
	}

	પાછા ખોટું;
}

સ્થિર તાર્કિક params_locked = ખોટું;

તાર્કિક param_is_readonly(param_t param)
{
	જો (params_locked && handle_in_range(param)) {
		માટે (અચલ auto &p : px4::parameters_readonly) {
			જો (static_cast<px4::params>(param) == p) {
				પાછા સાચું;
			}
		}
	}

	પાછા ખોટું;
}

શૂન્ય param_lock_readonly()
{
	params_locked = સાચું;
}

size_t param_size(param_t param)
{
	જો (handle_in_range(param)) {
		પસંદગી (param_type(param)) {
		સ્થિતિ PARAM_TYPE_INT32:
		સ્થિતિ PARAM_TYPE_FLOAT:
			પાછા 4;

		મૂળભૂત:
			પાછા 0;
		}
	}

	પાછા 0;
}

પૂર્ણાંક
param_get_system_default_કિંમત(param_t param, શૂન્ય *default_val)
{
	જો (!handle_in_range(param)) {
		પાછા PX4_ERROR;
	}

	પૂર્ણાંક ret = PX4_OK;

	પસંદગી (param_type(param)) {
	સ્થિતિ PARAM_TYPE_INT32:
		memcpy(default_val, &px4::parameters[param].val.i, param_size(param));
		તોડો;

	સ્થિતિ PARAM_TYPE_FLOAT:
		memcpy(default_val, &px4::parameters[param].val.f, param_size(param));
		તોડો;

	મૂળભૂત:
		ret = PX4_ERROR;
		તોડો;
	}

	પાછા ret;
}

પૂર્ણાંક
param_get_mark_used(param_t param, શૂન્ય *val)
{
	param_set_used(param);
	પાછા param_get(param, val);
}
