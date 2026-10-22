/****************************************************************************
 *
 *   Copyright (c) 2021 Technology Innovation Institute. All rights reserved.
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
 * @file parameters_ioctl.cpp
 *
 * Protected build kernel space interface to global parameter store.
 */

#define PARAM_IMPLEMENTATION

#ଅନ୍ତର୍ଭାଵ <errno.h>

#ଅନ୍ତର୍ଭାଵ "param.h"
#ଅନ୍ତର୍ଭାଵ "parameters_ioctl.h"
#ଅନ୍ତର୍ଭାଵ <px4_platform_common/defines.h>

ପୂର୍ଣ୍ଣସଂଖ୍ୟା	param_ioctl(unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା cmd, unsigned long arg)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret = OK;

	ବାଛ (cmd) {
	ସ୍ଥିତି PARAMIOCNOTIFY: {
			param_notify_changes();
		}
		ଭାଙ୍ଗ;

	ସ୍ଥିତି PARAMIOCFIND: {
			paramiocfind_t *data = (paramiocfind_t *)arg;

			ଯଦି (data->notification) {
				data->ret = param_find(data->name);

			} ଅନ୍ୟଥା {
				data->ret = param_find_no_notification(data->name);
			}
		}
		ଭାଙ୍ଗ;

	ସ୍ଥିତି PARAMIOCCOUNTUSED: {
			paramioccountused_t *data = (paramioccountused_t *)arg;
			data->ret = param_ଗଣନା_used();
		}
		ଭାଙ୍ଗ;

	ସ୍ଥିତି PARAMIOCFORUSEDINDEX: {
			paramiocforusedindex_t *data = (paramiocforusedindex_t *)arg;
			data->ret =  param_for_used_index(data->index);
		}
		ଭାଙ୍ଗ;

	ସ୍ଥିତି PARAMIOCGETUSEDINDEX: {
			paramiocgetusedindex_t *data = (paramiocgetusedindex_t *)arg;
			data->ret = param_get_used_index(data->param);
		}
		ଭାଙ୍ଗ;

	ସ୍ଥିତି PARAMIOCUNSAVED: {
			paramiocunsaved_t *data = (paramiocunsaved_t *)arg;
			data->ret = param_ମୂଲ୍ୟ_unsaved(data->param);
		}
		ଭାଙ୍ଗ;

	ସ୍ଥିତି PARAMIOCGET: {
			paramiocget_t *data = (paramiocget_t *)arg;

			ଯଦି (data->deflt) {
				data->ret = param_get_default_ମୂଲ୍ୟ(data->param, data->val);

			} ଅନ୍ୟଥା {
				data->ret = param_get(data->param, data->val);
			}
		}
		ଭାଙ୍ଗ;

	ସ୍ଥିତି PARAMIOCAUTOSAVE: {
			paramiocautosave_t *data = (paramiocautosave_t *)arg;
			param_control_autosave(data->enable);
		}
		ଭାଙ୍ଗ;

	ସ୍ଥିତି PARAMIOCSET: {
			paramiocset_t *data = (paramiocset_t *)arg;

			ଯଦି (data->notification) {
				data->ret = param_set(data->param, data->val);

			} ଅନ୍ୟଥା {
				data->ret = param_set_no_notification(data->param, data->val);
			}
		}
		ଭାଙ୍ଗ;

	ସ୍ଥିତି PARAMIOCUSED: {
			paramiocused_t *data = (paramiocused_t *)arg;
			data->ret = param_used(data->param);
		}
		ଭାଙ୍ଗ;

	ସ୍ଥିତି PARAMIOCSETUSED: {
			paramiocsetused_t *data = (paramiocsetused_t *)arg;
			param_set_used(data->param);
		}
		ଭାଙ୍ଗ;

	ସ୍ଥିତି PARAMIOCSETDEFAULT: {
			paramiocsetdefault_t *data = (paramiocsetdefault_t *)arg;
			data->ret = param_set_default_ମୂଲ୍ୟ(data->param, data->val);
		}
		ଭାଙ୍ଗ;

	ସ୍ଥିତି PARAMIOCRESET: {
			paramiocreset_t *data = (paramiocreset_t *)arg;

			ଯଦି (data->notification) {
				data->ret = param_reset(data->param);

			} ଅନ୍ୟଥା {
				data->ret = param_reset_no_notification(data->param);
			}
		}
		ଭାଙ୍ଗ;

	ସ୍ଥିତି PARAMIOCRESETGROUP: {
			paramiocresetgroup_t *data = (paramiocresetgroup_t *)arg;

			ଯଦି (data->type == PARAM_RESET_EXCLUDES) {
				param_reset_excludes(data->group, data->ଅଙ୍କ_in_group);

			} ଅନ୍ୟଥା ଯଦି (data->type == PARAM_RESET_SPECIFIC) {
				param_reset_specific(data->group, data->ଅଙ୍କ_in_group);

			} ଅନ୍ୟଥା {
				param_reset_all();
			}
		}
		ଭାଙ୍ଗ;

	ସ୍ଥିତି PARAMIOCSAVEDEFAULT: {
			paramiocsavedefault_t *data = (paramiocsavedefault_t *)arg;
			data->ret = param_save_default(data->blocking);
		}
		ଭାଙ୍ଗ;

	ସ୍ଥିତି PARAMIOCLOADDEFAULT: {
			paramiocloaddefault_t *data = (paramiocloaddefault_t *)arg;
			data->ret = param_load_default();
		}
		ଭାଙ୍ଗ;

	ସ୍ଥିତି PARAMIOCEXPORT: {
			paramiocexport_t *data = (paramiocexport_t *)arg;
			data->ret = param_export(data->filename, nullptr);
		}
		ଭାଙ୍ଗ;

	ସ୍ଥିତି PARAMIOCHASH: {
			paramiochash_t *data = (paramiochash_t *)arg;
			data->ret = param_hash_check();
		}
		ଭାଙ୍ଗ;

	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		ret = -ENOTTY;
		ଭାଙ୍ଗ;
	}

	ଫେରନ୍ତୁ ret;
}
