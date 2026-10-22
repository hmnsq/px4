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

#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>

/**
 * @file parameters_common.cpp
 *
 * Global parameter store, functions common to kernel and user sides
 *
 */

ਸਥਿਰ constexpr uint16_t param_info_ਗਿਣਤੀ = ਆਕਾਰ(px4::parameters) / ਆਕਾਰ(param_info_s);

/**
 * Test whether a param_t is value.
 *
 * @param param			The parameter handle to test.
 * @return			True if the handle is valid.
 */
ਸਥਿਰ constexpr ਤਾਰਕਿਕ handle_ਵਿੱਚ_range(param_t param) { ਵਾਪਸ (param < param_info_ਗਿਣਤੀ); }

ਬਿਨਾਨਿਸ਼ਾਨੀ param_ਗਿਣਤੀ()
{
	ਵਾਪਸ param_info_ਗਿਣਤੀ;
}

ਪੂਰਨ param_get_ਸੂਚਕਾਂਕ(param_t param)
{
	ਜੇ (handle_ਵਿੱਚ_range(param)) {
		ਵਾਪਸ (ਬਿਨਾਨਿਸ਼ਾਨੀ)param;
	}

	ਵਾਪਸ -1;
}

param_t param_for_ਸੂਚਕਾਂਕ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਸੂਚਕਾਂਕ)
{
	ਜੇ (ਸੂਚਕਾਂਕ < param_info_ਗਿਣਤੀ) {
		ਵਾਪਸ (param_t)ਸੂਚਕਾਂਕ;
	}

	ਵਾਪਸ PARAM_INVALID;
}

ਅਚਲ ਅੱਖਰ *param_name(param_t param)
{
	ਵਾਪਸ handle_ਵਿੱਚ_range(param) ? px4::parameters[param].name : ਖਾਲੀ_ਸੂਚਕ;
}

param_type_t param_type(param_t param)
{
	ਵਾਪਸ handle_ਵਿੱਚ_range(param) ? px4::parameters_type[param] : PARAM_TYPE_UNKNOWN;
}

ਤਾਰਕਿਕ param_is_volatile(param_t param)
{
	ਜੇ (handle_ਵਿੱਚ_range(param)) {
		ਲਈ (ਅਚਲ ਸਵੈਚਾਲਿਤ &p : px4::parameters_volatile) {
			ਜੇ (static_cast<px4::params>(param) == p) {
				ਵਾਪਸ ਸੱਚ;
			}
		}
	}

	ਵਾਪਸ ਝੂਠ;
}

ਸਥਿਰ ਤਾਰਕਿਕ params_locked = ਝੂਠ;

ਤਾਰਕਿਕ param_is_readonly(param_t param)
{
	ਜੇ (params_locked && handle_ਵਿੱਚ_range(param)) {
		ਲਈ (ਅਚਲ ਸਵੈਚਾਲਿਤ &p : px4::parameters_readonly) {
			ਜੇ (static_cast<px4::params>(param) == p) {
				ਵਾਪਸ ਸੱਚ;
			}
		}
	}

	ਵਾਪਸ ਝੂਠ;
}

ਖਾਲੀ param_ਤਾਲਾ_readonly()
{
	params_locked = ਸੱਚ;
}

size_t param_ਮਾਪ(param_t param)
{
	ਜੇ (handle_ਵਿੱਚ_range(param)) {
		ਚੋਣ (param_type(param)) {
		ਹਾਲਤ PARAM_TYPE_INT32:
		ਹਾਲਤ PARAM_TYPE_FLOAT:
			ਵਾਪਸ 4;

		ਮੂਲ:
			ਵਾਪਸ 0;
		}
	}

	ਵਾਪਸ 0;
}

ਪੂਰਨ
param_get_system_default_ਮੁੱਲ(param_t param, ਖਾਲੀ *default_val)
{
	ਜੇ (!handle_ਵਿੱਚ_range(param)) {
		ਵਾਪਸ PX4_ERROR;
	}

	ਪੂਰਨ ret = PX4_OK;

	ਚੋਣ (param_type(param)) {
	ਹਾਲਤ PARAM_TYPE_INT32:
		ਸਮ੍ਰਿਤੀ_ਨਕਲ(default_val, &px4::parameters[param].val.i, param_ਮਾਪ(param));
		ਰੋਕੋ;

	ਹਾਲਤ PARAM_TYPE_FLOAT:
		ਸਮ੍ਰਿਤੀ_ਨਕਲ(default_val, &px4::parameters[param].val.f, param_ਮਾਪ(param));
		ਰੋਕੋ;

	ਮੂਲ:
		ret = PX4_ERROR;
		ਰੋਕੋ;
	}

	ਵਾਪਸ ret;
}

ਪੂਰਨ
param_get_mark_ਵਰਤਿਆ(param_t param, ਖਾਲੀ *val)
{
	param_ਸੈੱਟ_ਵਰਤਿਆ(param);
	ਵਾਪਸ param_get(param, val);
}
