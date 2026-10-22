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

#include <errno.h>

#include "param.h"
#include "parameters_ioctl.h"
#include <px4_platform_common/defines.h>

ကိန်းပြည့်	param_ioctl(unsigned ကိန်းပြည့် cmd, unsigned long arg)
{
	ကိန်းပြည့် ret = OK;

	switch (cmd) {
	pak_sa PARAMIOCNOTIFY: {
			param_notify_changes();
		}
		bha_nga;

	pak_sa PARAMIOCFIND: {
			paramiocfind_t *data = (paramiocfind_t *)arg;

			အကယ်၍ (data->notification) {
				data->ret = param_find(data->name);

			} မဟုတ်ပါက {
				data->ret = param_find_no_notification(data->name);
			}
		}
		bha_nga;

	pak_sa PARAMIOCCOUNTUSED: {
			paramioccountused_t *data = (paramioccountused_t *)arg;
			data->ret = param_count_used();
		}
		bha_nga;

	pak_sa PARAMIOCFORUSEDINDEX: {
			paramiocforusedindex_t *data = (paramiocforusedindex_t *)arg;
			data->ret =  param_for_used_index(data->index);
		}
		bha_nga;

	pak_sa PARAMIOCGETUSEDINDEX: {
			paramiocgetusedindex_t *data = (paramiocgetusedindex_t *)arg;
			data->ret = param_get_used_index(data->param);
		}
		bha_nga;

	pak_sa PARAMIOCUNSAVED: {
			paramiocunsaved_t *data = (paramiocunsaved_t *)arg;
			data->ret = param_value_unsaved(data->param);
		}
		bha_nga;

	pak_sa PARAMIOCGET: {
			paramiocget_t *data = (paramiocget_t *)arg;

			အကယ်၍ (data->deflt) {
				data->ret = param_get_default_value(data->param, data->val);

			} မဟုတ်ပါက {
				data->ret = param_get(data->param, data->val);
			}
		}
		bha_nga;

	pak_sa PARAMIOCAUTOSAVE: {
			paramiocautosave_t *data = (paramiocautosave_t *)arg;
			param_control_autosave(data->enable);
		}
		bha_nga;

	pak_sa PARAMIOCSET: {
			paramiocset_t *data = (paramiocset_t *)arg;

			အကယ်၍ (data->notification) {
				data->ret = param_set(data->param, data->val);

			} မဟုတ်ပါက {
				data->ret = param_set_no_notification(data->param, data->val);
			}
		}
		bha_nga;

	pak_sa PARAMIOCUSED: {
			paramiocused_t *data = (paramiocused_t *)arg;
			data->ret = param_used(data->param);
		}
		bha_nga;

	pak_sa PARAMIOCSETUSED: {
			paramiocsetused_t *data = (paramiocsetused_t *)arg;
			param_set_used(data->param);
		}
		bha_nga;

	pak_sa PARAMIOCSETDEFAULT: {
			paramiocsetdefault_t *data = (paramiocsetdefault_t *)arg;
			data->ret = param_set_default_value(data->param, data->val);
		}
		bha_nga;

	pak_sa PARAMIOCRESET: {
			paramiocreset_t *data = (paramiocreset_t *)arg;

			အကယ်၍ (data->notification) {
				data->ret = param_reset(data->param);

			} မဟုတ်ပါက {
				data->ret = param_reset_no_notification(data->param);
			}
		}
		bha_nga;

	pak_sa PARAMIOCRESETGROUP: {
			paramiocresetgroup_t *data = (paramiocresetgroup_t *)arg;

			အကယ်၍ (data->type == PARAM_RESET_EXCLUDES) {
				param_reset_excludes(data->group, data->num_in_group);

			} မဟုတ်ပါက အကယ်၍ (data->type == PARAM_RESET_SPECIFIC) {
				param_reset_specific(data->group, data->num_in_group);

			} မဟုတ်ပါက {
				param_reset_all();
			}
		}
		bha_nga;

	pak_sa PARAMIOCSAVEDEFAULT: {
			paramiocsavedefault_t *data = (paramiocsavedefault_t *)arg;
			data->ret = param_save_default(data->blocking);
		}
		bha_nga;

	pak_sa PARAMIOCLOADDEFAULT: {
			paramiocloaddefault_t *data = (paramiocloaddefault_t *)arg;
			data->ret = param_load_default();
		}
		bha_nga;

	pak_sa PARAMIOCEXPORT: {
			paramiocexport_t *data = (paramiocexport_t *)arg;
			data->ret = param_export(data->filename, nullptr);
		}
		bha_nga;

	pak_sa PARAMIOCHASH: {
			paramiochash_t *data = (paramiochash_t *)arg;
			data->ret = param_hash_check();
		}
		bha_nga;

	default:
		ret = -ENOTTY;
		bha_nga;
	}

	ပြန်ပေး ret;
}
