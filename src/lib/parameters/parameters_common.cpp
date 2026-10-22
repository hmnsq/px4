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

#अन्तर्भाव <string.h>

/**
 * @file parameters_common.cpp
 *
 * Global parameter store, functions common to kernel and user sides
 *
 */

स्थिर constexpr uint16_t param_info_मोजणी = परिमाण(px4::parameters) / परिमाण(param_info_s);

/**
 * Test whether a param_t is value.
 *
 * @param param			The parameter handle to test.
 * @return			True if the handle is valid.
 */
स्थिर constexpr तार्किक handle_in_range(param_t param) { प्रत्यागम (param < param_info_मोजणी); }

unsigned param_मोजणी()
{
	प्रत्यागम param_info_मोजणी;
}

पूर्णाङ्क param_get_index(param_t param)
{
	यदि (handle_in_range(param)) {
		प्रत्यागम (unsigned)param;
	}

	प्रत्यागम -1;
}

param_t param_for_index(unsigned index)
{
	यदि (index < param_info_मोजणी) {
		प्रत्यागम (param_t)index;
	}

	प्रत्यागम PARAM_INVALID;
}

ध्रुव वर्ण *param_name(param_t param)
{
	प्रत्यागम handle_in_range(param) ? px4::parameters[param].name : nullptr;
}

param_type_t param_type(param_t param)
{
	प्रत्यागम handle_in_range(param) ? px4::parameters_type[param] : PARAM_TYPE_UNKNOWN;
}

तार्किक param_is_volatile(param_t param)
{
	यदि (handle_in_range(param)) {
		प्रति (ध्रुव auto &p : px4::parameters_volatile) {
			यदि (static_cast<px4::params>(param) == p) {
				प्रत्यागम सत्य;
			}
		}
	}

	प्रत्यागम असत्य;
}

स्थिर तार्किक params_locked = असत्य;

तार्किक param_is_readonly(param_t param)
{
	यदि (params_locked && handle_in_range(param)) {
		प्रति (ध्रुव auto &p : px4::parameters_readonly) {
			यदि (static_cast<px4::params>(param) == p) {
				प्रत्यागम सत्य;
			}
		}
	}

	प्रत्यागम असत्य;
}

शून्य param_lock_readonly()
{
	params_locked = सत्य;
}

size_t param_size(param_t param)
{
	यदि (handle_in_range(param)) {
		विकल्प (param_type(param)) {
		पक्ष PARAM_TYPE_INT32:
		पक्ष PARAM_TYPE_FLOAT:
			प्रत्यागम 4;

		स्वभाव:
			प्रत्यागम 0;
		}
	}

	प्रत्यागम 0;
}

पूर्णाङ्क
param_get_system_default_मूल्य(param_t param, शून्य *default_val)
{
	यदि (!handle_in_range(param)) {
		प्रत्यागम PX4_ERROR;
	}

	पूर्णाङ्क ret = PX4_OK;

	विकल्प (param_type(param)) {
	पक्ष PARAM_TYPE_INT32:
		memcpy(default_val, &px4::parameters[param].val.i, param_size(param));
		भङ्ग;

	पक्ष PARAM_TYPE_FLOAT:
		memcpy(default_val, &px4::parameters[param].val.f, param_size(param));
		भङ्ग;

	स्वभाव:
		ret = PX4_ERROR;
		भङ्ग;
	}

	प्रत्यागम ret;
}

पूर्णाङ्क
param_get_mark_used(param_t param, शून्य *val)
{
	param_set_used(param);
	प्रत्यागम param_get(param, val);
}
