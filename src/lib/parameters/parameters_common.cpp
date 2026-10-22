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

#ଅନ୍ତର୍ଭାଵ <string.h>

/**
 * @file parameters_common.cpp
 *
 * Global parameter store, functions common to kernel and user sides
 *
 */

ସ୍ଥିର constexpr uint16_t param_info_ଗଣନା = ଆକାର(px4::parameters) / ଆକାର(param_info_s);

/**
 * Test whether a param_t is value.
 *
 * @param param			The parameter handle to test.
 * @return			True if the handle is valid.
 */
ସ୍ଥିର constexpr ତାର୍କିକ handle_in_range(param_t param) { ଫେରନ୍ତୁ (param < param_info_ଗଣନା); }

unsigned param_ଗଣନା()
{
	ଫେରନ୍ତୁ param_info_ଗଣନା;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା param_get_index(param_t param)
{
	ଯଦି (handle_in_range(param)) {
		ଫେରନ୍ତୁ (unsigned)param;
	}

	ଫେରନ୍ତୁ -1;
}

param_t param_for_index(unsigned index)
{
	ଯଦି (index < param_info_ଗଣନା) {
		ଫେରନ୍ତୁ (param_t)index;
	}

	ଫେରନ୍ତୁ PARAM_INVALID;
}

ଅଚଳ ଅକ୍ଷର *param_name(param_t param)
{
	ଫେରନ୍ତୁ handle_in_range(param) ? px4::parameters[param].name : nullptr;
}

param_type_t param_type(param_t param)
{
	ଫେରନ୍ତୁ handle_in_range(param) ? px4::parameters_type[param] : PARAM_TYPE_UNKNOWN;
}

ତାର୍କିକ param_is_volatile(param_t param)
{
	ଯଦି (handle_in_range(param)) {
		ପାଇଁ (ଅଚଳ auto &p : px4::parameters_volatile) {
			ଯଦି (static_cast<px4::params>(param) == p) {
				ଫେରନ୍ତୁ ସତ୍ୟ;
			}
		}
	}

	ଫେରନ୍ତୁ ମିଥ୍ୟା;
}

ସ୍ଥିର ତାର୍କିକ params_locked = ମିଥ୍ୟା;

ତାର୍କିକ param_is_readonly(param_t param)
{
	ଯଦି (params_locked && handle_in_range(param)) {
		ପାଇଁ (ଅଚଳ auto &p : px4::parameters_readonly) {
			ଯଦି (static_cast<px4::params>(param) == p) {
				ଫେରନ୍ତୁ ସତ୍ୟ;
			}
		}
	}

	ଫେରନ୍ତୁ ମିଥ୍ୟା;
}

ଶୂନ୍ୟ param_lock_readonly()
{
	params_locked = ସତ୍ୟ;
}

size_t param_size(param_t param)
{
	ଯଦି (handle_in_range(param)) {
		ବାଛ (param_type(param)) {
		ସ୍ଥିତି PARAM_TYPE_INT32:
		ସ୍ଥିତି PARAM_TYPE_FLOAT:
			ଫେରନ୍ତୁ 4;

		ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
			ଫେରନ୍ତୁ 0;
		}
	}

	ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା
param_get_system_default_ମୂଲ୍ୟ(param_t param, ଶୂନ୍ୟ *default_val)
{
	ଯଦି (!handle_in_range(param)) {
		ଫେରନ୍ତୁ PX4_ERROR;
	}

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret = PX4_OK;

	ବାଛ (param_type(param)) {
	ସ୍ଥିତି PARAM_TYPE_INT32:
		memcpy(default_val, &px4::parameters[param].val.i, param_size(param));
		ଭାଙ୍ଗ;

	ସ୍ଥିତି PARAM_TYPE_FLOAT:
		memcpy(default_val, &px4::parameters[param].val.f, param_size(param));
		ଭାଙ୍ଗ;

	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		ret = PX4_ERROR;
		ଭାଙ୍ଗ;
	}

	ଫେରନ୍ତୁ ret;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା
param_get_mark_used(param_t param, ଶୂନ୍ୟ *val)
{
	param_set_used(param);
	ଫେରନ୍ତୁ param_get(param, val);
}
